# NPSPEC-FONT-MANAGER-0001 – Nova Font Manager

## Status

Angenommen

## Kategorie

Font / Management

## Zweck

NovaOS definiert einen zentralen Font Manager für Erkennung, Registrierung, Auswahl, Aktivierung und Verwaltung von Schriftressourcen.

Der Font Manager stellt Fonts für Text Shaping, Layout und Rendering bereit, ohne selbst Textsemantik oder Glyphenpositionierung zu bestimmen.

## Grundprinzipien

```text
Font ≠ Typeface
Font ≠ Glyph
Font ≠ Script
Font ≠ Language
Font Family ≠ Font File
Font Discovery ≠ Font Authority
Font Manager ≠ Text Shaper
```

## Architektur

```text
Font Sources
     ↓
Validation
     ↓
Font Manager
     ↓
Font Registry
     ↓
Font Selection
     ↓
Shaping / Layout / Rendering
```

## Font-Modell

```text
FontFace
├── FontID
├── FamilyID
├── FamilyName
├── Style
├── Weight
├── Width
├── Slant
├── UnicodeCoverage
├── Scripts[]
├── Features[]
├── VariationAxes[]
├── Source
└── State
```

`FontID` und `FamilyID` sind stabile interne Identitäten und nicht von lokalisierten Fontnamen abhängig.

## Font-Quellen

Fonts dürfen stammen aus:

```text
System Fonts
User Fonts
Application Fonts
Solution Fonts
Document-Embedded Fonts
Temporary Fonts
Remote Font Providers
```

Quelle, Scope und Lebenszyklus müssen unterscheidbar bleiben.

## Registrierung

```text
Discover
   ↓
Validate
   ↓
Parse Metadata
   ↓
Assign Identity
   ↓
Register
   ↓
Available
```

Registrierung bedeutet nicht automatisch, dass ein Font global sichtbar oder vertrauenswürdig ist.

## Scopes

Fonts dürfen in unterschiedlichen Scopes registriert werden:

```text
System
User
Session
Application
Solution
Document
Temporary
```

Ein lokaler Font darf keinen gleichnamigen globalen Font unkontrolliert ersetzen.

## Auswahl

Font-Auswahl berücksichtigt mindestens:

```text
Requested Family
Style
Weight
Width
Slant
Unicode Coverage
Script
Language
Variation Axes
Policy
Scope
```

Das Ergebnis ist eine konkrete `FontID`.

## Font Fallback

Fehlen benötigte Glyphen:

```text
Requested Font
      ↓
Coverage Check
      ↓
Missing Glyph
      ↓
Fallback Resolution
      ↓
Compatible Font
```

Fallback soll Grapheme- und Shaping-Sequenzen möglichst zusammenhalten.

## Variable Fonts

Variable Fonts müssen über definierte Achsen unterstützt werden können:

```text
Weight
Width
Optical Size
Slant
Custom Axes
```

Achsenwerte müssen validiert und auf unterstützte Bereiche begrenzt werden.

## Cache

Der Font Manager darf abgeleitete Daten cachen:

```text
Metadata
Coverage
Fallback Decisions
Shaping Data
Font Tables
```

Caches sind rekonstruierbar und nicht Source of Truth.

## Sicherheit

Fontdateien sind potenziell nicht vertrauenswürdige Binärdaten.

Sie müssen vor systemweiter Verwendung kontrolliert geparst und validiert werden.

Fehlerhafte Fonts dürfen nicht die Stabilität anderer Prozesse oder des Textsystems gefährden.

## Änderungen

Installation, Entfernung oder Aktualisierung eines Fonts muss die betroffenen Registrierungen und Caches konsistent invalidieren.

Bereits aktive Textoperationen dürfen definierte Font-Handles bis zum sicheren Abschluss weiterverwenden können.

## Normative Anforderungen

1. NovaOS MUSS einen zentralen Font Manager bereitstellen.
2. Fonts MÜSSEN stabile interne `FontID`s besitzen.
3. Fontdatei, Font Face und Font Family MÜSSEN getrennte Konzepte bleiben.
4. System-, User-, Application-, Solution- und Document-Fonts MÜSSEN unterschiedliche Scopes besitzen können.
5. Fonts MÜSSEN vor Registrierung validiert werden.
6. Font Discovery DARF keine zusätzliche Authority erzeugen.
7. Font-Auswahl MUSS Unicode Coverage berücksichtigen können.
8. Font Fallback MUSS systemweit unterstützt werden.
9. Variable Fonts MÜSSEN unterstützt werden können.
10. Lokale Fonts DÜRFEN globale Fonts nicht unkontrolliert überschreiben.
11. Font-Caches MÜSSEN rekonstruierbar und invalidierbar sein.
12. FontID, Quelle, Scope, Coverage, Features, Variation Axes und Zustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TEXT-UNICODE-0001`
- `NPSPEC-TEXT-GRAPHEME-0001`
- `NPSPEC-TEXT-SCRIPT-0001`
- `NPSPEC-TEXT-SHAPING-0001`
- `NPSPEC-TEXT-SECURITY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine zentrale Font-Infrastruktur, die Schriftressourcen unabhängig von Anwendung und Speicherort verwaltet. Fonts können sicher registriert, nach Scope getrennt, anhand ihrer Eigenschaften ausgewählt und über ein einheitliches Fallback-System für Shaping, Layout und Rendering bereitgestellt werden.