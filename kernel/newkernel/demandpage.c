/* NovaOS C-Kernel - Nova Demand Paging (siehe demandpage.h fuer Scope). */
#include "demandpage.h"
#include "vas.h"
#include "pagetable.h"

#define DEMANDPAGE_PAGE_SIZE 4096u

static uint32_t g_fault_count = 0;
static uint32_t g_resolved_count = 0;
static uint32_t g_invalid_count = 0;

void demandpage_initialize(void) {
    g_fault_count = 0;
    g_resolved_count = 0;
    g_invalid_count = 0;
}

int demandpage_fault(uint32_t vas_id, uint32_t faulting_address, uint32_t permissions,
                      uint32_t preferred_numa_node) {
    (void)preferred_numa_node; /* siehe demandpage.h-Scope-Hinweis: PMM liefert aktuell
                                   immer PMM_NUMA_UNKNOWN, der Parameter ist bewusst noch
                                   inert. */
    ++g_fault_count;

    uint32_t page_base = faulting_address & ~(DEMANDPAGE_PAGE_SIZE - 1u);

    uint32_t phys, flags;
    if (pagetable_query(page_base, &phys, &flags) == 0) {
        return DEMANDPAGE_ALREADY_MAPPED;
    }

    uint32_t region_type;
    if (vas_region_type_at(vas_id, page_base, &region_type) != 0 ||
        region_type == VAS_REGION_GUARD) {
        ++g_invalid_count;
        return DEMANDPAGE_INVALID;
    }

    if (vas_map(vas_id, page_base, DEMANDPAGE_PAGE_SIZE, permissions) != 0) {
        return DEMANDPAGE_OOM;
    }

    /* Anforderung #5: keine Daten eines vorherigen Sicherheitskontexts
       duerfen im frisch bereitgestellten Frame sichtbar bleiben. */
    volatile uint32_t *page = (volatile uint32_t *)(uintptr_t)page_base;
    for (uint32_t i = 0; i < DEMANDPAGE_PAGE_SIZE / 4u; ++i) {
        page[i] = 0;
    }

    ++g_resolved_count;
    return DEMANDPAGE_RESOLVED;
}

uint32_t demandpage_fault_count(void) { return g_fault_count; }
uint32_t demandpage_resolved_count(void) { return g_resolved_count; }
uint32_t demandpage_invalid_count(void) { return g_invalid_count; }

#define DEMANDPAGE_TEST_VIRT 0x3A000000u

int demandpage_self_test(void) {
    demandpage_initialize();

    uint32_t vas_id;
    if (vas_create(77u, VAS_TYPE_PROCESS, &vas_id) != 0) return -1;
    if (vas_reserve_region(vas_id, DEMANDPAGE_TEST_VIRT, 2u * DEMANDPAGE_PAGE_SIZE, VAS_REGION_HEAP) != 0) {
        vas_destroy(vas_id);
        return -1;
    }

    /* Fault auf eine Adresse ausserhalb jeder Region muss INVALID liefern,
       ohne die Resolved-Zaehlung zu erhoehen. */
    if (demandpage_fault(vas_id, DEMANDPAGE_TEST_VIRT + 0x01000000u, VAS_PERM_READ, 0) != DEMANDPAGE_INVALID) {
        vas_destroy(vas_id);
        return -1;
    }
    if (demandpage_invalid_count() != 1u) {
        vas_destroy(vas_id);
        return -1;
    }

    /* Echter Fault: muss aufgeloest werden. */
    if (demandpage_fault(vas_id, DEMANDPAGE_TEST_VIRT + 4u, VAS_PERM_READ | VAS_PERM_WRITE, 0) != DEMANDPAGE_RESOLVED) {
        vas_destroy(vas_id);
        return -1;
    }
    if (demandpage_resolved_count() != 1u) {
        vas_destroy(vas_id);
        return -1;
    }

    /* Inhalt muss genullt sein. */
    volatile uint32_t *page = (volatile uint32_t *)(uintptr_t)DEMANDPAGE_TEST_VIRT;
    if (page[1] != 0) {
        vas_destroy(vas_id);
        return -1;
    }

    /* Erneuter Fault auf dieselbe (jetzt bereits gemappte) Seite muss
       ALREADY_MAPPED liefern, ohne einen zweiten Frame zu allokieren. */
    if (demandpage_fault(vas_id, DEMANDPAGE_TEST_VIRT + 8u, VAS_PERM_READ, 0) != DEMANDPAGE_ALREADY_MAPPED) {
        vas_destroy(vas_id);
        return -1;
    }
    if (demandpage_resolved_count() != 1u) {
        vas_destroy(vas_id);
        return -1;
    }

    /* Guard-Region darf nie per Demand Paging aufgeloest werden. */
    if (vas_reserve_region(vas_id, DEMANDPAGE_TEST_VIRT + 2u * DEMANDPAGE_PAGE_SIZE,
                            DEMANDPAGE_PAGE_SIZE, VAS_REGION_GUARD) != 0) {
        vas_destroy(vas_id);
        return -1;
    }
    if (demandpage_fault(vas_id, DEMANDPAGE_TEST_VIRT + 2u * DEMANDPAGE_PAGE_SIZE, VAS_PERM_READ, 0)
        != DEMANDPAGE_INVALID) {
        vas_destroy(vas_id);
        return -1;
    }

    if (demandpage_fault_count() != 4u) {
        vas_destroy(vas_id);
        return -1;
    }

    vas_destroy(vas_id);
    return 0;
}
