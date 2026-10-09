
# NPSPEC-NOVALANG-DEBUGGING-0001 – NovaLang Debugging

## Status

Angenommen

## Kategorie

NovaLang / Runtime / Debugging

## Zweck

Definiert die Debugging-Infrastruktur von NovaLang zur Analyse von Programmen, Tasks und Solutions.

Ziel sind präzise Fehlerdiagnosen, effiziente Entwicklungswerkzeuge und einheitliches Debugging für AOT, JIT und Interpreter.

## Architektur

| Komponente | Aufgabe |
|---|---|
| Debug Agent | Steuerung der Programmausführung |
| Debug Protocol | Kommunikation mit NovaLang Studio |
| Symbol Resolver | Auflösung von Funktionen und Variablen |
| Breakpoint Manager | Verwaltung von Haltepunkten |
| State Inspector | Untersuchung des Programmzustands |
| Stack Inspector | Analyse von Aufrufstacks |
| Task Inspector | Untersuchung asynchroner Tasks |

Der Debug Agent arbeitet über definierte Runtime-Schnittstellen.

## Debug-Funktionen

NovaLang unterstützt:

- Breakpoints und bedingte Breakpoints
- Step Into, Step Over und Step Out
- Pause, Continue und Stop
- Call Stack und Async Stack
- Anzeige lokaler Variablen und Objekte
- Überwachung von Ausdrücken
- Exception-Breakpoints
- Thread- und Task-Inspektion

Watch-Ausdrücke dürfen im sicheren Standardmodus keine Seiteneffekte auslösen.

## Debug-Informationen

Compiler und Runtime stellen versionierte Debug-Informationen bereit:

- Zuordnung zwischen Quellcode und ausführbaren Instruktionen
- Symbolnamen und Typinformationen
- Variablenbereiche und Lebenszeiten
- Funktions- und Modulidentitäten
- Inline- und Async-Zustandsinformationen

Optimierter Code muss Abweichungen zwischen Quellcode und tatsächlicher Ausführung kenntlich machen.

## Ausführungsmodi

| Modus | Debugging |
|---|---|
| Interpreter | Direkte Instruktions- und Zustandsanalyse |
| JIT | Debugging über Code- und Quelltextzuordnungen |
| AOT | Debugging über native Symbole und Debug-Informationen |

Alle Modi verwenden ein gemeinsames Debug-Protokoll.

## NovaLang Studio

NovaLang Studio integriert Debugging direkt in Code-Editor und Logic Graph.

Bei Solutions können Capability-Knoten, Datenflüsse und Custom Scripts untersucht werden.

Die Darstellung muss zwischen tatsächlichen Laufzeitwerten und rekonstruierten oder optimierungsbedingt nicht verfügbaren Werten unterscheiden.

## Sicherheit

- Debugging benötigt eine ausdrückliche Autorisierung.
- Debug-Zugriffe unterliegen den Schutzdomänen von NovaOS.
- Fremde Prozesse und geschützte Speicherbereiche dürfen nicht unautorisiert untersucht werden.
- Debugging darf keine zusätzlichen Capability-Berechtigungen erzeugen.
- Sensible Daten müssen entsprechend ihrer Zugriffsregeln geschützt bleiben.
- Produktivsysteme müssen Debugging deaktivieren oder einschränken können.

## Fehlerdiagnostik

Bei Laufzeitfehlern sollen folgende Informationen verfügbar sein:

- Fehlertyp und Beschreibung
- Betroffene Quelltextposition
- Call Stack und Task-Kontext
- Modul- und Runtime-Version
- Relevante Ressourcen- und Ausführungsinformationen

Crash-Dumps dürfen nur gemäß den geltenden Sicherheits- und Datenschutzregeln erstellt werden.

## Normative Anforderungen

1. NovaLang MUSS ein einheitliches Debugging-Modell für AOT, JIT und Interpreter unterstützen.
2. Breakpoints, Einzelschrittausführung und Zustandsinspektion MÜSSEN verfügbar sein.
3. Compiler MÜSSEN Quelltextzuordnungen und Debug-Symbole erzeugen können.
4. Async-Tasks und Exceptions MÜSSEN diagnostizierbar sein.
5. Debug-Zugriffe MÜSSEN ausdrücklich autorisiert werden.
6. Debugging DARF Capability- und Sandbox-Grenzen nicht umgehen.
7. Optimierungsbedingt nicht verfügbare Werte MÜSSEN entsprechend gekennzeichnet werden.
8. NovaLang Studio MUSS das gemeinsame Debug-Protokoll verwenden können.
9. Die Debugging-Infrastruktur DARF keine .NET-Runtime voraussetzen.

## Ergebnis

NovaLang erhält eine einheitliche, sichere Debugging-Infrastruktur für native Programme, Bytecode und Solutions mit vollständiger Integration in NovaLang Studio und kontrollierter Laufzeitinspektion.
