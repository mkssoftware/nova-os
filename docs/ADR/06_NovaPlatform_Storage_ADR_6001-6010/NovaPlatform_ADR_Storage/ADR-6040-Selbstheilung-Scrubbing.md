# ADR-6040: Selbstheilung und Scrubbing

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Reliability Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS / Nova OS
- **Ersetzt:** Keine

---

# 1. Kontext

NovaFS wurde von Beginn an mit dem Ziel entwickelt, Daten nicht nur zu speichern, sondern ihre Integrität während der gesamten Lebensdauer sicherzustellen.

Bereits vorhandene Komponenten sind:

- Copy-on-Write
- Prüfsummen
- Merkle-Trees
- Objektbasiertes Dateisystem
- Storage Pools
- Redundanz
- Snapshots
- Recovery-System
- Transaktionen
- Protection Policies

Diese Komponenten ermöglichen den Aufbau einer vollständig integrierten **Selbstheilungs- und Scrubbing-Architektur**.

NovaFS soll beschädigte Daten nicht nur erkennen, sondern – sofern eine gültige Kopie vorhanden ist – automatisch reparieren.

---

# 2. Problemstellung

Datenträger altern.

Dabei können auftreten:

- Bit Rot
- stille Datenkorruption
- fehlerhafte Speicherzellen
- Übertragungsfehler
- Controllerfehler
- RAM-Fehler
- beschädigte Metadaten
- teilweise defekte Datenträger

Werden Daten nur beim Zugriff geprüft, können ungenutzte Daten über Jahre unbemerkt beschädigt bleiben.

NovaFS benötigt deshalb:

- kontinuierliche Integritätsprüfung
- automatische Reparatur
- periodische Hintergrundprüfung
- vollständige Protokollierung

Moderne Copy-on-Write-Dateisysteme wie Btrfs und ZFS nutzen regelmäßiges Scrubbing zur Prüfung aller Daten- und Metadatenblöcke anhand ihrer Prüfsummen. Liegt eine redundante, gültige Kopie vor, kann eine beschädigte Kopie automatisch ersetzt werden. :contentReference[oaicite:0]{index=0}

---

# 3. Betrachtete Alternativen

## Alternative A – Keine Integritätsprüfung

Daten werden ausschließlich gelesen.

**Vorteile**

- keine Hintergrundlast
- einfache Implementierung

**Nachteile**

- Bit Rot bleibt unentdeckt
- stille Datenkorruption
- keine automatische Reparatur

---

## Alternative B – Manuelle Dateisystemprüfung

Der Administrator startet regelmäßig Prüfprogramme.

**Vorteile**

- geringer Implementierungsaufwand

**Nachteile**

- keine kontinuierliche Überwachung
- hoher Verwaltungsaufwand
- Fehler bleiben lange unentdeckt

---

## Alternative C – Automatische Selbstheilung mit Scrubbing

NovaFS überprüft kontinuierlich sämtliche Datenobjekte.

Beschädigte Daten werden automatisch repariert, sofern eine gültige Kopie existiert.

**Vorteile**

- frühzeitige Fehlererkennung
- automatische Reparatur
- hohe Datenintegrität
- geringer Administrationsaufwand

**Nachteile**

- zusätzliche Hintergrund-I/O
- höherer Implementierungsaufwand

---

# 4. Entscheidung

NovaFS verwendet eine vollständig integrierte **Selbstheilungs- und Scrubbing-Architektur**.

Das System besteht aus zwei Komponenten:

- Self-Healing Manager
- Scrubbing Manager

Beide arbeiten eng mit:

- Merkle-Trees
- Prüfsummen
- Storage Pools
- Replikation
- Recovery
- Snapshots

zusammen.

---

# 5. Begründung

## Früherkennung

Beschädigungen werden erkannt, bevor Anwendungen auf die Daten zugreifen.

---

## Automatische Reparatur

Existiert eine gültige redundante Kopie, erfolgt die Wiederherstellung automatisch.

---

## Permanente Integrität

Das Dateisystem überprüft regelmäßig sämtliche Daten.

---

## Geringerer Administrationsaufwand

Manuelle Integritätsprüfungen werden weitgehend überflüssig.

---

## Integration

Die Architektur nutzt bereits vorhandene NovaFS-Komponenten.

---

# 6. Architektur

```text
Storage Object

        │

Checksum

        │

Merkle Tree

        │

Scrubbing Manager

        │

Self-Healing Manager

        │

Storage Pool
```

---

# 7. Komponenten

NovaFS definiert folgende Komponenten:

- Scrubbing Manager
- Self-Healing Manager
- Checksum Manager
- Merkle Manager
- Recovery Manager
- Storage Pool Manager
- Replication Manager
- Event Manager

---

# 8. Scrubbing

Beim Scrubbing werden sämtliche Objekte gelesen.

Für jedes Objekt erfolgt:

```text
Objekt lesen

↓

Prüfsumme berechnen

↓

Merkle prüfen

↓

Integrität OK?

↓

Ja → nächstes Objekt

Nein → Self-Healing
```

Scrubbing ist ausdrücklich **keine Dateisystemreparatur (fsck)**. Es dient der Überprüfung von Daten- und Metadaten anhand ihrer Integritätsinformationen und kann bei vorhandener Redundanz beschädigte Kopien ersetzen. :contentReference[oaicite:1]{index=1}

---

# 9. Selbstheilung

Kann ein beschädigtes Objekt erkannt werden, beginnt automatisch die Wiederherstellung.

```text
Objekt beschädigt

↓

Alternative Kopie suchen

↓

Prüfen

↓

Neue Kopie erzeugen

↓

Metadaten aktualisieren

↓

Commit
```

Alle Reparaturen erfolgen transaktional.

---

# 10. Fehlerquellen

Die Selbstheilung unterstützt unter anderem:

- beschädigte Daten
- beschädigte Metadaten
- fehlerhafte Prüfsummen
- defekte Spiegel
- teilweise ausgefallene Geräte
- beschädigte Replikate

---

# 11. Reparaturquellen

Eine Reparatur kann erfolgen durch:

- Spiegel
- RAID-Replikate
- Storage Pool
- Snapshot
- Recovery-System
- Backup
- Cluster-Replikation

Existiert keine gültige Kopie, wird der Fehler protokolliert und an den Administrator gemeldet.

---

# 12. Periodisches Scrubbing

NovaFS unterstützt:

- tägliches Scrubbing
- wöchentliches Scrubbing
- monatliches Scrubbing
- benutzerdefinierte Intervalle
- manuelles Scrubbing

Standardmäßig wird ein **monatliches vollständiges Scrubbing** empfohlen, analog zu bewährten Verfahren moderner Dateisysteme. :contentReference[oaicite:2]{index=2}

---

# 13. Online-Scrubbing

Scrubbing läuft vollständig im Hintergrund.

```text
Systembetrieb

↓

Scrubbing

↓

Priorisierung

↓

Niedrige IO-Priorität

↓

Weiterarbeiten
```

Anwendungen bleiben uneingeschränkt nutzbar.

---

# 14. Integration mit Merkle-Trees

Jeder gelesene Datenpfad wird zusätzlich über den Merkle-Tree validiert.

```text
Objekt

↓

Checksum

↓

Merkle Path

↓

Root Hash

↓

Validierung
```

Dadurch wird sowohl die Integrität des Objekts als auch der Objektstruktur geprüft.

---

# 15. Integration mit Snapshots

Snapshots werden ebenfalls überprüft.

Gemeinsam genutzte Objekte werden nur einmal validiert.

Dadurch bleibt der Prüfaufwand gering.

---

# 16. Ereignisse

Der Event Manager erzeugt unter anderem:

- Scrubbing gestartet
- Scrubbing abgeschlossen
- Objekt repariert
- Objekt beschädigt
- Reparatur fehlgeschlagen
- Datenträgerfehler erkannt
- Integritätswarnung

---

# 17. Positive Konsequenzen

Die Architektur bietet zahlreiche Vorteile.

## Früherkennung

Beschädigungen werden bereits erkannt, bevor Anwendungen betroffen sind.

---

## Automatische Reparatur

Viele Fehler werden ohne Benutzereingriff behoben.

---

## Hohe Datenintegrität

Prüfsummen und Merkle-Trees schützen dauerhaft alle Objekte.

---

## Geringerer Administrationsaufwand

Regelmäßige Hintergrundprüfungen erfolgen automatisch.

---

## Optimale NovaFS-Integration

Self-Healing arbeitet direkt mit:

- Snapshots
- Storage Pools
- Replikation
- Recovery
- Merkle-Trees
- Prüfsummen

zusammen.

---

# 18. Negative Konsequenzen und Risiken

## Zusätzliche I/O

Scrubbing erzeugt Hintergrundlast.

---

## CPU-Belastung

Prüfsummen und Merkle-Verifikation benötigen Rechenleistung.

---

## Keine Reparatur ohne Redundanz

Ist keine gültige Kopie vorhanden, kann lediglich eine Beschädigung erkannt werden. :contentReference[oaicite:3]{index=3}

---

# 19. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Scrubbing Manager
- Self-Healing Manager
- Merkle Manager
- Checksum Manager
- Object Manager
- Storage Pool Manager
- Recovery Manager
- Snapshot Manager
- Replication Manager
- Event Manager

Alle zukünftigen Integritätsfunktionen MÜSSEN auf dieser Architektur aufbauen.

---

# 20. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Alle Datenobjekte MÜSSEN Prüfsummen besitzen.
- Alle Objekte MÜSSEN regelmäßig überprüft werden.
- Scrubbing MUSS im Hintergrund ausführbar sein.
- Reparaturen MÜSSEN transaktional erfolgen.
- Reparaturen DÜRFEN ausschließlich mit validierten Kopien erfolgen.
- Jede Reparatur MUSS vollständig protokolliert werden.
- Alle Integritätsprüfungen MÜSSEN den Merkle-Tree validieren.
- Scrubbing MUSS jederzeit pausiert und später fortgesetzt werden können.
- Fehler ohne Reparaturmöglichkeit MÜSSEN gemeldet werden.

---

# 21. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6017 – Metadaten-Prüfsummen
- ADR-6018 – Daten-Prüfsummen
- ADR-6023 – Multi-Disk-Storage-Pools
- ADR-6030 – Merkle-Tree für Systemintegrität
- ADR-6033 – Separates Recovery-Subvolume
- ADR-6034 – Snapshot-Architektur
- zukünftige ADR – Scrubbing Manager
- zukünftige ADR – Self-Healing Manager
- Btrfs Documentation – Scrub :contentReference[oaicite:4]{index=4}
- FreeBSD Handbook – ZFS Self-Healing :contentReference[oaicite:5]{index=5}
- Linux Manual – btrfs-scrub(8) :contentReference[oaicite:6]{index=6}

---

# 22. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Selbstheilung und Scrubbing als verbindlicher Bestandteil der NovaFS-Architektur angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.