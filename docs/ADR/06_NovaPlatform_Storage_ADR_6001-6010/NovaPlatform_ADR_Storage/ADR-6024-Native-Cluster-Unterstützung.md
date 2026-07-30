# ADR-6024: Native Cluster-Unterstützung

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Cluster Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS
- **Ersetzt:** Keine

---

## 1. Kontext

NovaFS soll sowohl auf einzelnen Computern als auch auf hochverfügbaren Storage-Systemen eingesetzt werden.

Während klassische Dateisysteme primär für einzelne Rechner entwickelt wurden, soll NovaFS von Beginn an den Betrieb in einem verteilten Cluster unterstützen.

Ein Cluster soll ermöglichen:

- horizontale Skalierung,
- hohe Verfügbarkeit,
- automatische Lastverteilung,
- verteilte Speicherung,
- Self-Healing,
- Online-Erweiterung,
- standortübergreifende Speicherung,
- gemeinsame Nutzung eines logischen Dateisystems.

Die Clusterfähigkeit ist daher kein nachträglich hinzugefügtes Feature, sondern ein grundlegender Bestandteil der Architektur.

---

## 2. Problemstellung

Ein einzelner Rechner besitzt natürliche Grenzen hinsichtlich:

- Speicherkapazität,
- Anzahl der Datenträger,
- I/O-Leistung,
- Ausfallsicherheit,
- Verfügbarkeit.

Für große Installationen reicht ein einzelner Storage-Server nicht aus.

NovaFS benötigt daher eine native Clusterarchitektur.

---

## 3. Betrachtete Alternativen

### Alternative A – Kein Cluster

NovaFS arbeitet ausschließlich lokal.

**Vorteile**

- einfache Implementierung
- geringer Verwaltungsaufwand

**Nachteile**

- keine Skalierung
- keine Hochverfügbarkeit
- begrenzte Speicherkapazität

---

### Alternative B – Externer Cluster

Die Clusterverwaltung erfolgt außerhalb von NovaFS.

**Vorteile**

- Wiederverwendung vorhandener Lösungen

**Nachteile**

- eingeschränkte Integration
- komplexe Administration
- keine objektbezogenen Entscheidungen

---

### Alternative C – Native Clusterarchitektur

NovaFS integriert sämtliche Clusterfunktionen direkt.

Jeder Clusterknoten arbeitet als gleichberechtigter Bestandteil eines gemeinsamen Dateisystems.

**Vorteile**

- vollständige Integration
- optimale Performance
- gemeinsame Objektverwaltung
- automatische Lastverteilung
- Self-Healing
- Online-Skalierung

**Nachteile**

- höhere Implementierungskomplexität
- zusätzlicher Verwaltungsaufwand

---

## 4. Entscheidung

NovaFS besitzt eine **native Clusterarchitektur**.

Mehrere Storage Nodes bilden gemeinsam ein einziges logisches Dateisystem.

Alle Clusterfunktionen sind Bestandteil von NovaFS.

Der Cluster arbeitet objektorientiert.

Objekte können unabhängig voneinander auf verschiedene Storage Nodes verteilt werden.

---

## 5. Begründung

### Horizontale Skalierung

Neue Storage Nodes können jederzeit hinzugefügt werden.

Die verfügbare Speicherkapazität wächst nahezu linear.

---

### Hohe Verfügbarkeit

Fällt ein Storage Node aus, bleiben die übrigen Knoten verfügbar.

Self-Healing rekonstruiert fehlende Fragmente automatisch.

---

### Einheitlicher Namensraum

Der Benutzer sieht unabhängig von der Anzahl der Server nur ein einziges Dateisystem.

---

### Optimale Lastverteilung

Objekte werden automatisch über mehrere Storage Nodes verteilt.

Dadurch können:

- Lesezugriffe,
- Schreibzugriffe,
- Rekonstruktionen

parallel erfolgen.

---

### Zukunftssicherheit

Die Clusterarchitektur bildet die Grundlage für:

- Geo-Cluster,
- Cloud-Speicher,
- Edge-Storage,
- Rechenzentren,
- hybride Speicherlösungen.

---

## 6. Clusterarchitektur

Ein NovaFS-Cluster besteht mindestens aus folgenden Komponenten:

```text
NovaFS Cluster

├── Metadata Service
├── Storage Nodes
├── Cluster Manager
├── Object Manager
├── Transaction Manager
├── Recovery Manager
├── Self-Healing Manager
└── Monitoring Service
```

Die konkrete Implementierung kann später verteilt oder zusammengefasst werden.

---

## 7. Storage Nodes

Jeder Storage Node besitzt:

- Node-ID
- UUID
- Storage Pools
- Zonen
- Failure Domains
- Gesundheitsstatus
- verfügbare Kapazität
- Performanceinformationen

```text
Storage Node

├── Node UUID
├── Storage Pools
├── Zones
├── Health
├── Capacity
├── Statistics
└── Network Interface
```

---

## 8. Objektverteilung

Objekte werden automatisch auf geeignete Storage Nodes verteilt.

```text
Objekt

        │

        ▼

Protection Policy

        │

        ▼

Placement Engine

        │

        ▼

Node A

Node B

Node C
```

Die Auswahl berücksichtigt:

- Failure Domains,
- Auslastung,
- Speicherklasse,
- Erasure Coding,
- Netzwerkentfernung,
- verfügbare Kapazität.

---

## 9. Clusterkommunikation

Alle Clusterknoten kommunizieren über ein natives NovaFS-Clusterprotokoll.

Die Kommunikation umfasst mindestens:

- Metadaten
- Transaktionen
- Heartbeats
- Recovery
- Rebalancing
- Statusinformationen
- Replikationsbefehle

Die Kommunikation erfolgt verschlüsselt und authentifiziert.

---

## 10. Knotenausfall

Fällt ein Storage Node aus, erfolgt automatisch:

```text
Heartbeat fehlt

        │

        ▼

Node als ausgefallen markieren

        │

        ▼

Protection Policy prüfen

        │

        ▼

Self-Healing starten

        │

        ▼

Fragmente rekonstruieren

        │

        ▼

Cluster aktualisieren
```

Für Anwendungen bleibt das Dateisystem verfügbar.

---

## 11. Clustererweiterung

Neue Storage Nodes können online hinzugefügt werden.

```text
Neuer Node

        │

        ▼

Cluster beitreten

        │

        ▼

Storage Pools registrieren

        │

        ▼

Neue Zonen erzeugen

        │

        ▼

Rebalancing starten
```

Bestehende Daten bleiben währenddessen erreichbar.

---

## 12. Positive Konsequenzen

Die native Clusterunterstützung bietet zahlreiche Vorteile.

### Hohe Skalierbarkeit

Neue Server können jederzeit integriert werden.

---

### Hohe Verfügbarkeit

Der Ausfall einzelner Storage Nodes führt nicht zum Ausfall des Dateisystems.

---

### Automatische Lastverteilung

Objekte werden gleichmäßig verteilt.

---

### Unterstützung großer Speicher

Clustersysteme können auf Hunderte oder Tausende Datenträger wachsen.

---

### Optimale Integration

Alle NovaFS-Funktionen arbeiten direkt mit der Clusterarchitektur zusammen.

---

### Grundlage für Geo-Cluster

Standortübergreifende Installationen können später ohne Architekturänderungen realisiert werden.

---

## 13. Negative Konsequenzen und Risiken

### Höhere Komplexität

NovaFS benötigt zusätzliche Clusterkomponenten.

---

### Netzwerkabhängigkeit

Die Performance hängt teilweise vom Netzwerk ab.

---

### Konsistenzverwaltung

Verteilte Transaktionen benötigen zusätzliche Synchronisationsmechanismen.

---

### Monitoring erforderlich

Clusterzustand und Node-Gesundheit müssen kontinuierlich überwacht werden.

---

## 14. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Cluster Manager
- Storage Pool Manager
- Object Manager
- Placement Engine
- Transaction Manager
- Recovery Manager
- Self-Healing Manager
- Monitoring Manager
- Failure Domain Manager
- Rebalancing Manager
- Network Manager

Alle zukünftigen Speicherfunktionen MÜSSEN clusterfähig sein.

---

## 15. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Jeder Storage Node MUSS eine eindeutige Node-ID besitzen.
- Jeder Cluster MUSS eine eindeutige Cluster-UUID besitzen.
- Alle Clusterknoten MÜSSEN denselben Metadatenbestand verwenden.
- Die Datenplatzierung MUSS automatisch erfolgen.
- Neue Storage Nodes MÜSSEN online hinzugefügt werden können.
- Knotenausfälle MÜSSEN automatisch erkannt werden.
- Self-Healing MUSS nach einem Knotenausfall automatisch starten.
- Rebalancing MUSS online erfolgen.
- Die Clusterkommunikation MUSS verschlüsselt und authentifiziert sein.
- Alle Änderungen MÜSSEN transaktional erfolgen.

---

## 16. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6019 – Schutzrichtlinien pro Objekt
- ADR-6020 – Reed-Solomon als Standard-Erasure-Coding
- ADR-6021 – Failure Domains
- ADR-6022 – Zonenbasierte Datenverteilung
- ADR-6023 – Multi-Disk-Storage-Pools
- zukünftige ADR – Cluster-Kommunikationsprotokoll
- zukünftige ADR – Metadata Service
- zukünftige ADR – Placement Engine
- zukünftige ADR – Geo-Cluster

---

## 17. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Native Cluster-Unterstützung als grundlegender Bestandteil der NovaFS-Architektur angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.