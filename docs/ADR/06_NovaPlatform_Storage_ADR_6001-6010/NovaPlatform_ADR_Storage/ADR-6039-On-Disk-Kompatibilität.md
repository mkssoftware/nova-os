# ADR-6039: On-Disk-Kompatibilität und Feature-Flags

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Storage Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS / Nova OS
- **Ersetzt:** Keine

---

# 1. Kontext

NovaFS ist als langfristig stabiles Dateisystem konzipiert.

Bereits definierte Kernkomponenten sind unter anderem:

- Objektbasiertes Dateisystem
- Copy-on-Write
- Snapshots
- Merkle-Trees
- Prüfsummen
- Verschlüsselung
- Kompression
- Tiered Storage
- Storage Pools
- Capability-System

Diese Architektur wird sich über viele Jahre weiterentwickeln.

Gleichzeitig müssen ältere NovaFS-Versionen weiterhin in der Lage sein, ältere Datenträger zuverlässig zu lesen.

Hierfür wird eine klar definierte **On-Disk-Kompatibilitätsstrategie** mit **Feature-Flags** eingeführt.

Moderne Dateisysteme wie ext4 unterscheiden dabei zwischen vollständig kompatiblen Features, schreibgeschützt kompatiblen Features und inkompatiblen Erweiterungen, um sichere Vorwärts- und Rückwärtskompatibilität zu gewährleisten. :contentReference[oaicite:0]{index=0}

---

# 2. Problemstellung

Ohne Versionierung des On-Disk-Formats entstehen folgende Risiken:

- ältere Kernel beschädigen neue Dateisysteme
- unbekannte Features werden ignoriert
- inkonsistente Datenstrukturen
- schwierige Upgrades
- fehlende Migrationspfade

NovaFS benötigt deshalb eine standardisierte Verwaltung sämtlicher On-Disk-Features.

---

# 3. Betrachtete Alternativen

## Alternative A – Starres Dateiformat

Das On-Disk-Format bleibt dauerhaft unverändert.

**Vorteile**

- einfache Implementierung
- keine Versionsverwaltung

**Nachteile**

- praktisch keine Erweiterbarkeit
- zukünftige Innovationen erschwert
- vollständige Neuformatierung erforderlich

---

## Alternative B – Formatversion ohne Feature-Flags

Es existiert lediglich eine globale Versionsnummer.

**Vorteile**

- einfache Verwaltung

**Nachteile**

- sehr grobe Kompatibilität
- kleine Erweiterungen erzwingen neue Formatversionen
- eingeschränkte Flexibilität

---

## Alternative C – Feature-Flags mit On-Disk-Versionierung

Das Dateisystem besitzt eine Basisversion sowie einzeln aktivierbare Features.

**Vorteile**

- maximale Erweiterbarkeit
- sichere Migration
- hohe Rückwärtskompatibilität
- feingranulare Kompatibilitätsprüfung

**Nachteile**

- komplexere Mount-Logik
- zusätzlicher Verwaltungsaufwand

---

# 4. Entscheidung

NovaFS verwendet eine Kombination aus:

- On-Disk-Formatversion
- Feature-Flags
- Kompatibilitätsklassen

Neue Funktionen werden grundsätzlich als Feature implementiert.

Das Basisformat bleibt möglichst stabil.

---

# 5. Begründung

## Langfristige Stabilität

Das On-Disk-Format soll über Jahrzehnte stabil bleiben.

---

## Erweiterbarkeit

Neue Funktionen können ergänzt werden, ohne das gesamte Dateisystemformat zu ändern.

---

## Sichere Mount-Entscheidungen

Der Kernel kann anhand der Feature-Flags eindeutig entscheiden, ob:

- Lesen und Schreiben möglich ist,
- ausschließlich Lesen zulässig ist oder
- das Dateisystem nicht eingebunden werden darf.

Dieses Vorgehen folgt einem bewährten Muster moderner Dateisysteme. :contentReference[oaicite:1]{index=1}

---

## Zukunftssicherheit

Neue NovaFS-Versionen bleiben möglichst kompatibel zu älteren Datenträgern.

---

# 6. Architektur

```text
Superblock

├── On-Disk Version
├── Compatible Features
├── ReadOnly Features
├── Incompatible Features
├── Experimental Features
└── Reserved Features
```

Alle Feature-Informationen befinden sich ausschließlich im Superblock.

---

# 7. On-Disk-Version

Das Dateisystem besitzt eine Basisversion.

```text
Major

Minor

Patch
```

Beispiel:

```text
1.0.0
```

Die Versionsnummer beschreibt ausschließlich das Basisformat.

Neue Funktionen werden bevorzugt über Feature-Flags aktiviert.

---

# 8. Feature-Klassen

NovaFS definiert vier Feature-Klassen.

## Compatible

Ältere Kernel dürfen das Dateisystem vollständig verwenden.

Unbekannte Features können ignoriert werden.

---

## Read-Only Compatible

Ältere Kernel dürfen das Dateisystem ausschließlich lesend einbinden.

Schreibzugriffe sind nicht zulässig.

---

## Incompatible

Ältere Kernel dürfen das Dateisystem nicht mounten.

---

## Experimental

Features befinden sich im Entwicklungsstatus.

Produktivsysteme dürfen diese standardmäßig nicht aktivieren.

---

# 9. Beispiel

```text
Compatible

• Extended Metadata
• Statistics
• Performance Hints

ReadOnly

• Neue Prüfsummen

Incompatible

• Neues Objektformat
• Neue B-Tree-Struktur

Experimental

• Quantenkompression
• KI-Metadaten
```

Die konkrete Liste wird in der NovaFS-Spezifikation gepflegt.

---

# 10. Feature-ID

Jedes Feature besitzt:

- Feature-ID
- Name
- Version
- Kategorie
- Status
- Beschreibung

```text
Feature

├── ID
├── Name
├── Version
├── Class
├── Status
└── Flags
```

---

# 11. Mount-Prozess

Beim Mounten erfolgt folgende Prüfung:

```text
Superblock

↓

Version prüfen

↓

Feature-Flags lesen

↓

Alle unterstützt?

↓

Ja

↓

Mount

Nein

↓

ReadOnly?

↓

Ja

↓

ReadOnly Mount

Nein

↓

Mount verweigern
```

Dadurch wird verhindert, dass unbekannte On-Disk-Strukturen versehentlich verändert werden. :contentReference[oaicite:2]{index=2}

---

# 12. Aktivierung neuer Features

Neue Features können aktiviert werden:

- beim Erstellen des Dateisystems
- durch ein Upgrade-Werkzeug
- durch Administratorrichtlinien
- automatisch während einer Migration

Die Aktivierung erfolgt stets transaktional.

---

# 13. Migration

Neue Features dürfen bestehende Daten nicht beschädigen.

```text
Altes Format

↓

Migration

↓

Validierung

↓

Feature aktiv

↓

Commit
```

Bei Fehlern erfolgt automatisch ein Rollback.

---

# 14. Feature-Abhängigkeiten

Features können voneinander abhängen.

Beispiel:

```text
Merkle Tree

↓

Integrity Metadata

↓

Object Versioning
```

Abhängigkeiten werden beim Aktivieren geprüft.

---

# 15. Integration mit Snapshots

Snapshots speichern den vollständigen Feature-Status.

Dadurch bleibt ein Snapshot exakt reproduzierbar.

Auch nach späteren Feature-Upgrades bleibt ein Snapshot konsistent.

---

# 16. Integration mit Recovery

Das Recovery-System muss:

- unbekannte Features erkennen,
- Kompatibilität prüfen,
- unterstützte Migrationen durchführen,
- inkompatible Mounts verhindern.

---

# 17. Positive Konsequenzen

Die Architektur bietet zahlreiche Vorteile.

## Hohe Kompatibilität

Neue NovaFS-Versionen bleiben möglichst lange kompatibel.

---

## Sichere Upgrades

Neue Funktionen können kontrolliert eingeführt werden.

---

## Zukunftssicherheit

Das Basisformat bleibt langfristig stabil.

---

## Flexible Erweiterbarkeit

Neue Funktionen benötigen in der Regel keine neue Dateisystemversion.

---

## Sichere Mount-Entscheidungen

Unbekannte Features können zuverlässig erkannt werden.

---

## Optimale NovaFS-Integration

Alle zukünftigen Erweiterungen verwenden dieselbe Feature-Verwaltung.

---

# 18. Negative Konsequenzen und Risiken

## Komplexere Mount-Logik

Der Kernel muss alle Feature-Klassen auswerten.

---

## Größere Testmatrix

Jede Feature-Kombination muss validiert werden.

---

## Migrationsaufwand

Neue inkompatible Features benötigen Upgrade-Werkzeuge.

---

# 19. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Superblock Manager
- Object Manager
- Mount Manager
- Recovery Manager
- Upgrade Manager
- Snapshot Manager
- Transaction Manager
- NovaFS Tools
- Nova Installer
- Nova SDK

Alle zukünftigen On-Disk-Erweiterungen MÜSSEN über das Feature-Flag-System eingeführt werden.

---

# 20. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Das On-Disk-Format MUSS eine eigene Versionsnummer besitzen.
- Jedes neue On-Disk-Feature MUSS eine eindeutige Feature-ID besitzen.
- Neue Funktionen SOLLEN bevorzugt über Feature-Flags aktiviert werden.
- Unbekannte inkompatible Features MÜSSEN den Mountvorgang verhindern.
- Read-Only-kompatible Features DÜRFEN ausschließlich schreibgeschützt eingebunden werden.
- Feature-Aktivierungen MÜSSEN transaktional erfolgen.
- Feature-Abhängigkeiten MÜSSEN geprüft werden.
- Alle aktivierten Features MÜSSEN im Superblock gespeichert werden.
- Experimental-Features DÜRFEN standardmäßig nicht aktiviert sein.

---

# 21. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6014 – Objektbasiertes Dateisystem
- ADR-6016 – Multiple Superblocks
- ADR-6030 – Merkle-Tree für Systemintegrität
- ADR-6034 – Snapshot-Architektur
- ADR-6038 – API-Design von NovaFS
- zukünftige ADR – Upgrade Manager
- zukünftige ADR – NovaFS Tools
- Linux Kernel Documentation – ext4 On-Disk Layout und Feature Flags :contentReference[oaicite:3]{index=3}
- Btrfs Documentation – Filesystem Features und On-Disk-Format :contentReference[oaicite:4]{index=4}

---

# 22. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** On-Disk-Kompatibilität und Feature-Flags als verbindlicher Bestandteil der NovaFS-Architektur angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.