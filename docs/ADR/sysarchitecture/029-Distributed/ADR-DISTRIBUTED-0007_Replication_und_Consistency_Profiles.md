# ADR-DISTRIBUTED-0007 – Replication und Consistency Profiles

## Status

Angenommen

## Kategorie

Distributed Architecture / Storage / Consistency

## Kontext

Distributed Storage kann Objekte über mehrere Nodes und Failure Domains replizieren.

Dabei existiert kein universelles Konsistenzmodell, das für alle Daten optimal ist.

Ein Konfigurationsobjekt benötigt möglicherweise starke Konsistenz, während Cache-, Telemetrie- oder Mediendaten schwächere Garantien akzeptieren können.

NovaOS benötigt daher explizite **Consistency Profiles**.

## Entscheidung

NovaOS trennt **Replication** und **Consistency** als eigenständige, aber koordinierte Contracts.

```text
Object
  ↓
Replication Profile
  +
Consistency Profile
  ↓
Distributed Storage Plan
```

Die Anzahl der Replikate bestimmt nicht automatisch deren Konsistenzsemantik.

## Replication Profile

Das Replication Profile beschreibt die gewünschte physische Redundanz.

Beispiele:

```text
Replica Count
Failure Domain Separation
Minimum Available Replicas
Placement Constraints
Recovery Requirements
```

Beispiel:

```text
Replicas = 3
Failure Domains = 3
Minimum Available = 2
```

## Consistency Profile

Das Consistency Profile beschreibt die erforderlichen Sichtbarkeits- und Ordnungsregeln.

NovaOS unterstützt mindestens konzeptionell:

```text
Strong
Causal
Eventual
```

Weitere Profile können später ergänzt werden.

## Strong Consistency

```text
Write
  ↓
Coordination
  ↓
Committed State
  ↓
Readers
```

Nach erfolgreichem Commit muss der definierte konsistente Zustand gelten.

Dieses Profil wird verwendet, wenn widersprüchliche gleichzeitig gültige Zustände nicht akzeptabel sind.

## Causal Consistency

Kausal voneinander abhängige Änderungen müssen in ihrer logischen Reihenfolge sichtbar bleiben.

```text
A
↓
B
↓
C
```

Unabhängige Änderungen können dagegen parallel verarbeitet werden.

## Eventual Consistency

Replikate dürfen vorübergehend unterschiedliche Zustände besitzen.

```text
Replica A = V3
Replica B = V2
Replica C = V3
```

Ohne weitere Änderungen sollen sie schließlich zu einem gültigen gemeinsamen Zustand konvergieren.

## Versionen und Konflikte

Verteilte Änderungen müssen eindeutig versionierbar sein.

```text
Object ID
   ├── Version A
   └── Version B
```

Entstehen konkurrierende Versionen, müssen sie anhand einer definierten Conflict Policy behandelt werden.

NovaOS darf konkurrierende Zustände nicht stillschweigend überschreiben.

## Quorum

Consistency Profiles können Quorum-Anforderungen definieren.

```text
N = Replicas
R = Read Requirement
W = Write Requirement
```

Die konkrete Quorum-Strategie bleibt Implementierungsdetail des Providers, solange der zugesicherte Contract erfüllt wird.

## Degraded State

Kann das geforderte Consistency Profile aufgrund von Ausfällen nicht eingehalten werden, muss NovaOS dies sichtbar machen.

```text
Required:
    Strong

Available:
    Insufficient Replicas

Result:
    Degraded / Unavailable
```

NovaOS darf die Konsistenzanforderung nicht stillschweigend abschwächen.

## Reconciliation

Nach Wiederherstellung der Verbindung müssen Replikate kontrolliert synchronisiert werden.

```text
Partition Healed
      ↓
Compare Versions
      ↓
Conflict Detection
      ↓
Reconciliation
      ↓
Verified State
```

## Normative Anforderungen

1. NovaOS MUSS Replication und Consistency als getrennte Contracts behandeln.
2. Replication Profiles MÜSSEN Replikatanzahl und Failure-Domain-Anforderungen beschreiben können.
3. Consistency Profiles MÜSSEN explizit definiert werden.
4. NovaOS MUSS mindestens Strong, Causal und Eventual Consistency modellieren können.
5. Konsistenzanforderungen DÜRFEN bei Ausfällen nicht stillschweigend abgeschwächt werden.
6. Verteilte Änderungen MÜSSEN versionierbar sein.
7. Konflikte MÜSSEN erkannt und kontrolliert behandelt werden.
8. Consistency Profiles DÜRFEN Quorum-Anforderungen definieren.
9. Nicht erfüllbare Consistency Contracts MÜSSEN als Degraded oder Unavailable sichtbar werden.
10. Replikate MÜSSEN nach Partitionen kontrolliert reconciliert werden können.
11. Replikation MUSS Failure Domains berücksichtigen können.

## Konsequenzen

### Positive Konsequenzen

- Konsistenz kann passend zum Datentyp gewählt werden,
- Redundanz und Konsistenz bleiben sauber getrennt,
- unnötig starke Synchronisation kann vermieden werden,
- Partitionen und konkurrierende Versionen werden explizit behandelt.

### Negative Konsequenzen

- mehrere Konsistenzmodelle erhöhen die Komplexität,
- Anwendungen und Capabilities müssen ihre benötigte Konsistenz deklarieren.

## Abhängigkeiten

- `ADR-DISTRIBUTED-0002_Distributed_Storage`
- `ADR-DISTRIBUTED-0005_Cluster_Placement`
- `ADR-DISTRIBUTED-0006_Failure_Domain_Aware_Placement`
- `ADR-OBJECT-0003_Object_Versioning`
- `ADR-DECLARATIVE-0007_Declarative_Storage`
- `ADR-RESILIENCE-0003_Hierarchical_Failure_Domains`

## Zugehörige NPSPECs

- `NPSPEC-DISTRIBUTED-REPLICATION-PROFILE-0001`
- `NPSPEC-DISTRIBUTED-CONSISTENCY-PROFILE-0001`
- `NPSPEC-DISTRIBUTED-QUORUM-0001`
- `NPSPEC-DISTRIBUTED-CONFLICT-RECONCILIATION-0001`

## Ergebnis

NovaOS behandelt Redundanz und Konsistenz als explizite Eigenschaften verteilter Daten:

```text
Object
  ↓
Replication Requirements
  +
Consistency Requirements
  ↓
Placement
  ↓
Replication
  ↓
Consistency Enforcement
  ↓
Verified Distributed State
```

Der zentrale Grundsatz lautet:

```text
Wie oft Daten existieren
und wie konsistent diese Kopien sein müssen,

sind zwei getrennte Entscheidungen.
```