# NPSPEC-BOOTCONTROL-ICONBUTTON-0001
# Icon Button

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-ICONBUTTON-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-BUTTON-0001, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTCONTROL-0004, NPSPEC-BOOTINPUT-0001 |

---

# 1. Ziel

Diese Spezifikation definiert den **Icon Button** der NovaOS Boot UI.

Ein Icon Button ist eine spezialisierte Form des Standard-Buttons, deren primäre Darstellung aus einem Symbol besteht. Er wird für kompakte, häufig genutzte oder universell erkennbare Aktionen eingesetzt.

Typische Einsatzbereiche:

- Zurück
- Weiter
- Schließen
- Einstellungen
- Aktualisieren
- Hilfe
- Suche
- Navigation

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Icon Buttons.

Sie umfasst:

- Navigation
- Dialoge
- Symbolleisten
- Listen
- Recovery
- Boot Manager
- Wartungsoberflächen

Nicht Bestandteil sind:

- Text Buttons
- Menüeinträge
- Listenicons

---

# 3. Architektur

```text
Boot Control Framework

↓

Button

↓

Icon Button

↓

Template

↓

Renderer
```

---

# 4. Grundprinzip

Ein Icon Button stellt genau eine Aktion über ein Symbol dar.

Text ist optional und dient ausschließlich der Accessibility oder Tooltip-Anzeige.

Das Symbol besitzt immer höhere Priorität als Text.

---

# 5. Aufgaben

Der Icon Button übernimmt:

- Aktionsauslösung
- Symboldarstellung
- Fokusdarstellung
- Zustandswechsel
- Themeintegration
- Accessibility
- Animationen
- Tooltip-Unterstützung

---

# 6. Aufbau

```text
Icon Button

├── Background

├── Border

├── Icon

├── Focus Indicator

├── Hover Layer

└── Ripple Layer (optional)
```

---

# 7. Eigenschaften

Jeder Icon Button besitzt:

- ID
- Icon
- Aktion
- Tooltip
- Style
- Template
- Zustand
- Größe
- Sichtbarkeit

Optional:

- unterstützenden Text
- Toggle-Zustand

---

# 8. Iconquellen

Unterstützt werden:

- SVG
- Vektorpfade
- Symbolschrift
- eingebettete Systemicons

Rastergrafiken sind nur als Fallback zulässig.

---

# 9. Größen

Empfohlene Standardgrößen:

| Typ | Größe |
|------|-------|
| Small | 32 × 32 DLU |
| Normal | 40 × 40 DLU |
| Large | 48 × 48 DLU |
| Touch | 56 × 56 DLU |

Das Icon wird automatisch proportional skaliert.

---

# 10. Layout

Das Symbol wird:

- horizontal zentriert
- vertikal zentriert

Innerhalb der verfügbaren Fläche dargestellt.

Optional kann zusätzlicher Text unter oder neben dem Symbol angezeigt werden.

---

# 11. Zustände

```text
Normal

↓

Hovered

↓

Focused

↓

Pressed

↓

Released

↓

Disabled
```

Optional:

```text
Selected

Busy

Checked
```

---

# 12. Eingaben

Unterstützt:

- Maus
- Tastatur
- Touch
- Controller

Aktivierung:

- Linksklick
- Enter
- Space
- Controller A
- Touch

---

# 13. Tooltip

Jeder Icon Button sollte einen Tooltip besitzen.

Der Tooltip beschreibt:

- Aktion
- Tastenkürzel
- optionale Zusatzinformationen

Screenreader verwenden diese Beschreibung ebenfalls.

---

# 14. Darstellung

Standarddarstellung:

- Glass Material
- runde oder leicht abgerundete Form
- zentriertes Symbol
- transparenter Hintergrund
- Focus Glow

Die Darstellung erfolgt vollständig über Theme und Template.

---

# 15. Animationen

Unterstützt werden:

- Hover Fade
- Focus Glow
- Press Scale
- Icon Fade
- Shadow Transition
- Material Transition

Animationen verändern niemals das Symbol selbst.

---

# 16. Accessibility

Jeder Icon Button besitzt:

- Name
- Rolle
- Beschreibung
- Shortcut
- Status

Besitzt der Button keinen sichtbaren Text, ist ein Accessibility-Name verpflichtend.

---

# 17. Themeintegration

Das Theme definiert:

- Symbolfarbe
- Hintergrund
- Rahmen
- Fokusfarbe
- Schatten
- Transparenz
- Animationen

Icons übernehmen automatisch die Themefarben.

---

# 18. Speicherverwaltung

Icon Buttons verwenden:

- gemeinsame Icon-Ressourcen
- statische Templates
- zentrale Styleobjekte

SVG-Daten werden nur einmal geladen.

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Klick
- O(1)-Iconwechsel
- O(1)-Zustandswechsel
- deterministische Laufzeit

Icons werden gecacht und wiederverwendet.

---

# 20. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- fehlendes Icon
- ungültige Icon-ID
- beschädigte SVG-Daten
- fehlendes Template
- Nullpointer

Ist kein Icon verfügbar, wird ein Standard-Platzhaltersymbol dargestellt.

---

# 21. Sicherheit

Das System verhindert:

- ungültige Iconreferenzen
- rekursive Iconladevorgänge
- doppelte Klickereignisse
- fehlende Accessibility-Namen
- inkonsistente Zustände

Destruktive Icon Buttons müssen zusätzlich farblich und semantisch gekennzeichnet werden.

---

# 22. API

```c
typedef struct nova_icon_button nova_icon_button_t;

nova_result_t
nova_icon_button_create(
    nova_icon_t icon,
    nova_icon_button_t** button);

nova_result_t
nova_icon_button_set_icon(
    nova_icon_button_t* button,
    nova_icon_t icon);

nova_result_t
nova_icon_button_set_tooltip(
    nova_icon_button_t* button,
    const char* tooltip);

nova_result_t
nova_icon_button_set_action(
    nova_icon_button_t* button,
    nova_action_t action);

bool
nova_icon_button_pressed(
    const nova_icon_button_t* button);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Button-ID
- Icon-ID
- Klickanzahl
- Fokuswechsel
- Zustandswechsel
- Renderzeit
- Speicherverbrauch
- Fehleranzahl

---

# 24. Testfälle

Zu testen sind:

- SVG-Icons
- Themewechsel
- Hover
- Pressed
- Disabled
- Fokus
- Touch
- Controller
- Accessibility
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Icon Buttons auf dem Standard-Button basieren.
- Symbole korrekt skaliert und zentriert dargestellt werden.
- Tooltip und Accessibility-Name vorhanden sind.
- Themefarben automatisch übernommen werden.
- SVG-Icons bevorzugt verwendet werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTCONTROL-ICONBUTTON-0001 – Einheitlicher Icon Button
- ADR-BOOTTHEME-0001 – Theme-System
- ADR-BOOTRENDER-0017 – Image Rendering
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0002 – Deterministische UI-Verarbeitung

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCONTROL-BUTTON-0001 – Button
- NPSPEC-BOOTCONTROL-0003 – Control Styling
- NPSPEC-BOOTCONTROL-0004 – Control Templates
- NPSPEC-BOOTRENDER-0017 – Image Rendering
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 27. Zusammenfassung

Der **Icon Button** ist eine spezialisierte Variante des Standard-Buttons und dient der platzsparenden Darstellung häufig verwendeter Aktionen. Er verwendet skalierbare Vektorsymbole, unterstützt vollständige Theme-Integration, konsistente Zustandsübergänge, Animationen sowie Accessibility. Durch die Wiederverwendung des allgemeinen Button-Frameworks und zentral verwalteter Icon-Ressourcen entsteht ein performantes, konsistentes und erweiterbares Steuerelement für die NovaOS-Bootoberfläche auf BIOS- und UEFI-Systemen.