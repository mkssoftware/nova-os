# NPSPEC-SCENE-DEPENDENCY-0001 – Nova Scene Dependency Graph

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Scene Graph

## 1. Zweck

Nova Scene Dependency Graph definiert die Verwaltung und Auswertung von Abhängigkeiten zwischen Scene Nodes, Eigenschaften und abgeleiteten Szenenergebnissen.

Ziel ist die präzise Bestimmung betroffener Szenenelemente nach Zustandsänderungen, ohne unveränderte Teilstrukturen unnötig neu berechnen zu müssen.

## 2. Architekturprinzipien

- **Dependency-Driven:** Aktualisierungen erfolgen anhand tatsächlicher Abhängigkeiten.
- **Incremental:** Nur betroffene Ergebnisse werden invalidiert.
- **State-Based:** Abhängigkeiten beziehen sich auf definierte Szenenzustände.
- **Transactional:** Änderungen am Abhängigkeitsmodell werden konsistent veröffentlicht.
- **Hierarchical:** Beziehungen innerhalb der Szenenhierarchie werden berücksichtigt.
- **Deterministic:** Identische Zustände erzeugen identische logische Abhängigkeitsergebnisse.
- **Renderer-Independent:** Die Abhängigkeitsverwaltung ist unabhängig vom Rendering-Backend.

## 3. Abhängigkeitsmodell

Der Scene Dependency Graph besteht aus gerichteten Beziehungen zwischen grafischen Zuständen und abgeleiteten Ergebnissen.

Eine Abhängigkeit enthält mindestens:

- **Source:** Ausgangszustand oder Quellenelement.
- **Target:** Abhängiges Element oder Ergebnis.
- **Dependency Type:** Art der Abhängigkeit.
- **Affected Properties:** Relevante Eigenschaften.
- **State Version:** Zugehörige Szenenversion.

Eine gerichtete Beziehung bedeutet, dass Änderungen der Quelle eine Aktualisierung des Ziels erforderlich machen können.

## 4. Abhängigkeitstypen

NovaOS unterstützt insbesondere:

- **Hierarchy Dependency:** Eltern-Kind-Beziehungen.
- **Property Dependency:** Abhängigkeiten zwischen Eigenschaften.
- **Layout Dependency:** Auswirkungen auf Layoutberechnungen.
- **Transform Dependency:** Abhängigkeiten von Transformationen.
- **Visibility Dependency:** Auswirkungen auf Sichtbarkeit.
- **Geometry Dependency:** Abhängigkeiten geometrischer Ergebnisse.
- **Material Dependency:** Auswirkungen auf Materialien und Effekte.
- **Resource Dependency:** Abhängigkeiten von Grafikressourcen.
- **Content Dependency:** Auswirkungen durch Inhaltsänderungen.

Weitere Abhängigkeitstypen DÜRFEN ergänzt werden.

## 5. Hierarchische Abhängigkeiten

Die Szenenhierarchie bildet die Grundlage struktureller Abhängigkeiten.

Änderungen eines Parent Nodes können untergeordnete Nodes beeinflussen.

Dabei MUSS NovaOS unterscheiden zwischen:

- Lokalen Änderungen ohne Auswirkungen auf Nachfahren.
- Änderungen mit Auswirkungen auf abhängige Nachfahren.
- Änderungen, die übergeordnete Ergebnisse beeinflussen.
- Änderungen der Hierarchiestruktur.

Eine Änderung des Parent Nodes DARF nicht grundsätzlich zur vollständigen Invalidierung aller Nachfahren führen.

## 6. Dynamische Abhängigkeiten

Abhängigkeiten DÜRFEN während der Laufzeit entstehen, verändert oder entfernt werden.

Dies betrifft insbesondere:

- Property Bindings
- Layoutbeziehungen
- Ressourcenwechsel
- Material- und Effektänderungen
- Dynamische Szenenstrukturen

Änderungen des Dependency Graph MÜSSEN mit den zugehörigen Szenenzuständen konsistent bleiben.

Ungültige oder veraltete Abhängigkeiten MÜSSEN erkannt werden.

## 7. Dependency Propagation

Nach einer Szenenänderung ermittelt NovaOS die unmittelbar und mittelbar betroffenen Ergebnisse.

Die Verarbeitung erfolgt durch:

1. Übernahme des Scene Change Sets.
2. Identifikation geänderter Quellen.
3. Ermittlung direkter Abhängigkeiten.
4. Auswertung transitiver Abhängigkeiten.
5. Zusammenfassung betroffener Ergebnisse.
6. Weitergabe erforderlicher Invalidierungen.

Bereits ausgewertete Abhängigkeiten SOLLEN innerhalb eines Verarbeitungsvorgangs nicht mehrfach verarbeitet werden.

## 8. Zyklen und Konsistenz

Zyklische Abhängigkeiten können bei dynamischen Property Bindings oder berechneten Eigenschaften entstehen.

NovaOS MUSS:

- Zyklen erkennen.
- Nicht auflösbare Abhängigkeitszyklen verhindern.
- Ungültige Abhängigkeitsänderungen kontrolliert ablehnen.
- Den zuletzt gültigen Zustand erhalten.

Die Verarbeitung DARF keine unendlichen Aktualisierungsschleifen erzeugen.

## 9. Inkrementelle Invalidierung

Der Scene Dependency Graph bestimmt, welche abgeleiteten Szenenergebnisse ihre Gültigkeit verlieren.

Dazu gehören insbesondere:

- Effektive Property-Werte
- Layout-Ergebnisse
- Transformationen
- Geometrische Begrenzungen
- Sichtbarkeitsinformationen
- Materialabhängige Ergebnisse
- Szenen-Caches

Eine Invalidierung bedeutet nicht zwangsläufig, dass unmittelbar eine Neuberechnung erfolgen muss.

Die tatsächliche Aktualisierung erfolgt bedarfsgerecht durch die zuständigen Komponenten.

## 10. Integration in die Grafikarchitektur

Die Verantwortlichkeiten sind getrennt:

- **Scene Change Tracking:** Erkennt Änderungen innerhalb des Scene Graph.
- **Scene Dependency Graph:** Bestimmt betroffene Szenenelemente und abgeleitete Ergebnisse.
- **Graphics Dependency Tracking:** Verwaltet übergreifende grafische Abhängigkeiten.
- **Damage Tracking:** Bestimmt betroffene Bildbereiche.
- **Render Graph:** Organisiert erforderliche Rendering-Operationen.
- **Render Engine:** Führt Rendering-Operationen aus.

Der Scene Dependency Graph DARF keine Rendering-Befehle erzeugen oder ausführen.

Szenenübergreifende Abhängigkeiten werden an Graphics Dependency Tracking übergeben.

## 11. Versionsverwaltung und Optimierung

Abhängigkeitsinformationen MÜSSEN mit den veröffentlichten Szenenversionen konsistent sein.

NovaOS SOLL:

- Unveränderte Abhängigkeitsstrukturen wiederverwenden.
- Redundante Abhängigkeiten vermeiden.
- Mehrfache Invalidierungen zusammenfassen.
- Unveränderte Ergebnisse erhalten.
- Nicht mehr benötigte Abhängigkeiten freigeben.
- Vollständige Graphdurchläufe vermeiden.

Aktive Scene Snapshots MÜSSEN weiterhin mit gültigen Abhängigkeitsinformationen verarbeitet werden können.

## 12. Fehlerbehandlung

Bei unvollständigen oder ungültigen Abhängigkeitsinformationen MUSS NovaOS eine konservative Aktualisierung der betroffenen Szenenbereiche ermöglichen.

Fehlerhafte Abhängigkeiten DÜRFEN:

- Keine inkonsistenten Szenenzustände erzeugen.
- Keine erforderlichen Aktualisierungen dauerhaft unterdrücken.
- Keine unkontrollierten Aktualisierungsschleifen verursachen.

## 13. Verbindliche Anforderungen

1. Szenenabhängigkeiten MÜSSEN eindeutig modelliert werden.
2. Abhängigkeiten MÜSSEN versionskonsistent verwaltet werden.
3. Direkte und transitive Abhängigkeiten MÜSSEN ausgewertet werden können.
4. Hierarchische Beziehungen MÜSSEN berücksichtigt werden.
5. Dynamische Abhängigkeiten MÜSSEN unterstützt werden.
6. Nicht auflösbare Abhängigkeitszyklen MÜSSEN erkannt und verhindert werden.
7. Unveränderte Ergebnisse SOLLEN nicht unnötig invalidiert werden.
8. Mehrfache Invalidierungen SOLLEN zusammengefasst werden.
9. Betroffene Ergebnisse MÜSSEN für nachgelagerte Grafikkomponenten verfügbar sein.
10. Bei unvollständigen Abhängigkeiten MUSS eine korrekte konservative Aktualisierung möglich sein.
11. Der Dependency Graph MUSS unabhängig von konkreten Rendering-Backends funktionieren.

## 14. Ergebnis

Nova Scene Dependency Graph ermöglicht eine präzise, versionsbasierte Verwaltung grafischer Abhängigkeiten innerhalb des Nova Scene Graph.

Durch gezielte Dependency Propagation und inkrementelle Invalidierung werden ausschließlich tatsächlich betroffene Szenenergebnisse zur Aktualisierung vorgesehen.

Damit bildet das System die Grundlage für Nova Incremental Scene Updates, effiziente Cache-Nutzung und eine ressourcenschonende Grafikverarbeitung.