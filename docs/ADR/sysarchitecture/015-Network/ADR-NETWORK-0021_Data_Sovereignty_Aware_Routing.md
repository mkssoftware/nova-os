# ADR-NETWORK-0021 – Data-Sovereignty-Aware Routing

## Status

Angenommen

## Kategorie

Network / Routing / Data Sovereignty / Information Flow / Policy

## Kontext

NovaOS behandelt Data Sovereignty und Information Flow als systemweite Architekturprinzipien. Daten dürfen nicht allein deshalb über einen Netzwerkpfad übertragen werden, weil dieser technisch erreichbar oder besonders performant ist.

Routingentscheidungen müssen berücksichtigen können, wo Daten verarbeitet, transportiert oder zwischengespeichert werden dürfen und welche Trust-, Provider- oder administrativen Grenzen dabei eingehalten werden müssen.

Data Sovereignty ist dabei keine nachträgliche Routingpräferenz, sondern kann einen verbindlichen Hard Constraint darstellen.

---

## Entscheidung

NovaOS integriert **Data Sovereignty als First-Class Constraint in Routing, Path Selection und Network Intents**.

```text
NetworkFlow
     ↓
Data-Sovereignty Constraints
     ↓
Available Paths
     ↓
Permitted Paths
     ↓
Policy / Optimization
     ↓
Selected Route
```

Ein Netzwerkpfad wird nur dann für einen Flow zugelassen, wenn er dessen verbindliche Data-Sovereignty-Anforderungen erfüllt.

Erst innerhalb dieser zulässigen Pfade dürfen Latenz, Bandbreite, Energie, Kosten oder andere Optimierungsziele berücksichtigt werden.

---

## Grundprinzip

```text
Reachable
    ≠
Permitted
```

und:

```text
Sovereignty before optimization.
```

---

## Sovereignty Constraints

Ein `NetworkFlow` beziehungsweise `Network Intent` kann Data-Sovereignty-Anforderungen tragen.

Diese können sich beispielsweise auf folgende Eigenschaften beziehen:

- geografische Regionen,
- administrative Domains,
- Network Provider,
- Trust Domains,
- Overlay- oder VPN-Anforderungen,
- zulässige Transitbereiche,
- Verarbeitung oder Speicherung während der Übertragung.

Die konkrete Policy-Sprache und Repräsentation werden in NPSPECs definiert.

---

## Hard Constraint

Verbindliche Data-Sovereignty-Regeln werden vor der eigentlichen Routingoptimierung ausgewertet.

```text
Available Routes
      ↓
Security
      ↓
Data Sovereignty
      ↓
Other Hard Constraints
      ↓
Eligible Routes
      ↓
Soft Optimization
```

Ein schnellerer, günstigerer oder stabilerer Pfad darf nicht ausgewählt werden, wenn er eine verbindliche Sovereignty Policy verletzt.

---

## Route Metadata

Damit Sovereignty-aware Routing möglich ist, können Netzwerkpfade kontrollierte Metadaten über relevante Eigenschaften bereitstellen.

```text
Route
 ├── Provider
 ├── Administrative Domain
 ├── Trust Properties
 ├── Location Properties
 └── Sovereignty Properties
```

Nicht bekannte Eigenschaften müssen explizit als unbekannt behandelt werden.

```text
Unknown
    ≠
Allowed
```

Ob ein unbekannter Pfad zulässig ist, entscheidet die jeweilige Policy.

---

## Policy-Based Routing

Data Sovereignty wird direkt in das Policy-Based Routing integriert.

```text
Destination
     ↓
Candidate Routes
     ↓
Routing Policy
     +
Sovereignty Policy
     ↓
Eligible Routes
```

Es entsteht kein unabhängiger zweiter Routingmechanismus.

Sovereignty Constraints begrenzen vielmehr die Menge der für das allgemeine Routing zulässigen Pfade.

---

## Multipath

Bei Multipath Networking muss jeder verwendete Pfad die geltenden Sovereignty Constraints erfüllen.

```text
NetworkFlow
   ├── Path A → permitted
   ├── Path B → permitted
   └── Path C → prohibited
```

Ein unzulässiger Pfad darf weder als zusätzlicher Datenpfad noch als automatischer Failover-Pfad verwendet werden.

---

## Overlay und VPN

Overlay- oder VPN-Netzwerke können Bestandteil einer Data-Sovereignty-Policy sein.

Ein VPN macht einen darunterliegenden Netzwerkpfad jedoch nicht automatisch sovereignty-konform.

```text
Overlay Path
      ↓
Underlay Paths
      ↓
Sovereignty Evaluation
```

Soweit für die Policy relevant und technisch bestimmbar, müssen sowohl Overlay- als auch Underlay-Eigenschaften berücksichtigt werden.

---

## Service Discovery

Data Sovereignty kann bereits die Auswahl einer Service Instance begrenzen.

```text
Service Discovery
       ↓
Candidate Instances
       ↓
Sovereignty Constraints
       ↓
Eligible Instances
       ↓
Route Selection
```

Dadurch kann NovaOS beispielsweise eine zulässige regionale Service Instance auswählen, bevor der konkrete Netzwerkpfad geplant wird.

---

## Connection Migration

Connection Migration darf einen Flow nicht auf einen sovereignty-widrigen Pfad verschieben.

Vor jeder Migration muss der neue Pfad erneut gegen die geltenden Hard Constraints geprüft werden.

Dies gilt auch für automatische Migrationen aufgrund von Mobility, Ausfällen oder Performanceoptimierung.

---

## Graceful Degradation

Ist kein zulässiger Netzwerkpfad verfügbar, darf NovaOS eine Data-Sovereignty-Anforderung nicht stillschweigend abschwächen.

```text
No Permitted Route
       ↓
Explicit Degradation allowed?
   ├── Yes → permitted alternative
   └── No  → Fail / Wait / Replan
```

Eine schwächere Policy darf ausschließlich verwendet werden, wenn der zugrunde liegende Contract dies ausdrücklich erlaubt.

---

## Adaptive Routing

Adaptive Systeme dürfen innerhalb der sovereignty-konformen Pfade optimieren.

```text
Permitted Paths
      ↓
Adaptive Optimization
      ↓
Selected Path
```

Lernende oder prädiktive Verfahren dürfen niemals eigenständig einen durch Hard Constraints ausgeschlossenen Pfad freigeben.

Prediction Error darf ausschließlich Soft Policies beeinflussen.

---

## Introspection

Sovereignty-aware Routing muss kontrolliert nachvollziehbar sein.

NovaOS soll insbesondere darstellen können:

```text
NetworkFlow
     ↓
Sovereignty Policy
     ↓
Candidate Routes
     ↓
Excluded / Eligible Routes
     ↓
Selected Route
```

Dabei muss nachvollziehbar sein, warum ein Pfad zugelassen oder ausgeschlossen wurde, soweit die erforderlichen Informationen gegenüber dem anfragenden Kontext offengelegt werden dürfen.

---

## Normative Anforderungen

1. NovaOS MUSS Data Sovereignty als First-Class Routing Constraint unterstützen.
2. Data-Sovereignty-Anforderungen MÜSSEN Bestandteil von `NetworkFlow`- beziehungsweise Network-Intent-Contracts sein können.
3. Verbindliche Sovereignty Constraints MÜSSEN vor Routingoptimierungen ausgewertet werden.
4. Ein durch Sovereignty Policy ausgeschlossener Pfad DARF NICHT aufgrund von Performance-, Kosten- oder Verfügbarkeitsvorteilen verwendet werden.
5. Routingpfade MÜSSEN relevante Sovereignty-Eigenschaften deklarieren können.
6. Unbekannte Sovereignty-Eigenschaften DÜRFEN NICHT automatisch als zulässig behandelt werden.
7. Data-Sovereignty-Routing MUSS in Policy-Based Routing integriert werden.
8. Bei Multipath MUSS jeder verwendete Pfad die geltenden Sovereignty Constraints erfüllen.
9. Overlay- und VPN-Nutzung DARF Underlay-Sovereignty-Anforderungen NICHT automatisch aufheben.
10. Service Discovery MUSS durch Data-Sovereignty-Constraints begrenzt werden können.
11. Connection Migration MUSS den neuen Pfad erneut gegen geltende Sovereignty Constraints prüfen.
12. Adaptive Routing DARF nur innerhalb der sovereignty-konformen Lösungsmenge optimieren.
13. Prediction Error DARF verbindliche Sovereignty Constraints NICHT verändern.
14. Eine Abschwächung von Sovereignty-Anforderungen MUSS ausdrücklich durch den Contract erlaubt sein.
15. Routingentscheidungen und relevante Sovereignty-Ausschlüsse MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Data Sovereignty wird technisch bis in die Netzwerkpfadauswahl durchgesetzt,
- Anwendungen müssen geografische und administrative Routingdetails nicht selbst implementieren,
- Service Discovery, Routing, Multipath, VPN und Migration verwenden dieselben Sovereignty Constraints,
- Performanceoptimierung kann weiterhin innerhalb zulässiger Pfade stattfinden,
- Verstöße durch automatische Routingentscheidungen werden verhindert.

### Negative Konsequenzen

- zuverlässige Informationen über Netzwerkpfade sind nicht immer vollständig verfügbar,
- strenge Policies können die Menge nutzbarer Pfade deutlich reduzieren,
- Routingplanung benötigt zusätzliche Policy- und Metadatenverarbeitung.

---

## Verworfene Alternativen

### Data Sovereignty ausschließlich auf Anwendungsebene

Verworfen.

Anwendungen besitzen weder eine vollständige Sicht auf die tatsächlichen Netzwerkpfade noch sollten sie systemweite Routing- und Providerregeln selbst implementieren.

### Data Sovereignty nur als Routingpräferenz

Verworfen.

Verbindliche regulatorische oder organisatorische Anforderungen dürfen nicht zugunsten besserer Performance ignoriert werden.

### VPN automatisch als sovereignty-konform behandeln

Verworfen.

Verschlüsselung schützt Dateninhalte, sagt jedoch nicht automatisch aus, über welche geografischen oder administrativen Domains der zugrunde liegende Netzwerkpfad verläuft.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0007_Multipath_Networking`
- `ADR-NETWORK-0008_Policy_Based_Routing`
- `ADR-NETWORK-0012_Service_Discovery`
- `ADR-NETWORK-0014_Native_Overlay_VPN_Architektur`
- `ADR-NETWORK-0019_Connection_Migration`
- `ADR-NETWORK-0020_Intent_Based_Networking`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-SOVEREIGNTY-0001`
- `NPSPEC-NETWORK-SOVEREIGNTY-POLICY-0001`
- `NPSPEC-NETWORK-SOVEREIGNTY-ROUTE-METADATA-0001`
- `NPSPEC-NETWORK-SOVEREIGNTY-ROUTING-0001`
- `NPSPEC-NETWORK-SOVEREIGNTY-MULTIPATH-0001`
- `NPSPEC-NETWORK-SOVEREIGNTY-INTROSPECTION-0001`
- `NPSPEC-NETWORK-SOVEREIGNTY-TEST-0001`

---

## Ergebnis

NovaOS macht Data Sovereignty zu einem verbindlichen Bestandteil der Netzwerkpfadauswahl:

```text
Network Intent / NetworkFlow
            ↓
   Sovereignty Constraints
            ↓
      Candidate Paths
            ↓
       Permitted Paths
            ↓
 Routing / Multipath Optimization
            ↓
       Selected Path
```

Damit werden Netzwerkpfade nicht nur danach bewertet, ob sie technisch erreichbar und performant sind, sondern zuerst danach, ob die Übertragung über sie überhaupt zulässig ist.