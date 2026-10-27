/* NovaOS C-Kernel - Nova Interrupt Manager 1.0 (Port aus sync32.inc). */
#include "irqmgr.h"

#define NP_ERR_IRQ_INVALID_SOURCE    (-111)
#define NP_ERR_IRQ_ALREADY_BOUND     (-113)
#define NP_ERR_IRQ_INVALID_VECTOR    (-110)
#define NP_ERR_IRQ_VECTOR_EXHAUSTED  (-112)

typedef struct {
    irq_handler_fn_t handler; /* 0 = frei */
    uint32_t context;
    uint32_t flags;
    uint32_t irq_num;
} irqh_rec_t;

static irqh_rec_t irq_handler_table[IRQ_MANAGER_SLOTS];
static uint32_t irq_counts[IRQ_MANAGER_SLOTS];
static uint8_t irq_vector_used[256];
static uint32_t irq_stat_total = 0;
static uint32_t irq_stat_spurious = 0;
static uint32_t irq_stat_unhandled = 0;
static int irq_mgr_ready = 0;

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void irq_manager_initialize(void) {
    for (uint32_t i = 0; i < IRQ_MANAGER_SLOTS; ++i) {
        irq_handler_table[i] = (irqh_rec_t){0};
        irq_counts[i] = 0;
    }

    for (uint32_t v = 0; v < 256; ++v) irq_vector_used[v] = 0;
    for (uint32_t v = 0; v < IRQ_VECTOR_DYN_BASE; ++v) irq_vector_used[v] = 1; /* 0-47 reserviert */

    irq_stat_total = 0;
    irq_stat_spurious = 0;
    irq_stat_unhandled = 0;

    /* VSVC-Service "nova.irq.manager" wird NICHT registriert -
       vsvc_register (§054) ist noch nicht portiert, siehe irqmgr.h. */

    irq_mgr_ready = 1;
}

int irq_register_handler(uint32_t irq_num, irq_handler_fn_t handler, uint32_t context, uint32_t flags) {
    if (irq_num >= IRQ_MANAGER_SLOTS) return -1;

    irqh_rec_t *rec = &irq_handler_table[irq_num];
    if (rec->handler != 0) return -1; /* NP_ERR_IRQ_ALREADY_BOUND */

    rec->handler = handler;
    rec->context = context;
    rec->flags = flags;
    rec->irq_num = irq_num;
    return 0;
}

int irq_unregister_handler(uint32_t irq_num) {
    if (irq_num >= IRQ_MANAGER_SLOTS) return -1;

    irq_handler_table[irq_num] = (irqh_rec_t){0};
    return 0;
}

uint32_t irq_dispatch_hw(uint32_t irq_num) {
    if (irq_num >= IRQ_MANAGER_SLOTS) {
        ++irq_stat_spurious;
        return NP_IRQ_NOT_HANDLED;
    }

    ++irq_stat_total;
    ++irq_counts[irq_num];

    irqh_rec_t *rec = &irq_handler_table[irq_num];
    if (rec->handler == 0) {
        ++irq_stat_unhandled;
        return NP_IRQ_NOT_HANDLED;
    }

    rec->handler(rec->context);
    return NP_IRQ_HANDLED;
}

int np_interrupt_vector_allocate(uint32_t count, uint32_t *out_first_vector) {
    if (count == 0 || count > 208u) return -1;

    for (uint32_t start = IRQ_VECTOR_DYN_BASE; start < 256u; ++start) {
        uint32_t end = start + count;
        if (end > 256u) break;

        uint32_t v;
        for (v = start; v < end; ++v) {
            if (irq_vector_used[v] != 0) break;
        }
        if (v == end) {
            for (v = start; v < end; ++v) irq_vector_used[v] = 1;
            *out_first_vector = start;
            return 0;
        }
    }
    return -1;
}

int np_interrupt_vector_free(uint32_t first_vector, uint32_t count) {
    if (first_vector < IRQ_VECTOR_DYN_BASE) return -1;
    if (count == 0) return -1;
    if (first_vector + count > 256u) return -1;

    for (uint32_t v = first_vector; v < first_vector + count; ++v) irq_vector_used[v] = 0;
    return 0;
}

static uint32_t irq_st_cb_called = 0;

static uint32_t irq_st_test_handler(uint32_t context) {
    (void)context;
    irq_st_cb_called = 1;
    return NP_IRQ_HANDLED;
}

int irq_manager_self_test(void) {
    if (!irq_mgr_ready) return -1;

    /* Test 1 (PIC-Maske): unser interrupt.c laeuft immer im Legacy-PIC-
       Modus (nicht APIC) und maskiert nie alle IRQs - 0xFF waere ein
       Fehler. Die urspruenglichen ASM-Tests 1/2 (sidt gegen idt_table
       vergleichen) sind redundant zu interrupt_self_test() (idt_table ist
       dort static) und werden deshalb bewusst nicht wiederholt. */
    if (inb(0x21) == 0xFFu) return -1;

    /* Test 2: irq_register_handler -> Handler im Slot */
    if (irq_register_handler(5, irq_st_test_handler, 0, 0) != 0) return -1;
    if (irq_handler_table[5].handler != irq_st_test_handler) return -1;

    /* Test 3: Doppel-Registrierung -> Fehler */
    if (irq_register_handler(5, irq_st_test_handler, 0, 0) == 0) return -1;

    /* Test 4: irq_dispatch_hw -> Handler aufgerufen */
    irq_st_cb_called = 0;
    if (irq_dispatch_hw(5) != NP_IRQ_HANDLED) return -1;
    if (irq_st_cb_called != 1) return -1;

    /* Test 5: Statistiken aktualisiert */
    if (irq_stat_total != 1) return -1;
    if (irq_counts[5] != 1) return -1;

    /* Test 6: irq_unregister_handler -> Slot geleert */
    if (irq_unregister_handler(5) != 0) return -1;
    if (irq_handler_table[5].handler != 0) return -1;

    /* Test 7: np_interrupt_vector_allocate(1) -> Vektor >= 48 */
    uint32_t vec;
    if (np_interrupt_vector_allocate(1, &vec) != 0) return -1;
    if (vec < IRQ_VECTOR_DYN_BASE) return -1;

    /* Test 8: np_interrupt_vector_free -> anschliessend wieder allokierbar */
    if (np_interrupt_vector_free(vec, 1) != 0) return -1;
    uint32_t vec2;
    if (np_interrupt_vector_allocate(1, &vec2) != 0) return -1;
    np_interrupt_vector_free(vec2, 1); /* aufraeumen */

    return 0;
}
