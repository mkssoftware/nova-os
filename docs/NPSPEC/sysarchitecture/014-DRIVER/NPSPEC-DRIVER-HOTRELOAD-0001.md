# NPSPEC-DRIVER-HOTRELOAD-0001 – Nova Driver Hot Reload

## Status

Entwurf

## Kategorie

Driver / Hot Reload / Live Evolution / Resilience

## Zweck

NovaOS definiert das kontrollierte Ersetzen eines laufenden Treibers ohne vollständigen Systemneustart.

```text
Old Driver
    ↓
Quiesce
    ↓
State Transfer
    ↓
New Driver
    ↓
Verify
    ↓
Activate
```

Hot Reload soll Treiberupdates, Fehlerbehebung und Recovery ermöglichen, während das betroffene Gerät möglichst verfügbar bleibt.

## Grundprinzip

```text
Hot Reload ≠ Forced Replacement
Loaded ≠ Active
Activated ≠ Verified
State Transfer ≠ Memory Copy
```

Ein neuer Treiber darf erst übernehmen, nachdem Kompatibilität, Ressourcen und Zustand ausreichend validiert wurden.

## Voraussetzungen

Ein Treiber muss Hot Reload explizit unterstützen.

```text
Driver
├── HotReload Supported
├── State Schema
├── Compatibility Information
└── Quiesce Support
```

Treiber ohne sicheren Übergabemechanismus dürfen einen Neustart oder Device Reset verlangen.

## Reload Lifecycle

```text
Prepare
   ↓
Validate New Driver
   ↓
Quiesce Old Driver
   ↓
Drain / Cancel I/O
   ↓
Export State
   ↓
Initialize New Driver
   ↓
Import State
   ↓
Switch
   ↓
Verify
   ↓
Retire Old Driver
```

Jede Phase muss einen eindeutig bestimmbaren Zustand besitzen.

## Quiesce

Vor dem Austausch muss der alte Treiber neue Arbeit stoppen können.

```text
Active
  ↓
Quiescing
  ↓
Quiesced
```

Laufende I/O-Requests müssen abhängig von ihrer Semantik:

```text
Complete
Cancel
Transfer
```

werden.

## Zustandsübertragung

Nur explizit definierter Zustand darf übertragen werden.

Beispiele:

```text
Device Configuration
Queue State
Logical Connections
Power State
Protocol State
```

Interne Pointer, Kernel-Adressen oder implementationsabhängige Speicherstrukturen dürfen nicht als portables State-Format vorausgesetzt werden.

## State Schema

Übertragbarer Zustand muss versioniert sein.

```text
State
├── Schema ID
├── Schema Version
└── Data
```

Der neue Treiber muss prüfen, ob er die bereitgestellte State-Version übernehmen kann.

## Ressourcen

Hardware-Ressourcen müssen während des Übergangs kontrolliert verwaltet werden.

```text
Old Driver
    ↓
Resource Handover
    ↓
New Driver
```

Capabilities, DMA-Mappings, Interrupts und Gerätezugriffe dürfen nicht unkontrolliert gleichzeitig beiden Treibern gehören.

## I/O

Während des Switches muss eindeutig definiert sein, welcher Treiber für einen Request verantwortlich ist.

```text
Old Requests → Old Driver / Transfer
New Requests → New Driver
```

Ein Request darf nicht versehentlich doppelt ausgeführt werden.

## Verification

Nach Aktivierung muss der neue Treiber überprüft werden können.

```text
New Driver
   ↓
Health Check
   ↓
Device Verification
   ↓
Healthy
```

`Started` oder `Loaded` darf nicht als `Healthy` interpretiert werden.

## Rollback

Der alte Treiber soll bis zum erfolgreichen Abschluss des Reloads als Fallback verfügbar bleiben, sofern dies technisch sicher möglich ist.

```text
New Driver Failed
      ↓
Rollback
      ↓
Old Driver
```

Ist Rollback nicht sicher möglich, muss NovaOS auf Device Reset, Fallback Driver oder kontrollierte Deaktivierung wechseln können.

## Sicherheit

Der neue Treiber muss vor Aktivierung dieselben Trust-, Signature-, Capability- und Sandbox-Prüfungen wie bei einem normalen Ladevorgang durchlaufen.

Hot Reload darf keine Sicherheitsprüfung umgehen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
DriverID
Old Version
New Version
Reload Phase
State Schema
Pending I/O
Verification State
Rollback State
Error
```

## Normative Anforderungen

1. NovaOS SOLL kontrolliertes Hot Reload von Treibern unterstützen.
2. Ein Treiber MUSS Hot Reload explizit unterstützen, bevor ein zustandserhaltender Austausch durchgeführt wird.
3. Der alte Treiber MUSS vor dem Switch kontrolliert quiesziert werden.
4. Laufende I/O-Requests MÜSSEN eindeutig abgeschlossen, abgebrochen oder übertragen werden.
5. Übertragbarer Treiberzustand MUSS explizit definiert und versioniert sein.
6. Implementationsabhängige Pointer oder Kernel-Adressen DÜRFEN NICHT als portables State-Format vorausgesetzt werden.
7. Ressourcen und Capabilities MÜSSEN kontrolliert auf den neuen Treiber übertragen werden.
8. Ein I/O-Request DARF NICHT durch den Reload doppelt ausgeführt werden.
9. Der neue Treiber MUSS vor Aktivierung die normalen Security- und Trust-Prüfungen durchlaufen.
10. `Loaded` oder `Started` DARF NICHT automatisch als `Healthy` gelten.
11. Ein fehlgeschlagener Reload MUSS Rollback, Reset, Fallback oder kontrollierte Deaktivierung ermöglichen.
12. Reload-, Verification- und Rollback-Zustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DRIVER-MODEL-0001`
- `NPSPEC-DRIVER-FRAMEWORK-0001`
- `NPSPEC-DRIVER-ISOLATION-0001`
- `NPSPEC-DRIVER-CAPABILITY-0001`
- `NPSPEC-IO-REQUEST-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `ADR-DRIVER-0008`

## Ergebnis

```text
Old Driver
    ↓
Quiesce + State Export
    ↓
Controlled Switch
    ↓
New Driver
    ↓
Verify
   ├→ Healthy → Retire Old
   └→ Failed  → Rollback / Reset
```

NovaOS erhält damit einen kontrollierten Hot-Reload-Mechanismus, der Treiber zur Laufzeit ersetzen kann, ohne I/O-Zustand, Ressourcenbesitz oder Systemsicherheit unkontrolliert zu verändern.