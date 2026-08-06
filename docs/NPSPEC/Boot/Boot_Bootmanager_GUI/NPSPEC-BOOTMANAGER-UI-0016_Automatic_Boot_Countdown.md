# NPSPEC-BOOTMANAGER-UI-0016
# Automatic Boot Countdown

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTMANAGER-UI-0016 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Manager UI |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTMANAGER-UI-0001, NPSPEC-BOOTCONTROL-PROGRESS-0001, NPSPEC-BOOTCONTROL-LABEL-0001, NPSPEC-BOOTCONTROL-BUTTON-0001, NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTCONFIG-0001 |

---

# 1. Ziel

Diese Spezifikation definiert den **Automatic Boot Countdown** des NovaOS Boot Managers.

Der Countdown steuert den automatischen Start des Standard-Boot-Eintrags nach Ablauf einer konfigurierbaren Wartezeit. Während des Countdowns kann der Benutzer jederzeit eingreifen, den Countdown anhalten oder einen anderen Boot-Eintrag auswählen.

Die Funktion soll einen schnellen Systemstart ermöglichen, ohne den Zugriff auf den Boot Manager einzuschränken.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Standard-Boot
- Mehrfach-Boot-Systeme
- Recovery-Menü
- Netzwerk-Boot
- Snapshot-Boot
- Self-Healing
- zukünftige Bootmodule

---

# 3. Ziele

Der Countdown muss:

- automatisch den Standard-Boot starten
- jederzeit unterbrechbar sein
- den verbleibenden Zeitraum eindeutig anzeigen
- vollständig per Tastatur bedienbar sein
- deterministisch arbeiten
- konfigurierbar sein

---

# 4. Bildschirmaufbau

```text
--------------------------------------------------------

Automatischer Start in

██████████████████████░░░░░░░░░░░░

5 Sekunden

Standard:
NovaOS Stable

--------------------------------------------------------

[ Jetzt starten ]

[ Countdown pausieren ]

--------------------------------------------------------
```

---

# 5. Komponenten

Der Countdown besteht aus:

- Titel
- Fortschrittsanzeige
- Restzeit
- Standard-Boot-Eintrag
- Aktionsbereich

Alle Komponenten verwenden Standard-Boot Controls.

---

# 6. Darstellung

Folgende Informationen werden angezeigt:

- verbleibende Sekunden
- Fortschrittsbalken
- Standardbetriebssystem
- Bootsymbol
- Bootmodus

Optional:

- geschätzte Bootdauer
- letzter erfolgreicher Start

---

# 7. Verhalten

Der Countdown startet automatisch beim Anzeigen des Boot Managers.

Standardwert:

```text
5 Sekunden
```

Der Wert ist konfigurierbar.

---

# 8. Countdownsteuerung

Unterstützt werden:

- Start
- Pause
- Fortsetzen
- Sofort starten
- Zurücksetzen

---

# 9. Benutzerinteraktion

Der Countdown wird automatisch pausiert bei:

- Mausbewegung
- Tastendruck
- Touch-Eingabe
- Fokuswechsel
- Auswahl eines anderen Boot-Eintrags
- Öffnen eines Dialogs

Nach Abschluss der Benutzerinteraktion kann der Countdown – abhängig von der Konfiguration – automatisch fortgesetzt oder dauerhaft gestoppt werden.

---

# 10. Startbedingungen

Der automatische Start erfolgt nur wenn:

- kein Dialog geöffnet ist
- keine Recovery aktiv ist
- kein Fehler vorliegt
- keine Benutzereingabe erfolgt
- der Standard-Boot-Eintrag gültig ist

---

# 11. Abbruchbedingungen

Der Countdown wird beendet bei:

- Benutzerinteraktion
- Fehler
- Recovery
- Diagnose
- Änderung des Standard-Boot-Eintrags
- deaktivierter Countdownfunktion

---

# 12. Fortschrittsanzeige

Der Fortschritt zeigt:

- Restzeit
- Prozent
- Startzeitpunkt

Animationen erfolgen kontinuierlich.

---

# 13. Navigation

Während des Countdowns bleiben sämtliche Bedienelemente erreichbar.

Unterstützt werden:

- Tastatur
- Maus
- Touch
- Gamepad (optional)

---

# 14. Fokus

Der Countdown besitzt standardmäßig keinen Fokus.

Erst bei Benutzerinteraktion wird der Fokus gemäß Focus Management vergeben.

---

# 15. Animationen

Unterstützt werden:

- Fortschrittsanimation
- Zahlenübergänge
- Fade-In
- Fade-Out
- Buttonanimationen

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

Der Countdown unterstützt:

- Screenreader
- hohe Kontraste
- große Schriftgrößen
- reduzierte Animationen

Die verbleibende Zeit wird zusätzlich textlich dargestellt.

---

# 18. Fehlerfälle

Zu behandeln sind:

- ungültiger Standard-Boot-Eintrag
- Countdown-Konfiguration beschädigt
- Bootziel nicht erreichbar
- Bootfehler während Countdown
- Recovery erforderlich

Der Countdown wird in diesen Fällen automatisch gestoppt.

---

# 19. Sicherheit

Der automatische Boot darf nur erfolgen wenn:

- Integritätsprüfung erfolgreich
- Bootziel gültig
- Bootkonfiguration vollständig
- keine kritischen Fehler erkannt wurden

Unsichere oder beschädigte Boot-Einträge dürfen niemals automatisch gestartet werden.

---

# 20. Speicherverwaltung

Die Oberfläche verwendet:

- Countdown Pool
- Progress Pool
- Resource Cache
- Theme Cache

Heap-Allokationen während der Darstellung sind verboten.

---

# 21. Performance

Anforderungen:

- Initialisierung < 20 ms
- O(1)-Countdown Update
- O(1)-Theme Lookup
- deterministische Aktualisierung

Die Aktualisierung erfolgt über den zentralen UI-Timer.

---

# 22. API

```c
typedef struct
{
    bool enabled;
    bool paused;
    uint32_t timeout_seconds;
    uint32_t remaining_seconds;
    uint32_t default_entry;
}
nova_boot_countdown_t;

nova_result_t
nova_boot_countdown_start(void);

nova_result_t
nova_boot_countdown_pause(void);

nova_result_t
nova_boot_countdown_resume(void);

nova_result_t
nova_boot_countdown_cancel(void);

const nova_boot_countdown_t*
nova_boot_countdown_status(void);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Countdown aktiviert
- Startzeit
- Restzeit
- Standard-Boot-Eintrag
- Benutzerunterbrechungen
- automatischer Start
- Theme

---

# 24. Testfälle

Zu testen sind:

- automatischer Start
- Countdown-Pause
- Countdown-Fortsetzung
- Benutzerunterbrechung
- Bootfehler
- Themewechsel
- Accessibility
- verschiedene Timeoutwerte
- deaktivierter Countdown
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- der Countdown automatisch startet.
- der Standard-Boot-Eintrag nach Ablauf gestartet wird.
- jede Benutzereingabe den Countdown korrekt beeinflusst.
- Dark-, Light- und High-Contrast-Themes unterstützt werden.
- der Fortschritt kontinuierlich dargestellt wird.
- ungültige Bootziele nicht automatisch gestartet werden.
- keine Heap-Allokationen während der Darstellung stattfinden.
- sämtliche Animationen ausschließlich Motion Tokens verwenden.

---

# 26. Referenzen

## ADRs

- ADR-BOOTMANAGER-UI-0016 – Automatic Boot Countdown
- ADR-BOOTCONFIG-0001 – Boot Configuration
- ADR-BOOTDESIGN-0001 – Boot Design Language

## NPSPECs

- NPSPEC-BOOTMANAGER-UI-0001 – Main Boot Selection Screen
- NPSPEC-BOOTCONTROL-PROGRESS-0001 – Progress Indicator
- NPSPEC-BOOTCONTROL-LABEL-0001 – Label
- NPSPEC-BOOTCONTROL-BUTTON-0001 – Button
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTMOTION-0001 – Motion Engine
- NPSPEC-BOOTCONFIG-0001 – Boot Configuration

---

# 27. Zusammenfassung

Der **Automatic Boot Countdown** steuert den automatischen Start des konfigurierten Standard-Boot-Eintrags nach einer frei definierbaren Wartezeit. Die Funktion visualisiert den verbleibenden Zeitraum durch einen Fortschrittsbalken und eine numerische Restzeitanzeige, reagiert unmittelbar auf Benutzereingaben und gewährleistet, dass nur gültige und erfolgreich geprüfte Bootziele automatisch gestartet werden. Durch die vollständige Integration in die NovaOS Design Language, das Theme-System und die Motion Engine entsteht eine konsistente, sichere und benutzerfreundliche Startautomatisierung innerhalb des NovaOS Boot Managers.