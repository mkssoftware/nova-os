
# NPSPEC-NOVALANG-AOT-0001 – NovaLang Ahead-of-Time Compiler

## Status

Angenommen

## Kategorie

NovaLang / Compiler / AOT

## Zweck

Definiert die Ahead-of-Time-Kompilierung (AOT) von NovaLang. Sie übersetzt Programme vor ihrer Ausführung vollständig in nativen Maschinencode.

Ziel sind maximale Ausführungsgeschwindigkeit, kurze Startzeiten und geringer Laufzeit-Overhead ohne JIT-Abhängigkeit.

## Architektur

Der AOT-Compiler verwendet die gemeinsame Nova IR und die nativen Compiler-Backends.

Kompilierungsablauf:

1. Quellcode analysieren und typprüfen.
2. Nova IR erzeugen und verifizieren.
3. Plattformunabhängige Optimierungen durchführen.
4. Zielarchitekturspezifische Optimierungen anwenden.
5. Nativen Maschinencode erzeugen.
6. Module und benötigte Runtime-Komponenten linken.
7. Ausführbare Datei oder Bibliothek erstellen.

## Ausgabeformate

| Ausgabe | Verwendung |
|---|---|
| Executable | Native Anwendungen |
| Shared Library | Dynamisch ladbare Module |
| Static Library | Statisch eingebundene Komponenten |
| Solution Module | Native Solution-Komponenten |
| System Module | NovaOS-Systemkomponenten |

Binärformate und Aufrufkonventionen richten sich nach der jeweiligen Zielplattform.

## Kompilierungsmodi

- **Debug:** Debugsymbole und minimale Optimierung.
- **Release:** Optimierte native Ausführung.
- **Size:** Minimale Binärgröße.
- **Performance:** Aggressive Optimierung.
- **LTO:** Modulübergreifende Link-Time Optimization.

Nicht benötigte Funktionen und Bibliothekskomponenten sollen durch Dead Code Elimination entfernt werden.

## Runtime-Integration

AOT-Programme verwenden ausschließlich die tatsächlich benötigten NovaLang-Runtime-Dienste.

- Speicherverwaltung und Objektmodell bleiben kompatibel.
- Exceptions und Async/Await werden unterstützt.
- Reflection benötigt verfügbare Metadaten.
- Dynamische Module müssen kompatible ABI-Verträge besitzen.
- Garbage Collection darf bei Bedarf eingebunden werden.

AOT bedeutet nicht automatisch, dass sämtliche Runtime-Komponenten entfallen.

## Generics und dynamische Funktionen

Generische Typen und Funktionen dürfen spezialisiert kompiliert werden.

Dynamische Aufrufe, Reflection und spät geladene Module benötigen explizit verfügbare Metadaten und Laufzeitunterstützung.

Nicht statisch auflösbare Abhängigkeiten müssen diagnostiziert oder über definierte Laufzeitmechanismen behandelt werden.

## NovaOS-Integration

- Native Programme unterliegen dem NovaOS-Capability-Modell.
- AOT-Code darf keine Berechtigungsprüfungen umgehen.
- Logic-Graph-Skripte dürfen als native Module kompiliert werden.
- Systemmodule können ohne Interpreter oder JIT ausgeführt werden.
- Ressourcenlimits und Isolation bleiben durch NovaOS durchsetzbar.

## Portabilität

AOT unterstützt Cross-Compilation für unterschiedliche Prozessorarchitekturen.

Vorgesehene Ziele sind x86, x86-64 und ARM64.

Jedes erzeugte Binary muss seine Zielarchitektur, ABI-Version und benötigten Runtime-Merkmale eindeutig festlegen.

## Normative Anforderungen

1. NovaLang MUSS native AOT-Kompilierung unterstützen.
2. AOT MUSS die gemeinsame, verifizierte Nova IR verwenden.
3. Erzeugte Programme MÜSSEN ohne Interpreter und JIT ausführbar sein.
4. Die NovaLang-Sprachsemantik MUSS vollständig erhalten bleiben.
5. Runtime-Komponenten MÜSSEN bedarfsgerecht eingebunden werden können.
6. Speicher- und Capability-Sicherheitsregeln MÜSSEN eingehalten werden.
7. Debug-, Release- und größenoptimierte Builds MÜSSEN unterstützt werden.
8. Cross-Compilation und reproduzierbare Builds MÜSSEN möglich sein.
9. AOT DARF keine .NET-Runtime voraussetzen.

## Ergebnis

NovaLang erhält einen nativen AOT-Compiler für schnelle, ressourcenschonende und eigenständig ausführbare Programme, Bibliotheken und NovaOS-Systemkomponenten ohne Interpreter- oder JIT-Abhängigkeit.
