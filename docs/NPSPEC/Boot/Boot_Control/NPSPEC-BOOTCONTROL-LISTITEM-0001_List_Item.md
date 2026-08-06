# NPSPEC-BOOTCONTROL-LISTITEM-0001
# List Item

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-LISTITEM-0001 |
| Titel | List Item |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Hoch |
| Gültigkeitsbereich | Gesamte NovaOS Boot UI |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-LIST-0001, NPSPEC-BOOTCONTROL-LABEL-0001, NPSPEC-BOOTCONTROL-IMAGE-0001, NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTDESIGN-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **List Item Control** der NovaOS Bootoberfläche.

Ein List Item repräsentiert genau einen Eintrag innerhalb einer Liste und dient zur Darstellung auswählbarer Informationen, Boot-Einträge, Wiederherstellungspunkte, Snapshots, Laufwerke oder anderer strukturierter Datensätze.

Das Control besitzt ein einheitliches Erscheinungsbild und unterstützt Tastatur-, Maus- und Touchbedienung.

---

# 2. Geltungsbereich

Das Control wird verwendet in:

- Boot Manager
- Snapshot-Auswahl
- Recovery-Menüs
- Festplattenlisten
- Partitionslisten
- Firmwarelisten
- Netzwerkliste
- Diagnoselisten
- Einstellungen
- zukünftigen Listenansichten

---

# 3. Ziele

Das List Item muss:

- vollständig wiederverwendbar sein
- responsive sein
- barrierefrei sein
- animiert werden können
- Touch unterstützen
- virtuelle Listen unterstützen

---

# 4. Aufbau

Ein List Item besteht aus:

- Hintergrund
- optionalem Icon
- Titel
- Untertitel
- Statusbereich
- Zusatzinformationen
- Fokusrahmen
- Auswahlindikator

---

# 5. Grundstruktur

```text
+--------------------------------------------------------------+
| [Icon]  Titel                           Status               |
|         Untertitel                                           |
|         Zusatzinformationen                                  |
+--------------------------------------------------------------+
```

Alle Bereiche sind optional.

---

# 6. Layout

Das Layout besteht aus:

- Leading Area
- Content Area
- Trailing Area

Die Content Area wächst flexibel.

---

# 7. Größen

Standardhöhe:

```text
72 px
```

Kompakt:

```text
56 px
```

Groß:

```text
96 px
```

Die Höhe wird durch Design Tokens definiert.

---

# 8. Inhalte

Unterstützt werden:

- Text
- Icons
- SVG
- PNG
- Statussymbole
- Badges
- Fortschrittsanzeigen

---

# 9. Zustände

Unterstützt werden:

- Normal
- Hover
- Focus
- Selected
- Disabled
- Pressed
- Error

Jeder Zustand besitzt eigene Design Tokens.

---

# 10. Fokus

Der Fokus besitzt:

- sichtbaren Fokusrahmen
- Animation
- Tastaturnavigation
- Screenreader-Unterstützung

---

# 11. Auswahl

Ein Item kann:

- nicht ausgewählt
- ausgewählt
- mehrfach ausgewählt (optional)

sein.

Mehrfachauswahl wird vom Container gesteuert.

---

# 12. Navigation

Unterstützt werden:

- Pfeil hoch
- Pfeil runter
- Home
- End
- Page Up
- Page Down
- Enter
- Space

---

# 13. Maus

Unterstützt werden:

- Hover
- Klick
- Doppelklick
- Kontextmenü
- Scroll

---

# 14. Touch

Unterstützt werden:

- Tap
- Double Tap
- Long Press
- Swipe (optional)

---

# 15. Animationen

Animationen:

- Hover Fade
- Focus Fade
- Selection Animation
- Press Animation

Alle Animationen verwenden Motion Tokens.

---

# 16. Darstellung

Das List Item unterstützt:

- Glasmaterial
- Schatten
- Blur
- Farbverläufe
- Themefarben

---

# 17. Performance

Das Control unterstützt:

- Virtualisierung
- Lazy Rendering
- Dirty Regions
- Caching

Auch sehr große Listen müssen flüssig bleiben.

---

# 18. Accessibility

Unterstützt werden:

- Screenreader
- Tastaturbedienung
- High Contrast
- große Schrift
- reduzierte Animationen

---

# 19. Sicherheit

Das Control:

- besitzt keine eigene Logik
- führt keine privilegierten Operationen aus
- verarbeitet ausschließlich UI-Ereignisse

---

# 20. Speicherverwaltung

Jedes List Item besitzt:

- Layoutdaten
- Zustandsdaten
- Style Cache
- Renderdaten

Nicht sichtbare Elemente dürfen recycelt werden.

---

# 21. API

```c
typedef enum
{
    NOVA_LIST_ITEM_NORMAL,
    NOVA_LIST_ITEM_HOVER,
    NOVA_LIST_ITEM_FOCUS,
    NOVA_LIST_ITEM_SELECTED,
    NOVA_LIST_ITEM_DISABLED,
    NOVA_LIST_ITEM_ERROR
}
nova_list_item_state_t;

typedef struct
{
    nova_object_t base;

    char title[256];
    char subtitle[256];

    nova_image_t* icon;

    bool selected;
    bool enabled;

    nova_list_item_state_t state;

}
nova_list_item_t;

nova_result_t
nova_list_item_initialize(
    nova_list_item_t* item);

nova_result_t
nova_list_item_set_title(
    nova_list_item_t* item,
    const char* title);

nova_result_t
nova_list_item_set_subtitle(
    nova_list_item_t* item,
    const char* subtitle);

nova_result_t
nova_list_item_set_icon(
    nova_list_item_t* item,
    nova_image_t* image);

nova_result_t
nova_list_item_set_selected(
    nova_list_item_t* item,
    bool selected);
```

---

# 22. Diagnosedaten

Zu protokollieren sind:

- Item-ID
- Status
- Fokus
- Auswahlstatus
- Renderzeit
- Layoutzeit
- Animationen

---

# 23. Testfälle

Zu testen sind:

- Fokuswechsel
- Auswahl
- Mehrfachauswahl
- Tastatursteuerung
- Maussteuerung
- Touchsteuerung
- Themewechsel
- große Listen
- Virtualisierung
- Accessibility

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Zustände korrekt dargestellt werden.
- Tastatur-, Maus- und Touchbedienung vollständig unterstützt werden.
- Fokus und Auswahl eindeutig sichtbar sind.
- Virtualisierung für große Listen verfügbar ist.
- das Control vollständig über Design Tokens gestaltet werden kann.
- Accessibility-Anforderungen erfüllt werden.
- sämtliche Animationen Motion Tokens verwenden.
- alle Funktionen über die definierte API verfügbar sind.

---

# 25. Referenzen

## ADRs

- ADR-BOOTCONTROL-LISTITEM-0001 – List Item Control

## NPSPECs

- NPSPEC-BOOTCONTROL-LIST-0001 – List
- NPSPEC-BOOTCONTROL-LABEL-0001 – Label
- NPSPEC-BOOTCONTROL-IMAGE-0001 – Image
- NPSPEC-BOOTINPUT-0001 – Input Architecture
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language

---

# 26. Zusammenfassung

Das **List Item Control** stellt den standardisierten Eintrag einer Liste innerhalb der NovaOS-Bootoberfläche dar. Es unterstützt Icons, Titel, Untertitel, Statusinformationen und Auswahlzustände sowie vollständige Tastatur-, Maus- und Touchbedienung. Durch Virtualisierung, Lazy Rendering und Design-Token-basierte Gestaltung ermöglicht es performante, konsistente und barrierefreie Listen für Bootmanager, Recovery, Diagnosesysteme und zukünftige Komponenten der Bootplattform.