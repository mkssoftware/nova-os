# NPSPEC-RENDER-COST-0001 – Nova Rendering Cost Estimation

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Render Engine

## 1. Zweck

Nova Rendering Cost Estimation definiert die Abschätzung des erforderlichen Rechen-, Speicher- und Synchronisationsaufwands grafischer Rendering-Operationen.

Ziel ist die Auswahl effizienter Rendering-Strategien, ohne sämtliche möglichen Verfahren tatsächlich ausführen zu müssen.

Die Kostenabschätzung dient insbesondere Nova Adaptive Rendering Strategy, dem Nova Render Graph und dem Nova Frame Scheduler.

## 2. Architekturprinzipien

- **Predictive:** Verarbeitungskosten werden vor der Ausführung abgeschätzt.
- **Measurement-Based:** Tatsächliche Laufzeitmessungen können Schätzwerte verbessern.
- **Hardware-Aware:** Hardwarefähigkeiten und Leistungsunterschiede werden berücksichtigt.
- **Dependency-Aware:** Grafische Abhängigkeiten fließen in die Berechnung ein.
- **Region-Aware:** Größe und Verteilung betroffener Bildbereiche werden berücksichtigt.
- **Resource-Aware:** Speicherbedarf und Datenübertragungen beeinflussen die Kosten.
- **Low-Overhead:** Die Kostenabschätzung darf selbst keinen unverhältnismäßigen Aufwand verursachen.
- **Deterministic:** Ein reproduzierbarer Schätzmodus wird unterstützt.

## 3. Kostenmodell

NovaOS verwendet ein mehrdimensionales Kostenmodell.

Ein Cost Estimate enthält mindestens:

- **CPU Cost:** Erwarteter CPU-Verarbeitungsaufwand.
- **GPU Cost:** Erwarteter GPU-Ausführungsaufwand.
- **Memory Cost:** Benötigter zusätzlicher Speicher.
- **Bandwidth Cost:** Erwartete Speicher- und Datenübertragungen.
- **Synchronization Cost:** Aufwand erforderlicher Synchronisation.
- **Latency Cost:** Erwartete Fertigstellungszeit.
- **Confidence:** Zuverlässigkeit der Schätzung.

Die Kosten DÜRFEN in unterschiedlichen Einheiten erfasst werden.

Eine gewichtete Gesamtbewertung DARF zur Strategieauswahl verwendet werden.

## 4. Einflussfaktoren

Die Kostenabschätzung berücksichtigt insbesondere:

- Größe und Anzahl der Damage Regions
- Anzahl sichtbarer Scene Nodes
- Anzahl erforderlicher Render Passes
- Anzahl der Draw Calls
- Geometriekomplexität
- Shader- und Materialkomplexität
- Anzahl und Größe verwendeter Texturen
- Verfügbare Cache-Ergebnisse
- CPU- und GPU-Auslastung
- Ressourcenübertragungen
- Hardwarefähigkeiten

Nicht verfügbare Informationen MÜSSEN durch definierte konservative Annahmen behandelt werden.

## 5. Rendering-Kosten

NovaOS SOLL die Kosten einzelner Rendering-Operationen abschätzen können.

Dazu gehören:

- Draw Commands
- Compute Dispatches
- Rasterisierung
- Text Rendering
- Geometrieverarbeitung
- Materialeffekte
- Buffer-Kopien
- Ressourcenübergänge

Die Kosten einer Operation DÜRFEN anhand ihrer Parameter, ihrer Eingabedaten und vorhandener Messwerte bestimmt werden.

## 6. Partial vs. Full Rendering

Die Kostenabschätzung MUSS Partial Rendering und Full Rendering vergleichen können.

Dabei werden berücksichtigt:

- Betroffene Pixelanzahl
- Anzahl einzelner Regionen
- Aufwand der Regionsverwaltung
- Zusätzliche Draw Calls
- Erforderliche Buffer-Erhaltung
- Effektabhängigkeiten
- Kosten einer vollständigen Neuberechnung

Partial Rendering SOLL bevorzugt werden, wenn dessen erwartete Gesamtkosten geringer sind.

Eine geringe Damage-Fläche allein garantiert keine geringeren Rendering-Kosten.

## 7. Cache Cost Estimation

NovaOS SOLL die Kosten einer Cache-Wiederverwendung mit denen einer Neuberechnung vergleichen.

Dabei gilt:

**Reuse Cost = Validation Cost + Access Cost + Synchronization Cost**

**Recalculation Cost = Preparation Cost + Execution Cost + Resource Cost**

Die Formeln beschreiben logische Kostenbestandteile und keine zwingend additiven Laufzeiten.

Cache-Ergebnisse SOLLEN verwendet werden, wenn ihre Wiederverwendung einen ausreichenden Vorteil bietet.

Ungültige Cache-Ergebnisse DÜRFEN nicht als verfügbare Alternative bewertet werden.

## 8. Material Cost Estimation

Die Nova Material Engine stellt Informationen zur erwarteten Verarbeitungskomplexität bereit.

Besonders berücksichtigt werden:

- Blur-Radius
- Refraction
- Reflection
- Transparenz
- Backdrop Sampling
- Liquid Glass
- Mehrstufige Materialeffekte

Die Kostenabschätzung MUSS zusätzliche Eingabebereiche und erforderliche Render Passes berücksichtigen.

Materialeffekte DÜRFEN nicht ausschließlich anhand ihrer sichtbaren Ausgabefläche bewertet werden.

## 9. Render Graph Cost Estimation

Der Nova Render Graph SOLL Kosteninformationen für seine Render Passes bereitstellen können.

Diese umfassen:

- Erwartete Ausführungszeit
- Ressourcenbedarf
- Abhängigkeiten
- Synchronisationsaufwand
- Parallelisierungsmöglichkeiten
- Wiederverwendbare Ergebnisse

Die Gesamtlatenz MUSS kritische Abhängigkeitspfade berücksichtigen.

Parallel ausführbare Operationen DÜRFEN bei der Latenzabschätzung nicht grundsätzlich als vollständig sequenziell behandelt werden.

## 10. Hardwareprofil

NovaOS SOLL hardwareabhängige Kostenprofile unterstützen.

Diese können Informationen enthalten über:

- CPU-Leistung
- GPU-Leistung
- Speicherbandbreite
- Ressourcenübertragungskosten
- Unterstützte Grafikfunktionen
- Synchronisationskosten
- Rendering-Backend

Hardwareprofile DÜRFEN anhand tatsächlicher Messungen angepasst werden.

Nicht unterstützte Rendering-Verfahren MÜSSEN von der Strategieauswahl ausgeschlossen werden.

## 11. Performance Feedback

Nova Graphics Performance Metrics stellt tatsächliche Messwerte bereit.

NovaOS SOLL insbesondere erfassen können:

- CPU-Vorbereitungszeit
- GPU-Ausführungszeit
- Render-Pass-Latenz
- Speicherverbrauch
- Datenübertragungsvolumen
- Cache-Hit-Rate
- Synchronisationswartezeiten

Die Messwerte DÜRFEN zur Verbesserung zukünftiger Kostenschätzungen verwendet werden.

Messungen MÜSSEN dem jeweiligen Rendering-Backend und Hardwareprofil zugeordnet werden können.

## 12. Adaptive Kalibrierung

NovaOS DARF Kostenmodelle während des Betriebs kalibrieren.

Dabei SOLL:

- Die Abweichung zwischen Schätzung und Messung ermittelt werden.
- Eine geeignete Glättung kurzfristiger Schwankungen erfolgen.
- Wiederkehrende Fehlbewertungen erkannt werden.
- Die Anpassung ohne zusätzliche Rendering-Ausführungen möglich sein.

Eine Kalibrierung DARF keine unkontrollierten Strategiewechsel verursachen.

## 13. Unsicherheit und Entscheidungsstabilität

Kostenschätzungen besitzen unterschiedliche Genauigkeiten.

NovaOS MUSS unsichere Schätzwerte erkennen können.

Bei ähnlichen erwarteten Kosten SOLL eine bestehende gültige Strategie beibehalten werden.

Ein Strategiewechsel SOLL erst erfolgen, wenn sein erwarteter Vorteil den zusätzlichen Wechselaufwand ausreichend übersteigt.

Hierfür DÜRFEN Schwellenwerte und Hysterese verwendet werden.

## 14. Deterministic Mode

NovaOS MUSS einen deterministischen Kostenabschätzungsmodus unterstützen können.

In diesem Modus:

- Werden definierte Kostenmodelle verwendet.
- Bleiben Eingabeparameter nachvollziehbar.
- Werden nicht reproduzierbare Laufzeitänderungen ausgeschlossen.
- Erzeugen identische Eingaben identische Schätzergebnisse.

Dynamische Kalibrierung DARF in diesem Modus deaktiviert werden.

## 15. Integration

Die Verantwortlichkeiten sind wie folgt getrennt:

- **Rendering Cost Estimation:** Ermittelt erwartete Verarbeitungskosten.
- **Adaptive Rendering Strategy:** Wählt anhand der Kosten geeignete Verfahren.
- **Render Graph:** Plant Rendering-Operationen und deren Abhängigkeiten.
- **Frame Scheduler:** Berücksichtigt Latenz und zeitliche Anforderungen.
- **Graphics Performance:** Erfasst tatsächliche Laufzeitmessungen.

Rendering Cost Estimation trifft keine eigenständigen Entscheidungen über die endgültige Rendering-Strategie.

## 16. Fehlerbehandlung

Bei fehlenden oder ungültigen Kosteninformationen MUSS NovaOS definierte Standardwerte oder konservative Schätzverfahren verwenden können.

Eine fehlgeschlagene Kostenabschätzung DARF die Rendering-Ausführung nicht verhindern.

Die tatsächliche Rendering-Korrektheit MUSS unabhängig von der Genauigkeit der Kostenschätzung gewährleistet bleiben.

## 17. Verbindliche Anforderungen

1. NovaOS MUSS Rendering-Kosten abschätzen können.
2. Das Kostenmodell MUSS CPU-, GPU-, Speicher- und Synchronisationsaufwand berücksichtigen können.
3. Rendering-Latenz MUSS gesondert bewertbar sein.
4. Damage Regions und grafische Abhängigkeiten MÜSSEN berücksichtigt werden.
5. Partial und Full Rendering MÜSSEN vergleichbar sein.
6. Cache-Wiederverwendung und Neuberechnung SOLLEN anhand ihrer Kosten verglichen werden.
7. Material- und Effektkomplexität MUSS berücksichtigt werden können.
8. Render-Graph-Abhängigkeiten MÜSSEN bei der Latenzabschätzung berücksichtigt werden.
9. Hardwareabhängige Kostenprofile SOLLEN unterstützt werden.
10. Tatsächliche Performance-Messwerte SOLLEN zur Kalibrierung verwendet werden können.
11. Die Unsicherheit von Kostenschätzungen MUSS darstellbar sein.
12. Unnötige Strategiewechsel SOLLEN vermieden werden.
13. Die Kostenabschätzung MUSS mit Nova Adaptive Rendering Strategy integrierbar sein.
14. Ein deterministischer Schätzmodus MUSS verfügbar sein.
15. Der Aufwand der Kostenabschätzung SOLL deutlich unter dem erwarteten Optimierungsnutzen liegen.
16. Ungenaue oder fehlende Kostenschätzungen DÜRFEN die visuelle Korrektheit nicht beeinträchtigen.

## 18. Ergebnis

Nova Rendering Cost Estimation stellt eine systematische Grundlage zur Bewertung unterschiedlicher Rendering-Verfahren bereit.

Durch die Kombination aus Kostenmodellen, Hardwareprofilen, Abhängigkeitsanalyse und tatsächlichen Performance-Messungen kann NovaOS den voraussichtlichen Aufwand grafischer Operationen bestimmen.

Damit ermöglicht das System fundierte Entscheidungen zwischen Partial Rendering, Full Rendering, Cache-Wiederverwendung und alternativen Ausführungsstrategien, ohne unnötige Berechnungen durchführen zu müssen.