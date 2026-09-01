# ADR-NETWORK-0003 – Transportabstraktion über semantische Flow Requirements

## Status

Angenommen

## Kategorie

Network / Transport / Semantic Requirements / Execution Contracts / Policy

## Kontext

Anwendungen wählen Netzwerktransporte traditionell häufig direkt über konkrete Protokolle wie TCP oder UDP.

Dadurch werden Anforderungen wie Zuverlässigkeit, Reihenfolge, Latenz oder Verlusttoleranz mit einer konkreten Transportimplementierung gekoppelt.

NovaOS verfolgt dagegen das Prinzip, dass Anwendungen möglichst beschreiben sollen, **welche Eigenschaften ein Datenfluss benötigt**, während das System einen geeigneten Mechanismus auswählen kann.

Dieses Modell entspricht den systemweiten Explicit Contracts und der automatischen Algorithmus- und Provider-Auswahl von NovaOS.

---

## Entscheidung

NovaOS führt eine **Transportabstraktion über semantische Flow Requirements** ein.

Anwendungen können Netzwerkkommunikation anhand ihrer benötigten Eigenschaften beschreiben, anstatt zwingend ein konkretes Transportprotokoll festzulegen.

```text
Application
    ↓
Flow Requirements
    ↓
Hard Constraints
    ↓
Eligible Transports
    ↓
Policy / Optimization
    ↓
Selected Transport Provider
```

TCP, UDP und zukünftige Transportprotokolle werden als mögliche Provider dieser Anforderungen behandelt.

Eine explizite Auswahl eines Transportprotokolls bleibt möglich.

---

## Grundprinzip

```text
Describe communication requirements,
not implementation mechanisms.
```

---

## Flow Requirements

Ein Netzwerkfluss kann semantische Anforderungen deklarieren.

Dazu können beispielsweise gehören:

- Reliability,
- Ordering,
- Latency,
- Deadline,
- Throughput,
- Loss Tolerance,
- Jitter,
- Connection Semantics,
- Data Sovereignty,
- Security Requirements.

Die genaue Menge und Typisierung dieser Eigenschaften wird in den NPSPECs definiert.

---

## Harte und weiche Anforderungen

Flow Requirements unterscheiden zwischen verbindlichen Constraints und Optimierungspräferenzen.

```text
Flow Requirements
      ↓
Hard Constraints
      ↓
Eligible Transports
      ↓
Soft Preferences
      ↓
Best Suitable Transport
```

Ein Transport, der eine harte Anforderung nicht erfüllen kann, darf nicht allein aufgrund besserer Performance ausgewählt werden.

---

## Transport Provider

Konkrete Transportprotokolle werden über Transport Provider integriert.

Beispiele:

```text
Reliable Ordered Flow
        ↓
TCP Provider

Low-Latency Datagram Flow
        ↓
UDP Provider

Future Flow
        ↓
Future Transport Provider
```

Diese Zuordnung ist nicht statisch. Mehrere Provider können dieselben Anforderungen erfüllen.

Damit können zukünftige Transportprotokolle integriert werden, ohne das Anwendungsmodell grundsätzlich zu verändern.

---

## Explizite Transportauswahl

Entwickler dürfen einen konkreten Transport bevorzugen oder explizit anfordern.

Konzeptionell:

```text
OpenFlow(requirements)
→ automatic selection

OpenFlow(requirements, preferred=TransportX)
→ preference

OpenFlow(requirements, required=TransportX)
→ explicit requirement
```

Eine explizite Auswahl darf jedoch keine übergeordneten Security-, Data-Sovereignty- oder anderen Hard Constraints verletzen.

---

## Verbindung zum Execution Contract

Flow Requirements können Bestandteil beziehungsweise Spezialisierung des `Nova.ExecutionContract` sein.

Dadurch können Netzwerkentscheidungen dieselben systemweiten Anforderungen berücksichtigen wie andere NovaOS-Ausführungsentscheidungen.

```text
Nova.ExecutionContract
        ↓
Network Flow Requirements
        ↓
Transport Selection
```

Deadline, Resource Budget, Trust, Data Sovereignty und Determinism können dadurch durchgängig erhalten bleiben.

---

## Adaptive Auswahl

NovaOS darf geeignete Transport Provider anhand aktueller Bedingungen auswählen.

Dabei können beispielsweise berücksichtigt werden:

- Netzwerkpfad,
- Latenz,
- Verlustverhalten,
- verfügbare Bandbreite,
- Energiebedarf,
- Provider-Fähigkeiten.

Adaptive Entscheidungen sind ausschließlich Optimierungsentscheidungen.

Prediction Error darf zur Verbesserung zukünftiger Auswahlentscheidungen verwendet werden, jedoch niemals Hard Constraints überschreiben.

---

## Protokollkompatibilität

Direkte TCP-, UDP- oder andere protokollspezifische APIs dürfen für Kompatibilität und Low-Level-Anwendungen verfügbar bleiben.

```text
Semantic Flow API
        ↓ preferred

Explicit Protocol API
        ↓ supported
```

Die semantische Flow-Abstraktion ersetzt daher nicht die Möglichkeit, bewusst mit konkreten Netzwerkprotokollen zu arbeiten.

---

## Introspection

Die Transportentscheidung muss kontrolliert nachvollziehbar sein.

NovaOS soll darstellen können:

```text
Flow
 ↓
Requirements
 ↓
Selected Transport
 ↓
Network Path
```

Damit können Entwickler und Systemdiagnose erkennen, welcher Transport verwendet wurde und welche Anforderungen die Auswahl beeinflusst haben.

---

## Normative Anforderungen

1. NovaOS MUSS Netzwerkflüsse über semantische Flow Requirements beschreiben können.
2. Anwendungen SOLLEN für allgemeine Netzwerkkommunikation nicht zwingend ein konkretes Transportprotokoll auswählen müssen.
3. Flow Requirements MÜSSEN harte Constraints und weiche Präferenzen unterscheiden können.
4. Hard Constraints MÜSSEN vor Transportoptimierungen ausgewertet werden.
5. Konkrete Transportprotokolle MÜSSEN als austauschbare Transport Provider integrierbar sein.
6. Mehrere Transport Provider MÜSSEN denselben semantischen Flow Contract erfüllen können.
7. NovaOS MUSS TCP und UDP über dieses Modell integrieren können.
8. Zukünftige Transport Provider MÜSSEN ohne grundlegende Änderung des Flow-Modells integrierbar sein.
9. Entwickler MÜSSEN einen konkreten Transport explizit auswählen können.
10. Explizite Transportauswahl DARF übergeordnete Hard Constraints NICHT verletzen.
11. Flow Requirements MÜSSEN mit `Nova.ExecutionContract` integrierbar sein.
12. Adaptive Transportauswahl DARF ausschließlich zulässige Provider optimieren.
13. Protokollspezifische APIs MÜSSEN für geeignete Low-Level- und Kompatibilitätsanwendungen möglich bleiben.
14. Gewählter Transport und relevante Auswahlentscheidung MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Anwendungen werden weniger an einzelne Transportprotokolle gekoppelt,
- zukünftige Transportprotokolle können leichter integriert werden,
- NovaOS kann Transporte workload- und situationsabhängig auswählen,
- Netzwerkkommunikation integriert sich in das Execution-Contract-Modell,
- Entwickler behalten die Möglichkeit expliziter Kontrolle.

### Negative Konsequenzen

- Transport Provider müssen ihre garantierbaren Eigenschaften deklarieren,
- automatische Auswahl benötigt eine systemweite Resolution Policy,
- Debugging benötigt Introspection über die tatsächlich verwendeten Transporte.

---

## Verworfene Alternativen

### Anwendungen wählen immer TCP oder UDP direkt

Verworfen.

Dies koppelt semantische Kommunikationsanforderungen unnötig an konkrete Implementierungen.

### Vollständig automatische Transportauswahl ohne Override

Verworfen.

Low-Level-, Kompatibilitäts- und Spezialanwendungen benötigen weiterhin explizite Kontrolle.

### Einheitliches Transportprotokoll für alle Workloads

Verworfen.

Unterschiedliche Kommunikationsmuster besitzen unterschiedliche Anforderungen an Zuverlässigkeit, Latenz, Reihenfolge und Datenübertragung.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-NETWORK-0001_IPv6_als_First_Class_Protokoll`
- `ADR-NETWORK-0002_IPv4_als_vollwertige_Kompatibilität`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0018_Locality_als_Optimierungsziel`
- `ADR-IPC-0004_Unified_Call_Abstraction`
- `ADR-IPC-0005_Lokale_und_entfernte_Aufrufe_unter_einem_Contract`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-FLOW-0001`
- `NPSPEC-NETWORK-FLOW-REQUIREMENTS-0001`
- `NPSPEC-NETWORK-TRANSPORT-PROVIDER-0001`
- `NPSPEC-NETWORK-TRANSPORT-SELECTION-0001`
- `NPSPEC-NETWORK-TRANSPORT-OVERRIDE-0001`
- `NPSPEC-NETWORK-FLOW-TEST-0001`

---

## Ergebnis

NovaOS abstrahiert Netzwerktransporte über die semantischen Anforderungen eines Datenflusses:

```text
Application
    ↓
Semantic Flow Requirements
    ↓
Transport Resolution
 ┌────┼──────────┐
TCP  UDP   Future Transport
```

Damit beschreibt eine Anwendung primär, welche Kommunikationssemantik sie benötigt, während NovaOS einen geeigneten Transport auswählen kann. Eine explizite Protokollauswahl bleibt weiterhin möglich.