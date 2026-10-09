
# NPSPEC-NOVALANG-BYTECODE-0001 – NovaLang Bytecode

## Status

Angenommen

## Kategorie

NovaLang / Compiler / Bytecode

## Zweck

Definiert Nova Bytecode als kompakte, plattformunabhängige Ausführungsform für NovaLang.

Er ermöglicht die Ausführung durch eine native virtuelle Maschine oder einen JIT-Compiler, ohne eine .NET-Runtime vorauszusetzen.

## Architektur

Nova Bytecode wird aus der validierten Nova IR erzeugt.

Ausführungsmöglichkeiten:

- **Interpreter:** Direkte Ausführung der Bytecode-Instruktionen.
- **JIT:** Übersetzung häufig ausgeführter Abschnitte in nativen Maschinencode.
- **AOT:** Übersetzung in nativen Code vor der Ausführung.

Bytecode ist eine optionale Ausführungsform. NovaLang-Programme dürfen auch direkt aus Nova IR nativ kompiliert werden.

## Bytecode-Format

Ein Bytecode-Modul enthält:

| Bereich | Inhalt |
|---|---|
| Header | Formatversion, Merkmale, Prüfinformationen |
| Types | Typdefinitionen und Signaturen |
| Constants | Konstantenpool |
| Functions | Funktionen und Instruktionen |
| Imports | Externe Modulreferenzen |
| Metadata | Attribute und Laufzeitinformationen |
| Capabilities | Deklarierte Capability-Abhängigkeiten |
| Debug | Optionale Quelltextzuordnungen |

Das Format verwendet eine eindeutig definierte binäre Kodierung.

## Instruktionsmodell

Nova Bytecode verwendet eine typisierte, registerbasierte virtuelle Maschine.

Wesentliche Instruktionsgruppen:

- Laden und Speichern von Werten
- Arithmetik und logische Operationen
- Vergleiche und Kontrollfluss
- Funktions- und Methodenaufrufe
- Objekt- und Speicheroperationen
- Exceptions und Fehlerbehandlung
- Async/Await und Task-Steuerung
- Kontrollierte Capability-Aufrufe

## Beispiel

NovaLang:

```vb
Public Function Addieren(a As Integer, b As Integer) As Integer
    Return a + b
End Function
```

Vereinfachte symbolische Bytecode-Darstellung:

```text
FUNC Addieren(i32, i32) -> i32
    ADD_CHECKED_I32 R2, R0, R1
    RET R2
END
```

Die Darstellung dient der Veranschaulichung und definiert keine endgültigen Opcodes.

## Validierung und Sicherheit

Vor der Ausführung muss der Bytecode-Verifier prüfen:

- Formatversion und strukturelle Integrität
- Gültige Instruktionen und Sprungziele
- Typkonsistenz und Registerinitialisierung
- Speicher- und Objektzugriffsregeln
- Korrekte Funktionssignaturen
- Capability-Aufrufverträge

Ungültiger Bytecode darf nicht ausgeführt werden.

Capability-Deklarationen sind keine Berechtigungen. Tatsächliche Systemzugriffe benötigen autorisierte Capability-Handles.

## Laufzeitverhalten

- Interpreter und JIT müssen dieselbe beobachtbare NovaLang-Semantik einhalten.
- Laufzeitprüfungen dürfen nicht durch Optimierungen umgangen werden.
- Ressourcenlimits und Abbruchmechanismen müssen durchsetzbar bleiben.
- Bytecode-Module müssen unabhängig voneinander ladbar sein.
- Nicht vertrauenswürdiger Bytecode darf keinen direkten Zugriff auf privilegierten Speicher oder Kernel-Funktionen erhalten.

## Versionierung

Nova Bytecode besitzt eine eigene Formatversion, unabhängig von Sprachversion und Nova IR.

Die Runtime muss inkompatible Bytecode-Versionen erkennen und kontrolliert zurückweisen.

## Normative Anforderungen

1. Nova Bytecode MUSS plattformunabhängig und versioniert sein.
2. Bytecode MUSS aus validierter Nova IR erzeugt werden.
3. Das Instruktionsmodell MUSS typisiert und eindeutig definiert sein.
4. Bytecode MUSS vor der Ausführung verifiziert werden.
5. Interpreter, JIT und AOT MÜSSEN die NovaLang-Semantik erhalten.
6. Bytecode DARF keine Capability-Berechtigungen selbst erzeugen.
7. Ungültige oder nicht unterstützte Instruktionen DÜRFEN nicht ausgeführt werden.
8. Die Bytecode-Runtime DARF keine .NET-Abhängigkeit besitzen.

## Ergebnis

NovaLang erhält ein kompaktes, verifizierbares und plattformunabhängiges Bytecode-Format für Interpreter, JIT und AOT, das die native Kompilierung ergänzt und die Sicherheitsarchitektur von NovaOS respektiert.
