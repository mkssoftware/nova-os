/* NovaOS C-Kernel - §103 Capability Framework 1.0 (Port aus
   security32.inc, NPSPEC-KERNEL-0103). Fail-closed Capability-Pruefung
   fuer Objektzugriffe. NICHT zu verwechseln mit program.c's einfacherer
   Capability-Registry (program32.inc) oder verify.c's cap_query-Stub
   (wartet auf cap_integration32.inc/security32.inc §104+ - drei
   unterschiedliche Capability-Konzepte, die schon im alten ASM
   nebeneinander existierten). */
#ifndef NOVA_CAP_H
#define NOVA_CAP_H

#include <stdint.h>

#define NP_CAP_QUERY     0x001u
#define NP_CAP_MODIFY    0x002u
#define NP_CAP_WAIT      0x004u
#define NP_CAP_SIGNAL    0x008u
#define NP_CAP_DUPLICATE 0x010u
#define NP_CAP_TRANSFER  0x020u
#define NP_CAP_SUBSCRIBE 0x040u
#define NP_CAP_DELETE    0x080u
#define NP_CAP_ADMIN     0x100u

#define CAP_STATE_FREE    0u
#define CAP_STATE_VALID   1u
#define CAP_STATE_REVOKED 2u
#define CAP_STATE_EXPIRED 3u

#define CAP_DOMAIN_KERNEL 1u
#define CAP_DOMAIN_BOOT   2u

#define CAP_MAX_CAPS 32u

#define NP_ERR_CAP_INVALID  (-60)
#define NP_ERR_CAP_REVOKED  (-61)
#define NP_ERR_CAP_EXPIRED  (-62)
#define NP_ERR_CAP_EXHAUSTED (-63)
#define NP_ERR_CAP_RIGHTS   (-64)
#define NP_ERR_CAP_DELEGATE (-65)
#define NP_ERR_CAP_DOMAIN   (-66)
#define NP_ERR_CAP_POLICY   (-67)
#define NP_ERR_CAP_STALE    (-68)
#define NP_ERR_CAP_LIMIT    (-69)

/* Leert den Pool, registriert evbus-Schemata und zwei Bootstrap-Caps
   (Kernel-Root, Boot-Kontext). 0 = Erfolg. */
int cap_initialize(void);
int cap_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_cap_id gesetzt), sonst NP_ERR_CAP_* (negativ).
   delegable_rights MUSS Teilmenge von rights sein. */
int cap_create(uint32_t obj_id, uint32_t rights, uint32_t delegable_rights, uint32_t domain,
               uint32_t *out_cap_id);

/* 0 = Erlaubt, NP_ERR_CAP_* (negativ) bei Ablehnung (fail-closed). */
int cap_check(uint32_t cap_id, uint32_t required_rights);

/* 0 = Erfolg (*out_cap_id gesetzt), sonst NP_ERR_CAP_* (negativ). Rechte
   duerfen nur abgeschwaecht werden (Teilmenge von src). */
int cap_derive(uint32_t src_cap_id, uint32_t new_rights, uint32_t new_delegable, uint32_t *out_cap_id);

int cap_revoke(uint32_t cap_id); /* 0 = Erfolg, NP_ERR_CAP_* bei Fehler */

#endif
