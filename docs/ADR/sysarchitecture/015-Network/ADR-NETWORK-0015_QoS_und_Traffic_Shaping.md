# ADR-NETWORK-0015 – QoS und Traffic Shaping

## Status

Angenommen

## Kategorie

Network / QoS / Traffic Shaping / Resource Economy / NetworkFlow

## Kontext

NovaOS modelliert Netzwerkkommunikation über `NetworkFlow` und semantische Flow Requirements. Unterschiedliche Flows besitzen dabei unterschiedliche Anforderungen an Latenz, Bandbreite, Jitter, Priorität oder Zuverlässigkeit.

Gleichzeitig sind Netzwerkressourcen begrenzt und werden häufig von mehreren Execution Domains und Flows gemeinsam genutzt.

NovaOS benötigt deshalb eine systemweite QoS- und Traffic-Shaping-Architektur, die wichtige Kommunikationsflüsse bevorzugen und Ressourcen kontrolliert verteilen kann, ohne Security, Hard Constraints oder Resource Budgets durch Priorisierung zu umgehen.

---

## Entscheidung

NovaOS integriert **Quality of Service (QoS) und Traffic Shaping als native Eigenschaften von `NetworkFlow` und der systemweiten Resource Economy**.

```text
NetworkFlow
     ↓
Flow Requirements
     ↓
QoS Classification
     ↓
Resource Budgets
     ↓
Traffic Scheduling / Shaping
     ↓
Network Path
```

QoS beschreibt die gewünschte beziehungsweise erforderliche Dienstqualität.

Traffic Shaping kontrolliert die tatsächliche Nutzung begrenzter Netzwerkressourcen.

Beide bleiben getrennte, aber koordinierte Mechanismen.

---

## Grundprinzip

```text
QoS expresses service requirements.

Traffic Shaping controls resource usage.
```

---

## Semantische QoS-Klassen

NovaOS verwendet semantische QoS-Eigenschaften anstelle einer ausschließlich numerischen globalen Priorität.

Geeignete Klassen können beispielsweise sein:

- Realtime,
- Latency Critical,
- Interactive,
- Normal,
- Background,
- Maintenance.

Die konkrete Definition und Abbildung auf Netzwerkmechanismen wird in NPSPECs festgelegt.

Anwendungen sollen primär ihre Kommunikationsanforderungen beschreiben und nicht hardware- oder protokollspezifische Queue-Werte festlegen müssen.

---

## QoS und Flow Requirements

QoS wird mit den semantischen Flow Requirements aus `ADR-NETWORK-0003` verbunden.

```text
NetworkFlow
 ├── Latency Requirement
 ├── Deadline
 ├── Throughput Requirement
 ├── Jitter Requirement
 └── QoS Class
```

Provider und Netzwerkpfade können diese Informationen zur Ressourcen- und Queue-Auswahl verwenden.

---

## Hard Constraints vor QoS

QoS darf keine übergeordneten Hard Constraints überschreiben.

```text
NetworkFlow
     ↓
Security / Sovereignty / Authority
     ↓
Hard Flow Constraints
     ↓
Resource Limits
     ↓
QoS Optimization
```

Ein hochpriorisierter Flow darf daher weder unzulässige Netzwerkpfade verwenden noch Sicherheits- oder Data-Sovereignty-Regeln umgehen.

---

## Traffic Shaping

Traffic Shaping kontrolliert die Ressourcennutzung eines Flows beziehungsweise einer Resource Domain.

Es kann insbesondere verwendet werden, um:

- Bandbreite zu begrenzen,
- Bursts zu kontrollieren,
- Fairness durchzusetzen,
- Ressourcen für andere Flows verfügbar zu halten,
- vereinbarte Budgets durchzusetzen.

Die konkreten Algorithmen und Queueing-Verfahren werden nicht durch diese ADR festgelegt.

---

## Hierarchische Ressourcensteuerung

Netzwerkressourcen werden hierarchisch kontrollierbar.

```text
System
  ↓
Execution Domain
  ↓
Task Group / Service
  ↓
NetworkFlow
```

Dadurch können sowohl globale als auch domänen- und flowspezifische Budgets und QoS-Policies umgesetzt werden.

Eine Execution Domain darf durch das Erzeugen vieler hochpriorisierter Flows ihre übergeordneten Limits nicht umgehen.

---

## Provider- und Hardwareintegration

Netzwerkprovider dürfen semantische NovaOS-QoS-Anforderungen auf verfügbare Mechanismen abbilden.

Dazu können beispielsweise gehören:

- Hardware Queues,
- Transport Queues,
- Interface Scheduling,
- Protokollmarkierungen,
- Provider-spezifische QoS-Funktionen.

```text
Semantic QoS
     ↓
Provider Mapping
     ↓
Available Mechanism
```

Fehlt eine entsprechende Hardwarefunktion, muss die Architektur weiterhin funktionieren, gegebenenfalls mit geringeren Garantien.

---

## Multipath und Routing

QoS-Anforderungen dürfen in Policy-Based Routing und Multipath Selection einfließen.

Ein Flow mit niedriger Latenzanforderung kann beispielsweise einen anderen zulässigen Netzwerkpfad bevorzugen als ein Background-Transfer.

QoS bleibt dabei ein Auswahlkriterium innerhalb der bereits durch Hard Constraints zugelassenen Pfade.

---

## Realtime

Realtime-Netzwerkkommunikation benötigt mehr als eine hohe Priorität.

Verbindliche Garantien setzen entsprechende Ressourcenreservierungen und geeignete Provider beziehungsweise Netzwerkpfade voraus.

```text
Realtime Requirement
       ↓
Feasibility
       ↓
Resource Reservation
       ↓
QoS Enforcement
```

NovaOS darf eine nicht vorhandene Realtime-Garantie nicht lediglich aufgrund einer QoS-Klasse behaupten.

---

## Backpressure

Traffic Shaping und QoS müssen mit systemweitem Backpressure zusammenarbeiten.

Wird ein Flow begrenzt, muss der daraus entstehende Druck soweit möglich bis zum verursachenden Producer propagiert werden.

Unbegrenztes Puffern ist kein Ersatz für Traffic Shaping oder Backpressure.

---

## Adaptive Optimierung

NovaOS darf Soft-QoS-Policies und Traffic-Verteilung anhand beobachteter Bedingungen adaptiv optimieren.

Prediction Error kann dabei zur Verbesserung zukünftiger Entscheidungen verwendet werden.

Adaptive Optimierung darf jedoch:

- keine Hard Constraints verändern,
- keine Resource Budgets umgehen,
- keine nicht vorhandenen Garantien erzeugen.

---

## Deterministic Mode

Im Deterministic Mode müssen QoS- und Shaping-Entscheidungen reproduzierbar kontrollierbar sein.

Adaptive Klassifizierung oder dynamische Optimierungen können dafür eingeschränkt beziehungsweise durch festgelegte Policies ersetzt werden.

---

## Introspection

QoS und Traffic Shaping müssen kontrolliert introspektierbar sein.

NovaOS soll insbesondere nachvollziehbar machen können:

```text
NetworkFlow
     ↓
QoS Class
     ↓
Resource Budget
     ↓
Applied Shaping
     ↓
Provider / Queue / Path
```

Damit können Latenzprobleme, Bandbreitenbegrenzungen und Prioritätsentscheidungen diagnostiziert werden.

---

## Normative Anforderungen

1. NovaOS MUSS QoS und Traffic Shaping als native Netzwerkfunktionen unterstützen.
2. QoS MUSS mit `NetworkFlow` und dessen Flow Requirements integrierbar sein.
3. QoS und Traffic Shaping MÜSSEN als getrennte, koordinierte Konzepte behandelt werden.
4. NovaOS SOLL semantische QoS-Klassen anstelle ausschließlich globaler numerischer Prioritäten verwenden.
5. Hard Constraints MÜSSEN vor QoS-Optimierungen ausgewertet werden.
6. QoS DARF Security-, Trust- oder Data-Sovereignty-Constraints NICHT umgehen.
7. Traffic Shaping MUSS Netzwerkressourcen pro Resource Context kontrollieren können.
8. Netzwerkbudgets MÜSSEN hierarchisch durchsetzbar sein.
9. Hohe Priorität DARF Resource Budgets NICHT automatisch aufheben.
10. Provider MÜSSEN semantische QoS-Anforderungen auf verfügbare Netzwerkmechanismen abbilden können.
11. QoS-Anforderungen MÜSSEN bei Routing- und Multipath-Entscheidungen berücksichtigt werden können.
12. Realtime-Garantien DÜRFEN nur bei tatsächlich vorhandenen Ressourcen- und Provider-Garantien zugesichert werden.
13. Traffic Shaping MUSS mit Backpressure kombinierbar sein.
14. Adaptive QoS-Optimierung DARF nur innerhalb zulässiger Policies und Budgets erfolgen.
15. QoS-, Budget- und Shaping-Entscheidungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- interaktive und zeitkritische Flows können gezielt bevorzugt werden,
- Netzwerkressourcen bleiben systemweit kontrollierbar,
- Anwendungen deklarieren Anforderungen statt hardwarespezifischer Queue-Parameter,
- QoS integriert sich in Routing, Multipath und Resource Economy,
- Realtime und normale Priorisierung bleiben klar getrennt.

### Negative Konsequenzen

- Provider müssen unterschiedliche QoS-Mechanismen auf gemeinsame Semantik abbilden,
- tatsächliche Garantien hängen teilweise von Netzwerk und Gegenstelle ab,
- hierarchisches Traffic Shaping erhöht Scheduling- und Accounting-Aufwand.

---

## Verworfene Alternativen

### Eine globale numerische Netzwerkpriorität

Verworfen.

Eine einzelne Zahl beschreibt weder die semantischen Anforderungen eines Flows noch dessen Ressourcen- und Realtime-Eigenschaften ausreichend.

### QoS ohne Resource Budgets

Verworfen.

Priorität ohne Ressourcenbegrenzung könnte anderen Flows Ressourcen entziehen und die systemweite Resource Economy umgehen.

### Traffic Shaping ausschließlich im Netzwerktreiber

Verworfen.

Damit wären Execution Domains, `NetworkFlow`, Flow Requirements und systemweite Ressourcenpolitik nicht ausreichend berücksichtigt.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-NETWORK-0003_Transportabstraktion_über_semantische_Flow_Requirements`
- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0007_Multipath_Networking`
- `ADR-NETWORK-0008_Policy_Based_Routing`
- `ADR-NETWORK-0013_Virtual_Networks_und_Network_Namespaces`
- `ADR-NETWORK-0014_Native_Overlay_VPN_Architektur`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-IPC-0006_Backpressure_als_systemweite_IPC_Eigenschaft`
- `ADR-IO-0003_QoS_und_Priority_Aware_IO`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-QOS-0001`
- `NPSPEC-NETWORK-QOS-CLASS-0001`
- `NPSPEC-NETWORK-TRAFFIC-SHAPING-0001`
- `NPSPEC-NETWORK-BANDWIDTH-BUDGET-0001`
- `NPSPEC-NETWORK-QOS-PROVIDER-0001`
- `NPSPEC-NETWORK-QOS-REALTIME-0001`
- `NPSPEC-NETWORK-QOS-TEST-0001`

---

## Ergebnis

NovaOS verbindet semantische Kommunikationsanforderungen mit kontrollierter Netzwerkressourcennutzung:

```text
NetworkFlow
     ↓
Semantic QoS
     ↓
Resource Budget
     ↓
Traffic Scheduling / Shaping
     ↓
Provider / Network Path
```

Damit werden Netzwerkpriorisierung und Bandbreitensteuerung Bestandteil der systemweiten Resource Economy, ohne Hard Constraints, Sicherheitsgrenzen oder tatsächliche Ressourcengarantien zu verwischen.