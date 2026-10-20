/* NovaOS C-Kernel - Kernel Process Manager (Port aus process32.inc,
   ADR-2011 / NPSPEC-KERNEL-0004). */
#ifndef NOVA_PROCESS_H
#define NOVA_PROCESS_H

#include <stdint.h>

#define PROCESS_CAPACITY 4u
#define OBJECT_TYPE_PROCESS 4u

void process_manager_initialize(void);
int process_manager_self_test(void); /* 0 = ok */

/* Gibt die neue PID zurueck, oder 0 wenn die Tabelle voll ist. */
uint32_t process_create(uint32_t flags, uint32_t page_directory_cr3);

/* Gibt den internen Prozessdatensatz zurueck, oder NULL wenn PID unbekannt. */
void *process_lookup(uint32_t pid);

uint32_t process_count(void);

#endif
