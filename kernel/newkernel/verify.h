/* NovaOS C-Kernel - Verification-Subsystem (Port aus verify32.inc, NPSPEC-VERIFY-*). */
#ifndef NOVA_VERIFY_H
#define NOVA_VERIFY_H

#include <stdint.h>

#define VFY_PASS    0u
#define VFY_WARN    1u
#define VFY_FAIL    2u
#define VFY_UNKNOWN 3u

#define VFY_TYPE_CAPABILITY   0u
#define VFY_TYPE_CONTRACT     1u
#define VFY_TYPE_MEMORY       2u
#define VFY_TYPE_INFOFLOW     3u
#define VFY_TYPE_RUNTIME      4u
#define VFY_TYPE_TEMPORAL     5u
#define VFY_TYPE_TYPE         6u
#define VFY_TYPE_REPRODUCIBLE 7u
#define VFY_TYPE_COUNT        8u

#define VFY_CAPACITY      16u
#define VFY_PRED_CAPACITY 8u

void verify_initialize(void);
int verify_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_result enthaelt VFY_*), -1 wenn type >= VFY_TYPE_COUNT.
   Ist die Ergebnistabelle voll, wird *out_result = VFY_UNKNOWN ohne Fehler. */
int verify_check(uint32_t type, uint32_t subject_id, uint32_t *out_result);

/* 0 = Erfolg, -1 wenn die Praedikat-Tabelle voll ist. */
int verify_contract_register(uint32_t subject_id, uint32_t precond_hash, uint32_t postcond_hash);

#endif
