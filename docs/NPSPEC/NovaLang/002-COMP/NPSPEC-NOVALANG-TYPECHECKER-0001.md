
# NPSPEC-NOVALANG-TYPECHECKER-0001 – NovaLang Type Checker

## Status

Angenommen

## Kategorie

NovaLang / Compiler / Typprüfung

## Zweck

Definiert die statische Typprüfung von NovaLang. Der Type Checker stellt sicher, dass Ausdrücke, Zuweisungen, Funktionsaufrufe und Typverträge vor der Codegenerierung korrekt sind.

## Architektur

Der Type Checker verarbeitet den AST nach der Namens- und Symbolauflösung.

Er verwendet das NovaLang-Typsystem und stellt seine Ergebnisse der semantischen Analyse sowie der Nova IR zur Verfügung.

Die Typprüfung erfolgt unabhängig von der Zielarchitektur.

## Prüfbereiche

| Bereich | Prüfung |
|---|---|
| Variablen | Deklaration, Zuweisung, Typinferenz |
| Ausdrücke | Operandentypen, Ergebnistypen |
| Funktionen | Parameter, Rückgabewerte, Überladungen |
| Objekte | Vererbung, Interfaces, Memberzugriff |
| Generics | Typargumente und Constraints |
| Nullability | Zulässige Nullwerte und Dereferenzierung |
| Async/Await | Task-Typen und Await-Ausdrücke |
| Pattern Matching | Typkompatibilität und Typverengung |
| Capabilities | Typisierte Schnittstellen und Verträge |

## Typinferenz und Konvertierung

NovaLang verwendet standardmäßig:

- `Option Strict On`
- `Option Infer On`

Beispiel:

```vb
Dim zahl = 42
Dim text As String = "NovaOS"
Dim ergebnis As Integer = zahl + 10
```

Der Type Checker muss Typen aus Initialisierungen und Ausdruckskontexten ableiten können.

Implizite, potenziell verlustbehaftete Konvertierungen sind bei `Option Strict On` unzulässig.

Explizite Konvertierungen müssen auf ihre statische Zulässigkeit geprüft werden.

## Kontrollflussanalyse

Die Typprüfung berücksichtigt den Kontrollfluss für:

- Definitive Zuweisung vor Verwendung
- Nullability und Typverengung
- Erreichbarkeit von Code
- Vollständigkeit erforderlicher Rückgabepfade

Laufzeitabhängige Eigenschaften müssen zusätzlich durch geeignete Laufzeitprüfungen abgesichert werden.

## Fehlerbehandlung

Typfehler werden als strukturierte Compilerdiagnosen ausgegeben.

Jede Diagnose enthält mindestens Fehlercode, Beschreibung und Quelltextposition.

NovaLang Studio darf bei unvollständigem Quellcode mit vorläufigen Typinformationen weiterarbeiten.

## NovaOS-Integration

Capability-Schnittstellen werden anhand ihrer deklarierten Typen und Verträge geprüft.

Der Type Checker darf aus einem gültigen Typ oder `Imports` keine Zugriffsberechtigung ableiten.

Die tatsächliche Autorisierung bleibt Aufgabe des NovaOS-Capability-Systems.

## Normative Anforderungen

1. NovaLang MUSS eine statische Typprüfung vor der Codegenerierung durchführen.
2. Der Type Checker MUSS das definierte NovaLang-Typsystem verwenden.
3. Typinferenz, Überladungsauflösung und Generic-Constraints MÜSSEN geprüft werden.
4. `Option Strict On` und `Option Infer On` MÜSSEN standardmäßig gelten.
5. Ungültige oder mehrdeutige Typoperationen MÜSSEN diagnostiziert werden.
6. Nullability und definitive Zuweisung MÜSSEN kontrollflussabhängig geprüft werden.
7. Typprüfung DARF keine Capability-Autorisierung ersetzen.
8. Fehlerhafte Programme DÜRFEN nicht als gültiger ausführbarer Code übersetzt werden.
9. `.nova`, `.nlf` und `.nui` MÜSSEN dieselben grundlegenden Typregeln verwenden.

## Ergebnis

NovaLang erhält einen statischen, kontrollflussbewussten Type Checker mit Typinferenz, strikter Typprüfung und einheitlicher Integration in Compiler, Nova IR und NovaLang Studio.
