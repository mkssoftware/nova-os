# NPSPEC-BOOTMANAGER-UI-0005
# Self-Healing Menu

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTMANAGER-UI-0005 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Manager UI |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager / Self-Healing Environment |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTMANAGER-UI-0001, NPSPEC-BOOTCONTROL-TILE-0001, NPSPEC-BOOTCONTROL-PROGRESS-0001, NPSPEC-BOOTDIALOG-0001, NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTSELFHEAL-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Self-Healing Menu** des NovaOS Boot Managers.

Das Self-Healing Menu dient als zentrale Oberfläche für die automatische Diagnose, Reparatur und Wiederherstellung systemkritischer Komponenten. Es bündelt sämtliche Self-Healing-Funktionen in einer eigenständigen Benutzeroberfläche und ermöglicht sowohl automatische als auch manuelle Reparaturabläufe.

Das Menü muss unabhängig vom installierten Betriebssystem funktionieren.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Bootloader
- Boot Manager
- Kernel
- Systempartitionen
- Konfigurationsdateien
- Snapshots
- Integritätsprüfung
- zukünftige Self-Healing-Module

---

# 3. Ziele

Das Self-Healing Menu muss:

- automatische Reparaturen ermöglichen
- den aktuellen Systemzustand transparent darstellen
- sichere Wiederherstellungsoptionen bereitstellen
- vollständig per Tastatur bedienbar sein
- deterministisch arbeiten
- modular erweiterbar sein

---

# 4. Bildschirmaufbau

```text
+-------------------------------------------------------------+

Zurück

NovaOS Self-Healing

--------------------------------------------------------------

Systemstatus

--------------------------------------------------------------

Self-Healing Option Tiles

--------------------------------------------------------------

Statusbeschreibung

--------------------------------------------------------------

Fortschritt

--------------------------------------------------------------

[ Reparatur starten ]

[ Bericht anzeigen ]

[ Zurück ]

+-------------------------------------------------------------+
```

---

# 5. Komponenten

Der Bildschirm besteht aus:

- Header
- Systemstatus
- Self-Healing-Optionen
- Informationsbereich
- Fortschrittsanzeige
- Aktionsbereich

Alle Komponenten verwenden Standard-Boot Controls.

---

# 6. Header

Der Header enthält:

- Zurücknavigation
- Self-Healing-Icon
- Titel
- Modulversion

Der Header bleibt permanent sichtbar.

---

# 7. Systemstatus

Der Statusbereich zeigt:

- Gesamtzustand
- Integritätsstatus
- Snapshotstatus
- Bootstatus
- Kernelstatus
- Dateisystemstatus
- letzter Self-Healing-Lauf

Der Gesamtstatus wird zusätzlich als Status Badge dargestellt.

---

# 8. Self-Healing Optionen

Mindestens folgende Optionen werden unterstützt:

- Gesamtsystem prüfen
- Bootloader reparieren
- Kernel reparieren
- Systemdateien wiederherstellen
- Snapshot wiederherstellen
- Integritätsprüfung
- beschädigte Dateien ersetzen
- automatische Reparatur
- Self-Healing-Protokoll anzeigen

Neue Module können ergänzt werden.

---

# 9. Self-Healing Tile

Jede Funktion wird als Boot Option Tile dargestellt.

Ein Tile enthält:

- Icon
- Titel
- Kurzbeschreibung
- Status Badge
- Verfügbarkeitsstatus

---

# 10. Informationsbereich

Beim Markieren einer Funktion werden angezeigt:

- Beschreibung
- erkannte Probleme
- erwartete Dauer
- Voraussetzungen
- mögliche Auswirkungen

---

# 11. Fortschrittsanzeige

Während einer Reparatur werden angezeigt:

- Fortschrittsbalken
- aktueller Arbeitsschritt
- geschätzte Restzeit
- Anzahl abgeschlossener Schritte
- erkannte Fehler

Die Anzeige verwendet den Progress Indicator.

---

# 12. Aktionsbereich

Standardaktionen:

- Reparatur starten
- Bericht anzeigen
- Zurück

Optional:

- Automatische Reparatur aktivieren
- Diagnose exportieren
- Snapshot auswählen

---

# 13. Navigation

Unterstützt werden:

- Tastatur
- Maus
- Touch
- Gamepad (optional)

Die Navigation erfolgt gemäß Boot Input Architecture.

---

# 14. Fokus

Der Fokus muss:

- jederzeit eindeutig sichtbar sein
- weich animiert werden
- auch während Reparaturvorgängen erhalten bleiben

---

# 15. Animationen

Unterstützt werden:

- Tileanimationen
- Fokusanimationen
- Fortschrittsanimationen
- Statusanimationen
- Dialoganimationen

Alle Bewegungen verwenden Motion Tokens.

---

# 16. Themeintegration

Unterstützt werden:

- Dark Theme
- Light Theme
- High Contrast Theme

Alle Farben stammen ausschließlich aus Design Tokens.

---

# 17. Accessibility

Das Self-Healing Menu unterstützt:

- Screenreader
- Tastatursteuerung
- hohe Kontraste
- große Schriftgrößen
- reduzierte Animationen

Alle Reparaturfunktionen müssen vollständig ohne Maus erreichbar sein.

---

# 18. Fehlerfälle

Zu behandeln sind:

- Reparatur fehlgeschlagen
- Snapshot beschädigt
- Integritätsprüfung fehlgeschlagen
- Datenträgerfehler
- fehlende Recovery-Komponenten
- nicht unterstützte Reparatur

Fehler werden über Dialoge dargestellt.

---

# 19. Sicherheit

Vor jeder Reparatur werden geprüft:

- Integrität
- Datenträgerstatus
- Snapshotverfügbarkeit
- Signaturen
- ausreichende Ressourcen

Kritische Aktionen verlangen eine ausdrückliche Bestätigung.

---

# 20. Speicherverwaltung

Die Oberfläche verwendet:

- Tile Pool
- Progress Pool
- Dialog Pool
- Resource Cache
- Theme Cache

Heap-Allokationen während der Darstellung sind verboten.

---

# 21. Performance

Anforderungen:

- Öffnungszeit < 75 ms
- O(1)-Tile Lookup
- O(1)-Status Lookup
- O(1)-Theme Lookup
- deterministische Renderzeit

Alle Self-Healing-Module werden beim Öffnen erkannt.

---

# 22. API

```c
typedef enum
{
    NOVA_SELFHEAL_VERIFY,
    NOVA_SELFHEAL_BOOTLOADER,
    NOVA_SELFHEAL_KERNEL,
    NOVA_SELFHEAL_SYSTEM_FILES,
    NOVA_SELFHEAL_SNAPSHOT,
    NOVA_SELFHEAL_AUTO_REPAIR,
    NOVA_SELFHEAL_REPORT
}
nova_selfheal_action_t;

nova_result_t
nova_selfheal_menu_open(void);

nova_result_t
nova_selfheal_execute(
    nova_selfheal_action_t action);

nova_result_t
nova_selfheal_menu_close(void);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Integritätsstatus
- Anzahl reparierter Komponenten
- Reparaturdauer
- erkannte Fehler
- Theme
- Benutzeraktionen
- Validierungsfehler

---

# 24. Testfälle

Zu testen sind:

- automatische Reparatur
- Kernelreparatur
- Bootloaderreparatur
- Snapshotwiederherstellung
- Integritätsprüfung
- Themewechsel
- Tastatursteuerung
- Accessibility
- Fortschrittsanzeige
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Self-Healing-Funktionen als Boot Option Tiles dargestellt werden.
- der Systemzustand jederzeit sichtbar ist.
- Fortschrittsanzeigen den aktuellen Reparaturstatus korrekt darstellen.
- Dark-, Light- und High-Contrast-Themes unterstützt werden.
- Reparaturmodule modular erweitert werden können.
- kritische Aktionen bestätigt werden müssen.
- keine Heap-Allokationen während der Darstellung stattfinden.
- sämtliche Animationen ausschließlich Motion Tokens verwenden.

---

# 26. Referenzen

## ADRs

- ADR-BOOTMANAGER-UI-0005 – Self-Healing Menu
- ADR-BOOTSELFHEAL-0001 – Self-Healing Framework
- ADR-BOOTDESIGN-0001 – Boot Design Language

## NPSPECs

- NPSPEC-BOOTMANAGER-UI-0001 – Main Boot Selection Screen
- NPSPEC-BOOTCONTROL-TILE-0001 – Boot Option Tile
- NPSPEC-BOOTCONTROL-PROGRESS-0001 – Progress Indicator
- NPSPEC-BOOTDIALOG-0001 – Dialog
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTMOTION-0001 – Motion Engine
- NPSPEC-BOOTSELFHEAL-0001 – Self-Healing Framework

---

# 27. Zusammenfassung

Das **Self-Healing Menu** stellt die zentrale Benutzeroberfläche für alle automatischen Diagnose-, Reparatur- und Wiederherstellungsfunktionen des NovaOS Boot Managers bereit. Es kombiniert Systemstatus, modulare Reparaturfunktionen, Fortschrittsanzeigen und detaillierte Diagnoseinformationen in einer konsistenten Oberfläche auf Basis der NovaOS Design Language. Durch vollständige Integration in das Theme-System, die Motion Engine und das Accessibility-Framework ermöglicht das Self-Healing Menu eine sichere, nachvollziehbare und benutzerfreundliche Wiederherstellung kritischer Systemkomponenten bereits vor dem Start des Betriebssystems.