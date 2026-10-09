
# NPSPEC-NOVALANG-EXPRESSIONS-0001 – NovaLang Expressions

## Status

Angenommen

## Kategorie

NovaLang / Ausdrücke

## Zweck

Definiert Aufbau, Typisierung und Auswertung von Ausdrücken in NovaLang. Die Syntax orientiert sich möglichst exakt an VB.NET und verwendet die native NovaOS-Sprachsemantik.

## Ausdrucksarten

| Kategorie | Beispiele |
|---|---|
| Literale | `42`, `"NovaOS"`, `True`, `Nothing` |
| Variablen | `name`, `counter` |
| Arithmetik | `a + b`, `a * b` |
| Vergleiche | `a > b`, `a Is Nothing` |
| Logik | `aktiviert AndAlso bereit` |
| Zeichenketten | `"Hallo " & name` |
| Memberzugriff | `person.Name` |
| Funktionsaufruf | `Berechnen(a, b)` |
| Indexzugriff | `zahlen(0)` |
| Objekterzeugung | `New Person("Max")` |
| Typkonvertierung | `CInt(wert)`, `CType(objekt, Person)` |
| Bedingter Ausdruck | `If(aktiviert, "Ja", "Nein")` |
| Lambda | `Function(x As Integer) x * 2` |
| Asynchron | `Await LadenAsync()` |
| Nullable-Zugriff | `person?.Name` |
| Objektinitialisierung | `New Person With {.Name = "Max"}` |

## Syntaxregeln

- Jeder Ausdruck besitzt einen statisch bestimmbaren Typ.
- Ausdrücke dürfen beliebig verschachtelt werden, sofern ihre Typen kompatibel sind.
- Klammern `()` steuern die Gruppierung.
- Operatorpriorität und Assoziativität folgen `NPSPEC-NOVALANG-OPERATORS-0001`.
- Funktionsargumente werden grundsätzlich von links nach rechts ausgewertet.
- `If(condition, trueValue, falseValue)` wertet nur den ausgewählten Ergebniszweig aus.
- `If(value, fallback)` verwendet den Ersatzwert nur, wenn der erste Ausdruck `Nothing` ergibt.
- `Await` ist ausschließlich in zulässigen asynchronen Kontexten erlaubt.
- Zuweisungen sind Anweisungen und keine eigenständigen Wertausdrücke.

## Beispiel

```vb
Dim a As Integer = 10
Dim b As Integer = 20

Dim ergebnis As Integer = (a + b) * 2
Dim groesser As Boolean = a > b
Dim meldung As String = If(groesser, "Größer", "Kleiner")
Dim name As String = person?.Name
```

## Auswertungssemantik

Ausdrücke werden entsprechend ihrer definierten Reihenfolge ausgewertet. Compileroptimierungen dürfen beobachtbare Nebenwirkungen nicht verändern.

Reine Ausdrücke sind deterministisch. Ausdrücke mit Funktionsaufrufen, I/O oder Capabilities unterliegen dem jeweiligen Ausführungskontext.

Fehlerhafte Typkonvertierungen, ungültige Zugriffe und numerische Fehler müssen kontrolliert behandelt werden.

## Normative Anforderungen

1. NovaLang MUSS die definierten VB.NET-orientierten Ausdrucksformen unterstützen.
2. Jeder Ausdruck MUSS statisch typprüfbar sein.
3. Auswertungsreihenfolge und Operatorpriorität MÜSSEN eindeutig definiert sein.
4. Bedingte Ausdrücke MÜSSEN Kurzschlussauswertung unterstützen.
5. Ungültige Speicherzugriffe DÜRFEN nicht entstehen.
6. Ausdrücke DÜRFEN keine Capability-Berechtigungen erzeugen.
7. `.nova`, `.nlf` und `.nui` MÜSSEN dieselbe Ausdruckssemantik verwenden.

## Ergebnis

NovaLang besitzt ein einheitliches, VB.NET-orientiertes Ausdruckssystem mit statischer Typprüfung, kontrollierter Auswertung und Unterstützung moderner NovaOS-Typen und Laufzeitmechanismen.
