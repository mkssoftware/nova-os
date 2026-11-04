# NPSPEC-LIQUID-INCREMENTAL-0001 – Nova Incremental Liquid Glass Rendering

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine / Nova Liquid Glass

## 1. Zweck

Nova Incremental Liquid Glass Rendering definiert die inkrementelle Berechnung und Darstellung von Liquid-Glass-Materialien innerhalb von NovaOS.

Ziel ist, ausschließlich tatsächlich veränderte Bereiche und Effekte neu zu berechnen, während unveränderte Ergebnisse erhalten bleiben.

Liquid Glass MUSS ereignis- und änderungsgesteuert arbeiten. Eine permanente Neuberechnung unveränderter Glasflächen ist nicht vorgesehen.

## 2. Architekturprinzipien

- **Incremental:** Nur betroffene Ergebnisse werden aktualisiert.
- **Dependency-Driven:** Änderungen werden anhand ihrer tatsächlichen Auswirkungen verarbeitet.
- **Region-Based:** Neuberechnungen erfolgen möglichst auf begrenzten Pixelregionen.
- **Retained:** Gültige Effektergebnisse bleiben erhalten.
- **Cache-Aware:** Berechnete Zwischenstufen werden wiederverwendet.
- **Backdrop-Aware:** Änderungen hinter Glasflächen werden berücksichtigt.
- **Pipeline-Aware:** Unveränderte Effektstufen werden übersprungen.
- **Adaptive:** Partielle und vollständige Berechnung werden kostenabhängig ausgewählt.
- **Deterministic:** Identische Zustände erzeugen reproduzierbare logische Rendering-Entscheidungen.

## 3. Architektur

Nova Incremental Liquid Glass Rendering besteht aus folgenden Komponenten:

| Komponente | Verantwortung |
|---|---|
| Liquid Update Manager | Koordination der Glasaktualisierung |
| Liquid Dependency Resolver | Ermittlung betroffener Effekte |
| Liquid Damage Mapper | Berechnung räumlicher Änderungen |
| Liquid Cache Validator | Prüfung vorhandener Effektergebnisse |
| Liquid Render Planner | Planung notwendiger Renderoperationen |
| Liquid Quality Resolver | Auswahl zulässiger Qualitätsstufen |

Die Komponenten verwenden die bestehenden Subsysteme für Material Dependency Tracking, Damage Tracking, Render Graph und GPU Resource Management.

Eine eigenständige parallele Ressourcen- oder Damage-Verwaltung DARF nicht entstehen.

## 4. Liquid Glass State

Jede Liquid-Glass-Instanz besitzt einen versionierten Materialzustand.

Dieser umfasst mindestens:

- Geometrie und Transformation.
- Transparenz und Tint.
- Blur-Parameter.
- Refraction-Parameter.
- Reflection-Parameter.
- Lighting-Parameter.
- Backdrop-Abhängigkeiten.
- Aktives Qualitätsprofil.
- Gültigkeit vorhandener Effektergebnisse.

Änderungen MÜSSEN atomar als konsistente Zustandsversion veröffentlicht werden.

## 5. Incremental Rendering Process

Der logische Ablauf lautet:

`State Change → Dependency Analysis → Damage Mapping → Cache Validation → Render Graph → Partial Rendering → Composition`

Dabei gilt:

1. Änderungen am Material oder seinen Eingaben werden erkannt.
2. Betroffene Effektstufen werden ermittelt.
3. Räumliche Auswirkungen werden berechnet.
4. Vorhandene Zwischenergebnisse werden validiert.
5. Nur notwendige Renderoperationen werden eingeplant.
6. Ungültige Bereiche werden neu berechnet.
7. Ergebnisse werden nach erfolgreicher Ausführung veröffentlicht.

Unveränderte Ergebnisse SOLLEN ohne erneute GPU-Berechnung verwendet werden.

## 6. Incremental Effect Processing

Die einzelnen Liquid-Glass-Effekte MÜSSEN unabhängig invalidierbar sein, soweit ihre Abhängigkeiten dies zulassen.

| Effekt | Inkrementelles Verhalten |
|---|---|
| Backdrop | Aktualisierung betroffener Hintergrundbereiche |
| Blur | Neuberechnung unter Berücksichtigung des Filterbereichs |
| Refraction | Aktualisierung betroffener Hintergrundabtastungen |
| Reflection | Aktualisierung abhängiger Reflexionsbereiche |
| Lighting | Neuberechnung veränderter Beleuchtungsergebnisse |
| Tint | Aktualisierung betroffener Materialpixel |
| Composition | Zusammenführung gültiger und aktualisierter Ergebnisse |

Änderungen einer Effektstufe MÜSSEN an alle davon abhängigen Stufen weitergegeben werden.

## 7. Backdrop-Aware Rendering

Liquid Glass MUSS Änderungen hinter seiner Oberfläche berücksichtigen.

Dabei gilt:

- Hintergrundänderungen können auch bei unveränderter Glasgeometrie eine Aktualisierung erfordern.
- Blur und Refraction können Eingabepixel außerhalb der sichtbaren Glasfläche benötigen.
- Änderungen an verdeckten, aber für Effekte relevanten Oberflächen MÜSSEN berücksichtigt werden.
- Unveränderte Hintergrundbereiche SOLLEN wiederverwendet werden.
- Geschützte Oberflächeninhalte DÜRFEN nicht unautorisiert abgetastet werden.

Die räumlichen Abhängigkeiten MÜSSEN durch Nova Material Dependency Tracking und Nova Damage Tracking ausgewertet werden.

## 8. Partial Rendering

NovaOS MUSS Liquid Glass innerhalb begrenzter Damage Regions neu berechnen können.

Dabei gilt:

- Ausgabe-Damage und benötigte Eingaberegionen MÜSSEN unterschieden werden.
- Filterradien, Refraction-Verschiebungen und Transformationen MÜSSEN berücksichtigt werden.
- Gültige Bereiche dürfen unverändert bleiben.
- Teilaktualisierungen MÜSSEN mit den verwendeten Render Targets kompatibel sein.
- Bei unklaren Abhängigkeiten MUSS konservativ neu berechnet werden.

Eine vollständige Neuberechnung bleibt zulässig, wenn sie sicherer oder effizienter ist.

## 9. Cache Reuse

Nova Incremental Liquid Glass Rendering MUSS mit Nova Material Effect Caching zusammenarbeiten.

Wiederverwendbare Ergebnisse umfassen:

- Backdrop-Zwischenergebnisse.
- Blur-Ergebnisse.
- Refraction-Ergebnisse.
- Reflection-Ergebnisse.
- Lighting-Ergebnisse.
- Fertige Liquid-Glass-Regionen.

Cache-Einträge MÜSSEN anhand ihrer Eingaben, Versionen, Qualitätsparameter und räumlichen Gültigkeit geprüft werden.

Ungültige Cache-Ergebnisse DÜRFEN nicht verwendet werden.

## 10. Render Graph Integration

Nova Render Graph MUSS die erforderlichen Liquid-Glass-Operationen organisieren.

Dabei gilt:

- Unveränderte Effektstufen SOLLEN übersprungen werden.
- Gültige Cache-Ergebnisse können Render-Passes ersetzen.
- Abhängige Verarbeitungsschritte MÜSSEN korrekt geordnet werden.
- GPU Resource Barriers und Synchronisation bleiben verbindlich.
- Temporäre Ressourcen SOLLEN wiederverwendet werden.

Die Liquid-Glass-Logik beschreibt die erforderlichen Operationen; die Render Engine führt sie aus.

## 11. Frame Scheduling

Liquid Glass MUSS mit Nova Frame Scheduling zusammenarbeiten.

Dabei gilt:

- Änderungen erzeugen bei Bedarf eine Frame-Anforderung.
- Mehrere Änderungen DÜRFEN zusammengefasst werden.
- Animationen benötigen explizite Aktualisierungszeitpunkte.
- Unveränderte Glasflächen DÜRFEN keine neuen Frames erzwingen.
- Fehlgeschlagene oder verworfene Frames DÜRFEN ausstehende Änderungen nicht verlieren.

Ein statischer Liquid-Glass-Desktop SOLL keine fortlaufenden Rendering-Aufträge erzeugen.

## 12. Adaptive Rendering

NovaOS SOLL anhand der Rendering-Kosten zwischen verschiedenen Strategien wählen können:

- Partielle Effektberechnung.
- Wiederverwendung vollständiger Effektergebnisse.
- Zusammenfassung kompatibler Render-Passes.
- Vollständige Neuberechnung.
- Reduzierte Materialqualität.
- Definierter Material-Fallback.

Die Auswahl MUSS die festgelegten Qualitätsrichtlinien und die visuelle Korrektheit berücksichtigen.

## 13. Synchronisation und Konsistenz

NovaOS MUSS konsistente Material- und Ressourcen-Snapshots verwenden.

Dabei gilt:

- Laufende GPU-Operationen verwenden unveränderliche Zustandsversionen.
- Aktualisierte Ergebnisse werden erst nach erfolgreicher Berechnung veröffentlicht.
- Veraltete Ergebnisse DÜRFEN aktuelle Zustände nicht überschreiben.
- Ressourcen bleiben bis zum Abschluss aller GPU-Zugriffe gültig.
- Die sichtbare Darstellung DARF keine Mischung inkompatibler Effektversionen enthalten.

## 14. Fehlerbehandlung und Fallback

Bei folgenden Situationen MUSS eine sichere Behandlung erfolgen:

- Ungültige Effektabhängigkeiten.
- Fehlende Cache-Ergebnisse.
- Unvollständige Damage-Informationen.
- Nicht unterstützte partielle Berechnung.
- GPU-Ressourcenmangel.
- Fehlgeschlagene Shader oder Render-Passes.
- GPU-Geräteverlust.

Bei unsicherer Teilaktualisierung MUSS eine vollständige Neuberechnung möglich sein.

Ist Liquid Glass nicht ausführbar, MUSS Nova Material Rendering Fallback eine geeignete Ersatzdarstellung bereitstellen.

## 15. Verbindliche Anforderungen

1. Liquid Glass MUSS inkrementell renderbar sein.
2. Material- und Backdrop-Änderungen MÜSSEN erkannt werden.
3. Effektabhängigkeiten MÜSSEN separat ausgewertet werden.
4. Räumliche Änderungen MÜSSEN korrekt auf Eingabe- und Ausgaberegionen abgebildet werden.
5. Unveränderte Effektergebnisse SOLLEN wiederverwendet werden.
6. Nova Material Effect Caching MUSS integriert werden.
7. Nova Damage Tracking MUSS berücksichtigt werden.
8. Nova Render Graph MUSS nur erforderliche Verarbeitungsschritte einplanen können.
9. GPU-Ressourcen und Synchronisation MÜSSEN korrekt verwaltet werden.
10. Unveränderte Liquid-Glass-Flächen DÜRFEN keine permanente Neuberechnung auslösen.
11. Partielle und vollständige Berechnung MÜSSEN unterstützt werden.
12. Adaptive Qualitätsstufen SOLLEN berücksichtigt werden.
13. Ungültige oder veraltete Ergebnisse DÜRFEN nicht veröffentlicht werden.
14. Geschützte Oberflächeninhalte MÜSSEN isoliert bleiben.
15. Ein geeigneter Material-Fallback MUSS verfügbar sein.
16. Im deterministischen Modus MÜSSEN logische Aktualisierungsentscheidungen bei identischen Eingaben reproduzierbar sein.

## 16. Ergebnis

Nova Incremental Liquid Glass Rendering ermöglicht die gezielte Aktualisierung komplexer Glasmaterialien anhand tatsächlicher Änderungen und Effektabhängigkeiten.

Durch partielle Berechnung, Wiederverwendung gültiger Zwischenergebnisse und ereignisgesteuerte Frame-Erzeugung werden unnötige GPU-Operationen vermieden.

Damit kann NovaOS Liquid Glass als zentrales Oberflächenmaterial einsetzen, ohne unveränderte Glasflächen permanent neu rendern zu müssen.