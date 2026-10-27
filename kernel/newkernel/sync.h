/* NovaOS C-Kernel - Synchronisation 1.0 Bootstrap (Port aus sync32.inc,
   §016 / NPSPEC-SYNC-ATOMIC-0001 / NPSPEC-SYNC-BARRIER-0001 /
   NPSPEC-SYNC-SPINLOCK-0001 / NPSPEC-SYNC-SEMAPHORE-0001 /
   NPSPEC-SYNC-LOCKFREE-0001). Atomare Operationen, Speicherbarrieren,
   Spinlocks (inkl. IRQ-sicher), Semaphoren (nur Try-Wait, kein
   Blockieren), Completion-Objekte, Sequence-Locks, Referenzzaehlung.
   RCU, blockierende Mutexe und Wait-Queues brauchen Scheduler-
   Integration und sind NICHT Teil dieses Moduls (wie im alten ASM-
   Kommentar selbst vermerkt). Read-Modify-Write-Atomics nutzen GCC-
   Atomic-Builtins (lowern auf x86-32 direkt zu lock cmpxchg/xadd/xchg,
   kein libatomic-Aufruf) statt handgeschriebener Inline-Assembly - das
   ist fuer diese Operationen weniger fehleranfaellig als Byte-genaues
   Hand-Asm und bei denselben Garantien 1:1 aequivalent zum alten ASM.
   `sync_initialize` registriert NICHT den VSVC-Service "nova.sync.core"
   (vsvc_register/§054 Versioned-Service-Registry ist noch nicht
   portiert) - bewusste Vereinfachung, wie bei anderen Cross-Modul-
   Abhaengigkeiten auf unportierte Subsysteme. */
#ifndef NOVA_SYNC_H
#define NOVA_SYNC_H

#include <stdint.h>

#define SYNC_SPIN_UNLOCKED 0u
#define SYNC_SPIN_LOCKED   1u

typedef struct {
    uint32_t count;
    uint32_t maximum;
    uint32_t flags;
} sync_semaphore_t;

typedef struct {
    uint32_t signaled;
    uint32_t generation;
} sync_completion_t;

void sync_initialize(void);
int sync_self_test(void); /* 0 = ok (alle 11 Teiltests) */

/* --- Atomare Lade-/Speicheroperationen --- */
uint32_t sync_atomic_load_u32(const volatile uint32_t *ptr);
void sync_atomic_store_u32(volatile uint32_t *ptr, uint32_t value);
uint32_t sync_atomic_exchange_u32(volatile uint32_t *ptr, uint32_t new_value);
/* 0 = getauscht, -1 = nicht getauscht (*out_actual, falls != NULL, erhaelt
   in beiden Faellen den tatsaechlichen Wert zum Vergleichszeitpunkt). */
int sync_atomic_compare_exchange_u32(volatile uint32_t *ptr, uint32_t expected,
                                      uint32_t desired, uint32_t *out_actual);
uint32_t sync_atomic_fetch_add_u32(volatile uint32_t *ptr, uint32_t increment); /* gibt Altwert zurueck */

/* --- Speicherbarrieren --- */
void sync_memory_barrier(void);
void sync_read_barrier(void);
void sync_write_barrier(void);
void sync_compiler_barrier(void);

/* --- Spinlock --- */
void sync_spin_init(volatile uint32_t *lock);
void sync_spin_lock(volatile uint32_t *lock);
int sync_spin_try_lock(volatile uint32_t *lock); /* 0 = erworben, -1 = belegt */
void sync_spin_unlock(volatile uint32_t *lock);
uint32_t sync_spin_lock_irqsave(volatile uint32_t *lock); /* gibt gesichertes EFLAGS zurueck */
void sync_spin_unlock_irqrestore(volatile uint32_t *lock, uint32_t saved_eflags);

/* --- Semaphore (nur Try-Wait) --- */
void sync_semaphore_init(sync_semaphore_t *sema, uint32_t initial_count, uint32_t maximum);
int sync_semaphore_try_wait(sync_semaphore_t *sema); /* 0 = erworben, -1 = leer */
int sync_semaphore_release(sync_semaphore_t *sema, uint32_t release_count); /* 0 = ok, -1 = Ueberlauf */

/* --- Completion-Objekt --- */
void sync_completion_init(sync_completion_t *completion);
void sync_completion_signal(sync_completion_t *completion);
int sync_completion_is_done(sync_completion_t *completion); /* 0 = signalisiert, -1 = ausstehend */

/* --- Sequence-Lock --- */
void sync_seqlock_init(volatile uint32_t *seqlock);
uint32_t sync_seqlock_read_begin(volatile uint32_t *seqlock); /* gibt geraden Snapshot zurueck */
int sync_seqlock_read_retry(volatile uint32_t *seqlock, uint32_t snapshot); /* 0 = ok, -1 = wiederholen */
void sync_seqlock_write_lock(volatile uint32_t *seqlock);
void sync_seqlock_write_unlock(volatile uint32_t *seqlock);

/* --- Referenzzaehlung --- */
int sync_refcount_retain(volatile uint32_t *refcount); /* 0 = ok, -1 = Objekt bereits tot */
int sync_refcount_release(volatile uint32_t *refcount); /* 0 = noch lebendig, -1 = auf null gefallen */

#endif
