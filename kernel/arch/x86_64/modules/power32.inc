; ---------------------------------------------------------------------------
; Nova Power Management (Erweiterung von integration32.inc)
; NPSPEC-POWER-ARCH-0001         - Architektur
; NPSPEC-POWER-ACPI-0001         - ACPI-Integration
; NPSPEC-POWER-BATTERY-0001      - Batterie-Management
; NPSPEC-POWER-BOOST-0001        - Boost-Steuerung
; NPSPEC-POWER-CPUIDLE-0001      - CPU-Idle-States
; NPSPEC-POWER-CPUPERFORMANCE-0001 - P-States / Perf-Skalierung
; NPSPEC-POWER-DEVICE-0001       - Geraete-Power-Management
; NPSPEC-POWER-DISPLAY-0001      - Display-Power
; NPSPEC-POWER-DVFS-0001         - Dynamic Voltage/Frequency Scaling
; NPSPEC-POWER-ENERGYACCOUNTING-0001 - Energie-Buchfuehrung
; NPSPEC-POWER-ENERGYBUDGET-0001 - Energie-Budget
; NPSPEC-POWER-HETEROGENEOUSCPU-0001 - Big.LITTLE / Hybride CPUs
; NPSPEC-POWER-HIBERNATE-0001    - Hibernation
; NPSPEC-POWER-HYBRIDSLEEP-0001  - Hybrid-Sleep
; NPSPEC-POWER-INTROSPECTION-0001- Introspection
; NPSPEC-POWER-LOWPOWERIDLE-0001 - Low-Power-Idle (S0ix)
; NPSPEC-POWER-NETWORK-0001      - Netzwerk-Power
; NPSPEC-POWER-PCIEASPM-0001     - PCIe ASPM
; NPSPEC-POWER-PLATFORM-0001     - Plattform-Power
; NPSPEC-POWER-POLICY-0001       - Power-Policy
; NPSPEC-POWER-RUNTIME-0001      - Runtime-PM
; NPSPEC-POWER-STORAGE-0001      - Storage-Power
; NPSPEC-POWER-SUSPEND-0001      - Suspend-to-RAM
; NPSPEC-POWER-WAKE-0001         - Wake-Sources
; NPSPEC-POWER-WAKELEASE-0001    - Wake-Leases (Wakelock)
; ---------------------------------------------------------------------------

; CPU-Idle-Zustaende (NPSPEC-POWER-CPUIDLE-0001)
CPUIDLE_C0            equ 0    ; Aktiv
CPUIDLE_C1            equ 1    ; Halt (HLT)
CPUIDLE_C1E           equ 2    ; Enhanced Halt
CPUIDLE_C3            equ 3    ; Sleep (Flush+Stop)
CPUIDLE_C6            equ 4    ; Deep Power Down
CPUIDLE_C7            equ 5    ; Enhanced Deep Power Down

; P-State-IDs (NPSPEC-POWER-CPUPERFORMANCE-0001)
PSTATE_MAX            equ 0    ; Maximale Leistung
PSTATE_TURBO          equ 1    ; Turbo-Boost (NPSPEC-POWER-BOOST-0001)
PSTATE_NOMINAL        equ 2    ; Nennleistung
PSTATE_EFFICIENT      equ 3    ; Energie-effizient
PSTATE_MIN            equ 4    ; Minimum

; DVFS-Stufen (NPSPEC-POWER-DVFS-0001)
DVFS_LEVEL_HIGH       equ 0    ; Volle Spannung/Frequenz
DVFS_LEVEL_MEDIUM     equ 1
DVFS_LEVEL_LOW        equ 2
DVFS_LEVEL_ULTRA_LOW  equ 3

; Geraete-Power-Zustaende (NPSPEC-POWER-DEVICE-0001)
DEV_PM_D0             equ 0    ; Voll aktiv
DEV_PM_D1             equ 1    ; Light Sleep
DEV_PM_D2             equ 2    ; Deeper Sleep
DEV_PM_D3HOT          equ 3    ; Off (Strom erhalten)
DEV_PM_D3COLD         equ 4    ; Off (kein Strom)

; Platform-Power-Zustaende (NPSPEC-POWER-PLATFORM-0001)
PLAT_S0               equ 0    ; Working
PLAT_S0IX             equ 1    ; Low Power Idle (NPSPEC-POWER-LOWPOWERIDLE-0001)
PLAT_S3               equ 3    ; Suspend-to-RAM (NPSPEC-POWER-SUSPEND-0001)
PLAT_S4               equ 4    ; Hibernate (NPSPEC-POWER-HIBERNATE-0001)
PLAT_S5               equ 5    ; Soft Off

; Hybrid-CPU-Typen (NPSPEC-POWER-HETEROGENEOUSCPU-0001)
CPU_CORE_BIG          equ 0    ; Performance-Kern
CPU_CORE_LITTLE       equ 1    ; Effizienz-Kern

; Wake-Quellen (NPSPEC-POWER-WAKE-0001)
WAKE_SRC_RTC          equ 0x01
WAKE_SRC_USB          equ 0x02
WAKE_SRC_NETWORK      equ 0x04  ; (NPSPEC-POWER-NETWORK-0001)
WAKE_SRC_KBD          equ 0x08
WAKE_SRC_PCIE         equ 0x10  ; (NPSPEC-POWER-PCIEASPM-0001)
WAKE_SRC_DISPLAY      equ 0x20  ; (NPSPEC-POWER-DISPLAY-0001)

; Energie-Budget-Einheit: Milliwatt-Sekunden (mWs)

; CPU-Core-Record (32 Bytes, NPSPEC-POWER-HETEROGENEOUSCPU-0001)
CPU_CORE_REC_SIZE     equ 32
CC_ID                 equ 0
CC_TYPE               equ 4    ; CPU_CORE_BIG / LITTLE
CC_PSTATE             equ 8    ; Aktueller P-State
CC_CSTATE             equ 12   ; Aktueller C-State
CC_FREQ_KHZ           equ 16   ; Aktuelle Frequenz
CC_VOLT_MV            equ 20   ; Aktuelle Spannung (mV)
CC_ENERGY_MWS         equ 24   ; Verbrauchte Energie (mWs)
CC_IDLE_TICKS         equ 28   ; Idle-Ticks

CPU_CORE_CAPACITY     equ 8

; Geraete-Power-Record (32 Bytes, NPSPEC-POWER-DEVICE-0001)
DEV_PWR_REC_SIZE      equ 32
DP_DEVICE_ID          equ 0
DP_PM_STATE           equ 4    ; DEV_PM_D*
DP_BUDGET_MWS         equ 8    ; Energie-Budget
DP_USED_MWS           equ 12   ; Verbrauchte Energie
DP_WAKE_MASK          equ 16   ; erlaubte Wake-Quellen
DP_RUNTIME_AUTOSUSPEND equ 20  ; Timeout in Ticks
DP_LAST_ACCESS        equ 24

DEV_PWR_CAPACITY      equ 8

; Batterie-Record (32 Bytes, NPSPEC-POWER-BATTERY-0001)
BAT_REC_SIZE          equ 32
BT_PRESENT            equ 0    ; 1=vorhanden
BT_CAPACITY_MAH       equ 4    ; Design-Kapazitaet (mAh)
BT_CHARGE_MAH         equ 8    ; Aktuelle Ladung (mAh)
BT_VOLTAGE_MV         equ 12   ; Aktuelle Spannung (mV)
BT_CURRENT_MA         equ 16   ; Strom (mA, signed)
BT_TEMP_MC            equ 20   ; Temperatur (Millicelsius)
BT_CHARGE_PERCENT     equ 24   ; Ladezustand 0..100
BT_TIME_TO_EMPTY      equ 28   ; Minuten bis leer (0xFFFF=unbekannt)

; ---------------------------------------------------------------------------
; power_extended_initialize
; Erweitert den bestehenden power_manager um §043-Funktionen.
; CF=0 Erfolg
; ---------------------------------------------------------------------------
power_extended_initialize:
    ; CPU-Core-Tabelle initialisieren
    mov edi, power_cpu_cores
    xor eax, eax
    mov ecx, (CPU_CORE_CAPACITY * CPU_CORE_REC_SIZE) / 4
    rep stosd
    ; Geraete-Power-Tabelle
    mov edi, power_dev_table
    mov ecx, (DEV_PWR_CAPACITY * DEV_PWR_REC_SIZE) / 4
    rep stosd
    ; Batterie-Record
    mov edi, power_battery
    mov ecx, BAT_REC_SIZE / 4
    rep stosd

    mov dword [power_platform_state], PLAT_S0
    mov dword [power_ext_idle_state], CPUIDLE_C0
    mov dword [power_dvfs_level], DVFS_LEVEL_MEDIUM
    mov dword [power_energy_total_mws], 0
    mov dword [power_budget_mws], 100000   ; 100 Watt-Sekunden Default
    mov dword [power_wake_sources], WAKE_SRC_RTC | WAKE_SRC_KBD
    mov dword [power_cpu_count], 0
    mov dword [power_dev_count], 0

    ; Virtuellen CPU-Core 0 registrieren (NPSPEC-POWER-HETEROGENEOUSCPU-0001)
    mov eax, 0               ; Core-ID
    mov ebx, CPU_CORE_BIG    ; Performance-Core
    call power_register_cpu_core

    mov dword [power_ext_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; power_register_cpu_core: EAX=core_id, EBX=type
; ---------------------------------------------------------------------------
power_register_cpu_core:
    cmp dword [power_cpu_count], CPU_CORE_CAPACITY
    jae .full
    push edi
    mov edi, power_cpu_cores
    push ecx
    mov ecx, [power_cpu_count]
    imul ecx, CPU_CORE_REC_SIZE
    add edi, ecx
    pop ecx
    mov [edi + CC_ID], eax
    mov [edi + CC_TYPE], ebx
    mov dword [edi + CC_PSTATE], PSTATE_NOMINAL
    mov dword [edi + CC_CSTATE], CPUIDLE_C0
    mov dword [edi + CC_FREQ_KHZ], 2000000  ; 2 GHz Default
    mov dword [edi + CC_VOLT_MV], 1000      ; 1V Default
    mov dword [edi + CC_ENERGY_MWS], 0
    mov dword [edi + CC_IDLE_TICKS], 0
    inc dword [power_cpu_count]
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; power_set_pstate: EAX=core_id, EBX=pstate
; (NPSPEC-POWER-CPUPERFORMANCE-0001, NPSPEC-POWER-BOOST-0001)
; ---------------------------------------------------------------------------
power_set_pstate:
    push esi
    push ecx
    mov esi, power_cpu_cores
    mov ecx, [power_cpu_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + CC_ID], eax
    je .found
    add esi, CPU_CORE_REC_SIZE
    dec ecx
    jmp .scan
.found:
    mov [esi + CC_PSTATE], ebx
    ; Frequenz je nach P-State (vereinfacht, NPSPEC-POWER-DVFS-0001)
    cmp ebx, PSTATE_TURBO
    je .turbo
    cmp ebx, PSTATE_MAX
    je .max
    cmp ebx, PSTATE_EFFICIENT
    je .efficient
    cmp ebx, PSTATE_MIN
    je .min
    mov dword [esi + CC_FREQ_KHZ], 2000000   ; nominal: 2GHz
    jmp .done
.turbo:
    mov dword [esi + CC_FREQ_KHZ], 4000000   ; 4GHz
    jmp .done
.max:
    mov dword [esi + CC_FREQ_KHZ], 3500000   ; 3.5GHz
    jmp .done
.efficient:
    mov dword [esi + CC_FREQ_KHZ], 1200000   ; 1.2GHz
    jmp .done
.min:
    mov dword [esi + CC_FREQ_KHZ], 400000    ; 400MHz
.done:
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
; power_enter_idle: EAX=cstate
; Setzt CPU in Idle-Zustand (NPSPEC-POWER-CPUIDLE-0001).
; ---------------------------------------------------------------------------
power_enter_idle:
    mov [power_ext_idle_state], eax
    cmp eax, CPUIDLE_C0
    je .done
    cmp eax, CPUIDLE_C1
    jb .done
    ; HLT-Instruktion fuer C1+
    hlt
.done:
    clc
    ret

; ---------------------------------------------------------------------------
; power_device_register: EAX=dev_id, EBX=budget_mws, ECX=wake_mask
; (NPSPEC-POWER-DEVICE-0001, NPSPEC-POWER-RUNTIME-0001)
; ---------------------------------------------------------------------------
power_device_register:
    cmp dword [power_dev_count], DEV_PWR_CAPACITY
    jae .full
    push edi
    mov edi, power_dev_table
    push ecx
    push edx
    mov edx, [power_dev_count]
    imul edx, DEV_PWR_REC_SIZE
    add edi, edx
    pop edx
    mov [edi + DP_DEVICE_ID], eax
    mov dword [edi + DP_PM_STATE], DEV_PM_D0
    mov [edi + DP_BUDGET_MWS], ebx
    mov dword [edi + DP_USED_MWS], 0
    pop ecx
    mov [edi + DP_WAKE_MASK], ecx
    mov dword [edi + DP_RUNTIME_AUTOSUSPEND], 500  ; 5s bei 100Hz
    mov eax, [timer_ticks]
    mov [edi + DP_LAST_ACCESS], eax
    inc dword [power_dev_count]
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; power_device_set_state: EAX=dev_id, EBX=pm_state
; ---------------------------------------------------------------------------
power_device_set_state:
    push esi
    push ecx
    mov esi, power_dev_table
    mov ecx, [power_dev_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + DP_DEVICE_ID], eax
    je .found
    add esi, DEV_PWR_REC_SIZE
    dec ecx
    jmp .scan
.found:
    mov [esi + DP_PM_STATE], ebx
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
; power_energy_account: EAX=mws_delta
; Bucht Energie in Gesamtkonto (NPSPEC-POWER-ENERGYACCOUNTING-0001).
; ---------------------------------------------------------------------------
power_energy_account:
    add [power_energy_total_mws], eax
    ; Budget-Pruefung (NPSPEC-POWER-ENERGYBUDGET-0001)
    mov ecx, [power_energy_total_mws]
    cmp ecx, [power_budget_mws]
    jb .ok
    ; Budget ueberschritten: Drosselung erhoehen
    cmp dword [power_dvfs_level], DVFS_LEVEL_ULTRA_LOW
    je .ok
    inc dword [power_dvfs_level]
.ok:
    clc
    ret

; ---------------------------------------------------------------------------
; power_set_platform_state: EAX=PLAT_*
; (NPSPEC-POWER-PLATFORM-0001, NPSPEC-POWER-SUSPEND-0001,
;  NPSPEC-POWER-HIBERNATE-0001, NPSPEC-POWER-HYBRIDSLEEP-0001)
; ---------------------------------------------------------------------------
power_set_platform_state:
    mov [power_platform_state], eax
    cmp eax, PLAT_S3
    je .suspend
    cmp eax, PLAT_S4
    je .hibernate
    cmp eax, PLAT_S0IX
    je .low_power
    clc
    ret
.suspend:
    ; S3: Inhalt in RAM sichern, Strom halten
    ; Stub: nur Status setzen
    clc
    ret
.hibernate:
    ; S4: Inhalt auf Storage schreiben
    ; Stub: power_manager_request_shutdown aufrufen
    call power_manager_request_shutdown
    ret
.low_power:
    ; S0ix: CPU in tiefen Idle-State versetzen
    mov eax, CPUIDLE_C6
    call power_enter_idle
    ret

; ---------------------------------------------------------------------------
; power_extended_self_test
; ---------------------------------------------------------------------------
power_extended_self_test:
    push esi
    xor esi, esi

    cmp dword [power_ext_ready], 1
    je .t2
    inc esi
.t2:
    ; CPU-Core vorhanden?
    cmp dword [power_cpu_count], 0
    jne .t3
    inc esi
.t3:
    ; P-State setzen
    mov eax, 0              ; Core 0
    mov ebx, PSTATE_EFFICIENT
    call power_set_pstate
    jnc .t4
    inc esi
.t4:
    ; Energie buchen
    mov eax, 100            ; 100 mWs
    call power_energy_account
    jnc .t5
    inc esi
.t5:
    ; Geraet registrieren
    mov eax, 0xFF           ; Test-Geraet
    mov ebx, 5000           ; 5 Ws Budget
    mov ecx, WAKE_SRC_USB
    call power_device_register
    jnc .done
    inc esi
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
power_ext_ready:          dd 0
power_platform_state:     dd PLAT_S0
power_ext_idle_state:     dd CPUIDLE_C0
power_dvfs_level:         dd DVFS_LEVEL_MEDIUM
power_energy_total_mws:   dd 0
power_budget_mws:         dd 100000
power_wake_sources:       dd 0
power_cpu_count:          dd 0
power_dev_count:          dd 0

align 4
power_cpu_cores:
    times CPU_CORE_CAPACITY * CPU_CORE_REC_SIZE db 0

align 4
power_dev_table:
    times DEV_PWR_CAPACITY * DEV_PWR_REC_SIZE db 0

align 4
power_battery:
    times BAT_REC_SIZE db 0
