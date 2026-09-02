# ADR-SECURITY-0016 – Data Sovereignty als Hard Constraint

## Status

Angenommen

## Kategorie

Security / Data Sovereignty / Information Flow / Policy

## Kontext

NovaOS kann Daten lokal, auf anderen Geräten, in Netzwerken oder auf entfernten Compute- und Storage-Systemen verarbeiten.

Dabei können Daten verbindlichen Anforderungen unterliegen, beispielsweise hinsichtlich:

- geografischer Regionen,
- administrativer Domains,
- Cloud- oder Service-Provider,
- Trust Domains,
- Speicherorten,
- Verarbeitungsorten,
- Netzwerk- und Transitpfaden.

Solche Anforderungen dürfen nicht lediglich als Optimierungspräferenz behandelt werden.

---

## Entscheidung

NovaOS behandelt **Data Sovereignty als systemweite Hard Constraint**.

```text
Operation
   ↓
Data Sovereignty Constraints
   ↓
Permitted?
   ├── No  → Reject / Replan
   └── Yes → Further Planning
```

Data-Sovereignty-Anforderungen werden vor Performance-, Kosten-, Energie- und anderen weichen Optimierungszielen ausgewertet.

---

## Grundprinzip

```text
Reachable ≠ Permitted
```

und:

```text
Sovereignty before optimization.
```

---

## Sovereignty Context

Daten und Operationen können einen Sovereignty Context besitzen.

Dieser kann unter anderem Anforderungen definieren an:

- Storage Location,
- Processing Location,
- Geographic Region,
- Administrative Domain,
- Provider,
- Trust Domain,
- Network Transit,
- Remote Services.

Die konkrete Repräsentation wird in NPSPECs definiert.

---

## Systemweite Durchsetzung

Data Sovereignty gilt über Subsystemgrenzen hinweg.

```text
Data
 ├── Storage
 ├── Network
 ├── Nova.Call
 ├── Remote Execution
 ├── Service Discovery
 ├── Migration
 └── Backup / Replication
```

Ein Subsystem darf Daten nicht an einen Ort bewegen, der durch deren Sovereignty Context ausgeschlossen ist.

---

## Information Flow Control

Data Sovereignty ist Teil der systemweiten Information-Flow-Architektur.

```text
Data
 ↓
Information Flow Policy
 ↓
Sovereignty Constraints
 ↓
Destination
```

Eine vorhandene Capability zur Datenverarbeitung oder Übertragung hebt Sovereignty Constraints nicht auf.

---

## Planung und Optimierung

NovaOS berücksichtigt nur Kandidaten, die alle harten Sovereignty Constraints erfüllen.

```text
Candidates
    ↓
Sovereignty Filtering
    ↓
Permitted Candidates
    ↓
Performance / Cost / Energy / Locality Optimization
```

Ein schnellerer oder günstigerer Kandidat darf nicht gewählt werden, wenn er die Sovereignty-Anforderungen verletzt.

---

## Netzwerk und Remote Execution

Bei Remote Execution müssen sowohl Ziel als auch relevanter Datenpfad berücksichtigt werden.

```text
Data
 ↓
Network Path
 ↓
Remote Execution Domain
```

Ein erlaubter Zielstandort macht einen verbotenen Transitpfad nicht automatisch zulässig.

Verschlüsselung oder VPN ändern ebenfalls nicht automatisch die Sovereignty-Eigenschaften des zugrunde liegenden Pfades.

---

## Migration und Replanning

Ändert sich während einer Operation:

- der Provider,
- der Netzwerkpfad,
- die Execution Location,
- der Storage Location,
- die Service Instance,

müssen relevante Sovereignty Constraints erneut berücksichtigt werden.

Migration darf bestehende Sovereignty-Grenzen nicht umgehen.

---

## Unknown State

Für harte Sovereignty-Anforderungen gilt:

```text
Unknown
    ≠
Allowed
```

Ist beispielsweise die relevante geografische oder administrative Zuordnung eines Providers nicht zuverlässig bestimmbar, darf NovaOS ihn nicht automatisch als zulässig behandeln.

---

## Graceful Degradation

Kann keine zulässige Ausführungsoption gefunden werden, darf NovaOS nicht stillschweigend auf einen verbotenen Standort oder Provider ausweichen.

Zulässige Reaktionen sind beispielsweise:

```text
Reject
Wait
Replan
Use Local Execution
Explicit Degradation
```

Eine Degradation muss selbst weiterhin alle Hard Constraints erfüllen.

---

## Adaptive Systeme

Adaptive Systeme dürfen zulässige Kandidaten bewerten und priorisieren.

```text
Hard Sovereignty Filter
        ↓
Eligible Candidates
        ↓
Adaptive Optimization
```

Prediction Error oder andere Lernmechanismen dürfen niemals zur Abschwächung einer Sovereignty Constraint führen.

---

## Audit und Introspection

NovaOS muss relevante Sovereignty-Entscheidungen nachvollziehbar machen können.

```text
Data
 ├── Sovereignty Policy
 ├── Requested Operation
 ├── Candidate Location
 ├── Provider
 ├── Relevant Path
 └── Decision
```

Damit muss erkennbar sein, warum eine bestimmte Verarbeitung oder Datenbewegung erlaubt oder abgelehnt wurde.

---

## Normative Anforderungen

1. NovaOS MUSS Data Sovereignty als Hard Constraint behandeln.
2. Sovereignty Constraints MÜSSEN vor weichen Optimierungszielen ausgewertet werden.
3. Capabilities DÜRFEN Sovereignty Constraints NICHT automatisch aufheben.
4. Sovereignty MUSS mit Information Flow Control integriert sein.
5. Storage-, Network- und Compute-Locations MÜSSEN durch Sovereignty Policies beschränkbar sein.
6. Remote Execution MUSS vor Datenübertragung auf Sovereignty-Konformität geprüft werden.
7. Netzwerk- und Transitpfade MÜSSEN bei entsprechenden Policies berücksichtigt werden können.
8. VPN oder Verschlüsselung DÜRFEN verbotene Pfade NICHT automatisch zulässig machen.
9. Migration und Replanning DÜRFEN bestehende Sovereignty Constraints NICHT umgehen.
10. Relevante Änderungen MÜSSEN eine erneute Sovereignty-Bewertung auslösen können.
11. Unbekannte sicherheitskritische Sovereignty-Eigenschaften MÜSSEN fail-safe behandelt werden.
12. NovaOS DARF bei fehlenden zulässigen Kandidaten NICHT stillschweigend auf nicht konforme Alternativen ausweichen.
13. Adaptive Systeme DÜRFEN Sovereignty Constraints NICHT abschwächen.
14. Sovereignty-Entscheidungen MÜSSEN kontrolliert auditierbar und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Data Sovereignty wird systemweit durchsetzbar,
- lokale und entfernte Verarbeitung folgen denselben Regeln,
- Netzwerk, Storage und Compute können gemeinsam bewertet werden,
- Optimierungen können keine regulatorischen oder organisatorischen Grenzen umgehen,
- gute Grundlage für datenschutzkritische und verteilte Workloads.

### Negative Konsequenzen

- zulässige Ausführungsoptionen können eingeschränkt werden,
- Location- und Provider-Metadaten müssen zuverlässig verfügbar sein,
- Replanning und Migration benötigen zusätzliche Policyprüfungen.

---

## Verworfene Alternativen

### Data Sovereignty als Präferenz

Verworfen.

Eine Präferenz könnte zugunsten von Performance oder Kosten übergangen werden.

### Sovereignty nur im Netzwerk

Verworfen.

Auch Storage und Compute Location sind relevant.

### Verschlüsselung macht jeden Standort zulässig

Verworfen.

Vertraulichkeit und Data Sovereignty sind unterschiedliche Anforderungen.

---

## Abhängigkeiten

- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-SECURITY-0005_MAC_für_systemweite_harte_Sicherheitsregeln`
- `ADR-SECURITY-0006_ABAC_als_zentrale_Policyentscheidung`
- `ADR-SECURITY-0015_Information_Flow_Control`
- `ADR-NETWORK-0021_Data_Sovereignty_Aware_Routing`
- `ADR-DISTCOMM-0011_Sovereignty_Preflight_vor_Remote_Execution`

---

## Zugehörige NPSPECs

- `NPSPEC-SECURITY-DATA-SOVEREIGNTY-0001`
- `NPSPEC-SECURITY-SOVEREIGNTY-CONTEXT-0001`
- `NPSPEC-SECURITY-SOVEREIGNTY-POLICY-0001`
- `NPSPEC-SECURITY-SOVEREIGNTY-ENFORCEMENT-0001`
- `NPSPEC-SECURITY-SOVEREIGNTY-TEST-0001`

---

## Ergebnis

Data Sovereignty ist in NovaOS keine optionale Präferenz, sondern eine verbindliche Grenze:

```text
Data / Operation
       ↓
Hard Sovereignty Constraints
       ↓
Permitted Candidates
       ↓
Optimization
       ↓
Execution
```

NovaOS optimiert ausschließlich innerhalb der zulässigen Sovereignty-Grenzen.

Damit können Performance, Kosten oder adaptive Entscheidungen niemals Vorrang vor verbindlichen Anforderungen an Speicherort, Verarbeitung und Informationsfluss erhalten.