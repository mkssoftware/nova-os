# ADR-IPC-0006 – Backpressure als systemweite IPC-Eigenschaft

## Status

Angenommen

## Kategorie

IPC / Backpressure / Flow Control / Resource Economy / Structured Concurrency

## Kontext

Asynchrone IPC erlaubt es Produzenten, Arbeit schneller zu erzeugen, als ein Empfänger sie verarbeiten kann.

Ohne kontrollierte Rückwirkung können dadurch:

- IPC-Queues wachsen,
- Speicher gebunden werden,
- Latenzen steigen,
- Resource Budgets überschritten werden,
- nachgelagerte Services überlastet werden.

Lokale Einzelmechanismen zur Queue-Begrenzung reichen nicht aus, wenn Kommunikation Teil längerer Service- und Datenpipelines ist.

NovaOS benötigt Backpressure deshalb als gemeinsame Eigenschaft seiner IPC-Architektur.

---

## Entscheidung

NovaOS behandelt **Backpressure als systemweite IPC-Eigenschaft**.

Ein Empfänger oder nachgelagerter Verarbeitungspfad muss Überlastung kontrolliert an vorgelagerte Produzenten zurückmelden können.

```text
Producer
   ↓
IPC
   ↓
Consumer
   ↓
Capacity Pressure
   ↑
Backpressure
```

Backpressure wird mit IPC Contracts, Structured Concurrency und der systemweiten Resource Economy integriert.

---

## Grundprinzip

```text
Production must respect
downstream capacity.
```

---

## Explizite Kapazität

IPC-Ressourcen werden nicht als unbegrenzt betrachtet.

Endpunkte und Kommunikationspfade können begrenzte Kapazitäten besitzen, beispielsweise für:

- ausstehende Requests,
- Nachrichten,
- Buffer,
- Speicher,
- Bandbreite,
- Verarbeitungskapazität.

Die konkrete Kapazitätssteuerung wird in den NPSPECs definiert.

---

## Backpressure Propagation

Backpressure soll über mehrere IPC-Stufen propagierbar sein.

```text
Producer
   ↓
Service A
   ↓
Service B
   ↓
Consumer
   ↑
Pressure
   ↑
Pressure
   ↑
Pressure
```

Dadurch wird Überlastung möglichst an der Quelle begrenzt, anstatt ausschließlich am letzten überlasteten Systembestandteil behandelt zu werden.

---

## Reaktion auf Backpressure

Abhängig vom Contract kann Backpressure zu kontrollierten Reaktionen führen, beispielsweise:

- Warten,
- Drosseln,
- Ablehnen,
- Priorisieren,
- expliziter Degradation.

Daten dürfen nicht stillschweigend verworfen werden, sofern der Contract dies nicht ausdrücklich erlaubt.

---

## Resource Economy

Backpressure ist mit der systemweiten Resource Economy verbunden.

IPC-Verbrauch bleibt dem verursachenden Resource Context zurechenbar.

```text
IPC Demand
    ↓
Resource Budget
    ↓
Available Capacity
    ↓
Accept / Backpressure
```

Hohe Priority oder QoS darf Backpressure nicht automatisch außer Kraft setzen.

---

## Structured Concurrency

Backpressure muss mit Structured Concurrency, Cancellation und Deadlines zusammenarbeiten.

Ein wartender Producer darf nicht unbegrenzt blockiert bleiben, wenn:

- sein Scope beendet wird,
- Cancellation erfolgt,
- eine Deadline abläuft.

Damit bleibt Backpressure Bestandteil des kontrollierten Lebenszyklus eines Calls oder Tasks.

---

## QoS und Priorität

Backpressure darf QoS- und Prioritätsinformationen berücksichtigen.

Höher priorisierte Kommunikation kann bevorzugt behandelt werden, sofern dadurch keine Hard Constraints oder garantierten Ressourcen anderer Domains verletzt werden.

Priority bedeutet dabei nicht unbegrenzte Queue- oder Verarbeitungskapazität.

---

## Lokale und entfernte IPC

Backpressure gilt unabhängig davon, ob ein Provider lokal oder entfernt ausgeführt wird.

```text
Local IPC
Remote IPC
Service Pipeline
```

Der konkrete Mechanismus kann transportabhängig sein, die logische Backpressure-Semantik bleibt jedoch Bestandteil des Contracts.

---

## Zero-Copy und Shared Buffers

Backpressure ist auch für Zero-Copy IPC erforderlich.

Gemeinsam genutzte Buffer bleiben Ressourcen und können durch langsame Consumer lange gebunden werden.

Buffer-Lifetime, Ownership und Wiederverwendung müssen deshalb in die Backpressure- und Ressourcensteuerung einbezogen werden.

---

## Normative Anforderungen

1. NovaOS MUSS Backpressure als systemweite IPC-Eigenschaft unterstützen.
2. IPC-Kapazität DARF NICHT grundsätzlich als unbegrenzt angenommen werden.
3. Empfänger MÜSSEN Überlastung kontrolliert signalisieren können.
4. Backpressure MUSS über geeignete IPC- und Serviceketten propagierbar sein.
5. IPC Contracts MÜSSEN die relevante Backpressure-Semantik ausdrücken können.
6. Überlastung DARF NICHT standardmäßig zu unkontrolliertem Queue-Wachstum führen.
7. Daten DÜRFEN ohne entsprechende Contract-Semantik NICHT stillschweigend verworfen werden.
8. Backpressure MUSS mit der systemweiten Resource Economy integrierbar sein.
9. Priority und QoS DÜRFEN Resource Budgets und Hard Constraints NICHT umgehen.
10. Backpressure MUSS mit Cancellation und Deadlines zusammenarbeiten.
11. Zero-Copy- und Shared-Buffer-Ressourcen MÜSSEN bei Backpressure berücksichtigt werden.
12. Backpressure MUSS sowohl für lokale als auch für entfernte IPC verwendbar sein.
13. Reaktionen auf Backpressure MÜSSEN kontrolliert und contract-basiert erfolgen.
14. IPC-Auslastung und relevante Backpressure-Zustände MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- verhindert unkontrolliertes Queue-Wachstum,
- schützt Services vor Überlastung,
- begrenzt Speicher- und Bufferbindung,
- integriert IPC in die Resource Economy,
- ermöglicht stabile mehrstufige Daten- und Servicepipelines,
- Überlastung kann bis zum verursachenden Producer zurückwirken.

### Negative Konsequenzen

- Producer müssen mit begrenzter Downstream-Kapazität umgehen können,
- Backpressure-Propagation erhöht die Contract-Komplexität,
- Priorisierung und Resource Budgets müssen konsistent zusammenspielen.

---

## Verworfene Alternativen

### Unbegrenzte IPC-Queues

Verworfen.

Sie verschieben Überlastung lediglich in Speicherverbrauch und steigende Latenz.

### Backpressure ausschließlich innerhalb einzelner Provider

Verworfen.

Überlastung in mehrstufigen Pipelines muss über Providergrenzen hinweg propagierbar sein.

### Nachrichten bei Überlastung grundsätzlich verwerfen

Verworfen.

Datenverlust darf nur Bestandteil einer expliziten Contract-Semantik sein.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-IPC-0001_Typed_IPC`
- `ADR-IPC-0004_Unified_Call_Abstraction`
- `ADR-IPC-0005_Lokale_und_entfernte_Aufrufe_unter_einem_Contract`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0009_Unified_Data_Object_Pipelining`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-IO-0003_QoS_und_Priority_Aware_IO`
- `ADR-IO-0006_IO_Resource_Budgets`

---

## Zugehörige NPSPECs

- `NPSPEC-IPC-BACKPRESSURE-0001`
- `NPSPEC-IPC-CAPACITY-0001`
- `NPSPEC-IPC-FLOW-CONTROL-0001`
- `NPSPEC-IPC-BACKPRESSURE-PROPAGATION-0001`
- `NPSPEC-IPC-BACKPRESSURE-RESOURCE-0001`
- `NPSPEC-IPC-BACKPRESSURE-TEST-0001`

---

## Ergebnis

NovaOS behandelt Überlastung nicht als isoliertes Queue-Problem, sondern als propagierbare Eigenschaft des gesamten IPC-Pfads:

```text
Producer
   ↓
IPC / Services
   ↓
Consumer
   ↑
Backpressure
   ↑
Resource-aware Flow Control
```

Dadurch passt sich die erzeugte Arbeit kontrolliert an die tatsächlich verfügbare Verarbeitungskapazität nachgelagerter Systemkomponenten an.