# ADR-6013: B+-Trees als zentrale Datenstruktur

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Metadata Architecture
- **Erstellt:** 2026-07-22
- **Gültig für:** NovaFS
- **Ersetzt:** Keine

---

## 1. Kontext

NovaFS ist als modernes, transaktionales Dateisystem konzipiert und soll Milliarden von Dateien, Verzeichnissen und Metadatenobjekten effizient verwalten können.

Die gesamte interne Metadatenverwaltung muss folgende Anforderungen erfüllen:

- logarithmische Suchzeiten,
- minimale Anzahl an Datenträgerzugriffen,
- hohe Cache-Effizienz,
- Unterstützung für Copy-on-Write,
- atomare Transaktionen,
- Snapshots,
- Clusterbetrieb,
- parallelen Zugriff,
- sehr große Dateisysteme.

Da sämtliche Metadatenstrukturen dauerhaft auf Massenspeichern liegen, muss die gewählte Datenstruktur speziell für blockorientierte Speichermedien optimiert sein. B+-Bäume sind hierfür seit Jahrzehnten eine bewährte Wahl, da sie wenige Ebenen benötigen und alle Nutzdaten in den Blattknoten speichern. 

---

## 2. Problemstellung

Lineare Listen oder einfache Binärbäume skalieren bei großen Dateisystemen nur unzureichend.

Insbesondere entstehen folgende Probleme:

- hohe Baumtiefe,
- viele Datenträgerzugriffe,
- schlechte Cache-Ausnutzung,
- ineffiziente Bereichsabfragen,
- schwierige Parallelisierung,
- aufwendige Metadatenverwaltung.

NovaFS benötigt daher eine Datenstruktur, die speziell für große Speicherseiten und blockorientierte Datenträger entwickelt wurde.

---

## 3. Betrachtete Alternativen

### Alternative A – Lineare Listen

Metadateneinträge werden sequentiell gespeichert.

**Vorteile**

- sehr einfache Implementierung
- geringer Verwaltungsaufwand

**Nachteile**

- lineare Suchzeit
- ungeeignet für große Dateisysteme
- schlechte Skalierung

---

### Alternative B – Binäre Suchbäume (AVL / Red-Black)

Jeder Knoten besitzt maximal zwei Kindknoten.

**Vorteile**

- logarithmische Suche
- bewährte Algorithmen

**Nachteile**

- große Baumhöhe
- viele Pointer
- viele Datenträgerzugriffe
- nicht optimal für Massenspeicher

---

### Alternative C – Hash-Tabellen

Direkter Zugriff über Hashwerte.

**Vorteile**

- sehr schnelle Punktabfragen

**Nachteile**

- keine Bereichsabfragen
- keine sortierte Reihenfolge
- ungeeignet für Verzeichnisstrukturen
- schlechte Snapshot-Eigenschaften

---

### Alternative D – B-Bäume

Daten können sowohl in inneren als auch in Blattknoten gespeichert werden.

**Vorteile**

- geringe Baumhöhe
- blockorientiert

**Nachteile**

- komplexere Struktur
- weniger effizient für Bereichsabfragen

---

### Alternative E – B+-Trees

Alle eigentlichen Datensätze befinden sich ausschließlich in den Blattknoten.

Innere Knoten enthalten lediglich Schlüssel zur Navigation.

Die Blattknoten sind zusätzlich miteinander verkettet. Dadurch eignen sich B+-Bäume besonders für blockorientierte Dateisysteme und schnelle Bereichsabfragen.

**Vorteile**

- geringe Baumhöhe
- sehr wenige Datenträgerzugriffe
- effiziente Bereichsabfragen
- optimale Cache-Nutzung
- ideal für Copy-on-Write
- bewährte Technologie

**Nachteile**

- komplexere Implementierung
- Split- und Merge-Operationen notwendig

---

## 4. Entscheidung

NovaFS verwendet **B+-Trees als zentrale Datenstruktur für sämtliche Metadaten**.

Alle persistenten Verwaltungsstrukturen werden als eigenständige B+-Trees implementiert.

Mindestens folgende Bäume werden definiert:

- Root Tree
- Object Tree
- Directory Tree
- Extent Tree
- Free Space Tree
- Checksum Tree
- Protection Policy Tree
- Snapshot Tree
- Cluster Metadata Tree

Jeder Baum besitzt einen eigenen Root-Knoten.

---

## 5. Begründung

Die Wahl eines B+-Trees bietet für NovaFS entscheidende Vorteile.

### Optimierung für Datenträger

Ein einzelner Knoten entspricht einem vollständigen Dateisystemblock.

Dadurch kann ein kompletter Knoten mit einem einzigen Lesevorgang geladen werden.

---

### Geringe Baumhöhe

Durch die hohe Anzahl möglicher Kindknoten bleibt die Baumhöhe selbst bei Milliarden Einträgen sehr klein.

Dadurch reduziert sich die Anzahl notwendiger Datenträgerzugriffe erheblich. 

---

### Effiziente Bereichsabfragen

Die Blattknoten werden miteinander verkettet.

Dadurch können:

- Verzeichnislisten,
- Dateibereiche,
- Snapshots,
- Extentbereiche

ohne erneuten Baumabstieg gelesen werden.

---

### Optimale Cache-Ausnutzung

Innere Knoten enthalten ausschließlich Schlüssel und Kindzeiger.

Dadurch passen mehr Schlüssel in einen Block.

Dies reduziert die Baumhöhe zusätzlich.

---

### Perfekte Integration mit Copy-on-Write

Da jeder Knoten vollständig geschrieben wird, bevor der Parent aktualisiert wird, harmoniert der B+-Tree hervorragend mit dem Copy-on-Write-Modell von NovaFS.

---

### Hohe Erweiterbarkeit

Neue Metadatenstrukturen können jederzeit als weiterer B+-Tree ergänzt werden.

Die Kernarchitektur bleibt dabei unverändert.

---

## 6. Positive Konsequenzen

Die Verwendung von B+-Trees bringt zahlreiche Vorteile.

### Hohe Skalierbarkeit

Auch Dateisysteme mit Milliarden Objekten bleiben performant.

---

### Wenige Datenträgerzugriffe

Durch den hohen Verzweigungsgrad müssen nur wenige Ebenen durchlaufen werden.

---

### Einheitliche Architektur

Alle Metadaten verwenden dieselbe Datenstruktur.

Dadurch sinkt die Komplexität der Implementierung.

---

### Einfache Snapshot-Unterstützung

Neue Generationen benötigen lediglich neue Root-Knoten.

Vorhandene Teilbäume können gemeinsam genutzt werden.

---

### Optimale Grundlage für Cluster

Metadaten können baumweise repliziert werden.

Nur geänderte Knoten müssen übertragen werden.

---

### Gute Parallelisierbarkeit

Unterschiedliche Teilbäume können unabhängig voneinander verarbeitet werden.

---

## 7. Negative Konsequenzen und Risiken

### Höhere Implementierungskomplexität

Ein vollständiger B+-Tree benötigt:

- Split-Operationen,
- Merge-Operationen,
- Balancing,
- Root-Neubildung,
- konsistente Schlüsselverwaltung.

---

### Schreibaufwand

Eine Änderung eines Blattknotens kann durch Copy-on-Write mehrere Ebenen des Baumes betreffen.

Dies erhöht den Schreibaufwand gegenüber einfachen Listen.

---

### Speicherverbrauch

Während einer Transaktion existieren alte und neue Baumknoten gleichzeitig.

Dies ist jedoch ein bewusst akzeptierter Bestandteil des Copy-on-Write-Konzepts.

---

## 8. Auswirkungen auf andere Module

Diese Entscheidung betrifft nahezu sämtliche Kernkomponenten von NovaFS.

Direkt betroffen sind:

- Superblock
- Transaction Manager
- Checkpoint Manager
- Object Manager
- Directory Manager
- Extent Manager
- Allocation Manager
- Snapshot Manager
- Recovery Manager
- Cluster Manager
- Security Manager
- Protection Policy Manager

Alle zukünftigen Metadatenstrukturen SOLLEN ebenfalls als B+-Trees implementiert werden.

---

## 9. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Jeder B+-Tree MUSS einen eindeutigen Root-Knoten besitzen.
- Jeder Baumknoten MUSS genau einem logischen Dateisystemblock entsprechen.
- Alle Blattknoten MÜSSEN auf derselben Baumtiefe liegen.
- Innere Knoten DÜRFEN ausschließlich Schlüssel und Kindzeiger enthalten.
- Nutzdaten MÜSSEN ausschließlich in Blattknoten gespeichert werden.
- Blattknoten MÜSSEN miteinander verkettet sein.
- Änderungen an Baumknoten MÜSSEN ausschließlich über Copy-on-Write erfolgen.
- Jeder Baumknoten MUSS eine Prüfsumme besitzen.
- Jeder Baumknoten MUSS einer Generation zugeordnet sein.
- Root-Knoten DÜRFEN ausschließlich atomar umgeschaltet werden.

---

## 10. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6001 – Extentbasierte Speicherung
- ADR-6012 – Copy-on-Write für Metadaten
- zukünftige ADR – Transaktionsmodell
- zukünftige ADR – Checkpoints
- zukünftige ADR – Snapshots
- zukünftige ADR – Cluster-Metadaten

---

## 11. Statushistorie

- **2026-07-22:** Version 1.0 erstellt.
- **2026-07-22:** B+-Trees als zentrale Metadatenstruktur von NovaFS angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.