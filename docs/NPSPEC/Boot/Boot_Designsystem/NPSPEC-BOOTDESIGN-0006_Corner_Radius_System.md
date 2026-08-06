# NPSPEC-BOOTDESIGN-0006
# Corner Radius System

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTDESIGN-0006 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Design System |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTDESIGN-0002, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTRENDER-0013, NPSPEC-BOOTTHEME-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Corner Radius System** der NovaOS-Bootoberfläche.

Das Corner Radius System beschreibt sämtliche Abrundungen der Benutzeroberfläche. Alle Radien werden über semantische Design Tokens definiert und sorgen für ein konsistentes Erscheinungsbild sämtlicher Komponenten.

Komponenten dürfen niemals feste Radiuswerte verwenden.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Buttons
- Karten
- Dialoge
- Listen
- Textfelder
- Menüs
- Kontextmenüs
- Tooltips
- Bilder
- sämtliche UI-Komponenten

---

# 3. Ziele

Das Radius-System muss:

- konsistente Abrundungen gewährleisten
- semantische Tokens verwenden
- Themewechsel unterstützen
- deterministisch arbeiten
- Accessibility unterstützen
- leicht erweiterbar sein

---

# 4. Architektur

```text
Theme

↓

Radius Tokens

↓

Control Styles

↓

Renderer

↓

UI-Komponenten
```

---

# 5. Komponenten

Das System besteht aus:

- Radius Registry
- Token Resolver
- Theme Integration
- Validation Engine
- Radius Cache
- Style Resolver

---

# 6. Grundprinzip

Alle Komponenten verwenden ausschließlich Radius Tokens.

Beispiel:

```text
Dialog

↓

Radius.Large

↓

Theme

↓

Render Engine
```

Direkte Pixelwerte innerhalb von Komponenten sind unzulässig.

---

# 7. Radius Tokens

Pflichttokens:

```text
Radius.None

Radius.Tiny

Radius.Small

Radius.Medium

Radius.Large

Radius.XLarge

Radius.Pill

Radius.Circle
```

Die konkreten Werte werden ausschließlich durch das Theme definiert.

---

# 8. Radiusrollen

Empfohlene Verwendung:

| Token | Typische Verwendung |
|--------|---------------------|
| Radius.None | Linien, Separatoren |
| Radius.Tiny | kleine Badges |
| Radius.Small | Buttons |
| Radius.Medium | Textfelder |
| Radius.Large | Karten |
| Radius.XLarge | Dialoge |
| Radius.Pill | Schalter |
| Radius.Circle | Avatare, Statuspunkte |

Diese Zuordnung dient als Designrichtlinie.

---

# 9. Einheitlichkeit

Alle Komponenten derselben Kategorie verwenden identische Radius Tokens.

Beispiele:

- alle Standardbuttons → Radius.Small
- alle Dialoge → Radius.XLarge
- alle Karten → Radius.Large

Abweichungen müssen ausdrücklich dokumentiert werden.

---

# 10. Skalierung

Die semantischen Tokens bleiben unabhängig von der Bildschirmauflösung konstant.

Themes können abhängig von:

- DPI
- Bildschirmgröße
- Accessibility

abweichende numerische Radiuswerte definieren.

---

# 11. Themeintegration

Themes dürfen ausschließlich:

- Radiuswerte
- Skalierungsregeln
- Rundungsprofile

ändern.

Die Namen der Tokens bleiben unverändert.

---

# 12. Rendering

Die Rendering Engine unterstützt:

- Anti-Aliasing
- Clip-Radien
- Maskierung
- Schatten an Rundungen
- Transparenzen

Alle Radien werden pixelgenau gerendert.

---

# 13. Animation

Radiuswerte dürfen animiert werden.

Beispiele:

- Dialog öffnet
- Hover
- Fokus
- Morphing

Animationen erfolgen ausschließlich über die Motion Engine.

---

# 14. Accessibility

Accessibility kann:

- Radien vergrößern
- Radien vereinfachen
- Animationen reduzieren

Die semantische Tokenstruktur bleibt erhalten.

---

# 15. Validierung

Vor der Aktivierung eines Themes werden geprüft:

- vollständige Radius Tokens
- ungültige Werte
- negative Radien
- doppelte Tokendefinitionen

Ungültige Themes werden abgelehnt.

---

# 16. Sicherheit

Das System verhindert:

- negative Radiuswerte
- ungültige Tokenreferenzen
- beschädigte Themewerte
- rekursive Tokenzuordnungen

Alle Werte werden beim Theme-Laden validiert.

---

# 17. Speicherverwaltung

Das Radius-System verwendet:

- statische Tokentabellen
- Theme-Puffer
- Style-Pools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Radius Lookup
- O(1)-Theme Lookup
- O(1)-Token Lookup
- deterministische Laufzeit

Alle Radiuswerte werden beim Theme-Laden vorbereitet.

---

# 19. API

```c
typedef enum
{
    NOVA_RADIUS_NONE,
    NOVA_RADIUS_TINY,
    NOVA_RADIUS_SMALL,
    NOVA_RADIUS_MEDIUM,
    NOVA_RADIUS_LARGE,
    NOVA_RADIUS_XLARGE,
    NOVA_RADIUS_PILL,
    NOVA_RADIUS_CIRCLE
}
nova_radius_token_t;

float
nova_radius_get(
    nova_radius_token_t token);

bool
nova_radius_exists(
    nova_radius_token_t token);

nova_result_t
nova_radius_reload(void);
```

---

# 20. Diagnosedaten

Zu erfassen sind:

- Theme-ID
- Anzahl Radius Tokens
- ungültige Radien
- Themewechsel
- Validierungsfehler
- Ladezeit

---

# 21. Testfälle

Zu testen sind:

- Buttons
- Karten
- Dialoge
- Textfelder
- Menüs
- Animationen
- Themewechsel
- Accessibility
- ungültige Radiuswerte
- Fehlerfälle

---

# 22. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Abrundungen ausschließlich über Radius Tokens definiert werden.
- Komponenten derselben Kategorie identische Radius Tokens verwenden.
- Themewechsel ohne Änderungen an Komponenten möglich sind.
- Animationen von Radiuswerten unterstützt werden.
- ungültige Radiusdefinitionen erkannt werden.
- keine festen Radiuswerte in Komponenten gespeichert werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Erscheinungsbild erzeugen.

---

# 23. Referenzen

## ADRs

- ADR-BOOTDESIGN-0006 – Corner Radius System
- ADR-BOOTDESIGN-0002 – Design Tokens
- ADR-BOOTTHEME-0001 – Theme System
- ADR-BOOTRENDER-0013 – Rounded Geometry

## NPSPECs

- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTDESIGN-0002 – Boot Design Tokens
- NPSPEC-BOOTCONTROL-0003 – Control Styling
- NPSPEC-BOOTRENDER-0013 – Rounded Geometry
- NPSPEC-BOOTTHEME-0001 – Theme System

---

# 24. Zusammenfassung

Das **Corner Radius System** definiert die semantische Verwaltung sämtlicher Abrundungen innerhalb der NovaOS-Bootoberfläche. Es abstrahiert Radiuswerte über Design Tokens und ermöglicht dadurch konsistente Gestaltung, Themeintegration, Animationen und Accessibility-Anpassungen, ohne dass UI-Komponenten feste Werte enthalten. Durch zentrale Tokenverwaltung, deterministische Verarbeitung und statische Speicherverwaltung bildet das System die Grundlage für eine moderne, einheitliche und langfristig wartbare Formensprache der gesamten Bootoberfläche.