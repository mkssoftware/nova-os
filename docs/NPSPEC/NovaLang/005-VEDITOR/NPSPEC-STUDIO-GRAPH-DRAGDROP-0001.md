
# NPSPEC-STUDIO-GRAPH-DRAGDROP-0001 – NovaLang Studio Graph Drag & Drop

## Status

Angenommen

## Kategorie

NovaOS / NovaLang Studio / Logic Graph Editor / Drag & Drop

## Zweck

Definiert die Drag-and-drop-Interaktionen innerhalb des Logic Graph Editors.

Ziel ist die schnelle, intuitive und sichere Erstellung sowie Bearbeitung von Graphen durch direktes Verschieben, Einfügen und Verbinden von Elementen.

## Architektur

Das Drag-and-drop-System besteht aus:

- **Drag Manager:** Verwaltung aktiver Ziehvorgänge.
- **Drop Target Resolver:** Erkennung gültiger Ablageziele.
- **Drag Preview Renderer:** Darstellung einer Vorschau.
- **Compatibility Validator:** Prüfung von Typen und Verbindungen.
- **Graph Mutation Controller:** Übernahme gültiger Änderungen.
- **Undo Integration:** Rücknahme abgeschlossener Aktionen.

Das System verwendet das bestehende Graphmodell und dessen Validierungsregeln.

## Unterstützte Operationen

- Knoten aus der Node Library auf das Canvas ziehen
- Vorhandene Knoten verschieben
- Mehrere Knoten gemeinsam verschieben
- Verbindungen zwischen Ports erstellen
- Verbindungen auf andere kompatible Ports umlegen
- Knoten in Gruppen und Subgraphs einfügen
- Knoten zwischen Graphen kopieren oder verschieben
- Custom Scripts und unterstützte Ressourcen einfügen

## Interaktionsablauf

Ein Drag-and-drop-Vorgang durchläuft folgende Zustände:

1. **Idle:** Keine aktive Interaktion.
2. **Dragging:** Element wird bewegt.
3. **Preview:** Ablageposition und mögliche Verbindung werden angezeigt.
4. **Validation:** Ziel und Operation werden geprüft.
5. **Commit:** Gültige Änderung wird übernommen.
6. **Cancel:** Vorgang wird ohne Änderung beendet.

Ungültige Ablageziele müssen unmittelbar erkennbar sein.

## Visuelle Rückmeldung

Während des Ziehens werden dargestellt:

- Vorschau des bewegten Elements
- Mögliche Ablageposition
- Kompatible Ports und Verbindungen
- Ungültige Ziele
- Optionale Rasterausrichtung
- Hinweise auf erforderliche Berechtigungen

Die Vorschau darf die tatsächliche Graphdefinition nicht verändern.

## Knoten und Verbindungen

Beim Einfügen erhält jeder neue Knoten eine eindeutige Node-ID.

Beim Verschieben bleiben vorhandene IDs und Verbindungen erhalten.

Neue oder geänderte Verbindungen müssen vor der Übernahme durch den Graph Type Checker geprüft werden.

## Capability-Integration

Capabilities können direkt aus dem Capability Browser auf das Canvas gezogen werden.

Dabei wird ein entsprechender Capability Node erzeugt.

Das Einfügen einer Capability darf keine Berechtigung automatisch erteilen oder eine Systemoperation ausführen.

## Undo und Transaktionen

Jeder abgeschlossene Drag-and-drop-Vorgang wird als zusammenhängende Editoraktion behandelt.

Mehrfachverschiebungen und kombinierte Änderungen müssen vollständig rückgängig gemacht werden können.

Abgebrochene oder ungültige Vorgänge dürfen keine unvollständigen Graphänderungen hinterlassen.

## Performance

Die Vorschau muss ohne vollständige Neuberechnung des Graphen aktualisiert werden können.

Bei umfangreichen Graphen werden nur betroffene Elemente und Verbindungen neu dargestellt.

Animationen dürfen die Eingabereaktionszeit nicht beeinträchtigen.

## Normative Anforderungen

1. Der Graph Editor MUSS Drag-and-drop für Knoten und Verbindungen unterstützen.
2. Mehrfachauswahl und gemeinsames Verschieben MÜSSEN möglich sein.
3. Gültige und ungültige Ablageziele MÜSSEN visuell unterscheidbar sein.
4. Verbindungen MÜSSEN vor ihrer Übernahme typgeprüft werden.
5. Neue Knoten MÜSSEN eindeutige Instanz-IDs erhalten.
6. Beim Verschieben MÜSSEN bestehende Identitäten erhalten bleiben.
7. Abgebrochene Vorgänge DÜRFEN das Graphmodell nicht verändern.
8. Abgeschlossene Änderungen MÜSSEN über Undo rückgängig gemacht werden können.
9. Drag-and-drop MUSS mit dem Graph Validator integriert sein.
10. Capability Nodes DÜRFEN durch Einfügen keine Berechtigungen erhalten.
11. Vorschauen DÜRFEN keine Graph-Ausführung auslösen.
12. Die Interaktion MUSS auch bei umfangreichen Graphen reaktionsschnell bleiben.
13. Alle Drag-and-drop-Funktionen MÜSSEN ohne KI funktionieren.

## Ergebnis

NovaLang Studio erhält ein konsistentes, performantes und sicheres Drag-and-drop-System zur direkten visuellen Bearbeitung von Logic Graphs und NovaOS-Solutions.
