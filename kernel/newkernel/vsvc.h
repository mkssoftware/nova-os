/* NovaOS C-Kernel - §105 Versioned Kernel Service ABI (Port aus
   cap_integration32.inc, NPSPEC-KERNEL-0105). Registry fuer versionierte
   Kernel-Dienste mit Capability-Anforderungen, Feature-Maske und
   Lifecycle-Zustand (ACTIVE->QUIESCING->OFFLINE). Referenziert per
   16-Byte-UUID statt per Name/ID, damit mehrere unabhaengige Anbieter
   nicht kollidieren koennen. */
#ifndef NOVA_VSVC_H
#define NOVA_VSVC_H

#include <stdint.h>

#define VSVC_MAX_SERVICES 16u

#define VSVC_STATE_FREE         0u
#define VSVC_STATE_ACTIVE       1u
#define VSVC_STATE_QUIESCING    2u
#define VSVC_STATE_OFFLINE      3u
#define VSVC_STATE_UNREGISTERED 4u

#define VSVC_STAB_INTERNAL      0u
#define VSVC_STAB_EXPERIMENTAL  1u
#define VSVC_STAB_PROVISIONAL   2u
#define VSVC_STAB_STABLE        3u
#define VSVC_STAB_LEGACY        4u
#define VSVC_STAB_SECURITY_ONLY 5u

#define VSVC_ARCH_X86_32  0x00000001u
#define VSVC_ARCH_X86_64  0x00000002u
#define VSVC_ARCH_ARM64   0x00000004u
#define VSVC_ARCH_RISCV64 0x00000008u
#define VSVC_ARCH_ANY     0xFFFFFFFFu

#define VSVC_CALL_THREAD_CTX 0x00000001u
#define VSVC_CALL_IRQ_SAFE   0x00000002u
#define VSVC_CALL_EARLY_BOOT 0x00000008u
#define VSVC_CALL_PANIC_SAFE 0x00000010u
#define VSVC_CALL_MAY_BLOCK  0x00000020u

/* 16-Byte-UUID, 4 x uint32_t (wie im alten ASM: id[0..3]). */
typedef struct {
    uint32_t id[4];
} vsvc_uuid_t;

void vsvc_manager_initialize(void);
int vsvc_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt, Zustand -> ACTIVE), -1 bei Duplikat,
   voller Tabelle, oder table_size < 24 Byte (minimaler Tabellen-Header). */
int vsvc_register(const vsvc_uuid_t *uuid, uint32_t major, uint32_t minor, uint32_t patch,
                   uint32_t stability, const void *table, uint32_t table_size,
                   uint32_t features, uint32_t required_caps, uint32_t arch_mask,
                   uint32_t flags, const char *name, uint32_t *out_slot);

int vsvc_find(const vsvc_uuid_t *uuid, uint32_t *out_slot); /* 0 = gefunden */

/* 0 = Erfolg (Referenzzaehler erhoeht), -1 wenn nicht gefunden, nicht
   ACTIVE, Major-Version ausserhalb [min_major,max_major], Minor-Version
   < min_minor, nicht alle required_features vorhanden, oder
   Architektur-Maske inkompatibel. */
int vsvc_acquire(const vsvc_uuid_t *uuid, uint32_t min_major, uint32_t max_major,
                  uint32_t min_minor, uint32_t required_features, uint32_t *out_slot);

/* 0 = Erfolg (Referenzzaehler verringert; faellt er auf 0 waehrend
   QUIESCING, geht der Dienst sofort auf OFFLINE), -1 wenn slot unbekannt
   oder Referenzzaehler bereits 0. */
int vsvc_release(uint32_t slot);

/* 0 = Erfolg (Zustand -> QUIESCING, oder sofort OFFLINE bei Refcount=0),
   -1 wenn slot unbekannt oder nicht ACTIVE. */
int vsvc_quiesce(uint32_t slot);

uint32_t vsvc_state(uint32_t slot);    /* VSVC_STATE_FREE wenn unbekannt */
uint32_t vsvc_refcount(uint32_t slot); /* 0 wenn unbekannt */

#endif
