
# NPSPEC-NOVALANG-METADATA-0001 – NovaLang Metadata

## Status

Angenommen

## Kategorie

NovaLang / Metadaten und Attribute

## Zweck

Definiert die Beschreibung von Typen, Funktionen, Eigenschaften und anderen Sprachelementen durch strukturierte Metadaten. Die Syntax orientiert sich an VB.NET und verwendet das native Metadatensystem von NovaOS.

## Attribute

Metadaten werden über Attribute in spitzen Klammern deklariert.

```vb
<Description("Berechnet die Summe")>
<Version("1.0")>
Public Function Addieren(a As Integer, b As Integer) As Integer
    Return a + b
End Function
```

Attribute können auf Typen, Methoden, Eigenschaften, Felder, Parameter und Module angewendet werden.

## Metadatentypen

| Kategorie | Verwendung |
|---|---|
| Identität | Name, GUID, Version |
| Beschreibung | Dokumentation, Anzeigename |
| Typinformationen | Typen, Parameter, Rückgabewerte |
| Verträge | Voraussetzungen, Nachbedingungen |
| Serialisierung | Feldnamen, Formate |
| UI | Darstellung, Bindungen |
| Capability | Erforderliche Berechtigungen |
| Laufzeit | Ausführungsanforderungen |

## Benutzerdefinierte Attribute

```vb
Public Class DescriptionAttribute
    Inherits Attribute

    Public ReadOnly Property Text As String

    Public Sub New(text As String)
        Me.Text = text
    End Sub
End Class
```

Attributargumente müssen zur Übersetzungszeit bestimmbar sein. Benutzerdefinierte Attribute müssen die festgelegten Attributverträge erfüllen.

## Metadatenverarbeitung

- Metadaten werden vom Compiler geprüft und in einer versionierten Struktur gespeichert.
- Attribute dürfen mehrfach verwendet werden, sofern ihre Definition dies erlaubt.
- Metadaten können zur Übersetzungszeit oder autorisiert zur Laufzeit ausgewertet werden.
- Nicht benötigte Metadaten dürfen entsprechend ihrem Aufbewahrungsvertrag entfernt werden.
- Metadaten müssen unabhängig vom Ausführungsbackend interpretierbar sein.

## NovaOS-Integration

Metadaten unterstützen Capabilities, Solutions, Logic Graph, NovaLang Studio und generative Benutzeroberflächen.

Capability-Attribute beschreiben Anforderungen, erteilen jedoch keine Berechtigungen.

Metadaten dürfen keine Sicherheitsgrenzen umgehen oder ohne Autorisierung ausführbaren Code auslösen.

## Normative Anforderungen

1. NovaLang MUSS Attribute nach VB.NET-orientierter Syntax unterstützen.
2. Attribute MÜSSEN statisch typgeprüft werden.
3. Benutzerdefinierte Attribute MÜSSEN unterstützt werden.
4. Metadaten MÜSSEN strukturiert und versioniert gespeichert werden.
5. Metadaten MÜSSEN kontrolliert introspektierbar sein.
6. Attribute DÜRFEN keine Capability-Berechtigungen erzeugen.
7. `.nova`, `.nlf` und `.nui` MÜSSEN dasselbe Metadatenmodell verwenden.

## Ergebnis

NovaLang besitzt ein natives, erweiterbares Metadatensystem zur Beschreibung und Analyse von Sprachelementen, unabhängig von der .NET-Runtime und sicher integriert in NovaOS.
