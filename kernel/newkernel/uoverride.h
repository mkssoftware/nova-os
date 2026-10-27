/* NovaOS C-Kernel - Nova User Override Manager (Port aus adaptive32.inc,
   NPSPEC-ADAPTIVE-USEROVERRIDE-0001). Erlaubt Userspace (oder
   privilegiertem Kernel-Code), adaptive Policy-Entscheidungen mit
   persistenten Key-Value-Einstellungen zu ueberschreiben; Overrides
   haben Vorrang vor allen EWMA-getriebenen adaptiven Entscheidungen.
   `uoverride_set` persistiert echte ueber `sconf_set` (echte Cross-Modul-
   Verdrahtung mit sconf.c, 1:1 wie im alten ASM). */
#ifndef NOVA_UOVERRIDE_H
#define NOVA_UOVERRIDE_H

#include <stdint.h>

#define UOVER_CAPACITY 8u

void uoverride_initialize(void);
int uoverride_self_test(void); /* 0 = ok */

/* 0 = Erfolg (neu angelegt oder vorhandenen Override aktualisiert,
   Version erhoeht, aktiv markiert, via sconf_set persistiert), -1 wenn
   key==0 oder Tabelle voll. */
int uoverride_set(uint32_t key, uint32_t value);

/* 0 = Erfolg (out_value/out_version gesetzt), -1 wenn key unbekannt oder
   nicht aktiv. */
int uoverride_get(uint32_t key, uint32_t *out_value, uint32_t *out_version);

/* 0 = Erfolg (deaktiviert, adaptive Entscheidung nimmt wieder Vorrang),
   -1 wenn key unbekannt. */
int uoverride_clear(uint32_t key);

#endif
