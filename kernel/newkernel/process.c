/* NovaOS C-Kernel - Kernel Process Manager (Port aus process32.inc).

   Dynamisch wachsende Prozesstabelle (2026-11-05, Antwort auf "viele
   feste Grenzen nicht dynamisch loesbar" - siehe [[project_general_allocator]]
   fuer kalloc.c, die hierfuer gebaute Grundlage). PROCESS_CAPACITY
   (process.h) ist jetzt nur noch die STARTKAPAZITAET, kein hartes
   Maximum mehr: process_create() verdoppelt die Tabelle per
   krealloc() (kalloc.h), sobald kein freier Slot mehr existiert, statt
   0 (Tabelle voll) zurueckzugeben. Erschoepfung ist jetzt ausschliesslich
   echte Systemspeicher-Erschoepfung (PMM leer), nicht mehr ein
   willkuerlicher Compile-Zeit-Wert. security.h's `SECURITY_CAPACITY`
   (eigene, seit laengerem nicht mehr synchron gehaltene Konstante, siehe
   dortigen Kommentar) bleibt bewusst unangetastet - eigene, unabhaengige
   Tabelle, nicht Teil dieser Aenderung. */
#include "process.h"
#include "object.h"
#include "kalloc.h"
#include <stddef.h>

typedef struct {
    uint32_t pid;
    uint32_t state;
    uint32_t cr3;
    uint32_t handle;
    uint32_t parent;
    uint32_t flags;
    uint32_t exit_status;
} process_record_t;

static process_record_t *process_table = NULL;
static uint32_t process_table_capacity = 0;
static uint32_t process_count_value = 0;
static uint32_t process_next_pid = 1;

void process_manager_initialize(void) {
    if (process_table) {
        kfree(process_table);
        process_table = NULL;
    }
    process_table = (process_record_t *)kcalloc(PROCESS_CAPACITY, sizeof(process_record_t));
    process_table_capacity = process_table ? PROCESS_CAPACITY : 0u;
    process_count_value = 0;
    process_next_pid = 1;
}

uint32_t process_count(void) {
    return process_count_value;
}

void *process_lookup(uint32_t pid) {
    for (uint32_t i = 0; i < process_table_capacity; ++i) {
        process_record_t *rec = &process_table[i];
        if (rec->state == PROCESS_STATE_EMPTY) {
            continue;
        }
        if (rec->pid == pid) {
            return rec;
        }
    }
    return 0;
}

/* Verdoppelt die Tabelle (mind. auf PROCESS_CAPACITY, falls sie durch
   eine fehlgeschlagene fruehere Initialisierung bei 0 steht). Der neu
   hinzugewonnene Bereich wird explizit genullt - krealloc() kopiert nur
   die alten Bytes, zeroed den Zuwachs NICHT automatisch (anders als
   kcalloc() bei der ERSTEN Allokation). 0 = Erfolg (*out_slot = erster
   garantiert freier Slot im neuen Bereich), -1 bei Speichererschoepfung
   (process_table/process_table_capacity bleiben in diesem Fall
   UNVERAENDERT gueltig, wie bei krealloc() ueblich). */
static int process_table_grow(uint32_t *out_slot) {
    uint32_t new_capacity = (process_table_capacity == 0u) ? PROCESS_CAPACITY : process_table_capacity * 2u;
    process_record_t *grown = (process_record_t *)krealloc(process_table,
                                                             new_capacity * (uint32_t)sizeof(process_record_t));
    if (!grown) return -1;

    for (uint32_t i = process_table_capacity; i < new_capacity; ++i) {
        grown[i] = (process_record_t){0};
    }
    *out_slot = process_table_capacity;
    process_table = grown;
    process_table_capacity = new_capacity;
    return 0;
}

uint32_t process_create(uint32_t flags, uint32_t page_directory_cr3) {
    uint32_t slot;
    for (slot = 0; slot < process_table_capacity; ++slot) {
        if (process_table[slot].state == PROCESS_STATE_EMPTY) {
            break;
        }
    }
    if (slot >= process_table_capacity) {
        if (process_table_grow(&slot) != 0) {
            return 0;
        }
    }

    uint32_t pid = process_next_pid;
    uint32_t handle = object_create(OBJECT_TYPE_PROCESS, 0, 0);
    if (handle == 0) {
        return 0;
    }

    process_record_t *rec = &process_table[slot];
    rec->pid = pid;
    rec->state = PROCESS_STATE_RUNNING;
    rec->cr3 = page_directory_cr3;
    rec->handle = handle;
    rec->parent = 0;
    rec->flags = flags;
    rec->exit_status = 0;

    ++process_count_value;
    ++process_next_pid;
    return pid;
}

uint32_t process_get_cr3(uint32_t pid) {
    process_record_t *rec = (process_record_t *)process_lookup(pid);
    return rec ? rec->cr3 : 0u;
}

uint32_t process_find_by_cr3(uint32_t cr3) {
    if (cr3 == 0) {
        return 0u;
    }
    for (uint32_t i = 0; i < process_table_capacity; ++i) {
        process_record_t *rec = &process_table[i];
        if (rec->state == PROCESS_STATE_EMPTY) {
            continue;
        }
        if (rec->cr3 == cr3) {
            return rec->pid;
        }
    }
    return 0u;
}

uint32_t process_state(uint32_t pid) {
    process_record_t *rec = (process_record_t *)process_lookup(pid);
    return rec ? rec->state : PROCESS_STATE_EMPTY;
}

int process_suspend(uint32_t pid) {
    process_record_t *rec = (process_record_t *)process_lookup(pid);
    if (!rec || rec->state != PROCESS_STATE_RUNNING) {
        return -1;
    }
    rec->state = PROCESS_STATE_SUSPENDED;
    return 0;
}

int process_resume(uint32_t pid) {
    process_record_t *rec = (process_record_t *)process_lookup(pid);
    if (!rec || rec->state != PROCESS_STATE_SUSPENDED) {
        return -1;
    }
    rec->state = PROCESS_STATE_RUNNING;
    return 0;
}

int process_terminate(uint32_t pid, uint32_t exit_status) {
    process_record_t *rec = (process_record_t *)process_lookup(pid);
    if (!rec) {
        return -1;
    }
    if (rec->state != PROCESS_STATE_RUNNING && rec->state != PROCESS_STATE_SUSPENDED) {
        return -1;
    }
    rec->state = PROCESS_STATE_TERMINATED;
    rec->exit_status = exit_status;
    return 0;
}

uint32_t process_exit_status(uint32_t pid) {
    process_record_t *rec = (process_record_t *)process_lookup(pid);
    if (!rec || rec->state != PROCESS_STATE_TERMINATED) {
        return 0u;
    }
    return rec->exit_status;
}

int process_reap(uint32_t pid) {
    process_record_t *rec = (process_record_t *)process_lookup(pid);
    if (!rec || rec->state != PROCESS_STATE_TERMINATED) {
        return -1;
    }
    *rec = (process_record_t){0};
    --process_count_value;
    return 0;
}

static inline uint32_t read_cr3(void) {
    uint32_t value;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(value));
    return value;
}

int process_manager_self_test(void) {
    uint32_t cr3 = read_cr3();
    uint32_t pid = process_create(1 /* Kernel/System-Prozess */, cr3);
    if (pid != 1) return -1;

    process_record_t *rec = (process_record_t *)process_lookup(pid);
    if (!rec) return -1;
    if (rec->state != PROCESS_STATE_RUNNING) return -1;
    if (rec->cr3 != cr3) return -1;
    if (process_count_value != 1) return -1;

    /* NPSPEC-PROZESS-LIFECYCLE-0001: vollen Lebenszyklus an einem
       EIGENEN Testprozess beweisen (nicht am obigen dauerhaften PID 1,
       der von anderen Selbsttests weiterverwendet wird). */
    uint32_t test_pid = process_create(0, cr3);
    if (test_pid == 0) return -1;
    if (process_state(test_pid) != PROCESS_STATE_RUNNING) return -1;

    if (process_suspend(test_pid) != 0) return -1;
    if (process_state(test_pid) != PROCESS_STATE_SUSPENDED) return -1;
    if (process_suspend(test_pid) == 0) return -1; /* bereits suspendiert */

    if (process_resume(test_pid) != 0) return -1;
    if (process_state(test_pid) != PROCESS_STATE_RUNNING) return -1;

    if (process_terminate(test_pid, 0x2A) != 0) return -1;
    if (process_state(test_pid) != PROCESS_STATE_TERMINATED) return -1;
    if (process_exit_status(test_pid) != 0x2A) return -1;
    if (process_terminate(test_pid, 0) == 0) return -1; /* schon beendet */

    uint32_t count_before_reap = process_count_value;
    if (process_reap(test_pid) != 0) return -1;
    if (process_count_value != count_before_reap - 1u) return -1;
    if (process_lookup(test_pid) != 0) return -1; /* Slot wirklich frei */
    if (process_exit_status(test_pid) != 0) return -1; /* mit Slot verschwunden */

    /* Beweist die eigentliche Aenderung dieser Session: mehr Prozesse
       anlegen, als die urspruengliche PROCESS_CAPACITY jemals erlaubt
       haette (vorher: process_create() haette hier 0 geliefert) - die
       Tabelle MUSS dafuer mindestens einmal per process_table_grow()
       gewachsen sein. Alle Testprozesse werden danach wieder beendet und
       eingesammelt, damit kein Slot fuer spaetere Selbsttests belegt
       bleibt (nur PID 1 bleibt wie zuvor dauerhaft bestehen). */
    uint32_t growth_pids[PROCESS_CAPACITY + 3u];
    uint32_t growth_count = PROCESS_CAPACITY + 3u;
    for (uint32_t i = 0; i < growth_count; ++i) {
        growth_pids[i] = process_create(0, cr3);
        if (growth_pids[i] == 0) return -1; /* waere vor dieser Aenderung hier fehlgeschlagen */
    }
    if (process_state(growth_pids[growth_count - 1u]) != PROCESS_STATE_RUNNING) return -1;
    if (process_get_cr3(growth_pids[growth_count - 1u]) != cr3) return -1;

    for (uint32_t i = 0; i < growth_count; ++i) {
        if (process_terminate(growth_pids[i], 0) != 0) return -1;
        if (process_reap(growth_pids[i]) != 0) return -1;
    }
    if (process_lookup(growth_pids[growth_count - 1u]) != 0) return -1;

    return 0;
}
