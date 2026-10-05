# NPSPEC-SYSTEM-RESOURCES-0001 – Nova System Resources

## Status

Angenommen

## Kategorie

System / Resources

## Zweck

NovaOS definiert ein einheitliches Modell für die Verwaltung systemweiter Ressourcen.

Ressourcen werden identifiziert, einem Owner zugeordnet, budgetiert, autorisiert und über ihren gesamten Lebenszyklus kontrolliert.

## Grundprinzipien

```text
Resource ≠ Authority
ResourceID ≠ Physical Location
Ownership ≠ Permission
Allocation ≠ Unlimited Usage
Visibility ≠ Access
Resource Limit ≠ Resource Availability
```

## Ressourcenmodell

Eine Ressource besitzt mindestens:

```text
SystemResource
├── ResourceID
├── ResourceType
├── OwnerID
├── Scope
├── State
└── Usage
```

Optional:

```text
Budget
Priority
Lifetime
Location
Policy
SecurityContext
```

## Ressourcentypen

Das Modell gilt unter anderem für:

```text
CPU Time
Memory
Storage
I/O
Network
GPU / Accelerator
Devices
Handles
IPC Resources
Tasks
Temporary Resources
```

Subsysteme dürfen spezialisierte Ressourcenmodelle darauf aufbauen.

## Eigentümer und Scope

Ressourcenverbrauch muss einem verantwortlichen Kontext zugeordnet werden können:

```text
System
User
Service
Program
Solution
Workspace
Process
Task
```

Untergeordnete Kontexte dürfen Ressourcenlimits übergeordneter Kontexte nicht umgehen.

## Resource Accounting

```text
Allocate
   ↓
Account Usage
   ↓
Use
   ↓
Release
   ↓
Return Resource
```

Ressourcenverbrauch muss möglichst dem tatsächlichen Verursacher zugerechnet werden.

## Budgets

NovaOS kann Ressourcenbudgets definieren:

```text
Hard Limit
Soft Limit
Reservation
Priority
Deadline
Quota
```

Budgets können hierarchisch wirken.

```text
User Budget
    ↓
Program Budget
    ↓
Process Budget
    ↓
Task Budget
```

## Ressourcenanforderung

```text
Resource Request
      ↓
Authority Check
      ↓
Policy Evaluation
      ↓
Budget Check
      ↓
Allocation
```

Eine erfolgreiche Berechtigungsprüfung garantiert nicht, dass genügend Ressourcen verfügbar sind.

## Ressourcendruck

Bei Ressourcenknappheit kann NovaOS kontrolliert reagieren:

```text
Reclaim
Cache Cleanup
Throttle
Reduce Quality
Suspend
Deny Allocation
Terminate
```

Die Auswahl erfolgt nach System Policy, Priorität und Sicherheitsanforderungen.

## Isolation

Ein fehlerhafter oder ressourcenintensiver Kontext darf andere Systembereiche nicht unbegrenzt verdrängen.

Ressourcenlimits müssen deshalb auf Prozesse, Programme, Solutions und Systemdienste anwendbar sein.

## Sicherheit

Der Besitz oder Verbrauch einer Ressource erzeugt keine zusätzliche Authority.

Geschützte Ressourcen benötigen weiterhin entsprechende Capabilities.

```text
ResourceID
    ↓
Capability Check
    ↓
Authorized Handle
```

## Introspection

NovaOS muss mindestens folgende Informationen kontrolliert bereitstellen können:

```text
Owner
Resource Type
Current Usage
Budget
Limits
State
Pressure
```

## Normative Anforderungen

1. Systemressourcen MÜSSEN eindeutig identifizierbar sein.
2. Ressourcenverbrauch MUSS einem verantwortlichen Kontext zugeordnet werden können.
3. NovaOS MUSS Resource Accounting unterstützen.
4. Ressourcenbudgets MÜSSEN hierarchisch anwendbar sein können.
5. Untergeordnete Kontexte DÜRFEN übergeordnete Limits nicht umgehen.
6. Berechtigung DARF nicht mit Ressourcenverfügbarkeit gleichgesetzt werden.
7. NovaOS MUSS kontrolliert auf Ressourcendruck reagieren können.
8. Reclaimable Ressourcen SOLLEN vor kritischen Maßnahmen zurückgewonnen werden.
9. Ressourcenintensive Komponenten MÜSSEN begrenzbar sein.
10. Ressourcenbesitz DARF keine zusätzliche Authority erzeugen.
11. Geschützte Ressourcen MÜSSEN capability-basiert kontrollierbar sein.
12. Ressourcenverbrauch und Ressourcenstatus MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-KERNEL-0001`
- `NPSPEC-SYSTEM-FOUNDATION-0001`
- `NPSPEC-SYSTEM-POLICY-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt ein einheitliches Ressourcenmodell für CPU, Speicher, Storage, I/O, Netzwerk, Geräte und weitere Systemressourcen. Verbrauch wird eindeutig zugeordnet, budgetiert und kontrolliert, sodass einzelne Komponenten das Gesamtsystem nicht unkontrolliert dominieren können.