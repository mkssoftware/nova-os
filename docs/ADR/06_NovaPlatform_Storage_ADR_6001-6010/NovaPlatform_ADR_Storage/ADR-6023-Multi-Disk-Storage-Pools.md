# ADR-6023: Multi-Disk-Storage-Pools

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Storage Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS
- **Ersetzt:** Keine

---

## 1. Kontext

NovaFS soll nicht nur einzelne Datenträger verwalten können, sondern auch beliebig große Speicherverbünde (Storage Pools).

Ein Storage Pool fasst mehrere physische Speichermedien zu einer einzigen logischen Speichereinheit zusammen.

Dabei sollen folgende Speichertypen gemeinsam nutzbar sein:

- HDD
- SATA-SSD
- NVMe-SSD
- Persistent Memory
- zukünftige Speichertechnologien

Ein Storage Pool bildet die Grundlage für:

- Erasure Coding
- Spiegelung
- Tiering
- Rebalancing
- Self-Healing
- Clusterbetrieb
- Online-Erweiterungen

Moderne Speichersysteme nutzen Storage Pools, um mehrere physische Geräte zu einem logischen Speicher zusammenzufassen und Verwaltung, Erweiterung sowie Ausfallsicherheit zu vereinfachen. :contentReference[oaicite:0]{index=0}

---

## 2. Problemstellung

Ein Dateisystem, das ausschließlich auf einem einzelnen Datenträger arbeitet, besitzt erhebliche Einschränkungen:

- begrenzte Kapazität,
- eingeschränkte Performance,
- keine einfache Erweiterbarkeit,
- schwierige Redundanz,
- keine Lastverteilung,
- keine verteilte Speicherung.

NovaFS benötigt daher eine zusätzliche Verwaltungsebene oberhalb einzelner Datenträger.

---

## 3. Betrachtete Alternativen

### Alternative A – Einzelner Datenträger

Jedes Dateisystem verwendet genau ein Speichermedium.

**Vorteile**

- einfache Implementierung
- geringer Verwaltungsaufwand

**Nachteile**

- keine Skalierbarkeit
- keine Online-Erweiterung
- keine Lastverteilung

---

### Alternative B – Betriebssystem-RAID

Die Zusammenfassung mehrerer Datenträger erfolgt außerhalb von NovaFS.

**Vorteile**

- bewährte Technologie
- geringe Komplexität im Dateisystem

**Nachteile**

- eingeschränkte Kontrolle
- keine Integration mit Schutzrichtlinien
- keine Integration mit Erasure Coding
- keine objektspezifischen Entscheidungen

---

### Alternative C – Native Storage Pools

NovaFS verwaltet sämtliche Datenträger selbst.

Mehrere Speichermedien werden zu einem logischen Pool zusammengefasst.

**Vorteile**

- vollständige Kontrolle
- optimale Integration
- Online-Erweiterung
- Self-Healing
- Rebalancing
- Tiering
- Erasure Coding

**Nachteile**

- komplexere Implementierung
- zusätzlicher Verwaltungsaufwand

---

## 4. Entscheidung

NovaFS verwendet **native Multi-Disk-Storage-Pools**.

Ein Storage Pool besteht aus einem oder mehreren physikalischen Datenträgern.

Alle Speichervorgänge erfolgen ausschließlich über den Storage Pool.

Objekte kennen keine physischen Datenträger.

Sie arbeiten ausschließlich mit:

- Extents
- Zonen
- Protection Policies

Der Storage Pool entscheidet über die physische Platzierung.

---

## 5. Begründung

### Nahezu unbegrenzte Skalierbarkeit

Neue Datenträger können jederzeit in einen bestehenden Pool aufgenommen werden.

Das Dateisystem muss dafür nicht neu erstellt werden.

---

### Online-Erweiterung

Speicherkapazität kann im laufenden Betrieb erweitert werden.

Ein Neustart ist nicht erforderlich.

---

### Grundlage für Tiering

Ein Storage Pool kann unterschiedliche Speichermedien enthalten.

Beispielsweise:

- NVMe
- SSD
- HDD

NovaFS entscheidet automatisch über die optimale Platzierung.

---

### Unterstützung von Schutzrichtlinien

Protection Policies können innerhalb desselben Pools unterschiedliche Speicherstrategien verwenden.

Beispiele:

- Spiegelung
- Reed-Solomon
- Archivspeicherung

---

### Grundlage für Cluster

Ein Storage Pool kann später auf mehrere Storage Nodes erweitert werden.

Dadurch entsteht ein verteilter Speicherverbund.

---

## 6. Aufbau eines Storage Pools

Ein Pool besteht logisch aus:

- Pool-ID
- UUID
- Datenträgerliste
- Zonen
- Speicherklassen
- Kapazitätsinformationen
- Health Status
- Rebalancing-Status

```text
Storage Pool

├── Pool UUID
├── Pool Name
├── Devices
├── Zones
├── Storage Classes
├── Capacity
├── Used Space
├── Free Space
├── Health Status
└── Statistics
```

---

## 7. Datenträgerstruktur

Ein Pool kann beliebig viele Datenträger enthalten.

```text
Storage Pool

├── Disk 1
├── Disk 2
├── Disk 3
├── Disk 4
└── ...
```

Jeder Datenträger besitzt:

- Geräte-ID
- UUID
- Kapazität
- Gesundheitsstatus
- Failure Domain
- Zonen

---

## 8. Datenplatzierung

Die physische Platzierung erfolgt automatisch.

```text
Objekt

        │

        ▼

Extent Manager

        │

        ▼

Zone Manager

        │

        ▼

Storage Pool

        │

        ▼

Geeigneter Datenträger
```

Die Auswahl berücksichtigt:

- freien Speicher,
- Speicherklasse,
- I/O-Auslastung,
- Schutzrichtlinie,
- Failure Domain,
- Erasure-Coding-Profil.

---

## 9. Online-Erweiterung

Neue Datenträger können jederzeit hinzugefügt werden.

```text
Neuer Datenträger

        │

        ▼

Pool erweitern

        │

        ▼

Neue Zonen erzeugen

        │

        ▼

Rebalancing starten
```

Alle Daten bleiben währenddessen verfügbar.

---

## 10. Datenträgerausfall

Fällt ein Datenträger aus, erfolgt:

```text
Datenträger ausgefallen

        │

        ▼

Failure erkennen

        │

        ▼

Protection Policy prüfen

        │

        ▼

Self-Healing

        │

        ▼

Daten rekonstruieren

        │

        ▼

Neuen Datenträger integrieren
```

---

## 11. Positive Konsequenzen

Die Verwendung von Multi-Disk-Storage-Pools bietet zahlreiche Vorteile.

### Hohe Skalierbarkeit

Pools können nahezu beliebig erweitert werden.

---

### Online-Erweiterung

Neue Speichermedien können ohne Unterbrechung integriert werden.

---

### Lastverteilung

Daten werden gleichmäßig über mehrere Datenträger verteilt.

---

### Unterstützung von Tiering

Mehrere Speicherklassen können gleichzeitig genutzt werden.

---

### Grundlage für Self-Healing

Beschädigte Daten können automatisch auf andere Datenträger rekonstruiert werden.

---

### Optimale Clusterintegration

Storage Pools bilden die Grundlage verteilter Speichersysteme.

---

## 12. Negative Konsequenzen und Risiken

### Höherer Verwaltungsaufwand

NovaFS benötigt einen Storage Pool Manager.

---

### Komplexere Datenplatzierung

Die optimale Auswahl eines Datenträgers erfordert zusätzliche Entscheidungslogik.

---

### Rebalancing

Beim Hinzufügen oder Entfernen von Datenträgern müssen Daten teilweise neu verteilt werden.

Dieser Vorgang erfolgt online.

---

## 13. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Storage Pool Manager
- Zone Manager
- Extent Manager
- Allocation Manager
- Protection Policy Manager
- Mirror Manager
- Erasure Coding Manager
- Tiering Manager
- Failure Domain Manager
- Self-Healing Manager
- Cluster Manager
- Rebalancing Manager

Alle zukünftigen Speicherfunktionen MÜSSEN Storage Pools unterstützen.

---

## 14. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Jeder Datenträger MUSS genau einem Storage Pool zugeordnet sein.
- Jeder Storage Pool MUSS eine eindeutige UUID besitzen.
- Datenträger DÜRFEN im laufenden Betrieb hinzugefügt werden.
- Datenträger DÜRFEN im laufenden Betrieb entfernt werden, sofern die Schutzrichtlinien dies erlauben.
- Rebalancing MUSS online erfolgen.
- Die Datenplatzierung MUSS automatisch erfolgen.
- Alle Speicherentscheidungen MÜSSEN die Failure Domains berücksichtigen.
- Storage Pools MÜSSEN unterschiedliche Speicherklassen unterstützen.
- Jeder Datenträger MUSS seinen eigenen Gesundheitsstatus bereitstellen.
- Jeder Storage Pool MUSS transaktional verwaltet werden.

---

## 15. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6019 – Schutzrichtlinien pro Objekt
- ADR-6020 – Reed-Solomon als Standard-Erasure-Coding
- ADR-6021 – Failure Domains
- ADR-6022 – Zonenbasierte Datenverteilung
- zukünftige ADR – Tiering
- zukünftige ADR – Self-Healing
- zukünftige ADR – Cluster Architecture

---

## 16. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Multi-Disk-Storage-Pools als verbindliche Speicherarchitektur von NovaFS angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.