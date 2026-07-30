# ADR-6012: Copy-on-Write für Metadaten

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Metadata Architecture
- **Erstellt:** 2026-07-22
- **Gültig für:** NovaFS
- **Ersetzt:** Keine

---

## 1. Kontext

NovaFS ist als transaktionales Dateisystem konzipiert und soll hohe Zuverlässigkeit, atomare Änderungen, Snapshot-Unterstützung, Selbstheilung sowie Live-Systemupdates ermöglichen.

Metadaten gehören zu den kritischsten Bestandteilen eines Dateisystems. Bereits eine teilweise beschädigte Verzeichnisstruktur oder ein unvollständig geschriebener B+-Tree kann das gesamte Dateisystem unbenutzbar machen.

NovaFS benötigt deshalb ein Verfahren, das sicherstellt, dass Metadaten niemals in einem inkonsistenten Zustand auf den Datenträger geschrieben werden.

---

## 2. Problemstellung

Klassische Dateisysteme überschreiben Metadaten häufig direkt an ihrer ursprünglichen Position (In-Place Update).

Wird ein Schreibvorgang beispielsweise durch einen Stromausfall oder einen Hardwarefehler unterbrochen, können folgende Probleme entstehen:

- beschädigte Verzeichnisstrukturen,
- unvollständige B+-Trees,
- fehlerhafte Blockzuordnungen,
- ungültige Zeiger,
- inkonsistente Prüfsummen,
- nicht mehr erreichbare Dateien.

Zusätzliche Journale können diese Risiken zwar reduzieren, erhöhen jedoch die Schreiblast und vergrößern die Komplexität der Wiederherstellung.

NovaFS soll Metadaten grundsätzlich atomar aktualisieren.

---

## 3. Betrachtete Alternativen

### Alternative A – In-Place Updates

Metadaten werden direkt überschrieben.

**Vorteile**

- geringer Speicherbedarf
- einfache Implementierung

**Nachteile**

- hohe Gefahr inkonsistenter Strukturen
- schwierige Crash-Recovery
- Snapshots benötigen zusätzliche Mechanismen

---

### Alternative B – Journaling

Vor jeder Änderung werden die Metadaten in ein Journal geschrieben.

**Vorteile**

- bewährtes Verfahren
- gute Wiederherstellbarkeit

**Nachteile**

- doppelte Schreibvorgänge
- Journal muss regelmäßig bereinigt werden
- zusätzlicher Verwaltungsaufwand

---

### Alternative C – Copy-on-Write (CoW)

Metadaten werden niemals überschrieben.

Stattdessen werden:

- neue Blöcke erzeugt,
- vollständig geschrieben,
- geprüft,
- anschließend atomar referenziert.

**Vorteile**

- atomare Änderungen
- ideale Grundlage für Snapshots
- einfache Recovery
- keine teilweise überschriebenen Strukturen

**Nachteile**

- höherer Verwaltungsaufwand
- temporär erhöhter Speicherbedarf

---

## 4. Entscheidung

NovaFS verwendet für **sämtliche Metadaten ausschließlich Copy-on-Write**.

Bereits vorhandene Metadatenblöcke dürfen niemals direkt überschrieben werden.

Jede Änderung erzeugt neue Metadatenblöcke.

Erst nachdem alle neuen Blöcke erfolgreich geschrieben und geprüft wurden, wird der neue Wurzelknoten (Root) atomar aktiviert.

---

## 5. Begründung

Copy-on-Write bildet die Grundlage nahezu aller modernen Funktionen von NovaFS.

### Atomare Änderungen

Eine Metadatenänderung wird erst sichtbar, wenn sie vollständig abgeschlossen wurde.

Es existiert niemals ein teilweise aktualisierter Baum.

---

### Sichere Crash-Recovery

Kommt es während einer Transaktion zu einem Stromausfall, bleiben die bisherigen Metadaten vollständig erhalten.

Nach dem Neustart verwendet NovaFS automatisch den letzten gültigen Checkpoint.

---

### Snapshot-Unterstützung

Snapshots können vorhandene Metadaten gemeinsam verwenden.

Da bestehende Blöcke unverändert bleiben, entstehen Snapshots nahezu ohne zusätzlichen Aufwand.

---

### Rollback

Fehlerhafte Änderungen können verworfen werden.

Da ältere Metadaten unverändert existieren, genügt das Umschalten auf einen älteren Root-Knoten.

---

### Live-Systemupdates

Unveränderliche Systemgenerationen basieren direkt auf diesem Verfahren.

Neue Generationen erzeugen neue Metadatenstrukturen, während bestehende Generationen unverändert bleiben.

---

### Integritätsprüfung

Neue Metadaten können vollständig geprüft werden, bevor sie aktiviert werden.

Dies umfasst:

- Prüfsummen,
- Strukturprüfung,
- Referenzprüfung,
- Konsistenzprüfung.

---

## 6. Positive Konsequenzen

Die Verwendung von Copy-on-Write bietet zahlreiche Vorteile.

### Höhere Datensicherheit

Beschädigte Teilstrukturen entstehen praktisch nicht mehr.

---

### Atomare Transaktionen

Entweder wird eine komplette Änderung sichtbar oder überhaupt keine.

Zwischenzustände existieren nicht.

---

### Einfache Recovery

Das Dateisystem benötigt lediglich den letzten gültigen Checkpoint.

Ein langwieriges Journal-Replay entfällt weitgehend.

---

### Effiziente Snapshots

Snapshots bestehen hauptsächlich aus zusätzlichen Root-Referenzen.

Bereits vorhandene Metadaten werden gemeinsam genutzt.

---

### Unveränderliche Generationen

System- und Recovery-Generationen können dauerhaft unverändert gespeichert werden.

---

### Unterstützung von Selbstheilung

Beschädigte Metadaten können mit älteren Generationen verglichen und bei Bedarf wiederhergestellt werden.

---

## 7. Negative Konsequenzen und Risiken

### Erhöhter Speicherbedarf

Während einer laufenden Transaktion existieren alte und neue Metadaten gleichzeitig.

Dieser Mehrbedarf ist jedoch zeitlich begrenzt.

---

### Komplexere Speicherverwaltung

Nicht mehr referenzierte Metadaten müssen später sicher freigegeben werden.

NovaFS benötigt hierfür:

- Referenzzählung,
- Generation Tracking,
- Garbage Collection.

---

### Mehr Schreibvorgänge

Eine Änderung erzeugt neue Blöcke anstatt bestehende zu überschreiben.

Durch moderne SSDs und extentbasierte Speicherung wird dieser Nachteil weitgehend kompensiert.

---

## 8. Auswirkungen auf andere Module

Diese Entscheidung betrifft nahezu alle Kernkomponenten von NovaFS.

Direkt betroffen sind:

- Superblock
- Checkpoints
- Transaction Manager
- Root Tree
- Object Tree
- Directory Tree
- Extent Tree
- Free Space Tree
- Checksum Tree
- Snapshot Manager
- Recovery Manager
- Live Update Manager
- System Generation Manager
- Garbage Collector

Darüber hinaus basiert auch die Cluster-Replikation auf unveränderlichen Metadatenversionen.

---

## 9. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Metadatenblöcke DÜRFEN niemals direkt überschrieben werden.
- Jede Änderung MUSS neue Blöcke erzeugen.
- Neue Metadaten MÜSSEN vollständig geschrieben werden, bevor sie referenziert werden.
- Jeder neue Metadatenblock MUSS eine Prüfsumme besitzen.
- Jeder Metadatenblock MUSS einer Generation zugeordnet sein.
- Root-Knoten DÜRFEN ausschließlich atomar umgeschaltet werden.
- Alte Metadaten DÜRFEN erst freigegeben werden, wenn keine Snapshots, Generationen oder laufenden Prozesse mehr darauf verweisen.
- Copy-on-Write MUSS für sämtliche B+-Trees einheitlich verwendet werden.
- Die Aktivierung neuer Metadaten MUSS ausschließlich über einen gültigen Checkpoint erfolgen.

---

## 10. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-0001 – Extentbasierte Speicherung
- zukünftige ADR – Transaktionsmodell
- zukünftige ADR – Checkpoints
- zukünftige ADR – Snapshots
- zukünftige ADR – Live-Systemgenerationen

---

## 11. Statushistorie

- **2026-07-22:** Version 1.0 erstellt.
- **2026-07-22:** Als verbindliches Metadatenmodell für NovaFS angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.