/* NovaOS C-Kernel - Nova State Desired Store (Port aus adaptive32.inc,
   NPSPEC-STATE-DESIRED-0001). Speichert den beabsichtigten/Ziel-Wert je
   verfolgtem State-Objekt (spiegelt sactual.h's API mit "desired"-
   Semantik). Speist konzeptionell srec.h's Reconciliation - Verdrahtung
   selbst NICHT portiert. */
#ifndef NOVA_SDESIRED_H
#define NOVA_SDESIRED_H

#include <stdint.h>

#define SDESIRED_CAPACITY 8u

void sdesired_initialize(void);
int sdesired_self_test(void); /* 0 = ok */

/* 0 = Erfolg (neu angelegt oder vorhandenen Eintrag aktualisiert,
   Writes-Zaehler erhoeht), -1 wenn obj_id==0 oder Tabelle voll. */
int sdesired_write(uint32_t obj_id, uint32_t desired_value, uint32_t priority);

/* 0 = Erfolg (out_desired_value/out_priority gesetzt), -1 wenn obj_id
   unbekannt. */
int sdesired_read(uint32_t obj_id, uint32_t *out_desired_value, uint32_t *out_priority);

#endif
