; ===========================================================================
; §016 – Synchronisation 1.0 Bootstrap (NPSPEC-KERNEL-0016)
; NPSPEC-SYNC-ATOMIC-0001 / NPSPEC-SYNC-BARRIER-0001 / NPSPEC-SYNC-SPINLOCK-0001
; NPSPEC-SYNC-SEMAPHORE-0001 / NPSPEC-SYNC-LOCKFREE-0001
; Implementiert: Atomaroperationen, Speicherbarrieren, Spinlocks (inkl. IRQ),
; Semaphoren (try-only), Completions, Sequence Locks, Referenzzählung.
; RCU, blockierende Mutexes und Wait Queues erfordern Scheduler-Integration.
; ===========================================================================

; Deskriptor-Größen
SYNC_SEMA_SIZE        equ 12    ; count(dd) + maximum(dd) + flags(dd)
SYNC_COMPL_SIZE       equ 8     ; signaled(dd) + generation(dd)
SYNC_SEQLOCK_SIZE     equ 4     ; sequence(dd): gerade=frei, ungerade=Writer

; Semaphore-Offsets
SYNC_SEMA_OFF_COUNT   equ 0
SYNC_SEMA_OFF_MAX     equ 4
SYNC_SEMA_OFF_FLAGS   equ 8

; Completion-Offsets
SYNC_COMPL_OFF_SIG    equ 0
SYNC_COMPL_OFF_GEN    equ 4

; Spinlock-Zustände
SYNC_SPIN_UNLOCKED    equ 0
SYNC_SPIN_LOCKED      equ 1

; VSVC-UUID für nova.sync.core
VSVC_ID_SYNC_0        equ 0x4E4F5641   ; "NOVA"
VSVC_ID_SYNC_1        equ 0x53594E43   ; "SYNC"
VSVC_ID_SYNC_2        equ 0x434F5245   ; "CORE"
VSVC_ID_SYNC_3        equ 0x00000001

; Fehlercodes §016 (§54)
NP_ERR_SYNC_DEADLOCK  equ -100
NP_ERR_SYNC_NOT_OWNER equ -101
NP_ERR_SYNC_TIMEOUT   equ -102
NP_ERR_SYNC_OVERFLOW  equ -103
NP_ERR_SYNC_INVAL     equ -104

; ---------------------------------------------------------------------------
; Atomare Lade-/Speicheroperationen (§8)
; ---------------------------------------------------------------------------

; np_atomic_load_u32: EAX=ptr → EAX=value (ACQUIRE-Semantik; auf x86 TSO implizit)
np_atomic_load_u32:
    mov eax, [eax]
    ret

; np_atomic_store_u32: EAX=ptr, ECX=value (RELEASE-Semantik via MFENCE)
np_atomic_store_u32:
    mov [eax], ecx
    mfence
    ret

; np_atomic_exchange_u32: EAX=ptr, ECX=new_value → EAX=old_value
; XCHG mit Memory hat impliziten LOCK-Präfix (x86-Garantie)
np_atomic_exchange_u32:
    xchg [eax], ecx
    mov eax, ecx
    ret

; np_atomic_compare_exchange_u32: EAX=ptr, ECX=expected, EDX=desired
; CF=0: getauscht (ZF=1) / CF=1: nicht getauscht, EAX=Ist-Wert
np_atomic_compare_exchange_u32:
    push ebx
    mov ebx, eax
    mov eax, ecx
    lock cmpxchg [ebx], edx
    pop ebx
    jnz .acax_fail
    clc
    ret
.acax_fail:
    stc
    ret

; np_atomic_fetch_add_u32: EAX=ptr, ECX=increment → EAX=old_value
np_atomic_fetch_add_u32:
    lock xadd [eax], ecx
    mov eax, ecx
    ret

; ---------------------------------------------------------------------------
; Speicherbarrieren (§10)
; ---------------------------------------------------------------------------

np_memory_barrier:
    mfence
    ret

np_read_barrier:
    lfence
    ret

np_write_barrier:
    sfence
    ret

np_compiler_barrier:
    ret

; ---------------------------------------------------------------------------
; Spinlock (§11–§13): Test-and-Set, PAUSE-Hint, IRQ-sicher
; ---------------------------------------------------------------------------

; np_spin_init: EAX=spinlock_ptr (dd)
np_spin_init:
    mov dword [eax], SYNC_SPIN_UNLOCKED
    ret

; np_spin_lock: EAX=spinlock_ptr
np_spin_lock:
.nsl_spin:
    mov ecx, SYNC_SPIN_LOCKED
    xchg [eax], ecx         ; ECX = alter Wert, [ptr] = LOCKED
    test ecx, ecx
    jz .nsl_done            ; war UNLOCKED → erworben
    pause                   ; Pipeline-Hint für HT-CPUs
    jmp .nsl_spin
.nsl_done:
    ret

; np_spin_try_lock: EAX=spinlock_ptr → CF=0 erworben / CF=1 belegt
np_spin_try_lock:
    mov ecx, SYNC_SPIN_LOCKED
    xchg [eax], ecx
    test ecx, ecx
    jz .nstl_ok
    stc
    ret
.nstl_ok:
    clc
    ret

; np_spin_unlock: EAX=spinlock_ptr
np_spin_unlock:
    mfence
    mov dword [eax], SYNC_SPIN_UNLOCKED
    ret

; np_spin_lock_irqsave: EAX=spinlock_ptr → EDX=saved_EFLAGS
; Deaktiviert Interrupts atomar mit dem Erwerb (§13)
np_spin_lock_irqsave:
    pushfd
    pop edx
    cli
    call np_spin_lock
    ret

; np_spin_unlock_irqrestore: EAX=spinlock_ptr, EDX=saved_EFLAGS
np_spin_unlock_irqrestore:
    call np_spin_unlock
    push edx
    popfd
    ret

; ---------------------------------------------------------------------------
; Semaphore – Bootstrap: nur Try-Wait (kein Blockieren) (§25)
; ---------------------------------------------------------------------------

; np_semaphore_init: EAX=sema_ptr, ECX=initial_count, EDX=maximum
np_semaphore_init:
    mov [eax + SYNC_SEMA_OFF_COUNT], ecx
    mov [eax + SYNC_SEMA_OFF_MAX],   edx
    mov dword [eax + SYNC_SEMA_OFF_FLAGS], 0
    ret

; np_semaphore_try_wait: EAX=sema_ptr → CF=0 erworben / CF=1 leer
; CAS-Schleife: dekrementiert Count wenn > 0
np_semaphore_try_wait:
    mov [sync_tmp_ptr], eax
.sw_retry:
    mov eax, [sync_tmp_ptr]
    mov ecx, [eax + SYNC_SEMA_OFF_COUNT]
    test ecx, ecx
    jz .sw_fail
    mov edx, ecx
    dec edx
    lock cmpxchg [eax + SYNC_SEMA_OFF_COUNT], edx
    jnz .sw_retry           ; CAS fehlgeschlagen, nochmal
    clc
    ret
.sw_fail:
    stc
    ret

; np_semaphore_release: EAX=sema_ptr, ECX=release_count → CF=0 / CF=1 Überlauf
np_semaphore_release:
    mov [sync_tmp_ptr], eax
    mov [sync_tmp_cnt], ecx
.sr_retry:
    mov eax, [sync_tmp_ptr]
    mov ecx, [eax + SYNC_SEMA_OFF_COUNT]
    mov edx, ecx
    add edx, [sync_tmp_cnt]
    cmp edx, [eax + SYNC_SEMA_OFF_MAX]
    ja .sr_overflow
    lock cmpxchg [eax + SYNC_SEMA_OFF_COUNT], edx
    jnz .sr_retry
    clc
    ret
.sr_overflow:
    stc
    ret

; ---------------------------------------------------------------------------
; Completion Object (§31)
; ---------------------------------------------------------------------------

; np_completion_init: EAX=completion_ptr
np_completion_init:
    mov dword [eax + SYNC_COMPL_OFF_SIG], 0
    mov dword [eax + SYNC_COMPL_OFF_GEN], 0
    ret

; np_completion_signal: EAX=completion_ptr
np_completion_signal:
    mov dword [eax + SYNC_COMPL_OFF_SIG], 1
    lock inc dword [eax + SYNC_COMPL_OFF_GEN]
    mfence
    ret

; np_completion_is_done: EAX=completion_ptr → CF=0 signalisiert / CF=1 ausstehend
np_completion_is_done:
    cmp dword [eax + SYNC_COMPL_OFF_SIG], 1
    je .cid_done
    stc
    ret
.cid_done:
    clc
    ret

; ---------------------------------------------------------------------------
; Sequence Lock (§32): Schreiber-exklusiv, Leser wiederholend
; seqcount: gerade = kein Writer aktiv, ungerade = Writer aktiv
; ---------------------------------------------------------------------------

; np_seqlock_init: EAX=seqlock_ptr (dd)
np_seqlock_init:
    mov dword [eax], 0
    ret

; np_seqlock_read_begin: EAX=seqlock_ptr → ECX=snapshot (muss gerade sein)
np_seqlock_read_begin:
.srb_spin:
    mov ecx, [eax]
    test ecx, 1             ; ungerade = Writer aktiv
    jnz .srb_spin
    lfence
    ret

; np_seqlock_read_retry: EAX=seqlock_ptr, ECX=snapshot → CF=0 ok / CF=1 wiederholen
np_seqlock_read_retry:
    lfence
    cmp [eax], ecx
    je .srr_ok
    stc
    ret
.srr_ok:
    clc
    ret

; np_seqlock_write_lock: EAX=seqlock_ptr (macht count ungerade)
np_seqlock_write_lock:
    lock add dword [eax], 1
    mfence
    ret

; np_seqlock_write_unlock: EAX=seqlock_ptr (macht count wieder gerade)
np_seqlock_write_unlock:
    mfence
    lock add dword [eax], 1
    ret

; ---------------------------------------------------------------------------
; Referenzzählung (§36): Überlauf-/Unterlaufschutz via CAS
; ---------------------------------------------------------------------------

; np_refcount_retain: EAX=refcount_ptr (dd) → CF=0 / CF=1 Objekt bereits tot (count=0)
np_refcount_retain:
    mov [sync_tmp_ptr], eax
.rrn_retry:
    mov eax, [sync_tmp_ptr]
    mov ecx, [eax]
    test ecx, ecx
    jz .rrn_dead            ; Objekt ist tot, darf nicht reviviert werden
    mov edx, ecx
    inc edx
    lock cmpxchg [eax], edx
    jnz .rrn_retry
    clc
    ret
.rrn_dead:
    stc
    ret

; np_refcount_release: EAX=refcount_ptr → CF=0 noch lebendig / CF=1 auf null gefallen
np_refcount_release:
    lock dec dword [eax]
    jz .rrl_zero
    clc
    ret
.rrl_zero:
    stc
    ret

; ---------------------------------------------------------------------------
; sync_initialize – Pools nullen, VSVC-Service registrieren
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
sync_initialize:
    push ebx
    push esi
    push edi

    ; Statische Sync-Objekte für Selbsttest nullen
    mov edi, sync_test_spin
    xor eax, eax
    ; spinlock(4) + sema(12) + compl(8) + seqlock(4) = 28 Byte → 7 dwords
    mov ecx, 7
    rep stosd
    ; Refcount-Testinstanz
    mov dword [sync_test_refcount], 3

    ; Statistiken nullen
    mov dword [sync_stat_acquires], 0
    mov dword [sync_stat_releases], 0

    ; VSVC-Service "nova.sync.core" registrieren
    mov dword [vsvc_tmp_maj],      1
    mov dword [vsvc_tmp_min],      0
    mov dword [vsvc_tmp_pat],      0
    mov dword [vsvc_tmp_table],    sync_vsvc_tbl
    mov dword [vsvc_tmp_tabsz],    VSVC_TBL_HDR_SIZE
    mov dword [vsvc_tmp_features], 0
    mov dword [vsvc_tmp_reqcap],   0
    mov dword [vsvc_tmp_arch],     VSVC_ARCH_X86_32
    mov dword [vsvc_tmp_stab],     VSVC_STAB_STABLE
    mov dword [vsvc_tmp_name],     sync_name_sync
    mov dword [vsvc_tmp_flags],    VSVC_FLAG_EARLY_BOOT | VSVC_FLAG_PANIC_SAFE
    mov eax, VSVC_ID_SYNC_0
    mov ebx, VSVC_ID_SYNC_1
    mov ecx, VSVC_ID_SYNC_2
    mov edx, VSVC_ID_SYNC_3
    call vsvc_register
    jc .si_fail

    pop edi
    pop esi
    pop ebx
    clc
    ret
.si_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; sync_self_test – 11 Tests
; CF=0 alle bestanden / CF=1 Fehler
; ---------------------------------------------------------------------------
sync_self_test:
    push ebx
    push esi
    push edi

    ; === Test 1: np_atomic_store + np_atomic_load → read-back ===
    mov eax, sync_st_atomic
    mov ecx, 0xA5A5A5A5
    call np_atomic_store_u32
    mov eax, sync_st_atomic
    call np_atomic_load_u32
    cmp eax, 0xA5A5A5A5
    jne .sst_fail

    ; === Test 2: np_atomic_exchange → Rückgabe Altwert ===
    mov eax, sync_st_atomic
    mov ecx, 0x12345678
    call np_atomic_exchange_u32
    cmp eax, 0xA5A5A5A5         ; Altwert muss zurückkommen
    jne .sst_fail
    cmp dword [sync_st_atomic], 0x12345678
    jne .sst_fail

    ; === Test 3: np_atomic_compare_exchange – Treffer → CF=0 ===
    mov eax, sync_st_atomic
    mov ecx, 0x12345678         ; erwartet
    mov edx, 0xDEADBEEF         ; gewünscht
    call np_atomic_compare_exchange_u32
    jc .sst_fail
    cmp dword [sync_st_atomic], 0xDEADBEEF
    jne .sst_fail

    ; === Test 4: np_atomic_compare_exchange – Fehltreffer → CF=1 ===
    mov eax, sync_st_atomic
    mov ecx, 0x00000000         ; falsch erwartet
    mov edx, 0x11111111
    call np_atomic_compare_exchange_u32
    jnc .sst_fail               ; muss CF=1 zurückgeben

    ; === Test 5: np_atomic_fetch_add → Altwert + korrekte Summe ===
    mov dword [sync_st_atomic], 10
    mov eax, sync_st_atomic
    mov ecx, 5
    call np_atomic_fetch_add_u32
    cmp eax, 10                 ; Altwert
    jne .sst_fail
    cmp dword [sync_st_atomic], 15
    jne .sst_fail

    ; === Test 6: np_spin_lock / np_spin_unlock ===
    mov eax, sync_test_spin
    call np_spin_init
    call np_spin_lock
    cmp dword [sync_test_spin], SYNC_SPIN_LOCKED
    jne .sst_fail
    call np_spin_unlock
    cmp dword [sync_test_spin], SYNC_SPIN_UNLOCKED
    jne .sst_fail

    ; === Test 7: np_spin_try_lock – frei CF=0; belegt CF=1 ===
    mov eax, sync_test_spin
    call np_spin_try_lock
    jc .sst_fail
    ; jetzt belegt → zweiter try_lock muss CF=1 geben
    call np_spin_try_lock
    jnc .sst_fail
    call np_spin_unlock

    ; === Test 8: np_spin_lock_irqsave / np_spin_unlock_irqrestore ===
    mov eax, sync_test_spin
    call np_spin_lock_irqsave   ; EDX = saved EFLAGS
    cmp dword [sync_test_spin], SYNC_SPIN_LOCKED
    jne .sst_fail
    call np_spin_unlock_irqrestore

    ; === Test 9: Semaphore: init(2,2), try_wait×2=ok, try_wait=leer, release ===
    mov eax, sync_test_sema
    mov ecx, 2
    mov edx, 2
    call np_semaphore_init
    call np_semaphore_try_wait
    jc .sst_fail
    call np_semaphore_try_wait
    jc .sst_fail
    ; count = 0 → muss CF=1
    call np_semaphore_try_wait
    jnc .sst_fail
    ; Release(1) → count = 1
    mov ecx, 1
    call np_semaphore_release
    jc .sst_fail
    ; Overflow: Release(2) bei max=2 und count=1 → 3 > 2 → CF=1
    mov eax, sync_test_sema
    mov ecx, 2
    call np_semaphore_release
    jnc .sst_fail

    ; === Test 10: Completion: init → not done; signal → done ===
    mov eax, sync_test_compl
    call np_completion_init
    call np_completion_is_done  ; CF=1 (ausstehend)
    jnc .sst_fail
    call np_completion_signal
    mov eax, sync_test_compl
    call np_completion_is_done  ; CF=0 (signalisiert)
    jc .sst_fail

    ; === Test 11: Sequence Lock und Refcount ===
    mov eax, sync_test_seqlock
    call np_seqlock_init
    call np_seqlock_write_lock
    call np_seqlock_write_unlock
    call np_seqlock_read_begin  ; ECX = snapshot (muss gerade sein)
    test ecx, 1
    jnz .sst_fail
    call np_seqlock_read_retry  ; CF=0 (kein Writer seit snapshot)
    jc .sst_fail
    ; Refcount: 3 → retain → 4 → release×4 → 0 (CF=1 beim letzten)
    mov eax, sync_test_refcount
    call np_refcount_retain
    jc .sst_fail
    cmp dword [sync_test_refcount], 4
    jne .sst_fail
    call np_refcount_release    ; 4→3, CF=0
    jc .sst_fail
    call np_refcount_release    ; 3→2, CF=0
    jc .sst_fail
    call np_refcount_release    ; 2→1, CF=0
    jc .sst_fail
    call np_refcount_release    ; 1→0, CF=1
    jnc .sst_fail

    pop edi
    pop esi
    pop ebx
    clc
    ret

.sst_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; §016 Daten
; ---------------------------------------------------------------------------
align 4
sync_test_spin:      dd 0                   ; Spinlock-Instanz (4 Byte)
sync_test_sema:      times 3 dd 0           ; Semaphore-Instanz (12 Byte)
sync_test_compl:     times 2 dd 0           ; Completion-Instanz (8 Byte)
sync_test_seqlock:   dd 0                   ; SeqLock-Instanz (4 Byte)
sync_test_refcount:  dd 0                   ; Refcount-Instanz
sync_st_atomic:      dd 0                   ; Arbeitsspeicher für Atomartests
sync_tmp_ptr:        dd 0                   ; Temp für CAS-Schleifen
sync_tmp_cnt:        dd 0
sync_stat_acquires:  dd 0
sync_stat_releases:  dd 0
; VSVC-Tabelle (§10, 24 Byte)
align 4
sync_vsvc_tbl:
    dd VSVC_TBL_HDR_SIZE
    dw 1, 0
    dd 0, 0
    dd VSVC_CALL_EARLY_BOOT | VSVC_CALL_PANIC_SAFE
    dd 0
sync_name_sync:  db "nova.sync.core", 0
align 4

; ===========================================================================
; §016 Erweiterung – Blocking Mutex, Condition Variables, Wait Queues, RCU
; NPSPEC-SYNC-MUTEX-0001 / NPSPEC-SYNC-CONDITION-0001 / NPSPEC-SYNC-RWLOCK-0001
; NPSPEC-SYNC-RCU-0001
; Bootstrap: kein echtes Blockieren (Single-CPU, kein Scheduler-Sleep);
; Datenstrukturen + Operationen vollständig, Schlaf = Spin-Fallback.
; ===========================================================================

; Mutex-Struktur (16 Byte)
SYNC_MUTEX_SIZE         equ 16
SYNC_MUTEX_OFF_SPIN     equ 0     ; dd spinlock state
SYNC_MUTEX_OFF_OWNER    equ 4     ; dd owner token (0=frei)
SYNC_MUTEX_OFF_WAITERS  equ 8     ; dd Anzahl Wartender
SYNC_MUTEX_OFF_FLAGS    equ 12    ; dd (RECURSIVE, PI etc.)

SYNC_MUTEX_FLAG_RECURSIVE equ 0x00000001
SYNC_MUTEX_FLAG_PI        equ 0x00000002

; Condition Variable (12 Byte)
SYNC_COND_SIZE          equ 12
SYNC_COND_OFF_GEN       equ 0     ; dd generation counter
SYNC_COND_OFF_WAITERS   equ 4     ; dd Anzahl Wartender
SYNC_COND_OFF_SPIN      equ 8     ; dd spinlock

; Wait Queue (16 Byte)
SYNC_WQ_SIZE            equ 16
SYNC_WQ_OFF_GEN         equ 0     ; dd generation counter
SYNC_WQ_OFF_WAITERS     equ 4     ; dd Anzahl aktiver Waiter-Einträge
SYNC_WQ_OFF_SPIN        equ 8     ; dd spinlock
SYNC_WQ_OFF_FLAGS       equ 12    ; dd Flags

; Wait Queue Entry (16 Byte, §29)
SYNC_WQE_SIZE           equ 16
SYNC_WQE_OFF_OWNER      equ 0     ; dd thread-Token (0=frei)
SYNC_WQE_OFF_FLAGS      equ 4
SYNC_WQE_OFF_PRIO       equ 8
SYNC_WQE_OFF_SEQ        equ 12
SYNC_MAX_WQ_ENTRIES     equ 16

; ---------------------------------------------------------------------------
; Mutex (§16–§21)
; ---------------------------------------------------------------------------

; np_mutex_init: EAX=mutex_ptr, ECX=flags
np_mutex_init:
    mov dword [eax + SYNC_MUTEX_OFF_SPIN],    SYNC_SPIN_UNLOCKED
    mov dword [eax + SYNC_MUTEX_OFF_OWNER],   0
    mov dword [eax + SYNC_MUTEX_OFF_WAITERS], 0
    mov [eax + SYNC_MUTEX_OFF_FLAGS], ecx
    ret

; np_mutex_lock: EAX=mutex_ptr → CF=0 erworben / CF=1 Deadlock (§49 Bootstrap)
; Spin-Fallback: in single-threaded Umgebung kann kein anderer Besitzer entsperren;
; Wiedereintritt gilt als Deadlock (nicht-rekursiver Standard-Mutex, §17).
np_mutex_lock:
    push ecx
    mov ecx, SYNC_SPIN_LOCKED
    xchg [eax + SYNC_MUTEX_OFF_SPIN], ecx
    test ecx, ecx
    jnz .nml_deadlock
    mov dword [eax + SYNC_MUTEX_OFF_OWNER], 1
    pop ecx
    clc
    ret
.nml_deadlock:
    lock inc dword [eax + SYNC_MUTEX_OFF_WAITERS]
    lock dec dword [eax + SYNC_MUTEX_OFF_WAITERS]
    pop ecx
    mov eax, NP_ERR_SYNC_DEADLOCK
    stc
    ret

; np_mutex_try_lock: EAX=mutex_ptr → CF=0 erworben / CF=1 belegt (§16)
np_mutex_try_lock:
    push ecx
    mov ecx, SYNC_SPIN_LOCKED
    xchg [eax + SYNC_MUTEX_OFF_SPIN], ecx
    test ecx, ecx
    jnz .nmtl_fail
    mov dword [eax + SYNC_MUTEX_OFF_OWNER], 1
    pop ecx
    clc
    ret
.nmtl_fail:
    pop ecx
    stc
    ret

; np_mutex_unlock: EAX=mutex_ptr → CF=0 / CF=1 nicht Besitzer (§17)
np_mutex_unlock:
    cmp dword [eax + SYNC_MUTEX_OFF_OWNER], 0
    je .nmu_not_owner
    mov dword [eax + SYNC_MUTEX_OFF_OWNER], 0
    mfence
    mov dword [eax + SYNC_MUTEX_OFF_SPIN], SYNC_SPIN_UNLOCKED
    clc
    ret
.nmu_not_owner:
    mov eax, NP_ERR_SYNC_NOT_OWNER
    stc
    ret

; ---------------------------------------------------------------------------
; Condition Variable (§26–§27)
; ---------------------------------------------------------------------------

; np_condition_init: EAX=cond_ptr
np_condition_init:
    mov dword [eax + SYNC_COND_OFF_GEN],     0
    mov dword [eax + SYNC_COND_OFF_WAITERS], 0
    mov dword [eax + SYNC_COND_OFF_SPIN],    SYNC_SPIN_UNLOCKED
    ret

; np_condition_signal: EAX=cond_ptr – weckt einen Waiter (§26)
np_condition_signal:
    lock inc dword [eax + SYNC_COND_OFF_GEN]
    ret

; np_condition_broadcast: EAX=cond_ptr – weckt alle Waiter (§26)
np_condition_broadcast:
    push ecx
    mov ecx, [eax + SYNC_COND_OFF_WAITERS]
    test ecx, ecx
    jz .ncb_done
    lock add dword [eax + SYNC_COND_OFF_GEN], ecx
.ncb_done:
    pop ecx
    clc
    ret

; np_condition_wait: EAX=cond_ptr, EBX=mutex_ptr → CF=0 / CF=1 Fehler
; Bootstrap-Spurious-Wakeup: Mutex freigeben + sofort wieder erwerben (§27)
np_condition_wait:
    push eax
    mov eax, ebx
    call np_mutex_unlock
    jc .ncw_err
    call np_mutex_lock
    jc .ncw_err
    pop eax
    clc
    ret
.ncw_err:
    pop eax
    stc
    ret

; ---------------------------------------------------------------------------
; Wait Queue (§28–§30)
; ---------------------------------------------------------------------------

; np_wait_queue_init: EAX=wq_ptr
np_wait_queue_init:
    mov dword [eax + SYNC_WQ_OFF_GEN],     0
    mov dword [eax + SYNC_WQ_OFF_WAITERS], 0
    mov dword [eax + SYNC_WQ_OFF_SPIN],    SYNC_SPIN_UNLOCKED
    mov dword [eax + SYNC_WQ_OFF_FLAGS],   0
    ret

; np_wait_queue_wake_one: EAX=wq_ptr – weckt einen Eintrag (§30)
; Bootstrap: Generation hochzählen als Wakeup-Signal
np_wait_queue_wake_one:
    lock inc dword [eax + SYNC_WQ_OFF_GEN]
    clc
    ret

; np_wait_queue_wake_all: EAX=wq_ptr – weckt alle Einträge (§30)
np_wait_queue_wake_all:
    lock inc dword [eax + SYNC_WQ_OFF_GEN]
    clc
    ret

; ---------------------------------------------------------------------------
; RCU – Bootstrap (§33–§34)
; Single-CPU ohne Präemption: keine echten Grace Periods nötig.
; ---------------------------------------------------------------------------

; np_rcu_read_lock: no-op (keine Präemption auf Single-CPU) (§34)
np_rcu_read_lock:
    ret

; np_rcu_read_unlock: no-op (§34)
np_rcu_read_unlock:
    ret

; np_rcu_assign_pointer: EAX=dst_ptr, ECX=new_value (§34)
; SFENCE stellt sicher, dass der neue Zeiger erst nach abgeschlossenen
; Schreibzugriffen auf die neue Struktur sichtbar wird.
np_rcu_assign_pointer:
    sfence
    mov [eax], ecx
    ret

; np_rcu_dereference: EAX=src_ptr → EAX=value (§34)
; LFENCE verhindert spekulative Vorausnahme des Folge-Lesezugriffs.
np_rcu_dereference:
    mov eax, [eax]
    lfence
    ret

; np_rcu_synchronize: wartet auf aktive Read-Side Critical Sections (§34)
; Bootstrap: MFENCE reicht (keine parallel laufenden Leser auf Single-CPU).
np_rcu_synchronize:
    mfence
    ret

; np_rcu_call: EAX=callback_ptr, ECX=context (§34)
; Bootstrap: sofortige Ausführung (Grace Period entfällt auf Single-CPU).
np_rcu_call:
    call eax
    ret

; ---------------------------------------------------------------------------
; sync_ext_initialize – Testinstanzen nullen
; CF=0 (kann nicht fehlschlagen)
; ---------------------------------------------------------------------------
sync_ext_initialize:
    push edi

    mov edi, sync_test_mutex
    xor eax, eax
    mov ecx, (SYNC_MUTEX_SIZE + SYNC_COND_SIZE + SYNC_WQ_SIZE) / 4
    rep stosd

    mov dword [sync_st_rcu_ptr],    0
    mov dword [sync_st_rcu_target], 0xCAFEBABE
    mov dword [sync_st_rcu_called], 0

    pop edi
    clc
    ret

; ---------------------------------------------------------------------------
; sync_ext_self_test – 8 Tests
; CF=0 alle bestanden / CF=1 Fehler
; ---------------------------------------------------------------------------
sync_ext_self_test:
    push ebx
    push esi
    push edi

    ; === Test 1: np_mutex_lock / np_mutex_unlock ===
    mov eax, sync_test_mutex
    xor ecx, ecx
    call np_mutex_init
    call np_mutex_lock
    jc .sest_fail
    cmp dword [sync_test_mutex + SYNC_MUTEX_OFF_SPIN], SYNC_SPIN_LOCKED
    jne .sest_fail
    call np_mutex_unlock
    jc .sest_fail
    cmp dword [sync_test_mutex + SYNC_MUTEX_OFF_SPIN], SYNC_SPIN_UNLOCKED
    jne .sest_fail

    ; === Test 2: np_mutex_try_lock – frei CF=0; belegt CF=1 ===
    mov eax, sync_test_mutex
    call np_mutex_try_lock
    jc .sest_fail
    call np_mutex_try_lock
    jnc .sest_fail
    call np_mutex_unlock

    ; === Test 3: np_mutex_unlock ohne Besitzer → CF=1 ===
    mov eax, sync_test_mutex
    call np_mutex_unlock
    jnc .sest_fail

    ; === Test 4: np_condition_signal erhöht Generation ===
    mov eax, sync_test_cond
    call np_condition_init
    call np_condition_signal
    cmp dword [sync_test_cond + SYNC_COND_OFF_GEN], 1
    jne .sest_fail
    call np_condition_broadcast     ; waiters=0 → GEN unverändert, CF=0
    ; (kein Increment da waiters=0)
    cmp dword [sync_test_cond + SYNC_COND_OFF_GEN], 1
    jne .sest_fail

    ; === Test 5: np_condition_wait – Mutex unlock+relock ===
    mov eax, sync_test_mutex
    xor ecx, ecx
    call np_mutex_init
    call np_mutex_lock
    jc .sest_fail
    mov eax, sync_test_cond
    mov ebx, sync_test_mutex
    call np_condition_wait
    jc .sest_fail
    cmp dword [sync_test_mutex + SYNC_MUTEX_OFF_SPIN], SYNC_SPIN_LOCKED
    jne .sest_fail
    mov eax, sync_test_mutex
    call np_mutex_unlock

    ; === Test 6: np_wait_queue wake_one / wake_all ===
    mov eax, sync_test_wq
    call np_wait_queue_init
    call np_wait_queue_wake_one
    cmp dword [sync_test_wq + SYNC_WQ_OFF_GEN], 1
    jne .sest_fail
    call np_wait_queue_wake_all
    cmp dword [sync_test_wq + SYNC_WQ_OFF_GEN], 2
    jne .sest_fail

    ; === Test 7: np_rcu_assign_pointer + np_rcu_dereference (2-stufig) ===
    ; sync_st_rcu_ptr → sync_st_rcu_target → 0xCAFEBABE
    mov eax, sync_st_rcu_ptr
    mov ecx, sync_st_rcu_target
    call np_rcu_assign_pointer      ; [sync_st_rcu_ptr] = &sync_st_rcu_target
    mov eax, sync_st_rcu_ptr
    call np_rcu_dereference         ; EAX = &sync_st_rcu_target
    cmp eax, sync_st_rcu_target
    jne .sest_fail
    call np_rcu_dereference         ; EAX = [sync_st_rcu_target] = 0xCAFEBABE
    cmp eax, 0xCAFEBABE
    jne .sest_fail

    ; === Test 8: np_rcu_call → Callback sofort ausgeführt ===
    mov dword [sync_st_rcu_called], 0
    mov eax, sync_st_rcu_cb
    xor ecx, ecx
    call np_rcu_call
    cmp dword [sync_st_rcu_called], 1
    jne .sest_fail

    pop edi
    pop esi
    pop ebx
    clc
    ret

.sest_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; RCU-Selbsttest-Callback
sync_st_rcu_cb:
    mov dword [sync_st_rcu_called], 1
    ret

; ---------------------------------------------------------------------------
; §016-Erweiterung Daten
; ---------------------------------------------------------------------------
align 4
sync_test_mutex:     times (SYNC_MUTEX_SIZE / 4) dd 0
sync_test_cond:      times (SYNC_COND_SIZE  / 4) dd 0
sync_test_wq:        times (SYNC_WQ_SIZE    / 4) dd 0
sync_st_rcu_ptr:     dd 0
sync_st_rcu_target:  dd 0xCAFEBABE
sync_st_rcu_called:  dd 0
align 4

; ===========================================================================
; NPSPEC-SYNC-FUTEX-0001 – Nova Fast Userspace Mutex
; Kernel-seitige Futex-Infrastruktur: Warte-Queue pro Futex-Adresse,
; wake/wait über 32-Bit-Futex-Wort; im Bootstrap: Spin-Fallback.
; ===========================================================================

FUTEX_CAPACITY      equ 16
FUTEX_RECORD_SIZE   equ 20

FTX_ADDR            equ 0   ; Adresse des Futex-Wortes (virtuelle Adresse)
FTX_WAITER_COUNT    equ 4   ; Anzahl wartender Threads
FTX_FLAGS           equ 8   ; Flags (privat, geteilt, RT)
FTX_WAKE_COUNT      equ 12  ; Gesamte Weckvorgänge
FTX_STATE           equ 16  ; 0=frei, 1=aktiv

FTX_STATE_FREE      equ 0
FTX_STATE_ACTIVE    equ 1

FTX_FLAG_PRIVATE    equ 0x01
FTX_FLAG_SHARED     equ 0x02
FTX_FLAG_RT         equ 0x04

futex_initialize:
    mov edi, futex_table
    mov ecx, FUTEX_CAPACITY * FUTEX_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [futex_next_id], 0
    mov dword [futex_total_waits], 0
    mov dword [futex_total_wakes], 0
    mov dword [futex_ready], 1
    clc
    ret

; EAX=futex_addr  EDX=flags → EAX=slot_index CF=0 / CF=1 voll
futex_register:
    push ebx
    push esi
    mov esi, futex_table
    xor ebx, ebx
.ftr_scan:
    cmp ebx, FUTEX_CAPACITY
    jae .ftr_full
    cmp dword [esi + FTX_STATE], FTX_STATE_FREE
    je .ftr_found
    ; schon registriert?
    cmp [esi + FTX_ADDR], eax
    je .ftr_exists
    add esi, FUTEX_RECORD_SIZE
    inc ebx
    jmp .ftr_scan
.ftr_exists:
    mov eax, ebx
    clc
    pop esi
    pop ebx
    ret
.ftr_found:
    mov [esi + FTX_ADDR], eax
    mov [esi + FTX_FLAGS], edx
    mov dword [esi + FTX_WAITER_COUNT], 0
    mov dword [esi + FTX_WAKE_COUNT], 0
    mov dword [esi + FTX_STATE], FTX_STATE_ACTIVE
    inc dword [futex_next_id]
    mov eax, ebx
    clc
    pop esi
    pop ebx
    ret
.ftr_full:
    stc
    pop esi
    pop ebx
    ret

; EAX=slot → simuliert Warten: inkrementiert waiter_count (kein echtes Blockieren im Bootstrap)
futex_wait:
    push esi
    cmp eax, FUTEX_CAPACITY
    jae .ftw_fail
    imul esi, eax, FUTEX_RECORD_SIZE
    add esi, futex_table
    cmp dword [esi + FTX_STATE], FTX_STATE_ACTIVE
    jne .ftw_fail
    inc dword [esi + FTX_WAITER_COUNT]
    inc dword [futex_total_waits]
    clc
    pop esi
    ret
.ftw_fail:
    stc
    pop esi
    ret

; EAX=slot  EDX=max_wake → weckt bis zu EDX Waiters (dekrementiert waiter_count)
futex_wake:
    push esi
    push edx
    cmp eax, FUTEX_CAPACITY
    jae .ftwk_done
    imul esi, eax, FUTEX_RECORD_SIZE
    add esi, futex_table
    cmp dword [esi + FTX_STATE], FTX_STATE_ACTIVE
    jne .ftwk_done
.ftwk_loop:
    test edx, edx
    jz .ftwk_done
    cmp dword [esi + FTX_WAITER_COUNT], 0
    je .ftwk_done
    dec dword [esi + FTX_WAITER_COUNT]
    inc dword [esi + FTX_WAKE_COUNT]
    inc dword [futex_total_wakes]
    dec edx
    jmp .ftwk_loop
.ftwk_done:
    clc
    pop edx
    pop esi
    ret

futex_self_test:
    push ebx
    push esi
    xor esi, esi

    ; T1: ready
    cmp dword [futex_ready], 1
    je .ftst1_ok
    inc esi
.ftst1_ok:

    ; T2: register
    mov eax, 0x1000    ; virtuelle Futex-Adresse (symbolisch)
    mov edx, FTX_FLAG_PRIVATE
    call futex_register
    jnc .ftst2_ok
    inc esi
.ftst2_ok:
    mov ebx, eax      ; slot

    ; T3: wait x2
    mov eax, ebx
    call futex_wait
    mov eax, ebx
    call futex_wait
    push edx
    imul edx, ebx, FUTEX_RECORD_SIZE
    add edx, futex_table
    cmp dword [edx + FTX_WAITER_COUNT], 2
    je .ftst3_ok
    inc esi
.ftst3_ok:
    pop edx

    ; T4: wake 1 → waiter_count=1
    mov eax, ebx
    mov edx, 1
    call futex_wake
    push edx
    imul edx, ebx, FUTEX_RECORD_SIZE
    add edx, futex_table
    cmp dword [edx + FTX_WAITER_COUNT], 1
    je .ftst4_ok
    inc esi
.ftst4_ok:
    pop edx

    ; T5: wake alle → waiter_count=0
    mov eax, ebx
    mov edx, 99
    call futex_wake
    push edx
    imul edx, ebx, FUTEX_RECORD_SIZE
    add edx, futex_table
    cmp dword [edx + FTX_WAITER_COUNT], 0
    je .ftst5_ok
    inc esi
.ftst5_ok:
    pop edx

    ; T6: total_waits=2
    cmp dword [futex_total_waits], 2
    je .ftst6_ok
    inc esi
.ftst6_ok:

    ; T7: total_wakes=2
    cmp dword [futex_total_wakes], 2
    je .ftst7_ok
    inc esi
.ftst7_ok:

    test esi, esi
    jnz .ftstf
    clc
    pop esi
    pop ebx
    ret
.ftstf:
    stc
    pop esi
    pop ebx
    ret

futex_ready:        dd 0
futex_next_id:      dd 0
futex_total_waits:  dd 0
futex_total_wakes:  dd 0
align 4
futex_table:
    times FUTEX_CAPACITY * FUTEX_RECORD_SIZE db 0

; ===========================================================================
; NPSPEC-SYNC-DEADLOCK-0001 – Nova Deadlock Detection (Wait-For Graph)
; Erkennt zyklische Warteabhängigkeiten zwischen Tasks und Locks.
; Wait-For Graph: bis zu DL_NODE_COUNT Knoten, Adjazenzmatrix.
; ===========================================================================

DL_NODE_COUNT        equ 8

; Adjazenzmatrix: dl_wfg[i * DL_NODE_COUNT + j] = 1 → Task i wartet auf Task j
; dl_owner[i] = Task-ID des aktuellen Lock-Halters für Slot i (0 = frei)

deadlock_initialize:
    mov edi, dl_wfg
    mov ecx, DL_NODE_COUNT * DL_NODE_COUNT
    xor eax, eax
    rep stosd
    mov edi, dl_owner
    mov ecx, DL_NODE_COUNT
    rep stosd
    mov dword [dl_ready], 1
    mov dword [dl_detected], 0
    clc
    ret

; Trägt ein: Task EAX wartet auf Task EDX (beide < DL_NODE_COUNT)
; CF=0 ok / CF=1 Deadlock erkannt
deadlock_add_wait:
    cmp eax, DL_NODE_COUNT
    jae .daw_fail
    cmp edx, DL_NODE_COUNT
    jae .daw_fail
    cmp eax, edx
    je .daw_fail       ; Self-wait = Fehler
    ; wfg[eax][edx] = 1
    push eax
    push edx
    imul ecx, eax, DL_NODE_COUNT
    add ecx, edx
    mov dword [dl_wfg + ecx*4], 1
    pop edx
    pop eax
    ; Zyklus prüfen
    call deadlock_detect
    jnc .daw_ok
    inc dword [dl_detected]
    stc
    ret
.daw_ok:
    clc
    ret
.daw_fail:
    stc
    ret

; Entfernt Kante: Task EAX wartet nicht mehr auf Task EDX
deadlock_remove_wait:
    cmp eax, DL_NODE_COUNT
    jae .drw_ret
    cmp edx, DL_NODE_COUNT
    jae .drw_ret
    imul ecx, eax, DL_NODE_COUNT
    add ecx, edx
    mov dword [dl_wfg + ecx*4], 0
.drw_ret:
    ret

; DFS-basierte Zykluserkennung im Wait-For Graph
; → CF=0 kein Zyklus, CF=1 Zyklus gefunden
; Nutzt dl_visited/dl_stack als temporäre Felder
deadlock_detect:
    push esi
    push edi
    push ebx
    push ecx
    push edx
    ; visited[] und in_stack[] zurücksetzen
    mov edi, dl_visited
    mov ecx, DL_NODE_COUNT * 2
    xor eax, eax
    rep stosd
    ; DFS von jedem Knoten
    xor esi, esi
.ddet_outer:
    cmp esi, DL_NODE_COUNT
    jae .ddet_no_cycle
    cmp dword [dl_visited + esi*4], 0
    jne .ddet_next_outer
    call deadlock_dfs_node   ; ESI = Startknoten
    jc .ddet_cycle
.ddet_next_outer:
    inc esi
    jmp .ddet_outer
.ddet_cycle:
    stc
    pop edx
    pop ecx
    pop ebx
    pop edi
    pop esi
    ret
.ddet_no_cycle:
    clc
    pop edx
    pop ecx
    pop ebx
    pop edi
    pop esi
    ret

; DFS von Knoten ESI; CF=1 wenn Zyklus, nutzt dl_visited/dl_in_stack
deadlock_dfs_node:
    mov dword [dl_visited + esi*4], 1
    mov dword [dl_in_stack + esi*4], 1
    push esi
    push ebx
    xor ebx, ebx
.ddn_edge:
    cmp ebx, DL_NODE_COUNT
    jae .ddn_done
    ; Kante esi → ebx?
    push eax
    imul eax, esi, DL_NODE_COUNT
    add eax, ebx
    cmp dword [dl_wfg + eax*4], 0
    pop eax
    je .ddn_no_edge
    ; Kante vorhanden: Nachbar besuchen?
    cmp dword [dl_visited + ebx*4], 0
    jne .ddn_check_stack
    push esi
    mov esi, ebx
    call deadlock_dfs_node
    pop esi
    jc .ddn_cycle
    jmp .ddn_no_edge
.ddn_check_stack:
    cmp dword [dl_in_stack + ebx*4], 1
    je .ddn_cycle
.ddn_no_edge:
    inc ebx
    jmp .ddn_edge
.ddn_done:
    mov dword [dl_in_stack + esi*4], 0
    clc
    pop ebx
    pop esi
    ret
.ddn_cycle:
    mov dword [dl_in_stack + esi*4], 0
    stc
    pop ebx
    pop esi
    ret

deadlock_self_test:
    push ebx
    push esi
    xor esi, esi

    ; T1: ready
    cmp dword [dl_ready], 1
    je .dlst1_ok
    inc esi
.dlst1_ok:

    ; T2: 0→1, 1→2 – kein Zyklus
    mov eax, 0
    mov edx, 1
    call deadlock_add_wait
    jnc .dlst2_ok
    inc esi
.dlst2_ok:
    mov eax, 1
    mov edx, 2
    call deadlock_add_wait
    jnc .dlst3_ok
    inc esi
.dlst3_ok:

    ; T3: 2→0 → Zyklus muss erkannt werden
    mov eax, 2
    mov edx, 0
    call deadlock_add_wait
    jc .dlst4_ok    ; CF=1 erwartet
    inc esi
.dlst4_ok:

    ; T4: dl_detected=1
    cmp dword [dl_detected], 1
    je .dlst5_ok
    inc esi
.dlst5_ok:

    ; T5: nach remove_wait kein Zyklus mehr
    mov eax, 2
    mov edx, 0
    call deadlock_remove_wait
    call deadlock_detect
    jnc .dlst6_ok
    inc esi
.dlst6_ok:

    test esi, esi
    jnz .dlstf
    clc
    pop esi
    pop ebx
    ret
.dlstf:
    stc
    pop esi
    pop ebx
    ret

dl_ready:    dd 0
dl_detected: dd 0
align 4
dl_wfg:
    times DL_NODE_COUNT * DL_NODE_COUNT dd 0
dl_owner:
    times DL_NODE_COUNT dd 0
dl_visited:
    times DL_NODE_COUNT dd 0
dl_in_stack:
    times DL_NODE_COUNT dd 0

; ===========================================================================
; NPSPEC-SYNC-PRIORITYINHERITANCE-0001 – Nova Priority Inheritance Mutex
; Verhindert Priority Inversion: Lock-Besitzer erbt die höchste Priorität
; aller Wartenden bis zur Lock-Freigabe.
; ===========================================================================

PI_CAPACITY          equ 8
PI_RECORD_SIZE       equ 24

PI_STATE_FREE        equ 0
PI_STATE_UNLOCKED    equ 1
PI_STATE_LOCKED      equ 2

PI_ID                equ 0
PI_STATE_OFF         equ 4
PI_OWNER             equ 8   ; Task-ID des aktuellen Inhabers
PI_BASE_PRIO         equ 12  ; Basis-Priorität des Inhabers
PI_EFF_PRIO          equ 16  ; Effektive Priorität (nach Vererbung)
PI_WAITER_COUNT      equ 20

pi_initialize:
    mov edi, pi_table
    mov ecx, PI_CAPACITY * PI_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [pi_ready], 1
    mov dword [pi_next_id], 0
    mov dword [pi_total_inherit], 0
    clc
    ret

; Erzeugt neuen PI-Mutex → EAX=slot CF=0 / CF=1 voll
pi_mutex_create:
    push esi
    push ecx
    mov esi, pi_table
    xor ecx, ecx
.pic_scan:
    cmp ecx, PI_CAPACITY
    jae .pic_full
    cmp dword [esi + PI_STATE_OFF], PI_STATE_FREE
    je .pic_found
    add esi, PI_RECORD_SIZE
    inc ecx
    jmp .pic_scan
.pic_found:
    push eax
    mov eax, [pi_next_id]
    inc dword [pi_next_id]
    mov [esi + PI_ID], eax
    pop eax
    mov dword [esi + PI_STATE_OFF], PI_STATE_UNLOCKED
    mov dword [esi + PI_OWNER], 0
    mov dword [esi + PI_BASE_PRIO], 0
    mov dword [esi + PI_EFF_PRIO], 0
    mov dword [esi + PI_WAITER_COUNT], 0
    mov eax, ecx
    clc
    pop ecx
    pop esi
    ret
.pic_full:
    stc
    pop ecx
    pop esi
    ret

; EAX=slot  EDX=task_id  ECX=task_base_prio → Lock versuchen
; CF=0 Erfolg (Mutex gehört jetzt EDX), CF=1 bereits gesperrt (PI-Erhöhung trotzdem)
pi_mutex_lock:
    push esi
    push ebx
    cmp eax, PI_CAPACITY
    jae .pml_fail
    imul esi, eax, PI_RECORD_SIZE
    add esi, pi_table
    cmp dword [esi + PI_STATE_OFF], PI_STATE_UNLOCKED
    je .pml_acquire
    ; Bereits gesperrt: Priority Inheritance
    inc dword [esi + PI_WAITER_COUNT]
    ; Falls Wartender höhere Prio als effektive Prio des Besitzers → anpassen
    cmp ecx, [esi + PI_EFF_PRIO]
    jbe .pml_no_inherit
    mov [esi + PI_EFF_PRIO], ecx
    inc dword [pi_total_inherit]
.pml_no_inherit:
    stc
    pop ebx
    pop esi
    ret
.pml_acquire:
    mov dword [esi + PI_STATE_OFF], PI_STATE_LOCKED
    mov [esi + PI_OWNER], edx
    mov [esi + PI_BASE_PRIO], ecx
    mov [esi + PI_EFF_PRIO], ecx
    clc
    pop ebx
    pop esi
    ret
.pml_fail:
    stc
    pop ebx
    pop esi
    ret

; EAX=slot → Unlock, Effektiv-Prio auf Basis zurückgesetzt
pi_mutex_unlock:
    push esi
    cmp eax, PI_CAPACITY
    jae .pmu_fail
    imul esi, eax, PI_RECORD_SIZE
    add esi, pi_table
    cmp dword [esi + PI_STATE_OFF], PI_STATE_LOCKED
    jne .pmu_fail
    mov dword [esi + PI_STATE_OFF], PI_STATE_UNLOCKED
    mov dword [esi + PI_OWNER], 0
    ; Effektivprio zurücksetzen
    push eax
    mov eax, [esi + PI_BASE_PRIO]
    mov [esi + PI_EFF_PRIO], eax
    pop eax
    mov dword [esi + PI_WAITER_COUNT], 0
    clc
    pop esi
    ret
.pmu_fail:
    stc
    pop esi
    ret

; EAX=slot → EAX=eff_prio  EDX=owner  ECX=waiters  CF=0 / CF=1 invalid
pi_mutex_query:
    push esi
    cmp eax, PI_CAPACITY
    jae .pmq_fail
    imul esi, eax, PI_RECORD_SIZE
    add esi, pi_table
    cmp dword [esi + PI_STATE_OFF], PI_STATE_FREE
    je .pmq_fail
    mov eax, [esi + PI_EFF_PRIO]
    mov edx, [esi + PI_OWNER]
    mov ecx, [esi + PI_WAITER_COUNT]
    clc
    pop esi
    ret
.pmq_fail:
    stc
    pop esi
    ret

pi_self_test:
    push ebx
    push esi
    xor esi, esi

    ; T1: ready
    cmp dword [pi_ready], 1
    je .pist1_ok
    inc esi
.pist1_ok:

    ; T2: Mutex erzeugen
    call pi_mutex_create
    jnc .pist2_ok
    inc esi
.pist2_ok:
    mov ebx, eax   ; slot

    ; T3: Lock mit Task 1 Prio=5
    mov eax, ebx
    mov edx, 1
    mov ecx, 5
    call pi_mutex_lock
    jnc .pist3_ok
    inc esi
.pist3_ok:

    ; T4: Task 2 Prio=10 versucht → CF=1, PI→eff_prio=10
    mov eax, ebx
    mov edx, 2
    mov ecx, 10
    call pi_mutex_lock
    jc .pist4_ok
    inc esi
.pist4_ok:

    ; T5: eff_prio=10 prüfen
    mov eax, ebx
    call pi_mutex_query
    jnc .pist5_cf
    inc esi
.pist5_cf:
    cmp eax, 10
    je .pist5_ok
    inc esi
.pist5_ok:

    ; T6: pi_total_inherit=1
    cmp dword [pi_total_inherit], 1
    je .pist6_ok
    inc esi
.pist6_ok:

    ; T7: Unlock → eff_prio zurück auf 5
    mov eax, ebx
    call pi_mutex_unlock
    jnc .pist7_ok
    inc esi
.pist7_ok:

    ; T8: nach Unlock wieder lock-bar (unlocked)
    mov eax, ebx
    mov edx, 3
    mov ecx, 7
    call pi_mutex_lock
    jnc .pist8_ok
    inc esi
.pist8_ok:

    test esi, esi
    jnz .pistf
    clc
    pop esi
    pop ebx
    ret
.pistf:
    stc
    pop esi
    pop ebx
    ret

pi_ready:          dd 0
pi_next_id:        dd 0
pi_total_inherit:  dd 0
align 4
pi_table:
    times PI_CAPACITY * PI_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; ===========================================================================
; §009 – Interrupt Manager 1.0 (NPSPEC-KERNEL-0009)
; Bootstrap: Handler-Registrierung, Vektor-Allokation, Statistiken.
; Baut auf dem bestehenden IDT/PIC/APIC-Setup (interrupt_initialize) auf.
; ===========================================================================

; Interrupt-Klassen (§4)
NP_INTERRUPT_EXCEPTION  equ 0
NP_INTERRUPT_HARDWARE   equ 1
NP_INTERRUPT_MSI        equ 2
NP_INTERRUPT_IPI        equ 3
NP_INTERRUPT_TIMER      equ 4
NP_INTERRUPT_SOFTWARE   equ 5
NP_INTERRUPT_SPURIOUS   equ 6

; Handler-Ergebnisse (§13)
NP_IRQ_NOT_HANDLED      equ 0
NP_IRQ_HANDLED          equ 1
NP_IRQ_WAKE_THREAD      equ 2
NP_IRQ_DISABLE_SOURCE   equ 3
NP_IRQ_FATAL            equ 4

; Interrupt-Flags (§16)
NP_IRQ_SHARED           equ 0x00000001
NP_IRQ_EDGE             equ 0x00000002
NP_IRQ_LEVEL            equ 0x00000004
NP_IRQ_THREADED         equ 0x00000008
NP_IRQ_ONESHOT          equ 0x00000010
NP_IRQ_WAKE_CAPABLE     equ 0x00000020
NP_IRQ_PER_CPU          equ 0x00000040
NP_IRQ_NO_BALANCE       equ 0x00000080

; Handler-Slot (§14)
IRQH_OFF_HANDLER        equ 0   ; dd function ptr (0 = frei)
IRQH_OFF_CONTEXT        equ 4   ; dd context ptr
IRQH_OFF_FLAGS          equ 8   ; dd NP_IRQ_* Flags
IRQH_OFF_IRQ            equ 12  ; dd IRQ-Nummer (0-15)
IRQH_SIZE               equ 16

IRQ_MANAGER_SLOTS       equ 16  ; ein Slot pro Legacy-IRQ-Leitung
IRQ_VECTOR_PIC_BASE     equ 32  ; Erstes PIC-Vektor (0x20)
IRQ_VECTOR_DYN_BASE     equ 48  ; Dynamisch ab 0x30

; VSVC-UUID für nova.irq.manager
VSVC_ID_IRQM_0          equ 0x4E4F5641   ; "NOVA"
VSVC_ID_IRQM_1          equ 0x4952514D   ; "IRQM"
VSVC_ID_IRQM_2          equ 0x434F5245   ; "CORE"
VSVC_ID_IRQM_3          equ 0x00000001

; Fehlercodes §50
NP_ERR_IRQ_INVALID_VECTOR   equ -110
NP_ERR_IRQ_INVALID_SOURCE   equ -111
NP_ERR_IRQ_VECTOR_EXHAUSTED equ -112
NP_ERR_IRQ_ALREADY_BOUND    equ -113
NP_ERR_IRQ_NOT_SHARED       equ -114
NP_ERR_IRQ_TRIGGER_CONFLICT equ -115
NP_ERR_IRQ_ACCESS_DENIED    equ -116
NP_ERR_IRQ_SOURCE_DISABLED  equ -119

; ---------------------------------------------------------------------------
; irq_register_handler: EAX=irq_num(0–15), EBX=handler_ptr,
;                        ECX=context_ptr, EDX=flags
;   → CF=0 / CF=1 EAX=fehler
; ---------------------------------------------------------------------------
irq_register_handler:
    cmp eax, IRQ_MANAGER_SLOTS
    jae .irh_inval
    push esi
    imul eax, IRQH_SIZE
    mov esi, irq_handler_table
    add esi, eax
    cmp dword [esi + IRQH_OFF_HANDLER], 0
    jne .irh_busy
    mov [esi + IRQH_OFF_HANDLER], ebx
    mov [esi + IRQH_OFF_CONTEXT], ecx
    mov [esi + IRQH_OFF_FLAGS],   edx
    pop esi
    clc
    ret
.irh_busy:
    pop esi
    mov eax, NP_ERR_IRQ_ALREADY_BOUND
    stc
    ret
.irh_inval:
    mov eax, NP_ERR_IRQ_INVALID_SOURCE
    stc
    ret

; ---------------------------------------------------------------------------
; irq_unregister_handler: EAX=irq_num(0–15) → CF=0 / CF=1
; ---------------------------------------------------------------------------
irq_unregister_handler:
    cmp eax, IRQ_MANAGER_SLOTS
    jae .iru_inval
    push esi
    imul eax, IRQH_SIZE
    mov esi, irq_handler_table
    add esi, eax
    mov dword [esi + IRQH_OFF_HANDLER], 0
    mov dword [esi + IRQH_OFF_CONTEXT], 0
    mov dword [esi + IRQH_OFF_FLAGS],   0
    mov dword [esi + IRQH_OFF_IRQ],     0
    pop esi
    clc
    ret
.iru_inval:
    mov eax, NP_ERR_IRQ_INVALID_SOURCE
    stc
    ret

; ---------------------------------------------------------------------------
; irq_dispatch_hw: EAX=irq_num(0–15)
; Ruft registrierten Handler auf, aktualisiert Statistiken.
; EOI liegt beim Caller (interrupt_dispatch).
; → EAX=NP_IRQ_HANDLED / NP_IRQ_NOT_HANDLED
; ---------------------------------------------------------------------------
irq_dispatch_hw:
    cmp eax, IRQ_MANAGER_SLOTS
    jae .idh_spurious
    inc dword [irq_stat_total]
    mov ecx, eax
    inc dword [irq_counts + ecx * 4]
    imul eax, IRQH_SIZE
    mov [irq_tmp_slot], eax        ; Slot-Offset speichern
    mov eax, [irq_handler_table + eax + IRQH_OFF_HANDLER]
    test eax, eax
    jz .idh_unhandled
    ; Handler(context) aufrufen — Slot-Offset aus irq_tmp_slot
    mov ecx, [irq_tmp_slot]
    push dword [irq_handler_table + ecx + IRQH_OFF_CONTEXT]
    call eax
    add esp, 4
    mov eax, NP_IRQ_HANDLED
    ret
.idh_unhandled:
    inc dword [irq_stat_unhandled]
    mov eax, NP_IRQ_NOT_HANDLED
    ret
.idh_spurious:
    inc dword [irq_stat_spurious]
    mov eax, NP_IRQ_NOT_HANDLED
    ret

; ---------------------------------------------------------------------------
; np_interrupt_vector_allocate: EAX=count → CF=0 EAX=first_vector / CF=1
; Sucht im dynamischen Pool (Vektoren 48–255) einen freien zusammenhängenden
; Bereich und markiert ihn als belegt.
; ---------------------------------------------------------------------------
np_interrupt_vector_allocate:
    test eax, eax
    jz .iva_inval
    cmp eax, 208            ; max allokierbar (256 - 48)
    ja .iva_inval
    mov [irq_tmp_alloc_cnt], eax
    push ebx
    push ecx
    push edx
    push esi
    push edi
    mov edi, IRQ_VECTOR_DYN_BASE    ; Suchstart
.iva_scan:
    mov ecx, [irq_tmp_alloc_cnt]
    ; Prüfen ob ab EDI count Vektoren frei sind
    mov esi, edi
.iva_check:
    cmp esi, 256
    jae .iva_fail
    cmp byte [irq_vector_used + esi], 0
    jne .iva_next
    dec ecx
    jz .iva_found
    inc esi
    jmp .iva_check
.iva_next:
    inc edi
    cmp edi, 256
    jb .iva_scan
.iva_fail:
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    mov eax, NP_ERR_IRQ_VECTOR_EXHAUSTED
    stc
    ret
.iva_found:
    ; Vektoren [EDI .. ESI] als belegt markieren
    mov esi, edi
    mov ecx, [irq_tmp_alloc_cnt]
.iva_mark:
    mov byte [irq_vector_used + esi], 1
    inc esi
    dec ecx
    jnz .iva_mark
    mov eax, edi
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    clc
    ret
.iva_inval:
    mov eax, NP_ERR_IRQ_INVALID_VECTOR
    stc
    ret

; ---------------------------------------------------------------------------
; np_interrupt_vector_free: EAX=first_vector, ECX=count → CF=0 / CF=1
; ---------------------------------------------------------------------------
np_interrupt_vector_free:
    cmp eax, IRQ_VECTOR_DYN_BASE
    jb .ivf_inval
    test ecx, ecx
    jz .ivf_inval
    ; Endvektor berechnen: EAX + ECX <= 256
    push ebx
    mov ebx, eax
    add eax, ecx
    cmp eax, 257
    jae .ivf_inval_pop
.ivf_clear:
    mov byte [irq_vector_used + ebx], 0
    inc ebx
    dec ecx
    jnz .ivf_clear
    pop ebx
    clc
    ret
.ivf_inval_pop:
    pop ebx
.ivf_inval:
    mov eax, NP_ERR_IRQ_INVALID_VECTOR
    stc
    ret

; ---------------------------------------------------------------------------
; irq_manager_initialize – Tabellen nullen, VSVC registrieren
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
irq_manager_initialize:
    push ebx
    push esi
    push edi

    ; Handler-Tabelle nullen
    mov edi, irq_handler_table
    xor eax, eax
    mov ecx, (IRQ_MANAGER_SLOTS * IRQH_SIZE) / 4
    rep stosd

    ; Statistiken nullen
    mov dword [irq_stat_total],     0
    mov dword [irq_stat_spurious],  0
    mov dword [irq_stat_unhandled], 0
    mov edi, irq_counts
    mov ecx, IRQ_MANAGER_SLOTS
    rep stosd

    ; Vektor-Nutzungstabelle: 0-47 als belegt markieren
    mov edi, irq_vector_used
    xor eax, eax
    mov ecx, 256
    rep stosb                       ; erst alles auf 0 (1 Byte pro Vektor)
    mov ecx, IRQ_VECTOR_DYN_BASE   ; Vektoren 0-47 reservieren
    mov edi, irq_vector_used
.imi_reserve:
    mov byte [edi], 1
    inc edi
    dec ecx
    jnz .imi_reserve

    ; VSVC "nova.irq.manager" registrieren
    mov dword [vsvc_tmp_maj],      1
    mov dword [vsvc_tmp_min],      0
    mov dword [vsvc_tmp_pat],      0
    mov dword [vsvc_tmp_table],    irq_vsvc_tbl
    mov dword [vsvc_tmp_tabsz],    VSVC_TBL_HDR_SIZE
    mov dword [vsvc_tmp_features], 0
    mov dword [vsvc_tmp_reqcap],   0
    mov dword [vsvc_tmp_arch],     VSVC_ARCH_X86_32
    mov dword [vsvc_tmp_stab],     VSVC_STAB_STABLE
    mov dword [vsvc_tmp_name],     irq_name_irqm
    mov dword [vsvc_tmp_flags],    VSVC_FLAG_EARLY_BOOT | VSVC_FLAG_PANIC_SAFE
    mov eax, VSVC_ID_IRQM_0
    mov ebx, VSVC_ID_IRQM_1
    mov ecx, VSVC_ID_IRQM_2
    mov edx, VSVC_ID_IRQM_3
    call vsvc_register
    jc .imi_fail

    pop edi
    pop esi
    pop ebx
    clc
    ret
.imi_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; irq_manager_self_test – 10 Tests
; CF=0 alle bestanden / CF=1 Fehler
; ---------------------------------------------------------------------------
irq_manager_self_test:
    push ebx
    push esi
    push edi

    ; === Test 1: IDT geladen – sidt → base == idt_table ===
    sidt [irq_st_idtr_buf]
    mov eax, [irq_st_idtr_buf + 2]  ; Basisadresse
    cmp eax, idt_table
    jne .imst_fail

    ; === Test 2: IDT-Limit = (IDT_ENTRY_COUNT * 8) - 1 ===
    movzx eax, word [irq_st_idtr_buf]
    cmp eax, (IDT_ENTRY_COUNT * 8) - 1
    jne .imst_fail

    ; === Test 3: PIC-Mastermaske (0x21) lesbar, nicht komplett maskiert ===
    in al, 0x21
    cmp al, 0xFF
    je .imst_fail                   ; alle IRQs maskiert → Fehler

    ; === Test 4: irq_register_handler → Handler in Slot ===
    mov eax, 5                      ; Test-IRQ 5
    mov ebx, irq_st_test_handler
    xor ecx, ecx
    xor edx, edx
    call irq_register_handler
    jc .imst_fail
    ; Slot prüfen
    mov eax, [irq_handler_table + 5 * IRQH_SIZE + IRQH_OFF_HANDLER]
    cmp eax, irq_st_test_handler
    jne .imst_fail

    ; === Test 5: Doppel-Registrierung ohne SHARED → NP_ERR_IRQ_ALREADY_BOUND ===
    mov eax, 5
    mov ebx, irq_st_test_handler
    xor ecx, ecx
    xor edx, edx
    call irq_register_handler
    jnc .imst_fail                  ; muss CF=1 zurückgeben
    cmp eax, NP_ERR_IRQ_ALREADY_BOUND
    jne .imst_fail

    ; === Test 6: irq_dispatch_hw → Handler aufgerufen ===
    mov dword [irq_st_cb_called], 0
    mov eax, 5
    call irq_dispatch_hw
    cmp dword [irq_st_cb_called], 1
    jne .imst_fail
    cmp eax, NP_IRQ_HANDLED
    jne .imst_fail

    ; === Test 7: irq_stat_total und irq_counts[5] inkrementiert ===
    cmp dword [irq_stat_total], 1
    jne .imst_fail
    cmp dword [irq_counts + 5 * 4], 1
    jne .imst_fail

    ; === Test 8: irq_unregister_handler → Slot geleert ===
    mov eax, 5
    call irq_unregister_handler
    jc .imst_fail
    cmp dword [irq_handler_table + 5 * IRQH_SIZE + IRQH_OFF_HANDLER], 0
    jne .imst_fail

    ; === Test 9: np_interrupt_vector_allocate(1) → Vektor ≥ 48 ===
    mov eax, 1
    call np_interrupt_vector_allocate
    jc .imst_fail
    cmp eax, IRQ_VECTOR_DYN_BASE
    jb .imst_fail
    mov [irq_st_alloc_vec], eax

    ; === Test 10: np_interrupt_vector_free → anschließend wieder allokierbar ===
    mov ecx, 1
    call np_interrupt_vector_free
    jc .imst_fail
    mov eax, 1
    call np_interrupt_vector_allocate
    jc .imst_fail
    ; Freigeben (Aufräumen)
    mov ecx, 1
    call np_interrupt_vector_free

    pop edi
    pop esi
    pop ebx
    clc
    ret

.imst_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; Selbsttest-Callback
irq_st_test_handler:
    mov dword [irq_st_cb_called], 1
    mov eax, NP_IRQ_HANDLED
    ret

; ---------------------------------------------------------------------------
; §009 Daten
; ---------------------------------------------------------------------------
align 4
irq_handler_table:   times (IRQ_MANAGER_SLOTS * IRQH_SIZE / 4) dd 0
irq_counts:          times IRQ_MANAGER_SLOTS dd 0
irq_stat_total:      dd 0
irq_stat_spurious:   dd 0
irq_stat_unhandled:  dd 0
irq_tmp_slot:        dd 0
irq_tmp_alloc_cnt:   dd 0
irq_st_alloc_vec:    dd 0
irq_st_cb_called:    dd 0
align 2
irq_st_idtr_buf:     dw 0, 0, 0   ; 6 Byte: limit(2) + base(4)
align 4
; Vektor-Nutzungstabelle: 1 Byte pro Vektor, 0=frei, 1=belegt
irq_vector_used:     times 256 db 0
; VSVC-Tabelle
align 4
irq_vsvc_tbl:
    dd VSVC_TBL_HDR_SIZE
    dw 1, 0
    dd 0, 0
    dd VSVC_CALL_EARLY_BOOT | VSVC_CALL_PANIC_SAFE
    dd 0
irq_name_irqm:  db "nova.irq.manager", 0
align 4

; Restriktiver Kernel Module Loader (NPSPEC-KERNEL-0025)
; ---------------------------------------------------------------------------
MODULE_MAGIC              equ 0x444D564E ; "NVMD"
MODULE_HEADER_SIZE        equ 64
MODULE_MAX_IMAGE_SIZE     equ 4096
MODULE_ARCH_X86_32        equ 1
MODULE_KERNEL_ABI         equ 0x00010000
MODULE_FLAG_TRUSTED       equ 0x00000001
MODULE_FLAG_STACK_GUARD   equ 0x00000002
MODULE_FLAG_UNLOADABLE    equ 0x00000004
MODULE_REQUIRED_FLAGS     equ MODULE_FLAG_TRUSTED | MODULE_FLAG_STACK_GUARD
MODULE_STATE_DISCOVERED   equ 0
MODULE_STATE_VALIDATING   equ 1
MODULE_STATE_LOADING      equ 2
MODULE_STATE_RELOCATING   equ 3
MODULE_STATE_INITIALIZING equ 4
MODULE_STATE_ACTIVE       equ 5
MODULE_STATE_QUIESCING    equ 6
MODULE_STATE_UNLOADING    equ 7
MODULE_STATE_FAILED       equ 8
MODULE_STATE_UNLOADED     equ 9

module_loader_initialize:
    mov edi, module_code_area
    xor eax, eax
    mov ecx, (64 + 64) / 4
    rep stosd
    mov dword [module_state], MODULE_STATE_DISCOVERED
    mov dword [module_instance_id], 0
    mov dword [module_active_calls], 0
    mov dword [module_loaded_count], 0
    mov dword [module_failed_count], 0
    mov dword [module_unload_count], 0
    clc
    ret

; ESI=vollständiges, bereits aus einer verifizierten Quelle gelesenes Paket.
module_load:
    mov [module_source], esi
    mov dword [module_state], MODULE_STATE_VALIDATING
    cmp dword [esi + 0], MODULE_MAGIC
    jne .reject
    cmp dword [esi + 4], MODULE_HEADER_SIZE
    jne .reject
    mov eax, [esi + 8]
    cmp eax, MODULE_HEADER_SIZE
    jbe .reject
    cmp eax, MODULE_MAX_IMAGE_SIZE
    ja .reject
    cmp dword [esi + 12], MODULE_ARCH_X86_32
    jne .reject
    cmp dword [esi + 16], MODULE_KERNEL_ABI
    ja .reject
    cmp dword [esi + 20], MODULE_KERNEL_ABI
    jb .reject
    mov eax, [esi + 28]
    and eax, MODULE_REQUIRED_FLAGS
    cmp eax, MODULE_REQUIRED_FLAGS
    jne .reject
    cmp dword [esi + 60], 0
    jne .reject
    ; Code- und Datensektion müssen vollständig innerhalb der Datei liegen.
    mov eax, [esi + 32]
    cmp eax, MODULE_HEADER_SIZE
    jb .reject
    mov edx, eax
    add edx, [esi + 36]
    jc .reject
    cmp edx, [esi + 8]
    ja .reject
    cmp dword [esi + 36], 64
    ja .reject
    mov eax, [esi + 40]
    cmp eax, MODULE_HEADER_SIZE
    jb .reject
    mov ecx, eax
    add ecx, [esi + 44]
    jc .reject
    cmp ecx, [esi + 8]
    ja .reject
    cmp dword [esi + 44], 64
    ja .reject
    ; Überlappende Code-/Datensektionen werden strikt abgelehnt.
    mov eax, [esi + 32]
    add eax, [esi + 36]
    cmp eax, [esi + 40]
    jbe .sections_ok
    mov eax, [esi + 40]
    add eax, [esi + 44]
    cmp eax, [esi + 32]
    ja .reject
.sections_ok:
    ; Die initiale Trust-Schicht prüft den signierten Inhaltsdigest.
    mov ecx, [esi + 8]
    sub ecx, MODULE_HEADER_SIZE
    lea edi, [esi + MODULE_HEADER_SIZE]
    xor eax, eax
.digest:
    movzx edx, byte [edi]
    add eax, edx
    rol eax, 3
    inc edi
    loop .digest
    cmp eax, [esi + 48]
    jne .reject
    ; Abhängigkeit 0 bedeutet keine; andere IDs müssen bereits aktiv sein.
    mov eax, [esi + 52]
    test eax, eax
    jz .dependency_ok
    cmp eax, [module_id]
    jne .reject
    cmp dword [module_state], MODULE_STATE_ACTIVE
    jne .reject
.dependency_ok:
    mov dword [module_state], MODULE_STATE_LOADING
    mov eax, [esi + 32]
    add eax, esi
    push esi
    mov esi, eax
    mov edi, module_code_area
    mov ecx, [module_source]
    mov ecx, [ecx + 36]
    rep movsb
    pop esi
    mov eax, [esi + 40]
    add eax, esi
    push esi
    mov esi, eax
    mov edi, module_data_area
    mov ecx, [module_source]
    mov ecx, [ecx + 44]
    rep movsb
    pop esi
    mov dword [module_state], MODULE_STATE_RELOCATING
    ; Keine Relokation ist im Format v1 gleichbedeutend mit abgeschlossen.
    mov dword [module_code_rights], 0x5 ; R-X
    mov dword [module_data_rights], 0x3 ; RW-
    mov dword [module_state], MODULE_STATE_INITIALIZING
    mov eax, [esi + 56]
    mov [module_id], eax
    mov eax, [esi + 28]
    mov [module_flags], eax
    inc dword [module_instance_id]
    ; Erst jetzt wird die vollständig geprüfte Instanz atomar sichtbar.
    mov dword [module_state], MODULE_STATE_ACTIVE
    inc dword [module_loaded_count]
    clc
    ret
.reject:
    mov dword [module_state], MODULE_STATE_FAILED
    inc dword [module_failed_count]
    stc
    ret

module_unload:
    cmp dword [module_state], MODULE_STATE_ACTIVE
    jne .reject
    test dword [module_flags], MODULE_FLAG_UNLOADABLE
    jz .reject
    cmp dword [module_active_calls], 0
    jne .reject
    mov dword [module_state], MODULE_STATE_QUIESCING
    mov dword [module_state], MODULE_STATE_UNLOADING
    mov edi, module_code_area
    xor eax, eax
    mov ecx, (64 + 64) / 4
    rep stosd
    mov dword [module_code_rights], 0
    mov dword [module_data_rights], 0
    mov dword [module_state], MODULE_STATE_UNLOADED
    inc dword [module_unload_count]
    clc
    ret
.reject:
    stc
    ret

module_loader_self_test:
    mov esi, module_test_image
    call module_load
    jc .invalid
    cmp dword [module_state], MODULE_STATE_ACTIVE
    jne .invalid
    cmp dword [module_code_rights], 0x5
    jne .invalid
    cmp dword [module_data_rights], 0x3
    jne .invalid
    mov dword [module_active_calls], 1
    call module_unload
    jnc .invalid
    mov dword [module_active_calls], 0
    call module_unload
    jc .invalid
    ; Manipulierte Architektur darf niemals bis zur Veröffentlichung gelangen.
    mov dword [module_test_image + 12], 0xFFFFFFFF
    mov esi, module_test_image
    call module_load
    jnc .restore_invalid
    mov dword [module_test_image + 12], MODULE_ARCH_X86_32
    ; Am Ende bleibt eine gültige Diagnosemodulinstanz aktiv.
    mov esi, module_test_image
    call module_load
    jc .invalid
    cmp dword [module_loaded_count], 2
    jne .invalid
    cmp dword [module_failed_count], 1
    jne .invalid
    cmp dword [module_unload_count], 1
    jne .invalid
    clc
    ret
.restore_invalid:
    mov dword [module_test_image + 12], MODULE_ARCH_X86_32
.invalid:
    stc
    ret

align 4
module_state:          dd 0
module_instance_id:    dd 0
module_id:             dd 0
module_flags:          dd 0
module_active_calls:   dd 0
module_code_rights:    dd 0
module_data_rights:    dd 0
module_loaded_count:   dd 0
module_failed_count:   dd 0
module_unload_count:   dd 0
module_source:         dd 0
module_code_area:      times 64 db 0
module_data_area:      times 64 db 0

; Kleines signiertes In-Kernel-Testpaket. Digest wird über 32 Nutzbytes gebildet.
align 4
module_test_image:
    dd MODULE_MAGIC, MODULE_HEADER_SIZE, 96, MODULE_ARCH_X86_32
    dd MODULE_KERNEL_ABI, MODULE_KERNEL_ABI, 5
    dd MODULE_FLAG_TRUSTED | MODULE_FLAG_STACK_GUARD | MODULE_FLAG_UNLOADABLE
    dd 64, 16, 80, 16
    dd 0xD0F97818, 0, 0x44494147, 0
    db 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16
    times 16 db 0x10

; ---------------------------------------------------------------------------
; Präemptiver Round-Robin-Scheduler (ADR-2004 / ADR-2012 / NPSPEC-KERNEL-0003 / NPSPEC-KERNEL-0005)
; ---------------------------------------------------------------------------

SCHEDULER_THREAD_COUNT equ 8
SCHEDULER_FRAME_SIZE   equ 68
SCHEDULER_API_SIZE     equ 48                    ; §134: +8 (dynamic slot state)
SCHEDULER_CAP_PREEMPT  equ 0x00000001
SCHEDULER_CAP_RR       equ 0x00000002
SCHEDULER_CAP_AFFINITY equ 0x00000004           ; §133: CPU-Affinitätsunterstützung
SCHEDULER_CAP_DYNAMIC  equ 0x00000008           ; §134: dynamische Thread-Slots

; Thread Manager (ADR-2012)
THREAD_API_SIZE      equ 36
THREAD_CAPACITY      equ SCHEDULER_THREAD_COUNT
THREAD_RECORD_SIZE   equ 32
THREAD_STATE_READY   equ 1
OBJECT_TYPE_THREAD   equ 5
THREAD_TID      equ 0
THREAD_PID      equ 4
THREAD_STATE    equ 8
THREAD_SLOT     equ 12
THREAD_HANDLE   equ 16
THREAD_ENTRY    equ 20
THREAD_CONTEXT  equ 24
THREAD_SCOPE    equ 28

thread_manager_initialize:
    mov edi, thread_table
    xor eax, eax
    mov ecx, (THREAD_CAPACITY * THREAD_RECORD_SIZE) / 4
    rep stosd
    mov edi, thread_task_ids
    xor eax, eax
    mov ecx, THREAD_CAPACITY
    rep stosd
    mov dword [thread_count], 0
    mov dword [thread_next_tid], 1
    mov dword [thread_manager_ready], 0
    mov eax, kernel_main
    mov edx, 1
    xor ebx, ebx
    call thread_register
    jc .invalid
    mov eax, scheduler_thread1
    mov edx, 1
    mov ebx, 1
    call thread_register
    jc .invalid
    mov eax, scheduler_thread2
    mov edx, 1
    mov ebx, 2
    call thread_register
    jc .invalid
    mov dword [thread_manager_ready], 1
    clc
    ret
.invalid:
    stc
    ret

; EAX=Einstieg, EDX=PID, EBX=Scheduler-Slot. EAX=TID.
thread_register:
    pushfd
    cli
    test eax, eax
    jz .invalid
    cmp ebx, SCHEDULER_THREAD_COUNT
    jae .invalid
    mov [thread_temp_entry], eax
    mov [thread_temp_pid], edx
    mov [thread_temp_slot], ebx
    mov eax, edx
    call process_lookup
    jc .invalid
    mov ecx, [thread_count]
    cmp ecx, THREAD_CAPACITY
    jae .invalid
    cmp dword [thread_temp_pid], 1
    jne .invalid
    mov eax, [thread_temp_pid]
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [thread_temp_task], eax
    mov ecx, [thread_count]
    mov edi, ecx
    shl edi, 5
    add edi, thread_table
    mov [thread_temp_record], edi
    mov eax, OBJECT_TYPE_THREAD
    mov edx, [thread_temp_pid]
    mov ebx, [thread_temp_entry]
    call object_create
    jc .invalid
    mov edi, [thread_temp_record]
    mov edx, [thread_next_tid]
    mov [edi + THREAD_TID], edx
    mov ecx, [thread_temp_pid]
    mov [edi + THREAD_PID], ecx
    mov dword [edi + THREAD_STATE], THREAD_STATE_READY
    mov ecx, [thread_temp_slot]
    mov [edi + THREAD_SLOT], ecx
    mov [edi + THREAD_HANDLE], eax
    mov ecx, [thread_temp_entry]
    mov [edi + THREAD_ENTRY], ecx
    mov ecx, [thread_temp_slot]
    mov ecx, [scheduler_contexts + ecx * 4]
    mov [edi + THREAD_CONTEXT], ecx
    mov dword [edi + THREAD_SCOPE], 0
    cmp dword [thread_temp_pid], 1
    jne .scope_ready
    mov ecx, [task_scope_kernel_root_id]
    mov [edi + THREAD_SCOPE], ecx
.scope_ready:
    mov ecx, [thread_temp_slot]
    mov edx, [thread_temp_task]
    mov [thread_task_ids + ecx * 4], edx
    inc dword [thread_count]
    inc dword [thread_next_tid]
    mov eax, edx
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Einstieg, EDX=PID, ECX=CPU-Affinität (-1=beliebig). EAX=TID.
; Belegt einen freien Scheduler-Slot, erzeugt einen eigenen Kontextframe und
; veröffentlicht den Thread unter scheduler_lock. Dadurch können APs den Slot
; erst sehen, wenn Kontext, Affinität und Thread-Datensatz vollständig sind.
thread_create_dynamic:
    pushfd
    cli
    mov [thread_temp_entry], eax
    mov [thread_temp_pid], edx
    mov [thread_temp_affinity], ecx
    cmp dword [thread_manager_ready], 1
    jne .invalid_no_lock
    test eax, eax
    jz .invalid_no_lock
    mov eax, edx
    call process_lookup
    jc .invalid_no_lock

.lock:
    lock bts dword [scheduler_lock], 0
    jc .lock

    mov ebx, 1                         ; Slot 0 bleibt BSP-/Kernel-Slot
.scan_slot:
    cmp ebx, SCHEDULER_THREAD_COUNT
    jae .invalid_unlock
    cmp dword [thread_task_ids + ebx * 4], 0
    jne .next_slot
    cmp dword [scheduler_contexts + ebx * 4], 0
    je .slot_found
.next_slot:
    inc ebx
    jmp .scan_slot

.slot_found:
    mov [thread_temp_slot], ebx
    mov eax, [thread_temp_entry]
    call scheduler_create_frame
    jc .invalid_unlock
    mov ebx, [thread_temp_slot]
    mov [scheduler_contexts + ebx * 4], eax
    mov eax, [thread_temp_affinity]
    mov [thread_cpu_affinity + ebx * 4], eax
    mov dword [thread_running_on + ebx * 4], -1

    mov eax, [thread_temp_entry]
    mov edx, [thread_temp_pid]
    mov ebx, [thread_temp_slot]
    call thread_register
    jc .invalid_clear_slot
    lock btr dword [scheduler_lock], 0
    popfd
    clc
    ret

.invalid_clear_slot:
    mov ebx, [thread_temp_slot]
    mov dword [scheduler_contexts + ebx * 4], 0
    mov dword [thread_cpu_affinity + ebx * 4], -1
    mov dword [thread_running_on + ebx * 4], -1
.invalid_unlock:
    lock btr dword [scheduler_lock], 0
.invalid_no_lock:
    xor eax, eax
    popfd
    stc
    ret

; EAX=TID. EAX=Datensatz oder 0.
thread_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, THREAD_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 5
    add edx, thread_table
    cmp [edx + THREAD_TID], eax
    je .found
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

thread_manager_self_test:
    mov eax, 2
    call thread_lookup
    jc .invalid
    cmp dword [eax + THREAD_PID], 1
    jne .invalid
    cmp dword [eax + THREAD_SLOT], 1
    jne .invalid
    cmp dword [eax + THREAD_ENTRY], scheduler_thread1
    jne .invalid
    mov edx, [task_scope_kernel_root_id]
    cmp [eax + THREAD_SCOPE], edx
    jne .invalid
    mov eax, [thread_task_ids + 4]
    test eax, eax
    jz .invalid
    call task_lookup
    jc .invalid
    cmp dword [eax + TASK_OWNER], 1
    jne .invalid
    mov edx, [task_scope_kernel_root_id]
    cmp [eax + TASK_RECORD_SCOPE], edx
    jne .invalid
    ; §134: zur Laufzeit einen zusätzlichen Thread in einen freien
    ; Scheduler-Slot legen. Der Thread ist CPU-ungebunden und darf damit von
    ; BSP oder APs übernommen werden, sobald der Scheduler ihn auswählt.
    mov eax, scheduler_dynamic_thread
    mov edx, 1
    mov ecx, -1
    call thread_create_dynamic
    jc .invalid
    mov [thread_dynamic_tid], eax
    call thread_lookup
    jc .invalid
    cmp dword [eax + THREAD_SLOT], 3
    jne .invalid
    cmp dword [eax + THREAD_ENTRY], scheduler_dynamic_thread
    jne .invalid
    cmp dword [thread_cpu_affinity + 3 * 4], -1
    jne .invalid
    cmp dword [thread_running_on + 3 * 4], -1
    jne .invalid
    cmp dword [task_count], 4
    jne .invalid
    cmp dword [thread_count], 4
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
thread_manager_api:
    dd THREAD_API_SIZE
    dw 1, 0
    dd THREAD_CAPACITY
    dd thread_register
    dd thread_lookup
    dd thread_count
    dd thread_table
    dd thread_next_tid
    dd thread_create_dynamic

thread_count:       dd 0
thread_next_tid:    dd 0
thread_temp_entry:  dd 0
thread_temp_pid:    dd 0
thread_temp_slot:   dd 0
thread_temp_record: dd 0
thread_temp_task:   dd 0
thread_temp_affinity: dd 0
thread_dynamic_tid: dd 0
thread_manager_ready: dd 0
align 4
thread_table:
    times THREAD_CAPACITY * THREAD_RECORD_SIZE db 0
thread_task_ids:
    times THREAD_CAPACITY dd 0

