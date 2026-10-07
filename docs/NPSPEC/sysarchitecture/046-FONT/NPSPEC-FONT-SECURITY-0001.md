# NPSPEC-FONT-SECURITY-0001 – Nova Font Security

## Status

Angenommen

## Kategorie

Font / Security

## Zweck

NovaOS definiert Sicherheitsanforderungen für das Laden, Validieren, Verarbeiten und Rendern von Fonts.

Fontdateien gelten grundsätzlich als potenziell nicht vertrauenswürdige Binärdaten und dürfen die Sicherheit oder Stabilität des Systems nicht gefährden.

## Grundprinzipien

```text
Valid Font ≠ Trusted Font
Installed Font ≠ Trusted Font
Font Discovery ≠ Authority
Font Name ≠ Font Identity
Font Metadata ≠ Trusted Metadata
Font Rendering ≠ Code Execution
```

## Sicherheitsmodell

```text
Font Source
    ↓
Format Detection
    ↓
Structural Validation
    ↓
Security Validation
    ↓
Font Parser
    ↓
Font Registry
    ↓
Shaping / Rendering
```

Ungültige Fonts werden vor der normalen Verwendung abgewiesen oder isoliert.

## Vertrauensgrenzen

Fonts können aus unterschiedlichen Quellen stammen:

```text
System
User
Application
Solution
Document
Remote
Temporary
```

Quelle und Installationsort dürfen nicht automatisch Vertrauen erzeugen.

Systemfonts dürfen strengeren Integritäts- und Signaturanforderungen unterliegen.

## Parser-Isolation

Komplexe Fontformate sollen außerhalb hochprivilegierter Komponenten verarbeitet werden.

Parser müssen gegen mindestens folgende Fehlerklassen geschützt sein:

```text
Malformed Tables
Invalid Offsets
Integer Overflow
Buffer Overrun
Recursive Structures
Excessive Allocation
Decompression Bombs
Pathological Glyph Complexity
```

Ein Parserfehler darf nicht zur Kompromittierung des Kernels oder anderer Prozesse führen.

## Ressourcenbegrenzung

Fontverarbeitung muss begrenzbar sein:

```text
File Size
Table Size
Glyph Count
Contour Complexity
Layer Count
Bitmap Size
SVG Complexity
Variation Complexity
Memory
CPU Time
```

Überschreitungen müssen kontrolliert fehlschlagen.

## Font-Identität

Angezeigte Namen wie:

```text
Family Name
Full Name
PostScript Name
Style Name
```

dürfen nicht als sichere Identität verwendet werden.

NovaOS verwendet intern `FontID` und validierte Herkunftsinformationen.

## Color Glyphs

SVG-, Bitmap- und Layer-basierte Glyphen gelten ebenfalls als nicht vertrauenswürdiger Fontinhalt.

Insbesondere SVG-Glyphen dürfen nicht implizit:

```text
Scripts ausführen
Netzwerkzugriffe durchführen
Dateien öffnen
externe Ressourcen laden
Systemfähigkeiten anfordern
```

## Font Cache

Caches dürfen ausschließlich validierte oder eindeutig als nicht vertrauenswürdig markierte Fontdaten enthalten.

Cache-Einträge müssen an die relevante Fontversion beziehungsweise Inhaltsidentität gebunden sein.

Manipulierte oder veraltete Cache-Daten müssen invalidiert werden können.

## Updates und Austausch

Wird eine Fontressource verändert, muss ihre Integrität erneut bewertet werden.

```text
Font Change
    ↓
Invalidate
    ↓
Revalidate
    ↓
Update Registry
    ↓
Invalidate Derived Caches
```

Ein Austausch darf nicht allein aufgrund identischer Fontnamen als dieselbe vertrauenswürdige Ressource gelten.

## Fehlerbehandlung

Fehlerhafte Fonts dürfen nicht die gesamte Textdarstellung blockieren.

```text
Invalid Font
    ↓
Reject / Isolate
    ↓
Fallback Font
```

Soweit möglich wird auf einen sicheren Font zurückgefallen.

## Normative Anforderungen

1. NovaOS MUSS Fontdateien als potenziell nicht vertrauenswürdige Eingaben behandeln.
2. Fonts MÜSSEN vor ihrer Verwendung strukturell validiert werden.
3. Fontparser SOLLEN von hochprivilegierten Systemkomponenten isoliert sein.
4. Parserfehler DÜRFEN Kernel oder andere Prozesse nicht kompromittieren.
5. CPU-, Speicher- und Komplexitätsverbrauch der Fontverarbeitung MUSS begrenzbar sein.
6. Fontnamen DÜRFEN nicht als sichere Fontidentität verwendet werden.
7. Fontquelle und Installationsort DÜRFEN nicht automatisch Vertrauen erzeugen.
8. Color-Glyph-Inhalte MÜSSEN denselben Sicherheitsgrenzen unterliegen.
9. Fonts DÜRFEN keine implizite Codeausführung oder externe Ressourcenauflösung auslösen.
10. Änderungen an Fonts MÜSSEN eine erneute Validierung auslösen können.
11. Fehlerhafte Fonts SOLLEN durch sicheren Fallback ersetzt werden können.
12. Validierungsstatus, Herkunft, FontID, Integrität, Isolation und Fehlergrund MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FONT-MANAGER-0001`
- `NPSPEC-FONT-FALLBACK-0001`
- `NPSPEC-FONT-VARIABLE-0001`
- `NPSPEC-FONT-COLORGLYPH-0001`
- `NPSPEC-TEXT-SECURITY-0001`
- `NPSPEC-TRUST-SYSTEMCOMPONENT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS behandelt Fonts als klar abgegrenzte, potenziell nicht vertrauenswürdige Ressourcen. Validierung, Isolation, Ressourcenlimits, stabile Fontidentitäten und sichere Fallbacks verhindern, dass manipulierte oder fehlerhafte Fonts die Sicherheit und Stabilität des Text- und Grafiksystems gefährden.