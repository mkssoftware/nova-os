/* NovaOS C-Kernel - Nova Virtual Address Space (siehe vas.h fuer Scope und
   Anpassungshinweise). */
#include "vas.h"
#include "pagetable.h"
#include "framealloc.h"

#define VAS_PAGE_SIZE 4096u

static vas_record_t g_vas[VAS_CAPACITY];

void vas_initialize(void) {
    for (uint32_t i = 0; i < VAS_CAPACITY; ++i) {
        g_vas[i].id = 0;
        g_vas[i].owner = 0;
        g_vas[i].type = 0;
        g_vas[i].state = VAS_STATE_FREE;
        g_vas[i].generation = 0;
        g_vas[i].fault_count = 0;
        for (uint32_t r = 0; r < VAS_REGION_CAPACITY; ++r) {
            g_vas[i].regions[r].used = 0;
        }
        for (uint32_t m = 0; m < VAS_MAPPING_CAPACITY; ++m) {
            g_vas[i].mappings[m].used = 0;
        }
    }
}

static vas_record_t *find_vas(uint32_t id) {
    if (id == 0 || id > VAS_CAPACITY) return 0;
    vas_record_t *rec = &g_vas[id - 1];
    if (rec->id != id || rec->state != VAS_STATE_ACTIVE) return 0;
    return rec;
}

int vas_create(uint32_t owner, uint32_t type, uint32_t *out_id) {
    for (uint32_t i = 0; i < VAS_CAPACITY; ++i) {
        if (g_vas[i].state != VAS_STATE_FREE) continue;
        g_vas[i].id = i + 1u;
        g_vas[i].owner = owner;
        g_vas[i].type = type;
        g_vas[i].state = VAS_STATE_ACTIVE;
        ++g_vas[i].generation;
        g_vas[i].fault_count = 0;
        for (uint32_t r = 0; r < VAS_REGION_CAPACITY; ++r) g_vas[i].regions[r].used = 0;
        for (uint32_t m = 0; m < VAS_MAPPING_CAPACITY; ++m) g_vas[i].mappings[m].used = 0;
        *out_id = g_vas[i].id;
        return 0;
    }
    return -1;
}

static int unmap_one_mapping(vas_mapping_t *mapping) {
    uint32_t pages = (mapping->size + VAS_PAGE_SIZE - 1u) / VAS_PAGE_SIZE;
    for (uint32_t p = 0; p < pages; ++p) {
        uint32_t vpage = mapping->virtual_base + p * VAS_PAGE_SIZE;
        uint32_t phys, flags;
        if (pagetable_query(vpage, &phys, &flags) == 0) {
            pagetable_unmap(vpage);
            if (mapping->owns_frames) {
                frame_unref(phys);
            }
        }
    }
    mapping->used = 0;
    return 0;
}

int vas_destroy(uint32_t id) {
    vas_record_t *rec = find_vas(id);
    if (!rec) return -1;

    rec->state = VAS_STATE_DYING;
    for (uint32_t m = 0; m < VAS_MAPPING_CAPACITY; ++m) {
        if (rec->mappings[m].used) {
            unmap_one_mapping(&rec->mappings[m]);
        }
    }
    for (uint32_t r = 0; r < VAS_REGION_CAPACITY; ++r) {
        rec->regions[r].used = 0;
    }
    rec->state = VAS_STATE_FREE;
    rec->id = 0;
    return 0;
}

static int ranges_overlap(uint32_t base_a, uint32_t size_a, uint32_t base_b, uint32_t size_b) {
    uint32_t end_a = base_a + size_a;
    uint32_t end_b = base_b + size_b;
    return (base_a < end_b) && (base_b < end_a);
}

int vas_reserve_region(uint32_t id, uint32_t base, uint32_t size, uint32_t region_type) {
    vas_record_t *rec = find_vas(id);
    if (!rec || size == 0) return -1;

    for (uint32_t r = 0; r < VAS_REGION_CAPACITY; ++r) {
        if (!rec->regions[r].used) continue;
        if (ranges_overlap(base, size, rec->regions[r].base, rec->regions[r].size)) {
            return -1;
        }
    }
    for (uint32_t r = 0; r < VAS_REGION_CAPACITY; ++r) {
        if (rec->regions[r].used) continue;
        rec->regions[r].base = base;
        rec->regions[r].size = size;
        rec->regions[r].type = region_type;
        rec->regions[r].used = 1;
        return 0;
    }
    return -1;
}

static vas_region_t *find_containing_region(vas_record_t *rec, uint32_t base, uint32_t size) {
    for (uint32_t r = 0; r < VAS_REGION_CAPACITY; ++r) {
        if (!rec->regions[r].used) continue;
        uint32_t rbase = rec->regions[r].base;
        uint32_t rend = rbase + rec->regions[r].size;
        if (base >= rbase && (base + size) <= rend) {
            return &rec->regions[r];
        }
    }
    return 0;
}

static int vas_map_impl(uint32_t id, uint32_t virtual_base, uint32_t size, uint32_t permissions,
                         uint32_t fixed_physical_base, int use_fixed) {
    vas_record_t *rec = find_vas(id);
    if (!rec || size == 0) return -1;

    /* W^X: Schreib- UND Ausfuehrbarkeit gleichzeitig nur mit expliziter
       Ausnahme (NPSPEC-MEMORY-VAS-0001 Anforderung #8). */
    if ((permissions & VAS_PERM_WRITE) && (permissions & VAS_PERM_EXEC) &&
        !(permissions & VAS_PERM_FORCE_WX)) {
        return -1;
    }

    vas_region_t *region = find_containing_region(rec, virtual_base, size);
    if (!region || region->type == VAS_REGION_GUARD) {
        return -1;
    }

    uint32_t slot = VAS_MAPPING_CAPACITY;
    for (uint32_t m = 0; m < VAS_MAPPING_CAPACITY; ++m) {
        if (!rec->mappings[m].used) { slot = m; break; }
    }
    if (slot == VAS_MAPPING_CAPACITY) return -1;

    uint32_t flags = PAGETABLE_PRESENT;
    if (permissions & VAS_PERM_WRITE) flags |= PAGETABLE_WRITE;
    if (permissions & VAS_PERM_USER) flags |= PAGETABLE_USER;

    uint32_t pages = (size + VAS_PAGE_SIZE - 1u) / VAS_PAGE_SIZE;
    uint32_t first_phys = 0;
    for (uint32_t p = 0; p < pages; ++p) {
        uint32_t phys;
        if (use_fixed) {
            phys = fixed_physical_base + p * VAS_PAGE_SIZE;
        } else {
            uint32_t numa;
            phys = frame_alloc(&numa);
            if (phys == 0) {
                for (uint32_t u = 0; u < p; ++u) {
                    uint32_t vpage = virtual_base + u * VAS_PAGE_SIZE;
                    uint32_t qphys, qflags;
                    if (pagetable_query(vpage, &qphys, &qflags) == 0) {
                        pagetable_unmap(vpage);
                        frame_unref(qphys);
                    }
                }
                return -1;
            }
        }
        if (p == 0) first_phys = phys;
        uint32_t vpage = virtual_base + p * VAS_PAGE_SIZE;
        if (pagetable_map(vpage, phys, flags) != 0) {
            if (!use_fixed) frame_unref(phys);
            for (uint32_t u = 0; u < p; ++u) {
                uint32_t vp2 = virtual_base + u * VAS_PAGE_SIZE;
                uint32_t qphys, qflags;
                if (pagetable_query(vp2, &qphys, &qflags) == 0) {
                    pagetable_unmap(vp2);
                    if (!use_fixed) frame_unref(qphys);
                }
            }
            return -1;
        }
    }

    rec->mappings[slot].virtual_base = virtual_base;
    rec->mappings[slot].size = pages * VAS_PAGE_SIZE;
    rec->mappings[slot].permissions = permissions;
    rec->mappings[slot].physical_base = first_phys;
    rec->mappings[slot].owns_frames = use_fixed ? 0u : 1u;
    rec->mappings[slot].used = 1;
    return 0;
}

int vas_map(uint32_t id, uint32_t virtual_base, uint32_t size, uint32_t permissions) {
    return vas_map_impl(id, virtual_base, size, permissions, 0, 0);
}

int vas_map_fixed(uint32_t id, uint32_t virtual_base, uint32_t physical_base,
                   uint32_t size, uint32_t permissions) {
    return vas_map_impl(id, virtual_base, size, permissions, physical_base, 1);
}

int vas_protect(uint32_t id, uint32_t virtual_base, uint32_t new_permissions) {
    vas_record_t *rec = find_vas(id);
    if (!rec) return -1;

    if ((new_permissions & VAS_PERM_WRITE) && (new_permissions & VAS_PERM_EXEC) &&
        !(new_permissions & VAS_PERM_FORCE_WX)) {
        return -1;
    }

    for (uint32_t m = 0; m < VAS_MAPPING_CAPACITY; ++m) {
        vas_mapping_t *mapping = &rec->mappings[m];
        if (!mapping->used || mapping->virtual_base != virtual_base) continue;

        uint32_t flags = PAGETABLE_PRESENT;
        if (new_permissions & VAS_PERM_WRITE) flags |= PAGETABLE_WRITE;
        if (new_permissions & VAS_PERM_USER) flags |= PAGETABLE_USER;

        uint32_t pages = mapping->size / VAS_PAGE_SIZE;
        for (uint32_t p = 0; p < pages; ++p) {
            uint32_t vpage = virtual_base + p * VAS_PAGE_SIZE;
            if (pagetable_protect(vpage, flags) != 0) {
                return -1;
            }
        }
        mapping->permissions = new_permissions;
        return 0;
    }
    return -1;
}

int vas_map_shared(uint32_t id, uint32_t virtual_base, uint32_t physical_base,
                    uint32_t size, uint32_t permissions) {
    vas_record_t *rec = find_vas(id);
    if (!rec || size == 0) return -1;

    if ((permissions & VAS_PERM_WRITE) && (permissions & VAS_PERM_EXEC) &&
        !(permissions & VAS_PERM_FORCE_WX)) {
        return -1;
    }

    vas_region_t *region = find_containing_region(rec, virtual_base, size);
    if (!region || region->type == VAS_REGION_GUARD) {
        return -1;
    }

    uint32_t slot = VAS_MAPPING_CAPACITY;
    for (uint32_t m = 0; m < VAS_MAPPING_CAPACITY; ++m) {
        if (!rec->mappings[m].used) { slot = m; break; }
    }
    if (slot == VAS_MAPPING_CAPACITY) return -1;

    uint32_t flags = PAGETABLE_PRESENT;
    if (permissions & VAS_PERM_WRITE) flags |= PAGETABLE_WRITE;
    if (permissions & VAS_PERM_USER) flags |= PAGETABLE_USER;

    uint32_t pages = (size + VAS_PAGE_SIZE - 1u) / VAS_PAGE_SIZE;
    for (uint32_t p = 0; p < pages; ++p) {
        uint32_t phys = physical_base + p * VAS_PAGE_SIZE;
        if (frame_ref(phys) != 0) {
            for (uint32_t u = 0; u < p; ++u) {
                frame_unref(physical_base + u * VAS_PAGE_SIZE);
                pagetable_unmap(virtual_base + u * VAS_PAGE_SIZE);
            }
            return -1;
        }
        uint32_t vpage = virtual_base + p * VAS_PAGE_SIZE;
        if (pagetable_map(vpage, phys, flags) != 0) {
            frame_unref(phys);
            for (uint32_t u = 0; u < p; ++u) {
                frame_unref(physical_base + u * VAS_PAGE_SIZE);
                pagetable_unmap(virtual_base + u * VAS_PAGE_SIZE);
            }
            return -1;
        }
    }

    rec->mappings[slot].virtual_base = virtual_base;
    rec->mappings[slot].size = pages * VAS_PAGE_SIZE;
    rec->mappings[slot].permissions = permissions;
    rec->mappings[slot].physical_base = physical_base;
    rec->mappings[slot].owns_frames = 1u;
    rec->mappings[slot].used = 1;
    return 0;
}

int vas_unmap(uint32_t id, uint32_t virtual_base) {
    vas_record_t *rec = find_vas(id);
    if (!rec) return -1;

    for (uint32_t m = 0; m < VAS_MAPPING_CAPACITY; ++m) {
        if (rec->mappings[m].used && rec->mappings[m].virtual_base == virtual_base) {
            return unmap_one_mapping(&rec->mappings[m]);
        }
    }
    return -1;
}

int vas_mapping_permissions(uint32_t id, uint32_t virtual_base, uint32_t *out_permissions) {
    vas_record_t *rec = find_vas(id);
    if (!rec) return -1;
    for (uint32_t m = 0; m < VAS_MAPPING_CAPACITY; ++m) {
        if (rec->mappings[m].used && rec->mappings[m].virtual_base == virtual_base) {
            *out_permissions = rec->mappings[m].permissions;
            return 0;
        }
    }
    return -1;
}

int vas_region_type_at(uint32_t id, uint32_t virtual_address, uint32_t *out_type) {
    vas_record_t *rec = find_vas(id);
    if (!rec) return -1;
    for (uint32_t r = 0; r < VAS_REGION_CAPACITY; ++r) {
        if (!rec->regions[r].used) continue;
        uint32_t base = rec->regions[r].base;
        uint32_t end = base + rec->regions[r].size;
        if (virtual_address >= base && virtual_address < end) {
            *out_type = rec->regions[r].type;
            return 0;
        }
    }
    return -1;
}

int vas_region_count(uint32_t id, uint32_t *out_count) {
    vas_record_t *rec = find_vas(id);
    if (!rec) return -1;
    uint32_t count = 0;
    for (uint32_t r = 0; r < VAS_REGION_CAPACITY; ++r) {
        if (rec->regions[r].used) ++count;
    }
    *out_count = count;
    return 0;
}

int vas_mapping_count(uint32_t id, uint32_t *out_count) {
    vas_record_t *rec = find_vas(id);
    if (!rec) return -1;
    uint32_t count = 0;
    for (uint32_t m = 0; m < VAS_MAPPING_CAPACITY; ++m) {
        if (rec->mappings[m].used) ++count;
    }
    *out_count = count;
    return 0;
}

int vas_state_of(uint32_t id, uint32_t *out_state) {
    if (id == 0 || id > VAS_CAPACITY) return -1;
    if (g_vas[id - 1].id != id) return -1; /* bereits zerstoerter/freier Slot */
    *out_state = g_vas[id - 1].state;
    return 0;
}

/* Virtueller Testbereich fern von Identity-Map/APIC/Framebuffer und vom
   pagetable_self_test()-Scratch-Bereich (0x02000000). */
#define VAS_TEST_VIRT 0x03000000u

int vas_self_test(void) {
    vas_initialize();

    uint32_t id;
    if (vas_create(42u, VAS_TYPE_PROCESS, &id) != 0) return -1;

    uint32_t state;
    if (vas_state_of(id, &state) != 0 || state != VAS_STATE_ACTIVE) return -1;

    if (vas_reserve_region(id, VAS_TEST_VIRT, 2u * VAS_PAGE_SIZE, VAS_REGION_HEAP) != 0) return -1;
    /* Ueberlappende Zweitreservierung muss fehlschlagen. */
    if (vas_reserve_region(id, VAS_TEST_VIRT + VAS_PAGE_SIZE, VAS_PAGE_SIZE, VAS_REGION_DATA) == 0) return -1;

    uint32_t region_count;
    if (vas_region_count(id, &region_count) != 0 || region_count != 1u) return -1;

    /* W^X: Write+Exec ohne Ausnahme muss verweigert werden. */
    if (vas_map(id, VAS_TEST_VIRT, VAS_PAGE_SIZE, VAS_PERM_READ | VAS_PERM_WRITE | VAS_PERM_EXEC) == 0) return -1;

    /* Mapping ausserhalb jeder reservierten Region muss fehlschlagen. */
    if (vas_map(id, VAS_TEST_VIRT + 0x01000000u, VAS_PAGE_SIZE, VAS_PERM_READ) == 0) return -1;

    if (vas_map(id, VAS_TEST_VIRT, 2u * VAS_PAGE_SIZE, VAS_PERM_READ | VAS_PERM_WRITE) != 0) return -1;

    uint32_t mapping_count;
    if (vas_mapping_count(id, &mapping_count) != 0 || mapping_count != 1u) return -1;

    volatile uint32_t *scratch = (volatile uint32_t *)(uintptr_t)VAS_TEST_VIRT;
    *scratch = 0x00564153u; /* "VAS" */
    if (*scratch != 0x00564153u) return -1;

    /* Guard-Region: reserviert, aber niemals map-faehig. */
    if (vas_reserve_region(id, VAS_TEST_VIRT + 2u * VAS_PAGE_SIZE, VAS_PAGE_SIZE, VAS_REGION_GUARD) != 0) return -1;
    if (vas_map(id, VAS_TEST_VIRT + 2u * VAS_PAGE_SIZE, VAS_PAGE_SIZE, VAS_PERM_READ) == 0) return -1;

    /* Protection-Change: READ+WRITE -> nur READ, danach WRITE+EXEC ohne
       Ausnahme verweigert, Mapping bleibt unveraendert bestehen. */
    if (vas_protect(id, VAS_TEST_VIRT, VAS_PERM_READ) != 0) return -1;
    if (vas_protect(id, VAS_TEST_VIRT, VAS_PERM_WRITE | VAS_PERM_EXEC) == 0) return -1;
    if (vas_protect(id, VAS_TEST_VIRT + 0x00500000u, VAS_PERM_READ) == 0) return -1; /* kein Mapping dort */

    if (vas_unmap(id, VAS_TEST_VIRT) != 0) return -1;
    if (vas_mapping_count(id, &mapping_count) != 0 || mapping_count != 0u) return -1;

    /* Unmap != Free: der zweite Frame der ehemaligen Mehrseiten-Mapping war
       nicht separat referenziert, Pruefung beschraenkt sich auf Buchhaltung
       (kein direkter PMM-Introspektions-Zugriff aus diesem Test). */

    if (vas_destroy(id) != 0) return -1;
    if (vas_state_of(id, &state) == 0) return -1; /* ID jetzt ungueltig */

    /* Kapazitaetsgrenze. */
    uint32_t ids[VAS_CAPACITY];
    for (uint32_t i = 0; i < VAS_CAPACITY; ++i) {
        if (vas_create(i, VAS_TYPE_PROCESS, &ids[i]) != 0) return -1;
    }
    uint32_t overflow_id;
    if (vas_create(999u, VAS_TYPE_PROCESS, &overflow_id) == 0) return -1;
    for (uint32_t i = 0; i < VAS_CAPACITY; ++i) {
        if (vas_destroy(ids[i]) != 0) return -1;
    }

    return 0;
}
