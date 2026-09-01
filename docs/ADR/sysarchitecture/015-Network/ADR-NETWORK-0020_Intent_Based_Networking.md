# ADR-NETWORK-0020 – Intent-Based Networking

## Status

Angenommen

## Kategorie

Network / Intent-Based Networking / Declarative System Model / NetworkFlow / Policy

## Kontext

NovaOS verwendet mit `NetworkFlow`, semantischen Flow Requirements, Policy-Based Routing, Service Discovery, QoS, Multipath und Connection Migration bereits eine Netzwerkarchitektur, bei der Anwendungen nicht jede technische Netzwerkentscheidung selbst treffen müssen.

Eine Anwendung soll beispielsweise ausdrücken können, dass sie eine sichere, latenzarme Verbindung zu einem bestimmten Service benötigt, ohne selbst Interface, IP-Version, Transport, Route, VPN, Multipath-Strategie oder Congestion-Control-Verfahren festlegen zu müssen.

Diese Architektur wird durch ein übergeordnetes Intent-Based-Networking-Modell vereinheitlicht.

---

## Entscheidung

NovaOS führt **Intent-Based Networking als native deklarative Steuerungsebene der Netzwerkarchitektur** ein.

Anwendungen und Systemdienste beschreiben primär das gewünschte Kommunikationsziel und dessen Constraints.

NovaOS plant daraus eine zulässige konkrete Netzwerkkonfiguration.

```text
Network Intent
      ↓
Constraints
      ↓
Policy Resolution
      ↓
Planning
      ↓
NetworkFlow
      ↓
Transport / Route / Provider
      ↓
Actual Network State
      ↓
Verification
```

Intent beschreibt **was** erreicht werden soll.

Die Netzwerkarchitektur entscheidet **wie** dieses Ziel innerhalb der geltenden Constraints umgesetzt wird.

---

## Grundprinzip

```text
Declare the required outcome,
not the network implementation.
```

---

## Network Intent

Ein Network Intent beschreibt die semantischen Anforderungen einer Kommunikation.

Dazu können insbesondere gehören:

- Ziel-Service oder Network Identity,
- Security Requirements,
- Trust Requirements,
- Data-Sovereignty-Anforderungen,
- Reliability,
- Latency,
- Deadline,
- Throughput,
- Jitter,
- Location Constraints,
- Resource Budget,
- Connectivity Requirements.

Die konkrete Repräsentation wird in NPSPECs definiert.

---

## Integration mit NetworkFlow

Ein Network Intent wird in einen oder mehrere konkrete `NetworkFlow`-Contracts überführt.

```text
Network Intent
      ↓
Planning
      ↓
NetworkFlow Contract
      ↓
Execution
```

`NetworkFlow` bleibt damit die operative Kommunikationsabstraktion.

Intent-Based Networking bildet die deklarative Planungsebene darüber.

---

## Declarative System Model

Intent-Based Networking folgt direkt dem Declarative System Model von NovaOS.

```text
Desired Network State
        ↓
Constraints
        ↓
Policies
        ↓
Planning
        ↓
Execution
        ↓
Actual Network State
        ↓
Verification
        ↓
Reconciliation
```

Netzwerkzustand wird damit nicht ausschließlich durch imperative Einzeloperationen erzeugt, sondern kann aus einem gewünschten Zustand abgeleitet und kontinuierlich überprüft werden.

---

## Hard Constraints und Optimierung

Network Intents unterscheiden verbindliche Anforderungen von Optimierungszielen.

```text
Network Intent
      ↓
Hard Constraints
      ↓
Feasible Solutions
      ↓
Soft Preferences
      ↓
Optimization
      ↓
Selected Plan
```

Hard Constraints können beispielsweise umfassen:

- Security,
- Authority,
- Data Sovereignty,
- erforderliches VPN,
- erforderliche Trust-Eigenschaften,
- Determinism,
- verbindliche Realtime-Anforderungen.

Erst innerhalb der zulässigen Lösungsmenge dürfen Performance, Energie, Kosten oder andere Soft Preferences optimiert werden.

---

## Automatische Netzwerkauswahl

NovaOS darf aus einem Intent automatisch geeignete Mechanismen auswählen.

Dazu gehören beispielsweise:

```text
IP Provider
Transport Provider
Route
Network Interface
Multipath Strategy
Overlay / VPN
QoS Policy
Congestion Control
```

Anwendungen müssen diese Entscheidungen nicht selbst treffen.

Sie dürfen jedoch konkrete Mechanismen bevorzugen oder explizit verlangen, sofern dies mit den geltenden Hard Constraints vereinbar ist.

---

## Dynamische Reconciliation

Ein Network Intent bleibt während der Lebensdauer einer Kommunikation relevant.

Ändert sich die Netzwerkumgebung, darf NovaOS prüfen, ob der aktuelle Zustand den Intent weiterhin erfüllt.

```text
Intent
  ↓
Current Network State
  ↓
Still compliant?
 ├── Yes → Continue
 └── No  → Replan
```

Replanning kann beispielsweise:

- Route Selection,
- Connection Migration,
- Multipath,
- Provider Selection,
- VPN-Auswahl

anpassen.

Die logische Kommunikationsabsicht bleibt dabei erhalten.

---

## Service Discovery

Network Intents können logische Services statt konkreter Netzwerkadressen referenzieren.

```text
Intent
  ↓
Service Discovery
  ↓
Eligible Service Instance
  ↓
NetworkFlow
```

Damit können Service Discovery und Intent-Based Networking gemeinsam Location Transparency ermöglichen.

---

## Adaptive Optimierung

NovaOS darf innerhalb zulässiger Lösungsräume adaptive Verfahren zur Verbesserung von Netzwerkentscheidungen verwenden.

```text
Predicted Outcome
       ↓
Network Decision
       ↓
Observed Outcome
       ↓
Prediction Error
       ↓
Policy Improvement
```

Prediction Error darf ausschließlich Soft Policies und Optimierungsmodelle beeinflussen.

Hard Constraints bleiben unveränderlich.

---

## Graceful Degradation

Kann ein Intent nicht vollständig erfüllt werden, darf NovaOS nur dann eine schwächere Lösung verwenden, wenn der Contract dies ausdrücklich erlaubt.

```text
Intent
  ↓
No Exact Solution
  ↓
Permitted Degradation?
 ├── Yes → Explicit Degraded Plan
 └── No  → Fail / Wait / Replan
```

Eine stille Abschwächung von Security-, Trust- oder Data-Sovereignty-Anforderungen ist unzulässig.

---

## Deterministic Mode

Im Deterministic Mode muss die Umsetzung eines Network Intent kontrollierbar und reproduzierbar sein.

Adaptive oder dynamische Auswahlmechanismen können dafür eingeschränkt oder durch festgelegte Policies ersetzt werden.

Der Intent selbst bleibt unverändert.

---

## Introspection

Intent-Based Networking muss kontrolliert introspektierbar sein.

NovaOS soll die Beziehung darstellen können:

```text
Network Intent
      ↓
Resolved Constraints
      ↓
Selected Plan
      ↓
NetworkFlow
      ↓
Transport / Route / Provider
      ↓
Actual State
```

Damit kann nachvollzogen werden, warum NovaOS eine bestimmte Netzwerkentscheidung getroffen hat.

---

## Normative Anforderungen

1. NovaOS MUSS Intent-Based Networking als native deklarative Netzwerksteuerung unterstützen.
2. Network Intents MÜSSEN gewünschte Kommunikationsziele und Constraints beschreiben können.
3. Network Intents SOLLEN unabhängig von konkreten Interfaces, Adressen, Routen und Transportprotokollen formulierbar sein.
4. Network Intents MÜSSEN in konkrete `NetworkFlow`-Contracts überführbar sein.
5. Hard Constraints MÜSSEN vor Soft Preferences und Optimierungszielen ausgewertet werden.
6. NovaOS DARF geeignete Netzwerkmechanismen automatisch auswählen.
7. Anwendungen MÜSSEN Mechanismen bevorzugen oder explizit verlangen können, sofern Hard Constraints dies zulassen.
8. Intent-Based Networking MUSS mit Service Discovery und Policy-Based Routing integrierbar sein.
9. Intent-Based Networking MUSS Multipath, Overlay/VPN und Connection Migration berücksichtigen können.
10. Der tatsächliche Netzwerkzustand MUSS gegen relevante Intents überprüfbar sein.
11. NovaOS MUSS bei relevanten Zustandsänderungen ein Replanning ermöglichen können.
12. Adaptive Optimierung DARF ausschließlich innerhalb zulässiger Lösungsräume erfolgen.
13. Prediction Error DARF Hard Constraints NICHT verändern.
14. Eine Abschwächung verbindlicher Anforderungen MUSS explizit durch den Contract erlaubt sein.
15. Intent, Planung, ausgewählte Mechanismen und tatsächlicher Zustand MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Anwendungen beschreiben Kommunikationsziele statt Netzwerkimplementierungen,
- NovaOS kann Transport, Route, Interface, VPN und andere Mechanismen gemeinsam optimieren,
- Netzwerkentscheidungen können auf Änderungen der Umgebung reagieren,
- Location Transparency wird konsequent umgesetzt,
- Netzwerkverhalten integriert sich direkt in das Declarative System Model,
- zukünftige Netzwerkmechanismen können eingeführt werden, ohne Anwendungscontracts grundlegend zu verändern.

### Negative Konsequenzen

- die Planungs- und Policy-Ebene wird komplexer,
- Konflikte zwischen Intents und Policies müssen eindeutig aufgelöst werden,
- gute Introspection ist notwendig, damit automatische Entscheidungen nachvollziehbar bleiben.

---

## Verworfene Alternativen

### Ausschließlich imperative Netzwerk-APIs

Verworfen.

Anwendungen müssten dadurch technische Netzwerkdetails selbst auswählen und könnten nur schwer auf dynamische Netzwerkbedingungen reagieren.

### Vollständig autonome Netzwerkoptimierung ohne Contracts

Verworfen.

Automatische Entscheidungen benötigen klare Constraints und dürfen Security, Sovereignty oder andere Hard Requirements nicht eigenständig verändern.

### Network Intent als Ersatz für NetworkFlow

Verworfen.

Intent beschreibt den gewünschten Zustand; `NetworkFlow` bleibt die operative Kommunikationsabstraktion.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0003_Declarative_System_Model`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-NETWORK-0003_Transportabstraktion_über_semantische_Flow_Requirements`
- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0007_Multipath_Networking`
- `ADR-NETWORK-0008_Policy_Based_Routing`
- `ADR-NETWORK-0012_Service_Discovery`
- `ADR-NETWORK-0014_Native_Overlay_VPN_Architektur`
- `ADR-NETWORK-0015_QoS_und_Traffic_Shaping`
- `ADR-NETWORK-0016_Adaptive_Congestion_Control`
- `ADR-NETWORK-0019_Connection_Migration`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-INTENT-0001`
- `NPSPEC-NETWORK-INTENT-CONTRACT-0001`
- `NPSPEC-NETWORK-INTENT-PLANNER-0001`
- `NPSPEC-NETWORK-INTENT-POLICY-0001`
- `NPSPEC-NETWORK-INTENT-RECONCILIATION-0001`
- `NPSPEC-NETWORK-INTENT-INTROSPECTION-0001`
- `NPSPEC-NETWORK-INTENT-TEST-0001`

---

## Ergebnis

NovaOS erweitert seine Netzwerkarchitektur um eine deklarative Intent-Ebene:

```text
Application / System Service
          ↓
     Network Intent
          ↓
Constraints + Policies
          ↓
       Planning
          ↓
      NetworkFlow
          ↓
Transport / Route / Overlay / QoS
          ↓
   Actual Network State
          ↓
 Verification / Reconciliation
```

Damit beschreiben Anwendungen primär das gewünschte Kommunikationsergebnis, während NovaOS die konkrete technische Umsetzung plant, überwacht und bei Bedarf innerhalb der erlaubten Constraints anpasst.