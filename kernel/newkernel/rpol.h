/* NovaOS C-Kernel - Nova Recovery Policy Manager (Port aus adaptive32.inc,
   NPSPEC-RESILIENCE-RECOVERYPOLICY-0001). Bildet Fehlerklasse + Schweregrad
   auf eine Recovery-Aktion ab (verbindet fclass.h's Klassifizierung mit
   einer Aktionsauswahl). */
#ifndef NOVA_RPOL_H
#define NOVA_RPOL_H

#include <stdint.h>

#define RPOL_CAPACITY 8u

#define RPOL_NONE      0u
#define RPOL_LOG_ONLY  1u
#define RPOL_RESTART   2u
#define RPOL_FAILOVER  3u
#define RPOL_DEGRADE   4u
#define RPOL_PANIC_ACT 5u

void rpol_initialize(void);
int rpol_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn class_id==0 oder Tabelle voll. */
int rpol_register(uint32_t class_id, uint32_t min_severity, uint32_t action, uint32_t *out_slot);

/* 0 = Policy gefunden (observed_severity >= min_severity, *out_action
   gesetzt, fires erhoeht), -1 wenn keine passende Policy. */
int rpol_evaluate(uint32_t class_id, uint32_t observed_severity, uint32_t *out_action);

#endif
