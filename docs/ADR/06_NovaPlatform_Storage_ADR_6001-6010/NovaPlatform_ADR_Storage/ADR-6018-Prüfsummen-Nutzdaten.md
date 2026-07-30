# ADR-6018: Prüfsummen für Nutzdaten

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Data Integrity
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS
- **Ersetzt:** Keine

---

## 1. Kontext

NovaFS verfolgt das Ziel, Daten über ihre gesamte Lebensdauer zuverlässig vor unbemerkter Beschädigung zu schützen.

Während Metadaten für die Struktur des Dateisystems verantwortlich sind, stellen die eigentlichen Nutzdaten den größten Wert für den Benutzer dar.

Typische Ursachen für Datenbeschädigungen sind:

- Bit Rot (Silent Data Corruption)
- SSD- oder HDD-Defekte
- fehlerhafte Speichercontroller
- RAM-Fehler
- DMA-Fehler
- Firmwarefehler
- fehlerhafte Übertragungen
- unvollständige Schreibvorgänge

Viele Speichersysteme erkennen diese Fehler nicht automatisch.

NovaFS soll daher sämtliche Nutzdaten durch integrierte Prüfsummen absichern und fehlerhafte Daten frühzeitig erkennen. End-to-End-Prüfsummen gelten als zentrale Technik zur Erkennung stiller Datenkorruption in modernen Speichersystemen. :contentReference[oaicite:0]{index=0}

---

## 2. Problemstellung

Ein Datenträger kann beschädigte Daten liefern, obwohl der Lesevorgang selbst erfolgreich abgeschlossen wurde.

Ohne Prüfsummen kann NovaFS nicht unterscheiden zwischen:

- korrekten Daten,
- beschädigten Daten,
- teilweise überschriebenen Daten,
- falsch adressierten Daten.

Der Benutzer bemerkt Datenfehler häufig erst Monate oder Jahre später.

NovaFS benötigt deshalb eine End-to-End-Integritätsprüfung sämtlicher Nutzdaten.

---

## 3. Betrachtete Alternativen

### Alternative A – Keine Prüfsummen

Nutzdaten werden unverändert gespeichert.

**Vorteile**

- maximale Geschwindigkeit
- kein zusätzlicher Speicherbedarf

**Nachteile**

- keine Fehlererkennung
- keine Selbstheilung
- hohes Risiko stiller Datenkorruption

---

### Alternative B – Prüfsummen pro Datei

Jede Datei besitzt lediglich eine einzelne Prüfsumme.

**Vorteile**

- einfache Implementierung
- geringer Verwaltungsaufwand

**Nachteile**

- Teilfehler innerhalb einer Datei werden erst nach vollständigem Lesen erkannt
- keine Block-Reparatur möglich
- ungeeignet für große Dateien

---

### Alternative C – Prüfsummen pro Datenblock

Jeder Datenblock besitzt eine eigene Prüfsumme.

Die Prüfsummen werden unabhängig gespeichert und beim Lesen überprüft.

**Vorteile**

- frühzeitige Fehlererkennung
- Blockweise Reparatur
- Unterstützung von Self-Healing
- Unterstützung von Erasure Coding
- optimale Integration in Copy-on-Write

**Nachteile**

- zusätzlicher Speicherbedarf
- zusätzlicher CPU-Aufwand

---

## 4. Entscheidung

NovaFS verwendet **Prüfsummen für sämtliche Nutzdatenblöcke**.

Jeder Datenblock erhält eine eigene Prüfsumme.

Beim Lesen MUSS die Prüfsumme überprüft werden.

Stimmt die Prüfsumme nicht mit dem gelesenen Datenblock überein, gilt dieser Block als beschädigt.

Die Prüfsummen werden getrennt von den Nutzdaten im **Checksum Tree** gespeichert.

---

## 5. Begründung

### End-to-End-Integrität

Die Integrität der Daten wird vom Zeitpunkt des Schreibens bis zum Lesen überprüft.

Dadurch können Fehler erkannt werden, die durch Hardware, Firmware oder Übertragungsfehler entstehen. :contentReference[oaicite:1]{index=1}

---

### Früherkennung von Bit Rot

Beschädigte Daten werden bereits beim ersten Lesezugriff erkannt.

Fehler bleiben nicht über Jahre unbemerkt bestehen.

---

### Grundlage für Self-Healing

Erkennt NovaFS einen fehlerhaften Datenblock, kann automatisch versucht werden, diesen wiederherzustellen durch:

- Spiegelkopien,
- Erasure Coding,
- Snapshots,
- Systemgenerationen,
- Backup-Quellen.

---

### Unterstützung von Scrubbing

Während eines Scrubs werden sämtliche Datenblöcke gelesen und ihre Prüfsummen überprüft.

Beschädigungen können erkannt und repariert werden, bevor Anwendungen betroffen sind.

---

### Integration in Copy-on-Write

Bei jeder Änderung entsteht ein neuer Datenblock.

Für diesen neuen Block wird automatisch eine neue Prüfsumme berechnet.

Alte Generationen behalten ihre ursprünglichen Prüfsummen.

---

## 6. Prüfsummenarchitektur

Die Prüfsummen werden logisch getrennt von den Nutzdaten gespeichert.

```text
Datei

│

├── Datenblock A
├── Datenblock B
├── Datenblock C

│

▼

Checksum Tree

├── Checksum A
├── Checksum B
└── Checksum C
```

Dadurch können Prüfsummen unabhängig von den eigentlichen Daten verwaltet werden.

---

## 7. Prüfsummenberechnung

Beim Schreiben:

```text
Anwendung

│

▼

Datenblock erzeugen

│

▼

Prüfsumme berechnen

│

▼

Checksum Tree aktualisieren

│

▼

Datenblock speichern
```

Beim Lesen:

```text
Datenblock lesen

│

▼

Prüfsumme berechnen

│

▼

Checksum Tree lesen

│

▼

Vergleich

│

├── gültig → Daten zurückgeben

└── ungültig → Recovery starten
```

---

## 8. Prüfsummenalgorithmus

NovaFS trennt zwischen schneller Fehlererkennung und kryptographischer Integrität.

Standardalgorithmus:

- CRC32C

Optionale zukünftige Algorithmen:

- xxHash64
- BLAKE3
- SHA-256

Der verwendete Algorithmus wird im Superblock gespeichert und ist Bestandteil der Dateisystemversion.

CRC32C bietet auf moderner Hardware eine sehr schnelle Integritätsprüfung und wird in mehreren Speichersystemen als Standard für Fehlererkennung eingesetzt. :contentReference[oaicite:2]{index=2}

---

## 9. Positive Konsequenzen

Die Verwendung von Datenprüfsummen bietet zahlreiche Vorteile.

### Hohe Datensicherheit

Beschädigte Daten werden zuverlässig erkannt.

---

### Unterstützung automatischer Reparatur

Self-Healing kann fehlerhafte Blöcke gezielt rekonstruieren.

---

### Unterstützung von Erasure Coding

Nur beschädigte Datenblöcke müssen rekonstruiert werden.

---

### Unterstützung von Storage Pools

Integritätsprüfungen funktionieren unabhängig vom Speicherort.

---

### Schutz großer Dateien

Auch einzelne beschädigte Blöcke innerhalb großer Dateien werden erkannt.

---

### Grundlage für zukünftige Merkle Trees

Die Blockprüfsummen können später als Basis eines kryptographischen Merkle-Baums verwendet werden.

---

## 10. Negative Konsequenzen und Risiken

### Zusätzlicher Speicherbedarf

Für jeden Datenblock muss eine Prüfsumme gespeichert werden.

---

### Zusätzlicher CPU-Aufwand

Beim Lesen und Schreiben müssen Prüfsummen berechnet werden.

Durch Hardwarebeschleunigung bleibt dieser Aufwand gering.

---

### Prüfsummen ersetzen keine Redundanz

Prüfsummen erkennen Fehler.

Zur Wiederherstellung werden zusätzlich benötigt:

- Spiegelung,
- Erasure Coding,
- Snapshots,
- Backups.

---

## 11. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Checksum Tree
- Extent Manager
- Object Manager
- Recovery Manager
- Self-Healing Manager
- Scrubbing Manager
- Storage Pool Manager
- Cluster Manager
- Erasure Coding Manager
- Snapshot Manager

Alle zukünftigen Datenspeicherformate MÜSSEN Datenprüfsummen unterstützen.

---

## 12. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Jeder Datenblock MUSS eine Prüfsumme besitzen.
- Die Prüfsumme MUSS unmittelbar vor dem Schreiben berechnet werden.
- Die Prüfsumme MUSS bei jedem Lesen überprüft werden.
- Prüfsummen MÜSSEN getrennt von den Nutzdaten gespeichert werden.
- Beschädigte Datenblöcke DÜRFEN nicht ungeprüft an Anwendungen zurückgegeben werden.
- Bei einer fehlerhaften Prüfsumme SOLL automatisch eine Wiederherstellung eingeleitet werden.
- Der verwendete Prüfsummenalgorithmus MUSS im Superblock dokumentiert sein.
- Neue Dateiformate und Speichertypen MÜSSEN die Prüfsummenarchitektur unterstützen.

---

## 13. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6001 – Extentbasierte Speicherung
- ADR-6012 – Copy-on-Write für Metadaten
- ADR-6013 – B+-Trees als zentrale Datenstruktur
- ADR-6015 – Transaktionales Journaling
- ADR-6017 – Prüfsummen für Metadaten
- zukünftige ADR – Self-Healing
- zukünftige ADR – Scrubbing
- zukünftige ADR – Merkle Tree

---

## 14. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Prüfsummen für sämtliche Nutzdaten als verpflichtender Bestandteil der NovaFS-Architektur angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.