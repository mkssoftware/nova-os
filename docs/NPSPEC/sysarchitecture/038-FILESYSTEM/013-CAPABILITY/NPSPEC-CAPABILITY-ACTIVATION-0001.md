# NPSPEC-CAPABILITY-ACTIVATION-0001 – Nova Capability Activation

## Status

Angenommen

## Kategorie

Capability / Activation

## Zweck

NovaOS definiert die kontrollierte Aktivierung einer registrierten Capability-Implementierung.

Aktivierung überführt eine geeignete Implementierung in einen ausführbaren Zustand. Sie ist von Registrierung, Discovery, Resolution, Permission und eigentlicher Capability-Ausführung getrennt.

## Grundprinzipien

```text
Registered ≠ Active
Available ≠ Active
Activation ≠ Execution
Activation ≠ Authority
Activation ≠ Permission
Activation ≠ Provider Selection
```

## Modell

Eine Aktivierung bezieht sich auf eine konkrete Implementierung:

```text
CapabilityActivation
├── CapabilityID
├── ProviderID
├── ImplementationID
├── ActivationMode
├── ExecutionContext
├── State
└── Lifetime
```

## Aktivierungsmodi

NovaOS unterstützt mindestens:

```text
Boot
OnDemand
Event
Dependency
Preload
Manual
```

`OnDemand` ist der bevorzugte Modus für Capabilities, die nicht dauerhaft aktiv sein müssen.

## Ablauf

```text
Resolved Implementation
        ↓
Compatibility Check
        ↓
Trust + Policy Check
        ↓
Dependency Resolution
        ↓
Resource Allocation
        ↓
Isolation Setup
        ↓
Initialize
        ↓
Ready
```

Erst im Zustand `Ready` darf die Implementierung neue Ausführungsanforderungen annehmen.

## Zustände

Mindestens:

```text
Inactive
Activating
Ready
Busy
Suspended
Deactivating
Failed
Unavailable
```

Zustandsübergänge müssen kontrolliert und introspektierbar sein.

## Aktivierungskontext

Abhängig von Implementierung und Isolation kann eine Capability aktiviert werden als:

```text
In-Process Component
Isolated Process
Service
Driver Domain
Sandbox
Remote Provider
```

Die Aktivierungsform darf den öffentlichen Capability-Vertrag nicht verändern.

## Ressourcen

Vor Aktivierung müssen notwendige Ressourcen verfügbar sein.

Dazu können gehören:

```text
Memory
CPU
GPU
Device
Runtime
Libraries
IPC
Temporary Storage
```

Ressourcen werden gemäß Resource Economy und geltender Policy zugeteilt.

## Authority

Aktivierung erzeugt keine Nutzungsberechtigung.

```text
Active Implementation
        ≠
Authorized Caller
```

Eine dauerhaft aktive Implementierung darf daraus keine dauerhafte Authority für spätere Aufrufe ableiten.

Aufrufbezogene Authority wird separat für die jeweilige Ausführung bereitgestellt.

## Deaktivierung

Eine Implementierung darf deaktiviert werden bei:

```text
Idle
Resource Pressure
Update
Provider Replacement
Trust Change
Policy Change
Failure
Shutdown
```

Dabei müssen laufende Ausführungen gemäß ihrer Lifecycle- und Cancellation-Regeln behandelt werden.

## Fehler

Schlägt die Aktivierung fehl, darf NovaOS eine alternative kompatible Implementierung auswählen.

```text
Implementation A → Activation Failed
        ↓
Implementation B → Activate
```

Hard Requirements, Trust und Policy dürfen durch einen Fallback nicht umgangen werden.

## Live Evolution

Bei unterstützten Implementierungen darf NovaOS eine aktive Implementierung kontrolliert ersetzen:

```text
Load New
   ↓
Validate
   ↓
Prepare
   ↓
Transfer State
   ↓
Switch
   ↓
Verify
   ↓
Retire Old
```

## Normative Anforderungen

1. Nur registrierte und gültige Implementierungen DÜRFEN aktiviert werden.
2. Aktivierung und Capability-Ausführung MÜSSEN getrennt bleiben.
3. Aktivierung DARF keine Permission oder Authority erzeugen.
4. Kompatibilität, Trust und Policy MÜSSEN vor Aktivierung berücksichtigt werden.
5. Abhängigkeiten MÜSSEN vor oder während der Aktivierung kontrolliert aufgelöst werden.
6. Aktivierung MUSS unterschiedliche Isolations- und Ausführungsformen unterstützen können.
7. Ressourcen MÜSSEN vor beziehungsweise während der Aktivierung kontrolliert zugeteilt werden.
8. `OnDemand`-Aktivierung MUSS unterstützt werden.
9. Fehlgeschlagene Aktivierungen MÜSSEN kontrolliert behandelt werden.
10. Alternative Implementierungen DÜRFEN als Fallback aktiviert werden.
11. Fallbacks DÜRFEN Hard Requirements oder Sicherheitsrichtlinien nicht umgehen.
12. Deaktivierung und Suspendierung MÜSSEN unterstützt werden können.
13. Live Replacement SOLL bei geeigneten Implementierungen unterstützt werden.
14. Aktivierungszustand, Implementierung und Fehlergrund MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-REGISTRATION-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-IMPLEMENTATION-0001`
- `NPSPEC-CAPABILITY-DEPENDENCY-0001`
- `NPSPEC-CAPABILITY-COMPATIBILITY-0001`
- `NPSPEC-CAPABILITY-TRUST-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`

## Ergebnis

NovaOS besitzt einen kontrollierten Lifecycle für die Aktivierung von Capability-Implementierungen. Registrierte Implementierungen können bedarfsgerecht vorbereitet, isoliert, aktiviert, suspendiert, ersetzt und deaktiviert werden, ohne Aktivierung mit Authority, Permission oder eigentlicher Capability-Ausführung zu vermischen.