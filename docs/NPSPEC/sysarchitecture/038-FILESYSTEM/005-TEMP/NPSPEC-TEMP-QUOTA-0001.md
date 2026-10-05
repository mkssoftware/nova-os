# NPSPEC-TEMP-QUOTA-0001 – Nova Temporary Resource Quotas

## Status

Angenommen

## Kategorie

Temporary Resources / Resource Economy

## Zweck

NovaOS definiert Quotas für temporäre Ressourcen.

Quotas begrenzen den Ressourcenverbrauch einzelner Benutzer, Prozesse, Programme, Solutions, Workspaces und Systemsichten und verhindern, dass temporäre Ressourcen unbegrenzt Systemkapazität belegen.

## Grundprinzipien

```text
Quota ≠ Reservation
Quota ≠ Current Usage
Quota ≠ Permission
Quota ≠ Guaranteed Capacity
Temporary Resource ≤ Applicable Quota
```

## Quota-Modell

Eine Quota kann mindestens definieren:

```text
TempQuota
├── Scope
├── OwnerID
├── ResourceType
├── SoftLimit
├── HardLimit
└── CurrentUsage
```

Mögliche Ressourcenmetriken:

```text
Storage
Memory
Object Count
Lifetime
Shared Memory
Generated Data
```

## Scopes

Quotas können auf unterschiedlichen Ebenen gelten:

```text
System
User
Session
Workspace
Solution
Program
Process
```

Mehrere Quotas dürfen gleichzeitig auf eine Ressource wirken.

Die effektiv zulässige Nutzung darf keine übergeordnete Begrenzung überschreiten.

## Soft- und Hard-Limit

```text
Usage < SoftLimit
    → Normal

Usage ≥ SoftLimit
    → Warning / Cleanup / Throttling

Usage ≥ HardLimit
    → Allocation Denied
```

Das Überschreiten eines Soft-Limits darf automatische Cleanup- oder Reclaim-Maßnahmen auslösen.

Ein Hard-Limit darf nur durch eine ausdrücklich autorisierte Policy überschritten werden.

## Ressourcenanforderung

```text
Temp Allocation Request
        ↓
Determine Scope
        ↓
Evaluate Quotas
        ↓
Resource Budget Check
        ↓
Allocate / Reclaim / Deny
```

Eine fehlgeschlagene Temp-Allokation muss kontrolliert an den Aufrufer gemeldet werden.

## Cleanup

Bei Quota-Druck kann NovaOS bevorzugt reclaimable Ressourcen entfernen:

```text
Expired
Released
Invalid Cache
Orphaned
Low Priority Temp
```

Aktive oder für Recovery benötigte Ressourcen dürfen nicht allein wegen eines Soft-Limits unkontrolliert gelöscht werden.

## Accounting

Erzeugung und Freigabe temporärer Ressourcen müssen in das Resource Accounting einfließen.

```text
Allocate → Usage +
Destroy  → Usage -
```

Abstürze dürfen nicht dauerhaft zu falschen Quota-Zählerständen führen.

## Sicherheit

Quotas erzeugen keine Authority.

Ein Prozess darf durch Delegation, zusätzliche Temp-Scopes oder gemeinsam genutzte Ressourcen seine effektiven Ressourcenlimits nicht unkontrolliert umgehen.

## Normative Anforderungen

1. NovaOS MUSS Quotas für temporäre Ressourcen unterstützen.
2. Quotas MÜSSEN an definierte Scopes und Owner bindbar sein.
3. Soft- und Hard-Limits MÜSSEN unterstützt werden können.
4. Hard-Limits MÜSSEN vor neuen Allokationen geprüft werden.
5. Mehrere gleichzeitig geltende Quotas MÜSSEN gemeinsam berücksichtigt werden.
6. Temp-Allokationen MÜSSEN im Resource Accounting erfasst werden.
7. Freigegebene Ressourcen MÜSSEN aus der Nutzung herausgerechnet werden.
8. Quota-Druck DARF kontrollierten Cleanup auslösen.
9. Recovery-Ressourcen DÜRFEN nicht unkontrolliert gelöscht werden.
10. Quotas DÜRFEN keine zusätzliche Authority erzeugen.
11. Quotas DÜRFEN nicht durch untergeordnete Scopes umgangen werden.
12. Quota-Verletzungen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TEMP-MODEL-0001`
- `NPSPEC-TEMP-TTL-0001`
- `NPSPEC-TEMP-CACHE-0001`
- `NPSPEC-TEMP-RECOVERY-0001`
- `NPSPEC-TEMP-CLEANUP-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`

## Ergebnis

NovaOS begrenzt temporären Ressourcenverbrauch kontrolliert pro Scope und Owner. Soft-Limits können Cleanup und Reclaim auslösen, während Hard-Limits eine unkontrollierte Ressourcenbelegung verhindern und vollständig in das systemweite Resource Accounting integriert sind.