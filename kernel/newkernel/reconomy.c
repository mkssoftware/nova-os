/* NovaOS C-Kernel - Nova Resource Economy Manager (Port aus
   adaptive32.inc). */
#include "reconomy.h"
#include "raccount.h"

typedef struct {
    uint32_t owner; /* 0 = frei */
    uint32_t budget;
    uint32_t spent;
    uint32_t refills;
} reconomy_rec_t;

static reconomy_rec_t reconomy_table[RECON_CAPACITY];
static int reconomy_ready = 0;

void reconomy_initialize(void) {
    if (reconomy_ready == 1) return;

    for (uint32_t i = 0; i < RECON_CAPACITY; ++i) reconomy_table[i] = (reconomy_rec_t){0};
    reconomy_ready = 1;
}

int reconomy_register(uint32_t owner, uint32_t initial_budget) {
    if (reconomy_ready != 1 || owner == 0) return -1;

    for (uint32_t i = 0; i < RECON_CAPACITY; ++i) {
        if (reconomy_table[i].owner == 0) {
            reconomy_table[i].owner = owner;
            reconomy_table[i].budget = initial_budget;
            reconomy_table[i].spent = 0;
            reconomy_table[i].refills = 0;
            return 0;
        }
    }
    return -1;
}

int reconomy_spend(uint32_t owner, uint32_t cost) {
    if (reconomy_ready != 1) return -1;

    for (uint32_t i = 0; i < RECON_CAPACITY; ++i) {
        reconomy_rec_t *rec = &reconomy_table[i];
        if (rec->owner != owner) continue;
        if (rec->budget < cost) return -1;

        rec->budget -= cost;
        rec->spent += cost;

        raccount_charge(owner, cost, 0);
        return 0;
    }
    return -1;
}

int reconomy_refill(uint32_t owner, uint32_t refill_amount) {
    if (reconomy_ready != 1) return -1;

    for (uint32_t i = 0; i < RECON_CAPACITY; ++i) {
        if (reconomy_table[i].owner == owner) {
            reconomy_table[i].budget += refill_amount;
            ++reconomy_table[i].refills;
            return 0;
        }
    }
    return -1;
}

int reconomy_self_test(void) {
    if (reconomy_register(1, 50) != 0) return -1;

    if (reconomy_spend(1, 30) != 0) return -1;
    if (reconomy_spend(1, 30) == 0) return -1; /* Budget erschoepft */

    if (reconomy_refill(1, 100) != 0) return -1;
    if (reconomy_spend(1, 30) != 0) return -1;

    return 0;
}
