
# NPSPEC-STUDIO-GRAPH-DEBUGGER-0001 – NovaLang Studio Graph Debugger

## Status

Angenommen

## Kategorie

NovaOS / NovaLang Studio / Logic Graph Editor / Debugger

## Zweck

Definiert die grafische Debugging-Oberfläche für Logic Graphs innerhalb von NovaLang Studio.

Ziel ist die direkte Untersuchung von Ausführungsabläufen, Knoten, Datenflüssen und Fehlern auf dem Graph Canvas.

## Architektur

Der Graph Debugger besteht aus:

- **Debug Controller:** Steuerung der Debug-Sitzung.
- **Breakpoint Manager:** Verwaltung visueller Haltepunkte.
- **Execution Visualizer:** Darstellung aktiver und abgeschlossener Knoten.
- **Data Inspector:** Untersuchung von Portwerten und Variablen.
- **Call Stack Viewer:** Darstellung verschachtelter Ausführungen.
- **Event Timeline:** Chronologische Anzeige von Ereignissen.
- **NovaLang Debug Bridge:** Integration des Script-Debuggers.

Der Graph Debugger verwendet die bestehende Logic Graph Debugging Runtime.

## Debug-Steuerung

Unterstützt werden:

- Starten und Beenden
- Pause und Fortsetzen
- Step Into
- Step Over
- Step Out
- Haltepunkte setzen und entfernen
- Bedingte Haltepunkte
- Wechsel zwischen parallelen Ausführungskontexten

Die Steuerung erfolgt über eine unmittelbar erreichbare Debug-Leiste.

## Visuelle Darstellung

Das Graph Canvas zeigt:

- Aktive Knoten durch dezente Hervorhebung
- Zuletzt ausgeführte Verbindungen
- Wartende und abgeschlossene Aufgaben
- Haltepunkte
- Fehlerhafte Knoten
- Aktuelle Ausführungsposition

Animationen müssen abschaltbar sein und dürfen die Debug-Ausführung nicht beeinflussen.

## Dateninspektion

Der Data Inspector ermöglicht:

- Anzeige typisierter Portwerte
- Untersuchung von Graphvariablen
- Anzeige von Ein- und Ausgangsdaten
- Vergleich von Werten zwischen Ausführungsschritten
- Überwachung ausgewählter Variablen

Vertrauliche Daten dürfen nur entsprechend der Debug-Berechtigungen angezeigt werden.

## NovaLang-Integration

Beim Betreten eines Custom Script Nodes kann direkt in den NovaLang Debugger gewechselt werden.

Graph- und Script-Debugger verwenden denselben Ausführungskontext.

NovaLang-Breakpoints, Exceptions und Aufrufstacks müssen gemeinsam mit dem Logic Graph nachvollziehbar bleiben.

## Asynchrone Ausführung

Parallele Tasks, Await-Zustände und Ereignisse werden getrennt dargestellt.

Der Debugger muss eindeutig zwischen pausierten, wartenden und laufenden Ausführungen unterscheiden.

Einzelschritte dürfen keine undefinierten Zustandsänderungen anderer Tasks verursachen.

## Record und Replay

Aufgezeichnete Ausführungen können über die Event Timeline untersucht werden.

Die Integration verwendet das bestehende Logic Graph Determinism-System.

Replay darf keine externen Seiteneffekte unbeabsichtigt erneut ausführen.

## Sicherheit

Debugging benötigt eine autorisierte Debug-Sitzung.

Der Debugger darf keine Capability-Grenzen umgehen oder unberechtigt auf andere Solutions zugreifen.

Die produktive Graph-Ausführung muss auch ohne angeschlossenen Debugger funktionieren.

## Normative Anforderungen

1. NovaLang Studio MUSS einen integrierten visuellen Graph Debugger bereitstellen.
2. Haltepunkte MÜSSEN direkt auf dem Canvas gesetzt werden können.
3. Continue, Pause, Step Into, Step Over und Step Out MÜSSEN unterstützt werden.
4. Aktive und fehlerhafte Knoten MÜSSEN visuell erkennbar sein.
5. Portwerte und Graphvariablen MÜSSEN untersuchbar sein.
6. Parallele und asynchrone Ausführungen MÜSSEN unterscheidbar sein.
7. NovaLang-Script-Debugging MUSS nahtlos integriert sein.
8. Eine chronologische Ereignisansicht MUSS verfügbar sein.
9. Record- und Replay-Daten MÜSSEN untersucht werden können.
10. Debug-Overlays DÜRFEN die Graphdefinition nicht verändern.
11. Debug-Zugriffe MÜSSEN autorisiert und isoliert sein.
12. Debugging DARF keine Capability- oder Ressourcenlimits umgehen.
13. Der Graph Debugger MUSS ohne KI funktionieren.

## Ergebnis

NovaLang Studio erhält einen integrierten visuellen Debugger, der Logic Graphs und NovaLang-Scripts gemeinsam untersuchbar macht und komplexe Ausführungsabläufe unmittelbar auf dem Graph Canvas nachvollziehbar darstellt.
