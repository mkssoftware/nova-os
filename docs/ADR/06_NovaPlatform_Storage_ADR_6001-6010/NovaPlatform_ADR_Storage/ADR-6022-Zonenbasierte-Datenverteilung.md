# ADR-6022: Zonenbasierte Datenverteilung

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Storage Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS
- **Ersetzt:** Keine

---

## 1. Kontext

NovaFS ist für den Einsatz auf einzelnen Datenträgern, Storage-Pools und verteilten Clustern ausgelegt.

Mit zunehmender Größe eines Speichersystems steigen die Anforderungen an:

- Ausfallsicherheit
- Performance
- Parallelität
- Skalierbarkeit
- Self-Healing
- Erasure Coding
- Tiering
- Datenlokalität

Eine rein blockorientierte Speicherung berücksichtigt diese Anforderungen nur unzureichend.

NovaFS führt deshalb **Speicherzonen (Storage Zones)** als logische Verteilungseinheit ein.

Diese Zonen dienen ausschließlich der internen Datenorganisation und sind unabhängig von hardwareseitigen Technologien wie NVMe Zoned Namespaces (ZNS). Sie können jedoch zukünftig auf solche Speichertechnologien abgebildet werden. :contentReference[oaicite:0]{index=0}

---

## 2. Problemstellung

Werden Daten ausschließlich fortlaufend auf Datenträgern gespeichert, entstehen verschiedene Nachteile:

- ungleichmäßige Lastverteilung,
- erhöhte Fragmentierung,
- geringere Parallelität,
- ineffiziente Rekonstruktionen,
- schlechte Skalierbarkeit,
- schwierige Verteilung im Cluster.

NovaFS benötigt daher eine zusätzliche logische Ebene zur Organisation der Daten.

---

## 3. Betrachtete Alternativen

### Alternative A – Keine Zonierung

Alle Daten werden ausschließlich über Extents verwaltet.

**Vorteile**

- einfache Implementierung
- geringer Verwaltungsaufwand

**Nachteile**

- eingeschränkte Skalierbarkeit
- keine Lastverteilung
- erschwerte Parallelisierung

---

### Alternative B – Hardwareabhängige Zonierung

Die Zonierung orientiert sich ausschließlich an den Eigenschaften des Speichermediums.

**Vorteile**

- optimale Nutzung spezieller Hardware

**Nachteile**

- starke Hardwareabhängigkeit
- eingeschränkte Portabilität
- unterschiedliche Implementierungen erforderlich

---

### Alternative C – Logische Speicherzonen

NovaFS definiert eigene logische Speicherzonen.

Diese sind unabhängig vom verwendeten Datenträger.

**Vorteile**

- hardwareunabhängig
- hohe Skalierbarkeit
- gute Lastverteilung
- Grundlage für Tiering
- Grundlage für Cluster
- einfache Erweiterbarkeit

**Nachteile**

- zusätzliche Verwaltungsebene
- komplexere Datenplatzierung

---

## 4. Entscheidung

NovaFS verwendet **logische Speicherzonen** zur Organisation aller Nutzdaten.

Jeder Extent wird genau einer Zone zugeordnet.

Eine Zone bildet die kleinste logische Einheit für:

- Datenplatzierung,
- Lastverteilung,
- Rebalancing,
- Fragmentierung,
- Self-Healing,
- Erasure Coding,
- Tiering.

Die Zonenzuordnung erfolgt vollständig transparent.

---

## 5. Begründung

### Gleichmäßige Lastverteilung

Neue Daten werden über mehrere Zonen verteilt.

Dadurch entstehen:

- gleichmäßigere Schreiblast,
- bessere Parallelität,
- geringere Hotspots.

---

### Unterstützung großer Storage-Pools

Zonen können unabhängig voneinander verwaltet werden.

Neue Datenträger können zusätzliche Zonen bereitstellen, ohne bestehende Datenstrukturen grundlegend zu verändern.

---

### Grundlage für Rebalancing

Beim Hinzufügen oder Entfernen von Datenträgern müssen nicht sämtliche Daten verschoben werden.

NovaFS kann gezielt einzelne Zonen migrieren.

---

### Unterstützung von Tiering

Jede Zone kann einer bestimmten Speicherklasse zugeordnet werden.

Beispiele:

- RAM
- Persistent Memory
- NVMe
- SSD
- HDD
- Archivspeicher

Das Tiering erfolgt auf Zonenebene.

---

### Unterstützung von Self-Healing

Beschädigte Daten können zonenweise überprüft und rekonstruiert werden.

Dadurch sinkt der Verwaltungsaufwand erheblich.

---

## 6. Zonenmodell

Eine Zone besteht logisch aus:

- Zonen-ID
- Speicherklasse
- verfügbarem Speicher
- belegtem Speicher
- Fragmentierungsgrad
- Integritätsstatus
- Performance-Metriken

```text
Zone

├── Zone-ID
├── Storage Tier
├── Capacity
├── Used Space
├── Free Space
├── Fragmentation
├── Health Status
└── Statistics
```

---

## 7. Datenverteilung

Neue Extents werden automatisch geeigneten Zonen zugewiesen.

```text
Datei

        │

        ▼

Extent Manager

        │

        ▼

Zone Selection

        │

        ▼

Zone 3

        │

        ▼

Physischer Datenträger
```

Die Auswahl berücksichtigt unter anderem:

- verfügbare Kapazität,
- Speicherklasse,
- aktuelle Auslastung,
- Schutzrichtlinien,
- Failure Domains.

---

## 8. Zonenselektion

Bei der Auswahl einer Zielzone berücksichtigt NovaFS mindestens:

- freien Speicher,
- Fragmentierung,
- I/O-Auslastung,
- Temperatur der Daten (Hot/Warm/Cold),
- Speicherklasse,
- Failure Domain,
- Schutzrichtlinie,
- Erasure-Coding-Profil.

Dadurch wird eine möglichst gleichmäßige Verteilung erreicht.

---

## 9. Beziehung zu Failure Domains

Speicherzonen und Failure Domains sind unterschiedliche Konzepte.

- **Speicherzonen** organisieren Daten logisch.
- **Failure Domains** beschreiben Komponenten, die gemeinsam ausfallen können (z. B. Datenträger, Controller, Rack oder Standort).:contentReference[oaicite:1]{index=1}

NovaFS MUSS sicherstellen, dass redundante Fragmente nicht nur auf verschiedene Zonen, sondern auch auf unterschiedliche Failure Domains verteilt werden.

---

## 10. Rebalancing

Beim Hinzufügen neuer Speicher erfolgt automatisch ein zonenweises Rebalancing.

```text
Neuer Datenträger

        │

        ▼

Neue Zonen

        │

        ▼

Rebalancing

        │

        ▼

Migration einzelner Zonen
```

Während des Rebalancings bleiben sämtliche Daten verfügbar.

---

## 11. Positive Konsequenzen

Die Verwendung logischer Speicherzonen bietet zahlreiche Vorteile.

### Hohe Skalierbarkeit

Neue Speicher können einfach integriert werden.

---

### Gleichmäßige Lastverteilung

Hotspots werden reduziert.

---

### Effizientes Rebalancing

Nur betroffene Zonen müssen migriert werden.

---

### Grundlage für Tiering

Speicherklassen können flexibel kombiniert werden.

---

### Unterstützung von Self-Healing

Prüfungen und Reparaturen erfolgen zonenweise.

---

### Optimale Clusterintegration

Zonen können auf unterschiedliche Clusterknoten verteilt werden.

---

## 12. Negative Konsequenzen und Risiken

### Zusätzlicher Verwaltungsaufwand

NovaFS benötigt einen Zone Manager.

---

### Komplexere Datenplatzierung

Die Auswahl der optimalen Zone erfordert zusätzliche Entscheidungslogik.

---

### Rebalancing-Kosten

Bei Änderungen der Speichertopologie müssen Daten zwischen Zonen migriert werden.

Diese Vorgänge erfolgen online und transaktional.

---

## 13. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Zone Manager
- Extent Manager
- Allocation Manager
- Tiering Manager
- Storage Pool Manager
- Object Manager
- Protection Policy Manager
- Failure Domain Manager
- Self-Healing Manager
- Cluster Manager
- Rebalancing Manager

Alle zukünftigen Speichererweiterungen SOLLEN das Zonenkonzept unterstützen.

---

## 14. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Jeder Extent MUSS genau einer Zone zugeordnet sein.
- Jede Zone MUSS eine eindeutige Zonen-ID besitzen.
- Zonen MÜSSEN hardwareunabhängig definiert sein.
- Die Zonenauswahl MUSS automatisch erfolgen.
- Rebalancing MUSS online möglich sein.
- Tiering SOLL auf Zonenebene erfolgen.
- Redundante Fragmente DÜRFEN nicht ausschließlich innerhalb derselben Zone gespeichert werden.
- Failure Domains MÜSSEN zusätzlich zur Zonierung berücksichtigt werden.
- Neue Speichergeräte SOLLEN automatisch neue Zonen bereitstellen.

---

## 15. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6013 – B+-Trees als zentrale Datenstruktur
- ADR-6019 – Schutzrichtlinien pro Objekt
- ADR-6020 – Reed-Solomon als Standard-Erasure-Coding
- ADR-6021 – Failure Domains
- zukünftige ADR – Storage Pools
- zukünftige ADR – Tiering
- zukünftige ADR – Rebalancing
- Microsoft Learn – Fault Domain Awareness :contentReference[oaicite:2]{index=2}
- IBM Storage Ceph – Failure Domains :contentReference[oaicite:3]{index=3}

---

## 16. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Zonenbasierte Datenverteilung als verbindlicher Bestandteil der NovaFS-Architektur angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.