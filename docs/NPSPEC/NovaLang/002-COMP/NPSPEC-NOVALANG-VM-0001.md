
# NPSPEC-NOVALANG-VM-0001 – NovaLang Virtual Machine

## Status

Angenommen

## Kategorie

NovaLang / Runtime / Virtual Machine

## Zweck

Definiert die NovaLang Virtual Machine (Nova VM) als native Ausführungsumgebung für verifizierten Nova Bytecode.

Sie ermöglicht plattformunabhängige, sichere und ressourcenschonende Programmausführung ohne .NET-Abhängigkeit.

## Architektur

Die Nova VM ist Bestandteil der NovaLang Runtime und verwendet eine typisierte, registerbasierte Architektur.

Kernkomponenten:

| Komponente | Aufgabe |
|---|---|
| Bytecode Loader | Laden und Auflösen von Modulen |
| Bytecode Verifier | Prüfung vor der Ausführung |
| Interpreter | Ausführung virtueller Instruktionen |
| Execution Context | Register, Stack und Aufrufzustand |
| Memory Interface | Zugriff auf verwaltete Objekte |
| Task Interface | Async/Await und Structured Concurrency |
| Capability Bridge | Kontrollierte NovaOS-Systemzugriffe |
| JIT Interface | Optionale native Codegenerierung |

Die VM darf ohne JIT vollständig funktionsfähig sein.

## Ausführungsmodell

- Jede Ausführung besitzt einen isolierten Execution Context.
- Funktionen verwenden virtuelle Register und definierte Aufrufrahmen.
- Kontrollfluss erfolgt über validierte Sprungziele.
- Objekte werden über die gemeinsame Runtime-Speicherverwaltung verwaltet.
- Exceptions und Async/Await folgen der NovaLang-Sprachsemantik.
- Die VM unterstützt kontrollierte Unterbrechung und Fortsetzung.

## Bytecode-Ausführung

Ablauf:

1. Bytecode-Modul laden.
2. Formatversion und Abhängigkeiten prüfen.
3. Bytecode vollständig verifizieren.
4. Execution Context initialisieren.
5. Instruktionen interpretieren oder optional JIT-kompilieren.
6. Ressourcen und Abbruchbedingungen überwachen.
7. Ausführung kontrolliert beenden.

Nicht verifizierter Bytecode darf niemals ausgeführt werden.

## Isolation und Sicherheit

Die VM besitzt keinen uneingeschränkten Zugriff auf NovaOS.

- Systemoperationen erfolgen ausschließlich über autorisierte Capability-Handles.
- Bytecode darf keine beliebigen nativen Speicheradressen verwenden.
- Modul- und Ausführungskontexte müssen voneinander isolierbar sein.
- CPU-, Speicher- und Ausführungslimits müssen durchsetzbar sein.
- Fehlerhafte Module dürfen die Runtime oder andere Ausführungskontexte nicht kompromittieren.

Die tatsächliche Schutzgrenze zwischen unterschiedlichen Vertrauensbereichen wird durch NovaOS-Isolationsmechanismen abgesichert.

## Performance

Die VM unterstützt:

- Effiziente Registerausführung
- Schnelle Funktionsaufrufe
- Wiederverwendung validierter Module
- Lazy Loading von Abhängigkeiten
- Optionales JIT und Profiling
- Minimale Runtime-Konfiguration für schwache Hardware

Optimierungen dürfen die definierte Programmsemantik nicht verändern.

## Deterministische Ausführung

Die VM muss einen deterministischen Ausführungsmodus ermöglichen.

Dabei werden nichtdeterministische Einflüsse kontrolliert oder über definierte Schnittstellen bereitgestellt.

Dies unterstützt Tests, Simulationen und reproduzierbare Logic-Graph-Ausführungen.

## Normative Anforderungen

1. Die Nova VM MUSS verifizierten Nova Bytecode ausführen können.
2. Die VM MUSS eine typisierte, registerbasierte Architektur verwenden.
3. Der Interpreter MUSS ohne JIT funktionsfähig sein.
4. Bytecode MUSS vor der Ausführung vollständig verifiziert werden.
5. Speicherzugriffe MÜSSEN den NovaLang-Sicherheitsregeln entsprechen.
6. Systemzugriffe MÜSSEN gültige Capability-Handles erfordern.
7. Ressourcenlimits, Cancellation und kontrollierte Beendigung MÜSSEN unterstützt werden.
8. Die VM MUSS mit der gemeinsamen NovaLang Runtime zusammenarbeiten.
9. Ein deterministischer Ausführungsmodus MUSS unterstützt werden.
10. Die Nova VM DARF keine .NET-Runtime voraussetzen.

## Ergebnis

NovaLang erhält eine kompakte, sichere und plattformunabhängige virtuelle Maschine mit registerbasierter Bytecode-Ausführung, optionalem JIT, kontrollierter Ressourcenverwaltung und vollständiger Integration in NovaOS.
