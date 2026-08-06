# NPSPEC-BOOTMANAGER-UI-0013
# Boot Failure Screen

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTMANAGER-UI-0013 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Manager UI |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager / Recovery / Self-Healing |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTMANAGER-UI-0001, NPSPEC-BOOTCONTROL-DIALOG-0001, NPSPEC-BOOTCONTROL-BUTTON-0001, NPSPEC-BOOTCONTROL-LABEL-0001, NPSPEC-BOOTCONTROL-STATUSBADGE-0001, NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTERROR-0001 |

---

# 1. Ziel

Diese Spezifikation definiert den **Boot Failure Screen** des NovaOS Boot Managers.

Der Boot Failure Screen stellt eine standardisierte Benutzeroberfläche für sämtliche Fehler bereit, die während des Bootvorgangs auftreten können. Er informiert den Benutzer verständlich über den Fehlerzustand, bietet geeignete Wiederherstellungsoptionen an und verhindert, dass der Benutzer in einer unklaren Situation zurückgelassen wird.

Die Oberfläche muss sowohl für Endanwender als auch für Administratoren verständlich sein.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Bootloaderfehler
- Kernelfehler
- Dateisystemfehler
- Snapshotfehler
- Recoveryfehler
- Netzwerk-Bootfehler
- Integritätsfehler
- Signaturfehler
- Firmwarefehler
- zukünftige Bootfehler

---

# 3. Ziele

Der Boot Failure Screen muss:

- Fehler eindeutig darstellen
- mögliche Ursachen erläutern
- Wiederherstellungsoptionen anbieten
- Diagnose erleichtern
- vollständig per Tastatur bedienbar sein
- deterministisch arbeiten

---

# 4. Bildschirmaufbau

```text
+---------------------------------------------------------------+

NovaOS Logo

---------------------------------------------------------------

Fehlersymbol

Boot Failure

---------------------------------------------------------------

Kurzbeschreibung

---------------------------------------------------------------

Fehlerdetails

---------------------------------------------------------------

Empfohlene Maßnahmen

---------------------------------------------------------------

[ Erneut versuchen ]

[ Recovery ]

[ Self-Healing ]

[ Diagnose ]

[ Snapshot wählen ]

[ Bootmenü ]

[ Herunterfahren ]

+---------------------------------------------------------------+
```

---

# 5. Komponenten

Der Bildschirm besteht aus:

- Header
- Fehlersymbol
- Kurzbeschreibung
- Detailbereich
- Handlungsempfehlungen
- Aktionsbereich

Alle Komponenten verwenden Standard-Boot Controls.

---

# 6. Header

Der Header enthält:

- NovaOS Logo
- Titel
- Fehlerstatus

Der Header bleibt dauerhaft sichtbar.

---

# 7. Fehlersymbol

Für jeden Fehlertyp existiert ein eigenes Symbol.

Beispiele:

- Bootloader
- Kernel
- Datenträger
- Netzwerk
- Snapshot
- Recovery
- Signatur
- Integrität

Die Darstellung erfolgt über semantische Icon Tokens.

---

# 8. Kurzbeschreibung

Die Kurzbeschreibung besteht aus:

- Fehlername
- Fehlerklasse
- kurzer Erklärung

Beispiel:

```text
Kernel konnte nicht geladen werden.
```

Texte müssen lokalisierbar sein.

---

# 9. Fehlerdetails

Optional können angezeigt werden:

- Fehlercode
- betroffenes Modul
- Bootphase
- Datenträger
- Partition
- Kernelversion
- Firmwaremodus
- Zeitstempel

Debug-Informationen werden standardmäßig ausgeblendet.

---

# 10. Handlungsempfehlungen

Für jeden Fehler werden passende Empfehlungen angezeigt.

Beispiele:

- Recovery starten
- Snapshot wiederherstellen
- Datenträger prüfen
- Netzwerk prüfen
- Bootloader reparieren
- Kernel erneut laden

Empfehlungen richten sich nach dem erkannten Fehler.

---

# 11. Aktionen

Standardaktionen:

- Erneut versuchen
- Recovery
- Self-Healing
- Diagnose
- Snapshot auswählen
- Bootmenü
- Herunterfahren

Optional:

- Erweiterte Optionen
- Firmware Setup
- Netzwerkdiagnose
- Fehlerbericht exportieren

---

# 12. Navigation

Unterstützt werden:

- Tastatur
- Maus
- Touch

Die Navigation erfolgt gemäß Boot Input Architecture.

---

# 13. Fokus

Der Fokus muss:

- jederzeit sichtbar sein
- weich animiert werden
- standardmäßig auf der empfohlenen Aktion liegen

---

# 14. Animationen

Unterstützt werden:

- Fade-In
- Dialoganimation
- Fokusanimation
- Statusanimation
- Seitenwechsel

Alle Animationen verwenden Motion Tokens.

---

# 15. Themeintegration

Unterstützt werden:

- Dark Theme
- Light Theme
- High Contrast Theme

Alle Farben stammen aus Design Tokens.

---

# 16. Accessibility

Der Boot Failure Screen unterstützt:

- Screenreader
- Tastatursteuerung
- hohe Kontraste
- große Schriftgrößen
- reduzierte Animationen

Fehler werden zusätzlich textlich beschrieben.

---

# 17. Fehlerklassen

Mindestens folgende Klassen werden unterstützt:

- Hardware
- Firmware
- Bootloader
- Kernel
- Dateisystem
- Sicherheit
- Netzwerk
- Recovery
- Snapshot
- Unbekannt

Jede Fehlerklasse besitzt eigene Standardmaßnahmen.

---

# 18. Sicherheit

Der Bildschirm darf:

- keine Speicheradressen
- keine kryptographischen Schlüssel
- keine geheimen Konfigurationsdaten
- keine internen Debugdaten

anzeigen.

Erweiterte Diagnosedaten sind ausschließlich im Diagnosemodus verfügbar.

Empfohlene Wiederherstellungsoptionen dürfen den Benutzer nicht automatisch zu unsicheren Aktionen verleiten. UEFI-Plattformen können bei schwerwiegenden Fehlern definierte Wiederherstellungsoptionen bereitstellen, wobei die eigentliche Recovery-Strategie von der Plattform abhängt. :contentReference[oaicite:0]{index=0}

---

# 19. Speicherverwaltung

Die Oberfläche verwendet:

- Dialog Pool
- Icon Cache
- Resource Cache
- Theme Cache

Heap-Allokationen während der Darstellung sind verboten.

---

# 20. Performance

Anforderungen:

- Anzeige < 50 ms nach Fehlererkennung
- O(1)-Fehler Lookup
- O(1)-Theme Lookup
- O(1)-Dialog Lookup
- deterministische Renderzeit

---

# 21. API

```c
typedef enum
{
    NOVA_BOOT_FAILURE_BOOTLOADER,
    NOVA_BOOT_FAILURE_KERNEL,
    NOVA_BOOT_FAILURE_FILESYSTEM,
    NOVA_BOOT_FAILURE_RECOVERY,
    NOVA_BOOT_FAILURE_SNAPSHOT,
    NOVA_BOOT_FAILURE_NETWORK,
    NOVA_BOOT_FAILURE_SECURITY,
    NOVA_BOOT_FAILURE_UNKNOWN
}
nova_boot_failure_type_t;

typedef struct
{
    nova_boot_failure_type_t type;
    uint32_t error_code;
    bool recovery_available;
    bool self_healing_available;
}
nova_boot_failure_t;

nova_result_t
nova_boot_failure_open(
    const nova_boot_failure_t* failure);

nova_result_t
nova_boot_failure_retry(void);

nova_result_t
nova_boot_failure_close(void);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- Fehlerklasse
- Fehlercode
- Bootphase
- Modul
- Benutzeraktion
- Ladezeit
- Theme

---

# 23. Testfälle

Zu testen sind:

- Kernelfehler
- Bootloaderfehler
- Snapshotfehler
- Recoveryfehler
- Netzwerkfehler
- Integritätsfehler
- Themewechsel
- Accessibility
- Tastatursteuerung
- Fehlerdialoge

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Bootfehler eindeutig dargestellt werden.
- verständliche Handlungsempfehlungen angezeigt werden.
- Recovery-, Diagnose- und Self-Healing-Funktionen direkt erreichbar sind.
- Dark-, Light- und High-Contrast-Themes unterstützt werden.
- sicherheitskritische Informationen nicht angezeigt werden.
- sämtliche Texte lokalisierbar sind.
- keine Heap-Allokationen während der Darstellung stattfinden.
- sämtliche Animationen ausschließlich Motion Tokens verwenden.

---

# 25. Referenzen

## ADRs

- ADR-BOOTMANAGER-UI-0013 – Boot Failure Screen
- ADR-BOOTERROR-0001 – Boot Error Framework
- ADR-BOOTDESIGN-0001 – Boot Design Language

## NPSPECs

- NPSPEC-BOOTMANAGER-UI-0001 – Main Boot Selection Screen
- NPSPEC-BOOTCONTROL-DIALOG-0001 – Dialog
- NPSPEC-BOOTCONTROL-BUTTON-0001 – Button
- NPSPEC-BOOTCONTROL-LABEL-0001 – Label
- NPSPEC-BOOTCONTROL-STATUSBADGE-0001 – Status Badge
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTMOTION-0001 – Motion Engine
- NPSPEC-BOOTERROR-0001 – Boot Error Framework

---

# 26. Zusammenfassung

Der **Boot Failure Screen** bildet die zentrale Fehleroberfläche des NovaOS Boot Managers. Er informiert den Benutzer verständlich über Bootprobleme, klassifiziert Fehler nach Typ, zeigt kontextabhängige Handlungsempfehlungen an und ermöglicht den direkten Wechsel zu Recovery-, Self-Healing- und Diagnosefunktionen. Durch die vollständige Integration in die NovaOS Design Language, das Theme-System und die Motion Engine entsteht eine konsistente, sichere und benutzerfreundliche Fehlerbehandlung für sämtliche Bootphasen.