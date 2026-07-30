# ADR-6027: Unveränderliche Systemgenerationen

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** System Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS
- **Ersetzt:** Keine

---

# 1. Kontext

NovaFS soll die Grundlage für hochverfügbare und fehlertolerante Betriebssysteme bilden.

Traditionelle Betriebssysteme aktualisieren Systemdateien direkt ("in-place"). Dadurch entstehen Risiken wie:

- unvollständige Updates,
- beschädigte Systemdateien,
- nicht reproduzierbare Zustände,
- schwierige Rollbacks,
- inkonsistente Installationen.

NovaFS verfolgt stattdessen das Konzept **unveränderlicher Systemgenerationen (Immutable System Generations)**.

Dabei wird eine bestehende Systemgeneration niemals verändert. Jede Aktualisierung erzeugt eine vollständig neue Generation. Dieses Architekturprinzip wird auch von modernen imagebasierten Betriebssystemen genutzt, um atomare Updates und reproduzierbare Systemzustände zu ermöglichen. :contentReference[oaicite:0]{index=0}

---

# 2. Problemstellung

Direkte Änderungen am laufenden System besitzen mehrere Nachteile:

- beschädigte Updates können das gesamte System unbrauchbar machen,
- Rollbacks sind aufwendig,
- unterschiedliche Installationen entwickeln sich auseinander,
- Fehler lassen sich schwer reproduzieren,
- Systemdateien können unbemerkt verändert werden.

NovaFS benötigt daher ein Modell, bei dem jede Systemversion vollständig erhalten bleibt.

---

# 3. Betrachtete Alternativen

## Alternative A – In-Place-Updates

Systemdateien werden direkt überschrieben.

**Vorteile**

- geringer zusätzlicher Speicherbedarf
- einfache Implementierung

**Nachteile**

- keine vollständigen Rollbacks
- hohes Risiko inkonsistenter Zustände
- schwierige Wiederherstellung

---

## Alternative B – Snapshots

Vor jedem Update wird ein Snapshot erzeugt.

**Vorteile**

- Rollback möglich
- geringe Implementierungskosten

**Nachteile**

- Snapshots beschreiben nur einen Zeitpunkt
- Änderungen erfolgen weiterhin am aktiven System
- zunehmende Komplexität über längere Zeiträume

---

## Alternative C – Unveränderliche Systemgenerationen

Jede Änderung erzeugt eine vollständig neue Generation.

Bestehende Generationen bleiben unverändert erhalten.

**Vorteile**

- atomare Updates
- reproduzierbare Systeme
- sichere Rollbacks
- hohe Integrität
- einfache Wiederherstellung

**Nachteile**

- zusätzlicher Verwaltungsaufwand
- Verwaltung mehrerer Generationen erforderlich

---

# 4. Entscheidung

NovaFS verwendet **unveränderliche Systemgenerationen**.

Eine Systemgeneration stellt einen vollständigen, konsistenten und unveränderlichen Zustand des Betriebssystems dar.

Nach ihrer Aktivierung DARF eine Generation nicht mehr verändert werden.

Änderungen erfolgen ausschließlich durch das Erzeugen einer neuen Generation.

---

# 5. Begründung

## Atomare Updates

Ein Update besteht aus folgenden Schritten:

```text
Generation N

        │

Neue Generation erzeugen

        │

Validierung

        │

Boot-Eintrag umschalten

        │

Generation N+1 aktiv
```

Der Wechsel erfolgt atomar.

---

## Sichere Rollbacks

Schlägt ein Update fehl, kann unmittelbar auf eine frühere Generation zurückgeschaltet werden.

```text
Generation 15

↓

Generation 16

↓

Generation 17

↓

Boot

↓

Fehler

↓

Generation 16 aktivieren
```

Es werden keine Dateien zurückkopiert.

---

## Reproduzierbarkeit

Jede Generation bleibt dauerhaft erhalten.

Ein bestimmter Fehler kann jederzeit exakt reproduziert werden.

---

## Integrität

Aktive Systemdateien können nicht dauerhaft verändert werden.

Manipulationen werden dadurch erheblich erschwert.

Unveränderliche Systembereiche verbessern zudem die Vorhersagbarkeit und Stabilität des Betriebssystems. :contentReference[oaicite:1]{index=1}

---

# 6. Aufbau einer Systemgeneration

Jede Generation besitzt mindestens:

- Generation-ID
- UUID
- Erstellungszeitpunkt
- Root-Objekt
- Objektbaum
- B+-Tree-Referenzen
- Prüfsummen
- digitale Signatur
- Status

```text
Generation

├── Generation ID
├── UUID
├── Root Object
├── Object Tree
├── Checksum Tree
├── Signature
├── Timestamp
└── Status
```

---

# 7. Bootvorgang

Beim Systemstart wird ausschließlich eine vollständige Generation aktiviert.

```text
Bootloader

        │

Generationen lesen

        │

Verfügbare Generationen

        │

Benutzerwahl oder Standard

        │

Root Generation mounten

        │

System starten
```

Der Bootloader greift niemals auf teilweise aktualisierte Daten zu.

---

# 8. Aktualisierung

Systemupdates erfolgen ausschließlich durch das Erzeugen einer neuen Generation.

```text
Aktuelle Generation

        │

Neue Objekte erzeugen

        │

Neue Metadaten

        │

Neue Root-Referenz

        │

Neue Generation registrieren
```

Bestehende Generationen bleiben vollständig erhalten.

---

# 9. Speicheroptimierung

NovaFS verwendet Copy-on-Write.

Unveränderte Objekte werden zwischen mehreren Generationen gemeinsam genutzt.

```text
Generation 10

↓

Objekt A
Objekt B
Objekt C

↓

Generation 11

↓

Objekt A
Objekt B

Objekt D (neu)
```

Nur geänderte Objekte benötigen zusätzlichen Speicher.

Dieses Modell ähnelt modernen imagebasierten Systemen, bei denen identische Inhalte zwischen Versionen gemeinsam genutzt werden. :contentReference[oaicite:2]{index=2}

---

# 10. Bereinigung

Nicht mehr benötigte Generationen können entfernt werden.

Dabei gelten folgende Regeln:

- aktive Generation niemals löschen,
- Standardgeneration niemals automatisch löschen,
- referenzierte Objekte niemals löschen,
- Bereinigung erfolgt transaktional.

---

# 11. Positive Konsequenzen

Die Verwendung unveränderlicher Systemgenerationen bietet zahlreiche Vorteile.

## Atomare Updates

Das System befindet sich jederzeit in einem konsistenten Zustand.

---

## Sichere Rollbacks

Ein früherer Systemzustand kann innerhalb weniger Sekunden wieder aktiviert werden.

---

## Reproduzierbare Systeme

Jede Generation beschreibt einen exakt definierten Systemzustand.

---

## Hohe Integrität

Aktive Generationen können nicht versehentlich überschrieben werden.

---

## Geringer Speicherverbrauch

Copy-on-Write verhindert unnötige Datenkopien.

---

## Optimale Grundlage für Nova OS

Systemaktualisierungen, Wiederherstellung und Self-Healing greifen auf dasselbe Generationsmodell zurück.

---

# 12. Negative Konsequenzen und Risiken

## Höherer Verwaltungsaufwand

NovaFS benötigt einen Generation Manager.

---

## Zusätzliche Metadaten

Jede Generation besitzt eigene Verwaltungsinformationen.

---

## Speicherverwaltung

Alte Generationen müssen regelmäßig bereinigt werden.

---

# 13. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Generation Manager
- Object Manager
- Transaction Manager
- Boot Manager
- Bootloader
- Snapshot Manager
- Recovery Manager
- Self-Healing Manager
- Checksum Manager
- Storage Pool Manager

Alle zukünftigen Systemupdates MÜSSEN über Systemgenerationen erfolgen.

---

# 14. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Jede Systemgeneration MUSS unveränderlich sein.
- Änderungen DÜRFEN niemals an einer aktiven Generation erfolgen.
- Jede Aktualisierung MUSS eine neue Generation erzeugen.
- Der Wechsel zwischen Generationen MUSS atomar erfolgen.
- Rollbacks MÜSSEN ohne Datenkopien möglich sein.
- Jede Generation MUSS eindeutig identifizierbar sein.
- Generationen MÜSSEN digital signierbar sein.
- Nicht mehr referenzierte Generationen SOLLEN automatisch bereinigt werden.
- Copy-on-Write MUSS zur gemeinsamen Nutzung unveränderter Objekte verwendet werden.

---

# 15. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6012 – Copy-on-Write für Metadaten
- ADR-6013 – B+-Trees als zentrale Datenstruktur
- ADR-6014 – Objektbasiertes Dateisystem
- ADR-6015 – Transaktionales Journaling
- ADR-6018 – Prüfsummen für Nutzdaten
- ADR-6023 – Multi-Disk-Storage-Pools
- zukünftige ADR – Boot Manager
- zukünftige ADR – Recovery Manager
- CentOS Automotive SIG – OSTree Image-Based Operating Systems :contentReference[oaicite:3]{index=3}
- Microsoft Factory OS – State Separation :contentReference[oaicite:4]{index=4}

---

# 16. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Unveränderliche Systemgenerationen als verbindlicher Bestandteil der NovaFS-Architektur angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.