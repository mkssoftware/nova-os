# NPSPEC-POWER-STORAGE-0001 – Nova Storage Power Management

## Status

Angenommen

## Kategorie

Power / Storage

## Zweck

NovaOS definiert die energieeffiziente Steuerung physischer Speichergeräte.

Storage Power Management reduziert den Energieverbrauch inaktiver Speicherhardware, ohne Dateisystem-, Volume-, Objekt- oder Datenintegrität zu gefährden.

## Grundprinzipien

```text
Storage Power State ≠ Filesystem State
Device Idle ≠ Volume Offline
Device Off ≠ Device Removed
Logical Availability ≠ Physical Activity
Power Saving ≠ Data Loss
Requested State ≠ Effective State
```

## Architektur

```text
Filesystem / VFS
       ↓
Storage I/O
       ↓
Runtime Power Manager
       ↓
Storage Power Manager
       ↓
Storage Driver / HAL
       ↓
Physical Storage
```

Die höheren Storage-Schichten bleiben von konkreten Hardware-Energiezuständen abstrahiert.

## Modell

```text
StoragePowerContext
├── DeviceID
├── DeviceType
├── SupportedStates[]
├── CurrentState
├── ActivityState
├── TransitionLatency
├── WakeLatency
├── PendingIO
├── Constraints
└── Policy
```

## Unterstützte Geräte

Das Modell gilt unter anderem für:

```text
NVMe
SATA
SAS
USB Storage
eMMC
UFS
SD
Virtual Storage
Registered Storage Providers
```

Gerätespezifische Mechanismen bleiben hinter dem jeweiligen Provider verborgen.

## Energiezustände

NovaOS verwendet abstrakte Zustände:

```text
Active
Idle
LowPower
Standby
Sleep
Off
Unavailable
```

Nicht jedes Gerät muss alle Zustände unterstützen.

## Zustandsauswahl

```text
I/O Activity
    +
Expected Idle Duration
    +
Wake Latency
    +
Power Policy
    +
Execution Contracts
    +
Hardware Limits
      ↓
Storage Power State
```

Der tiefste verfügbare Zustand ist nicht automatisch der optimale Zustand.

## I/O-Koordination

Vor einem inkompatiblen Zustandswechsel müssen relevante Operationen berücksichtigt werden:

```text
Pending Writes
Transactions
Flush Requirements
Device Commands
Recovery Operations
```

Erforderliche Daten müssen vor dem Übergang konsistent an das Gerät übergeben werden.

Ein Power-State-Wechsel darf nicht als Ersatz für Filesystem- oder Storage-Transaktionen verwendet werden.

## Runtime Power Management

Inaktive Speichergeräte dürfen automatisch energiesparende Zustände erreichen.

Neue I/O-Anforderungen lösen bei Bedarf eine kontrollierte Reaktivierung aus:

```text
I/O Request
    ↓
Device Resume
    ↓
Verify State
    ↓
Execute I/O
```

## Latenz

Tiefe Energiesparzustände können erhebliche Wake-Latenzen verursachen.

NovaOS muss daher Anforderungen wie:

```text
Interactive I/O
Realtime I/O
Deadline
Background Work
Expected Idle Duration
```

berücksichtigen können.

## Geräteabhängige Mechanismen

Provider dürfen beispielsweise verwenden:

```text
NVMe Power States
NVMe APST
SATA Link Power Management
SATA Standby
Device Sleep
USB Runtime Power Management
Platform-Specific Storage Power States
```

Diese Mechanismen dürfen höheren Schichten nicht als allgemeines NovaOS-Modell aufgezwungen werden.

## Fehlerverhalten

Schlägt Suspend oder Resume fehl:

```text
Failure
  ↓
Verify Device State
  ↓
Safe Supported State
  ↓
Report / Degrade
```

Ein angeforderter Zustand darf nicht ungeprüft als erreicht gelten.

Datenintegrität besitzt Vorrang vor Energieeinsparung.

## Normative Anforderungen

1. NovaOS MUSS Storage Power Management hardwareunabhängig abstrahieren.
2. Storage Power State und Filesystem State MÜSSEN getrennte Konzepte bleiben.
3. Inaktive Speichergeräte MÜSSEN energiesparende Zustände verwenden können.
4. Pending I/O und Schreiboperationen MÜSSEN vor inkompatiblen Transitionen berücksichtigt werden.
5. Storage-Transaktionen DÜRFEN durch Power Management nicht verletzt werden.
6. Wake-Latenz MUSS bei der Zustandsauswahl berücksichtigt werden können.
7. Runtime Resume MUSS vor erforderlichem I/O abgeschlossen und verifiziert sein.
8. Gerätespezifische Power-Mechanismen MÜSSEN hinter Providern abstrahiert bleiben.
9. Device Off, Device Suspend und Device Removal MÜSSEN getrennt behandelt werden.
10. Angeforderter und effektiver Power State MÜSSEN unterscheidbar sein.
11. Datenintegrität MUSS Vorrang vor Energieoptimierung besitzen.
12. DeviceID, Aktivität, Power State, Transitionen, Wake-Latenz und Constraints MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-PLATFORM-0001`
- `NPSPEC-POWER-DEVICE-0001`
- `NPSPEC-POWER-RUNTIME-0001`
- `NPSPEC-STORAGE-DEVICE-0001`
- `NPSPEC-STORAGE-VOLUME-0001`
- `NPSPEC-FILESYSTEM-TRANSACTION-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`

## Ergebnis

NovaOS kann physische Speichergeräte abhängig von Aktivität, Latenzanforderungen und Energiepolitik dynamisch in geeignete Energiesparzustände versetzen. Dateisysteme, Volumes und Objekte bleiben davon logisch unabhängig, während laufende I/O-Operationen, Transaktionen und Datenintegrität jederzeit geschützt werden.