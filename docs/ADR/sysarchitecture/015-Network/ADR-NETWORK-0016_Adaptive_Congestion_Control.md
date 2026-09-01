# ADR-NETWORK-0016 – Adaptive Congestion Control

## Status

Angenommen

## Kategorie

Network / Congestion Control / Adaptive Systems / Transport / NetworkFlow

## Kontext

Netzwerkpfade unterscheiden sich erheblich hinsichtlich Bandbreite, Latenz, Verlust, Jitter, Queueing und zeitlicher Veränderung. Ein statisches Congestion-Control-Verfahren kann deshalb nicht für alle Netzwerkbedingungen und Workloads optimal sein.

NovaOS besitzt mit `NetworkFlow`, semantischen Flow Requirements, Transportprovidern, Multipath Networking und QoS bereits den notwendigen Kontext, um Congestion Control gezielt an Kommunikationsanforderungen und beobachtete Netzwerkbedingungen anzupassen.

Congestion Control bleibt dabei eine Transport- und Netzwerkoptimierung. Sie darf weder Sicherheitsanforderungen noch Resource Budgets oder andere Hard Constraints verändern.

---

## Entscheidung

NovaOS unterstützt **Adaptive Congestion Control als native Eigenschaft der Transport- und NetworkFlow-Architektur**.

```text
NetworkFlow
     ↓
Flow Requirements
     ↓
Transport Provider
     ↓
Congestion-Control Policy
     ↓
Observed Network State
     ↓
Adaptive Control
```

Transportprovider können unterschiedliche Congestion-Control-Verfahren bereitstellen.

NovaOS darf automatisch ein geeignetes Verfahren beziehungsweise dessen zulässige Parameter auswählen und während eines Flows an veränderte Bedingungen anpassen.

---

## Grundprinzip

```text
Observe
   ↓
Predict
   ↓
Control
   ↓
Measure Result
   ↓
Prediction Error
   ↓
Adapt
```

Adaptive Congestion Control optimiert innerhalb festgelegter Contracts und Grenzen.

---

## Trennung von Mechanismus und Policy

Konkrete Congestion-Control-Algorithmen sind Mechanismen.

Die Entscheidung, welcher Mechanismus für einen Flow geeignet ist, wird als Policy behandelt.

```text
Congestion-Control Policy
          ↓
Eligible Algorithms
          ↓
Selection
          ↓
Transport Mechanism
```

Dadurch kann NovaOS neue Verfahren integrieren, ohne die `NetworkFlow`-Abstraktion zu verändern.

---

## Algorithmusauswahl

Die Auswahl folgt dem allgemeinen NovaOS-Modell für algorithmische APIs.

Standardmäßig darf das System automatisch einen geeigneten Algorithmus auswählen.

```text
Automatic
→ NovaOS selects algorithm

Preferred Algorithm
→ Prefer selected algorithm

Required Algorithm
→ Use algorithm if permitted
```

Eine explizite Auswahl durch Anwendungen oder Systemkomponenten bleibt möglich.

Hard Constraints dürfen dadurch nicht verletzt werden.

---

## Flow Requirements

Congestion Control berücksichtigt die semantischen Anforderungen des jeweiligen `NetworkFlow`.

Relevant können insbesondere sein:

- Latency,
- Throughput,
- Jitter,
- Deadline,
- Loss Tolerance,
- QoS Class,
- Resource Budget.

Ein interaktiver Flow kann dadurch andere Optimierungsziele besitzen als ein großer Background-Transfer.

---

## Adaptive Beobachtung

NovaOS darf aktuelle Netzwerkbedingungen beobachten und für Congestion-Control-Entscheidungen verwenden.

Dazu können beispielsweise gehören:

- Round-Trip Time,
- Verlust,
- Durchsatz,
- Queueing-Verhalten,
- verfügbare Kapazität,
- Pfadänderungen.

Die konkrete Messung und Regelung wird in NPSPECs definiert.

---

## Prediction Error

Adaptive Congestion Control integriert sich in das allgemeine NovaOS-Lernmodell.

```text
Predicted Network Behavior
          ↓
Control Decision
          ↓
Observed Result
          ↓
Prediction Error
          ↓
Policy Correction
```

Wiederholte Abweichungen können zur Anpassung zukünftiger Soft-Policy-Entscheidungen führen.

Prediction Error darf niemals Hard Constraints verändern.

---

## Multipath

Bei Multipath Networking können Netzwerkpfade unterschiedliche Congestion-Eigenschaften besitzen.

```text
NetworkFlow
   ├── Path A → Congestion State A
   └── Path B → Congestion State B
```

Congestion Control muss daher mit Multipath- und Path-Selection-Mechanismen koordinierbar sein.

Die Optimierung eines einzelnen Pfads darf nicht blind die Eigenschaften des gesamten Flows ignorieren.

---

## QoS und Resource Economy

Congestion Control muss mit QoS, Traffic Shaping und Resource Budgets zusammenarbeiten.

```text
Flow Requirements
       ↓
Resource Budget
       ↓
QoS / Shaping
       ↓
Congestion Control
```

Congestion Control darf keine Bandbreitenbudgets oder Traffic-Shaping-Grenzen umgehen.

Eine hohe QoS-Klasse bedeutet ebenfalls nicht, dass ein Flow beliebig aggressiv Netzwerkressourcen beanspruchen darf.

---

## Fairness

Adaptive Optimierung darf andere Flows nicht systematisch verdrängen.

Fairness ist deshalb Bestandteil der Congestion-Control-Policy und der systemweiten Resource Economy.

Die konkrete Fairness-Definition kann abhängig von Netzwerk, Provider und Resource Domain variieren.

---

## Deterministic Mode

Im Deterministic Mode muss NovaOS adaptive Congestion-Control-Entscheidungen kontrollieren können.

Je nach Contract können:

- ein festgelegter Algorithmus,
- definierte Parameter,
- eingeschränkte Adaption

verwendet werden.

Nicht deterministische Lernentscheidungen dürfen keine erforderliche Reproduzierbarkeit verletzen.

---

## Fail-Safe Verhalten

Adaptive Congestion Control darf niemals Voraussetzung für grundlegende Netzwerkkorrektheit sein.

Falls adaptive Modelle oder Messdaten nicht verfügbar beziehungsweise nicht vertrauenswürdig sind, muss ein definierter nicht-adaptiver beziehungsweise konservativer Mechanismus verwendet werden können.

```text
Adaptive Policy unavailable
          ↓
Safe Congestion-Control Policy
```

---

## Introspection

Congestion-Control-Entscheidungen müssen kontrolliert introspektierbar sein.

NovaOS soll insbesondere darstellen können:

```text
NetworkFlow
     ↓
Transport Provider
     ↓
Congestion-Control Algorithm
     ↓
Relevant Network State
     ↓
Adaptive Decision
```

Damit können Performanceprobleme und adaptive Entscheidungen nachvollzogen werden.

---

## Normative Anforderungen

1. NovaOS MUSS adaptive Congestion-Control-Verfahren unterstützen können.
2. Congestion Control MUSS mit `NetworkFlow` und Transportprovidern integrierbar sein.
3. Konkrete Congestion-Control-Algorithmen MÜSSEN von der Auswahl-Policy getrennt sein.
4. NovaOS MUSS unterschiedliche Congestion-Control-Algorithmen integrieren können.
5. NovaOS DARF standardmäßig automatisch einen geeigneten Algorithmus auswählen.
6. Anwendungen MÜSSEN einen Algorithmus bevorzugen oder explizit anfordern können.
7. Hard Constraints MÜSSEN vor algorithmischen Optimierungen ausgewertet werden.
8. Flow Requirements MÜSSEN Congestion-Control-Entscheidungen beeinflussen können.
9. Congestion Control MUSS mit Multipath Networking koordinierbar sein.
10. Congestion Control MUSS QoS, Traffic Shaping und Resource Budgets respektieren.
11. Adaptive Optimierung DARF Resource Budgets NICHT umgehen.
12. Prediction Error DARF zur Anpassung von Soft Policies verwendet werden.
13. Adaptive Entscheidungen DÜRFEN Hard Constraints NICHT verändern.
14. Deterministic Mode MUSS adaptive Congestion-Control-Entscheidungen kontrollieren können.
15. Bei Ausfall adaptiver Mechanismen MUSS ein sicherer nicht-adaptiver beziehungsweise konservativer Fallback möglich sein.
16. Verwendeter Algorithmus und relevante adaptive Entscheidungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Congestion Control kann an unterschiedliche Workloads und Netzwerkbedingungen angepasst werden,
- neue Algorithmen können als Mechanismen ergänzt werden,
- Flow Requirements beeinflussen die Optimierungsstrategie,
- Multipath, QoS und Resource Economy können gemeinsam berücksichtigt werden,
- Prediction Error ermöglicht kontinuierliche Verbesserung,
- Anwendungen behalten explizite Kontrolle, wenn diese benötigt wird.

### Negative Konsequenzen

- adaptive Auswahl erhöht die Komplexität der Transportprovider,
- Messungen und Modellierung verursachen zusätzlichen Aufwand,
- wechselnde Algorithmen und Parameter erschweren Performanceanalyse ohne ausreichende Introspection.

---

## Verworfene Alternativen

### Ein systemweit festgelegter Congestion-Control-Algorithmus

Verworfen.

Unterschiedliche Netzwerkpfade und Workloads besitzen unterschiedliche Anforderungen und Eigenschaften.

### Vollständig autonome KI-gesteuerte Congestion Control

Verworfen.

Adaptive beziehungsweise lernende Verfahren dürfen keine Voraussetzung für Korrektheit und Stabilität der Netzwerkkommunikation sein.

### Congestion Control unabhängig von QoS und Resource Economy

Verworfen.

Dadurch könnten lokale Transportoptimierungen systemweite Ressourcen- und Fairnessziele verletzen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-NETWORK-0003_Transportabstraktion_über_semantische_Flow_Requirements`
- `ADR-NETWORK-0004_TCP_UDP_QUIC_als_Transportprovider`
- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0007_Multipath_Networking`
- `ADR-NETWORK-0008_Policy_Based_Routing`
- `ADR-NETWORK-0015_QoS_und_Traffic_Shaping`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0020_AI_nicht_Kernel_Correctness_Voraussetzung`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-CONGESTION-0001`
- `NPSPEC-NETWORK-CONGESTION-PROVIDER-0001`
- `NPSPEC-NETWORK-CONGESTION-SELECTION-0001`
- `NPSPEC-NETWORK-CONGESTION-ADAPTIVE-0001`
- `NPSPEC-NETWORK-CONGESTION-MULTIPATH-0001`
- `NPSPEC-NETWORK-CONGESTION-TEST-0001`

---

## Ergebnis

NovaOS behandelt Congestion Control als adaptive, policygesteuerte Transportfunktion:

```text
NetworkFlow
     ↓
Flow Requirements
     ↓
Eligible Congestion Algorithms
     ↓
Policy Selection
     ↓
Network Feedback
     ↺
Adaptive Correction
```

Damit kann NovaOS Congestion Control an Netzwerkpfad und Workload anpassen, während Hard Constraints, Resource Budgets und die grundlegende Netzwerkkorrektheit unabhängig von adaptiven Entscheidungen bleiben.