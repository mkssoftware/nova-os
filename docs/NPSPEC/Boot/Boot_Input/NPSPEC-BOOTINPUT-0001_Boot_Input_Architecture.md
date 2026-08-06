# NPSPEC-BOOTINPUT-0001
# Boot Input Architecture

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTINPUT-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Input System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | ADR-BOOTINPUT-0001, NPSPEC-BOOTUI-0001, NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTNAV-0001, NPSPEC-BOOTLAYOUT-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die vollständige Eingabearchitektur der NovaOS-Bootoberfläche.

Die Architektur stellt eine einheitliche Abstraktionsschicht für sämtliche Eingabegeräte bereit und sorgt dafür, dass alle UI-Komponenten unabhängig von der zugrunde liegenden Firmware (BIOS oder UEFI) identisch auf Benutzereingaben reagieren.

Die Architektur bildet die Grundlage für sämtliche zukünftigen Eingabesysteme innerhalb des Boot Managers.

---

# 2. Geltungsbereich

Die Spezifikation gilt für:

- BIOS
- UEFI
- Boot Manager
- Recovery Environment
- Setup
- Installer
- Diagnosemodule

---

# 3. Ziele

Die Architektur muss:

- hardwareunabhängig arbeiten
- BIOS und UEFI abstrahieren
- deterministisch sein
- reproduzierbare Eingaben liefern
- mehrere Eingabegeräte gleichzeitig unterstützen
- zukünftige Eingabegeräte integrieren können

---

# 4. Architektur

```text
Firmware

↓

Firmware Input Driver

↓

Input HAL

↓

Input Manager

↓

Event Queue

↓

Focus Manager

↓

Navigation

↓

Controls

↓

Application Logic
```

---

# 5. Komponenten

Das Eingabesystem besteht aus:

- Firmware Input Layer
- Hardware Abstraction Layer
- Device Manager
- Event Queue
- Input Dispatcher
- Focus Manager
- Navigation Engine
- Gesture Engine
- Accessibility Layer

---

# 6. Unterstützte Eingabegeräte

Die Architektur unterstützt:

### Tastatur

- USB
- PS/2
- Bluetooth (UEFI)

### Maus

- USB HID
- PS/2
- UEFI Pointer

### Touch

- HID Touch
- Multitouch (optional)

### Controller

- XInput-kompatibel
- HID Gamepad

### Fernbedienungen

- HID-kompatibel

### Zukünftige Geräte

- Sprachsteuerung
- Eye Tracking
- Spezialhardware

---

# 7. Abstraktionsmodell

Alle Geräte erzeugen dieselben logischen Ereignisse.

Beispiel:

```text
USB Keyboard

↓

Key Event

↓

Input Manager

↓

Focused Control
```

oder

```text
Mouse

↓

Pointer Event

↓

Input Manager

↓

Focused Control
```

Controls kennen niemals die tatsächliche Hardware.

---

# 8. Ereignismodell

Alle Eingaben werden als Events verarbeitet.

Grundtypen:

```text
Key Down

Key Up

Character Input

Pointer Move

Pointer Down

Pointer Up

Wheel

Touch Begin

Touch Move

Touch End

Controller Button

Controller Axis

Gesture
```

---

# 9. Event Queue

Alle Ereignisse werden in einer zentralen Queue gespeichert.

Eigenschaften:

- FIFO
- deterministisch
- threadfrei
- keine Prioritätsumkehr
- feste Größe

Überlauf führt zu einem protokollierten Fehler.

---

# 10. Event Dispatcher

Der Dispatcher übernimmt:

- Zielermittlung
- Focus Routing
- Navigation Routing
- Gesture Routing
- Bubble Routing

Der Dispatcher besitzt keine UI-Logik.

---

# 11. Focus Routing

Die Eingabe gelangt ausschließlich zum aktuell fokussierten Control.

Falls kein Fokus existiert:

```text
Root Control

↓

Navigation

↓

Default Focus
```

---

# 12. Eingabemodi

Unterstützt werden:

- Keyboard
- Pointer
- Touch
- Controller
- Mixed Input

Der aktive Modus wird automatisch erkannt.

---

# 13. Gerätewechsel

Ein Gerätewechsel erfolgt automatisch.

Beispiel:

```text
Keyboard

↓

Mouse

↓

Controller
```

Der Fokus bleibt erhalten.

Nur visuelle Hinweise dürfen angepasst werden.

---

# 14. Wiederholte Eingaben

Unterstützt werden:

- Initial Delay
- Auto Repeat
- Controller Repeat

Parameter werden zentral konfiguriert.

---

# 15. Navigation

Navigation erfolgt über:

- Fokus
- Richtung
- Aktionen

Die Navigation kennt:

- Links
- Rechts
- Oben
- Unten
- Vorwärts
- Rückwärts

---

# 16. Koordinatensystem

Alle Pointer verwenden:

```text
Logical UI Coordinates
```

Keine Hardwarekoordinaten gelangen in Controls.

---

# 17. Touch

Touch wird behandelt als:

```text
Pointer

+

Gesture
```

Unterstützt:

- Tap
- Long Press
- Drag
- Swipe

Optional:

- Pinch

---

# 18. Controller

Controller unterstützen:

- Digital Pad
- Analog Stick
- Trigger
- Face Buttons
- Shoulder Buttons

Die Zuordnung erfolgt über ein zentrales Mapping.

---

# 19. Tastatur

Unterstützt:

- Scan Codes
- Unicode
- Dead Keys
- Modifier

Modifier:

- Shift
- Ctrl
- Alt
- AltGr

---

# 20. Maus

Unterstützt:

- Position
- Links
- Rechts
- Mitte
- Wheel

Optional:

- Zusatztasten

---

# 21. Accessibility

Das Input-System unterstützt:

- Sticky Keys
- Slow Keys
- Repeat Keys
- Alternative Navigation
- Screen Reader Events

---

# 22. Sicherheit

Das System verhindert:

- doppelte Events
- ungültige Geräte
- Queue Overflow
- rekursive Events
- beschädigte Eventdaten

Alle Eingaben werden validiert.

---

# 23. Performance

Anforderungen:

- O(1)-Queue
- O(1)-Dispatch
- O(1)-Routing
- deterministische Laufzeit

Die Eingabelatenz darf unabhängig von der Anzahl der Controls bleiben.

---

# 24. API

```c
typedef struct nova_input_event
{
    nova_input_type_t type;

    uint64_t timestamp;

    union
    {
        nova_key_event_t key;

        nova_pointer_event_t pointer;

        nova_touch_event_t touch;

        nova_controller_event_t controller;
    };
}
nova_input_event_t;

nova_result_t
nova_input_initialize(void);

nova_result_t
nova_input_poll(void);

nova_result_t
nova_input_dispatch(void);

nova_result_t
nova_input_post(
    const nova_input_event_t* event);
```

---

# 25. Diagnosedaten

Zu erfassen sind:

- Geräte
- Queue-Auslastung
- Dispatch-Zeit
- Fokuswechsel
- Eventanzahl
- Wiederholungen
- Fehler
- Gerätewechsel

---

# 26. Testfälle

Zu testen sind:

- BIOS-Tastatur
- UEFI-Tastatur
- Maus
- Touch
- Controller
- Gerätewechsel
- Queue Overflow
- Accessibility
- Mixed Input
- Fehlerfälle

---

# 27. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche unterstützten Eingabegeräte identisch behandelt werden.
- BIOS und UEFI dieselbe Eingabeschnittstelle bereitstellen.
- alle Events deterministisch verarbeitet werden.
- Queue und Dispatcher O(1) arbeiten.
- Controls keine Hardwaredetails kennen.
- Accessibility vollständig unterstützt wird.
- keine dynamischen Speicherallokationen während der Laufzeit stattfinden.
- sämtliche Eingaben reproduzierbar verarbeitet werden.

---

# 28. Referenzen

## ADRs

- ADR-BOOTINPUT-0001 – Einheitliche Eingabearchitektur
- ADR-BOOTNAV-0001 – Navigation Model
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0001 – Performance Budget

## NPSPECs

- NPSPEC-BOOTUI-0001 – Boot UI Architecture
- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTNAV-0001 – Boot Navigation Model
- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 29. Zusammenfassung

Die **Boot Input Architecture** definiert die zentrale Eingabeschicht der NovaOS-Bootoberfläche. Sie abstrahiert sämtliche unterstützten Eingabegeräte über eine einheitliche Event-Architektur, verarbeitet Ereignisse deterministisch über eine zentrale Event Queue und leitet sie über Focus- und Navigationsmanager an die entsprechenden Controls weiter. Durch Hardwareunabhängigkeit, feste Speicherstrukturen, vollständige Accessibility-Unterstützung und identisches Verhalten auf BIOS- und UEFI-Systemen bildet sie die Grundlage für eine konsistente und erweiterbare Benutzereingabe im gesamten NovaOS-Bootmanager.