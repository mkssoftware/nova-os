/* NovaOS C-Kernel - Kernel Object Manager (Port aus memory32.inc,
   ADR-2008 / NPSPEC-KERNEL-0012). Handle = (Generation<<16) | (Slot+1). */
#ifndef NOVA_OBJECT_H
#define NOVA_OBJECT_H

#include <stdint.h>

#define OBJECT_TABLE_CAPACITY 64u
#define OBJECT_STATE_LIVE 1u

void object_manager_initialize(void);
int object_manager_self_test(void); /* 0 = ok */

/* type muss != 0 sein. Gibt Handle zurueck, oder 0 bei Fehler (voll/ungueltig). */
uint32_t object_create(uint32_t type, uint32_t payload0, uint32_t payload1);

/* Gibt den Objekt-Header zurueck, oder NULL wenn Handle ungueltig/veraltet ist. */
void *object_lookup(uint32_t handle);

int object_retain(uint32_t handle);  /* 0 = ok */
int object_release(uint32_t handle); /* 0 = ok */

uint32_t object_live_count(void);

#endif
