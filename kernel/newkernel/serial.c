/* NovaOS C-Kernel - serielle Grundausgabe (COM1, 16550-kompatibel). */
#include "serial.h"
#include <stdint.h>

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint64_t serial_rdtsc(void) {
    uint32_t lo, hi;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

static void serial_raw_putc(unsigned char c) {
    while (!(inb(0x3FD) & 0x20)) { }
    outb(0x3F8, c);
}

/* Jede neue Zeile beginnt mit einem Millisekunden-Zeitstempel seit Boot
   [NNNNNms], damit sich die Boot-Zeit zwischen beliebigen zwei
   Ausgabezeilen direkt in Millisekunden nachpruefen laesst. Kalibrierung
   erfolgt einmalig in serial_init() ueber einen echten PIT-Zaehler-
   Durchlauf (Polling, OHNE IRQs - zu diesem Zeitpunkt ist IF=0, also vor
   interrupt.c's sti; selbe PIT-Konstanten wie interrupt.c's timer_initialize,
   das den Zaehler danach ohnehin erneut programmiert). */
static uint64_t serial_boot_tsc = 0;
static uint32_t serial_tsc_khz = 1; /* 1 = sicherer Nenner, falls Kalibrierung fehlschlaegt */
static int serial_at_line_start = 1;

/* 64-Bit-durch-32-Bit-Division ohne __udivdi3 (im Freestanding-Build nicht
   linkbar, siehe die bestehende 64-Bit-Regel): einfache bitweise
   Restoring-Division nur mit Shift/Vergleich/Subtraktion. */
static uint32_t serial_udiv64_32(uint64_t numerator, uint32_t denom) {
    uint64_t remainder = 0;
    uint64_t quotient = 0;
    for (int i = 63; i >= 0; --i) {
        remainder = (remainder << 1) | ((numerator >> i) & 1u);
        quotient <<= 1;
        if (remainder >= denom) {
            remainder -= denom;
            quotient |= 1u;
        }
    }
    return (uint32_t)quotient;
}

static uint16_t serial_pit_read_count(void) {
    outb(0x43, 0x00); /* Latch-Kommando, Channel 0 */
    uint8_t lo = inb(0x40);
    uint8_t hi = inb(0x40);
    return (uint16_t)lo | ((uint16_t)hi << 8);
}

static void serial_calibrate_tsc(void) {
    /* Channel 0, lobyte/hibyte, Mode 3, binaer - identische Konstanten wie
       interrupt.c's PIT_COMMAND/PIT_DIVISOR (~100Hz, 10ms pro Durchlauf). */
    const uint16_t divisor = 11932u;
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFFu));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFFu));

    uint16_t last = serial_pit_read_count();
    uint64_t start = serial_rdtsc();

    /* Auf einen vollen Zaehler-Durchlauf warten (Counter zaehlt runter und
       springt beim Nachladen wieder hoch) - signalisiert ein volles
       10ms-Intervall. */
    uint16_t now;
    do {
        now = serial_pit_read_count();
        if (now > last) break;
        last = now;
    } while (1);

    uint64_t end = serial_rdtsc();
    uint32_t delta = (uint32_t)(end - start);
    uint32_t khz = delta / 10u; /* 10ms Intervall -> kHz = Zyklen/10 */
    if (khz > 0) {
        serial_tsc_khz = khz;
    }
}

static void serial_put_timestamp(void) {
    uint64_t elapsed_cycles = serial_rdtsc() - serial_boot_tsc;
    uint32_t elapsed_ms = serial_udiv64_32(elapsed_cycles, serial_tsc_khz);

    serial_raw_putc('[');

    char digits[10];
    int n = 0;
    if (elapsed_ms == 0) {
        digits[n++] = '0';
    } else {
        while (elapsed_ms > 0) {
            digits[n++] = (char)('0' + (elapsed_ms % 10u));
            elapsed_ms /= 10u;
        }
    }
    while (n > 0) {
        serial_raw_putc((unsigned char)digits[--n]);
    }

    serial_raw_putc('m');
    serial_raw_putc('s');
    serial_raw_putc(']');
    serial_raw_putc(' ');
}

void serial_init(void) {
    outb(0x3F9, 0x00);
    outb(0x3FB, 0x80);
    outb(0x3F8, 0x01);
    outb(0x3F9, 0x00);
    outb(0x3FB, 0x03);
    outb(0x3FA, 0xC7);
    outb(0x3FC, 0x0B);

    serial_boot_tsc = serial_rdtsc();
    serial_calibrate_tsc();
}

void serial_putc(char c) {
    while (!(inb(0x3FD) & 0x20)) { }
    outb(0x3F8, (unsigned char)c);
}

void serial_puts(const char *s) {
    while (*s) {
        if (serial_at_line_start) {
            serial_put_timestamp();
            serial_at_line_start = 0;
        }
        serial_putc(*s);
        if (*s == '\n') {
            serial_at_line_start = 1;
        }
        ++s;
    }
}

void serial_put_hex32(unsigned int value) {
    static const char digits[] = "0123456789ABCDEF";
    for (int shift = 28; shift >= 0; shift -= 4) {
        serial_putc(digits[(value >> shift) & 0xF]);
    }
}
