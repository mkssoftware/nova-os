
# NPSPEC-NOVALANG-SCHEDULING-0001 – NovaLang Task Scheduling

## Status

Angenommen

## Kategorie

NovaLang / Runtime / Scheduling

## Zweck

Definiert die Planung und Ausführung von NovaLang-Tasks innerhalb der Runtime.

Ziel sind geringe Latenzen, faire Ressourcennutzung, effiziente Nebenläufigkeit und die Integration in den NovaOS-Scheduler.

## Architektur

Der NovaLang Task Scheduler verwaltet logische Tasks und verteilt ausführbare Arbeit auf verfügbare Ausführungsthreads.

| Komponente | Aufgabe |
|---|---|
| Task Queue | Verwaltung ausführbarer Tasks |
| Worker Pool | Ausführung auf verfügbaren Threads |
| Dispatcher | Auswahl des nächsten Tasks |
| Timer Service | Zeitgesteuerte Fortsetzung |
| Cancellation Manager | Kontrollierter Task-Abbruch |
| Resource Controller | Überwachung von Ausführungsbudgets |

Der NovaOS-Kernel-Scheduler bleibt für die tatsächliche CPU-Zuteilung zuständig.

## Scheduling-Modell

- Tasks sind keine eigenständigen Betriebssystemthreads.
- `Async/Await` gibt während des Wartens den Ausführungsthread frei.
- Ausführbare Tasks werden fair eingeplant.
- CPU-intensive Arbeit darf auf mehrere Worker verteilt werden.
- Work Stealing darf zur Lastverteilung eingesetzt werden.
- Blockierende Operationen dürfen den gesamten Scheduler nicht anhalten.

## Prioritäten

NovaLang unterstützt logische Scheduling-Prioritäten:

| Priorität | Verwendung |
|---|---|
| Low | Hintergrundaufgaben |
| Normal | Standardausführung |
| High | Latenzkritische Aufgaben |

Prioritäten sind Scheduling-Hinweise und garantieren keine CPU-Zeit.

Der Scheduler muss Starvation durch geeignete Fairness-Regeln begrenzen.

Echtzeitgarantien werden ausschließlich über ausdrücklich unterstützte NovaOS-Mechanismen bereitgestellt.

## Structured Concurrency

Tasks gehören zu einem definierten übergeordneten Task-Scope.

- Untergeordnete Tasks werden beim Verlassen des Scopes abgeschlossen oder abgebrochen.
- Cancellation wird hierarchisch weitergegeben.
- Fehler werden an den zuständigen Task-Scope gemeldet.
- Verwaiste Tasks dürfen nicht unkontrolliert weiterlaufen.

## Ressourcenverwaltung

Der Scheduler berücksichtigt:

- Maximale Anzahl gleichzeitig ausführbarer Tasks
- CPU- und Laufzeitbudgets
- Speicherverbrauch
- Deadlines und Cancellation
- Prioritäten des NovaOS-Ausführungskontexts

Überschrittene Limits müssen kontrolliert behandelt werden.

## Deterministischer Modus

Für Tests, Simulationen und Logic Graph unterstützt NovaLang einen deterministischen Scheduler.

Dabei werden Task-Reihenfolge, Fortsetzungspunkte und externe Ereignisse nach definierten Regeln verarbeitet.

Parallelität darf in diesem Modus eingeschränkt werden, um reproduzierbare Abläufe sicherzustellen.

## NovaOS-Integration

- Der Runtime-Scheduler verwendet NovaOS-Threads und Scheduling-Schnittstellen.
- Tasks erben die Sicherheits- und Ressourcenbeschränkungen ihres Execution Context.
- Capability-Aufrufe dürfen keine zusätzlichen Scheduling-Privilegien erzeugen.
- UI-nahe Tasks dürfen den Compositor oder die Ereignisverarbeitung nicht blockieren.
- Unterschiedliche Schutzdomänen bleiben durch NovaOS isoliert.

## Normative Anforderungen

1. NovaLang MUSS einen nativen Task Scheduler bereitstellen.
2. Der Scheduler MUSS `Async/Await` und Structured Concurrency unterstützen.
3. Wartende Tasks DÜRFEN keine Ausführungsthreads unnötig blockieren.
4. Fairness, Cancellation und Ressourcenlimits MÜSSEN berücksichtigt werden.
5. Task-Prioritäten MÜSSEN unterstützt werden, ohne Echtzeitgarantien vorzutäuschen.
6. Ein deterministischer Scheduling-Modus MUSS verfügbar sein.
7. Der Runtime-Scheduler MUSS mit dem NovaOS-Kernel-Scheduler zusammenarbeiten.
8. AOT, JIT und Interpreter MÜSSEN dieselben Scheduling-Verträge verwenden.
9. Der Scheduler DARF keine .NET-Runtime voraussetzen.

## Ergebnis

NovaLang erhält einen ressourcenschonenden, prioritätsbewussten Task Scheduler mit Structured Concurrency, effizienter Async-Ausführung und deterministischem Modus, während NovaOS die tatsächliche CPU-Zuteilung kontrolliert.
