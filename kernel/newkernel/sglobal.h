/* NovaOS C-Kernel - Nova State Global Store (Port aus adaptive32.inc,
   NPSPEC-STATE-GLOBAL-0001). Kernel-weiter benannter Zustandsspeicher mit
   16 Slots. Jedes Subsystem kann Werte unter einem Key veroeffentlichen
   oder abfragen. `sglobal_publish` bumpt die Version und zeichnet den
   Uebergang via `shist_record` auf (echte Cross-Modul-Verdrahtung mit
   shist.c, 1:1 wie im alten ASM). */
#ifndef NOVA_SGLOBAL_H
#define NOVA_SGLOBAL_H

#include <stdint.h>

#define SGLOBAL_CAPACITY 16u

void sglobal_initialize(void);
int sglobal_self_test(void); /* 0 = ok */

/* 0 = Erfolg (neuer Slot oder Update, Version erhoeht, shist_record
   aufgerufen), -1 wenn key==0 oder Tabelle voll. */
int sglobal_publish(uint32_t key, uint32_t value, uint32_t owner);

/* 0 = Erfolg (out_value/out_version/out_owner gesetzt), -1 wenn Key
   unbekannt. */
int sglobal_get(uint32_t key, uint32_t *out_value, uint32_t *out_version, uint32_t *out_owner);

#endif
