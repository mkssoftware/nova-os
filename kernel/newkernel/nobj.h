/* NovaOS C-Kernel - Nova Object-Subsystem (Port aus object32.inc,
   NPSPEC-OBJECT-*). Eigenstaendig von unserem Kernel Object Manager
   (object.c/object.h, aus memory32.inc) - andere Funktionalitaet
   (Daten-/Stream-/Reactive-/Reference-Objekte mit Pipeline-Verkettung). */
#ifndef NOVA_NOBJ_H
#define NOVA_NOBJ_H

#include <stdint.h>

#define OBJ_TYPE_DATA      0u
#define OBJ_TYPE_STREAM    1u
#define OBJ_TYPE_REACTIVE  2u
#define OBJ_TYPE_REFERENCE 3u

#define OBJ_REL_NONE     0u
#define OBJ_REL_PARENT   1u
#define OBJ_REL_CHILD    2u
#define OBJ_REL_ALIAS    3u
#define OBJ_REL_PIPELINE 4u

#define OBJ_FLAG_IMMUTABLE 0x01u
#define OBJ_FLAG_ZEROCOPY  0x02u
#define OBJ_FLAG_REACTIVE  0x04u
#define OBJ_FLAG_VERSIONED 0x08u

#define OBJ_CAPACITY 16u

void object_initialize(void);
int object_self_test(void); /* 0 = ok */

/* 0 = Erfolg, *out_obj_id erhaelt die neue ID; -1 wenn Tabelle voll.
   parent_id = 0xFFFFFFFF bedeutet kein Elternobjekt. */
int obj_create(uint32_t type, uint32_t flags, uint32_t parent_id, uint32_t *out_obj_id);

/* 0 = Erfolg, -1 wenn obj_id unbekannt. */
int obj_addref(uint32_t obj_id);

/* 0 = Erfolg, -1 wenn obj_id unbekannt oder Refcount bereits 0. */
int obj_release(uint32_t obj_id);

/* 0 = Erfolg, -1 wenn obj_id unbekannt. */
int obj_version_bump(uint32_t obj_id);

#endif
