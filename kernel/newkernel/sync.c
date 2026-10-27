/* NovaOS C-Kernel - Synchronisation 1.0 Bootstrap (Port aus sync32.inc,
   siehe sync.h). */
#include "sync.h"

static int sync_ready = 0;

/* Testfixturen (entsprechen den statischen §016-Daten im alten ASM). */
static uint32_t sync_st_atomic = 0;
static uint32_t sync_test_spin = 0;
static sync_semaphore_t sync_test_sema;
static sync_completion_t sync_test_compl;
static uint32_t sync_test_seqlock = 0;
static uint32_t sync_test_refcount = 0;

void sync_initialize(void) {
    sync_st_atomic = 0;
    sync_test_spin = SYNC_SPIN_UNLOCKED;
    sync_test_sema = (sync_semaphore_t){0};
    sync_test_compl = (sync_completion_t){0};
    sync_test_seqlock = 0;
    sync_test_refcount = 3;

    /* VSVC-Service "nova.sync.core" wird NICHT registriert - vsvc_register
       (§054) ist noch nicht portiert, siehe sync.h. */

    sync_ready = 1;
}

uint32_t sync_atomic_load_u32(const volatile uint32_t *ptr) {
    return *ptr;
}

void sync_atomic_store_u32(volatile uint32_t *ptr, uint32_t value) {
    *ptr = value;
    __asm__ volatile ("mfence" ::: "memory");
}

uint32_t sync_atomic_exchange_u32(volatile uint32_t *ptr, uint32_t new_value) {
    return __atomic_exchange_n(ptr, new_value, __ATOMIC_SEQ_CST);
}

int sync_atomic_compare_exchange_u32(volatile uint32_t *ptr, uint32_t expected,
                                      uint32_t desired, uint32_t *out_actual) {
    uint32_t actual = expected;
    int swapped = __atomic_compare_exchange_n(ptr, &actual, desired, 0,
                                               __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    if (out_actual) *out_actual = actual;
    return swapped ? 0 : -1;
}

uint32_t sync_atomic_fetch_add_u32(volatile uint32_t *ptr, uint32_t increment) {
    return __atomic_fetch_add(ptr, increment, __ATOMIC_SEQ_CST);
}

void sync_memory_barrier(void) {
    __asm__ volatile ("mfence" ::: "memory");
}

void sync_read_barrier(void) {
    __asm__ volatile ("lfence" ::: "memory");
}

void sync_write_barrier(void) {
    __asm__ volatile ("sfence" ::: "memory");
}

void sync_compiler_barrier(void) {
    __asm__ volatile ("" ::: "memory");
}

void sync_spin_init(volatile uint32_t *lock) {
    *lock = SYNC_SPIN_UNLOCKED;
}

void sync_spin_lock(volatile uint32_t *lock) {
    while (__atomic_exchange_n(lock, SYNC_SPIN_LOCKED, __ATOMIC_SEQ_CST) != SYNC_SPIN_UNLOCKED) {
        __asm__ volatile ("pause");
    }
}

int sync_spin_try_lock(volatile uint32_t *lock) {
    return (__atomic_exchange_n(lock, SYNC_SPIN_LOCKED, __ATOMIC_SEQ_CST) == SYNC_SPIN_UNLOCKED) ? 0 : -1;
}

void sync_spin_unlock(volatile uint32_t *lock) {
    __asm__ volatile ("mfence" ::: "memory");
    *lock = SYNC_SPIN_UNLOCKED;
}

uint32_t sync_spin_lock_irqsave(volatile uint32_t *lock) {
    uint32_t flags;
    __asm__ volatile (
        "pushfl\n\t"
        "popl %0\n\t"
        "cli"
        : "=r"(flags)
        :
        : "memory"
    );
    sync_spin_lock(lock);
    return flags;
}

void sync_spin_unlock_irqrestore(volatile uint32_t *lock, uint32_t saved_eflags) {
    sync_spin_unlock(lock);
    __asm__ volatile (
        "pushl %0\n\t"
        "popfl"
        :
        : "r"(saved_eflags)
        : "memory", "cc"
    );
}

void sync_semaphore_init(sync_semaphore_t *sema, uint32_t initial_count, uint32_t maximum) {
    sema->count = initial_count;
    sema->maximum = maximum;
    sema->flags = 0;
}

int sync_semaphore_try_wait(sync_semaphore_t *sema) {
    uint32_t count = __atomic_load_n(&sema->count, __ATOMIC_SEQ_CST);
    for (;;) {
        if (count == 0) return -1;
        uint32_t desired = count - 1;
        if (__atomic_compare_exchange_n(&sema->count, &count, desired, 0,
                                         __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
            return 0;
        }
    }
}

int sync_semaphore_release(sync_semaphore_t *sema, uint32_t release_count) {
    uint32_t count = __atomic_load_n(&sema->count, __ATOMIC_SEQ_CST);
    for (;;) {
        uint32_t desired = count + release_count;
        if (desired > sema->maximum) return -1;
        if (__atomic_compare_exchange_n(&sema->count, &count, desired, 0,
                                         __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
            return 0;
        }
    }
}

void sync_completion_init(sync_completion_t *completion) {
    completion->signaled = 0;
    completion->generation = 0;
}

void sync_completion_signal(sync_completion_t *completion) {
    completion->signaled = 1;
    __atomic_fetch_add(&completion->generation, 1, __ATOMIC_SEQ_CST);
    __asm__ volatile ("mfence" ::: "memory");
}

int sync_completion_is_done(sync_completion_t *completion) {
    return (completion->signaled == 1) ? 0 : -1;
}

void sync_seqlock_init(volatile uint32_t *seqlock) {
    *seqlock = 0;
}

uint32_t sync_seqlock_read_begin(volatile uint32_t *seqlock) {
    uint32_t snapshot;
    for (;;) {
        snapshot = __atomic_load_n(seqlock, __ATOMIC_SEQ_CST);
        if ((snapshot & 1u) == 0) break;
        __asm__ volatile ("pause");
    }
    __asm__ volatile ("lfence" ::: "memory");
    return snapshot;
}

int sync_seqlock_read_retry(volatile uint32_t *seqlock, uint32_t snapshot) {
    __asm__ volatile ("lfence" ::: "memory");
    return (__atomic_load_n(seqlock, __ATOMIC_SEQ_CST) == snapshot) ? 0 : -1;
}

void sync_seqlock_write_lock(volatile uint32_t *seqlock) {
    __atomic_fetch_add(seqlock, 1, __ATOMIC_SEQ_CST);
    __asm__ volatile ("mfence" ::: "memory");
}

void sync_seqlock_write_unlock(volatile uint32_t *seqlock) {
    __asm__ volatile ("mfence" ::: "memory");
    __atomic_fetch_add(seqlock, 1, __ATOMIC_SEQ_CST);
}

int sync_refcount_retain(volatile uint32_t *refcount) {
    uint32_t count = __atomic_load_n(refcount, __ATOMIC_SEQ_CST);
    for (;;) {
        if (count == 0) return -1;
        uint32_t desired = count + 1;
        if (__atomic_compare_exchange_n(refcount, &count, desired, 0,
                                         __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
            return 0;
        }
    }
}

int sync_refcount_release(volatile uint32_t *refcount) {
    uint32_t old_value = __atomic_fetch_sub(refcount, 1, __ATOMIC_SEQ_CST);
    return (old_value == 1) ? -1 : 0;
}

int sync_self_test(void) {
    if (!sync_ready) return -1;

    /* Test 1: store + load -> read-back */
    sync_atomic_store_u32(&sync_st_atomic, 0xA5A5A5A5u);
    if (sync_atomic_load_u32(&sync_st_atomic) != 0xA5A5A5A5u) return -1;

    /* Test 2: exchange -> Altwert zurueck */
    if (sync_atomic_exchange_u32(&sync_st_atomic, 0x12345678u) != 0xA5A5A5A5u) return -1;
    if (sync_st_atomic != 0x12345678u) return -1;

    /* Test 3: compare_exchange - Treffer */
    uint32_t actual;
    if (sync_atomic_compare_exchange_u32(&sync_st_atomic, 0x12345678u, 0xDEADBEEFu, &actual) != 0) return -1;
    if (sync_st_atomic != 0xDEADBEEFu) return -1;

    /* Test 4: compare_exchange - Fehltreffer */
    if (sync_atomic_compare_exchange_u32(&sync_st_atomic, 0x00000000u, 0x11111111u, &actual) == 0) return -1;

    /* Test 5: fetch_add -> Altwert + Summe */
    sync_st_atomic = 10;
    if (sync_atomic_fetch_add_u32(&sync_st_atomic, 5) != 10) return -1;
    if (sync_st_atomic != 15) return -1;

    /* Test 6: spin_lock / spin_unlock */
    sync_spin_init(&sync_test_spin);
    sync_spin_lock(&sync_test_spin);
    if (sync_test_spin != SYNC_SPIN_LOCKED) return -1;
    sync_spin_unlock(&sync_test_spin);
    if (sync_test_spin != SYNC_SPIN_UNLOCKED) return -1;

    /* Test 7: spin_try_lock - frei / belegt */
    if (sync_spin_try_lock(&sync_test_spin) != 0) return -1;
    if (sync_spin_try_lock(&sync_test_spin) == 0) return -1;
    sync_spin_unlock(&sync_test_spin);

    /* Test 8: spin_lock_irqsave / spin_unlock_irqrestore */
    uint32_t saved_flags = sync_spin_lock_irqsave(&sync_test_spin);
    if (sync_test_spin != SYNC_SPIN_LOCKED) return -1;
    sync_spin_unlock_irqrestore(&sync_test_spin, saved_flags);

    /* Test 9: Semaphore init(2,2), try_wait x2 ok, try_wait leer, release */
    sync_semaphore_init(&sync_test_sema, 2, 2);
    if (sync_semaphore_try_wait(&sync_test_sema) != 0) return -1;
    if (sync_semaphore_try_wait(&sync_test_sema) != 0) return -1;
    if (sync_semaphore_try_wait(&sync_test_sema) == 0) return -1; /* count=0 */
    if (sync_semaphore_release(&sync_test_sema, 1) != 0) return -1; /* count=1 */
    if (sync_semaphore_release(&sync_test_sema, 2) == 0) return -1; /* 1+2=3 > max=2 */

    /* Test 10: Completion init -> pending; signal -> done */
    sync_completion_init(&sync_test_compl);
    if (sync_completion_is_done(&sync_test_compl) == 0) return -1;
    sync_completion_signal(&sync_test_compl);
    if (sync_completion_is_done(&sync_test_compl) != 0) return -1;

    /* Test 11: Sequence-Lock und Refcount */
    sync_seqlock_init(&sync_test_seqlock);
    sync_seqlock_write_lock(&sync_test_seqlock);
    sync_seqlock_write_unlock(&sync_test_seqlock);
    uint32_t snapshot = sync_seqlock_read_begin(&sync_test_seqlock);
    if (snapshot & 1u) return -1;
    if (sync_seqlock_read_retry(&sync_test_seqlock, snapshot) != 0) return -1;

    if (sync_refcount_retain(&sync_test_refcount) != 0) return -1; /* 3 -> 4 */
    if (sync_test_refcount != 4) return -1;
    if (sync_refcount_release(&sync_test_refcount) != 0) return -1; /* 4 -> 3 */
    if (sync_refcount_release(&sync_test_refcount) != 0) return -1; /* 3 -> 2 */
    if (sync_refcount_release(&sync_test_refcount) != 0) return -1; /* 2 -> 1 */
    if (sync_refcount_release(&sync_test_refcount) == 0) return -1; /* 1 -> 0 */

    return 0;
}
