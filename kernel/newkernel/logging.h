/* NovaOS C-Kernel - strukturiertes Early-/Kernel-Logging
   (Port aus memory32.inc, NPSPEC-KERNEL-0023). */
#ifndef NOVA_LOGGING_H
#define NOVA_LOGGING_H

#include <stdint.h>

#define LOG_LEVEL_CRITICAL 6u

void logging_initialize(void);
int logging_self_test(void); /* 0 = ok */

/* 0 = Erfolg. level<=7, category<=13, sonst Fehler (wie im alten ASM). */
int logging_write(uint32_t level, uint32_t category, uint32_t component, uint32_t event);

#endif
