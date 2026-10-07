# NPSPEC-TIME-NAMESPACE-0001 – Nova Time Namespace

## Status

Angenommen

## Kategorie

Time / Namespace

## Zweck

NovaOS definiert Time Namespaces als isolierte Sichten auf bestimmte Clock Domains.

Damit können Prozesse, Programme, Solutions, Container, Compatibility Environments und virtuelle Laufzeitumgebungen eigene Zeitabbildungen erhalten, ohne die globale Systemzeit zu verändern.

## Grundprinzipien

```text
Time Namespace ≠ Clock Domain
Time Namespace ≠ Virtual Clock
Time Namespace ≠ Time Zone
Time Namespace ≠ Permission Boundary
Namespace Time ≠ Host Time
Visibility ≠ Authority
```

## Modell

```text
TimeNamespace
├── TimeNamespaceID
├── ParentNamespaceID
├── OwnerID
├── ClockMappings[]
├── Policy
└── State
```

Eine Clock-Zuordnung beschreibt:

```text
ClockMapping
├── SourceClockDomainID
├── ExposedClockDomainID
├── Offset
├── Rate
└── Flags
```

## Architektur

```text
System Clock Domains
        ↓
Time Namespace
        ↓
Clock Mapping
        ↓
Process / Program / Solution
        ↓
Visible Time
```

Die zugrunde liegende globale Clock Domain bleibt unverändert.

## Namespace-Sichten

Time Namespaces dürfen unter anderem verwendet werden für:

```text
Process
Program
Solution
Container
Compatibility Environment
Virtual Machine
Test Environment
Simulation
```

Mehrere Ausführungsumgebungen dürfen gleichzeitig unterschiedliche Zeitansichten besitzen.

## Mapping

Ein Namespace kann eine vorhandene Clock Domain direkt oder transformiert bereitstellen:

```text
Host Monotonic
      ↓
Offset / Mapping
      ↓
Namespace Monotonic
```

Mögliche Transformationen sind abhängig von der jeweiligen Clock Domain:

```text
Identity
Offset
Virtual Mapping
Controlled Rate
Pause
```

Nicht jede Transformation ist für jede Domain zulässig.

## Hierarchie

Time Namespaces dürfen hierarchisch aufgebaut sein:

```text
System
  ↓
Container
  ↓
Application
  ↓
Test Environment
```

Ein Child Namespace erbt ausschließlich die explizit definierten oder erlaubten Clock-Mappings seines Parents.

## Monotone Zeit

Eine als monoton deklarierte Namespace-Clock muss innerhalb dieses Namespaces weiterhin monoton bleiben.

```text
T2 >= T1
```

Änderungen von Offset oder Mapping dürfen diese Eigenschaft nicht verletzen.

## Wall Clock

Ein Namespace darf eine abweichende Wall-Clock-Sicht besitzen:

```text
System Wall Clock
      ↓
Namespace Offset
      ↓
Visible Wall Clock
```

Dies verändert weder RTC noch globale Wall Clock.

## Timer und Deadlines

Timer und Deadlines innerhalb eines Time Namespaces müssen gegen die dort sichtbare Clock Domain ausgewertet werden.

```text
Namespace Clock
      ↓
Deadline
      ↓
Timer
```

Eine Änderung des Namespace-Mappings muss bestehende Timer gemäß ihrer Domain-Semantik konsistent behandeln.

## Sicherheit

Das Lesen einer Namespace-Zeit erzeugt keine Authority.

Änderungen an:

```text
Offset
Rate
Mapping
Parent
Policy
```

benötigen explizite Berechtigung.

Ein Prozess darf seine Zeitansicht nicht automatisch verändern, nur weil er Teil des Namespaces ist.

## Lebenszyklus

```text
Create
  ↓
Configure
  ↓
Activate
  ↓
Use
  ↓
Update / Freeze
  ↓
Destroy
```

Beim Entfernen eines Namespaces müssen abhängige Timer, Prozesse und Clock-Mappings kontrolliert behandelt werden.

## Normative Anforderungen

1. NovaOS MUSS isolierte Time Namespaces unterstützen können.
2. Time Namespace und Clock Domain MÜSSEN getrennte Konzepte bleiben.
3. Time Namespaces DÜRFEN globale Clock Domains nicht implizit verändern.
4. Mehrere Time Namespaces MÜSSEN gleichzeitig existieren können.
5. Clock Domains MÜSSEN pro Namespace explizit abbildbar sein.
6. Time Namespaces MÜSSEN hierarchisch organisierbar sein können.
7. Monotone Namespace-Clocks DÜRFEN nicht rückwärts laufen.
8. Wall-Clock-Offsets MÜSSEN namespace-lokal möglich sein.
9. Timer und Deadlines MÜSSEN die Clock-Sicht ihres Namespaces berücksichtigen.
10. Änderungen an Clock-Mappings MÜSSEN explizite Authority benötigen.
11. Namespace-Zugehörigkeit DARF keine zusätzliche Authority erzeugen.
12. TimeNamespaceID, Parent, Clock-Mappings, Transformationen und Zustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-MONOTONIC-0001`
- `NPSPEC-TIME-CLOCKDOMAIN-0001`
- `NPSPEC-TIME-TIMER-0001`
- `NPSPEC-TIME-DEADLINE-0001`
- `NPSPEC-TIME-VIRTUAL-0001`
- `NPSPEC-NAMESPACE-PROCESS-0001`
- `NPSPEC-NAMESPACE-APPLICATION-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann unterschiedlichen Ausführungsumgebungen isolierte Zeitansichten bereitstellen. Prozesse, Programme, Solutions und virtuelle Umgebungen können eigene Clock-Mappings besitzen, während globale Zeitdomänen unverändert bleiben und monotone Eigenschaften, Timer und Deadlines konsistent erhalten werden.