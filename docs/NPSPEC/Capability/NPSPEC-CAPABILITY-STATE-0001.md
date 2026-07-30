# NPSPEC-CAPABILITY-STATE-0001: Zustandslose und zustandsbehaftete Fähigkeiten

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Version:** 1.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Gesamte Nova Platform
- **Referenzen:** ADR-CAPABILITY-0002, ADR-CAPABILITY-0005, ADR-CAPABILITY-0011, ADR-CAPABILITY-0012, NPSPEC-CAPABILITY-0001, NPSPEC-CAPABILITY-LIFECYCLE-0001, NPSPEC-CAPABILITY-COMPOSITION-0001, NPSPEC-CAPABILITY-PROFILE-0001

---

# 1. Zweck

Diese Spezifikation definiert das Zustandsmodell (State Model) der Nova Capability Architecture.

Sie beschreibt, wie Capabilities mit internem oder externem Zustand umgehen und wie die Runtime zustandslose (Stateless) sowie zustandsbehaftete (Stateful) Fähigkeiten verwaltet.

Das Zustandsmodell bildet die Grundlage für:

- Skalierung
- Reproduzierbarkeit
- Parallelisierung
- Fehlertoleranz
- Persistenz
- Recovery
- Scheduling
- Migration

---

# 2. Ziele

Das Zustandsmodell verfolgt folgende Ziele:

- klare Trennung zwischen Stateless und Stateful
- deterministische Ausführung
- reproduzierbare Ergebnisse
- horizontale Skalierbarkeit
- kontrollierte Persistenz
- einfache Migration
- konsistente Zustandsverwaltung
- langfristige Erweiterbarkeit

---

# 3. Grundprinzipien

Nova unterscheidet zwei grundlegende Capability-Typen:

```text
Stateless

Stateful
```

Jede Capability muss ihren Zustand explizit deklarieren.

Versteckte Zustände sind nicht zulässig.

Die explizite Trennung zwischen zustandslosen und zustandsbehafteten Komponenten verbessert Skalierbarkeit und Wartbarkeit verteilter Systeme. :contentReference[oaicite:0]{index=0}

---

# 4. Architekturübersicht

```text
Capability
├── State Model
│   ├── Stateless
│   └── Stateful
├── State Storage
├── State Lifecycle
├── State Synchronization
├── State Migration
└── Recovery
```

---

# 5. Stateless Capability

Eine Stateless Capability speichert keinen internen Zustand zwischen zwei Ausführungen.

Jeder Aufruf ist vollständig unabhängig.

Beispiel:

```text
Input
↓
Capability
↓
Output
```

Alle erforderlichen Informationen werden über Eingänge bereitgestellt.

---

# 6. Eigenschaften zustandsloser Fähigkeiten

Stateless Capabilities besitzen folgende Eigenschaften:

- keine Sitzungsdaten
- keine internen Speicherstände
- keine Abhängigkeit vorheriger Aufrufe
- beliebig parallel ausführbar
- leicht skalierbar
- deterministische Wiederholung möglich

Sie sind der bevorzugte Capability-Typ.

---

# 7. Beispiele für Stateless Capabilities

```text
Bild skalieren
Text übersetzen
Hash berechnen
Datei komprimieren
Einheiten umrechnen
Matrix multiplizieren
FFT
Bild filtern
```

Alle Ergebnisse hängen ausschließlich von den Eingabedaten ab.

---

# 8. Stateful Capability

Eine Stateful Capability besitzt einen Zustand, der mehrere Ausführungen überdauert.

Der Zustand beeinflusst zukünftige Ergebnisse.

Beispiel:

```text
Input
↓
Capability
↓
State Update
↓
Output
```

---

# 9. Eigenschaften zustandsbehafteter Fähigkeiten

Stateful Capabilities können:

- Sitzungen verwalten
- Modelle trainieren
- Geräte steuern
- Arbeitsabläufe fortsetzen
- Langzeitprozesse verwalten
- Zwischenergebnisse speichern

Sie benötigen eine explizite Zustandsverwaltung.

---

# 10. Beispiele für Stateful Capabilities

```text
KI-Training
Robotiksteuerung
Workflow Engine
Datenbank
Dateisystem
Chat-Sitzung
Simulation
Langzeitmessung
```

Diese Fähigkeiten besitzen einen dauerhaft relevanten Zustand.

---

# 11. State Storage

Zustände dürfen nicht beliebig gespeichert werden.

Nova unterscheidet:

```text
Transient
Persistent
Shared
Distributed
```

Jeder Zustand besitzt genau einen definierten Speicherort.

---

# 12. Transient State

Der Zustand existiert ausschließlich während einer Ausführung.

Beispiele:

```text
Lokale Variablen
Zwischenspeicher
Temporäre Daten
Berechnungspuffer
```

Nach Abschluss wird dieser Zustand verworfen.

---

# 13. Persistent State

Persistente Zustände bleiben erhalten.

Beispiele:

```text
Projekt
Trainiertes Modell
Roboterkonfiguration
Benutzereinstellungen
Datenbank
```

Persistente Zustände werden versioniert gespeichert.

---

# 14. Shared State

Mehrere Capabilities können auf denselben Zustand zugreifen.

Beispiele:

```text
Model Repository
Dateisystem
Objektspeicher
Messdaten
```

Der Zugriff erfolgt ausschließlich über definierte Schnittstellen.

---

# 15. Distributed State

Der Zustand kann verteilt gespeichert werden.

Beispiele:

```text
Cluster
Cloud
Edge
Mehrere Geräte
```

Die Runtime verwaltet Synchronisation und Konsistenz.

---

# 16. State Identifier

Jeder Zustand besitzt eine eindeutige Identität.

```text
State ID
Owner
Version
Zeitstempel
Typ
Speicherort
```

Dadurch können Zustände eindeutig referenziert werden.

---

# 17. State Lifecycle

Der Zustand besitzt einen eigenen Lebenszyklus.

```text
Erzeugt
↓
Initialisiert
↓
Aktiv
↓
Geändert
↓
Gespeichert
↓
Archiviert
↓
Gelöscht
```

Alle Übergänge werden durch die Runtime verwaltet.

---

# 18. State Synchronization

Verteilte Zustände können synchronisiert werden.

Unterstützt werden:

```text
Synchron
Asynchron
Eventbasiert
Snapshot
Streaming
```

Die gewählte Strategie wird deklarativ beschrieben.

---

# 19. Recovery

Stateful Capabilities müssen Wiederherstellung unterstützen.

Möglichkeiten:

```text
Snapshot
Journal
Checkpoint
Rollback
Replay
```

Nach einem Ausfall muss der zuletzt konsistente Zustand wiederhergestellt werden.

---

# 20. Migration

Zustände dürfen zwischen Implementierungen migriert werden.

Beispiel:

```text
CPU
↓
GPU
↓
Cluster
```

Die semantische Bedeutung des Zustands bleibt erhalten.

---

# 21. Parallelisierung

Stateless Capabilities dürfen beliebig parallel ausgeführt werden.

Stateful Capabilities müssen deklarieren:

```text
Nicht parallel
Instanzbezogen parallel
Vollständig parallel
Synchronisiert parallel
```

Die Runtime berücksichtigt diese Angaben beim Scheduling.

---

# 22. Zustandskonsistenz

Stateful Capabilities deklarieren ihr Konsistenzmodell.

Beispiele:

```text
Strong Consistency
Eventual Consistency
Read Your Writes
Snapshot Isolation
```

Die Runtime setzt die gewählte Strategie um.

---

# 23. Discovery-Unterstützung

Discovery berücksichtigt den State-Typ.

Beispiele:

```text
Stateless
↓
Bevorzugt für Parallelisierung
Stateful
↓
Bevorzugt für Langzeitprozesse
```

Diese Information fließt in das Ranking ein.

---

# 24. Runtime-Verhalten

Vor jeder Ausführung prüft die Runtime:

- State Model
- Speicherort
- Recovery-Unterstützung
- Parallelisierbarkeit
- Konsistenzmodell

Anschließend wird die geeignete Ausführungsstrategie gewählt.

---

# 25. YAML-Beispiel

```yaml
state:
  model: stateful
  storage:
    type: persistent
  synchronization:
    mode: snapshot
  recovery:
    strategy: checkpoint
  parallelExecution:
    mode: synchronized
  consistency:
    model: strong
```

Ein Beispiel für eine zustandslose Capability:

```yaml
state:
  model: stateless
  storage:
    type: transient
  parallelExecution:
    mode: unrestricted
```

---

# 26. Validierungsregeln

Das Zustandsmodell gilt als gültig, wenn:

- genau ein State Model definiert ist,
- Speicherstrategie angegeben wird,
- Recovery-Regeln vorhanden sind,
- Stateful Capabilities eine Konsistenzstrategie deklarieren,
- Parallelisierungsregeln eindeutig beschrieben sind.

Ungültige Zustandsdefinitionen dürfen nicht registriert werden.

---

# 27. Qualitätsanforderungen

Das Zustandsmodell muss:

- explizit sein,
- deterministisch sein,
- reproduzierbare Ausführungen ermöglichen,
- Recovery unterstützen,
- Migration ermöglichen,
- horizontale Skalierung unterstützen,
- Parallelisierung beschreiben,
- langfristig erweiterbar bleiben.

Eine klare Trennung zwischen zustandslosen und zustandsbehafteten Diensten erleichtert Skalierung, Ausfallsicherheit und den Austausch einzelner Komponenten. :contentReference[oaicite:1]{index=1}

---

# 28. Konformitätskriterien

Eine Capability erfüllt diese Spezifikation, wenn:

- ihr Zustandsmodell eindeutig deklariert ist,
- Stateless- oder Stateful-Verhalten vollständig beschrieben wird,
- Speicherstrategie definiert ist,
- Recovery-Regeln vorhanden sind,
- Parallelisierungsregeln angegeben werden,
- die Runtime den Zustand vollständig verwalten kann,
- Discovery den State-Typ berücksichtigen kann.

---

# 29. Zusammenfassung

Das Zustandsmodell definiert die standardisierte Behandlung von zustandslosen und zustandsbehafteten Nova Capabilities.

Während Stateless Capabilities vollständig unabhängig voneinander ausgeführt und beliebig skaliert werden können, besitzen Stateful Capabilities einen explizit verwalteten Zustand mit definierten Regeln für Persistenz, Synchronisation, Recovery und Migration.

Durch die konsequente Trennung beider Modelle erhält die Nova Platform eine hochskalierbare, reproduzierbare und ausfallsichere Architektur, in der jede Capability ihr Zustandsverhalten deklarativ beschreibt und von der Runtime kontrolliert verwaltet wird.