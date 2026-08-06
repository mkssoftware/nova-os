# NPSPEC-BOOTCONTROL-CARD-0001
# Glass Card

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-CARD-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCOMPOSITOR-0006, NPSPEC-BOOTCOMPOSITOR-0007, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTCONTROL-0004, NPSPEC-BOOTLAYOUT-0001, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Glass Card Control** der NovaOS Boot UI.

Eine Glass Card ist ein containerbasiertes UI-Control zur Gruppierung zusammengehöriger Inhalte. Sie basiert auf dem NovaOS-Glass-Design und dient als visuelle Einheit für Informationen, Einstellungen oder interaktive Elemente.

Typische Einsatzbereiche:

- Bootoptionen
- Informationsbereiche
- Recovery-Ansichten
- Diagnosekarten
- Systemeinstellungen
- Geräteinformationen
- Statusübersichten
- Dashboards

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Glass Cards.

Sie umfasst:

- Standard Cards
- Informationskarten
- Interaktive Karten
- Statuskarten
- Gruppierungscontainer

Nicht Bestandteil sind:

- Dialoge
- Fenster
- Popups

---

# 3. Architektur

```text
Boot Control Framework

↓

Glass Card

├── Background

├── Header

├── Content

├── Footer (optional)

└── Renderer
```

---

# 4. Grundprinzip

Eine Glass Card stellt einen logischen Inhaltscontainer dar.

Sie gruppiert beliebige Controls, besitzt jedoch selbst keine fachliche Logik.

Die Card verwaltet ausschließlich Darstellung, Layout und optionale Interaktion.

---

# 5. Aufgaben

Die Glass Card übernimmt:

- Gruppierung von Controls
- Hintergrunddarstellung
- Themeintegration
- Materialdarstellung
- Layoutverwaltung
- Accessibility

---

# 6. Aufbau

```text
Glass Card

├── Glass Background

├── Border

├── Shadow

├── Header

├── Content Area

├── Footer

└── Overlay (optional)
```

---

# 7. Eigenschaften

Jede Card besitzt:

- ID
- Titel
- Sichtbarkeit
- Style
- Template
- Inhalt

Optional:

- Symbol
- Footer
- Aktionen
- Status Badge

---

# 8. Card-Typen

Unterstützt werden:

- Standard
- Information
- Warning
- Error
- Success
- Interactive
- Custom

Der Typ beeinflusst ausschließlich Darstellung und Standardstil.

---

# 9. Zustände

```text
Normal

Focused

Hovered

Pressed

Disabled

Hidden
```

Interaktive Karten unterstützen zusätzlich:

```text
Selected
```

---

# 10. Layout

Die Card besitzt:

```text
Header

↓

Content

↓

Footer
```

Alle Bereiche sind optional.

Die Positionierung der enthaltenen Controls erfolgt durch das Layout-System.

---

# 11. Darstellung

Die Glass Card verwendet:

- Glass Material
- Acrylic Material (optional)
- Hintergrundunschärfe
- Transparenz
- abgerundete Ecken
- Schatten

Alle Materialparameter werden durch das Theme definiert.

---

# 12. Inhalte

Die Card kann beliebige Controls enthalten:

- Labels
- Buttons
- Listen
- Bilder
- Status Badges
- Progress Indicators
- Switches
- Textfelder

Die Card besitzt keine inhaltlichen Einschränkungen.

---

# 13. Animationen

Optional unterstützt:

- Fade
- Scale
- Hover Glow
- Shadow Transition
- Material Transition

Animationen werden über das Motion-System ausgeführt.

---

# 14. Accessibility

Jede Card besitzt:

- Name
- Rolle
- Beschreibung

Interaktive Cards sind fokussierbar.

Nicht interaktive Cards werden als Gruppierungscontainer behandelt.

---

# 15. Themeintegration

Das Theme definiert:

- Hintergrund
- Transparenz
- Blur
- Schatten
- Rahmen
- Farben
- Abstände

Keine Darstellungseigenschaft ist fest codiert.

---

# 16. Ressourcenverwaltung

Cards verwenden:

- gemeinsame Templates
- gemeinsame Styles
- Materialdefinitionen

Glass- und Acrylic-Materialien werden zentral verwaltet.

---

# 17. Speicherverwaltung

Cards verwenden:

- Objektpools
- statische Templates
- gemeinsame Materialobjekte

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Layoutaktualisierung
- O(1)-Stylewechsel
- O(1)-Darstellung
- deterministische Laufzeit

Die Materialdarstellung darf den Renderpfad nicht blockieren.

---

# 19. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- beschädigtes Template
- ungültiges Material
- fehlender Style
- Nullpointer
- ungültiger Child-Container

Kann Glass Rendering nicht verwendet werden, erfolgt automatisch ein Fallback auf ein Standardmaterial.

---

# 20. Sicherheit

Das System verhindert:

- rekursive Containerstrukturen
- ungültige Materialreferenzen
- fehlerhafte Child-Hierarchien
- beschädigte Layoutdaten

Eine Card darf niemals selbst Fokus oder Eingaben ihrer enthaltenen Controls blockieren.

---

# 21. API

```c
typedef struct nova_glass_card nova_glass_card_t;

typedef enum
{
    NOVA_CARD_STANDARD,
    NOVA_CARD_INFORMATION,
    NOVA_CARD_WARNING,
    NOVA_CARD_ERROR,
    NOVA_CARD_SUCCESS,
    NOVA_CARD_INTERACTIVE,
    NOVA_CARD_CUSTOM
}
nova_card_type_t;

nova_result_t
nova_glass_card_create(
    nova_card_type_t type,
    nova_glass_card_t** card);

nova_result_t
nova_glass_card_set_title(
    nova_glass_card_t* card,
    const char* title);

nova_result_t
nova_glass_card_add_child(
    nova_glass_card_t* card,
    nova_control_t* control);

nova_result_t
nova_glass_card_set_visible(
    nova_glass_card_t* card,
    bool visible);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- Card-ID
- Card-Typ
- Anzahl enthaltener Controls
- Renderzeit
- Materialtyp
- Speicherverbrauch
- Fehleranzahl

---

# 23. Testfälle

Zu testen sind:

- Standard Card
- Interactive Card
- Warning Card
- Themewechsel
- Glass Material
- Acrylic Material
- Accessibility
- Layout
- Animationen
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Card-Typen korrekt dargestellt werden.
- Glass- und Acrylic-Materialien vollständig unterstützt werden.
- Inhalte unabhängig vom Card-Typ korrekt dargestellt werden.
- Theme und Templates vollständig verwendet werden.
- Accessibility vollständig integriert ist.
- Material-Fallbacks zuverlässig funktionieren.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 25. Referenzen

## ADRs

- ADR-BOOTCONTROL-CARD-0001 – Glass Card
- ADR-BOOTCOMPOSITOR-0006 – Glass Material
- ADR-BOOTCOMPOSITOR-0007 – Acrylic Material
- ADR-BOOTTHEME-0001 – Theme-System
- ADR-BOOTACCESS-0001 – Accessibility

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCOMPOSITOR-0006 – Glass Material
- NPSPEC-BOOTCOMPOSITOR-0007 – Acrylic Material
- NPSPEC-BOOTCONTROL-0003 – Control Styling
- NPSPEC-BOOTCONTROL-0004 – Control Templates
- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 26. Zusammenfassung

Die **Glass Card** definiert den standardisierten Container zur Gruppierung zusammengehöriger Inhalte innerhalb der NovaOS-Bootoberfläche. Sie kombiniert das NovaOS-Glass- beziehungsweise Acrylic-Material mit flexiblen Inhaltsbereichen, vollständiger Theme-Integration, Animationen und Accessibility. Durch die klare Trennung zwischen Container, Layout und Darstellung sowie statische Speicherverwaltung entsteht ein modernes, konsistentes und performantes UI-Element für BIOS- und UEFI-basierte Bootumgebungen.