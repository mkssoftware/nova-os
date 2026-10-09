
# NPSPEC-NOVALANG-EXECUTION-0001 – NovaLang Execution Model

## Status

Angenommen

## Kategorie

NovaLang / Runtime / Ausführungsmodell

## Zweck

Definiert die Ausführung von NovaLang-Programmen, Funktionen und Solutions über AOT, JIT und Interpreter.

Ziel sind einheitliche Sprachsemantik, kontrollierte Nebenläufigkeit, geringe Startzeiten und sichere Integration in NovaOS.

## Ausführungsarchitektur

NovaLang unterstützt drei Ausführungswege:

| Modus | Beschreibung |
|---|---|
| AOT | Vorab kompilierter nativer Maschinencode |
| JIT | Laufzeitkompilierung von verifiziertem Bytecode |
| Interpreter | Direkte Ausführung von verifiziertem Bytecode |

Alle Modi verwenden dieselben Sprach-, Typ-, Speicher- und Fehlerregeln.

Die Runtime wählt den verfügbaren Ausführungsweg entsprechend Programmkonfiguration und Systemressourcen.

## Execution Context

Jede Ausführung besitzt einen definierten Kontext mit:

- Programm- und Modulidentität
- Aufrufstack und lokalem Zustand
- Runtime- und Speicherumgebung
- Autorisierten Capability-Handles
- Ressourcenbudgets
- Cancellation- und Fehlerzustand

Kontexte dürfen nur über definierte Schnittstellen miteinander kommunizieren.

## Ausführungslebenszyklus

1. Programm oder Solution-Komponente laden.
2. Versionen, Abhängigkeiten und Integrität prüfen.
3. Bytecode bei Bedarf verifizieren.
4. Execution Context und Ressourcenlimits einrichten.
5. Module initialisieren und Einstiegspunkt aufrufen.
6. Aufgaben unter Runtime-Kontrolle ausführen.
7. Bei Abschluss, Fehler oder Abbruch Ressourcen freigeben.
8. Ausführungsstatus und Diagnosen bereitstellen.

## Nebenläufigkeit

NovaLang verwendet Structured Concurrency.

- Tasks besitzen einen definierten übergeordneten Ausführungskontext.
- Untergeordnete Tasks müssen kontrolliert abgeschlossen oder abgebrochen werden.
- `Async/Await` darf Threads nicht unnötig blockieren.
- Gemeinsamer veränderbarer Zustand benötigt geeignete Synchronisation.
- Scheduling erfolgt über die NovaLang Runtime und NovaOS.

## Fehler und Abbruch

Exceptions werden entsprechend der NovaLang-Fehlersemantik behandelt.

Nicht behandelte Fehler beenden den betroffenen Ausführungskontext kontrolliert.

Cancellation muss an untergeordnete Tasks weitergegeben werden. Ressourcenfreigaben über `Finally`, `Using` und `Dispose` bleiben gewährleistet.

## Deterministische Ausführung

Ein deterministischer Modus muss verfügbar sein.

Dabei werden Scheduling, Zeitquellen, Zufallswerte und externe Eingaben über kontrollierte Schnittstellen bereitgestellt.

Dies ermöglicht reproduzierbare Tests, Simulationen und Logic-Graph-Ausführungen.

## NovaOS-Integration

- Systemoperationen benötigen gültige Capability-Handles.
- Execution Contexts unterliegen CPU-, Speicher- und Task-Limits.
- Unterschiedliche Schutzdomänen bleiben isoliert.
- Logic-Graph-Skripte verwenden ausschließlich bereitgestellte Capabilities.
- AOT-, JIT- und Interpreter-Code besitzen keine unterschiedlichen Sicherheitsprivilegien.

## Normative Anforderungen

1. NovaLang MUSS ein einheitliches Ausführungsmodell für AOT, JIT und Interpreter besitzen.
2. Jede Ausführung MUSS einem definierten Execution Context zugeordnet sein.
3. Ausführungsmodi MÜSSEN dieselbe beobachtbare Sprachsemantik einhalten.
4. Structured Concurrency und Cancellation MÜSSEN unterstützt werden.
5. Fehler und Ressourcenfreigaben MÜSSEN kontrolliert behandelt werden.
6. Capability- und Ressourcenlimits MÜSSEN unabhängig vom Ausführungsmodus gelten.
7. Deterministische Ausführung MUSS unterstützt werden.
8. Nicht autorisierte Systemzugriffe DÜRFEN nicht möglich sein.
9. Die Ausführung DARF keine .NET-Runtime voraussetzen.

## Ergebnis

NovaLang erhält ein einheitliches, sicheres und ressourcenkontrolliertes Ausführungsmodell für native Programme, Bytecode und Solutions mit Structured Concurrency und optional deterministischer Ausführung.
