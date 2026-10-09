
# NPSPEC-NOVALANG-ANNOTATIONS-0001 – NovaLang Annotations

## Status

Angenommen

## Kategorie

NovaLang / Annotationen

## Zweck

Definiert Annotationen zur Dokumentation, statischen Analyse und Steuerung von Entwicklungswerkzeugen. Annotationen ergänzen Metadaten, ohne die grundlegende Sprachsemantik zu verändern.

## Annotationsarten

| Kategorie | Verwendung |
|---|---|
| Dokumentation | Beschreibungen, Parameterhinweise |
| Compiler | Warnungen, Optimierungshinweise |
| Nullability | Zusätzliche Nullzustandsverträge |
| Sicherheit | Kennzeichnung sensibler Daten |
| Verträge | Vorbedingungen, Nachbedingungen |
| Analyse | Diagnose- und Prüfhinweise |
| Entwicklung | Veraltet, experimentell, TODO |

## Syntax

NovaLang verwendet VB.NET-kompatible Attribute für strukturierte Annotationen.

```vb
<Obsolete("Stattdessen NeueMethode verwenden")>
Public Sub AlteMethode()
End Sub
```

Dokumentationsannotation erfolgt über XML-Dokumentationskommentare.

```vb
''' <summary>
''' Addiert zwei Zahlen.
''' </summary>
''' <param name="a">Erste Zahl</param>
''' <param name="b">Zweite Zahl</param>
''' <returns>Summe</returns>
Public Function Addieren(a As Integer, b As Integer) As Integer
    Return a + b
End Function
```

## Verarbeitungsregeln

- Strukturierte Annotationen verwenden das Attributsystem aus `NPSPEC-NOVALANG-METADATA-0001`.
- Dokumentationskommentare werden vom Compiler und NovaLang Studio ausgewertet.
- Annotationen müssen syntaktisch und semantisch validiert werden.
- Unbekannte Annotationen werden entsprechend ihrem Typ als Fehler oder Diagnose behandelt.
- Analysehinweise dürfen die garantierte Sprachsemantik nicht verändern.
- Sicherheitsrelevante Verträge müssen unabhängig von Annotationen durchgesetzt werden.
- Annotationen dürfen keine Capability-Berechtigungen erteilen.

## NovaOS-Integration

NovaLang Studio nutzt Annotationen für IntelliSense, Dokumentation, Diagnostik und statische Analyse.

Solutions und Logic Graph können Annotationen zur Beschreibung von Ein- und Ausgaben verwenden.

Generative Benutzeroberflächen dürfen Annotationen als Hinweise auswerten, müssen jedoch die tatsächlichen Typ- und Capability-Verträge einhalten.

## Normative Anforderungen

1. NovaLang MUSS strukturierte Annotationen und XML-Dokumentationskommentare unterstützen.
2. Strukturierte Annotationen MÜSSEN das bestehende Metadatenmodell verwenden.
3. Annotationen MÜSSEN statisch validierbar sein.
4. Compiler und Entwicklungswerkzeuge MÜSSEN relevante Annotationen auswerten können.
5. Annotationen DÜRFEN keine Sicherheitsregeln oder Capability-Prüfungen umgehen.
6. `.nova`, `.nlf` und `.nui` MÜSSEN dieselben grundlegenden Annotationsregeln verwenden.

## Ergebnis

NovaLang besitzt ein einheitliches Annotationssystem für Dokumentation, statische Analyse und Entwicklungswerkzeuge, ohne eine zusätzliche Sprachsyntax oder ein paralleles Metadatensystem einzuführen.
