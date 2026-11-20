/* NovaOS C-Kernel - Nova Executable Format (NEF) 1.0 - Nativer Loader
   (Phase 4 des NEF-Masterauftrags).

   Baut auf nef.h/nef.c (Phase 2, reine Formatvalidierung) auf und fuehrt
   eine bereits strukturell gueltige NEF-Datei tatsaechlich aus: Oeffnen
   ueber vfsapi.h, vollstaendiges Einlesen, Validierung (nef_validate_
   header), Versions-/Architektur-/ABI-Pruefung, eigener Hardware-
   Adressraum (paging_create_address_space), Section-fuer-Section-
   Abbildung inkl. Nullfuellung des BSS-Wachstums (vsize > file_size),
   Stack-Aufbau, echter praeemptiv geplanter Scheduler-Slot (scheduler_
   claim_dynamic_user_slot, dieselbe Infrastruktur wie ushell_process.c)
   und kontrollierter Rueckgabewert bei jedem Fehlerpfad.

   Bewusste Scope-Grenze dieser Phase (ehrlich, keine vorgetaeuschte
   Unterstuetzung - siehe Masterauftrag "keine Dummy-Implementierungen"):
   IMPORTS/EXPORTS/RELOCATIONS/TLS-Sections mit Inhalt (file_size>0 ODER
   vsize>0) werden MIT EINEM EIGENEN FEHLERCODE abgelehnt, NICHT
   stillschweigend uebersprungen oder als Erfolg vorgetaeuscht. Grund:
   nef.h definiert fuer diese vier Sections bisher nur den rohen
   Section-Typ, aber KEIN Eintragsformat (kein Symbol-/Relokations-
   Record-Layout, keine Modul-Registry fuer Importaufloesung ueber
   mehrere geladene NEF-Images hinweg) - das zu entwerfen ist eine
   eigene, groessere Design-Entscheidung (Masterauftrag Abschnitt 4+9:
   dynamisches Linken/TLS), bewusst NICHT in dieser Phase erfunden,
   sondern einer spaeteren Phase vorbehalten. Dieser Loader fuehrt daher
   vorerst ausschliesslich vollstaendig statisch gelinkte, direkt
   ausfuehrbare NEF-Images aus (CODE/RODATA/DATA/BSS) - exakt das, was
   nef_main.c's `build`/`link`-Unterbefehle heute erzeugen koennen.

   Architektur-Grenze (wie bei nef.c/ushell_process.c dokumentiert):
   NEF_SEC_FLAG_EXEC steuert NUR, ob nef_validate_section() eine Section
   als W^X-Verstoss ablehnt - dieser 32-Bit-Non-PAE-Kernel hat kein NX-Bit
   (siehe paging.h), kann also "nicht ausfuehrbar" fuer eine Datenseite
   NIE auf Hardware-Ebene erzwingen. Dieser Loader mappt Sections ohne
   NEF_SEC_FLAG_WRITE daher konsequent OHNE PAGING_PAGE_WRITE (das IST
   durchsetzbar und wird durchgesetzt), gibt sich aber nicht faelschlich
   als vollstaendige W^X-Hardware-Durchsetzung aus. */
#ifndef NOVA_NEF_LOADER_H
#define NOVA_NEF_LOADER_H

#include <stdint.h>

/* Obergrenze fuer die Dateigroesse einer ladbaren NEF-Datei (Bootstrap-
   Grenze, analog zu USHELL_CODE_PAGES - dieser Kernel hat keine
   Demand-Paging-Unterstuetzung fuer Programmdateien, die komplette Datei
   wird per kalloc() eingelesen, bevor irgendetwas gemappt wird). */
#define NEF_LOADER_MAX_FILE_SIZE 65536u

/* Obergrenze fuer die Anzahl Sections EINES Images - begrenzt die
   statische Section-Zwischentabelle in nef_loader.c. */
#define NEF_LOADER_MAX_SECTIONS 16u

/* Obergrenze fuer die Gesamtzahl physischer Seiten (alle Sections +
   Stack zusammen), die EIN nef_load_and_spawn()-Aufruf belegen darf -
   schuetzt den ohnehin knappen Bootstrap-PMM-Pool (siehe pmm.h,
   project_pmm_capacity_increase.md) vor einem einzelnen, zu grossen
   Image. */
#define NEF_LOADER_MAX_PAGES 64u

/* Groesse (Bytes) des User-Stacks, den der Loader fuer jeden gestarteten
   Prozess anlegt - eine Seite, wie bei ushell_process.c's Prozessen. */
#define NEF_LOADER_STACK_BYTES 4096u

#define NEF_LOAD_OK                       0
#define NEF_LOAD_ERR_OPEN                (-1)  /* vfsapi_lookup fehlgeschlagen */
#define NEF_LOAD_ERR_QUERY                (-2)  /* vfsapi_query fehlgeschlagen */
#define NEF_LOAD_ERR_TOO_LARGE            (-3)  /* Datei > NEF_LOADER_MAX_FILE_SIZE */
#define NEF_LOAD_ERR_READ                 (-4)  /* vfsapi_read fehlgeschlagen/unvollstaendig */
#define NEF_LOAD_ERR_OOM                  (-5)  /* kalloc() fuer den Datei-Puffer fehlgeschlagen */
#define NEF_LOAD_ERR_VALIDATE             (-6)  /* nef_validate_header lieferte != 0, siehe out_validate_err */
#define NEF_LOAD_ERR_VERSION              (-7)  /* nef_header_version_supported() == 0 */
#define NEF_LOAD_ERR_NOT_EXECUTABLE       (-8)  /* NEF_FLAG_EXECUTABLE fehlt */
#define NEF_LOAD_ERR_ARCH                 (-9)  /* target_arch != NEF_ARCH_X86_32 */
#define NEF_LOAD_ERR_ABI                  (-10) /* abi_id != NEF_ABI_NOVA_NATIVE */
#define NEF_LOAD_ERR_TOO_MANY_SECTIONS    (-11) /* section_count > NEF_LOADER_MAX_SECTIONS */
#define NEF_LOAD_ERR_UNSUPPORTED_DYNAMIC  (-12) /* nichtleere IMPORTS/EXPORTS/RELOCATIONS/TLS-Section */
#define NEF_LOAD_ERR_EXTENSION            (-13) /* unbekannte REQUIRED-Extension */
#define NEF_LOAD_ERR_IMAGE_TOO_LARGE      (-14) /* Gesamt-Seitenbedarf > NEF_LOADER_MAX_PAGES */
#define NEF_LOAD_ERR_ADDRESS_SPACE        (-15) /* paging_create_address_space fehlgeschlagen */
#define NEF_LOAD_ERR_NO_MEMORY            (-16) /* pmm_alloc_page waehrend des Mappens erschoepft */
#define NEF_LOAD_ERR_MAP                  (-17) /* paging_map_page_into fehlgeschlagen */
#define NEF_LOAD_ERR_PROCESS              (-18) /* process_create fehlgeschlagen (Tabelle voll) */
#define NEF_LOAD_ERR_SCHEDULER            (-19) /* scheduler_claim_dynamic_user_slot fehlgeschlagen */

/* Beschreibt einen per nef_load_and_spawn() gestarteten Prozess - genug
   Zustand, um ihn spaeter VOLLSTAENDIG wieder abzubauen (nef_unload()).
   Masterauftrag Abschnitt 7 fordert ausdruecklich echte Ressourcen-
   bereinigung bei Programmende, nicht nur bei einem Lade-Fehler - ohne
   diese Struktur koennte ein Aufrufer den Scheduler-Slot und die
   gemappten physischen Seiten eines bereits laufenden Prozesses nicht
   mehr identifizieren (process.h's process_lookup() gibt bewusst keinen
   typisierten Zeiger auf interne Datensaetze heraus). */
#define NEF_LOADER_MAX_TRACKED_PAGES (NEF_LOADER_MAX_PAGES + 2u) /* Sections+Stack + eigener Kernel-Stack */

typedef struct {
    uint32_t pid;
    uint32_t directory_phys;
    uint32_t scheduler_slot;    /* 0xFFFFFFFFu = kein Slot belegt */
    uint32_t pages[NEF_LOADER_MAX_TRACKED_PAGES];
    uint32_t page_count;
} nef_process_t;

/* Laedt die NEF-Datei unter path (path_len Byte, KEIN Nullterminator)
   und spawnt sie als neuen, eigenstaendigen Ring-3-Prozess mit echtem
   Scheduler-Slot (laeuft automatisch mit, sobald scheduler_enable()
   aktiv ist - identischer Mechanismus wie ushell_process.c). loader_pid
   ist die PID, unter der die vfsapi.h-Dateizugriffe erfolgen (braucht
   SECURITY_CAP_FS_READ, siehe security.h) - fuer Boot-Selbsttests/
   kernelinitiiertes Laden ist das PID 1 (Kernel, hat SECURITY_KERNEL_
   CAPS). 0 = Erfolg (*out_proc vollstaendig gefuellt), sonst ein
   NEF_LOAD_ERR_*-Code (*out_err, falls out_err != 0) - *out_validate_err
   erhaelt zusaetzlich den genauen NEF_VALIDATE_ERR_*-Code, wenn der
   Fehler NEF_LOAD_ERR_VALIDATE war (sonst unveraendert; dieser Parameter
   darf NULL sein). Bei JEDEM Fehler ab dem Oeffnen der Datei werden alle
   in diesem Aufruf bereits zugeteilten Ressourcen (Datei-Puffer,
   physische Seiten, Prozess, Scheduler-Slot) wieder freigegeben - siehe
   ushell_process.c fuer dieselbe Disziplin und deren eine bewusst
   verbleibende Ausnahme (das Page-Directory selbst kann nicht
   zurueckgegeben werden, paging.h bietet dafuer keine Gegenfunktion -
   nef_unload() unten erbt dieselbe Grenze). */
int nef_load_and_spawn(uint32_t loader_pid, const char *path, uint32_t path_len,
                        nef_process_t *out_proc, int *out_err, int *out_validate_err);

/* Programmende-Ressourcenbereinigung (Masterauftrag Abschnitt 7): beendet
   proc->pid (process_terminate+process_reap, NPSPEC-PROZESS-LIFECYCLE-
   0001), gibt proc->scheduler_slot frei (scheduler_release_dynamic_slot)
   und jede von proc->pages[] gehaltene physische Seite zurueck
   (pmm_free_page). proc wird danach auf den Leerzustand zurueckgesetzt.
   NICHT zurueckgegeben wird proc->directory_phys selbst (siehe
   nef_load_and_spawn()-Kommentar) - niemand schaltet nach diesem Aufruf
   je wieder per CR3 darauf um, die verbleibenden Stale-Mappings sind
   harmlos. Sicher mit proc==0 oder proc->pid==0 (No-Op). */
void nef_unload(nef_process_t *proc);

/* Baut zur Laufzeit ein winziges, eingebettetes Testprogramm (eine
   CODE-Section, kein Syscall-Bedarf) als echte NEF-Datei unter
   "/nef_loader_selftest.bin" (NovaFS, per vfsapi.h), laedt+spawnt sie
   ECHT ueber nef_load_and_spawn(), prueft Prozess/Adressraum/Scheduler-
   Slot/abgebildeten Code-Inhalt, terminiert+reapt den Testprozess
   wieder (NPSPEC-PROZESS-LIFECYCLE-0001) und loescht die Testdatei -
   hinterlaesst beim Boot KEINE dauerhaft belegten Ressourcen (wichtig:
   laeuft laut kmain.c VOR ushell_process_initialize(), das ohnehin
   schon knapp am Bootstrap-PMM-Limit liegt, siehe project_pmm_capacity_
   increase.md). 0 = Erfolg. */
int nef_loader_self_test(void);

#endif
