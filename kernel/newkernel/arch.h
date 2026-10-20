/* NovaOS C-Kernel - Architecture-Model Introspection-Registry (Port aus
   arch32.inc, NPSPEC-ARCH-*). */
#ifndef NOVA_ARCH_H
#define NOVA_ARCH_H

#include <stdint.h>

#define ARCH_PROP_DETERMINISTIC  0x0001u
#define ARCH_PROP_ZEROCOPY       0x0002u
#define ARCH_PROP_TRANSACTIONAL  0x0004u
#define ARCH_PROP_LOCATION_TRANS 0x0008u
#define ARCH_PROP_LIVE_EVOLVE    0x0010u
#define ARCH_PROP_STRUCTURED_CC  0x0020u
#define ARCH_PROP_INTROSPECTABLE 0x0040u
#define ARCH_PROP_RESOURCE_ECO   0x0080u

#define ARCH_INTRO_CAPACITY 24u

void arch_initialize(void);
int arch_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn die Tabelle voll ist. */
int arch_register_module(uint32_t module_id, uint32_t version, uint32_t properties,
                          uint32_t entry_point);

/* Gibt den internen Record zurueck, oder NULL wenn module_id unbekannt. */
void *arch_introspect(uint32_t module_id);

#endif
