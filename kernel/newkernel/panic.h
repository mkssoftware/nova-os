/* NovaOS C-Kernel - Panic-Reporter und Minimal-Crash-Dump
   (Port aus integration32.inc, ADR-2014). */
#ifndef NOVA_PANIC_H
#define NOVA_PANIC_H

#include <stdint.h>

void panic_manager_initialize(void);
int panic_manager_self_test(void); /* 0 = ok */

void crash_dump_initialize(void);
int crash_dump_self_test(void); /* 0 = ok */

/* error_code/subsystem entsprechen EAX/EDX im alten ASM-Aufruf.
   EIP wird per Rueckadresse des Aufrufers angenaehert. */
void kernel_panic(uint32_t error_code, uint32_t subsystem, const char *message);

void kernel_halt(void);

#endif
