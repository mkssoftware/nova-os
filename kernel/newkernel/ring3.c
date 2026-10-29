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
   wuerde): ruft echte Syscalls ueber den Dispatch in syscall.c auf.

   Ablauf: (1) SYSCALL_CORE_READY (Heartbeat/Beweis, dass Service+Op+ABI
   korrekt ausgewertet werden und iretd nach Ring 3 zurueckkehrt); (2)
   SYSCALL_SERVICE_GUI/GUI_OP_CREATE_SURFACE (x=20,y=50,w=16,h=4 -> Handle
   in EDI, siehe syscall.h fuer das EDX=x|(y<<16)/ESI=w|(h<<16)-Protokoll);
   (3) vier direkte Schreibzugriffe auf die bereits in ring3_initialize()
   user-gemappte Pufferseite RING3_GUI_BUFFER_VADDR (0x402000) - der
   Ring-3-Client schreibt seinen Pixelinhalt bewusst SELBST (siehe
   render2d.h/Architekturentscheidung: client-seitiges Zeichnen statt
   Draw-Syscall pro Pixel); (4) GUI_OP_ATTACH_BUFFER (Handle aus EDI nach
   EDX uebernommen); (5) GUI_OP_COMMIT (macht die Surface fuer den
   Compositor-Thread sichtbar - siehe compositor_thread_body); (6)
   SYSCALL_SERVICE_INPUT/INPUT_OP_POLL (EDX=0x00402020, eine freie Stelle
   weit genug hinter den vier oben geschriebenen Pixeln innerhalb
   derselben bereits gemappten GUI-Pufferseite - spart eine eigene
   Seitenzuteilung nur fuer eine 36-Byte-Struktur, siehe syscall.h
   syscall_input_poll_args_t); (7) ECHTE Tastatur-Reaktion: `cmp dword
   [0x00402020], 0` (has_event, erstes Feld der Struktur) + `je` ueber
   den folgenden Block - liegt ein Event vor, faerbt das Testprogramm
   seine vier Pixel auf Cyan um und haengt den (unveraenderten) Puffer
   per GUI_OP_ATTACH_BUFFER/GUI_OP_COMMIT erneut an (EDX=EDI, das Handle
   aus Schritt 2 steht dort immer noch - weder syscall_dispatch_gui noch
   syscall_dispatch_input schreiben EDI). Das ist der erste Fall, in dem
   ein echtes Tastatur-/Maus-Event sichtbares Verhalten eines Ring-3-
   Clients ausloest, nicht nur eine Dispatch-Statusabfrage. Ohne
   anstehendes Event (im automatisierten Boot-Test der Normalfall, da
   niemand real tippt) wird der Block uebersprungen - das Programm laeuft
   dann identisch zu vorher weiter; (8) SYSCALL_CORE_EXIT (Beweis, dass
   der Frame-Rewrite den Ruecksprung nach Ring 0 tatsaechlich auslöst).
   Der abschliessende jmp-$ ist nur ein Sicherheitsnetz, falls der
   EXIT-Rueckweg (noch) nicht greift.

   Kernel-seitiger Beweis fuer Schritt 7: syscall.c's syscall_dispatch_
   input loggt einmalig (ueber ein von syscall_manager_self_test nach
   dessen eigenem, garantiert leerem Abschluss "scharf geschaltetes"
   Flag, siehe dort) die Vkey/Typ-Werte des ERSTEN danach wirklich
   zugestellten Events - exakt das gleiche "erstes-echtes-Ergebnis"-
   Muster wie compositor.c's Framebuffer-Pixel-Beweis fuer GUI_OP_COMMIT.

   WICHTIG: syscall_dispatch() ueberschreibt EAX IMMER mit dem Status-Code
   (siehe syscall.c) - EAX muss daher vor JEDEM int-0x80-Aufruf neu
   gesetzt werden, exakt wie schon beim urspruenglichen READY->EXIT-
   Uebergang (siehe auch syscall_manager_self_test's gleichnamiger Bug-
   Fix in dieser Sitzung).

   B8 01 00 00 00   mov eax, 1          ; SYSCALL_SERVICE_CORE
   BB 02 00 00 00   mov ebx, 2          ; SYSCALL_CORE_READY
   B9 01 00 00 00   mov ecx, 1          ; SYSCALL_ABI_VERSION
   31 D2            xor edx, edx
   31 F6            xor esi, esi
   CD 80            int 0x80
   ; --- GUI_OP_CREATE_SURFACE: x=20,y=50,w=16,h=4 ---
   B8 02 00 00 00   mov eax, 2          ; SYSCALL_SERVICE_GUI
   BB 01 00 00 00   mov ebx, 1          ; GUI_OP_CREATE_SURFACE
   B9 01 00 00 00   mov ecx, 1          ; SYSCALL_ABI_VERSION
   BA 14 00 32 00   mov edx, 0x00320014 ; x=20 (0x14), y=50 (0x32)<<16
   BE 10 00 04 00   mov esi, 0x00040010 ; w=16 (0x10), h=4<<16
   CD 80            int 0x80            ; -> EDI = neues Handle
   ; --- Pixelinhalt direkt in die gemappte GUI-Pufferseite schreiben ---
   C7 05 00 20 40 00 00 00 FF 00   mov dword [0x00402000], 0x00FF0000 (rot)
   C7 05 04 20 40 00 00 FF 00 00   mov dword [0x00402004], 0x0000FF00 (gruen)
   C7 05 08 20 40 00 FF 00 00 00   mov dword [0x00402008], 0x000000FF (blau)
   C7 05 0C 20 40 00 FF FF FF 00   mov dword [0x0040200C], 0x00FFFFFF (weiss)
   ; --- GUI_OP_ATTACH_BUFFER(Handle) ---
   89 FA            mov edx, edi        ; Handle von CREATE_SURFACE uebernehmen
   B8 02 00 00 00   mov eax, 2          ; SYSCALL_SERVICE_GUI
   BB 02 00 00 00   mov ebx, 2          ; GUI_OP_ATTACH_BUFFER
   B9 01 00 00 00   mov ecx, 1          ; SYSCALL_ABI_VERSION
   CD 80            int 0x80
   ; --- GUI_OP_COMMIT(Handle) ---
   B8 02 00 00 00   mov eax, 2          ; SYSCALL_SERVICE_GUI
   BB 03 00 00 00   mov ebx, 3          ; GUI_OP_COMMIT
   B9 01 00 00 00   mov ecx, 1          ; SYSCALL_ABI_VERSION (EDX=Handle unveraendert)
   CD 80            int 0x80
   ; --- SYSCALL_SERVICE_INPUT/INPUT_OP_POLL ---
   B8 04 00 00 00   mov eax, 4          ; SYSCALL_SERVICE_INPUT
   BB 01 00 00 00   mov ebx, 1          ; INPUT_OP_POLL
   B9 01 00 00 00   mov ecx, 1          ; SYSCALL_ABI_VERSION
   BA 20 20 40 00   mov edx, 0x00402020 ; syscall_input_poll_args_t-Adresse
   CD 80            int 0x80
   ; --- bei has_event!=0: Pixel auf Cyan, erneut anhaengen+committen ---
   83 3D 20 20 40 00 00   cmp dword [0x00402020], 0   ; has_event
   74 4C                  je weiter (+0x4C = 76 Byte, ueberspringt den Block)
   C7 05 00 20 40 00 FF FF 00 00   mov dword [0x00402000], 0x0000FFFF (cyan)
   C7 05 04 20 40 00 FF FF 00 00   mov dword [0x00402004], 0x0000FFFF
   C7 05 08 20 40 00 FF FF 00 00   mov dword [0x00402008], 0x0000FFFF
   C7 05 0C 20 40 00 FF FF 00 00   mov dword [0x0040200C], 0x0000FFFF
   89 FA                   mov edx, edi        ; Handle aus Schritt 2, unveraendert
   B8 02 00 00 00          mov eax, 2          ; SYSCALL_SERVICE_GUI
   BB 02 00 00 00          mov ebx, 2          ; GUI_OP_ATTACH_BUFFER
   B9 01 00 00 00          mov ecx, 1          ; SYSCALL_ABI_VERSION
   CD 80                   int 0x80
   B8 02 00 00 00          mov eax, 2          ; SYSCALL_SERVICE_GUI
   BB 03 00 00 00          mov ebx, 3          ; GUI_OP_COMMIT
   B9 01 00 00 00          mov ecx, 1          ; SYSCALL_ABI_VERSION
   CD 80                   int 0x80
   ; weiter:
   ; --- SYSCALL_CORE_EXIT ---
   B8 01 00 00 00   mov eax, 1          ; SYSCALL_SERVICE_CORE
   BB 01 00 00 00   mov ebx, 1          ; SYSCALL_CORE_EXIT
   CD 80            int 0x80
   EB FE            jmp $ (Sicherheitsnetz) */
static const uint8_t ring3_program[] = {
    0xB8, 0x01, 0x00, 0x00, 0x00,
    0xBB, 0x02, 0x00, 0x00, 0x00,
    0xB9, 0x01, 0x00, 0x00, 0x00,
    0x31, 0xD2,
    0x31, 0xF6,
    0xCD, 0x80,
    /* GUI_OP_CREATE_SURFACE */
    0xB8, 0x02, 0x00, 0x00, 0x00,
    0xBB, 0x01, 0x00, 0x00, 0x00,
    0xB9, 0x01, 0x00, 0x00, 0x00,
    0xBA, 0x14, 0x00, 0x32, 0x00,
    0xBE, 0x10, 0x00, 0x04, 0x00,
    0xCD, 0x80,
    /* Pixelinhalt in die GUI-Pufferseite schreiben */
    0xC7, 0x05, 0x00, 0x20, 0x40, 0x00, 0x00, 0x00, 0xFF, 0x00,
    0xC7, 0x05, 0x04, 0x20, 0x40, 0x00, 0x00, 0xFF, 0x00, 0x00,
    0xC7, 0x05, 0x08, 0x20, 0x40, 0x00, 0xFF, 0x00, 0x00, 0x00,
    0xC7, 0x05, 0x0C, 0x20, 0x40, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    /* GUI_OP_ATTACH_BUFFER */
    0x89, 0xFA,
    0xB8, 0x02, 0x00, 0x00, 0x00,
    0xBB, 0x02, 0x00, 0x00, 0x00,
    0xB9, 0x01, 0x00, 0x00, 0x00,
    0xCD, 0x80,
    /* GUI_OP_COMMIT */
    0xB8, 0x02, 0x00, 0x00, 0x00,
    0xBB, 0x03, 0x00, 0x00, 0x00,
    0xB9, 0x01, 0x00, 0x00, 0x00,
    0xCD, 0x80,
    /* SYSCALL_SERVICE_INPUT / INPUT_OP_POLL */
    0xB8, 0x04, 0x00, 0x00, 0x00,
    0xBB, 0x01, 0x00, 0x00, 0x00,
    0xB9, 0x01, 0x00, 0x00, 0x00,
    0xBA, 0x20, 0x20, 0x40, 0x00,
    0xCD, 0x80,
    /* bei has_event!=0: Pixel auf Cyan, erneut anhaengen+committen */
    0x83, 0x3D, 0x20, 0x20, 0x40, 0x00, 0x00,
    0x74, 0x4C,
    0xC7, 0x05, 0x00, 0x20, 0x40, 0x00, 0xFF, 0xFF, 0x00, 0x00,
    0xC7, 0x05, 0x04, 0x20, 0x40, 0x00, 0xFF, 0xFF, 0x00, 0x00,
    0xC7, 0x05, 0x08, 0x20, 0x40, 0x00, 0xFF, 0xFF, 0x00, 0x00,
    0xC7, 0x05, 0x0C, 0x20, 0x40, 0x00, 0xFF, 0xFF, 0x00, 0x00,
    0x89, 0xFA,
    0xB8, 0x02, 0x00, 0x00, 0x00,
    0xBB, 0x02, 0x00, 0x00, 0x00,
    0xB9, 0x01, 0x00, 0x00, 0x00,
    0xCD, 0x80,
    0xB8, 0x02, 0x00, 0x00, 0x00,
    0xBB, 0x03, 0x00, 0x00, 0x00,
    0xB9, 0x01, 0x00, 0x00, 0x00,
    0xCD, 0x80,
    /* SYSCALL_CORE_EXIT */
    0xB8, 0x01, 0x00, 0x00, 0x00,
    0xBB, 0x01, 0x00, 0x00, 0x00,
    0xCD, 0x80,
    0xEB, 0xFE,
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

    uint32_t gui_buffer_page = pmm_alloc_page(&numa);
    if (gui_buffer_page == 0) return -1;
    uint8_t *gui_buffer_dst = (uint8_t *)gui_buffer_page;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE; ++i) gui_buffer_dst[i] = 0;

    if (paging_map_page(RING3_GUI_BUFFER_VADDR, gui_buffer_page,
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
