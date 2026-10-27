/* NovaOS C-Kernel - Nova Interrupt Manager 1.0 (Port aus sync32.inc,
   §009 / NPSPEC-KERNEL-0009). Bootstrap: Handler-Registrierung pro
   Legacy-IRQ-Leitung (0-15), dynamische Vektor-Allokation (48-255),
   Dispatch-Statistiken. Baut konzeptionell auf dem bestehenden
   IDT/PIC-Setup aus interrupt.c auf, ruft aber (noch) nicht automatisch
   aus `interrupt_dispatch_c` heraus - das waere die naechste Ausbaustufe,
   sobald ein echter Treiber einen Handler registrieren muss. `vsvc_register`
   (§054 Versioned-Service-Registry) ist noch nicht portiert, daher
   registriert `irq_manager_initialize` KEINEN VSVC-Service (wie schon bei
   sync.c). */
#ifndef NOVA_IRQMGR_H
#define NOVA_IRQMGR_H

#include <stdint.h>

#define IRQ_MANAGER_SLOTS   16u  /* ein Slot pro Legacy-IRQ-Leitung */
#define IRQ_VECTOR_DYN_BASE 48u  /* dynamischer Pool: Vektoren 48-255 */

#define NP_IRQ_NOT_HANDLED 0u
#define NP_IRQ_HANDLED     1u

/* Handler-Rueckgabewert: NP_IRQ_HANDLED/NOT_HANDLED/... (nur HANDLED wird
   vom Dispatch ausgewertet, Rest ist fuer spaetere Ausbaustufen reserviert). */
typedef uint32_t (*irq_handler_fn_t)(uint32_t context);

void irq_manager_initialize(void);
int irq_manager_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn irq_num >= IRQ_MANAGER_SLOTS oder Slot bereits
   belegt (ohne NP_IRQ_SHARED-Flag, hier nicht weiter ausgewertet). */
int irq_register_handler(uint32_t irq_num, irq_handler_fn_t handler, uint32_t context, uint32_t flags);

/* 0 = Erfolg, -1 wenn irq_num >= IRQ_MANAGER_SLOTS. */
int irq_unregister_handler(uint32_t irq_num);

/* Ruft den registrierten Handler fuer irq_num auf (falls vorhanden) und
   aktualisiert die Statistiken. Gibt NP_IRQ_HANDLED/NP_IRQ_NOT_HANDLED
   zurueck. EOI bleibt Aufgabe des Aufrufers (wie im alten ASM). */
uint32_t irq_dispatch_hw(uint32_t irq_num);

/* 0 = Erfolg (*out_first_vector gesetzt), -1 wenn count ungueltig
   (0 oder > 208) oder kein zusammenhaengender Bereich mehr frei ist. */
int np_interrupt_vector_allocate(uint32_t count, uint32_t *out_first_vector);

/* 0 = Erfolg, -1 wenn first_vector < IRQ_VECTOR_DYN_BASE, count==0 oder
   der Bereich ueber Vektor 256 hinausreicht. */
int np_interrupt_vector_free(uint32_t first_vector, uint32_t count);

#endif
