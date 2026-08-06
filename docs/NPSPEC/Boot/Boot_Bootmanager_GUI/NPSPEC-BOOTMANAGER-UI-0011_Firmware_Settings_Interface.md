# NPSPEC-BOOTMANAGER-UI-0011
# Firmware Settings Interface

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTMANAGER-UI-0011 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Manager UI |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager / BIOS / UEFI |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTMANAGER-UI-0001, NPSPEC-BOOTCONTROL-LIST-0001, NPSPEC-BOOTCONTROL-TILE-0001, NPSPEC-BOOTCONTROL-DIALOG-0001, NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTFIRMWARE-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die **Firmware Settings Interface** des NovaOS Boot Managers.

Die Firmware Settings Interface stellt eine einheitliche Oberfläche bereit, über die Benutzer Informationen zur Systemfirmware einsehen und – sofern von der Plattform unterstützt – direkt in das Firmware-Setup (BIOS/UEFI) wechseln können. Sie dient außerdem als zentrale Informationsseite für Firmware-, Secure-Boot- und Plattformstatus.

Die Oberfläche ersetzt **nicht** das BIOS-/UEFI-Setup des Mainboard-Herstellers, sondern bildet dessen Einstiegspunkt und stellt Firmwareinformationen konsistent innerhalb der NovaOS-Bootumgebung dar. UEFI definiert standardisierte Schnittstellen zwischen Firmware und Betriebssystem, die den Übergang in die Firmwareumgebung ermöglichen. :contentReference[oaicite:0]{index=0}

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- BIOS-Systeme
- UEFI-Systeme
- Secure Boot
- TPM
- Firmwareinformationen
- Firmware-Neustart
- zukünftige Firmwaredienste

---

# 3. Ziele

Die Firmware Settings Interface muss:

- Firmwareinformationen übersichtlich darstellen
- den Neustart in das Firmware-Setup unterstützen
- Sicherheitsinformationen anzeigen
- vollständig per Tastatur bedienbar sein
- deterministisch arbeiten
- modular erweiterbar sein

---

# 4. Bildschirmaufbau

```text
+-------------------------------------------------------------+

Zurück

Firmware Settings

--------------------------------------------------------------

Firmwareinformationen

--------------------------------------------------------------

Sicherheitsstatus

--------------------------------------------------------------

Firmwarefunktionen

--------------------------------------------------------------

[ Firmware Setup öffnen ]

[ Secure Boot Informationen ]

[ Plattformdetails ]

[ Aktualisieren ]

[ Zurück ]

+-------------------------------------------------------------+
```

---

# 5. Komponenten

Die Oberfläche besteht aus:

- Header
- Firmwareinformationen
- Sicherheitsbereich
- Funktionsbereich
- Aktionsbereich

Alle Komponenten verwenden Standard-Boot Controls.

---

# 6. Header

Der Header enthält:

- Zurücknavigation
- Firmware-Icon
- Titel
- Firmwaremodus

---

# 7. Firmwareinformationen

Folgende Informationen werden dargestellt:

- Firmwaretyp
- Firmwarehersteller
- Firmwareversion
- Builddatum
- BIOS-/UEFI-Modus
- Systemhersteller
- Mainboardmodell
- Plattformarchitektur

---

# 8. Sicherheitsinformationen

Der Sicherheitsbereich zeigt:

- Secure Boot Status
- TPM Status
- Platform Key vorhanden
- Secure Boot Datenbank
- Firmwareschutz
- Bootmodus

Diese Informationen werden ausschließlich lesend dargestellt.

---

# 9. Firmwarefunktionen

Unterstützte Funktionen:

- Firmware Setup öffnen
- Firmwareinformationen aktualisieren
- Plattforminformationen anzeigen
- Secure-Boot-Status anzeigen
- TPM-Informationen anzeigen
- Firmwarediagnose

Optionale Erweiterungen:

- Firmware-Logs
- Firmware-Ereignisse
- Firmware-Validierung

---

# 10. Firmware Setup

Wird diese Funktion ausgewählt:

- wird der Benutzer informiert
- erfolgt eine Bestätigung
- startet das System anschließend direkt in das BIOS-/UEFI-Setup

Die eigentliche Firmwareoberfläche wird ausschließlich vom Plattformhersteller bereitgestellt. Moderne Betriebssysteme und Firmware unterstützen standardisierte Übergänge in das UEFI-Setup. :contentReference[oaicite:1]{index=1}

---

# 11. Plattforminformationen

Optional können dargestellt werden:

- CPU
- Chipsatz
- Firmwarearchitektur
- ACPI-Version
- SMBIOS-Version
- UEFI-Version
- unterstützte Bootmodi

---

# 12. Aktionen

Standardaktionen:

- Firmware Setup öffnen
- Aktualisieren
- Plattformdetails
- Zurück

Optional:

- Firmwarebericht exportieren
- Diagnose starten

---

# 13. Navigation

Unterstützt werden:

- Tastatur
- Maus
- Touch

Die Navigation erfolgt gemäß Boot Input Architecture.

---

# 14. Fokus

Der Fokus muss:

- jederzeit sichtbar sein
- weich animiert werden
- niemals verloren gehen

---

# 15. Animationen

Unterstützt werden:

- Seitenübergänge
- Fokusanimationen
- Dialoganimationen
- Statusanimationen

Alle Animationen verwenden Motion Tokens.

---

# 16. Themeintegration

Unterstützt werden:

- Dark Theme
- Light Theme
- High Contrast Theme

Alle Farben stammen ausschließlich aus Design Tokens.

---

# 17. Accessibility

Die Firmware Settings Interface unterstützt:

- Screenreader
- Tastatursteuerung
- hohe Kontraste
- große Schriftgrößen
- reduzierte Animationen

Alle Firmwareinformationen besitzen textliche Beschreibungen.

---

# 18. Fehlerfälle

Zu behandeln sind:

- Firmwareinformationen nicht verfügbar
- UEFI-Dienste nicht verfügbar
- BIOS-System ohne direkten Firmware-Neustart
- TPM nicht erkannt
- Secure Boot nicht unterstützt

Nicht verfügbare Funktionen werden deaktiviert und erläutert.

---

# 19. Sicherheit

Die Oberfläche verhindert:

- unbeabsichtigte Firmware-Neustarts
- unbestätigte Firmwarewechsel
- direkte Änderungen an Firmwareparametern
- unautorisierte Firmwareoperationen

Die NovaOS-Bootoberfläche nimmt selbst keine Änderungen an Firmwareeinstellungen vor.

---

# 20. Speicherverwaltung

Die Oberfläche verwendet:

- Resource Cache
- Theme Cache
- Dialog Pool
- Information Pool

Heap-Allokationen während der Darstellung sind verboten.

---

# 21. Performance

Anforderungen:

- Initialisierung < 75 ms
- O(1)-Information Lookup
- O(1)-Theme Lookup
- deterministische Renderzeit

Firmwareinformationen werden beim Öffnen vollständig geladen.

---

# 22. API

```c
typedef struct
{
    bool uefi;
    bool secure_boot;
    bool tpm_present;
    bool firmware_setup_supported;
}
nova_firmware_status_t;

nova_result_t
nova_firmware_ui_open(void);

nova_result_t
nova_firmware_enter_setup(void);

const nova_firmware_status_t*
nova_firmware_status(void);

nova_result_t
nova_firmware_refresh(void);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Firmwaretyp
- Firmwareversion
- Secure-Boot-Status
- TPM-Status
- Plattforminformationen
- Benutzeraktionen
- Ladezeit

---

# 24. Testfälle

Zu testen sind:

- BIOS-System
- UEFI-System
- Secure Boot aktiviert
- Secure Boot deaktiviert
- TPM vorhanden
- TPM nicht vorhanden
- Themewechsel
- Accessibility
- Firmware-Neustart
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Firmwareinformationen vollständig dargestellt werden.
- der Neustart in das Firmware-Setup unterstützt wird, sofern die Plattform dies ermöglicht.
- BIOS- und UEFI-Systeme korrekt erkannt werden.
- Secure-Boot- und TPM-Status eindeutig dargestellt werden.
- Dark-, Light- und High-Contrast-Themes unterstützt werden.
- keine Firmwareparameter direkt verändert werden.
- keine Heap-Allokationen während der Darstellung stattfinden.
- sämtliche Animationen ausschließlich Motion Tokens verwenden.

---

# 26. Referenzen

## ADRs

- ADR-BOOTMANAGER-UI-0011 – Firmware Settings Interface
- ADR-BOOTFIRMWARE-0001 – Firmware Integration
- ADR-BOOTDESIGN-0001 – Boot Design Language

## NPSPECs

- NPSPEC-BOOTMANAGER-UI-0001 – Main Boot Selection Screen
- NPSPEC-BOOTCONTROL-LIST-0001 – List
- NPSPEC-BOOTCONTROL-TILE-0001 – Boot Option Tile
- NPSPEC-BOOTCONTROL-DIALOG-0001 – Dialog
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTMOTION-0001 – Motion Engine
- NPSPEC-BOOTFIRMWARE-0001 – Firmware Integration

---

# 27. Zusammenfassung

Die **Firmware Settings Interface** stellt die zentrale Verwaltungsoberfläche für Firmwareinformationen innerhalb des NovaOS Boot Managers bereit. Sie ermöglicht die Anzeige von BIOS-/UEFI-, Secure-Boot- und TPM-Informationen sowie – sofern von der Plattform unterstützt – den sicheren Neustart in das Firmware-Setup. Durch vollständige Integration in die NovaOS Design Language, das Theme-System und die Motion Engine entsteht eine konsistente, sichere und benutzerfreundliche Schnittstelle zwischen NovaOS und der zugrunde liegenden Plattformfirmware.