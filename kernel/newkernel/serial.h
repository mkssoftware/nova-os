/* NovaOS C-Kernel - serielle Grundausgabe (COM1, 16550-kompatibel). */
#ifndef NOVA_SERIAL_H
#define NOVA_SERIAL_H

void serial_init(void);
void serial_putc(char c);
void serial_puts(const char *s);
void serial_put_hex32(unsigned int value);

#endif
