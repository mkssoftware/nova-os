/* NovaOS C-Kernel - Nova Memory Protection Policy (siehe protect.h). */
#include "protect.h"
#include "vas.h"

static uint32_t g_fault_count = 0;

void protect_initialize(void) {
    g_fault_count = 0;
}

int protect_change(uint32_t vas_id, uint32_t virtual_base, uint32_t new_permissions, int authorized) {
    uint32_t current;
    if (vas_mapping_permissions(vas_id, virtual_base, &current) != 0) {
        ++g_fault_count;
        return -1;
    }

    /* Erweiterung = mindestens ein Bit in new_permissions, das in current
       nicht gesetzt ist. */
    uint32_t added_bits = new_permissions & ~current;
    if (added_bits != 0 && !authorized) {
        ++g_fault_count;
        return -1;
    }

    if (vas_protect(vas_id, virtual_base, new_permissions) != 0) {
        ++g_fault_count;
        return -1;
    }
    return 0;
}

uint32_t protect_fault_count(void) { return g_fault_count; }

#define PROTECT_TEST_VIRT 0x3D000000u

int protect_self_test(void) {
    protect_initialize();

    uint32_t vas_id;
    if (vas_create(7u, VAS_TYPE_PROCESS, &vas_id) != 0) return -1;
    if (vas_reserve_region(vas_id, PROTECT_TEST_VIRT, 4096u, VAS_REGION_DATA) != 0) return -1;
    if (vas_map(vas_id, PROTECT_TEST_VIRT, 4096u, VAS_PERM_READ) != 0) return -1;

    /* Erweiterung (READ -> READ+WRITE) ohne Autorisierung muss fehlschlagen. */
    uint32_t faults_before = protect_fault_count();
    if (protect_change(vas_id, PROTECT_TEST_VIRT, VAS_PERM_READ | VAS_PERM_WRITE, 0) == 0) {
        vas_destroy(vas_id);
        return -1;
    }
    if (protect_fault_count() != faults_before + 1u) {
        vas_destroy(vas_id);
        return -1;
    }

    /* Dieselbe Erweiterung MIT Autorisierung muss gelingen. */
    if (protect_change(vas_id, PROTECT_TEST_VIRT, VAS_PERM_READ | VAS_PERM_WRITE, 1) != 0) {
        vas_destroy(vas_id);
        return -1;
    }

    /* Reine Einschraenkung (READ+WRITE -> READ) ist immer erlaubt, auch
       ohne Autorisierung. */
    if (protect_change(vas_id, PROTECT_TEST_VIRT, VAS_PERM_READ, 0) != 0) {
        vas_destroy(vas_id);
        return -1;
    }

    /* Nicht existierendes Mapping zaehlt als Fault. */
    faults_before = protect_fault_count();
    if (protect_change(vas_id, PROTECT_TEST_VIRT + 0x00100000u, VAS_PERM_READ, 1) == 0) {
        vas_destroy(vas_id);
        return -1;
    }
    if (protect_fault_count() != faults_before + 1u) {
        vas_destroy(vas_id);
        return -1;
    }

    vas_destroy(vas_id);
    return 0;
}
