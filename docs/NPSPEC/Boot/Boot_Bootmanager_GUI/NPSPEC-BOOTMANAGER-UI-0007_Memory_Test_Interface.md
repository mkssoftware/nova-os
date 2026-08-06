# NPSPEC-BOOTMANAGER-UI-0007
# Memory Test Interface

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTMANAGER-UI-0007 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Manager UI |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager / Diagnostics |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTMANAGER-UI-0001, NPSPEC-BOOTCONTROL-PROGRESS-0001, NPSPEC-BOOTCONTROL-LABEL-0001, NPSPEC-BOOTCONTROL-STATUSBADGE-0001, NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTDIAG-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die **Memory Test Interface** des NovaOS Boot Managers.

Die Memory Test Interface stellt sämtliche Funktionen zur Diagnose des Arbeitsspeichers bereit. Sie visualisiert den aktuellen Testfortschritt, erkannte Fehler sowie technische Informationen über den installierten Speicher und ermöglicht sowohl automatische als auch benutzerdefinierte Speichertests.

Die Oberfläche arbeitet vollständig unabhängig vom installierten Betriebssystem.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- physikalischen Arbeitsspeicher
- ECC-Speicher
- UEFI-Systeme
- BIOS-Systeme
- Mehrprozessorsysteme
- zukünftige Speichertestmodule

---

# 3. Ziele

Die Memory Test Interface muss:

- den Teststatus jederzeit darstellen
- Speichertests konfigurieren können
- Fehler eindeutig visualisieren
- vollständig per Tastatur bedienbar sein
- deterministisch arbeiten
- modular erweiterbar sein

---

# 4. Bildschirmaufbau

```text
+--------------------------------------------------------------+

Zurück

Memory Test

---------------------------------------------------------------

Systeminformationen

---------------------------------------------------------------

Testkonfiguration

---------------------------------------------------------------

Fortschritt

██████████████░░░░░░░░░░

---------------------------------------------------------------

Aktueller Test

---------------------------------------------------------------

Erkannte Fehler

---------------------------------------------------------------

[ Test starten ]

[ Test abbrechen ]

[ Bericht speichern ]

[ Zurück ]

+--------------------------------------------------------------+
```

---

# 5. Komponenten

Der Bildschirm besteht aus:

- Header
- Systeminformationen
- Testkonfiguration
- Fortschrittsbereich
- Fehlerbereich
- Aktionsbereich

Alle Komponenten verwenden Standard-Boot Controls.

---

# 6. Header

Der Header enthält:

- Zurücknavigation
- Memory-Test-Icon
- Titel
- Modulversion

Der Header bleibt permanent sichtbar.

---

# 7. Systeminformationen

Folgende Informationen werden angezeigt:

- installierter RAM
- erkannter RAM
- Anzahl Speicherkanäle
- Speichertyp
- Taktfrequenz
- ECC-Unterstützung
- Anzahl Module
- erkannte CPU

Diese Informationen werden vor Testbeginn geladen.

---

# 8. Testkonfiguration

Unterstützte Optionen:

- Schnelltest
- Standardtest
- Vollständiger Test
- Benutzerdefinierter Test
- Endlosschleife
- ECC-Prüfung (falls verfügbar)
- Parallelmodus
- Mehrere Testdurchläufe

Neue Testmodule können ergänzt werden.

---

# 9. Fortschrittsbereich

Während des Tests werden angezeigt:

- Fortschrittsbalken
- aktueller Test
- aktueller Speicherbereich
- getestete Größe
- Gesamtspeicher
- Prozent
- verstrichene Zeit
- geschätzte Restzeit

Die Fortschrittsanzeige verwendet den Progress Indicator.

---

# 10. Aktueller Test

Es wird angezeigt:

- Testalgorithmus
- aktueller Durchlauf
- Blockgröße
- Testmuster
- Speicheradresse
- Prüfschritt

---

# 11. Fehlerbereich

Für jeden Fehler werden dargestellt:

- Fehlernummer
- Speicheradresse
- erwarteter Wert
- gelesener Wert
- Bitfehler
- Zeitpunkt
- ECC-Status (optional)

Mehrere Fehler werden in einer Liste dargestellt.

---

# 12. Aktionen

Standardaktionen:

- Test starten
- Test abbrechen
- Bericht speichern
- Zurück

Optional:

- Test pausieren
- Fehler exportieren
- Test wiederholen

---

# 13. Navigation

Unterstützt werden:

- Tastatur
- Maus
- Touch

Während eines laufenden Tests bleiben alle Statusinformationen erreichbar.

---

# 14. Fokus

Der Fokus muss:

- jederzeit sichtbar sein
- auch während laufender Tests erhalten bleiben
- nicht durch Statusaktualisierungen verloren gehen

---

# 15. Animationen

Unterstützt werden:

- Fortschrittsanimation
- Statuswechsel
- Buttonanimationen
- Dialoganimationen
- Abschlussanimation

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

Die Memory Test Interface unterstützt:

- Screenreader
- Tastatursteuerung
- hohe Kontraste
- große Schriftgrößen
- reduzierte Animationen

Statusänderungen müssen zusätzlich textlich ausgegeben werden.

---

# 18. Fehlerfälle

Zu behandeln sind:

- kein Speicher erkannt
- Testmodul nicht verfügbar
- ECC-Fehler
- Test abgebrochen
- Hardwarefehler
- interner Diagnosefehler

Fehler werden über Dialoge und Status Badges dargestellt.

---

# 19. Sicherheit

Vor Testbeginn werden geprüft:

- erkannter Arbeitsspeicher
- verfügbare Testmodule
- Hardwareinitialisierung
- Speicherzugriff
- Systemintegrität

Ein laufender Test darf keine Systemdaten verändern.

---

# 20. Speicherverwaltung

Die Oberfläche verwendet:

- Progress Pool
- List Pool
- Resource Cache
- Theme Cache

Heap-Allokationen während der Darstellung sind verboten.

---

# 21. Performance

Anforderungen:

- Initialisierung < 100 ms
- O(1)-Status Lookup
- O(1)-Fehler Lookup
- O(1)-Theme Lookup
- deterministische Aktualisierung

Die Aktualisierung der Anzeige erfolgt unabhängig vom Testalgorithmus.

---

# 22. API

```c
typedef enum
{
    NOVA_MEMTEST_QUICK,
    NOVA_MEMTEST_STANDARD,
    NOVA_MEMTEST_FULL,
    NOVA_MEMTEST_CUSTOM,
    NOVA_MEMTEST_LOOP
}
nova_memtest_mode_t;

typedef struct
{
    uint64_t total_memory;
    uint64_t tested_memory;
    uint32_t progress_percent;
    uint32_t error_count;
    bool running;
}
nova_memtest_status_t;

nova_result_t
nova_memtest_open(void);

nova_result_t
nova_memtest_start(
    nova_memtest_mode_t mode);

nova_result_t
nova_memtest_stop(void);

const nova_memtest_status_t*
nova_memtest_status(void);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Speicherkapazität
- Testmodus
- aktueller Durchlauf
- Fehleranzahl
- Laufzeit
- Theme
- Benutzeraktionen

---

# 24. Testfälle

Zu testen sind:

- Schnelltest
- Volltest
- ECC-Test
- mehrere Durchläufe
- Testabbruch
- Fehlererkennung
- Themewechsel
- Accessibility
- Berichtserstellung
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Testinformationen kontinuierlich dargestellt werden.
- Fortschritt und Restzeit korrekt aktualisiert werden.
- Speicherfehler eindeutig identifiziert und visualisiert werden.
- Dark-, Light- und High-Contrast-Themes unterstützt werden.
- sämtliche Testmodi auswählbar sind.
- laufende Tests ohne Verlust der Benutzeroberfläche überwacht werden können.
- keine Heap-Allokationen während der Darstellung stattfinden.
- sämtliche Animationen ausschließlich Motion Tokens verwenden.

---

# 26. Referenzen

## ADRs

- ADR-BOOTMANAGER-UI-0007 – Memory Test Interface
- ADR-BOOTDIAG-0001 – Diagnostics Framework
- ADR-BOOTDESIGN-0001 – Boot Design Language

## NPSPECs

- NPSPEC-BOOTMANAGER-UI-0001 – Main Boot Selection Screen
- NPSPEC-BOOTCONTROL-PROGRESS-0001 – Progress Indicator
- NPSPEC-BOOTCONTROL-LABEL-0001 – Label
- NPSPEC-BOOTCONTROL-STATUSBADGE-0001 – Status Badge
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTMOTION-0001 – Motion Engine
- NPSPEC-BOOTDIAG-0001 – Diagnostics Framework

---

# 27. Zusammenfassung

Die **Memory Test Interface** stellt eine spezialisierte Diagnoseoberfläche zur Überprüfung des Arbeitsspeichers innerhalb des NovaOS Boot Managers bereit. Sie kombiniert konfigurierbare Testverfahren, detaillierte Hardwareinformationen, Echtzeit-Fortschrittsanzeigen und eine präzise Fehlerdarstellung in einer konsistenten Benutzeroberfläche auf Basis der NovaOS Design Language. Durch vollständige Themeintegration, Accessibility-Unterstützung und deterministische Aktualisierung ermöglicht sie eine zuverlässige Analyse des Systemspeichers bereits vor dem Start des Betriebssystems.