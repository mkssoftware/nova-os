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

#endif
