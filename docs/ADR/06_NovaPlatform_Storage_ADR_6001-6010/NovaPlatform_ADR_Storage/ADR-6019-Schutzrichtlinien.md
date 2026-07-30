# ADR-6019: Schutzrichtlinien pro Datei

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Storage Protection
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS
- **Ersetzt:** Keine

---

## 1. Kontext

NovaFS soll sowohl auf einzelnen Computern als auch in hochverfügbaren Storage-Pools und Clustern eingesetzt werden.

Nicht jede Datei besitzt dieselben Anforderungen hinsichtlich:

- Verfügbarkeit
- Redundanz
- Wiederherstellbarkeit
- Performance
- Speicherverbrauch
- Verschlüsselung
- Integrität

Beispielsweise unterscheiden sich die Anforderungen einer temporären Cache-Datei erheblich von denen einer virtuellen Maschine oder einer geschäftskritischen Datenbank.

NovaFS soll deshalb **Schutzrichtlinien (Protection Policies)** individuell pro Datei oder Objekt unterstützen.

---

## 2. Problemstellung

Klassische Dateisysteme verwenden häufig eine globale Redundanzstrategie für das gesamte Dateisystem.

Dies führt zu Nachteilen:

- unnötig hoher Speicherverbrauch
- unnötige Schreiblast
- mangelnde Flexibilität
- keine Optimierung für unterschiedliche Dateitypen

NovaFS benötigt daher ein System, mit dem jede Datei individuell geschützt werden kann.

---

## 3. Betrachtete Alternativen

### Alternative A – Einheitliche Schutzstrategie

Das gesamte Dateisystem verwendet dieselbe Schutzstrategie.

**Vorteile**

- einfache Implementierung
- geringer Verwaltungsaufwand

**Nachteile**

- keine Flexibilität
- ineffiziente Ressourcennutzung
- unterschiedliche Anforderungen können nicht berücksichtigt werden

---

### Alternative B – Schutzrichtlinien pro Volume

Jedes Volume erhält eine eigene Schutzrichtlinie.

**Vorteile**

- einfache Verwaltung
- geeignet für klassische RAID-Systeme

**Nachteile**

- keine individuelle Optimierung
- unterschiedliche Dateien innerhalb eines Volumes können nicht unterschiedlich behandelt werden

---

### Alternative C – Schutzrichtlinien pro Datei bzw. Objekt

Jedes Objekt besitzt eine eigene Schutzrichtlinie.

Diese legt fest, wie das Objekt gespeichert und geschützt wird.

**Vorteile**

- maximale Flexibilität
- optimale Ressourcennutzung
- ideale Grundlage für Storage-Tiering
- Unterstützung unterschiedlicher Workloads

**Nachteile**

- höherer Verwaltungsaufwand
- komplexere Speicherverwaltung

---

## 4. Entscheidung

NovaFS verwendet **Schutzrichtlinien auf Objektebene**.

Jede Datei, jedes Verzeichnis und jedes speicherrelevante Objekt besitzt eine zugewiesene Protection Policy.

Diese Richtlinie bestimmt unter anderem:

- Redundanz
- Erasure Coding
- Spiegelung
- Verschlüsselung
- Prüfsummen
- Komprimierung
- Tiering
- Replikation
- Selbstheilung

Die Richtlinie ist Bestandteil der Objektmetadaten.

---

## 5. Begründung

### Unterschiedliche Anforderungen

Nicht jede Datei benötigt dieselbe Schutzstufe.

Beispiele:

| Objekttyp | Schutz |
|-----------|--------|
| Cache | Keine Redundanz |
| Temporäre Dateien | Minimal |
| Dokumente | Spiegelung |
| Datenbanken | Erasure Coding |
| VM-Images | Hohe Redundanz |
| Betriebssystem | Maximale Integrität |
| Recovery-Image | Schreibgeschützt + Spiegelung |

---

### Optimale Ressourcennutzung

Speicherplatz wird nur dort eingesetzt, wo er tatsächlich benötigt wird.

Dadurch sinken:

- Speicherverbrauch
- Schreiblast
- Netzwerkverkehr

---

### Grundlage für Self-Healing

Die Schutzrichtlinie legt fest, welche Wiederherstellungsmechanismen verwendet werden dürfen.

Beispielsweise:

- Spiegelkopie
- Erasure Coding
- Snapshot
- Backup
- Systemgeneration

---

### Unterstützung zukünftiger Speichertechnologien

Neue Speicherklassen können später über zusätzliche Richtlinien integriert werden, ohne bestehende Objekte zu verändern.

---

## 6. Aufbau einer Schutzrichtlinie

Eine Schutzrichtlinie besteht mindestens aus folgenden Eigenschaften:

- Policy-ID
- Name
- Redundanzmodus
- Anzahl Replikate
- Erasure-Coding-Profil
- Prüfsummenalgorithmus
- Verschlüsselungsprofil
- Komprimierungsprofil
- Speicherklasse
- Tiering-Regeln
- Self-Healing-Regeln

---

### Beispiel

```text
Protection Policy

Policy-ID: 12

Mirror: 2

Erasure Coding: deaktiviert

Compression: Zstd

Encryption: AES-256

Checksum: CRC32C

Storage Tier: SSD

Self-Healing: aktiviert
```

---

## 7. Vererbung

Schutzrichtlinien können vererbt werden.

```text
Storage Pool
        │
        ▼
Subvolume
        │
        ▼
Verzeichnis
        │
        ▼
Datei
```

Jedes Objekt KANN die geerbte Richtlinie überschreiben.

---

## 8. Änderung einer Schutzrichtlinie

Eine Richtlinienänderung erfolgt online.

Der Ablauf lautet:

```text
Neue Policy auswählen
        │
        ▼
Migration vorbereiten
        │
        ▼
Neue Datenblöcke erzeugen
        │
        ▼
Prüfsummen erzeugen
        │
        ▼
Transaktion abschließen
        │
        ▼
Neue Policy aktivieren
```

Während der Migration bleiben alle Daten verfügbar.

---

## 9. Standardrichtlinien

NovaFS definiert mindestens folgende Standardrichtlinien:

| Richtlinie | Beschreibung |
|------------|--------------|
| NONE | Keine Redundanz |
| STANDARD | Standard-Schutz |
| MIRROR_2 | Zweifache Spiegelung |
| MIRROR_3 | Dreifache Spiegelung |
| EC_4_2 | Erasure Coding 4+2 |
| EC_8_2 | Erasure Coding 8+2 |
| EC_8_3 | Erasure Coding 8+3 |
| EC_8_4 | Erasure Coding 8+4 |
| SYSTEM | Betriebssystem |
| RECOVERY | Recovery-Partition |
| ARCHIVE | Langzeitarchiv |
| HIGH_SECURITY | Maximale Integrität und Verschlüsselung |

---

## 10. Positive Konsequenzen

Die Verwendung von Schutzrichtlinien pro Datei bietet zahlreiche Vorteile.

### Hohe Flexibilität

Jedes Objekt erhält genau den benötigten Schutz.

---

### Optimale Ressourcennutzung

Redundanz wird nur dort eingesetzt, wo sie erforderlich ist.

---

### Online-Migration

Schutzrichtlinien können ohne Unterbrechung geändert werden.

---

### Unterstützung unterschiedlicher Workloads

Datenbanken, Archive, Medien und temporäre Dateien können optimal behandelt werden.

---

### Grundlage zukünftiger Speichertechnologien

Neue Redundanz- und Speicherverfahren können über zusätzliche Richtlinien integriert werden.

---

## 11. Negative Konsequenzen und Risiken

### Höherer Verwaltungsaufwand

Die Dateisystemverwaltung muss Schutzrichtlinien auswerten und anwenden.

---

### Komplexere Migration

Beim Wechsel einer Richtlinie müssen Daten gegebenenfalls neu organisiert werden.

---

### Zusätzliche Metadaten

Jedes Objekt benötigt eine Referenz auf seine Schutzrichtlinie.

Der Speicherbedarf hierfür ist gering.

---

## 12. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Object Manager
- Protection Policy Manager
- Extent Manager
- Storage Pool Manager
- Erasure Coding Manager
- Mirror Manager
- Encryption Manager
- Compression Manager
- Tiering Manager
- Recovery Manager
- Self-Healing Manager
- Snapshot Manager
- Cluster Manager

Alle zukünftigen Speichermechanismen SOLLEN über Schutzrichtlinien konfiguriert werden.

---

## 13. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Jedes speicherrelevante Objekt MUSS eine Schutzrichtlinie besitzen.
- Schutzrichtlinien MÜSSEN vererbbar sein.
- Jedes Objekt DARF seine geerbte Richtlinie überschreiben.
- Änderungen an Schutzrichtlinien MÜSSEN transaktional erfolgen.
- Richtlinienänderungen SOLLEN online durchgeführt werden.
- Die Schutzrichtlinie MUSS Bestandteil der Objektmetadaten sein.
- Standardrichtlinien MÜSSEN Bestandteil jeder NovaFS-Installation sein.
- Neue Schutzmechanismen SOLLEN über das Richtliniensystem integriert werden.

---

## 14. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6012 – Copy-on-Write für Metadaten
- ADR-6013 – B+-Trees als zentrale Datenstruktur
- ADR-6014 – Objektbasiertes Dateisystem
- ADR-6017 – Prüfsummen für Metadaten
- ADR-6018 – Prüfsummen für Nutzdaten
- zukünftige ADR – Erasure Coding
- zukünftige ADR – Storage Pools
- zukünftige ADR – Self-Healing

---

## 15. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Schutzrichtlinien pro Datei als verbindlicher Bestandteil der NovaFS-Architektur angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.