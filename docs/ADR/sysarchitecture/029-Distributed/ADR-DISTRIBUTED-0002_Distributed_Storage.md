# ADR-DISTRIBUTED-0002 – Distributed Storage

## Status

Angenommen

## Kategorie

Distributed Architecture / Storage

## Kontext

NovaOS soll Objekte und Daten nicht grundsätzlich an ein einzelnes physisches Gerät oder einen einzelnen Storage Provider binden.

Daten können lokal, auf anderen NovaOS-Systemen oder über mehrere Storage Provider verteilt gespeichert werden.

Dabei müssen Security, Data Sovereignty, Verfügbarkeit, Integrität und Recovery weiterhin garantiert werden.

## Entscheidung

NovaOS unterstützt **Distributed Storage** als Erweiterung des normalen Storage-Modells.

```text
Storage Requirement
        ↓
Storage Contract
        ↓
Candidate Providers
   ┌────┼─────┐
 Local Remote Distributed
   └────┼─────┘
        ↓
Storage Plan
```

Der physische Speicherort ist eine Planungsentscheidung und nicht Teil der logischen Objektidentität.

## Object Identity

Objekte behalten ihre stabile Identität unabhängig vom Speicherort.

```text
Object ID
   ↓
Storage Resolution
   ↓
Physical Location
```

Verschieben, Replizieren oder Migrieren eines Objekts darf dessen Object ID nicht verändern.

## Location Transparency

Consumer sollen Objekte über ihre logische Identität adressieren.

```text
Consumer
   ↓
Object ID
   ↓
Resolver
   ↓
Storage Provider
```

Location Transparency darf jedoch relevante Unterschiede bei Latenz, Verfügbarkeit, Kosten oder Security nicht verbergen.

## Storage Contract

Distributed Storage muss deklarative Storage-Anforderungen berücksichtigen.

Beispiele:

```text
Persistence
Encryption
Integrity
Availability
Replication
Retention
Locality
Data Sovereignty
Recovery
```

Nur Storage Provider, die den erforderlichen Contract erfüllen können, dürfen verwendet werden.

## Replikation

Objekte können mehrere physische Repräsentationen besitzen.

```text
Object ID
   ├── Replica A
   ├── Replica B
   └── Replica C
```

Alle Replikate gehören logisch zum selben Objekt.

Eine echte Objektkopie erzeugt dagegen eine neue Object ID.

## Konsistenz

Distributed Storage muss die erforderliche Konsistenz explizit definieren.

```text
Object
  ↓
Consistency Contract
  ↓
Replication / Synchronization
```

Das System darf nicht implizit davon ausgehen, dass alle Replikate jederzeit identisch oder erreichbar sind.

## Data Sovereignty

Der Speicherort eines Objekts muss geltende Data-Sovereignty-Policies erfüllen.

```text
Object
  ↓
Storage Location
  ↓
Policy Check
```

Nicht zulässige Speicherorte dürfen nicht als Provider verwendet werden.

## Security

Remote Storage muss dieselben grundlegenden Sicherheitsanforderungen erfüllen wie lokaler Storage.

Dazu gehören insbesondere:

```text
Authorization
Encryption
Integrity
Trust
Provenance
```

Erreichbarkeit eines Storage Providers bedeutet nicht automatisch, dass dieser verwendet werden darf.

## Partial Failures

Distributed Storage muss mit partiellen Ausfällen umgehen können.

```text
Replica A = Available
Replica B = Offline
Replica C = Available
```

Der Ausfall eines Providers darf nicht automatisch bedeuten, dass das logische Objekt nicht verfügbar ist.

Der tatsächliche Zustand muss aus dem Storage Contract und den verbleibenden Replikaten bestimmt werden.

## Reconciliation

Distributed Storage verwendet State Reconciliation.

```text
Desired Storage State
        ↓
Observe Replicas
        ↓
Compare
        ↓
Repair / Replicate / Migrate
        ↓
Verify
```

Fehlende oder ungültige Replikate können dadurch kontrolliert wiederhergestellt werden.

## Datenlokalität

Distributed Storage und Distributed Execution sollen gemeinsam geplant werden können.

```text
Data Location
      +
Execution Requirement
      ↓
System Planning
```

Wenn möglich, soll unnötige Datenbewegung vermieden werden.

## Normative Anforderungen

1. NovaOS MUSS verteilte Storage Provider unterstützen können.
2. Object Identity MUSS vom physischen Speicherort unabhängig bleiben.
3. Replikation DARF keine neue logische Object Identity erzeugen.
4. Distributed Storage MUSS über explizite Storage Contracts gesteuert werden.
5. Konsistenzanforderungen MÜSSEN explizit definierbar sein.
6. Security-, Trust- und Data-Sovereignty-Constraints MÜSSEN eingehalten werden.
7. Partial Failures MÜSSEN als normaler Zustand verteilter Speicherung behandelt werden.
8. Replikation, Repair und Migration MÜSSEN durch Reconciliation steuerbar sein.
9. Storage Provider MÜSSEN vor ihrer Verwendung autorisiert und validiert werden.
10. Datenlokalität SOLL gemeinsam mit Distributed Execution berücksichtigt werden.

## Konsequenzen

### Positive Konsequenzen

- Objekte sind nicht an einzelne Geräte gebunden,
- Replikation und Migration werden transparent möglich,
- Verfügbarkeit und Recovery können über mehrere Systeme verteilt werden,
- Storage und Execution können gemeinsam nach Datenlokalität optimiert werden.

### Negative Konsequenzen

- Konsistenz und Partial Failures erhöhen die Komplexität,
- verteilte Replikation benötigt zusätzliche Synchronisations- und Recovery-Mechanismen.

## Abhängigkeiten

- `ADR-DISTRIBUTED-0001_Distributed_Execution`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-OBJECT-0002_Stabile_Object_Identity`
- `ADR-DECLARATIVE-0007_Declarative_Storage`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`
- `ADR-STORAGE-0005_Versioned_Storage`
- `ADR-STORAGE-0015_Checksumming_für_Integrität`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`

## Zugehörige NPSPECs

- `NPSPEC-DISTRIBUTED-STORAGE-0001`
- `NPSPEC-DISTRIBUTED-REPLICATION-0001`
- `NPSPEC-DISTRIBUTED-CONSISTENCY-0001`
- `NPSPEC-DISTRIBUTED-STORAGE-RECONCILIATION-0001`

## Ergebnis

NovaOS trennt die logische Identität eines Objekts von dessen physischer Speicherung:

```text
Object
   ↓
Storage Contract
   ↓
Constraint Solving
   ↓
Distributed Storage Plan
   ↓
Local / Remote Replicas
   ↓
Reconciliation
   ↓
Verified Storage State
```

Der zentrale Grundsatz lautet:

```text
Ein Objekt besitzt eine
logische Identität.

Wo und wie seine Daten gespeichert werden,
ist eine kontrollierte Systementscheidung.
```