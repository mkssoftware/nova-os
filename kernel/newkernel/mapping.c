/* NovaOS C-Kernel - Nova Memory Mapping Semantics (siehe mapping.h). */
#include "mapping.h"
#include "vas.h"

typedef struct {
    uint32_t vas_id;
    uint32_t virtual_base;
    uint32_t map_type;
    uint32_t cache_policy;
    uint32_t used;
} mapping_track_t;

static mapping_track_t g_tracked[MAPPING_TRACK_CAPACITY];

void mapping_initialize(void) {
    for (uint32_t i = 0; i < MAPPING_TRACK_CAPACITY; ++i) {
        g_tracked[i].used = 0;
    }
}

static int32_t find_tracked(uint32_t vas_id, uint32_t virtual_base) {
    for (uint32_t i = 0; i < MAPPING_TRACK_CAPACITY; ++i) {
        if (g_tracked[i].used && g_tracked[i].vas_id == vas_id &&
            g_tracked[i].virtual_base == virtual_base) {
            return (int32_t)i;
        }
    }
    return -1;
}

static int32_t find_free_slot(void) {
    for (uint32_t i = 0; i < MAPPING_TRACK_CAPACITY; ++i) {
        if (!g_tracked[i].used) return (int32_t)i;
    }
    return -1;
}

int mapping_create(uint32_t vas_id, uint32_t virtual_base, uint32_t size,
                    uint32_t permissions, uint32_t map_type,
                    uint32_t backing_object_id, uint32_t backing_permissions) {
    (void)backing_object_id;

    /* Anforderung #6: Mapping-Permissions duerfen die Rechte des Backing
       Objects bei OBJECT-backed Mappings nicht erweitern. */
    if (map_type == MAPPING_TYPE_OBJECT) {
        if ((permissions & ~backing_permissions) != 0) {
            return -1;
        }
    }

    int32_t slot = find_free_slot();
    if (slot < 0) return -1;

    if (vas_map(vas_id, virtual_base, size, permissions) != 0) {
        return -1;
    }

    g_tracked[slot].vas_id = vas_id;
    g_tracked[slot].virtual_base = virtual_base;
    g_tracked[slot].map_type = map_type;
    g_tracked[slot].cache_policy = MAPPING_CACHE_WRITEBACK;
    g_tracked[slot].used = 1;
    return 0;
}

int mapping_create_device(uint32_t vas_id, uint32_t virtual_base, uint32_t physical_base,
                           uint32_t size, uint32_t permissions, uint32_t cache_policy) {
    int32_t slot = find_free_slot();
    if (slot < 0) return -1;

    if (vas_map_fixed(vas_id, virtual_base, physical_base, size, permissions) != 0) {
        return -1;
    }

    g_tracked[slot].vas_id = vas_id;
    g_tracked[slot].virtual_base = virtual_base;
    g_tracked[slot].map_type = MAPPING_TYPE_DEVICE;
    g_tracked[slot].cache_policy = cache_policy;
    g_tracked[slot].used = 1;
    return 0;
}

int mapping_destroy(uint32_t vas_id, uint32_t virtual_base) {
    int32_t slot = find_tracked(vas_id, virtual_base);
    if (slot < 0) return -1;

    if (vas_unmap(vas_id, virtual_base) != 0) {
        return -1;
    }
    g_tracked[slot].used = 0;
    return 0;
}

int mapping_type_of(uint32_t vas_id, uint32_t virtual_base, uint32_t *out_type) {
    int32_t slot = find_tracked(vas_id, virtual_base);
    if (slot < 0) return -1;
    *out_type = g_tracked[slot].map_type;
    return 0;
}

int mapping_cache_policy_of(uint32_t vas_id, uint32_t virtual_base, uint32_t *out_policy) {
    int32_t slot = find_tracked(vas_id, virtual_base);
    if (slot < 0) return -1;
    *out_policy = g_tracked[slot].cache_policy;
    return 0;
}

uint32_t mapping_tracked_count(void) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < MAPPING_TRACK_CAPACITY; ++i) {
        if (g_tracked[i].used) ++count;
    }
    return count;
}

#define MAPPING_TEST_VIRT_A 0x3C000000u
#define MAPPING_TEST_VIRT_B 0x3C100000u
#define MAPPING_TEST_DEVICE_PHYS 0xFEB00000u /* fern von APIC/Framebuffer, nur Buchhaltung im Test */

int mapping_self_test(void) {
    mapping_initialize();

    uint32_t vas_id;
    if (vas_create(55u, VAS_TYPE_PROCESS, &vas_id) != 0) return -1;
    if (vas_reserve_region(vas_id, MAPPING_TEST_VIRT_A, 4096u, VAS_REGION_HEAP) != 0) return -1;
    if (vas_reserve_region(vas_id, MAPPING_TEST_VIRT_B, 4096u, VAS_REGION_MMAP) != 0) return -1;

    /* Anonymous Mapping. */
    if (mapping_create(vas_id, MAPPING_TEST_VIRT_A, 4096u, VAS_PERM_READ | VAS_PERM_WRITE,
                        MAPPING_TYPE_ANONYMOUS, 0, 0) != 0) {
        vas_destroy(vas_id);
        return -1;
    }
    uint32_t type;
    if (mapping_type_of(vas_id, MAPPING_TEST_VIRT_A, &type) != 0 || type != MAPPING_TYPE_ANONYMOUS) {
        vas_destroy(vas_id);
        return -1;
    }
    if (mapping_tracked_count() != 1u) {
        vas_destroy(vas_id);
        return -1;
    }

    /* Object-backed Mapping darf Backing-Rechte nicht erweitern. */
    if (mapping_create(vas_id, MAPPING_TEST_VIRT_B, 4096u, VAS_PERM_READ | VAS_PERM_WRITE,
                        MAPPING_TYPE_OBJECT, 42u, VAS_PERM_READ) == 0) {
        vas_destroy(vas_id);
        return -1;
    }
    /* Mit ausreichender Backing-Permission muss es gelingen. */
    if (mapping_create(vas_id, MAPPING_TEST_VIRT_B, 4096u, VAS_PERM_READ,
                        MAPPING_TYPE_OBJECT, 42u, VAS_PERM_READ | VAS_PERM_WRITE) != 0) {
        vas_destroy(vas_id);
        return -1;
    }

    if (mapping_destroy(vas_id, MAPPING_TEST_VIRT_A) != 0) {
        vas_destroy(vas_id);
        return -1;
    }
    if (mapping_tracked_count() != 1u) {
        vas_destroy(vas_id);
        return -1;
    }
    /* Doppeltes Destroy muss fehlschlagen. */
    if (mapping_destroy(vas_id, MAPPING_TEST_VIRT_A) == 0) {
        vas_destroy(vas_id);
        return -1;
    }

    if (mapping_destroy(vas_id, MAPPING_TEST_VIRT_B) != 0) {
        vas_destroy(vas_id);
        return -1;
    }

    /* Device-Mapping: direkte physische Adresse, eigene Cache-Policy.
       Nutzt die bereits bei vas_create() reservierte HEAP-Region an
       MAPPING_TEST_VIRT_A weiter - Regionen bleiben bis vas_destroy()
       reserviert, auch wenn ihr Mapping bereits entfernt wurde. */
    if (mapping_create_device(vas_id, MAPPING_TEST_VIRT_A, MAPPING_TEST_DEVICE_PHYS, 4096u,
                               VAS_PERM_READ | VAS_PERM_WRITE, MAPPING_CACHE_UNCACHED) != 0) {
        vas_destroy(vas_id);
        return -1;
    }
    uint32_t policy;
    if (mapping_cache_policy_of(vas_id, MAPPING_TEST_VIRT_A, &policy) != 0 ||
        policy != MAPPING_CACHE_UNCACHED) {
        vas_destroy(vas_id);
        return -1;
    }
    if (mapping_destroy(vas_id, MAPPING_TEST_VIRT_A) != 0) {
        vas_destroy(vas_id);
        return -1;
    }

    vas_destroy(vas_id);
    return 0;
}
