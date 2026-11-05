/* NovaOS C-Kernel - Userspace-Shell-Prozess-Lader (siehe
   ushell_process.h fuer Scope/Architektur). Spawnt drei unabhaengige
   Ring-3-Prozesse (Taskbar, Startmenue, Desktop-Icons) ueber eine
   gemeinsame, generische ushell_spawn()-Hilfsfunktion - alle drei haben
   ihren eigenen Adressraum, eigenen Scheduler-Slot (eigenem Kernel-Stack,
   siehe ring3_set_kernel_stack) und eigenen GUI-Puffer. Taskbar und
   Startmenue koordinieren sich ueber das winzige Sichtbarkeits-Flag in
   syscall.c (siehe dort) - die Icons sind rein statisch und brauchen
   keine laufende Koordination mit den anderen beiden.

   W^X-Trennung (NPSPEC-SECURITY-0001-artige Haertung, Code-Review-Fund
   2026-11-04): Code-Seiten (.text+.rdata) wurden bisher MIT
   PAGING_PAGE_WRITE gemappt, weil Compiler/Linker .text/.rdata/.data/.bss
   desselben Blobs in EINEM gemeinsamen Adressbereich ablegten - ein
   Ring-3-Prozess konnte damit seinen eigenen ausfuehrbaren Code
   ueberschreiben. Fix: `ushell-link.ld`/`startmenu-link.ld`/
   `icons-link.ld` richten den ersten schreibbaren Abschnitt (.data) jetzt
   per `. = ALIGN(4096);` auf eine Seitengrenze aus (Marker-Symbol
   `__rw_start`), und die zugehoerigen Blob-Header (`ushell_blob.h`/
   `startmenu_blob.h`) exportieren die daraus resultierende Groesse als
   `USHELL_RO_SIZE`/`STARTMENU_RO_SIZE` - `ushell_spawn()` mappt Seiten
   unterhalb dieser Grenze OHNE `PAGING_PAGE_WRITE`. NX/Nichtausfuehrbarkeit
   fuer Datenseiten ist mit der aktuellen 32-Bit-Non-PAE-Paging-
   Implementierung (siehe paging.h - nur PRESENT/WRITE/USER, kein NX-Bit,
   das braucht PAE oder Long Mode) technisch NICHT umsetzbar - bewusst
   NICHT Teil dieser Aenderung, siehe NPSPEC-Empfehlung "auf geeigneter
   Hardware spaeter durchsetzen".

   AUSNAHME Desktop-Icons: beim Neulinken fuer diese Aenderung stellte sich
   heraus, dass der aktuelle `icons_main.c`/`svgicon.c`-Quellstand (der
   SVG-Icon-Cache, `SVGICON_CACHE_SLOTS * svgicon_bitmap_t`) inzwischen
   einen weit groesseren .data/.bss-Fussabdruck hat, als in das bestehende,
   gegen `USHELL_CODE_PAGES` (64 KiB) bemessene Budget passt - der
   ausgelieferte `icons_blob.h` ist daher vermutlich bereits aelter als der
   aktuelle Quellstand. Eine Neuerzeugung dieses einen Blobs wuerde zudem
   das Seiten-Budget sprengen und braucht eine eigene Entscheidung (Cache
   verkleinern oder Budget erhoehen) - bewusst NICHT Teil dieser Aenderung.
   Icons bleiben daher vorerst wie bisher vollstaendig beschreibbar
   (ICONS_RO_SIZE=0 unten) - kein Rueckschritt, nur (noch) keine
   Verbesserung fuer diesen einen Prozess.

   Ressourcen-Aufraeumung bei Fehlschlag (zweiter Code-Review-Fund):
   `ushell_spawn()` gab bei einem Fehler in einem spaeteren Schritt
   zuvor bereits erfolgreich reservierte Ressourcen NICHT frei. Fix:
   ein einzelner `cleanup`-Pfad gibt jede bereits zugeteilte physische
   Seite (Code/Stack/Args/GUI-Puffer/Kernel-Stack, alle ueber
   `pmm_free_page()` - auch der ueber `pmm_alloc_contiguous()` bezogene
   GUI-Puffer besteht aus einzelnen Rahmen und wird daher Seite fuer Seite
   freigegeben) sowie einen ggf. bereits erzeugten Prozess (per
   `process_terminate()`+`process_reap()`, NPSPEC-PROZESS-LIFECYCLE-0001)
   wieder frei. **Bewusste, dokumentierte Restgrenze:** das per
   `paging_create_address_space()` angelegte Page-Directory (und ggf. beim
   Mappen privatisierte Seitentabellen, siehe paging.h's Copy-on-Write-
   Hinweis) kann NICHT zurueckgegeben werden - `paging.h` bietet aktuell
   keine Gegenfunktion dazu (muesste pro Adressraum nachverfolgen, welche
   Seitentabellen privatisiert wurden, um nur diese sicher freizugeben,
   ohne vom Kernel-Basis-Adressraum geerbte, weiterhin geteilte Tabellen
   zu beschaedigen) - das waere eine eigene, groessere Erweiterung von
   paging.c und bewusst NICHT Teil dieser Aenderung (deckungsgleich mit
   der Nutzer-Einschaetzung: fuer einen Bootstrap-Prototyp hinnehmbar,
   fuer eine allgemeine Prozessverwaltung spaeter nachzuruesten). Die
   verbleibenden Stale-Mappings in diesem einen, dauerhaft verwaisten
   Directory sind harmlos: niemand schaltet je wieder per CR3 darauf um,
   und die zugehoerigen physischen Rahmen der einzelnen Code-/Daten-Seiten
   selbst werden (siehe oben) korrekt zurueckgegeben. */
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
   Prozess braucht hier eine fuer sein eigenes Fenster passende Groesse.
   ro_size ist die Groesse (Bytes, Vielfaches von 4096) des schreib-
   geschuetzten Code-Praefixes dieses Blobs (USHELL_RO_SIZE/
   STARTMENU_RO_SIZE, 0 = vollstaendig beschreibbar wie bisher - siehe
   Dateikopf, aktuell fuer Icons). Bei JEDEM Fehler ab hier werden alle in
   DIESEM Aufruf bereits zugeteilten physischen Seiten und ein ggf. schon
   erzeugter Prozess wieder freigegeben (siehe Dateikopf fuer die eine
   bewusst verbleibende Ausnahme: das Page-Directory selbst). */
static int ushell_spawn(ushell_instance_t *inst, uint32_t gui_buffer_bytes, uint32_t ro_size) {
    if (inst->blob_size > USHELL_CODE_PAGES * PMM_PAGE_SIZE) return -1;

    uint32_t code_pages[USHELL_CODE_PAGES];
    uint32_t code_pages_count = 0;
    uint32_t stack_page = 0;
    uint32_t args_page = 0;
    uint32_t buffer_first_page = 0;
    uint32_t buffer_pages = 0;
    uint32_t kstack_page = 0;
    uint32_t pid = 0;

    uint32_t dir_phys;
    if (paging_create_address_space(&dir_phys) != 0) return -1;

    for (uint32_t i = 0; i < USHELL_CODE_PAGES; ++i) {
        uint32_t numa;
        uint32_t page = pmm_alloc_page(&numa);
        if (page == 0) goto cleanup;
        code_pages[i] = page;
        code_pages_count = i + 1u;

        uint8_t *dst = (uint8_t *)(uintptr_t)page;
        uint32_t blob_offset = i * PMM_PAGE_SIZE;
        for (uint32_t b = 0; b < PMM_PAGE_SIZE; ++b) {
            uint32_t src_index = blob_offset + b;
            dst[b] = (src_index < inst->blob_size) ? inst->blob[src_index] : 0u;
        }
        /* W^X-Trennung (siehe Dateikopf): Seiten unterhalb ro_size sind
           .text/.rdata - PAGING_PAGE_WRITE bewusst WEGLASSEN. Seiten ab
           ro_size (.data/.bss, oder bei ro_size==0 der komplette Bereich
           wie bisher bei Icons) bleiben beschreibbar. */
        uint32_t page_flags = PAGING_PAGE_PRESENT | PAGING_PAGE_USER;
        if (blob_offset >= ro_size) page_flags |= PAGING_PAGE_WRITE;
        if (paging_map_page_into(dir_phys, USHELL_CODE_VADDR + blob_offset, page, page_flags) != 0) {
            goto cleanup;
        }
    }

    {
        uint32_t numa;
        stack_page = pmm_alloc_page(&numa);
    }
    if (stack_page == 0) goto cleanup;
    {
        uint8_t *stack_dst = (uint8_t *)(uintptr_t)stack_page;
        for (uint32_t i = 0; i < PMM_PAGE_SIZE; ++i) stack_dst[i] = 0;
    }
    if (paging_map_page_into(dir_phys, USHELL_STACK_VADDR, stack_page,
                              PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE | PAGING_PAGE_USER) != 0) {
        goto cleanup;
    }

    {
        uint32_t numa;
        args_page = pmm_alloc_page(&numa);
    }
    if (args_page == 0) goto cleanup;
    {
        uint8_t *args_dst = (uint8_t *)(uintptr_t)args_page;
        for (uint32_t i = 0; i < PMM_PAGE_SIZE; ++i) args_dst[i] = 0;
    }
    if (paging_map_page_into(dir_phys, USHELL_ARGS_VADDR, args_page,
                              PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE | PAGING_PAGE_USER) != 0) {
        goto cleanup;
    }

    buffer_pages = (gui_buffer_bytes + PMM_PAGE_SIZE - 1u) / PMM_PAGE_SIZE;
    if (pmm_alloc_contiguous(buffer_pages, &buffer_first_page) != 0) { buffer_pages = 0; goto cleanup; }
    {
        uint8_t *buffer_dst = (uint8_t *)(uintptr_t)buffer_first_page;
        for (uint32_t i = 0; i < buffer_pages * PMM_PAGE_SIZE; ++i) buffer_dst[i] = 0;
    }
    for (uint32_t i = 0; i < buffer_pages; ++i) {
        if (paging_map_page_into(dir_phys, USHELL_GUI_BUFFER_VADDR + i * PMM_PAGE_SIZE,
                                  buffer_first_page + i * PMM_PAGE_SIZE,
                                  PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE | PAGING_PAGE_USER) != 0) {
            goto cleanup;
        }
    }

    /* Eigener Kernel-Stack (ESP0) fuer diesen Slot (siehe ring3.h/
       scheduler.h fuer die Begruendung: zwei gleichzeitig laufende
       Ring-3-Prozesse duerfen sich NICHT denselben Interrupt-Landeplatz
       teilen). Liegt innerhalb der Identity-Map (wie jede PMM-Seite),
       daher OHNE eigenes paging_map_page_into() direkt per physischer
       Adresse nutzbar - genau wie der GUI-Puffer. Braucht keine eigene
       Prozess-Mapping, da er NUR bei CPL=0 (Interrupt-Handler-Kontext)
       verwendet wird. */
    {
        uint32_t kstack_numa;
        kstack_page = pmm_alloc_page(&kstack_numa);
    }
    if (kstack_page == 0) goto cleanup;
    {
        uint8_t *kstack_dst = (uint8_t *)(uintptr_t)kstack_page;
        for (uint32_t i = 0; i < PMM_PAGE_SIZE; ++i) kstack_dst[i] = 0;
    }
    uint32_t kernel_stack_top = kstack_page + PMM_PAGE_SIZE - 16u;

    pid = process_create(0x4 /* PROCESS_FLAG_USERSPACE, siehe ring3.c */, dir_phys);
    if (pid == 0) goto cleanup;

    {
        uint32_t slot;
        if (scheduler_claim_dynamic_user_slot(USHELL_CODE_VADDR + inst->entry_offset, USHELL_STACK_TOP,
                                               RING3_CODE_SELECTOR, RING3_DATA_SELECTOR,
                                               -1, dir_phys, kernel_stack_top, &slot) != 0) {
            goto cleanup;
        }
        inst->scheduler_slot = slot;
    }

    inst->pid = pid;
    inst->directory_phys = dir_phys;
    inst->gui_buffer_pages = buffer_pages;
    return 0;

cleanup:
    /* Jede bereits zugeteilte physische Seite zurueckgeben (siehe
       Dateikopf: das Page-Directory selbst bleibt die eine bewusste
       Ausnahme). Reihenfolge ist unkritisch - pmm_free_page() nimmt
       Rahmen in beliebiger Reihenfolge zurueck. */
    if (pid != 0) {
        process_terminate(pid, 0xFFFFFFFFu /* Spawn-Fehlschlag */);
        process_reap(pid);
    }
    if (kstack_page != 0) pmm_free_page(kstack_page);
    for (uint32_t i = 0; i < buffer_pages; ++i) {
        if (buffer_first_page != 0) pmm_free_page(buffer_first_page + i * PMM_PAGE_SIZE);
    }
    if (args_page != 0) pmm_free_page(args_page);
    if (stack_page != 0) pmm_free_page(stack_page);
    for (uint32_t i = 0; i < code_pages_count; ++i) pmm_free_page(code_pages[i]);
    return -1;
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
    if (ushell_spawn(&taskbar_instance, taskbar_buffer_bytes, USHELL_RO_SIZE) != 0) return -1;

    uint32_t startmenu_buffer_bytes = STARTMENU_WIDTH * STARTMENU_HEIGHT * 4u;
    if (ushell_spawn(&startmenu_instance, startmenu_buffer_bytes, STARTMENU_RO_SIZE) != 0) return -1;

    /* NPSPEC-SECURITY-CAPABILITY-0001: NUR der Startmenue-Prozess (der den
       Shutdown-Button zeichnet, siehe ushell/startmenu_main.c) darf
       SYSCALL_CORE_SHUTDOWN tatsaechlich ausloesen - erste echte Laufzeit-
       Vergabe des bis dahin nie ausserhalb seiner eigenen Selbsttests
       genutzten §103 Capability Frameworks. */
    if (syscall_grant_shutdown_capability(startmenu_instance.pid) != 0) return -1;

    /* ICONS_RO_SIZE=0 (vollstaendig beschreibbar, wie vor dieser
       Aenderung) - der ausgelieferte icons_blob.h wurde bewusst NICHT
       neu erzeugt, siehe Dateikopf-Hinweis zum SVG-Icon-Cache-
       Fussabdruck. */
    uint32_t icons_buffer_bytes = ICONS_GRID_WIDTH * ICONS_GRID_HEIGHT * 4u;
    if (ushell_spawn(&icons_instance, icons_buffer_bytes, 0u) != 0) return -1;

    return 0;
}

int ushell_process_self_test(void) {
    if (ushell_instance_self_test(&taskbar_instance) != 0) return -1;
    if (ushell_instance_self_test(&startmenu_instance) != 0) return -1;
    if (ushell_instance_self_test(&icons_instance) != 0) return -1;
    return 0;
}
