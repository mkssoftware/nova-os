/* NovaOS C-Kernel - Nova State Actual Store (Port aus adaptive32.inc,
   NPSPEC-STATE-ACTUAL-0001). Speichert den beobachteten/gemessenen
   aktuellen Wert je verfolgtem State-Objekt. Speist konzeptionell
   srec.h's Reconciliation - Verdrahtung selbst NICHT portiert. */
#ifndef NOVA_SACTUAL_H
#define NOVA_SACTUAL_H

#include <stdint.h>

#define SACTUAL_CAPACITY 8u

void sactual_initialize(void);
int sactual_self_test(void); /* 0 = ok */

/* 0 = Erfolg (neu angelegt oder vorhandenen Eintrag aktualisiert,
   Writes-Zaehler erhoeht), -1 wenn obj_id==0 oder Tabelle voll. */
int sactual_write(uint32_t obj_id, uint32_t actual_value);

/* 0 = Erfolg (out_actual_value/out_tick gesetzt), -1 wenn obj_id
   unbekannt. */
int sactual_read(uint32_t obj_id, uint32_t *out_actual_value, uint32_t *out_tick);

#endif
