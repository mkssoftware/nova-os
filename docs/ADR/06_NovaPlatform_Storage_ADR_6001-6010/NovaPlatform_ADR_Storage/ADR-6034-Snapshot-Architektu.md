# ADR-6034: Snapshot-Architektur

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Storage Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS / Nova OS
- **Ersetzt:** Keine

---

# 1. Kontext

NovaFS basiert auf folgenden Kernprinzipien:

- Copy-on-Write
- Objektbasiertes Dateisystem
- Unveränderliche Systemgenerationen
- Merkle-Trees
- Transaktionales Journaling
- Self-Healing

Diese Architektur ermöglicht die Implementierung nahezu kostenfreier Snapshots.

Ein Snapshot stellt einen vollständig konsistenten Zustand eines Subvolumes oder Objekts zu einem bestimmten Zeitpunkt dar.

Da NovaFS Copy-on-Write verwendet, müssen beim Erzeugen eines Snapshots keine Daten kopiert werden. Stattdessen wird lediglich auf den bestehenden Objektbaum verwiesen. Dieses Prinzip wird unter anderem in ZFS und ähnlichen Copy-on-Write-Dateisystemen verwendet, wodurch Snapshots nahezu ohne zusätzlichen Speicher- oder Zeitaufwand erzeugt werden können. :contentReference[oaicite:0]{index=0}

---

# 2. Problemstellung

Ein modernes Dateisystem benötigt Snapshots für:

- Wiederherstellung
- Rollbacks
- Backups
- Klonen
- Tests
- Softwareupdates
- virtuelle Maschinen
- Container
- Benutzerdaten

Klassische Kopien kompletter Dateisysteme verursachen:

- hohen Speicherverbrauch
- lange Erstellungszeiten
- inkonsistente Zustände
- unnötige I/O-Last

NovaFS benötigt daher eine effiziente Snapshot-Architektur.

---

# 3. Betrachtete Alternativen

## Alternative A – Vollständige Kopie

Der komplette Datenbestand wird kopiert.

**Vorteile**

- einfache Implementierung
- vollständig unabhängig

**Nachteile**

- hoher Speicherbedarf
- langsame Erstellung
- hoher I/O-Aufwand

---

## Alternative B – Blockbasierte Snapshots

Snapshots werden auf Blockebene verwaltet.

**Vorteile**

- effizient
- bewährtes Verfahren

**Nachteile**

- geringe Objektintegration
- eingeschränkte Flexibilität

---

## Alternative C – Objektbasierte Copy-on-Write-Snapshots

Snapshots bestehen ausschließlich aus einer zusätzlichen Referenz auf den bestehenden Objektbaum.

Nur später geänderte Objekte werden neu angelegt.

**Vorteile**

- nahezu sofortige Erstellung
- minimaler Speicherbedarf
- optimale NovaFS-Integration
- atomare Konsistenz
- einfache Rollbacks

**Nachteile**

- komplexere Referenzverwaltung
- Garbage Collection erforderlich

---

# 4. Entscheidung

NovaFS verwendet **objektbasierte Copy-on-Write-Snapshots**.

Ein Snapshot besteht ausschließlich aus einer neuen Root-Referenz auf den aktuellen Objektbaum.

Alle Objekte bleiben unverändert erhalten.

Erst spätere Änderungen erzeugen neue Objekte.

---

# 5. Begründung

## Nahezu sofortige Erstellung

Beim Erzeugen eines Snapshots werden keine Daten kopiert.

Es wird lediglich eine neue Root-Referenz gespeichert.

---

## Minimaler Speicherbedarf

Unveränderte Objekte werden gemeinsam genutzt.

Nur geänderte Objekte benötigen zusätzlichen Speicher.

Snapshots verbrauchen daher zunächst nahezu keinen zusätzlichen Platz und wachsen nur durch spätere Änderungen. :contentReference[oaicite:1]{index=1}

---

## Konsistenz

Ein Snapshot beschreibt immer einen vollständig konsistenten Zeitpunkt.

Teilweise Aktualisierungen sind ausgeschlossen.

---

## Rollback

Ein Rollback besteht lediglich im Wechsel auf eine andere Snapshot-Root.

Es müssen keine Daten kopiert werden.

---

## Integration mit NovaFS

Snapshots nutzen dieselben Objektstrukturen wie:

- Systemgenerationen
- Recovery
- Klone
- Backup
- Self-Healing

---

# 6. Architektur

```text
Subvolume

        │

Root Object

        │

Snapshot erstellen

        │

Neue Snapshot Root

        │

Gemeinsame Objekte
```

Es entstehen zunächst keine zusätzlichen Datenkopien.

---

# 7. Snapshot-Struktur

Jeder Snapshot besitzt mindestens:

- Snapshot-ID
- UUID
- Name
- Erstellungszeit
- Root Object
- Merkle Root
- Generation
- Beschreibung
- Besitzer
- Status

```text
Snapshot

├── Snapshot ID
├── UUID
├── Name
├── Root Object
├── Merkle Root
├── Generation
├── Timestamp
└── Flags
```

---

# 8. Copy-on-Write

Nach einem Snapshot bleiben sämtliche Objekte unverändert.

```text
Snapshot

↓

Objekt A
Objekt B
Objekt C

↓

Änderung an Objekt B

↓

Objekt B'

↓

Snapshot zeigt weiterhin auf B

Live-System zeigt auf B'
```

Nur geänderte Objekte werden neu geschrieben.

Dieses Verhalten ist eine direkte Folge des Copy-on-Write-Prinzips. :contentReference[oaicite:2]{index=2}

---

# 9. Snapshot-Typen

NovaFS unterstützt mindestens folgende Snapshot-Arten:

- Manuell
- Automatisch
- Vor Update
- Nach Update
- Zeitgesteuert
- Vor Backup
- Recovery
- Read-Only
- Read-Write (Clone)

Weitere Snapshot-Typen können ergänzt werden.

---

# 10. Rollback

Ein Rollback erfolgt durch Wechsel der Root-Referenz.

```text
Snapshot 1

↓

Snapshot 2

↓

Snapshot 3

↓

Rollback

↓

Snapshot 2 aktiv
```

Die Objektdaten bleiben unverändert.

---

# 11. Klonen

Ein Snapshot kann als Grundlage eines neuen Subvolumes dienen.

```text
Snapshot

↓

Clone

↓

Eigene Änderungen

↓

Copy-on-Write
```

Clone und Snapshot teilen zunächst sämtliche Objekte.

---

# 12. Bereinigung

Snapshots können gelöscht werden.

Dabei gelten folgende Regeln:

- referenzierte Objekte bleiben erhalten,
- gemeinsam genutzte Objekte werden nicht gelöscht,
- nur unreferenzierte Objekte werden freigegeben,
- Bereinigung erfolgt transaktional.

---

# 13. Integration mit Merkle-Tree

Jeder Snapshot besitzt:

- eigenen Merkle Root
- eigene Signatur (optional)
- eigene Integritätsprüfung

```text
Snapshot

↓

Merkle Root

↓

Digitale Signatur
```

Damit kann jeder Snapshot unabhängig validiert werden.

---

# 14. Positive Konsequenzen

Die Snapshot-Architektur bietet zahlreiche Vorteile.

## Sofortige Erstellung

Snapshots werden nahezu ohne Verzögerung erzeugt.

---

## Geringer Speicherbedarf

Nur Änderungen benötigen zusätzlichen Speicher.

---

## Sichere Rollbacks

Jeder Snapshot beschreibt einen vollständigen Zustand.

---

## Optimale Backup-Grundlage

Backups können auf konsistenten Snapshots basieren.

---

## Klone

Neue Subvolumes lassen sich nahezu ohne Speicherverbrauch erzeugen.

---

## Optimale NovaFS-Integration

Alle Kernmechanismen nutzen dieselbe Objektarchitektur.

---

# 15. Negative Konsequenzen und Risiken

## Referenzverwaltung

Gemeinsam genutzte Objekte benötigen Referenzzählung.

---

## Garbage Collection

Nicht mehr referenzierte Objekte müssen sicher entfernt werden.

---

## Viele Snapshots

Sehr große Snapshot-Mengen erhöhen den Verwaltungsaufwand.

---

# 16. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Snapshot Manager
- Object Manager
- Generation Manager
- Merkle Manager
- Backup Manager
- Recovery Manager
- Clone Manager
- Transaction Manager
- Garbage Collector
- Self-Healing Manager

Alle zukünftigen Rollback- und Backup-Funktionen SOLLEN auf der Snapshot-Architektur basieren.

---

# 17. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Snapshots MÜSSEN atomar erstellt werden.
- Snapshots DÜRFEN keine vollständigen Datenkopien erzeugen.
- Snapshots MÜSSEN Copy-on-Write verwenden.
- Jeder Snapshot MUSS einen eigenen Root Object Pointer besitzen.
- Jeder Snapshot MUSS eindeutig identifizierbar sein.
- Jeder Snapshot SOLL einen eigenen Merkle Root besitzen.
- Rollbacks MÜSSEN ausschließlich durch Root-Wechsel erfolgen.
- Snapshot-Löschungen DÜRFEN keine gemeinsam genutzten Objekte entfernen.
- Garbage Collection MUSS ausschließlich nicht referenzierte Objekte freigeben.

---

# 18. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6012 – Copy-on-Write für Metadaten
- ADR-6014 – Objektbasiertes Dateisystem
- ADR-6027 – Unveränderliche Systemgenerationen
- ADR-6030 – Merkle-Tree für Systemintegrität
- ADR-6033 – Separates Recovery-Subvolume
- zukünftige ADR – Snapshot Manager
- zukünftige ADR – Clone Manager
- OpenZFS Documentation – Copy-on-Write und Snapshots :contentReference[oaicite:3]{index=3}
- NetApp ONTAP – Snapshot Technology :contentReference[oaicite:4]{index=4}
- Copy-on-Write & Snapshots – dsa.how :contentReference[oaicite:5]{index=5}

---

# 19. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Snapshot-Architektur als verbindlicher Bestandteil der NovaFS-Architektur angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.