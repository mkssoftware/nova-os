# NPSPEC-FONT-VARIABLE-0001 – Nova Variable Fonts

## Status

Angenommen

## Kategorie

Font / Variable Fonts

## Zweck

NovaOS definiert die systemweite Unterstützung variabler Fonts, bei denen mehrere Schriftvarianten innerhalb einer Fontressource über kontinuierliche oder diskrete Variationsachsen erzeugt werden.

Variable Fonts werden in Font Manager, Fallback, Shaping, Layout und Rendering integriert.

## Grundprinzipien

```text
Variable Font ≠ Static Font Collection
Variation Axis ≠ Font Style
Axis Value ≠ Font Identity
Font Instance ≠ Separate Font File
Variation ≠ Text Transformation
```

## Modell

```text
VariableFont
├── FontID
├── Axes[]
├── NamedInstances[]
├── DefaultCoordinates
└── SupportedFeatures
```

Eine Achse besitzt mindestens:

```text
VariationAxis
├── AxisTag
├── Minimum
├── Default
├── Maximum
└── CurrentValue
```

## Standardachsen

NovaOS muss bekannte OpenType-Variationsachsen unterstützen können:

```text
wght → Weight
wdth → Width
slnt → Slant
ital → Italic
opsz → Optical Size
```

Fonts dürfen zusätzliche herstellerspezifische Achsen bereitstellen.

## Font-Instanz

Eine konkrete Darstellung entsteht aus Font und Variationskoordinaten:

```text
FontID
  +
Axis Coordinates
  ↓
Font Instance
```

Beispiel:

```text
FontID: NovaSans
wght: 550
wdth: 95
opsz: 14
```

Eine solche Instanz benötigt keine eigene Fontdatei oder neue `FontID`.

## Wertebereiche

Achsenwerte müssen gegen die vom Font definierten Bereiche validiert werden.

```text
Minimum ≤ Value ≤ Maximum
```

Ungültige Werte dürfen nicht ungeprüft an Shaping oder Rendering weitergegeben werden.

## Named Instances

Variable Fonts dürfen vordefinierte Instanzen bereitstellen:

```text
Regular
Medium
Semibold
Bold
Condensed
```

Named Instances sind benannte Koordinatensätze und keine eigenständigen Fontidentitäten.

## Shaping

Variationskoordinaten müssen an die Shaping-Schicht übergeben werden können.

```text
Text
 + FontID
 + Variation Coordinates
        ↓
      Shaping
        ↓
Variable Glyph Run
```

Änderungen von Achsen können Glyphenform, Metriken und Positionierung beeinflussen und erfordern gegebenenfalls erneutes Shaping.

## Optical Size

Unterstützt ein Font `opsz`, darf NovaOS die optische Größe automatisch aus der effektiven Textgröße ableiten.

Automatische Auswahl muss durch explizite Benutzer- oder Anwendungswerte überschreibbar sein.

## Fallback

Font Fallback soll relevante Variationswerte soweit möglich auf den Fallback-Font übertragen.

Nicht unterstützte Achsen werden nicht künstlich emuliert, sofern keine explizite Policy dies vorsieht.

## Animation

Variationsachsen dürfen animiert werden, sofern Font, Renderer und UI-Kontext dies unterstützen.

Animationen müssen Ressourcen- und Energieanforderungen berücksichtigen.

## Cache

Caches müssen Variationskoordinaten berücksichtigen.

```text
FontID
+
Variation Coordinates
+
Shaping Context
=
Cache Identity
```

Unterschiedliche relevante Koordinaten dürfen nicht fälschlich denselben Glyphen- oder Shaping-Cache verwenden.

## Normative Anforderungen

1. NovaOS MUSS variable Fonts unterstützen können.
2. Variationsachsen MÜSSEN über stabile Axis Tags adressierbar sein.
3. Minimum, Default und Maximum einer Achse MÜSSEN berücksichtigt werden.
4. Benutzerdefinierte Achsen MÜSSEN unterstützt werden können.
5. Eine Font-Instanz DARF keine neue `FontID` erfordern.
6. Named Instances MÜSSEN als Koordinatensätze behandelbar sein.
7. Variationskoordinaten MÜSSEN an Shaping und Rendering übergeben werden können.
8. Änderungen metrisch relevanter Achsen MÜSSEN erneutes Layout oder Shaping auslösen können.
9. `opsz` MUSS automatisch und explizit steuerbar sein.
10. Fallback SOLL kompatible Variationswerte übernehmen können.
11. Caches MÜSSEN relevante Variationskoordinaten berücksichtigen.
12. Achsen, Wertebereiche, aktuelle Koordinaten und Named Instances MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FONT-MANAGER-0001`
- `NPSPEC-FONT-FALLBACK-0001`
- `NPSPEC-TEXT-SHAPING-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS behandelt variable Fonts als dynamisch parametrisierbare Fontressourcen. Gewicht, Breite, Neigung, optische Größe und benutzerdefinierte Achsen können ohne separate Fontdateien kontrolliert werden und bleiben konsistent in Font-Auswahl, Shaping, Fallback, Layout und Rendering integriert.