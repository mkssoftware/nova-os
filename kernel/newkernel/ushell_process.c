/* NovaOS C-Kernel - Userspace-Taskbar-Prozess-Lader (siehe
   ushell_process.h fuer Scope/Architektur). */
#include "ushell_process.h"
#include "ushell_blob.h"
#include "pmm.h"
#include "paging.h"
#include "process.h"
#include "scheduler.h"
#include "ring3.h"
#include "display.h"
#include "desktop_shell.h"

/* Layout dieses Prozesses - MUSS mit ushell/shell_main.c uebereinstimmen
   (dort als eigenstaendige Konstanten dupliziert, da dieser Baum
   unabhaengig vom Kernel kompiliert wird, siehe dortigen Scope-Hinweis). */
#define USHELL_CODE_VADDR  0x00500000u
#define USHELL_CODE_PAGES  4u /* 16 KiB, deckt .text+.rdata+.data+.bss (siehe shell.elf) mit Marge - 0x500000..0x504000 */
#define USHELL_STACK_VADDR 0x00506000u /* bewusst NACH dem Code-Bereich (0x504000), keine Ueberlappung */
#define USHELL_STACK_TOP   0x00506FF0u
#define USHELL_ARGS_VADDR  0x00400000u /* Bootstrap-Userspace-Fenster, siehe syscall.h */
/* Dieselbe feste virtuelle Adresse wie ring3.c's Demoprozess (RING3_GUI_
   BUFFER_VADDR, siehe ring3.h) - KEIN Konflikt, da dieser Prozess einen
   eigenen, von paging_create_address_space() angelegten Adressraum hat
   (dieselbe Zahl meint dort eine andere physische Seite). syscall.c's
   GUI_OP_ATTACH_BUFFER loest genau DIESE eine, fest verdrahtete virtuelle
   Adresse auf - ein anderer Wert wuerde dort in eine falsche (oder gar
   keine) physische Seite aufgeloest, siehe dessen Scope-Hinweis. */
#define USHELL_GUI_BUFFER_VADDR RING3_GUI_BUFFER_VADDR

static uint32_t ushell_pid = 0;
static uint32_t ushell_scheduler_slot = 0xFFFFFFFFu;
static uint32_t ushell_directory_phys = 0;
static uint32_t ushell_gui_buffer_pages = 0;

int ushell_process_initialize(void) {
    ushell_pid = 0;
    ushell_scheduler_slot = 0xFFFFFFFFu;
    ushell_directory_phys = 0;
    ushell_gui_buffer_pages = 0;

    uint32_t width = display_width();
    uint32_t height = display_height();
    if (width == 0u || height <= TASKBAR_HEIGHT) return -1;

    uint32_t dir_phys;
    if (paging_create_address_space(&dir_phys) != 0) return -1;

    /* Code: USHELL_CODE_PAGES einzelne (nicht notwendig zusammenhaengende)
       Seiten - anders als der GUI-Puffer wird Code nie als EIN roher,
       physischer Flachzeiger an einen CR3-unabhaengigen Leser weitergegeben,
       jede Seite wird einzeln ueber ihre eigene virtuelle Adresse
       angesprochen (vom Prozessor selbst, per Paging). */
    if (USHELL_BLOB_SIZE > USHELL_CODE_PAGES * PMM_PAGE_SIZE) return -1;
    for (uint32_t i = 0; i < USHELL_CODE_PAGES; ++i) {
        uint32_t numa;
        uint32_t page = pmm_alloc_page(&numa);
        if (page == 0) return -1;
        uint8_t *dst = (uint8_t *)(uintptr_t)page;
        uint32_t blob_offset = i * PMM_PAGE_SIZE;
        for (uint32_t b = 0; b < PMM_PAGE_SIZE; ++b) {
            uint32_t src_index = blob_offset + b;
            dst[b] = (src_index < USHELL_BLOB_SIZE) ? ushell_blob[src_index] : 0u;
        }
        if (paging_map_page_into(dir_phys, USHELL_CODE_VADDR + blob_offset, page,
                                  PAGING_PAGE_PRESENT | PAGING_PAGE_USER) != 0) {
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

    /* GUI-Puffer: MUSS physisch zusammenhaengend sein (siehe
       pmm_alloc_contiguous in pmm.h) - der Compositor dereferenziert ihn
       spaeter als EINEN rohen, flachen Zeiger (surface_buffer_addr()),
       unabhaengig vom gerade aktiven CR3. */
    uint32_t buffer_bytes = width * TASKBAR_HEIGHT * 4u;
    uint32_t buffer_pages = (buffer_bytes + PMM_PAGE_SIZE - 1u) / PMM_PAGE_SIZE;
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
    ushell_gui_buffer_pages = buffer_pages;

    uint32_t pid = process_create(0x4 /* PROCESS_FLAG_USERSPACE, siehe ring3.c */, dir_phys);
    if (pid == 0) return -1;

    /* Braucht einen eigenen, dauerhaft reservierten Slot (siehe
       SCHEDULER_THREAD_COUNT=9 in scheduler.h - die urspruenglichen 8
       waren zum Zeitpunkt dieses Aufrufs bereits vollstaendig durch
       scheduler_thread1/2, thread_manager_self_test()s eigenen, nie
       freigegebenen Testthread sowie Compositor-/Desktop-Shell-/Input-/
       Readiness-Thread belegt - per echtem Boot-Test gefunden, siehe
       dortigen Kommentar). */
    uint32_t slot;
    if (scheduler_claim_dynamic_user_slot(USHELL_CODE_VADDR + USHELL_ENTRY_OFFSET, USHELL_STACK_TOP,
                                           RING3_CODE_SELECTOR, RING3_DATA_SELECTOR,
                                           -1, dir_phys, &slot) != 0) {
        return -1;
    }

    ushell_pid = pid;
    ushell_scheduler_slot = slot;
    ushell_directory_phys = dir_phys;
    return 0;
}

int ushell_process_self_test(void) {
    if (ushell_pid == 0) return -1;
    if (ushell_scheduler_slot == 0xFFFFFFFFu) return -1;
    if (ushell_directory_phys == 0) return -1;
    if (process_get_cr3(ushell_pid) != ushell_directory_phys) return -1;
    if (scheduler_slot_directory(ushell_scheduler_slot) != ushell_directory_phys) return -1;
    if (scheduler_context_at(ushell_scheduler_slot) == 0) return -1;
    if (ushell_gui_buffer_pages == 0) return -1;

    /* Code-Inhalt pruefen: kurzzeitig in den neuen Adressraum wechseln (das
       ist sicher - der Adressraum ist ein Klon des Kernel-Basis-Adressraums
       plus privater Zusatzmappings, Kernel-Code/-Daten bleiben erreichbar)
       und die ersten Bytes an USHELL_CODE_VADDR gegen den eingebetteten
       Blob pruefen. Schaltet in JEDEM Fall (auch bei einem Fehler mitten-
       drin) zurueck auf den Kernel-Basis-Adressraum, bevor der Rest des
       Boots fortfaehrt. */
    uint32_t kernel_dir = paging_kernel_directory_physical();
    paging_switch_address_space(ushell_directory_phys);
    const uint8_t *code = (const uint8_t *)(uintptr_t)USHELL_CODE_VADDR;
    int mismatch = 0;
    uint32_t check_len = (USHELL_BLOB_SIZE < 64u) ? USHELL_BLOB_SIZE : 64u;
    for (uint32_t i = 0; i < check_len; ++i) {
        if (code[i] != ushell_blob[i]) { mismatch = 1; break; }
    }
    paging_switch_address_space(kernel_dir);
    if (mismatch) return -1;

    return 0;
}
