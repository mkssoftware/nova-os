# NPSPEC-CAPABILITY-0001: Nova Capability Architecture

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Version:** 1.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Gesamte Nova Platform
- **Referenzen:** ADR-CAPABILITY-0001 bis ADR-CAPABILITY-0015

---

# 1. Zweck

Diese Spezifikation definiert die Capability-Architektur von NovaOS.

Sie beschreibt das zentrale Ausführungsmodell des Betriebssystems und legt fest, wie Funktionen bereitgestellt, gefunden, kombiniert, ausgeführt und verwaltet werden.

Capabilities bilden die kleinste fachliche Funktionseinheit der Plattform und ersetzen klassische Anwendungen als primäres Architekturmodell.

---

# 2. Ziele

Die Nova Capability Architecture verfolgt folgende Ziele:

- maximale Wiederverwendbarkeit
- vollständige Modularität
- deklarative Systembeschreibung
- reproduzierbare Workflows
- hardwareunabhängige Ausführung
- KI-gestützte Planung
- stabile Verträge
- hohe Austauschbarkeit
- vollständige Nachvollziehbarkeit
- langfristige Erweiterbarkeit

---

# 3. Architekturübersicht

```text
                   Benutzer
                       │
                       ▼
              Benutzerabsicht (Intent)
                       │
                       ▼
                 Intent Planner
                       │
                       ▼
            Capability Graph Builder
                       │
                       ▼
               Validierter Graph
                       │
                       ▼
                    Pipeline
                       │
                       ▼
                Pipeline Runtime
                       │
        ┌──────────────┼──────────────┐
        ▼              ▼              ▼
   CPU Runtime     GPU Runtime    Remote Runtime
        │              │              │
        └──────────────┼──────────────┘
                       ▼
                 Capabilities
                       │
                       ▼
                   Ergebnisse
```

---

# 4. Architekturprinzipien

Die Nova Capability Architecture basiert auf folgenden Grundprinzipien:

1. Fähigkeiten ersetzen Anwendungen als primäres Softwaremodell.
2. Jede Fähigkeit besitzt genau eine klar definierte Verantwortung.
3. Fähigkeiten kommunizieren ausschließlich über typisierte Ein- und Ausgänge.
4. Fähigkeiten können zu Laufzeitmodulen zusammengefasst werden.
5. Fähigkeiten werden ausschließlich über stabile Capability Contracts angesprochen.
6. Implementierungen bleiben austauschbar.
7. Fähigkeiten sind systemweit nutzbar.
8. Fähigkeiten besitzen deklarative Beschreibungen.
9. Fähigkeiten laufen standardmäßig isoliert.
10. Klassische Anwendungen bilden ausschließlich eine Kompatibilitätsschicht.
11. Fähigkeiten können lokal, hardwarebeschleunigt oder entfernt ausgeführt werden.
12. Pipelines sind eigenständige ausführbare Objekte.
13. Benutzerabsichten werden automatisch in Fähigkeiten-Graphen übersetzt.
14. Die Benutzeroberfläche bleibt stabil, während Inhalte kontextabhängig wechseln.
15. Automatische Entscheidungen sind nachvollziehbar.

Diese Architekturentscheidungen bilden gemeinsam die verbindliche Grundlage der Nova Platform. Architekturentscheidungen sollten dauerhaft dokumentiert werden, um ihre Begründung und Auswirkungen nachvollziehbar zu halten. :contentReference[oaicite:0]{index=0}

---

# 5. Capability

Eine Capability beschreibt genau eine fachliche Funktion.

Sie besitzt:

- eine eindeutige ID
- einen Namen
- eine Version
- deklarative Metadaten
- Capability Contract
- Ein- und Ausgänge
- Berechtigungen
- Qualitätsmerkmale

Eine Capability beschreibt ausschließlich **was** sie leistet.

Nicht beschrieben werden:

- Programmiersprache
- Prozessmodell
- Hardware
- Threadmodell
- Speicherlayout

---

# 6. Capability Descriptor

Jede Capability besitzt einen deklarativen Descriptor.

Beispiel:

```yaml
id: nova.image.resize

version: 1.2.0

name: Resize Image

description: Skaliert Rasterbilder.

inputs:

- image

outputs:

- image

permissions:

- filesystem.read

- filesystem.write

supportedExecution:

- cpu

- gpu

- remote
```

Der Descriptor ist die einzige öffentlich sichtbare Beschreibung einer Capability.

---

# 7. Capability Contract

Der Capability Contract definiert die öffentliche Schnittstelle.

Er beschreibt:

- Eingänge
- Ausgänge
- Datentypen
- Fehler
- Version
- Garantien
- Berechtigungen

Implementierungsdetails sind nicht Bestandteil des Contracts.

---

# 8. Capability Registry

Alle Capabilities werden in der Capability Registry registriert.

Die Registry verwaltet:

- IDs
- Versionen
- Descriptoren
- Contracts
- Implementierungen
- Tags
- Kategorien
- Berechtigungen
- Signaturen

Die Registry dient ausschließlich der Beschreibung.

Sie enthält keine Ausführungslogik.

---

# 9. Capability Runtime

Die Capability Runtime übernimmt:

- Instanziierung
- Lebenszyklus
- Isolation
- Scheduling
- Berechtigungsprüfung
- Fehlerbehandlung
- Ressourcenverwaltung
- Auswahl geeigneter Implementierungen

---

# 10. Ausführungsmodelle

NovaOS unterstützt verschiedene Ausführungsorte.

```text
CPU

GPU

SIMD

DSP

FPGA

NPU

TPU

Container

Virtuelle Maschine

Remote Node

Cluster

Cloud
```

Die Auswahl erfolgt automatisch durch die Runtime.

---

# 11. Capability Graph

Ein Capability Graph beschreibt den vollständigen Datenfluss.

Beispiel:

```text
PDF
↓
OCR
↓
Sprache erkennen
↓
Übersetzen
↓
Zusammenfassen
↓
Markdown
```

Ein Graph beschreibt ausschließlich fachliche Zusammenhänge.

---

# 12. Pipeline

Aus jedem validierten Graphen entsteht eine Pipeline.

Pipelines besitzen:

- ID
- Version
- Eingänge
- Ausgänge
- Metadaten
- Signaturen
- Provenance
- Validierungsinformationen

Pipelines sind eigenständige ausführbare Objekte.

---

# 13. Intent Planning

Benutzer formulieren ausschließlich ihre Absicht.

Beispiel:

```text
"Fasse dieses PDF zusammen."
```

Der Intent Planner erstellt automatisch:

```text
Intent
↓
Capability Suche
↓
Graph
↓
Pipeline
↓
Ausführung
```

---

# 14. Solutions

Solutions definieren Arbeitsbereiche.

Eine Solution enthält beispielsweise:

- Layout
- Navigation
- bevorzugte Pipelines
- Ansichten
- gespeicherte Kontexte

Solutions enthalten keine fachliche Logik.

---

# 15. Kontextmodell

NovaOS unterscheidet zwischen:

- Systemoberfläche
- Arbeitskontext
- Pipeline
- Capability

Nur der Arbeitskontext verändert sich.

Die Shell bleibt dauerhaft stabil.

---

# 16. Berechtigungen

Jede Capability besitzt deklarierte Berechtigungen.

Beispiele:

```text
filesystem.read

filesystem.write

camera

microphone

network

bluetooth

serial

location
```

Die Runtime überprüft sämtliche Berechtigungen vor der Ausführung.

---

# 17. Isolation

Capabilities laufen standardmäßig isoliert.

Direkte Kommunikation zwischen Fähigkeiten ist nicht zulässig.

Alle Kommunikation erfolgt über:

- Capability Contracts
- Runtime
- typisierte Ports

---

# 18. Explainability

Alle automatischen Entscheidungen werden dokumentiert.

Hierzu gehören insbesondere:

- Capability-Auswahl
- Pipelineplanung
- Hardwarewahl
- Priorisierung
- Optimierungen

Jede Entscheidung besitzt eine nachvollziehbare Begründung.

---

# 19. Provenance

Optional kann jede Pipelineausführung dokumentieren:

- Pipeline-Version
- Capability-Versionen
- Parameter
- Zeitpunkte
- Hardware
- Laufzeiten
- Fehler
- Ergebnisse
- automatische Entscheidungen

Dadurch entsteht vollständige Nachvollziehbarkeit.

---

# 20. Kompatibilität

Klassische Anwendungen bleiben erhalten.

Sie werden über einen Application Host integriert.

Anwendungen können:

- Capabilities nutzen
- Capability Adapter bereitstellen
- schrittweise migriert werden

Sie bilden ausschließlich eine Kompatibilitätsschicht.

---

# 21. Nicht Bestandteil dieser Spezifikation

Nicht beschrieben werden:

- konkrete Programmiersprachen
- Binärformate
- Paketformate
- Netzwerkprotokolle
- UI-Komponenten
- Dateisysteme
- Scheduler-Algorithmen

Diese werden in eigenen NPSPEC-Dokumenten spezifiziert.

---

# 22. Qualitätsanforderungen

Die Capability Architecture muss folgende Eigenschaften erfüllen:

- modular
- deklarativ
- deterministisch
- reproduzierbar
- auditierbar
- hardwareunabhängig
- sicher
- versionierbar
- erweiterbar
- fehlertolerant
- austauschbar
- nachvollziehbar

---

# 23. Konformitätskriterien

Eine Implementierung gilt als konform, wenn:

- alle Plattformfunktionen als Capabilities modelliert werden,
- Capability Contracts eingehalten werden,
- Descriptoren deklarativ beschrieben sind,
- Fähigkeiten isoliert ausgeführt werden,
- Pipelines reproduzierbar sind,
- automatische Entscheidungen nachvollziehbar dokumentiert werden,
- Benutzerabsichten in Capability-Graphen übersetzt werden,
- Hardwaretransparenz gewährleistet ist,
- klassische Anwendungen ausschließlich als Kompatibilitätsschicht behandelt werden.

---

# 24. Zusammenfassung

Die Nova Capability Architecture definiert das zentrale Softwaremodell von NovaOS.

Anstelle klassischer Anwendungen basiert das gesamte System auf kleinen, klar abgegrenzten Fähigkeiten, die über stabile Capability Contracts miteinander verbunden werden. Benutzer formulieren ausschließlich ihre Absichten, während die Plattform daraus automatisch validierte Fähigkeiten-Graphen und reproduzierbare Pipelines erzeugt.

Durch deklarative Beschreibungen, austauschbare Implementierungen, hardwareunabhängige Ausführung, transparente Entscheidungsprozesse und systemweite Wiederverwendbarkeit entsteht eine modulare, KI-fähige und langfristig wartbare Plattformarchitektur, die die Grundlage aller höheren Komponenten von NovaOS bildet.