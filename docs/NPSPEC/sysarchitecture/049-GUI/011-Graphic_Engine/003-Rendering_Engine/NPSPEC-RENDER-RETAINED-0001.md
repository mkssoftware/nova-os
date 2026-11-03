# NPSPEC-RENDER-RETAINED-0001 – Nova Retained Rendering

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Render Engine

## 1. Zweck

Nova Retained Rendering definiert die zustandsbasierte Darstellung grafischer Inhalte, deren Beschreibung und berechnete Ergebnisse über mehrere Rendering-Zyklen erhalten bleiben.

Anwendungen beschreiben den gewünschten grafischen Zustand, anstatt sämtliche Zeichenoperationen bei jeder Aktualisierung erneut auszuführen.

Ziel ist die Vermeidung redundanter Berechnungen durch persistente Szenenzustände, gezielte Änderungsverarbeitung und Wiederverwendung gültiger Darstellungsergebnisse.

## 2. Architekturprinzipien

- **Retained:** Grafische Beschreibungen bleiben zwischen Frames erhalten.
- **State-Driven:** Rendering basiert auf veröffentlichten grafischen Zuständen.
- **Declarative:** Anwendungen beschreiben das gewünschte Darstellungsergebnis.
- **Incremental:** Nur betroffene Inhalte werden aktualisiert.
- **Dependency-Aware:** Grafische Abhängigkeiten bestimmen erforderliche Neuberechnungen.
- **Cache-Aware:** Gültige Rendering-Ergebnisse werden wiederverwendet.
- **Event-Driven:** Rendering erfolgt bei tatsächlichem Aktualisierungsbedarf.
- **Deterministic:** Identische Zustände erzeugen identische logische Darstellungsergebnisse.
- **Renderer-Independent:** Die grafische Beschreibung bleibt unabhängig vom Rendering-Backend.

## 3. Retained Rendering Model

Retained Rendering basiert auf dem Nova Scene Graph.

Dieser verwaltet:

- Persistente Scene Nodes
- Grafische Eigenschaften
- Hierarchische Beziehungen
- Transformationen
- Materialien und Effekte
- Ressourcenreferenzen
- Veröffentlichte Zustandsversionen

Die Render Engine interpretiert diese Informationen und erzeugt daraus erforderliche Rendering-Operationen.

Die Lebensdauer eines Scene Nodes ist unabhängig von der Anzahl erzeugter Frames.

## 4. Zustandsbasierte Darstellung

Anwendungen verändern den grafischen Zustand über definierte Schnittstellen.

Die Verarbeitung erfolgt grundsätzlich durch:

1. Änderung grafischer Eigenschaften.
2. Atomare Veröffentlichung des neuen Zustands.
3. Ermittlung tatsächlicher Zustandsänderungen.
4. Auswertung grafischer Abhängigkeiten.
5. Bestimmung erforderlicher Rendering-Operationen.
6. Wiederverwendung gültiger Ergebnisse.
7. Aktualisierung betroffener Inhalte.

Unveränderte grafische Zustände SOLLEN keine erneute Rendering-Ausführung auslösen.

## 5. Persistente Szeneninformationen

NovaOS MUSS grafische Beschreibungen unabhängig von einzelnen Rendering-Zyklen erhalten können.

Dazu gehören insbesondere:

- Scene-Node-Strukturen
- Effektive Eigenschaften
- Geometrische Informationen
- Transformationen
- Sichtbarkeitsinformationen
- Materialzuordnungen
- Ressourcenreferenzen

Persistente Szeneninformationen MÜSSEN von temporären Rendering-Ressourcen unterschieden werden.

Der Verlust eines Rendering-Caches DARF nicht den Verlust der grafischen Szenenbeschreibung verursachen.

## 6. Änderungsverarbeitung

Retained Rendering verwendet:

- Graphics State Change Tracking
- Scene Change Tracking
- Scene Dependency Graph
- Graphics Dependency Tracking
- Nova Damage Tracking

Diese Komponenten bestimmen, welche Inhalte erneut verarbeitet werden müssen.

Eine Änderung eines Scene Nodes DARF nicht grundsätzlich die vollständige Neuberechnung der Szene erzwingen.

## 7. Rendering-Ergebnis-Wiederverwendung

NovaOS SOLL gültige Rendering-Ergebnisse zwischen Frames wiederverwenden.

Dies kann umfassen:

- Gerenderte Teilbereiche
- Textdarstellungen
- Geometrieergebnisse
- Materialeffekte
- Vorberechnete Schatten
- Blur- und Liquid-Glass-Ergebnisse

Die Wiederverwendung MUSS anhand relevanter Zustände und Abhängigkeiten validiert werden.

Retained Rendering erfordert nicht, dass jedes grafische Element dauerhaft als separate Textur gespeichert wird.

## 8. Integration mit Incremental Rendering

Retained Rendering stellt persistente grafische Beschreibungen bereit.

Nova Incremental Rendering bestimmt, welche Verarbeitungsschritte aufgrund von Änderungen erneut ausgeführt werden müssen.

Nova Partial Rendering begrenzt erforderliche Zeichenoperationen auf betroffene Bildbereiche.

Diese Verfahren SOLLEN gemeinsam verwendet werden, bleiben jedoch logisch getrennt.

## 9. Render Graph Integration

Der Nova Render Graph verarbeitet die erforderlichen Rendering-Aufträge.

Dabei SOLL NovaOS:

- Unveränderte Render Passes vermeiden.
- Gültige Pass-Ergebnisse wiederverwenden.
- Betroffene Rendering-Abhängigkeiten aktualisieren.
- Ressourcen effizient wiederverwenden.
- Unnötige GPU-Befehle vermeiden.

Die persistente Szenenbeschreibung DARF nicht von einem bestimmten Render Graph oder GPU-Backend abhängig sein.

## 10. Animationen und dynamische Inhalte

Retained Rendering MUSS Animationen und zeitabhängige Inhalte unterstützen.

Dynamische Eigenschaften können durch:

- Animationen
- Ereignisse
- Datenbindungen
- Interaktionen
- Externe Zustandsänderungen

aktualisiert werden.

Animationen SOLLEN ausschließlich die tatsächlich betroffenen Eigenschaften und Darstellungsergebnisse invalidieren.

Zeitabhängige Inhalte DÜRFEN neue Frames auslösen, auch wenn keine direkte Benutzerinteraktion erfolgt.

## 11. Ressourcenverwaltung

Retained Rendering MUSS mit der zentralen Grafikressourcenverwaltung zusammenarbeiten.

Dabei gilt:

- Persistente Ressourcenreferenzen MÜSSEN gültig bleiben.
- Nicht mehr benötigte Ressourcen MÜSSEN freigegeben werden können.
- Temporäre Rendering-Ressourcen DÜRFEN unabhängig vom Szenenzustand verworfen werden.
- Gemeinsame Ressourcen SOLLEN ohne unnötige Duplikation verwendet werden.
- Aktive Rendering-Aufträge DÜRFEN nicht durch vorzeitige Ressourcenfreigaben beeinträchtigt werden.

## 12. Integration mit Nova UI

Retained Rendering ist das bevorzugte Darstellungsmodell der nativen Nova UI Engine.

UI-Elemente werden durch deklarative Szeneninformationen repräsentiert.

Änderungen an:

- Styles
- Templates
- Eigenschaften
- Layouts
- Datenbindungen
- Interaktionszuständen

führen zu gezielten Aktualisierungen der entsprechenden Szenenelemente.

Die UI Engine bleibt für UI-Logik und Layout verantwortlich.

Die Render Engine übernimmt ausschließlich die grafische Darstellung.

## 13. Immediate Rendering Kompatibilität

NovaOS MUSS weiterhin Immediate Rendering unterstützen.

Anwendungen mit eigenen Rendering-Engines können ihre Inhalte über definierte Rendering- und Surface-Schnittstellen bereitstellen.

Retained und Immediate Rendering DÜRFEN innerhalb derselben grafischen Umgebung kombiniert werden.

Eine Anwendung DARF nicht zur Verwendung des Retained Rendering Model gezwungen werden.

## 14. Fehlerbehandlung

Bei ungültigen Szenenzuständen, fehlenden Ressourcen oder beschädigten Rendering-Caches MUSS NovaOS eine kontrollierte Wiederherstellung ermöglichen.

Dabei MUSS:

- Der zuletzt gültige Szenenzustand erhalten bleiben.
- Ungültiges Caching verhindert werden.
- Eine vollständige Neuberechnung möglich sein.
- Die Veröffentlichung unvollständiger Rendering-Ergebnisse verhindert werden.

## 15. Verbindliche Anforderungen

1. NovaOS MUSS Retained Rendering unterstützen.
2. Grafische Szenenbeschreibungen MÜSSEN zwischen Rendering-Zyklen erhalten bleiben.
3. Retained Rendering MUSS auf konsistenten veröffentlichten Szenenzuständen basieren.
4. Zustandsänderungen MÜSSEN über Change Tracking erfasst werden.
5. Grafische Abhängigkeiten MÜSSEN bei Aktualisierungen berücksichtigt werden.
6. Unveränderte Szenenelemente SOLLEN nicht unnötig neu verarbeitet werden.
7. Gültige Rendering-Ergebnisse SOLLEN wiederverwendet werden.
8. Rendering-Caches MÜSSEN unabhängig vom persistenten Szenenzustand verwaltet werden können.
9. Retained Rendering MUSS mit Incremental und Partial Rendering kombinierbar sein.
10. Animationen und zeitabhängige Inhalte MÜSSEN unterstützt werden.
11. Immediate Rendering MUSS weiterhin möglich bleiben.
12. Die Architektur MUSS unabhängig von konkreten Rendering-Backends funktionieren.
13. Bei ungültigen Zwischenergebnissen MUSS eine vollständige Neuberechnung möglich sein.
14. Die visuelle Korrektheit MUSS unabhängig von verwendeten Optimierungen erhalten bleiben.

## 16. Ergebnis

Nova Retained Rendering ermöglicht eine persistente und zustandsbasierte Verwaltung grafischer Darstellungen.

Anwendungen beschreiben den gewünschten Zustand, während NovaOS erforderliche Rendering-Operationen selbstständig ermittelt.

In Verbindung mit Incremental Rendering, Partial Rendering und dem Nova Render Graph werden unveränderte Inhalte wiederverwendet und unnötige Berechnungen vermieden.

Damit bildet Retained Rendering das bevorzugte Darstellungsmodell der nativen NovaOS-Oberfläche und eine zentrale Grundlage für effiziente, deklarative Grafikverarbeitung.