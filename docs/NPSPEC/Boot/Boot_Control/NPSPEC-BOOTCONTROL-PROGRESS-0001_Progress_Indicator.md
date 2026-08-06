# NPSPEC-BOOTCONTROL-PROGRESS-0001
# Progress Indicator

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-PROGRESS-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-0002, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTCONTROL-0004, NPSPEC-BOOTMOTION-0011, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Progress Indicator Control** der NovaOS Boot UI.

Ein Progress Indicator visualisiert den Fortschritt einer laufenden Operation. Er informiert den Benutzer über den aktuellen Bearbeitungsstand oder signalisiert, dass eine Aufgabe aktiv ausgeführt wird.

Typische Einsatzbereiche:

- Bootvorgang
- Recovery
- Backup
- Wiederherstellung
- Datenträgerprüfung
- Systemdiagnose
- Modulinitialisierung
- Updateprozesse

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Fortschrittsanzeigen.

Sie umfasst:

- Determinate Progress Bar
- Indeterminate Progress Bar
- Circular Progress Indicator
- Spinner
- Progress Ring

Nicht Bestandteil sind:

- Slider
- Scrollbars
- Ladeanimationen ohne Statusbezug

---

# 3. Architektur

```text
Boot Control Framework

↓

Progress Indicator

↓

Animation Engine

↓

Renderer
```

---

# 4. Grundprinzip

Ein Progress Indicator stellt den Fortschritt einer einzelnen Operation dar.

Es werden zwei Betriebsarten unterschieden:

- **Determinate** (Fortschritt bekannt)
- **Indeterminate** (Fortschritt unbekannt)

---

# 5. Aufgaben

Der Progress Indicator übernimmt:

- Fortschrittsdarstellung
- Statusvisualisierung
- Animationen
- Themeintegration
- Accessibility
- Ereignisdarstellung

---

# 6. Aufbau

```text
Progress Indicator

├── Background

├── Track

├── Progress Fill

├── Value Label (optional)

├── Status Label (optional)

└── Animation Layer
```

---

# 7. Eigenschaften

Jeder Progress Indicator besitzt:

- ID
- Minimum
- Maximum
- Aktueller Wert
- Modus
- Sichtbarkeit
- Style
- Template

Optional:

- Prozentanzeige
- Statusbeschreibung
- Zeitinformation

---

# 8. Betriebsarten

### Determinate

Der Fortschritt ist bekannt.

```text
0 %

↓

100 %
```

---

### Indeterminate

Der Fortschritt ist unbekannt.

Es wird ausschließlich Aktivität dargestellt.

---

# 9. Zustände

```text
Normal

Running

Paused

Completed

Cancelled

Error

Hidden
```

---

# 10. Wertebereich

Bei Determinate:

```
Minimum ≤ Wert ≤ Maximum
```

Standard:

```
0

↓

100
```

---

# 11. Darstellung

Unterstützt werden:

- horizontale Progress Bar
- vertikale Progress Bar
- Ring
- Kreis
- Spinner

Die konkrete Darstellung wird durch Template und Style bestimmt.

---

# 12. Animationen

Unterstützt werden:

- Progress Fill
- Spinner Rotation
- Ring Rotation
- Fade
- Completed Animation
- Error Animation

Alle Animationen werden über das Motion-System gesteuert.

---

# 13. Aktualisierung

Der Ablauf:

```text
Neue Werte

↓

Validierung

↓

Animation

↓

Rendern
```

Der Fortschritt darf niemals rückwärts springen, sofern dies nicht ausdrücklich erlaubt ist.

---

# 14. Statusanzeige

Optional können dargestellt werden:

- Prozentwert
- Dateiname
- aktueller Schritt
- Restzeit
- Geschwindigkeit

Die Statusanzeige ist unabhängig von der Fortschrittsgrafik.

---

# 15. Themeintegration

Das Theme definiert:

- Trackfarbe
- Fortschrittsfarbe
- Fehlerfarbe
- Transparenz
- Schatten
- Animationen

Alle Darstellungsparameter stammen ausschließlich aus dem Theme.

---

# 16. Accessibility

Jeder Progress Indicator besitzt:

- Name
- Rolle
- Beschreibung
- aktueller Fortschritt
- Status

Screenreader geben den aktuellen Prozentwert oder den Status "Fortschritt läuft" aus.

---

# 17. Speicherverwaltung

Progress Indicators verwenden:

- gemeinsame Templates
- gemeinsame Styles
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Wertänderung
- O(1)-Animation
- O(1)-Statuswechsel
- deterministische Laufzeit

Nur geänderte Bereiche werden neu gerendert.

---

# 19. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- Wert außerhalb des Bereichs
- ungültiger Modus
- beschädigtes Template
- fehlender Style
- Nullpointer

Ungültige Werte werden automatisch auf den zulässigen Bereich begrenzt.

---

# 20. Sicherheit

Das System verhindert:

- Werte außerhalb des Bereichs
- inkonsistente Zustände
- fehlerhafte Animationen
- rekursive Aktualisierungen
- beschädigte Templates

Ein abgeschlossener Fortschritt darf nicht erneut aktualisiert werden, sofern keine neue Operation gestartet wurde.

---

# 21. API

```c
typedef struct nova_progress_indicator nova_progress_indicator_t;

typedef enum
{
    NOVA_PROGRESS_DETERMINATE,
    NOVA_PROGRESS_INDETERMINATE
}
nova_progress_mode_t;

nova_result_t
nova_progress_create(
    nova_progress_indicator_t** progress);

nova_result_t
nova_progress_set_range(
    nova_progress_indicator_t* progress,
    double minimum,
    double maximum);

nova_result_t
nova_progress_set_value(
    nova_progress_indicator_t* progress,
    double value);

nova_result_t
nova_progress_set_mode(
    nova_progress_indicator_t* progress,
    nova_progress_mode_t mode);

double
nova_progress_get_value(
    const nova_progress_indicator_t* progress);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- Progress-ID
- Modus
- aktueller Wert
- Status
- Aktualisierungen
- Renderzeit
- Speicherverbrauch
- Fehleranzahl

---

# 23. Testfälle

Zu testen sind:

- Determinate
- Indeterminate
- Spinner
- Ring
- Prozentanzeige
- Statuswechsel
- Themewechsel
- Accessibility
- Animationen
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- determinate und indeterminate Modi vollständig unterstützt werden.
- Fortschrittswerte korrekt dargestellt werden.
- Animationen flüssig und deterministisch ausgeführt werden.
- Theme und Templates vollständig verwendet werden.
- Accessibility vollständig integriert ist.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 25. Referenzen

## ADRs

- ADR-BOOTCONTROL-PROGRESS-0001 – Progress Indicator
- ADR-BOOTMOTION-0011 – Progress Animations
- ADR-BOOTTHEME-0001 – Theme-System
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0002 – Deterministische UI-Verarbeitung

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCONTROL-0002 – Control State Model
- NPSPEC-BOOTCONTROL-0003 – Control Styling
- NPSPEC-BOOTCONTROL-0004 – Control Templates
- NPSPEC-BOOTMOTION-0011 – Progress Animations
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 26. Zusammenfassung

Der **Progress Indicator** definiert das standardisierte Steuerelement zur Darstellung des Fortschritts laufender Operationen innerhalb der NovaOS-Bootoberfläche. Er unterstützt determinierte und indeterminierte Betriebsarten, verschiedene Darstellungsformen wie Fortschrittsbalken oder Spinner, vollständige Theme-Integration, Animationen sowie Accessibility. Durch deterministische Aktualisierung, statische Speicherverwaltung und eine klare Trennung zwischen Fortschrittslogik und Darstellung entsteht ein konsistentes und performantes UI-Control für BIOS- und UEFI-basierte Bootumgebungen.