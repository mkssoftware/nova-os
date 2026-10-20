/* NovaOS C-Kernel - Copy-on-Write-Tracking (Port aus memory32.inc,
   NPSPEC-MEMORY-COW-0001). */
#ifndef NOVA_COW_H
#define NOVA_COW_H

#include <stdint.h>

#define COW_CAPACITY 16

void cow_initialize(void);
int cow_self_test(void); /* 0 = ok */

/* Gibt den Slot (>=0) zurueck, oder -1 wenn alle Slots belegt sind. */
int cow_register_page(uint32_t phys_addr);

/* Simuliert einen Schreibzugriff: refcount>1 -> Kopie (COW_STATE_COPIED).
   0 = Erfolg (mit oder ohne Kopie), -1 = ungueltiger/freier Slot. */
int cow_on_write(int slot);

#endif
