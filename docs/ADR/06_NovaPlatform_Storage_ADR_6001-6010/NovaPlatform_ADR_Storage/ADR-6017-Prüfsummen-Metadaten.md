# ADR-6017: Prüfsummen für Metadaten

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Data Integrity
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS
- **Ersetzt:** Keine

---

## 1. Kontext

NovaFS ist als hochzuverlässiges Dateisystem konzipiert.

Eine der größten Gefahren moderner Speichersysteme sind **stille Datenfehler (Silent Data Corruption bzw. Bit Rot)**.

Besonders kritisch sind dabei Metadaten.

Bereits die Beschädigung eines einzigen Metadatenblocks kann dazu führen, dass:

- komplette Verzeichnisbäume unzugänglich werden,
- Dateien nicht mehr gefunden werden,
- Extents verloren gehen,
- Snapshots beschädigt werden,
- ganze Dateisysteme nicht mehr mountbar sind.

NovaFS benötigt daher einen Mechanismus, der beschädigte Metadaten bereits beim Lesen erkennt.

Moderne Dateisysteme wie ext4 und XFS schützen ihre Metadaten ebenfalls durch Prüfsummen, um beschädigte Metadatenblöcke zuverlässig erkennen zu können. :contentReference[oaicite:0]{index=0}

---

## 2. Problemstellung

Massenspeicher liefern nicht immer exakt dieselben Daten zurück, die zuvor geschrieben wurden.

Mögliche Ursachen sind:

- Bitfehler
- SSD-Defekte
- HDD-Oberflächenfehler
- RAM-Fehler
- DMA-Fehler
- Firmwarefehler
- fehlerhafte Controller
- unvollständige Schreibvorgänge
- Übertragungsfehler

Ohne Integritätsprüfung kann das Dateisystem beschädigte Metadaten nicht von gültigen unterscheiden.

NovaFS benötigt daher eine integrierte Prüfsummenprüfung.

---

## 3. Betrachtete Alternativen

### Alternative A – Keine Prüfsummen

Metadaten werden ohne Integritätsprüfung gespeichert.

**Vorteile**

- geringster Speicherbedarf
- höchste Schreibgeschwindigkeit

**Nachteile**

- beschädigte Metadaten bleiben unentdeckt
- keine Selbstheilung möglich
- hohes Risiko von Dateisystemfehlern

---

### Alternative B – Prüfsummen nur für den Superblock

Nur Superblöcke besitzen Prüfsummen.

**Vorteile**

- einfache Implementierung
- Schutz des Dateisystemstarts

**Nachteile**

- übrige Metadaten bleiben ungeschützt
- beschädigte B+-Trees werden nicht erkannt

---

### Alternative C – Prüfsummen für sämtliche Metadaten

Jeder Metadatenblock besitzt eine eigene Prüfsumme.

Die Prüfsumme wird bei jedem Lesezugriff überprüft.

**Vorteile**

- vollständiger Schutz aller Metadaten
- frühzeitige Fehlererkennung
- Grundlage für Selbstheilung
- Unterstützung von Scrubbing
- Integration in Copy-on-Write

**Nachteile**

- geringer zusätzlicher Speicherbedarf
- kleiner zusätzlicher CPU-Aufwand

---

## 4. Entscheidung

NovaFS verwendet **Prüfsummen für sämtliche Metadatenblöcke**.

Jeder Metadatenblock erhält eine eigene Prüfsumme.

Beim Lesen MUSS die Prüfsumme überprüft werden.

Stimmt die Prüfsumme nicht mit dem gelesenen Block überein, gilt der Block als beschädigt.

---

## 5. Begründung

### Früherkennung von Beschädigungen

Beschädigte Metadaten werden unmittelbar beim Lesen erkannt.

Ein beschädigter Block wird niemals ungeprüft weiterverarbeitet.

---

### Grundlage für Selbstheilung

Erst durch eine Prüfsumme kann NovaFS erkennen, welche Kopie eines Metadatenblocks gültig ist.

Anschließend kann automatisch:

- eine Spiegelkopie,
- eine Erasure-Coding-Rekonstruktion,
- ein Snapshot,
- oder eine ältere Generation

zur Wiederherstellung verwendet werden.

---

### Unterstützung von Scrubbing

Während eines Scrubs können sämtliche Metadaten vollständig überprüft werden.

Fehler werden erkannt, bevor Anwendungen darauf zugreifen.

---

### Integration in Copy-on-Write

Da Metadaten niemals überschrieben werden, wird für jeden neu erzeugten Block automatisch eine neue Prüfsumme berechnet.

Alte Generationen behalten ihre ursprünglichen Prüfsummen.

---

### Schutz gegen Fehladressierung

Neben der Prüfsumme KANN ein Metadatenblock zusätzliche Identifikationsinformationen enthalten:

- Dateisystem-UUID
- Generation
- Blocktyp
- Blockadresse
- Owner-ID

Damit kann NovaFS erkennen, ob zwar ein gültiger Block gelesen wurde, dieser jedoch an einer falschen Position liegt. Moderne Dateisysteme wie XFS verwenden hierfür zusätzlich zu Prüfsummen auch UUIDs, Eigentümerinformationen und Blockadressen. :contentReference[oaicite:1]{index=1}

---

## 6. Betroffene Metadaten

Mindestens folgende Strukturen besitzen Prüfsummen:

- Superblock
- Journal
- Checkpoints
- Root Tree
- Object Tree
- Directory Tree
- Extent Tree
- Free Space Tree
- Checksum Tree
- Snapshot Tree
- Protection Policy Tree
- Cluster Metadata Tree
- Allocation Maps
- Object Header
- Inodes
- Extent-Listen

Neue Metadatenstrukturen SOLLEN grundsätzlich ebenfalls Prüfsummen verwenden.

---

## 7. Prüfsummenberechnung

Für jeden Metadatenblock wird eine Prüfsumme über den vollständigen Blockinhalt berechnet.

Die Berechnung erfolgt unmittelbar vor dem Schreiben.

Beim Lesen wird dieselbe Prüfsumme erneut berechnet und verglichen.

```text
Metadatenblock
        │
        ▼
Prüfsumme berechnen
        │
        ▼
Prüfsumme speichern
        │
        ▼
Datenträger
```

Beim Lesen:

```text
Block lesen
      │
      ▼
Prüfsumme neu berechnen
      │
      ▼
Vergleich
      │
 ┌────┴────┐
 │         │
Gültig  Ungültig
 │         │
 ▼         ▼
Weiter  Recovery
```

---

## 8. Prüfsummenalgorithmus

NovaFS trennt bewusst zwischen Fehlererkennung und kryptographischer Integrität.

Für Metadaten wird standardmäßig ein schneller, hardwarefreundlicher Algorithmus verwendet.

Empfohlener Standard:

- CRC32C

Optionale zukünftige Erweiterungen:

- xxHash64
- BLAKE3
- SHA-256 (für kryptographische Integrität)

Der verwendete Algorithmus wird im Superblock gespeichert und ist Bestandteil der Dateisystemversion. CRC32C wird beispielsweise auch in ext4 für Metadatenprüfsummen verwendet. :contentReference[oaicite:2]{index=2}

---

## 9. Positive Konsequenzen

Die Verwendung von Metadatenprüfsummen bietet zahlreiche Vorteile.

### Frühe Fehlererkennung

Beschädigungen werden sofort erkannt.

---

### Hohe Zuverlässigkeit

Beschädigte Metadaten gelangen nicht in die weitere Verarbeitung.

---

### Unterstützung automatischer Reparatur

Nur mit Prüfsummen kann NovaFS eindeutig bestimmen, welche Blockkopie gültig ist.

---

### Grundlage für Self-Healing

Automatische Reparaturen können vollständig ohne Benutzereingriff erfolgen.

---

### Unterstützung von Storage Pools

Auch in verteilten Speichersystemen können beschädigte Blöcke zuverlässig erkannt werden.

---

## 10. Negative Konsequenzen und Risiken

### Zusätzlicher Speicherbedarf

Jeder Metadatenblock benötigt Platz für seine Prüfsumme.

Der Aufwand ist gering.

---

### Zusätzlicher CPU-Aufwand

Bei jedem Lesen und Schreiben muss die Prüfsumme berechnet werden.

Moderne Prozessoren besitzen hierfür häufig Hardwareunterstützung.

---

### Prüfsummen ersetzen keine Redundanz

Eine Prüfsumme erkennt lediglich Fehler.

Zur Wiederherstellung sind zusätzlich erforderlich:

- Spiegelung,
- Erasure Coding,
- Snapshots,
- oder Backups.

---

## 11. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Superblock Manager
- Transaction Manager
- Journal Manager
- Checkpoint Manager
- Object Manager
- Recovery Manager
- Self-Healing Manager
- Scrubbing Manager
- Storage Pool Manager
- Cluster Manager

Alle zukünftigen Metadatenstrukturen MÜSSEN Prüfsummen unterstützen.

---

## 12. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Jeder Metadatenblock MUSS eine Prüfsumme besitzen.
- Die Prüfsumme MUSS vor jedem Schreiben neu berechnet werden.
- Die Prüfsumme MUSS bei jedem Lesen überprüft werden.
- Ungültige Metadatenblöcke DÜRFEN nicht verwendet werden.
- Beschädigte Metadaten SOLLEN automatisch repariert werden.
- Prüfsummen MÜSSEN Bestandteil jeder Dateisystemgeneration sein.
- Neue Metadatenstrukturen MÜSSEN Prüfsummen unterstützen.
- Der verwendete Prüfsummenalgorithmus MUSS im Superblock gespeichert werden.

---

## 13. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6001 – Extentbasierte Speicherung
- ADR-6012 – Copy-on-Write für Metadaten
- ADR-6013 – B+-Trees als zentrale Datenstruktur
- ADR-6015 – Transaktionales Journaling
- ADR-6016 – Mehrere Superblöcke
- zukünftige ADR – Self-Healing
- zukünftige ADR – Scrubbing
- zukünftige ADR – Datenprüfsummen

---

## 14. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Metadatenprüfsummen als verpflichtender Bestandteil der NovaFS-Architektur angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.