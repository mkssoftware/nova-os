/* Infrastruktur-Test: erste C-Funktion im NovaOS-Kernel. */

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void c_hello(void) __asm__("c_hello");

void c_hello(void) {
    const char *msg = "NOVA: C-Kernel-Infrastruktur aktiv\r\n";
    while (*msg) {
        while (!(inb(0x3FD) & 0x20)) { }
        outb(0x3F8, (unsigned char)*msg++);
    }
}
