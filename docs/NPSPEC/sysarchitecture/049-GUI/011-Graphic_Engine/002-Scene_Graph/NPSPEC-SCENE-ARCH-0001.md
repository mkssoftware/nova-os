# NPSPEC-SCENE-ARCH-0001 – Nova Scene Graph Architecture

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Scene Graph

## 1. Zweck

Die Nova Scene Graph Architecture definiert die strukturierte und persistente Verwaltung grafischer Szenen innerhalb von NovaOS.

Der Scene Graph bildet den aktuellen grafischen Zustand durch hierarchisch organisierte Elemente und deren Eigenschaften ab.

Ziel ist eine konsistente, deterministische und ressourcenschonende Grundlage für deklarative Grafikverarbeitung, Änderungsverfolgung und inkrementelles Rendering.

## 2. Architekturprinzipien

- **Retained:** Grafische Szenen bleiben zwischen Rendering-Zyklen erhalten.
- **State-Driven:** Die Szenenstruktur repräsentiert den aktuellen grafischen Zustand.
- **Hierarchical:** Elemente werden in einer definierten Hierarchie organisiert.
- **Declarative:** Elemente beschreiben ihre Eigenschaften statt konkreter Zeichenoperationen.
- **Transactional:** Zustandsänderungen werden atomar übernommen.
- **Incremental:** Nur betroffene Szenenbereiche werden aktualisiert.
- **Dependency-Aware:** Beziehungen und Abhängigkeiten werden berücksichtigt.
- **Renderer-Independent:** Die Szenenstruktur ist unabhängig von konkreten Rendering-Backends.
- **Deterministic:** Identische Zustände und Eingaben erzeugen identische logische Szenenergebnisse.

## 3. Szenenmodell

Eine Szene besteht aus einer hierarchischen Struktur grafischer Elemente.

Jede Szene besitzt:

- Eindeutige Szenenidentität
- Definiertes Wurzelelement
- Verwaltete grafische Elemente
- Veröffentlichte Zustandsversion
- Zugeordnete Ressourcenreferenzen
- Änderungs- und Abhängigkeitsinformationen

Mehrere Szenen DÜRFEN unabhängig voneinander existieren.

Szenen können grafischen Oberflächen, Fenstern oder anderen Darstellungskontexten zugeordnet werden.

## 4. Scene Nodes

Grafische Elemente werden durch Scene Nodes repräsentiert.

Ein Scene Node besitzt mindestens:

- Eindeutige Identität
- Elementtyp
- Elternbeziehung
- Geordnete Kindbeziehungen
- Grafische Eigenschaften
- Zustandsversion

Optionale Eigenschaften umfassen:

- Geometrie und Transformation
- Sichtbarkeit und Transparenz
- Clipping
- Material und Effekte
- Inhalt und Ressourcenreferenzen
- Semantische Informationen
- Interaktionszustände

Scene Nodes DÜRFEN unterschiedliche grafische Funktionen erfüllen.

## 5. Szenenhierarchie

Der Scene Graph organisiert Elemente in einer gerichteten, azyklischen Eltern-Kind-Hierarchie.

Die Hierarchie bestimmt:

- Strukturelle Zugehörigkeit
- Lokale und globale Transformationen
- Vererbte Eigenschaften
- Sichtbarkeitsbeziehungen
- Clipping
- Zeichenreihenfolge innerhalb der Szene

Jeder Scene Node besitzt höchstens ein übergeordnetes Element.

Zyklische Eltern-Kind-Beziehungen sind unzulässig.

Zusätzliche grafische Abhängigkeiten werden unabhängig von der Szenenhierarchie verwaltet.

## 6. Zustandsverwaltung

Der Scene Graph verwaltet den veröffentlichten Zustand grafischer Elemente.

Zustandsänderungen erfolgen über Nova Graphics State Transactions.

Dabei MUSS:

- Der bestehende veröffentlichte Zustand konsistent bleiben.
- Die Änderung vor ihrer Veröffentlichung validiert werden.
- Eine neue Zustandsversion atomar veröffentlicht werden.
- Die Zuordnung zwischen Elementen und Eigenschaften erhalten bleiben.
- Die weitere Änderungsverarbeitung auf konsistenten Zuständen basieren.

Nicht abgeschlossene Änderungen DÜRFEN für Rendering und Komposition nicht sichtbar sein.

## 7. Änderungsverfolgung

Der Scene Graph ist in Nova Graphics State Change Tracking integriert.

Er unterstützt insbesondere die Erkennung von:

- Hinzugefügten und entfernten Elementen
- Änderungen grafischer Eigenschaften
- Hierarchieänderungen
- Geometrie- und Transformationsänderungen
- Sichtbarkeitsänderungen
- Material- und Inhaltsänderungen
- Änderungen von Ressourcenreferenzen

Änderungen werden als versionsbezogene Change Sets bereitgestellt.

Unveränderte Teilstrukturen SOLLEN ohne erneute Verarbeitung erhalten bleiben.

## 8. Abhängigkeitsverwaltung

Der Scene Graph stellt strukturelle Beziehungen und Zustandsinformationen für Nova Graphics Dependency Tracking bereit.

Abhängigkeiten können insbesondere entstehen durch:

- Eltern-Kind-Beziehungen
- Layout und Transformationen
- Eigenschaftenbindungen
- Materialien und Effekte
- Ressourcenreferenzen
- Oberflächenüberlagerungen

Die eigentliche Verwaltung und Propagation zusätzlicher Abhängigkeiten erfolgt durch Graphics Dependency Tracking.

Der Scene Graph DARF keine redundante, unabhängige Abhängigkeitsverwaltung erzwingen.

## 9. Inkrementelle Szenenaktualisierung

Nach einer veröffentlichten Zustandsänderung werden ausschließlich betroffene Szenenbereiche aktualisiert, soweit dies technisch sinnvoll ist.

NovaOS SOLL:

- Unveränderte Scene Nodes wiederverwenden.
- Bereits berechnete Transformationen erhalten.
- Gültige Geometrie- und Layoutinformationen wiederverwenden.
- Betroffene Teilstrukturen gezielt aktualisieren.
- Unnötige vollständige Szenendurchläufe vermeiden.

Bei komplexen Änderungen oder ungültigen Zwischenergebnissen DARF eine vollständige Szenenauswertung erfolgen.

## 10. Rendering-Integration

Der Scene Graph beschreibt den grafischen Zustand, führt jedoch selbst keine Rendering-Operationen aus.

Er stellt der Rendering-Architektur konsistente Szeneninformationen bereit.

Die weitere Verarbeitung erfolgt durch:

- Nova Render Graph
- Nova Render Engine
- Nova Material Engine
- Nova Compositor

Die konkrete Auswahl von Rendering-Operationen, GPU-Befehlen und Ausgabeverfahren bleibt Aufgabe dieser Komponenten.

Rendering MUSS auf einer konsistenten veröffentlichten Szenenversion basieren.

## 11. Ressourcenverwaltung

Scene Nodes können Grafikressourcen referenzieren.

Dazu gehören insbesondere:

- Texturen
- Bilder
- Schriftressourcen
- Geometrien
- Materialien
- Effektparameter
- Grafikbuffer

Der Scene Graph verwaltet die Zuordnung und Lebensdauer seiner Ressourcenreferenzen.

Die eigentliche Speicherverwaltung erfolgt durch die zuständigen Ressourcenmanager.

Referenzierte Ressourcen DÜRFEN nicht freigegeben werden, solange sie von aktiven Szenenversionen benötigt werden.

## 12. Parallelität und Isolation

Szenen und unabhängige Teilstrukturen DÜRFEN parallel ausgewertet werden.

Dabei MUSS gewährleistet sein:

- Konsistente veröffentlichte Zustände
- Sichere parallele Lesezugriffe
- Kontrollierte Zustandsänderungen
- Eindeutige Versionszuordnung
- Sichere Ressourcenlebensdauer

Anwendungen DÜRFEN keine unberechtigten Zugriffe auf fremde Szenen oder Scene Nodes erhalten.

Die Ausführung des Scene Graph ist nicht an einen bestimmten CPU-Kern oder Thread gebunden.

## 13. Integration mit Nova UI

Nova UI verwendet den Scene Graph zur Verwaltung seiner grafischen Darstellung.

UI-Elemente können durch Scene Nodes oder zugeordnete grafische Teilstrukturen repräsentiert werden.

UI-spezifische Funktionen wie:

- Controls
- Styles
- Templates
- Datenbindungen
- Layout
- Ereignisverarbeitung

bleiben Aufgabe der Nova UI Engine.

Der Scene Graph MUSS unabhängig von konkreten UI-Controls und Anwendungslogik bleiben.

## 14. Verbindliche Anforderungen

1. Der Scene Graph MUSS grafische Szenen persistent verwalten.
2. Jede Szene und jeder Scene Node MUSS eindeutig identifizierbar sein.
3. Die Szenenhierarchie MUSS azyklisch und konsistent bleiben.
4. Grafische Zustände MÜSSEN versionsbasiert verwaltet werden.
5. Zustandsänderungen MÜSSEN atomar veröffentlicht werden.
6. Rendering MUSS auf konsistenten Szenenversionen basieren.
7. Änderungen MÜSSEN für Graphics Change Tracking verfügbar sein.
8. Strukturelle und grafische Abhängigkeiten MÜSSEN berücksichtigt werden.
9. Unveränderte Szenenbereiche SOLLEN wiederverwendet werden.
10. Ressourcenreferenzen MÜSSEN während ihrer Verwendung gültig bleiben.
11. Der Scene Graph MUSS unabhängig von konkreten Rendering-Backends funktionieren.
12. Szenenzugriffe MÜSSEN durch definierte Berechtigungen und Isolation geschützt sein.

## 15. Ergebnis

Die Nova Scene Graph Architecture stellt eine persistente, hierarchische und versionsbasierte Verwaltung grafischer Szenen bereit.

Sie bildet die strukturelle Grundlage für Nova State-Driven Graphics und das Nova Declarative Graphics Model.

Durch atomare Zustandsänderungen, gezielte Aktualisierungen und die Wiederverwendung unveränderter Szenenbereiche ermöglicht sie eine konsistente, deterministische und ressourcenschonende Grafikverarbeitung.