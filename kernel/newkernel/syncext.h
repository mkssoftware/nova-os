/* NovaOS C-Kernel - Synchronisation 1.0 Erweiterung (Port aus sync32.inc,
   §016-Erweiterung / NPSPEC-SYNC-MUTEX-0001 / NPSPEC-SYNC-CONDITION-0001 /
   NPSPEC-SYNC-RWLOCK-0001 / NPSPEC-SYNC-RCU-0001). Blockierender Mutex,
   Condition Variables, Wait Queues, RCU - alles im Bootstrap-Modus:
   Single-CPU ohne Praeemption, daher kein echtes Blockieren/Schlafen
   (Schlaf-Pfade sind Spin-Fallbacks, RCU-Grace-Periods sind No-Ops),
   exakt wie im alten ASM dokumentiert. Baut auf sync.h's Spinlock-
   Konstanten auf. */
#ifndef NOVA_SYNCEXT_H
#define NOVA_SYNCEXT_H

#include <stdint.h>

typedef struct {
    uint32_t spin;
    uint32_t owner;   /* 0 = frei */
    uint32_t waiters;
    uint32_t flags;
} sync_mutex_t;

#define SYNC_MUTEX_FLAG_RECURSIVE 0x00000001u
#define SYNC_MUTEX_FLAG_PI        0x00000002u

typedef struct {
    uint32_t generation;
    uint32_t waiters;
    uint32_t spin;
} sync_condition_t;

typedef struct {
    uint32_t generation;
    uint32_t waiters;
    uint32_t spin;
    uint32_t flags;
} sync_wait_queue_t;

typedef void (*sync_rcu_callback_t)(uint32_t context);

void sync_ext_initialize(void);
int sync_ext_self_test(void); /* 0 = ok (alle 8 Teiltests) */

/* --- Mutex --- */
void sync_mutex_init(sync_mutex_t *mutex, uint32_t flags);
/* 0 = erworben, -1 = Deadlock (Wiedereintritt gilt im Bootstrap-Modus als
   Deadlock, da kein zweiter Thread den Mutex je freigeben koennte). */
int sync_mutex_lock(sync_mutex_t *mutex);
int sync_mutex_try_lock(sync_mutex_t *mutex); /* 0 = erworben, -1 = belegt */
int sync_mutex_unlock(sync_mutex_t *mutex); /* 0 = ok, -1 = nicht Besitzer */

/* --- Condition Variable --- */
void sync_condition_init(sync_condition_t *cond);
void sync_condition_signal(sync_condition_t *cond); /* weckt einen Waiter */
void sync_condition_broadcast(sync_condition_t *cond); /* weckt alle Waiter */
/* Bootstrap-Spurious-Wakeup: gibt den Mutex frei und erwirbt ihn sofort
   wieder. 0 = ok, -1 = Fehler (siehe sync_mutex_unlock/lock). */
int sync_condition_wait(sync_condition_t *cond, sync_mutex_t *mutex);

/* --- Wait Queue --- */
void sync_wait_queue_init(sync_wait_queue_t *wq);
void sync_wait_queue_wake_one(sync_wait_queue_t *wq);
void sync_wait_queue_wake_all(sync_wait_queue_t *wq);

/* --- RCU (Bootstrap: Single-CPU ohne Praeemption, keine echten Grace
   Periods noetig) --- */
void sync_rcu_read_lock(void);  /* no-op */
void sync_rcu_read_unlock(void); /* no-op */
void sync_rcu_assign_pointer(void **dst, void *new_value); /* SFENCE vor dem Schreiben */
void *sync_rcu_dereference(void *const *src); /* LFENCE nach dem Lesen */
void sync_rcu_synchronize(void); /* MFENCE reicht auf Single-CPU */
void sync_rcu_call(sync_rcu_callback_t callback, uint32_t context); /* sofortige Ausfuehrung */

#endif
