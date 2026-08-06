# NPSPEC-BOOTDIAG-0003
# Input Event Tracing

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTDIAG-0003 |
| Titel | Input Event Tracing |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Diagnostics |
| Priorität | Hoch |
| Gültigkeitsbereich | Gesamte NovaOS Boot UI |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTDIAG-0001, NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTINPUT-0008, NPSPEC-BOOTINPUT-0010, NPSPEC-BOOTPERF-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Input Event Tracing Framework** der NovaOS-Bootumgebung.

Das Framework zeichnet sämtliche Eingabeereignisse der Bootoberfläche auf, um die Analyse von Bedienproblemen, Fokusfehlern, Performanceproblemen und unerwartetem UI-Verhalten zu ermöglichen.

Das Tracing dient ausschließlich Diagnose- und Entwicklungszwecken und beeinflusst weder die Eingabelogik noch das Verhalten der Benutzeroberfläche.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Tastatur
- Maus
- Touch
- Touchpad
- Gamepad
- Firmware-Eingaben
- Accessibility-Eingaben
- sämtliche Boot Controls

---

# 3. Ziele

Das Tracing muss:

- alle Eingabeereignisse erfassen
- Ereignisse chronologisch speichern
- minimale Laufzeitkosten verursachen
- deterministisch arbeiten
- filterbar sein
- exportierbar sein

---

# 4. Architektur

Das Framework besteht aus:

- Input Collector
- Event Queue Monitor
- Focus Tracker
- Dispatch Tracer
- Timing Recorder
- Export Service
- Diagnostics API

Alle Komponenten arbeiten unabhängig vom eigentlichen Input Dispatcher.

---

# 5. Erfasste Eingabegeräte

Unterstützt werden:

- USB-Tastaturen
- PS/2-Tastaturen
- USB-Mäuse
- PS/2-Mäuse
- Touchscreens
- Precision Touchpads
- Gamepads
- zukünftige Eingabegeräte

---

# 6. Ereignistypen

Mindestens folgende Ereignisse werden protokolliert:

- Taste gedrückt
- Taste losgelassen
- Taste wiederholt
- Mausbewegung
- Mausklick
- Doppelklick
- Scrollen
- Touch Down
- Touch Move
- Touch Up
- Fokuswechsel
- Shortcut-Ausführung

---

# 7. Ereignisstruktur

Jedes Ereignis enthält:

- Event-ID
- Zeitstempel
- Eingabegerät
- Ereignistyp
- Zielobjekt
- Fokusobjekt
- Statusflags

Optional:

- Scan Code
- Virtual Key
- Unicode-Zeichen

---

# 8. Zeitmessung

Für jedes Ereignis werden gespeichert:

- Erfassungszeit
- Dispatch-Zeit
- Verarbeitungszeit
- Abschlusszeit

Hieraus werden folgende Kennzahlen berechnet:

- Eingabelatenz
- Dispatchdauer
- Bearbeitungsdauer

---

# 9. Fokusverfolgung

Bei jedem Fokuswechsel werden protokolliert:

- vorheriges Objekt
- neues Objekt
- Ursache
- Eingabegerät
- Navigationsrichtung

---

# 10. Ereigniskette

Das Framework verfolgt den vollständigen Lebenszyklus:

```text
Gerät

↓

Treiber

↓

Input Queue

↓

Dispatcher

↓

Focus Manager

↓

UI Control

↓

Aktion

↓

Rendering
```

Jeder Übergang kann einzeln analysiert werden.

---

# 11. Ereignisfilter

Filter sind möglich nach:

- Eingabegerät
- Ereignistyp
- Zeitbereich
- UI-Control
- Fenster
- Dialog
- Benutzeraktion

---

# 12. Performance

Zu jedem Ereignis werden gemessen:

- Queue-Länge
- Queue-Wartezeit
- Dispatch-Zeit
- Handler-Zeit
- Render-Auslösung

---

# 13. Tastaturereignisse

Zusätzlich werden gespeichert:

- Scan Code
- Virtual Key
- Modifier
- Repeat Counter
- Shortcut-ID

Passwort- oder PIN-Eingaben dürfen niemals im Klartext protokolliert werden.

---

# 14. Mausereignisse

Erfasst werden:

- Position
- Delta
- Buttons
- Scrollrichtung
- Scrollmenge
- Pointer Capture

---

# 15. Touchereignisse

Zu erfassen sind:

- Touch-ID
- Position
- Kontaktfläche
- Druck (falls verfügbar)
- Bewegungsrichtung
- Dauer

---

# 16. Fehlererkennung

Das Framework erkennt:

- verlorene Ereignisse
- doppelte Ereignisse
- ungültige Fokuswechsel
- Queue-Überläufe
- Dispatch-Timeouts
- Eingabelatenzen

---

# 17. Ring Buffer

Alle Ereignisse werden in einem Ring Buffer gespeichert.

Eigenschaften:

- feste Größe
- FIFO
- Überschreiben ältester Einträge
- keine Heap-Allokationen

---

# 18. Export

Unterstützte Exportformate:

- Nova Diagnostic Format (NDF)
- JSON
- CSV
- Binärformat

Export erfolgt ausschließlich auf Benutzeranforderung.

---

# 19. Datenschutz

Folgende Daten dürfen niemals gespeichert werden:

- Passwörter
- PINs
- Recovery Keys
- Verschlüsselungsschlüssel
- TPM-Geheimnisse

Texteingaben werden standardmäßig maskiert oder vollständig unterdrückt.

---

# 20. Sicherheit

Das Tracing darf:

- Eingaben niemals verändern
- Eingaben niemals verzögern
- Fokus nicht beeinflussen
- Eingaben nicht blockieren

Die Ereigniserfassung erfolgt ausschließlich passiv.

Firmware- und Bootdiagnosen sollten Diagnoseinformationen getrennt von der eigentlichen Eingabeverarbeitung erfassen, um das Bootverhalten nicht zu verändern. :contentReference[oaicite:0]{index=0}

---

# 21. Speicherverwaltung

Das Framework verwendet:

- Event Pool
- Ring Buffer
- Statistics Cache
- Export Buffer

Alle Speicherbereiche werden während der Initialisierung reserviert.

Heap-Allokationen während der Laufzeit sind verboten.

---

# 22. API

```c
typedef enum
{
    NOVA_INPUT_EVENT_KEY_DOWN,
    NOVA_INPUT_EVENT_KEY_UP,
    NOVA_INPUT_EVENT_KEY_REPEAT,
    NOVA_INPUT_EVENT_MOUSE_MOVE,
    NOVA_INPUT_EVENT_MOUSE_BUTTON,
    NOVA_INPUT_EVENT_MOUSE_WHEEL,
    NOVA_INPUT_EVENT_TOUCH_DOWN,
    NOVA_INPUT_EVENT_TOUCH_MOVE,
    NOVA_INPUT_EVENT_TOUCH_UP,
    NOVA_INPUT_EVENT_FOCUS_CHANGED
}
nova_input_event_type_t;

typedef struct
{
    uint64_t timestamp_us;
    nova_input_event_type_t type;
    uint32_t device_id;
    uint32_t target_object;
    uint32_t flags;
}
nova_input_trace_event_t;

nova_result_t
nova_input_trace_initialize(void);

nova_result_t
nova_input_trace_record(
    const nova_input_trace_event_t* event);

const nova_input_trace_event_t*
nova_input_trace_get(
    uint32_t index);

nova_result_t
nova_input_trace_export(void);

void
nova_input_trace_reset(void);
```

---

# 23. Diagnosedaten

Zu protokollieren sind:

- Eingabegerät
- Ereignistyp
- Zeitstempel
- Fokuswechsel
- Queue-Länge
- Eingabelatenz
- Dispatchdauer
- Handlerdauer
- Render-Auslösung
- Fehlerereignisse

---

# 24. Testfälle

Zu testen sind:

- Tastatureingaben
- Mausbewegungen
- Touchsteuerung
- Fokusnavigation
- Shortcut-Ausführung
- Queue-Überlauf
- Exportfunktion
- Ring-Buffer-Rotation
- Performance unter Last
- Datenschutz bei Passwortdialogen

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Eingabeereignisse vollständig und chronologisch erfasst werden.
- Fokuswechsel lückenlos nachvollziehbar sind.
- Eingabelatenzen und Dispatchzeiten gemessen werden.
- keine sensitiven Eingabedaten protokolliert werden.
- der Ring Buffer ohne Heap-Allokationen arbeitet.
- Export in alle definierten Formate möglich ist.
- das Tracing den Bootvorgang nicht messbar beeinflusst.
- sämtliche Daten über die Diagnostics API verfügbar sind.

---

# 26. Referenzen

## ADRs

- ADR-BOOTDIAG-0003 – Input Event Tracing
- ADR-BOOTINPUT-0001 – Input Architecture
- ADR-BOOTPERF-0001 – Boot Performance Requirements

## NPSPECs

- NPSPEC-BOOTDIAG-0001 – Boot UI Diagnostics
- NPSPEC-BOOTINPUT-0001 – Input Architecture
- NPSPEC-BOOTINPUT-0008 – Focus Management
- NPSPEC-BOOTINPUT-0010 – Keyboard Shortcuts
- NPSPEC-BOOTPERF-0001 – Boot UI Performance Requirements

---

# 27. Zusammenfassung

Das **Input Event Tracing Framework** stellt die vollständige Diagnoseinfrastruktur für sämtliche Benutzereingaben innerhalb der NovaOS-Bootoberfläche bereit. Es zeichnet den gesamten Lebenszyklus eines Eingabeereignisses – von der Geräteerfassung über den Dispatcher bis zur Ausführung der UI-Aktion – deterministisch auf. Durch Zeitmessungen, Fokusverfolgung, Ring-Buffer-Speicherung und Exportfunktionen ermöglicht das Framework eine präzise Analyse von Eingabelatenzen, Navigationsproblemen und Bedienfehlern, ohne das Verhalten der Bootoberfläche oder deren Sicherheit zu beeinflussen.