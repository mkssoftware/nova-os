/* NovaOS C-Kernel - Nova Shared Memory Objects (siehe shared.h fuer Scope). */
#include "shared.h"
#include "framealloc.h"
#include "pagetable.h"
#include "vas.h"

#define SHARED_PAGE_SIZE 4096u

typedef struct {
    uint32_t vas_id;
    uint32_t virtual_base;
    uint32_t used;
} shared_attach_t;

typedef struct {
    uint32_t id; /* 1-basiert, 0 = freier Slot */
    uint32_t frames[SHARED_MAX_PAGES]; /* NICHT zwingend physisch zusammenhaengend,
                                           siehe shared.h-Scope-Hinweis */
    uint32_t page_count;
    shared_attach_t attachments[SHARED_MAX_ATTACH];
} shared_record_t;

static shared_record_t g_shared[SHARED_CAPACITY];

void shared_initialize(void) {
    for (uint32_t i = 0; i < SHARED_CAPACITY; ++i) {
        g_shared[i].id = 0;
    }
}

static shared_record_t *find_shared(uint32_t id) {
    if (id == 0 || id > SHARED_CAPACITY) return 0;
    shared_record_t *rec = &g_shared[id - 1];
    if (rec->id != id) return 0;
    return rec;
}

static uint32_t attach_count_of(shared_record_t *rec) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < SHARED_MAX_ATTACH; ++i) {
        if (rec->attachments[i].used) ++count;
    }
    return count;
}

#define SHARED_ZERO_SCRATCH_VIRT 0x38000000u /* eigener Scratch-Bereich, siehe pool.c/pagetable.c */

static void zero_frame(uint32_t phys) {
    pagetable_map(SHARED_ZERO_SCRATCH_VIRT, phys, PAGETABLE_PRESENT | PAGETABLE_WRITE);
    volatile uint32_t *scratch = (volatile uint32_t *)(uintptr_t)SHARED_ZERO_SCRATCH_VIRT;
    for (uint32_t i = 0; i < SHARED_PAGE_SIZE / 4u; ++i) {
        scratch[i] = 0;
    }
    pagetable_unmap(SHARED_ZERO_SCRATCH_VIRT);
}

int shared_create(uint32_t size_bytes, uint32_t *out_id) {
    if (size_bytes == 0) return -1;
    uint32_t pages = (size_bytes + SHARED_PAGE_SIZE - 1u) / SHARED_PAGE_SIZE;
    if (pages > SHARED_MAX_PAGES) return -1;

    uint32_t slot = SHARED_CAPACITY;
    for (uint32_t i = 0; i < SHARED_CAPACITY; ++i) {
        if (g_shared[i].id == 0) { slot = i; break; }
    }
    if (slot == SHARED_CAPACITY) return -1;

    shared_record_t *rec = &g_shared[slot];

    /* Einzeln allokieren statt frame_alloc_contiguous(): das SMO-Backing
       muss NICHT physisch zusammenhaengend sein (siehe shared.h-Scope-
       Hinweis) - ein realer, bereits fragmentierter PMM-Pool wuerde eine
       zusammenhaengende Mehrseiten-Allokation hier oft verweigern
       (dokumentierte Best-Effort-Grenze von frame_alloc_contiguous()). */
    uint32_t got = 0;
    for (; got < pages; ++got) {
        uint32_t numa;
        uint32_t page = frame_alloc(&numa);
        if (page == 0) break;
        rec->frames[got] = page;
    }
    if (got != pages) {
        for (uint32_t i = 0; i < got; ++i) frame_unref(rec->frames[i]);
        return -1;
    }

    for (uint32_t i = 0; i < pages; ++i) {
        zero_frame(rec->frames[i]);
    }

    rec->page_count = pages;
    for (uint32_t i = 0; i < SHARED_MAX_ATTACH; ++i) rec->attachments[i].used = 0;
    rec->id = slot + 1u;
    *out_id = rec->id;
    return 0;
}

int shared_destroy(uint32_t id) {
    shared_record_t *rec = find_shared(id);
    if (!rec || attach_count_of(rec) != 0) return -1;

    for (uint32_t p = 0; p < rec->page_count; ++p) {
        frame_unref(rec->frames[p]);
    }
    rec->id = 0;
    return 0;
}

int shared_map(uint32_t id, uint32_t vas_id, uint32_t virtual_base, uint32_t permissions) {
    shared_record_t *rec = find_shared(id);
    if (!rec) return -1;

    uint32_t slot = SHARED_MAX_ATTACH;
    for (uint32_t i = 0; i < SHARED_MAX_ATTACH; ++i) {
        if (!rec->attachments[i].used) { slot = i; break; }
    }
    if (slot == SHARED_MAX_ATTACH) return -1;

    /* Jede (ggf. nicht zusammenhaengende) Seite einzeln mappen. */
    uint32_t mapped = 0;
    for (; mapped < rec->page_count; ++mapped) {
        uint32_t vpage = virtual_base + mapped * SHARED_PAGE_SIZE;
        if (vas_map_shared(vas_id, vpage, rec->frames[mapped], SHARED_PAGE_SIZE, permissions) != 0) {
            break;
        }
    }
    if (mapped != rec->page_count) {
        for (uint32_t u = 0; u < mapped; ++u) {
            vas_unmap(vas_id, virtual_base + u * SHARED_PAGE_SIZE);
        }
        return -1;
    }

    rec->attachments[slot].vas_id = vas_id;
    rec->attachments[slot].virtual_base = virtual_base;
    rec->attachments[slot].used = 1;
    return 0;
}

int shared_unmap(uint32_t id, uint32_t vas_id, uint32_t virtual_base) {
    shared_record_t *rec = find_shared(id);
    if (!rec) return -1;

    for (uint32_t i = 0; i < SHARED_MAX_ATTACH; ++i) {
        if (rec->attachments[i].used && rec->attachments[i].vas_id == vas_id &&
            rec->attachments[i].virtual_base == virtual_base) {
            for (uint32_t p = 0; p < rec->page_count; ++p) {
                vas_unmap(vas_id, virtual_base + p * SHARED_PAGE_SIZE);
            }
            rec->attachments[i].used = 0;
            return 0;
        }
    }
    return -1;
}

int shared_attach_count(uint32_t id, uint32_t *out_count) {
    shared_record_t *rec = find_shared(id);
    if (!rec) return -1;
    *out_count = attach_count_of(rec);
    return 0;
}

int shared_page_count(uint32_t id, uint32_t *out_count) {
    shared_record_t *rec = find_shared(id);
    if (!rec) return -1;
    *out_count = rec->page_count;
    return 0;
}

#define SHARED_TEST_VIRT_A 0x37000000u
#define SHARED_TEST_VIRT_B 0x37100000u

int shared_self_test(void) {
    shared_initialize();

    uint32_t smo_id;
    if (shared_create(2u * SHARED_PAGE_SIZE, &smo_id) != 0) return -1;

    uint32_t page_count;
    if (shared_page_count(smo_id, &page_count) != 0 || page_count != 2u) return -1;

    uint32_t vas_a, vas_b;
    if (vas_create(201u, VAS_TYPE_PROCESS, &vas_a) != 0) return -1;
    if (vas_create(202u, VAS_TYPE_PROCESS, &vas_b) != 0) { vas_destroy(vas_a); return -1; }

    if (vas_reserve_region(vas_a, SHARED_TEST_VIRT_A, 2u * SHARED_PAGE_SIZE, VAS_REGION_SHARED) != 0) goto fail;
    if (vas_reserve_region(vas_b, SHARED_TEST_VIRT_B, 2u * SHARED_PAGE_SIZE, VAS_REGION_SHARED) != 0) goto fail;

    /* Unterschiedliche Teilnehmer duerfen unterschiedliche Adressen UND
       unterschiedliche Permissions besitzen (Anforderung #4). */
    if (shared_map(smo_id, vas_a, SHARED_TEST_VIRT_A, VAS_PERM_READ | VAS_PERM_WRITE) != 0) goto fail;
    if (shared_map(smo_id, vas_b, SHARED_TEST_VIRT_B, VAS_PERM_READ) != 0) goto fail;

    uint32_t attach_count;
    if (shared_attach_count(smo_id, &attach_count) != 0 || attach_count != 2u) goto fail;

    /* Zerstoeren mit aktiven Teilnehmern muss verweigert werden
       (Anforderung #8). */
    if (shared_destroy(smo_id) == 0) goto fail;

    /* Echter Shared-Memory-Beweis: Schreiben ueber vas_a's Adresse,
       Lesen ueber vas_b's (andere) Adresse muss denselben Inhalt zeigen -
       dasselbe physische Backing, zwei unabhaengige Mappings. */
    volatile uint32_t *write_ptr = (volatile uint32_t *)(uintptr_t)SHARED_TEST_VIRT_A;
    *write_ptr = 0x00534D4Fu; /* "SMO" */
    volatile uint32_t *read_ptr = (volatile uint32_t *)(uintptr_t)SHARED_TEST_VIRT_B;
    if (*read_ptr != 0x00534D4Fu) goto fail;

    if (shared_unmap(smo_id, vas_a, SHARED_TEST_VIRT_A) != 0) goto fail;
    if (shared_attach_count(smo_id, &attach_count) != 0 || attach_count != 1u) goto fail;

    /* Entferntes Mapping darf das SMO nicht zerstoert haben - weiterhin
       ueber vas_b lesbar. */
    if (*read_ptr != 0x00534D4Fu) goto fail;

    if (shared_unmap(smo_id, vas_b, SHARED_TEST_VIRT_B) != 0) goto fail;
    if (shared_attach_count(smo_id, &attach_count) != 0 || attach_count != 0u) goto fail;

    if (shared_destroy(smo_id) != 0) goto fail;

    vas_destroy(vas_a);
    vas_destroy(vas_b);
    return 0;

fail:
    vas_destroy(vas_a);
    vas_destroy(vas_b);
    return -1;
}
