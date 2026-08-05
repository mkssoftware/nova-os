# NPSPEC-BOOTDIALOG-0007
# Progress Dialog

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTDIALOG-0007 |
| Titel | Progress Dialog |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Dialog System |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTDIALOG-0001 bis NPSPEC-BOOTDIALOG-0006, NPSPEC-BOOTMOTION-0011 |
| Zugehörige ADRs | ADR-BOOTDIALOG-0007, ADR-BOOTMOTION-0011, ADR-BOOTTASK-0001, ADR-BOOTACCESS-0001, ADR-BOOTPERF-0003 |

---

# 1. Ziel

Diese Spezifikation definiert den **Progress Dialog** der NovaOS Boot UI.

Der Progress Dialog informiert den Benutzer über den Fortschritt langlaufender Boot- oder Wartungsvorgänge.

Das System unterstützt:

- Fortschrittsbalken
- unbestimmten Fortschritt
- Prozentanzeige
- Statusmeldungen
- Zeitabschätzung
- Abbruch (optional)
- Hintergrundinformationen
- Aufgabenwechsel

Der Benutzer erhält jederzeit Rückmeldung über den aktuellen Arbeitsfortschritt.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Fortschrittsdialoge.

Sie umfasst:

- Backup
- Wiederherstellung
- Recovery
- Datenträgerprüfung
- Dateisystemreparatur
- Verschlüsselung
- Entschlüsselung
- Systemwartung

Nicht Bestandteil sind:

- Warnungen
- Fehlermeldungen
- Informationsdialoge

---

# 3. Architektur

```text
Task

↓

Progress Manager

↓

Progress Dialog

↓

Progress View

↓

Rendering
```

---

# 4. Grundprinzip

Der Progress Dialog stellt den aktuellen Bearbeitungszustand einer Aufgabe dar.

Er zeigt:

- Status
- Fortschritt
- aktuelle Aktivität
- optional Restzeit

Der Dialog aktualisiert sich automatisch während der Ausführung.

---

# 5. Aufgaben

Der Progress Dialog übernimmt:

- Fortschrittsanzeige
- Statusanzeige
- Prozentberechnung
- Zeitdarstellung
- Aufgabenbeschreibung
- Benutzerinformation
- Ergebnisanzeige
- Abschlussmeldung

---

# 6. Dialogaufbau

Der Dialog besteht aus:

```text
Titel

↓

Beschreibung

↓

Progress Bar

↓

Status

↓

Zeitinformation

↓

Optionale Schaltflächen
```

---

# 7. Fortschrittsarten

Unterstützt werden:

- Determinate
- Indeterminate

**Determinate**

zeigt einen bekannten Fortschritt.

**Indeterminate**

zeigt laufende Aktivität ohne bekannten Endwert.

---

# 8. Prozentanzeige

Ist der Fortschritt bekannt:

```text
0 %

...

100 %
```

Die Anzeige erfolgt ganzzahlig.

---

# 9. Statusmeldungen

Beispiele:

- Initialisierung
- Dateien werden gelesen
- Datenträger wird geprüft
- Wiederherstellung läuft
- Abschluss

Statusmeldungen dürfen sich während der Laufzeit ändern.

---

# 10. Zeitinformationen

Optional werden angezeigt:

- vergangene Zeit
- geschätzte Restzeit
- Gesamtdauer

Ist keine Schätzung möglich, wird keine Restzeit dargestellt.

---

# 11. Fortschrittsbalken

Der Balken unterstützt:

- lineare Darstellung
- indeterminierte Animation
- Theme-Farben
- High-Contrast-Modus

Der Balken orientiert sich an NPSPEC-BOOTMOTION-0011.

---

# 12. Benutzerinteraktion

Je nach Aufgabe können unterstützt werden:

- Cancel
- Pause
- Continue
- Close

Kritische Systemaufgaben dürfen das Abbrechen deaktivieren.

---

# 13. Abschluss

Nach erfolgreichem Abschluss:

```text
100 %

↓

Success Message

↓

Close
```

Der Dialog bleibt optional bis zur Benutzerbestätigung geöffnet.

---

# 14. Fehler

Tritt während der Ausführung ein Fehler auf:

```text
Progress

↓

Error Dialog
```

Der Progress Dialog wird beendet oder pausiert.

---

# 15. Darstellung

Der Progress Dialog verwendet:

- Glass Dialog
- Progress Bar
- Statussymbol
- optionale Hintergrundinformationen

Die Größe richtet sich nach Inhalt und Safe Display Area.

---

# 16. Motion

Standardanimation:

```text
Fade

+

Scale

+

Progress Animation
```

Bei Reduced Motion:

```text
Fade
```

Die Fortschrittsanimation bleibt funktional erhalten.

---

# 17. Accessibility

Unterstützt werden:

- Screenreader
- High Contrast
- Reduced Motion
- skalierbare Schriftgrößen
- Tastaturbedienung

Fortschrittsänderungen sollen bei größeren Änderungen angekündigt werden.

---

# 18. Speicherverwaltung

Progress Dialoge verwenden:

- statische Dialogobjekte
- feste Fortschrittsstrukturen
- feste Animationsobjekte

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Statusaktualisierung
- O(1)-Fortschrittsaktualisierung
- deterministische Laufzeit
- keine Heap-Allokationen

Fortschrittsaktualisierungen dürfen die Benutzeroberfläche nicht blockieren.

---

# 20. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültiger Fortschrittswert
- negativer Fortschritt
- Fortschritt über 100 %
- beschädigte Statusdaten
- Nullpointer

Ungültige Werte werden auf den zulässigen Bereich begrenzt.

---

# 21. Sicherheit

Das System verhindert:

- Fortschrittswerte außerhalb des gültigen Bereichs
- irreführende Prozentanzeigen
- inkonsistente Statusmeldungen
- Fokusverlust
- beschädigte Fortschrittsdaten

---

# 22. API

```c
typedef enum
{
    NOVA_PROGRESS_DETERMINATE,
    NOVA_PROGRESS_INDETERMINATE
}
nova_progress_mode_t;

nova_result_t
nova_progress_dialog_show(
    const char* title,
    nova_progress_mode_t mode);

nova_result_t
nova_progress_dialog_update(
    uint32_t progress,
    const char* status);

nova_result_t
nova_progress_dialog_close(void);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Fortschrittsmodus
- Prozentwert
- Statusmeldungen
- Laufzeit
- Restzeit
- Aktualisierungsrate
- Speicherverbrauch
- Fehleranzahl

---

# 24. Testfälle

Zu testen sind:

- Determinate Progress
- Indeterminate Progress
- 0 %
- 100 %
- Statuswechsel
- Cancel
- Fehler während Progress
- Accessibility
- Reduced Motion
- Recovery

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- bestimmte und unbestimmte Fortschrittsanzeigen unterstützt werden.
- Prozentwerte korrekt dargestellt werden.
- Statusmeldungen während der Ausführung aktualisiert werden.
- Abschluss- und Fehlerzustände korrekt verarbeitet werden.
- Accessibility vollständig unterstützt wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTDIALOG-0007 – Fortschrittsdialoge
- ADR-BOOTMOTION-0011 – Fortschrittsanimationen
- ADR-BOOTTASK-0001 – Hintergrundaufgaben
- ADR-BOOTACCESS-0001 – Barrierefreie Navigation
- ADR-BOOTPERF-0003 – Performance Budget

## NPSPECs

- NPSPEC-BOOTDIALOG-0001 – Boot Dialog Architecture
- NPSPEC-BOOTDIALOG-0003 – Glass Dialog
- NPSPEC-BOOTMOTION-0011 – Progress Animations
- NPSPEC-BOOTCONTROL-PROGRESSBAR-0001 – Progress Bar
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 27. Zusammenfassung

Der **Progress Dialog** stellt den Fortschritt langlaufender Vorgänge innerhalb der NovaOS Bootoberfläche dar. Er unterstützt bestimmte und unbestimmte Fortschrittsanzeigen, Statusmeldungen, optionale Zeitinformationen und sichere Benutzerinteraktionen. Durch die vollständige Integration in Motion, Accessibility und das Dialogsystem entsteht eine konsistente und performante Fortschrittsdarstellung für BIOS- und UEFI-basierte Bootumgebungen.