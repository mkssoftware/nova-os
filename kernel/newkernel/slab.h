/* NovaOS C-Kernel - Slab Allocator (Port aus memory32.inc,
   NPSPEC-MEMORY-SLAB-0001). 4 feste Objekt-Klassen (16/32/64/128 Byte),
   je 16 Objekte pro Klasse; alloc = pop aus freier Liste, free = push zurueck. */
#ifndef NOVA_SLAB_H
#define NOVA_SLAB_H

#include <stdint.h>

#define SLAB_SIZE_0 16u
#define SLAB_SIZE_1 32u
#define SLAB_SIZE_2 64u
#define SLAB_SIZE_3 128u

void slab_initialize(void);
int slab_self_test(void); /* 0 = ok */

/* NULL bei zu grosser Anforderung (>128 Byte) oder wenn die Klasse erschoepft ist. */
void *slab_alloc(uint32_t size);

/* size muss dieselbe Groessenklasse wie bei der Allokation treffen. */
void slab_free(void *ptr, uint32_t size);

#endif
