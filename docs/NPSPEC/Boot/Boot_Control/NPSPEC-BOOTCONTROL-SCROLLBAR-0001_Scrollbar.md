# NPSPEC-BOOTCONTROL-SCROLLBAR-0001
# Scrollbar

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-SCROLLBAR-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-SCROLLVIEW-0001, NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-0002, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTCONTROL-0004, NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Scrollbar-Control** der NovaOS Boot UI.

Eine Scrollbar visualisiert die aktuelle Position innerhalb eines scrollbaren Inhalts und ermöglicht dessen Navigation. Sie dient als optionales Eingabeelement einer Scroll View und kann vertikal oder horizontal ausgerichtet sein.

Typische Einsatzbereiche:

- Listen
- Textansichten
- Einstellungsseiten
- Dateiansichten
- Protokolle
- Diagnosedaten
- Scroll Views

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Scrollbars.

Sie umfasst:

- vertikale Scrollbars
- horizontale Scrollbars
- automatisch einblendbare Scrollbars
- permanente Scrollbars

Nicht Bestandteil sind:

- Slider
- Progress Bars
- Scroll Views

---

# 3. Architektur

```text
Boot Control Framework

↓

Scrollbar

├── Track

├── Thumb

├── Buttons (optional)

└── Scroll View
```

---

# 4. Grundprinzip

Eine Scrollbar besitzt keine eigenen Daten.

Sie repräsentiert ausschließlich den sichtbaren Bereich eines zugeordneten scrollbaren Inhalts.

Alle Positionsänderungen werden unmittelbar an die zugehörige Scroll View weitergegeben.

---

# 5. Aufgaben

Die Scrollbar übernimmt:

- Positionsdarstellung
- Navigation
- Thumb-Steuerung
- Themeintegration
- Accessibility
- Ereignisverarbeitung

---

# 6. Aufbau

```text
Scrollbar

├── Track

├── Thumb

├── Decrease Button (optional)

├── Increase Button (optional)

└── Focus Indicator
```

---

# 7. Eigenschaften

Jede Scrollbar besitzt:

- ID
- Orientierung
- Position
- Sichtbarkeit
- Style
- Template

Optional:

- Pfeiltasten
- Auto-Hide
- Mindestgröße des Thumb

---

# 8. Orientierung

Unterstützt werden:

```text
Horizontal
```

und

```text
Vertikal
```

Die Orientierung beeinflusst ausschließlich die Darstellung.

---

# 9. Zustände

```text
Normal

Hovered

Focused

Pressed

Disabled

Hidden
```

Zusätzlich:

```text
Dragging
```

---

# 10. Thumb

Der Thumb repräsentiert:

- aktuelle Position
- Größe des sichtbaren Bereichs

Die Thumb-Größe ist proportional zur Größe des sichtbaren Inhalts.

Eine definierte Mindestgröße muss jederzeit eingehalten werden.

---

# 11. Eingaben

Unterstützt:

- Maus
- Touch
- Tastatur
- Controller

Aktionen:

- Drag
- Klick auf Track
- Klick auf Pfeiltasten
- Mausrad (über Scroll View)

---

# 12. Navigation

Unterstützt werden:

- Zeilenweise
- Seitenweise
- Direktes Ziehen
- Sprung zum Klickpunkt

Das Verhalten wird zentral konfiguriert.

---

# 13. Darstellung

Die Scrollbar verwendet:

- Glass Material
- Track
- Thumb
- Focus Glow
- Themefarben

Alle visuellen Eigenschaften werden ausschließlich über Theme und Template definiert.

---

# 14. Animationen

Optional unterstützt:

- Fade In
- Fade Out
- Thumb Transition
- Hover Animation

Animationen werden über das Motion-System gesteuert.

---

# 15. Auto-Hide

Optional kann die Scrollbar automatisch ausgeblendet werden.

Eigenschaften:

- Einblendung bei Interaktion
- Ausblendung nach Timeout
- keine Beeinträchtigung der Bedienbarkeit

---

# 16. Accessibility

Jede Scrollbar besitzt:

- Name
- Rolle
- Beschreibung
- Position
- Bereich

Screenreader geben die aktuelle Scrollposition aus.

---

# 17. Themeintegration

Das Theme definiert:

- Trackfarbe
- Thumbfarbe
- Transparenz
- Schatten
- Hoverfarbe
- Fokusfarbe

Es existieren keine fest codierten Farben.

---

# 18. Speicherverwaltung

Scrollbars verwenden:

- gemeinsame Templates
- gemeinsame Styles
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Positionsänderung
- O(1)-Thumbberechnung
- O(1)-Rendering
- deterministische Laufzeit

Nur geänderte Bereiche werden neu gerendert.

---

# 20. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Position
- ungültige Thumbgröße
- beschädigtes Template
- fehlender Style
- Nullpointer

Ungültige Positionen werden automatisch begrenzt.

---

# 21. Sicherheit

Das System verhindert:

- Scrollpositionen außerhalb des gültigen Bereichs
- ungültige Thumbgrößen
- rekursive Scrollereignisse
- beschädigte Verknüpfungen zur Scroll View
- inkonsistente Zustände

Eine Scrollbar darf niemals unabhängig von ihrer Scroll View arbeiten.

---

# 22. API

```c
typedef struct nova_scrollbar nova_scrollbar_t;

typedef enum
{
    NOVA_SCROLLBAR_HORIZONTAL,
    NOVA_SCROLLBAR_VERTICAL
}
nova_scrollbar_orientation_t;

nova_result_t
nova_scrollbar_create(
    nova_scrollbar_orientation_t orientation,
    nova_scrollbar_t** scrollbar);

nova_result_t
nova_scrollbar_set_position(
    nova_scrollbar_t* scrollbar,
    double position);

nova_result_t
nova_scrollbar_attach(
    nova_scrollbar_t* scrollbar,
    nova_scroll_view_t* view);

double
nova_scrollbar_get_position(
    const nova_scrollbar_t* scrollbar);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Scrollbar-ID
- Orientierung
- Position
- Thumbgröße
- Drag-Ereignisse
- Renderzeit
- Speicherverbrauch
- Fehleranzahl

---

# 24. Testfälle

Zu testen sind:

- Horizontal
- Vertikal
- Drag
- Track-Klick
- Auto-Hide
- Themewechsel
- Accessibility
- Animationen
- Mindestgröße des Thumb
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Scrollpositionen korrekt dargestellt werden.
- der Thumb proportional zur sichtbaren Inhaltsgröße skaliert.
- Auto-Hide optional unterstützt wird.
- Theme und Templates vollständig verwendet werden.
- Accessibility vollständig integriert ist.
- ausschließlich die zugehörige Scroll View gesteuert wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTCONTROL-SCROLLBAR-0001 – Scrollbar
- ADR-BOOTCONTROL-SCROLLVIEW-0001 – Scroll View
- ADR-BOOTTHEME-0001 – Theme-System
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0002 – Deterministische UI-Verarbeitung

## NPSPECs

- NPSPEC-BOOTCONTROL-SCROLLVIEW-0001 – Scroll View
- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCONTROL-0002 – Control State Model
- NPSPEC-BOOTCONTROL-0003 – Control Styling
- NPSPEC-BOOTCONTROL-0004 – Control Templates
- NPSPEC-BOOTINPUT-0001 – Input System
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 27. Zusammenfassung

Die **Scrollbar** definiert das standardisierte Steuerelement zur Navigation innerhalb scrollbarer Inhalte der NovaOS-Bootoberfläche. Sie visualisiert die aktuelle Position, unterstützt horizontale und vertikale Ausrichtungen, optionale Auto-Hide-Funktionen sowie vollständige Theme- und Accessibility-Integration. Durch deterministische Positionsberechnung, enge Kopplung an die Scroll View und statische Speicherverwaltung entsteht ein performantes und konsistentes UI-Control für BIOS- und UEFI-basierte Bootumgebungen.