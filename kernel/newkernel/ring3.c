/* NovaOS C-Kernel - Ring-3-Bootstrap (siehe ring3.h). */
#include "ring3.h"
#include "pmm.h"
#include "paging.h"
#include "process.h"

extern uint8_t stack_top[] __asm__("stack_top");

#define RING0_DATA_SELECTOR 0x10
#define RING3_CODE_SELECTOR 0x1B
#define RING3_DATA_SELECTOR 0x23
#define TSS_SELECTOR        0x28

#define RING3_CODE_VADDR  0x00400000
#define RING3_STACK_VADDR 0x00401000
#define RING3_STACK_TOP   0x00401FF0

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

/* Minimales Ring-3-Testprogramm (handgeschriebenes Maschinencode-Shellcode,
   da ein von GCC kompiliertes C-Programm beim Kopieren auf eine frische
   Seite Relokationen/Aufrufe in den Rest des Kernel-Images verlieren
   wuerde): wiederholt EAX="RIN3"-Magic setzen, int 0x80 (Heartbeat-Syscall),
   zurueckspringen. Rest der Seite bleibt mit Nullen gefuellt (unerreichbar).
   B8 33 4E 49 52   mov eax, 0x52494E33
   CD 80            int 0x80
   EB F7            jmp $-9 (zurueck zu "mov eax") */
static const uint8_t ring3_program[] = {
    0xB8, 0x33, 0x4E, 0x49, 0x52,
    0xCD, 0x80,
    0xEB, 0xF7,
};

static uint64_t ring3_gdt[6];
static uint8_t kernel_tss[104];
static int ring3_ready = 0;
static uint32_t ring3_pid = 0;

static inline uint32_t read_cr3(void) {
    uint32_t value;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(value));
    return value;
}

int ring3_initialize(void) {
    for (uint32_t i = 0; i < sizeof(kernel_tss); ++i) kernel_tss[i] = 0;
    *(uint32_t *)&kernel_tss[4] = (uint32_t)stack_top;   /* ESP0 */
    *(uint16_t *)&kernel_tss[8] = RING0_DATA_SELECTOR;    /* SS0 */
    *(uint16_t *)&kernel_tss[102] = (uint16_t)sizeof(kernel_tss); /* IOPB-Offset = Ende, keine IOPB */

    uint32_t tss_base = (uint32_t)kernel_tss;
    uint64_t tss_descriptor =
        (uint64_t)(sizeof(kernel_tss) - 1u)
        | ((uint64_t)(tss_base & 0xFFFFFFu) << 16)
        | ((uint64_t)0x89u << 40)
        | ((uint64_t)((tss_base >> 24) & 0xFFu) << 56);

    ring3_gdt[0] = 0x0000000000000000ull;
    ring3_gdt[1] = 0x00CF9A000000FFFFull; /* Ring-0 Code, Selector 0x08 (identisch zu boot.asm) */
    ring3_gdt[2] = 0x00CF92000000FFFFull; /* Ring-0 Data, Selector 0x10 (identisch zu boot.asm) */
    ring3_gdt[3] = 0x00CFFA000000FFFFull; /* Ring-3 Code, Selector 0x1B */
    ring3_gdt[4] = 0x00CFF2000000FFFFull; /* Ring-3 Data, Selector 0x23 */
    ring3_gdt[5] = tss_descriptor;        /* TSS, Selector 0x28 */

    struct {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed)) gdtr = {
        .limit = (uint16_t)(sizeof(ring3_gdt) - 1u),
        .base = (uint32_t)ring3_gdt,
    };
    /* Entries 1/2 sind byte-identisch zur boot.asm-GDT, daher braucht kein
       Segmentregister (CS/DS/SS/...) nach dem lgdt neu geladen zu werden -
       der versteckte Deskriptor-Cache bleibt inhaltlich korrekt. */
    __asm__ volatile ("lgdt %0" : : "m"(gdtr));

    uint16_t tss_selector = TSS_SELECTOR;
    __asm__ volatile ("ltr %0" : : "m"(tss_selector));

    uint32_t numa;
    uint32_t code_page = pmm_alloc_page(&numa);
    if (code_page == 0) return -1;
    uint8_t *code_dst = (uint8_t *)code_page;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE; ++i) code_dst[i] = 0;
    for (uint32_t i = 0; i < sizeof(ring3_program); ++i) code_dst[i] = ring3_program[i];

    if (paging_map_page(RING3_CODE_VADDR, code_page, PAGING_PAGE_PRESENT | PAGING_PAGE_USER) != 0) {
        return -1;
    }

    uint32_t stack_page = pmm_alloc_page(&numa);
    if (stack_page == 0) return -1;
    uint8_t *stack_dst = (uint8_t *)stack_page;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE; ++i) stack_dst[i] = 0;

    if (paging_map_page(RING3_STACK_VADDR, stack_page,
                         PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE | PAGING_PAGE_USER) != 0) {
        return -1;
    }

    /* TLB-Eintraege der zuvor (ohne User-Bit) identisch gemappten Seiten
       verwerfen - sonst greift die CPU weiter auf die alten Supervisor-
       Only-Uebersetzungen zu. */
    __asm__ volatile ("mov %%cr3, %%eax\n\t mov %%eax, %%cr3" ::: "eax");

    /* Teilt vorerst den Kernel-Adressraum (kein eigenes Page-Directory -
       echte Prozessisolation braucht einen Page-Directory-Kopierer, der
       noch nicht portiert ist). */
    uint32_t pid = process_create(0x4 /* PROCESS_FLAG_USERSPACE (lokal, siehe io_qos32.inc) */,
                                   read_cr3());
    if (pid == 0) return -1;
    ring3_pid = pid;

    ring3_ready = 1;
    return 0;
}

int ring3_self_test(void) {
    if (!ring3_ready) return -1;
    if (ring3_pid == 0) return -1;
    if (process_lookup(ring3_pid) == 0) return -1;

    struct {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed)) gdtr;
    __asm__ volatile ("sgdt %0" : "=m"(gdtr));
    if (gdtr.base != (uint32_t)ring3_gdt) return -1;
    if (gdtr.limit != (uint16_t)(sizeof(ring3_gdt) - 1u)) return -1;

    uint16_t tr;
    __asm__ volatile ("str %0" : "=r"(tr));
    if (tr != TSS_SELECTOR) return -1;

    if (*(uint32_t *)&kernel_tss[4] != (uint32_t)stack_top) return -1;
    if (*(uint16_t *)&kernel_tss[8] != RING0_DATA_SELECTOR) return -1;

    /* Ring-3/Ring-0-Deskriptoren muessen DPL=3 bzw. DPL=0 tragen (Byte 5
       jedes 8-Byte-Deskriptors enthaelt den Access-Byte mit den DPL-Bits
       5-6). */
    if (((ring3_gdt[3] >> 45) & 0x3u) != 3u) return -1; /* Ring-3 Code DPL */
    if (((ring3_gdt[4] >> 45) & 0x3u) != 3u) return -1; /* Ring-3 Data DPL */
    if (((ring3_gdt[1] >> 45) & 0x3u) != 0u) return -1; /* Ring-0 Code DPL */

    return 0;
}

void ring3_enter(void) {
    __asm__ volatile (
        "cli\n\t"
        "mov $" STR(RING3_DATA_SELECTOR) ", %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "push $" STR(RING3_DATA_SELECTOR) "\n\t"
        "push $" STR(RING3_STACK_TOP) "\n\t"
        "push $0x202\n\t"
        "push $" STR(RING3_CODE_SELECTOR) "\n\t"
        "push $" STR(RING3_CODE_VADDR) "\n\t"
        "iret\n\t"
        :
        :
        : "eax"
    );
    for (;;) { /* unerreichbar - iret springt nach Ring 3, siehe ring3.h */
        __asm__ volatile ("hlt");
    }
}
