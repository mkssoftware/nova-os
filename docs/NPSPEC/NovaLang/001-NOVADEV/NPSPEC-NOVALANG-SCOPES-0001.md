
# NPSPEC-NOVALANG-SCOPES-0001 – NovaLang Scopes

## Status

Angenommen

## Kategorie

NovaLang / Gültigkeitsbereiche

## Zweck

Definiert die Sichtbarkeit, Namensauflösung und Lebensdauer von Deklarationen innerhalb verschiedener Gültigkeitsbereiche. Die Regeln orientieren sich möglichst exakt an VB.NET.

## Gültigkeitsbereiche

| Bereich | Bedeutung |
|---|---|
| Global | Projektweite Deklarationen und Namensräume |
| Namespace | Typen und Module innerhalb eines Namensraums |
| Type | Mitglieder einer Klasse, Struktur oder eines Moduls |
| Member | Parameter und lokale Deklarationen einer Funktion |
| Block | Lokale Deklarationen innerhalb einer Kontrollstruktur |
| Lambda | Parameter und erfasste Variablen eines Lambda-Ausdrucks |

Gültigkeitsbereiche sind lexikalisch definiert und können ineinander verschachtelt sein.

## Namensauflösung

Bezeichner werden vom aktuellen Gültigkeitsbereich ausgehend nach außen aufgelöst.

- Lokale Deklarationen besitzen Vorrang vor weiter außen sichtbaren Deklarationen.
- Gleichnamige Deklarationen im selben Bereich sind unzulässig, sofern keine gültige Überladung vorliegt.
- Namensüberschattung folgt den VB.NET-orientierten Regeln.
- Groß- und Kleinschreibung wird ignoriert.
- Mehrdeutige Referenzen verursachen einen Compilerfehler.
- `Me`, `MyBase` und `MyClass` verwenden den jeweiligen Typkontext.

## Blockgültigkeit

```vb
Public Sub Berechnen()

    Dim zahl As Integer = 10

    If zahl > 5 Then
        Dim ergebnis As Integer = zahl * 2
        Console.WriteLine(ergebnis)
    End If

    ' ergebnis ist hier nicht sichtbar.

End Sub
```

Lokale Deklarationen sind nur innerhalb ihres festgelegten lexikalischen Bereichs sichtbar.

## Sichtbarkeitsmodifikatoren

| Modifikator | Zugriff |
|---|---|
| `Public` | Öffentlich |
| `Private` | Innerhalb des deklarierenden Typs |
| `Protected` | Deklarierender Typ und abgeleitete Typen |
| `Friend` | Innerhalb derselben Assembly |
| `Protected Friend` | Assembly oder abgeleitete Typen |
| `Private Protected` | Abgeleitete Typen innerhalb derselben Assembly |

Die Zugriffsberechtigung wird statisch geprüft. Sprachliche Sichtbarkeit ersetzt keine NovaOS-Capability-Autorisierung.

## Lebensdauer

- Lokale Variablen besitzen grundsätzlich die Lebensdauer ihres Ausführungskontexts.
- Objektfelder existieren entsprechend der Lebensdauer ihrer Instanz.
- `Shared`- und `Static`-Variablen besitzen einen definierten erweiterten Lebenszyklus.
- Durch Closures erfasste Variablen bleiben so lange erhalten, wie sie gültig benötigt werden.
- Asynchrone Funktionen erhalten erforderliche lokale Zustände über `Await` hinweg.
- Das Verlassen eines Gültigkeitsbereichs darf keine ungültigen Speicherreferenzen erzeugen.

Sichtbarkeit und Speicherlebensdauer sind voneinander zu unterscheiden.

## Normative Anforderungen

1. NovaLang MUSS lexikalische Gültigkeitsbereiche unterstützen.
2. Namensauflösung MUSS eindeutig und unabhängig von Groß- und Kleinschreibung erfolgen.
3. Deklarationen DÜRFEN nur innerhalb ihrer gültigen Bereiche referenziert werden.
4. Sichtbarkeitsmodifikatoren MÜSSEN statisch geprüft werden.
5. Closures und asynchrone Funktionen MÜSSEN gültige Variablenlebensdauern gewährleisten.
6. Gültigkeitsbereiche DÜRFEN keine Capability- oder Prozessisolation umgehen.
7. `.nova`, `.nlf` und `.nui` MÜSSEN dieselben grundlegenden Scope-Regeln verwenden.

## Ergebnis

NovaLang besitzt ein einheitliches, VB.NET-orientiertes Scope-Modell mit lexikalischer Namensauflösung, kontrollierter Sichtbarkeit und sicherer Variablenlebensdauer.
