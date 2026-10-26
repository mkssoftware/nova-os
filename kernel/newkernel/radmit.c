/* NovaOS C-Kernel - Nova Resource Admission Control (Port aus
   adaptive32.inc). */
#include "radmit.h"

typedef struct {
    uint32_t id; /* 0 = frei */
    uint32_t cpu;
    uint32_t mem;
    uint32_t state; /* 0 = frei, 1 = aktiv */
    uint32_t owner;
} radmit_rec_t;

static radmit_rec_t radmit_table[RADMIT_CAPACITY];
static int radmit_ready = 0;
static uint32_t radmit_cpu_used = 0;
static uint32_t radmit_mem_used = 0;
static uint32_t radmit_next_id = 1;
static uint32_t radmit_rejected = 0;

void radmit_initialize(void) {
    for (uint32_t i = 0; i < RADMIT_CAPACITY; ++i) radmit_table[i] = (radmit_rec_t){0};
    radmit_cpu_used = 0;
    radmit_mem_used = 0;
    radmit_next_id = 1;
    radmit_rejected = 0;
    radmit_ready = 1;
}

int radmit_request(uint32_t cpu_units, uint32_t mem_units, uint32_t owner, uint32_t *out_slot) {
    if (radmit_ready != 1) return -1;

    if (radmit_cpu_used + cpu_units > RADMIT_CPU_MAX) { ++radmit_rejected; return -1; }
    if (radmit_mem_used + mem_units > RADMIT_MEM_MAX) { ++radmit_rejected; return -1; }

    for (uint32_t i = 0; i < RADMIT_CAPACITY; ++i) {
        if (radmit_table[i].state == 0) {
            radmit_table[i].id = radmit_next_id++;
            radmit_table[i].cpu = cpu_units;
            radmit_table[i].mem = mem_units;
            radmit_table[i].state = 1;
            radmit_table[i].owner = owner;
            radmit_cpu_used += cpu_units;
            radmit_mem_used += mem_units;
            *out_slot = i;
            return 0;
        }
    }
    ++radmit_rejected;
    return -1;
}

int radmit_release(uint32_t slot) {
    if (slot >= RADMIT_CAPACITY) return -1;
    radmit_rec_t *rec = &radmit_table[slot];
    if (rec->state != 1) return 0; /* bereits frei: kein Fehler (wie altes ASM) */

    radmit_cpu_used -= rec->cpu;
    radmit_mem_used -= rec->mem;
    rec->state = 0;
    rec->id = 0;
    return 0;
}

int radmit_self_test(void) {
    uint32_t slot;
    if (radmit_request(10, 50, 1, &slot) != 0) return -1;
    if (radmit_cpu_used != 10) return -1;

    if (radmit_release(slot) != 0) return -1;
    if (radmit_cpu_used != 0) return -1;

    uint32_t overflow_slot;
    if (radmit_request(300, 0, 0, &overflow_slot) == 0) return -1; /* >256: abgelehnt */

    return 0;
}
