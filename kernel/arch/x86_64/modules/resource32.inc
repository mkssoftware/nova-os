; ---------------------------------------------------------------------------
; Nova Resource-Management §026
; NPSPEC-RESOURCE-ACCOUNTING-0001 - Ressource-Buchfuehrung
; NPSPEC-RESOURCE-ADMISSION-0001  - Admission-Control
; NPSPEC-RESOURCE-ARBITRATION-0001- Ressource-Arbitrierung
; NPSPEC-RESOURCE-CPU-0001        - CPU-Ressourcen
; NPSPEC-RESOURCE-DEADLINE-0001   - Deadline-Ressourcen
; NPSPEC-RESOURCE-ECONOMY-0001    - Oekonomisches Modell
; NPSPEC-RESOURCE-ENERGY-0001     - Energie-Ressourcen
; NPSPEC-RESOURCE-GPU-0001        - GPU-Ressourcen
; NPSPEC-RESOURCE-GUARANTEE-0001  - Garantien
; NPSPEC-RESOURCE-IO-0001         - I/O-Ressourcen
; NPSPEC-RESOURCE-LATENCY-0001    - Latenz-Garantien
; NPSPEC-RESOURCE-MEMORY-0001     - Speicher-Ressourcen
; NPSPEC-RESOURCE-NETWORK-0001    - Netzwerk-Ressourcen
; NPSPEC-RESOURCE-NPU-0001        - NPU-Ressourcen
; NPSPEC-RESOURCE-RECLAIM-0001    - Ressource-Rueckforderung
; NPSPEC-RESOURCE-RESERVATION-0001- Reservierungen
; NPSPEC-RESOURCE-THERMAL-0001    - Thermische Ressourcen
; ---------------------------------------------------------------------------

; Ressource-Typen
RES_TYPE_CPU          equ 0    ; (NPSPEC-RESOURCE-CPU-0001)
RES_TYPE_MEMORY       equ 1    ; (NPSPEC-RESOURCE-MEMORY-0001)
RES_TYPE_IO           equ 2    ; (NPSPEC-RESOURCE-IO-0001)
RES_TYPE_NETWORK      equ 3    ; (NPSPEC-RESOURCE-NETWORK-0001)
RES_TYPE_GPU          equ 4    ; (NPSPEC-RESOURCE-GPU-0001)
RES_TYPE_NPU          equ 5    ; (NPSPEC-RESOURCE-NPU-0001)
RES_TYPE_ENERGY       equ 6    ; (NPSPEC-RESOURCE-ENERGY-0001)
RES_TYPE_THERMAL      equ 7    ; (NPSPEC-RESOURCE-THERMAL-0001)
RES_TYPE_COUNT        equ 8

; Guarantee-Typen (NPSPEC-RESOURCE-GUARANTEE-0001)
RES_GUAR_NONE         equ 0
RES_GUAR_BEST_EFFORT  equ 1
RES_GUAR_SOFT         equ 2
RES_GUAR_HARD         equ 3    ; Harte Reservierung

; Ressource-Bucket (32 Bytes, je Typ)
RES_BUCKET_SIZE       equ 32
RB_TYPE               equ 0
RB_TOTAL              equ 4    ; Gesamtkapazitaet
RB_RESERVED           equ 8    ; Reserviert (NPSPEC-RESOURCE-RESERVATION-0001)
RB_USED               equ 12   ; Aktuell genutzt
RB_GUARANTEE          equ 16   ; Garantie-Typ
RB_ECONOMY_COST       equ 20   ; Kosten/Einheit (NPSPEC-RESOURCE-ECONOMY-0001)
RB_ADMIT_THRESHOLD    equ 24   ; Admission-Schwelle in % (NPSPEC-RESOURCE-ADMISSION-0001)
RB_RECLAIM_THRESHOLD  equ 28   ; Rueckforderungs-Schwelle (NPSPEC-RESOURCE-RECLAIM-0001)

; ---------------------------------------------------------------------------
; resource_initialize
; Legt Standard-Buckets fuer alle Ressource-Typen an.
; CF=0 Erfolg
; ---------------------------------------------------------------------------
resource_initialize:
    push edi
    mov edi, res_buckets
    xor eax, eax
    mov ecx, (RES_TYPE_COUNT * RES_BUCKET_SIZE) / 4
    rep stosd
    pop edi

    ; CPU-Bucket: 100 "Einheiten" = 100%
    mov edi, res_buckets
    mov dword [edi + RB_TYPE], RES_TYPE_CPU
    mov dword [edi + RB_TOTAL], 100
    mov dword [edi + RB_GUARANTEE], RES_GUAR_SOFT
    mov dword [edi + RB_ADMIT_THRESHOLD], 80
    mov dword [edi + RB_RECLAIM_THRESHOLD], 90

    ; Memory-Bucket: in Pages (vereinfacht 1024 Pages)
    add edi, RES_BUCKET_SIZE
    mov dword [edi + RB_TYPE], RES_TYPE_MEMORY
    mov dword [edi + RB_TOTAL], 1024
    mov dword [edi + RB_GUARANTEE], RES_GUAR_HARD
    mov dword [edi + RB_ADMIT_THRESHOLD], 85
    mov dword [edi + RB_RECLAIM_THRESHOLD], 95

    ; IO-Bucket: IOPS
    add edi, RES_BUCKET_SIZE
    mov dword [edi + RB_TYPE], RES_TYPE_IO
    mov dword [edi + RB_TOTAL], 10000
    mov dword [edi + RB_GUARANTEE], RES_GUAR_SOFT
    mov dword [edi + RB_ADMIT_THRESHOLD], 70
    mov dword [edi + RB_RECLAIM_THRESHOLD], 85

    ; Network: Mbps
    add edi, RES_BUCKET_SIZE
    mov dword [edi + RB_TYPE], RES_TYPE_NETWORK
    mov dword [edi + RB_TOTAL], 1000
    mov dword [edi + RB_GUARANTEE], RES_GUAR_BEST_EFFORT
    mov dword [edi + RB_ADMIT_THRESHOLD], 80
    mov dword [edi + RB_RECLAIM_THRESHOLD], 90

    mov dword [res_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; res_request: EAX=type, EBX=amount -> CF=0 erlaubt
; (NPSPEC-RESOURCE-ADMISSION-0001, NPSPEC-RESOURCE-ACCOUNTING-0001)
; ---------------------------------------------------------------------------
res_request:
    cmp eax, RES_TYPE_COUNT
    jae .invalid
    push edi
    push ecx
    mov edi, res_buckets
    push eax
    imul eax, RES_BUCKET_SIZE
    add edi, eax
    pop eax
    ; Admission-Check
    mov ecx, [edi + RB_TOTAL]
    push edx
    mov edx, [edi + RB_ADMIT_THRESHOLD]
    ; max_admit = total * threshold / 100
    imul ecx, edx
    push eax
    mov eax, ecx
    mov ecx, 100
    div ecx
    pop ecx                ; eax = max_admit
    pop edx
    ; Bereits genutzt + angefragt <= max_admit?
    mov ecx, [edi + RB_USED]
    add ecx, ebx
    cmp ecx, eax
    ja .deny
    ; Buchen
    add [edi + RB_USED], ebx
    pop ecx
    pop edi
    clc
    ret
.deny:
    pop ecx
    pop edi
    stc
    ret
.invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; res_release: EAX=type, EBX=amount
; (NPSPEC-RESOURCE-RECLAIM-0001)
; ---------------------------------------------------------------------------
res_release:
    cmp eax, RES_TYPE_COUNT
    jae .invalid
    push edi
    mov edi, res_buckets
    push eax
    imul eax, RES_BUCKET_SIZE
    add edi, eax
    pop eax
    cmp [edi + RB_USED], ebx
    jb .underflow
    sub [edi + RB_USED], ebx
    pop edi
    clc
    ret
.underflow:
    mov dword [edi + RB_USED], 0
    pop edi
    clc
    ret
.invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; res_reserve: EAX=type, EBX=amount, ECX=guarantee
; (NPSPEC-RESOURCE-RESERVATION-0001, NPSPEC-RESOURCE-GUARANTEE-0001)
; ---------------------------------------------------------------------------
res_reserve:
    cmp eax, RES_TYPE_COUNT
    jae .invalid
    push edi
    mov edi, res_buckets
    push eax
    imul eax, RES_BUCKET_SIZE
    add edi, eax
    pop eax
    ; Nur reservieren wenn Kapazitaet vorhanden
    mov edx, [edi + RB_TOTAL]
    sub edx, [edi + RB_RESERVED]
    cmp ebx, edx
    ja .no_capacity
    add [edi + RB_RESERVED], ebx
    mov [edi + RB_GUARANTEE], ecx
    pop edi
    clc
    ret
.no_capacity:
    pop edi
    stc
    ret
.invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; resource_self_test
; ---------------------------------------------------------------------------
resource_self_test:
    push esi
    xor esi, esi
    cmp dword [res_ready], 1
    je .t2
    inc esi
.t2:
    mov eax, RES_TYPE_CPU
    mov ebx, 20        ; 20% CPU
    call res_request
    jnc .t3
    inc esi
.t3:
    call res_release
    jnc .t4
    inc esi
.t4:
    mov eax, RES_TYPE_MEMORY
    mov ebx, 64        ; 64 Pages
    mov ecx, RES_GUAR_HARD
    call res_reserve
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
res_ready:    dd 0

align 4
res_buckets:
    times RES_TYPE_COUNT * RES_BUCKET_SIZE db 0
