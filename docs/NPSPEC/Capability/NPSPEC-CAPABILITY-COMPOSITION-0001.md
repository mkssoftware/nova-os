# NPSPEC-CAPABILITY-COMPOSITION-0001: Zusammensetzung mehrerer Fähigkeiten

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Version:** 1.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Gesamte Nova Platform
- **Referenzen:** ADR-CAPABILITY-0001, ADR-CAPABILITY-0002, ADR-CAPABILITY-0012, ADR-CAPABILITY-0013, NPSPEC-CAPABILITY-0001, NPSPEC-CAPABILITY-MODEL-0001, NPSPEC-CAPABILITY-DEPENDENCY-0001, NPSPEC-CAPABILITY-DISCOVERY-0001

---

# 1. Zweck

Diese Spezifikation definiert die Zusammensetzung (Composition) mehrerer Nova Capabilities zu größeren fachlichen Einheiten.

Eine Composition beschreibt die koordinierte Zusammenarbeit mehrerer eigenständiger Fähigkeiten, ohne deren Eigenständigkeit aufzuheben.

Sie bildet die Grundlage für:

- Capability Pipelines
- Solutions
- Workflows
- Intent-Ausführung
- Automatisierung
- Wiederverwendung
- Orchestrierung

---

# 2. Ziele

Das Compositionsmodell verfolgt folgende Ziele:

- modulare Wiederverwendung
- lose Kopplung
- deterministische Ausführung
- hohe Erweiterbarkeit
- klare Verantwortlichkeiten
- einfache Wartbarkeit
- Parallelisierung
- Wiederverwendbarkeit von Teilabläufen

Modulare Komposition über klar definierte Schnittstellen verbessert Erweiterbarkeit und Wartbarkeit komplexer Softwaresysteme. :contentReference[oaicite:0]{index=0}

---

# 3. Grundprinzipien

Eine Composition besteht ausschließlich aus vorhandenen Capabilities.

Eine Composition:

- besitzt keinen eigenen fachlichen Algorithmus,
- verändert keine Capability,
- verbindet ausschließlich Capability Contracts,
- bleibt deklarativ beschreibbar.

---

# 4. Architekturübersicht

```text
Composition

│

├── Capabilities

├── Datenflüsse

├── Kontrollfluss

├── Bedingungen

├── Parameterbindungen

├── Fehlerbehandlung

├── Ereignisse

├── Ausführungsregeln

└── Ergebnisse
```

---

# 5. Composition

Eine Composition beschreibt:

- welche Capabilities verwendet werden,
- in welcher Reihenfolge sie ausgeführt werden,
- welche Daten übertragen werden,
- welche Bedingungen gelten.

Die Composition ist selbst keine Capability.

---

# 6. Composition Identifier

Jede Composition besitzt:

```text
Composition ID

Version

Name

Beschreibung
```

Beispiel:

```text
nova.solution.document.translate
```

---

# 7. Composition Graph

Intern wird jede Composition als gerichteter Graph dargestellt.

```text
Capability

↓

Capability

↓

Capability

↓

Ergebnis
```

Jeder Knoten repräsentiert genau eine Capability.

---

# 8. Datenfluss

Der Datenfluss verbindet Ausgänge mit Eingängen.

```text
OCR

↓

Text

↓

Übersetzung

↓

Text

↓

PDF Export
```

Nur kompatible Datentypen dürfen verbunden werden.

---

# 9. Kontrollfluss

Der Kontrollfluss beschreibt die Ausführungsreihenfolge.

Unterstützt werden:

```text
Sequenziell

Parallel

Bedingt

Wiederholung

Verzweigung

Zusammenführung
```

---

# 10. Sequenzielle Composition

Die häufigste Form.

```text
A

↓

B

↓

C

↓

D
```

Jede Capability startet nach erfolgreichem Abschluss ihres Vorgängers.

---

# 11. Parallele Composition

Unabhängige Capabilities dürfen gleichzeitig ausgeführt werden.

```text
        A

      ↙   ↘

     B     C

      ↘   ↙

        D
```

Die Runtime entscheidet über die tatsächliche Parallelisierung.

---

# 12. Bedingte Composition

Capabilities können abhängig von Bedingungen ausgeführt werden.

```text
OCR erfolgreich?

↓

Ja → Translate

↓

Nein → Fehlerbehandlung
```

Bedingungen sind deklarativ definiert.

---

# 13. Wiederholungen

Schleifen dürfen Bestandteil einer Composition sein.

Beispiele:

```text
Für jedes Dokument

↓

OCR

↓

Ergebnis sammeln
```

Die maximale Wiederholungszahl kann begrenzt werden.

---

# 14. Verzweigungen

Mehrere alternative Pfade sind zulässig.

```text
Bild?

↓

Ja → OCR

↓

Nein → Dokument Parser
```

Die Auswahl erfolgt anhand deklarierter Regeln.

---

# 15. Zusammenführung

Mehrere Ergebnisse können zusammengeführt werden.

```text
Audio

↓

Transkript

↓

Übersetzung

↓

Zusammenfassung

↓

Report
```

Zusammenführungen besitzen definierte Merge-Regeln.

---

# 16. Parameterbindungen

Parameter können zwischen Capabilities weitergegeben werden.

Beispiel:

```text
Bildgröße

↓

Resize

↓

Thumbnail Generator
```

Parameterbindungen werden deklarativ beschrieben.

---

# 17. Ereignisse

Capabilities können Ereignisse austauschen.

Beispiele:

```text
Started

Progress

Completed

Warning

Cancelled

Error
```

Ereignisse dürfen den Datenfluss nicht ersetzen.

---

# 18. Fehlerbehandlung

Jede Composition definiert Fehlerregeln.

Beispiele:

```text
Abbruch

Wiederholung

Alternative Capability

Rollback

Ignorieren
```

Fehlerstrategien werden pro Verbindung definiert.

---

# 19. Dynamische Composition

Der Intent Planner darf Compositionen zur Laufzeit erzeugen.

```text
Benutzerziel

↓

Intent Planner

↓

Composition

↓

Pipeline

↓

Runtime
```

Dynamisch erzeugte Compositionen werden wie statische behandelt.

---

# 20. Verschachtelte Composition

Compositionen dürfen andere Compositionen enthalten.

```text
Composition

│

├── OCR Workflow

├── Translate Workflow

└── Export Workflow
```

Hierdurch entstehen wiederverwendbare Bausteine.

---

# 21. Wiederverwendung

Eine Composition darf beliebig oft verwendet werden.

Beispiel:

```text
OCR Workflow

↓

Rechnung

↓

Vertrag

↓

Brief

↓

Archiv
```

Die Composition bleibt unverändert.

---

# 22. Ausführungsmodell

Die Runtime verarbeitet Compositionen wie folgt:

```text
Composition

↓

Validierung

↓

Dependency Resolution

↓

Capability Graph

↓

Scheduling

↓

Execution

↓

Ergebnisse
```

Alle Schritte erfolgen deterministisch.

---

# 23. Optimierung

Die Runtime darf Compositionen optimieren.

Beispiele:

- Parallelisierung
- Zwischenspeicherung
- Ressourcenoptimierung
- Hardwarebeschleunigung
- Zusammenfassung identischer Berechnungen

Die fachliche Bedeutung darf dabei niemals verändert werden.

---

# 24. YAML-Beispiel

```yaml
composition:

  id: nova.solution.translate.pdf

  version: 1.0

  steps:

    - capability: nova.document.ocr

    - capability: nova.language.translate

    - capability: nova.document.export.pdf

  execution:

    mode: sequential
```

---

# 25. Validierungsregeln

Eine Composition gilt als gültig, wenn:

- alle referenzierten Capabilities existieren,
- alle Capability Contracts kompatibel sind,
- Datentypen verbunden werden können,
- keine unzulässigen Zyklen entstehen,
- Bedingungen vollständig definiert sind,
- Fehlerregeln vorhanden sind.

Ungültige Compositionen dürfen nicht ausgeführt werden.

---

# 26. Qualitätsanforderungen

Das Compositionsmodell muss:

- vollständig deklarativ sein,
- deterministisch ausführbar sein,
- parallele Ausführung unterstützen,
- verschachtelte Compositionen ermöglichen,
- Wiederverwendung fördern,
- automatische Optimierungen erlauben,
- reproduzierbare Ergebnisse liefern,
- unabhängig von Implementierungen bleiben.

Die Komposition unabhängiger Module über stabile Verträge ermöglicht den Aufbau größerer Systeme bei gleichzeitiger Wahrung klarer Verantwortlichkeiten und Austauschbarkeit der Komponenten. :contentReference[oaicite:1]{index=1}

---

# 27. Konformitätskriterien

Eine Composition erfüllt diese Spezifikation, wenn:

- ausschließlich registrierte Capabilities verwendet werden,
- alle Verbindungen typkompatibel sind,
- der Kontrollfluss vollständig definiert ist,
- Fehlerbehandlung beschrieben wird,
- Ausführungsregeln eindeutig sind,
- die Composition durch die Runtime validiert und ausgeführt werden kann,
- identische Eingaben reproduzierbare Ergebnisse liefern.

---

# 28. Zusammenfassung

Das Compositionsmodell definiert die standardisierte Zusammensetzung mehrerer Nova Capabilities zu komplexen fachlichen Abläufen.

Durch deklarative Graphen, typisierte Datenflüsse, klar definierten Kontrollfluss sowie standardisierte Regeln für Bedingungen, Parallelisierung und Fehlerbehandlung entstehen wiederverwendbare, reproduzierbare und skalierbare Lösungen.

Die Composition bildet damit die Brücke zwischen einzelnen Capabilities und vollständigen Workflows, Solutions und Intent-basierten Ausführungen innerhalb der Nova Platform.