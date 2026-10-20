/* NovaOS C-Kernel - Trust-Subsystem (Port aus trust32.inc, NPSPEC-TRUST-*). */
#ifndef NOVA_TRUST_H
#define NOVA_TRUST_H

#include <stdint.h>

#define TRUST_LEVEL_NONE   0u
#define TRUST_LEVEL_LOW    1u
#define TRUST_LEVEL_MEDIUM 2u
#define TRUST_LEVEL_HIGH   3u
#define TRUST_LEVEL_ANCHOR 4u

#define TRUST_SRC_UNKNOWN      0u
#define TRUST_SRC_KERNEL       1u
#define TRUST_SRC_SIGNATURE    2u
#define TRUST_SRC_ATTESTATION  3u
#define TRUST_SRC_SUPPLY_CHAIN 4u
#define TRUST_SRC_MEDIA        5u

#define TRUST_FLAG_REVOCABLE 0x01u
#define TRUST_FLAG_AUDITED   0x02u
#define TRUST_FLAG_AGENT     0x04u
#define TRUST_FLAG_SOFTWARE  0x08u
#define TRUST_FLAG_REVOKED   0x80u

#define TRUST_CAPACITY       16u
#define TRUST_AUDIT_CAPACITY 16u

/* Registriert zusaetzlich den Kernel-Trust-Anker (Subjekt 0, TRUST_LEVEL_ANCHOR). */
int trust_mgr_initialize(void);
int trust_mgr_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int trust_grant(uint32_t subject_id, uint32_t level, uint32_t source, uint32_t flags);

/* 0 = ok (*out_actual_level erhaelt den Level), -1 wenn widerrufen, Level zu
   niedrig oder subject_id unbekannt. */
int trust_check(uint32_t subject_id, uint32_t min_level, uint32_t *out_actual_level);

/* 0 = Erfolg, -1 wenn subject_id unbekannt. */
int trust_revoke(uint32_t subject_id);

#endif
