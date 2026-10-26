/* NovaOS C-Kernel - Kernel Component Manager (Port aus memory32.inc,
   ADR-2009 / NPSPEC-KERNEL-0001 Subsystem-Registry). */
#ifndef NOVA_COMPONENT_H
#define NOVA_COMPONENT_H

#include <stdint.h>

#define COMPONENT_CAPACITY 8u
#define OBJECT_TYPE_COMPONENT 2u

void component_manager_initialize(void);
int component_manager_self_test(void); /* 0 = ok */

/* id muss != 0 und eindeutig sein. Gibt den Objekt-Handle zurueck, oder 0
   bei Fehler (voll oder Doppel-Registrierung). */
uint32_t component_register(uint32_t id, uint32_t version, uint32_t flags);

/* Gibt die interne Record-Adresse zurueck, oder NULL wenn nicht registriert. */
void *component_lookup(uint32_t id);

int component_activate(uint32_t id); /* 0 = ok */

/* 0 = Komponente existiert und ist aktiv, -1 sonst. */
int component_is_active(uint32_t id);

uint32_t component_count(void);

#endif
