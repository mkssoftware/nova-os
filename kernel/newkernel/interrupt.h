/* NovaOS C-Kernel - Interrupt- und Timer-Architektur (Port aus
   interrupt32.inc, nur IDT/PIC-Setup + Selbsttest + PIT-Timer; der volle
   Dispatch (Scheduler/Keyboard/Syscall-Routing) folgt, sobald Scheduler,
   Exception-Manager und Display-Server portiert sind - siehe
   interrupt_dispatch_c in interrupt.c fuer den aktuellen Minimal-Stub). */
#ifndef NOVA_INTERRUPT_H
#define NOVA_INTERRUPT_H

#include <stdint.h>

#define IDT_ENTRY_COUNT 256u
#define IDT_GATE_FLAGS  0x8Eu
#define PIC1_COMMAND    0x20u
#define PIC1_DATA       0x21u
#define PIC2_COMMAND    0xA0u
#define PIC2_DATA       0xA1u
#define PIC_EOI         0x20u
#define PIT_COMMAND     0x43u
#define PIT_CHANNEL0    0x40u
#define PIT_DIVISOR     11932u /* ungefaehr 100 Hz */

/* Programmiert IDT (Exceptions, IRQ0/1, Syscall-Gate) und remapt den 8259
   PIC auf Vektoren 32..47 (nur Timer + Tastatur unmaskiert), dann lidt. */
void interrupt_initialize(void);
int interrupt_self_test(void); /* 0 = ok */

void timer_initialize(void); /* programmiert PIT Channel 0 auf ~100Hz */
uint32_t timer_get_ticks(void);
/* Wartet aktiv auf mindestens 2 echte IRQ0-Ereignisse. Erfordert, dass
   Interrupts via interrupt_enable() aktiv sind. 0 = ok, -1 = Timeout. */
int timer_self_test(void);

void interrupt_enable(void);  /* sti */
void interrupt_disable(void); /* cli */

#endif
