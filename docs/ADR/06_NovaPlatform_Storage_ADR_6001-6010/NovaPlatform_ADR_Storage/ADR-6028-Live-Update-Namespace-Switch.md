# ADR-6028: Live-Update durch Namespace-Switch

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** System Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS
- **Ersetzt:** Keine

---

# 1. Kontext

NovaFS verwendet unveränderliche Systemgenerationen (ADR-6027).

Nach jeder Systemaktualisierung existiert eine neue vollständige Generation des Betriebssystems.

Ein klassischer Neustart ist zwar möglich, führt jedoch zu:

- Unterbrechungen laufender Dienste,
- längeren Wartungsfenstern,
- Ausfallzeiten,
- Neustart aller Prozesse.

NovaFS soll daher langfristig einen Mechanismus unterstützen, bei dem ein laufendes System möglichst ohne vollständigen Neustart auf eine neue Systemgeneration umgestellt werden kann.

Hierzu wird das Konzept eines **Namespace-Switches** eingeführt.

Der Begriff orientiert sich an Betriebssystemkonzepten wie Mount-Namespaces sowie Mechanismen wie `pivot_root()` und `switch_root()`, geht jedoch funktional darüber hinaus und beschreibt eine NovaFS-spezifische Architektur. :contentReference[oaicite:0]{index=0}

---

# 2. Problemstellung

Klassische Betriebssystemupdates besitzen mehrere Nachteile:

- vollständiger Neustart erforderlich,
- lange Downtime,
- Unterbrechung aller Anwendungen,
- Verlust flüchtiger Zustände,
- hohe Auswirkungen bei Servern.

NovaFS benötigt daher eine Architektur, welche einen möglichst unterbrechungsfreien Wechsel auf eine neue Systemgeneration ermöglicht.

---

# 3. Betrachtete Alternativen

## Alternative A – Vollständiger Neustart

Nach jedem Update wird das gesamte System neu gestartet.

**Vorteile**

- einfache Implementierung
- bewährtes Verfahren
- geringer Verwaltungsaufwand

**Nachteile**

- vollständige Downtime
- alle Prozesse werden beendet
- Dienste sind kurzfristig nicht verfügbar

---

## Alternative B – Einzelne Prozesse aktualisieren

Nur einzelne Programme werden neu gestartet.

**Vorteile**

- geringe Unterbrechung

**Nachteile**

- inkonsistente Systemstände
- unterschiedliche Bibliotheksversionen
- schwierige Abhängigkeitsverwaltung

---

## Alternative C – Namespace-Switch

Eine neue Systemgeneration wird vollständig vorbereitet.

Nach erfolgreicher Validierung wird der aktive Root-Namespace atomar auf die neue Generation umgeschaltet.

Bestehende Prozesse können kontrolliert migriert oder neu gestartet werden.

**Vorteile**

- minimale Ausfallzeit
- atomarer Wechsel
- konsistenter Systemzustand
- sichere Rollbacks

**Nachteile**

- hohe Implementierungskomplexität
- zusätzliche Kernelunterstützung erforderlich

---

# 4. Entscheidung

NovaFS definiert einen **Namespace-Switch** als Standardmechanismus für Live-Systemupdates.

Eine neue Systemgeneration wird vollständig vorbereitet.

Nach erfolgreicher Prüfung erfolgt ein atomarer Wechsel des aktiven System-Namespaces.

Die bisherige Generation bleibt unverändert erhalten und kann bei Bedarf sofort wieder aktiviert werden.

---

# 5. Begründung

## Atomarer Wechsel

Der Benutzer arbeitet jederzeit mit genau einer vollständigen Systemgeneration.

Ein teilweiser Wechsel ist ausgeschlossen.

---

## Minimale Downtime

Die eigentliche Umschaltung dauert nur sehr kurze Zeit.

Die aufwendigen Vorbereitungen erfolgen bereits vor dem Wechsel.

---

## Sichere Rollbacks

Falls Probleme erkannt werden, kann unmittelbar wieder zur vorherigen Generation gewechselt werden.

---

## Konsistenter Systemzustand

Alle Systemkomponenten greifen nach der Umschaltung auf dieselbe Generation zu.

Mischzustände werden vermieden.

---

## Grundlage für zukünftige Live-Systeme

Die Architektur ermöglicht langfristig:

- Live-Patching,
- Rolling Updates,
- Hochverfügbarkeitslösungen,
- unterbrechungsarme Serverupdates.

---

# 6. Ablauf des Namespace-Switches

```text
Generation N

        │

Neue Generation erzeugen

        │

Validierung

        │

Namespace vorbereiten

        │

Atomarer Namespace-Switch

        │

Generation N+1 aktiv
```

Die bisherige Generation verbleibt unverändert auf dem Datenträger.

---

# 7. Komponenten

Der Namespace-Switch umfasst mindestens folgende Komponenten:

- Generation Manager
- Namespace Manager
- Boot Manager
- Process Manager
- Object Manager
- Transaction Manager
- Recovery Manager
- Validation Manager

```text
Namespace Switch

├── Generation Manager
├── Namespace Manager
├── Validation
├── Process Migration
├── Root Switch
└── Rollback Controller
```

---

# 8. Prozessbehandlung

Nicht alle Prozesse können während eines Namespace-Switches identisch behandelt werden.

NovaFS unterscheidet mindestens:

| Prozessart | Verhalten |
|------------|-----------|
| Kernel | bleibt aktiv |
| Systemdienste | kontrollierter Neustart oder Migration |
| Benutzerprozesse | konfigurierbare Behandlung |
| Kurzlebige Prozesse | normal beenden |
| Kritische Dienste | priorisierte Umschaltung |

Die genaue Migrationsstrategie wird in einer separaten Spezifikation definiert.

---

# 9. Rollback

Ein Rollback erfolgt analog zum Wechsel.

```text
Generation N+1

        │

Fehler erkannt

        │

Rollback

        │

Namespace zurückschalten

        │

Generation N aktiv
```

Da beide Generationen unverändert erhalten bleiben, sind keine Datenkopien erforderlich.

---

# 10. Beziehung zu Copy-on-Write

Der Namespace-Switch nutzt die bestehenden Copy-on-Write-Strukturen.

Unveränderte Objekte werden gemeinsam genutzt.

Nur geänderte Objekte gehören zur neuen Generation.

Dadurch bleibt der Speicherverbrauch gering.

---

# 11. Beziehung zum Bootloader

Der Bootloader verwendet ebenfalls das Generationsmodell.

Falls ein Live-Wechsel nicht möglich oder nicht erfolgreich ist, kann beim nächsten Start automatisch auf eine andere Generation gewechselt werden.

Somit verwenden Bootloader und Live-Update dieselbe Generationsverwaltung.

---

# 12. Positive Konsequenzen

Die Einführung des Namespace-Switches bietet zahlreiche Vorteile.

## Minimale Ausfallzeiten

Der eigentliche Wechsel erfolgt atomar.

---

## Sichere Rollbacks

Frühere Generationen bleiben vollständig erhalten.

---

## Konsistenter Systemzustand

Alle Komponenten verwenden dieselbe Systemgeneration.

---

## Wiederverwendbare Architektur

Bootloader, Recovery und Live-Update greifen auf dieselbe Generationslogik zurück.

---

## Grundlage für Hochverfügbarkeit

Server können zukünftig nahezu unterbrechungsfrei aktualisiert werden.

---

## Zukunftssicherheit

Die Architektur kann später auf Container, virtuelle Maschinen und verteilte Cluster erweitert werden.

---

# 13. Negative Konsequenzen und Risiken

## Hohe Implementierungskomplexität

Der Namespace-Switch erfordert zusätzliche Kernel- und Systemkomponenten.

---

## Prozessmigration

Nicht jeder laufende Prozess kann transparent übernommen werden.

---

## Ressourcenverwaltung

Während des Wechsels können zwei vollständige Generationen gleichzeitig aktiv sein.

---

## Umfangreiche Validierung

Vor jeder Umschaltung müssen sämtliche Systemkomponenten geprüft werden.

---

# 14. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Generation Manager
- Namespace Manager
- Boot Manager
- Process Manager
- Transaction Manager
- Recovery Manager
- Self-Healing Manager
- Validation Manager
- Object Manager
- Security Manager

Alle zukünftigen Systemupdates SOLLEN den Namespace-Switch unterstützen.

---

# 15. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Jede neue Systemgeneration MUSS vollständig erstellt werden, bevor sie aktiviert wird.
- Der Namespace-Switch MUSS atomar erfolgen.
- Während eines Switches DÜRFEN keine inkonsistenten Systemzustände sichtbar sein.
- Frühere Generationen MÜSSEN unverändert erhalten bleiben.
- Rollbacks MÜSSEN jederzeit möglich sein.
- Der Namespace-Switch MUSS mit Copy-on-Write kompatibel sein.
- Bootloader und Live-Update MÜSSEN dieselbe Generationsverwaltung verwenden.
- Fehler während der Aktivierung MÜSSEN automatisch einen Rollback auslösen.
- Die Prozessmigration SOLL konfigurierbar sein.
- Die Architektur MUSS zukünftige Live-Patching-Mechanismen unterstützen.

---

# 16. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6012 – Copy-on-Write für Metadaten
- ADR-6015 – Transaktionales Journaling
- ADR-6023 – Multi-Disk-Storage-Pools
- ADR-6024 – Native Cluster-Unterstützung
- ADR-6027 – Unveränderliche Systemgenerationen
- zukünftige ADR – Process Manager
- zukünftige ADR – Boot Manager
- zukünftige ADR – Live-Patching
- Linux `pivot_root(2)` Manual :contentReference[oaicite:1]{index=1}
- Linux `switch_root(8)` Manual :contentReference[oaicite:2]{index=2}

---

# 17. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Namespace-Switch als Standardmechanismus für Live-Systemupdates in NovaFS angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.