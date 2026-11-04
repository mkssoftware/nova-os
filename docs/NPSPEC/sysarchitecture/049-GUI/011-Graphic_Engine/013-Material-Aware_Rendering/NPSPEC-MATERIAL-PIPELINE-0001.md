# NPSPEC-MATERIAL-PIPELINE-0001 – Nova Material Rendering Pipelines

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine / Nova Render Engine

## 1. Zweck

Nova Material Rendering Pipelines definiert die Erstellung, Auswahl und Ausführung materialabhängiger Rendering-Pipelines innerhalb von NovaOS.

Ziel ist die effiziente Verarbeitung von Materialeffekten durch wiederverwendbare, hardwareunabhängig beschriebene Pipelines.

Die Render Engine MUSS ausschließlich die Verarbeitungsschritte ausführen, die für das jeweilige Material und dessen aktuellen Zustand erforderlich sind.

## 2. Architekturprinzipien

- **Declarative:** Material-Pipelines werden unabhängig von der GPU-Implementierung beschrieben.
- **Modular:** Pipelines bestehen aus kombinierbaren Verarbeitungsschritten.
- **Dependency-Driven:** Ausführungsreihenfolge und Ressourcen ergeben sich aus Abhängigkeiten.
- **Incremental:** Unveränderte Pipeline-Ergebnisse werden wiederverwendet.
- **Cache-Aware:** Kompatible Pipeline-Konfigurationen werden zwischengespeichert.
- **Adaptive:** Pipelines können anhand verfügbarer Ressourcen ausgewählt werden.
- **Backend-Independent:** GPU- und Softwareimplementierungen verwenden dasselbe logische Modell.
- **Deterministic:** Identische Eingaben erzeugen reproduzierbare logische Pipeline-Pläne.

## 3. Pipeline Architecture

Nova Material Rendering Pipelines besteht aus folgenden Komponenten:

| Komponente | Verantwortung |
|---|---|
| Pipeline Manager | Verwaltung der Material-Pipelines |
| Pipeline Descriptor | Deklarative Pipeline-Beschreibung |
| Pipeline Builder | Zusammenstellung benötigter Verarbeitungsschritte |
| Pipeline Validator | Prüfung von Kompatibilität und Abhängigkeiten |
| Pipeline Selector | Auswahl geeigneter Pipeline-Varianten |
| Pipeline Cache | Wiederverwendung gültiger Konfigurationen |
| Pipeline Executor | Übergabe an Render Graph und Render Engine |

Die Material-Pipeline beschreibt die Verarbeitung.

Der Render Graph organisiert deren ausführbare Render-Passes.

## 4. Pipeline Model

Jede Material-Pipeline besitzt mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Pipeline ID | Eindeutige Identifikation |
| Pipeline Version | Version der Pipeline-Definition |
| Material Type | Zugehöriger Materialtyp |
| Input Resources | Benötigte Eingaberessourcen |
| Processing Stages | Definierte Verarbeitungsschritte |
| Output Resources | Erzeugte Ergebnisse |
| Dependency Set | Ressourcen- und Effektabhängigkeiten |
| Shader Configuration | Benötigte Shader und Varianten |
| Quality Profile | Unterstützte Qualitätsstufe |
| Backend Requirements | Erforderliche Hardwarefähigkeiten |

Pipeline-Definitionen MÜSSEN nach ihrer Veröffentlichung unveränderlich sein.

## 5. Processing Stages

NovaOS MUSS kombinierbare Material-Verarbeitungsschritte unterstützen.

Dazu gehören:

- **Color:** Farbverarbeitung und Tint.
- **Alpha:** Transparenz und Blending.
- **Backdrop:** Bereitstellung benötigter Hintergrundinformationen.
- **Blur:** Räumliche Hintergrundfilterung.
- **Refraction:** Berechnung optischer Lichtbrechung.
- **Reflection:** Berechnung von Reflexionen.
- **Lighting:** Licht- und Highlight-Verarbeitung.
- **Distortion:** Geometrische oder optische Verzerrung.
- **Composition:** Zusammenführung der Materialergebnisse.

Nicht benötigte Stufen DÜRFEN entfallen.

Kompatible Stufen SOLLEN zusammengefasst werden, wenn dadurch keine Abhängigkeiten verletzt werden.

## 6. Material Pipeline Selection

NovaOS MUSS Pipelines anhand folgender Kriterien auswählen:

1. Materialtyp und Materialeigenschaften.
2. Aktivierte Effekte.
3. Benötigte Eingaberessourcen.
4. GPU- und Backend-Fähigkeiten.
5. Gewähltes Qualitätsprofil.
6. Verfügbarkeit gültiger Pipeline-Konfigurationen.
7. Erwartete Rendering-Kosten.

Die Auswahl MUSS die definierten Materialeigenschaften und Sicherheitsanforderungen erhalten.

## 7. Pipeline Construction

Der logische Ablauf lautet:

`Material State → Effect Resolution → Pipeline Selection → Pipeline Validation → Render Graph → Execution`

Dabei gilt:

- Nur erforderliche Verarbeitungsschritte werden aufgenommen.
- Ressourcenabhängigkeiten MÜSSEN explizit beschrieben werden.
- Verarbeitungsschritte MÜSSEN in gültiger Reihenfolge ausführbar sein.
- Shader- und Ressourcenlayouts MÜSSEN kompatibel sein.
- Temporäre Ressourcen SOLLEN effizient wiederverwendet werden.

Die Pipeline-Konstruktion DARF keine unnötige vollständige Neuberechnung erzwingen.

## 8. Render Graph Integration

Material-Pipelines MÜSSEN in Nova Render Graph integrierbar sein.

Dabei gilt:

- Verarbeitungsschritte werden geeigneten Render-Passes zugeordnet.
- Ressourcenabhängigkeiten werden an den Render Graph übergeben.
- GPU Resource Barriers werden durch die zuständigen Subsysteme geplant.
- Kompatible Render-Passes DÜRFEN zusammengeführt werden.
- Unabhängige Verarbeitungsschritte DÜRFEN parallel ausgeführt werden.
- Die tatsächliche Ausführung erfolgt durch die Render Engine.

Eine Material-Pipeline MUSS nicht zwingend einem einzelnen Render Pass entsprechen.

## 9. Incremental Pipeline Execution

NovaOS MUSS inkrementelle Material-Pipeline-Ausführung unterstützen.

Dabei gilt:

- Änderungen einzelner Materialparameter invalidieren nur betroffene Ergebnisse.
- Unveränderte Verarbeitungsschritte SOLLEN übersprungen werden.
- Gültige Zwischenergebnisse SOLLEN wiederverwendet werden.
- Materialabhängige Damage Regions MÜSSEN berücksichtigt werden.
- Änderungen am Backdrop MÜSSEN abhängige Effekte invalidieren.

Bei unklaren Abhängigkeiten MUSS eine konservative Neuberechnung erfolgen.

## 10. Pipeline Caching

NovaOS SOLL gültige Material-Pipeline-Konfigurationen wiederverwenden.

Cache-Schlüssel MÜSSEN mindestens berücksichtigen:

- Material- und Pipeline-Version.
- Aktivierte Verarbeitungsschritte.
- Shader-Varianten.
- Ressourcenlayouts.
- Backend-Kompatibilität.
- Qualitätsprofil.

Dabei MUSS zwischen Pipeline-Konfigurationen und gerenderten Materialergebnissen unterschieden werden.

Das Pipeline Caching ersetzt nicht Nova Material Effect Caching.

## 11. Liquid Glass Pipeline

NovaOS MUSS komplexe Liquid-Glass-Pipelines unterstützen.

Eine Liquid-Glass-Pipeline kann folgende Verarbeitungsschritte enthalten:

`Backdrop → Blur → Refraction → Reflection → Lighting → Tint → Composition`

Dabei gilt:

- Nicht benötigte Effekte DÜRFEN entfallen.
- Gemeinsame Backdrop-Ergebnisse SOLLEN wiederverwendet werden.
- Änderungen hinter der Glasoberfläche MÜSSEN berücksichtigt werden.
- Blur- und Refraction-Abhängigkeiten MÜSSEN räumlich korrekt behandelt werden.
- Unveränderte Effektergebnisse SOLLEN erhalten bleiben.

Die konkrete Pipeline DARF je nach Materialkonfiguration und GPU-Fähigkeiten variieren.

## 12. Adaptive Pipeline Selection

NovaOS SOLL unterschiedliche Pipeline-Varianten unterstützen.

Mögliche Strategien:

- Vollständige GPU-Pipeline.
- Optimierte GPU-Pipeline.
- Vereinfachte Material-Pipeline.
- Softwarebasierte Pipeline.
- Minimale Solid-Darstellung.

Adaptive Entscheidungen MÜSSEN die festgelegten Qualitätsrichtlinien berücksichtigen.

Eine Qualitätsreduzierung DARF Lesbarkeit und grundlegende Bedienbarkeit nicht beeinträchtigen.

## 13. Pipeline Lifecycle

NovaOS definiert folgende Zustände:

`Defined → Validated → Prepared → Ready → Active → Retired → Released`

Zusätzliche Zustände:

- **Compiling:** Benötigte Shader werden vorbereitet.
- **Failed:** Pipeline-Erstellung fehlgeschlagen.
- **Unsupported:** Backend-Anforderungen nicht erfüllt.
- **Obsolete:** Pipeline-Version nicht mehr aktuell.

Aktive Pipeline-Ressourcen DÜRFEN nicht vor Abschluss ihrer GPU-Verwendung freigegeben werden.

## 14. Sicherheit und Fehlerbehandlung

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Ungültige Pipeline-Definitionen.
- Fehlende Ressourcen.
- Inkompatible Shader-Layouts.
- Nicht unterstützte GPU-Funktionen.
- Ungültige Effektabhängigkeiten.
- Fehlgeschlagene Shader-Kompilierung.
- GPU-Ressourcenmangel.
- Geräteverlust.

Ungültige Pipelines DÜRFEN nicht ausgeführt werden.

Bei Fehlern MUSS ein geeigneter Material-Fallback verwendet oder die betroffene Darstellung kontrolliert abgebrochen werden.

## 15. Verbindliche Anforderungen

1. NovaOS MUSS deklarative Material-Pipelines unterstützen.
2. Material-Pipelines MÜSSEN aus kombinierbaren Verarbeitungsschritten bestehen können.
3. Pipeline-Definitionen MÜSSEN versioniert sein.
4. Pipeline-Abhängigkeiten MÜSSEN explizit beschrieben werden.
5. Nicht benötigte Verarbeitungsschritte SOLLEN entfallen.
6. Pipeline-Konfigurationen MÜSSEN vor der Ausführung validiert werden.
7. Nova Render Graph MUSS integriert werden.
8. Nova Shader System MUSS integriert werden.
9. Inkrementelle Pipeline-Ausführung MUSS unterstützt werden.
10. Gültige Pipeline-Konfigurationen SOLLEN wiederverwendet werden.
11. Materialabhängige Damage Regions MÜSSEN berücksichtigt werden.
12. Liquid-Glass-Pipelines MÜSSEN unterstützt werden.
13. Adaptive Pipeline-Varianten SOLLEN verfügbar sein.
14. GPU-Ressourcen und Synchronisation MÜSSEN korrekt verwaltet werden.
15. Material-Fallbacks MÜSSEN unterstützt werden.
16. Im deterministischen Modus MÜSSEN Pipeline-Auswahl und logische Ausführungsplanung bei identischen Eingaben reproduzierbar sein.

## 16. Ergebnis

Nova Material Rendering Pipelines stellt eine modulare Infrastruktur zur effizienten Ausführung materialabhängiger Grafikoperationen bereit.

Durch deklarative Pipeline-Beschreibungen, dynamische Auswahl, Render-Graph-Integration und inkrementelle Verarbeitung werden unnötige GPU-Operationen reduziert.

Damit ermöglicht NovaOS leistungsfähige und wiederverwendbare Rendering-Pipelines für Solid, Acrylic, Frosted Glass, Crystal und insbesondere Liquid Glass.