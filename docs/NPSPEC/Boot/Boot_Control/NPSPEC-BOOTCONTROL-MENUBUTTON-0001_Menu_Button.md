# NPSPEC-BOOTCONTROL-MENUBUTTON-0001
# Menu Button

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-MENUBUTTON-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-BUTTON-0001, NPSPEC-BOOTNAV-0001, NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTLAYOUT-0005 |

---

# 1. Ziel

Diese Spezifikation definiert den **Menu Button** der NovaOS Boot UI.

Ein Menu Button öffnet ein zugeordnetes Menü oder eine Liste auswählbarer Aktionen. Er dient als Einstiegspunkt für gruppierte Funktionen, ohne zusätzlichen Platz auf der Oberfläche zu beanspruchen.

Typische Einsatzbereiche:

- Hauptmenü
- Kontextmenü
- Bootoptionen
- Erweiterte Einstellungen
- Recovery-Menüs
- Diagnosefunktionen
- Aktionsmenüs

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Menu Buttons.

Sie umfasst:

- Hauptmenüs
- Untermenüs
- Kontextmenüs
- Dropdown-Menüs
- Symbolleisten
- Dialoge

Nicht Bestandteil sind:

- Listen
- normale Buttons
- Navigationsseiten

---

# 3. Architektur

```text
Boot Control Framework

↓

Button

↓

Menu Button

↓

Menu Controller

↓

Menu Overlay

↓

Renderer
```

---

# 4. Grundprinzip

Ein Menu Button löst keine direkte Fachfunktion aus.

Seine Hauptaufgabe besteht darin, ein Menü zu öffnen oder zu schließen.

Die eigentliche Aktion wird erst nach Auswahl eines Menüeintrags ausgeführt.

---

# 5. Aufgaben

Der Menu Button übernimmt:

- Menü öffnen
- Menü schließen
- Fokusverwaltung
- Zustandsverwaltung
- Menüpositionierung
- Accessibility
- Animationen
- Ereignisauslösung

---

# 6. Aufbau

```text
Menu Button

├── Background

├── Border

├── Icon (optional)

├── Text

├── Arrow Indicator

└── Focus Indicator
```

Der Pfeil kennzeichnet eindeutig, dass ein Menü geöffnet wird.

---

# 7. Eigenschaften

Jeder Menu Button besitzt:

- ID
- Text
- Icon (optional)
- Menüreferenz
- Zustand
- Style
- Template
- Größe
- Sichtbarkeit

---

# 8. Menübindung

Ein Menu Button ist genau einem Menü zugeordnet.

```text
Button

↓

Menu

↓

Menu Items
```

Mehrere Buttons dürfen auf dasselbe Menü verweisen.

---

# 9. Zustände

```text
Normal

↓

Hovered

↓

Focused

↓

Pressed

↓

Expanded

↓

Collapsed

↓

Disabled
```

**Expanded** kennzeichnet ein geöffnetes Menü.

---

# 10. Menüposition

Das Menü wird standardmäßig:

- unterhalb des Buttons
- linksbündig
- innerhalb der Safe Display Area

angezeigt.

Kann dies nicht eingehalten werden, erfolgt eine automatische Neupositionierung.

---

# 11. Eingaben

Unterstützt:

- Maus
- Tastatur
- Touch
- Controller

Öffnen über:

- Linksklick
- Enter
- Space
- Pfeil nach unten
- Controller A

---

# 12. Tastatursteuerung

Unterstützt werden:

- ↑
- ↓
- ←
- →
- Enter
- Escape
- Home
- End

Escape schließt ausschließlich das geöffnete Menü.

---

# 13. Fokus

Beim Öffnen:

```text
Menu Button

↓

erstes aktives Menu Item
```

Beim Schließen wird der Fokus automatisch auf den Menu Button zurückgesetzt.

---

# 14. Darstellung

Ein Menu Button verwendet:

- Glass Material
- Text
- optionales Symbol
- Pfeilindikator
- Focus Glow

Die Darstellung stammt vollständig aus Theme und Template.

---

# 15. Animationen

Unterstützt werden:

- Fade
- Scale
- Arrow Rotation
- Focus Glow
- Shadow Transition

Die Menüanimation wird synchron mit dem Overlay ausgeführt.

---

# 16. Accessibility

Der Menu Button besitzt:

- Name
- Rolle
- Beschreibung
- Expanded-Status
- Shortcut

Screenreader geben zusätzlich den aktuellen Menüstatus aus.

---

# 17. Themeintegration

Das Theme definiert:

- Farben
- Symbolfarben
- Pfeildarstellung
- Transparenz
- Schatten
- Animationen

Alle visuellen Eigenschaften stammen aus dem Theme.

---

# 18. Speicherverwaltung

Menu Buttons verwenden:

- statische Templates
- gemeinsame Styles
- zentrale Menüreferenzen

Menüeinträge werden getrennt verwaltet.

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Menüöffnung
- O(1)-Menüschließung
- O(1)-Zustandswechsel
- deterministische Laufzeit

Menüs werden bei Bedarf gerendert.

---

# 20. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- fehlendes Menü
- ungültige Menüreferenz
- beschädigte Menüstruktur
- fehlendes Template
- Nullpointer

Kann das Menü nicht geöffnet werden, bleibt der Button im Zustand **Collapsed**.

---

# 21. Sicherheit

Das System verhindert:

- mehrfach geöffnete Menüs desselben Buttons
- rekursive Menüöffnungen
- Fokusverlust
- ungültige Menüreferenzen
- inkonsistente Expanded-Zustände

Pro Menu Button darf höchstens ein Menü gleichzeitig geöffnet sein.

---

# 22. API

```c
typedef struct nova_menu_button nova_menu_button_t;

nova_result_t
nova_menu_button_create(
    const char* text,
    nova_menu_t* menu,
    nova_menu_button_t** button);

nova_result_t
nova_menu_button_open(
    nova_menu_button_t* button);

nova_result_t
nova_menu_button_close(
    nova_menu_button_t* button);

bool
nova_menu_button_expanded(
    const nova_menu_button_t* button);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Button-ID
- Menü-ID
- Öffnungen
- Schließungen
- Fokuswechsel
- Expanded-Dauer
- Speicherverbrauch
- Fehleranzahl

---

# 24. Testfälle

Zu testen sind:

- Menü öffnen
- Menü schließen
- Tastatursteuerung
- Controller
- Touch
- Safe Display Area
- Themewechsel
- Accessibility
- Mehrfachöffnungen
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- jeder Menu Button genau ein Menü steuern kann.
- Menüs korrekt geöffnet und geschlossen werden.
- der Fokus nach dem Schließen zum Button zurückkehrt.
- der Expanded-Status korrekt verwaltet wird.
- Accessibility vollständig unterstützt wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTCONTROL-MENUBUTTON-0001 – Menu Button
- ADR-BOOTNAV-0001 – Navigationsmodell
- ADR-BOOTINPUT-0001 – Eingabearchitektur
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0002 – Deterministische UI-Verarbeitung

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCONTROL-BUTTON-0001 – Button
- NPSPEC-BOOTNAV-0001 – Boot Navigation Model
- NPSPEC-BOOTLAYOUT-0005 – Overlay Layout
- NPSPEC-BOOTINPUT-0001 – Input System
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 27. Zusammenfassung

Der **Menu Button** ist ein spezialisiertes Button-Control zum Öffnen und Schließen von Menüs innerhalb der NovaOS-Bootoberfläche. Er kombiniert eine standardisierte Button-Interaktion mit einer integrierten Menüsteuerung, automatischer Fokusverwaltung und vollständiger Theme- sowie Accessibility-Unterstützung. Durch deterministische Zustandsübergänge, sichere Menüverwaltung und statische Speicherstrukturen entsteht ein konsistentes und performantes Steuerelement für BIOS- und UEFI-basierte Bootumgebungen.