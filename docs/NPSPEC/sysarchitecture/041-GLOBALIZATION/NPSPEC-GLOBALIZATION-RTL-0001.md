# NPSPEC-GLOBALIZATION-RTL-0001 – Nova Right-to-Left Support

## Status

Angenommen

## Kategorie

Globalization / RTL

## Zweck

NovaOS definiert die systemweite Unterstützung für rechts-nach-links geschriebene Sprachen und bidirektionale Inhalte.

RTL beeinflusst Textfluss und Oberflächenlayout, ohne Daten, Identitäten oder die logische Bedeutung von Bedienelementen zu verändern.

## Grundprinzipien

```text
RTL ≠ Mirrored Application
Text Direction ≠ Language
Text Direction ≠ Identity
Visual Order ≠ Logical Order
RTL ≠ Reversed Text
Mixed Direction ≠ Invalid Content
```

## Modell

```text
DirectionContext
├── Direction
├── Language
├── Script
├── BaseDirection
└── BidirectionalRules
```

Unterstützte Basisrichtungen:

```text
LTR
RTL
Auto
```

`Auto` bestimmt die geeignete Basisrichtung aus dem jeweiligen Textkontext.

## Textdarstellung

NovaOS muss Unicode-basierte bidirektionale Textdarstellung unterstützen.

```text
Logical Text
     ↓
Unicode Bidirectional Processing
     ↓
Visual Text
```

Die logische Zeichenreihenfolge gespeicherter Inhalte darf durch die visuelle Darstellung nicht verändert werden.

## Gemischte Inhalte

RTL- und LTR-Inhalte müssen innerhalb desselben Textes korrekt kombinierbar sein.

Beispiele:

```text
Arabischer Text + URL
Hebräischer Text + Zahl
RTL-Text + Dateiname
RTL-Text + Quellcode
```

Technische Inhalte dürfen ihre notwendige interne Schreibrichtung behalten.

## UI-Layout

RTL-fähige Oberflächen dürfen ihre visuelle Anordnung spiegeln:

```text
LTR                    RTL

Navigation | Content   Content | Navigation
← Back                 Back →
```

Spiegelbar sind insbesondere:

```text
Layout Flow
Navigation
Margins
Alignment
Directional Icons
Animations
Panels
```

## Nicht spiegelbare Elemente

Nicht jedes visuelle Element darf automatisch gespiegelt werden.

Beispiele:

```text
Logos
Fotos
Diagramme mit fester Semantik
Mediensteuerung
Uhren
Quellcode
Technische Symbole
```

Ob ein Element gespiegelt wird, muss durch seine semantische Rolle bestimmt werden.

## Eingabe und Cursor

Textfelder müssen bidirektionale Eingabe unterstützen.

Dabei müssen insbesondere:

```text
Cursor Movement
Selection
Insertion
Deletion
Copy / Paste
Keyboard Navigation
```

der logischen und visuellen Textstruktur entsprechend funktionieren.

## Programme und Solutions

Programme, Solutions und generierte UIs sollen die effektive Schreibrichtung aus dem Globalization Context beziehen.

Deklarative UI soll bevorzugt richtungsneutrale Eigenschaften verwenden:

```text
Start
End
```

anstatt ausschließlich:

```text
Left
Right
```

Dadurch kann dieselbe UI-Struktur für LTR und RTL verwendet werden.

## Sicherheit

Bidirektionale Unicode-Steuerzeichen dürfen sicherheitsrelevante Identitäten oder technische Inhalte nicht irreführend darstellen.

Bei sicherheitskritischen Namen, Pfaden oder Identifikatoren darf NovaOS eine eindeutige oder kanonische Darstellung verwenden.

## Normative Anforderungen

1. NovaOS MUSS LTR- und RTL-Schreibrichtungen unterstützen.
2. Bidirektionaler Unicode-Text MUSS korrekt verarbeitet werden.
3. Visuelle Reihenfolge und logische Zeichenreihenfolge MÜSSEN getrennt bleiben.
4. RTL-Darstellung DARF gespeicherte Textdaten nicht umordnen.
5. Gemischte RTL-/LTR-Inhalte MÜSSEN unterstützt werden.
6. UI-Komponenten SOLLEN richtungsneutrale Layoutattribute unterstützen.
7. Spiegelung MUSS anhand der semantischen Rolle eines UI-Elements steuerbar sein.
8. Nicht spiegelbare Inhalte MÜSSEN explizit erhalten bleiben können.
9. Texteingabe, Cursor und Auswahl MÜSSEN bidirektionalen Text unterstützen.
10. Programme und Solutions MÜSSEN den effektiven Direction Context abfragen können.
11. Bidirektionale Darstellung DARF sicherheitsrelevante Identitäten nicht verfälschen.
12. Effektive Schreibrichtung und verwendete Bidirectional Rules MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-GLOBALIZATION-LANGUAGE-0001`
- `NPSPEC-GLOBALIZATION-LOCALE-0001`
- `NPSPEC-GLOBALIZATION-NEGOTIATION-0001`
- `NPSPEC-GLOBALIZATION-RESOURCE-0001`

## Ergebnis

NovaOS unterstützt LTR-, RTL- und gemischte bidirektionale Inhalte systemweit. Text, Eingabe und Benutzeroberflächen können sich an die effektive Schreibrichtung anpassen, während logische Datenreihenfolge, technische Semantik und stabile Systemidentitäten unverändert bleiben.