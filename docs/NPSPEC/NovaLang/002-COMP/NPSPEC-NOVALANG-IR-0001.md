
# NPSPEC-NOVALANG-IR-0001 – NovaLang Intermediate Representation

## Status

Angenommen

## Kategorie

NovaLang / Compiler / Intermediate Representation

## Zweck

Definiert Nova IR als typisierte, plattformunabhängige Zwischenrepräsentation zwischen semantischer Analyse, Optimierung und Codegenerierung.

Nova IR ermöglicht die gemeinsame Nutzung unterschiedlicher Compiler-Backends, ohne die NovaLang-Sprachsemantik zu verändern.

## Architektur

Compiler-Pipeline:

1. NovaLang-Quellcode
2. Lexer und Parser
3. AST und semantische Analyse
4. Nova IR
5. Optimierung
6. Zielarchitektur-Backend

Nova IR ist unabhängig von Quellsyntax, Betriebssystem und Prozessorarchitektur.

## IR-Modell

Nova IR verwendet:

- Typisierte Instruktionen
- Funktionen und Module
- Basic Blocks mit expliziten Kontrollflusskanten
- Static Single Assignment (SSA) für temporäre Werte
- Explizite Speicherzugriffe und Seiteneffekte
- Definierte Aufrufkonventionen und Fehlerpfade

Wesentliche Instruktionsgruppen:

| Gruppe | Funktion |
|---|---|
| Arithmetic | Arithmetik und Vergleiche |
| Memory | Speicherzugriff und Objektoperationen |
| Control | Sprünge, Verzweigungen, Rückgaben |
| Call | Funktions- und Methodenaufrufe |
| Object | Objekterzeugung und Memberzugriffe |
| Async | Asynchrone Ausführung und Suspension |
| Exception | Fehlerbehandlung und Unwinding |
| Capability | Autorisierte Systemoperationen |

## Beispiel

NovaLang:

```vb
Public Function Addieren(a As Integer, b As Integer) As Integer
    Return a + b
End Function
```

Vereinfachte Nova IR:

```text
function Addieren(a: i32, b: i32) -> i32
block entry:
    %result = add.checked.i32 %a, %b
    return %result
```

Die Darstellung ist illustrativ. Das verbindliche IR-Format wird separat spezifiziert.

## Typen und Semantik

- IR-Typen müssen eindeutig definiert sein.
- Überläufe, Konvertierungen und Nullability müssen ihre NovaLang-Semantik behalten.
- Speicher- und Objektlebenszeiten müssen abbildbar sein.
- Async/Await und Exceptions müssen explizit repräsentierbar sein.
- Seiteneffekte müssen für Optimierungen erkennbar bleiben.

## Optimierung und Validierung

Nova IR unterstützt unter anderem Constant Folding, Dead Code Elimination, Inlining und Loop Optimization.

Vor der Codegenerierung muss die IR validiert werden.

Der Validator prüft Typkonsistenz, Kontrollfluss, SSA-Regeln, gültige Referenzen und erforderliche Sicherheitsverträge.

Optimierungen dürfen weder beobachtbare Programmsemantik noch Capability- oder Speicherisolationsregeln verletzen.

## Versionierung

Nova IR besitzt eine eigene Formatversion.

Serialisierte IR muss ihre Version und benötigten Laufzeitmerkmale deklarieren. Inkompatible Versionen müssen erkannt und kontrolliert zurückgewiesen werden.

## Normative Anforderungen

1. Nova IR MUSS typisiert und plattformunabhängig sein.
2. Kontrollfluss und Seiteneffekte MÜSSEN explizit dargestellt werden.
3. SSA MUSS für temporäre Werte unterstützt werden.
4. Die vollständige NovaLang-Semantik MUSS abbildbar sein.
5. Nova IR MUSS vor der Codegenerierung validiert werden.
6. Optimierungen DÜRFEN Sicherheitsprüfungen nicht unzulässig entfernen oder umgehen.
7. Capability-Aufrufe DÜRFEN keine zusätzlichen Berechtigungen erzeugen.
8. Nova IR MUSS versionierbar und für mehrere Backends nutzbar sein.

## Ergebnis

Nova IR bildet die einheitliche, sichere und optimierbare Zwischenschicht des NovaLang-Compilers und ermöglicht die unabhängige Entwicklung von Optimierungen, Laufzeitumgebungen und Prozessor-Backends.
