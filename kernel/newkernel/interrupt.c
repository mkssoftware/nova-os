/* NovaOS C-Kernel - Interrupt- und Timer-Architektur (Port aus
   interrupt32.inc). ISR-Trampoline liegen in interrupt_isr.asm. */
#include "interrupt.h"
#include "serial.h"

typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t offset_high;
} __attribute__((packed)) idt_entry_t;

static idt_entry_t idt_table[IDT_ENTRY_COUNT];
static volatile uint32_t timer_ticks_value = 0;

/* ISR-Trampoline aus interrupt_isr.asm. Die __asm__-Labels verhindern das
   automatische Underscore-Praefix des PE-Targets, damit die Namen exakt auf
   die von NASM erzeugten Symbole passen (gleiches Problem wie bei kmain,
   nur umgekehrt: hier referenziert C ein von ASM definiertes Symbol). */
extern uint32_t exception_stub_table[32] __asm__("exception_stub_table");
extern void irq0_stub(void) __asm__("irq0_stub");
extern void irq1_stub(void) __asm__("irq1_stub");
extern void syscall_stub(void) __asm__("syscall_stub");
extern void isr_unexpected(void) __asm__("isr_unexpected");

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void io_wait(void) {
    outb(0x80, 0);
}

static void idt_set_gate(uint32_t vector, uint32_t handler_addr) {
    idt_entry_t *entry = &idt_table[vector];
    entry->offset_low = (uint16_t)(handler_addr & 0xFFFFu);
    entry->selector = 0x08; /* CODE_SEGMENT aus boot.asm's GDT */
    entry->zero = 0;
    entry->flags = (uint8_t)IDT_GATE_FLAGS;
    entry->offset_high = (uint16_t)((handler_addr >> 16) & 0xFFFFu);
}

/* Minimaler Dispatch (cdecl, aus interrupt_isr.asm aufgerufen): zaehlt
   Timer-Ticks und quittiert IRQs. Scheduler-/Keyboard-/Syscall-Routing
   folgt, sobald die jeweiligen Subsysteme portiert sind. Gibt den
   (unveraenderten) Frame-Zeiger zurueck. */
uint32_t interrupt_dispatch_c(uint32_t vector, uint32_t frame_ptr) __asm__("interrupt_dispatch_c");
uint32_t interrupt_dispatch_c(uint32_t vector, uint32_t frame_ptr) {
    if (vector == 32) {
        ++timer_ticks_value;
        outb(PIC1_COMMAND, PIC_EOI);
        return frame_ptr;
    }
    if (vector == 33) {
        (void)inb(0x60); /* Scancode quittieren, noch kein Input-Router */
        outb(PIC1_COMMAND, PIC_EOI);
        return frame_ptr;
    }
    if (vector >= 32 && vector < 48) {
        /* Slave-IRQs (34..47): minimaler EOI-Fallback. */
        outb(PIC2_COMMAND, PIC_EOI);
        outb(PIC1_COMMAND, PIC_EOI);
        return frame_ptr;
    }
    if (vector < 32) {
        /* Exception ohne Exception-Manager (noch nicht portiert). */
        serial_puts("NOVA: EXCEPTION (kein Exception-Manager aktiv) - HALT\r\n");
        for (;;) {
            __asm__ volatile ("cli");
            __asm__ volatile ("hlt");
        }
    }
    /* 0x80 Syscall oder isr_unexpected (255): noch kein Dispatch. */
    return frame_ptr;
}

void interrupt_initialize(void) {
    __asm__ volatile ("cli");

    for (uint32_t v = 0; v < IDT_ENTRY_COUNT; ++v) {
        idt_set_gate(v, (uint32_t)isr_unexpected);
    }
    for (uint32_t v = 0; v < 32; ++v) {
        idt_set_gate(v, exception_stub_table[v]);
    }
    idt_set_gate(32, (uint32_t)irq0_stub);
    idt_set_gate(33, (uint32_t)irq1_stub);
    idt_set_gate(0x80, (uint32_t)syscall_stub);
    idt_table[0x80].flags = 0xEE; /* present, Ring-3, Interrupt-Gate */

    struct {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed)) idtr = {
        .limit = (uint16_t)(IDT_ENTRY_COUNT * 8u - 1u),
        .base = (uint32_t)idt_table,
    };
    __asm__ volatile ("lidt %0" : : "m"(idtr));

    /* UEFI/OVMF kann externe Interrupts ueber den APIC-Pfad hinterlassen;
       der Bootstrap-Interruptmanager arbeitet noch mit dem 8259 PIC, daher
       Local APIC und IMCR deterministisch in den Legacy-INTR-Zustand
       bringen (Port aus interrupt32.inc, nicht-APIC-Zweig). */
    uint32_t msr_lo, msr_hi;
    __asm__ volatile ("rdmsr" : "=a"(msr_lo), "=d"(msr_hi) : "c"(0x1Bu));
    msr_lo &= 0xFFFFF7FFu; /* globales APIC-Enable (Bit 11) loeschen */
    __asm__ volatile ("wrmsr" : : "a"(msr_lo), "d"(msr_hi), "c"(0x1Bu));

    outb(0x22, 0x70);
    uint8_t imcr = inb(0x23);
    imcr &= 0xFEu;
    outb(0x23, imcr);

    /* 8259 PIC: IRQs auf Vektoren 32..47 verschieben. */
    outb(PIC1_COMMAND, 0x11);
    io_wait();
    outb(PIC2_COMMAND, 0x11);
    io_wait();
    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();
    outb(PIC1_DATA, 0x04);
    io_wait();
    outb(PIC2_DATA, 0x02);
    io_wait();
    outb(PIC1_DATA, 0x01);
    io_wait();
    outb(PIC2_DATA, 0x01);
    io_wait();
    outb(PIC1_DATA, 0xFC); /* nur Timer und Tastatur freigeben */
    outb(PIC2_DATA, 0xFF);
}

int interrupt_self_test(void) {
    int errors = 0;

    struct {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed)) idtr;
    __asm__ volatile ("sidt %0" : "=m"(idtr));

    if (idtr.base != (uint32_t)idt_table) ++errors;
    if (idtr.limit != (uint16_t)(IDT_ENTRY_COUNT * 8u - 1u)) ++errors;

    uint32_t vec0 = idt_table[0].offset_low | ((uint32_t)idt_table[0].offset_high << 16);
    if (vec0 == (uint32_t)isr_unexpected) ++errors; /* Vektor 0 muss den echten Exception-Stub tragen, nicht den Default */

    uint32_t vec32 = idt_table[32].offset_low | ((uint32_t)idt_table[32].offset_high << 16);
    if (vec32 != (uint32_t)irq0_stub) ++errors;

    uint32_t vec80 = idt_table[0x80].offset_low | ((uint32_t)idt_table[0x80].offset_high << 16);
    if (vec80 != (uint32_t)syscall_stub) ++errors;

    if (idt_table[0x80].flags != 0xEE) ++errors;

    return (errors == 0) ? 0 : -1;
}

void timer_initialize(void) {
    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0, (uint8_t)(PIT_DIVISOR & 0xFFu));
    outb(PIT_CHANNEL0, (uint8_t)((PIT_DIVISOR >> 8) & 0xFFu));
    timer_ticks_value = 0;
}

uint32_t timer_get_ticks(void) {
    return timer_ticks_value;
}

void interrupt_enable(void) {
    __asm__ volatile ("sti");
}

void interrupt_disable(void) {
    __asm__ volatile ("cli");
}

int timer_self_test(void) {
    uint32_t spins = 50000000u;
    while (timer_ticks_value < 2) {
        __asm__ volatile ("pause");
        if (--spins == 0) {
            return -1;
        }
    }
    return 0;
}
