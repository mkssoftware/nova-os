/* NovaOS C-Kernel - Nova TLB Management (siehe tlb.h fuer Scope). */
#include "tlb.h"
#include "smp.h"

#define TLB_PAGE_SIZE 4096u

static uint32_t g_invalidation_count = 0;
static uint32_t g_flush_count = 0;

void tlb_initialize(void) {
    g_invalidation_count = 0;
    g_flush_count = 0;
}

void tlb_invalidate_page(uint32_t virtual_page) {
    __asm__ volatile ("invlpg (%0)" : : "r"(virtual_page) : "memory");
    ++g_invalidation_count;
}

void tlb_invalidate_range(uint32_t virtual_base, uint32_t size) {
    uint32_t pages = (size + TLB_PAGE_SIZE - 1u) / TLB_PAGE_SIZE;
    for (uint32_t i = 0; i < pages; ++i) {
        tlb_invalidate_page(virtual_base + i * TLB_PAGE_SIZE);
    }
}

void tlb_flush_all(void) {
    uint32_t cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));
    __asm__ volatile ("mov %0, %%cr3" : : "r"(cr3) : "memory");
    ++g_flush_count;
}

int tlb_shootdown(uint32_t ap_index, uint32_t virtual_page) {
    (void)virtual_page;
    if (ap_index >= smp_ap_online_count()) {
        return -1;
    }
    /* Siehe tlb.h-Kopfkommentar: APs idlen dauerhaft mit IF=0 und koennen
       derzeit keinen Shootdown-IPI entgegennehmen. */
    return -2;
}

uint32_t tlb_invalidation_count(void) { return g_invalidation_count; }
uint32_t tlb_flush_count(void) { return g_flush_count; }

#define TLB_TEST_VIRT 0x3E000000u

int tlb_self_test(void) {
    tlb_initialize();

    tlb_invalidate_page(TLB_TEST_VIRT);
    if (tlb_invalidation_count() != 1) return -1;

    tlb_invalidate_range(TLB_TEST_VIRT, 3u * TLB_PAGE_SIZE);
    if (tlb_invalidation_count() != 4) return -1; /* 1 + 3 Seiten */

    uint32_t flush_before = tlb_flush_count();
    tlb_flush_all();
    if (tlb_flush_count() != flush_before + 1u) return -1;

    /* Shootdown an einen nie online gekommenen AP-Index muss -1 liefern. */
    if (tlb_shootdown(0xFFFFu, TLB_TEST_VIRT) != -1) return -1;

    /* Shootdown an einen tatsaechlich onlinen AP (falls vorhanden) muss
       ehrlich -2 liefern (siehe Scope-Hinweis) - im reinen UP-Betrieb
       (smp_ap_online_count() == 0) ist dieser Zweig nicht erreichbar und
       wird uebersprungen. */
    if (smp_ap_online_count() > 0) {
        if (tlb_shootdown(0u, TLB_TEST_VIRT) != -2) return -1;
    }

    return 0;
}
