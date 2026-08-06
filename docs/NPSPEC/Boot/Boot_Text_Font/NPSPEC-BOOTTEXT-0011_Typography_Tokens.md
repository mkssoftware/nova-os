# NPSPEC-BOOTTEXT-0011
# Typography Tokens

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTTEXT-0011 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Text System |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTTEXT-0001, NPSPEC-BOOTTEXT-0004, NPSPEC-BOOTTEXT-0008, NPSPEC-BOOTTHEME-0001, NPSPEC-BOOTCONTROL-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Typography Token System** der NovaOS-Bootoberfläche.

Typography Tokens stellen eine zentrale, semantische Beschreibung sämtlicher typografischer Eigenschaften bereit. UI-Komponenten verwenden ausschließlich diese Tokens und greifen niemals direkt auf Schriftgrößen, Schriftarten oder Farben zu.

Dadurch bleibt die gesamte Bootoberfläche konsistent, leicht wartbar und vollständig durch Themes steuerbar.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Labels
- Buttons
- Dialoge
- Listen
- Menüs
- Statusanzeigen
- Tooltips
- Recovery
- Installer
- Diagnosemodule

---

# 3. Ziele

Das Typography-System muss:

- semantisch arbeiten
- themegesteuert sein
- konsistente Typografie gewährleisten
- DPI-unabhängig sein
- deterministisch arbeiten
- zukünftige Erweiterungen unterstützen

---

# 4. Architektur

```text
Theme

↓

Typography Tokens

↓

Control Style

↓

Text Layout

↓

Renderer
```

---

# 5. Komponenten

Das Typography-System besteht aus:

- Typography Registry
- Token Resolver
- Theme Integration
- Font Resolver
- Style Engine
- Validation Engine

---

# 6. Grundprinzip

Ein Control verwendet niemals feste Schriftparameter.

Stattdessen:

```text
Label

↓

Typography Token

↓

Theme

↓

Font

↓

Layout
```

Alle typografischen Eigenschaften stammen aus dem Token.

---

# 7. Tokenstruktur

Ein Typography Token enthält mindestens:

- Token-ID
- Fontfamilie
- Schriftgröße
- Schriftstil
- Schriftgewicht
- Zeilenhöhe
- Letter Spacing
- Textfarbe

Optional:

- Kerning
- Schatten
- Outline

---

# 8. Standardtokens

Pflichttokens:

```text
Display

Title

Headline

Subtitle

Body

BodySmall

Caption

Button

Menu

Status

Monospace
```

Weitere Tokens können ergänzt werden.

---

# 9. Fontfamilien

Ein Token verweist ausschließlich auf eine logische Fontfamilie.

Beispiele:

```text
System

Monospace

Display

Symbols
```

Die tatsächliche Fontdatei wird durch das Theme bestimmt.

---

# 10. Schriftgrößen

Größen werden ausschließlich als logische Tokens definiert.

Beispiel:

```text
Display

↓

36 DIU

Title

↓

28 DIU

Body

↓

16 DIU
```

Physische Pixelwerte werden nicht gespeichert.

---

# 11. Schriftgewicht

Unterstützt werden:

- Light
- Regular
- Medium
- SemiBold
- Bold

Die Zuordnung erfolgt durch den Font Resolver.

---

# 12. Zeilenhöhe

Jeder Token definiert:

- Standard-Line Height
- Minimalhöhe
- maximale Höhe (optional)

Die Zeilenhöhe wird unabhängig vom Font berechnet.

---

# 13. Letter Spacing

Jeder Token besitzt:

- Standardwert
- optionalen Überschreibungswert

Das Theme kann Letter Spacing anpassen.

---

# 14. Themeintegration

Alle Typography Tokens werden vollständig vom Theme bereitgestellt.

Ein Themewechsel erzeugt automatisch neue Tokenwerte.

UI-Komponenten bleiben unverändert.

---

# 15. Accessibility

Accessibility kann überschreiben:

- Schriftgröße
- Zeilenhöhe
- Letter Spacing

Die Tokenstruktur bleibt erhalten.

---

# 16. Versionierung

Jeder Token besitzt:

- Version
- Änderungszeitpunkt
- Theme-ID

Ungültige Token werden beim Laden verworfen.

---

# 17. Sicherheit

Das System verhindert:

- doppelte Token-IDs
- ungültige Schriftgrößen
- rekursive Tokenreferenzen
- fehlende Fontfamilien
- inkonsistente Themezuordnungen

Alle Tokens werden beim Theme-Laden validiert.

---

# 18. Speicherverwaltung

Das Typography-System verwendet:

- statische Tokentabellen
- Theme-Pools
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Token Lookup
- O(1)-Font Lookup
- O(1)-Theme Lookup
- deterministische Laufzeit

Alle Tokens werden beim Theme-Laden vorbereitet.

---

# 20. API

```c
typedef enum
{
    NOVA_TYPOGRAPHY_DISPLAY,
    NOVA_TYPOGRAPHY_TITLE,
    NOVA_TYPOGRAPHY_HEADLINE,
    NOVA_TYPOGRAPHY_SUBTITLE,
    NOVA_TYPOGRAPHY_BODY,
    NOVA_TYPOGRAPHY_BODY_SMALL,
    NOVA_TYPOGRAPHY_CAPTION,
    NOVA_TYPOGRAPHY_BUTTON,
    NOVA_TYPOGRAPHY_MENU,
    NOVA_TYPOGRAPHY_STATUS,
    NOVA_TYPOGRAPHY_MONOSPACE
}
nova_typography_token_t;

const nova_typography_style_t*
nova_typography_get(
    nova_typography_token_t token);

bool
nova_typography_exists(
    nova_typography_token_t token);

nova_result_t
nova_typography_reload(void);
```

---

# 21. Diagnosedaten

Zu erfassen sind:

- Token-ID
- Theme-ID
- Fontfamilie
- Schriftgröße
- Schriftgewicht
- Zeilenhöhe
- Ladezeit
- Fehler

---

# 22. Testfälle

Zu testen sind:

- sämtliche Standardtokens
- Themewechsel
- Fontwechsel
- Accessibility
- große Schriftgrößen
- Letter Spacing
- Versionierung
- Fehlerfälle

---

# 23. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- alle UI-Komponenten ausschließlich Typography Tokens verwenden.
- sämtliche Token durch Themes bereitgestellt werden.
- Themewechsel ohne Änderungen an Controls funktionieren.
- Accessibility Token korrekt überschreiben kann.
- Fontparameter nicht direkt in Controls gespeichert werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 24. Referenzen

## ADRs

- ADR-BOOTTEXT-0011 – Typography Tokens
- ADR-BOOTTHEME-0001 – Theme System
- ADR-BOOTFONT-0001 – Font Management
- ADR-BOOTTEXT-0008 – Text Layout

## NPSPECs

- NPSPEC-BOOTTEXT-0001 – Boot Text Engine
- NPSPEC-BOOTTEXT-0004 – Font Loading
- NPSPEC-BOOTTEXT-0008 – Text Layout
- NPSPEC-BOOTTHEME-0001 – Theme System
- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework

---

# 25. Zusammenfassung

Das **Typography Token System** definiert die semantische Verwaltung sämtlicher typografischer Eigenschaften innerhalb der NovaOS-Bootoberfläche. Es entkoppelt UI-Komponenten vollständig von konkreten Schriftparametern und ermöglicht eine zentrale Steuerung von Schriftarten, Größen, Zeilenhöhen und weiteren Typografieeigenschaften über Themes. Durch deterministische Tokenauflösung, vollständige Accessibility-Integration und statische Speicherverwaltung gewährleistet das System eine konsistente, flexible u