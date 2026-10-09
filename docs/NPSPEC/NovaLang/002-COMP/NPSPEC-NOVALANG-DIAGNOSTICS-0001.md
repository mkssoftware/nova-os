
# NPSPEC-NOVALANG-DIAGNOSTICS-0001 – NovaLang Diagnostics

## Status

Angenommen

## Kategorie

NovaLang / Compiler / Runtime / Diagnostik

## Zweck

Definiert ein einheitliches Diagnosesystem für Compiler, Runtime, Nova VM und NovaLang Studio.

Ziel sind verständliche Fehlermeldungen, strukturierte Diagnosedaten und eine schnelle Ursachenanalyse bei möglichst geringem Ressourcenverbrauch.

## Architektur

Alle NovaLang-Komponenten verwenden ein gemeinsames Diagnosemodell.

| Komponente | Aufgabe |
|---|---|
| Diagnostic Engine | Erfassung und Verarbeitung |
| Diagnostic Catalog | Verwaltung von Fehlercodes |
| Source Mapper | Zuordnung zu Quelltextpositionen |
| Runtime Reporter | Meldung von Laufzeitproblemen |
| Diagnostic Sink | Ausgabe an Konsole, Datei oder Studio |
| Diagnostic Filter | Filterung und Priorisierung |

## Diagnoseklassen

| Klasse | Bedeutung |
|---|---|
| Error | Fehler, der eine Operation verhindert |
| Warning | Mögliches Problem |
| Information | Technischer Hinweis |
| Hint | Verbesserungsvorschlag |
| Fatal | Kritischer, nicht fortsetzbarer Fehler |

Jede Diagnose besitzt mindestens:

- Eindeutigen Fehlercode
- Schweregrad
- Beschreibung
- Ursprungskomponente
- Quelltextposition oder Laufzeitkontext, sofern verfügbar

## Fehlercodes

Fehlercodes verwenden das Format:

`NL-<BEREICH>-<NUMMER>`

Beispiele:

- `NL-SYN-0001` – Syntaxfehler
- `NL-TYP-0001` – Typkonflikt
- `NL-RUN-0001` – Laufzeitfehler
- `NL-CAP-0001` – Capability-Zugriff verweigert

Ein veröffentlichter Fehlercode darf nicht für eine andere Fehlerbedeutung wiederverwendet werden.

## Compilerdiagnostik

Der Compiler meldet Fehler aus Lexer, Parser, Type Checker, Verifier und Codegenerierung.

Diagnosen sollen konkrete Korrekturhinweise enthalten und mehrere unabhängige Fehler in einem Durchlauf erfassen können.

Fehlerhafte Programme dürfen nicht als gültige ausführbare Artefakte freigegeben werden.

## Runtimediagnostik

Die Runtime erfasst:

- Exceptions und nicht behandelte Fehler
- Ungültige Modul- und Bytecode-Zustände
- Ressourcenüberschreitungen
- Task- und Cancellation-Fehler
- Capability- und Sandbox-Verletzungen

Diagnosen dürfen keine vertraulichen Daten oder unautorisierten Speicherinhalte offenlegen.

## NovaLang Studio

NovaLang Studio verwendet strukturierte Diagnosen für:

- Fehlerkennzeichnung im Editor
- Fehlerliste und Quelltextnavigation
- Korrekturvorschläge
- Build- und Laufzeitberichte
- Zuordnung von Fehlern zu Logic-Graph-Knoten

Diagnosen müssen inkrementell aktualisiert werden können.

## Performance

- Diagnostik darf die normale Programmausführung nicht unnötig verlangsamen.
- Diagnosepuffer müssen begrenzbar sein.
- Wiederholte Meldungen dürfen zusammengefasst werden.
- Kritische Fehler müssen auch bei eingeschränkten Ressourcen gemeldet werden können.

## Normative Anforderungen

1. NovaLang MUSS ein gemeinsames strukturiertes Diagnosemodell besitzen.
2. Jede Diagnose MUSS einen stabilen Fehlercode und Schweregrad enthalten.
3. Compiler und Runtime MÜSSEN Diagnosen über definierte Schnittstellen bereitstellen.
4. Quelltextpositionen und Ausführungskontexte MÜSSEN soweit verfügbar zugeordnet werden.
5. Fehlerhafte ausführbare Artefakte DÜRFEN nicht freigegeben werden.
6. Diagnosen DÜRFEN keine Sicherheits- oder Datenschutzgrenzen verletzen.
7. Diagnosepuffer und Ressourcenverbrauch MÜSSEN begrenzbar sein.
8. NovaLang Studio MUSS Diagnosen maschinenlesbar verarbeiten können.
9. AOT, JIT und Interpreter MÜSSEN dasselbe Diagnosemodell verwenden.

## Ergebnis

NovaLang erhält ein einheitliches, ressourcenschonendes Diagnosesystem mit stabilen Fehlercodes, strukturierten Meldungen und direkter Integration in Compiler, Runtime, Nova VM und NovaLang Studio.
