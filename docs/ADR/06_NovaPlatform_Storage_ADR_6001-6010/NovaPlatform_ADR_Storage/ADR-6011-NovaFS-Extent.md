# ADR-0001: Extentbasierte Speicherung

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Storage Architecture
- **Erstellt:** 2026-07-21
- **Gültig für:** NovaFS
- **Ersetzt:** Keine

---

## 1. Kontext

NovaFS soll als modernes Dateisystem für Nova OS entwickelt werden und sowohl auf einzelnen Datenträgern als auch in Multi-Disk-Pools und verteilten Clustern eingesetzt werden können.

Das Dateisystem muss hohe Leistung bei großen Dateien, geringer Fragmentierung, Snapshot-Unterstützung, Erasure Coding, Kompression, Verschlüsselung und Copy-on-Write bieten. Gleichzeitig soll es für zukünftige Erweiterungen wie Tiering, Rebalancing und Selbstheilung vorbereitet sein.

Die Wahl des internen Speicherlayouts bildet die Grundlage für nahezu alle weiteren Komponenten von NovaFS.

---

## 2. Problemstellung

Ein klassisches blockbasiertes Dateisystem speichert Dateien als Liste einzelner Blöcke oder indirekter Blocktabellen.

Dieses Verfahren verursacht bei großen Dateien:

- hohe Metadatenmengen,
- starke Fragmentierung,
- schlechte sequentielle Lese- und Schreibleistung,
- komplexe Blockverwaltung,
- hohen Verwaltungsaufwand bei Snapshot- und Copy-on-Write-Operationen.

Darüber hinaus erschwert eine blockweise Speicherung moderne Verfahren wie:

- Kompression,
- Verschlüsselung,
- Erasure Coding,
- Datenmigration,
- Tiering,
- Cluster-Rebalancing.

NovaFS benötigt deshalb ein flexibleres Speichermodell.

---

## 3. Betrachtete Alternativen

### Alternative A – Klassische Blocklisten

Jeder Datenblock wird einzeln verwaltet.

**Vorteile**

- sehr einfach
- historisch bewährt

**Nachteile**

- hoher Metadatenaufwand
- schlechte Skalierung
- hohe Fragmentierung

---

### Alternative B – Indirekte Blocktabellen

Verwendung mehrstufiger Blocktabellen ähnlich ext2/ext3.

**Vorteile**

- bessere Skalierung
- bekannte Implementierung

**Nachteile**

- viele indirekte Zugriffe
- komplexe Verwaltungsstruktur
- ungeeignet für moderne Storage-Konzepte

---

### Alternative C – Extentbasierte Speicherung

Dateien bestehen aus zusammenhängenden Speicherbereichen (Extents).

Jeder Extent beschreibt:

- Startblock
- Länge
- logischen Offset
- zusätzliche Eigenschaften

**Vorteile**

- geringe Metadatenmenge
- hohe Performance
- einfache Erweiterbarkeit
- optimale Grundlage für Copy-on-Write
- geeignet für Kompression und Erasure Coding

**Nachteile**

- komplexerer Allocator
- Extent-Splitting notwendig

---

## 4. Entscheidung

NovaFS verwendet ausschließlich **Extent-basierte Datenspeicherung**.

Dateien bestehen aus einer Liste von Extents.

Ein Extent beschreibt einen zusammenhängenden logischen Speicherbereich.

Alle Datenoperationen erfolgen über Extents.

Eine Speicherung einzelner Blockketten wird nicht unterstützt.

---

## 5. Begründung

Die Extent-basierte Speicherung bietet gegenüber klassischen Blocklisten erhebliche Vorteile.

### Weniger Metadaten

Eine Datei mit 1 GB benötigt bei 4-KiB-Blöcken:

- 262.144 Blockeinträge

Mit Extents dagegen beispielsweise:

- 3 bis 20 Extents

Die Metadaten werden dadurch drastisch reduziert.

---

### Höhere Performance

Große zusammenhängende Bereiche können direkt gelesen werden.

Dadurch sinkt:

- Anzahl der Tree-Zugriffe
- Anzahl der Verwaltungsoperationen
- CPU-Last

und gleichzeitig steigt die sequentielle Übertragungsrate.

---

### Weniger Fragmentierung

Der Allocator kann bevorzugt große zusammenhängende Bereiche reservieren.

Dadurch entstehen deutlich weniger Fragmente.

---

### Optimale Grundlage für Copy-on-Write

Neue Daten werden in neue Extents geschrieben.

Bestehende Extents bleiben unverändert.

Snapshots können dieselben Extents gemeinsam verwenden.

---

### Optimale Grundlage für Erasure Coding

Ein Extent lässt sich direkt in Fragmente zerlegen.

Beispielsweise:

```text
Extent
    ↓

Fragment 0
Fragment 1
Fragment 2
Fragment 3
Parity 0
Parity 1
```

Dadurch muss nicht jeder einzelne Block separat verarbeitet werden.

---

### Optimale Grundlage für Kompression

Jeder Extent kann:

- unkomprimiert
- LZ4
- ZSTD

gespeichert werden.

Die Kompression wird somit extentweise verwaltet.

---

### Optimale Grundlage für Verschlüsselung

Auch Verschlüsselung erfolgt extentweise.

Dadurch bleiben:

- Schlüsselrotation
- Migration
- Rebalancing

einfach implementierbar.

---

## 6. Positive Konsequenzen

Durch die Extent-Architektur entstehen zahlreiche Vorteile.

### Hohe Skalierbarkeit

Auch sehr große Dateien können mit wenigen Metadateneinträgen beschrieben werden.

---

### Hohe I/O-Leistung

Besonders bei:

- Videos
- Datenbanken
- Virtuellen Maschinen
- Images

entstehen deutlich weniger Verwaltungszugriffe.

---

### Effiziente Snapshots

Da Extents unverändert bleiben können, müssen Snapshots nur auf dieselben Extents verweisen.

---

### Einfache Datenmigration

Ein kompletter Extent kann verschoben werden.

Die Datei selbst bleibt unverändert.

---

### Effizientes Tiering

Ein Extent kann zwischen

- HDD
- SSD
- NVMe
- Remote Storage

verschoben werden.

---

### Geringerer Speicherbedarf für Metadaten

Der Object Tree bleibt klein.

Auch Cache-Strukturen werden effizienter.

---

### Unterstützung zukünftiger Funktionen

Extentbasierte Speicherung bildet die Grundlage für:

- Kompression
- Verschlüsselung
- Deduplizierung
- Tiering
- Erasure Coding
- Clusterbetrieb
- Rebalancing
- Live-Migration

---

## 7. Negative Konsequenzen und Risiken

### Komplexerer Allocator

Der freie Speicher muss intelligent verwaltet werden.

NovaFS benötigt daher:

- Free Space Tree
- Zone Manager
- Rebalancer

---

### Extent-Splitting

Kleine Änderungen innerhalb großer Dateien können einen Extent teilen.

Beispiel:

```text
Vorher

-------------------------

Nachher

------|---|--------------
```

Dies erhöht die Anzahl der Extents.

NovaFS muss daher regelmäßig zusammenhängende Extents wieder zusammenführen können.

---

### Defragmentierung

Obwohl deutlich seltener als bei Blocklisten, kann langfristig Fragmentierung entstehen.

NovaFS benötigt deshalb:

- Online-Defragmentierung
- Hintergrundoptimierung
- intelligentes Rebalancing

---

## 8. Auswirkungen auf andere Module

Diese Entscheidung betrifft nahezu alle NovaFS-Komponenten.

Direkt betroffen sind:

- Superblock
- Object Tree
- Extent Tree
- Allocation Manager
- Free Space Tree
- Protection Policy Manager
- Snapshot Manager
- Compression Layer
- Encryption Layer
- Erasure Coding
- Rebalancer
- Tiering Manager
- Cluster Layer
- Recovery System

Auch zukünftige Erweiterungen bauen auf dieser Architektur auf.

---

## 9. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Jede Datei MUSS mindestens einen Extent besitzen.
- Jeder Extent MUSS einen logischen Offset besitzen.
- Jeder Extent MUSS einen physischen Startblock besitzen.
- Jeder Extent MUSS seine Länge speichern.
- Jeder Extent MUSS einer Generation zugeordnet sein.
- Jeder Extent KANN komprimiert werden.
- Jeder Extent KANN verschlüsselt werden.
- Jeder Extent KANN Bestandteil eines Erasure-Coding-Stripes sein.
- Jeder Extent MUSS transaktional geändert werden.
- Bereits referenzierte Extents DÜRFEN niemals direkt überschrieben werden.
- Änderungen erfolgen ausschließlich über Copy-on-Write.

---

## 10. Referenzen

- NPSPEC-NOVAFS-0001
- RFC-0001
- RFC-0001.1
- RFC-0001.2
- zukünftige NPSPEC-TRANSACTION-0001
- zukünftige NPSPEC-CLUSTER-0001
- zukünftige NPSPEC-STORAGE-0001

---

## 11. Statushistorie

- **2026-07-21:** Version 1.0 erstellt.
- **2026-07-21:** Als grundlegende Speicherarchitektur von NovaFS angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.