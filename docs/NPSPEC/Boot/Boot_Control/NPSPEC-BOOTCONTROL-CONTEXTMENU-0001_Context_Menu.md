# NPSPEC-BOOTCONTROL-CONTEXTMENU-0001
# Context Menu

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-CONTEXTMENU-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Mittel |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-MENUBUTTON-0001, NPSPEC-BOOTCONTROL-LIST-0001, NPSPEC-BOOTLAYOUT-0005, NPSPEC-BOOTMOTION-0007, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Context Menu Control** der NovaOS Boot UI.

Ein Context Menu stellt kontextabhängige Aktionen für ein bestimmtes Objekt oder Control bereit. Es erscheint nur auf explizite Benutzeranforderung und enthält ausschließlich Aktionen, die für das aktuell ausgewählte Objekt relevant sind.

Typische Einsatzbereiche:

- Bootoptionen
- Laufwerke
- Partitionen
- Dateien
- Recovery-Einträge
- Diagnoseobjekte
- Listen
- Karten

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Context Menüs.

Sie umfasst:

- Standard Context Menu
- verschachtelte Menüs
- Icon-Menüs
- Menügruppen

Nicht Bestandteil sind:

- Hauptmenüs
- Dialoge
- Tooltips

---

# 3. Architektur

```text
Boot Control Framework

↓

Context Menu

├── Overlay Layer

├── Menu Items

├── Separator

└── Renderer
```

---

# 4. Grundprinzip

Ein Context Menu gehört immer genau zu einem Zielobjekt.

Es wird ausschließlich auf explizite Benutzeraktion geöffnet.

Ein Context Menu besitzt keinen dauerhaften Zustand.

---

# 5. Aufgaben

Das Context Menu übernimmt:

- Anzeige kontextbezogener Aktionen
- Navigation
- Auswahl
- Themeintegration
- Accessibility
- Animationen

---

# 6. Aufbau

```text
Context Menu

├── Background

├── Menu Items

├── Separator

├── Icons (optional)

├── Shortcut Labels (optional)

└── Shadow
```

---

# 7. Eigenschaften

Jedes Context Menu besitzt:

- ID
- Zielobjekt
- Sichtbarkeit
- Style
- Template

Optional:

- Symbole
- Tastenkürzel
- Untermenüs

---

# 8. Menüeinträge

Ein Menüeintrag kann besitzen:

- Titel
- Icon
- Shortcut
- Status
- Untermenü
- Aktion

Nicht verfügbare Aktionen werden deaktiviert dargestellt.

---

# 9. Zustände

```text
Hidden

Opening

Visible

Closing
```

Menüeinträge besitzen:

```text
Normal

Hovered

Focused

Pressed

Disabled
```

---

# 10. Positionierung

Das Context Menu erscheint:

- neben dem Zielobjekt
- innerhalb der Safe Display Area
- automatisch umpositioniert bei Platzmangel

Es darf niemals Bildschirmbereiche außerhalb der Anzeige verwenden.

---

# 11. Eingaben

Unterstützt:

- Maus
- Tastatur
- Controller
- Touch (Long Press)

Tastatur:

- ↑
- ↓
- →
- ←
- Enter
- Escape

---

# 12. Untermenüs

Untermenüs werden unterstützt.

Regeln:

- maximal eine geöffnete Untermenüebene pro Pfad
- automatische Positionierung
- automatische Schließung beim Verlassen

---

# 13. Darstellung

Das Context Menu verwendet:

- Glass Material
- Schatten
- Transparenz
- Themefarben
- Focus Glow

Die Darstellung erfolgt vollständig über Theme und Template.

---

# 14. Animationen

Unterstützt werden:

- Fade In
- Fade Out
- Scale
- Shadow Transition

Animationen werden durch das Motion-System ausgeführt.

---

# 15. Accessibility

Jedes Context Menu besitzt:

- Name
- Rolle
- Beschreibung

Jeder Menüeintrag besitzt:

- Titel
- Status
- Position

Screenreader geben den aktuellen Menüeintrag aus.

---

# 16. Themeintegration

Das Theme definiert:

- Hintergrund
- Transparenz
- Farben
- Schatten
- Schriftarten
- Icons
- Abstände

Keine Designparameter sind fest codiert.

---

# 17. Speicherverwaltung

Context Menüs verwenden:

- gemeinsame Templates
- gemeinsame Styles
- Objektpools

Menüeinträge werden dynamisch referenziert.

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Öffnen
- O(1)-Schließen
- O(1)-Navigation
- deterministische Laufzeit

Nur sichtbare Menüeinträge werden gerendert.

---

# 19. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültiges Zielobjekt
- beschädigtes Template
- fehlende Aktion
- fehlerhafte Untermenüs
- Nullpointer

Kann ein Menüeintrag nicht erstellt werden, bleibt das restliche Menü funktionsfähig.

---

# 20. Sicherheit

Das System verhindert:

- rekursive Untermenüs
- mehrere aktive Context Menüs
- ungültige Aktionen
- Menüdarstellung außerhalb der Safe Display Area
- blockierte Eingaben

Beim Öffnen eines neuen Context Menüs wird ein vorhandenes automatisch geschlossen.

---

# 21. API

```c
typedef struct nova_context_menu nova_context_menu_t;

nova_result_t
nova_context_menu_create(
    nova_context_menu_t** menu);

nova_result_t
nova_context_menu_add_item(
    nova_context_menu_t* menu,
    nova_menu_item_t* item);

nova_result_t
nova_context_menu_show(
    nova_context_menu_t* menu,
    nova_control_t* target);

nova_result_t
nova_context_menu_hide(
    nova_context_menu_t* menu);

bool
nova_context_menu_visible(
    const nova_context_menu_t* menu);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- Menü-ID
- Zielobjekt
- Anzahl der Einträge
- Öffnungen
- Schließungen
- Renderzeit
- Speicherverbrauch
- Fehleranzahl

---

# 23. Testfälle

Zu testen sind:

- Öffnen
- Schließen
- Untermenüs
- Tastatur
- Maus
- Controller
- Touch
- Themewechsel
- Accessibility
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Context Menüs ausschließlich kontextbezogene Aktionen anzeigen.
- Untermenüs korrekt unterstützt werden.
- die Safe Display Area jederzeit eingehalten wird.
- Theme und Templates vollständig verwendet werden.
- Accessibility vollständig integriert ist.
- maximal ein Context Menu gleichzeitig aktiv ist.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 25. Referenzen

## ADRs

- ADR-BOOTCONTROL-CONTEXTMENU-0001 – Context Menu
- ADR-BOOTLAYOUT-0005 – Overlay Layout
- ADR-BOOTMOTION-0007 – Transition System
- ADR-BOOTTHEME-0001 – Theme-System
- ADR-BOOTACCESS-0001 – Accessibility

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCONTROL-MENUBUTTON-0001 – Menu Button
- NPSPEC-BOOTCONTROL-LIST-0001 – List
- NPSPEC-BOOTLAYOUT-0005 – Overlay Layout
- NPSPEC-BOOTMOTION-0007 – Transition System
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 26. Zusammenfassung

Das **Context Menu** definiert das standardisierte Steuerelement zur Bereitstellung kontextabhängiger Aktionen innerhalb der NovaOS-Bootoberfläche. Es unterstützt verschachtelte Menüs, automatische Positionierung innerhalb der Safe Display Area, vollständige Theme-Integration, Animationen und Accessibility. Durch zentrale Verwaltung, deterministische Navigation und statische Speicherverwaltung entsteht ein konsistentes und performantes Kontextmenüsystem für BIOS- und UEFI-basierte Bootumgebungen.