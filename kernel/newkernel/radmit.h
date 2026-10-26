/* NovaOS C-Kernel - Nova Resource Admission Control (Port aus
   adaptive32.inc, NPSPEC-RESOURCE-ADMISSION-0001). Prueft vor
   Ausfuehrung ob CPU+Memory-Budget verfuegbar ist. Admission !=
   Authorization != Scheduling. NICHT zu verwechseln mit resource.c
   (res_request/res_release/res_reserve aus einem anderen NPSPEC) - ein
   eigenstaendiges, schon im alten ASM paralleles Subsystem. */
#ifndef NOVA_RADMIT_H
#define NOVA_RADMIT_H

#include <stdint.h>

#define RADMIT_CAPACITY 8u
#define RADMIT_CPU_MAX  256u
#define RADMIT_MEM_MAX  1024u

void radmit_initialize(void);
int radmit_self_test(void); /* 0 = ok */

/* 0 = zugelassen (*out_slot gesetzt), -1 = abgelehnt (Budget erschoepft
   oder Tabelle voll). */
int radmit_request(uint32_t cpu_units, uint32_t mem_units, uint32_t owner, uint32_t *out_slot);
int radmit_release(uint32_t slot); /* 0 = Erfolg, -1 wenn slot ungueltig */

#endif
