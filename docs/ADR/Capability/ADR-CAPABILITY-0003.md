# ADR-CAPABILITY-0003: Fähigkeiten werden über typisierte Ein- und Ausgänge verbunden

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-26
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine
- **Abhängigkeiten:** ADR-CAPABILITY-0001, ADR-CAPABILITY-0002, ADR-DATAFLOW-0001
- **Folgedokumente:** NPSPEC-CAPABILITY-PORT-0001, NPSPEC-DATAFLOW-0001, NPSPEC-DATATYPE-0001

---

## 1. Kontext

NovaOS basiert auf einem fähigkeitsorientierten Softwaremodell.

Einzelne Fähigkeiten werden zu größeren Arbeitsabläufen kombiniert. Damit dies zuverlässig, sicher und automatisierbar möglich ist, benötigen alle Fähigkeiten eine einheitliche und eindeutig definierte Schnittstelle.

Klassische Anwendungen kommunizieren häufig über:

- Dateien,
- Kommandozeilenparameter,
- Umgebungsvariablen,
- proprietäre APIs,
- RPC,
- Netzwerkprotokolle,
- Zwischenablagen,
- gemeinsam genutzte Speicherbereiche.

Diese Schnittstellen sind häufig nicht standardisiert, nur unzureichend dokumentiert oder lediglich technisch beschrieben.

NovaOS verfolgt einen anderen Ansatz.

Jede Fähigkeit beschreibt ihre Ein- und Ausgänge deklarativ und verwendet dabei streng typisierte Ports.

Dadurch kann das Betriebssystem Fähigkeiten automatisch verbinden, validieren und optimieren.

---

## 2. Problemstellung

Ohne standardisierte und typisierte Schnittstellen entstehen zahlreiche Probleme.

### 2.1 Fehlende Interoperabilität

Fähigkeiten unterschiedlicher Entwickler verwenden unterschiedliche Schnittstellen.

Dadurch sinkt die Wiederverwendbarkeit erheblich.

### 2.2 Laufzeitfehler

Inkompatible Daten werden häufig erst während der Ausführung erkannt.

Fehler können dadurch erst nach mehreren Verarbeitungsschritten auftreten.

### 2.3 Unklare Semantik

Ein Datentyp wie `String` oder `ByteArray` beschreibt nicht, welche fachliche Bedeutung die Daten besitzen.

Ein String kann beispielsweise sein:

- ein Dateiname,
- eine URL,
- eine E-Mail-Adresse,
- natürlicher Text,
- JSON,
- XML,
- Quellcode,
- SQL.

### 2.4 Keine automatische Pipeline-Erstellung

Der Pipeline Planner kann Fähigkeiten nicht automatisch kombinieren, wenn deren Schnittstellen nicht eindeutig beschrieben sind.

### 2.5 Schlechte Optimierungsmöglichkeiten

Ohne bekannte Datentypen kann NovaOS weder Speicher noch Hardwarepfade oder Konverter automatisch auswählen.

---

## 3. Entscheidung

Alle Fähigkeiten kommunizieren ausschließlich über **typisierte Ein- und Ausgangsports**.

Jeder Port besitzt mindestens:

- eine eindeutige Kennung,
- eine Richtung,
- einen Datentyp,
- eine Kardinalität,
- einen Pflichtstatus,
- optionale Einschränkungen,
- Sicherheitsinformationen,
- Dokumentation.

Verbindungen zwischen zwei Fähigkeiten dürfen nur hergestellt werden, wenn ihre Ports typkompatibel sind.

Die Typprüfung erfolgt bereits vor der Ausführung einer Pipeline.

---

## 4. Grundprinzipien

### 4.1 Jede Fähigkeit besitzt Ports

Eine Fähigkeit besitzt ausschließlich definierte Schnittstellen.

Beispiel:

```text
Capability

Inputs
├── image

Outputs
├── text
└── layout
```

Direkte Kommunikation außerhalb dieser Ports ist nicht Bestandteil des Capability-Modells.

---

### 4.2 Jeder Port besitzt einen Datentyp

Ein Port muss genau einen Datentyp deklarieren.

Beispiel:

```text
image:
    Nova.Image.Raster

text:
    Nova.Document.PlainText
```

Nicht zulässig sind generische Ports wie:

```text
Object

Any

Variant
```

außer in speziell definierten Infrastrukturkomponenten.

---

### 4.3 Ein Port besitzt eine Richtung

Jeder Port besitzt genau eine Richtung.

Mögliche Richtungen:

```text
Input

Output

EventInput

EventOutput

ControlInput

ControlOutput
```

Die Richtung bestimmt, wie der Port innerhalb einer Pipeline verwendet werden darf.

---

### 4.4 Typkompatibilität ist Voraussetzung

Eine Verbindung ist nur zulässig, wenn Ausgangs- und Eingangstyp kompatibel sind.

Beispiel:

```text
Nova.Image.Raster
        │
        ▼
OCR
        │
        ▼
Nova.Document.PlainText
```

Ungültig wäre:

```text
Nova.Audio.Stream
        │
        ▼
OCR
```

Da OCR ausschließlich Bilddaten akzeptiert.

---

### 4.5 Konvertierungen erfolgen explizit

Eine automatische implizite Typumwandlung findet nicht statt.

Erforderliche Umwandlungen werden durch eigenständige Konverterfähigkeiten durchgeführt.

Beispiel:

```text
Markdown
      │
      ▼
Markdown → HTML
      │
      ▼
HTML
```

Dadurch bleiben sämtliche Transformationen nachvollziehbar.

---

### 4.6 Mehrere Ports sind zulässig

Eine Fähigkeit darf mehrere Eingänge und mehrere Ausgänge besitzen.

Beispiel:

```text
Inputs

├── image
├── language
└── configuration

Outputs

├── text
├── confidence
└── layout
```

Jeder Port wird unabhängig beschrieben.

---

### 4.7 Optionale Ports

Ports können optional sein.

Beispiel:

```text
sourceLanguage
required = false
```

Ist kein Wert vorhanden, verwendet die Fähigkeit ihre definierte Standardstrategie.

---

### 4.8 Ports besitzen Metadaten

Ein Port kann zusätzliche Informationen deklarieren.

Beispiele:

- zulässige Werte,
- Größenbeschränkungen,
- Einheiten,
- Sicherheitsklassifizierung,
- unterstützte Formate,
- Dokumentation,
- Standardwerte.

Diese Metadaten unterstützen den Pipeline Planner bei der automatischen Planung.

---

### 4.9 Ports beschreiben ausschließlich Daten

Ports übertragen Daten oder Ereignisse.

Sie enthalten keine Implementierungsdetails.

Beispielsweise werden folgende Informationen nicht über Ports übertragen:

- Speicheradressen,
- Prozess-IDs,
- interne Klassen,
- Objektinstanzen,
- Betriebssystemhandles.

Die interne Implementierung bleibt vollständig verborgen.

---

### 4.10 Ports sind stabil versioniert

Änderungen an Ports müssen versioniert werden.

Breaking Changes entstehen beispielsweise durch:

- entfernte Ports,
- geänderte Datentypen,
- geänderte Semantik,
- geänderte Kardinalität.

Dadurch bleiben bestehende Pipelines reproduzierbar.

---

## 5. Beispiel

Eine OCR-Fähigkeit kann wie folgt beschrieben werden:

```text
Capability

Input

image
Type:
Nova.Image.Raster

Output

text
Type:
Nova.Document.PlainText

layout
Type:
Nova.Document.LayoutMap
```

Eine Übersetzungsfähigkeit könnte anschließend direkt an den `text`-Port angeschlossen werden.

---

## 6. Typprüfung

Vor dem Start einer Pipeline überprüft NovaOS:

- Existenz aller Ports,
- Datentypen,
- Kardinalität,
- Pflichtparameter,
- Sicherheitsklassifizierung,
- Versionskompatibilität,
- erforderliche Konverter,
- unterstützte Transportmodi.

Nur vollständig gültige Pipelines dürfen ausgeführt werden.

---

## 7. Vorteile

Das Portmodell ermöglicht:

- automatische Pipeline-Erstellung,
- statische Typprüfung,
- frühe Fehlererkennung,
- hohe Wiederverwendbarkeit,
- eindeutige Dokumentation,
- bessere Optimierung,
- einfache Austauschbarkeit,
- verteilte Ausführung,
- automatische Konverter,
- stabile APIs.

---

## 8. Nachteile

Das Modell erfordert:

- detaillierte Typdefinitionen,
- zusätzliche Metadaten,
- größere Descriptor-Dateien,
- ein zentrales Typregister.

Diese Nachteile werden bewusst akzeptiert, da sie langfristig eine deutlich robustere Plattform ermöglichen.

---

## 9. Auswirkungen auf andere Komponenten

Dieses ADR beeinflusst insbesondere:

- Capability Descriptor
- Capability Registry
- Pipeline Planner
- Pipeline Executor
- Nova Type Registry
- Dataflow Engine
- Runtime
- Visual Pipeline Editor
- Capability SDK
- Debugger
- KI-Assistent

Alle diese Komponenten verlassen sich auf typisierte Ports als primären Kommunikationsmechanismus.

---

## 10. Implementierungshinweise

Jeder Capability Descriptor muss alle Ports deklarieren.

Für jeden Port werden mindestens gespeichert:

- Name,
- Richtung,
- Datentyp,
- Pflichtstatus,
- Kardinalität,
- Beschreibung,
- Version,
- Sicherheitsklassifizierung,
- Transportmodus.

Die Runtime darf ausschließlich deklarierte Ports verwenden.

---

## 11. Konformitätskriterien

Eine Fähigkeit gilt als konform, wenn:

- alle Ein- und Ausgänge explizit beschrieben sind,
- jeder Port genau einen Datentyp besitzt,
- keine impliziten Datentypen verwendet werden,
- Ports versioniert werden,
- Portänderungen dokumentiert werden,
- Verbindungen ausschließlich über kompatible Ports erfolgen,
- keine versteckten Kommunikationskanäle existieren.

---

## 12. Zusammenfassung

NovaOS definiert typisierte Ein- und Ausgangsports als einzige standardisierte Kommunikationsschnittstelle zwischen Fähigkeiten.

Durch die Kombination deklarativer Ports, eines zentralen Typsystems und einer statischen Typprüfung entstehen sichere, nachvollziehbare und automatisch planbare Datenflüsse.

Dieses Architekturprinzip bildet gemeinsam mit dem Capability-Modell und dem Dataflow-System das Fundament der gesamten NovaOS-Plattform.