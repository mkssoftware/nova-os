
# NPSPEC-STUDIO-DEBUGGER-0001 – NovaLang Studio Debugger

## Status

Angenommen

## Kategorie

NovaLang Studio / Debugging / Entwicklungswerkzeuge

## Zweck

Definiert den integrierten Debugger von NovaLang Studio zur kontrollierten Untersuchung und Fehleranalyse von NovaLang-Programmen, Solutions und Logic Graphs.

Ziel ist eine leistungsfähige, übersichtliche Debugging-Umgebung mit direktem Zugriff auf Ausführungszustände, Variablen, Tasks und Datenflüsse.

Die Implementierung basiert auf `NPSPEC-NOVALANG-DEBUGGING-0001`.

## Architektur

| Komponente | Aufgabe |
|---|---|
| Debug Manager | Verwaltung von Debug-Sitzungen |
| Debug Adapter | Verbindung zur NovaLang Runtime |
| Breakpoint Manager | Verwaltung von Haltepunkten |
| Execution Controller | Steuerung der Programmausführung |
| Variable Inspector | Untersuchung von Variablen und Objekten |
| Stack Inspector | Analyse von Aufrufstacks |
| Task Inspector | Untersuchung von Tasks und Threads |
| Graph Debugger | Debugging von Logic-Graph-Datenflüssen |
| Debug Console | Diagnoseausgaben und kontrollierte Ausdrücke |

Alle Komponenten verwenden das gemeinsame, versionierte NovaLang-Debug-Protokoll.

## Debug-Sitzungen

Eine Debug-Sitzung besitzt:

- Eindeutige Sitzungsidentität
- Zielprozess oder Solution
- Ausführungsmodus
- Debug-Konfiguration
- Aktuellen Ausführungszustand
- Zugeordnete Breakpoints
- Autorisierten Zugriffskontext

Unterstützte Sitzungszustände:

`Created → Running ↔ Paused → Stopped`

Fehlerhafte oder getrennte Sitzungen werden gesondert gekennzeichnet.

## Ausführungssteuerung

Der Debugger unterstützt:

- Start Debugging
- Attach to Process
- Pause
- Continue
- Step Into
- Step Over
- Step Out
- Run to Cursor
- Restart
- Stop Debugging

Ein Neustart muss den definierten Startzustand wiederherstellen oder auf nicht reproduzierbare Zustände hinweisen.

## Breakpoints

Unterstützt werden:

- Zeilen-Breakpoints
- Bedingte Breakpoints
- Hit-Count-Breakpoints
- Exception-Breakpoints
- Funktions-Breakpoints
- Logic-Graph-Breakpoints

Breakpoints müssen aktivierbar, deaktivierbar und entfernbar sein.

Nicht auflösbare Breakpoints werden entsprechend gekennzeichnet.

## Variableninspektion

Der Variable Inspector zeigt:

- Lokale Variablen
- Funktionsparameter
- Objektfelder und Eigenschaften
- Datentypen
- Aktuelle Werte
- Nullability-Zustände
- Verschachtelte Datenstrukturen

Große Objekte und Sammlungen werden bedarfsgerecht geladen.

Die Inspektion darf nicht automatisch nebenwirkungsbehaftete Eigenschaften ausführen.

## Watch-Ausdrücke

Benutzer können Ausdrücke zur Beobachtung definieren.

Beispiel:

```vb
person.Alter
ergebnis > 100
werte.Count
```

Watch-Ausdrücke werden standardmäßig ohne Seiteneffekte ausgewertet.

Nicht sicher auswertbare Ausdrücke müssen zurückgewiesen oder ausdrücklich als potenziell nebenwirkungsbehaftet gekennzeichnet werden.

## Call Stack

Der Stack Inspector stellt dar:

- Aktuelle Funktion
- Aufrufende Funktionen
- Modul- und Quelltextpositionen
- Async-Aufrufketten
- Zugehörigen Task-Kontext

Einzelne Stack Frames können ausgewählt und untersucht werden.

Optimierungsbedingt nicht verfügbare Informationen müssen eindeutig gekennzeichnet werden.

## Task- und Thread-Debugging

Der Debugger unterstützt:

- Anzeige aktiver Tasks und Threads
- Task-Zustände
- Wartebedingungen
- Cancellation-Status
- Zugehörige Execution Contexts
- Wechsel zwischen Ausführungskontexten

Die Debugging-Steuerung muss zwischen dem Anhalten eines einzelnen Ausführungskontexts und dem Anhalten des gesamten Zielprozesses unterscheiden.

## Logic-Graph-Debugging

Der Graph Debugger ermöglicht die direkte Untersuchung von Solutions.

Unterstützt werden:

- Breakpoints auf Graph-Knoten
- Schrittweise Ausführung zwischen Knoten
- Anzeige von Eingangs- und Ausgangswerten
- Visualisierung des aktiven Datenflusses
- Untersuchung von Capability-Aufrufen
- Anzeige von Ausführungsdauer und Fehlerzuständen
- Wechsel vom Graph-Knoten zum Custom Script

Die angezeigte Reihenfolge muss der tatsächlichen Ausführung entsprechen.

Bei parallelen Graph-Zweigen werden unabhängige Ausführungspfade kenntlich gemacht.

## Capability-Debugging

Capability-Aufrufe können anhand ihrer freigegebenen Diagnoseinformationen untersucht werden.

Angezeigt werden können:

- Capability-Identität
- Vertragsversion
- Aufrufstatus
- Ein- und Ausgabedaten
- Fehlermeldungen
- Ausführungsdauer

Geschützte Daten müssen maskiert oder ausgeblendet werden.

Debugging darf keine zusätzlichen Capability-Berechtigungen erzeugen.

## UI-Debugging

Für `.nui`-Oberflächen unterstützt der Debugger:

- Untersuchung von Datenbindungen
- Anzeige aktueller UI-Zustände
- Ereignisverfolgung
- Navigation zwischen UI-Komponente und Quellcode
- Diagnose fehlerhafter Bindungen

Die Inspektion muss über autorisierte UI- und Runtime-Schnittstellen erfolgen.

## Ausführungsmodi

| Modus | Debugging |
|---|---|
| Interpreter | Direkte Analyse virtueller Instruktionen |
| JIT | Debugging über Quelltext- und Codezuordnungen |
| AOT | Debugging über native Symbole und Debug-Informationen |

Alle Modi verwenden dieselben grundlegenden Debugging-Funktionen.

Abweichungen durch Optimierungen müssen erkennbar sein.

## Debug Console

Die Debug Console bietet:

- Ausgabe von Laufzeitmeldungen
- Anzeige von Exceptions
- Kontrollierte Auswertung von Ausdrücken
- Navigation zu Fehlerursprüngen
- Filterung nach Task und Quelle

Die Konsole darf nicht automatisch privilegierte Systembefehle ausführen.

## Benutzeroberfläche

Der Debugger verwendet die NovaOS-Designsprache.

- Kompakte Debug-Werkzeugleiste
- Direkt erreichbare Ausführungssteuerung
- Integrierte Breakpoint-Markierungen
- Frei anordenbare Debug-Panels
- Kontextabhängige Variablenanzeige
- Hervorhebung der aktuellen Ausführungsposition
- Einheitliche Darstellung in Code Editor und Logic Graph

Häufig verwendete Debugging-Funktionen müssen ohne zusätzliche Menüebenen erreichbar sein.

## Performance

- Debugging-Komponenten werden bei Bedarf geladen.
- Variablen und Objekte werden verzögert inspiziert.
- Ereignispuffer und Diagnoseausgaben sind begrenzbar.
- Nicht aktive Debug-Sitzungen sollen keine unnötigen Ressourcen verbrauchen.
- Umfangreiche Inspektionen erfolgen asynchron.
- Der Debugger muss seine eigene Ressourcenbelastung begrenzen.

## Sicherheit

- Debugging benötigt eine ausdrückliche Autorisierung.
- Attach to Process unterliegt den NovaOS-Zugriffsregeln.
- Geschützte Prozesse und Speicherbereiche dürfen nicht unautorisiert untersucht werden.
- Watch-Ausdrücke dürfen standardmäßig keine Seiteneffekte verursachen.
- Capability- und Sandbox-Grenzen bleiben wirksam.
- Debugging darf keine Berechtigungen verändern oder erweitern.
- Nicht vertrauenswürdige Programme werden in ihrer vorgesehenen Isolation ausgeführt.

## Normative Anforderungen

1. NovaLang Studio MUSS einen integrierten Debugger bereitstellen.
2. Der Debugger MUSS das gemeinsame NovaLang-Debug-Protokoll verwenden.
3. Breakpoints und schrittweise Ausführung MÜSSEN unterstützt werden.
4. Variablen, Objekte und Aufrufstacks MÜSSEN inspizierbar sein.
5. Async-Tasks und Threads MÜSSEN untersucht werden können.
6. Watch-Ausdrücke MÜSSEN standardmäßig seiteneffektfrei ausgewertet werden.
7. Logic-Graph-Knoten und Datenflüsse MÜSSEN debugbar sein.
8. Capability-Aufrufe MÜSSEN innerhalb ihrer Zugriffsrechte diagnostizierbar sein.
9. UI-Bindungen und Ereignisse MÜSSEN untersucht werden können.
10. AOT, JIT und Interpreter MÜSSEN ein gemeinsames Debugging-Modell verwenden.
11. Debug-Sitzungen MÜSSEN eindeutig identifizierbar und kontrolliert beendbar sein.
12. Debugging DARF keine Capability-, Speicher- oder Sandbox-Grenzen umgehen.
13. Ressourcenverbrauch und Debug-Ausgaben MÜSSEN begrenzbar sein.
14. Der Debugger MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaLang Studio erhält einen einheitlichen, sicheren und leistungsfähigen Debugger für NovaLang-Programme, Solutions, Logic Graph und deklarative Benutzeroberflächen.

Code, Datenflüsse, Tasks und Laufzeitzustände können direkt innerhalb der Entwicklungsumgebung untersucht werden, ohne zwischen verschiedenen Debugging-Werkzeugen wechseln zu müssen.
