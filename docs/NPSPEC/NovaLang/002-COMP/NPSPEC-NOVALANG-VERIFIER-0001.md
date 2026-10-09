
# NPSPEC-NOVALANG-VERIFIER-0001 – NovaLang Verifier

## Status

Angenommen

## Kategorie

NovaLang / Compiler / Verifikation

## Zweck

Definiert den NovaLang Verifier zur statischen Überprüfung von Nova IR und Nova Bytecode vor Optimierung, Codegenerierung beziehungsweise Ausführung.

Ziel ist die Erkennung ungültiger Programmstrukturen und die Durchsetzung definierter Sicherheitsinvarianten.

## Architektur

Der Verifier besteht aus zwei Prüfkomponenten:

| Komponente | Aufgabe |
|---|---|
| IR Verifier | Prüfung der Nova IR vor und nach Optimierungen |
| Bytecode Verifier | Prüfung von Bytecode vor der Ausführung |

Beide verwenden gemeinsame Typ- und Sicherheitsregeln.

## Prüfbereiche

- **Struktur:** Gültige Module, Funktionen und Instruktionen.
- **Typen:** Typkonsistenz, Signaturen und Konvertierungen.
- **Kontrollfluss:** Gültige Sprungziele und Rückgabepfade.
- **Datenfluss:** Initialisierte Register, SSA-Regeln und Wertverwendung.
- **Speicher:** Zulässige Referenzen, Speicheroperationen und Lebenszeiten.
- **Exceptions:** Korrekte Fehlerpfade und Exception-Handler.
- **Async:** Gültige Suspend-/Resume-Zustände.
- **Capabilities:** Korrekte Aufrufsignaturen und autorisierte Handle-Verwendung.
- **Ressourcen:** Einhaltung statisch prüfbarer Grenzen.

## Verifikationsablauf

1. Eingabeformat und Version prüfen.
2. Struktur und Instruktionen validieren.
3. Typ- und Datenflussanalyse durchführen.
4. Kontrollfluss und Sicherheitsinvarianten prüfen.
5. Ergebnis mit strukturierten Diagnosen ausgeben.
6. Nur erfolgreich verifizierte Eingaben freigeben.

Nach jeder IR-Transformation muss die Gültigkeit der resultierenden IR sichergestellt werden.

## Sicherheitsmodell

Der Verifier verhindert die Ausführung strukturell ungültigen Codes.

Er darf jedoch nicht als vollständiger Beweis für die Fehlerfreiheit eines Programms betrachtet werden.

Insbesondere bleiben folgende Aufgaben getrennt:

- Runtime-Prüfungen für dynamische Speicher- und Typbedingungen
- Capability-Autorisierung durch NovaOS
- Ressourcenüberwachung während der Ausführung
- Formale Verifikation sicherheitskritischer Komponenten

Native Fremdmodule benötigen eigene Vertrauens- und Isolationsmechanismen.

## Fehlerbehandlung

Bei fehlgeschlagener Verifikation werden Fehlercode, Position beziehungsweise Instruktionsreferenz und Ursache ausgegeben.

Ungültige Module dürfen nicht ausgeführt werden. Ein automatisches Überspringen fehlerhafter Instruktionen ist unzulässig.

## Normative Anforderungen

1. Nova IR und Nova Bytecode MÜSSEN vor ihrer Verwendung verifiziert werden.
2. Typen, Kontrollfluss und Datenfluss MÜSSEN geprüft werden.
3. Sicherheitsrelevante Instruktionen MÜSSEN definierte Verträge erfüllen.
4. Ungültige Module DÜRFEN nicht ausgeführt werden.
5. Optimierungen DÜRFEN Verifikationsinvarianten nicht verletzen.
6. Capability-Deklarationen DÜRFEN nicht als Autorisierung gelten.
7. Verifikationsfehler MÜSSEN eindeutig diagnostiziert werden.
8. Der Verifier MUSS unabhängig von der .NET-Runtime funktionieren.

## Ergebnis

NovaLang erhält einen eigenständigen IR- und Bytecode-Verifier, der ungültige Programmstrukturen zuverlässig zurückweist und eine überprüfbare Sicherheitsgrundlage für Compiler, Interpreter und JIT bereitstellt.
