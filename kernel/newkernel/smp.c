/* NovaOS C-Kernel - SMP/AP-Aktivierung (Port aus security32.inc, siehe
   smp.h). */
#include "smp.h"
#include "hal.h"
#include "interrupt.h"

#define LAPIC_BASE             0xFEE00000u
#define LAPIC_SPURIOUS         0x0F0u
#define LAPIC_LVT0             0x350u
#define LAPIC_ICR_LO           0x300u
#define LAPIC_ICR_HI           0x310u
#define LAPIC_ICR_DELIVERY_STS (1u << 12)
#define LAPIC_IPI_INIT         0x00004500u
#define LAPIC_IPI_SIPI         0x00004600u
#define LAPIC_IPI_FIXED        0x00004000u

#define AP_TRAMPOLINE_BASE   0x8000u
#define AP_TRAMPOLINE_VECTOR 0x08u /* SIPI-Vektor = AP_TRAMPOLINE_BASE >> 12 */
#define AP_BOOT_TIMEOUT_TICKS 50u  /* ~500ms bei 100Hz PIT */
#define AP_STACK_SIZE        4096u

#define SMP_MAX_APS (ACPI_CPU_CAPACITY - 1u) /* Slot 0 = BSP */

extern const uint8_t ap_trampoline_blob[] __asm__("ap_trampoline_blob");
extern const uint8_t ap_trampoline_blob_end[] __asm__("ap_trampoline_blob_end");
extern void ap_entry32_pm(void) __asm__("ap_entry32_pm");

static volatile uint32_t smp_ap_online = 0;
static volatile uint32_t smp_next_ap_slot = 0;
static volatile uint32_t smp_ap_heartbeat[SMP_MAX_APS];
static int smp_lapic_enabled = 0;

/* Pro AP ein eigener, dedizierter Stack - mehrere APs duerfen NIEMALS
   denselben Stack gleichzeitig benutzen (Race Condition). Jeder Eintrag
   wird einzeln in den Trampolin-Patch-Bereich kopiert, bevor genau
   dieser AP per SIPI gestartet wird (siehe smp_initialize - strikt
   sequenziell: der naechste AP startet erst, nachdem der vorherige sich
   bereits gemeldet hat, der Trampolin-Patch-Bereich ist also nie
   gleichzeitig fuer zwei APs in Benutzung). */
static uint8_t ap_stacks[SMP_MAX_APS][AP_STACK_SIZE] __attribute__((aligned(16)));

static inline uint32_t lapic_read(uint32_t reg) {
    return *(volatile uint32_t *)(uintptr_t)(LAPIC_BASE + reg);
}

static inline void lapic_write(uint32_t reg, uint32_t value) {
    *(volatile uint32_t *)(uintptr_t)(LAPIC_BASE + reg) = value;
}

static void apic_wait_icr_idle(void) {
    uint32_t spins = 200000u;
    while (lapic_read(LAPIC_ICR_LO) & LAPIC_ICR_DELIVERY_STS) {
        __asm__ volatile ("pause");
        if (--spins == 0) break;
    }
}

/* Aktiviert den Local APIC im Virtual-Wire-Kompatibilitaetsmodus - siehe
   smp.h fuer die Begruendung (LINT0=ExtINT/unmaskiert reicht die
   bestehende 8259-PIC-Zustellung unveraendert durch). */
static void lapic_enable_virtual_wire(void) {
    uint32_t msr_lo, msr_hi;
    __asm__ volatile ("rdmsr" : "=a"(msr_lo), "=d"(msr_hi) : "c"(0x1Bu));
    msr_lo |= (1u << 11); /* globales APIC-Enable setzen */
    __asm__ volatile ("wrmsr" : : "a"(msr_lo), "d"(msr_hi), "c"(0x1Bu));

    /* Spurious-Interrupt-Vektor-Register: Software-Enable (Bit 8) + Vektor 0xFF. */
    uint32_t svr = lapic_read(LAPIC_SPURIOUS);
    svr |= 0x100u;
    svr = (svr & 0xFFFFFF00u) | 0xFFu;
    lapic_write(LAPIC_SPURIOUS, svr);

    /* LINT0 = ExtINT (Delivery-Mode 0b111), unmaskiert - "virtual wire":
       das bestehende 8259-PIC-Signal wird transparent durchgereicht. */
    lapic_write(LAPIC_LVT0, 0x700u);
}

static void smp_send_init_sipi(uint32_t apic_id) {
    lapic_write(LAPIC_ICR_HI, apic_id << 24);
    lapic_write(LAPIC_ICR_LO, LAPIC_IPI_INIT);
    apic_wait_icr_idle();

    uint32_t target = timer_get_ticks() + 1u;
    while (timer_get_ticks() < target) {
        __asm__ volatile ("pause");
    }

    lapic_write(LAPIC_ICR_HI, apic_id << 24);
    lapic_write(LAPIC_ICR_LO, LAPIC_IPI_SIPI | AP_TRAMPOLINE_VECTOR);
    apic_wait_icr_idle();

    for (volatile uint32_t i = 0; i < 200u; ++i) {
        __asm__ volatile ("pause");
    }

    lapic_write(LAPIC_ICR_HI, apic_id << 24);
    lapic_write(LAPIC_ICR_LO, LAPIC_IPI_SIPI | AP_TRAMPOLINE_VECTOR);
    apic_wait_icr_idle();
}

/* Kopiert den Trampolin-Blob einmalig nach 0x8000 und patcht die
   AP-unabhaengigen Felder (GDT/PM-Einstieg/Code-Selektor). Das
   Stack-Feld (+0x10) wird separat PRO AP gepatcht, siehe
   smp_patch_ap_stack. */
static void smp_prepare_trampoline(void) {
    uint8_t *dst = (uint8_t *)(uintptr_t)AP_TRAMPOLINE_BASE;
    uint32_t len = (uint32_t)(ap_trampoline_blob_end - ap_trampoline_blob);
    for (uint32_t i = 0; i < len; ++i) dst[i] = ap_trampoline_blob[i];

    struct {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed)) gdtr;
    __asm__ volatile ("sgdt %0" : "=m"(gdtr));

    *(uint16_t *)(uintptr_t)(AP_TRAMPOLINE_BASE + 0x02) = gdtr.limit;
    *(uint32_t *)(uintptr_t)(AP_TRAMPOLINE_BASE + 0x04) = gdtr.base;
    *(uint32_t *)(uintptr_t)(AP_TRAMPOLINE_BASE + 0x08) = (uint32_t)(uintptr_t)ap_entry32_pm;
    *(uint16_t *)(uintptr_t)(AP_TRAMPOLINE_BASE + 0x0C) = 0x08u;
}

static void smp_patch_ap_stack(uint32_t stack_top) {
    *(uint32_t *)(uintptr_t)(AP_TRAMPOLINE_BASE + 0x10) = stack_top;
    __asm__ volatile ("wbinvd");
}

/* Vom AP-Trampolin (ap_trampoline.asm) nach dem Uebergang nach Ring 0 /
   Protected Mode aufgerufen. CS=0x08, DS/ES/SS/FS/GS bereits neu
   geladen, eigener Stack gesetzt (siehe ap_trampoline.asm, Patch-Feld
   +0x10). Kein Paging aktiv - der AP adressiert direkt physisch/linear
   flach, was fuer alle von ihm beruehrten Adressen (eigener Code/Stack,
   LAPIC-MMIO) ausreicht, da unser Kernel ohnehin komplett unterhalb
   16 MiB identity-gemappt waere. Kehrt nie zurueck.

   Weist sich selbst einen 0-basierten Slot zu (atomarer Zaehler) - da
   smp_initialize APs strikt sequenziell startet (naechster AP erst nach
   Online-Meldung des vorherigen), entspricht der hier zugewiesene Slot
   immer genau dem Index, fuer den smp_initialize kurz zuvor den Stack
   gepatcht hat. Danach: dauerhafte Endlosschleife, die NUR den eigenen
   Heartbeat-Zaehler erhoeht - echter, fortlaufend beobachtbarer
   Parallelbetrieb, aber bewusst OHNE Interrupts (IF=0) und OHNE
   Scheduler-Beitritt (siehe smp.h). */
void ap_entry_c(void) __asm__("ap_entry_c");
void ap_entry_c(void) {
    uint32_t slot = __atomic_fetch_add(&smp_next_ap_slot, 1u, __ATOMIC_SEQ_CST);
    __atomic_fetch_add(&smp_ap_online, 1u, __ATOMIC_SEQ_CST);

    for (;;) {
        if (slot < SMP_MAX_APS) {
            __atomic_fetch_add(&smp_ap_heartbeat[slot], 1u, __ATOMIC_SEQ_CST);
        }
        __asm__ volatile ("pause");
    }
}

int smp_initialize(void) {
    smp_ap_online = 0;
    smp_next_ap_slot = 0;
    for (uint32_t i = 0; i < SMP_MAX_APS; ++i) smp_ap_heartbeat[i] = 0;

    uint32_t cpu_count = acpi_cpu_count();
    if (cpu_count <= 1u) {
        return 0; /* UP-Betrieb: LAPIC bleibt deaktiviert, kein Risiko */
    }

    lapic_enable_virtual_wire();
    smp_lapic_enabled = 1;
    smp_prepare_trampoline();

    uint32_t ap_count = cpu_count - 1u;
    if (ap_count > SMP_MAX_APS) ap_count = SMP_MAX_APS;

    for (uint32_t slot = 0; slot < ap_count; ++slot) {
        uint32_t apic_id = acpi_cpu_apic_id(slot + 1u);
        uint32_t stack_top = (uint32_t)(uintptr_t)&ap_stacks[slot][AP_STACK_SIZE - 16u];
        stack_top &= 0xFFFFFFF0u;

        uint32_t before = smp_ap_online;
        smp_patch_ap_stack(stack_top);
        smp_send_init_sipi(apic_id);

        uint32_t deadline = timer_get_ticks() + AP_BOOT_TIMEOUT_TICKS;
        while (smp_ap_online == before && timer_get_ticks() < deadline) {
            __asm__ volatile ("pause");
        }
        /* Timeout = Soft-Fail (1:1 wie im alten ASM) - naechste CPU
           trotzdem versuchen. */
    }

    return 0;
}

uint32_t smp_ap_online_count(void) {
    return smp_ap_online;
}

uint32_t smp_ap_heartbeat_count(uint32_t ap_index) {
    if (ap_index >= SMP_MAX_APS) return 0;
    return smp_ap_heartbeat[ap_index];
}

int smp_self_test(void) {
    uint32_t cpu_count = acpi_cpu_count();

    if (cpu_count <= 1u) {
        if (smp_lapic_enabled) return -1; /* UP: LAPIC darf nicht angefasst worden sein */
        if (smp_ap_online != 0) return -1;
        return 0;
    }

    if (!smp_lapic_enabled) return -1;
    if (smp_ap_online == 0) return -1; /* mindestens ein AP muss sich gemeldet haben */

    /* Echte, fortlaufende Parallelausfuehrung nachweisen: Heartbeat-
       Schnappschuss nehmen, kurz warten (echte Timer-Ticks, kein reines
       Spin-Delay), erneut pruefen - jeder online gemeldete AP muss in
       der Zwischenzeit WEITERGEZAEHLT haben. */
    uint32_t before[SMP_MAX_APS];
    for (uint32_t i = 0; i < SMP_MAX_APS; ++i) before[i] = smp_ap_heartbeat[i];

    uint32_t deadline = timer_get_ticks() + 3u;
    while (timer_get_ticks() < deadline) {
        __asm__ volatile ("pause");
    }

    uint32_t online_aps = smp_ap_online;
    if (online_aps > SMP_MAX_APS) online_aps = SMP_MAX_APS;
    for (uint32_t i = 0; i < online_aps; ++i) {
        if (smp_ap_heartbeat[i] <= before[i]) return -1;
    }

    return 0;
}
