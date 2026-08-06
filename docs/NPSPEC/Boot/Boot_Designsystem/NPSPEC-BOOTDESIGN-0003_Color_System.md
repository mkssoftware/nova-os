# NPSPEC-BOOTDESIGN-0003
# Color System

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTDESIGN-0003 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Design System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTDESIGN-0002, NPSPEC-BOOTTHEME-0001, NPSPEC-BOOTACCESS-0001, NPSPEC-BOOTCONTROL-0003 |

---

# 1. Ziel

Diese Spezifikation definiert das **Color System** der NovaOS-Bootoberfläche.

Das Color System beschreibt sämtliche Farbdefinitionen der Bootumgebung auf semantischer Ebene. Farben werden ausschließlich über Design Tokens und Themes bereitgestellt. UI-Komponenten dürfen niemals feste Farbwerte verwenden.

Das System gewährleistet eine konsistente Darstellung, unterstützt unterschiedliche Themes und erfüllt die Anforderungen an Barrierefreiheit und langfristige Erweiterbarkeit.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Hintergrundfarben
- Oberflächenfarben
- Textfarben
- Statusfarben
- Akzentfarben
- Rahmenfarben
- Fokusfarben
- Transparenzen
- sämtliche UI-Komponenten

---

# 3. Ziele

Das Color System muss:

- semantische Farben verwenden
- Themewechsel unterstützen
- hohen Kontrast ermöglichen
- deterministisch arbeiten
- Accessibility berücksichtigen
- leicht erweiterbar sein

---

# 4. Architektur

```text
Theme

↓

Color Tokens

↓

Control Styles

↓

UI-Komponenten

↓

Renderer
```

---

# 5. Komponenten

Das Color System besteht aus:

- Color Registry
- Token Resolver
- Theme Integration
- Contrast Validator
- Accessibility Manager
- Color Cache

---

# 6. Grundprinzip

Farben werden ausschließlich über semantische Rollen verwendet.

Beispiel:

```text
Button

↓

Color.Surface.Primary

↓

Theme

↓

RGBA-Wert
```

Direkte RGB- oder Hex-Werte in Komponenten sind unzulässig.

---

# 7. Farbrollen

Pflichtrollen:

```text
Color.Background

Color.Surface

Color.Surface.Secondary

Color.Primary

Color.Secondary

Color.Accent

Color.Text.Primary

Color.Text.Secondary

Color.Border

Color.Focus

Color.Disabled
```

---

# 8. Statusfarben

Unterstützt werden:

```text
Color.Success

Color.Warning

Color.Error

Color.Information

Color.Progress
```

Statusfarben dürfen ausschließlich zur Darstellung semantischer Zustände verwendet werden.

---

# 9. Interaktionsfarben

Das System definiert Farben für:

- Hover
- Focus
- Pressed
- Selected
- Disabled
- Active

Alle Zustände besitzen eigene semantische Tokens.

---

# 10. Transparenzen

Farben können Transparenzwerte enthalten.

Unterstützte Ebenen:

- vollständig deckend
- halbtransparent
- Glasoberflächen
- Overlays
- Dialoge

Die konkrete Deckkraft wird durch das Theme bestimmt.

---

# 11. Farbhierarchie

Das System unterscheidet:

```text
Background

↓

Surface

↓

Primary Elements

↓

Accent Elements

↓

Text

↓

Overlay
```

Diese Reihenfolge definiert die visuelle Priorität.

---

# 12. Farbvererbung

Komponenten können Farben übernehmen.

Beispiel:

```text
Dialog

↓

Surface

↓

Button

↓

Surface.Primary
```

Explizite Überschreibungen erfolgen ausschließlich über Themes.

---

# 13. Accessibility

Das Color System muss:

- ausreichende Kontrastverhältnisse gewährleisten
- Fokuszustände deutlich darstellen
- Farbenblindheit berücksichtigen
- Informationen niemals ausschließlich über Farben vermitteln

Empfohlene Mindestkontraste orientieren sich an WCAG-Richtlinien.

---

# 14. Themeintegration

Themes definieren ausschließlich die konkreten Farbwerte.

Die semantischen Farbnamen bleiben unverändert.

Dadurch können unterschiedliche Themes dieselben UI-Komponenten verwenden.

---

# 15. Validierung

Vor der Aktivierung eines Themes werden geprüft:

- vollständige Farbdefinitionen
- fehlende Tokens
- Kontrastwerte
- ungültige Farbformate
- doppelte Token

Ungültige Themes werden nicht aktiviert.

---

# 16. Sicherheit

Das System verhindert:

- fehlende Farbdefinitionen
- ungültige Farbwerte
- rekursive Tokenreferenzen
- doppelte Farbtoken
- inkonsistente Farbtabellen

Alle Farbdefinitionen werden beim Laden validiert.

---

# 17. Speicherverwaltung

Das Color System verwendet:

- statische Farbtabellen
- Theme-Puffer
- Token-Pools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Color Lookup
- O(1)-Token Lookup
- O(1)-Theme Lookup
- deterministische Laufzeit

Alle Farbwerte werden beim Laden des Themes vorbereitet.

---

# 19. API

```c
typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
}
nova_color_t;

const nova_color_t*
nova_color_get(
    const char* token_name);

bool
nova_color_exists(
    const char* token_name);

nova_result_t
nova_color_reload_theme(void);
```

---

# 20. Diagnosedaten

Zu erfassen sind:

- Theme-ID
- Anzahl Farbtoken
- Kontrastfehler
- fehlende Farben
- Ladezeit
- Validierungsstatus

---

# 21. Testfälle

Zu testen sind:

- Themewechsel
- Farbvererbung
- Statusfarben
- Transparenzen
- Fokusfarben
- Kontrastprüfung
- Accessibility
- fehlende Farbtoken
- Fehlerfälle

---

# 22. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Farben ausschließlich über semantische Tokens bereitgestellt werden.
- Themewechsel ohne Änderungen an UI-Komponenten möglich sind.
- alle Interaktions- und Statusfarben definiert sind.
- Accessibility-Anforderungen erfüllt werden.
- ungültige Farbdefinitionen erkannt werden.
- keine festen Farbwerte in UI-Komponenten gespeichert werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Erscheinungsbild erzeugen.

---

# 23. Referenzen

## ADRs

- ADR-BOOTDESIGN-0003 – Color System
- ADR-BOOTDESIGN-0001 – Boot Design Language
- ADR-BOOTDESIGN-0002 – Design Tokens
- ADR-BOOTTHEME-0001 – Theme System

## NPSPECs

- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTDESIGN-0002 – Boot Design Tokens
- NPSPEC-BOOTTHEME-0001 – Theme System
- NPSPEC-BOOTACCESS-0001 – Accessibility Framework
- NPSPEC-BOOTCONTROL-0003 – Control Styling

---

# 24. Zusammenfassung

Das **Color System** definiert die semantische Farbarchitektur der NovaOS-Bootoberfläche. Es abstrahiert sämtliche Farbdefinitionen über Design Tokens und Themes und trennt dadurch Gestaltung konsequent von der Implementierung. Durch zentrale Farbverwaltung, Themeintegration, Kontrastprüfung, Unterstützung für Accessibility und statische Speicherverwaltung stellt das System eine konsistente, flexible und langfristig wartbare Farbgestaltung für BIOS- und UEFI-basierte Bootumgebungen sicher.