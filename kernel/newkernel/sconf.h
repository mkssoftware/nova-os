/* NovaOS C-Kernel - Nova Self-Configuration (Port aus adaptive32.inc,
   NPSPEC-AUTONOMY-SELFCONFIG-0001). Laufzeit-Key/Value-Konfigurations-
   speicher: erlaubt Subsystemen, abstimmbare Parameter zur Laufzeit ohne
   Neubau zu lesen/schreiben. */
#ifndef NOVA_SCONF_H
#define NOVA_SCONF_H

#include <stdint.h>

#define SCONF_CAPACITY 8u

void sconf_initialize(void);
int sconf_self_test(void); /* 0 = ok */

/* 0 = Erfolg (neuer Eintrag mit version=1, oder vorhandener Key
   aktualisiert mit erhoehter version), -1 wenn key==0 oder Tabelle voll. */
int sconf_set(uint32_t key, uint32_t value);

/* 0 = Erfolg (out_value/out_version gesetzt), -1 wenn key unbekannt. */
int sconf_get(uint32_t key, uint32_t *out_value, uint32_t *out_version);

#endif
