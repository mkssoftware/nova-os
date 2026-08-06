# NPSPEC-BOOTDESIGN-0004
# Typography System

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTDESIGN-0004 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Design System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTDESIGN-0002, NPSPEC-BOOTTEXT-0001, NPSPEC-BOOTTEXT-0004, NPSPEC-BOOTTEXT-0011, NPSPEC-BOOTRESOURCE-0009 |

---

# 1. Ziel

Diese Spezifikation definiert das **Typography System** der NovaOS-Bootoberfläche.

Das Typography System beschreibt sämtliche Regeln für Schriftarten, Schriftgrößen, Schriftschnitte, Zeilenabstände und Text-Hierarchien innerhalb der Bootumgebung. Ziel ist eine konsistente, gut lesbare und barrierearme Darstellung aller Texte.

Alle typografischen Eigenschaften werden ausschließlich über semantische Typography Tokens bereitgestellt.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Überschriften
- Dialogtitel
- Fließtexte
- Menüs
- Listen
- Buttons
- Statusanzeigen
- Fehlermeldungen
- Hinweise
- sämtliche UI-Komponenten

---

# 3. Ziele

Das Typography System muss:

- konsistente Typografie gewährleisten
- hohe Lesbarkeit sicherstellen
- Themewechsel unterstützen
- Accessibility unterstützen
- deterministisch arbeiten
- zukünftige Schriftarten unterstützen

---

# 4. Architektur

```text
Theme

↓

Typography Tokens

↓

Text Engine

↓

UI-Komponenten

↓

Renderer
```

---

# 5. Komponenten

Das System besteht aus:

- Typography Registry
- Font Resolver
- Typography Tokens
- Font Resources
- Text Layout Engine
- Accessibility Manager

---

# 6. Grundprinzip

Alle Komponenten verwenden ausschließlich semantische Typography Tokens.

Beispiel:

```text
Dialogtitel

↓

Typography.Title

↓

Theme

↓

Font Resource
```

Direkte Schriftgrößen oder Schriftarten in Komponenten sind unzulässig.

---

# 7. Typografische Rollen

Pflichtrollen:

```text
Display

Headline

Title

Subtitle

Body

Body.Small

Caption

Button

Menu

Status

Monospace
```

Jede Rolle besitzt definierte Eigenschaften.

---

# 8. Schriftfamilien

Unterstützt werden:

- UI Font
- Display Font
- Monospace Font
- Symbol Font
- Fallback Font

Die tatsächlichen Schriftdateien werden über Font Resources bereitgestellt.

---

# 9. Schriftschnitte

Unterstützt werden:

- Thin (optional)
- Light
- Regular
- Medium
- SemiBold
- Bold

Künstliche Fettschrift oder Kursivdarstellung ist nicht zulässig.

---

# 10. Schriftgrößen

Größen werden ausschließlich über Typography Tokens definiert.

Beispiel:

```text
Typography.Display

Typography.Title

Typography.Body

Typography.Caption
```

Die konkrete Punktgröße wird durch das Theme festgelegt.

---

# 11. Zeilenhöhe

Jede Typografierolle definiert:

- Mindestzeilenhöhe
- Standardzeilenhöhe
- maximale Zeilenhöhe

Zeilen dürfen sich nicht überlappen.

---

# 12. Zeichenabstand

Unterstützt werden:

- Standard
- Verdichtet
- Erweitert

Letter Spacing wird semantisch definiert.

---

# 13. Textausrichtung

Unterstützt werden:

- linksbündig
- zentriert
- rechtsbündig

Blocksatz wird innerhalb der Bootoberfläche nicht verwendet.

---

# 14. Lokalisierung

Das Typography System unterstützt:

- Unicode
- Font Fallback
- unterschiedliche Schriftsysteme
- sprachabhängige Schriftarten

Die Auswahl erfolgt automatisch über die Text Engine.

---

# 15. Accessibility

Das System unterstützt:

- größere Schriftgrößen
- hohe Lesbarkeit
- ausreichende Zeilenabstände
- klare Textkontraste

Texte müssen auch bei vergrößerter Darstellung vollständig lesbar bleiben.

---

# 16. Themeintegration

Themes dürfen verändern:

- Schriftarten
- Schriftgrößen
- Schriftschnitte
- Zeilenhöhen
- Letter Spacing

Die semantischen Rollen bleiben unverändert.

---

# 17. Sicherheit

Das System verhindert:

- fehlende Fonts
- ungültige Typography Tokens
- beschädigte Font Resources
- rekursive Fontreferenzen
- doppelte Typography IDs

Alle Schriftdefinitionen werden vor der Aktivierung validiert.

---

# 18. Speicherverwaltung

Das Typography System verwendet:

- statische Typography-Tabellen
- Font Registry
- Glyph Cache
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Typography Lookup
- O(1)-Font Lookup
- O(1)-Token Lookup
- deterministische Laufzeit

Typografiedaten werden beim Theme-Laden vorbereitet.

---

# 20. API

```c
typedef struct
{
    uint32_t font_id;
    uint32_t token_id;
    uint16_t size;
    uint16_t line_height;
}
nova_typography_style_t;

const nova_typography_style_t*
nova_typography_get(
    const char* token_name);

bool
nova_typography_exists(
    const char* token_name);

nova_result_t
nova_typography_reload(void);
```

---

# 21. Diagnosedaten

Zu erfassen sind:

- Theme-ID
- Anzahl Typography Tokens
- verwendete Fonts
- fehlende Glyphen
- Font-Fallbacks
- Ladezeit
- Validierungsfehler

---

# 22. Testfälle

Zu testen sind:

- Überschriften
- Fließtexte
- Buttons
- Menüs
- Statusanzeigen
- Themewechsel
- Font Fallback
- Unicode
- Accessibility
- Fehlerfälle

---

# 23. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Texte ausschließlich Typography Tokens verwenden.
- Themewechsel ohne Änderungen an UI-Komponenten möglich sind.
- Font Resources korrekt eingebunden werden.
- Unicode vollständig unterstützt wird.
- Font-Fallback zuverlässig funktioniert.
- Accessibility-Anforderungen erfüllt werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Erscheinungsbild erzeugen.

---

# 24. Referenzen

## ADRs

- ADR-BOOTDESIGN-0004 – Typography System
- ADR-BOOTDESIGN-0002 – Design Tokens
- ADR-BOOTTEXT-0001 – Text Engine
- ADR-BOOTRESOURCE-0009 – Font Resources

## NPSPECs

- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTDESIGN-0002 – Boot Design Tokens
- NPSPEC-BOOTTEXT-0001 – Boot Text Engine
- NPSPEC-BOOTTEXT-0004 – Font Loading
- NPSPEC-BOOTTEXT-0011 – Typography Tokens
- NPSPEC-BOOTRESOURCE-0009 – Font Resources

---

# 25. Zusammenfassung

Das **Typography System** definiert die semantische Typografie der NovaOS-Bootoberfläche. Es beschreibt Schriftrollen, Schriftarten, Größen, Zeilenhöhen und Abstände unabhängig von ihrer konkreten Darstellung und bindet diese über Typography Tokens und Font Resources ein. Durch konsistente Textregeln, Themeintegration, Unicode-Unterstützung, Font-Fallback und statische Speicherverwaltung gewährleistet das System eine hochwertige, barrierearme und langfristig stabile Textdarstellung auf BIOS- und UEFI-basierten Bootsystemen.