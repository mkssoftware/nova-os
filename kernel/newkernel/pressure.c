/* NovaOS C-Kernel - Nova Memory Pressure (siehe pressure.h fuer Scope). */
#include "pressure.h"
#include "pmm.h"
#include "framealloc.h"

#define PRESSURE_PCT_LOW      50u
#define PRESSURE_PCT_MEDIUM   25u
#define PRESSURE_PCT_HIGH     10u
#define PRESSURE_PCT_CRITICAL 5u

/* Libgcc-Vermeidung (siehe die dokumentierte __udivdi3-Falle in time.c/
   svgicon.c): dieser freestanding Build linkt kein libgcc, jede
   uint64_t/uint32_t-Division kompiliert sonst zu einem unaufloesbaren
   Aufruf von __udivdi3. Eigene Inline-Assembly-Variante (divl: 64-Bit-
   Dividend in EDX:EAX, 32-Bit-Divisor, Quotient passt hier immer in
   32 Bit, da free_now*100 << 2^32 fuer PMM_MAX_FRAMES=1024). */
static uint32_t fx_udiv64_32(uint64_t dividend, uint32_t divisor) {
    uint32_t low = (uint32_t)dividend;
    uint32_t high = (uint32_t)(dividend >> 32);
    uint32_t quotient;
    __asm__ ("divl %[divisor]"
             : "=a"(quotient)
             : "a"(low), "d"(high), [divisor] "r"(divisor));
    return quotient;
}

static uint32_t g_baseline = 0;
static uint32_t g_level = PRESSURE_LEVEL_NONE;
static uint32_t g_update_count = 0;

void pressure_initialize(void) {
    g_baseline = pmm_frame_count();
    if (g_baseline == 0) g_baseline = 1; /* Division-durch-0-Schutz, siehe pressure_update() */
    g_level = PRESSURE_LEVEL_NONE;
    g_update_count = 0;
}

static uint32_t level_from_percent(uint32_t free_percent) {
    if (free_percent >= PRESSURE_PCT_LOW) return PRESSURE_LEVEL_NONE;
    if (free_percent >= PRESSURE_PCT_MEDIUM) return PRESSURE_LEVEL_LOW;
    if (free_percent >= PRESSURE_PCT_HIGH) return PRESSURE_LEVEL_MEDIUM;
    if (free_percent >= PRESSURE_PCT_CRITICAL) return PRESSURE_LEVEL_HIGH;
    return PRESSURE_LEVEL_CRITICAL;
}

uint32_t pressure_update(void) {
    uint32_t free_now = pmm_frame_count();
    uint32_t percent = fx_udiv64_32((uint64_t)free_now * 100u, g_baseline);
    if (percent > 100u) percent = 100u;
    g_level = level_from_percent(percent);
    ++g_update_count;
    return g_level;
}

uint32_t pressure_level(void) { return g_level; }

uint32_t pressure_numa_level(uint32_t numa_node) {
    (void)numa_node;
    return g_level;
}

int pressure_should_throttle(void) {
    return g_level >= PRESSURE_LEVEL_HIGH;
}

int pressure_is_exhausted(void) {
    return g_level == PRESSURE_LEVEL_CRITICAL;
}

uint32_t pressure_update_count(void) { return g_update_count; }

#define PRESSURE_TEST_MAX_HELD PMM_MAX_FRAMES

/* Gross genug fuer den realen Framebestand (PMM_MAX_FRAMES) - als
   statisches Array statt Stackvariable, um den __chkstk_ms-/Stack-
   groessen-Fallstrick bei grossen lokalen Puffern zu vermeiden (gleiche
   Lektion wie svgicon.c). */
static uint32_t g_test_held[PRESSURE_TEST_MAX_HELD];

int pressure_self_test(void) {
    pressure_initialize();

    if (pressure_update() != PRESSURE_LEVEL_NONE) return -1;
    if (pressure_update_count() != 1u) return -1;
    if (pressure_should_throttle()) return -1;
    if (pressure_is_exhausted()) return -1;

    /* Baseline erneut als die AKTUELLE Framezahl festlegen (frisch, ohne
       von frueheren Modulen schon verbrauchte Frames), damit die
       Prozentrechnung in diesem Test deterministisch ist. */
    pressure_initialize();
    uint32_t baseline_free = pmm_frame_count();
    if (baseline_free < 20u) return -1; /* Test braucht genug Spielraum */

    /* ~96% allokieren (> Baseline-CRITICAL-Schwelle von 5%) -> CRITICAL. */
    uint32_t to_hold = (baseline_free * 96u) / 100u;
    if (to_hold > PRESSURE_TEST_MAX_HELD) to_hold = PRESSURE_TEST_MAX_HELD;
    uint32_t held_count = 0;
    for (; held_count < to_hold; ++held_count) {
        uint32_t numa;
        uint32_t page = frame_alloc(&numa);
        if (page == 0) break;
        g_test_held[held_count] = page;
    }

    uint32_t level = pressure_update();
    int ok_critical_or_high = (level == PRESSURE_LEVEL_CRITICAL || level == PRESSURE_LEVEL_HIGH);
    if (!ok_critical_or_high) {
        for (uint32_t i = 0; i < held_count; ++i) frame_unref(g_test_held[i]);
        return -1;
    }
    if (!pressure_should_throttle()) {
        for (uint32_t i = 0; i < held_count; ++i) frame_unref(g_test_held[i]);
        return -1;
    }

    for (uint32_t i = 0; i < held_count; ++i) {
        frame_unref(g_test_held[i]);
    }

    if (pressure_update() != PRESSURE_LEVEL_NONE) return -1;
    if (pressure_is_exhausted()) return -1;

    return 0;
}
