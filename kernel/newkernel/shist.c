/* NovaOS C-Kernel - Nova State History Ring (Port aus adaptive32.inc). */
#include "shist.h"
#include "resilience.h"

typedef struct {
    uint32_t obj_id;
    uint32_t old_state;
    uint32_t new_state;
    uint32_t tick;
} shist_rec_t;

static shist_rec_t shist_ring[SHIST_CAPACITY];
static int shist_ready = 0;
static uint32_t shist_head = 0;
static uint32_t shist_count = 0;

void shist_initialize(void) {
    if (shist_ready == 1) return;

    for (uint32_t i = 0; i < SHIST_CAPACITY; ++i) shist_ring[i] = (shist_rec_t){0};
    shist_head = 0;
    shist_count = 0;
    shist_ready = 1;
}

void shist_record(uint32_t obj_id, uint32_t old_state, uint32_t new_state) {
    if (shist_ready != 1) return;

    shist_ring[shist_head].obj_id = obj_id;
    shist_ring[shist_head].old_state = old_state;
    shist_ring[shist_head].new_state = new_state;
    shist_ring[shist_head].tick = resilience_global_tick();

    shist_head = (shist_head + 1 >= SHIST_CAPACITY) ? 0 : shist_head + 1;
    if (shist_count < SHIST_CAPACITY) ++shist_count;
}

uint32_t shist_count_transitions(void) {
    return shist_count;
}

int shist_self_test(void) {
    shist_record(1, 0, 1);
    shist_record(1, 1, 2);

    if (shist_count_transitions() != 2) return -1;
    if (shist_ring[0].obj_id != 1 || shist_ring[0].old_state != 0 || shist_ring[0].new_state != 1) return -1;

    return 0;
}
