/* NovaOS C-Kernel - serielle Grundausgabe (COM1, 16550-kompatibel).
   SMP-/IRQ-sicher (siehe serial.c Dateikopf): jeder einzelne Aufruf von
   serial_putc()/serial_puts()/serial_put_hex32() ist atomar gegen
   gleichzeitige Aufrufe von einer anderen CPU (SMP) ODER aus einem
   Interrupt-Handler auf DERSELBEN CPU (IRQ-sicherer Spinlock, siehe
   sync.h sync_spin_lock_irqsave()). Mehrere AUFEINANDERFOLGENDE Aufrufe
   derselben Zeile (das im gesamten Kernel etablierte Muster
   `serial_puts("..."); serial_put_hex32(x); serial_puts("...\r\n");`)
   bleiben bewusst NICHT als Gesamtblock atomar - nur jeder einzelne
   Aufruf fuer sich. */
#ifndef NOVA_SERIAL_H
#define NOVA_SERIAL_H

void serial_init(void);
int serial_self_test(void); /* 0 = ok */
void serial_putc(char c);
void serial_puts(const char *s);
void serial_put_hex32(unsigned int value);

#endif
