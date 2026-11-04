# NPSPEC-MATERIAL-INCREMENTAL-0001 – Nova Incremental Material Rendering

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine / Nova Render Engine

## 1. Zweck

Nova Incremental Material Rendering definiert die gezielte Neuberechnung ausschließlich veränderter oder ungültiger Materialergebnisse.

Ziel ist die Vermeidung unnötiger GPU-Operationen, insbesondere bei komplexen Materialeffekten wie Liquid Glass, Blur, Refraction und Reflection.

Unveränderte und weiterhin gültige Materialergebnisse SOLLEN ohne erneute Berechnung wiederverwendet werden.

## 2. Architekturprinzipien

- **Change-Driven:** Materialberechnungen erfolgen aufgrund tatsächlicher Änderungen.
- **Dependency-Aware:** Nur abhängige Verarbeitungsschritte werden aktualisiert.
- **Region-Based:** Neuberechnungen werden räumlich begrenzt.
- **Retained:** Gültige Materialergebnisse bleiben erhalten.
- **Cache-Aware:** Zwischenergebnisse werden wiederverwendet.
- **Pipeline-Aware:** Unveränderte Pipeline-Stufen werden übersprungen.
- **Adaptive:** Partielle und vollständige Neuberechnung werden kostenabhängig ausgewählt.
- **Deterministic:** Identische Eingaben erzeugen identische logische Aktualisierungsentscheidungen.

## 3. Architektur

Nova Incremental Material Rendering besteht aus folgenden Komponenten:

| Komponente | Verantwortung |
|---|---|
| Material Update Manager | Koordination inkrementeller Aktualisierungen |
| Change Resolver | Ermittlung geänderter Materialzustände |
| Dependency Evaluator | Bestimmung betroffener Effekte |
| Region Mapper | Berechnung betroffener Ausgaberegionen |
| Cache Validator | Prüfung vorhandener Materialergebnisse |
| Incremental Render Planner | Planung notwendiger Renderoperationen |
| Render Cost Evaluator | Entscheidung zwischen partieller und vollständiger Berechnung |

Die Änderungserkennung erfolgt über Nova Material Dependency Tracking.

Die räumliche Invalidierung erfolgt in Zusammenarbeit mit Nova Damage Tracking.

## 4. Incremental Rendering Model

Jede Materialinstanz MUSS folgende Informationen verwalten können:

| Eigenschaft | Beschreibung |
|---|---|
| Material ID | Eindeutige Materialidentifikation |
| Material Version | Aktuelle Zustandsversion |
| Dependency Version | Version der Materialabhängigkeiten |
| Valid Regions | Gültige Ergebnisbereiche |
| Invalid Regions | Neu zu berechnende Bereiche |
| Cached Results | Verfügbare Zwischenergebnisse |
| Pipeline State | Aktuelle Verarbeitungskonfiguration |
| Render State | Zustand der Materialberechnung |

Materialzustand und gerenderte Ergebnisse MÜSSEN getrennt verwaltet werden.

## 5. Incremental Update Process

Der logische Ablauf lautet:

`Material Change → Dependency Analysis → Damage Mapping → Cache Validation → Render Planning → Partial Execution → Result Publication`

Dabei gilt:

1. Änderungen werden anhand der Materialversion erkannt.
2. Betroffene Effekte werden über den Dependency Graph ermittelt.
3. Räumliche Auswirkungen werden berechnet.
4. Vorhandene Cache-Ergebnisse werden validiert.
5. Ausschließlich erforderliche Verarbeitungsschritte werden geplant.
6. Ungültige Bereiche werden neu berechnet.
7. Ergebnisse werden nach erfolgreicher Ausführung veröffentlicht.

Nicht betroffene Materialbereiche SOLLEN unverändert bleiben.

## 6. Partial Material Rendering

NovaOS MUSS partielle Materialberechnungen unterstützen.

Dabei gilt:

- Änderungen werden als Damage Regions beschrieben.
- Verarbeitungsschritte erhalten die für sie relevanten Eingaberegionen.
- Filterabhängigkeiten MÜSSEN berücksichtigt werden.
- Unveränderte Ausgaberegionen SOLLEN wiederverwendet werden.
- Clipping und Transformationen MÜSSEN korrekt verarbeitet werden.

Die tatsächliche Berechnungsregion DARF größer als die sichtbare Damage Region sein, wenn Effekte zusätzliche Eingabepixel benötigen.

## 7. Incremental Effect Processing

NovaOS SOLL Materialeffekte unabhängig voneinander aktualisieren können.

| Effekt | Inkrementelles Verhalten |
|---|---|
| Color / Tint | Aktualisierung betroffener Materialpixel |
| Transparency | Aktualisierung betroffener Kompositionsbereiche |
| Blur | Neuberechnung unter Berücksichtigung des Filterradius |
| Refraction | Aktualisierung betroffener Hintergrundabtastungen |
| Reflection | Neuberechnung abhängiger Reflexionsbereiche |
| Lighting | Aktualisierung betroffener Beleuchtungsergebnisse |
| Backdrop | Aktualisierung geänderter Hintergrundbereiche |

Unabhängige Effektergebnisse SOLLEN erhalten bleiben.

## 8. Pipeline Stage Reuse

NovaOS MUSS die Wiederverwendung gültiger Pipeline-Zwischenergebnisse ermöglichen.

Dabei gilt:

- Jede wiederverwendbare Stufe besitzt nachvollziehbare Eingabeabhängigkeiten.
- Unveränderte Eingaben erhalten gültige Ergebnisse.
- Änderungen invalidieren nur abhängige Stufen und Regionen.
- Gemeinsame Zwischenergebnisse SOLLEN mehrfach verwendet werden.
- Eine Änderung der Pipeline-Konfiguration MUSS betroffene Ergebnisse invalidieren.

Die eigentliche Speicherung erfolgt über Nova Material Effect Caching.

## 9. Backdrop-Aware Incremental Rendering

Backdrop-abhängige Materialien MÜSSEN Änderungen hinter ihrer Oberfläche berücksichtigen.

Dabei gilt:

- Hintergrundänderungen können Materialergebnisse invalidieren.
- Blur benötigt zusätzliche Eingabepixel entsprechend seiner Filterwirkung.
- Refraction benötigt die tatsächlich abgetasteten Hintergrundregionen.
- Reflection benötigt die relevanten Reflexionsquellen.
- Änderungen verdeckter, aber für Materialeffekte benötigter Inhalte MÜSSEN berücksichtigt werden.

Nicht sicher bestimmbare Abhängigkeiten MÜSSEN konservativ invalidiert werden.

## 10. Adaptive Partial Rendering

NovaOS SOLL zwischen partieller und vollständiger Materialberechnung wählen können.

Die Entscheidung berücksichtigt:

- Größe der betroffenen Regionen.
- Anzahl und Fragmentierung der Damage Regions.
- Kosten der Materialeffekte.
- Verfügbarkeit gültiger Zwischenergebnisse.
- GPU- und Speicherbandbreitenkosten.
- Aufwand zusätzlicher Render-Passes.

Eine vollständige Neuberechnung DARF gewählt werden, wenn sie voraussichtlich effizienter oder zur Sicherstellung der Korrektheit erforderlich ist.

## 11. Render Graph Integration

Nova Incremental Material Rendering MUSS mit Nova Render Graph zusammenarbeiten.

Dabei gilt:

- Nur notwendige Materialoperationen werden zur Ausführung eingeplant.
- Unveränderte Render-Passes SOLLEN entfallen, sofern gültige Ergebnisse vorliegen.
- Ressourcenabhängigkeiten bleiben verbindlich.
- GPU Resource Barriers MÜSSEN korrekt berücksichtigt werden.
- Unabhängige Operationen DÜRFEN parallel ausgeführt werden.

Eine übersprungene Operation MUSS durch ein gültiges Ergebnis ersetzt werden können.

## 12. Frame Scheduling Integration

Nova Incremental Material Rendering MUSS ereignisgesteuert arbeiten.

Dabei gilt:

- Materialänderungen können einen Frame anfordern.
- Unveränderte Materialien erzeugen keinen eigenständigen Rendering-Bedarf.
- Mehrere Änderungen DÜRFEN zusammengefasst werden.
- Animationen werden anhand ihrer tatsächlichen Aktualisierungsanforderungen verarbeitet.
- Fehlgeschlagene oder verworfene Frames DÜRFEN notwendige Materialänderungen nicht verlieren.

Ein unveränderter Materialzustand DARF keine permanente Neuberechnung auslösen.

## 13. Synchronization und Versioning

NovaOS MUSS konsistente Material-Snapshots verwenden.

Dabei gilt:

- Renderoperationen beziehen sich auf definierte Materialversionen.
- Während der GPU-Ausführung verwendete Ressourcen bleiben gültig.
- Neue Materialzustände DÜRFEN laufende Berechnungen nicht verändern.
- Veraltete Ergebnisse DÜRFEN nicht als aktuelle Ergebnisse veröffentlicht werden.
- Ressourcen werden erst nach Abschluss aller relevanten GPU-Zugriffe freigegeben.

## 14. Liquid Glass Integration

Nova Incremental Material Rendering MUSS Liquid Glass besonders effizient unterstützen.

Dabei gilt:

- Backdrop-Ergebnisse SOLLEN wiederverwendet werden.
- Blur wird nur für betroffene Regionen neu berechnet.
- Refraction berücksichtigt veränderte Hintergrundabtastungen.
- Reflection und Lighting werden abhängig von ihren Eingaben aktualisiert.
- Unveränderte Effektschichten SOLLEN erhalten bleiben.
- Materialanimationen invalidieren ausschließlich tatsächlich betroffene Ergebnisse.

Die Liquid-Glass-Darstellung MUSS auch bei partieller Aktualisierung visuell konsistent bleiben.

## 15. Fehlerbehandlung und Fallback

NovaOS MUSS folgende Situationen kontrolliert behandeln:

- Ungültige Materialabhängigkeiten.
- Fehlende Cache-Ergebnisse.
- Unvollständige Damage Regions.
- Nicht unterstützte partielle Renderoperationen.
- Ressourcenmangel.
- GPU-Geräteverlust.
- Fehlgeschlagene Render-Passes.

Bei unsicherer partieller Aktualisierung MUSS eine vollständige Neuberechnung möglich sein.

Falls GPU-Rendering nicht verfügbar ist, MUSS ein geeigneter Material-Fallback verwendet werden.

## 16. Verbindliche Anforderungen

1. NovaOS MUSS inkrementelles Material-Rendering unterstützen.
2. Materialänderungen MÜSSEN versioniert erfasst werden.
3. Nova Material Dependency Tracking MUSS integriert werden.
4. Nova Damage Tracking MUSS räumliche Änderungen bereitstellen.
5. Nur betroffene Materialergebnisse SOLLEN neu berechnet werden.
6. Gültige Zwischenergebnisse SOLLEN wiederverwendet werden.
7. Materialeffekte MÜSSEN ihre räumlichen Abhängigkeiten berücksichtigen.
8. Backdrop-Änderungen MÜSSEN korrekt verarbeitet werden.
9. Nova Material Effect Caching MUSS integrierbar sein.
10. Nova Render Graph MUSS inkrementelle Ausführungspläne unterstützen.
11. Nova Frame Scheduling MUSS ereignisgesteuerte Aktualisierungen ermöglichen.
12. Partielle und vollständige Neuberechnung MÜSSEN auswählbar sein.
13. GPU-Ressourcen und Synchronisation MÜSSEN korrekt verwaltet werden.
14. Veraltete Ergebnisse DÜRFEN nicht als aktuell veröffentlicht werden.
15. Liquid Glass MUSS inkrementell aktualisierbar sein.
16. Bei unklaren Abhängigkeiten MUSS konservativ neu berechnet werden.
17. Im deterministischen Modus MÜSSEN logische Aktualisierungsentscheidungen bei identischen Eingaben reproduzierbar sein.

## 17. Ergebnis

Nova Incremental Material Rendering ermöglicht die gezielte Aktualisierung veränderter Materialbereiche und Verarbeitungsschritte.

Durch Material Dependency Tracking, Damage Regions, Pipeline Stage Reuse und Material Effect Caching werden unnötige GPU-Berechnungen vermieden.

Damit kann NovaOS auch komplexe Materialien wie Liquid Glass effizient darstellen, ohne bei jeder Änderung sämtliche Materialeffekte vollständig neu berechnen zu müssen.