/* NovaOS C-Kernel - Bootstrap-Kernel-Heap (Port aus memory32.inc,
   ADR-2003 / NPSPEC-KERNEL-0008). */
#ifndef NOVA_HEAP_H
#define NOVA_HEAP_H

#include <stdint.h>

#define HEAP_ALIGNMENT 16u

void heap_initialize(void);
int heap_self_test(void); /* 0 = ok */

/* NULL bei size==0, size>PMM_PAGE_SIZE oder PMM-OOM. 16-Byte-ausgerichtet. */
void *heap_allocate(uint32_t size);

#endif
