/* NovaOS C-Kernel - serielle Grundausgabe (COM1, 16550-kompatibel). */
#include "serial.h"

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void serial_init(void) {
    outb(0x3F9, 0x00);
    outb(0x3FB, 0x80);
    outb(0x3F8, 0x01);
    outb(0x3F9, 0x00);
    outb(0x3FB, 0x03);
    outb(0x3FA, 0xC7);
    outb(0x3FC, 0x0B);
}

void serial_putc(char c) {
    while (!(inb(0x3FD) & 0x20)) { }
    outb(0x3F8, (unsigned char)c);
}

void serial_puts(const char *s) {
    while (*s) {
        serial_putc(*s++);
    }
}

void serial_put_hex32(unsigned int value) {
    static const char digits[] = "0123456789ABCDEF";
    for (int shift = 28; shift >= 0; shift -= 4) {
        serial_putc(digits[(value >> shift) & 0xF]);
    }
}
