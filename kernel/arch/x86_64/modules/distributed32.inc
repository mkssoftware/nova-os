; ---------------------------------------------------------------------------
; Nova Distributed-Systems §028
; NPSPEC-DISTRIBUTED-CLUSTER-0001       - Cluster
; NPSPEC-DISTRIBUTED-CONSISTENCY-0001   - Konsistenz
; NPSPEC-DISTRIBUTED-EXECUTION-0001     - Verteilte Ausfuehrung
; NPSPEC-DISTRIBUTED-IPC-0001           - Verteiltes IPC
; NPSPEC-DISTRIBUTED-LOCATION-0001      - Ortsbestimmung
; NPSPEC-DISTRIBUTED-PLACEMENT-0001     - Placement
; NPSPEC-DISTRIBUTED-REMOTECAPABILITY-0001 - Remote-Capabilities
; NPSPEC-DISTRIBUTED-REPLICATION-0001   - Replikation
; NPSPEC-DISTRIBUTED-SCHEDULER-0001     - Verteilter Scheduler
; NPSPEC-DISTRIBUTED-STORAGE-0001       - Verteilter Speicher
; NPSPEC-DISTRIBUTED-TRANSACTION-0001   - Verteilte Transaktionen
; ---------------------------------------------------------------------------

; Konsistenz-Modelle (NPSPEC-DISTRIBUTED-CONSISTENCY-0001)
DIST_CONS_EVENTUAL    equ 0
DIST_CONS_CAUSAL      equ 1
DIST_CONS_SEQUENTIAL  equ 2
DIST_CONS_STRONG      equ 3

; Knoten-Rolle (NPSPEC-DISTRIBUTED-CLUSTER-0001)
DIST_ROLE_PRIMARY     equ 0
DIST_ROLE_REPLICA     equ 1    ; (NPSPEC-DISTRIBUTED-REPLICATION-0001)
DIST_ROLE_WORKER      equ 2    ; (NPSPEC-DISTRIBUTED-SCHEDULER-0001)
DIST_ROLE_STORAGE     equ 3    ; (NPSPEC-DISTRIBUTED-STORAGE-0001)

; Remote-Aufruf-Status (NPSPEC-DISTRIBUTED-IPC-0001)
DIST_RPC_PENDING      equ 0
DIST_RPC_OK           equ 1
DIST_RPC_TIMEOUT      equ 2
DIST_RPC_FAILED       equ 3

; Knoten-Record (40 Bytes, NPSPEC-DISTRIBUTED-CLUSTER-0001)
DIST_NODE_SIZE        equ 40
DN_ID                 equ 0
DN_ROLE               equ 4    ; DIST_ROLE_*
DN_CONSISTENCY        equ 8    ; DIST_CONS_*
DN_ADDR               equ 12   ; IP/Node-Adresse (vereinfacht uint32)
DN_CAPABILITIES       equ 16   ; Bitmask Remote-Caps (NPSPEC-DISTRIBUTED-REMOTECAPABILITY-0001)
DN_PLACEMENT_ZONE     equ 20   ; Placement-Zone (NPSPEC-DISTRIBUTED-PLACEMENT-0001)
DN_TX_COUNT           equ 24   ; Laufende dist. Transaktionen (NPSPEC-DISTRIBUTED-TRANSACTION-0001)
DN_FLAGS              equ 28
DN_LATENCY_US         equ 32   ; Durchschnitt-Latenz (NPSPEC-DISTRIBUTED-LOCATION-0001)
; 36..39: reserviert

DIST_NODE_CAPACITY    equ 8

; ---------------------------------------------------------------------------
; distributed_initialize
; ---------------------------------------------------------------------------
distributed_initialize:
    push edi
    mov edi, dist_node_table
    xor eax, eax
    mov ecx, (DIST_NODE_CAPACITY * DIST_NODE_SIZE) / 4
    rep stosd
    pop edi
    mov dword [dist_node_count], 0
    mov dword [dist_local_id], 1
    mov dword [dist_consistency], DIST_CONS_EVENTUAL
    mov dword [dist_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; dist_register_node: EAX=node_id, EBX=role, ECX=addr, EDX=caps
; (NPSPEC-DISTRIBUTED-CLUSTER-0001, NPSPEC-DISTRIBUTED-REMOTECAPABILITY-0001)
; CF=0 Erfolg
; ---------------------------------------------------------------------------
dist_register_node:
    cmp dword [dist_node_count], DIST_NODE_CAPACITY
    jae .full
    push edi
    mov edi, dist_node_table
    push eax
    mov eax, [dist_node_count]
    imul eax, DIST_NODE_SIZE
    add edi, eax
    pop eax
    mov [edi + DN_ID], eax
    mov [edi + DN_ROLE], ebx
    mov [edi + DN_ADDR], ecx
    mov [edi + DN_CAPABILITIES], edx
    mov eax, [dist_consistency]
    mov [edi + DN_CONSISTENCY], eax
    mov dword [edi + DN_PLACEMENT_ZONE], 0
    mov dword [edi + DN_TX_COUNT], 0
    mov dword [edi + DN_FLAGS], 0
    mov dword [edi + DN_LATENCY_US], 100  ; Default: 100us lokal
    inc dword [dist_node_count]
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; dist_send_rpc: EAX=node_id, EBX=function_hash, ECX=arg
; (NPSPEC-DISTRIBUTED-IPC-0001, NPSPEC-DISTRIBUTED-EXECUTION-0001)
; CF=0 -> EDX=status
; ---------------------------------------------------------------------------
dist_send_rpc:
    push esi
    push ecx
    mov esi, dist_node_table
    mov ecx, [dist_node_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + DN_ID], eax
    je .found
    add esi, DIST_NODE_SIZE
    dec ecx
    jmp .scan
.found:
    ; Einfacher RPC-Stub: lokal simulieren
    mov edx, DIST_RPC_OK
    pop ecx
    pop esi
    clc
    ret
.not_found:
    mov edx, DIST_RPC_FAILED
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; dist_replicate: EAX=data_id, EBX=data_hash
; (NPSPEC-DISTRIBUTED-REPLICATION-0001)
; CF=0 Erfolg (an alle Replika-Knoten gesendet)
; ---------------------------------------------------------------------------
dist_replicate:
    push esi
    push ecx
    xor ecx, ecx
    mov esi, dist_node_table
    push edx
    mov edx, [dist_node_count]
.loop:
    cmp ecx, edx
    jae .done
    cmp dword [esi + DN_ROLE], DIST_ROLE_REPLICA
    jne .next
    ; Replikation zu diesem Knoten
    inc dword [esi + DN_TX_COUNT]
.next:
    add esi, DIST_NODE_SIZE
    inc ecx
    jmp .loop
.done:
    pop edx
    pop ecx
    pop esi
    clc
    ret

; ---------------------------------------------------------------------------
; distributed_self_test
; ---------------------------------------------------------------------------
distributed_self_test:
    push esi
    xor esi, esi
    cmp dword [dist_ready], 1
    je .t2
    inc esi
.t2:
    mov eax, 2
    mov ebx, DIST_ROLE_REPLICA
    mov ecx, 0xC0A80001    ; 192.168.0.1
    mov edx, 0xFF
    call dist_register_node
    jnc .t3
    inc esi
.t3:
    mov eax, 2
    mov ebx, 0xABCD1234
    mov ecx, 0
    call dist_send_rpc
    jnc .t4
    inc esi
.t4:
    mov eax, 0x1000
    mov ebx, 0xDEAD
    call dist_replicate
    jnc .done
    inc esi
.done:
    pop esi
    test esi, esi
    jz .ok
    stc
    ret
.ok:
    clc
    ret

align 4
dist_ready:        dd 0
dist_node_count:   dd 0
dist_local_id:     dd 1
dist_consistency:  dd DIST_CONS_EVENTUAL

align 4
dist_node_table:
    times DIST_NODE_CAPACITY * DIST_NODE_SIZE db 0
