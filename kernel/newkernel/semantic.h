/* NovaOS C-Kernel - Semantic-Layer (Port aus semantic32.inc, NPSPEC-SEMANTIC-*). */
#ifndef NOVA_SEMANTIC_H
#define NOVA_SEMANTIC_H

#include <stdint.h>

#define STYPE_UNKNOWN       0u
#define STYPE_INTEGER       1u
#define STYPE_STRING        2u
#define STYPE_BLOB          3u
#define STYPE_CAPABILITY    4u
#define STYPE_RESOURCE      5u
#define STYPE_FILE          6u
#define STYPE_IPC_ENDPOINT  7u
#define STYPE_EXECUTION_CTX 8u
#define STYPE_COUNT         9u

#define SREL_NONE       0u
#define SREL_DEPENDS_ON 1u
#define SREL_PROVIDES   2u
#define SREL_CONSUMES   3u
#define SREL_COMPATIBLE 4u

#define SVAL_OK            0u
#define SVAL_TYPE_MISMATCH 1u
#define SVAL_RANGE_ERROR   2u
#define SVAL_COMPAT_ERROR  3u

#define SEM_CAPACITY 16u

typedef uint32_t (*semmgr_validate_fn_t)(uint32_t value);

void semmgr_initialize(void);
int semmgr_self_test(void); /* 0 = ok (identisch zum alten ASM: prueft nur semmgr_ready) */

/* 0 = Erfolg, *out_sem_id erhaelt die neue Semantik-ID; -1 wenn voll oder
   type >= STYPE_COUNT. */
int semmgr_register(uint32_t name_hash, uint32_t type, uint32_t flags, uint32_t *out_sem_id);

/* Gibt den internen Record zurueck, oder NULL wenn name_hash unbekannt. */
void *semmgr_query(uint32_t name_hash);

/* *out_result erhaelt SVAL_*. 0 = sem_id gefunden, -1 = unbekannt (dann
   SVAL_TYPE_MISMATCH). */
int semmgr_validate(uint32_t sem_id, uint32_t value, uint32_t *out_result);

#endif
