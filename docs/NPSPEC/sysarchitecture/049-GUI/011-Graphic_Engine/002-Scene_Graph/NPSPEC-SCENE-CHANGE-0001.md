# NPSPEC-SCENE-CHANGE-0001 – Nova Scene Change Tracking

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Scene Graph

## 1. Zweck

Nova Scene Change Tracking definiert die Erkennung und Verwaltung struktureller und zustandsbezogener Änderungen innerhalb des Nova Scene Graph.

Ziel ist die gezielte Aktualisierung betroffener Scene Nodes und Teilstrukturen, ohne unveränderte Szenenbereiche erneut verarbeiten zu müssen.

## 2. Architekturprinzipien

- **State-Driven:** Änderungen werden anhand veröffentlichter Szenenzustände erkannt.
- **Incremental:** Nur betroffene Nodes und Teilstrukturen werden aktualisiert.
- **Transactional:** Änderungen werden atomar veröffentlicht.
- **Versioned:** Jede Änderung ist einer Szenenversion zugeordnet.
- **Hierarchical:** Änderungen berücksichtigen die Szenenhierarchie.
- **Dependency-Aware:** Auswirkungen werden über definierte Abhängigkeiten ermittelt.
- **Deterministic:** Identische Zustandsübergänge erzeugen identische logische Änderungsinformationen.
- **Renderer-Independent:** Die Änderungserkennung ist unabhängig vom Rendering-Backend.

## 3. Änderungsmodell

Jede relevante Szenenänderung wird als Scene Change Record erfasst.

Ein Change Record enthält mindestens:

- Scene ID
- Node ID
- Änderungstyp
- Betroffene Eigenschaften oder Beziehungen
- Ausgangs- und Zielversion
- Änderungsrelevante Informationen

Mehrere zusammengehörige Änderungen werden in einem Scene Change Set zusammengefasst.

## 4. Änderungstypen

NovaOS unterscheidet insbesondere:

- **Node Created:** Neuer Scene Node.
- **Node Removed:** Entfernter Scene Node.
- **Node Reparented:** Änderung der Elternbeziehung.
- **Children Reordered:** Änderung der Kindreihenfolge.
- **Property Changed:** Änderung einer Node-Eigenschaft.
- **Geometry Changed:** Änderung geometrischer Eigenschaften.
- **Transform Changed:** Änderung einer Transformation.
- **Visibility Changed:** Änderung der Sichtbarkeit.
- **Content Changed:** Änderung grafischer Inhalte.
- **Resource Changed:** Änderung referenzierter Ressourcen.

Weitere Änderungstypen DÜRFEN ergänzt werden.

## 5. Änderungserkennung

Scene Change Tracking verarbeitet die durch Graphics State Transactions veröffentlichten Änderungen.

Das System MUSS:

- Hinzugefügte und entfernte Nodes erkennen.
- Hierarchieänderungen erfassen.
- Relevante Property-Änderungen berücksichtigen.
- Tatsächliche Zustandsänderungen von redundanten Schreiboperationen unterscheiden.
- Änderungen eindeutig einer Szenenversion zuordnen.
- Konsistente Change Sets bereitstellen.

Fehlgeschlagene Transaktionen DÜRFEN keine veröffentlichten Scene Change Sets erzeugen.

## 6. Hierarchische Änderungsverfolgung

Änderungen können einzelne Nodes oder vollständige Teilstrukturen betreffen.

NovaOS MUSS zwischen folgenden Auswirkungen unterscheiden können:

- **Local Change:** Betrifft ausschließlich den Node.
- **Subtree Change:** Betrifft den Node und abhängige Nachfahren.
- **Ancestor Change:** Erfordert die Aktualisierung übergeordneter Ergebnisse.
- **Structural Change:** Verändert die Szenenhierarchie.

Die tatsächliche Ausbreitung erfolgt über das Nova Scene Dependency Graph und Graphics Dependency Tracking.

Eine Änderung eines Parent Nodes DARF nicht grundsätzlich die vollständige Neuberechnung aller Child Nodes erzwingen.

## 7. Scene Change Sets

Ein Scene Change Set repräsentiert die Änderungen zwischen zwei veröffentlichten Szenenversionen.

Change Sets MÜSSEN:

- Eindeutig versioniert sein.
- Konsistente Änderungen enthalten.
- Strukturelle Änderungen korrekt abbilden.
- Redundante Änderungen zusammenfassen können.
- Für nachgelagerte Verarbeitung verfügbar sein.

Mehrfache Änderungen derselben Eigenschaft SOLLEN anhand des effektiven Ausgangs- und Zielzustands zusammengefasst werden.

## 8. Inkrementelle Verarbeitung

NovaOS SOLL Änderungsinformationen verwenden, um:

- Unveränderte Scene Nodes wiederzuverwenden.
- Betroffene Teilstrukturen gezielt auszuwerten.
- Geometrie- und Transformations-Caches selektiv zu invalidieren.
- Layout-Neuberechnungen zu begrenzen.
- Rendering-relevante Änderungen weiterzugeben.
- Unnötige vollständige Szenendurchläufe zu vermeiden.

Bei unvollständigen Änderungsinformationen MUSS eine konservative Aktualisierung möglich sein.

## 9. Integration in die Grafikarchitektur

Die Verantwortlichkeiten sind getrennt:

- **Graphics State Change Tracking:** Erfasst allgemeine grafische Zustandsänderungen.
- **Scene Change Tracking:** Erfasst und klassifiziert Änderungen innerhalb der Szenenstruktur.
- **Scene Dependency Graph:** Ermittelt betroffene Scene Nodes und Teilstrukturen.
- **Graphics Dependency Tracking:** Verarbeitet übergreifende grafische Abhängigkeiten.
- **Damage Tracking:** Bestimmt betroffene Bildbereiche.
- **Render Graph:** Organisiert erforderliche Rendering-Operationen.

Scene Change Tracking führt selbst keine Rendering-Operationen aus.

## 10. Versionsverwaltung und Lebensdauer

Scene Change Sets sind an veröffentlichte Szenenversionen gebunden.

NovaOS MUSS sicherstellen, dass:

- Änderungen in korrekter Versionsreihenfolge verarbeitet werden.
- Veraltete Change Sets erkannt werden.
- Aktive Scene Snapshots gültig bleiben.
- Entfernte Nodes während aktiver Referenzen erhalten bleiben.
- Nicht mehr benötigte Änderungsinformationen freigegeben werden können.

Übersprungene Zwischenversionen DÜRFEN zusammengefasst werden, sofern sämtliche relevanten Auswirkungen erhalten bleiben.

## 11. Verbindliche Anforderungen

1. Strukturelle und zustandsbezogene Szenenänderungen MÜSSEN erfasst werden.
2. Änderungen MÜSSEN eindeutig Scene Nodes und Szenenversionen zugeordnet sein.
3. Scene Change Sets MÜSSEN transaktionskonsistent sein.
4. Hierarchieänderungen MÜSSEN korrekt berücksichtigt werden.
5. Redundante Änderungen SOLLEN zusammengefasst werden.
6. Unveränderte Teilstrukturen SOLLEN wiederverwendet werden.
7. Änderungen MÜSSEN für das Scene Dependency Graph verfügbar sein.
8. Rendering-relevante Änderungen MÜSSEN an die zuständigen Grafikkomponenten weitergegeben werden.
9. Fehlgeschlagene Transaktionen DÜRFEN keine veröffentlichten Änderungen erzeugen.
10. Bei unvollständiger Änderungserkennung MUSS eine korrekte konservative Aktualisierung erfolgen können.
11. Die Änderungserkennung MUSS unabhängig von konkreten Rendering-Backends funktionieren.

## 12. Ergebnis

Nova Scene Change Tracking ermöglicht die präzise Erfassung struktureller und zustandsbezogener Änderungen innerhalb des Nova Scene Graph.

Durch versionsbasierte Change Sets, hierarchische Änderungsanalyse und gezielte Invalidierung können unveränderte Szenenbereiche erhalten bleiben.

Damit bildet das System die Grundlage für Nova Incremental Scene Updates und eine effiziente, zustandsbasierte Grafikverarbeitung.