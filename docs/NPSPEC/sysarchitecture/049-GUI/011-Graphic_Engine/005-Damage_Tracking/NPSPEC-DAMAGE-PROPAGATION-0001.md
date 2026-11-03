# NPSPEC-DAMAGE-PROPAGATION-0001 – Nova Damage Propagation

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Damage Tracking

## 1. Zweck

Nova Damage Propagation definiert die kontrollierte Weitergabe grafischer Änderungen zwischen Scene Nodes, Surfaces, Rendering-Ressourcen und abhängigen Grafikeffekten.

Ziel ist die vollständige Erfassung aller durch eine Änderung betroffenen Bildbereiche, ohne unabhängige Inhalte unnötig zu invalidieren.

Die Weitergabe erfolgt inkrementell, abhängigkeitsbasiert und versionssicher.

## 2. Architekturprinzipien

- **Change-Driven:** Nur relevante Änderungen lösen eine Weitergabe aus.
- **Dependency-Aware:** Damage folgt tatsächlichen grafischen Abhängigkeiten.
- **Region-Based:** Betroffene Bereiche werden räumlich begrenzt.
- **Hierarchical:** Änderungen werden innerhalb der Scene-Hierarchie weitergegeben.
- **Material-Aware:** Effekte und Backdrop-Abhängigkeiten werden berücksichtigt.
- **Incremental:** Unveränderte Abhängigkeiten bleiben erhalten.
- **Cycle-Safe:** Mehrfache oder zyklische Abhängigkeiten werden kontrolliert behandelt.
- **Versioned:** Jede Weitergabe ist einem konsistenten Grafikzustand zugeordnet.

## 3. Propagation Model

Eine Damage-Weitergabe beschreibt die Übertragung einer Änderung von einer Quelle zu einem abhängigen Ziel.

Sie enthält mindestens:

- **Source ID:** Verursachende Ressource oder Scene Node.
- **Target ID:** Abhängiges Ziel.
- **Damage Region:** Betroffener Bereich.
- **Coordinate Transform:** Transformation zwischen Koordinatensystemen.
- **Dependency Type:** Art der grafischen Abhängigkeit.
- **State Version:** Zugehörige Zustandsversion.

Mehrere Änderungen DÜRFEN zu einer gemeinsamen Weitergabe zusammengefasst werden.

## 4. Propagation Types

NovaOS MUSS folgende Arten unterstützen können:

| Typ | Beschreibung |
|---|---|
| Hierarchical | Weitergabe innerhalb der Scene-Graph-Hierarchie |
| Spatial | Weitergabe zwischen räumlich betroffenen Bereichen |
| Resource | Weitergabe über gemeinsam verwendete Rendering-Ressourcen |
| Material | Weitergabe durch Material- und Effektabhängigkeiten |
| Backdrop | Weitergabe bei Änderungen hinter transparenten Oberflächen |
| Surface | Weitergabe zwischen abhängigen Surfaces |
| Composition | Weitergabe an betroffene Composition Layers und Ausgaben |

Die Weitergabe MUSS auf tatsächlichen Abhängigkeiten basieren.

## 5. Propagation Process

Die Verarbeitung erfolgt grundsätzlich in folgenden Schritten:

1. Grafische Änderung erkennen.
2. Ursprüngliche Damage Region bestimmen.
3. Direkte Abhängigkeiten ermitteln.
4. Damage in das Zielkoordinatensystem übertragen.
5. Erforderliche Effektbereiche berücksichtigen.
6. Damage mit bereits vorhandenen Regionen vereinigen.
7. Weitere betroffene Abhängigkeiten verarbeiten.
8. Ergebnis an Render Graph und Compositor übergeben.

Die Verarbeitung endet, wenn keine zusätzlichen relevanten Damage Regions entstehen.

## 6. Hierarchical Propagation

Innerhalb des Scene Graph wird Damage entlang betroffener Hierarchiepfade weitergegeben.

Dabei MÜSSEN berücksichtigt werden:

- Parent-Child-Beziehungen
- Transformationen
- Clipping
- Sichtbarkeit
- Opacity
- Strukturänderungen

Unabhängige Teilbäume SOLLEN nicht invalidiert werden.

Die hierarchische Weitergabe DARF externe grafische Abhängigkeiten nicht ignorieren.

## 7. Material und Backdrop Propagation

Grafikeffekte können Änderungen außerhalb ihrer eigenen Geometrie benötigen.

NovaOS MUSS insbesondere berücksichtigen:

- Blur und Filterradien
- Refraction und Sampling-Verschiebungen
- Reflection und abhängige Bildbereiche
- Schatten und Effektbegrenzungen
- Liquid Glass und darunterliegende Inhalte

Damage MUSS anhand der tatsächlichen Sampling-Abhängigkeiten auf betroffene Ausgabegebiete übertragen werden.

Eine Änderung hinter einem Liquid-Glass-Element MUSS dessen Darstellung invalidieren, sofern sie das sichtbare Effektergebnis beeinflussen kann.

## 8. Spatial Propagation

Damage Regions MÜSSEN zwischen unterschiedlichen Koordinatensystemen übertragen werden können.

Dabei gilt:

- Transformationen müssen korrekt angewendet werden.
- Geometrische Rundungen müssen konservativ erfolgen.
- Erforderliche Effektbereiche müssen erhalten bleiben.
- Zielregionen dürfen auf gültige Ausgabebereiche begrenzt werden.

Bei nicht zuverlässig bestimmbaren Auswirkungen MUSS eine konservative Erweiterung erfolgen.

## 9. Dependency Propagation

Nova Damage Dependency Analysis stellt die relevanten Abhängigkeitsbeziehungen bereit.

Nova Damage Propagation verwendet diese zur tatsächlichen Weitergabe der Änderungen.

Dabei gilt:

- Nur betroffene Abhängigkeiten SOLLEN verarbeitet werden.
- Mehrfach erreichte Ziele SOLLEN zusammengefasst werden.
- Weitergaben MÜSSEN bis zu einem stabilen Ergebnis verarbeitet werden.
- Unzulässige Abhängigkeitszyklen MÜSSEN erkannt werden.

Bei nicht sicher auflösbaren Zyklen MUSS eine konservative Invalidierung erfolgen.

## 10. Occlusion Handling

Vollständig verdeckte Änderungen DÜRFEN für die aktuelle sichtbare Ausgabe zurückgestellt werden.

Dabei MUSS berücksichtigt werden, dass verdeckte Inhalte weiterhin von transparenten Materialien oder anderen Effekten benötigt werden können.

Die Zurückstellung DARF weder erforderliche Cache-Invalidierungen noch spätere Aktualisierungen verhindern.

## 11. Incremental Propagation

NovaOS SOLL unveränderte Abhängigkeitsstrukturen wiederverwenden.

Neue Änderungen werden ausschließlich durch betroffene Bereiche des Dependency Graph weitergegeben.

Bereits verarbeitete Kombinationen aus Ziel, Zustandsversion und Damage Region SOLLEN erkannt werden, um redundante Verarbeitung zu vermeiden.

Eine erneute Weitergabe MUSS erfolgen, wenn zusätzliche betroffene Bereiche hinzukommen.

## 12. Versioning und Synchronization

Die Damage-Weitergabe MUSS auf einem konsistenten Grafikzustand basieren.

Dabei gilt:

- Änderungen müssen einer eindeutigen Zustandsversion zugeordnet sein.
- Laufende Propagation darf neue Änderungen nicht verlieren.
- Ältere Verarbeitungsergebnisse dürfen neueren Damage nicht überschreiben.
- Damage darf erst nach erfolgreicher Aktualisierung bestätigt werden.

Die Weitergabe DARF parallel erfolgen, sofern die Ergebnisse korrekt zusammengeführt werden.

## 13. Performance und Fallback

NovaOS SOLL den Aufwand der Damage-Weitergabe minimieren.

Dazu gehören:

- Zusammenfassung mehrfacher Änderungen
- Begrenzung auf betroffene Abhängigkeiten
- Wiederverwendung gültiger Transformationen
- Vermeidung redundanter Weitergaben
- Adaptive Zusammenfassung von Damage Regions

Wenn eine präzise Weitergabe nicht möglich ist, MUSS eine konservative Invalidierung der betroffenen Surface oder Ausgabe erfolgen können.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS Damage entlang tatsächlicher grafischer Abhängigkeiten weitergeben.
2. Hierarchische und nicht hierarchische Abhängigkeiten MÜSSEN unterstützt werden.
3. Damage Regions MÜSSEN zwischen Koordinatensystemen transformiert werden können.
4. Material- und Backdrop-Abhängigkeiten MÜSSEN berücksichtigt werden.
5. Änderungen hinter transparenten Effekten MÜSSEN bei relevanten Abhängigkeiten weitergegeben werden.
6. Mehrfach erreichte Damage Regions SOLLEN zusammengefasst werden.
7. Unabhängige Inhalte SOLLEN nicht unnötig invalidiert werden.
8. Die Weitergabe MUSS bis zur vollständigen Erfassung relevanter Auswirkungen erfolgen.
9. Zyklische Abhängigkeiten MÜSSEN sicher behandelt werden.
10. Verdeckte Änderungen DÜRFEN nur bei gewährleisteter Korrektheit zurückgestellt werden.
11. Unveränderte Abhängigkeitsstrukturen SOLLEN wiederverwendet werden.
12. Die Verarbeitung MUSS versionssicher erfolgen.
13. Neuere Änderungen DÜRFEN nicht durch ältere Verarbeitungsergebnisse verloren gehen.
14. Bei unvollständigen Abhängigkeitsinformationen MUSS eine konservative Invalidierung erfolgen.
15. Im deterministischen Modus MÜSSEN identische Eingaben reproduzierbare Damage-Ergebnisse erzeugen.

## 15. Ergebnis

Nova Damage Propagation gewährleistet die vollständige und kontrollierte Weitergabe grafischer Änderungen innerhalb der NovaOS-Grafikarchitektur.

Durch hierarchische, räumliche und abhängigkeitsbasierte Verarbeitung werden sämtliche betroffenen Rendering-Bereiche erkannt, während unabhängige Inhalte unverändert bleiben.

Damit bildet das Subsystem die Grundlage für korrektes Partial Rendering, Incremental Composition und eine bedarfsgesteuerte Frame-Erzeugung.