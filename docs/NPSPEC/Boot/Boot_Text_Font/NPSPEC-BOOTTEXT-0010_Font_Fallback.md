# NPSPEC-BOOTTEXT-0010
# Font Fallback

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTTEXT-0010 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Text System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTTEXT-0003, NPSPEC-BOOTTEXT-0004, NPSPEC-BOOTTEXT-0005, NPSPEC-BOOTFONT-0001, NPSPEC-BOOTTHEME-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Font Fallback System** der NovaOS-Bootoberfläche.

Das Font Fallback System stellt sicher, dass jeder Unicode-Codepunkt auch dann dargestellt werden kann, wenn die primäre Schriftart keine passende Glyphe enthält. Hierzu wird eine definierte Kette alternativer Schriftarten durchsucht, bis eine geeignete Darstellung gefunden wird.

Das Verhalten muss deterministisch und unabhängig von BIOS, UEFI oder Plattform sein.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- sämtliche UI-Texte
- Dialoge
- Buttons
- Listen
- Menüs
- Tooltips
- Statusanzeigen
- Recovery
- Installer
- Diagnosemodule

---

# 3. Ziele

Das Font-Fallback-System muss:

- Unicode vollständig unterstützen
- deterministisch arbeiten
- mehrere Fallback-Ebenen unterstützen
- Themeintegration besitzen
- hohe Performance bieten
- konsistente Darstellung gewährleisten

---

# 4. Architektur

```text
Unicode Code Point

↓

Primary Font

↓

Fallback Resolver

↓

Fallback Chain

↓

Glyph Resolver

↓

Glyph Rasterizer

↓

Renderer
```

---

# 5. Komponenten

Das Font-Fallback-System besteht aus:

- Font Resolver
- Fallback Manager
- Font Registry
- Unicode Resolver
- Glyph Resolver
- Missing Glyph Handler

---

# 6. Grundprinzip

Der Ablauf erfolgt in mehreren Schritten:

```text
Unicode

↓

Primärfont

↓

Glyph vorhanden?

↓

Ja

↓

Rendering

↓

Nein

↓

Fallback Font

↓

Rendering
```

Die Suche endet beim ersten gültigen Treffer.

---

# 7. Fallback-Kette

Die Standardsuche erfolgt in folgender Reihenfolge:

```text
Primärfont

↓

Theme-Fallback

↓

Systemfont

↓

Universeller Unicode-Font

↓

Missing Glyph
```

Die Reihenfolge ist systemweit fest definiert.

---

# 8. Fontprioritäten

Jeder Font besitzt:

- Priorität
- Sprachunterstützung
- Unicode-Abdeckung
- Version

Bei mehreren Treffern gewinnt der Font mit der höchsten Priorität.

---

# 9. Unicode-Abdeckung

Jeder Font beschreibt:

- unterstützte Unicode-Bereiche
- unterstützte Scripts
- verfügbare Glyphen

Die Auswahl erfolgt ausschließlich anhand dieser Informationen.

---

# 10. Missing Glyph

Kann keine passende Glyphe gefunden werden:

```text
U+FFFD
```

oder

```text
Missing Glyph Symbol
```

Die Darstellung wird niemals abgebrochen.

---

# 11. Sprachunterstützung

Das System berücksichtigt:

- Sprache
- Region
- Schriftfamilie
- Unicode-Script

Dadurch können sprachspezifische Fallbacks verwendet werden.

---

# 12. Themeintegration

Das Theme definiert:

- Primärfont
- Standard-Fallback
- Monospace-Fallback
- Symbolfont

Themes dürfen keine feste Unicode-Zuordnung vorgeben.

---

# 13. Glyph Cache

Nach erfolgreicher Auflösung wird die Glyphe im Glyph Atlas gespeichert.

Weitere Zugriffe verwenden ausschließlich den Cache.

Die Fallback-Suche wird nicht erneut durchgeführt.

---

# 14. Fontwechsel

Ändert sich der Primärfont:

```text
Themewechsel

↓

Neuer Primärfont

↓

Fallback bleibt erhalten

↓

Layout aktualisieren
```

Bereits geladene Fallback-Fonts bleiben registriert.

---

# 15. Internationalisierung

Unterstützt werden:

- Lateinisch
- Kyrillisch
- Griechisch
- Arabisch (optional)
- Hebräisch (optional)
- CJK
- Hangul
- Hiragana
- Katakana

Neue Schriftsysteme können ergänzt werden, ohne den Resolver zu ändern.

---

# 16. Accessibility

Accessibility verwendet dieselbe Fallback-Kette.

Screen Reader erhalten stets den ursprünglichen Unicode-Codepunkt und niemals die Information, welcher Font verwendet wurde.

---

# 17. Sicherheit

Das System verhindert:

- rekursive Fallback-Ketten
- doppelte Fontreferenzen
- beschädigte Fonttabellen
- ungültige Unicode-Zuordnungen
- Endlosschleifen

Alle Fallback-Beziehungen werden beim Systemstart validiert.

---

# 18. Speicherverwaltung

Das Font-Fallback-System verwendet:

- statische Fonttabellen
- Glyph-Pools
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Font Lookup
- O(1)-Unicode Lookup
- O(1)-Glyph Cache Lookup
- O(k)-Fallback-Suche (k = Anzahl der konfigurierten Fallback-Schriften)

Die Anzahl der Fallback-Schritte muss klein und fest begrenzt sein.

---

# 20. API

```c
typedef struct
{
    const nova_font_t* primary;
    const nova_font_t* fallback;
}
nova_font_resolution_t;

nova_result_t
nova_font_fallback_initialize(void);

const nova_font_t*
nova_font_resolve(
    uint32_t codepoint,
    const nova_font_t* preferred);

bool
nova_font_has_glyph(
    const nova_font_t* font,
    uint32_t codepoint);

const nova_font_resolution_t*
nova_font_last_resolution(void);
```

---

# 21. Diagnosedaten

Zu erfassen sind:

- Primärfont
- verwendeter Fallbackfont
- Unicode-Codepunkt
- Anzahl der Fallback-Schritte
- Missing-Glyph-Ereignisse
- Cache-Hits
- Fehler

---

# 22. Testfälle

Zu testen sind:

- vollständige Glyphenabdeckung
- fehlende Glyphen
- Unicode-Symbole
- CJK-Schriften
- Themewechsel
- Glyph Cache
- Missing Glyph
- Mehrsprachigkeit
- Fehlerfälle

---

# 23. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- jede verfügbare Glyphe über die Fallback-Kette gefunden wird.
- Missing Glyphs ausschließlich als letzte Option verwendet werden.
- die Fallback-Reihenfolge deterministisch ist.
- Glyphen nach erfolgreicher Auflösung zwischengespeichert werden.
- Themewechsel korrekt verarbeitet werden.
- keine rekursiven Fallback-Ketten entstehen.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 24. Referenzen

## ADRs

- ADR-BOOTTEXT-0010 – Font Fallback
- ADR-BOOTFONT-0001 – Font Management
- ADR-BOOTTHEME-0001 – Theme System
- ADR-BOOTTEXT-0003 – Unicode Character Support

## NPSPECs

- NPSPEC-BOOTTEXT-0003 – Unicode Character Support
- NPSPEC-BOOTTEXT-0004 – Font Loading
- NPSPEC-BOOTTEXT-0005 – Glyph Rasterization
- NPSPEC-BOOTFONT-0001 – Font Management
- NPSPEC-BOOTTHEME-0001 – Theme System

---

# 25. Zusammenfassung

Das **Font Fallback System** definiert den standardisierten Mechanismus zur Auswahl alternativer Schriftarten innerhalb der NovaOS-Bootoberfläche. Es stellt sicher, dass Unicode-Zeichen auch dann korrekt dargestellt werden, wenn der primäre Font keine passende Glyphe enthält. Durch eine deterministische Fallback-Kette, Unicode-basierte Fontauswahl, Integration in den Glyph Atlas sowie statische Speicherverwaltung gewährleistet das System eine konsistente, internationale und performante Textdarstellung auf BIOS- und UEFI-basierten Systemen.