# NPSPEC-SCENE-UPDATE-0001 – Nova Incremental Scene Updates

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Scene Graph

## 1. Zweck

Nova Incremental Scene Updates definiert die gezielte Aktualisierung veränderter Szenenelemente und abgeleiteter Szenenergebnisse.

Anstatt den vollständigen Scene Graph nach jeder Zustandsänderung neu auszuwerten, verarbeitet NovaOS ausschließlich die tatsächlich betroffenen Elemente und Abhängigkeiten.

Ziel ist die Minimierung von CPU-Aufwand, Speicherzugriffen und unnötigen Neuberechnungen.

## 2. Architekturprinzipien

- **Incremental:** Nur betroffene Szenenbereiche werden aktualisiert.
- **Change-Driven:** Aktualisierungen basieren auf tatsächlichen Zustandsänderungen.
- **Dependency-Aware:** Abhängigkeiten bestimmen den erforderlichen Aktualisierungsumfang.
- **Retained:** Unveränderte Szenenzustände bleiben erhalten.
- **Transactional:** Aktualisierungen basieren auf konsistenten Zustandsversionen.
- **Lazy Evaluation:** Berechnungen erfolgen möglichst erst bei tatsächlichem Bedarf.
- **Cache-Aware:** Gültige Zwischenergebnisse werden wiederverwendet.
- **Deterministic:** Identische Eingaben erzeugen identische logische Aktualisierungsergebnisse.
- **Renderer-Independent:** Die Szenenaktualisierung ist unabhängig vom Rendering-Backend.

## 3. Aktualisierungsmodell

Jede inkrementelle Szenenaktualisierung basiert auf:

- Veröffentlichter Szenenversion
- Scene Change Set
- Scene Dependency Graph
- Betroffenen Scene Nodes
- Invalidierten Zwischenergebnissen
- Verfügbaren Cache-Einträgen

Die Aktualisierung MUSS auf einer konsistenten Szenenversion erfolgen.

Unveränderte Scene Nodes und gültige Berechnungsergebnisse SOLLEN erhalten bleiben.

## 4. Aktualisierungskategorien

NovaOS unterscheidet insbesondere:

- **Property Update:** Neuberechnung effektiver Eigenschaften.
- **Hierarchy Update:** Aktualisierung struktureller Beziehungen.
- **Layout Update:** Neuberechnung betroffener Layoutbereiche.
- **Transform Update:** Aktualisierung effektiver Transformationen.
- **Geometry Update:** Neuberechnung geometrischer Informationen.
- **Visibility Update:** Aktualisierung der Sichtbarkeit.
- **Material Update:** Aktualisierung materialspezifischer Zustände.
- **Resource Update:** Verarbeitung geänderter Ressourcenreferenzen.
- **Bounds Update:** Aktualisierung grafischer Begrenzungen.

Eine Zustandsänderung DARF mehrere Aktualisierungskategorien betreffen.

## 5. Aktualisierungsablauf

Die Verarbeitung erfolgt grundsätzlich in folgenden Schritten:

1. Übernahme eines veröffentlichten Scene Change Sets.
2. Ermittlung unmittelbar veränderter Scene Nodes.
3. Auswertung des Scene Dependency Graph.
4. Bestimmung betroffener abgeleiteter Ergebnisse.
5. Invalidierung ausschließlich betroffener Cache-Einträge.
6. Bedarfsgerechte Neuberechnung erforderlicher Ergebnisse.
7. Bereitstellung aktualisierter Szeneninformationen.
8. Übergabe relevanter Änderungen an Damage Tracking und Rendering.

Die Verarbeitung DARF mehrere kompatible Change Sets zusammenfassen.

## 6. Hierarchische Aktualisierung

Änderungen innerhalb der Szenenhierarchie können Auswirkungen auf übergeordnete oder untergeordnete Elemente besitzen.

NovaOS MUSS:

- Lokale Änderungen von hierarchischen Auswirkungen unterscheiden.
- Abhängige Teilstrukturen gezielt aktualisieren.
- Transformationen und vererbte Eigenschaften berücksichtigen.
- Strukturelle Änderungen konsistent verarbeiten.
- Unveränderte Teilstrukturen wiederverwenden.

Eine Änderung eines Parent Nodes DARF nicht grundsätzlich eine vollständige Neuberechnung aller Child Nodes erzwingen.

## 7. Lazy Evaluation

Abgeleitete Szenenergebnisse SOLLEN erst berechnet werden, wenn sie tatsächlich benötigt werden.

Dazu gehören insbesondere:

- Effektive Transformationen
- Geometrische Begrenzungen
- Vererbte Eigenschaften
- Layout-Ergebnisse
- Sichtbarkeitsinformationen

Invalidierte Ergebnisse DÜRFEN bis zu ihrer nächsten Verwendung als ungültig markiert bleiben.

Vor ihrer Verwendung MUSS jedoch sichergestellt werden, dass sie zur benötigten Szenenversion passen.

## 8. Cache-Wiederverwendung

NovaOS SOLL gültige Berechnungsergebnisse zwischen Szenenversionen wiederverwenden.

Dies umfasst insbesondere:

- Effektive Property-Werte
- Transformationsmatrizen
- Layout-Ergebnisse
- Geometrische Begrenzungen
- Sichtbarkeitsinformationen
- Abgeleitete Materialparameter

Cache-Einträge MÜSSEN anhand ihrer Zustandsversionen und Abhängigkeiten validierbar sein.

Eine Änderung DARF nur diejenigen Cache-Einträge invalidieren, deren Ergebnisse tatsächlich betroffen sein können.

## 9. Aktualisierungsreihenfolge

Abhängige Ergebnisse MÜSSEN in einer gültigen Reihenfolge aktualisiert werden.

NovaOS SOLL:

- Abhängigkeiten vor ihren Verbrauchern auswerten.
- Unabhängige Aktualisierungen parallel ermöglichen.
- Mehrfachberechnungen vermeiden.
- Zyklische Abhängigkeiten kontrolliert behandeln.
- Die Konsistenz veröffentlichter Szenenversionen erhalten.

Parallelisierung DARF keine unterschiedlichen logischen Ergebnisse verursachen.

## 10. Integration mit Damage Tracking

Scene Updates bestimmen die erforderlichen Änderungen an Szeneninformationen.

Damage Tracking bestimmt anschließend die betroffenen Bildbereiche.

Dabei MÜSSEN insbesondere berücksichtigt werden:

- Alte und neue geometrische Begrenzungen
- Transformationen
- Sichtbarkeitsänderungen
- Clipping
- Überlagerungen
- Material- und Effektabhängigkeiten

Scene Updates selbst erzeugen keine Pixel und führen keine Rendering-Operationen aus.

## 11. Adaptive Aktualisierung

NovaOS DARF zwischen inkrementeller und vollständiger Szenenauswertung wechseln.

Eine vollständige Aktualisierung kann sinnvoll sein bei:

- Umfangreichen Hierarchieänderungen
- Sehr großen Change Sets
- Hohem Verwaltungsaufwand inkrementeller Berechnungen
- Ungültigen oder unvollständigen Abhängigkeitsinformationen
- Erforderlicher Wiederherstellung eines konsistenten Zustands

Die Strategieauswahl erfolgt durch Nova Adaptive Graphics Processing.

Die Korrektheit MUSS unabhängig von der gewählten Strategie gewährleistet bleiben.

## 12. Fehlerbehandlung

Fehlgeschlagene Aktualisierungen DÜRFEN keine inkonsistenten Szenenergebnisse veröffentlichen.

NovaOS MUSS:

- Ungültige Cache-Einträge erkennen.
- Fehlende Abhängigkeiten kontrolliert behandeln.
- Veraltete Berechnungsergebnisse erkennen.
- Bei Bedarf eine konservative Neuberechnung durchführen.
- Den zuletzt gültigen veröffentlichten Szenenzustand erhalten.

Eine fehlgeschlagene Optimierung DARF die korrekte Darstellung nicht verhindern.

## 13. Verbindliche Anforderungen

1. Szenenaktualisierungen MÜSSEN auf konsistenten veröffentlichten Zuständen basieren.
2. Änderungen MÜSSEN über Scene Change Tracking berücksichtigt werden.
3. Abhängige Ergebnisse MÜSSEN über den Scene Dependency Graph ermittelt werden.
4. Unveränderte Scene Nodes SOLLEN nicht erneut verarbeitet werden.
5. Gültige Zwischenergebnisse SOLLEN wiederverwendet werden.
6. Invalidierte Ergebnisse DÜRFEN nicht als gültige Ergebnisse verwendet werden.
7. Aktualisierungen MÜSSEN in einer gültigen Abhängigkeitsreihenfolge erfolgen.
8. Unabhängige Aktualisierungen DÜRFEN parallel ausgeführt werden.
9. Mehrere kompatible Änderungen SOLLEN zusammengefasst werden.
10. Bei unvollständigen Abhängigkeiten MUSS eine konservative Aktualisierung möglich sein.
11. Aktualisierte Szeneninformationen MÜSSEN für Damage Tracking und Rendering verfügbar sein.
12. Die Aktualisierungsarchitektur MUSS unabhängig von konkreten Rendering-Backends funktionieren.

## 14. Ergebnis

Nova Incremental Scene Updates ermöglicht die gezielte und bedarfsgerechte Aktualisierung grafischer Szenenzustände.

Durch Change Tracking, Dependency Analysis, Lazy Evaluation und Cache-Wiederverwendung vermeidet NovaOS unnötige Berechnungen unveränderter Szenenbereiche.

Damit bildet das System eine wesentliche Grundlage für effizientes Partial Rendering, geringe CPU-Auslastung und eine reaktionsschnelle grafische Oberfläche.