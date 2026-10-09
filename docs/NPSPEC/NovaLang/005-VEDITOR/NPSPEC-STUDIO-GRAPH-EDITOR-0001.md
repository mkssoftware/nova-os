
# NPSPEC-STUDIO-GRAPH-EDITOR-0001 – NovaLang Studio Logic Graph Editor

## Status

Angenommen

## Kategorie

NovaOS / NovaLang Studio / Logic Graph Editor

## Zweck

Definiert den visuellen Editor zur Erstellung, Bearbeitung und Untersuchung von Logic Graphs innerhalb von NovaLang Studio.

Ziel ist eine intuitive, leistungsfähige und ressourcenschonende Oberfläche, mit der Solutions durch die Kombination von Capabilities, Custom Scripts und Datenflüssen erstellt werden können.

## Architektur

Der Graph Editor besteht aus:

- **Graph Canvas:** Interaktive Arbeitsfläche für Knoten und Verbindungen.
- **Node Palette:** Übersicht verfügbarer Knotentypen und Capabilities.
- **Connection Editor:** Erstellung und Bearbeitung typisierter Verbindungen.
- **Property Inspector:** Bearbeitung von Knoteneigenschaften.
- **Graph Navigator:** Navigation durch umfangreiche Graphen und Subgraphs.
- **Validation Overlay:** Darstellung von Typfehlern und ungültigen Verbindungen.
- **Debug Overlay:** Visualisierung laufender Ausführungen.
- **Graph Serializer Bridge:** Speicherung und Wiederherstellung der Graphdefinition.

Der Editor verwendet das bestehende Logic-Graph-Schema und besitzt keine eigene Ausführungssemantik.

## Bedienkonzept

Der Editor unterstützt:

- Drag-and-drop von Knoten und Capabilities
- Verbindungserstellung durch Ziehen zwischen Ports
- Zoom, Verschieben und automatische Ausrichtung
- Mehrfachauswahl, Kopieren und Einfügen
- Undo und Redo
- Kontextmenüs und Tastenkombinationen
- Schnellsuche nach Knoten und Fähigkeiten
- Gruppierung und Erstellung von Subgraphs
- Automatische Anordnung von Knoten
- Direkte Anzeige von Porttypen und Verbindungsfehlern

Häufig verwendete Funktionen müssen unmittelbar erreichbar sein, ohne verschachtelte Menüstrukturen.

## Capability-Integration

Capabilities werden aus dem zentralen Capability Browser bereitgestellt.

Eine Capability kann direkt auf die Arbeitsfläche gezogen und als Capability Node eingefügt werden.

Beispiel:

**Network Capability → Custom Script → Storage Capability**

Das Custom Script verarbeitet ausschließlich die bereitgestellten Daten und Ressourcen.

Der Editor zeigt benötigte Berechtigungen an, erteilt diese jedoch nicht selbstständig.

## Custom Scripts

Custom Script Nodes können direkt mit dem NovaLang Editor geöffnet werden.

Die zugehörigen `.nlf`-Dateien verwenden die reguläre NovaLang-Syntax.

Änderungen an Script-Schnittstellen müssen mit den verbundenen Ports synchronisiert und erneut validiert werden.

## Validierung

Der Editor integriert den Graph Validator und Type Checker.

Fehler werden unmittelbar an betroffenen Knoten, Ports oder Verbindungen angezeigt.

Ungültige Verbindungen dürfen nicht unbemerkt als ausführbar gespeichert werden.

Unvollständige Graphen dürfen als Entwurf gespeichert werden, müssen aber vor der Ausführung validiert werden.

## Debugging und Vorschau

Während einer Debug-Sitzung können dargestellt werden:

- Aktive und wartende Knoten
- Ausgeführte Verbindungen
- Eingangs- und Ausgangswerte
- Haltepunkte
- Fehler und Warnungen
- Ausführungsdauer einzelner Knoten

Debugging verwendet die bestehende Graph-Debugging-Infrastruktur.

## Speicherung

Graphdefinitionen werden über das standardisierte Logic-Graph-Serialisierungsformat gespeichert.

Editorinformationen wie Position, Zoom und Gruppendarstellung werden getrennt von der Ausführungssemantik verwaltet.

Graph-IDs, Port-IDs und Versionsinformationen müssen erhalten bleiben.

## Sicherheit

Der Editor darf keine Capabilities ohne Autorisierung ausführen.

Berechtigungen werden über die verifizierte Solution-Identität und die zentrale Berechtigungsverwaltung kontrolliert.

Vorschau und Debugging unterliegen denselben Sicherheits- und Ressourcenregeln wie die reguläre Ausführung.

## Normative Anforderungen

1. NovaLang Studio MUSS einen integrierten visuellen Logic Graph Editor bereitstellen.
2. Knoten und Verbindungen MÜSSEN per Drag-and-drop bearbeitbar sein.
3. Ports MÜSSEN ihre Typen und Verbindungsregeln anzeigen können.
4. Capabilities MÜSSEN direkt aus dem Capability Browser eingefügt werden können.
5. Custom Script Nodes MÜSSEN mit dem NovaLang Editor integriert sein.
6. Undo, Redo, Zoom, Mehrfachauswahl und Schnellsuche MÜSSEN unterstützt werden.
7. Subgraphs MÜSSEN visuell erstellt und bearbeitet werden können.
8. Validierungsfehler MÜSSEN unmittelbar erkennbar sein.
9. Der Editor MUSS die Graph-Debugging-Infrastruktur unterstützen.
10. Editor-Metadaten DÜRFEN die Graph-Ausführungssemantik nicht verändern.
11. Graphdefinitionen MÜSSEN versioniert und verlustfrei gespeichert werden können.
12. Der Editor DARF keine Capability- oder Berechtigungsgrenzen umgehen.
13. Die Bedienoberfläche SOLL auch bei umfangreichen Graphen reaktionsschnell bleiben.
14. Alle grundlegenden Editorfunktionen MÜSSEN ohne KI verfügbar sein.

## Ergebnis

NovaLang Studio erhält einen modernen, übersichtlichen und vollständig integrierten Logic Graph Editor, mit dem NovaOS-Solutions visuell erstellt, validiert, getestet und debuggt werden können.
