/* NovaOS C-Kernel - Nova Policy Learning Engine (Port aus adaptive32.inc,
   NPSPEC-ADAPTIVE-POLICYLEARNING-0001). Verfolgt Policy-Regel-Ergebnisse
   und passt Regel-Gewichte per EWMA an: 128=neutral, >128=bevorzugen,
   <128=vermeiden. Verbindet sich konzeptionell mit `autopol.c`
   (nach jedem Policy-Feuern soll `pollearn_feedback` das gelernte
   Gewicht anpassen) - Verdrahtung selbst NICHT portiert. */
#ifndef NOVA_POLLEARN_H
#define NOVA_POLLEARN_H

#include <stdint.h>

#define POLLEARN_CAPACITY 8u
#define POLLEARN_ALPHA    24u /* Lernrate (von 256) */

void pollearn_initialize(void);
int pollearn_self_test(void); /* 0 = ok */

/* 0 = Erfolg (Startgewicht=128), -1 wenn rule_id==0 oder Tabelle voll. */
int pollearn_register(uint32_t rule_id, uint32_t trigger);

/* success: 1 = Erfolg (Gewicht per EWMA Richtung 255), 0 = Fehlschlag
   (Richtung 0). 0 = Erfolg (Fires-Zaehler erhoeht), -1 wenn rule_id
   unbekannt. */
int pollearn_feedback(uint32_t rule_id, uint32_t success);

/* 0 = Erfolg (out_weight/out_fires gesetzt), -1 wenn rule_id unbekannt. */
int pollearn_query(uint32_t rule_id, uint32_t *out_weight, uint32_t *out_fires);

#endif
