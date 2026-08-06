# NPSPEC-BOOTMANAGER-UI-0001
## Main Boot Selection Screen

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTMANAGER-UI-0001 |
| Titel | Main Boot Selection Screen |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Manager UI |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTLAYOUT-0001, NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTRESOURCE-0001, NPSPEC-BOOTINPUT-0001 |

---

# 1. Ziel

Diese Spezifikation definiert den **Hauptbildschirm des NovaOS Boot Managers**.

Der Main Boot Selection Screen ist die zentrale Benutzeroberfläche unmittelbar nach dem Start des Boot Managers. Er dient der Auswahl des zu startenden Betriebssystems oder Boot-Eintrags und stellt gleichzeitig den Einstiegspunkt für Recovery-, Diagnose- und Systemeinstellungen dar.

Der Bildschirm muss innerhalb weniger Sekunden verständlich sein und sowohl per Tastatur, Maus als auch Touch vollständig bedienbar bleiben.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- BIOS
- UEFI
- lokale Datenträger
- Mehrfach-Boot-Systeme
- Recovery
- Installer
- Diagnosemodule

---

# 3. Ziele

Der Hauptbildschirm muss:

- schnell verständlich sein
- alle Bootoptionen übersichtlich darstellen
- vollständig barrierefrei bedienbar sein
- moderne Animationen verwenden
- nahtlos zur NovaOS Design Language passen
- auf allen Auflösungen funktionieren

---

# 4. Bildschirmaufbau

Der Bildschirm besteht aus:

```text
+------------------------------------------------------+

NovaOS Logo

Titel

Untertitel

--------------------------------------------------------

Boot Option Tiles

Boot Option Tiles

Boot Option Tiles

--------------------------------------------------------

Information Panel

--------------------------------------------------------

Aktionen

Boot

Recovery

Diagnose

Einstellungen

Herunterfahren

--------------------------------------------------------

Statusleiste

Countdown

Version

Datum

Eingabegeräte

+------------------------------------------------------+
```

---

# 5. Hauptbereiche

Der Bildschirm besitzt folgende Bereiche:

- Header
- Bootliste
- Informationsbereich
- Aktionsbereich
- Statusleiste

Alle Bereiche besitzen feste Layoutregeln.

---

# 6. Header

Der Header enthält:

- NovaOS Logo
- Boot Manager Titel
- Versionsnummer
- optional Buildnummer

Der Header bleibt während der gesamten Sitzung sichtbar.

---

# 7. Bootliste

Die Bootliste verwendet **Boot Option Tiles**.

Jeder Eintrag enthält:

- Betriebssystemname
- Icon
- Datenträger
- Partition
- Bootmodus
- Standardkennzeichnung
- Fehlerstatus (optional)

---

# 8. Boot Option Tile

Jedes Tile besitzt:

- Glass Card
- Betriebssystemicon
- Titel
- Beschreibung
- Status Badge

Die Darstellung erfolgt gemäß:

- Boot Card
- Design Tokens
- Theme Tokens

---

# 9. Informationsbereich

Beim Markieren eines Eintrags werden angezeigt:

- vollständiger Name
- Version
- Kernel
- Datenträger
- Partitionsgröße
- Bootmodus
- letzter erfolgreicher Start
- Beschreibung

---

# 10. Aktionsbereich

Standardaktionen:

- Starten
- Erweiterte Optionen
- Recovery
- Diagnose
- Einstellungen
- Speicherprüfung
- Herunterfahren
- Neustart

Neue Aktionen können ergänzt werden.

---

# 11. Statusleiste

Die Statusleiste zeigt:

- Countdown
- aktuelle Uhrzeit
- Tastaturbelegung
- Eingabegeräte
- Theme
- Bootmodus
- Version

---

# 12. Navigation

Unterstützt werden:

- Maus
- Tastatur
- Touch
- Gamepad (optional)

Navigation erfolgt gemäß NPSPEC-BOOTINPUT.

---

# 13. Fokus

Der Fokus muss:

- jederzeit sichtbar sein
- weich animiert werden
- eindeutig hervorgehoben sein

Es darf niemals mehrere aktive Fokusobjekte geben.

---

# 14. Animationen

Animationen umfassen:

- Seiteneinblendung
- Tile-Auswahl
- Fokuswechsel
- Buttonanimationen
- Dialogübergänge

Alle Animationen verwenden Motion Tokens.

---

# 15. Themes

Unterstützt werden:

- Dark
- Light
- High Contrast

Alle Themes verwenden dieselben Design Tokens.

---

# 16. Accessibility

Der Bildschirm unterstützt:

- Screenreader
- hohe Kontraste
- große Schriftgrößen
- reduzierte Animationen
- vollständige Tastaturbedienung

---

# 17. Responsives Layout

Unterstützte Auflösungen:

- 800×600
- 1024×768
- 1280×720
- 1366×768
- 1920×1080
- höhere Auflösungen

Die Oberfläche skaliert proportional.

---

# 18. Fehlerfälle

Darzustellen sind:

- kein Betriebssystem gefunden
- beschädigter Bootloader
- beschädigte Partition
- fehlender Datenträger
- Recovery erforderlich

Fehler werden über Dialoge angezeigt.

---

# 19. Sicherheit

Der Bildschirm verhindert:

- ungültige Bootauswahl
- doppelte Booteinträge
- fehlerhafte Standardauswahl
- inkonsistente Zustände

Vor jedem Start erfolgt eine Validierung des Bootziels.

---

# 20. Speicherverwaltung

Die Oberfläche verwendet:

- Objektpools
- Tile Pools
- Resource Cache
- Theme Cache

Heap-Allokationen während der Darstellung sind verboten.

---

# 21. Performance

Anforderungen:

- Initialisierung < 150 ms nach UI-Start
- O(1)-Navigation
- O(1)-Tile Lookup
- O(1)-Theme Lookup
- deterministische Renderzeit

Alle Ressourcen werden vor dem ersten Frame geladen.

---

# 22. API

```c
typedef struct
{
    uint32_t boot_entry_count;
    uint32_t selected_entry;
    uint32_t default_entry;
}
nova_boot_selection_context_t;

nova_result_t
nova_boot_selection_initialize(void);

nova_result_t
nova_boot_selection_show(void);

nova_result_t
nova_boot_selection_select(
    uint32_t index);

nova_result_t
nova_boot_selection_boot_selected(void);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Anzahl Booteinträge
- ausgewählter Eintrag
- Standardeintrag
- Theme
- Eingabegerät
- Ladezeit
- FPS
- Validierungsfehler

---

# 24. Testfälle

Zu testen sind:

- ein Booteintrag
- mehrere Booteinträge
- beschädigte Einträge
- Themewechsel
- Tastatursteuerung
- Maussteuerung
- Touchsteuerung
- Accessibility
- verschiedene Auflösungen
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Booteinträge als Boot Option Tiles dargestellt werden.
- Navigation per Tastatur, Maus und Touch vollständig funktioniert.
- Informationen zum ausgewählten Eintrag korrekt angezeigt werden.
- Dark-, Light- und High-Contrast-Themes unterstützt werden.
- Animationen ausschließlich Motion Tokens verwenden.
- die Oberfläche auf allen unterstützten Auflösungen korrekt skaliert.
- keine Heap-Allokationen während der Darstellung stattfinden.
- die Bedienung auch bei beschädigten Bootkonfigurationen möglich bleibt.

---

# 26. Referenzen

## ADRs

- ADR-BOOTMANAGER-UI-0001 – Main Boot Selection Screen
- ADR-BOOTLAYOUT-0001 – Layout Engine
- ADR-BOOTCONTROL-0001 – Control Framework
- ADR-BOOTDESIGN-0001 – Boot Design Language

## NPSPECs

- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTMOTION-0001 – Motion Engine
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTINPUT-0001 – Boot Input Architecture

---

# 27. Zusammenfassung

Der **Main Boot Selection Screen** ist die zentrale Benutzeroberfläche des NovaOS Boot Managers. Er vereint Bootauswahl, Systeminformationen und Verwaltungsfunktionen in einer modernen, konsistenten Oberfläche auf Basis der NovaOS Design Language. Durch Boot Option Tiles, responsive Layouts, vollständige Unterstützung für verschiedene Eingabegeräte, Accessibility-Funktionen und deterministische Performance bildet dieser Bildschirm den primären Einstiegspunkt in die gesamte NovaOS-Plattform.