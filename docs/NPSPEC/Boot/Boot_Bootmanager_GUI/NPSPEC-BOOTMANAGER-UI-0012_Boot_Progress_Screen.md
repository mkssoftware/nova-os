# NPSPEC-BOOTMANAGER-UI-0012
# Boot Progress Screen

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTMANAGER-UI-0012 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Manager UI |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager / Kernel Loader / Recovery |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTMANAGER-UI-0001, NPSPEC-BOOTCONTROL-PROGRESS-0001, NPSPEC-BOOTCONTROL-SPINNER-0001, NPSPEC-BOOTCONTROL-LABEL-0001, NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTBOOT-0001 |

---

# 1. Ziel

Diese Spezifikation definiert den **Boot Progress Screen** des NovaOS Boot Managers.

Der Boot Progress Screen informiert den Benutzer über den aktuellen Fortschritt des Bootvorgangs zwischen der Auswahl eines Boot-Eintrags und der Übergabe an den Kernel oder die Recovery-Umgebung. Ziel ist es, jederzeit transparent darzustellen, dass das System arbeitet und nicht eingefroren ist. Ein klarer Fortschrittsbildschirm verbessert die Benutzererfahrung insbesondere bei längeren Startvorgängen oder Systemaktualisierungen. :contentReference[oaicite:0]{index=0}

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- normaler Systemstart
- Recovery-Start
- Self-Healing
- Snapshot-Wiederherstellung
- Netzwerk-Boot
- Kernelinitialisierung
- Offline-Updates

---

# 3. Ziele

Der Boot Progress Screen muss:

- aktuellen Bootstatus visualisieren
- Fortschritt verständlich darstellen
- Benutzer über längere Bootzeiten informieren
- deterministisch arbeiten
- vollständig themefähig sein
- nahtlos in die NovaOS Design Language integriert sein

---

# 4. Bildschirmaufbau

```text
+--------------------------------------------------------------+

NovaOS Logo

---------------------------------------------------------------

Boot Animation

---------------------------------------------------------------

Aktueller Schritt

---------------------------------------------------------------

██████████████████████░░░░░░░░░░░░

72 %

---------------------------------------------------------------

Statusbeschreibung

---------------------------------------------------------------

Bootdetails (optional)

---------------------------------------------------------------

Version

Build

Firmware

+--------------------------------------------------------------+
```

---

# 5. Komponenten

Der Bildschirm besteht aus:

- Logo
- Bootanimation
- Fortschrittsanzeige
- Statusbeschreibung
- optionaler Detailbereich
- Informationsleiste

Alle Komponenten verwenden Standard-Boot Controls.

---

# 6. Logo

Das NovaOS-Logo bleibt während des gesamten Bootvorgangs sichtbar.

Das Logo darf:

- weich eingeblendet werden
- leicht animiert sein
- nicht vom Fortschrittsbereich ablenken

---

# 7. Bootanimation

Die Bootanimation unterstützt:

- Spinner
- NovaOS-Systempuls
- dezente Lichtanimation
- Statuswechsel

Animationen dürfen niemals den eigentlichen Fortschritt ersetzen.

---

# 8. Fortschrittsanzeige

Der Fortschrittsbereich zeigt:

- Prozentwert
- Fortschrittsbalken
- aktuellen Bootschritt
- Gesamtstatus

Ist kein exakter Fortschritt verfügbar, kann ein unbestimmter Fortschrittsmodus verwendet werden.

---

# 9. Bootphasen

Mindestens folgende Bootphasen werden unterstützt:

- Firmwareinitialisierung
- Hardwareerkennung
- Bootloader
- Kernel laden
- Treiber initialisieren
- Dateisystem vorbereiten
- Dienste starten
- Übergabe an Betriebssystem

Die UEFI-Bootphase umfasst unter anderem das Laden von Bootoptionen und die Übergabe an den Betriebssystem-Bootloader. :contentReference[oaicite:1]{index=1}

---

# 10. Statusbeschreibung

Die Statusanzeige beschreibt den aktuellen Vorgang.

Beispiele:

- Initialisiere Hardware
- Lade Kernel
- Überprüfe Dateisystem
- Starte Dienste
- Initialisiere Grafik
- Bereite Desktop vor

Texte müssen lokalisiert werden können.

---

# 11. Detailbereich

Optional können dargestellt werden:

- aktuelles Modul
- geladener Treiber
- Bootmodus
- Kernelversion
- verbleibende Schritte
- geschätzte Restzeit

Der Detailbereich kann vom Benutzer ein- oder ausgeblendet werden.

---

# 12. Informationsleiste

Die Informationsleiste kann anzeigen:

- NovaOS Version
- Buildnummer
- Firmwaremodus
- Architektur
- Startquelle

---

# 13. Navigation

Während des normalen Bootvorgangs besitzt der Bildschirm keine direkte Navigation.

Optional können unterstützt werden:

- ESC → Bootmenü
- F8 → Erweiterte Optionen
- F12 → Diagnose

Die Verfügbarkeit ist konfigurierbar.

---

# 14. Fokus

Der Bildschirm besitzt standardmäßig keinen interaktiven Fokus.

Bei eingeblendeten Dialogen wird der Fokus gemäß Focus Management vergeben.

---

# 15. Animationen

Unterstützt werden:

- Fade-In
- Progress Animation
- Spinner
- Systempuls
- Übergang zum Login
- Abschlussanimation

Alle Animationen verwenden Motion Tokens.

---

# 16. Themeintegration

Unterstützt werden:

- Dark Theme
- Light Theme
- High Contrast Theme

Farben, Animationen und Materialien stammen ausschließlich aus Design Tokens.

---

# 17. Accessibility

Der Boot Progress Screen unterstützt:

- Screenreader
- hohe Kontraste
- große Schriftgrößen
- reduzierte Animationen

Fortschritt muss zusätzlich textlich dargestellt werden.

---

# 18. Fehlerfälle

Zu behandeln sind:

- Bootfehler
- Kernel konnte nicht geladen werden
- Treiberfehler
- Recovery erforderlich
- Integritätsprüfung fehlgeschlagen
- Boot Timeout

Fehler werden in einen Fehlerdialog überführt.

---

# 19. Sicherheit

Während des Bootvorgangs dürfen:

- keine vertraulichen Informationen
- keine kryptographischen Schlüssel
- keine Speicheradressen
- keine Debugdaten

auf dem Bildschirm erscheinen.

Diagnosedetails werden ausschließlich im Diagnosemodus angezeigt.

---

# 20. Speicherverwaltung

Die Oberfläche verwendet:

- Progress Pool
- Animation Pool
- Resource Cache
- Theme Cache

Heap-Allokationen während der Darstellung sind verboten.

---

# 21. Performance

Anforderungen:

- Anzeige innerhalb von 50 ms nach Bootstart
- O(1)-Status Lookup
- O(1)-Theme Lookup
- O(1)-Animation Lookup
- deterministische Aktualisierung

Die Benutzeroberfläche darf den Bootvorgang nicht messbar verzögern.

---

# 22. API

```c
typedef enum
{
    NOVA_BOOT_STAGE_FIRMWARE,
    NOVA_BOOT_STAGE_BOOTLOADER,
    NOVA_BOOT_STAGE_KERNEL,
    NOVA_BOOT_STAGE_DRIVERS,
    NOVA_BOOT_STAGE_FILESYSTEM,
    NOVA_BOOT_STAGE_SERVICES,
    NOVA_BOOT_STAGE_HANDOFF
}
nova_boot_stage_t;

typedef struct
{
    nova_boot_stage_t stage;
    uint8_t progress;
    bool indeterminate;
}
nova_boot_progress_t;

nova_result_t
nova_boot_progress_open(void);

nova_result_t
nova_boot_progress_update(
    const nova_boot_progress_t* progress);

nova_result_t
nova_boot_progress_close(void);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- aktuelle Bootphase
- Fortschritt
- Bootdauer
- Theme
- Startquelle
- Kernelversion
- Fehlerstatus

---

# 24. Testfälle

Zu testen sind:

- normaler Boot
- langsamer Boot
- Snapshotstart
- Recovery
- Netzwerk-Boot
- Themewechsel
- Accessibility
- Fortschrittsanzeige
- unbestimmter Fortschritt
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Bootphasen eindeutig dargestellt werden.
- Fortschritt oder Aktivität jederzeit sichtbar ist.
- Statusmeldungen lokalisiert werden können.
- Dark-, Light- und High-Contrast-Themes unterstützt werden.
- Benutzer bei langen Bootvorgängen kontinuierlich informiert werden.
- keine sicherheitskritischen Informationen angezeigt werden.
- keine Heap-Allokationen während der Darstellung stattfinden.
- sämtliche Animationen ausschließlich Motion Tokens verwenden.

---

# 26. Referenzen

## ADRs

- ADR-BOOTMANAGER-UI-0012 – Boot Progress Screen
- ADR-BOOTBOOT-0001 – Boot Process
- ADR-BOOTDESIGN-0001 – Boot Design Language

## NPSPECs

- NPSPEC-BOOTMANAGER-UI-0001 – Main Boot Selection Screen
- NPSPEC-BOOTCONTROL-PROGRESS-0001 – Progress Indicator
- NPSPEC-BOOTCONTROL-SPINNER-0001 – Activity Indicator
- NPSPEC-BOOTCONTROL-LABEL-0001 – Label
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTMOTION-0001 – Motion Engine
- NPSPEC-BOOTBOOT-0001 – Boot Process

---

# 27. Zusammenfassung

Der **Boot Progress Screen** bildet die zentrale Fortschrittsanzeige zwischen Bootauswahl und Übergabe an das Betriebssystem. Er visualisiert den aktuellen Bootstatus durch Fortschrittsbalken, Bootphasen, Statusmeldungen und dezente Animationen, ohne sicherheitskritische Informationen preiszugeben. Durch vollständige Integration in die NovaOS Design Language, Unterstützung verschiedener Themes sowie deterministische Aktualisierung sorgt der Bildschirm für eine konsistente und vertrauenswürdige Benutzererfahrung während des gesamten Startvorgangs. Empfehlungen etablierter Firmware-UX-Leitfäden werden dabei berücksichtigt, indem bei längeren Bootvorgängen jederzeit klar kommuniziert wird, dass das System weiterhin arbeitet. :contentReference[oaicite:2]{index=2}