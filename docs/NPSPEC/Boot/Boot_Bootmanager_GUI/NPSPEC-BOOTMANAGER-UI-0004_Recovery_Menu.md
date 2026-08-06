# NPSPEC-BOOTMANAGER-UI-0004
# Recovery Menu

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTMANAGER-UI-0004 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Manager UI |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager / Recovery Environment |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTMANAGER-UI-0001, NPSPEC-BOOTDIALOG-0001, NPSPEC-BOOTCONTROL-LIST-0001, NPSPEC-BOOTCONTROL-TILE-0001, NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTRECOVERY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Recovery Menu** des NovaOS Boot Managers.

Das Recovery Menu stellt sämtliche Funktionen zur Diagnose, Reparatur und Wiederherstellung des Systems bereit. Es dient als zentrale Oberfläche für Wartungs- und Notfallmaßnahmen und muss auch bei beschädigten Betriebssysteminstallationen zuverlässig funktionieren.

Die Benutzeroberfläche ist vollständig unabhängig vom installierten Betriebssystem und basiert ausschließlich auf der NovaOS-Bootumgebung.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- lokale Recovery-Systeme
- Wiederherstellungspartitionen
- externe Recovery-Medien
- Netzwerk-Recovery (optional)
- zukünftige Recovery-Module

---

# 3. Ziele

Das Recovery Menu muss:

- alle Wiederherstellungsfunktionen zentral bereitstellen
- auch bei Systemfehlern funktionsfähig bleiben
- vollständig per Tastatur bedienbar sein
- deterministisch arbeiten
- modular erweiterbar sein
- dieselbe Designsprache wie der Boot Manager verwenden

---

# 4. Bildschirmaufbau

```text
+-----------------------------------------------------------+

Zurück

Recovery

------------------------------------------------------------

Recovery Option Tiles

------------------------------------------------------------

Beschreibung

------------------------------------------------------------

[ Ausführen ]

[ Zurück ]

+-----------------------------------------------------------+
```

---

# 5. Komponenten

Der Bildschirm besteht aus:

- Header
- Recovery-Optionsbereich
- Informationsbereich
- Aktionsbereich
- Statusbereich

Alle Komponenten verwenden Standard-Boot Controls.

---

# 6. Header

Der Header enthält:

- Zurücknavigation
- Recovery-Icon
- Titel
- Versionsnummer (optional)

Der Header bleibt permanent sichtbar.

---

# 7. Recovery-Optionen

Mindestens folgende Optionen werden unterstützt:

- Automatische Reparatur
- Bootloader reparieren
- Dateisystem prüfen
- Integrität prüfen
- Snapshot wiederherstellen
- System zurücksetzen
- Recovery-Konsole
- Speicherdiagnose
- Datenträgerdiagnose
- EFI-Reparatur
- Netzwerk-Recovery (optional)

Weitere Module können ergänzt werden.

---

# 8. Recovery Option Tile

Jede Recovery-Funktion wird als Boot Option Tile dargestellt.

Ein Tile enthält:

- Icon
- Titel
- Kurzbeschreibung
- Status Badge

---

# 9. Informationsbereich

Beim Markieren einer Funktion werden angezeigt:

- ausführliche Beschreibung
- Voraussetzungen
- Auswirkungen
- benötigte Zeit
- mögliche Risiken

---

# 10. Aktionsbereich

Standardaktionen:

- Ausführen
- Zurück

Optional:

- Hilfe
- Dokumentation
- Diagnosebericht anzeigen

---

# 11. Statusbereich

Der Statusbereich zeigt:

- erkannte Betriebssysteme
- Recovery-Version
- Integritätsstatus
- Datenträgerstatus
- Eingabegeräte

---

# 12. Navigation

Unterstützt werden:

- Tastatur
- Maus
- Touch
- Gamepad (optional)

Die Navigation folgt der Boot Input Architecture.

---

# 13. Fokus

Der Fokus muss:

- jederzeit eindeutig sichtbar sein
- weich animiert werden
- niemals verloren gehen

Der erste Recovery-Eintrag erhält initial den Fokus.

---

# 14. Animationen

Unterstützt werden:

- Seitenübergänge
- Tile-Auswahl
- Fokusanimationen
- Dialoganimationen
- Fortschrittsanzeigen

Alle Animationen verwenden Motion Tokens.

---

# 15. Themeintegration

Unterstützt werden:

- Dark Theme
- Light Theme
- High Contrast Theme

Alle Farben stammen ausschließlich aus Design Tokens.

---

# 16. Accessibility

Das Recovery Menu unterstützt:

- Screenreader
- Tastatursteuerung
- hohe Kontraste
- große Schriftgrößen
- reduzierte Animationen

Alle Funktionen müssen ohne Maus erreichbar sein.

---

# 17. Fehlerfälle

Zu behandeln sind:

- Recovery nicht verfügbar
- beschädigte Partition
- Snapshot nicht vorhanden
- Datenträgerfehler
- Integritätsprüfung fehlgeschlagen
- Reparatur fehlgeschlagen

Fehler werden über Dialoge dargestellt.

---

# 18. Sicherheit

Vor jeder Recovery-Aktion werden geprüft:

- Datenträgerstatus
- Berechtigungen
- Integrität
- Wiederherstellbarkeit
- erforderliche Ressourcen

Kritische Aktionen verlangen eine Bestätigung.

---

# 19. Speicherverwaltung

Die Oberfläche verwendet:

- Tile Pool
- Dialog Pool
- Resource Cache
- Theme Cache

Heap-Allokationen während der Darstellung sind verboten.

---

# 20. Performance

Anforderungen:

- Öffnungszeit < 75 ms
- O(1)-Tile Lookup
- O(1)-Theme Lookup
- O(1)-Navigation
- deterministische Renderzeit

Recovery-Module werden vor dem Anzeigen erkannt und registriert.

---

# 21. API

```c
typedef enum
{
    NOVA_RECOVERY_AUTO_REPAIR,
    NOVA_RECOVERY_BOOTLOADER,
    NOVA_RECOVERY_FILESYSTEM,
    NOVA_RECOVERY_VERIFY,
    NOVA_RECOVERY_SNAPSHOT,
    NOVA_RECOVERY_RESET,
    NOVA_RECOVERY_CONSOLE,
    NOVA_RECOVERY_MEMORY_TEST,
    NOVA_RECOVERY_DISK_TEST
}
nova_recovery_action_t;

nova_result_t
nova_recovery_menu_open(void);

nova_result_t
nova_recovery_execute(
    nova_recovery_action_t action);

nova_result_t
nova_recovery_menu_close(void);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- Recovery-Version
- erkannte Module
- Benutzeraktionen
- Theme
- Ladezeit
- Validierungsfehler

---

# 23. Testfälle

Zu testen sind:

- automatische Reparatur
- Bootloader-Reparatur
- Snapshot-Wiederherstellung
- Integritätsprüfung
- Dateisystemprüfung
- Themewechsel
- Tastatursteuerung
- Touchsteuerung
- Accessibility
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Recovery-Funktionen als Recovery Option Tiles dargestellt werden.
- Navigation per Tastatur, Maus und Touch vollständig funktioniert.
- Dark-, Light- und High-Contrast-Themes unterstützt werden.
- Recovery-Module modular erweitert werden können.
- kritische Aktionen vor der Ausführung bestätigt werden.
- beschädigte Recovery-Module eindeutig gekennzeichnet werden.
- keine Heap-Allokationen während der Darstellung stattfinden.
- sämtliche Animationen ausschließlich Motion Tokens verwenden.

---

# 25. Referenzen

## ADRs

- ADR-BOOTMANAGER-UI-0004 – Recovery Menu
- ADR-BOOTRECOVERY-0001 – Recovery Framework
- ADR-BOOTDESIGN-0001 – Boot Design Language

## NPSPECs

- NPSPEC-BOOTMANAGER-UI-0001 – Main Boot Selection Screen
- NPSPEC-BOOTDIALOG-0001 – Dialog
- NPSPEC-BOOTCONTROL-LIST-0001 – List
- NPSPEC-BOOTCONTROL-TILE-0001 – Boot Option Tile
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTMOTION-0001 – Motion Engine
- NPSPEC-BOOTRECOVERY-0001 – Recovery Framework

---

# 26. Zusammenfassung

Das **Recovery Menu** stellt die zentrale Oberfläche für Diagnose-, Reparatur- und Wiederherstellungsfunktionen innerhalb des NovaOS Boot Managers bereit. Es bündelt sämtliche Recovery-Module in einer modularen, konsistenten und vollständig themefähigen Benutzeroberfläche. Durch die Integration in die NovaOS Design Language, das Motion Framework und das Accessibility-System ermöglicht es eine sichere, leistungsfähige und benutzerfreundliche Wiederherstellung des Systems – selbst bei schwerwiegenden Startproblemen.