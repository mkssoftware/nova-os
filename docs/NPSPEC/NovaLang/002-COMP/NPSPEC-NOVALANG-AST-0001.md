
# NPSPEC-NOVALANG-AST-0001 – NovaLang Abstract Syntax Tree

## Status

Angenommen

## Kategorie

NovaLang / Compiler / AST

## Zweck

Definiert die Struktur und Verarbeitung des Abstract Syntax Tree (AST) als gemeinsame syntaktische Grundlage für NovaLang Compiler, statische Analyse und NovaLang Studio.

## Architektur

Der AST bildet die syntaktische Struktur eines NovaLang-Programms unabhängig von der Zielarchitektur ab.

Er entsteht aus dem Parser und wird anschließend durch die semantische Analyse verarbeitet.

Der AST ist nicht mit der ausführungsorientierten Nova IR gleichzusetzen.

## Knotenmodell

Jeder AST-Knoten besitzt:

- Eindeutigen Knotentyp
- Quelltextbereich mit Start- und Endposition
- Definierte Kindknoten und Eigenschaften
- Zugehörige Syntaxinformationen

Wesentliche Knotenkategorien:

| Kategorie | Inhalt |
|---|---|
| Deklarationen | Namespace, Module, Class, Structure, Interface |
| Mitglieder | Function, Sub, Property, Event, Field |
| Anweisungen | If, Select Case, For, While, Try |
| Ausdrücke | Operatoren, Aufrufe, Lambda, Await |
| Typen | Generics, Arrays, Nullability |
| Metadaten | Attribute, Annotationen |
| Fehlerknoten | Unvollständige oder fehlerhafte Syntax |

## Strukturbeispiel

NovaLang-Quellcode:

```vb
Public Function Addieren(a As Integer, b As Integer) As Integer
    Return a + b
End Function
```

Vereinfachte AST-Struktur:

```text
FunctionDeclaration
 ├─ Name: Addieren
 ├─ Parameters
 │   ├─ a As Integer
 │   └─ b As Integer
 ├─ ReturnType: Integer
 └─ ReturnStatement
     └─ BinaryExpression (+)
         ├─ Identifier: a
         └─ Identifier: b
```

## Verarbeitung

- Der Parser erzeugt den syntaktischen AST.
- Die semantische Analyse ordnet Typen, Symbole und Verträge zu.
- AST-Knoten bleiben nach ihrer Erstellung unveränderlich.
- Änderungen erzeugen neue Knoten oder Teilbäume.
- Unveränderte Teilbäume dürfen wiederverwendet werden.
- Die Überführung in Nova IR erfolgt nach erfolgreicher semantischer Prüfung.

## NovaLang Studio

Der AST unterstützt:

- Syntaxanalyse und Fehlerdiagnostik
- Codevervollständigung und Navigation
- Refactoring und Symbolsuche
- Inkrementelle Quelltextverarbeitung

Fehlerhafte oder unvollständige Quelltexte müssen durch explizite Fehlerknoten darstellbar bleiben.

## Normative Anforderungen

1. Der Compiler MUSS einen einheitlichen, typisierten AST-Knotenaufbau verwenden.
2. Jeder Knoten MUSS seinen Quelltextbereich eindeutig referenzieren.
3. AST-Knoten MÜSSEN nach ihrer Erstellung unveränderlich sein.
4. Semantische Informationen MÜSSEN eindeutig zugeordnet werden können.
5. Fehlerhafte Syntax MUSS ohne Abbruch der gesamten AST-Erstellung darstellbar sein.
6. Der AST MUSS inkrementelle Verarbeitung ermöglichen.
7. `.nova`, `.nlf` und `.nui` MÜSSEN dieselben grundlegenden AST-Knotentypen verwenden.
8. AST und Nova IR MÜSSEN architektonisch getrennt bleiben.

## Ergebnis

NovaLang erhält einen einheitlichen, unveränderlichen und inkrementell verarbeitbaren AST als Grundlage für Compiler, semantische Analyse und NovaLang Studio.
