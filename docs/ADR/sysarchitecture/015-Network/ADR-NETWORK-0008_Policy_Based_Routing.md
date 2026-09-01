# ADR-NETWORK-0008 – Policy-Based Routing

## Status

Angenommen

## Kategorie

Network / Routing / Policy / NetworkFlow / Multi-Homing

## Kontext

`ADR-NETWORK-0006` trennt Network Identity, Address und Route, während `ADR-NETWORK-0007` mehrere mögliche Netzwerkpfade pro `NetworkFlow` erlaubt.

Damit reicht klassisches Routing allein anhand einer Zieladresse nicht aus. Unterschiedliche Flows können trotz identischem Ziel unterschiedliche Anforderungen an Security, Data Sovereignty, Latenz, Bandbreite, Kosten oder Netzwerkprovider besitzen.

NovaOS benötigt deshalb eine Routing-Architektur, bei der die Route nicht ausschließlich aus der Zieladresse abgeleitet wird, sondern zusätzlich durch deklarative Policies und Flow Requirements bestimmt werden kann.

---

## Entscheidung

NovaOS verwendet **Policy-Based Routing als native Routing-Architektur**.

```text
NetworkFlow
     ↓
Destination / Addresses
     ↓
Available Routes
     ↓
Hard Policy Constraints
     ↓
Eligible Routes
     ↓
Soft Policy + Optimization
     ↓
Selected Route
```

Die klassische zielbasierte Routingtabelle bleibt ein wichtiger Mechanismus, ist jedoch nur ein Eingang der übergeordneten Route Selection.

---

## Grundprinzip

```text
Routing determines what is reachable.

Policy determines what is permitted and preferred.
```

---

## Routing Policy

Eine Routing Policy kann Eigenschaften des `NetworkFlow` und seines Execution Context berücksichtigen.

Dazu können insbesondere gehören:

- Flow Requirements,
- Security Context,
- Data Sovereignty,
- Trust Requirements,
- Resource Budget,
- Interface Policy,
- Provider Policy,
- Locality,
- QoS.

Die konkrete Policy-Sprache und Auswertungslogik werden in NPSPECs definiert.

---

## Hard Constraints

Verbindliche Routinganforderungen werden vor allen Optimierungszielen ausgewertet.

```text
Available Routes
      ↓
Security
      ↓
Data Sovereignty
      ↓
Trust / Required Provider
      ↓
Eligible Routes
```

Eine Route, die einen Hard Constraint verletzt, darf nicht aufgrund besserer Latenz, Bandbreite oder Kosten ausgewählt werden.

Existiert keine zulässige Route, muss dies als expliziter Routing- beziehungsweise Constraint-Fehler behandelt werden.

---

## Soft Policies

Nach Ermittlung der zulässigen Routen können Soft Policies die Auswahl optimieren.

Beispiele sind:

- niedrigere Latenz,
- höhere Bandbreite,
- geringerer Energieverbrauch,
- geringere monetäre Kosten,
- bevorzugtes Interface,
- bessere Locality,
- geringere Auslastung.

Soft Policies dürfen niemals Hard Constraints überschreiben.

---

## Flow-spezifisches Routing

Routingentscheidungen können für unterschiedliche `NetworkFlow`-Instanzen verschieden ausfallen.

```text
Same Destination

Flow A → Ethernet
Flow B → WLAN
Flow C → VPN / Trusted Provider
```

Damit muss nicht der gesamte Netzwerkverkehr eines Systems denselben bevorzugten Pfad verwenden.

---

## Multi-Homing und Multipath

Policy-Based Routing bildet die Auswahlgrundlage für Multi-Homing und Multipath Networking.

```text
NetworkFlow
     ↓
Routing Policy
     ↓
Eligible Paths
  ┌────┼────┐
Ethernet WLAN Mobile
```

Multipath darf nur Pfade verwenden, die durch die Routing Policy für den jeweiligen Flow zugelassen wurden.

---

## Data Sovereignty

Data Sovereignty ist ein Hard Constraint der Routingentscheidung.

Policies können Netzwerkpfade aufgrund von:

- Provider,
- Trust Domain,
- administrativer Domain,
- geografischen beziehungsweise souveränitätsbezogenen Anforderungen

ausschließen.

Ein technisch erreichbares Ziel bedeutet daher nicht automatisch, dass jeder verfügbare Netzwerkpfad verwendet werden darf.

---

## Resource Economy

Routing Policies dürfen Resource Budgets berücksichtigen.

Ein Netzwerkpfad kann beispielsweise technisch geeignet sein, aber aufgrund von:

- Bandbreitenbudgets,
- Interface-Ressourcen,
- Energieanforderungen,
- kostenpflichtigen Verbindungen

weniger geeignet oder nicht zulässig sein.

Routing darf Resource Accounting nicht umgehen.

---

## Adaptive Routing Policy

NovaOS darf Soft Policies anhand beobachteter Netzwerkbedingungen adaptiv optimieren.

Dabei können beispielsweise Latenz, Verlust, Jitter oder tatsächlicher Durchsatz berücksichtigt werden.

```text
Prediction
    ↓
Route Selection
    ↓
Observed Result
    ↓
Prediction Error
    ↓
Policy Adaptation
```

Adaptive Entscheidungen dürfen ausschließlich innerhalb der bereits zulässigen Routenmenge erfolgen.

---

## Deterministic Mode

Im Deterministic Mode müssen Routingentscheidungen reproduzierbar kontrollierbar sein.

Adaptive oder dynamische Soft-Policy-Entscheidungen können dafür deaktiviert oder durch festgelegte Routing Policies ersetzt werden.

Hard Constraints bleiben unverändert gültig.

---

## Introspection

Routingentscheidungen müssen kontrolliert introspektierbar sein.

NovaOS soll nachvollziehbar darstellen können:

```text
NetworkFlow
     ↓
Candidate Routes
     ↓
Applied Policies
     ↓
Rejected Routes
     ↓
Selected Route
```

Dadurch können Routingentscheidungen diagnostiziert werden, ohne sicherheitskritische Policy-Informationen unautorisiert offenzulegen.

---

## Normative Anforderungen

1. NovaOS MUSS Policy-Based Routing als native Routingfunktion unterstützen.
2. Routing DARF NICHT ausschließlich auf der Zieladresse basieren müssen.
3. Die klassische Routingtabelle MUSS als Routingmechanismus weiterhin unterstützt werden.
4. Routing Policies MÜSSEN Eigenschaften eines `NetworkFlow` berücksichtigen können.
5. Hard Constraints MÜSSEN vor Soft Policies und Optimierungszielen ausgewertet werden.
6. Eine Route, die einen Hard Constraint verletzt, DARF NICHT ausgewählt werden.
7. Das Fehlen einer zulässigen Route MUSS als expliziter Fehler behandelt werden.
8. Unterschiedliche Flows zum selben Ziel MÜSSEN unterschiedliche Routen verwenden können.
9. Security-, Trust- und Data-Sovereignty-Policies MÜSSEN Routingentscheidungen einschränken können.
10. Multipath Networking DARF nur durch die Policy zugelassene Pfade verwenden.
11. Resource Budgets MÜSSEN bei Routingentscheidungen berücksichtigt werden können.
12. Soft Policies DÜRFEN Performance-, Locality-, Energie- und Kostenmerkmale optimieren.
13. Adaptive Routingentscheidungen DÜRFEN Hard Constraints NICHT verändern oder umgehen.
14. Deterministic Mode MUSS adaptive Routingentscheidungen kontrollieren können.
15. Routingentscheidung und relevante Policy-Einflüsse MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Routing kann an den tatsächlichen Anforderungen eines Flows ausgerichtet werden,
- Data Sovereignty und Security werden Teil der Pfadauswahl,
- Multi-Homing und Multipath erhalten eine gemeinsame Policy-Grundlage,
- unterschiedliche Anwendungen können unterschiedliche Netzwerkpfade verwenden,
- adaptive Netzwerkoptimierung bleibt mit Hard Constraints vereinbar.

### Negative Konsequenzen

- Route Selection wird komplexer als klassische zielbasierte Routingtabellen,
- Policies benötigen klare Prioritäts- und Konfliktregeln,
- Routingdiagnose benötigt geeignete Introspection.

---

## Verworfene Alternativen

### Ausschließlich zielbasiertes Routing

Verworfen.

Die Zieladresse allein enthält keine Informationen über Security, Sovereignty, QoS oder Resource Requirements eines Flows.

### Globale Interface-Priorität für sämtlichen Netzwerkverkehr

Verworfen.

Unterschiedliche `NetworkFlow`-Instanzen können unterschiedliche Anforderungen besitzen.

### Adaptive Routingentscheidung ohne Hard-Constraint-Phase

Verworfen.

Performanceoptimierung darf niemals Security-, Trust- oder Data-Sovereignty-Anforderungen überschreiben.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-NETWORK-0003_Transportabstraktion_über_semantische_Flow_Requirements`
- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0006_Identity_Address_und_Route_strikt_trennen`
- `ADR-NETWORK-0007_Multipath_Networking`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0018_Locality_als_Optimierungsziel`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-POLICY-ROUTING-0001`
- `NPSPEC-NETWORK-ROUTE-POLICY-0001`
- `NPSPEC-NETWORK-ROUTE-SELECTION-0001`
- `NPSPEC-NETWORK-ROUTE-CONSTRAINT-0001`
- `NPSPEC-NETWORK-ROUTE-ADAPTIVE-0001`
- `NPSPEC-NETWORK-POLICY-ROUTING-TEST-0001`

---

## Ergebnis

NovaOS erweitert klassisches Routing um eine systemweite Policy-Ebene:

```text
NetworkFlow
     ↓
Reachable Routes
     ↓
Hard Constraints
     ↓
Permitted Routes
     ↓
Soft Policies
     ↓
Selected Route
```

Damit wird nicht nur entschieden, **welche Route ein Ziel erreichen kann**, sondern auch, **welche Route für den konkreten Flow verwendet werden darf und geeignet ist**.