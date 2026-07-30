# ADR-6016: Mehrere Superblöcke

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Filesystem Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS
- **Ersetzt:** Keine

---

## 1. Kontext

Der Superblock bildet das Herzstück eines Dateisystems.

Er enthält die grundlegenden Informationen, die zum Mounten und Verwalten des Dateisystems erforderlich sind, unter anderem:

- Dateisystemkennung
- Versionsinformationen
- Blockgröße
- UUID
- Position der Root-Bäume
- Checkpoint-Informationen
- Generation
- Prüfsummen
- Feature-Flags

Fällt der Superblock durch einen Hardwarefehler oder beschädigte Datenträgersektoren aus, kann das gesamte Dateisystem unter Umständen nicht mehr eingebunden werden.

NovaFS soll deshalb keinen einzelnen Superblock verwenden, sondern mehrere redundante Superblöcke.

---

## 2. Problemstellung

Ein einzelner Superblock stellt einen **Single Point of Failure** dar.

Mögliche Ursachen für eine Beschädigung sind:

- Stromausfälle
- SSD-/HDD-Defekte
- Bitfehler
- Firmwarefehler
- fehlerhafte Speichercontroller
- fehlerhafte Schreibvorgänge
- Benutzerfehler

Ist nur ein Superblock vorhanden, kann das Dateisystem möglicherweise nicht mehr erkannt oder eingebunden werden.

NovaFS benötigt daher mehrere voneinander unabhängige Kopien.

---

## 3. Betrachtete Alternativen

### Alternative A – Ein einzelner Superblock

Nur ein zentraler Superblock wird gespeichert.

**Vorteile**

- sehr einfache Implementierung
- geringer Speicherbedarf

**Nachteile**

- Single Point of Failure
- hohe Ausfallwahrscheinlichkeit
- schwierige Recovery

---

### Alternative B – Primärer und Backup-Superblock

Ein Haupt- und ein Sicherungssuperblock werden gespeichert.

**Vorteile**

- einfache Redundanz
- bewährtes Konzept

**Nachteile**

- nur eine Sicherung
- beide Kopien können gemeinsam beschädigt werden

---

### Alternative C – Mehrere verteilte Superblöcke

Mehrere vollständige Superblöcke werden an unterschiedlichen Positionen innerhalb des Dateisystems gespeichert.

Jeder Superblock besitzt:

- identische Struktur
- eigene Prüfsumme
- Generation
- Sequenznummer

**Vorteile**

- hohe Ausfallsicherheit
- schnelle Recovery
- Schutz gegen lokale Datenträgerfehler
- Grundlage für Selbstheilung

**Nachteile**

- etwas höherer Speicherbedarf
- zusätzlicher Verwaltungsaufwand

---

## 4. Entscheidung

NovaFS verwendet **mehrere redundante Superblöcke**, die an unterschiedlichen Positionen des Dateisystems gespeichert werden.

Jeder Superblock enthält die vollständigen Informationen zum Mounten des Dateisystems.

Beim Einbinden durchsucht NovaFS alle bekannten Superblockpositionen und wählt automatisch den **neuesten gültigen Superblock** aus.

---

## 5. Begründung

### Keine zentrale Fehlerquelle

Durch mehrere vollständige Superblöcke bleibt das Dateisystem auch dann mountbar, wenn einzelne Kopien beschädigt sind.

---

### Erhöhte Datensicherheit

Lokale Defekte einzelner Speicherbereiche führen nicht zum vollständigen Ausfall des Dateisystems.

---

### Grundlage für Selbstheilung

Beschädigte Superblöcke können automatisch durch gültige Kopien ersetzt werden.

---

### Unterstützung von Checkpoints

Jeder neue Checkpoint erzeugt einen aktualisierten Superblock mit einer höheren Generation und Sequenznummer.

---

### Unterstützung von Systemgenerationen

Auch beim Wechsel zwischen Betriebssystemgenerationen bleibt jederzeit mindestens ein gültiger Superblock erhalten.

---

## 6. Aufbau eines Superblocks

Jeder Superblock enthält mindestens folgende Informationen:

- Dateisystem-Signatur
- NovaFS-Version
- UUID
- Blockgröße
- Gesamtgröße
- Generation
- Sequenznummer
- Root Tree
- Object Tree
- Checkpoint Tree
- Journalposition
- Feature-Flags
- Zeitstempel
- Prüfsumme

---

### Beispielstruktur

```text
NovaFS Superblock

├── Magic Number
├── Filesystem UUID
├── Version
├── Block Size
├── Generation
├── Sequence Number
├── Root Tree Pointer
├── Object Tree Pointer
├── Journal Pointer
├── Checkpoint Pointer
├── Feature Flags
├── Timestamp
└── Checksum
```

---

## 7. Speicherorte

Die Positionen der Superblöcke sind fest definiert.

Beispielsweise:

```text
Superblock 0
Superblock 1
Superblock 2
Superblock 3
```

Sie SOLLEN möglichst weit voneinander entfernt gespeichert werden.

Auf großen Datenträgern KÖNNEN zusätzliche Superblöcke angelegt werden.

Bei Storage-Pools SOLL zusätzlich jeder Datenträger mindestens einen eigenen lokalen Superblock besitzen.

---

## 8. Auswahl beim Mounten

Beim Einbinden des Dateisystems erfolgt folgende Reihenfolge:

1. Alle bekannten Superblöcke lesen.
2. Prüfsummen überprüfen.
3. Signatur prüfen.
4. Versionskompatibilität prüfen.
5. Höchste Generation bestimmen.
6. Höchste Sequenznummer bestimmen.
7. Neuesten gültigen Superblock auswählen.

Ungültige oder beschädigte Superblöcke werden ignoriert.

---

## 9. Selbstheilung

Nach erfolgreichem Mounten überprüft NovaFS alle Superblockkopien.

Beschädigte Kopien können automatisch ersetzt werden.

Der Ablauf lautet:

```text
Superblock prüfen
        │
        ▼
Prüfsumme gültig?
        │
   Ja ───── Nein
    │          │
    ▼          ▼
 Weiter     Neu erzeugen
                │
                ▼
 Aus gültigem Superblock kopieren
```

Dieser Vorgang erfolgt ausschließlich transaktional.

---

## 10. Positive Konsequenzen

Die Verwendung mehrerer Superblöcke bietet zahlreiche Vorteile.

### Hohe Ausfallsicherheit

Der Ausfall einzelner Speicherbereiche verhindert nicht das Mounten des Dateisystems.

---

### Schnelle Recovery

Ein gültiger Superblock kann sofort verwendet werden.

---

### Automatische Reparatur

Beschädigte Kopien können ohne Benutzereingriff ersetzt werden.

---

### Unterstützung großer Storage-Pools

Jeder Datenträger besitzt eigene Einstiegspunkte.

---

### Gute Skalierbarkeit

Weitere Superblöcke können zukünftigen Dateisystemversionen hinzugefügt werden.

---

## 11. Negative Konsequenzen und Risiken

### Höherer Speicherbedarf

Mehrere vollständige Superblöcke benötigen zusätzlichen Speicherplatz.

Dieser Aufwand ist im Verhältnis zur Gesamtkapazität vernachlässigbar.

---

### Zusätzlicher Verwaltungsaufwand

Bei jedem Checkpoint müssen alle relevanten Superblöcke aktualisiert werden.

---

### Konsistenzprüfung erforderlich

Beim Mounten müssen Generation, Sequenznummer und Prüfsummen verglichen werden.

---

## 12. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Mount Manager
- Recovery Manager
- Checkpoint Manager
- Journal Manager
- Transaction Manager
- Storage Pool Manager
- Cluster Manager
- Self-Healing Manager

Alle zukünftigen Erweiterungen MÜSSEN mehrere Superblöcke unterstützen.

---

## 13. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Das Dateisystem MUSS mindestens vier redundante Superblöcke besitzen.
- Jeder Superblock MUSS vollständig sein.
- Jeder Superblock MUSS eine eigene Prüfsumme besitzen.
- Jeder Superblock MUSS eine Generation besitzen.
- Jeder Superblock MUSS eine Sequenznummer besitzen.
- Jeder Superblock MUSS transaktional aktualisiert werden.
- Der Mount-Vorgang MUSS den neuesten gültigen Superblock auswählen.
- Beschädigte Superblöcke DÜRFEN nicht verwendet werden.
- Beschädigte Superblöcke SOLLEN automatisch ersetzt werden.
- Jeder Storage-Pool MUSS mindestens einen gültigen Superblock pro Datenträger besitzen.

---

## 14. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6001 – Extentbasierte Speicherung
- ADR-6012 – Copy-on-Write für Metadaten
- ADR-6013 – B+-Trees als zentrale Datenstruktur
- ADR-6015 – Transaktionales Journaling
- zukünftige ADR – Checkpoints
- zukünftige ADR – Self-Healing
- zukünftige ADR – Storage Pools

---

## 15. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Mehrere redundante Superblöcke als verbindlicher Bestandteil der NovaFS-Architektur angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.