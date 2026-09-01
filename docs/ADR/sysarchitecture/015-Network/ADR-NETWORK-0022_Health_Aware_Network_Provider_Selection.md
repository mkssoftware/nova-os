# ADR-NETWORK-0022 – Health-Aware Network Provider Selection

## Status

Angenommen

## Kategorie

Network / Provider Selection / Health / Resilience / Adaptive Networking

## Kontext

NovaOS kann Netzwerkkommunikation über unterschiedliche Interfaces, Transportprovider, Overlay-Netzwerke und physische Netzwerkpfade führen.

Die technische Verfügbarkeit eines Providers bedeutet jedoch nicht, dass dieser aktuell für einen `NetworkFlow` geeignet ist. Ein Provider kann beispielsweise erreichbar sein, gleichzeitig aber hohe Verlustraten, instabile Latenz, wiederholte Fehler oder eingeschränkte Kapazität aufweisen.

Die Auswahl von Netzwerkprovidern soll deshalb neben Contracts und Policies auch deren aktuellen Gesundheitszustand berücksichtigen.

---

## Entscheidung

NovaOS führt **Health-Aware Network Provider Selection als native Eigenschaft der Netzwerkarchitektur** ein.

```text
NetworkFlow / Intent
        ↓
Hard Constraints
        ↓
Eligible Providers
        ↓
Provider Health
        ↓
Soft Policy / Optimization
        ↓
Selected Provider
```

Provider Health beeinflusst die Auswahl ausschließlich innerhalb der durch Hard Constraints zulässigen Provider.

Ein gesunder Provider darf niemals ausgewählt werden, wenn er Security-, Data-Sovereignty-, Authority- oder andere verbindliche Anforderungen verletzt.

---

## Grundprinzip

```text
Permitted first.
Healthy second.
Optimal third.
```

---

## Provider Health

NovaOS behandelt Health als dynamische Eigenschaft eines Netzwerkproviders.

Health kann aus unterschiedlichen Signalen abgeleitet werden, beispielsweise:

- Connectivity,
- Fehlerhäufigkeit,
- Paketverlust,
- Latenz und Jitter,
- verfügbare Kapazität,
- wiederholte Timeouts,
- Provider- oder Device-Fehler,
- Stabilität über die Zeit.

Die konkreten Messgrößen, Gewichtungen und Health States werden in NPSPECs definiert.

---

## Health ist nicht Availability

NovaOS unterscheidet zwischen Verfügbarkeit und Gesundheit.

```text
Available
    ≠
Healthy
```

Ein Provider kann technisch verfügbar sein, aber aktuell eine schlechte Kommunikationsqualität besitzen.

Umgekehrt darf ein guter historischer Health-Zustand nicht als Beweis aktueller Erreichbarkeit verwendet werden.

---

## Provider Selection

Die Providerauswahl folgt der allgemeinen Constraint-Hierarchie von NovaOS.

```text
Candidate Providers
        ↓
Hard Constraints
        ↓
Eligible Providers
        ↓
Health Evaluation
        ↓
Soft Requirements
        ↓
Optimization
        ↓
Selected Provider
```

Health ist damit ein wichtiges Auswahlkriterium, aber keine Berechtigungsebene.

---

## Health States

Provider dürfen einen abstrahierten Health State besitzen.

Konzeptionell können beispielsweise Zustände wie:

```text
Healthy
Degraded
Unhealthy
Unknown
```

verwendet werden.

Die konkrete Zustandsmaschine ist Bestandteil der NPSPECs.

`Unknown` darf nicht automatisch mit `Healthy` gleichgesetzt werden.

---

## Mehrere Provider

Sind mehrere zulässige Provider verfügbar, kann NovaOS Health zur Auswahl des geeigneteren Providers verwenden.

```text
Provider A → Healthy
Provider B → Degraded
Provider C → Unhealthy
```

Andere relevante Soft Constraints wie Locality, QoS, Energie, Kosten oder Performance bleiben Bestandteil der Gesamtentscheidung.

Health ist daher kein isolierter globaler Score, der alle anderen Anforderungen ersetzt.

---

## Multipath und Connection Migration

Provider Health wird mit Multipath Networking und Connection Migration integriert.

Eine Verschlechterung kann:

- einen zusätzlichen Pfad aktivieren,
- Traffic auf andere Pfade verlagern,
- eine Connection Migration auslösen,
- einen Provider aus zukünftigen Auswahlentscheidungen zurückstufen.

```text
Active Provider
      ↓
Health Degradation
      ↓
Policy Evaluation
      ↓
Continue / Multipath / Migrate / Replace
```

Nicht jede kurzfristige Verschlechterung muss automatisch einen Providerwechsel auslösen.

---

## Stabilität und Flapping

Health-Aware Selection darf nicht zu permanentem Wechsel zwischen ähnlich bewerteten Providern führen.

Die Policy muss Stabilität als eigenes Optimierungsziel berücksichtigen können.

Kurzfristige Messschwankungen dürfen daher gefiltert beziehungsweise über geeignete Hysterese behandelt werden.

Die konkrete Methodik wird in NPSPECs definiert.

---

## Adaptive Bewertung

NovaOS darf Health-Bewertung und Provider Selection adaptiv optimieren.

Das allgemeine Prediction-Error-Modell kann verwendet werden:

```text
Predicted Provider Quality
          ↓
Provider Selection
          ↓
Observed Quality
          ↓
Prediction Error
          ↓
Model Correction
```

Wiederholte Fehlprognosen können zukünftige Soft-Policy-Entscheidungen verändern.

Adaptive Verfahren dürfen keine Hard Constraints verändern.

---

## Graceful Degradation

Ist kein vollständig gesunder Provider verfügbar, darf ein degradierter Provider verwendet werden, sofern er weiterhin alle Hard Constraints erfüllt und der Contract dies zulässt.

```text
Healthy Provider unavailable
          ↓
Permitted Degraded Provider?
    ├── Yes → Degraded Operation
    └── No  → Fail / Wait / Replan
```

Ein technisch gesunder, aber policywidriger Provider ist kein zulässiger Fallback.

---

## Resource Economy

Health Monitoring verursacht selbst Ressourcenverbrauch.

Messungen und aktive Probes müssen deshalb verhältnismäßig und budgetierbar sein.

NovaOS soll vorhandene Telemetrie und reale Kommunikationsdaten bevorzugt wiederverwenden, bevor zusätzliche aktive Messungen erzeugt werden.

---

## Deterministic Mode

Im Deterministic Mode muss der Einfluss dynamischer Health-Daten kontrollierbar sein.

Der Contract kann beispielsweise:

- feste Provider,
- definierte Failover-Reihenfolgen,
- eingeschränkte Health-basierte Wechsel

vorgeben.

Damit bleibt deterministisches Netzwerkverhalten möglich.

---

## Introspection

Health-Aware Provider Selection muss kontrolliert introspektierbar sein.

NovaOS soll insbesondere darstellen können:

```text
NetworkFlow
     ↓
Candidate Providers
     ↓
Health States
     ↓
Policy Evaluation
     ↓
Selected Provider
```

Dabei soll nachvollziehbar sein, ob ein Provider aufgrund von Hard Constraints, Health oder Optimierungsentscheidungen nicht verwendet wurde.

---

## Normative Anforderungen

1. NovaOS MUSS den Health-Zustand von Netzwerkprovidern bei der Providerauswahl berücksichtigen können.
2. Provider Health MUSS als dynamische Eigenschaft behandelt werden.
3. Availability und Health MÜSSEN getrennte Konzepte bleiben.
4. Hard Constraints MÜSSEN vor Health-basierter Optimierung ausgewertet werden.
5. Health DARF Security-, Authority-, Trust- oder Data-Sovereignty-Constraints NICHT überschreiben.
6. NovaOS MUSS unterschiedliche Health-Zustände einschließlich eines unbekannten Zustands darstellen können.
7. `Unknown` DARF NICHT automatisch als `Healthy` behandelt werden.
8. Health MUSS gemeinsam mit anderen Soft Constraints bewertet werden können.
9. Health-Aware Selection MUSS mit Multipath Networking und Connection Migration integrierbar sein.
10. Kurzfristige Health-Schwankungen SOLLEN nicht zu unnötigem Provider-Flapping führen.
11. Degradierte Provider DÜRFEN verwendet werden, wenn der Contract dies erlaubt und alle Hard Constraints erfüllt bleiben.
12. Adaptive Health-Bewertung DARF ausschließlich Soft Policies beeinflussen.
13. Health Monitoring MUSS in die Resource Economy integrierbar sein.
14. Deterministic Mode MUSS dynamische Health-basierte Auswahl kontrollieren können.
15. Health-Zustände und relevante Providerauswahlentscheidungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- instabile Netzwerkprovider können automatisch vermieden werden,
- Failover und Connection Migration erhalten eine gemeinsame Health-Grundlage,
- Multipath kann auf tatsächliche Providerqualität reagieren,
- Netzwerkentscheidungen werden resilienter gegenüber partiellen Fehlern,
- adaptive Providerwahl bleibt mit Hard Constraints vereinbar.

### Negative Konsequenzen

- Health Monitoring erzeugt zusätzlichen Zustand und Messaufwand,
- kurzfristige Netzwerkänderungen können zu Fehlbewertungen führen,
- Provider Health muss über unterschiedliche Netzwerktypen hinweg sinnvoll normalisiert werden.

---

## Verworfene Alternativen

### Auswahl ausschließlich nach technischer Verfügbarkeit

Verworfen.

Ein erreichbarer Provider kann für einen konkreten Flow dennoch stark degradiert oder instabil sein.

### Ein globaler Health Score als alleinige Entscheidungsgrundlage

Verworfen.

Health ersetzt weder Hard Constraints noch Flow Requirements, QoS, Locality oder andere Optimierungsziele.

### KI-basierte Health-Bewertung als Voraussetzung

Verworfen.

Lernende Verfahren dürfen die Auswahl verbessern, aber nicht Voraussetzung für korrektes Provider Monitoring und Failover sein.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0020_AI_nicht_Kernel_Correctness_Voraussetzung`
- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0007_Multipath_Networking`
- `ADR-NETWORK-0008_Policy_Based_Routing`
- `ADR-NETWORK-0016_Adaptive_Congestion_Control`
- `ADR-NETWORK-0019_Connection_Migration`
- `ADR-NETWORK-0020_Intent_Based_Networking`
- `ADR-NETWORK-0021_Data_Sovereignty_Aware_Routing`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-PROVIDER-HEALTH-0001`
- `NPSPEC-NETWORK-PROVIDER-HEALTH-METRICS-0001`
- `NPSPEC-NETWORK-PROVIDER-HEALTH-STATE-0001`
- `NPSPEC-NETWORK-PROVIDER-SELECTION-0001`
- `NPSPEC-NETWORK-PROVIDER-FAILOVER-0001`
- `NPSPEC-NETWORK-PROVIDER-HEALTH-ADAPTIVE-0001`
- `NPSPEC-NETWORK-PROVIDER-HEALTH-TEST-0001`

---

## Ergebnis

NovaOS erweitert seine Providerauswahl um den aktuellen Gesundheitszustand der verfügbaren Netzwerkprovider:

```text
Network Intent / NetworkFlow
            ↓
       Hard Constraints
            ↓
     Eligible Providers
            ↓
       Health State
            ↓
  Soft Policy / Optimization
            ↓
      Selected Provider
```

Damit wird nicht lediglich geprüft, ob ein Netzwerkprovider vorhanden ist, sondern auch, ob er aktuell für den jeweiligen Flow geeignet ist, ohne Security, Data Sovereignty oder andere Hard Constraints zugunsten eines vermeintlich besseren Providers aufzuweichen.