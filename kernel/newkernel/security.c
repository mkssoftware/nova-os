/* NovaOS C-Kernel - Kernel Security / Capability Manager (Port aus
   security32.inc). */
#include "security.h"
#include "process.h"

typedef struct {
    uint32_t pid;
    uint32_t caps;
} security_record_t;

static security_record_t security_table[SECURITY_CAPACITY];
static uint32_t security_count = 0;

int security_grant(uint32_t pid, uint32_t caps) {
    if (caps == 0) return -1;
    if (process_lookup(pid) == 0) return -1;

    for (uint32_t i = 0; i < SECURITY_CAPACITY; ++i) {
        if (security_table[i].pid == pid) {
            security_table[i].caps |= caps;
            return 0;
        }
        if (security_table[i].pid == 0) {
            security_table[i].pid = pid;
            security_table[i].caps |= caps;
            ++security_count;
            return 0;
        }
    }
    return -1;
}

int security_revoke(uint32_t pid, uint32_t caps) {
    for (uint32_t i = 0; i < SECURITY_CAPACITY; ++i) {
        if (security_table[i].pid == pid) {
            security_table[i].caps &= ~caps;
            return 0;
        }
    }
    return -1;
}

int security_check(uint32_t pid, uint32_t required_caps) {
    for (uint32_t i = 0; i < SECURITY_CAPACITY; ++i) {
        if (security_table[i].pid == pid) {
            return ((security_table[i].caps & required_caps) == required_caps) ? 1 : 0;
        }
    }
    return 0;
}

void security_initialize(void) {
    for (uint32_t i = 0; i < SECURITY_CAPACITY; ++i) security_table[i] = (security_record_t){0};
    security_count = 0;
    security_grant(1, SECURITY_KERNEL_CAPS);
}

int security_self_test(void) {
    if (security_check(1, SECURITY_CAP_ADMIN | SECURITY_CAP_SERVICE) != 1) return -1;
    if (security_check(1, 0x80000000u) != 0) return -1; /* unbelegte Capability */
    if (security_check(0xFFFFFFFFu, SECURITY_CAP_MEMORY) != 0) return -1; /* unbekannte PID */
    if (security_count != 1) return -1;
    return 0;
}
