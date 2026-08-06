# NPSPEC-BOOTCONTROL-TOOLTIP-0001
# Tooltip

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-TOOLTIP-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Mittel |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTCONTROL-0004, NPSPEC-BOOTLAYOUT-0005, NPSPEC-BOOTMOTION-0007, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Tooltip-Control** der NovaOS Boot UI.

Ein Tooltip stellt kurze kontextbezogene Informationen zu einem Control oder einer Funktion bereit. Er erscheint automatisch oder auf Anforderung und unterstützt den Benutzer bei der Bedienung der Bootoberfläche.

Typische Einsatzbereiche:

- Erklärung von Buttons
- Beschreibung von Icons
- Zusatzinformationen
- Tastenkombinationen
- Statushinweise
- Warnhinweise
- Entwickleroptionen

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Tooltips.

Sie umfasst:

- Standard Tooltip
- Rich Tooltip
- Fehler-Tooltip
- Warn-Tooltip
- Informations-Tooltip

Nicht Bestandteil sind:

- Dialoge
- Benachrichtigungen
- Popups
- Kontextmenüs

---

# 3. Architektur

```text
Boot Control Framework

↓

Tooltip Manager

↓

Tooltip

↓

Overlay Layer

↓

Renderer
```

---

# 4. Grundprinzip

Ein Tooltip gehört immer genau zu einem übergeordneten Control.

Er erscheint nur temporär und besitzt keine eigenständige Benutzerinteraktion.

Ein Tooltip darf niemals den Arbeitsablauf unterbrechen.

---

# 5. Aufgaben

Der Tooltip übernimmt:

- Anzeige von Zusatzinformationen
- Positionierung
- automatische Ein- und Ausblendung
- Themeintegration
- Accessibility
- Animationen

---

# 6. Aufbau

```text
Tooltip

├── Background

├── Text

├── Icon (optional)

├── Arrow (optional)

└── Shadow
```

---

# 7. Eigenschaften

Jeder Tooltip besitzt:

- ID
- Text
- Sichtbarkeit
- Style
- Template
- Ziel-Control

Optional:

- Symbol
- Titel
- Shortcut
- Beschreibung

---

# 8. Anzeige

Ein Tooltip erscheint:

- nach Hover
- bei Tastaturfokus
- auf Controller-Fokus
- optional per Touch-Hold

Die Anzeige erfolgt zeitverzögert.

---

# 9. Ausblendung

Der Tooltip verschwindet automatisch:

- beim Verlassen des Controls
- nach einer Zeitüberschreitung
- bei Fokuswechsel
- bei Eingaben
- beim Öffnen eines Dialogs

---

# 10. Positionierung

Standard:

```text
Control

↓

Tooltip
```

Alternativ:

- oberhalb
- rechts
- links

Die Position wird automatisch angepasst, um die Safe Display Area einzuhalten.

---

# 11. Zustände

```text
Hidden

Appearing

Visible

Disappearing
```

Ein Tooltip besitzt keine Fokuszustände.

---

# 12. Darstellung

Der Tooltip verwendet:

- Glass Material
- weiche Schatten
- abgerundete Ecken
- Themefarben
- hohe Lesbarkeit

Alle Darstellungseigenschaften stammen aus Theme und Template.

---

# 13. Animationen

Unterstützt werden:

- Fade In
- Fade Out
- Scale
- Shadow Transition

Animationen erfolgen über das Motion-System.

---

# 14. Inhalte

Unterstützt werden:

- kurzer Text
- Symbol
- Tastenkürzel
- Statusinformation

Lange Erklärungen sind nicht zulässig.

---

# 15. Accessibility

Tooltips werden zusätzlich für Tastatur- und Controller-Nutzer bereitgestellt.

Screenreader können den Tooltip-Text auf Anforderung ausgeben.

Dekorative Tooltips werden ignoriert.

---

# 16. Themeintegration

Das Theme definiert:

- Hintergrund
- Schriftarten
- Transparenz
- Schatten
- Rand
- Animationen

Tooltips besitzen keine fest codierten Farben.

---

# 17. Speicherverwaltung

Tooltips verwenden:

- gemeinsame Templates
- gemeinsame Styles
- zentrale Tooltip-Instanzen

Mehrere gleichzeitig sichtbare Tooltips sollen vermieden werden.

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Einblenden
- O(1)-Ausblenden
- O(1)-Positionsberechnung
- deterministische Laufzeit

Es darf immer nur ein Tooltip gleichzeitig aktiv sein.

---

# 19. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- fehlender Tooltiptext
- ungültiges Ziel-Control
- beschädigtes Template
- ungültige Position
- Nullpointer

Kann der Tooltip nicht angezeigt werden, bleibt das zugehörige Control vollständig funktionsfähig.

---

# 20. Sicherheit

Das System verhindert:

- mehrere aktive Tooltips
- Tooltips außerhalb der Safe Display Area
- rekursive Tooltip-Anzeigen
- Fokusdiebstahl
- blockierende Tooltips

Tooltips dürfen niemals Eingaben abfangen.

---

# 21. API

```c
typedef struct nova_tooltip nova_tooltip_t;

nova_result_t
nova_tooltip_create(
    nova_tooltip_t** tooltip);

nova_result_t
nova_tooltip_set_text(
    nova_tooltip_t* tooltip,
    const char* text);

nova_result_t
nova_tooltip_attach(
    nova_tooltip_t* tooltip,
    nova_control_t* control);

nova_result_t
nova_tooltip_show(
    nova_tooltip_t* tooltip);

nova_result_t
nova_tooltip_hide(
    nova_tooltip_t* tooltip);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- Tooltip-ID
- Ziel-Control
- Einblendungen
- Ausblendungen
- Sichtbarkeitsdauer
- Renderzeit
- Fehleranzahl

---

# 23. Testfälle

Zu testen sind:

- Hover
- Tastaturfokus
- Controllerfokus
- Touch-Hold
- Positionierung
- Safe Display Area
- Themewechsel
- Accessibility
- Animationen
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Tooltips kontextbezogene Informationen korrekt darstellen.
- Positionen automatisch an den verfügbaren Platz angepasst werden.
- Ein- und Ausblendanimationen deterministisch ausgeführt werden.
- Theme und Templates vollständig verwendet werden.
- Accessibility vollständig integriert ist.
- maximal ein Tooltip gleichzeitig sichtbar ist.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 25. Referenzen

## ADRs

- ADR-BOOTCONTROL-TOOLTIP-0001 – Tooltip-Control
- ADR-BOOTLAYOUT-0005 – Overlay Layout
- ADR-BOOTMOTION-0007 – Transition System
- ADR-BOOTTHEME-0001 – Theme-System
- ADR-BOOTACCESS-0001 – Accessibility

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCONTROL-0003 – Control Styling
- NPSPEC-BOOTCONTROL-0004 – Control Templates
- NPSPEC-BOOTLAYOUT-0005 – Overlay Layout
- NPSPEC-BOOTMOTION-0007 – Transition System
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 26. Zusammenfassung

Das **Tooltip-Control** definiert die standardisierte Darstellung kontextbezogener Zusatzinformationen innerhalb der NovaOS-Bootoberfläche. Es unterstützt automatische Positionierung, zeitgesteuerte Ein- und Ausblendung, Theme-Integration, Animationen sowie vollständige Accessibility. Durch die Verwaltung über einen zentralen Tooltip-Manager, die Beschränkung auf eine aktive Instanz und statische Speicherverwaltung entsteht ein konsistentes, leichtgewichtiges und performantes Hilfesystem für BIOS- und UEFI-basierte Bootumgebungen.