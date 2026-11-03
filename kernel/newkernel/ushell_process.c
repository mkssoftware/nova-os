/* NovaOS C-Kernel - Userspace-Shell-Prozess-Lader (siehe
   ushell_process.h fuer Scope/Architektur). Spawnt drei unabhaengige
   Ring-3-Prozesse (Taskbar, Startmenue, Desktop-Icons) ueber eine
   gemeinsame, generische ushell_spawn()-Hilfsfunktion - alle drei haben
   ihren eigenen Adressraum, eigenen Scheduler-Slot (eigenem Kernel-Stack,
   siehe ring3_set_kernel_stack) und eigenen GUI-Puffer. Taskbar und
   Startmenue koordinieren sich ueber das winzige Sichtbarkeits-Flag in
   syscall.c (siehe dort) - die Icons sind rein statisch und brauchen
   keine laufende Koordination mit den anderen beiden. */
#include "ushell_process.h"
#include "ushell_blob.h"
#include "startmenu_blob.h"
#include "icons_blob.h"
#include "pmm.h"
#include "paging.h"
#include "process.h"
#include "scheduler.h"
#include "ring3.h"
#include "display.h"
#include "desktop_shell.h"
#include "syscall.h"

/* Layout ALLER DREI Prozesse - MUSS mit ushell/shell_main.c,
   ushell/startmenu_main.c UND ushell/icons_main.c uebereinstimmen (dort
   als eigenstaendige Konstanten dupliziert, da dieser Baum unabhaengig
   vom Kernel kompiliert wird). Alle drei Prozesse nutzen DIESELBEN
   virtuellen Adressen - kein Konflikt, da jeder Prozess seit
   paging_create_address_space() einen eigenen Adressraum hat. */
#define USHELL_CODE_VADDR  0x00500000u
/* 64 KiB, deckt .text+.rdata+.data+.bss mit Marge fuer ALLE drei
   Prozesse ab (gemeinsame Konstante, siehe ushell_spawn). Der groesste
   bisherige Verwender ist icons_main.c (svgicon.c mitkompiliert) mit
   56320 Bytes realer Blob-Groesse (14 Seiten) - mingw/PE legt bei
   -fdata-sections uninitialisierte Statics ($-Subsection ".data$name")
   als ECHTE Nullbytes statt platzsparendem .bss ab (siehe icons_main.c-
   Kommentar zu icons-link.ld), daher ist der reale Platzbedarf hier
   hoeher als die reine Codegroesse vermuten liesse. */
#define USHELL_CODE_PAGES  16u
/* bewusst NACH dem Code-Bereich (endet bei USHELL_CODE_VADDR +
   USHELL_CODE_PAGES*4096 = 0x510000), keine Ueberlappung. */
#define USHELL_STACK_VADDR 0x00512000u
#define USHELL_STACK_TOP   0x00512FF0u
#define USHELL_ARGS_VADDR  0x00400000u /* Bootstrap-Userspace-Fenster, siehe syscall.h */
/* Dieselbe feste virtuelle Adresse wie ring3.c's Demoprozess (RING3_GUI_
   BUFFER_VADDR, siehe ring3.h) - syscall.c's GUI_OP_ATTACH_BUFFER loest
   genau DIESE eine, fest verdrahtete virtuelle Adresse auf. */
#define USHELL_GUI_BUFFER_VADDR RING3_GUI_BUFFER_VADDR

#define STARTMENU_WIDTH  160u
#define STARTMENU_ITEM_H 20u
#define STARTMENU_ENTRY_COUNT 4u
#define STARTMENU_HEIGHT ((STARTMENU_ENTRY_COUNT + 1u) * STARTMENU_ITEM_H)

typedef struct {
    uint32_t pid;
    uint32_t scheduler_slot;
    uint32_t directory_phys;
    uint32_t gui_buffer_pages;
    const uint8_t *blob;
    uint32_t blob_size;
    uint32_t entry_offset;
} ushell_instance_t;

static ushell_instance_t taskbar_instance;
static ushell_instance_t startmenu_instance;
static ushell_instance_t icons_instance;

#define ICONS_COUNT       2u
#define ICONS_CELL_W      64u
#define ICONS_CELL_H      56u
#define ICONS_GRID_WIDTH  (ICONS_COUNT * ICONS_CELL_W)
#define ICONS_GRID_HEIGHT ICONS_CELL_H

static void ushell_instance_reset(ushell_instance_t *inst, const uint8_t *blob,
                                   uint32_t blob_size, uint32_t entry_offset) {
    inst->pid = 0;
    inst->scheduler_slot = 0xFFFFFFFFu;
    inst->directory_phys = 0;
    inst->gui_buffer_pages = 0;
    inst->blob = blob;
    inst->blob_size = blob_size;
    inst->entry_offset = entry_offset;
}

/* Baut einen vollstaendigen, eigenstaendigen Ring-3-Prozess aus einem
   kompilierten Blob auf: eigener Adressraum, Code/Stack/Args/GUI-Puffer
   gemappt, Prozess registriert, ECHTER praeemptiv geplanter Scheduler-
   Slot reserviert. gui_buffer_bytes bestimmt die Groesse des (physisch
   zusammenhaengenden, siehe pmm_alloc_contiguous) GUI-Puffers - jeder
   Prozess braucht hier eine fuer sein eigenes Fenster passende Groesse. */
static int ushell_spawn(ushell_instance_t *inst, uint32_t gui_buffer_bytes) {
    if (inst->blob_size > USHELL_CODE_PAGES * PMM_PAGE_SIZE) return -1;

    uint32_t dir_phys;
    if (paging_create_address_space(&dir_phys) != 0) return -1;

    for (uint32_t i = 0; i < USHELL_CODE_PAGES; ++i) {
        uint32_t numa;
        uint32_t page = pmm_alloc_page(&numa);
        if (page == 0) return -1;
        uint8_t *dst = (uint8_t *)(uintptr_t)page;
        uint32_t blob_offset = i * PMM_PAGE_SIZE;
        for (uint32_t b = 0; b < PMM_PAGE_SIZE; ++b) {
            uint32_t src_index = blob_offset + b;
            dst[b] = (src_index < inst->blob_size) ? inst->blob[src_index] : 0u;
        }
        /* PAGING_PAGE_WRITE noetig (nicht nur PRESENT|USER): der Compiler
           legt .text/.rdata UND .data/.bss desselben Blobs gemeinsam in
           diesen Code-Seiten ab (siehe ushell-link.ld/startmenu-link.ld/
           icons-link.ld - EIN gemeinsamer Adressbereich, keine separate
           .data-Seite). icons_main.c ist der erste Verwender mit einer
           tatsaechlich zur Laufzeit BESCHRIEBENEN statischen Variable in
           diesem Bereich (icon_bitmap, von svgicon_render() gefuellt) -
           Taskbar/Startmenue hatten dort bislang nur Code+konstante
           Daten, daher blieb diese fehlende Schreibrechte-Vergabe bisher
           folgenlos. Ohne dieses Flag faultet der erste Schreibzugriff
           (Ring-3-Seitenfehler auf eine PRESENT-aber-nicht-WRITE-Seite) -
           real durch einen Boot-Test mit dem Icons-Prozess aufgedeckt. */
        if (paging_map_page_into(dir_phys, USHELL_CODE_VADDR + blob_offset, page,
                                  PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE | PAGING_PAGE_USER) != 0) {
            return -1;
        }
    }

    uint32_t numa;
    uint32_t stack_page = pmm_alloc_page(&numa);
    if (stack_page == 0) return -1;
    uint8_t *stack_dst = (uint8_t *)(uintptr_t)stack_page;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE; ++i) stack_dst[i] = 0;
    if (paging_map_page_into(dir_phys, USHELL_STACK_VADDR, stack_page,
                              PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE | PAGING_PAGE_USER) != 0) {
        return -1;
    }

    uint32_t args_page = pmm_alloc_page(&numa);
    if (args_page == 0) return -1;
    uint8_t *args_dst = (uint8_t *)(uintptr_t)args_page;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE; ++i) args_dst[i] = 0;
    if (paging_map_page_into(dir_phys, USHELL_ARGS_VADDR, args_page,
                              PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE | PAGING_PAGE_USER) != 0) {
        return -1;
    }

    uint32_t buffer_pages = (gui_buffer_bytes + PMM_PAGE_SIZE - 1u) / PMM_PAGE_SIZE;
    uint32_t buffer_first_page;
    if (pmm_alloc_contiguous(buffer_pages, &buffer_first_page) != 0) return -1;
    uint8_t *buffer_dst = (uint8_t *)(uintptr_t)buffer_first_page;
    for (uint32_t i = 0; i < buffer_pages * PMM_PAGE_SIZE; ++i) buffer_dst[i] = 0;
    for (uint32_t i = 0; i < buffer_pages; ++i) {
        if (paging_map_page_into(dir_phys, USHELL_GUI_BUFFER_VADDR + i * PMM_PAGE_SIZE,
                                  buffer_first_page + i * PMM_PAGE_SIZE,
                                  PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE | PAGING_PAGE_USER) != 0) {
            return -1;
        }
    }
    inst->gui_buffer_pages = buffer_pages;

    /* Eigener Kernel-Stack (ESP0) fuer diesen Slot (siehe ring3.h/
       scheduler.h fuer die Begruendung: zwei gleichzeitig laufende
       Ring-3-Prozesse duerfen sich NICHT denselben Interrupt-Landeplatz
       teilen). Liegt innerhalb der Identity-Map (wie jede PMM-Seite),
       daher OHNE eigenes paging_map_page_into() direkt per physischer
       Adresse nutzbar - genau wie der GUI-Puffer. Braucht keine eigene
       Prozess-Mapping, da er NUR bei CPL=0 (Interrupt-Handler-Kontext)
       verwendet wird. */
    uint32_t kstack_numa;
    uint32_t kstack_page = pmm_alloc_page(&kstack_numa);
    if (kstack_page == 0) return -1;
    uint8_t *kstack_dst = (uint8_t *)(uintptr_t)kstack_page;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE; ++i) kstack_dst[i] = 0;
    uint32_t kernel_stack_top = kstack_page + PMM_PAGE_SIZE - 16u;

    uint32_t pid = process_create(0x4 /* PROCESS_FLAG_USERSPACE, siehe ring3.c */, dir_phys);
    if (pid == 0) return -1;

    uint32_t slot;
    if (scheduler_claim_dynamic_user_slot(USHELL_CODE_VADDR + inst->entry_offset, USHELL_STACK_TOP,
                                           RING3_CODE_SELECTOR, RING3_DATA_SELECTOR,
                                           -1, dir_phys, kernel_stack_top, &slot) != 0) {
        return -1;
    }

    inst->pid = pid;
    inst->scheduler_slot = slot;
    inst->directory_phys = dir_phys;
    return 0;
}

static int ushell_instance_self_test(const ushell_instance_t *inst) {
    if (inst->pid == 0) return -1;
    if (inst->scheduler_slot == 0xFFFFFFFFu) return -1;
    if (inst->directory_phys == 0) return -1;
    if (process_get_cr3(inst->pid) != inst->directory_phys) return -1;
    if (scheduler_slot_directory(inst->scheduler_slot) != inst->directory_phys) return -1;
    if (scheduler_context_at(inst->scheduler_slot) == 0) return -1;
    if (inst->gui_buffer_pages == 0) return -1;

    /* Code-Inhalt pruefen: kurzzeitig in den neuen Adressraum wechseln
       (sicher - ein Klon des Kernel-Basis-Adressraums plus privater
       Zusatzmappings) und die ersten Bytes an USHELL_CODE_VADDR gegen den
       eingebetteten Blob pruefen. Schaltet in JEDEM Fall zurueck, bevor
       der Rest des Boots fortfaehrt. */
    uint32_t kernel_dir = paging_kernel_directory_physical();
    paging_switch_address_space(inst->directory_phys);
    const uint8_t *code = (const uint8_t *)(uintptr_t)USHELL_CODE_VADDR;
    int mismatch = 0;
    uint32_t check_len = (inst->blob_size < 64u) ? inst->blob_size : 64u;
    for (uint32_t i = 0; i < check_len; ++i) {
        if (code[i] != inst->blob[i]) { mismatch = 1; break; }
    }
    paging_switch_address_space(kernel_dir);
    if (mismatch) return -1;

    return 0;
}

int ushell_process_initialize(void) {
    ushell_instance_reset(&taskbar_instance, ushell_blob, USHELL_BLOB_SIZE, USHELL_ENTRY_OFFSET);
    ushell_instance_reset(&startmenu_instance, startmenu_blob, STARTMENU_BLOB_SIZE, STARTMENU_ENTRY_OFFSET);
    ushell_instance_reset(&icons_instance, icons_blob, ICONS_BLOB_SIZE, ICONS_ENTRY_OFFSET);

    uint32_t width = display_width();
    uint32_t height = display_height();
    if (width == 0u || height <= TASKBAR_HEIGHT) return -1;

    uint32_t taskbar_buffer_bytes = width * TASKBAR_HEIGHT * 4u;
    if (ushell_spawn(&taskbar_instance, taskbar_buffer_bytes) != 0) return -1;

    uint32_t startmenu_buffer_bytes = STARTMENU_WIDTH * STARTMENU_HEIGHT * 4u;
    if (ushell_spawn(&startmenu_instance, startmenu_buffer_bytes) != 0) return -1;

    /* NPSPEC-SECURITY-CAPABILITY-0001: NUR der Startmenue-Prozess (der den
       Shutdown-Button zeichnet, siehe ushell/startmenu_main.c) darf
       SYSCALL_CORE_SHUTDOWN tatsaechlich ausloesen - erste echte Laufzeit-
       Vergabe des bis dahin nie ausserhalb seiner eigenen Selbsttests
       genutzten §103 Capability Frameworks. */
    if (syscall_grant_shutdown_capability(startmenu_instance.pid) != 0) return -1;

    uint32_t icons_buffer_bytes = ICONS_GRID_WIDTH * ICONS_GRID_HEIGHT * 4u;
    if (ushell_spawn(&icons_instance, icons_buffer_bytes) != 0) return -1;

    return 0;
}

int ushell_process_self_test(void) {
    if (ushell_instance_self_test(&taskbar_instance) != 0) return -1;
    if (ushell_instance_self_test(&startmenu_instance) != 0) return -1;
    if (ushell_instance_self_test(&icons_instance) != 0) return -1;
    return 0;
}
