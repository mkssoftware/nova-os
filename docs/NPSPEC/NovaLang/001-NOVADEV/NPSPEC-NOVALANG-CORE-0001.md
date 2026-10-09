# NPSPEC-NOVALANG-CORE-0001 – NovaLang Core Language

## Status

Angenommen

## Kategorie

NovaLang / Sprachkern

## Zweck

NovaLang ist die native, universelle Programmiersprache von NovaOS.

Sie verbindet die vertraute, gut lesbare Syntax von Visual Basic .NET (VB.NET) mit einem modernen, plattformunabhängigen Sprach- und Laufzeitmodell.

NovaLang dient zur Entwicklung von:

- Systemkomponenten und Diensten
- Capabilities und deren Implementierungen
- Klassischen Programmen
- Solutions und Custom Scripts
- Logic-Graph-Komponenten
- Deklarativen Benutzeroberflächen
- Automatisierungen und Agentenlogik

Die Sprache soll für Einsteiger verständlich und gleichzeitig leistungsfähig genug für komplexe Systemsoftware sein.

NovaLang ist eine eigenständige Sprache und benötigt weder die .NET-Runtime noch eine Microsoft-Laufzeitumgebung.

## Grundprinzipien

1. **VB.NET als syntaktische Grundlage:** Schlüsselwörter, Deklarationen, Kontrollstrukturen und Blockabschlüsse orientieren sich möglichst exakt an Visual Basic .NET.
2. **Eine gemeinsame Sprache:** Programme, Solutions, Logic Graph und deklarative UI verwenden denselben Sprachkern.
3. **Lesbarkeit:** Verständliche Schlüsselwörter haben Vorrang vor kryptischer Kurzsyntax.
4. **Typsicherheit:** Statische Typprüfung mit kontrollierter Typinferenz.
5. **Speichersicherheit:** Sichere Speicher- und Referenzverwaltung als Standard.
6. **Capability-Sicherheit:** Systemzugriffe erfolgen ausschließlich über autorisierte Fähigkeiten.
7. **Modularität:** Module, Namespaces, Klassen, Strukturen und Interfaces ermöglichen wiederverwendbare Komponenten.
8. **Nebenläufigkeit:** Native Unterstützung für asynchrone Ausführung und Structured Concurrency.
9. **Determinismus:** Reproduzierbare Ausführung muss unterstützt werden können.
10. **Unabhängigkeit:** Compiler und Runtime funktionieren ohne KI, NovaLang Studio oder externe Laufzeitumgebungen.

## Architektur

```text
NovaLang Source
       ↓
Lexer
       ↓
Parser
       ↓
Abstract Syntax Tree
       ↓
Semantic Analysis
       ↓
Type Checking
       ↓
Nova Intermediate Representation
       ↓
Compiler / Interpreter / JIT
       ↓
Nova Runtime
       ↓
NovaOS System Interfaces
       ↓
Authorized Capabilities
```

Die Sprachsemantik bleibt unabhängig vom gewählten Ausführungsbackend.

## Syntaxmodell

NovaLang übernimmt grundsätzlich die Syntaxkonventionen von VB.NET.

```vb
Imports Nova.System

Namespace Beispiel

    Public Class Rechner

        Public Function Addieren(a As Integer, b As Integer) As Integer
            Return a + b
        End Function

        Public Sub Starten()

            Dim ergebnis As Integer = Addieren(10, 20)

            If ergebnis > 20 Then
                Console.WriteLine("Ergebnis ist größer als 20")
            Else
                Console.WriteLine("Ergebnis ist höchstens 20")
            End If

        End Sub

    End Class

End Namespace
```

Wesentliche Syntaxmerkmale:

- `Dim`, `Const` und `As` für Deklarationen.
- `Sub` und `Function` für Prozeduren und Funktionen.
- `If ... Then ... Else ... End If` für Bedingungen.
- `For ... Next`, `For Each ... Next`, `While ... End While` und `Do ... Loop` für Schleifen.
- `Class ... End Class` für Klassen.
- `Structure ... End Structure` für Strukturen.
- `Interface ... End Interface` für Schnittstellen.
- `Namespace ... End Namespace` für Namensräume.
- `Imports` für Modulimporte.
- `'` für einzeilige Kommentare.
- `AndAlso`, `OrElse`, `Not` und `Mod` für logische beziehungsweise arithmetische Operationen.

Geschweifte Klammern sind keine regulären Blockbegrenzer.

Die vollständige Syntax wird durch die NovaLang-Syntax- und Grammatik-Spezifikationen definiert.

## Bezeichner

NovaLang orientiert sich am VB.NET-Modell für Bezeichner.

Groß- und Kleinschreibung unterscheiden keine Bezeichneridentitäten.

```vb
Dim MeineVariable As Integer = 10

meinevariable = 20
```

Beide Schreibweisen bezeichnen dieselbe Variable.

Unicode-Bezeichner werden unterstützt.

Die ursprüngliche Schreibweise bleibt für Quelltextdarstellung und Werkzeuge erhalten.

Sicherheitskritische Systemidentitäten wie `CapabilityID` und `ObjectID` unterliegen dagegen ihren eigenen Identitäts- und Vergleichsregeln.

## Typsystem

NovaLang verwendet ein statisches Typsystem mit kontrollierter Typinferenz.

Grundlegende Typen orientieren sich an VB.NET:

```text
Boolean
Byte
SByte
Short
UShort
Integer
UInteger
Long
ULong
Single
Double
Decimal
Char
String
Object
```

Zusätzlich werden unterstützt:

```text
Structure
Class
Interface
Enum
Array
Tuple
Nullable
Result
Generic Types
Semantic Types
Capability Handles
```

Die konkrete Größe und Repräsentation primitiver Typen wird plattformunabhängig definiert.

## Typinferenz

NovaLang erlaubt die automatische Ermittlung eindeutig bestimmbarer Typen.

```vb
Dim name = "NovaOS"
Dim version = 1
Dim enabled = True
```

Explizite Typangaben bleiben möglich:

```vb
Dim name As String = "NovaOS"
Dim version As Integer = 1
Dim enabled As Boolean = True
```

Typinferenz darf keine unsicheren oder mehrdeutigen Typannahmen erzeugen.

## Variablen und Konstanten

```vb
Dim counter As Integer = 0

Const MaxAttempts As Integer = 5
```

`Dim` deklariert eine Variable.

`Const` deklariert einen konstanten Wert.

Zusätzlich unterstützt NovaLang schreibgeschützte Eigenschaften und Felder.

Die Veränderbarkeit einer Referenz und die Veränderbarkeit ihres Zielobjekts werden getrennt behandelt.

## Funktionen und Prozeduren

```vb
Public Function Multiplizieren(a As Integer, b As Integer) As Integer
    Return a * b
End Function
```

```vb
Public Sub Begruessen(name As String)
    Console.WriteLine("Hallo " & name)
End Sub
```

NovaLang unterstützt:

- Parameter und Rückgabewerte
- Optionale Parameter
- Funktionsüberladung
- Generische Funktionen
- Rekursion
- Delegates und Lambdas
- Asynchrone Funktionen
- Typisierte Fehlerergebnisse

## Objektorientierte Programmierung

NovaLang unterstützt das vertraute VB.NET-Objektmodell.

```vb
Public Class Person

    Public Property Name As String
    Public Property Alter As Integer

    Public Sub New(name As String, alter As Integer)
        Me.Name = name
        Me.Alter = alter
    End Sub

End Class
```

Unterstützte Konzepte:

```text
Class
Structure
Interface
Enum
Inheritance
Encapsulation
Polymorphism
Properties
Constructors
Generics
```

Vererbung, Schnittstellenimplementierung und Sichtbarkeit werden statisch geprüft.

## Generics

```vb
Public Function Identitaet(Of T)(value As T) As T
    Return value
End Function
```

Generische Typen und Funktionen dürfen Constraints besitzen.

```vb
Public Class Container(Of T)
    Public Property Value As T
End Class
```

Generics müssen mit dem NovaOS-Typsystem und den semantischen Typen kompatibel sein.

## Fehlerbehandlung

NovaLang unterstützt VB.NET-nahe strukturierte Fehlerbehandlung.

```vb
Try
    Verarbeitung.Starten()
Catch ex As Exception
    Console.WriteLine(ex.Message)
Finally
    Verarbeitung.Aufraeumen()
End Try
```

Zusätzlich werden typisierte Fehlerergebnisse unterstützt.

```vb
Public Function Laden() As Result(Of String, Error)
    Return Result.Ok("Daten")
End Function
```

Exceptions, Fehlerwerte, Abbruch und Vertragsverletzungen müssen unterscheidbar bleiben.

Fehler dürfen keine Sicherheits- oder Isolationsgrenzen umgehen.

## Asynchrone Programmierung

NovaLang unterstützt `Async` und `Await`.

```vb
Public Async Function DatenLadenAsync() As Task(Of String)

    Dim daten As String = Await Netzwerk.LesenAsync()

    Return daten

End Function
```

Die konkrete Laufzeit basiert auf dem NovaOS-Taskmodell und benötigt keine .NET-Task-Runtime.

Asynchrone Aufgaben unterliegen Structured Concurrency, Abbruchregeln, Deadlines und Ressourcenbudgets.

## Ereignisse

NovaLang unterstützt das VB.NET-Ereignismodell.

```vb
Public Event DataChanged(value As String)

Public Sub Aktualisieren()
    RaiseEvent DataChanged("Aktualisiert")
End Sub
```

Ereignisse können mit typisierten Handlern verbunden werden.

Die Lebensdauer von Ereignisbindungen muss kontrollierbar sein.

## Semantische Typen

NovaLang integriert die semantischen Typen von NovaOS.

```text
ObjectID
CapabilityID
SolutionID
AppID
SemanticTypeID
ResourceHandle
```

Gleiche Speicherrepräsentation bedeutet nicht gleiche Typidentität.

```text
ObjectID ≠ CapabilityID
```

Semantische Typen dürfen nicht durch unkontrollierte Typkonvertierungen ihre Bedeutung verlieren.

## Capability-Integration

NovaLang besitzt keine automatische Berechtigung für Systemzugriffe.

Ein Capability-Aufruf erfolgt ausschließlich über eine autorisierte Schnittstelle.

```vb
Public Function DatenLesen(reader As INetworkReader) As Result(Of String, Error)

    Return reader.Read()

End Function
```

`INetworkReader` ist ein beispielhafter Capability-Schnittstellentyp.

Es gelten folgende Regeln:

- `Imports` erzeugt keine Berechtigungen.
- Eine CapabilityID erzeugt keine Berechtigung.
- Eine Interface-Referenz allein erzeugt keine Berechtigung.
- Capability-Handles werden ausschließlich durch autorisierte Systemmechanismen bereitgestellt.
- Berechtigungen unterliegen dem Execution Contract und der Systempolicy.

## Nova Logic Files

`.nlf` bezeichnet Nova Logic Files.

Eine `.nlf`-Datei verwendet die vollständige NovaLang-Sprachdefinition.

```text
.nlf = NovaLang
```

Es existiert kein separater Skriptdialekt.

Beispiel:

```vb
Public Function Verarbeiten(eingabe As String) As String

    Return eingabe & " verarbeitet"

End Function
```

Der jeweilige Ausführungskontext bestimmt, welche Capabilities verfügbar sind.

## Logic-Graph-Integration

NovaLang ist die Programmiersprache für Custom-Script-Knoten im Logic Graph.

```text
Network Capability
        ↓
Typed Output
        ↓
Custom NovaLang Script
        ↓
Typed Output
        ↓
Storage Capability
```

Custom Scripts dürfen keine zusätzlichen Systemberechtigungen selbst anfordern.

Sie erhalten ausschließlich die Eingaben und autorisierten Schnittstellen, die ihnen durch den Logic Graph bereitgestellt werden.

Alle Verbindungen werden anhand des NovaLang-Typsystems überprüft.

## Deklarative Benutzeroberflächen

NovaLang unterstützt deklarative UI-Definitionen als Bestandteil derselben Sprache.

`.nui` verwendet die deklarative NovaLang-Syntax und keine eigenständige Sprache.

Die konkrete VB.NET-orientierte Deklarationsform wird durch eine separate UI-Syntax-Spezifikation festgelegt.

Dabei gelten:

- Gemeinsame Typregeln mit NovaLang.
- Gemeinsame Ausdrücke und Bezeichner.
- Typisierte Eigenschaften und Bindungen.
- Deklarative Ereignisverbindungen.
- Keine impliziten Systemberechtigungen.
- Deterministische Interpretation ohne KI.

Generierte Oberflächen müssen als versionierte, portable Artefakte rekonstruierbar sein.

## Ausführungsmodelle

NovaLang unterstützt mehrere Ausführungsmodelle:

```text
NovaLang Source
       ↓
Nova Intermediate Representation
       ├── Native Compiler
       ├── Bytecode Runtime
       ├── Interpreter
       ├── JIT Compiler
       └── Deterministic Runtime
```

Die konkrete Verfügbarkeit hängt vom Ausführungskontext ab.

Die Sprachsemantik muss unabhängig vom Backend erhalten bleiben.

## Speicherverwaltung

NovaLang gewährleistet standardmäßig:

- Typ- und Speichersicherheit.
- Kontrollierte Objektlebensdauer.
- Sichere Referenzen.
- Schutz vor Use-after-free.
- Kontrollierten gemeinsamen Speicherzugriff.
- Ressourcenfreigabe über definierte Lebenszyklusmechanismen.

Die konkrete Speicherverwaltungsstrategie darf je nach Runtime variieren.

Speicherunsichere Systemoperationen sind nicht Bestandteil der gewöhnlichen sicheren Sprachausführung und benötigen ausdrücklich definierte Mechanismen.

## Ressourcenverwaltung

NovaLang-Ausführung unterliegt dem Ressourcenmodell von NovaOS.

```text
Execution Contract
├── CPU Budget
├── Memory Budget
├── I/O Budget
├── Deadline
├── Energy Budget
├── Capability Context
└── Determinism Requirements
```

Die Runtime muss Ressourcenüberschreitungen kontrolliert behandeln können.

## Sprachkompatibilität

NovaLang orientiert sich syntaktisch möglichst eng an VB.NET.

Dabei gilt:

```text
VB.NET-like Syntax
        +
NovaOS Type System
        +
NovaOS Capability Model
        +
Nova Runtime
        =
NovaLang
```

NovaLang ist jedoch keine vollständige VB.NET-Implementierung.

Insbesondere bestehen keine automatischen Abhängigkeiten von:

- Common Language Runtime (CLR)
- .NET Base Class Library
- Microsoft.VisualBasic Runtime
- Windows-spezifischen APIs

Eine spätere VB.NET-Kompatibilitätsschicht darf über ausdrücklich definierte Provider realisiert werden.

## Introspection

NovaLang muss folgende Informationen bereitstellen können:

```text
Language Version
Compiler Version
Module Identity
Type Information
Interface Contracts
Capability Requirements
Execution State
Task Hierarchy
Resource Usage
Runtime Diagnostics
```

Die Einsicht unterliegt den NovaOS-Sicherheitsrichtlinien.

## Normative Anforderungen

1. NovaLang MUSS sich syntaktisch möglichst eng an VB.NET orientieren.
2. NovaLang MUSS VB.NET-nahe Deklarationen, Kontrollstrukturen und Blockabschlüsse verwenden.
3. Reguläre Bezeichner MÜSSEN ohne Unterscheidung der Groß-/Kleinschreibung aufgelöst werden.
4. NovaLang MUSS eine eigenständige, von .NET unabhängige Sprach- und Laufzeitimplementierung besitzen.
5. `.nova`, `.nlf` und `.nui` MÜSSEN denselben Sprachkern verwenden.
6. Das Typsystem MUSS statische Typprüfung und kontrollierte Typinferenz unterstützen.
7. Objektorientierte und generische Programmierung MÜSSEN unterstützt werden.
8. Fehler MÜSSEN strukturiert und typisiert behandelbar sein.
9. `Async` und `Await` MÜSSEN mit dem NovaOS-Taskmodell integrierbar sein.
10. Speicher- und Typsicherheit MÜSSEN standardmäßig gewährleistet werden.
11. Systemzugriffe MÜSSEN über autorisierte Capabilities erfolgen.
12. Custom Scripts in Solutions DÜRFEN keine eigenen Capability-Berechtigungen anfordern.
13. Logic Graph und NovaLang MÜSSEN dieselben Typregeln verwenden.
14. Deklarative UI-Definitionen MÜSSEN Teil derselben Sprachsemantik sein.
15. Die Sprachsemantik MUSS unabhängig vom Ausführungsbackend bleiben.
16. NovaLang MUSS ohne KI und ohne NovaLang Studio übersetzbar und ausführbar sein.
17. Ausführungsressourcen MÜSSEN über Execution Contracts begrenzbar sein.
18. Sprachversion, Typinformationen, Capability-Anforderungen und Ausführungszustände MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-SEMANTICTYPE-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-TEXT-UNICODE-0001`
- `NPSPEC-TEXT-SECURITY-0001`

## Ergebnis

NovaLang ist eine eigenständige, moderne Programmiersprache mit einer möglichst VB.NET-kompatiblen Syntax und einer speziell für NovaOS entwickelten Laufzeitarchitektur.

Die Sprache vereint klassische Programmierung, Capabilities, Solutions, Logic Graph und deklarative Benutzeroberflächen unter einem gemeinsamen Sprach- und Typsystem.

NovaLang bleibt unabhängig von .NET, Windows, KI und Entwicklungswerkzeugen und bildet die native Grundlage für die Softwareentwicklung innerhalb von NovaOS.