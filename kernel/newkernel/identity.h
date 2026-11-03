/* NovaOS C-Kernel - Identity-Subsystem (Port aus identity32.inc, NPSPEC-IDENTITY-*). */
#ifndef NOVA_IDENTITY_H
#define NOVA_IDENTITY_H

#include <stdint.h>

#define IDENTMGR_TYPE_USER    0u
#define IDENTMGR_TYPE_DEVICE  1u
#define IDENTMGR_TYPE_SERVICE 2u
#define IDENT_TYPE_APP        3u
#define IDENT_TYPE_WORKLOAD   4u

#define IDENTMGR_STATE_ACTIVE  0u
#define IDENT_STATE_INACTIVE   1u
#define IDENT_STATE_LOCKED     2u
#define IDENTMGR_STATE_REVOKED 3u

#define CRED_TYPE_PASSWORD 0u
#define CRED_TYPE_PASSKEY  1u
#define CRED_TYPE_TOKEN    2u
#define CRED_TYPE_CERT     3u

#define MFA_FACTOR_NONE     0x00u
#define MFA_FACTOR_PASSWORD 0x01u
#define MFA_FACTOR_TOTP     0x02u
#define MFA_FACTOR_PASSKEY  0x04u
#define MFA_FACTOR_HARDWARE 0x08u

#define IDENTMGR_CAPACITY 8u

/* Registriert zusaetzlich die System-Geraeteidentitaet (ID 0). 0 = Erfolg. */
int identity_mgr_initialize(void);
int identity_mgr_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int ident_register(uint32_t id, uint32_t type, uint32_t cred_type, uint32_t mfa_mask);

/* 0 = authentifiziert, -1 = abgelehnt (Zustand/Hash/MFA) oder id unbekannt. */
int ident_authenticate(uint32_t id, uint32_t cred_hash, uint32_t mfa_presented);

/* 0 = Erfolg, -1 wenn id unbekannt. */
int ident_revoke(uint32_t id);

/* NPSPEC-IDENTITY-LIFECYCLE-0001 #2: expliziter Lifecycle-State - bisher
   wurden IDENT_STATE_INACTIVE/_LOCKED nie erreicht (nur ACTIVE beim
   Registrieren, REVOKED per ident_revoke). ACTIVE -> INACTIVE/LOCKED,
   0 = Erfolg, -1 wenn id unbekannt oder nicht ACTIVE. */
int ident_suspend(uint32_t id);  /* -> IDENT_STATE_INACTIVE */
int ident_lock(uint32_t id);     /* -> IDENT_STATE_LOCKED */

/* INACTIVE|LOCKED -> ACTIVE. 0 = Erfolg, -1 wenn id unbekannt oder
   REVOKED/bereits ACTIVE (Revocation ist bewusst endgueltig - siehe
   ident_revoke - und nicht ueber reactivate umkehrbar). */
int ident_reactivate(uint32_t id);

/* Aktueller Zustand (IDENT_STATE_x / IDENTMGR_STATE_x), oder
   IDENTMGR_STATE_ACTIVE faellt NICHT als Fallback zurueck - 0xFFFFFFFFu
   wenn id unbekannt (0 waere mit dem echten ACTIVE-Wert mehrdeutig). */
uint32_t ident_state(uint32_t id);

#endif
