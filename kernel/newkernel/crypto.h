/* NovaOS C-Kernel - Cryptographic Subsystem (Port aus crypto32.inc,
   NPSPEC-CRYPTO-ARCH-0001 und verwandte NPSPEC-CRYPTO-Spezifikationen). */
#ifndef NOVA_CRYPTO_H
#define NOVA_CRYPTO_H

#include <stdint.h>

#define CRYPTO_ALG_NONE       0u
#define CRYPTO_ALG_AES128     1u
#define CRYPTO_ALG_AES256     2u
#define CRYPTO_ALG_AESCBC128  3u
#define CRYPTO_ALG_AESGCM128  4u
#define CRYPTO_ALG_CHACHA20   5u
#define CRYPTO_ALG_SHA256     6u
#define CRYPTO_ALG_HMACSHA256 7u
#define CRYPTO_ALG_HKDF       8u
#define CRYPTO_ALG_PBKDF2     9u
#define CRYPTO_ALG_PQ_KYBER   0x10u

#define KEYTYPE_SYMMETRIC  0u
#define KEYTYPE_HMAC       1u
#define KEYTYPE_KDF_MASTER 2u
#define KEYTYPE_WRAP       3u
#define KEYTYPE_PQ         4u

#define KEYFLAG_EXPORTABLE 0x01u
#define KEYFLAG_ROTATEABLE 0x02u
#define KEYFLAG_SYSTEM     0x04u
#define KEYFLAG_EPHEMERAL  0x08u

#define KEYSTORE_CAPACITY 8u
#define RNG_STATE_SIZE    32u
#define KEY_MATERIAL_MAX  32u

void crypto_initialize(void);
int crypto_self_test(void); /* 0 = ok */

/* Fuellt buf mit pseudo-zufaelligen Bytes (Xorshift128, aus RDRAND/RDTSC-
   Seed). Immer erfolgreich. */
void crypto_random_bytes(uint8_t *buf, uint32_t len);

/* 0 = Erfolg (*out_key_id gesetzt), -1 wenn Keystore voll. key_len_bytes
   wird auf KEY_MATERIAL_MAX geclampt. */
int crypto_key_generate(uint32_t alg_id, uint32_t key_type, uint32_t key_len_bytes,
                         uint32_t flags, uint32_t *out_key_id);

/* 0 = gefunden (*out_slot erhaelt den internen Slot-Zeiger), -1 sonst. */
int crypto_key_find(uint32_t key_id, void **out_slot);

/* 0 = Erfolg (Secure-Erase des Materials + Slot), -1 wenn nicht gefunden. */
int crypto_key_delete(uint32_t key_id);

/* 0 = Erfolg (neues Material, Generation++), -1 wenn nicht gefunden oder
   KEYFLAG_ROTATEABLE nicht gesetzt. */
int crypto_key_rotate(uint32_t key_id);

/* Ueberschreibt buf byteweise mit 0 und setzt eine Speicherbarriere (mfence),
   damit der Compiler/die CPU das Loeschen nicht wegoptimiert/umordnet. */
void crypto_secure_zero(void *buf, uint32_t len);

/* TODO: echtes HKDF-SHA-256 statt Zufalls-Stub (bereits im alten ASM nur ein
   unvollstaendiger Stub ohne echte Material-Vermischung - 1:1 uebernommen).
   0 = Erfolg, -1 wenn master_key_id nicht gefunden. */
int crypto_kdf_derive(uint32_t master_key_id, const uint8_t *context, uint32_t context_len,
                       uint8_t *out, uint32_t out_len);

#endif
