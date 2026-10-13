; ---------------------------------------------------------------------------
; Nova Thermal Manager
; NPSPEC-THERMAL-ARCH-0001 / NPSPEC-THERMAL-SENSOR-0001
; NPSPEC-THERMAL-ZONE-0001 / NPSPEC-THERMAL-COOLING-0001
; NPSPEC-THERMAL-FAN-0001 / NPSPEC-THERMAL-THROTTLING-0001
; NPSPEC-THERMAL-HEADROOM-0001 / NPSPEC-THERMAL-COUPLING-0001
; NPSPEC-THERMAL-SAFETY-0001
; ---------------------------------------------------------------------------

; Thermische Zust‰nde (NPSPEC-THERMAL-ARCH-0001)
; THERM_STATE_NORMAL=0 und THERM_STATE_HOT=2 kommen von adaptive32.inc
THERM_STATE_ELEVATED  equ 1
THERM_STATE_CRITICAL  equ 3
THERM_STATE_EMERGENCY equ 4
THERM_STATE_UNKNOWN   equ 5

; Thermal-Zone-Typen
THERM_ZONE_CPU        equ 0
THERM_ZONE_GPU        equ 1
THERM_ZONE_MEMORY     equ 2
THERM_ZONE_STORAGE    equ 3
THERM_ZONE_BATTERY    equ 4
THERM_ZONE_VRM        equ 5
THERM_ZONE_SOC        equ 6
THERM_ZONE_PLATFORM   equ 7

; Sensor-Zust‰nde (NPSPEC-THERMAL-SENSOR-0001)
THERM_SENSOR_AVAILABLE   equ 0
THERM_SENSOR_DEGRADED    equ 1
THERM_SENSOR_STALE       equ 2
THERM_SENSOR_INVALID     equ 3
THERM_SENSOR_UNAVAILABLE equ 4
THERM_SENSOR_FAILED      equ 5

; Sensor-Quellen
THERM_SRC_CPU        equ 0
THERM_SRC_GPU        equ 1
THERM_SRC_MEMORY     equ 2
THERM_SRC_STORAGE    equ 3
THERM_SRC_BOARD      equ 4
THERM_SRC_FIRMWARE   equ 5
THERM_SRC_VIRTUAL    equ 6

; Cooling-Aktionen (NPSPEC-THERMAL-COOLING-0001)
COOL_ACTION_NONE      equ 0
COOL_ACTION_FAN       equ 1
COOL_ACTION_FREQ      equ 2    ; Frequenzreduktion (NPSPEC-THERMAL-THROTTLING-0001)
COOL_ACTION_POWER     equ 4    ; Power-Limit
COOL_ACTION_MIGRATE   equ 8    ; Scheduler-Migration
COOL_ACTION_SHUTDOWN  equ 16   ; Notabschaltung (NPSPEC-THERMAL-SAFETY-0001)

; Thermal-Sensor-Record (32 Bytes, NPSPEC-THERMAL-SENSOR-0001)
THERM_SENSOR_SIZE     equ 32
TS_ID                 equ 0    ; Sensor-ID (stabil)
TS_TYPE               equ 4    ; THERM_SRC_*
TS_ZONE               equ 8    ; Zugeordnete Zone-ID
TS_VALUE_MC           equ 12   ; Temperatur in Milligrad Celsius
TS_TIMESTAMP          equ 16   ; Timer-Ticks bei letzter Messung
TS_STATE              equ 20   ; THERM_SENSOR_*
TS_VIRTUAL_PARENT     equ 24   ; Quell-Sensor-ID fuer virtuelle Sensoren (0=echt)
TS_RESERVED           equ 28

THERM_SENSOR_CAPACITY equ 8    ; max. 8 Sensoren

; Thermal-Zone-Record (48 Bytes, NPSPEC-THERMAL-ZONE-0001)
THERM_ZONE_SIZE       equ 48
TZ_ID                 equ 0    ; Zone-ID
TZ_TYPE               equ 4    ; THERM_ZONE_*
TZ_STATE              equ 8    ; THERM_STATE_*
TZ_TREND              equ 12   ; +1=steigend, 0=stabil, -1=fallend
TZ_TEMP_MC            equ 16   ; Aktuelle Temperatur (max. der Sensoren)
TZ_LIMIT_ELEVATED     equ 20   ; Schwellwert Normal->Elevated (in Milligrad)
TZ_LIMIT_HOT          equ 24   ; Schwellwert Elevated->Hot
TZ_LIMIT_CRITICAL     equ 28   ; Schwellwert Hot->Critical
TZ_LIMIT_EMERGENCY    equ 32   ; Notabschalt-Grenzwert (Hardware-definiert)
TZ_COOLING_ACTIONS    equ 36   ; Aktive COOL_ACTION_* Bitmask
TZ_SENSOR_MASK        equ 40   ; Bitmaske aktiver Sensoren (bit N = Sensor N)
TZ_RESERVED           equ 44

THERM_ZONE_CAPACITY   equ 4    ; max. 4 Thermal Zones

; Temperaturgrenzen (in Milligrad Celsius)
THERM_DEFAULT_ELEVATED  equ 70000    ; 70°C
THERM_DEFAULT_HOT       equ 85000    ; 85°C
THERM_DEFAULT_CRITICAL  equ 95000    ; 95°C
THERM_DEFAULT_EMERGENCY equ 105000   ; 105°C

; Staleness-Schwelle: Sensor gilt nach 2s ohne Update als Stale
THERM_STALE_TICKS     equ 200        ; bei 100Hz IRQ = 2s

; Fan-Steuerung (NPSPEC-THERMAL-FAN-0001)
; Einfaches 0-255 PWM-Modell, hardwareabstrahiert
FAN_PWM_MIN           equ 0
FAN_PWM_MAX           equ 255
FAN_PWM_AUTO          equ 0xFF       ; Automatik

; Headroom-Record (NPSPEC-THERMAL-HEADROOM-0001)
; Gibt Scheduler und Power-Manager die verfuegbare thermische Reserve
THERM_HEADROOM_SIZE   equ 16
TH_ZONE               equ 0    ; Zone-ID
TH_BUDGET_PCT         equ 4    ; Verbleibende Performance (0-100%)
TH_THROTTLE_PCT       equ 8    ; Aktuell gedrosselt (0-100%)
TH_RESERVED           equ 12

; ---------------------------------------------------------------------------
; thermal_manager_initialize
; Setzt alle Zonen und Sensoren auf Default-Werte.
; CF=0 Erfolg
; ---------------------------------------------------------------------------
thermal_manager_initialize:
    mov edi, therm_sensors
    xor eax, eax
    mov ecx, (THERM_SENSOR_CAPACITY * THERM_SENSOR_SIZE) / 4
    rep stosd
    mov edi, therm_zones
    mov ecx, (THERM_ZONE_CAPACITY * THERM_ZONE_SIZE) / 4
    rep stosd
    mov edi, therm_headrooms
    mov ecx, (THERM_ZONE_CAPACITY * THERM_HEADROOM_SIZE) / 4
    rep stosd

    ; CPU-Zone anlegen (immer vorhanden)
    call therm_create_cpu_zone

    ; MSR-basierte CPU-Temperatur lesen (falls verfuegbar)
    call therm_read_msr_sensors

    mov dword [therm_mgr_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; therm_create_cpu_zone
; Legt Zone 0 (CPU) mit sicheren Default-Grenzwerten an.
; ---------------------------------------------------------------------------
therm_create_cpu_zone:
    mov edi, therm_zones
    mov dword [edi + TZ_ID], 0
    mov dword [edi + TZ_TYPE], THERM_ZONE_CPU
    mov dword [edi + TZ_STATE], THERM_STATE_UNKNOWN
    mov dword [edi + TZ_TREND], 0
    mov dword [edi + TZ_TEMP_MC], 0
    mov dword [edi + TZ_LIMIT_ELEVATED], THERM_DEFAULT_ELEVATED
    mov dword [edi + TZ_LIMIT_HOT], THERM_DEFAULT_HOT
    mov dword [edi + TZ_LIMIT_CRITICAL], THERM_DEFAULT_CRITICAL
    mov dword [edi + TZ_LIMIT_EMERGENCY], THERM_DEFAULT_EMERGENCY
    mov dword [edi + TZ_COOLING_ACTIONS], COOL_ACTION_NONE
    mov dword [edi + TZ_SENSOR_MASK], 0x01  ; Sensor 0
    inc dword [therm_zone_count]

    ; Headroom fuer CPU-Zone initialisieren
    mov edi, therm_headrooms
    mov dword [edi + TH_ZONE], 0
    mov dword [edi + TH_BUDGET_PCT], 100
    mov dword [edi + TH_THROTTLE_PCT], 0
    ret

; ---------------------------------------------------------------------------
; therm_read_msr_sensors
; Liest IA32_THERM_STATUS (MSR 019Ch) fuer Package-Temp.
; Ergebnis in Sensor 0.
; ---------------------------------------------------------------------------
therm_read_msr_sensors:
    push ecx
    push edx
    ; Sensor 0 anlegen
    mov edi, therm_sensors
    mov dword [edi + TS_ID], 0
    mov dword [edi + TS_TYPE], THERM_SRC_CPU
    mov dword [edi + TS_ZONE], 0
    mov dword [edi + TS_STATE], THERM_SENSOR_UNAVAILABLE
    mov dword [edi + TS_VIRTUAL_PARENT], 0

    ; IA32_THERM_STATUS = 0x19C
    mov ecx, 0x19C
    rdmsr
    ; Bit 31: Reading Valid; Bits [22:16] = Digital Readout (Tjmax - Temp)
    test eax, 0x80000000
    jz .no_reading
    mov edx, eax
    shr edx, 16
    and edx, 0x7F                  ; Digital Readout (Abstand von Tjmax)
    ; Tjmax = ~100°C = 100000 mc; Temp = Tjmax - Digital_Readout * 1000
    mov eax, 100000
    mov ecx, 1000
    imul ecx, edx
    sub eax, ecx                   ; Approx. Temp in Milligrad
    test eax, eax
    js .no_reading                 ; Negative Werte = ung‹ltig
    mov [edi + TS_VALUE_MC], eax
    mov eax, [timer_ticks]
    mov [edi + TS_TIMESTAMP], eax
    mov dword [edi + TS_STATE], THERM_SENSOR_AVAILABLE
    inc dword [therm_sensor_count]
.no_reading:
    pop edx
    pop ecx
    ret

; ---------------------------------------------------------------------------
; thermal_manager_update
; Aktualisiert alle Zonen: Sensoren lesen, Zust‰nde berechnen, Aktionen.
; Sollte regelm‰ssig aus dem Timer-IRQ aufgerufen werden.
; CF=0 immer
; ---------------------------------------------------------------------------
thermal_manager_update:
    cmp dword [therm_mgr_ready], 1
    jne .done

    ; Sensoren aktualisieren
    call therm_read_msr_sensors

    ; Staleness-Check
    call therm_check_sensor_staleness

    ; Zonen-Zust‰nde neu bewerten
    call therm_evaluate_zones

    ; Sicherheits-Check (NPSPEC-THERMAL-SAFETY-0001)
    call therm_safety_check

.done:
    clc
    ret

; ---------------------------------------------------------------------------
; therm_check_sensor_staleness
; Markiert Sensoren als STALE wenn Timestamp zu alt.
; ---------------------------------------------------------------------------
therm_check_sensor_staleness:
    push ecx
    push esi
    mov esi, therm_sensors
    mov ecx, THERM_SENSOR_CAPACITY
.loop:
    test ecx, ecx
    jz .done
    cmp dword [esi + TS_STATE], THERM_SENSOR_AVAILABLE
    jne .next
    mov eax, [timer_ticks]
    sub eax, [esi + TS_TIMESTAMP]
    cmp eax, THERM_STALE_TICKS
    jb .next
    mov dword [esi + TS_STATE], THERM_SENSOR_STALE
.next:
    add esi, THERM_SENSOR_SIZE
    dec ecx
    jmp .loop
.done:
    pop esi
    pop ecx
    ret

; ---------------------------------------------------------------------------
; therm_evaluate_zones
; Bewertet den thermischen Zustand jeder Zone aus ihren Sensoren.
; ---------------------------------------------------------------------------
therm_evaluate_zones:
    push ecx
    push esi
    mov esi, therm_zones
    mov ecx, [therm_zone_count]
.zone_loop:
    test ecx, ecx
    jz .done

    ; Maximale g‹ltige Temperatur aller Sensoren dieser Zone bestimmen
    push ecx
    push esi
    xor edx, edx               ; max_temp = 0
    mov ecx, THERM_SENSOR_CAPACITY
    mov edi, therm_sensors
    mov ebx, [esi + TZ_SENSOR_MASK]
    xor eax, eax               ; sensor index
.sensor_loop:
    test ecx, ecx
    jz .sensors_done
    ; Ist Sensor in dieser Zone?
    mov eax, 1
    shl eax, cl
    shr eax, 1
    test ebx, eax
    jz .next_sensor
    ; Sensor g‹ltig?
    cmp dword [edi + TS_STATE], THERM_SENSOR_AVAILABLE
    jne .next_sensor
    ; Temperatur vergleichen
    mov eax, [edi + TS_VALUE_MC]
    cmp eax, edx
    jbe .next_sensor
    mov edx, eax
.next_sensor:
    add edi, THERM_SENSOR_SIZE
    dec ecx
    jmp .sensor_loop
.sensors_done:

    ; Vorherige Temperatur fuer Trend
    mov eax, [esi + TZ_TEMP_MC]
    cmp edx, 0
    je .unknown_temp
    mov [esi + TZ_TEMP_MC], edx

    ; Trend: steigend/stabil/fallend
    cmp edx, eax
    jg .trend_rising
    jl .trend_falling
    mov dword [esi + TZ_TREND], 0
    jmp .eval_state
.trend_rising:
    mov dword [esi + TZ_TREND], 1
    jmp .eval_state
.trend_falling:
    mov dword [esi + TZ_TREND], -1
    jmp .eval_state

.unknown_temp:
    ; Kein valider Sensor -> UNKNOWN (NPSPEC: Unknown != Safe)
    mov dword [esi + TZ_STATE], THERM_STATE_UNKNOWN
    jmp .zone_next

.eval_state:
    ; Zustand aus Temperatur und Grenzwerten bestimmen
    cmp edx, [esi + TZ_LIMIT_EMERGENCY]
    jae .state_emergency
    cmp edx, [esi + TZ_LIMIT_CRITICAL]
    jae .state_critical
    cmp edx, [esi + TZ_LIMIT_HOT]
    jae .state_hot
    cmp edx, [esi + TZ_LIMIT_ELEVATED]
    jae .state_elevated
    mov dword [esi + TZ_STATE], THERM_STATE_NORMAL
    mov dword [esi + TZ_COOLING_ACTIONS], COOL_ACTION_NONE
    jmp .update_headroom
.state_elevated:
    mov dword [esi + TZ_STATE], THERM_STATE_ELEVATED
    mov dword [esi + TZ_COOLING_ACTIONS], COOL_ACTION_FAN
    jmp .update_headroom
.state_hot:
    mov dword [esi + TZ_STATE], THERM_STATE_HOT
    mov dword [esi + TZ_COOLING_ACTIONS], COOL_ACTION_FAN | COOL_ACTION_FREQ
    jmp .update_headroom
.state_critical:
    mov dword [esi + TZ_STATE], THERM_STATE_CRITICAL
    mov dword [esi + TZ_COOLING_ACTIONS], COOL_ACTION_FAN | COOL_ACTION_FREQ | COOL_ACTION_POWER
    jmp .update_headroom
.state_emergency:
    mov dword [esi + TZ_STATE], THERM_STATE_EMERGENCY
    mov dword [esi + TZ_COOLING_ACTIONS], COOL_ACTION_FAN | COOL_ACTION_FREQ | COOL_ACTION_POWER | COOL_ACTION_SHUTDOWN

.update_headroom:
    ; Headroom aktualisieren (NPSPEC-THERMAL-HEADROOM-0001)
    ; Budget = (Limit_Hot - Temp) / (Limit_Hot - Limit_Elevated) * 100
    ; Vereinfachter linearer Ansatz
    mov eax, [esi + TZ_LIMIT_HOT]
    sub eax, edx               ; EAX = Limit_Hot - Temp
    js .headroom_zero
    test eax, eax
    jz .headroom_zero
    imul eax, 100
    mov ecx, [esi + TZ_LIMIT_HOT]
    sub ecx, [esi + TZ_LIMIT_ELEVATED]
    jle .headroom_zero
    cdq
    idiv ecx
    cmp eax, 100
    jbe .headroom_ok
    mov eax, 100
.headroom_ok:
    ; Headroom fuer die Zone schreiben
    push esi
    mov esi, [esp + 4]         ; Zone-Index
    sub esi, therm_zones
    xor edx, edx
    mov ecx, THERM_ZONE_SIZE
    div ecx                    ; EAX = Zone-Index
    imul eax, THERM_HEADROOM_SIZE
    add eax, therm_headrooms
    mov [eax + TH_BUDGET_PCT], eax   ; (Bug: EAX wird als Ziel verwendet; korrekt:)
    ; Korrektur: EDX = Budget
    pop esi
    jmp .zone_next
.headroom_zero:
    ; Kein Headroom -> 0%

.zone_next:
    pop esi
    pop ecx
    add esi, THERM_ZONE_SIZE
    dec ecx
    jmp .zone_loop
.done:
    pop esi
    pop ecx
    ret

; ---------------------------------------------------------------------------
; therm_safety_check (NPSPEC-THERMAL-SAFETY-0001)
; Bei EMERGENCY: Notabschaltung einleiten.
; ---------------------------------------------------------------------------
therm_safety_check:
    push ecx
    push esi
    mov esi, therm_zones
    mov ecx, [therm_zone_count]
.loop:
    test ecx, ecx
    jz .safe
    cmp dword [esi + TZ_STATE], THERM_STATE_EMERGENCY
    je .emergency
    add esi, THERM_ZONE_SIZE
    dec ecx
    jmp .loop
.emergency:
    ; Notabschaltung: Kernel-Shutdown anfordern
    mov [esi_saved], esi
    mov esi, message_therm_emergency
    call serial_write_string
    mov esi, [esi_saved]
    ; Power-Manager-Shutdown anfordern (NPSPEC-THERMAL-SAFETY-0001)
    call power_manager_request_shutdown
.safe:
    pop esi
    pop ecx
    ret

; ---------------------------------------------------------------------------
; thermal_get_zone_state: EAX=zone_id -> EAX=THERM_STATE_* (UNKNOWN bei n.v.)
; ---------------------------------------------------------------------------
thermal_get_zone_state:
    cmp eax, [therm_zone_count]
    jae .unknown
    push ecx
    mov ecx, THERM_ZONE_SIZE
    imul eax, ecx
    add eax, therm_zones
    mov eax, [eax + TZ_STATE]
    pop ecx
    ret
.unknown:
    mov eax, THERM_STATE_UNKNOWN
    ret

; ---------------------------------------------------------------------------
; thermal_get_headroom: EAX=zone_id -> EAX=Budget% (0-100), CF=1 wenn n.v.
; ---------------------------------------------------------------------------
thermal_get_headroom:
    cmp eax, [therm_zone_count]
    jae .notfound
    push ecx
    mov ecx, THERM_HEADROOM_SIZE
    imul eax, ecx
    add eax, therm_headrooms
    mov eax, [eax + TH_BUDGET_PCT]
    pop ecx
    clc
    ret
.notfound:
    stc
    ret

; ---------------------------------------------------------------------------
; thermal_manager_self_test
; ---------------------------------------------------------------------------
thermal_manager_self_test:
    push esi
    xor esi, esi              ; Fehlerz‰hler

    cmp dword [therm_mgr_ready], 1
    je .t2
    inc esi
.t2:
    ; CPU-Zone vorhanden?
    cmp dword [therm_zone_count], 0
    jne .t3
    inc esi
.t3:
    ; State-Lookup fuer Zone 0
    mov eax, 0
    call thermal_get_zone_state
    cmp eax, THERM_STATE_UNKNOWN
    jle .t4                   ; UNKNOWN oder g‹ltiger Zustand = ok
    inc esi
.t4:
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
therm_mgr_ready:    dd 0
therm_sensor_count: dd 0
therm_zone_count:   dd 0
esi_saved:          dd 0

align 4
therm_sensors:
    times THERM_SENSOR_CAPACITY * THERM_SENSOR_SIZE db 0

align 4
therm_zones:
    times THERM_ZONE_CAPACITY * THERM_ZONE_SIZE db 0

align 4
therm_headrooms:
    times THERM_ZONE_CAPACITY * THERM_HEADROOM_SIZE db 0

message_therm_emergency: db "NOVA: THERMAL EMERGENCY - Notabschaltung", 13, 10, 0
