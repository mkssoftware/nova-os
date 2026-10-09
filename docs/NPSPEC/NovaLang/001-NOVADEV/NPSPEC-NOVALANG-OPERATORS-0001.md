
# NPSPEC-NOVALANG-OPERATORS-0001 – NovaLang Operators

## Status

Angenommen

## Kategorie

NovaLang / Operatoren

## Zweck

Definiert die Operatoren, ihre Priorität und Auswertungsregeln. NovaLang orientiert sich möglichst exakt an VB.NET und erweitert diese Regeln nur bei ausdrücklich spezifizierten NovaOS-Anforderungen.

## Operatoren

| Kategorie | Operatoren |
|---|---|
| Arithmetik | `+`, `-`, `*`, `/`, `\`, `Mod`, `^` |
| Verkettung | `&` |
| Vergleich | `=`, `<>`, `<`, `<=`, `>`, `>=` |
| Referenzidentität | `Is`, `IsNot` |
| Mustervergleich | `Like` |
| Logik | `Not`, `And`, `AndAlso`, `Or`, `OrElse`, `Xor` |
| Bitverschiebung | `<<`, `>>` |
| Zuweisung | `=`, `+=`, `-=`, `*=`, `/=`, `\=`, `^=`, `&=`, `<<=`, `>>=` |
| Typprüfung | `TypeOf ... Is`, `TypeOf ... IsNot` |
| Nullable-Zugriff | `?.`, `?()` |
| Nullable-Fallback | `If(value, fallback)` |

## Operatorpriorität

Von höchster zu niedrigster Priorität:

1. Potenzierung `^`
2. Unäre Operatoren `+`, `-`
3. Multiplikation `*`, `/`
4. Ganzzahldivision `\`
5. Modulo `Mod`
6. Addition und Subtraktion `+`, `-`
7. Verkettung `&`
8. Bitverschiebung `<<`, `>>`
9. Vergleichsoperatoren, `Is`, `IsNot`, `Like`, `TypeOf`
10. `Not`
11. `And`, `AndAlso`
12. `Or`, `OrElse`
13. `Xor`

Klammern `()` überschreiben die reguläre Priorität. Assoziativität und Sonderfälle folgen der formalen NovaLang-Grammatik.

## Auswertungsregeln

- Operanden werden grundsätzlich von links nach rechts ausgewertet.
- `AndAlso` und `OrElse` verwenden Kurzschlussauswertung.
- `And`, `Or` und `Xor` unterstützen die definierten logischen beziehungsweise bitweisen Operationen.
- `Is` und `IsNot` vergleichen Referenzidentitäten.
- `=` und `<>` verwenden die definierte Wertgleichheit.
- Zusammengesetzte Zuweisungen werten ihre Zielreferenz nur einmal aus.
- Ganzzahlüberläufe und ungültige Operationen werden im sicheren Standardmodus kontrolliert behandelt.
- Operatorüberladung ist für zulässige benutzerdefinierte Typen möglich und muss statisch überprüfbar sein.

## Beispiel

```vb
Dim a As Integer = 10
Dim b As Integer = 3

Dim summe As Integer = a + b
Dim rest As Integer = a Mod b
Dim gueltig As Boolean = a > b AndAlso b > 0

a += 5

If gueltig Then
    Console.WriteLine("Gültig")
End If
```

## Normative Anforderungen

1. NovaLang MUSS die definierten VB.NET-orientierten Operatoren unterstützen.
2. Operatorpriorität und Assoziativität MÜSSEN eindeutig festgelegt sein.
3. Operatoren MÜSSEN statisch typgeprüft werden.
4. Kurzschlussauswertung MUSS garantiert sein.
5. Operatorüberladungen DÜRFEN die Typ- und Speichersicherheit nicht umgehen.
6. Compiler und Interpreter MÜSSEN identische beobachtbare Operatorsemantik gewährleisten.
7. `.nova`, `.nlf` und `.nui` MÜSSEN dieselben Operatorregeln verwenden.

## Ergebnis

NovaLang besitzt ein einheitliches, VB.NET-orientiertes Operatorsystem mit definierter Priorität, statischer Typprüfung und sicherer Auswertung.
