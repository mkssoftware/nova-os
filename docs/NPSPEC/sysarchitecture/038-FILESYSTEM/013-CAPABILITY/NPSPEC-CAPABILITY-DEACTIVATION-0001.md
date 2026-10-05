# NPSPEC-CAPABILITY-DEACTIVATION-0001 – Nova Capability Deactivation

## Status

Angenommen

## Kategorie

Capability / Deactivation

## Zweck

NovaOS definiert die kontrollierte Deaktivierung aktiver Capability-Implementierungen.

Deaktivierung beendet die aktive Bereitstellung einer Implementierung, gibt zugehörige Ressourcen frei und stellt sicher, dass laufende Ausführungen, temporäre Authority und interner Zustand kontrolliert behandelt werden.

## Grundprinzipien

```text
Deactivation ≠ Unregistration
Deactivation ≠ Uninstall
Deactivation ≠ Revocation
Inactive ≠ Unavailable
Stop Accepting Calls ≠ Abort Running Calls
Deactivation ≠ Removal of Capability Identity
```

## Modell

```text
CapabilityDeactivation
├── ImplementationID
├── Reason
├── Mode
├── Deadline
├── StatePolicy
└── ResourcePolicy
```

## Gründe

Eine Deaktivierung kann ausgelöst werden durch:

```text
Idle
Resource Pressure
Update
Live Replacement
Policy Change
Trust Change
Failure
Administrative Action
Shutdown
```

## Modi

NovaOS unterstützt mindestens:

```text
Graceful
Immediate
Suspend
Replace
Failure
```

`Graceful` ist zu bevorzugen, wenn keine Sicherheits- oder Systemanforderung eine sofortige Beendigung verlangt.

## Ablauf

```text
Ready / Busy
     ↓
Deactivation Requested
     ↓
Stop New Executions
     ↓
Handle Running Executions
     ↓
Flush / Transfer State
     ↓
Release Temporary Authority
     ↓
Release Resources
     ↓
Deactivate
     ↓
Inactive
```

## Laufende Ausführungen

Je nach Deaktivierungsmodus können laufende Aufrufe:

```text
Complete
Cancel
Migrate
Transfer
Fail
```

NovaOS muss dabei Cancellation, Deadline, Transaktionen und Structured Concurrency berücksichtigen.

Eine Deaktivierung darf keine teilweise ausgeführten Änderungen unkontrolliert zurücklassen.

## Zustand

Persistenter oder übertragbarer Zustand muss gemäß Capability- und Implementation-Vertrag behandelt werden.

Bei Live Replacement darf geeigneter Zustand an die neue Implementierung übertragen werden:

```text
Old Implementation
      ↓
Validated State Transfer
      ↓
New Implementation
```

Provider-interner Zustand darf nicht ungeprüft zwischen inkompatiblen Implementierungen übertragen werden.

## Ressourcen

Nach Abschluss der Deaktivierung müssen nicht mehr benötigte Ressourcen freigegeben werden können:

```text
Memory
Buffers
IPC Channels
Device Access
Temporary Storage
Resource Reservations
Runtime Resources
```

## Authority

Temporäre, ausführungsgebundene Authority muss nach Ende der zugehörigen Ausführung beziehungsweise Deaktivierung ungültig werden.

Persistente Permissions werden durch die Deaktivierung nicht automatisch gelöscht.

```text
Implementation Inactive
        ≠
Permission Revoked
```

## Registrierung

Eine deaktivierte Implementierung darf weiterhin registriert bleiben:

```text
Registered
   +
Inactive
```

Sie kann später erneut aktiviert werden, sofern Trust, Policy, Kompatibilität und Abhängigkeiten dies zulassen.

## Fehler

Kann eine Implementierung nicht sauber deaktiviert werden, muss NovaOS kontrolliert eskalieren können:

```text
Graceful
   ↓ failure
Forced Cancellation
   ↓ failure
Isolation Termination
   ↓
Cleanup / Recovery
```

Sicherheits- und Integritätsanforderungen haben Vorrang vor einer vollständigen Zustandsbewahrung.

## Normative Anforderungen

1. Aktive Capability-Implementierungen MÜSSEN kontrolliert deaktivierbar sein.
2. Deaktivierung und Unregistration MÜSSEN getrennt bleiben.
3. Neue Ausführungen MÜSSEN während der Deaktivierung blockierbar sein.
4. Laufende Ausführungen MÜSSEN kontrolliert abgeschlossen, abgebrochen oder übertragen werden können.
5. Cancellation und Deadlines MÜSSEN berücksichtigt werden.
6. Temporäre Authority MUSS kontrolliert freigegeben beziehungsweise ungültig werden.
7. Nicht mehr benötigte Ressourcen MÜSSEN freigegeben werden.
8. Persistente Permissions DÜRFEN durch Deaktivierung nicht automatisch gelöscht werden.
9. Zustandsübertragung MUSS auf kompatible und validierte Implementierungen beschränkt sein.
10. Graceful Deactivation SOLL bevorzugt werden.
11. Sicherheitskritische Situationen MÜSSEN eine sofortige Deaktivierung erlauben.
12. Eine deaktivierte Implementierung DARF registriert bleiben.
13. Reaktivierung MUSS eine erneute Prüfung relevanter Bedingungen erlauben.
14. Deaktivierungszustand, Grund und Ergebnis MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-ACTIVATION-0001`
- `NPSPEC-CAPABILITY-IMPLEMENTATION-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-CAPABILITY-TRUST-0001`
- `NPSPEC-CAPABILITY-REGISTRATION-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`

## Ergebnis

NovaOS kann Capability-Implementierungen sicher aus dem aktiven Betrieb nehmen, laufende Ausführungen kontrolliert behandeln sowie Authority, Zustand und Ressourcen sauber verwalten. Die Implementierung kann dabei registriert bleiben und später erneut aktiviert werden.