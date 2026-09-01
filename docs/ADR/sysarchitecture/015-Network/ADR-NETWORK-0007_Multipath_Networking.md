# ADR-NETWORK-0007 – Multipath Networking

## Status

Angenommen

## Kategorie

Network / Multipath / Multi-Homing / Routing / NetworkFlow / Resilience

## Kontext

Moderne Systeme besitzen häufig mehrere gleichzeitig verfügbare Netzwerkpfade, beispielsweise Ethernet, WLAN, Mobilfunk oder mehrere physische beziehungsweise virtuelle Interfaces.

`ADR-NETWORK-0006` trennt Identity, Address und Route ausdrücklich voneinander. Eine Network Identity kann dadurch mehrere Adressen besitzen und eine Adresse über unterschiedliche Routen erreichbar sein.

NovaOS soll diese Möglichkeiten nicht nur für einfachen Failover verwenden. Ein `NetworkFlow` soll mehrere geeignete Netzwerkpfade nutzen können, wenn Transportprovider, Hardware und Flow Requirements dies erlauben.

---

## Entscheidung

NovaOS unterstützt **Multipath Networking als native Eigenschaft der Netzwerkarchitektur**.

```text
NetworkFlow
     ↓
Eligible Network Paths
  ┌──────┼──────┐
Path A  Path B  Path C
  └──────┼──────┘
     Transport
```

Ein Flow kann einen einzelnen oder mehrere Netzwerkpfade gleichzeitig beziehungsweise kontrolliert wechselnd verwenden.

Multipath ist eine Policy- und Optimierungsentscheidung und keine zwingende Eigenschaft jedes Flows.

---

## Grundprinzip

```text
One logical flow
may use multiple physical paths.
```

---

## Multipath und NetworkFlow

Die logische Identität eines `NetworkFlow` bleibt unabhängig von den tatsächlich verwendeten Netzwerkpfaden.

```text
NetworkFlow
   ├── Ethernet Path
   ├── WLAN Path
   └── Mobile Path
```

Das Hinzufügen, Entfernen oder Wechseln eines Pfads erzeugt daher nicht automatisch einen neuen logischen Flow.

Ob ein bestehender Transport eine solche Änderung ohne Neuaufbau unterstützt, hängt vom Transportprovider ab.

---

## Path Eligibility

Bevor ein Netzwerkpfad für Multipath verwendet werden darf, müssen seine Hard Constraints erfüllt sein.

Dazu gehören insbesondere:

- Security,
- Trust,
- Data Sovereignty,
- erforderliche Connectivity,
- Transport-Kompatibilität,
- verbindliche Flow Requirements.

```text
Available Paths
      ↓
Hard Constraints
      ↓
Eligible Paths
      ↓
Multipath Policy
```

Ein technisch erreichbarer Pfad ist nicht automatisch ein zulässiger Pfad.

---

## Path Selection

Zwischen zulässigen Pfaden darf NovaOS anhand von Flow Requirements und aktuellen Eigenschaften auswählen.

Dabei können beispielsweise berücksichtigt werden:

- Latenz,
- Bandbreite,
- Verlust,
- Jitter,
- Energiebedarf,
- Kosten,
- Locality,
- Auslastung.

Die konkrete Auswahl- und Verteilungsstrategie wird nicht durch diese ADR festgelegt.

---

## Transportprovider

Multipath-Fähigkeiten unterscheiden sich zwischen Transportprovidern.

```text
NetworkFlow
     ↓
Transport Provider
     ↓
Multipath Capability
```

Ein Provider muss deklarieren können, ob und in welcher Form er:

- mehrere Pfade,
- Path Migration,
- Failover

unterstützt.

NovaOS darf keine Multipath-Semantik vortäuschen, die der verwendete Transport nicht gewährleisten kann.

---

## Failover

Multipath Networking bildet eine Grundlage für kontrollierten Netzwerk-Failover.

```text
Primary Path
     ↓ failure
Alternative Eligible Path
     ↓
Continue / Reconnect
```

Ob ein Flow transparent fortgesetzt werden kann oder neu aufgebaut werden muss, hängt vom jeweiligen Transport Contract ab.

Ein Pfadwechsel darf nicht als garantiert unterbrechungsfrei dargestellt werden, wenn diese Garantie nicht existiert.

---

## Parallele Pfadnutzung

Geeignete Transportprovider dürfen mehrere Pfade gleichzeitig nutzen.

Dies kann beispielsweise verwendet werden für:

- Bandbreitenaggregation,
- Latenzoptimierung,
- Redundanz,
- Lastverteilung.

Parallele Nutzung ist jedoch nur zulässig, wenn die Semantik des Flows und des Transportproviders erhalten bleibt.

---

## Resource Economy

Jeder verwendete Netzwerkpfad verursacht eigene Ressourcen und muss entsprechend bilanziert werden.

Dazu können gehören:

- Bandbreite,
- Interface-Ressourcen,
- Buffer,
- CPU-Aufwand,
- Energie,
- Transportzustand.

Multipath darf Resource Budgets nicht umgehen, indem Ressourcen auf mehrere Interfaces verteilt werden.

---

## Adaptive Optimierung

NovaOS darf Pfadauswahl und Multipath-Nutzung adaptiv optimieren.

Prediction Error kann beispielsweise verwendet werden, wenn eine erwartete Verbesserung durch einen bestimmten Pfad tatsächlich nicht eintritt.

Adaptive Entscheidungen bleiben Soft Policy.

Hard Constraints dürfen niemals durch gelernte oder vorhergesagte Pfadpräferenzen überschrieben werden.

---

## Deterministic Mode

Im Deterministic Mode müssen Multipath-Entscheidungen kontrollierbar und reproduzierbar sein.

Adaptive Pfadwechsel oder dynamische Lastverteilung können dafür eingeschränkt oder durch festgelegte Policies ersetzt werden.

---

## Introspection

Die tatsächliche Multipath-Nutzung muss kontrolliert introspektierbar sein.

NovaOS soll nachvollziehbar machen können:

```text
NetworkFlow
   ↓
Transport
   ↓
Active Paths
   ├── Interface A
   └── Interface B
```

Dazu gehören insbesondere aktive, alternative und ausgefallene Pfade sowie relevante Gründe für Pfadentscheidungen.

---

## Normative Anforderungen

1. NovaOS MUSS Multipath Networking als native Netzwerkfunktion unterstützen können.
2. Ein `NetworkFlow` MUSS grundsätzlich mehrere Netzwerkpfade besitzen können.
3. Die logische Flow Identity DARF NICHT von einem einzelnen Netzwerkpfad abhängig sein.
4. Multipath DARF NICHT für jeden Flow erzwungen werden.
5. Netzwerkpfade MÜSSEN Hard Constraints erfüllen, bevor sie für Multipath verwendet werden.
6. Security-, Trust- und Data-Sovereignty-Regeln MÜSSEN für jeden verwendeten Pfad gelten.
7. Transportprovider MÜSSEN ihre Multipath- und Migrationseigenschaften deklarieren können.
8. NovaOS DARF keine stärkeren Multipath-Garantien annehmen, als der Transportprovider bereitstellt.
9. Kontrollierter Failover auf alternative zulässige Pfade MUSS unterstützt werden können.
10. Parallele Pfadnutzung MUSS für geeignete Transportprovider möglich sein.
11. Multipath-Ressourcen MÜSSEN der systemweiten Resource Economy zugerechnet werden.
12. Multipath DARF Resource Budgets NICHT umgehen.
13. Adaptive Pfadauswahl DARF ausschließlich innerhalb der zulässigen Pfadmenge optimieren.
14. Deterministic Mode MUSS dynamische Multipath-Entscheidungen kontrollieren können.
15. Aktive und alternative Netzwerkpfade MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- bessere Nutzung mehrerer Netzwerkinterfaces,
- höhere Resilienz bei Verbindungsausfällen,
- Grundlage für Bandbreitenaggregation und Lastverteilung,
- bessere Unterstützung mobiler und Multi-Homed-Systeme,
- NetworkFlow bleibt unabhängig von einzelnen physischen Pfaden,
- adaptive Netzwerkoptimierung wird möglich.

### Negative Konsequenzen

- Pfadauswahl und Ressourcensteuerung werden komplexer,
- nicht jeder Transport unterstützt echte Multipath-Semantik,
- parallele Pfade können zusätzlichen Energie- und Ressourcenverbrauch verursachen.

---

## Verworfene Alternativen

### Jeder Flow verwendet genau ein Interface

Verworfen.

Dies würde Multi-Homing und moderne Transportmöglichkeiten unnötig einschränken.

### Multipath ausschließlich als Routing-Funktion

Verworfen.

Transportsemantik, Flow Requirements und Resource Economy müssen ebenfalls berücksichtigt werden.

### Automatische Nutzung aller verfügbaren Pfade

Verworfen.

Nicht jeder Pfad ist hinsichtlich Security, Sovereignty, Kosten oder Flow Requirements zulässig oder sinnvoll.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-NETWORK-0003_Transportabstraktion_über_semantische_Flow_Requirements`
- `ADR-NETWORK-0004_TCP_UDP_QUIC_als_Transportprovider`
- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0006_Identity_Address_und_Route_strikt_trennen`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0018_Locality_als_Optimierungsziel`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-MULTIPATH-0001`
- `NPSPEC-NETWORK-MULTIPATH-PATH-0001`
- `NPSPEC-NETWORK-MULTIPATH-SELECTION-0001`
- `NPSPEC-NETWORK-MULTIPATH-FAILOVER-0001`
- `NPSPEC-NETWORK-MULTIPATH-RESOURCE-0001`
- `NPSPEC-NETWORK-MULTIPATH-TEST-0001`

---

## Ergebnis

NovaOS behandelt mehrere Netzwerkverbindungen als gemeinsam nutzbare Pfade eines logischen `NetworkFlow`:

```text
             NetworkFlow
                  ↓
           Eligible Paths
          /       |       \
     Ethernet    WLAN    Mobile
          \       |       /
           Transport Provider
```

Damit können Netzwerkpfade gewechselt, kombiniert und optimiert werden, ohne die logische Kommunikationsidentität an ein einzelnes Interface oder eine einzelne Route zu binden.