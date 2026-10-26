/* NovaOS C-Kernel - Nova Identity Registry (Port aus adaptive32.inc,
   NPSPEC-IDENTITY-0001). Stabile Identitaeten fuer Kernel/Prozesse/
   Dienste/Geraete. Identity != Authority: ein Eintrag hier verleiht keine
   Rechte. NICHT zu verwechseln mit identity.c (identity32.inc:
   identity_mgr_* mit Credentials/MFA) - ein eigenstaendiges, schon im alten
   ASM paralleles Subsystem. */
#ifndef NOVA_IDENTREG_H
#define NOVA_IDENTREG_H

#include <stdint.h>

#define IDENT_CAPACITY 16u

#define IDENT_TYPE_KERNEL  1u
#define IDENT_TYPE_PROCESS 2u
#define IDENT_TYPE_SERVICE 3u
#define IDENT_TYPE_DEVICE  4u

#define IDENT_STATE_ACTIVE  1u
#define IDENT_STATE_SUSP    2u
#define IDENT_STATE_REVOKED 3u

/* Registriert die Kernel-Identitaet (ID 1) automatisch. */
void identity_initialize(void);
int identity_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_id gesetzt), -1 wenn Tabelle voll. */
int identity_register(uint32_t type, uint32_t owner, uint32_t *out_id);

/* 0 = Erfolg (Typ und Zustand gesetzt), -1 wenn nicht gefunden. */
int identity_query(uint32_t identity_id, uint32_t *out_type, uint32_t *out_state);

/* 0 = Erfolg, -1 wenn new_state > IDENT_STATE_REVOKED oder nicht gefunden. */
int identity_set_state(uint32_t identity_id, uint32_t new_state);

#endif
