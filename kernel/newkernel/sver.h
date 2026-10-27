/* NovaOS C-Kernel - Nova State Versioning (Port aus adaptive32.inc,
   NPSPEC-STATE-VERSIONING-0001). Verfolgt monoton steigende
   Versionsnummern und Generation-Lineage je State-Objekt; jede Erhoehung
   zeichnet die Eltern-Version fuer Lineage-Tracing auf. */
#ifndef NOVA_SVER_H
#define NOVA_SVER_H

#include <stdint.h>

#define SVER_CAPACITY 8u

void sver_initialize(void);
int sver_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt, version=1, parent=0), -1 wenn obj_id==0
   oder Tabelle voll. */
int sver_create(uint32_t obj_id, uint32_t *out_slot);

/* 0 = Erfolg (*out_new_version gesetzt, parent=alte Version,
   generation-Zaehler global erhoeht), -1 wenn obj_id unbekannt. */
int sver_bump(uint32_t obj_id, uint32_t *out_new_version);

/* 0 = Erfolg (out_version/out_parent_version/out_generation gesetzt),
   -1 wenn obj_id unbekannt. */
int sver_query(uint32_t obj_id, uint32_t *out_version, uint32_t *out_parent_version,
               uint32_t *out_generation);

#endif
