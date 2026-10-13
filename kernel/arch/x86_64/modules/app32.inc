; ---------------------------------------------------------------------------
; Nova Application Model
; NPSPEC-APP-MODEL-0001 / NPSPEC-APP-COMPONENT-0001
; NPSPEC-APP-IDENTITY-0001 / NPSPEC-APP-SANDBOX-0001
; NPSPEC-APP-SERVICE-0001 / NPSPEC-APP-PORTABLE-0001
; NPSPEC-APP-CAPABILITYCOMPOSED-0001 / NPSPEC-APP-DYNAMICCOMPOSITION-0001
; NPSPEC-APP-DOCUMENTCENTRIC-0001 / NPSPEC-APP-OBJECTCENTRIC-0001
; NPSPEC-APP-INTENTCENTRIC-0001 / NPSPEC-APP-TASKCENTRIC-0001
; ---------------------------------------------------------------------------

; Application-Zustaende (NPSPEC-APP-MODEL-0001)
APP_STATE_INACTIVE    equ 0
APP_STATE_STARTING    equ 1
APP_STATE_RUNNING     equ 2
APP_STATE_SUSPENDED   equ 3
APP_STATE_STOPPING    equ 4
APP_STATE_STOPPED     equ 5
APP_STATE_FAILED      equ 6

; Application-Typen (NPSPEC-APP-MODEL-0001)
APP_TYPE_FOREGROUND   equ 0    ; Standard-App mit UI
APP_TYPE_BACKGROUND   equ 1    ; Dienst ohne UI
APP_TYPE_DOCUMENT     equ 2    ; Dokument-zentrischer App (NPSPEC-APP-DOCUMENTCENTRIC-0001)
APP_TYPE_OBJECT       equ 3    ; Objekt-zentrischer App (NPSPEC-APP-OBJECTCENTRIC-0001)
APP_TYPE_INTENT       equ 4    ; Intent-Handler (NPSPEC-APP-INTENTCENTRIC-0001)
APP_TYPE_TASK         equ 5    ; Aufgaben-App (NPSPEC-APP-TASKCENTRIC-0001)
APP_TYPE_SERVICE      equ 6    ; System-Service (NPSPEC-APP-SERVICE-0001)

; Sandbox-Level (NPSPEC-APP-SANDBOX-0001)
APP_SANDBOX_NONE      equ 0    ; Kein Sandboxing (Kernel/System)
APP_SANDBOX_STANDARD  equ 1    ; Standard (Ring-3, beschraenkte Capabilities)
APP_SANDBOX_STRICT    equ 2    ; Strikt (nur explizit gew‰hrte Capabilities)
APP_SANDBOX_ISOLATED  equ 3    ; Vollst‰ndig isoliert (kein IPC ohne Broker)

; Composition-Typen (NPSPEC-APP-DYNAMICCOMPOSITION-0001)
COMP_TYPE_STATIC      equ 0    ; Build-time fixed (NPSPEC-APP-CAPABILITYCOMPOSED-0001)
COMP_TYPE_DYNAMIC     equ 1    ; Runtime composition

; App-Flags
APP_FLAG_PORTABLE     equ 0x01  ; Portable/containerisiert (NPSPEC-APP-PORTABLE-0001)
APP_FLAG_SIGNED       equ 0x02  ; Kryptographisch signiert
APP_FLAG_SYSTEM       equ 0x04  ; System-Komponente
APP_FLAG_CAPABILITY   equ 0x08  ; Capability-composed (NPSPEC-APP-CAPABILITYCOMPOSED-0001)

; App-Record (64 Bytes, NPSPEC-APP-MODEL-0001 + NPSPEC-APP-IDENTITY-0001)
APP_RECORD_SIZE       equ 64
APP_ID                equ 0    ; Stabile App-ID (CRC32 des Bundle-Pfads)
APP_TYPE              equ 4    ; APP_TYPE_*
APP_STATE             equ 8    ; APP_STATE_*
APP_SANDBOX           equ 12   ; APP_SANDBOX_*
APP_FLAGS             equ 16   ; APP_FLAG_*
APP_PROCESS           equ 20   ; Prozess-Handle (0 = nicht gestartet)
APP_CAPABILITIES      equ 24   ; Capability-Bitmaske (aus cap_integration32.inc)
APP_PARENT            equ 28   ; Eltern-App-ID (0 = keine)
APP_COMP_TYPE         equ 32   ; COMP_TYPE_*
APP_INTENT_HASH       equ 36   ; Intent-Hash fuer Intent-Apps
APP_DOC_TYPE          equ 40   ; Dokument-Typ-ID fuer Dokument-Apps
APP_TASK_ID           equ 44   ; Task-Referenz fuer Task-Apps
APP_LOCALE            equ 48   ; LANG_*-Pr‰ferenz (NPSPEC-GLOBALIZATION-LOCALE-0001)
APP_RESERVED          equ 52   ; 12 Bytes

APP_CAPACITY          equ 8    ; max. 8 Apps gleichzeitig

; Intent-Record (32 Bytes, NPSPEC-APP-INTENTCENTRIC-0001)
INTENT_SIZE           equ 32
INT_HASH              equ 0    ; Intent-Hash (CRC32 des Namens)
INT_SENDER            equ 4    ; Sender-App-ID
INT_TARGET            equ 8    ; Ziel-App-ID (0 = Broadcast)
INT_DATA_TYPE         equ 12   ; Datentyp-Hash
INT_DATA_PTR          equ 16   ; Zeiger auf Nutzdaten
INT_DATA_LEN          equ 20   ; Laenge Nutzdaten
INT_FLAGS             equ 24   ; Flags
INT_RESERVED          equ 28

INTENT_CAPACITY       equ 16   ; max. 16 ausstehende Intents

; Service-Record (32 Bytes, NPSPEC-APP-SERVICE-0001)
SVC_RECORD_SIZE       equ 32
SVC_ID                equ 0    ; Service-ID
SVC_APP               equ 4    ; Provider-App-ID
SVC_API_VERSION       equ 8    ; API-Version
SVC_FLAGS             equ 12   ; 0x1=singleton, 0x2=system
SVC_ENDPOINT          equ 16   ; IPC-Endpoint-Handle
SVC_RESERVED          equ 20   ; 12 Bytes

SVC_CAPACITY          equ 8    ; max. 8 Services

; ---------------------------------------------------------------------------
; app_manager_initialize
; CF=0 Erfolg
; ---------------------------------------------------------------------------
app_manager_initialize:
    mov edi, app_records
    xor eax, eax
    mov ecx, (APP_CAPACITY * APP_RECORD_SIZE) / 4
    rep stosd
    mov edi, app_intents
    mov ecx, (INTENT_CAPACITY * INTENT_SIZE) / 4
    rep stosd
    mov edi, svc_records
    mov ecx, (SVC_CAPACITY * SVC_RECORD_SIZE) / 4
    rep stosd
    mov dword [app_count], 0
    mov dword [intent_count], 0
    mov dword [svc_count], 0
    mov dword [app_mgr_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; app_register
; Registriert eine App im App-Manager.
; EAX=app_id, EBX=app_type, ECX=sandbox_level, EDX=flags
; CF=0 Erfolg (ESI=Record-Zeiger), CF=1 voll
; ---------------------------------------------------------------------------
app_register:
    push edi
    cmp dword [app_count], APP_CAPACITY
    jae .full
    ; Freien Slot suchen
    mov edi, app_records
    push ecx
    mov ecx, APP_CAPACITY
.scan:
    test ecx, ecx
    jz .no_slot
    cmp dword [edi + APP_STATE], APP_STATE_INACTIVE
    je .got_slot
    add edi, APP_RECORD_SIZE
    dec ecx
    jmp .scan
.no_slot:
    pop ecx
    pop edi
    stc
    ret
.got_slot:
    pop ecx
    mov dword [edi + APP_ID], eax
    mov dword [edi + APP_TYPE], ebx
    mov dword [edi + APP_STATE], APP_STATE_STOPPED
    mov dword [edi + APP_SANDBOX], ecx
    mov dword [edi + APP_FLAGS], edx
    mov dword [edi + APP_PROCESS], 0
    mov dword [edi + APP_CAPABILITIES], 0
    mov dword [edi + APP_PARENT], 0
    mov dword [edi + APP_COMP_TYPE], COMP_TYPE_STATIC
    mov dword [edi + APP_INTENT_HASH], 0
    mov dword [edi + APP_DOC_TYPE], 0
    mov dword [edi + APP_TASK_ID], 0
    mov dword [edi + APP_LOCALE], LANG_DE_DE
    inc dword [app_count]
    mov esi, edi
    pop edi
    clc
    ret
.full:
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; app_start: ESI=app_record
; Wechselt Zustand auf STARTING. Prozess-Start erfolgt durch Process Manager.
; CF=0 Erfolg
; ---------------------------------------------------------------------------
app_start:
    cmp dword [esi + APP_STATE], APP_STATE_STOPPED
    jne .already_running
    mov dword [esi + APP_STATE], APP_STATE_STARTING
    clc
    ret
.already_running:
    stc
    ret

; ---------------------------------------------------------------------------
; app_stop: ESI=app_record
; CF=0 Erfolg
; ---------------------------------------------------------------------------
app_stop:
    cmp dword [esi + APP_STATE], APP_STATE_RUNNING
    je .ok
    cmp dword [esi + APP_STATE], APP_STATE_SUSPENDED
    je .ok
    cmp dword [esi + APP_STATE], APP_STATE_STARTING
    je .ok
    stc
    ret
.ok:
    mov dword [esi + APP_STATE], APP_STATE_STOPPING
    clc
    ret

; ---------------------------------------------------------------------------
; app_suspend / app_resume: NPSPEC-APP-MODEL-0001
; ---------------------------------------------------------------------------
app_suspend:
    cmp dword [esi + APP_STATE], APP_STATE_RUNNING
    jne .err
    mov dword [esi + APP_STATE], APP_STATE_SUSPENDED
    clc
    ret
.err: stc
    ret

app_resume:
    cmp dword [esi + APP_STATE], APP_STATE_SUSPENDED
    jne .err
    mov dword [esi + APP_STATE], APP_STATE_RUNNING
    clc
    ret
.err: stc
    ret

; ---------------------------------------------------------------------------
; app_find_by_id: EAX=app_id -> ESI=record (CF=0) oder CF=1
; (NPSPEC-APP-IDENTITY-0001)
; ---------------------------------------------------------------------------
app_find_by_id:
    push ecx
    push edi
    mov edi, app_records
    mov ecx, APP_CAPACITY
.scan:
    test ecx, ecx
    jz .not_found
    cmp dword [edi + APP_ID], eax
    je .found
    add edi, APP_RECORD_SIZE
    dec ecx
    jmp .scan
.found:
    mov esi, edi
    pop edi
    pop ecx
    clc
    ret
.not_found:
    pop edi
    pop ecx
    stc
    ret

; ---------------------------------------------------------------------------
; app_dispatch_intent: EBX=intent_hash, ECX=sender_id, EDX=target_id
; Stellt Intent in die Warteschlange.
; (NPSPEC-APP-INTENTCENTRIC-0001)
; CF=0 Erfolg, CF=1 Warteschlange voll
; ---------------------------------------------------------------------------
app_dispatch_intent:
    push edi
    cmp dword [intent_count], INTENT_CAPACITY
    jae .full
    ; Freien Slot suchen
    mov edi, app_intents
    push eax
    mov eax, INTENT_CAPACITY
.scan:
    test eax, eax
    jz .no_slot
    cmp dword [edi + INT_HASH], 0
    je .got_slot
    add edi, INTENT_SIZE
    dec eax
    jmp .scan
.no_slot:
    pop eax
    pop edi
    stc
    ret
.got_slot:
    pop eax
    mov [edi + INT_HASH], ebx
    mov [edi + INT_SENDER], ecx
    mov [edi + INT_TARGET], edx
    mov dword [edi + INT_DATA_PTR], 0
    mov dword [edi + INT_DATA_LEN], 0
    mov dword [edi + INT_FLAGS], 0
    inc dword [intent_count]
    pop edi
    clc
    ret
.full:
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; svc_register: EAX=svc_id, EBX=app_id, ECX=api_version, EDX=flags
; (NPSPEC-APP-SERVICE-0001)
; CF=0 Erfolg, CF=1 voll
; ---------------------------------------------------------------------------
svc_register:
    push edi
    cmp dword [svc_count], SVC_CAPACITY
    jae .full
    mov edi, svc_records
    push eax
    mov eax, SVC_CAPACITY
.scan:
    test eax, eax
    jz .no_slot
    cmp dword [edi + SVC_ID], 0
    je .got_slot
    add edi, SVC_RECORD_SIZE
    dec eax
    jmp .scan
.no_slot:
    pop eax
    pop edi
    stc
    ret
.got_slot:
    pop eax
    mov [edi + SVC_ID], eax
    mov [edi + SVC_APP], ebx
    mov [edi + SVC_API_VERSION], ecx
    mov [edi + SVC_FLAGS], edx
    mov dword [edi + SVC_ENDPOINT], 0
    inc dword [svc_count]
    pop edi
    clc
    ret
.full:
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; app_manager_self_test
; ---------------------------------------------------------------------------
app_manager_self_test:
    push esi
    xor esi, esi

    cmp dword [app_mgr_ready], 1
    je .t2
    inc esi
.t2:
    ; Test: App registrieren
    mov eax, 0xDEADBEEF        ; Test-App-ID
    mov ebx, APP_TYPE_FOREGROUND
    mov ecx, APP_SANDBOX_STANDARD
    mov edx, APP_FLAG_PORTABLE
    call app_register
    jnc .t3
    inc esi
    jmp .done
.t3:
    ; App-ID suchen
    mov eax, 0xDEADBEEF
    call app_find_by_id
    jnc .t4
    inc esi
    jmp .done
.t4:
    ; App starten
    call app_start
    jnc .t5
    inc esi
    jmp .done
.t5:
    ; App stoppen
    ; Zustand auf RUNNING setzen fuer Stop-Test
    mov dword [esi + APP_STATE], APP_STATE_RUNNING
    call app_stop
    jnc .cleanup
    inc esi
.cleanup:
    ; Cleanup: Slot zur‹ckgeben
    mov dword [esi + APP_STATE], APP_STATE_INACTIVE
    dec dword [app_count]
.done:
    test esi, esi
    jz .ok
    pop esi
    stc
    ret
.ok:
    pop esi
    clc
    ret

; ---------------------------------------------------------------------------
; Statische Daten
; ---------------------------------------------------------------------------
align 4
app_mgr_ready: dd 0
app_count:     dd 0
intent_count:  dd 0
svc_count:     dd 0

align 4
app_records:
    times APP_CAPACITY * APP_RECORD_SIZE db 0

align 4
app_intents:
    times INTENT_CAPACITY * INTENT_SIZE db 0

align 4
svc_records:
    times SVC_CAPACITY * SVC_RECORD_SIZE db 0
