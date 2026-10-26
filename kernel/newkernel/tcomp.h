/* NovaOS C-Kernel - Nova Transaction Compensation (Port aus
   adaptive32.inc, NPSPEC-TRANSACTION-COMPENSATION-0001). Saga-Style
   Compensation-Registry: jede Vorwaerts-Aktion hat eine registrierte
   Kompensations-Funktion (Undo). Bei Abort ruft tcomp_run_all alle
   registrierten Kompensatoren in LIFO-Reihenfolge auf. */
#ifndef NOVA_TCOMP_H
#define NOVA_TCOMP_H

#include <stdint.h>

#define TCOMP_CAPACITY 8u

typedef void (*tcomp_fn_t)(uint32_t arg);

void tcomp_initialize(void);
int tcomp_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn txn_id==0, comp_fn==0 oder
   Tabelle voll. */
int tcomp_register(uint32_t txn_id, tcomp_fn_t comp_fn, uint32_t arg, uint32_t *out_slot);
/* Ruft alle noch nicht erledigten Kompensatoren fuer txn_id in LIFO-
   Reihenfolge auf. 0 = Erfolg. */
int tcomp_run_all(uint32_t txn_id);

#endif
