# NPSPEC-BOOTCONTROL-TILE-0001
# Boot Option Tile

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-TILE-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-0002, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTCONTROL-0004, NPSPEC-BOOTLAYOUT-0004, NPSPEC-BOOTNAV-0003 |

---

# 1. Ziel

Diese Spezifikation definiert das **Boot Option Tile** der NovaOS Boot UI.

Ein Boot Option Tile repräsentiert eine bootfähige Option oder Systemfunktion in Form einer großen interaktiven Kachel. Es kombiniert Symbol, Titel, Beschreibung und Statusinformationen zu einem leicht erfassbaren Bedienelement.

Typische Anwendungsbereiche:

- Betriebssystem starten
- Recovery
- Backup
- Wiederherstellung
- Speicherdiagnose
- Firmware-Einstellungen
- Wartungswerkzeuge
- Erweiterte Bootoptionen

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Boot Option Tiles.

Sie umfasst:

- Startoptionen
- Recovery-Kacheln
- Wartungsfunktionen
- Diagnosefunktionen
- Werkzeuge
- Benutzerdefinierte Bootmodule

Nicht Bestandteil sind:

- Listen
- Buttons
- Dialoge

---

# 3. Architektur

```text
Boot Control Framework

↓

Boot Option Tile

↓

Tile Template

↓

Renderer
```

---

# 4. Grundprinzip

Ein Boot Option Tile repräsentiert genau eine auswählbare Funktion.

Die Kachel dient sowohl der Navigation als auch der Ausführung einer definierten Aktion.

Alle Informationen müssen auf einen Blick verständlich sein.

---

# 5. Aufgaben

Das Tile übernimmt:

- Funktionsdarstellung
- Auswahl
- Fokusverwaltung
- Statusanzeige
- Navigation
- Animationen
- Accessibility
- Themeintegration

---

# 6. Aufbau

```text
Boot Option Tile

├── Background

├── Icon

├── Title

├── Subtitle

├── Status Area

├── Badge (optional)

└── Focus Indicator
```

---

# 7. Eigenschaften

Jede Kachel besitzt:

- ID
- Titel
- Beschreibung
- Symbol
- Aktion
- Status
- Sichtbarkeit
- Style
- Template

Optional:

- Badge
- Zusatzinformationen
- Fortschrittsstatus

---

# 8. Tile-Typen

Unterstützt werden:

- Standard
- Primary
- Recovery
- Maintenance
- Diagnostic
- Destructive
- Custom

Der Typ beeinflusst ausschließlich Darstellung und Standardverhalten.

---

# 9. Zustände

```text
Normal

Hovered

Focused

Pressed

Selected

Disabled

Busy
```

Optional:

```text
Warning

Error

Recommended
```

---

# 10. Größen

Empfohlene Standardgrößen:

| Typ | Größe |
|------|-------|
| Small | 180 × 120 DLU |
| Medium | 240 × 160 DLU |
| Large | 320 × 220 DLU |

Alle Größen sind DPI-unabhängig.

---

# 11. Layout

Standardaufbau:

```text
Icon

↓

Title

↓

Subtitle

↓

Status
```

Alle Inhalte werden automatisch ausgerichtet und bei Bedarf gekürzt.

---

# 12. Statusanzeige

Optional können dargestellt werden:

- verfügbar
- empfohlen
- aktiv
- gesperrt
- beschädigt
- Wartung erforderlich

Statusinformationen erscheinen in der Status Area.

---

# 13. Navigation

Unterstützt werden:

- Maus
- Tastatur
- Touch
- Controller

Tastatur:

- ↑
- ↓
- ←
- →
- Enter
- Space

Controller:

- Steuerkreuz
- Analogstick
- Taste A

---

# 14. Fokus

Beim Fokus erhält das Tile:

- Focus Glow
- hervorgehobenen Rahmen
- optionale Größenanpassung

Es darf immer nur ein Tile gleichzeitig fokussiert sein.

---

# 15. Darstellung

Das Tile verwendet:

- Glass Material
- große Symbolfläche
- weiche Schatten
- abgerundete Ecken
- transparente Ebenen
- Fokusbeleuchtung

Alle visuellen Eigenschaften stammen aus Theme und Template.

---

# 16. Animationen

Unterstützt werden:

- Hover Fade
- Focus Glow
- Press Scale
- Shadow Transition
- Status Transition
- Material Transition

Animationen erfolgen synchron mit dem Motion-System.

---

# 17. Accessibility

Jedes Tile besitzt:

- Name
- Rolle
- Beschreibung
- Status
- Shortcut (optional)

Screenreader geben zusätzlich vorhandene Statusinformationen aus.

---

# 18. Themeintegration

Das Theme definiert:

- Hintergrund
- Farben
- Symboldarstellung
- Typografie
- Schatten
- Transparenz
- Animationen

Keine Darstellungseigenschaft ist fest im Tile codiert.

---

# 19. Speicherverwaltung

Tiles verwenden:

- gemeinsame Templates
- gemeinsame Styles
- Objektpools
- zentrale Icon-Ressourcen

Heap-Allokationen während der Laufzeit sind verboten.

---

# 20. Performance

Anforderungen:

- O(1)-Fokuswechsel
- O(1)-Statuswechsel
- O(1)-Aktivierung
- deterministische Laufzeit

Nur geänderte Tiles werden neu gerendert.

---

# 21. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- fehlende Aktion
- fehlendes Symbol
- ungültiger Tile-Typ
- beschädigtes Template
- Nullpointer

Fehlerhafte Tiles werden deaktiviert und gekennzeichnet.

---

# 22. Sicherheit

Das System verhindert:

- doppelte Tile-IDs
- rekursive Aktivierungen
- inkonsistente Statuskombinationen
- ungültige Fokuszustände
- unbeabsichtigte Mehrfachaktivierungen

Destruktive Funktionen müssen zusätzlich gekennzeichnet und vor der Ausführung bestätigt werden.

---

# 23. API

```c
typedef struct nova_boot_tile nova_boot_tile_t;

typedef enum
{
    NOVA_TILE_STANDARD,
    NOVA_TILE_PRIMARY,
    NOVA_TILE_RECOVERY,
    NOVA_TILE_MAINTENANCE,
    NOVA_TILE_DIAGNOSTIC,
    NOVA_TILE_DESTRUCTIVE,
    NOVA_TILE_CUSTOM
}
nova_boot_tile_type_t;

nova_result_t
nova_boot_tile_create(
    nova_boot_tile_type_t type,
    nova_boot_tile_t** tile);

nova_result_t
nova_boot_tile_set_title(
    nova_boot_tile_t* tile,
    const char* title);

nova_result_t
nova_boot_tile_set_icon(
    nova_boot_tile_t* tile,
    nova_icon_t icon);

nova_result_t
nova_boot_tile_set_action(
    nova_boot_tile_t* tile,
    nova_action_t action);

nova_result_t
nova_boot_tile_set_status(
    nova_boot_tile_t* tile,
    nova_status_t status);
```

---

# 24. Diagnosedaten

Zu erfassen sind:

- Tile-ID
- Tile-Typ
- Aktivierungen
- Fokuswechsel
- Statuswechsel
- Renderzeit
- Speicherverbrauch
- Fehleranzahl

---

# 25. Testfälle

Zu testen sind:

- Standard Tile
- Recovery Tile
- Maintenance Tile
- Diagnostic Tile
- Fokus
- Controllersteuerung
- Touch
- Themewechsel
- Accessibility
- Fehlerfälle

---

# 26. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- jede Kachel genau eine Funktion repräsentiert.
- Fokus und Navigation deterministisch arbeiten.
- Statusinformationen korrekt dargestellt werden.
- Theme und Templates vollständig verwendet werden.
- Accessibility vollständig unterstützt wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 27. Referenzen

## ADRs

- ADR-BOOTCONTROL-TILE-0001 – Boot Option Tile
- ADR-BOOTLAYOUT-0004 – Grid Layout
- ADR-BOOTTHEME-0001 – Theme-System
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0002 – Deterministische UI-Verarbeitung

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCONTROL-0002 – Control State Model
- NPSPEC-BOOTCONTROL-0003 – Control Styling
- NPSPEC-BOOTCONTROL-0004 – Control Templates
- NPSPEC-BOOTLAYOUT-0004 – Grid Layout
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 28. Zusammenfassung

Das **Boot Option Tile** definiert die standardisierte Kachel zur Darstellung bootrelevanter Funktionen innerhalb der NovaOS-Bootoberfläche. Es kombiniert Symbol, Titel, Beschreibung und Statusinformationen in einem großen, leicht bedienbaren UI-Element. Durch vollständige Theme-Integration, konsistente Zustandsverwaltung, Animationen, Accessibility und statische Speicherverwaltung bildet es die Grundlage einer modernen, übersichtlichen und performant bedienbaren Startoberfläche auf BIOS- und UEFI-Systemen.