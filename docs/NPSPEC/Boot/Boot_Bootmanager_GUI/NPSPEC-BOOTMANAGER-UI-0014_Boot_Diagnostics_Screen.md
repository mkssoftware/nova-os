# NPSPEC-BOOTMANAGER-UI-0014
# Boot Diagnostics Screen

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTMANAGER-UI-0014 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Manager UI |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager / Diagnostics / Recovery / Self-Healing |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTMANAGER-UI-0001, NPSPEC-BOOTCONTROL-TABVIEW-0001, NPSPEC-BOOTCONTROL-LIST-0001, NPSPEC-BOOTCONTROL-TREEVIEW-0001, NPSPEC-BOOTCONTROL-PROGRESS-0001, NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTDIAG-0001 |

---

# 1. Ziel

Diese Spezifikation definiert den **Boot Diagnostics Screen** des NovaOS Boot Managers.

Der Boot Diagnostics Screen stellt sämtliche Diagnoseinformationen der Pre-Boot-Umgebung zentral dar. Er dient sowohl der Fehleranalyse als auch der Überprüfung des aktuellen Systemzustands und bietet eine einheitliche Oberfläche für Endanwender, Administratoren und Entwickler.

Diagnoseinformationen werden ausschließlich lesend dargestellt. Änderungen am System erfolgen nur über dedizierte Diagnose- oder Recoveryfunktionen.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Boot Manager
- Bootloader
- Firmware
- Kernel Loader
- Hardwareinitialisierung
- Speicherdiagnose
- Datenträgerdiagnose
- Netzwerkdiagnose
- Recovery
- Self-Healing

---

# 3. Ziele

Der Boot Diagnostics Screen muss:

- Diagnoseinformationen strukturiert darstellen
- Fehler eindeutig visualisieren
- Systemzustände nachvollziehbar machen
- vollständig per Tastatur bedienbar sein
- deterministisch arbeiten
- modular erweiterbar sein

---

# 4. Bildschirmaufbau

```text
+--------------------------------------------------------------+

Zurück

Boot Diagnostics

---------------------------------------------------------------

Tabs

Übersicht | Hardware | Boot | Speicher | Datenträger |

Netzwerk | Sicherheit | Ereignisse

---------------------------------------------------------------

Diagnosebereich

---------------------------------------------------------------

Statusleiste

---------------------------------------------------------------

[ Aktualisieren ]

[ Bericht exportieren ]

[ Recovery ]

[ Schließen ]

+--------------------------------------------------------------+
```

---

# 5. Komponenten

Die Oberfläche besteht aus:

- Header
- Tableiste
- Diagnosebereich
- Statusleiste
- Aktionsbereich

Alle Komponenten verwenden Standard-Boot Controls.

---

# 6. Header

Der Header enthält:

- Zurücknavigation
- Diagnose-Icon
- Titel
- Diagnosemodus
- Zeitstempel der letzten Aktualisierung

---

# 7. Diagnose-Tabs

Mindestens folgende Registerkarten werden unterstützt:

- Übersicht
- Hardware
- Boot
- Speicher
- Datenträger
- Netzwerk
- Sicherheit
- Ereignisse

Weitere Diagnosemodule können eigene Tabs registrieren.

---

# 8. Übersicht

Die Übersichtsseite zeigt:

- Gesamtsystemstatus
- Firmwarestatus
- Bootstatus
- Hardwarestatus
- Integritätsstatus
- Recoverystatus
- Self-Healing-Status

Der Gesamtstatus wird zusätzlich als Status Badge dargestellt.

---

# 9. Hardware

Folgende Informationen werden dargestellt:

- CPU
- Kerne
- Arbeitsspeicher
- Mainboard
- Firmware
- TPM
- Grafikausgabe
- Eingabegeräte

---

# 10. Boot

Der Bootbereich zeigt:

- Firmwaremodus
- Bootloader
- Bootreihenfolge
- Bootquelle
- aktueller Bootpfad
- geladene Module
- Bootdauer
- Bootphase

---

# 11. Speicher

Der Speicherbereich zeigt:

- Gesamtspeicher
- erkannter Speicher
- ECC-Status
- letzter Speichertest
- Speicherfehler
- verfügbare Tests

---

# 12. Datenträger

Der Datenträgerbereich zeigt:

- erkannte Laufwerke
- Partitionen
- SMART-Status
- Dateisysteme
- Integritätsstatus
- Bootpartition

---

# 13. Netzwerk

Falls verfügbar werden dargestellt:

- Netzwerkadapter
- MAC-Adresse
- IPv4
- IPv6
- DHCP
- Linkstatus
- Netzwerk-Bootstatus

---

# 14. Sicherheit

Der Sicherheitsbereich zeigt:

- Secure Boot
- TPM
- Signaturstatus
- Measured Boot
- Integritätsprüfung
- Verschlüsselungsstatus

---

# 15. Ereignisprotokoll

Das Ereignisprotokoll enthält:

- Zeitstempel
- Schweregrad
- Quelle
- Modul
- Ereignis-ID
- Beschreibung

Schweregrade:

- Information
- Hinweis
- Warnung
- Fehler
- Kritisch

---

# 16. Statusleiste

Die Statusleiste zeigt:

- Diagnose läuft
- Diagnose abgeschlossen
- Anzahl Warnungen
- Anzahl Fehler
- Laufzeit
- letzter Scan

---

# 17. Aktionen

Standardaktionen:

- Aktualisieren
- Bericht exportieren
- Recovery öffnen
- Self-Healing öffnen
- Schließen

Optional:

- Detailansicht
- Diagnosedaten speichern
- Snapshot erzeugen

---

# 18. Navigation

Unterstützt werden:

- Tastatur
- Maus
- Touch

Tabs können per Tastatur oder Maus gewechselt werden.

---

# 19. Fokus

Der Fokus muss:

- jederzeit sichtbar sein
- weich animiert werden
- zwischen Tabs und Diagnosebereich wechseln können

---

# 20. Animationen

Unterstützt werden:

- Tabwechsel
- Fokusanimationen
- Listenanimationen
- Fortschrittsanimationen
- Dialoganimationen

Alle Animationen verwenden Motion Tokens.

---

# 21. Themeintegration

Unterstützt werden:

- Dark Theme
- Light Theme
- High Contrast Theme

Alle Farben stammen ausschließlich aus Design Tokens.

---

# 22. Accessibility

Der Boot Diagnostics Screen unterstützt:

- Screenreader
- Tastatursteuerung
- hohe Kontraste
- große Schriftgrößen
- reduzierte Animationen

Alle Diagramme und Statusanzeigen besitzen textuelle Alternativen.

---

# 23. Fehlerfälle

Zu behandeln sind:

- Diagnosemodul nicht verfügbar
- Hardwareinformationen unvollständig
- Firmwareinformationen nicht lesbar
- Datenträger nicht erreichbar
- Netzwerkdiagnose fehlgeschlagen
- Diagnose abgebrochen

Fehler werden eindeutig dargestellt.

---

# 24. Sicherheit

Der Bildschirm darf keine sensiblen Informationen anzeigen, insbesondere:

- Passwörter
- kryptographische Schlüssel
- geheime Token
- Speicherinhalte
- private Zertifikate

Debugdaten werden ausschließlich im Entwickler- bzw. Expertenmodus angezeigt.

---

# 25. Speicherverwaltung

Die Oberfläche verwendet:

- Tab Pool
- List Pool
- Event Pool
- Resource Cache
- Theme Cache

Heap-Allokationen während der Darstellung sind verboten.

---

# 26. Performance

Anforderungen:

- Initialisierung < 100 ms
- O(1)-Tab Lookup
- O(1)-Status Lookup
- O(1)-Theme Lookup
- deterministische Aktualisierung

Diagnoseinformationen dürfen asynchron aktualisiert werden, ohne die Benutzeroberfläche zu blockieren.

---

# 27. API

```c
typedef enum
{
    NOVA_DIAG_TAB_OVERVIEW,
    NOVA_DIAG_TAB_HARDWARE,
    NOVA_DIAG_TAB_BOOT,
    NOVA_DIAG_TAB_MEMORY,
    NOVA_DIAG_TAB_STORAGE,
    NOVA_DIAG_TAB_NETWORK,
    NOVA_DIAG_TAB_SECURITY,
    NOVA_DIAG_TAB_EVENTS
}
nova_diag_tab_t;

typedef struct
{
    uint32_t warnings;
    uint32_t errors;
    bool diagnostics_running;
}
nova_boot_diagnostics_status_t;

nova_result_t
nova_boot_diagnostics_open(void);

nova_result_t
nova_boot_diagnostics_refresh(void);

nova_result_t
nova_boot_diagnostics_export(void);

const nova_boot_diagnostics_status_t*
nova_boot_diagnostics_status(void);

nova_result_t
nova_boot_diagnostics_close(void);
```

---

# 28. Diagnosedaten

Zu erfassen sind:

- Bootdauer
- Firmwareinformationen
- Hardwarestatus
- Speicherstatus
- Datenträgerstatus
- Netzwerkstatus
- Sicherheitsstatus
- Ereignisanzahl
- Warnungen
- Fehler
- Benutzeraktionen

---

# 29. Testfälle

Zu testen sind:

- vollständige Hardwarediagnose
- Bootdiagnose
- Speichertest
- Datenträgerdiagnose
- Netzwerkdiagnose
- Themewechsel
- Accessibility
- Ereignisprotokoll
- Exportfunktion
- Fehlerfälle

---

# 30. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Diagnoseinformationen strukturiert dargestellt werden.
- alle Diagnosebereiche über Registerkarten erreichbar sind.
- Hardware-, Boot-, Speicher-, Datenträger-, Netzwerk- und Sicherheitsinformationen vollständig angezeigt werden.
- das Ereignisprotokoll chronologisch dargestellt wird.
- Dark-, Light- und High-Contrast-Themes unterstützt werden.
- Diagnoseberichte exportiert werden können.
- keine Heap-Allokationen während der Darstellung stattfinden.
- sämtliche Animationen ausschließlich Motion Tokens verwenden.

---

# 31. Referenzen

## ADRs

- ADR-BOOTMANAGER-UI-0014 – Boot Diagnostics Screen
- ADR-BOOTDIAG-0001 – Boot Diagnostics Framework
- ADR-BOOTDESIGN-0001 – Boot Design Language

## NPSPECs

- NPSPEC-BOOTMANAGER-UI-0001 – Main Boot Selection Screen
- NPSPEC-BOOTCONTROL-TABVIEW-0001 – Tab View
- NPSPEC-BOOTCONTROL-LIST-0001 – List
- NPSPEC-BOOTCONTROL-TREEVIEW-0001 – Tree View
- NPSPEC-BOOTCONTROL-PROGRESS-0001 – Progress Indicator
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTMOTION-0001 – Motion Engine
- NPSPEC-BOOTDIAG-0001 – Boot Diagnostics Framework

---

# 32. Zusammenfassung

Der **Boot Diagnostics Screen** bildet die zentrale Diagnoseoberfläche des NovaOS Boot Managers. Er vereint Hardware-, Firmware-, Boot-, Speicher-, Datenträger-, Netzwerk- und Sicherheitsinformationen in einer modularen Registerkartenansicht und ermöglicht eine strukturierte Analyse des gesamten Pre-Boot-Zustands. Durch die vollständige Integration in die NovaOS Design Language, das Theme-System und die Motion Engine entsteht eine leistungsfähige, konsistente und benutzerfreundliche Diagnoseoberfläche für Wartung, Fehleranalyse und Systemüberwachung. Moderne Bootumgebungen stellen ebenfalls strukturierte Diagnoseinformationen für Bootstatus und Hardware bereit, was diesen modularen Aufbau unterstützt. :contentReference[oaicite:0]{index=0}