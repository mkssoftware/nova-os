/* NovaOS C-Kernel - Address Space Layout Randomization
   (Port aus memory32.inc, NPSPEC-MEMORY-ASLR-0001). */
#ifndef NOVA_ASLR_H
#define NOVA_ASLR_H

#include <stdint.h>

#define ASLR_REGION_COUNT 8

#define ASLR_TYPE_STACK 1u
#define ASLR_TYPE_HEAP  2u
#define ASLR_TYPE_MMAP  3u
#define ASLR_TYPE_VDSO  4u

void aslr_initialize(void);
int aslr_self_test(void); /* 0 = ok */

/* Erzeugt einen 4KB-ausgerichteten Zufalls-Offset (max. 1020 KB) fuer
   region_type/base_hint. Gibt den Offset auch zurueck, wenn die Tabelle
   voll ist (nur das Tracking entfaellt dann), identisch zum alten ASM. */
uint32_t aslr_randomize_region(uint32_t region_type, uint32_t base_hint);

#endif
