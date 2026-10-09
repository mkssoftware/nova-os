
# NPSPEC-NOVALANG-SEMANTICS-0001 – NovaLang Language Semantics

## Status

Angenommen

## Kategorie

NovaLang / Sprachsemantik

## Zweck

Diese Spezifikation definiert die Bedeutung und das Ausführungsverhalten gültiger NovaLang-Programme.

NovaLang verwendet eine möglichst VB.NET-kompatible Sprachsyntax, besitzt jedoch eine eigenständige, typsichere und speichersichere Semantik auf Grundlage der NovaOS-Architektur.

Die Spezifikation legt fest, wie Ausdrücke, Variablen, Objekte, Funktionen, Kontrollstrukturen, Fehler, Nebenläufigkeit und Capability-Zugriffe interpretiert werden.

Die Semantik gilt einheitlich für `.nova`, `.nlf` und `.nui`.

## Grundprinzipien

1. **Eindeutigkeit:** Jedes gültige Sprachkonstrukt besitzt eine definierte Bedeutung.
2. **VB.NET-Nähe:** Das Verhalten orientiert sich an VB.NET, sofern keine ausdrücklich definierte NovaOS-Regel abweicht.
3. **Typsicherheit:** Operationen müssen die statischen und dynamischen Typverträge einhalten.
4. **Speichersicherheit:** Sichere Sprachkonstrukte dürfen keine ungültigen Speicherzugriffe verursachen.
5. **Determinismus:** Identische Eingaben und Ausführungskontexte müssen reproduzierbare Ergebnisse ermöglichen.
6. **Explizite Berechtigungen:** Systemzugriffe benötigen autorisierte Capabilities.
7. **Kontrollierte Nebenwirkungen:** Ressourcen- und Zustandsänderungen unterliegen definierten Regeln.
8. **Structured Concurrency:** Nebenläufige Aufgaben besitzen kontrollierte Lebensdauern.
9. **Backend-Unabhängigkeit:** Compiler, Interpreter und JIT müssen dieselbe beobachtbare Sprachsemantik umsetzen.
10. **KI-Unabhängigkeit:** Die Ausführung benötigt keine künstliche Intelligenz.

## Semantische Verarbeitung

```text
NovaLang Source
       ↓
Lexer
       ↓
Parser
       ↓
Abstract Syntax Tree
       ↓
Name Resolution
       ↓
Type Resolution
       ↓
Type Checking
       ↓
Semantic Validation
       ↓
Typed Nova IR
       ↓
Execution Backend
       ↓
Nova Runtime
```

Die semantische Analyse darf keine regulären Programmoperationen ausführen.

## Ausführungskontext

Jede NovaLang-Ausführung besitzt einen definierten Kontext.

```text
ExecutionContext
├── ModuleIdentity
├── SolutionIdentity
├── Scope
├── TypeEnvironment
├── CapabilityContext
├── ExecutionContract
├── TaskContext
├── CancellationContext
├── ResourceBudget
├── DeterminismMode
└── Diagnostics
```

Nicht jeder Kontext benötigt eine Solution-Identität.

Fehlende optionale Kontextinformationen dürfen keine zusätzlichen Berechtigungen erzeugen.

## Namensauflösung

Bezeichner werden anhand ihres Gültigkeitsbereichs aufgelöst.

```vb
Dim zahl As Integer = 10

Public Sub Berechnen()

    Dim ergebnis As Integer = zahl * 2

End Sub
```

Es gelten folgende Regeln:

- Lokale Deklarationen besitzen Vorrang innerhalb ihres Gültigkeitsbereichs.
- Namensräume und Imports werden nach definierten Auflösungsregeln berücksichtigt.
- Mehrdeutige Referenzen müssen diagnostiziert werden.
- Bezeichner unterscheiden nicht zwischen Groß- und Kleinschreibung.
- Die ursprüngliche Schreibweise bleibt erhalten.
- `Imports` verändert keine Berechtigungen.

Die Auflösung muss unabhängig von der Reihenfolge zufälliger Laufzeitereignisse erfolgen.

## Gültigkeitsbereiche

NovaLang unterstützt lexikalische Gültigkeitsbereiche.

```text
Global Scope
    ↓
Namespace Scope
    ↓
Type Scope
    ↓
Member Scope
    ↓
Local Scope
```

Eine Variable darf nur innerhalb ihres gültigen Bereichs verwendet werden.

Namensüberschattung muss eindeutig geregelt und diagnostizierbar sein.

## Variablen und Zuweisungen

```vb
Dim counter As Integer = 0

counter = 10
counter += 5
```

Eine Variable besitzt:

```text
Variable
├── Identity
├── DeclaredType
├── CurrentValue
├── Scope
├── Mutability
└── Lifetime
```

Zuweisungen müssen typkompatibel sein.

Bei Werttypen wird der Wert entsprechend seiner Kopiersemantik übertragen.

Bei Referenztypen wird eine gültige Referenz auf das Objekt übertragen.

## Initialisierung

Variablen und Felder müssen vor ihrer Verwendung einen definierten Zustand besitzen.

```vb
Dim zahl As Integer = 0
```

NovaLang darf keine uninitialisierten Speicherinhalte als reguläre Werte verfügbar machen.

Wo VB.NET-kompatible Standardinitialisierung vorgesehen ist, muss diese typabhängig eindeutig definiert sein.

Für nicht-nullbare Referenzen gelten zusätzliche Initialisierungspflichten.

## Konstanten

```vb
Const MaxAttempts As Integer = 5
```

Konstanten müssen zur Übersetzungszeit auswertbar sein.

Ihr Wert darf während der Programmausführung nicht verändert werden.

Konstantenauswertung darf keine unautorisierten Systemzugriffe oder nicht deterministischen Nebenwirkungen ausführen.

## Typsemantik

NovaLang verwendet statische Typprüfung mit kontrollierter Typinferenz.

```vb
Dim name As String = "NovaOS"
Dim version = 1
```

Jeder Ausdruck besitzt einen statisch bestimmbaren Typ.

Dynamische Typprüfungen sind nur dort erforderlich, wo die Sprachsemantik sie ausdrücklich vorsieht.

Typidentität, Zuweisbarkeit und Konvertierbarkeit bleiben unterschiedliche Eigenschaften.

## Wertsemantik

Werttypen werden entsprechend ihrer definierten Wertrepräsentation kopiert.

```vb
Dim a As Integer = 10
Dim b As Integer = a

b = 20
```

Nach der Zuweisung gilt:

```text
a = 10
b = 20
```

Die Veränderung von `b` beeinflusst `a` nicht.

## Referenzsemantik

Referenztypen besitzen eine Objektidentität.

```vb
Dim person1 As New Person("Max")
Dim person2 As Person = person1
```

Beide Variablen können auf dieselbe Instanz verweisen.

```text
person1 ─┐
         ├── Person Object
person2 ─┘
```

Die Lebensdauer der Instanz wird durch die NovaOS-Runtime abgesichert.

## Gleichheit und Identität

NovaLang unterscheidet Wertgleichheit und Referenzidentität.

```vb
If a = b Then
    Console.WriteLine("Gleich")
End If
```

```vb
If person1 Is person2 Then
    Console.WriteLine("Dieselbe Instanz")
End If
```

`=` verwendet die für die beteiligten Typen definierte Gleichheitssemantik.

`Is` und `IsNot` prüfen Referenzidentität beziehungsweise die entsprechend definierte Nothing-Identität.

Benutzerdefinierte Gleichheit darf die grundlegende Objektidentität nicht verändern.

## Auswertungsreihenfolge

Ausdrücke werden nach der festgelegten Operatorpräzedenz und Assoziativität ausgewertet.

Operanden und Argumente werden grundsätzlich von links nach rechts ausgewertet, sofern eine spezielle Sprachregel nichts anderes festlegt.

```vb
Dim ergebnis As Integer = A() + B()
```

`A()` wird vor `B()` ausgewertet.

Compileroptimierungen dürfen die beobachtbare Reihenfolge von Nebenwirkungen nicht verändern.

## Kurzschlussauswertung

`AndAlso` und `OrElse` verwenden Kurzschlussauswertung.

```vb
If person IsNot Nothing AndAlso person.Name = "Max" Then

    Console.WriteLine("Gefunden")

End If
```

Der rechte Ausdruck wird nur ausgewertet, wenn dies für das Ergebnis erforderlich ist.

`And` und `Or` besitzen dagegen ihre jeweils definierte vollständige beziehungsweise bitweise Auswertungssemantik.

## Numerische Semantik

Numerische Operationen besitzen festgelegte Wertebereiche und Überlaufregeln.

```vb
Dim ergebnis As Integer = 10 + 20
```

Es gelten folgende Anforderungen:

- Ganzzahlüberläufe müssen kontrolliert behandelt werden.
- Division durch null muss definiert fehlschlagen.
- Gleitkommaoperationen besitzen eine spezifizierte Rundungssemantik.
- `NaN` und Unendlichkeiten werden nach definierten Regeln behandelt.
- Konvertierungen dürfen nicht unbemerkt Speicher- oder Typsicherheit verletzen.

Optimierungen dürfen die numerische Semantik nicht ohne ausdrückliche Freigabe verändern.

## Zeichenkettensemantik

`String` repräsentiert Unicode-Text.

```vb
Dim text As String = "Hallo " & "NovaOS"
```

`&` führt Zeichenkettenverkettung aus.

Die interne Textrepräsentation darf von .NET abweichen.

Vergleich, Normalisierung, Segmentierung und kulturell abhängige Sortierung werden durch die NovaOS-Textdienste definiert.

Kulturelle Einstellungen dürfen nicht unbemerkt die Semantik von Programmbezeichnern verändern.

## Nothing-Semantik

`Nothing` wird entsprechend dem Zieltyp interpretiert.

```vb
Dim person As Person = Nothing
Dim alter As Integer? = Nothing
```

Bei Referenztypen bezeichnet `Nothing` das Fehlen einer gültigen Objektreferenz.

Bei `Nullable(Of T)` bezeichnet es einen Wert ohne enthaltenen Nutzwert.

Nicht-nullbare Typen dürfen keinen ungültigen Nothing-Zustand erhalten.

Ein Zugriff auf eine fehlende Referenz muss kontrolliert verhindert werden.

## Typkonvertierungen

NovaLang unterscheidet:

```text
Widening Conversion
Narrowing Conversion
Checked Conversion
Explicit Cast
Forbidden Conversion
```

Beispiel:

```vb
Dim zahl As Integer = 42
Dim gross As Long = CLng(zahl)
```

`Option Strict On` ist der Standard.

Implizite verlustbehaftete Konvertierungen sind im sicheren Standardmodus nicht zulässig.

Explizite Konvertierungen müssen bei ungültigen Werten kontrolliert fehlschlagen.

## Objektinitialisierung

```vb
Dim person As New Person With {
    .Name = "Max",
    .Alter = 30
}
```

Die Initialisierung erfolgt nach einer definierten Reihenfolge.

Eigenschaftsinitialisierungen werden in Quelltextreihenfolge ausgewertet.

Ein fehlerhafter Initialisierungsvorgang darf kein ungültiges Objekt als erfolgreich initialisiert veröffentlichen.

Ressourcen und bereits erfolgte externe Nebenwirkungen unterliegen den jeweiligen Fehler- und Transaktionsregeln.

## Konstruktoren

```vb
Public Sub New(name As String)

    Me.Name = name

End Sub
```

Ein Konstruktor initialisiert eine neue Objektinstanz.

Die Instanz darf erst dann als vollständig initialisiert gelten, wenn die erforderlichen Initialisierungsverträge erfüllt sind.

Die Veröffentlichung teilweise initialisierter Objekte muss durch die sichere Sprachsemantik verhindert oder ausdrücklich kontrolliert werden.

## Funktionen

```vb
Public Function Addieren(a As Integer, b As Integer) As Integer

    Return a + b

End Function
```

Funktionen besitzen:

```text
FunctionContract
├── Parameters
├── ReturnType
├── GenericConstraints
├── Visibility
├── ErrorContract
└── ExecutionRequirements
```

Parameter werden vor dem Funktionsaufruf entsprechend der definierten Auswertungsreihenfolge ausgewertet.

Rückgabewerte müssen mit dem deklarierten Rückgabetyp kompatibel sein.

## ByVal und ByRef

`ByVal` übergibt einen Wert beziehungsweise eine Referenz als eigenen Parameterwert.

```vb
Public Sub Anzeigen(ByVal text As String)
End Sub
```

`ByRef` erlaubt kontrollierten Zugriff auf die Variable des Aufrufers.

```vb
Public Sub Erhoehen(ByRef wert As Integer)

    wert += 1

End Sub
```

`ByRef` muss Lebensdauer-, Alias- und Nebenläufigkeitsregeln einhalten.

Ungültige Referenzen dürfen nicht entstehen.

## Kontrollfluss

NovaLang unterstützt die VB.NET-orientierten Kontrollstrukturen.

```text
If / ElseIf / Else / End If
Select Case / End Select
For / Next
For Each / Next
While / End While
Do / Loop
Exit
Continue
Return
```

Jede Kontrollstruktur besitzt eindeutig definierte Eintritts-, Wiederholungs- und Austrittsbedingungen.

## If-Semantik

```vb
If aktiviert Then

    Starten()

Else

    Stoppen()

End If
```

Die Bedingung muss einen zulässigen Boolean-Ausdruck ergeben.

Es wird ausschließlich der ausgewählte Zweig ausgeführt.

## Select-Case-Semantik

```vb
Select Case zustand

    Case SystemState.Running
        Console.WriteLine("Läuft")

    Case Else
        Console.WriteLine("Anderer Zustand")

End Select
```

Der Selektorausdruck wird einmal ausgewertet.

Die `Case`-Zweige werden nach den definierten Vergleichsregeln geprüft.

Nur der erste passende Zweig wird ausgeführt.

## Schleifensemantik

```vb
For i As Integer = 1 To 10

    Console.WriteLine(i)

Next
```

Die `For`-Schleife verwendet eine inklusive Endwertsemantik.

Startwert, Endwert und Schrittweite werden nach den festgelegten VB.NET-orientierten Regeln ausgewertet.

Schleifen müssen Abbruch, Fehler und Ressourcenlimits kontrolliert behandeln.

## For-Each-Semantik

```vb
For Each datei As FileInfo In dateien

    Console.WriteLine(datei.Name)

Next
```

`For Each` verwendet eine typisierte Iterationsschnittstelle.

Die Iteration muss definierte Regeln für:

- Elementreihenfolge
- Veränderung der Collection
- Iteratorlebensdauer
- Fehlerbehandlung
- Ressourcenfreigabe

besitzen.

## Exceptions

NovaLang unterstützt strukturierte Exception-Behandlung.

```vb
Try

    DatenLaden()

Catch ex As Exception

    Console.WriteLine(ex.Message)

Finally

    RessourcenFreigeben()

End Try
```

Exceptions werden entlang des gültigen Aufrufkontexts weitergereicht.

`Finally` wird bei regulärem Verlassen und bei behandelbaren Fehlern nach den definierten Unwinding-Regeln ausgeführt.

Ein Systemabsturz oder erzwungener Prozessabbruch garantiert keine Ausführung von `Finally`.

## Result-Semantik

NovaLang unterstützt typisierte Fehlerergebnisse.

```vb
Public Function Laden() As Result(Of String, Error)

    Return Result.Ok("Daten")

End Function
```

Ein `Result(Of T, E)` besitzt entweder einen Erfolgswert oder einen Fehlerwert.

Beide Zustände müssen eindeutig unterscheidbar sein.

Die Verarbeitung von Result-Werten erfolgt über typisierte Standardbibliotheksoperationen.

## Ressourcenlebensdauer

Ressourcen besitzen definierte Besitz- und Freigaberegeln.

```vb
Using stream As IStream = Datei.Oeffnen()

    stream.Write("NovaOS")

End Using
```

`Using` gewährleistet die kontrollierte Freigabe entsprechend dem Ressourcenvertrag.

Die Freigabe muss auch bei behandelbaren Exceptions erfolgen.

Die konkrete Implementierung verwendet NovaOS-Ressourcenmechanismen und benötigt kein .NET-`IDisposable`.

## Nebenwirkungen

NovaLang unterscheidet reine Berechnungen und Operationen mit Nebenwirkungen.

```text
Pure Computation
State Mutation
I/O Operation
Capability Invocation
Resource Allocation
Event Emission
Task Creation
```

Nebenwirkungen müssen innerhalb des autorisierten Ausführungskontexts erfolgen.

Die Runtime darf Nebenwirkungen nicht ohne entsprechende Semantik duplizieren oder neu anordnen.

## Ereignisse

```vb
Public Event DataChanged(value As String)
```

```vb
RaiseEvent DataChanged("Aktualisiert")
```

Ereignisse besitzen typisierte Parameter.

Handler werden entsprechend den definierten Ereignisregeln aufgerufen.

Die Ausführungsreihenfolge mehrerer Handler muss durch den jeweiligen Ereignisvertrag eindeutig festgelegt oder ausdrücklich als nicht garantiert dokumentiert werden.

Ereignisse dürfen keine Berechtigungen übertragen, die dem Empfänger nicht ausdrücklich gewährt wurden.

## Asynchrone Ausführung

```vb
Public Async Function LadenAsync() As Task(Of String)

    Dim daten As String = Await Netzwerk.LesenAsync()

    Return daten

End Function
```

`Async` definiert eine asynchrone Funktion.

`Await` wartet auf das Ergebnis einer asynchronen Operation, ohne zwingend den ausführenden Systemthread zu blockieren.

Die tatsächliche Ausführung erfolgt über die NovaOS-Runtime.

## Structured Concurrency

Asynchrone Aufgaben gehören zu einem kontrollierten Task-Kontext.

```text
Parent Task
├── Child Task A
├── Child Task B
└── Child Task C
```

Es gelten folgende Regeln:

- Aufgaben besitzen definierte Lebensdauern.
- Cancellation wird kontrolliert weitergegeben.
- Fehler werden nach definierten Regeln propagiert.
- Ressourcenbudgets werden eingehalten.
- Verwaiste Aufgaben dürfen nicht unkontrolliert weiterlaufen.
- Explizit entkoppelte Hintergrunddienste benötigen einen eigenen autorisierten Lebenszyklus.

## Gemeinsamer Zustand

Nebenläufige Zugriffe auf veränderliche Daten müssen synchronisiert werden.

```text
Shared State
    ↓
Synchronization Contract
    ↓
Controlled Access
```

Datenrennen dürfen innerhalb sicherer NovaLang-Konstrukte kein undefiniertes Speicherverhalten verursachen.

Synchronisationsmechanismen werden durch die NovaOS-Runtime bereitgestellt.

## Deterministische Ausführung

NovaLang unterstützt einen deterministischen Ausführungsmodus.

```text
DeterministicContext
├── Controlled Time
├── Controlled Randomness
├── Defined Scheduling
├── Stable Inputs
├── Explicit External Events
└── Reproducible Results
```

Determinismus gilt nur innerhalb eines vollständig definierten Ausführungskontexts.

Externe Ereignisse müssen aufgezeichnet, kontrolliert oder ausdrücklich als nicht deterministisch behandelt werden.

## Capability-Semantik

NovaLang verwendet das Capability-Sicherheitsmodell von NovaOS.

```vb
Public Function DatenLesen(reader As INetworkReader) As Result(Of String, Error)

    Return reader.Read()

End Function
```

Der Schnittstellentyp beschreibt die verfügbaren Operationen.

Die Berechtigung entsteht ausschließlich durch ein gültiges, autorisiertes Capability-Handle.

```text
Imports ≠ Permission
Interface ≠ Authority
CapabilityID ≠ Authorized Handle
```

Capability-Aufrufe unterliegen den Systemrichtlinien und Execution Contracts.

## Capability-Weitergabe

Eine Capability darf nur gemäß ihrer Autorisierungs- und Delegationsregeln weitergegeben werden.

Die bloße Übergabe eines typisierten Objekts darf keine unzulässige Rechteausweitung verursachen.

Delegierte Capabilities dürfen höchstens die ausdrücklich freigegebenen Rechte vermitteln.

## Logic-Graph-Semantik

NovaLang-Funktionen können als Custom-Script-Knoten in Solutions verwendet werden.

```text
Network Capability
        ↓
Typed Data
        ↓
Custom NovaLang Script
        ↓
Typed Result
        ↓
Storage Capability
```

Das Skript verarbeitet ausschließlich seine bereitgestellten Eingaben und autorisierten Schnittstellen.

Es darf keine zusätzlichen Systemberechtigungen selbst anfordern.

Die Ausführung erfolgt innerhalb des vom Logic Graph definierten Kontexts.

## Deklarative UI-Semantik

`.nui` verwendet dieselbe NovaLang-Typ- und Ausdruckssemantik.

Deklarative UI-Definitionen beschreiben Oberflächen, Eigenschaften, Bindungen und Interaktionen.

Es gelten folgende Regeln:

- UI-Eigenschaften werden typisiert.
- Bindungen besitzen definierte Datenquellen.
- Änderungen werden kontrolliert propagiert.
- Ereignisse werden über typisierte Handler verarbeitet.
- UI-Deklarationen erzeugen keine impliziten Systemberechtigungen.
- Die UI muss ohne KI deterministisch rekonstruierbar sein.

Die konkrete Reaktivitäts- und Lebensdauerseman­tik wird gesondert spezifiziert.

## Module und Imports

```vb
Imports Nova.System
Imports Nova.Text
```

`Imports` beeinflusst ausschließlich die Namensauflösung.

Es führt keine Module automatisch mit Systemberechtigungen aus.

Modulinitialisierung, Abhängigkeiten und Ladeverhalten müssen separat definiert sein.

## Fehlerklassen

NovaLang unterscheidet mindestens:

```text
Syntax Error
Type Error
Semantic Error
Recoverable Runtime Error
Exception
Contract Violation
Capability Denial
Cancellation
Resource Limit Exceeded
Runtime Failure
```

Fehlerklassen müssen eindeutig diagnostizierbar sein.

Sicherheitsverletzungen dürfen nicht durch gewöhnliche Exception-Behandlung in eine Berechtigung umgewandelt werden.

## Backend-Äquivalenz

NovaLang unterstützt verschiedene Ausführungsbackends.

```text
Typed Nova IR
├── Native Compiler
├── Bytecode Runtime
├── Interpreter
└── JIT Compiler
```

Alle Backends müssen für dieselbe Sprachversion und denselben Ausführungskontext semantisch äquivalente Ergebnisse liefern.

Zulässige Unterschiede betreffen ausschließlich ausdrücklich nicht garantierte Eigenschaften wie bestimmte Performance- oder Schedulingdetails.

## Semantische Versionierung

Die Sprachsemantik wird versioniert.

Änderungen an folgenden Eigenschaften gelten als semantisch relevant:

```text
Type Conversion
Operator Behavior
Evaluation Order
Object Lifetime
Exception Propagation
Async Behavior
Capability Rules
Nullability
Numeric Behavior
```

Inkompatible Änderungen benötigen eine explizite Sprachversionsänderung.

## Introspection

Die Runtime muss autorisiert bereitstellen können:

```text
LanguageVersion
SemanticVersion
ExecutionContext
ResolvedTypes
CallStack
TaskHierarchy
CapabilityContext
ResourceUsage
RuntimeDiagnostics
```

Introspection darf keine Sicherheits- oder Isolationsgrenzen umgehen.

## Normative Anforderungen

1. NovaLang MUSS eine eindeutig definierte Sprachsemantik besitzen.
2. Die Semantik MUSS sich grundsätzlich an VB.NET orientieren, sofern keine NovaOS-spezifische Abweichung festgelegt ist.
3. Namensauflösung MUSS ohne Unterscheidung der Groß- und Kleinschreibung erfolgen.
4. Gültigkeitsbereiche und Variablenlebensdauern MÜSSEN eindeutig definiert sein.
5. Ausdrücke MÜSSEN eine definierte Auswertungsreihenfolge besitzen.
6. Werttypen und Referenztypen MÜSSEN unterschiedliche, eindeutig definierte Semantiken besitzen.
7. Typkonvertierungen MÜSSEN kontrolliert und typsicher erfolgen.
8. Ungültige Speicherzugriffe DÜRFEN durch sichere Sprachkonstrukte nicht entstehen.
9. `Nothing` und Nullable-Typen MÜSSEN kontrolliert behandelt werden.
10. Kontrollstrukturen MÜSSEN deterministisch interpretierbare Ablaufregeln besitzen.
11. Exceptions und Result-Werte MÜSSEN unterscheidbare Fehlermechanismen bleiben.
12. Ressourcen MÜSSEN definierte Lebensdauer- und Freigaberegeln besitzen.
13. `Async` und `Await` MÜSSEN die NovaOS-Tasksemantik verwenden.
14. Nebenläufige Aufgaben MÜSSEN Structured Concurrency unterstützen.
15. Datenrennen DÜRFEN kein undefiniertes Speicherverhalten verursachen.
16. Capability-Typen und Imports DÜRFEN keine Berechtigungen erzeugen.
17. Capability-Aufrufe MÜSSEN autorisiert sein.
18. Custom Scripts in Solutions DÜRFEN keine eigenen Systemberechtigungen anfordern.
19. `.nova`, `.nlf` und `.nui` MÜSSEN dieselbe grundlegende Sprachsemantik verwenden.
20. Deklarative UI-Bindungen MÜSSEN typisiert und kontrolliert ausgeführt werden.
21. Unterschiedliche Ausführungsbackends MÜSSEN semantisch äquivalent sein.
22. Deterministische Ausführung MUSS für definierte Kontexte unterstützt werden.
23. Semantische Änderungen MÜSSEN versioniert werden.
24. Die Ausführung MUSS unabhängig von der .NET-Runtime erfolgen.
25. Semantische Informationen und Laufzeitdiagnosen MÜSSEN kontrolliert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NOVALANG-CORE-0001`
- `NPSPEC-NOVALANG-SYNTAX-0001`
- `NPSPEC-NOVALANG-GRAMMAR-0001`
- `NPSPEC-NOVALANG-LEXER-0001`
- `NPSPEC-NOVALANG-TYPES-0001`
- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-SEMANTICTYPE-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-TEXT-UNICODE-0001`
- `NPSPEC-TEXT-SECURITY-0001`

## Ergebnis

NovaLang besitzt eine einheitliche, VB.NET-orientierte und eigenständig implementierte Sprachsemantik.

Die Sprache verbindet vertraute Programmierkonzepte mit statischer Typprüfung, Speichersicherheit, kontrollierter Nebenläufigkeit, semantischen Typen und dem Capability-Sicherheitsmodell von NovaOS.

Programme, Solutions, Logic Graph und deklarative Benutzeroberflächen verwenden dieselben grundlegenden Ausführungsregeln.

NovaLang bleibt unabhängig von .NET, KI und dem gewählten Ausführungsbackend.
