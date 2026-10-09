
# NPSPEC-NOVALANG-NULLABILITY-0001 – NovaLang Nullability

## Status

Angenommen

## Kategorie

NovaLang / Nullability

## Zweck

Definiert den Umgang mit `Nothing`, nullable Typen und sicheren Zugriffen auf möglicherweise fehlende Werte. Ziel ist die Vermeidung ungültiger Referenzzugriffe bei möglichst VB.NET-kompatibler Syntax.

## Nullable-Typen

Nullable-Werttypen verwenden `?` oder `Nullable(Of T)`.

```vb
Dim alter As Integer? = Nothing
Dim temperatur As Nullable(Of Double) = 21.5
```

Referenztypen sind standardmäßig nicht-nullbar. Nullable-Referenzen werden ebenfalls mit `?` gekennzeichnet.

```vb
Dim name As String = "NovaOS"
Dim optionalName As String? = Nothing
```

Die Nullable-Referenzsyntax ist eine bewusste Erweiterung gegenüber VB.NET.

## Nothing

`Nothing` bezeichnet einen fehlenden Wert beziehungsweise eine fehlende Objektreferenz.

- Nullable-Typen dürfen `Nothing` enthalten.
- Nicht-nullbare Referenzen müssen gültig initialisiert sein.
- Ungültige Zuweisungen werden statisch erkannt.
- Unsichere Zugriffe werden durch Compiler und Runtime verhindert.

## Sichere Zugriffe

| Syntax | Bedeutung |
|---|---|
| `obj?.Member` | Bedingter Memberzugriff |
| `obj?(index)` | Bedingter Indexzugriff |
| `If(value, fallback)` | Ersatzwert bei `Nothing` |
| `Is Nothing` | Prüfung auf fehlende Referenz |
| `IsNot Nothing` | Prüfung auf vorhandene Referenz |
| `HasValue` | Prüfung eines Nullable-Werttyps |
| `Value` | Enthaltener Wert eines Nullable-Werttyps |

```vb
Dim name As String = If(person?.Name, "Unbekannt")

If person IsNot Nothing Then
    Console.WriteLine(person.Name)
End If
```

## Statische Nullability-Analyse

Der Compiler verfolgt den möglichen Nullzustand von Variablen und Ausdrücken.

```vb
Dim person As Person? = Laden()

If person IsNot Nothing Then
    person.Begruessen()
End If
```

Nach einer gültigen Prüfung darf der Compiler den Typ innerhalb des sicheren Kontrollflusses als nicht-nullbar behandeln.

Bei veränderlichem gemeinsamem Zustand muss die Gültigkeit der Prüfung gewährleistet bleiben.

## Nullable-Konvertierungen

- `T` darf implizit nach `T?` konvertiert werden.
- `T?` darf nur nach gültiger Prüfung oder expliziter kontrollierter Konvertierung nach `T` überführt werden.
- Nullable-Werttypen unterstützen die definierten angehobenen Operatoren.
- Vergleiche und Operationen mit `Nothing` folgen der festgelegten Typsemantik.

## Laufzeitverhalten

Ein nicht statisch ausschließbarer ungültiger Nullable-Zugriff muss kontrolliert fehlschlagen.

Die Runtime darf niemals ungültige Speicherreferenzen dereferenzieren.

Nullable-Informationen müssen für Typprüfung, Debugging und erforderliche Laufzeitverträge verfügbar bleiben.

## Normative Anforderungen

1. NovaLang MUSS nullable Wert- und Referenztypen unterstützen.
2. `?` MUSS nullable Typen kennzeichnen.
3. Nicht-nullbare Referenzen DÜRFEN keinen ungültigen `Nothing`-Zustand besitzen.
4. Der Compiler MUSS kontrollflussbasierte Nullability-Analyse durchführen.
5. Bedingte Zugriffe und Nullable-Fallback MÜSSEN unterstützt werden.
6. Ungültige Nullable-Zugriffe MÜSSEN sicher verhindert oder kontrolliert behandelt werden.
7. `.nova`, `.nlf` und `.nui` MÜSSEN dieselben Nullability-Regeln verwenden.

## Ergebnis

NovaLang kombiniert VB.NET-orientierte Nullable-Mechanismen mit statisch geprüften, nicht-nullbaren Referenzen und kontrollflussbasierter Nullability-Analyse.
