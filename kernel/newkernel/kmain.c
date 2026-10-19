/* NovaOS C-Kernel - erster C-Kernmain. Paralleler, objektbasierter Kernel-Kern;
   alte .inc-Subsysteme werden schrittweise hierher portiert. */

void kmain(void) __asm__("kmain");

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static void serial_init(void) {
    outb(0x3F9, 0x00);
    outb(0x3FB, 0x80);
    outb(0x3F8, 0x01);
    outb(0x3F9, 0x00);
    outb(0x3FB, 0x03);
    outb(0x3FA, 0xC7);
    outb(0x3FC, 0x0B);
}

static void serial_putc(char c) {
    while (!(inb(0x3FD) & 0x20)) { }
    outb(0x3F8, (unsigned char)c);
}

static void serial_puts(const char *s) {
    while (*s) {
        serial_putc(*s++);
    }
}

void kmain(void) {
    serial_init();
    serial_puts("NOVA-C: Paralleler C-Kernel-Kern gebootet\r\n");
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
