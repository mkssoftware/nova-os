/* NovaOS C-Kernel - Time-Subsystem (Port aus time32.inc). */
#include "time.h"
#include "interrupt.h"

typedef struct {
    uint32_t id;
    uint32_t owner;
    uint32_t type;
    uint32_t state;
    uint32_t domain;
    uint32_t deadline_lo;
    uint32_t deadline_hi;
    uint32_t period;
    uint32_t tolerance;
    uint32_t callback;
    uint32_t fire_count;
} time_timer_t;

typedef struct {
    uint32_t id;
    uint32_t type;
    uint32_t freq_hz;
    uint32_t freq_frac;
    uint32_t flags; /* 0x1=verfuegbar, 0x2=aktiv, 0x4=vertrauenswuerdig */
    uint32_t skew_ppb;
} time_clksrc_t;

typedef struct {
    uint32_t id;
    uint32_t offset_lo;
    uint32_t offset_hi;
    uint32_t scale;
} time_namespace_t;

static time_timer_t time_timers[TIMER_CAPACITY];
static time_clksrc_t time_clksrc_table[CLKSRC_CAPACITY];
static time_namespace_t time_namespaces[TIMENS_CAPACITY];
static time_civil_t time_rtc_buf;

static uint32_t time_timer_count = 0;
static uint32_t time_clksrc_count = 0;
static uint32_t time_ns_count = 0;
static uint32_t time_next_timer_id = 0;
static uint32_t time_sync_state = TIMESYNC_NONE;
static uint32_t time_leap_state = LEAP_STATE_NONE;
static uint32_t time_tsc_freq_khz = 0;
static int32_t time_ntp_offset_ms = 0;
static int32_t time_ptp_offset_ns = 0;
static int time_subsys_ready = 0;

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint64_t rdtsc_now(void) {
    uint32_t lo, hi;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

int time_clksrc_register(uint32_t type, uint32_t freq_hz, uint32_t flags) {
    if (time_clksrc_count >= CLKSRC_CAPACITY) {
        return -1;
    }
    time_clksrc_t *slot = 0;
    for (uint32_t i = 0; i < CLKSRC_CAPACITY; ++i) {
        if (time_clksrc_table[i].type == CLKSRC_NONE) {
            slot = &time_clksrc_table[i];
            break;
        }
    }
    if (!slot) {
        return -1;
    }
    slot->id = type;
    slot->type = type;
    slot->freq_hz = freq_hz;
    slot->freq_frac = 0;
    slot->flags = flags;
    slot->skew_ppb = 0;
    ++time_clksrc_count;
    return 0;
}

/* Bestimmt die TSC-Frequenz durch Messung ueber einen echten PIT-Tick
   (NPSPEC-TIME-CALIBRATION-0001). Braucht aktive Interrupts. */
static void time_calibrate_tsc(void) {
    uint64_t start = rdtsc_now();

    uint32_t target = timer_get_ticks() + 1;
    while (timer_get_ticks() < target) {
        __asm__ volatile ("pause");
    }

    /* delta ist TSC-Takte fuer ~10ms (1 Tick bei 100Hz) -> kHz = delta/10.
       Wie im alten ASM nur das untere 32-Bit-Wort verwenden (reicht fuer
       realistische TSC-Deltas ueber einen einzelnen PIT-Tick). */
    uint32_t delta = (uint32_t)(rdtsc_now() - start);
    time_tsc_freq_khz = delta / 10u;

    for (uint32_t i = 0; i < CLKSRC_CAPACITY; ++i) {
        if (time_clksrc_table[i].type == CLKSRC_TSC) {
            time_clksrc_table[i].freq_hz = time_tsc_freq_khz * 1000u;
            break;
        }
    }
}

static void time_ns_create_default(void) {
    time_namespace_t *ns = &time_namespaces[0];
    ns->id = 1;
    ns->offset_lo = 0;
    ns->offset_hi = 0;
    ns->scale = 0x10000u; /* 1.0 als Fixpunkt 16.16 */
    ++time_ns_count;
}

void time_subsystem_initialize(void) {
    for (uint32_t i = 0; i < TIMER_CAPACITY; ++i) time_timers[i] = (time_timer_t){0};
    for (uint32_t i = 0; i < CLKSRC_CAPACITY; ++i) time_clksrc_table[i] = (time_clksrc_t){0};
    for (uint32_t i = 0; i < TIMENS_CAPACITY; ++i) time_namespaces[i] = (time_namespace_t){0};

    time_timer_count = 0;
    time_clksrc_count = 0;
    time_sync_state = TIMESYNC_RTC;
    time_leap_state = LEAP_STATE_NONE;
    time_ntp_offset_ms = 0;
    time_tsc_freq_khz = 0;
    time_ns_count = 0;

    time_clksrc_register(CLKSRC_PIT, 100, 0x01u); /* verfuegbar */
    time_clksrc_register(CLKSRC_TSC, 0, 0x07u);   /* verfuegbar + aktiv + vertrauenswuerdig */

    time_calibrate_tsc();
    time_ns_create_default();

    time_subsys_ready = 1;
}

uint64_t time_get_monotonic_ticks(void) {
    return (uint64_t)timer_get_ticks();
}

uint64_t time_get_tsc(void) {
    return rdtsc_now();
}

uint32_t time_tsc_to_ns(uint64_t tsc_delta) {
    if (time_tsc_freq_khz == 0) {
        return 0;
    }
    /* Wie im alten ASM nur das untere 32-Bit-Wort verwenden. */
    return ((uint32_t)tsc_delta * 1000u) / time_tsc_freq_khz;
}

static uint32_t time_bcd_to_bin(uint8_t bcd) {
    return (uint32_t)((bcd >> 4) * 10u + (bcd & 0x0Fu));
}

void time_read_rtc(time_civil_t *out_civil) {
    outb(0x70, 0x00); out_civil->second = time_bcd_to_bin(inb(0x71));
    outb(0x70, 0x02); out_civil->minute = time_bcd_to_bin(inb(0x71));
    outb(0x70, 0x04); out_civil->hour = time_bcd_to_bin(inb(0x71));
    outb(0x70, 0x07); out_civil->day = time_bcd_to_bin(inb(0x71));
    outb(0x70, 0x08); out_civil->month = time_bcd_to_bin(inb(0x71));
    outb(0x70, 0x09); out_civil->year = time_bcd_to_bin(inb(0x71)) + 2000u;
}

int time_timer_arm(uint32_t owner, uint32_t type, uint32_t deadline_ticks,
                    uint32_t period_ticks, uint32_t *out_timer_id) {
    if (time_timer_count >= TIMER_CAPACITY) {
        return -1;
    }
    time_timer_t *slot = 0;
    for (uint32_t i = 0; i < TIMER_CAPACITY; ++i) {
        if (time_timers[i].state == TIMER_ST_FREE) {
            slot = &time_timers[i];
            break;
        }
    }
    if (!slot) {
        return -1;
    }

    ++time_next_timer_id;
    slot->id = time_next_timer_id;
    slot->owner = owner;
    slot->type = type;
    slot->state = TIMER_ST_ARMED;
    slot->domain = CLKDOM_SYSTEM;
    slot->deadline_lo = deadline_ticks;
    slot->deadline_hi = 0;
    slot->period = period_ticks;
    slot->tolerance = 0;
    slot->callback = 0;
    slot->fire_count = 0;

    ++time_timer_count;
    *out_timer_id = time_next_timer_id;
    return 0;
}

int time_timer_cancel(uint32_t timer_id) {
    for (uint32_t i = 0; i < TIMER_CAPACITY; ++i) {
        if (time_timers[i].id == timer_id && time_timers[i].state == TIMER_ST_ARMED) {
            time_timers[i].state = TIMER_ST_CANCELLED;
            --time_timer_count;
            return 0;
        }
    }
    return -1;
}

void time_timer_tick(void) {
    uint32_t now = timer_get_ticks();
    for (uint32_t i = 0; i < TIMER_CAPACITY; ++i) {
        time_timer_t *t = &time_timers[i];
        if (t->state != TIMER_ST_ARMED) continue;
        if (now < t->deadline_lo - t->tolerance) continue;

        ++t->fire_count;
        /* TODO: Callback-Aufruf ueber Funktionszeiger, sobald ein echter
           Timer-Consumer existiert. */

        if (t->type == TIMER_TYPE_PERIODIC) {
            t->deadline_lo = now + t->period;
        } else {
            t->state = TIMER_ST_FIRED;
            --time_timer_count;
        }
    }
}

void time_ntp_adjust(int32_t offset_ms) {
    time_ntp_offset_ms = offset_ms;
    time_sync_state = TIMESYNC_NTP;
}

void time_ptp_adjust(int32_t offset_ns) {
    time_ptp_offset_ns = offset_ns;
    time_sync_state = TIMESYNC_PTP;
}

uint64_t time_media_get_position(void) {
    if (time_tsc_freq_khz == 0) {
        return 0;
    }
    /* Wie im alten ASM nur das untere 32-Bit-Wort verwenden. */
    uint32_t tsc_lo = (uint32_t)time_get_tsc();
    return (uint64_t)((tsc_lo * 10u) / time_tsc_freq_khz); /* 100ns-Einheiten */
}

void time_monotonic_initialize(void) {
    time_subsystem_initialize();
}

int time_monotonic_self_test(void) {
    return time_subsystem_self_test();
}

void rtc_initialize(void) {
    time_read_rtc(&time_rtc_buf);
}

int rtc_self_test(void) {
    return (time_rtc_buf.year >= 2020u) ? 0 : -1;
}

void clocksource_initialize(void) {
    if (time_subsys_ready != 1) {
        time_subsystem_initialize();
    }
}

int clocksource_self_test(void) {
    return (time_clksrc_count != 0) ? 0 : -1;
}

void hrt_initialize(void) {
    /* HRT basiert auf TSC, keine eigene Hardware-Init noetig. */
}

int hrt_self_test(void) {
    return (time_tsc_freq_khz != 0) ? 0 : -1;
}

int time_subsystem_self_test(void) {
    if (time_subsys_ready != 1) return -1;
    if (time_tsc_freq_khz == 0) return -1;

    uint32_t timer_id = 0;
    if (time_timer_arm(1, TIMER_TYPE_ONESHOT, 0xFFFFu, 0, &timer_id) != 0) return -1;
    if (time_timer_cancel(timer_id) != 0) return -1;

    time_read_rtc(&time_rtc_buf);

    return 0;
}

typedef struct {
    uint32_t fn;
    uint32_t arg;
    uint32_t state;
    uint32_t reserved;
} deferred_work_t;

static deferred_work_t deferred_work_table[DEFERRED_WORK_CAPACITY];
static uint32_t deferred_count = 0;

void deferred_initialize(void) {
    for (uint32_t i = 0; i < DEFERRED_WORK_CAPACITY; ++i) {
        deferred_work_table[i] = (deferred_work_t){0};
    }
    deferred_count = 0;
}

int deferred_self_test(void) {
    return (deferred_count == 0) ? 0 : -1;
}
