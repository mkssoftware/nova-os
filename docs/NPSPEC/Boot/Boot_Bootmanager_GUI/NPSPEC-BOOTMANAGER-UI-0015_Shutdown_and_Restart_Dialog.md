# NPSPEC-BOOTMANAGER-UI-0015
# Shutdown and Restart Dialog

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTMANAGER-UI-0015 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Manager UI |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager / Recovery / Installer |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-DIALOG-0001, NPSPEC-BOOTCONTROL-BUTTON-0001, NPSPEC-BOOTCONTROL-LABEL-0001, NPSPEC-BOOTCONTROL-STATUSBADGE-0001, NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTPOWER-0001 |

---

# 1. Ziel

Diese Spezifikation definiert den **Shutdown and Restart Dialog** des NovaOS Boot Managers.

Der Dialog stellt eine einheitliche Oberfläche zum sicheren Ausschalten oder Neustarten des Systems innerhalb der Bootumgebung bereit. Er wird verwendet, wenn der Benutzer den Boot Manager verlassen möchte oder ein Neustart nach Recovery-, Diagnose- oder Wartungsarbeiten erforderlich ist.

Der Dialog muss eindeutig, sicher und schnell verständlich sein.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Boot Manager
- Recovery Environment
- Installer
- Diagnosemodule
- Self-Healing
- Wartungsmodus

---

# 3. Ziele

Der Dialog muss:

- unbeabsichtigtes Ausschalten verhindern
- klare Auswahlmöglichkeiten bieten
- den aktuellen Systemstatus anzeigen
- vollständig per Tastatur bedienbar sein
- deterministisch arbeiten
- die NovaOS Design Language verwenden

---

# 4. Dialogaufbau

```text
+-----------------------------------------------------------+

Power Icon

Shutdown / Restart

------------------------------------------------------------

Möchten Sie das System herunterfahren
oder neu starten?

------------------------------------------------------------

Systemstatus

------------------------------------------------------------

Optionen

○ Herunterfahren

○ Neustarten

○ Neustart in Recovery

○ Neustart in Firmware Setup

------------------------------------------------------------

[ Bestätigen ]

[ Abbrechen ]

+-----------------------------------------------------------+
```

---

# 5. Komponenten

Der Dialog besteht aus:

- Header
- Symbolbereich
- Beschreibung
- Statusbereich
- Optionsbereich
- Aktionsbereich

Alle Komponenten verwenden Standard-Boot Controls.

---

# 6. Header

Der Header enthält:

- Power-Icon
- Dialogtitel
- optionale Unterüberschrift

---

# 7. Statusbereich

Der Statusbereich zeigt:

- aktueller Bootmodus
- Firmwaremodus
- erkannte Betriebssysteme
- laufende Diagnose
- Recovery aktiv
- Self-Healing aktiv

Falls Wartungsprozesse aktiv sind, werden diese angezeigt.

---

# 8. Optionen

Mindestens folgende Aktionen werden unterstützt:

- Herunterfahren
- Neustarten
- Neustart in Recovery
- Neustart in Firmware Setup
- Neustart in Netzwerk-Boot (optional)

Weitere Aktionen können durch Module registriert werden.

---

# 9. Beschreibung

Die Beschreibung erläutert:

- welche Aktion ausgeführt wird
- ob Daten gespeichert werden
- ob ein Neustart erforderlich ist
- mögliche Auswirkungen

Texte müssen vollständig lokalisierbar sein.

---

# 10. Aktionsbereich

Standardaktionen:

- Bestätigen
- Abbrechen

Optional:

- Erweiterte Optionen
- Energieinformationen
- Hilfe

Die primäre Aktion richtet sich nach der ausgewählten Option.

---

# 11. Navigation

Unterstützt werden:

- Tastatur
- Maus
- Touch

Standard-Tasten:

- Enter → Bestätigen
- Esc → Abbrechen
- Pfeiltasten → Auswahl wechseln
- Tab → Fokus wechseln

---

# 12. Fokus

Beim Öffnen erhält:

- die aktuell empfohlene Aktion den Fokus.

Der Fokus muss:

- eindeutig sichtbar
- weich animiert
- jederzeit nachvollziehbar sein.

---

# 13. Animationen

Unterstützt werden:

- Fade-In
- Fade-Out
- Fokusanimation
- Buttonanimation
- Dialogübergang

Alle Animationen verwenden Motion Tokens.

---

# 14. Themeintegration

Unterstützt werden:

- Dark Theme
- Light Theme
- High Contrast Theme

Alle Farben stammen ausschließlich aus den Boot Design Tokens.

---

# 15. Accessibility

Der Dialog unterstützt:

- Screenreader
- vollständige Tastatursteuerung
- hohe Kontraste
- große Schriftgrößen
- reduzierte Animationen

Alle Optionen besitzen eindeutige Beschriftungen.

---

# 16. Fehlerfälle

Zu behandeln sind:

- Neustart nicht möglich
- Firmware Setup nicht verfügbar
- Recovery nicht vorhanden
- laufende Diagnose verhindert Ausschalten
- Energieverwaltung nicht verfügbar

Fehler werden als Dialog dargestellt.

---

# 17. Sicherheit

Vor der Ausführung werden geprüft:

- laufende Recovery-Prozesse
- aktive Self-Healing-Vorgänge
- kritische Schreiboperationen
- Firmware-Neustart verfügbar

Der Dialog verhindert unbeabsichtigte Energieoperationen durch eine explizite Bestätigung. Systemdialoge für Neustart und Ausschalten sollten klar als systemweite, modale Aktionen erkennbar sein und die Aufmerksamkeit des Benutzers erfordern. :contentReference[oaicite:0]{index=0}

---

# 18. Speicherverwaltung

Die Oberfläche verwendet:

- Dialog Pool
- Button Pool
- Resource Cache
- Theme Cache

Heap-Allokationen während der Darstellung sind verboten.

---

# 19. Performance

Anforderungen:

- Öffnungszeit < 30 ms
- O(1)-Dialog Lookup
- O(1)-Theme Lookup
- O(1)-Action Lookup
- deterministische Renderzeit

---

# 20. API

```c
typedef enum
{
    NOVA_POWER_ACTION_SHUTDOWN,
    NOVA_POWER_ACTION_RESTART,
    NOVA_POWER_ACTION_RECOVERY,
    NOVA_POWER_ACTION_FIRMWARE_SETUP,
    NOVA_POWER_ACTION_NETWORK_BOOT
}
nova_power_action_t;

typedef struct
{
    bool firmware_supported;
    bool recovery_available;
    bool diagnostics_running;
}
nova_power_dialog_status_t;

nova_result_t
nova_power_dialog_open(void);

nova_result_t
nova_power_dialog_execute(
    nova_power_action_t action);

nova_result_t
nova_power_dialog_cancel(void);

const nova_power_dialog_status_t*
nova_power_dialog_status(void);
```

---

# 21. Diagnosedaten

Zu erfassen sind:

- gewählte Aktion
- Bootmodus
- Firmwaremodus
- Recoverystatus
- Diagnosestatus
- Benutzeraktion
- Dialogdauer

---

# 22. Testfälle

Zu testen sind:

- Herunterfahren
- Neustart
- Neustart in Recovery
- Neustart in Firmware
- Dialogabbruch
- Themewechsel
- Accessibility
- Tastatursteuerung
- Touchsteuerung
- Fehlerfälle

---

# 23. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Energieoptionen korrekt dargestellt werden.
- Herunterfahren und Neustart zuverlässig funktionieren.
- Recovery- und Firmware-Neustart unterstützt werden, sofern verfügbar.
- Dark-, Light- und High-Contrast-Themes unterstützt werden.
- sämtliche Texte lokalisiert werden können.
- kritische Energieoperationen bestätigt werden müssen.
- keine Heap-Allokationen während der Darstellung stattfinden.
- sämtliche Animationen ausschließlich Motion Tokens verwenden.

---

# 24. Referenzen

## ADRs

- ADR-BOOTMANAGER-UI-0015 – Shutdown and Restart Dialog
- ADR-BOOTPOWER-0001 – Power Management
- ADR-BOOTDESIGN-0001 – Boot Design Language

## NPSPECs

- NPSPEC-BOOTCONTROL-DIALOG-0001 – Dialog
- NPSPEC-BOOTCONTROL-BUTTON-0001 – Button
- NPSPEC-BOOTCONTROL-LABEL-0001 – Label
- NPSPEC-BOOTCONTROL-STATUSBADGE-0001 – Status Badge
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTMOTION-0001 – Motion Engine
- NPSPEC-BOOTPOWER-0001 – Power Management

---

# 25. Zusammenfassung

Der **Shutdown and Restart Dialog** stellt die standardisierte Benutzeroberfläche für sämtliche Energieoperationen innerhalb des NovaOS Boot Managers bereit. Er ermöglicht das sichere Herunterfahren, den Neustart sowie – sofern unterstützt – den direkten Neustart in Recovery-, Firmware- oder Netzwerk-Boot-Modi. Durch die vollständige Integration in die NovaOS Design Language, das Theme-System und die Motion Engine entsteht eine konsistente, barrierefreie und sichere Benutzeroberfläche für alle Energie- und Neustartfunktionen innerhalb der Pre-Boot-Umgebung.