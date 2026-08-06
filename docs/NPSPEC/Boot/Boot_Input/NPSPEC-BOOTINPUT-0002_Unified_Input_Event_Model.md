# NPSPEC-BOOTINPUT-0002
# Unified Input Event Model

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTINPUT-0002 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Input System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTNAV-0001, NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Unified Input Event Model** der NovaOS Boot UI.

Das Unified Input Event Model stellt ein einheitliches Ereignismodell bereit, das sämtliche Eingabegeräte unabhängig von ihrer Hardware oder Firmware abstrahiert. Jedes UI-Control verarbeitet ausschließlich standardisierte Ereignisse und besitzt keinerlei Kenntnis über BIOS-, UEFI- oder Hardware-spezifische Eingaben.

Das Ereignismodell bildet die zentrale Kommunikationsschnittstelle zwischen Eingabegeräten und der Benutzeroberfläche.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- BIOS
- UEFI
- Boot Manager
- Recovery
- Installer
- Diagnosemodule
- sämtliche UI-Controls

---

# 3. Ziele

Das Event Model muss:

- hardwareunabhängig sein
- deterministisch arbeiten
- reproduzierbar sein
- plattformneutral sein
- synchronisierte Ereignisse liefern
- zukünftige Geräte unterstützen

---

# 4. Architektur

```text
Input Device

↓

Firmware Driver

↓

Input HAL

↓

Unified Event

↓

Event Queue

↓

Dispatcher

↓

Focused Control
```

---

# 5. Grundprinzip

Alle Eingabegeräte erzeugen dieselbe Ereignisstruktur.

Beispiele:

```text
Keyboard

↓

Key Down
```

```text
Mouse

↓

Pointer Move
```

```text
Touch

↓

Touch Begin
```

Alle Ereignisse besitzen denselben Header.

---

# 6. Event Header

Jedes Ereignis enthält:

- Event-ID
- Event-Typ
- Zeitstempel
- Geräte-ID
- Geräteklasse
- Ziel-Control (optional)
- Flags
- Sequenznummer

Der Header ist für alle Eventtypen identisch.

---

# 7. Event-Kategorien

Unterstützt werden:

### Keyboard

- KeyDown
- KeyUp
- CharacterInput

### Pointer

- Move
- ButtonDown
- ButtonUp
- Wheel

### Touch

- Begin
- Update
- End
- Cancel

### Controller

- Button
- Axis
- Trigger

### Navigation

- FocusNext
- FocusPrevious
- Navigate

### System

- DeviceAdded
- DeviceRemoved
- DeviceChanged

---

# 8. Event-Lebenszyklus

```text
Hardware

↓

HAL

↓

Unified Event

↓

Queue

↓

Dispatcher

↓

Control

↓

Handled
```

Ein Event darf genau einmal verarbeitet werden.

---

# 9. Event Flags

Unterstützt werden:

```text
Handled

Consumed

Repeat

Synthetic

Accessibility

Generated
```

Flags dürfen kombiniert werden.

---

# 10. Zeitmodell

Jedes Ereignis besitzt:

- monotonen Zeitstempel
- Sequenznummer
- Gerätezeit (optional)

Die Reihenfolge der Ereignisse wird ausschließlich durch die Sequenznummer bestimmt.

---

# 11. Geräteidentität

Jedes Event enthält:

- Device ID
- Device Type
- Device Instance

Die eigentliche Hardware bleibt verborgen.

---

# 12. Event Payload

Je nach Eventtyp:

### Keyboard

- Key
- Modifier
- Unicode

### Pointer

- Position
- Buttons
- Delta

### Touch

- Kontaktfläche
- Druck
- Position

### Controller

- Button
- Achse
- Trigger

---

# 13. Routing

Der Dispatcher bestimmt das Ziel anhand:

- Fokus
- Capture
- Navigation
- Overlay
- Dialog

Das Event kennt sein Ziel nicht selbst.

---

# 14. Event Capture

Ein Control kann ein Event temporär übernehmen.

Beispiele:

- Drag
- Slider
- Scrollbar

Während Capture erhalten andere Controls keine Pointer-Ereignisse desselben Zeigers.

---

# 15. Event Bubbling

Unterstützt wird:

```text
Control

↓

Parent

↓

Root
```

Ein Event kann jederzeit als **Handled** markiert werden.

---

# 16. Ereignisfilter

Vor der Zustellung können Filter ausgeführt werden:

- Accessibility
- Gesten
- Navigation
- Dialogsystem
- Eingabesperren

Filter verändern niemals die Reihenfolge der Ereignisse.

---

# 17. Mehrfacheingaben

Mehrere Geräte dürfen gleichzeitig aktiv sein.

Beispiele:

- Tastatur + Maus
- Controller + Maus
- Touch + Tastatur

Alle Ereignisse werden gemeinsam über dieselbe Queue verarbeitet.

---

# 18. Accessibility

Accessibility erzeugt zusätzliche Ereignisse:

- FocusChanged
- Announcement
- NavigationHint

Diese besitzen denselben Header wie alle anderen Events.

---

# 19. Sicherheit

Das System verhindert:

- doppelte Sequenznummern
- ungültige Eventtypen
- beschädigte Payloads
- rekursive Events
- manipulierte Flags

Alle Ereignisse werden vor der Verarbeitung validiert.

---

# 20. Speicherverwaltung

Events verwenden:

- feste Eventstrukturen
- Objektpools
- statische Queues

Heap-Allokationen während der Laufzeit sind verboten.

---

# 21. Performance

Anforderungen:

- O(1)-Eventerzeugung
- O(1)-Queue
- O(1)-Dispatch
- deterministische Laufzeit

Das Ereignismodell darf keine geräteabhängigen Sonderpfade besitzen.

---

# 22. API

```c
typedef enum
{
    NOVA_EVENT_KEY_DOWN,
    NOVA_EVENT_KEY_UP,
    NOVA_EVENT_CHARACTER,
    NOVA_EVENT_POINTER_MOVE,
    NOVA_EVENT_POINTER_DOWN,
    NOVA_EVENT_POINTER_UP,
    NOVA_EVENT_POINTER_WHEEL,
    NOVA_EVENT_TOUCH_BEGIN,
    NOVA_EVENT_TOUCH_UPDATE,
    NOVA_EVENT_TOUCH_END,
    NOVA_EVENT_CONTROLLER_BUTTON,
    NOVA_EVENT_CONTROLLER_AXIS,
    NOVA_EVENT_NAVIGATION,
    NOVA_EVENT_SYSTEM
}
nova_event_type_t;

typedef struct
{
    uint64_t sequence;
    uint64_t timestamp;
    uint32_t device_id;
    nova_event_type_t type;
    uint32_t flags;
}
nova_event_header_t;

typedef struct
{
    nova_event_header_t header;
    union
    {
        nova_key_event_t key;
        nova_pointer_event_t pointer;
        nova_touch_event_t touch;
        nova_controller_event_t controller;
    };
}
nova_input_event_t;
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Sequenznummer
- Eventtyp
- Geräteklasse
- Dispatchzeit
- Ziel-Control
- Verarbeitungsdauer
- Fehler
- Queueposition

---

# 24. Testfälle

Zu testen sind:

- Tastatur
- Maus
- Touch
- Controller
- Mehrfachgeräte
- Queue
- Bubbling
- Capture
- Accessibility
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Geräte identische Eventstrukturen erzeugen.
- jedes Event genau einmal verarbeitet wird.
- alle Ereignisse deterministisch sortiert werden.
- Capture und Bubbling korrekt funktionieren.
- Accessibility dieselbe Eventarchitektur verwendet.
- keine dynamischen Speicherallokationen erfolgen.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTINPUT-0001 – Einheitliche Eingabearchitektur
- ADR-BOOTINPUT-0002 – Unified Input Event Model
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0001 – Performance Budget

## NPSPECs

- NPSPEC-BOOTINPUT-0001 – Boot Input Architecture
- NPSPEC-BOOTNAV-0001 – Boot Navigation Model
- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 27. Zusammenfassung

Das **Unified Input Event Model** definiert die standardisierte Ereignisstruktur der NovaOS-Bootoberfläche. Sämtliche Eingabegeräte erzeugen identische Ereignisse mit einem gemeinsamen Header und gerätespezifischer Nutzlast. Durch deterministische Verarbeitung, einheitliches Routing, Unterstützung für Event Capture und Bubbling sowie statische Speicherverwaltung entsteht eine robuste, erweiterbare und hardwareunabhängige Grundlage für alle Eingaben innerhalb des NovaOS Boot Managers auf BIOS- und UEFI-Systemen.