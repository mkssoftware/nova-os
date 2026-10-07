# NPSPEC-POWER-ACPI-0001 – Nova ACPI Power Integration

## Status

Angenommen

## Kategorie

Power / ACPI

## Zweck

NovaOS definiert die Integration von ACPI als plattformspezifischen Provider für Energieverwaltung, Hardwarebeschreibung und Power Events.

ACPI wird hinter der Nova Platform Power Abstraction gekapselt und ist weder Voraussetzung für die allgemeine Power Architecture noch deren öffentliches Systemmodell.

## Grundprinzipien

```text
ACPI ≠ Nova Power Architecture
ACPI ≠ Power Policy
ACPI State ≠ Nova Power State
ACPI Object ≠ Nova Object
Firmware Data ≠ Trusted Data
ACPI Availability ≠ Platform Requirement
```

## Architektur

```text
Nova Power Manager
        ↓
Platform Power Interface
        ↓
ACPI Provider
├── Table Manager
├── Namespace
├── AML Interpreter
├── Event Handler
└── Power State Adapter
        ↓
Firmware / Hardware
```

Andere Plattformprovider dürfen parallel oder alternativ zu ACPI existieren.

## Initialisierung

```text
ACPI Discovery
     ↓
RSDP
     ↓
RSDT / XSDT
     ↓
Table Validation
     ↓
Namespace Construction
     ↓
AML Initialization
     ↓
Power Capability Registration
```

Fehlende oder ungültige ACPI-Daten dürfen nicht ungeprüft verwendet werden.

## ACPI-Tabellen

NovaOS muss relevante ACPI-Tabellen erkennen und validieren können.

Dazu können insbesondere gehören:

```text
RSDP
RSDT
XSDT
FADT
DSDT
SSDT
MADT
MCFG
HPET
SRAT
SLIT
```

Welche Tabellen tatsächlich verwendet werden, hängt von Plattform und unterstützten Systemfunktionen ab.

## AML

AML wird ausschließlich über einen kontrollierten Interpreter ausgeführt.

```text
AML
 ↓
Validation
 ↓
Interpreter
 ↓
Controlled Hardware / Platform Access
```

Firmwarecode darf keinen uneingeschränkten Zugriff auf NovaOS-Speicher oder andere Systemressourcen erhalten.

Fehlerhafte AML-Ausführung muss isolierbar und diagnostizierbar sein.

## Power States

Der ACPI Provider darf ACPI-Zustände auf NovaOS-Power-Zustände abbilden.

```text
Nova Power State
       ↕
ACPI Mapping
       ↕
Platform State
```

Dabei können unter anderem berücksichtigt werden:

```text
S-States
C-States
P-States
Device Power States
```

NovaOS bleibt gegenüber den konkreten ACPI-Zuständen abstrahiert.

## Events

ACPI-Ereignisse können beispielsweise umfassen:

```text
Power Button
Sleep Button
Lid
Battery
AC Adapter
Thermal Event
Device Event
Wake Event
```

Events werden zunächst als Plattformereignisse verarbeitet und anschließend an die zuständigen NovaOS-Subsysteme weitergeleitet.

## Power Transition

```text
Nova Power Request
       ↓
Platform Validation
       ↓
ACPI Preparation
       ↓
Device Coordination
       ↓
ACPI Transition
       ↓
Resume
       ↓
State Verification
```

Ein erfolgreicher Firmware-Aufruf darf nicht automatisch als erfolgreich abgeschlossene Systemtransition gelten.

## Fehlerbehandlung

ACPI kann fehlerhafte oder unvollständige Firmwareinformationen liefern.

NovaOS muss Zustände unterscheiden können:

```text
Available
Degraded
Unsupported
Invalid
Unavailable
Failed
```

Bei nicht kritischen ACPI-Problemen soll ein sicherer reduzierter Plattformbetrieb möglich bleiben.

## Sicherheit

ACPI-Tabellen und AML werden als Firmware-Eingaben behandelt und müssen validiert werden.

Firmwarebeschreibungen dürfen:

```text
keine Nova Authority erzeugen
keine Capability-Prüfung umgehen
keine Hardware-Sicherheitsgrenzen abschwächen
keine ungeprüften Kernelzugriffe erhalten
```

## Introspection

NovaOS muss mindestens introspektierbar machen:

```text
ACPI Availability
ACPI Revision
Loaded Tables
Table Validation State
Supported Power States
Wake Sources
AML State
Detected Firmware Errors
Last Power Transition
```

## Normative Anforderungen

1. ACPI MUSS als Platform-Power-Provider behandelt werden.
2. Die allgemeine Nova Power Architecture DARF nicht von ACPI abhängig sein.
3. ACPI-Tabellen MÜSSEN vor Verwendung validiert werden.
4. AML MUSS über einen kontrollierten Interpreter ausgeführt werden.
5. Firmwarecode DARF keinen uneingeschränkten Systemzugriff erhalten.
6. ACPI- und Nova-Power-Zustände MÜSSEN getrennt bleiben.
7. ACPI Events MÜSSEN in NovaOS-Systemereignisse übersetzbar sein.
8. Power Transitions MÜSSEN nach ihrer Ausführung verifiziert werden.
9. Fehlerhafte ACPI-Daten DÜRFEN nicht als gültige Plattforminformationen behandelt werden.
10. ACPI-Ausfälle SOLLEN einen sicheren reduzierten Betrieb ermöglichen, sofern die Hardware dies zulässt.
11. ACPI DARF das Capability- und Sicherheitsmodell nicht umgehen.
12. ACPI-Zustand, Tabellen, Fehler und unterstützte Funktionen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-PLATFORM-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-SYSTEM-DRIVERS-0001`
- `NPSPEC-THERMAL-ARCH-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS integriert ACPI als isolierten und validierten Plattformprovider, ohne die allgemeine Power Architecture an ACPI zu binden. ACPI-Tabellen, AML, Power States und Events werden in NovaOS-eigene Abstraktionen übersetzt, während Firmwarefehler, Sicherheitsgrenzen und alternative Plattformmechanismen berücksichtigt bleiben.