/* NovaOS C-Kernel - §105 Versioned Kernel Service ABI (siehe vsvc.h fuer
   Scope). */
#include "vsvc.h"
#include "evbus.h"

#define VSVC_TBL_HDR_SIZE 24u

/* Event-IDs §105 (fortlaufend nach NP_EVENT_DIAG_HEALTH_CHANGED=12, siehe
   observability.c). */
#define NP_EVENT_SVC_REGISTERED   13u
#define NP_EVENT_SVC_ACTIVATED    14u
#define NP_EVENT_SVC_OFFLINE      15u
#define NP_EVENT_SVC_QUIESCING    16u
#define NP_EVENT_SVC_UNREGISTERED 17u

typedef struct {
    int present;
    vsvc_uuid_t uuid;
    uint32_t major, minor, patch;
    uint32_t stability;
    uint32_t state;
    const void *table;
    uint32_t table_size;
    uint32_t features;
    uint32_t required_caps;
    uint32_t arch_mask;
    uint32_t flags;
    uint32_t refcount;
    const char *name;
} vsvc_record_t;

static vsvc_record_t vsvc_registry[VSVC_MAX_SERVICES];
static uint32_t vsvc_stat_registered = 0;
static uint32_t vsvc_stat_acquired = 0;

static int vsvc_uuid_equal(const vsvc_uuid_t *a, const vsvc_uuid_t *b) {
    return a->id[0] == b->id[0] && a->id[1] == b->id[1] &&
           a->id[2] == b->id[2] && a->id[3] == b->id[3];
}

int vsvc_find(const vsvc_uuid_t *uuid, uint32_t *out_slot) {
    for (uint32_t i = 0; i < VSVC_MAX_SERVICES; ++i) {
        if (vsvc_registry[i].present && vsvc_uuid_equal(&vsvc_registry[i].uuid, uuid)) {
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int vsvc_register(const vsvc_uuid_t *uuid, uint32_t major, uint32_t minor, uint32_t patch,
                   uint32_t stability, const void *table, uint32_t table_size,
                   uint32_t features, uint32_t required_caps, uint32_t arch_mask,
                   uint32_t flags, const char *name, uint32_t *out_slot) {
    uint32_t existing;
    if (vsvc_find(uuid, &existing) == 0) return -1; /* Duplikat */
    if (table_size < VSVC_TBL_HDR_SIZE) return -1;

    for (uint32_t i = 0; i < VSVC_MAX_SERVICES; ++i) {
        if (vsvc_registry[i].present) continue;

        vsvc_registry[i].present = 1;
        vsvc_registry[i].uuid = *uuid;
        vsvc_registry[i].major = major;
        vsvc_registry[i].minor = minor;
        vsvc_registry[i].patch = patch;
        vsvc_registry[i].stability = stability;
        vsvc_registry[i].state = VSVC_STATE_ACTIVE;
        vsvc_registry[i].table = table;
        vsvc_registry[i].table_size = table_size;
        vsvc_registry[i].features = features;
        vsvc_registry[i].required_caps = required_caps;
        vsvc_registry[i].arch_mask = arch_mask;
        vsvc_registry[i].flags = flags;
        vsvc_registry[i].refcount = 0;
        vsvc_registry[i].name = name;

        ++vsvc_stat_registered;
        evbus_publish(NP_EVENT_SVC_REGISTERED, uuid->id[0], 0u, 0, 0u);

        *out_slot = i;
        return 0;
    }
    return -1; /* voll */
}

int vsvc_acquire(const vsvc_uuid_t *uuid, uint32_t min_major, uint32_t max_major,
                  uint32_t min_minor, uint32_t required_features, uint32_t *out_slot) {
    uint32_t slot;
    if (vsvc_find(uuid, &slot) != 0) return -1;

    vsvc_record_t *rec = &vsvc_registry[slot];
    if (rec->state != VSVC_STATE_ACTIVE) return -1;
    if (rec->major < min_major || rec->major > max_major) return -1;
    if (rec->minor < min_minor) return -1;
    if ((rec->features & required_features) != required_features) return -1;
    if (rec->arch_mask != VSVC_ARCH_ANY && (rec->arch_mask & VSVC_ARCH_X86_32) == 0u) return -1;

    ++rec->refcount;
    ++vsvc_stat_acquired;
    *out_slot = slot;
    return 0;
}

int vsvc_release(uint32_t slot) {
    if (slot >= VSVC_MAX_SERVICES) return -1;
    vsvc_record_t *rec = &vsvc_registry[slot];
    if (!rec->present || rec->refcount == 0u) return -1;

    --rec->refcount;
    if (rec->state == VSVC_STATE_QUIESCING && rec->refcount == 0u) {
        rec->state = VSVC_STATE_OFFLINE;
        evbus_publish(NP_EVENT_SVC_OFFLINE, rec->uuid.id[0], 0u, 0, 0u);
    }
    return 0;
}

int vsvc_quiesce(uint32_t slot) {
    if (slot >= VSVC_MAX_SERVICES) return -1;
    vsvc_record_t *rec = &vsvc_registry[slot];
    if (!rec->present || rec->state != VSVC_STATE_ACTIVE) return -1;

    rec->state = VSVC_STATE_QUIESCING;
    evbus_publish(NP_EVENT_SVC_QUIESCING, rec->uuid.id[0], 0u, 0, 0u);

    if (rec->refcount == 0u) {
        rec->state = VSVC_STATE_OFFLINE;
        evbus_publish(NP_EVENT_SVC_OFFLINE, rec->uuid.id[0], 0u, 0, 0u);
    }
    return 0;
}

uint32_t vsvc_state(uint32_t slot) {
    if (slot >= VSVC_MAX_SERVICES || !vsvc_registry[slot].present) return VSVC_STATE_FREE;
    return vsvc_registry[slot].state;
}

uint32_t vsvc_refcount(uint32_t slot) {
    if (slot >= VSVC_MAX_SERVICES || !vsvc_registry[slot].present) return 0u;
    return vsvc_registry[slot].refcount;
}

/* Gemeinsamer Minimal-Tabellen-Header (24 Byte) fuer die vier Bootstrap-
   Dienste - Inhalt ist fuer die Bootstrap-Phase irrelevant, nur die
   Groesse muss >= VSVC_TBL_HDR_SIZE sein (siehe vsvc_register). */
static const uint8_t vsvc_bootstrap_tbl[VSVC_TBL_HDR_SIZE] = {0};

void vsvc_manager_initialize(void) {
    for (uint32_t i = 0; i < VSVC_MAX_SERVICES; ++i) vsvc_registry[i] = (vsvc_record_t){0};
    vsvc_stat_registered = 0;
    vsvc_stat_acquired = 0;

    evbus_register_schema(NP_EVENT_SVC_REGISTERED, NP_EVENT_CLASS_LIFECYCLE, 0u,
                           NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY);
    evbus_register_schema(NP_EVENT_SVC_ACTIVATED, NP_EVENT_CLASS_LIFECYCLE, 0u,
                           NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY);
    evbus_register_schema(NP_EVENT_SVC_OFFLINE, NP_EVENT_CLASS_LIFECYCLE, 0u,
                           NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY);
    evbus_register_schema(NP_EVENT_SVC_QUIESCING, NP_EVENT_CLASS_LIFECYCLE, 0u,
                           NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY);
    evbus_register_schema(NP_EVENT_SVC_UNREGISTERED, NP_EVENT_CLASS_LIFECYCLE, 0u,
                           NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY);

    uint32_t slot;
    vsvc_uuid_t kern_uuid = {{0x4E4F5641u, 0x4B45524Eu, 0x434F5245u, 0x00000001u}}; /* "NOVA"/"KERN"/"CORE" */
    vsvc_register(&kern_uuid, 1u, 0u, 0u, VSVC_STAB_STABLE, vsvc_bootstrap_tbl,
                  VSVC_TBL_HDR_SIZE, 0u, 0u, VSVC_ARCH_X86_32,
                  VSVC_CALL_EARLY_BOOT | VSVC_CALL_PANIC_SAFE, "nova.kernel.core", &slot);

    vsvc_uuid_t evbs_uuid = {{0x4E4F5641u, 0x45564253u, 0x434F5245u, 0x00000001u}}; /* "EVBS" */
    vsvc_register(&evbs_uuid, 1u, 0u, 0u, VSVC_STAB_STABLE, vsvc_bootstrap_tbl,
                  VSVC_TBL_HDR_SIZE, 0u, 0u, VSVC_ARCH_X86_32,
                  VSVC_CALL_EARLY_BOOT | VSVC_CALL_PANIC_SAFE, "nova.evbus.core", &slot);

    vsvc_uuid_t caps_uuid = {{0x4E4F5641u, 0x43415053u, 0x434F5245u, 0x00000001u}}; /* "CAPS" */
    vsvc_register(&caps_uuid, 1u, 0u, 0u, VSVC_STAB_STABLE, vsvc_bootstrap_tbl,
                  VSVC_TBL_HDR_SIZE, 0u, 0u, VSVC_ARCH_X86_32,
                  VSVC_CALL_EARLY_BOOT | VSVC_CALL_PANIC_SAFE, "nova.capability.core", &slot);

    vsvc_uuid_t diag_uuid = {{0x4E4F5641u, 0x44494147u, 0x434F5245u, 0x00000001u}}; /* "DIAG" */
    vsvc_register(&diag_uuid, 1u, 0u, 0u, VSVC_STAB_STABLE, vsvc_bootstrap_tbl,
                  VSVC_TBL_HDR_SIZE, 0u, 0u, VSVC_ARCH_X86_32,
                  VSVC_CALL_EARLY_BOOT | VSVC_CALL_PANIC_SAFE, "nova.diagnostics.core", &slot);
}

int vsvc_self_test(void) {
    vsvc_uuid_t test_uuid = {{0x54455354u, 0x53564300u, 0x00000000u, 0x00000099u}}; /* "TEST"/"SVC\0" */
    static const uint8_t test_tbl[VSVC_TBL_HDR_SIZE] = {0};

    uint32_t slot;
    if (vsvc_register(&test_uuid, 1u, 3u, 0u, VSVC_STAB_EXPERIMENTAL, test_tbl,
                       VSVC_TBL_HDR_SIZE, 0x0000000Fu, 0u, VSVC_ARCH_X86_32, 0u,
                       "nova.test.service", &slot) != 0) {
        return -1;
    }

    if (vsvc_register(&test_uuid, 1u, 3u, 0u, VSVC_STAB_EXPERIMENTAL, test_tbl,
                       VSVC_TBL_HDR_SIZE, 0x0000000Fu, 0u, VSVC_ARCH_X86_32, 0u,
                       "nova.test.service", &slot) == 0) {
        return -1; /* Duplikat */
    }

    uint32_t found_slot;
    if (vsvc_find(&test_uuid, &found_slot) != 0) return -1;

    uint32_t acquired_slot;
    if (vsvc_acquire(&test_uuid, 1u, 1u, 0u, 0u, &acquired_slot) != 0) return -1;
    if (vsvc_refcount(acquired_slot) != 1u) return -1;

    if (vsvc_acquire(&test_uuid, 2u, 3u, 0u, 0u, &acquired_slot) == 0) return -1; /* falsche Major-Version */
    if (vsvc_acquire(&test_uuid, 1u, 1u, 0u, 0x000000FFu, &acquired_slot) == 0) return -1; /* fehlendes Feature */

    if (vsvc_release(found_slot) != 0) return -1;
    if (vsvc_refcount(found_slot) != 0u) return -1;

    if (vsvc_quiesce(found_slot) != 0) return -1;
    if (vsvc_state(found_slot) != VSVC_STATE_OFFLINE) return -1;

    if (vsvc_acquire(&test_uuid, 1u, 1u, 0u, 0u, &acquired_slot) == 0) return -1; /* OFFLINE */

    vsvc_uuid_t kern_uuid = {{0x4E4F5641u, 0x4B45524Eu, 0x434F5245u, 0x00000001u}};
    uint32_t kern_slot;
    if (vsvc_find(&kern_uuid, &kern_slot) != 0) return -1;
    if (vsvc_state(kern_slot) != VSVC_STATE_ACTIVE) return -1;

    return 0;
}
