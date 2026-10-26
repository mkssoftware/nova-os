/* NovaOS C-Kernel - Nova Trust Anchor Chain (Port aus adaptive32.inc,
   NPSPEC-TRUST-ANCHOR-0001). Root-of-Trust-Kette fuer Boot-Verifikation
   und Subsystem-Attestation, Anker mit 32-Bit-Fingerprint aus dem CSPRNG.
   NICHT zu verwechseln mit trust.c (trust32.inc: trust_mgr_* mit
   Grant/Check/Revoke pro Subjekt) - ein eigenstaendiges, schon im alten
   ASM paralleles Subsystem. */
#ifndef NOVA_TRUSTANCHOR_H
#define NOVA_TRUSTANCHOR_H

#include <stdint.h>

#define TRUST_ANCHOR_COUNT 8u

#define TRUST_ANCH_BOOT     1u
#define TRUST_ANCH_SECURITY 2u
#define TRUST_ANCH_CAPROOT  3u
#define TRUST_ANCH_USER     4u

#define TRUST_ANC_FLAG_VALID   0x01u
#define TRUST_ANC_FLAG_REVOKED 0x02u

/* Registriert KERNEL_BOOT- und SECURITY-Anker mit Fingerprints aus
   csprng_next() (csprng.c muss bereits initialisiert sein). 0 = Erfolg. */
int trust_initialize(void);
int trust_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn Tabelle voll. */
int trust_register_anchor(uint32_t anchor_type, uint32_t fingerprint, uint32_t *out_slot);

/* 0 = gefunden und gueltig (*out_fingerprint gesetzt), -1 wenn nicht
   gefunden oder widerrufen. */
int trust_verify_anchor(uint32_t anchor_type, uint32_t *out_fingerprint);

#endif
