
# NPSPEC-NOVALANG-COMPILER-0001 – NovaLang Compiler

## Status

Angenommen

## Kategorie

NovaLang / Compiler

## Zweck

Definiert Architektur und grundlegende Anforderungen des nativen NovaLang-Compilers. Er übersetzt NovaLang-Quellcode in ausführbare Programme oder wiederverwendbare Module, unabhängig von .NET.

## Compilerarchitektur

Der Compiler verwendet eine modulare Verarbeitungskette:

1. **Lexer:** Zerlegung des Quellcodes in Tokens.
2. **Parser:** Erstellung des Abstract Syntax Tree (AST).
3. **Semantic Analyzer:** Namensauflösung, Typprüfung und Vertragsvalidierung.
4. **Nova IR:** Plattformunabhängige Zwischenrepräsentation.
5. **Optimizer:** Sichere Optimierung des Programms.
6. **Backend:** Erzeugung des Zielcodes.
7. **Linker:** Verbindung von Modulen und Abhängigkeiten.

Frontend, Optimizer und Backends müssen unabhängig weiterentwickelt werden können.

## Eingabe und Ausgabe

| Eingabe | Verwendung |
|---|---|
| `.nova` | Allgemeiner NovaLang-Quellcode |
| `.nlf` | NovaLang-Code innerhalb von Logic Graph |
| `.nui` | Deklarative Benutzeroberflächen |
| Projektkonfiguration | Zielplattform, Sprachversion, Buildoptionen |

Unterstützte Ausgaben:

- Native ausführbare Programme
- Native Bibliotheken und Module
- NovaOS-Solution-Komponenten
- Plattformunabhängige Nova IR
- Debug- und Metadaten

## Nova IR

Nova IR bildet die gemeinsame Grundlage für Optimierung und Codegenerierung.

- Typisierte Operationen und expliziter Kontrollfluss
- Definierte Speicher- und Objektsemantik
- Asynchrone Operationen und Fehlerbehandlung
- Capability-Aufrufe mit überprüfbaren Verträgen
- Versionierte und validierbare IR-Struktur

Nova IR darf keine unautorisierten Systemzugriffe ermöglichen.

## Optimierung

Der Compiler unterstützt mehrere Optimierungsstufen.

| Stufe | Verwendung |
|---|---|
| `O0` | Debugging, minimale Optimierung |
| `O1` | Schnelle, grundlegende Optimierung |
| `O2` | Standardoptimierung |
| `O3` | Aggressive Performanceoptimierung |
| `Os` | Optimierung der Programmgröße |

Optimierungen dürfen beobachtbare Semantik, Speichersicherheit, Capability-Prüfungen und definierte Ausführungsreihenfolgen nicht verletzen.

Aggressive Optimierungen können Spezialisierung, Inlining, Vektorisierung und Interprocedural Optimization verwenden.

## Backends

Der Compiler muss eine erweiterbare Backend-Architektur besitzen.

Vorgesehene Ziele:

- x86 und x86-64
- ARM64
- NovaOS-native Ausführungsformate
- Weitere Architekturen über zusätzliche Backends

Die Unterstützung einzelner Zielarchitekturen wird separat versioniert.

## Incremental Compilation

Der Compiler soll inkrementelle und parallele Übersetzung unterstützen.

Unveränderte Module dürfen aus einem validierten Build-Cache übernommen werden.

Abhängigkeiten und öffentliche Schnittstellen müssen für die Änderungsanalyse berücksichtigt werden.

## Diagnostik

Compilerfehler müssen mindestens Fehlercode, Beschreibung und Quelltextposition enthalten.

NovaLang Studio muss strukturierte Diagnosen, Warnungen und Korrekturhinweise erhalten können.

## Normative Anforderungen

1. Der Compiler MUSS NovaLang unabhängig von .NET übersetzen.
2. `.nova`, `.nlf` und `.nui` MÜSSEN denselben Sprachkern verwenden.
3. Frontend, Nova IR, Optimizer und Backends MÜSSEN modular getrennt sein.
4. Nova IR MUSS typisiert, versioniert und validierbar sein.
5. Optimierungen MÜSSEN die definierte Sprachsemantik erhalten.
6. Sicherheits- und Capability-Prüfungen DÜRFEN nicht wegoptimiert werden.
7. Der Compiler MUSS mehrere Zielarchitekturen unterstützen können.
8. Reproduzierbare Builds und strukturierte Diagnosen MÜSSEN unterstützt werden.
9. Inkrementelle und parallele Übersetzung SOLLEN unterstützt werden.

## Ergebnis

NovaLang erhält einen eigenständigen, modularen und hochoptimierenden nativen Compiler mit gemeinsamer Nova IR, erweiterbaren Backends und vollständiger Integration in NovaOS und NovaLang Studio.
