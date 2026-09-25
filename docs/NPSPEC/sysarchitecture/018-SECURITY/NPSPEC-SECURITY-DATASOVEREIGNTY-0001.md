# NPSPEC-SECURITY-DATASOVEREIGNTY-0001 – Nova Data Sovereignty

## Status

Angenommen

## Kategorie

Security / Data Protection / Data Sovereignty

## Zweck

NovaOS definiert Data Sovereignty zur Kontrolle darüber, wo Daten gespeichert, verarbeitet und übertragen werden dürfen.

```text
Data
 ↓
Sovereignty Policy
 ↓
Allowed Location / Provider / Domain
```

Technische Erreichbarkeit oder Verschlüsselung allein erzeugt keine Berechtigung zur Datenverarbeitung.

## Grundprinzipien

```text
Reachable ≠ Permitted
Encrypted ≠ Sovereign
Cloud ≠ Trusted
Fastest Location ≠ Allowed Location
Data Access ≠ Data Transfer Permission
```

Souveränitätsanforderungen gelten als harte Constraints, sofern sie als verpflichtend definiert wurden.

## Sovereignty-Modell

Daten dürfen einen Sovereignty Context besitzen:

```text
SovereigntyContext
├── DataID
├── Allowed Regions
├── Allowed Providers
├── Allowed Trust Domains
└── Policy
```

Optional:

```text
Forbidden Regions
Processing Restrictions
Replication Restrictions
Retention Policy
Export Policy
Legal Context
```

Die Policy muss an die logische Datenidentität gebunden werden können und darf nicht allein vom aktuellen Speicherort abhängen.

## Geltungsbereiche

Sovereignty-Regeln müssen anwendbar sein auf:

```text
Storage
Network Transfer
Remote Processing
Replication
Backup
Caching
Migration
Distributed Services
```

Ein zulässiger Primärspeicherort bedeutet nicht automatisch, dass Replikate oder Backups überall gespeichert werden dürfen.

## Policy-Auswertung

Vor einer relevanten Datenbewegung muss geprüft werden können:

```text
Data Policy
    +
Destination
    +
Provider
    +
Trust Domain
    +
Operation
    ↓
Allow / Deny
```

Unbekannte Standorte oder Trust Domains dürfen bei verpflichtenden Regeln nicht implizit akzeptiert werden.

## Netzwerk

Data Sovereignty integriert sich mit `Network Intent`.

```text
Sovereignty Requirements
          ↓
Network Intent
          ↓
Permitted Paths
          ↓
Destination
```

Routing, Multipath oder Migration dürfen keinen verbotenen Pfad als Optimierung verwenden.

## Storage und Replikation

Storage-Systeme müssen Sovereignty-Regeln bei:

```text
Placement
Replication
Migration
Backup
Recovery
```

berücksichtigen können.

Eine automatische Optimierung darf Daten nicht in eine verbotene Region oder Trust Domain verschieben.

## Remote Processing

Auch die Verarbeitung selbst darf beschränkt werden.

```text
Data
 ↓
Allowed Compute Location
 ↓
Execution
```

Location Transparency darf Sovereignty-Anforderungen nicht unsichtbar umgehen.

## Information Flow

Data Sovereignty ergänzt Information Flow Security.

```text
Information Flow
      +
Sovereignty Policy
      ↓
Allowed Destination
```

Damit kann sowohl die Art des Datenflusses als auch dessen geografischer oder organisatorischer Zielbereich kontrolliert werden.

## Priorität

Bei Konflikten gilt die NovaOS Constraint-Priorität:

```text
Safety
  ↓
Security
  ↓
Sovereignty / Trust
  ↓
Hard System Constraints
  ↓
User Preferences
  ↓
Optimization
```

Performance- oder Kostenoptimierung darf verpflichtende Sovereignty-Regeln nicht verletzen.

## Introspection und Audit

NovaOS soll mindestens darstellen können:

```text
DataID
Current Location
Allowed Regions
Provider
Trust Domain
Policy
Decision
Decision Reason
```

Sicherheitskritische Datenbewegungen sollen auditierbar sein.

## Normative Anforderungen

1. NovaOS MUSS Data-Sovereignty-Regeln unterstützen können.
2. Daten MÜSSEN zulässige Regionen, Provider und Trust Domains definieren können.
3. Sovereignty-Regeln MÜSSEN auf Storage, Netzwerk und Remote Processing anwendbar sein.
4. Replikation und Backup MÜSSEN Sovereignty-Regeln berücksichtigen können.
5. Verbindliche Sovereignty-Regeln DÜRFEN NICHT durch Optimierung umgangen werden.
6. Network Intent MUSS Sovereignty-Anforderungen berücksichtigen können.
7. Location Transparency DARF Sovereignty-Regeln NICHT umgehen.
8. Unbekannte Standorte DÜRFEN bei verpflichtenden Regeln NICHT implizit erlaubt werden.
9. Datenmigration MUSS vor Ausführung auf Sovereignty-Konformität prüfbar sein.
10. Sovereignty-Entscheidungen SOLLEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-SECURITY-INFORMATIONFLOW-0001`
- `NPSPEC-SECURITY-ABAC-0001`
- `NPSPEC-NETWORK-SOVEREIGNTY-0001`
- `NPSPEC-NETWORK-INTENT-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-SECURITY-0012`

## Ergebnis

```text
Data
 ↓
Sovereignty Constraints
 ↓
Storage + Network + Compute
 ↓
Permitted Locations
```

NovaOS erhält damit eine systemweite Kontrolle darüber, wo Daten gespeichert, übertragen und verarbeitet werden dürfen, ohne dass automatische Optimierung oder Location Transparency verbindliche Souveränitätsgrenzen umgehen können.