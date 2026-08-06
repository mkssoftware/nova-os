# NPSPEC-BOOTDESIGN-0002
# Boot Design Tokens

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTDESIGN-0002 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Design System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTTHEME-0001, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTLAYOUT-0001, NPSPEC-BOOTTEXT-0011 |

---

# 1. Ziel

Diese Spezifikation definiert das **Boot Design Token System** der NovaOS-Bootoberfläche.

Design Tokens bilden die kleinste semantische Einheit des Designsystems. Sie beschreiben Farben, Abstände, Typografie, Animationen, Transparenzen, Schatten, Radien und weitere visuelle Eigenschaften unabhängig von konkreten Implementierungen.

Alle UI-Komponenten greifen ausschließlich über Design Tokens auf Gestaltungsparameter zu.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Farben
- Typografie
- Layout
- Abstände
- Animationen
- Schatten
- Transparenzen
- Radien
- Icons
- sämtliche UI-Komponenten

---

# 3. Ziele

Das Design-Token-System muss:

- semantisch aufgebaut sein
- Themewechsel ermöglichen
- konsistente Gestaltung gewährleisten
- deterministisch arbeiten
- leicht erweiterbar sein
- Implementierungsdetails kapseln

---

# 4. Architektur

```text
Theme

↓

Design Tokens

↓

Control Styles

↓

UI-Komponenten

↓

Renderer
```

---

# 5. Komponenten

Das System besteht aus:

- Token Registry
- Token Resolver
- Theme Integration
- Validation Engine
- Version Manager
- Token Cache

---

# 6. Grundprinzip

UI-Komponenten verwenden ausschließlich Tokens.

Beispiel:

```text
Button

↓

Surface.Primary

↓

Theme

↓

Farbe
```

Direkte Farb- oder Größenangaben in Komponenten sind unzulässig.

---

# 7. Token-Kategorien

Pflichtkategorien:

- Colors
- Typography
- Spacing
- Radius
- Shadows
- Borders
- Motion
- Transparency
- Icons

Neue Kategorien können ergänzt werden.

---

# 8. Color Tokens

Beispiele:

```text
Color.Background

Color.Surface

Color.Primary

Color.Secondary

Color.Accent

Color.Success

Color.Warning

Color.Error

Color.Information

Color.Focus
```

Farbwerte werden ausschließlich im Theme definiert.

---

# 9. Typography Tokens

Verwendet werden:

- Display
- Title
- Headline
- Subtitle
- Body
- Caption
- Button
- Menu
- Status

Die Definition erfolgt in NPSPEC-BOOTTEXT-0011.

---

# 10. Spacing Tokens

Unterstützt werden:

```text
XS

S

M

L

XL

XXL
```

Konkrete Pixelwerte werden ausschließlich vom Theme festgelegt.

---

# 11. Radius Tokens

Beispiele:

```text
Radius.None

Radius.Small

Radius.Medium

Radius.Large

Radius.Pill
```

Alle Radien werden semantisch beschrieben.

---

# 12. Shadow Tokens

Unterstützt werden:

- Shadow.None
- Shadow.Small
- Shadow.Medium
- Shadow.Large
- Shadow.Dialog

Die Darstellung erfolgt durch die Rendering Engine.

---

# 13. Motion Tokens

Beispiele:

```text
Motion.Fast

Motion.Normal

Motion.Slow

Motion.Page

Motion.Dialog

Motion.Focus
```

Die konkrete Dauer wird durch das Theme festgelegt.

---

# 14. Transparency Tokens

Unterstützt werden:

- Transparent
- Surface
- Overlay
- Dialog
- Glass

Diese Tokens definieren ausschließlich Transparenzstufen.

---

# 15. Token Registry

Alle Tokens werden zentral registriert.

Jeder Token besitzt:

- Token-ID
- Kategorie
- Version
- Themewert

Die Registry dient als einzige Quelle für Tokenwerte.

---

# 16. Themeintegration

Themes dürfen ausschließlich Tokenwerte verändern.

Neue Komponenten bleiben dadurch automatisch kompatibel.

---

# 17. Accessibility

Accessibility kann überschreiben:

- Schriftgrößen
- Kontraste
- Fokusfarben
- Transparenzen

Die semantische Tokenstruktur bleibt unverändert.

---

# 18. Versionierung

Jeder Token besitzt:

- Version
- Änderungsdatum
- Theme-ID

Ungültige Tokens werden beim Laden verworfen.

---

# 19. Sicherheit

Das System verhindert:

- doppelte Token-IDs
- ungültige Kategorien
- rekursive Tokenreferenzen
- fehlende Themewerte
- beschädigte Tokentabellen

Alle Tokens werden beim Theme-Laden validiert.

---

# 20. Speicherverwaltung

Das Design-Token-System verwendet:

- statische Tokentabellen
- Theme-Pools
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 21. Performance

Anforderungen:

- O(1)-Token Lookup
- O(1)-Theme Lookup
- O(1)-Registry Lookup
- deterministische Laufzeit

Alle Token werden beim Theme-Laden vorbereitet.

---

# 22. API

```c
typedef enum
{
    NOVA_TOKEN_COLOR,
    NOVA_TOKEN_TYPOGRAPHY,
    NOVA_TOKEN_SPACING,
    NOVA_TOKEN_RADIUS,
    NOVA_TOKEN_SHADOW,
    NOVA_TOKEN_MOTION,
    NOVA_TOKEN_TRANSPARENCY,
    NOVA_TOKEN_ICON
}
nova_design_token_category_t;

const nova_design_token_t*
nova_design_token_get(
    const char* token_name);

bool
nova_design_token_exists(
    const char* token_name);

nova_result_t
nova_design_token_reload(void);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Tokenanzahl
- Theme-ID
- Tokenversion
- Kategorien
- Ladezeit
- Validierungsfehler

---

# 24. Testfälle

Zu testen sind:

- Farbtoken
- Typography Tokens
- Motion Tokens
- Themewechsel
- Accessibility
- Tokenversionierung
- fehlende Tokens
- doppelte Token-IDs
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche UI-Komponenten ausschließlich Design Tokens verwenden.
- Themewechsel ohne Änderungen an Komponenten möglich sind.
- semantische Kategorien vollständig unterstützt werden.
- Accessibility Token überschreiben kann.
- doppelte Token verhindert werden.
- keine festen Designwerte in UI-Komponenten gespeichert werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTDESIGN-0002 – Boot Design Tokens
- ADR-BOOTDESIGN-0001 – Boot Design Language
- ADR-BOOTTHEME-0001 – Theme System
- ADR-BOOTTEXT-0011 – Typography Tokens

## NPSPECs

- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTTHEME-0001 – Theme System
- NPSPEC-BOOTCONTROL-0003 – Control Styling
- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTTEXT-0011 – Typography Tokens

---

# 27. Zusammenfassung

Das **Boot Design Token System** definiert die semantische Grundlage aller visuellen Eigenschaften der NovaOS-Bootoberfläche. Es abstrahiert Farben, Typografie, Abstände, Animationen, Transparenzen und weitere Designparameter in wiederverwendbare Tokens, die zentral durch Themes verwaltet werden. Durch konsequente Trennung von Design und Implementierung, deterministische Tokenauflösung und statische Speicherverwaltung ermöglicht das System eine konsistente, erweiterbare und langfristig wartbare Gestaltung der gesamten Bootoberfläche.