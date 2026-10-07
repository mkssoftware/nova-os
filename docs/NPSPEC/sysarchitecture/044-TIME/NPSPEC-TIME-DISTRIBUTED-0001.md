# NPSPEC-TIME-DISTRIBUTED-0001 – Nova Distributed Time

## Status

Angenommen

## Kategorie

Time / Distributed Systems

## Zweck

NovaOS definiert ein Zeitmodell für verteilte Systeme, in denen mehrere Geräte, Nodes oder Execution Locations keine perfekt identische globale Uhr voraussetzen können.

Verteilte Operationen müssen Zeitunsicherheit, Netzwerkverzögerung und Clock-Abweichungen explizit berücksichtigen.

## Grundprinzipien

```text
Distributed Time ≠ Perfect Global Clock
Synchronized ≠ Identical
Timestamp ≠ Causality
Clock Order ≠ Event Order
Network Delay ≠ Clock Offset
Remote Time ≠ Local Time
```

## Modell

```text
DistributedTimeContext
├── NodeID
├── ClockDomainID
├── LocalTime
├── ReferenceTime
├── OffsetEstimate
├── Uncertainty
├── SynchronizationState
└── TrustState
```

## Architektur

```text
Node A Clock ──┐
Node B Clock ──┼─→ Synchronization Layer
Node C Clock ──┘
                       ↓
              Distributed Time Context
                       ↓
          Distributed Operations
```

Jeder Node behält seine eigene lokale Zeitbasis.

## Zeitvergleich

Zeitwerte verschiedener Nodes dürfen nur unter Berücksichtigung ihrer Unsicherheit verglichen werden.

```text
Node A:
Time ± Uncertainty

Node B:
Time ± Uncertainty
```

Überlappen die Unsicherheitsintervalle, darf daraus keine eindeutige zeitliche Reihenfolge abgeleitet werden.

## Synchronisation

Distributed Time darf Referenzen aus mehreren Mechanismen beziehen:

```text
NTP
PTP
Trusted Time
Local Clock
Platform Time
Peer Synchronization
```

Die verwendete Synchronisationsmethode bestimmt die erreichbare Genauigkeit und Unsicherheit.

## Kausalität

Zeitstempel allein dürfen nicht als vollständiger Nachweis kausaler Reihenfolge verwendet werden.

Für verteilte Ereignisse dürfen zusätzliche Mechanismen eingesetzt werden:

```text
Sequence Numbers
Logical Clocks
Causal Metadata
Transaction Ordering
Consensus Ordering
```

Physische Zeit und logische Ereignisordnung bleiben getrennte Konzepte.

## Remote Deadlines

Eine Deadline auf einem entfernten Node muss in einen geeigneten Remote-Zeitkontext übersetzt werden.

```text
Local Deadline
      ↓
Clock Relation
      +
Uncertainty
      ↓
Remote Deadline Window
```

Bei hoher Unsicherheit muss die Operation konservativ behandelt werden.

## Location Transparency

Bei Migration einer Operation zwischen Nodes müssen deren Zeitabhängigkeiten erhalten bleiben.

```text
Task
 ↓
Migration
 ↓
New Node
 ↓
Rebase Time Context
 ↓
Preserve Deadline Semantics
```

Location Transparency darf Zeitunterschiede zwischen Nodes nicht verbergen, wenn diese für die Korrektheit relevant sind.

## Partitionen

Bei Netzwerkpartitionen:

```text
Synchronization Lost
        ↓
Holdover
        ↓
Growing Uncertainty
        ↓
Degraded Distributed Time
```

Operationen mit strengeren Zeitbedingungen dürfen bei zu hoher Unsicherheit abgelehnt oder lokal gehalten werden.

## Sicherheit

Remote-Zeitinformationen erzeugen keine Authority.

Zeitinformationen müssen hinsichtlich:

```text
Source Identity
Trust
Integrity
Freshness
Uncertainty
```

bewertet werden.

## Normative Anforderungen

1. NovaOS DARF keine perfekt synchronisierte globale Uhr voraussetzen.
2. Jeder Node MUSS seine lokale Clock Domain behalten können.
3. Remote-Zeitwerte MÜSSEN ihre Unsicherheit ausdrücken können.
4. Zeitwerte unterschiedlicher Nodes DÜRFEN nicht ungeprüft direkt verglichen werden.
5. Überlappende Unsicherheitsintervalle MÜSSEN als zeitlich mehrdeutig behandelbar sein.
6. Physische Zeit und kausale Ereignisordnung MÜSSEN getrennte Konzepte bleiben.
7. Remote Deadlines MÜSSEN Clock-Abweichung und Unsicherheit berücksichtigen.
8. Migration MUSS zeitliche Execution-Contract-Semantik erhalten können.
9. Netzwerkpartitionen MÜSSEN einen Holdover-Betrieb ermöglichen.
10. Wachsende Unsicherheit MUSS zu kontrollierter Degradation führen können.
11. Remote-Zeitinformationen MÜSSEN hinsichtlich Herkunft und Trust validierbar sein.
12. Node, Clock Domain, Offset, Unsicherheit, Synchronisations- und Trust-Zustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-CLOCKDOMAIN-0001`
- `NPSPEC-TIME-DEADLINE-0001`
- `NPSPEC-TIME-SYNCHRONIZATION-0001`
- `NPSPEC-TIME-NTP-0001`
- `NPSPEC-TIME-PTP-0001`
- `NPSPEC-TIME-TRUSTED-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS behandelt Zeit in verteilten Systemen als lokale Zeitbasen mit expliziten Beziehungen und Unsicherheiten statt als perfekte globale Uhr. Dadurch können Remote Deadlines, Migration, Synchronisationsverlust und verteilte Ereignisse korrekt behandelt werden, ohne physische Zeit fälschlich mit kausaler Ordnung gleichzusetzen.