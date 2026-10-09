
# NPSPEC-SOLUTION-LOGIC-RUNTIME-0001 – NovaOS Solution Logic Runtime

## Status

Angenommen

## Kategorie

NovaOS / Solutions / Logic Runtime

## Zweck

Definiert die Ausführung von Logic Graphs innerhalb einer NovaOS-Solution.

Ziel ist eine sichere, effiziente und deterministisch kontrollierbare Verarbeitung von Capabilities, Custom Scripts, Ereignissen und Datenflüssen.

## Architektur

Die Solution Logic Runtime besteht aus:

- **Runtime Manager:** Verwaltung der Logic-Graph-Ausführungen.
- **Graph Loader:** Laden und Validieren der Graphdefinition.
- **Execution Engine:** Ausführung von Knoten und Verbindungen.
- **Graph Scheduler:** Koordination paralleler und asynchroner Aufgaben.
- **State Manager:** Verwaltung isolierter Laufzeitzustände.
- **Capability Bridge:** Kontrollierter Zugriff auf autorisierte Systemfähigkeiten.
- **NovaLang Runtime Bridge:** Ausführung von Custom Scripts.
- **Error Controller:** Fehlerbehandlung und kontrollierter Abbruch.

Die Runtime verwendet die bestehenden NovaOS- und NovaLang-Systemdienste.

## Ausführungsmodell

Beim Start einer Solution werden:

1. Solution-Identität und Integrität geprüft.
2. Graphdefinition und Abhängigkeiten geladen.
3. Knoten, Ports und Verbindungen validiert.
4. Capability-Anforderungen mit den Berechtigungen abgeglichen.
5. Laufzeitzustände und Ressourcenbudgets initialisiert.
6. Der Graph gemäß seinen Startbedingungen ausgeführt.

Die Runtime unterstützt synchrone, asynchrone, parallele und ereignisgesteuerte Ausführung.

## Capability-Integration

Systemzugriffe erfolgen ausschließlich über autorisierte Capability Nodes.

Custom Scripts dürfen keine Systemfähigkeiten selbstständig anfordern oder aufrufen.

Sie erhalten ausschließlich die durch den Logic Graph bereitgestellten Daten und zulässigen Ressourcenreferenzen.

Die zentrale NovaOS-Berechtigungsverwaltung bleibt maßgeblich.

## NovaLang-Integration

Custom Scripts verwenden die reguläre NovaLang Runtime.

`.nlf` enthält unveränderte NovaLang-Syntax und keinen eigenen Sprachdialekt.

Script-Ausführungen unterliegen denselben Typ-, Speicher-, Isolations- und Ressourcenregeln wie andere NovaLang-Ausführungen.

## Zustände und Transaktionen

Jede Solution besitzt einen isolierten Laufzeitkontext.

Graphzustände werden entsprechend ihrer definierten Gültigkeitsbereiche verwaltet.

Transaktionale Änderungen verwenden das bestehende Logic Graph Transaction-System.

Persistente Zustände benötigen autorisierte Speicherfähigkeiten.

## Fehler und Ressourcen

Fehler werden über definierte Fehlerpfade behandelt.

Nicht behandelte Fehler führen zum kontrollierten Abbruch des betroffenen Ausführungskontexts.

CPU-Zeit, Speicher, Parallelität und Ausführungsdauer unterliegen den festgelegten Ressourcenlimits.

## Lifecycle

Unterstützte Runtime-Zustände:

- **Created:** Ausführungskontext erstellt.
- **Ready:** Initialisierung abgeschlossen.
- **Running:** Graph wird ausgeführt.
- **Waiting:** Warten auf Ereignisse oder Ergebnisse.
- **Suspended:** Ausführung kontrolliert angehalten.
- **Completed:** Ausführung erfolgreich abgeschlossen.
- **Failed:** Ausführung fehlgeschlagen.
- **Cancelled:** Ausführung abgebrochen.

Zustandsübergänge müssen kontrolliert und nachvollziehbar erfolgen.

## Normative Anforderungen

1. Jede Solution MUSS einen isolierten Logic-Runtime-Kontext besitzen.
2. Graphdefinitionen MÜSSEN vor ihrer Ausführung validiert werden.
3. Solution-Identität und Integrität MÜSSEN geprüft werden.
4. Capability-Zugriffe MÜSSEN über autorisierte Capability Nodes erfolgen.
5. Custom Scripts DÜRFEN keine eigenständigen Systemzugriffe durchführen.
6. Die Runtime MUSS synchrone und asynchrone Ausführung unterstützen.
7. Parallele Aufgaben MÜSSEN den Graph-Scheduling-Regeln folgen.
8. Zustände MÜSSEN zwischen Solutions isoliert bleiben.
9. Transaktionen MÜSSEN die definierten Konsistenzregeln einhalten.
10. Ressourcenlimits MÜSSEN durchgesetzt werden.
11. Fehler und Abbrüche MÜSSEN kontrolliert behandelt werden.
12. Laufzeitzustände und Fehler MÜSSEN diagnostizierbar sein.
13. Die Runtime MUSS ohne grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine eigenständige, sichere und ressourcenkontrollierte Logic Runtime zur Ausführung von Solutions aus Capabilities, Logic Graphs und NovaLang-Scripts.
