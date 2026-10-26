/* NovaOS C-Kernel - Nova Security Audit Log (Port aus security32.inc,
   NPSPEC-SECURITY-AUDIT-0001). Ringpuffer mit 32 Eintraegen. */
#ifndef NOVA_AUDIT_H
#define NOVA_AUDIT_H

#include <stdint.h>

#define AUDIT_CAPACITY 32u

#define AUDIT_TYPE_NONE       0u
#define AUDIT_TYPE_CAP_GRANT  1u
#define AUDIT_TYPE_CAP_REVOKE 2u
#define AUDIT_TYPE_POLICY     3u
#define AUDIT_TYPE_VIOLATION  4u
#define AUDIT_TYPE_LOGIN      5u
#define AUDIT_TYPE_LOGOUT     6u
#define AUDIT_TYPE_SYSCALL    7u

void security_audit_initialize(void);
int security_audit_self_test(void); /* 0 = ok */

/* Schreibt einen Ringpuffer-Eintrag; ueberschreibt den aeltesten bei vollem
   Puffer. result: 0=deny, 1=allow. Immer 0 (Erfolg). */
int security_audit_log(uint32_t type, uint32_t subject, uint32_t object, uint32_t result);

uint32_t security_audit_count(uint32_t type); /* Anzahl Eintraege dieses Typs */

#endif
