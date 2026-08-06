# NPSPEC-BOOTMANAGER-UI-0003
# Advanced Boot Options

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTMANAGER-UI-0003 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Manager UI |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTMANAGER-UI-0001, NPSPEC-BOOTDIALOG-0001, NPSPEC-BOOTCONTROL-LIST-0001, NPSPEC-BOOTCONTROL-TILE-0001, NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTMOTION-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die **Advanced Boot Options** des NovaOS Boot Managers.

Die Advanced Boot Options stellen eine erweiterte Konfigurationsoberfläche für einen ausgewählten Boot-Eintrag bereit. Sie ermöglichen den Zugriff auf Diagnose-, Wiederherstellungs- und Startoptionen, ohne dass die Standard-Bootauswahl überladen wird.

Der Bildschirm richtet sich sowohl an fortgeschrittene Benutzer als auch an Administratoren und Servicetechniker.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- lokale Betriebssysteme
- Recovery-Systeme
- Installationsmedien
- Netzwerk-Boot
- zukünftige Bootmodule

---

# 3. Ziele

Die Advanced Boot Options müssen:

- alle erweiterten Startoptionen zentral bündeln
- leicht verständlich sein
- vollständig per Tastatur bedienbar sein
- deterministisch arbeiten
- Design Language und Themes übernehmen
- modular erweiterbar sein

---

# 4. Bildschirmaufbau

```text
+-----------------------------------------------------------+

Zurück

Advanced Boot Options

------------------------------------------------------------

Boot Option Tile

------------------------------------------------------------

Erweiterte Optionen

• Normal starten

• Recovery starten

• Safe Mode

• Safe Mode mit Netzwerk

• Safe Mode mit Konsole

• Kernelparameter

• Debugmodus

• Integrität prüfen

• Reparatur

• Boot Logging

• EFI Shell (optional)

------------------------------------------------------------

Beschreibung

------------------------------------------------------------

[ Start ]

[ Zurück ]

+-----------------------------------------------------------+
```

---

# 5. Komponenten

Der Bildschirm besteht aus:

- Header
- ausgewähltes Boot Tile
- Optionsliste
- Beschreibungsbereich
- Aktionsbereich

Alle Komponenten verwenden Standard-Boot Controls.

---

# 6. Header

Der Header enthält:

- Zurücknavigation
- Titel
- Betriebssystemname
- Betriebssystemicon

Der Header bleibt permanent sichtbar.

---

# 7. Boot Entry

Der aktuell ausgewählte Boot-Eintrag wird als Boot Option Tile dargestellt.

Angezeigt werden:

- Icon
- Name
- Version
- Datenträger
- Bootmodus

---

# 8. Optionsliste

Mindestens folgende Optionen werden unterstützt:

- Normal starten
- Recovery starten
- Safe Mode
- Safe Mode mit Netzwerk
- Safe Mode mit Konsole
- Boot Logging aktivieren
- Kernelparameter bearbeiten
- Integrität prüfen
- Reparatur starten
- EFI Shell
- Benutzerdefinierte Optionen

Die Liste ist modular erweiterbar.

---

# 9. Beschreibungsbereich

Beim Markieren einer Option werden angezeigt:

- Kurzbeschreibung
- Auswirkungen
- Voraussetzungen
- mögliche Risiken
- geschätzte Dauer (optional)

---

# 10. Aktionsbereich

Standardaktionen:

- Start
- Zurück

Optional:

- Standard wiederherstellen
- Hilfe
- Dokumentation

---

# 11. Navigation

Unterstützt werden:

- Maus
- Tastatur
- Touch
- Gamepad (optional)

Die Navigation erfolgt gemäß Boot Input Architecture.

---

# 12. Fokus

Der Fokus muss:

- jederzeit eindeutig sichtbar sein
- weich animiert werden
- niemals verloren gehen

Der erste Listeneintrag erhält initial den Fokus.

---

# 13. Animationen

Animationen:

- Seitenwechsel
- Fokuswechsel
- Tileanimation
- Buttonanimation
- Dialogübergänge

Alle Bewegungen verwenden Motion Tokens.

---

# 14. Themeintegration

Unterstützt werden:

- Dark Theme
- Light Theme
- High Contrast Theme

Alle Farben stammen ausschließlich aus Design Tokens.

---

# 15. Accessibility

Der Bildschirm unterstützt:

- Screenreader
- Tastaturbedienung
- hohe Kontraste
- große Schriftgrößen
- reduzierte Animationen

Alle Funktionen müssen ohne Zeigegerät erreichbar sein.

---

# 16. Fehlerfälle

Zu behandeln sind:

- Recovery nicht vorhanden
- Safe Mode nicht unterstützt
- beschädigte Bootkonfiguration
- fehlende Kernelparameter
- Integritätsprüfung fehlgeschlagen

Fehler werden als Dialog dargestellt.

---

# 17. Sicherheit

Vor dem Start werden geprüft:

- Bootziel vorhanden
- Konfiguration gültig
- Recovery verfügbar
- Berechtigungen
- Integrität

Ungültige Optionen werden deaktiviert.

---

# 18. Speicherverwaltung

Die Oberfläche verwendet:

- List Pool
- Tile Pool
- Resource Cache
- Theme Cache

Heap-Allokationen während der Darstellung sind verboten.

---

# 19. Performance

Anforderungen:

- Öffnungszeit < 75 ms
- O(1)-Option Lookup
- O(1)-Theme Lookup
- O(1)-Navigation
- deterministische Renderzeit

Alle Optionen werden vor dem Anzeigen geladen.

---

# 20. API

```c
typedef enum
{
    NOVA_BOOT_OPTION_NORMAL,
    NOVA_BOOT_OPTION_RECOVERY,
    NOVA_BOOT_OPTION_SAFE_MODE,
    NOVA_BOOT_OPTION_SAFE_MODE_NETWORK,
    NOVA_BOOT_OPTION_SAFE_MODE_CONSOLE,
    NOVA_BOOT_OPTION_KERNEL_PARAMETERS,
    NOVA_BOOT_OPTION_BOOT_LOG,
    NOVA_BOOT_OPTION_VERIFY,
    NOVA_BOOT_OPTION_REPAIR
}
nova_boot_option_t;

nova_result_t
nova_boot_options_open(
    uint32_t boot_entry_id);

nova_result_t
nova_boot_options_execute(
    nova_boot_option_t option);

nova_result_t
nova_boot_options_close(void);
```

---

# 21. Diagnosedaten

Zu erfassen sind:

- ausgewählte Option
- Boot Entry ID
- Theme
- Ladezeit
- Benutzeraktionen
- Validierungsfehler

---

# 22. Testfälle

Zu testen sind:

- Normalstart
- Recovery
- Safe Mode
- Kernelparameter
- Integritätsprüfung
- Themewechsel
- Tastatursteuerung
- Touchsteuerung
- Accessibility
- Fehlerfälle

---

# 23. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche erweiterten Bootoptionen korrekt dargestellt werden.
- Optionen modular erweitert werden können.
- Navigation per Tastatur, Maus und Touch vollständig funktioniert.
- Dark-, Light- und High-Contrast-Themes unterstützt werden.
- Beschreibungen kontextabhängig aktualisiert werden.
- ungültige Optionen automatisch deaktiviert werden.
- keine Heap-Allokationen während der Darstellung stattfinden.
- alle Animationen ausschließlich Motion Tokens verwenden.

---

# 24. Referenzen

## ADRs

- ADR-BOOTMANAGER-UI-0003 – Advanced Boot Options
- ADR-BOOTDIALOG-0001 – Dialog Framework
- ADR-BOOTDESIGN-0001 – Boot Design Language

## NPSPECs

- NPSPEC-BOOTMANAGER-UI-0001 – Main Boot Selection Screen
- NPSPEC-BOOTDIALOG-0001 – Dialog
- NPSPEC-BOOTCONTROL-LIST-0001 – List
- NPSPEC-BOOTCONTROL-TILE-0001 – Boot Option Tile
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTMOTION-0001 – Motion Engine

---

# 25. Zusammenfassung

Die **Advanced Boot Options** stellen eine zentrale Oberfläche für alle erweiterten Start- und Diagnosefunktionen des NovaOS Boot Managers bereit. Sie bündeln Recovery-, Safe-Mode-, Debug-, Reparatur- und Integritätsfunktionen in einer übersichtlichen, modular erweiterbaren Oberfläche. Durch die vollständige Integration in die NovaOS Design Language, das Theme-System und die Motion Engine entsteht eine konsistente und leistungsfähige Verwaltungsoberfläche für fortgeschrittene Bootvorgänge.