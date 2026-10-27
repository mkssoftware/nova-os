/* NovaOS C-Kernel - Synchronisation 1.0 Erweiterung (Port aus sync32.inc,
   siehe syncext.h). */
#include "syncext.h"
#include "sync.h"

static int sync_ext_ready = 0;

static sync_mutex_t sync_test_mutex;
static sync_condition_t sync_test_cond;
static sync_wait_queue_t sync_test_wq;
static uint32_t sync_st_rcu_target = 0;
static void *sync_st_rcu_ptr = 0;
static uint32_t sync_st_rcu_called = 0;

static void sync_st_rcu_cb(uint32_t context) {
    (void)context;
    sync_st_rcu_called = 1;
}

void sync_ext_initialize(void) {
    sync_test_mutex = (sync_mutex_t){0};
    sync_test_cond = (sync_condition_t){0};
    sync_test_wq = (sync_wait_queue_t){0};

    sync_st_rcu_ptr = 0;
    sync_st_rcu_target = 0xCAFEBABEu;
    sync_st_rcu_called = 0;

    sync_ext_ready = 1;
}

void sync_mutex_init(sync_mutex_t *mutex, uint32_t flags) {
    mutex->spin = SYNC_SPIN_UNLOCKED;
    mutex->owner = 0;
    mutex->waiters = 0;
    mutex->flags = flags;
}

int sync_mutex_lock(sync_mutex_t *mutex) {
    if (sync_spin_try_lock(&mutex->spin) != 0) {
        /* Bootstrap: Wiedereintritt gilt als Deadlock, da auf Single-CPU
           ohne echten Taskwechsel kein anderer Besitzer je entsperren
           koennte (1:1 wie im alten ASM). */
        __atomic_fetch_add(&mutex->waiters, 1, __ATOMIC_SEQ_CST);
        __atomic_fetch_sub(&mutex->waiters, 1, __ATOMIC_SEQ_CST);
        return -1;
    }
    mutex->owner = 1;
    return 0;
}

int sync_mutex_try_lock(sync_mutex_t *mutex) {
    if (sync_spin_try_lock(&mutex->spin) != 0) return -1;
    mutex->owner = 1;
    return 0;
}

int sync_mutex_unlock(sync_mutex_t *mutex) {
    if (mutex->owner == 0) return -1;
    mutex->owner = 0;
    sync_spin_unlock(&mutex->spin);
    return 0;
}

void sync_condition_init(sync_condition_t *cond) {
    cond->generation = 0;
    cond->waiters = 0;
    cond->spin = SYNC_SPIN_UNLOCKED;
}

void sync_condition_signal(sync_condition_t *cond) {
    __atomic_fetch_add(&cond->generation, 1, __ATOMIC_SEQ_CST);
}

void sync_condition_broadcast(sync_condition_t *cond) {
    uint32_t waiters = cond->waiters;
    if (waiters == 0) return;
    __atomic_fetch_add(&cond->generation, waiters, __ATOMIC_SEQ_CST);
}

int sync_condition_wait(sync_condition_t *cond, sync_mutex_t *mutex) {
    (void)cond;
    if (sync_mutex_unlock(mutex) != 0) return -1;
    if (sync_mutex_lock(mutex) != 0) return -1;
    return 0;
}

void sync_wait_queue_init(sync_wait_queue_t *wq) {
    wq->generation = 0;
    wq->waiters = 0;
    wq->spin = SYNC_SPIN_UNLOCKED;
    wq->flags = 0;
}

void sync_wait_queue_wake_one(sync_wait_queue_t *wq) {
    __atomic_fetch_add(&wq->generation, 1, __ATOMIC_SEQ_CST);
}

void sync_wait_queue_wake_all(sync_wait_queue_t *wq) {
    __atomic_fetch_add(&wq->generation, 1, __ATOMIC_SEQ_CST);
}

void sync_rcu_read_lock(void) {
    /* no-op: keine Praeemption auf Single-CPU */
}

void sync_rcu_read_unlock(void) {
    /* no-op */
}

void sync_rcu_assign_pointer(void **dst, void *new_value) {
    __asm__ volatile ("sfence" ::: "memory");
    *dst = new_value;
}

void *sync_rcu_dereference(void *const *src) {
    void *value = *src;
    __asm__ volatile ("lfence" ::: "memory");
    return value;
}

void sync_rcu_synchronize(void) {
    __asm__ volatile ("mfence" ::: "memory");
}

void sync_rcu_call(sync_rcu_callback_t callback, uint32_t context) {
    callback(context);
}

int sync_ext_self_test(void) {
    if (!sync_ext_ready) return -1;

    /* Test 1: mutex_lock / mutex_unlock */
    sync_mutex_init(&sync_test_mutex, 0);
    if (sync_mutex_lock(&sync_test_mutex) != 0) return -1;
    if (sync_test_mutex.spin != SYNC_SPIN_LOCKED) return -1;
    if (sync_mutex_unlock(&sync_test_mutex) != 0) return -1;
    if (sync_test_mutex.spin != SYNC_SPIN_UNLOCKED) return -1;

    /* Test 2: mutex_try_lock - frei / belegt */
    if (sync_mutex_try_lock(&sync_test_mutex) != 0) return -1;
    if (sync_mutex_try_lock(&sync_test_mutex) == 0) return -1;
    sync_mutex_unlock(&sync_test_mutex);

    /* Test 3: mutex_unlock ohne Besitzer -> Fehler */
    if (sync_mutex_unlock(&sync_test_mutex) == 0) return -1;

    /* Test 4: condition_signal erhoeht Generation */
    sync_condition_init(&sync_test_cond);
    sync_condition_signal(&sync_test_cond);
    if (sync_test_cond.generation != 1) return -1;
    sync_condition_broadcast(&sync_test_cond); /* waiters=0 -> unveraendert */
    if (sync_test_cond.generation != 1) return -1;

    /* Test 5: condition_wait - Mutex unlock+relock */
    sync_mutex_init(&sync_test_mutex, 0);
    if (sync_mutex_lock(&sync_test_mutex) != 0) return -1;
    if (sync_condition_wait(&sync_test_cond, &sync_test_mutex) != 0) return -1;
    if (sync_test_mutex.spin != SYNC_SPIN_LOCKED) return -1;
    sync_mutex_unlock(&sync_test_mutex);

    /* Test 6: wait_queue wake_one / wake_all */
    sync_wait_queue_init(&sync_test_wq);
    sync_wait_queue_wake_one(&sync_test_wq);
    if (sync_test_wq.generation != 1) return -1;
    sync_wait_queue_wake_all(&sync_test_wq);
    if (sync_test_wq.generation != 2) return -1;

    /* Test 7: rcu_assign_pointer + rcu_dereference (zweistufig, wie im
       alten ASM: zweiter Aufruf verkettet auf dem Rueckgabewert des
       ersten, liest also durch den Zeiger hindurch dessen Zielwert). */
    sync_rcu_assign_pointer(&sync_st_rcu_ptr, &sync_st_rcu_target);
    void *level1 = sync_rcu_dereference((void *const *)&sync_st_rcu_ptr);
    if (level1 != &sync_st_rcu_target) return -1;
    void *level2 = sync_rcu_dereference((void *const *)level1);
    if ((uint32_t)(uintptr_t)level2 != 0xCAFEBABEu) return -1;

    /* Test 8: rcu_call -> Callback sofort ausgefuehrt */
    sync_st_rcu_called = 0;
    sync_rcu_call(sync_st_rcu_cb, 0);
    if (sync_st_rcu_called != 1) return -1;

    return 0;
}
