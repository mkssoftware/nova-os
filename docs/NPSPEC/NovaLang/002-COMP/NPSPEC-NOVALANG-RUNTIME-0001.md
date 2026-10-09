
# NPSPEC-NOVALANG-RUNTIME-0001 – NovaLang Runtime

## Status

Angenommen

## Kategorie

NovaLang / Laufzeitumgebung

## Zweck

Definiert die native NovaLang Runtime als Ausführungsumgebung für NovaLang-Programme, Module und Solutions.

Sie stellt grundlegende Laufzeitdienste bereit, arbeitet unabhängig von .NET und integriert sich in die Sicherheits- und Ressourcenverwaltung von NovaOS.

## Architektur

Die Runtime ist modular aufgebaut und unterstützt:

- Native AOT-kompilierte Programme
- Nova Bytecode über Interpreter
- JIT-kompilierte Ausführung
- Gemeinsame Typ- und Objektsemantik
- Kontrollierte Ausführung von Solution-Komponenten

Nicht benötigte Runtime-Komponenten müssen nicht geladen werden.

## Kernkomponenten

| Komponente | Aufgabe |
|---|---|
| Execution Engine | Ausführung und Methodenaufrufe |
| Type System | Laufzeittypen und Typprüfung |
| Memory Manager | Objektallokation und Speicherverwaltung |
| Exception Runtime | Fehlerbehandlung und Stack-Unwinding |
| Task Runtime | Async/Await und Structured Concurrency |
| Module Loader | Laden und Verknüpfen von Modulen |
| Metadata Runtime | Kontrollierter Zugriff auf Typmetadaten |
| Capability Bridge | Autorisierte NovaOS-Systemzugriffe |
| Diagnostics | Laufzeitfehler, Tracing und Debugging |

## Speicherverwaltung

Die Runtime verwendet eine sichere, automatisierte Speicherverwaltung für verwaltete Objekte.

- Objektlebenszeiten werden automatisch verwaltet.
- Speicherfreigaben dürfen keine gültigen Referenzen beschädigen.
- Ressourcen wie Dateien und Handles werden deterministisch über `Using` beziehungsweise `Dispose` freigegeben.
- Speicherlimits werden durch NovaOS durchgesetzt.
- Garbage Collection darf die Ausführung sicherheitskritischer Bereiche nicht unkontrolliert unterbrechen.

Die konkrete Garbage-Collection-Strategie wird separat spezifiziert.

## Ausführung und Nebenläufigkeit

- `Async/Await` wird durch die Task Runtime unterstützt.
- Tasks folgen dem Structured-Concurrency-Modell.
- Abbruch und Ressourcenlimits müssen kontrolliert durchsetzbar sein.
- Fehler werden über Exceptions oder `Result(Of T, E)` behandelt.
- Die Runtime muss definierte deterministische Ausführungsmodi unterstützen.

## Module und Versionierung

Module werden anhand ihrer Identität, Version und Abhängigkeiten geladen.

Die Runtime prüft die Kompatibilität von Modulformat, ABI und benötigten Laufzeitfunktionen.

Unvertrauenswürdige Module dürfen keine privilegierten Laufzeitfunktionen direkt aufrufen.

## NovaOS-Integration

Die Runtime nutzt NovaOS-Dienste ausschließlich über definierte Schnittstellen.

- Systemzugriffe erfordern gültige Capability-Handles.
- `Imports` und Metadaten erteilen keine Berechtigungen.
- Logic-Graph-Skripte erhalten ausschließlich bereitgestellte Capabilities und Daten.
- Speicher-, CPU- und Task-Limits bleiben durchsetzbar.
- Fehlerhafte Komponenten müssen kontrolliert beendet oder isoliert werden können.

Die Runtime darf nicht selbstständig zusätzliche Berechtigungen anfordern, wenn der Ausführungskontext dies untersagt.

## Portabilität

Die Runtime trennt plattformunabhängige Sprachdienste von betriebssystemspezifischen Adaptern.

Dadurch können NovaLang-Programme auch außerhalb von NovaOS ausgeführt werden, sofern eine kompatible Runtime und die benötigten Dienste verfügbar sind.

## Normative Anforderungen

1. Die NovaLang Runtime MUSS unabhängig von .NET funktionieren.
2. AOT-, Interpreter- und JIT-Ausführung MÜSSEN dieselbe definierte Sprachsemantik einhalten.
3. Die Runtime MUSS modular und ressourcenschonend aufgebaut sein.
4. Speicher- und Objektverwaltung MÜSSEN die NovaLang-Sicherheitsregeln einhalten.
5. Async/Await, Exceptions und Structured Concurrency MÜSSEN unterstützt werden.
6. Systemzugriffe MÜSSEN über autorisierte Capabilities erfolgen.
7. Ressourcenlimits und kontrollierter Abbruch MÜSSEN durchsetzbar sein.
8. Module MÜSSEN vor dem Laden auf Kompatibilität geprüft werden.
9. `.nova`, `.nlf` und `.nui` MÜSSEN dieselbe grundlegende Laufzeitsemantik verwenden.

## Ergebnis

NovaLang erhält eine native, modulare und sichere Runtime für AOT, Interpreter und JIT, mit automatischer Speicherverwaltung, Structured Concurrency und direkter Integration in das Capability- und Ressourcenmodell von NovaOS.
