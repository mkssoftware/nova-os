# NPSPEC-POWER-LOWPOWERIDLE-0001 – Nova Low Power Idle

## Status

Angenommen

## Kategorie

Power / Low Power Idle

## Zweck

NovaOS definiert Low Power Idle als systemweiten, stark energieoptimierten Leerlaufzustand, bei dem das System logisch aktiv bleibt, während möglichst viele nicht benötigte Komponenten in tiefe Energiesparzustände wechseln.

Low Power Idle ermöglicht sehr geringe Leistungsaufnahme bei schneller Reaktionsfähigkeit, ohne einen klassischen vollständigen Suspend vorauszusetzen.

## Grundprinzipien

```text
Low Power Idle ≠ Suspend
Low Power Idle ≠ Hibernate
Low Power Idle ≠ CPU Idle
Low Power Idle ≠ Display Off
Logical Active ≠ Hardware Fully Active
Wake Event ≠ Full Boot
Low Power ≠ Lost Connectivity
```

## Modell

```text
LowPowerIdleContext
├── State
├── EntryReason
├── ActiveConstraints[]
├── WakeSources[]
├── RequiredServices[]
├── ConnectivityMode
├── PowerDomains[]
└── Residency
```

## Architektur

```text
System Idle Detection
        ↓
Low Power Idle Manager
        ↓
Constraint Evaluation
        ↓
Scheduler / Devices / Network
        ↓
Runtime Power Management
        ↓
Platform / HAL
        ↓
Low-Power Hardware State
```

## Eintritt

NovaOS darf Low Power Idle aktivieren, wenn keine Arbeit eine vollständig aktive Plattform benötigt.

```text
System Activity
      ↓
Idle Evaluation
      ↓
Constraint Check
      ↓
Quiesce Optional Work
      ↓
Reduce Active Components
      ↓
Low Power Idle
```

Der Übergang darf wesentlich leichter sein als ein vollständiger Suspend.

## Systemaktivität

Während Low Power Idle dürfen ausgewählte Funktionen weiterhin aktiv bleiben:

```text
Timers
Network Wake
Notifications
Maintenance
Audio
Sensors
Critical Services
Registered Background Work
```

Nur explizit zulässige Aktivitäten dürfen den tiefen Leerlauf regelmäßig verlassen.

## Connectivity

NovaOS unterstützt unterschiedliche Connectivity-Profile:

```text
Disconnected
Wake-Capable
Limited Connectivity
Connected Low Power
```

Die konkrete Verfügbarkeit hängt von Hardware, Policy und Execution Contracts ab.

## Komponenten

Während Low Power Idle können Komponenten unabhängig reduziert werden:

```text
CPU Cores
GPU
Accelerators
Storage
Network
Display
USB
Sensors
Peripheral Devices
```

Runtime Power Management übernimmt die konkrete Gerätesteuerung.

## Wake

Low Power Idle muss schnelle Wake-Pfade unterstützen.

Mögliche Wake Sources:

```text
Keyboard
Pointer
Touch
Power Button
Timer
Network
Notification
Device Event
Sensor Event
```

Nicht jedes Wake Event muss das gesamte System vollständig aktivieren.

NovaOS darf für Hintergrundereignisse nur die erforderlichen Komponenten temporär aktivieren.

## Background Work

Hintergrundarbeit muss kontrolliert werden:

```text
Wake
 ↓
Bounded Work Window
 ↓
Execute Allowed Work
 ↓
Quiesce
 ↓
Return to Low Power Idle
```

Dadurch wird verhindert, dass Hintergrundaktivität das System unbeabsichtigt dauerhaft aktiv hält.

## Platform Integration

Low Power Idle darf über unterschiedliche Plattformmechanismen realisiert werden:

```text
ACPI Low Power Idle
SoC Idle States
Modern Standby
Platform-Specific Idle
Firmware-Coordinated Idle
```

Das NovaOS-Modell bleibt davon unabhängig.

## Fallback

Unterstützt eine Plattform keinen systemweiten Low-Power-Idle-Modus, darf NovaOS vorhandene Mechanismen kombinieren:

```text
CPU Idle
+
Runtime Device Power Management
+
Display Power Management
+
Network Power Management
```

Low Power Idle ist daher keine Voraussetzung für grundlegendes Power Management.

## Normative Anforderungen

1. NovaOS MUSS Low Power Idle von Suspend und Hibernate trennen.
2. Low Power Idle MUSS einen logisch aktiven Systemzustand ermöglichen.
3. Nicht benötigte Komponenten SOLLEN während Low Power Idle tiefe Energiesparzustände erreichen.
4. Zulässige Hintergrundaktivitäten MÜSSEN explizit kontrollierbar sein.
5. Connectivity MUSS unabhängig vom allgemeinen Low-Power-Idle-Zustand modellierbar sein.
6. Wake Sources MÜSSEN explizit verwaltet werden.
7. Ein Wake Event DARF nur die tatsächlich benötigten Komponenten aktivieren.
8. Hintergrundarbeit SOLL zeitlich und ressourcenbezogen begrenzbar sein.
9. Runtime Power Management MUSS mit Low Power Idle koordiniert werden.
10. NovaOS DARF nicht von einem bestimmten ACPI- oder Plattformmechanismus abhängig sein.
11. Fehlende Low-Power-Idle-Unterstützung MUSS einen sicheren Fallback auf andere Power-Mechanismen ermöglichen.
12. Zustand, Residency, Wake-Gründe, aktive Constraints und Hintergrundaktivität MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-PLATFORM-0001`
- `NPSPEC-POWER-ACPI-0001`
- `NPSPEC-POWER-CPUIDLE-0001`
- `NPSPEC-POWER-DEVICE-0001`
- `NPSPEC-POWER-RUNTIME-0001`
- `NPSPEC-POWER-DISPLAY-0001`
- `NPSPEC-POWER-NETWORK-0001`
- `NPSPEC-POWER-SUSPEND-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`

## Ergebnis

NovaOS kann das logisch aktive System in einen stark energieoptimierten Low-Power-Idle-Zustand versetzen, in dem nur tatsächlich benötigte Funktionen aktiv bleiben. Geräte, CPU, Netzwerk und Hintergrundarbeit werden koordiniert reduziert, während schnelle Wake-Pfade und kontrollierte Hintergrundaktivität erhalten bleiben.