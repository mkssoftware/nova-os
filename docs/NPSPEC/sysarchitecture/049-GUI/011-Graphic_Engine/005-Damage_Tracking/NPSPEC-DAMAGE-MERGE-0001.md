# NPSPEC-DAMAGE-MERGE-0001 – Nova Damage Region Merging

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Damage Tracking

## 1. Zweck

Nova Damage Region Merging definiert die effiziente Zusammenfassung mehrerer Damage Regions zu optimierten Aktualisierungsbereichen.

Ziel ist die Reduzierung von Verwaltungsaufwand, Rendering-Operationen und GPU-Befehlen, ohne unnötig große Bildbereiche neu zu berechnen.

Die Zusammenfassung erfolgt abhängig von Geometrie, Rendering-Kosten und tatsächlichen grafischen Abhängigkeiten.

## 2. Architekturprinzipien

- **Region-Based:** Damage wird als Menge räumlicher Bereiche verwaltet.
- **Cost-Aware:** Zusammenfassungen berücksichtigen zusätzliche Rendering-Kosten.
- **Conservative:** Erforderliche Aktualisierungsbereiche bleiben vollständig erhalten.
- **Incremental:** Neue Damage Regions können bestehende Regionen erweitern.
- **Adaptive:** Die Strategie passt sich an Anzahl und Verteilung der Regionen an.
- **Material-Aware:** Effektabhängigkeiten werden berücksichtigt.
- **Deterministic:** Identische Eingaben erzeugen reproduzierbare Ergebnisse.

## 3. Merge Model

Eine Merge Operation verarbeitet mehrere Damage Regions und erzeugt eine optimierte Regionsmenge.

Dabei werden berücksichtigt:

- **Source Regions:** Ursprüngliche Damage-Bereiche.
- **Target Surface:** Zugehörige Ausgabe oder Surface.
- **Coordinate Space:** Gemeinsames Koordinatensystem.
- **Merged Regions:** Ergebnis der Zusammenfassung.
- **Merge Cost:** Erwarteter zusätzlicher Aufwand.
- **State Version:** Zugehörige Zustandsversion.

Regionen unterschiedlicher Surfaces DÜRFEN nicht ohne definierte Koordinaten- und Abhängigkeitsabbildung zusammengeführt werden.

## 4. Merge Strategies

NovaOS SOLL folgende Strategien unterstützen:

| Strategie | Beschreibung |
|---|---|
| Overlap Merge | Vereinigt überlappende Regionen |
| Adjacent Merge | Vereinigt unmittelbar benachbarte Regionen |
| Proximity Merge | Vereinigt nahe beieinanderliegende Regionen |
| Bounding Merge | Ersetzt mehrere Regionen durch ein Begrenzungsrechteck |
| Cost-Based Merge | Wählt Zusammenfassungen anhand geschätzter Kosten |
| Full Damage | Invalidiert die gesamte betroffene Surface |

Die Strategien DÜRFEN kombiniert werden.

## 5. Merge Conditions

Damage Regions DÜRFEN zusammengefasst werden, wenn:

1. Sie demselben gültigen Zielbereich zugeordnet sind.
2. Ihre Koordinatensysteme kompatibel sind.
3. Sämtliche ursprünglichen Damage-Bereiche erhalten bleiben.
4. Die Zusammenfassung keine erforderlichen Zustandsinformationen verliert.
5. Die erwarteten Gesamtkosten vertretbar sind.

Eine Zusammenfassung DARF zusätzliche unveränderte Pixel einschließen, sofern deren Verarbeitung zulässig ist.

## 6. Cost-Based Merging

NovaOS SOLL die Kosten einer Zusammenfassung bewerten.

Dabei werden insbesondere berücksichtigt:

- Anzahl der Damage Regions
- Zusätzlich zu rendernde Pixel
- CPU-Verwaltungsaufwand
- GPU-Command-Overhead
- Scissor- und Render-Pass-Wechsel
- Material- und Effektkosten
- Speicherbandbreite

Eine Zusammenfassung SOLL nur erfolgen, wenn ihre erwarteten Gesamtkosten geringer sind als die getrennte Verarbeitung.

## 7. Region Fragmentation

Viele kleine Damage Regions können die Rendering-Leistung beeinträchtigen.

NovaOS SOLL deshalb die Fragmentierung überwachen.

Bei hoher Fragmentierung DÜRFEN:

- Benachbarte Regionen zusammengefasst werden.
- Größere Begrenzungsrechtecke verwendet werden.
- Regionale Gruppen gebildet werden.
- Die gesamte Surface invalidiert werden.

Die maximale Regionsanzahl SOLL konfigurierbar sein.

Ein Wechsel zu Full Damage MUSS möglich sein.

## 8. Material-Aware Merging

Materialeffekte können unterschiedliche Rendering-Kosten verursachen.

NovaOS MUSS deshalb berücksichtigen, dass eine Zusammenfassung zusätzliche Effektberechnungen auslösen kann.

Dies betrifft insbesondere:

- Blur
- Refraction
- Reflection
- Shadows
- Liquid Glass

Die Zusammenfassung von Damage Regions DARF erforderliche Sampling- und Backdrop-Abhängigkeiten nicht entfernen.

## 9. Incremental Merging

Neue Damage Regions SOLLEN unmittelbar in bestehende Regionsmengen integriert werden können.

Dabei gilt:

- Überlappende Bereiche können sofort vereinigt werden.
- Bereits optimierte Regionen dürfen wiederverwendet werden.
- Neue Zustandsänderungen müssen erhalten bleiben.
- Laufende Rendering-Aufträge dürfen nicht verändert werden.

Die Zusammenfassung MUSS versionssicher erfolgen.

## 10. Parallel Processing

NovaOS DARF unabhängige Regionsgruppen parallel optimieren.

Dabei MÜSSEN:

- Gemeinsame Datenstrukturen geschützt werden.
- Überschneidungen korrekt behandelt werden.
- Ergebnisse verlustfrei zusammengeführt werden.
- Deterministische Ergebnisse möglich bleiben.

Eine Parallelisierung SOLL nur erfolgen, wenn ihr Nutzen den zusätzlichen Verwaltungsaufwand übersteigt.

## 11. Integration

Nova Damage Region Merging arbeitet mit folgenden Subsystemen zusammen:

- **Damage Region Management:** Verwaltung der ursprünglichen Regionen.
- **Damage Propagation:** Bereitstellung betroffener Bereiche.
- **Damage Dependency Analysis:** Berücksichtigung grafischer Abhängigkeiten.
- **Render Graph:** Optimierung erforderlicher Rendering-Operationen.
- **Render Engine:** Region-Based und Scissor Rendering.
- **Compositor:** Inkrementelle Zusammensetzung.
- **Frame Scheduler:** Zusammenfassung mehrerer Änderungen innerhalb eines Frames.

Die Entscheidung über die tatsächliche Rendering-Strategie verbleibt bei Render Graph und Render Engine.

## 12. Fehlerbehandlung

NovaOS MUSS ungültige Merge-Ergebnisse erkennen können.

Dazu gehören:

- Verlorene Damage-Bereiche
- Ungültige Koordinatentransformationen
- Fehlerhafte Regionsgrenzen
- Inkompatible Zustandsversionen
- Nicht berücksichtigte Effektabhängigkeiten

Bei fehlgeschlagener Optimierung MÜSSEN die ursprünglichen Damage Regions erhalten bleiben.

Ist deren korrekte Verarbeitung nicht gewährleistet, MUSS eine konservative Invalidierung erfolgen.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS mehrere Damage Regions zusammenfassen können.
2. Sämtliche ursprünglichen Damage-Bereiche MÜSSEN erhalten bleiben.
3. Überlappende Regionen SOLLEN automatisch vereinigt werden.
4. Benachbarte Regionen DÜRFEN zusammengefasst werden.
5. Die Zusammenfassung SOLL anhand geschätzter Gesamtkosten erfolgen.
6. Zusätzliche Rendering-Flächen MÜSSEN bei der Kostenbewertung berücksichtigt werden.
7. Die Regionsfragmentierung SOLL begrenzt werden.
8. Material- und Effektabhängigkeiten MÜSSEN erhalten bleiben.
9. Neue Damage Regions MÜSSEN inkrementell integriert werden können.
10. Zustandsversionen MÜSSEN berücksichtigt werden.
11. Full Damage MUSS als Optimierungs- und Sicherheitsfallback verfügbar sein.
12. Unabhängige Regionsgruppen DÜRFEN parallel verarbeitet werden.
13. Ungültige Merge-Ergebnisse DÜRFEN nicht verwendet werden.
14. Die ursprünglichen Regionen MÜSSEN bei fehlgeschlagener Optimierung wiederherstellbar bleiben.
15. Identische Eingaben MÜSSEN im deterministischen Modus reproduzierbare Ergebnisse erzeugen.

## 14. Ergebnis

Nova Damage Region Merging reduziert die Anzahl erforderlicher Aktualisierungsbereiche durch intelligente und kostenbasierte Zusammenfassung.

Dabei werden unnötige Rendering-Aufrufe vermieden, ohne die grafische Korrektheit zu beeinträchtigen.

Damit unterstützt das Subsystem eine effiziente inkrementelle Grafikverarbeitung und minimiert den Aufwand für Partial Rendering und Incremental Composition.