# ADR-ADAPTIVE-0009 – Adaptive Networking

## Status

Angenommen

## Kategorie

Adaptive Systems / Networking

## Kontext

NovaOS nutzt Netzwerke für Kommunikation, Distributed Execution, Storage, Remote Capabilities und Datenübertragung.

Netzwerkbedingungen verändern sich dynamisch:

```text
Latency
Bandwidth
Packet Loss
Congestion
Route Quality
Interface Availability
Energy Cost
Network Load
```

Statische Netzwerkentscheidungen können diese Veränderungen nur begrenzt berücksichtigen.

NovaOS soll deshalb Netzwerkverhalten beobachten, zukünftige Bedingungen vorhersagen und aus Prediction Error lernen.

## Entscheidung

NovaOS unterstützt **Adaptive Networking** als Optimierung innerhalb bestehender Network-, Security- und Execution-Contracts.

```text
Network State
     ↓
Prediction
     ↓
Network Decision
     ↓
Transfer / Communication
     ↓
Measured Result
     ↓
Prediction Error
     ↓
Model Correction
```

Adaptive Networking darf verbindliche Netzwerk- und Sicherheitsanforderungen nicht verändern.

## Network Predictions

NovaOS darf insbesondere vorhersagen:

```text
Latency
Bandwidth
Transfer Time
Packet Loss
Congestion
Connection Quality
Route Availability
Energy Cost
Remote Provider Reachability
```

Diese Vorhersagen dürfen in Execution Planning, Scheduling und Datenbewegung einfließen.

## Prediction Error

Vorhersagen werden mit tatsächlich beobachtetem Netzwerkverhalten verglichen.

Beispiel:

```text
Predicted Transfer: 40 ms
Actual Transfer:    73 ms
        ↓
Prediction Error
        ↓
Network Model Correction
```

Wiederholte Abweichungen sollen zukünftige Netzwerkentscheidungen verbessern.

## Adaptive Path Selection

Sind mehrere zulässige Netzwerkpfade vorhanden, darf NovaOS diese dynamisch bewerten.

```text
Path A
Path B
Path C
   ↓
Hard Constraint Filtering
   ↓
Valid Paths
   ↓
Adaptive Evaluation
   ↓
Selected Path
```

Bewertungskriterien können sein:

```text
Latency
Bandwidth
Reliability
Congestion
Energy
Cost
Locality
```

## Interface Selection

NovaOS darf zwischen verfügbaren Interfaces wählen.

Beispiele:

```text
Ethernet
Wi-Fi
Cellular
Virtual Network
Cluster Fabric
Specialized Interconnect
```

Die Auswahl erfolgt nur innerhalb zulässiger Contracts und Policies.

## Adaptive Data Transfer

Transferstrategien dürfen an Netzwerkbedingungen angepasst werden.

Beispiele:

```text
Chunk Size
Parallel Transfers
Compression
Buffering
Batching
Transfer Scheduling
```

Die Semantik und Integrität der übertragenen Daten darf dadurch nicht verändert werden.

## Congestion

Adaptive Networking darf Netzwerküberlastung erkennen und darauf reagieren.

```text
Increasing Latency
      +
Packet Loss
      +
Queue Growth
      ↓
Congestion Detection
      ↓
Adaptation
```

Mögliche Reaktionen:

```text
Reduce Transfer Rate
Reschedule Transfer
Use Alternative Path
Change Transfer Strategy
Move Compute toward Data
```

## Compute-to-Data

Netzwerkmodelle können Execution Planning beeinflussen.

```text
Large Data Transfer
       ↓
High Network Cost
       ↓
Compute-to-Data Evaluation
       ↓
Execute near Data
```

Dadurch kann NovaOS unnötige Datenbewegungen vermeiden.

## Distributed Execution

Adaptive Networking wird in Distributed Execution integriert.

```text
Remote Provider
      +
Network Prediction
      +
Execution Cost
      ↓
Execution Planning
```

Ein schneller Remote Provider kann ungeeignet sein, wenn Netzwerkbedingungen den End-to-End-Contract verletzen würden.

## Hard Constraints

Adaptive Networking arbeitet ausschließlich innerhalb verbindlicher Anforderungen.

Dazu gehören insbesondere:

```text
Security
Data Sovereignty
Trust
Authorization
Hard Deadline
Required Network Isolation
Required Encryption
Resource Budget
Explicit User Constraints
```

Das Grundmodell lautet:

```text
Available Network Options
          ↓
Hard Constraint Filtering
          ↓
Valid Network Space
          ↓
Adaptive Optimization
```

## Data Sovereignty

Adaptive Routing oder Interface Selection darf Daten nicht über unzulässige Orte oder Trust Domains führen.

```text
Possible Route
      ↓
Sovereignty Check
      ↓
Trust / Security Check
      ↓
Allowed Route
```

Eine bessere Performance rechtfertigt niemals eine Verletzung von Data Sovereignty.

## Replanning

Ändert sich der Netzwerkzustand wesentlich, darf NovaOS neu planen.

```text
Current Path
     ↓
Network Degradation
     ↓
Replanning
     ↓
Alternative Valid Path
```

Migration, Retry oder erneute Übertragung sind nur zulässig, wenn die Semantik der Operation dies erlaubt.

Insbesondere gilt bei Remote Execution:

```text
Lost Connection
      ≠
Remote Execution Failed
```

## Energy Awareness

Adaptive Networking darf den Energieverbrauch von Netzwerkinterfaces berücksichtigen.

Beispiel:

```text
Wi-Fi
Ethernet
Cellular
Remote Transfer
```

Energieoptimierung bleibt ein Soft Requirement, sofern kein explizites Energy Budget etwas anderes verlangt.

## User Decisions

Explizite Nutzerentscheidungen besitzen Vorrang vor adaptiven Netzwerkpräferenzen, solange keine Hard Constraints verletzt werden.

Beispiele:

```text
Use Ethernet
Avoid Cellular
Disable Remote Networking
Prefer Local Execution
```

Adaptive Modelle dürfen solche Entscheidungen nicht selbstständig überschreiben.

## Deterministic Mode

Adaptive Netzwerkzustände dürfen deterministische Ausführungen nicht unkontrolliert beeinflussen.

Wenn Network Selection für Reproduzierbarkeit relevant ist, muss der verwendete Zustand:

```text
Fixed
Versioned
oder
Ignored
```

werden können.

## Observability

Adaptive Networking muss mit Network Observability korrelierbar sein.

Relevante Messwerte sind beispielsweise:

```text
Predicted Latency
Measured Latency
Bandwidth
Packet Loss
Transfer Size
Transfer Duration
Selected Path
Selected Interface
Congestion
Prediction Error
```

## Explainability

NovaOS soll relevante Netzwerkentscheidungen erklären können.

Beispiele:

```text
Warum wurde dieser Netzwerkpfad gewählt?

Warum wurde Remote Execution verworfen?

Warum wurde Compute zu den Daten verschoben?

Warum wurde ein Transfer verzögert?

Warum wurde das Netzwerkinterface gewechselt?
```

## Cold Start

Ohne historische Messwerte verwendet NovaOS:

```text
Interface Capabilities
Static Network Metrics
Declared Topology
Current Measurements
Safe Defaults
```

Adaptive Modelle sind keine Voraussetzung für korrektes Networking.

## Normative Anforderungen

1. NovaOS SOLL Adaptive Networking unterstützen.
2. Adaptive Netzwerkentscheidungen SOLLEN Prediction Error als Lernsignal verwenden.
3. Vorhergesagte und tatsächlich gemessene Netzwerkbedingungen MÜSSEN korrelierbar sein.
4. Latency, Bandwidth, Packet Loss und Congestion MÜSSEN als adaptive Signale verwendbar sein.
5. Mehrere Netzwerkpfade und Interfaces DÜRFEN adaptiv bewertet werden.
6. Adaptive Path Selection DARF nur innerhalb gültiger Hard Constraints erfolgen.
7. Transferstrategien DÜRFEN dynamisch an Netzwerkbedingungen angepasst werden.
8. Adaptive Optimierung DARF Datenintegrität und Operationssemantik nicht verändern.
9. Network Congestion MUSS als Zustandsänderung berücksichtigt werden können.
10. Compute-to-Data DARF anhand tatsächlicher Netzwerkbedingungen optimiert werden.
11. Distributed Execution MUSS End-to-End-Netzwerkkosten berücksichtigen können.
12. Data Sovereignty, Trust, Security und Authorization MÜSSEN Vorrang vor Netzwerkoptimierung besitzen.
13. Replanning DARF nur auf weiterhin contract-konforme Netzwerkpfade erfolgen.
14. Verbindungsverlust DARF nicht automatisch als fehlgeschlagene Remote Execution interpretiert werden.
15. Explizite Nutzerentscheidungen MÜSSEN Vorrang vor adaptiven Netzwerkpräferenzen besitzen.
16. Deterministic Mode DARF durch adaptive Netzwerkentscheidungen nicht unkontrolliert beeinflusst werden.
17. Adaptive Netzwerkentscheidungen MÜSSEN mit Observability und Decision Tracing korrelierbar sein.
18. Ohne adaptive Modelle MUSS korrektes Networking weiterhin möglich sein.

## Konsequenzen

### Positive Konsequenzen

- Netzwerkentscheidungen passen sich realen Bedingungen an,
- Distributed Execution kann End-to-End realistischer geplant werden,
- unnötige Datenbewegungen können reduziert werden,
- Congestion und wechselnde Netzwerkqualität können automatisch berücksichtigt werden,
- wiederholte Fehlprognosen verbessern zukünftige Entscheidungen.

### Negative Konsequenzen

- Netzwerkmodelle erzeugen zusätzlichen Systemzustand,
- häufig wechselnde Bedingungen können Vorhersagen schnell entwerten,
- Replanning und Path Selection erhöhen die Komplexität verteilter Ausführungen.

## Abhängigkeiten

- `ADR-ADAPTIVE-0001_Prediction_Error_als_zentrales_Lernsignal`
- `ADR-ADAPTIVE-0003_Explizite_Nutzerentscheidung_hat_Vorrang_vor_Adaptation`
- `ADR-ADAPTIVE-0004_Adaptive_Scheduling`
- `ADR-ADAPTIVE-0008_Adaptive_Power_Management`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-EXECUTION-0003_Latency_und_Deadline_Requirements`
- `ADR-EXECUTION-0005_Data_Sovereignty_im_ExecutionContract`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-DISTRIBUTED-0001_Distributed_Execution`
- `ADR-DISTRIBUTED-0009_Location_Transparent_Execution`
- `ADR-DISTRIBUTED-0010_Remote_Execution_unter_Sovereignty_und_Trust_Constraints`
- `ADR-OBSERVABILITY-0002_Distributed_Tracing`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`

## Zugehörige NPSPECs

- `NPSPEC-ADAPTIVE-NETWORKING-0001`
- `NPSPEC-ADAPTIVE-NETWORK-PREDICTION-0001`
- `NPSPEC-ADAPTIVE-PATH-SELECTION-0001`
- `NPSPEC-ADAPTIVE-NETWORK-REPLANNING-0001`
- `NPSPEC-ADAPTIVE-NETWORK-FEEDBACK-0001`

## Ergebnis

NovaOS erweitert Networking um einen kontrollierten adaptiven Feedback-Zyklus:

```text
Observe Network
      ↓
Predict
      ↓
Select Path / Strategy
      ↓
Transfer
      ↓
Measure
      ↓
Prediction Error
      ↓
Adapt
```

Der zentrale Grundsatz lautet:

```text
NovaOS darf Netzwerkwege
und Übertragungsstrategien
an reale Bedingungen anpassen.

Es lernt aus der Differenz
zwischen erwarteter und
tatsächlicher Netzwerkleistung.

Security, Sovereignty,
Korrektheit und Contracts
bleiben dabei unverhandelbar.
```