# ADR-6015: Transaktionales Journaling

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Transaction Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS
- **Ersetzt:** Keine

---

## 1. Kontext

NovaFS ist als transaktionales Dateisystem konzipiert und muss selbst bei Stromausfällen, Kernelabstürzen oder Hardwarefehlern jederzeit in einen konsistenten Zustand zurückkehren können.

Da NovaFS Copy-on-Write für sämtliche Metadaten verwendet, entsteht bereits eine hohe Ausfallsicherheit. Dennoch werden zusätzliche Mechanismen benötigt, um:

- atomare Transaktionen,
- Crash-Recovery,
- parallele Schreibvorgänge,
- Checkpoints,
- Rollbacks,
- Systemgenerationen und
- Cluster-Synchronisation

zu unterstützen.

Hierfür wird ein transaktionales Journal eingeführt.

---

## 2. Problemstellung

Während einer Dateisystemoperation können zahlreiche Metadaten gleichzeitig verändert werden.

Beispiele:

- Datei erstellen
- Datei löschen
- Datei verschieben
- Verzeichnis umbenennen
- Schutzrichtlinie ändern
- Snapshot erzeugen
- Rebalancing
- Erasure-Coding-Migration

Fällt das System während einer solchen Operation aus, dürfen niemals teilweise übernommene Änderungen sichtbar werden.

Ein Journal dient dazu, noch nicht vollständig abgeschlossene Änderungen nachvollziehbar zu machen und nach einem Absturz konsistent abzuschließen oder zu verwerfen. Moderne Journaling-Dateisysteme protokollieren dazu Transaktionen und können sie nach einem Absturz wiederherstellen oder verwerfen. :contentReference[oaicite:0]{index=0}

---

## 3. Betrachtete Alternativen

### Alternative A – Kein Journal

Es werden ausschließlich Copy-on-Write und Prüfsummen verwendet.

**Vorteile**

- einfache Implementierung
- geringer Verwaltungsaufwand

**Nachteile**

- längere Recovery
- schwierige Wiederaufnahme unterbrochener Operationen
- keine explizite Transaktionshistorie

---

### Alternative B – Klassisches Metadaten-Journal

Nur Metadatenänderungen werden protokolliert.

**Vorteile**

- bewährtes Verfahren
- schnelle Wiederherstellung

**Nachteile**

- doppelte Schreibvorgänge
- eingeschränkte Unterstützung komplexer Transaktionen

---

### Alternative C – Vollständiges Datenjournal

Sowohl Metadaten als auch Nutzdaten werden vollständig protokolliert.

**Vorteile**

- maximale Sicherheit

**Nachteile**

- hoher Speicherbedarf
- erhebliche Schreibverstärkung
- geringere Leistung

---

### Alternative D – Transaktionales NovaFS-Journal

Das Journal verwaltet ausschließlich Transaktionen.

Es protokolliert:

- Transaktionsbeginn
- geänderte Metadaten
- Commit
- Abbruch
- Checkpoints

Nutzdaten werden nicht vollständig dupliziert.

**Vorteile**

- atomare Änderungen
- schnelle Recovery
- geringe Schreibverstärkung
- optimale Integration mit Copy-on-Write
- Grundlage für Cluster-Replikation

**Nachteile**

- komplexere Implementierung

---

## 4. Entscheidung

NovaFS verwendet ein **transaktionales Journal**, das ausschließlich der Verwaltung und Wiederherstellung von Transaktionen dient.

Das Journal ergänzt das Copy-on-Write-Modell, ersetzt es jedoch nicht.

Das Journal dient insbesondere dazu:

- laufende Transaktionen zu verfolgen,
- Commits atomar abzuschließen,
- Recovery nach Abstürzen zu beschleunigen,
- Checkpoints zu koordinieren,
- Rollbacks zu ermöglichen.

Das Journal ist als ringförmiger Log-Bereich innerhalb des Dateisystems vorgesehen.

---

## 5. Begründung

### Atomare Änderungen

Eine Transaktion gilt erst dann als erfolgreich abgeschlossen, wenn ein gültiger Commit-Eintrag geschrieben wurde.

Vorher bleiben sämtliche Änderungen unsichtbar.

---

### Schnelle Crash-Recovery

Nach einem Systemabsturz muss NovaFS nicht das gesamte Dateisystem durchsuchen.

Es genügt, die noch offenen Journaltransaktionen auszuwerten und anhand des Commit-Status zu entscheiden, ob sie übernommen oder verworfen werden. Dieses Prinzip wird auch von klassischen Journaling-Dateisystemen genutzt. :contentReference[oaicite:1]{index=1}

---

### Integration mit Copy-on-Write

Das Journal protokolliert ausschließlich den Ablauf einer Transaktion.

Die eigentlichen Metadaten befinden sich weiterhin in neuen Copy-on-Write-Blöcken.

Dadurch entstehen:

- keine teilweise überschriebenen Strukturen,
- keine inkonsistenten B+-Trees,
- keine beschädigten Root-Knoten.

---

### Grundlage für Checkpoints

Nach erfolgreichem Commit kann ein neuer Checkpoint erzeugt werden.

Dieser bildet einen vollständig konsistenten Zustand des Dateisystems.

---

### Grundlage für Clusterbetrieb

Im Cluster können Transaktions-IDs zwischen mehreren Nodes synchronisiert werden.

Dadurch lassen sich konsistente globale Zustände erzeugen.

---

## 6. Journalaufbau

Das Journal besteht logisch aus aufeinanderfolgenden Transaktionen.

```text
Journal

┌──────────────────────────────────────────┐
│ Journal Superblock                       │
├──────────────────────────────────────────┤
│ Transaction 100                          │
├──────────────────────────────────────────┤
│ Transaction 101                          │
├──────────────────────────────────────────┤
│ Transaction 102                          │
├──────────────────────────────────────────┤
│ ...                                      │
└──────────────────────────────────────────┘
```

Jede Transaktion besitzt:

- Header
- Metadatenreferenzen
- Statusinformationen
- Commit-Eintrag
- Prüfsumme

---

## 7. Transaktionsablauf

Der Ablauf einer NovaFS-Transaktion ist verbindlich festgelegt.

```text
Transaktion starten
        │
        ▼
Metadaten erzeugen
        │
        ▼
Neue CoW-Blöcke schreiben
        │
        ▼
Prüfsummen berechnen
        │
        ▼
Journal aktualisieren
        │
        ▼
Commit schreiben
        │
        ▼
Checkpoint erzeugen
        │
        ▼
Root atomar umschalten
```

Erst nach erfolgreichem Commit wird die Transaktion sichtbar.

---

## 8. Recovery

Beim Mounten prüft NovaFS:

- Journalintegrität
- Prüfsummen
- Commit-Einträge
- letzten gültigen Checkpoint

Danach gilt:

- vollständige Transaktionen werden übernommen,
- unvollständige Transaktionen werden verworfen.

Transaktionen ohne gültigen Commit dürfen niemals angewendet werden. Dieses Commit-basierte Recovery-Verfahren entspricht dem Grundprinzip moderner Journaling-Dateisysteme. :contentReference[oaicite:2]{index=2}

---

## 9. Positive Konsequenzen

Die Einführung eines transaktionalen Journals bietet zahlreiche Vorteile.

### Schnelle Wiederherstellung

Recovery dauert nur wenige Sekunden.

---

### Atomare Operationen

Es entstehen niemals teilweise abgeschlossene Änderungen.

---

### Hohe Datensicherheit

Auch bei Stromausfällen bleiben Metadaten konsistent.

---

### Unterstützung paralleler Transaktionen

Mehrere unabhängige Transaktionen können gleichzeitig vorbereitet werden.

---

### Optimale Grundlage für Clusterbetrieb

Transaktionen können eindeutig repliziert werden.

---

### Gute Erweiterbarkeit

Zukünftige Funktionen wie:

- verteilte Transaktionen,
- Replikation,
- Versionsverwaltung

bauen direkt auf dem Journal auf.

---

## 10. Negative Konsequenzen und Risiken

### Zusätzlicher Speicherbedarf

Für das Journal muss ein reservierter Bereich vorhanden sein.

---

### Erhöhter Implementierungsaufwand

Das Journal benötigt:

- Journal Manager
- Transaction Manager
- Checkpoint Manager
- Recovery Engine

---

### Schreibverstärkung

Zusätzliche Journalinformationen erzeugen weitere Schreibvorgänge.

Durch die Kombination mit Copy-on-Write bleiben diese jedoch deutlich geringer als bei einem vollständigen Datenjournal.

---

## 11. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Transaction Manager
- Journal Manager
- Checkpoint Manager
- Recovery Manager
- Object Manager
- Root Tree
- B+-Trees
- Snapshot Manager
- Generation Manager
- Cluster Manager
- Live Update Manager

Auch zukünftige verteilte Transaktionen bauen auf dieser Architektur auf.

---

## 12. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Jede schreibende Operation MUSS Bestandteil einer Transaktion sein.
- Jede Transaktion MUSS eine eindeutige Transaktions-ID besitzen.
- Jede Transaktion MUSS einen Begin-Eintrag besitzen.
- Jede Transaktion MUSS einen Commit- oder Abort-Eintrag besitzen.
- Änderungen DÜRFEN erst nach einem gültigen Commit sichtbar werden.
- Transaktionen ohne Commit MÜSSEN beim Recovery verworfen werden.
- Das Journal MUSS zyklisch wiederverwendbar sein.
- Jeder Journaleintrag MUSS eine Prüfsumme besitzen.
- Journal und Copy-on-Write MÜSSEN gemeinsam verwendet werden.
- Checkpoints DÜRFEN ausschließlich nach erfolgreich abgeschlossenem Commit erzeugt werden.

---

## 13. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6001 – Extentbasierte Speicherung
- ADR-6012 – Copy-on-Write für Metadaten
- ADR-6013 – B+-Trees als zentrale Datenstruktur
- ADR-6014 – Objektbasiertes Dateisystem
- zukünftige ADR – Checkpoints
- zukünftige ADR – Snapshot-Architektur
- zukünftige ADR – Cluster-Replikation

---

## 14. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Transaktionales Journaling als verbindlicher Bestandteil der NovaFS-Architektur angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.