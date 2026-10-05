# NPSPEC-TEMP-MODEL-0001 – Nova Temporary Resource Model

## Status

Angenommen

## Kategorie

Temporary Resources / Architecture

## Zweck

NovaOS definiert ein einheitliches Modell für temporäre Ressourcen.

Temporäre Ressourcen besitzen einen klaren Eigentümer, Gültigkeitsbereich und Lebenszyklus und können nach Ablauf automatisch freigegeben werden.

## Grundprinzipien

```text
Temporary ≠ Persistent
Temporary ≠ Cache
Temporary ≠ User Data
Lifetime ≠ Permission
Expiration ≠ Immediate Destruction
```

Temporäre Ressourcen sind grundsätzlich entbehrlich und dürfen nicht die einzige dauerhafte Kopie wichtiger Daten darstellen.

## Modell

Eine temporäre Ressource besitzt mindestens:

```text
TempResource
├── ResourceID
├── OwnerID
├── Scope
├── Lifetime
├── State
└── ResourceType
```

Optional:

```text
Expiration
ResourceBudget
CleanupPolicy
SecurityPolicy
```

## Scopes

NovaOS unterstützt mindestens:

```text
Process
Program
Solution
Workspace
User
System
```

Der Scope bestimmt, welchem Kontext die Ressource zugeordnet ist.

## Lebensdauer

Mögliche Lifetimes sind:

```text
UntilReleased
UntilProcessExit
UntilSessionEnd
UntilWorkspaceClose
UntilReboot
UntilExpiration
```

Eine Ressource kann früher freigegeben werden, wenn sie nicht mehr benötigt wird.

## Ressourcentypen

Das Modell ist nicht auf Dateien beschränkt.

Temporär können beispielsweise sein:

```text
Files
Buffers
Shared Memory
Generated Artifacts
Intermediate Results
IPC Resources
Runtime Objects
```

Spezialisierte Subsysteme definieren die konkrete Umsetzung.

## Zustände

```text
Allocated
Active
Released
Expired
Reclaimable
Destroyed
```

`Expired` bedeutet, dass die Ressource nicht mehr regulär verwendet werden soll.

Die tatsächliche Freigabe kann kontrolliert durch den Resource Manager erfolgen.

## Resource Economy

Temporäre Ressourcen unterliegen Ressourcenbudgets.

NovaOS kann Grenzen für beispielsweise:

```text
Storage
Memory
Object Count
Lifetime
```

festlegen.

Unter Ressourcendruck dürfen freigebbare oder abgelaufene temporäre Ressourcen bevorzugt zurückgewonnen werden.

## Sicherheit

Eine temporäre Ressource übernimmt keine zusätzliche Authority.

Zugriffe bleiben an den jeweiligen Sicherheits- und Capability-Kontext gebunden.

Sensible temporäre Daten müssen entsprechend ihrer Security Policy bereinigt werden können.

## Normative Anforderungen

1. Jede temporäre Ressource MUSS einen eindeutigen Eigentümer oder Systemkontext besitzen.
2. Jede temporäre Ressource MUSS einem Scope zugeordnet sein.
3. Die Lebensdauer MUSS bestimmbar sein.
4. Temporäre Ressourcen DÜRFEN keine erforderliche dauerhafte Datenkopie ersetzen.
5. Temporäre Ressourcen MÜSSEN kontrolliert freigegeben werden können.
6. Abgelaufene Ressourcen SOLLEN automatisch reclaimable werden.
7. Ressourcenbudgets MÜSSEN auf temporäre Ressourcen anwendbar sein.
8. Temporäre Ressourcen DÜRFEN keine zusätzliche Authority erzeugen.
9. Sensible temporäre Ressourcen MÜSSEN sicher bereinigbar sein.
10. Das Modell MUSS unterschiedliche temporäre Ressourcentypen unterstützen.

## Abhängigkeiten

- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-USERSPACE-TEMP-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`

## Ergebnis

NovaOS erhält ein systemweit einheitliches Modell für kurzlebige Ressourcen. Temporäre Dateien, Speicherbereiche und andere Laufzeitressourcen können dadurch eindeutig einem Kontext zugeordnet, budgetiert, geschützt und nach Ende ihrer Lebensdauer kontrolliert freigegeben werden.