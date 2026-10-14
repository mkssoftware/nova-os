; ---------------------------------------------------------------------------
; Nova Distributed Communications §017
; NPSPEC-DISTCOMM-BACKPRESSURE-0001  - Gegendruck-Steuerung
; NPSPEC-DISTCOMM-DEADLINE-0001      - Deadline-Propagation
; NPSPEC-DISTCOMM-HTTP-0001          - HTTP-Semantik
; NPSPEC-DISTCOMM-IDEMPOTENCY-0001   - Idempotenz-Tokens
; NPSPEC-DISTCOMM-MESSAGEBUS-0001    - Message-Bus
; NPSPEC-DISTCOMM-NEGOTIATION-0001   - Protokoll-Aushandlung
; NPSPEC-DISTCOMM-PUBSUB-0001        - Publish-Subscribe
; NPSPEC-DISTCOMM-QUEUE-0001         - Nachrichtenqueue
; NPSPEC-DISTCOMM-REMOTECAPABILITY-0001 - Remote-Capabilities
; NPSPEC-DISTCOMM-RETRY-0001         - Retry-Policy
; NPSPEC-DISTCOMM-RPC-0001           - Remote-Procedure-Call
; NPSPEC-DISTCOMM-SCHEMA-0001        - Schema-Versionierung
; NPSPEC-DISTCOMM-SERIALIZATION-0001 - Serialisierung
; NPSPEC-DISTCOMM-TLS-0001           - TLS-Kontext
; NPSPEC-DISTCOMM-TRACE-0001         - Distributed Tracing
; ---------------------------------------------------------------------------

; Nachrichtentypen (NPSPEC-DISTCOMM-MESSAGEBUS-0001)
DC_MSG_REQUEST        equ 0
DC_MSG_RESPONSE       equ 1
DC_MSG_EVENT          equ 2    ; (NPSPEC-DISTCOMM-PUBSUB-0001)
DC_MSG_HEARTBEAT      equ 3

; Transport-Protokolle
DC_PROTO_LOCAL        equ 0    ; Lokal / IPC
DC_PROTO_HTTP         equ 1    ; (NPSPEC-DISTCOMM-HTTP-0001)
DC_PROTO_RPC          equ 2    ; (NPSPEC-DISTCOMM-RPC-0001)
DC_PROTO_TLS          equ 3    ; (NPSPEC-DISTCOMM-TLS-0001)

; Retry-Strategie (NPSPEC-DISTCOMM-RETRY-0001)
DC_RETRY_NONE         equ 0
DC_RETRY_FIXED        equ 1    ; Festes Intervall
DC_RETRY_EXPONENTIAL  equ 2    ; Exponentielles Backoff
DC_RETRY_JITTER       equ 3    ; Mit Jitter

; Backpressure-Status (NPSPEC-DISTCOMM-BACKPRESSURE-0001)
DC_BP_OK              equ 0
DC_BP_SLOW            equ 1
DC_BP_OVERLOAD        equ 2
DC_BP_DROP            equ 3

; Nachrichten-Record (48 Bytes)
DC_MSG_REC_SIZE       equ 48
DM_ID                 equ 0    ; Nachrichten-ID
DM_TYPE               equ 4    ; DC_MSG_*
DM_PROTO              equ 8    ; DC_PROTO_*
DM_FLAGS              equ 12
DM_IDEMPOTENCY_KEY    equ 16   ; (NPSPEC-DISTCOMM-IDEMPOTENCY-0001)
DM_DEADLINE           equ 20   ; Ticks (NPSPEC-DISTCOMM-DEADLINE-0001)
DM_RETRY_COUNT        equ 24   ; Versuche bisher
DM_MAX_RETRIES        equ 28   ; (NPSPEC-DISTCOMM-RETRY-0001)
DM_TRACE_ID           equ 32   ; (NPSPEC-DISTCOMM-TRACE-0001)
DM_SCHEMA_VER         equ 36   ; (NPSPEC-DISTCOMM-SCHEMA-0001)
DM_PAYLOAD_LEN        equ 40
DM_PAYLOAD_PTR        equ 44

DC_MSG_CAPACITY       equ 8

; Pub/Sub-Topic-Record (32 Bytes, NPSPEC-DISTCOMM-PUBSUB-0001)
DC_TOPIC_REC_SIZE     equ 32
DT_TOPIC_ID           equ 0
DT_FLAGS              equ 4
DT_SUBSCRIBER_MASK    equ 8    ; Bitmaske registrierter Subscriber
DT_MSG_COUNT          equ 12
DT_BACKPRESSURE       equ 16   ; DC_BP_*
DT_LAST_TRACE         equ 20

DC_TOPIC_CAPACITY     equ 8

; ---------------------------------------------------------------------------
; distcom_initialize
; ---------------------------------------------------------------------------
distcom_initialize:
    push edi
    mov edi, dc_msg_table
    xor eax, eax
    mov ecx, (DC_MSG_CAPACITY * DC_MSG_REC_SIZE) / 4
    rep stosd
    mov edi, dc_topic_table
    mov ecx, (DC_TOPIC_CAPACITY * DC_TOPIC_REC_SIZE) / 4
    rep stosd
    pop edi
    mov dword [dc_msg_count], 0
    mov dword [dc_topic_count], 0
    mov dword [dc_next_msg_id], 1
    mov dword [dc_next_trace_id], 1
    mov dword [dc_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; dc_send: EAX=type, EBX=proto, ECX=deadline_ticks -> EDX=msg_id, CF=0
; (NPSPEC-DISTCOMM-RPC-0001, NPSPEC-DISTCOMM-MESSAGEBUS-0001)
; ---------------------------------------------------------------------------
dc_send:
    cmp dword [dc_msg_count], DC_MSG_CAPACITY
    jae .full
    push edi
    push esi
    mov edi, dc_msg_table
    push eax
    mov eax, [dc_msg_count]
    imul eax, DC_MSG_REC_SIZE
    add edi, eax
    pop eax
    mov edx, [dc_next_msg_id]
    mov [edi + DM_ID], edx
    mov [edi + DM_TYPE], eax
    mov [edi + DM_PROTO], ebx
    mov dword [edi + DM_FLAGS], 0
    ; Idempotenz-Key = msg_id XOR timer_ticks
    push eax
    mov eax, [timer_ticks]
    xor eax, edx
    mov [edi + DM_IDEMPOTENCY_KEY], eax
    pop eax
    mov [edi + DM_DEADLINE], ecx
    mov dword [edi + DM_RETRY_COUNT], 0
    mov dword [edi + DM_MAX_RETRIES], 3
    ; Trace-ID vergeben (NPSPEC-DISTCOMM-TRACE-0001)
    push eax
    mov eax, [dc_next_trace_id]
    mov [edi + DM_TRACE_ID], eax
    inc dword [dc_next_trace_id]
    pop eax
    mov dword [edi + DM_SCHEMA_VER], 1
    mov dword [edi + DM_PAYLOAD_LEN], 0
    mov dword [edi + DM_PAYLOAD_PTR], 0
    inc dword [dc_next_msg_id]
    inc dword [dc_msg_count]
    pop esi
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; dc_topic_register: EAX=topic_id
; (NPSPEC-DISTCOMM-PUBSUB-0001)
; ---------------------------------------------------------------------------
dc_topic_register:
    cmp dword [dc_topic_count], DC_TOPIC_CAPACITY
    jae .full
    push edi
    push ecx
    mov edi, dc_topic_table
    mov ecx, [dc_topic_count]
    imul ecx, DC_TOPIC_REC_SIZE
    add edi, ecx
    mov [edi + DT_TOPIC_ID], eax
    mov dword [edi + DT_FLAGS], 0
    mov dword [edi + DT_SUBSCRIBER_MASK], 0
    mov dword [edi + DT_MSG_COUNT], 0
    mov dword [edi + DT_BACKPRESSURE], DC_BP_OK
    inc dword [dc_topic_count]
    pop ecx
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; dc_subscribe: EAX=topic_id, EBX=subscriber_bit
; ---------------------------------------------------------------------------
dc_subscribe:
    push esi
    push ecx
    mov esi, dc_topic_table
    mov ecx, [dc_topic_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + DT_TOPIC_ID], eax
    je .found
    add esi, DC_TOPIC_REC_SIZE
    dec ecx
    jmp .scan
.found:
    or [esi + DT_SUBSCRIBER_MASK], ebx
    pop ecx
    pop esi
    clc
    ret
.not_found:
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; dc_backpressure_check: EAX=topic_id -> EBX=bp_state, CF=0 ok
; (NPSPEC-DISTCOMM-BACKPRESSURE-0001)
; ---------------------------------------------------------------------------
dc_backpressure_check:
    push esi
    push ecx
    mov esi, dc_topic_table
    mov ecx, [dc_topic_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + DT_TOPIC_ID], eax
    je .found
    add esi, DC_TOPIC_REC_SIZE
    dec ecx
    jmp .scan
.found:
    mov ebx, [esi + DT_BACKPRESSURE]
    pop ecx
    pop esi
    clc
    ret
.not_found:
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; distcom_self_test
; ---------------------------------------------------------------------------
distcom_self_test:
    push esi
    xor esi, esi
    cmp dword [dc_ready], 1
    je .t2
    inc esi
.t2:
    mov eax, DC_MSG_REQUEST
    mov ebx, DC_PROTO_LOCAL
    mov ecx, 1000
    call dc_send
    jnc .t3
    inc esi
.t3:
    mov eax, 1
    call dc_topic_register
    jnc .t4
    inc esi
.t4:
    mov eax, 1
    mov ebx, 0x01
    call dc_subscribe
    jnc .done
    inc esi
.done:
    test esi, esi
    pop esi
    jz .ok
    stc
    ret
.ok:
    clc
    ret

align 4
dc_ready:         dd 0
dc_msg_count:     dd 0
dc_topic_count:   dd 0
dc_next_msg_id:   dd 1
dc_next_trace_id: dd 1

align 4
dc_msg_table:
    times DC_MSG_CAPACITY * DC_MSG_REC_SIZE db 0

align 4
dc_topic_table:
    times DC_TOPIC_CAPACITY * DC_TOPIC_REC_SIZE db 0
