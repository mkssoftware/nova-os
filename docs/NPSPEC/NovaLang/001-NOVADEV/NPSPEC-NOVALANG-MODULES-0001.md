
# NPSPEC-NOVALANG-MODULES-0001 – NovaLang Modules

## Status

Angenommen

## Kategorie

NovaLang / Module und Namensräume

## Zweck

Definiert die Organisation, Sichtbarkeit, Initialisierung und Verwendung von Modulen und Namensräumen. Die Syntax orientiert sich möglichst exakt an VB.NET.

## Module

Ein `Module` ist ein statischer Programmcontainer für Funktionen, Konstanten, Felder und Eigenschaften.

```vb
Public Module Mathematik

    Public Const Pi As Double = 3.141592653589793

    Public Function Quadrat(x As Double) As Double
        Return x * x
    End Function

End Module
```

Modulmitglieder benötigen keine Objektinstanz. Module können nicht mit `New` instanziiert werden.

## Namensräume

`Namespace` organisiert Typen und Module hierarchisch.

```vb
Namespace Nova.Math

    Public Module Rechner

        Public Function Addieren(a As Integer, b As Integer) As Integer
            Return a + b
        End Function

    End Module

End Namespace
```

Namensräume dienen der logischen Organisation und erzeugen keine eigenständigen Laufzeitinstanzen.

## Imports

`Imports` bindet Namensräume oder Typnamen für die Namensauflösung ein.

```vb
Imports Nova.Math

Dim ergebnis As Integer = Rechner.Addieren(10, 20)
```

Unterstützt werden:

- Normale Imports
- Alias-Imports
- Vollständig qualifizierte Namen

`Imports` lädt keine Module automatisch zur Ausführung und erteilt keine Berechtigungen.

## Sichtbarkeit

| Modifikator | Bedeutung |
|---|---|
| `Public` | Öffentlich zugänglich |
| `Friend` | Innerhalb derselben Assembly zugänglich |
| `Private` | Innerhalb des Moduls zugänglich |

Die Assembly bezeichnet eine logisch zusammengehörige, versionierte Übersetzungseinheit und setzt keine .NET-Runtime voraus.

## Initialisierung und Lebensdauer

- Module werden innerhalb ihres definierten Ausführungskontexts verwaltet.
- Modulfelder werden vor ihrer ersten zulässigen Verwendung initialisiert.
- Die Initialisierung erfolgt höchstens einmal pro Modulinstanz und Ausführungskontext.
- Zirkuläre Initialisierungsabhängigkeiten müssen erkannt oder eindeutig behandelt werden.
- Veränderlicher Modulzustand unterliegt den Nebenläufigkeits- und Isolationsregeln von NovaOS.

## Abhängigkeiten

Module dürfen andere Module über ihre öffentlichen Schnittstellen verwenden.

Abhängigkeiten müssen zur Übersetzungs- beziehungsweise Ladezeit auflösbar sein. Versionierung und binäre Kompatibilität werden durch das NovaOS-Modulsystem geregelt.

Ein Modulimport oder eine Modulreferenz verleiht keine Capability-Berechtigungen.

## Normative Anforderungen

1. NovaLang MUSS `Module`, `Namespace` und `Imports` unterstützen.
2. Module DÜRFEN keine instanziierbaren Klassen darstellen.
3. Modulmitglieder MÜSSEN ohne Objektinstanz aufrufbar sein.
4. Namensauflösung MUSS Groß- und Kleinschreibung ignorieren.
5. Sichtbarkeit und Modulinitialisierung MÜSSEN eindeutig definiert sein.
6. Modulzustände MÜSSEN zwischen isolierten Ausführungskontexten geschützt sein.
7. Modulabhängigkeiten DÜRFEN keine Berechtigungsgrenzen umgehen.
8. `.nova`, `.nlf` und `.nui` MÜSSEN dieselben Modul- und Namensraumregeln verwenden.

## Ergebnis

NovaLang besitzt ein VB.NET-orientiertes Modulsystem zur strukturierten Organisation und Wiederverwendung von Code, mit kontrollierter Initialisierung, Sichtbarkeit und NovaOS-konformer Isolation.
