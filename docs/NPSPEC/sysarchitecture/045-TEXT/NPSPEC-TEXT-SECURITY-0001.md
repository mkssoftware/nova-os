# NPSPEC-TEXT-SECURITY-0001 – Nova Text Security

## Status

Angenommen

## Kategorie

Text / Security

## Zweck

NovaOS definiert systemweite Sicherheitsregeln für die Verarbeitung, Anzeige und Bewertung von Unicode-Text.

Ziel ist der Schutz vor visueller Täuschung, mehrdeutiger Textinterpretation, Encoding-Angriffen und manipulierten Unicode-Sequenzen, ohne legitime mehrsprachige Inhalte einzuschränken.

## Grundprinzipien

```text
Displayed Text ≠ Identity
Visual Equality ≠ Semantic Equality
Search Match ≠ Identity Match
Collation Equality ≠ Identity Equality
Valid Unicode ≠ Safe Identifier
Normalization ≠ Security
Unusual Text ≠ Malicious Text
```

## Sicherheitsmodell

```text
External Text
     ↓
Encoding Validation
     ↓
Unicode Validation
     ↓
Security Analysis
     ↓
Context Policy
     ↓
Accept / Warn / Annotate / Reject
```

Die Behandlung hängt vom jeweiligen Sicherheitskontext ab.

## Relevante Risiken

NovaOS muss mindestens folgende Unicode-Risiken erkennen können:

```text
Confusable Characters
Mixed-Script Spoofing
BiDi Manipulation
Invisible Characters
Control Characters
Unexpected Combining Marks
Normalization Differences
Malformed Encoding
Identifier Spoofing
Excessive Text Complexity
```

## Confusables

Visuell ähnliche Zeichen können unterschiedliche Codepoints besitzen.

Beispiel:

```text
Latin:    a
Cyrillic: а
```

NovaOS darf daraus keine automatische Identitätsgleichheit ableiten.

Sicherheitskritische Oberflächen sollen verdächtige Confusables hervorheben können.

## Mixed Scripts

Mehrere Scripts innerhalb eines Textes sind grundsätzlich erlaubt.

```text
Mixed Script ≠ Attack
```

In sicherheitskritischen Identifikatoren darf jedoch eine strengere Policy angewendet werden.

Beispiele:

```text
User Names
Domains
Package Names
Publisher Names
Security Principals
Commands
```

## BiDi-Sicherheit

BiDi-Steuerzeichen können die visuelle Reihenfolge verändern.

Sicherheitsrelevante Kontexte müssen insbesondere unsichtbare Direction Controls erkennen können.

```text
Logical Text
     ↓
BiDi Controls
     ↓
Different Visual Appearance
```

Quellcode, Dateinamen, URLs, Logs und Sicherheitsdialoge dürfen verdächtige Steuerzeichen markieren oder explizit darstellen.

## Unsichtbare Zeichen

Unsichtbare Zeichen dürfen analysiert werden, darunter:

```text
Zero Width Characters
Directional Controls
Variation Selectors
Join Controls
Non-Printing Controls
```

Ihre bloße Existenz ist kein Fehler.

Die Bewertung erfolgt kontextabhängig.

## Identifikatoren

Sicherheitskritische Identitäten dürfen nicht ausschließlich über benutzersichtbaren Text definiert werden.

```text
Display Name
    ≠
Stable Identity
```

NovaOS verwendet dafür stabile IDs wie:

```text
ObjectID
CapabilityID
AppID
SolutionID
PrincipalID
```

Text bleibt Darstellung oder benutzerbezogene Benennung.

## Normalisierung

Normalisierung darf für definierte Sicherheitsvergleiche verwendet werden, ersetzt jedoch keine Identitätsprüfung.

Unterschiedliche Komponenten an derselben Sicherheitsgrenze müssen dieselbe Normalisierungs- und Vergleichssemantik verwenden.

## Ressourcenbegrenzung

Textverarbeitung muss gegen pathologische Eingaben geschützt sein.

Grenzen dürfen insbesondere gelten für:

```text
Input Size
Combining Sequence Length
Segmentation Complexity
Shaping Complexity
Normalization Work
Search Complexity
```

Überschreitungen müssen kontrolliert behandelt werden.

## Policy

Text Security darf kontextabhängige Profile verwenden:

```text
General Text
User Interface
Identifier
Source Code
Command
URL
Security Critical
```

Ein allgemeiner Texteditor soll legitimen Unicode nicht unnötig einschränken, während sicherheitskritische Identifikatoren strengere Regeln verwenden dürfen.

## Normative Anforderungen

1. NovaOS MUSS Unicode-spezifische Sicherheitsrisiken systemweit behandeln können.
2. Gültiger Unicode DARF nicht automatisch als sicherer Identifikator gelten.
3. Confusable Characters MÜSSEN erkennbar sein können.
4. Mixed-Script-Inhalte MÜSSEN analysierbar sein.
5. Mixed Scripts DÜRFEN nicht pauschal verboten werden.
6. BiDi-Steuerzeichen MÜSSEN in sicherheitskritischen Kontexten erkennbar sein.
7. Unsichtbare Zeichen MÜSSEN kontextabhängig bewertbar sein.
8. Textvergleich DARF stabile Systemidentitäten nicht ersetzen.
9. Sicherheitsgrenzen MÜSSEN konsistente Encoding-, Normalisierungs- und Vergleichsregeln verwenden.
10. Textverarbeitung MUSS gegen pathologisch komplexe Eingaben begrenzbar sein.
11. Security Policies MÜSSEN je nach Textkontext unterschiedlich streng sein können.
12. erkannte Risiken, angewendete Policy und resultierende Entscheidung MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TEXT-UNICODE-0001`
- `NPSPEC-TEXT-UTF8-0001`
- `NPSPEC-TEXT-ENCODING-0001`
- `NPSPEC-TEXT-NORMALIZATION-0001`
- `NPSPEC-TEXT-GRAPHEME-0001`
- `NPSPEC-TEXT-SEGMENTATION-0001`
- `NPSPEC-TEXT-BIDI-0001`
- `NPSPEC-TEXT-SCRIPT-0001`
- `NPSPEC-TEXT-COLLATION-0001`
- `NPSPEC-SECURITY-INFORMATIONFLOW-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine gemeinsame Sicherheitsarchitektur für Unicode-Text. Visuelle Darstellung, Textvergleich und stabile Identität bleiben strikt getrennt, während Confusables, Mixed Scripts, BiDi-Manipulationen, unsichtbare Zeichen und pathologische Texte kontextabhängig erkannt und sicher behandelt werden können.