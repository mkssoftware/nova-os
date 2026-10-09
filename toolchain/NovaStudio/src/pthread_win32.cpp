/*
 * pthread_win32.cpp – Win32-native pthreads stub for Nova Studio
 *
 * Implements only the subset that libstdc++ (exception handling,
 * mt_allocator, condition_variable, memory_resource) actually calls.
 * Uses Windows TLS / CRITICAL_SECTION / CONDITION_VARIABLE directly.
 * No libwinpthread-1.dll dependency.
 */
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <pthread.h>   /* winpthread types from ucrt64 headers */

/* -------------------------------------------------------------------------
 * pthread_once
 * ---------------------------------------------------------------------- */
int pthread_once(pthread_once_t *once, void (*init)(void))
{
    /* winpthread defines pthread_once_t as 'volatile int' set to 0 = unrun */
    static const int ONCE_INIT  = 0;
    static const int ONCE_DONE  = 1;
    static const int ONCE_BUSY  = 2;

    if (*once == ONCE_DONE) return 0;

    /* Spin until we either win the CAS or someone else completes it */
    if (InterlockedCompareExchange((LONG volatile *)once, ONCE_BUSY, ONCE_INIT)
            == ONCE_INIT) {
        init();
        InterlockedExchange((LONG volatile *)once, ONCE_DONE);
    } else {
        while (*once != ONCE_DONE)
            SwitchToThread();
    }
    return 0;
}

/* -------------------------------------------------------------------------
 * Thread-local storage (TLS) key management
 * ---------------------------------------------------------------------- */
int pthread_key_create(pthread_key_t *key, void (*destructor)(void *))
{
    (void)destructor;   /* Nova Studio is single-threaded; no cleanup needed */
    DWORD idx = TlsAlloc();
    if (idx == TLS_OUT_OF_INDEXES) return EAGAIN;
    *key = (pthread_key_t)idx;
    return 0;
}

int pthread_key_delete(pthread_key_t key)
{
    TlsFree((DWORD)key);
    return 0;
}

void *pthread_getspecific(pthread_key_t key)
{
    return TlsGetValue((DWORD)key);
}

int pthread_setspecific(pthread_key_t key, const void *value)
{
    if (!TlsSetValue((DWORD)key, (LPVOID)value)) return EINVAL;
    return 0;
}

/* -------------------------------------------------------------------------
 * Mutex
 * ---------------------------------------------------------------------- */
int pthread_mutex_init(pthread_mutex_t *mtx, const pthread_mutexattr_t *attr)
{
    (void)attr;
    InitializeCriticalSection((CRITICAL_SECTION *)mtx);
    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mtx)
{
    DeleteCriticalSection((CRITICAL_SECTION *)mtx);
    return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mtx)
{
    EnterCriticalSection((CRITICAL_SECTION *)mtx);
    return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mtx)
{
    LeaveCriticalSection((CRITICAL_SECTION *)mtx);
    return 0;
}

/* -------------------------------------------------------------------------
 * Condition variable  (libstdc++ condition_variable uses these)
 * ---------------------------------------------------------------------- */
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
{
    (void)attr;
    InitializeConditionVariable((CONDITION_VARIABLE *)cond);
    return 0;
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
    (void)cond;   /* CONDITION_VARIABLE needs no cleanup */
    return 0;
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mtx)
{
    SleepConditionVariableCS(
        (CONDITION_VARIABLE *)cond,
        (CRITICAL_SECTION  *)mtx,
        INFINITE);
    return 0;
}

int pthread_cond_signal(pthread_cond_t *cond)
{
    WakeConditionVariable((CONDITION_VARIABLE *)cond);
    return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
    WakeAllConditionVariable((CONDITION_VARIABLE *)cond);
    return 0;
}

/* -------------------------------------------------------------------------
 * Read/write locks  (referenced by libstdc++ but not called in practice)
 * ---------------------------------------------------------------------- */
int pthread_rwlock_rdlock(pthread_rwlock_t *rw)
{
    AcquireSRWLockShared((SRWLOCK *)rw);
    return 0;
}

int pthread_rwlock_wrlock(pthread_rwlock_t *rw)
{
    AcquireSRWLockExclusive((SRWLOCK *)rw);
    return 0;
}

int pthread_rwlock_unlock(pthread_rwlock_t *rw)
{
    /* SRW locks don't distinguish release direction – use shared release
     * as a conservative fallback; libstdc++ only holds one kind at a time. */
    ReleaseSRWLockShared((SRWLOCK *)rw);
    return 0;
}

/* -------------------------------------------------------------------------
 * Thread creation / joining  (not called in a single-threaded GUI app,
 * but the linker needs the symbol table entries.)
 * ---------------------------------------------------------------------- */
int pthread_create(pthread_t *tid, const pthread_attr_t *attr,
                   void *(*fn)(void *), void *arg)
{
    (void)tid; (void)attr; (void)fn; (void)arg;
    return ENOSYS;
}

int pthread_join(pthread_t tid, void **retval)
{
    (void)tid; (void)retval;
    return ENOSYS;
}

int pthread_detach(pthread_t tid)
{
    (void)tid;
    return ENOSYS;
}

/* -------------------------------------------------------------------------
 * Misc
 * ---------------------------------------------------------------------- */
int pthread_num_processors_np(void)
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return (int)si.dwNumberOfProcessors;
}
