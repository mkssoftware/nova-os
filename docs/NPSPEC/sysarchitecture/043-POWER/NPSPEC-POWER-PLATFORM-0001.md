# NPSPEC-POWER-PLATFORM-0001 – Nova Platform Power Management

## Status

Angenommen

## Kategorie

Power / Platform

## Zweck

NovaOS definiert eine einheitliche Abstraktion für plattformspezifische Energieverwaltungsmechanismen.

Firmware-, Chipsatz- und Hardwarefunktionen werden über HAL und geeignete Treiber abstrahiert, sodass die höhere Power Architecture unabhängig von ACPI, Firmwaretyp, CPU-Architektur oder konkreter Hardware arbeiten kann.

## Grundprinzipien

```text
Platform Mechanism ≠ Power Policy
Firmware Interface ≠ Nova Power Interface
Platform State ≠ Device State
Platform Capability ≠ Authority
Requested State ≠ Reached State
Unsupported ≠ Failed
```

## Architektur

```text
Nova Power Manager
        ↓
Platform Power Interface
        ↓
HAL / Platform Drivers
        ↓
Firmware / Hardware
```

Plattformspezifische Details dürfen nicht unnötig in höhere Power-Schichten gelangen.

## Platform Power Provider

Eine Plattform stellt ihre Energieverwaltungsfunktionen über einen registrierten Provider bereit:

```text
PlatformPowerProvider
├── ProviderID
├── PlatformID
├── SupportedStates[]
├── SupportedTransitions[]
├── PowerControls[]
├── WakeSources[]
├── Capabilities
└── State
```

Der Provider übersetzt NovaOS-Anforderungen in plattformspezifische Mechanismen.

## Plattformmechanismen

Abhängig von der Hardware können beispielsweise verwendet werden:

```text
ACPI
UEFI
Device Firmware
CPU Power Interfaces
SoC Power Controllers
Platform Management Controller
Architecture-Specific Interfaces
```

NovaOS darf nicht voraussetzen, dass eine bestimmte Plattformtechnologie vorhanden ist.

## Erkennung

Beim Systemstart werden verfügbare Power-Funktionen erkannt:

```text
Platform Discovery
      ↓
Firmware Inspection
      ↓
Capability Detection
      ↓
Validation
      ↓
Platform Power Registration
```

Nur tatsächlich erkannte und validierte Funktionen dürfen als verfügbar gemeldet werden.

## Systemzustände

Der Platform Provider bildet NovaOS-Systemzustände auf unterstützte Plattformzustände ab:

```text
Nova State
    ↓
Platform Mapping
    ↓
Hardware / Firmware State
```

Beispielsweise:

```text
Active
Idle
Low Power
Suspend
Hibernate
Shutdown
```

Nicht jede Plattform muss jeden Zustand unterstützen.

## Transitionen

Zustandswechsel müssen kontrolliert erfolgen:

```text
Request
  ↓
Validate
  ↓
Prepare
  ↓
Quiesce
  ↓
Platform Transition
  ↓
Resume / Verify
```

Fehler während einer Transition müssen erkannt und an die Power Architecture zurückgemeldet werden.

## Wake Sources

Plattformen dürfen unterschiedliche Aufweckquellen bereitstellen:

```text
Power Button
Keyboard
Mouse
Timer
Network
RTC
USB
Lid
Device Event
```

Wake Sources müssen explizit konfigurierbar und introspektierbar sein.

## Firmware und Hardwaregrenzen

Hardware- und Firmware-Sicherheitsgrenzen dürfen durch NovaOS nicht umgangen werden.

```text
Hardware Safety
      >
Firmware Constraints
      >
Nova Power Policy
```

Nicht unterstützte Funktionen müssen sauber als `Unsupported` behandelt werden.

## Fallback

Fehlen erweiterte Plattformfunktionen, muss NovaOS einen sicheren reduzierten Betriebsmodus unterstützen können.

```text
Full Platform Power Support
        ↓ unavailable
Basic Power Support
        ↓
Safe Operation
```

Fehlende Optimierungsfunktionen dürfen nicht automatisch den Systemstart verhindern.

## Introspection

NovaOS muss mindestens ermitteln können:

```text
Platform Provider
Supported States
Current Platform State
Supported Transitions
Wake Sources
Firmware Interface
Unavailable Features
Last Transition Result
```

## Normative Anforderungen

1. NovaOS MUSS plattformspezifische Power-Mechanismen abstrahieren.
2. Power Policy und Plattformmechanismus MÜSSEN getrennt bleiben.
3. NovaOS DARF keine bestimmte Firmware- oder Plattformtechnologie voraussetzen.
4. Unterstützte Power-Funktionen MÜSSEN zur Laufzeit erkannt werden können.
5. Nicht unterstützte Funktionen MÜSSEN explizit als solche behandelt werden.
6. Angeforderter und tatsächlich erreichter Power State MÜSSEN unterscheidbar sein.
7. Systemzustände MÜSSEN auf plattformspezifische Zustände abbildbar sein.
8. Power Transitions MÜSSEN kontrolliert und validiert erfolgen.
9. Wake Sources MÜSSEN explizit verwaltet werden können.
10. Hardware- und Firmware-Sicherheitsgrenzen DÜRFEN nicht umgangen werden.
11. Fehlende optionale Power-Funktionen SOLLEN einen sicheren reduzierten Betrieb ermöglichen.
12. Plattformfähigkeiten, Zustände und Transitionsergebnisse MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-SYSTEM-DRIVERS-0001`
- `NPSPEC-SYSTEM-BOOT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-THERMAL-ARCH-0001`

## Ergebnis

NovaOS besitzt eine hardwareunabhängige Platform-Power-Schicht, die unterschiedliche Firmware-, Architektur- und Hardwaremechanismen hinter einer einheitlichen Schnittstelle abstrahiert. Dadurch kann die zentrale Power Architecture dieselben Systemkonzepte auf sehr unterschiedlichen Plattformen verwenden, ohne an ACPI, UEFI oder eine bestimmte Hardwarearchitektur gebunden zu sein.