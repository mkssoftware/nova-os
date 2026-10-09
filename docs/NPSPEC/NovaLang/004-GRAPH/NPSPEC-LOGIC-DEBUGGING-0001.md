
# NPSPEC-LOGIC-DEBUGGING-0001 – NovaOS Logic Graph Debugging

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Debugging

## Zweck

Definiert die kontrollierte Untersuchung und Fehlersuche während der Ausführung von Logic Graphs.

Ziel ist, Knoten, Datenflüsse, Zustände und Ausführungsabläufe nachvollziehbar zu machen, ohne die Sicherheit oder Isolation einer Solution zu beeinträchtigen.

## Architektur

Das Debugging-System besteht aus:

- **Graph Debugger:** Zentrale Steuerung von Debug-Sitzungen.
- **Breakpoint Manager:** Verwaltung von Haltepunkten.
- **Execution Controller:** Anhalten und schrittweises Ausführen.
- **State Inspector:** Untersuchung von Variablen und Zuständen.
- **Dataflow Inspector:** Anzeige übertragener Portwerte.
- **Debug Event Stream:** Bereitstellung von Laufzeitereignissen.
- **NovaLang Debug Bridge:** Verbindung zum NovaLang Debugger.

Das System integriert sich in Graph Runtime, Scheduler und Error Handling.

## Debug-Funktionen

Unterstützt werden:

- Haltepunkte an Knoten und Verbindungen
- Bedingte Haltepunkte
- Ausführung fortsetzen und anhalten
- Einzelne Knoten ausführen
- Schrittweise Ausführung von Subgraphs
- Untersuchung von Eingangs- und Ausgangsports
- Anzeige von Variablen und Graphzuständen
- Nachverfolgung von Fehlern und Ereignissen

## Ausführungssteuerung

Der Debugger unterstützt:

- **Continue:** Ausführung fortsetzen.
- **Pause:** Ausführung an einem sicheren Punkt anhalten.
- **Step Into:** In Subgraphs oder Custom Scripts wechseln.
- **Step Over:** Aktuellen Knoten vollständig ausführen.
- **Step Out:** Übergeordneten Ausführungskontext erreichen.
- **Stop:** Debug-Ausführung kontrolliert beenden.

Bei parallelen und asynchronen Aufgaben muss der betroffene Ausführungskontext eindeutig erkennbar sein.

## NovaLang-Integration

Custom Scripts verwenden die reguläre NovaLang-Debugging-Infrastruktur.

Beim Wechsel in einen Script Node können Quellcode, Variablen, Aufrufstack und Exceptions untersucht werden.

Graph- und Script-Debugging müssen innerhalb einer gemeinsamen Debug-Sitzung zusammenarbeiten.

## Zustände und Datenflüsse

Der State Inspector zeigt typisierte Werte und deren Gültigkeitsbereich.

Der Dataflow Inspector ermöglicht die Nachverfolgung von Daten zwischen verbundenen Ports.

Änderungen an Laufzeitwerten sind nur in ausdrücklich autorisierten Debug-Kontexten zulässig und müssen protokolliert werden.

## Determinismus und Replay

Der Debugger kann aufgezeichnete Ausführungen des Determinism-Systems untersuchen.

Replay ermöglicht die Analyse vergangener Knotenaktivierungen, Zustandsänderungen und Fehler.

Eine rückwärtsgerichtete Untersuchung ist nur möglich, soweit die benötigten Zustände und Ereignisse aufgezeichnet wurden.

## Sicherheit

Debug-Zugriff benötigt eine ausdrücklich autorisierte Berechtigung.

Der Debugger darf keine Capability-Grenzen umgehen oder unberechtigt auf andere Solutions zugreifen.

Vertrauliche Daten müssen gemäß den geltenden Zugriffsregeln geschützt bleiben.

## Normative Anforderungen

1. Die Graph Runtime MUSS Debug-Sitzungen unterstützen.
2. Haltepunkte an Knoten und Verbindungen MÜSSEN möglich sein.
3. Bedingte Haltepunkte MÜSSEN unterstützt werden.
4. Continue, Pause, Step Into, Step Over, Step Out und Stop MÜSSEN verfügbar sein.
5. Knoten-, Port- und Ausführungszustände MÜSSEN untersuchbar sein.
6. Parallele und asynchrone Ausführungen MÜSSEN eindeutig identifizierbar sein.
7. NovaLang-Script-Debugging MUSS integriert sein.
8. Fehler MÜSSEN ihrem verursachenden Ausführungskontext zugeordnet werden können.
9. Debug-Ereignisse MÜSSEN strukturiert bereitgestellt werden.
10. Debug-Zugriffe MÜSSEN autorisiert und isoliert sein.
11. Debugging DARF keine Capability- oder Ressourcenlimits umgehen.
12. Das Debugging-System MUSS unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine integrierte Debugging-Infrastruktur für Logic Graphs und NovaLang mit Haltepunkten, schrittweiser Ausführung, Datenflussanalyse und sicherer Laufzeitinspektion.
