/* NovaOS C-Kernel - Nova Resource Energy Manager (Port aus adaptive32.inc,
   NPSPEC-RESOURCE-ENERGY-0001). Power-rail-Budget-Durchsetzung: trackt
   mW pro Rail via EWMA. Bei Ueberschreitung des Caps wird die zugehoerige
   Power-Domain per `apower_update_load` auf niedrige Last gezwungen (echte
   Cross-Modul-Verdrahtung mit apower.c, 1:1 wie im alten ASM). */
#ifndef NOVA_RENERGY_H
#define NOVA_RENERGY_H

#include <stdint.h>

#define RENRG_CAPACITY 4u

void renergy_initialize(void);
int renergy_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn rail_id==0 oder Tabelle voll. */
int renergy_register(uint32_t rail_id, uint32_t cap_mw);

/* 0 = Erfolg (unter Cap), -1 = Cap ueberschritten (Violation gezaehlt,
   apower_update_load aufgerufen) oder Rail unbekannt. */
int renergy_update(uint32_t rail_id, uint32_t sample_mw);

/* 0 = Erfolg, -1 wenn Rail unbekannt. */
int renergy_query(uint32_t rail_id, uint32_t *out_power_mw, uint32_t *out_cap_mw, uint32_t *out_violations);

#endif
