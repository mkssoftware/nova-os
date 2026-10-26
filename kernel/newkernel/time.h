/* NovaOS C-Kernel - Time-Subsystem (Port aus time32.inc,
   NPSPEC-TIME-ARCH-0001 und verwandte NPSPEC-TIME-Spezifikationen). */
#ifndef NOVA_TIME_H
#define NOVA_TIME_H

#include <stdint.h>

#define CLKSRC_NONE    0u
#define CLKSRC_PIT     1u
#define CLKSRC_TSC     2u
#define CLKSRC_HPET    3u
#define CLKSRC_RTC     4u
#define CLKSRC_VIRTUAL 5u

#define CLKDOM_SYSTEM  0u
#define CLKDOM_WALL    1u
#define CLKDOM_LOCAL   2u
#define CLKDOM_MEDIA   3u
#define CLKDOM_VIRTUAL 4u

#define TIMER_TYPE_ONESHOT   0u
#define TIMER_TYPE_PERIODIC  1u
#define TIMER_TYPE_DEADLINE  2u
#define TIMER_TYPE_COALESCED 3u
#define TIMER_TYPE_PERSIST   4u

#define TIMER_ST_FREE      0u
#define TIMER_ST_ARMED     1u
#define TIMER_ST_FIRED     2u
#define TIMER_ST_CANCELLED 3u

#define TIMER_CAPACITY  16u
#define CLKSRC_CAPACITY 4u

#define TIMESYNC_NONE    0u
#define TIMESYNC_RTC     1u
#define TIMESYNC_NTP     2u
#define TIMESYNC_PTP     3u
#define TIMESYNC_TRUSTED 4u

#define LEAP_STATE_NONE    0u
#define LEAP_STATE_PENDING 1u
#define LEAP_STATE_ACTIVE  2u

#define TIMENS_CAPACITY 4u

typedef struct {
    uint32_t year;
    uint32_t month;
    uint32_t day;
    uint32_t hour;
    uint32_t minute;
    uint32_t second;
} time_civil_t;

/* Registriert PIT (100Hz) und TSC als Clock-Sources, kalibriert die
   TSC-Frequenz anhand eines echten PIT-Ticks (busy-wait, braucht aktive
   Interrupts) und legt den Default-Zeitnamespace an. */
void time_subsystem_initialize(void);
int time_subsystem_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int time_clksrc_register(uint32_t type, uint32_t freq_hz, uint32_t flags);

uint64_t time_get_monotonic_ticks(void); /* 100Hz-Ticks seit Boot */
uint64_t time_get_tsc(void);
uint32_t time_tsc_to_ns(uint64_t tsc_delta); /* 0 wenn Frequenz unbekannt */

/* Liest CMOS-RTC (Register 0x70/0x71) in *out_civil. */
void time_read_rtc(time_civil_t *out_civil);

/* 0 = Erfolg (out_timer_id gesetzt), -1 wenn Tabelle voll. */
int time_timer_arm(uint32_t owner, uint32_t type, uint32_t deadline_ticks,
                    uint32_t period_ticks, uint32_t *out_timer_id);
int time_timer_cancel(uint32_t timer_id); /* 0 = Erfolg, -1 wenn nicht gefunden */

/* Prueft alle bewaffneten Timer auf Ablauf; aus dem PIT-IRQ-Pfad aufzurufen
   (aktuell noch nicht an interrupt_dispatch_c angebunden, siehe
   interrupt.c-TODO fuer den vollen Scheduler-Dispatch). */
void time_timer_tick(void);

void time_ntp_adjust(int32_t offset_ms);
void time_ptp_adjust(int32_t offset_ns);

uint64_t time_media_get_position(void); /* 100-ns-Einheiten */

/* Legacy-kompatible Einsprungpunkte (entsprachen frueher separaten
   entry32.asm-Aufrufen; delegieren an time_subsystem_initialize/self_test). */
void time_monotonic_initialize(void);
int time_monotonic_self_test(void);
void rtc_initialize(void);
int rtc_self_test(void); /* 0 = ok, -1 wenn Jahr < 2020 (CMOS-Sanity-Check) */
void clocksource_initialize(void);
int clocksource_self_test(void);
void hrt_initialize(void);
int hrt_self_test(void);

/* Deferred-Work-Queue (Stub - Arbeitslisten-Infrastruktur). */
#define DEFERRED_WORK_CAPACITY 8u
void deferred_initialize(void);
int deferred_self_test(void);

#endif
