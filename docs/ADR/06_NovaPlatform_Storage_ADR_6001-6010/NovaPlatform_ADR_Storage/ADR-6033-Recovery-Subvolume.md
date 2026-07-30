# ADR-6033: Separates Recovery-Subvolume

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Storage Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS / Nova OS
- **Ersetzt:** Keine

---

# 1. Kontext

Nova OS verfolgt das Ziel, jederzeit eine vollständige Wiederherstellung des Systems zu ermöglichen.

Bereits bestehende Architekturentscheidungen definieren:

- unveränderliche Systemgenerationen
- Copy-on-Write
- Self-Healing
- Merkle-Trees
- Capability-System
- transaktionale Updates

Dennoch besteht das Risiko, dass das eigentliche System-Subvolume beschädigt wird oder nicht mehr bootfähig ist.

NovaFS führt deshalb ein **separates Recovery-Subvolume** ein.

Dieses Subvolume enthält ausschließlich die Wiederherstellungsumgebung und ist vollständig vom eigentlichen Betriebssystem getrennt.

Damit folgt NovaFS dem Grundgedanken moderner Recovery-Umgebungen, die getrennt vom eigentlichen System gespeichert werden, damit sie auch bei einem beschädigten oder verschlüsselten System weiterhin verfügbar bleiben. :contentReference[oaicite:0]{index=0}

---

# 2. Problemstellung

Befindet sich die Recovery-Umgebung innerhalb des normalen Systembereichs, entstehen mehrere Risiken:

- Beschädigung durch Dateisystemfehler
- gemeinsame Korruption mit dem Betriebssystem
- Manipulation durch Schadsoftware
- erschwerte Wiederherstellung
- fehlende Trennung der Verantwortlichkeiten

NovaFS benötigt daher einen vollständig getrennten Wiederherstellungsbereich.

---

# 3. Betrachtete Alternativen

## Alternative A – Recovery innerhalb des System-Subvolumes

Die Recovery-Dateien befinden sich im normalen Betriebssystem.

**Vorteile**

- einfache Implementierung
- geringer Verwaltungsaufwand

**Nachteile**

- keine Isolation
- gemeinsam mit dem System beschädigbar
- geringere Sicherheit

---

## Alternative B – Eigenständige Recovery-Partition

Eine separate physische Partition enthält ausschließlich die Recovery-Umgebung.

**Vorteile**

- gute Isolation
- unabhängig vom System

**Nachteile**

- unflexibel
- erschwerte Speicherverwaltung
- zusätzliche Partitionierung erforderlich

---

## Alternative C – Separates Recovery-Subvolume

Die Recovery-Umgebung befindet sich in einem eigenen NovaFS-Subvolume.

Dieses besitzt eigene Schutzrichtlinien und eigene Generationen.

**Vorteile**

- vollständige logische Trennung
- flexible Verwaltung
- Snapshot- und Generationsunterstützung
- optimale Integration in NovaFS

**Nachteile**

- zusätzlicher Verwaltungsaufwand
- eigenes Lifecycle-Management erforderlich

---

# 4. Entscheidung

NovaFS verwendet ein **eigenständiges Recovery-Subvolume**.

Dieses Subvolume ist logisch vom normalen System getrennt.

Es besitzt:

- eigene Root-Referenz
- eigene Generationen
- eigene Protection Policies
- eigene Integritätsprüfung
- eigene Boot-Einträge

Das Recovery-Subvolume kann unabhängig vom eigentlichen Betriebssystem aktualisiert werden.

---

# 5. Begründung

## Vollständige Isolation

Beschädigungen des normalen Betriebssystems beeinflussen das Recovery-System nicht.

---

## Höhere Sicherheit

Das Recovery-Subvolume kann deutlich restriktiver abgesichert werden.

Beispielsweise:

- nur lesbar
- digital signiert
- unveränderliche Generationen
- eigene Merkle-Roots

---

## Unabhängige Aktualisierung

Recovery-Komponenten können unabhängig vom Hauptsystem aktualisiert werden.

---

## Schnellere Wiederherstellung

Das System kann direkt in die Recovery-Umgebung booten.

---

## Optimale Integration

Da Recovery selbst ebenfalls auf NovaFS basiert, können sämtliche vorhandenen Funktionen wiederverwendet werden.

---

# 6. Architektur

```text
NovaFS

├── System Subvolume
│
├── User Subvolume
│
├── Application Subvolume
│
├── Configuration Subvolume
│
└── Recovery Subvolume
```

Alle Subvolumes besitzen eigene Root-Objekte.

---

# 7. Inhalt des Recovery-Subvolumes

Das Recovery-Subvolume enthält mindestens:

- Recovery Kernel
- Recovery Init
- NovaFS-Treiber
- Recovery Shell
- Diagnosewerkzeuge
- Dateisystemprüfung
- Self-Healing
- Boot-Reparatur
- Backup-Manager
- Netzwerkwerkzeuge
- Log-Viewer

Weitere Werkzeuge können ergänzt werden.

---

# 8. Bootvorgang

```text
UEFI

↓

Bootloader

↓

Recovery vorhanden?

↓

Ja

↓

Recovery starten

↓

Recovery Shell
```

Der Bootloader kann jederzeit direkt in das Recovery-Subvolume starten.

---

# 9. Integritätsprüfung

Das Recovery-Subvolume besitzt:

- eigene Prüfsummen
- eigenen Merkle-Tree
- eigene digitale Signatur

```text
Recovery Generation

↓

Merkle Root

↓

Digitale Signatur

↓

Validierung
```

Dadurch kann das Recovery-System unabhängig überprüft werden.

---

# 10. Recovery-Operationen

Innerhalb der Recovery-Umgebung können unter anderem folgende Funktionen ausgeführt werden:

- Rollback einer Generation
- Wiederherstellung aus Backup
- Self-Healing
- Integritätsprüfung
- Dateisystemreparatur
- Neuaufbau von Metadaten
- Import beschädigter Pools
- Bootloader-Reparatur
- Passwort-Reset (optional)
- Netzwerkdiagnose

---

# 11. Schutzrichtlinien

Für das Recovery-Subvolume gelten standardmäßig besonders strenge Regeln:

- Read-Only
- Copy-on-Write
- digitale Signatur
- Merkle-Tree
- unveränderliche Generationen
- erhöhte Prüfsummenprüfung

Schreibzugriffe erfolgen ausschließlich während autorisierter Updates.

---

# 12. Aktualisierung

Recovery-Komponenten werden separat aktualisiert.

```text
Recovery Generation 5

↓

Neue Generation

↓

Validierung

↓

Boot-Eintrag aktualisieren

↓

Recovery Generation 6
```

Das Hauptsystem bleibt hiervon unberührt.

---

# 13. Positive Konsequenzen

Das separate Recovery-Subvolume bietet zahlreiche Vorteile.

## Höhere Ausfallsicherheit

Beschädigungen des Hauptsystems beeinflussen die Recovery-Umgebung nicht.

---

## Schnellere Wiederherstellung

Boot direkt in das Recovery-System.

---

## Bessere Sicherheit

Recovery besitzt eigene Sicherheitsrichtlinien.

---

## Höhere Integrität

Eigene Merkle-Roots und Signaturen verhindern Manipulationen.

---

## Flexible Erweiterbarkeit

Neue Recovery-Werkzeuge können unabhängig integriert werden.

---

## Optimale NovaFS-Integration

Recovery nutzt dieselben Objekt- und Generationsmechanismen wie das Hauptsystem.

---

# 14. Negative Konsequenzen und Risiken

## Zusätzlicher Speicherbedarf

Das Recovery-Subvolume benötigt eigenen Speicherplatz.

---

## Höherer Verwaltungsaufwand

Recovery besitzt einen eigenen Lebenszyklus.

---

## Separate Aktualisierung

Recovery und Hauptsystem müssen unabhängig gepflegt werden.

---

# 15. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Recovery Manager
- Boot Manager
- Bootloader
- Generation Manager
- Merkle Manager
- Object Manager
- Self-Healing Manager
- Backup Manager
- Security Manager
- Transaction Manager

Alle Wiederherstellungsfunktionen SOLLEN ausschließlich über das Recovery-Subvolume erfolgen.

---

# 16. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Das Recovery-System MUSS in einem separaten Subvolume gespeichert werden.
- Das Recovery-Subvolume MUSS unabhängig bootfähig sein.
- Recovery MUSS eigene Generationen besitzen.
- Recovery MUSS eigene Merkle-Trees besitzen.
- Recovery MUSS digital signierbar sein.
- Recovery SOLL standardmäßig schreibgeschützt betrieben werden.
- Recovery-Aktualisierungen MÜSSEN transaktional erfolgen.
- Das Recovery-Subvolume DARF nicht vom normalen System abhängig sein.
- Bootloader und Recovery Manager MÜSSEN das Recovery-Subvolume direkt starten können.

---

# 17. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6012 – Copy-on-Write für Metadaten
- ADR-6027 – Unveränderliche Systemgenerationen
- ADR-6028 – Live-Update durch Namespace-Switch
- ADR-6030 – Merkle-Tree für Systemintegrität
- ADR-6031 – TPM als optionale Sicherheitskomponente
- ADR-6032 – Trust-Backend-Abstraktion
- zukünftige ADR – Recovery Manager
- zukünftige ADR – Secure Boot
- Microsoft Learn – Windows Recovery Environment (WinRE) Technical Reference :contentReference[oaicite:1]{index=1}
- Microsoft Learn – Recovery-Partitionen und Partitionslayout :contentReference[oaicite:2]{index=2}

---

# 18. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Separates Recovery-Subvolume als verbindlicher Bestandteil der NovaFS-Architektur angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.