# ADR-TRANSACTION-0003 – Configuration Transactions

## Status

Angenommen

## Kategorie

Transaction Architecture / Configuration / System State

## Kontext

Konfigurationsänderungen können in NovaOS mehrere zusammenhängende Werte, Komponenten und Abhängigkeiten gleichzeitig betreffen.

Beispiele sind:

- Systemeinstellungen,
- Service-Konfigurationen,
- Netzwerkparameter,
- Ressourcenlimits,
- Capability-Konfigurationen,
- Security Policies,
- Treibereinstellungen,
- Benutzer- und Anwendungseinstellungen.

Werden solche Werte einzeln verändert, können temporär ungültige oder widersprüchliche Konfigurationen entstehen.

```text
Old Configuration
      ↓
Change A
      ↓
Invalid Intermediate State
      ↓
Change B
      ↓
New Configuration
```

NovaOS benötigt deshalb ein transaktionales Modell für zusammengehörige Konfigurationsänderungen.

## Entscheidung

NovaOS behandelt zusammengehörige Konfigurationsänderungen als **Configuration Transactions** auf Basis der systemweiten System Transactions.

```text
Current Configuration
        ↓
Configuration Change Set
        ↓
Validate
        ↓
Prepare
        ↓
Commit
        ↓
New Configuration
```

Alle Änderungen eines Change Sets werden logisch gemeinsam sichtbar oder die bisherige gültige Konfiguration bleibt erhalten.

## Grundprinzip

```text
Configuration changes are applied
as coherent state transitions.
```

Eine Komponente soll keine teilweise angewendete Konfiguration beobachten müssen.

## Configuration Change Set

Mehrere zusammengehörige Änderungen werden zu einem Change Set zusammengefasst.

Beispiel:

```text
Network Change Set
 ├── Address
 ├── Gateway
 ├── DNS
 └── Routing Policy
```

Das Change Set bildet die atomare semantische Einheit.

## Validierung

Vor Commit muss die resultierende Konfiguration validiert werden.

Dabei können unter anderem geprüft werden:

```text
Type Constraints
Semantic Constraints
Dependencies
Capabilities
Security Policies
Resource Limits
Trust Requirements
Data Sovereignty
Cross-Setting Invariants
```

Entscheidend ist die Gültigkeit der resultierenden Gesamtkonfiguration, nicht nur die Gültigkeit einzelner Werte.

## Prepare

Falls eine Konfigurationsänderung zusätzliche Ressourcen oder Komponenten benötigt, können diese vor Commit vorbereitet werden.

```text
Configuration Change
        ↓
Prepare Dependencies
        ↓
Validate
        ↓
Commit
```

Dadurch soll verhindert werden, dass eine offensichtlich nicht realisierbare Konfiguration aktiviert wird.

## Commit

Nach erfolgreicher Validierung wird die neue Konfiguration atomar als gültige Konfigurationsversion veröffentlicht.

```text
Configuration N
      ↓
Atomic Commit
      ↓
Configuration N+1
```

Consumer sehen entweder Version `N` oder Version `N+1`, nicht einen undefinierten Mischzustand.

## Anwendung der Konfiguration

Der atomare Commit bedeutet nicht zwingend, dass jeder physische Effekt gleichzeitig abgeschlossen ist.

```text
Configuration Commit
        ↓
Apply / Reconcile
        ↓
Observed Configuration State
```

Komponenten können den neuen Zustand anschließend kontrolliert übernehmen.

## Versionierung

Relevante Konfigurationen müssen versionierbar sein.

Eine Configuration Transaction basiert auf einer bekannten Ausgangsversion.

```text
Transaction
base = Configuration 18
```

Hat sich die Konfiguration zwischenzeitlich geändert, muss ein möglicher Konflikt erkannt werden.

## Concurrent Changes

Parallele Configuration Transactions dürfen sich nicht unbemerkt überschreiben.

```text
Transaction A → Version 18
Transaction B → Version 18

A commits → Version 19
B attempts commit
        ↓
Conflict
```

Je nach Semantik kann NovaOS:

```text
Reject
Rebase
Merge
Retry
```

verwenden.

Ein automatisches Merge ist nur zulässig, wenn die Änderungen semantisch kompatibel sind.

## Rollback

Kann eine neue Konfiguration nicht erfolgreich angewendet oder verifiziert werden, kann die vorherige gültige Konfiguration wiederhergestellt werden.

```text
Configuration N
      ↓
Configuration N+1
      ↓
Apply Failure
      ↓
Rollback
      ↓
Configuration N
```

Falls bereits irreversible Side Effects entstanden sind, gelten die Compensation- und Reconciliation-Regeln der System Transactions.

## Runtime Configuration

Configuration Transactions können auch für Änderungen im laufenden Betrieb verwendet werden.

```text
Running Component
       ↓
Configuration Transaction
       ↓
Validated Configuration
       ↓
Hot Apply
```

Eine Komponente muss deklarieren können, welche Einstellungen:

```text
Hot Applicable
Restart Required
Reboot Required
Immutable
```

sind.

NovaOS kann daraus die notwendige Apply-Strategie ableiten.

## Mehrere Komponenten

Eine Configuration Transaction kann mehrere Komponenten betreffen, wenn ihre Einstellungen semantisch zusammengehören.

```text
Configuration Transaction
 ├── Service A
 ├── Service B
 └── Routing
```

Der Transaction Scope muss dabei so klein wie möglich bleiben.

## Deklaratives Systemmodell

Configuration Transactions sind mit dem Declarative System Model integriert.

```text
Configuration Intent
       ↓
Desired State Change
       ↓
Configuration Transaction
       ↓
Commit
       ↓
Reconciliation
```

Konfiguration ist damit Teil des gewünschten Systemzustands und kein davon getrenntes imperatives Nebenmodell.

## Authority

Eine Configuration Transaction erzeugt keine zusätzliche Authority.

Jede Änderung muss innerhalb der Authority des Initiators liegen.

Eine Transaktion mit mehreren Änderungen darf nur committed werden, wenn alle enthaltenen Änderungen autorisiert sind.

## Security

Security-relevante Konfigurationen dürfen durch atomare Transaktionen nicht bestehende Schutzmechanismen umgehen.

Insbesondere bleiben:

```text
Capability Checks
Trust Requirements
Data Sovereignty
Information Flow Policies
Security Invariants
```

auch während Prepare und Commit verbindlich.

## Sensitive Configuration

Secrets und Credentials sollen nicht als gewöhnliche Konfigurationswerte behandelt werden.

Konfigurationen sollen stattdessen Referenzen auf dafür vorgesehene Credential- oder Secret-Mechanismen verwenden können.

```text
Configuration
     ↓
Secret Reference
     ↓
Credential Broker
```

Damit verhindert die Transaction Architecture unnötige Secret-Kopien in Konfigurationshistorien.

## Recovery

Unterbrochene Configuration Transactions müssen anhand ihres Transaction State recoverbar sein.

```text
Interrupted Transaction
        ↓
Inspect State
        ↓
Complete / Rollback / Reconcile
```

Nach einem Crash darf NovaOS nicht davon ausgehen, dass eine begonnene Konfigurationsänderung vollständig übernommen wurde.

## Offline Recovery

Kritische Systemkonfigurationen müssen bei Bedarf durch NovaDOS analysiert und auf einen bekannten gültigen Zustand zurückgeführt werden können.

Dies gilt insbesondere, wenn eine fehlerhafte Konfiguration den normalen Systemstart verhindert.

## Audit und Introspection

Relevante Configuration Transactions müssen nachvollziehbar machen können:

```text
Who changed?
What changed?
Which version?
Which transaction?
Was validation successful?
Was commit successful?
Was rollback required?
```

Sensitive Werte müssen dabei entsprechend ihrer Schutzklasse verborgen oder reduziert dargestellt werden.

## Normative Anforderungen

1. NovaOS MUSS zusammengehörige Konfigurationsänderungen als Configuration Transactions unterstützen.
2. Configuration Transactions MÜSSEN auf dem System-Transaction-Modell basieren.
3. Mehrere zusammengehörige Änderungen MÜSSEN als gemeinsames Change Set behandelbar sein.
4. Die resultierende Gesamtkonfiguration MUSS vor Commit validierbar sein.
5. Teilweise angewendete Change Sets DÜRFEN nicht als gültige Konfiguration sichtbar werden.
6. Relevante Konfigurationen MÜSSEN versionierbar sein.
7. Parallele inkompatible Änderungen MÜSSEN erkannt werden.
8. Automatisches Merge DARF nur bei semantisch kompatiblen Änderungen erfolgen.
9. Configuration Commit und physische Anwendung MÜSSEN unterscheidbar sein.
10. Komponenten MÜSSEN die Apply-Semantik relevanter Einstellungen deklarieren können.
11. Fehlgeschlagene Anwendung MUSS Rollback, Compensation oder Reconciliation ermöglichen.
12. Configuration Transactions DÜRFEN keine zusätzliche Authority erzeugen.
13. Security-, Trust-, Capability- und Data-Sovereignty-Hard-Constraints MÜSSEN während der gesamten Transaction gelten.
14. Secrets SOLLEN nicht als gewöhnliche persistierte Konfigurationswerte behandelt werden.
15. Unterbrochene Configuration Transactions MÜSSEN recoverbar sein.
16. Kritische Konfigurationen MÜSSEN bei Bedarf über NovaDOS offline recoverbar sein.
17. Configuration Transactions MÜSSEN mit dem Declarative System Model integrierbar sein.
18. Change Set, Version, Commit und Recovery MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- keine sichtbaren teilweise angewendeten Konfigurationen,
- zusammenhängende Einstellungen bleiben konsistent,
- fehlerhafte Konfigurationen können kontrolliert zurückgesetzt werden,
- parallele Änderungen können sicher erkannt werden,
- Runtime- und Systemkonfiguration verwenden dasselbe Transaktionsmodell,
- Konfiguration integriert sich direkt in den deklarativen Systemzustand.

### Negative Konsequenzen

- Konfigurationen benötigen Versionierung und Validierungslogik,
- Komponenten müssen ihre Apply-Semantik beschreiben,
- komplexe Cross-Component-Konfigurationen benötigen koordinierte Reconciliation.

## Verworfene Alternativen

### Einstellungen sofort einzeln schreiben

Verworfen, da dadurch inkonsistente Zwischenzustände entstehen können.

### Nur dateibasierte atomare Ersetzung

Verworfen, da NovaOS-Konfiguration nicht ausschließlich aus Dateien besteht und mehrere Komponenten gleichzeitig betreffen kann.

### Jede Änderung erfordert Neustart

Verworfen, da viele Einstellungen sicher im laufenden Betrieb übernommen werden können.

### Automatisches Last-Writer-Wins

Verworfen, da konkurrierende Änderungen dadurch unbemerkt verloren gehen können.

## Abhängigkeiten

- `ADR-TRANSACTION-0001_System_Transactions`
- `ADR-TRANSACTION-0002_Atomic_Desired_State_Changes`
- `ADR-ARCH-0002_Declarative_System_Model`
- `ADR-ARCH-0003_Transaktionale_Systemoperationen`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-RESILIENCE-0022_NovaDOS_für_Offline_Recovery`
- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-IDENTITY-0011_Credential_Broker`

## Zugehörige NPSPECs

- `NPSPEC-TRANSACTION-CONFIGURATION-0001`
- `NPSPEC-TRANSACTION-CONFIGURATION-CHANGESET-0001`
- `NPSPEC-TRANSACTION-CONFIGURATION-VERSION-0001`
- `NPSPEC-TRANSACTION-CONFIGURATION-VALIDATION-0001`
- `NPSPEC-TRANSACTION-CONFIGURATION-CONFLICT-0001`
- `NPSPEC-TRANSACTION-CONFIGURATION-APPLY-0001`
- `NPSPEC-TRANSACTION-CONFIGURATION-RECOVERY-0001`
- `NPSPEC-TRANSACTION-CONFIGURATION-TEST-0001`

## Ergebnis

NovaOS behandelt zusammengehörige Konfigurationsänderungen als atomare und versionierte Zustandsübergänge:

```text
Configuration N
      ↓
Change Set
      ↓
Validate
      ↓
Prepare
      ↓
Atomic Commit
      ↓
Configuration N+1
      ↓
Apply / Reconcile
      ↓
Verify
```

Damit können Konfigurationen systemweit geändert werden, ohne teilweise angewendete Zustände sichtbar zu machen, während Runtime-Anwendung, Konflikterkennung und Recovery kontrolliert über dieselbe Transaktionsarchitektur erfolgen.