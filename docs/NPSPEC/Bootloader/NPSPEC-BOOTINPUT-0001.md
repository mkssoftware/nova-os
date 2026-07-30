# NPSPEC-BOOTINPUT-0001
# Nova Boot Input Framework

| Feld | Wert |
|------|------|
| Dokument | NPSPEC-BOOTINPUT-0001 |
| Titel | Nova Boot Input Framework |
| Version | 1.0.0 |
| Status | Angenommen |
| Kategorie | Boot Runtime |
| Bereich | Nova Boot Platform |
| Übergeordnet | NPSPEC-BOOTSTAGE2-0001 |
| Baut auf | NPSPEC-BOOTPLATFORM-0001, NPSPEC-BOOTMEMORY-0001 |
| Verwendet von | NPSPEC-BOOTGRAPHICS-0001, NPSPEC-BOOTUI-0001 |

---

# 1. Zweck

Das **Nova Boot Input Framework (NBIF)** definiert die vollständige Eingabearchitektur des Nova Bootloaders.

Es abstrahiert sämtliche Eingabegeräte von der Firmware und stellt der UI eine einheitliche Ereignisschnittstelle bereit.

Die Benutzeroberfläche arbeitet niemals direkt mit BIOS- oder UEFI-Eingaben.

---

# 2. Ziele

Das Input Framework soll

- plattformunabhängig arbeiten
- BIOS unterstützen
- UEFI unterstützen
- mehrere Eingabegeräte verwalten
- Touch vorbereiten
- Gamepads vorbereiten
- deterministisch arbeiten
- geringe Latenz besitzen

---

# 3. Unterstützte Geräte

Version 1 unterstützt

- PS/2 Tastatur
- USB Tastatur
- PS/2 Maus
- USB Maus

Vorbereitet

- Touchscreen
- Precision Touchpad
- Stylus
- Gamepad
- Joystick
- 3D-Maus

---

# 4. Architektur

```text
Firmware
↓
Platform Layer
↓
Input Driver
↓
Input Runtime
├── Keyboard Manager
├── Mouse Manager
├── Touch Manager
├── Event Queue
├── Focus Router
├── Shortcut Manager
└── Gesture Engine
```

---

# 5. Designprinzipien

Das Framework basiert auf

- Event Driven
- Plattformunabhängig
- Nicht blockierend
- Thread-Safe vorbereitet
- Erweiterbar

---

# 6. Input Pipeline

```text
Hardware

↓

Firmware

↓

Input Driver

↓

Raw Event

↓

Event Queue

↓

UI Event

↓

Widget
```

---

# 7. Input Runtime

Die Runtime verwaltet

- Geräte
- Ereignisse
- Fokus
- Wiederholungen
- Zustände
- Eingabeprioritäten

---

# 8. Device Manager

Der Device Manager erkennt automatisch

- angeschlossene Geräte
- entfernte Geräte
- Firmwareänderungen

Die Benutzeroberfläche erhält ausschließlich logische Geräte.

---

# 9. Input Device

```c
typedef struct
{
    uint64_t id;

    nova_input_device_type_t type;

    char name[64];

    bool connected;

    bool enabled;

} nova_input_device_t;
```

---

# 10. Gerätetypen

```c
typedef enum
{
    NOVA_INPUT_KEYBOARD,

    NOVA_INPUT_MOUSE,

    NOVA_INPUT_TOUCH,

    NOVA_INPUT_GAMEPAD,

    NOVA_INPUT_STYLUS,

    NOVA_INPUT_UNKNOWN

} nova_input_device_type_t;
```

---

# 11. Initialisierung

Die Geräte werden in folgender Reihenfolge initialisiert.

```text
Keyboard

↓

Mouse

↓

Touch

↓

Weitere Geräte
```

---

# 12. Anforderungen

Das Input Framework MUSS

- BIOS unterstützen
- UEFI unterstützen
- mehrere Geräte verwalten
- Ereignisse vereinheitlichen
- deterministisch arbeiten

---

# 13. SHOULD

Das Framework SOLLTE

- Hot-Plug vorbereiten
- Touch vorbereiten
- geringe Latenzen erreichen
- zukünftige Geräte unterstützen

---

# 14. MAY

Das Framework DARF

- biometrische Geräte
- Sprachsteuerung
- Eye Tracking
- VR-Controller

unterstützen.

---

# Architekturentscheidung (ADR)

Das Nova Boot Input Framework abstrahiert sämtliche Eingabegeräte in eine gemeinsame Laufzeitumgebung.

Die UI kennt ausschließlich logische Eingabeereignisse und besitzt keinerlei Kenntnisse über BIOS-Interrupts, UEFI-Protokolle oder konkrete Hardware.

Dadurch bleibt das gesamte Eingabesystem plattformunabhängig und kann später unverändert vom Kernel und Desktop übernommen werden.

---

# 15. Keyboard Manager

Der **Nova Keyboard Manager (NKM)** verwaltet sämtliche Tastatureingaben des Bootloaders.

Er abstrahiert BIOS- und UEFI-Tastatureingaben und stellt der UI ausschließlich logische Tastaturereignisse bereit.

---

# 16. Ziele

Der Keyboard Manager soll

- Scancodes vereinheitlichen
- Unicode erzeugen
- Tastaturlayouts unterstützen
- Modifier verwalten
- Key Repeat unterstützen
- Hotkeys erkennen
- geringe Latenz besitzen

---

# 17. Architektur

```text
Keyboard Driver
↓
Keyboard Manager
├── Scan Decoder
├── Layout Engine
├── Unicode Converter
├── Modifier Manager
├── Key Repeat
├── Shortcut Engine
└── Event Generator
```

---

# 18. Tastatur Pipeline

```text
Hardware

↓

Firmware

↓

Scan Code

↓

Key Decoder

↓

Unicode

↓

Key Event

↓

UI
```

---

# 19. Keyboard Objekt

```c
typedef struct
{
    uint64_t id;

    char layout[16];

    bool num_lock;

    bool caps_lock;

    bool scroll_lock;

} nova_keyboard_t;
```

---

# 20. Tastaturereignis

```c
typedef struct
{
    uint64_t timestamp;

    nova_keycode_t key;

    uint32_t unicode;

    bool pressed;

    bool repeated;

    uint32_t modifiers;

} nova_keyboard_event_t;
```

---

# 21. Scancode Layer

Intern arbeitet Nova mit einer eigenen Keycode-Tabelle.

```text
Firmware Scan Code

↓

Nova Keycode

↓

Unicode

↓

Widget
```

Dadurch bleiben BIOS und UEFI vollständig verborgen.

---

# 22. Nova Keycodes

```c
typedef enum
{
    NOVA_KEY_UNKNOWN = 0,

    NOVA_KEY_A,

    NOVA_KEY_B,

    NOVA_KEY_C,

    ...

    NOVA_KEY_F1,

    NOVA_KEY_F2,

    ...

    NOVA_KEY_ENTER,

    NOVA_KEY_ESCAPE,

    NOVA_KEY_SPACE,

    NOVA_KEY_TAB

} nova_keycode_t;
```

---

# 23. Unicode

Alle Tastatureingaben werden intern als Unicode verarbeitet.

Version 1 unterstützt

- UTF-8
- UTF-16 (intern vorbereitet)
- Unicode Codepoints

---

# 24. Tastaturlayouts

Version 1

- Deutsch
- Englisch (US)

Vorbereitet

- Französisch
- Spanisch
- Italienisch
- beliebige weitere Layouts

---

# 25. Layoutwechsel

Der Layoutwechsel erfolgt ausschließlich über den Keyboard Manager.

Widgets dürfen niemals selbst Tastaturbelegungen interpretieren.

---

# 26. Modifier

Unterstützt

- Shift
- Ctrl
- Alt
- AltGr
- Meta (vorbereitet)

---

# 27. Lock-Tasten

Unterstützt

- Caps Lock
- Num Lock
- Scroll Lock

Der aktuelle Status wird zentral gespeichert.

---

# 28. Key Repeat

Der Keyboard Manager unterstützt automatische Tastenwiederholung.

Standardwerte

| Parameter | Wert |
|-----------|-----:|
| Verzögerung | 500 ms |
| Wiederholrate | 30 Hz |

Diese Werte können systemweit angepasst werden.

---

# 29. Dead Keys

Vorbereitet

- Akzentzeichen
- Umlaute
- zusammengesetzte Zeichen

Version 1 beschränkt sich auf grundlegende Layouts.

---

# 30. Sondertasten

Unterstützt

- Escape
- Enter
- Tab
- Backspace
- Delete
- Insert
- Home
- End
- Page Up
- Page Down
- Pfeiltasten

---

# 31. Funktionstasten

Unterstützt

```text
F1

↓

F12
```

Reservierte Standardbelegung

| Taste | Funktion |
|--------|----------|
| F1 | Hilfe |
| F2 | Umbenennen (optional) |
| F5 | Aktualisieren |
| F8 | Erweiterte Bootoptionen |
| F10 | Boot-Menü |
| F12 | Diagnose |

---

# 32. Shortcut Engine

Shortcuts werden zentral verarbeitet.

Beispiele

- Ctrl+C (vorbereitet)
- Ctrl+V (vorbereitet)
- Alt+Enter
- Escape
- Enter

Die Zuordnung erfolgt über eine konfigurierbare Tabelle.

---

# 33. Shortcut Objekt

```c
typedef struct
{
    uint32_t modifiers;

    nova_keycode_t key;

    uint32_t command;

} nova_shortcut_t;
```

---

# 34. Eingabepuffer

Der Keyboard Manager verwendet einen Ringpuffer.

```text
Key Event

↓

Ring Buffer

↓

Event Queue

↓

Dispatcher
```

Dies verhindert den Verlust schneller Eingaben.

---

# 35. Fehlerbehandlung

Mögliche Fehler

- unbekannter Scancode
- ungültiges Layout
- Buffer Overflow
- fehlender Unicode-Wert
- Gerät getrennt

Diese Fehler werden protokolliert und beeinträchtigen die übrigen Geräte nicht.

---

# 36. Keyboard API

```c
nova_result_t
nova_keyboard_initialize(void);

void
nova_keyboard_update(void);

bool
nova_keyboard_get_event(
    nova_keyboard_event_t* event);
```

---

# 37. Anforderungen

Der Keyboard Manager MUSS

- Unicode unterstützen
- Key Repeat unterstützen
- Modifier verwalten
- Layouts unterstützen
- Ereignisse vereinheitlichen

---

# 38. SHOULD

Der Keyboard Manager SOLLTE

- Dead Keys vorbereiten
- Shortcuts unterstützen
- Ring Buffer verwenden
- geringe Latenz besitzen

---

# 39. MAY

Der Keyboard Manager DARF

- Makros
- benutzerdefinierte Layouts
- Mehrsprachigkeit
- OEM-Tastaturen

unterstützen.

---

# Architekturentscheidung (ADR)

Der Nova Keyboard Manager trennt **Firmware-Scancodes**, **logische Tastencodes** und **Unicode-Zeichen** vollständig voneinander.

Dadurch bleiben alle höheren Systemschichten unabhängig von BIOS, UEFI und der konkreten Hardware.

Gleichzeitig ermöglicht diese Architektur zukünftige Erweiterungen wie alternative Tastaturbelegungen, Makros und zusätzliche Eingabegeräte, ohne Änderungen an der öffentlichen API vorzunehmen.

---

# 40. Mouse Manager

Der **Nova Mouse Manager (NMM)** verwaltet sämtliche Maus- und Zeigereingaben innerhalb des Bootloaders.

Er abstrahiert

- PS/2 Maus
- USB Maus
- UEFI Pointer Protocol

zu einem gemeinsamen Eingabesystem.

Die Benutzeroberfläche arbeitet ausschließlich mit logischen Mausereignissen.

---

# 41. Ziele

Der Mouse Manager soll

- Mausbewegungen vereinheitlichen
- Cursor verwalten
- Doppelklick erkennen
- Scrollen unterstützen
- Drag & Drop vorbereiten
- geringe Latenz besitzen
- hohe Präzision erreichen

---

# 42. Architektur

```text
Mouse Driver
↓
Mouse Manager
├── Pointer Decoder
├── Cursor Manager
├── Button Manager
├── Scroll Manager
├── Double Click Detector
├── Drag Manager
└── Event Generator
```

---

# 43. Maus Pipeline

```text
Hardware

↓

Firmware

↓

Raw Mouse Data

↓

Pointer Decoder

↓

Mouse Event

↓

UI Runtime
```

---

# 44. Mouse Objekt

```c
typedef struct
{
    uint64_t id;

    int32_t x;

    int32_t y;

    bool visible;

    float sensitivity;

} nova_mouse_t;
```

---

# 45. Mouse Event

```c
typedef struct
{
    uint64_t timestamp;

    int32_t x;

    int32_t y;

    int32_t delta_x;

    int32_t delta_y;

    int32_t wheel;

    uint32_t buttons;

} nova_mouse_event_t;
```

---

# 46. Cursor

Der Cursor wird vollständig durch die Graphics Runtime dargestellt.

Unterstützt

- Standard Cursor
- Busy Cursor
- Text Cursor
- Resize Cursor (vorbereitet)
- Hand Cursor

Alle Cursor werden als SVG oder PNG dargestellt.

---

# 47. Cursor Eigenschaften

Der Cursor besitzt

- Position
- Sichtbarkeit
- Hotspot
- Animation
- Skalierung
- Theme

---

# 48. Cursor Objekt

```c
typedef struct
{
    int32_t x;

    int32_t y;

    uint16_t hotspot_x;

    uint16_t hotspot_y;

    bool visible;

} nova_cursor_t;
```

---

# 49. Zeigerbewegung

Unterstützt

- absolute Position
- relative Bewegung

Die Firmware entscheidet über die Quelle.

Die UI arbeitet ausschließlich mit absoluten Bildschirmkoordinaten.

---

# 50. Mausbeschleunigung

Version 1

Standardmäßig deaktiviert.

Vorbereitet

- lineare Beschleunigung
- adaptive Beschleunigung

---

# 51. Empfindlichkeit

Standard

```text
1.0
```

Unterstützt

```text
0.5

↓

2.0
```

Die Einstellung gilt systemweit.

---

# 52. Maustasten

Unterstützt

- Linke Taste
- Rechte Taste
- Mittlere Taste

Vorbereitet

- Zusatztasten
- Seitentasten

---

# 53. Scrollrad

Unterstützt

- Vertikales Scrollen

Vorbereitet

- Horizontales Scrollen
- High Resolution Scrolling

---

# 54. Doppelklick

Version 1 erkennt Doppelklicks.

Standard

```text
250 ms
```

Maximaler Abstand

```text
5 Pixel
```

---

# 55. Drag & Drop

Vorbereitet.

Version 1 unterstützt

- Drag Start
- Drag Move
- Drag End

Die eigentliche Dateiverarbeitung erfolgt durch die UI.

---

# 56. Maus Capture

Widgets können den Cursor temporär übernehmen.

Beispiele

- Slider
- Scrollbars
- Drag Operationen

Während des Captures erhalten ausschließlich diese Widgets Mausereignisse.

---

# 57. Hit Testing

Vor jedem Mausklick erfolgt

```text
Cursor

↓

Hit Test

↓

Widget

↓

Event
```

Das oberste sichtbare Widget erhält das Ereignis.

---

# 58. Hover

Unterstützt

- Mouse Enter
- Mouse Leave
- Hover

Hover-Ereignisse erzeugen keine Tastaturfokusänderung.

---

# 59. Mausereignisse

Unterstützt

```text
Move

Button Down

Button Up

Click

Double Click

Wheel

Enter

Leave

Hover
```

---

# 60. Cursor Begrenzung

Der Cursor wird automatisch innerhalb der Bildschirmgrenzen gehalten.

```text
0

↓

Screen Width

0

↓

Screen Height
```

Ungültige Koordinaten werden verworfen.

---

# 61. Multi-Monitor

Vorbereitet.

Version 1 verwendet ausschließlich

```text
Display 0
```

---

# 62. Cursor Animation

Unterstützt

- Fade
- Busy Rotation
- Klickanimation

Alle Animationen stammen aus dem UI Animation Framework.

---

# 63. Mouse API

```c
nova_result_t
nova_mouse_initialize(void);

void
nova_mouse_update(void);

bool
nova_mouse_get_event(
    nova_mouse_event_t* event);
```

---

# 64. Cursor API

```c
void
nova_cursor_show(void);

void
nova_cursor_hide(void);

void
nova_cursor_set_position(
    int32_t x,
    int32_t y);
```

---

# 65. Fehlerbehandlung

Mögliche Fehler

- Gerät getrennt
- ungültige Position
- Buffer Overflow
- unbekannte Taste
- Firmwarefehler

Alle Fehler werden an das Diagnosesystem gemeldet.

---

# 66. Anforderungen

Der Mouse Manager MUSS

- Cursor verwalten
- Maustasten unterstützen
- Scrollrad unterstützen
- Doppelklick erkennen
- Hit Testing vorbereiten

---

# 67. SHOULD

Der Mouse Manager SOLLTE

- Drag & Drop vorbereiten
- Cursoranimationen unterstützen
- geringe Latenz besitzen
- Touch kompatibel sein

---

# 68. MAY

Der Mouse Manager DARF

- Gaming-Mäuse
- Mehrfachtasten
- Gesten
- OEM-Erweiterungen

unterstützen.

---

# Architekturentscheidung (ADR)

Der Nova Mouse Manager trennt die **physische Maus**, den **logischen Cursor** und die **UI-Ereignisse** vollständig voneinander.

Die Hardware liefert ausschließlich Rohdaten, während der Mouse Manager daraus standardisierte Cursor- und Mausereignisse erzeugt.

Dadurch bleibt die Benutzeroberfläche unabhängig von der konkreten Hardware und kann später identisch im Kernel und Desktop verwendet werden.

---

# 69. Touch Manager

Der **Nova Touch Manager (NTM)** abstrahiert sämtliche berührungsempfindlichen Eingabegeräte.

Er vereinheitlicht

- UEFI Absolute Pointer
- HID Touchscreen
- Precision Touchpad
- zukünftige Touch-Geräte

zu einem gemeinsamen Eingabesystem.

Version 1 bereitet die Architektur vollständig vor.

---

# 70. Ziele

Der Touch Manager soll

- Touchscreens unterstützen
- Multi-Touch unterstützen
- Gesten erkennen
- Stylus vorbereiten
- Precision Touchpads unterstützen
- Touch und Maus vereinheitlichen

---

# 71. Architektur

```text
Touch Driver
↓
Touch Manager
├── Touch Decoder
├── Contact Manager
├── Gesture Engine
├── Stylus Manager
├── Palm Detection
├── Pointer Converter
└── Event Generator
```

---

# 72. Touch Pipeline

```text
Touch Hardware

↓

Firmware

↓

Raw Contact

↓

Touch Decoder

↓

Touch Event

↓

UI Runtime
```

---

# 73. Touch Device

```c
typedef struct
{
    uint64_t id;

    uint32_t max_contacts;

    bool supports_pressure;

    bool supports_hover;

    bool supports_gestures;

} nova_touch_device_t;
```

---

# 74. Touch Contact

Jeder Finger besitzt einen eigenen Contact.

```c
typedef struct
{
    uint32_t id;

    int32_t x;

    int32_t y;

    float pressure;

    bool active;

} nova_touch_contact_t;
```

---

# 75. Multi-Touch

Version 1 unterstützt

```text
Bis zu 10 Kontakte
```

Die tatsächliche Anzahl hängt von der Hardware ab.

---

# 76. Touch Ereignisse

Unterstützt

- Touch Down
- Touch Move
- Touch Up
- Cancel

Diese bilden die Grundlage sämtlicher Gesten.

---

# 77. Touch Event

```c
typedef struct
{
    uint64_t timestamp;

    uint32_t contact_id;

    int32_t x;

    int32_t y;

    float pressure;

    nova_touch_state_t state;

} nova_touch_event_t;
```

---

# 78. Touch States

```c
typedef enum
{
    NOVA_TOUCH_DOWN,

    NOVA_TOUCH_MOVE,

    NOVA_TOUCH_UP,

    NOVA_TOUCH_CANCEL

} nova_touch_state_t;
```

---

# 79. Pointer Abstraction

Touch und Maus werden intern vereinheitlicht.

```text
Mouse

↓

Pointer

↑

Touch
```

Dadurch benötigen Widgets keine gerätespezifische Behandlung.

---

# 80. Gesture Engine

Die Gesture Engine interpretiert Touchkontakte.

Version 1 erkennt

- Tap
- Double Tap
- Long Press
- Drag

Vorbereitet

- Pinch
- Rotate
- Swipe
- Zoom

---

# 81. Tap

Ein Tap entspricht einem normalen Mausklick.

```text
Finger

↓

Touch Down

↓

Touch Up

↓

Click
```

---

# 82. Double Tap

Version 1

Standardintervall

```text
250 ms
```

---

# 83. Long Press

Standard

```text
500 ms
```

Kann Kontextmenüs öffnen.

---

# 84. Drag

```text
Touch Down

↓

Move

↓

Drop
```

Wird intern als Pointer Capture behandelt.

---

# 85. Swipe

Vorbereitet

- Links
- Rechts
- Oben
- Unten

Nicht Bestandteil von Version 1.

---

# 86. Pinch

Vorbereitet

```text
Finger 1

↓

Finger 2

↓

Zoom
```

---

# 87. Rotation

Vorbereitet

```text
2 Finger

↓

Rotation
```

---

# 88. Stylus

Vorbereitet

Unterstützt zukünftig

- Druck
- Neigung
- Hover
- Radierer

---

# 89. Precision Touchpad

Vorbereitet

Unterstützt

- Gesten
- Zwei Finger Scrollen
- Drei Finger Gesten
- Vier Finger Gesten

Version 1 verwendet nur Pointer-Ereignisse.

---

# 90. Palm Rejection

Vorbereitet

Der Touch Manager kann unbeabsichtigte Berührungen ignorieren.

---

# 91. Touch Capture

Widgets können Touchkontakte übernehmen.

Beispiele

- Slider
- ScrollView
- Karten
- Zeichenflächen

---

# 92. Touch Routing

```text
Touch

↓

Hit Test

↓

Widget

↓

Pointer Capture
```

---

# 93. Touch API

```c
nova_result_t
nova_touch_initialize(void);

void
nova_touch_update(void);

bool
nova_touch_get_event(
    nova_touch_event_t* event);
```

---

# 94. Gesture API

```c
bool
nova_gesture_detect(
    nova_gesture_event_t* event);
```

---

# 95. Fehlerbehandlung

Mögliche Fehler

- ungültiger Kontakt
- verlorener Kontakt
- zu viele Kontakte
- Firmwarefehler
- ungültige Gesten

Diese Fehler dürfen die übrige Eingabeverarbeitung nicht beeinflussen.

---

# 96. Anforderungen

Der Touch Manager MUSS

- Touchkontakte verwalten
- Pointer abstrahieren
- Multi-Touch vorbereiten
- Gestenerkennung bereitstellen
- Touch Routing unterstützen

---

# 97. SHOULD

Der Touch Manager SOLLTE

- Stylus vorbereiten
- Touchpads unterstützen
- Palm Rejection vorbereiten
- geringe Latenz besitzen

---

# 98. MAY

Der Touch Manager DARF

- Handschrifterkennung vorbereiten
- Drucksensitivität
- Hover
- OEM-Gesten
- zukünftige Eingabegeräte

unterstützen.

---

# Architekturentscheidung (ADR)

Der Nova Touch Manager behandelt Touch, Maus und zukünftige Zeigegeräte als **Pointer**.

Widgets erhalten ausschließlich standardisierte Pointer-Ereignisse und besitzen keinerlei Kenntnisse über die tatsächliche Hardware.

Dadurch kann dieselbe Benutzeroberfläche unverändert mit Maus, Touchscreen, Precision Touchpad oder zukünftigen Eingabegeräten verwendet werden.

---

# 99. Event Framework

Das **Nova Input Event Framework (NIEF)** stellt die zentrale Ereignisverarbeitung des Bootloaders bereit.

Sämtliche Eingabegeräte erzeugen ausschließlich standardisierte Ereignisse.

Die Benutzeroberfläche verarbeitet niemals Hardwaredaten direkt.

---

# 100. Ziele

Das Event Framework soll

- Ereignisse vereinheitlichen
- Eingaben priorisieren
- zeitlich korrekt sortieren
- deterministisch arbeiten
- hohe Performance erreichen
- Lock-Free vorbereitet sein

---

# 101. Architektur

```text
Input Devices
↓
Input Manager
↓
Event Queue
↓
Event Dispatcher
├── Scheduler
├── Focus Router
├── Widget Router
├── Command Router
├── History
└── Diagnostics
```

---

# 102. Event Pipeline

```text
Hardware

↓

Raw Event

↓

Normalize

↓

Timestamp

↓

Queue

↓

Dispatcher

↓

Widget

↓

Application
```

Alle Ereignisse durchlaufen dieselbe Pipeline.

---

# 103. Event Queue

Alle Eingaben landen zunächst in einer zentralen Event Queue.

```text
Keyboard

↓

Mouse

↓

Touch

↓

Queue
```

Die Queue verhindert Datenverlust bei hoher Eingabefrequenz.

---

# 104. Ring Buffer

Die Queue verwendet einen Ring Buffer.

```text
Write

↓

Buffer

↓

Read

↓

Dispatcher
```

Eigenschaften

- konstante Laufzeit
- keine Speicherallokation
- deterministisch

---

# 105. Queue Objekt

```c
typedef struct
{
    nova_input_event_t* events;

    uint32_t capacity;

    uint32_t head;

    uint32_t tail;

} nova_event_queue_t;
```

---

# 106. Input Event

Alle Geräte erzeugen denselben Eventtyp.

```c
typedef struct
{
    uint64_t timestamp;

    nova_input_event_type_t type;

    uint64_t device;

    uint64_t target;

    uint32_t flags;

    void* payload;

} nova_input_event_t;
```

---

# 107. Event Typen

```c
typedef enum
{
    NOVA_EVENT_KEYBOARD,

    NOVA_EVENT_MOUSE,

    NOVA_EVENT_TOUCH,

    NOVA_EVENT_POINTER,

    NOVA_EVENT_COMMAND,

    NOVA_EVENT_SYSTEM,

    NOVA_EVENT_TIMER

} nova_input_event_type_t;
```

---

# 108. Zeitstempel

Jedes Ereignis besitzt einen monotonen Zeitstempel.

Einheit

```text
Nanosekunden
```

Quelle

```text
Nova Timer Runtime
```

---

# 109. Prioritäten

Alle Ereignisse besitzen Prioritäten.

```text
Critical

↓

Security

↓

Input

↓

Navigation

↓

Background
```

---

# 110. Scheduler

Der Scheduler verarbeitet Ereignisse entsprechend ihrer Priorität.

```text
Queue

↓

Priority Scheduler

↓

Dispatcher
```

---

# 111. Event Dispatcher

Der Dispatcher verteilt Ereignisse.

```text
Input Event

↓

Focus

↓

Widget

↓

Command

↓

Application
```

---

# 112. Event Routing

Routing erfolgt anhand

- Fokus
- Mausposition
- Capture
- Widgetstatus
- Fensterstatus

---

# 113. Pointer Capture

Ein Widget kann sämtliche Pointer-Ereignisse übernehmen.

Beispiele

- Slider
- Scrollbars
- Drag Operationen

Währenddessen erhalten andere Widgets keine Pointer-Ereignisse.

---

# 114. Keyboard Capture

Auch Tastatureingaben können exklusiv übernommen werden.

Beispiele

- PasswordBox
- TextBox
- Terminal

---

# 115. Event Bubbling

Version 1 unterstützt Bubbling.

```text
Widget

↓

Container

↓

Page

↓

Window

↓

Desktop
```

Nicht behandelte Ereignisse steigen automatisch auf.

---

# 116. Event Capturing

Vorbereitet.

```text
Desktop

↓

Window

↓

Container

↓

Widget
```

Kann später globale Filter ermöglichen.

---

# 117. Polling

Version 1 unterstützt Polling.

```text
Update

↓

Geräte

↓

Events

↓

Queue
```

---

# 118. Interrupts

Vorbereitet.

Spätere Kernelversionen können Interrupts direkt verwenden.

Die API bleibt identisch.

---

# 119. Synchronisation

Alle Eingaben werden zwischen zwei UI-Frames verarbeitet.

```text
Input

↓

Queue

↓

Update

↓

Render
```

Dadurch entstehen keine inkonsistenten Zustände.

---

# 120. Event History

Optional können Ereignisse protokolliert werden.

Beispiele

- Tastatur
- Maus
- Touch
- Commands

Diese Funktion dient ausschließlich der Diagnose.

---

# 121. Event Filter

Filter können Ereignisse verändern oder blockieren.

Beispiele

- Safe Mode
- Passwortdialog
- Sperrbildschirm
- Recovery

---

# 122. Event Replay

Vorbereitet.

Ermöglicht das Wiederholen aufgezeichneter Eingaben.

Verwendung

- Debugging
- Tests
- Regressionstests

---

# 123. Queue Overflow

Bei Überlauf

1. Diagnoseeintrag erzeugen

2. Hintergrundereignisse verwerfen

3. Kritische Ereignisse behalten

Das System darf niemals blockieren.

---

# 124. Event API

```c
nova_result_t
nova_event_post(
    const nova_input_event_t* event);

bool
nova_event_poll(
    nova_input_event_t* event);

void
nova_event_dispatch(void);
```

---

# 125. Diagnostics

Das Framework erfasst

- Queue-Auslastung
- Ereignisanzahl
- maximale Verzögerung
- verlorene Ereignisse
- Durchschnittslatenz

---

# 126. Statistik

```c
typedef struct
{
    uint64_t events_received;

    uint64_t events_processed;

    uint64_t events_dropped;

    uint64_t queue_peak;

    uint64_t average_latency;

} nova_input_statistics_t;
```

---

# 127. Fehlerbehandlung

Mögliche Fehler

- Queue Overflow
- ungültiges Ereignis
- unbekannter Gerätetyp
- Zeitstempel ungültig
- Dispatcherfehler

Alle Fehler werden an das Nova Diagnostics Framework gemeldet.

---

# 128. Anforderungen

Das Event Framework MUSS

- Ring Buffer verwenden
- Prioritäten unterstützen
- deterministisch arbeiten
- Zeitstempel verwenden
- Ereignisse vereinheitlichen

---

# 129. SHOULD

Das Event Framework SOLLTE

- Event History unterstützen
- Replay vorbereiten
- Polling unterstützen
- Interruptbetrieb vorbereiten

---

# 130. MAY

Das Event Framework DARF

- mehrere Queues
- Remote-Eingaben
- Netzwerkereignisse
- virtuelle Eingabegeräte

unterstützen.

---

# Architekturentscheidung (ADR)

Das Nova Event Framework bildet den zentralen Vermittler zwischen Hardware und Benutzeroberfläche.

Alle Eingaben werden zunächst normalisiert, zeitlich geordnet und priorisiert, bevor sie an Widgets oder Anwendungen weitergeleitet werden.

Dadurch entstehen:

- reproduzierbare Eingabeverarbeitung
- geringe Latenz
- deterministisches Verhalten
- vollständige Plattformunabhängigkeit
- einfache Erweiterbarkeit

---

# 131. Focus Framework

Das **Nova Focus Framework (NFF)** verwaltet den Eingabefokus innerhalb des gesamten Bootloaders.

Es stellt sicher, dass Tastatur-, Maus- und Touch-Eingaben jederzeit eindeutig einem Zielobjekt zugeordnet werden.

Zu keinem Zeitpunkt dürfen mehrere Widgets gleichzeitig den Tastaturfokus besitzen.

---

# 132. Ziele

Das Focus Framework soll

- eindeutigen Fokus garantieren
- Fensterfokus verwalten
- Widgetfokus verwalten
- Navigation unterstützen
- Modalität berücksichtigen
- deterministisch arbeiten

---

# 133. Architektur

```text
Input Runtime
↓
Focus Framework
├── Window Focus
├── Widget Focus
├── Pointer Focus
├── Keyboard Focus
├── Capture Manager
├── Focus Chain
└── Navigation Engine
```

---

# 134. Fokusmodell

Nova unterscheidet vier Fokusarten.

```text
Window Focus

↓

Page Focus

↓

Widget Focus

↓

Pointer Focus
```

Alle Ebenen werden unabhängig voneinander verwaltet.

---

# 135. Window Focus

Es existiert immer genau ein aktives Fenster.

Nur dieses Fenster erhält

- Tastatureingaben
- Standardkommandos
- Navigation

---

# 136. Page Focus

Innerhalb eines Fensters besitzt genau eine Seite den Fokus.

Dies vereinfacht zukünftige Mehrseitenfenster.

---

# 137. Widget Focus

Innerhalb einer Seite besitzt genau ein Widget den Tastaturfokus.

Beispiele

- Button
- TextBox
- PasswordBox
- ListView

Labels oder Bilder können standardmäßig keinen Fokus erhalten.

---

# 138. Pointer Focus

Der Pointer Focus beschreibt das Widget unter dem Mauszeiger oder Touchkontakt.

Dieser Fokus beeinflusst Hover-Effekte, jedoch nicht den Tastaturfokus.

---

# 139. Focus Objekt

```c
typedef struct
{
    uint64_t window;

    uint64_t page;

    uint64_t widget;

    uint64_t pointer;

} nova_focus_state_t;
```

---

# 140. Focus Chain

Alle fokussierbaren Widgets bilden eine Focus Chain.

```text
Button

↓

TextBox

↓

CheckBox

↓

Button

↓

ListView
```

Die Reihenfolge wird von der Layout Engine bestimmt.

---

# 141. Tab Navigation

Standard

```text
Tab

↓

Nächstes Widget

Shift+Tab

↓

Vorheriges Widget
```

Nicht sichtbare oder deaktivierte Widgets werden übersprungen.

---

# 142. Pfeilnavigation

Unterstützt

- Links
- Rechts
- Oben
- Unten

Die Navigation erfolgt anhand der geometrischen Position der Widgets.

---

# 143. Standard Fokus

Beim Öffnen einer Seite erhält automatisch das Standardwidget den Fokus.

Typische Beispiele

- "Boot starten"
- "Weiter"
- Passwortfeld

---

# 144. Fokuswechsel

Ein Fokuswechsel erfolgt durch

- Tab
- Maus
- Touch
- API
- Navigation

Der Wechsel wird zentral validiert.

---

# 145. Focus Capture

Widgets können den Fokus exklusiv übernehmen.

Beispiele

- Passwortdialog
- Terminal
- Texteingabe

Währenddessen ignoriert das Framework andere Fokusanforderungen.

---

# 146. Modaler Fokus

Ein modales Fenster besitzt immer den höchsten Fokus.

Alle Eingaben außerhalb dieses Fensters werden blockiert.

---

# 147. Pointer Capture

Ein Widget kann den Mauszeiger während einer Operation übernehmen.

Beispiele

- Slider
- ScrollView
- Drag & Drop

Der Capture endet automatisch nach Abschluss der Aktion.

---

# 148. Focus Ereignisse

Widgets erhalten

```text
OnFocus

↓

OnBlur

↓

OnActivate

↓

OnDeactivate
```

Diese Ereignisse lösen keine Zeichenoperationen direkt aus.

---

# 149. Focus Animation

Der Fokus wird visuell dargestellt durch

- Outline
- Glow
- Materialänderung

Die Animation stammt aus dem UI Animation Framework.

---

# 150. Focus Prioritäten

```text
Modal Dialog

↓

Aktives Fenster

↓

Aktive Seite

↓

Widget
```

Diese Reihenfolge ist verbindlich.

---

# 151. Active Window

Nur das aktive Fenster verarbeitet

- Tastatur
- Standardbefehle
- Navigation

Nicht aktive Fenster bleiben sichtbar, erhalten jedoch keine Eingaben.

---

# 152. Focus Lock

Während sicherheitskritischer Aktionen kann der Fokus gesperrt werden.

Beispiele

- Datenträgerformatierung
- Passwortprüfung
- Schlüsselverwaltung

---

# 153. Focus API

```c
void
nova_focus_set(
    nova_widget_t* widget);

void
nova_focus_clear(void);

nova_widget_t*
nova_focus_get(void);
```

---

# 154. Window Focus API

```c
void
nova_window_activate(
    nova_window_t* window);

nova_window_t*
nova_window_active(void);
```

---

# 155. Focus Diagnostics

Das Diagnosesystem protokolliert

- Fokuswechsel
- ungültige Fokusanforderungen
- verlorenen Fokus
- Capture-Zustände

Diese Daten unterstützen Debugging und Regressionstests.

---

# 156. Fehlerbehandlung

Mögliche Fehler

- ungültiges Widget
- Fokus auf verborgenes Widget
- Fokus auf deaktiviertes Widget
- Capture-Konflikt
- fehlendes aktives Fenster

Alle Fehler werden protokolliert und führen nicht zu undefiniertem Verhalten.

---

# 157. Anforderungen

Das Focus Framework MUSS

- genau einen Tastaturfokus besitzen
- Window Focus verwalten
- Pointer Focus verwalten
- Focus Capture unterstützen
- deterministisch arbeiten

---

# 158. SHOULD

Das Focus Framework SOLLTE

- geometrische Navigation unterstützen
- Animationen verwenden
- Diagnoseinformationen bereitstellen
- Accessibility unterstützen

---

# 159. MAY

Das Focus Framework DARF

- mehrere Fokusgruppen
- Controller-Fokus
- Sprachfokus
- Eye-Tracking-Fokus
- OEM-Erweiterungen

unterstützen.

---

# Architekturentscheidung (ADR)

Das Nova Focus Framework trennt **Window Focus**, **Page Focus**, **Widget Focus** und **Pointer Focus** vollständig voneinander.

Dadurch können zukünftige Funktionen wie Mehrfensterbetrieb, Touch-Eingaben, Gamepads oder Sprachsteuerung ergänzt werden, ohne die bestehende Fokuslogik zu verändern.

Die Benutzeroberfläche erhält dadurch ein konsistentes, vorhersehbares und deterministisches Eingabeverhalten auf allen unterstützten Plattformen.

---

# 160. Command Framework

Das **Nova Command Framework (NCF)** bildet die Brücke zwischen Benutzereingaben und der eigentlichen Programmlogik.

Widgets führen niemals direkt Aktionen aus.

Sie erzeugen ausschließlich Commands.

Diese Commands werden anschließend durch die Action Engine verarbeitet.

---

# 161. Ziele

Das Command Framework soll

- Eingaben von Logik trennen
- Wiederverwendbarkeit erhöhen
- Tastatur und Maus vereinheitlichen
- Makros vorbereiten
- Undo/Redo vorbereiten
- deterministisch arbeiten

---

# 162. Architektur

```text
Input

↓

Command Generator

↓

Command Queue

↓

Command Dispatcher

↓

Action Engine

↓

Boot Runtime
```

---

# 163. Ablauf

```text
Button

↓

Click

↓

Command

↓

Dispatcher

↓

Action

↓

Boot Funktion
```

Ein Button startet niemals direkt eine Funktion.

---

# 164. Command Objekt

```c
typedef struct
{
    uint64_t id;

    nova_command_type_t type;

    uint64_t sender;

    uint64_t target;

    void* parameter;

    uint64_t timestamp;

} nova_command_t;
```

---

# 165. Command Typen

```c
typedef enum
{
    NOVA_COMMAND_NONE,

    NOVA_COMMAND_BOOT,

    NOVA_COMMAND_RECOVERY,

    NOVA_COMMAND_SETTINGS,

    NOVA_COMMAND_OPEN,

    NOVA_COMMAND_CLOSE,

    NOVA_COMMAND_REFRESH,

    NOVA_COMMAND_CANCEL,

    NOVA_COMMAND_CONFIRM,

    NOVA_COMMAND_CUSTOM

} nova_command_type_t;
```

---

# 166. Command Queue

Alle Commands werden zunächst in einer Queue gespeichert.

```text
Widgets

↓

Queue

↓

Dispatcher

↓

Action
```

Die Queue arbeitet FIFO.

---

# 167. Dispatcher

Der Dispatcher entscheidet

- welches Modul
- welche Seite
- welches Fenster

den Command verarbeitet.

---

# 168. Action Engine

Die Action Engine setzt Commands in tatsächliche Operationen um.

Beispiele

```text
Command

↓

Boot starten
```

```text
Command

↓

Recovery öffnen
```

```text
Command

↓

Datenträger prüfen
```

---

# 169. Standard Commands

Version 1 definiert

- Boot
- Shutdown
- Restart
- Recovery
- Diagnose
- Refresh
- Save
- Cancel
- Apply
- Continue
- Back
- Exit

---

# 170. Widget Commands

Widgets erzeugen standardisierte Commands.

| Widget | Command |
|---------|---------|
| Button | Execute |
| CheckBox | Toggle |
| Switch | Toggle |
| Slider | ValueChanged |
| ListView | SelectionChanged |
| TextBox | TextChanged |

---

# 171. Navigation Commands

```text
Navigate Home

Navigate Back

Navigate Forward

Open Page

Close Page
```

---

# 172. Dialog Commands

```text
Dialog OK

Dialog Cancel

Dialog Yes

Dialog No

Dialog Retry
```

---

# 173. Sicherheitscommands

Für kritische Funktionen existieren eigene Command-Typen.

Beispiele

- Datenträger löschen
- Partition formatieren
- Bootloader installieren
- Schlüssel löschen

Diese Commands erfordern eine zusätzliche Bestätigung.

---

# 174. Shortcut Mapping

Die Shortcut Engine erzeugt Commands.

```text
F5

↓

Refresh Command
```

```text
Enter

↓

Execute Command
```

```text
Escape

↓

Cancel Command
```

---

# 175. Benutzerdefinierte Shortcuts

Vorbereitet.

Spätere Versionen können Tastenkombinationen konfigurieren.

Version 1 verwendet ausschließlich System-Shortcuts.

---

# 176. Command Parameter

Commands können Parameter transportieren.

Beispiele

```text
Partition ID

Dateiname

Pfad

Passwort

Bootoption
```

---

# 177. Command History

Optional können Commands gespeichert werden.

Verwendung

- Diagnose
- Tests
- Debugging

---

# 178. Undo / Redo

Vorbereitet.

Bootkritische Operationen sind grundsätzlich nicht rückgängig machbar.

Andere UI-Operationen können zukünftig Undo unterstützen.

---

# 179. Makros

Vorbereitet.

Mehrere Commands können zu einem Makro zusammengefasst werden.

Beispiel

```text
Recovery

↓

Datenträgerprüfung

↓

Neustart
```

---

# 180. Command API

```c
nova_result_t
nova_command_post(
    const nova_command_t* command);

bool
nova_command_poll(
    nova_command_t* command);

void
nova_command_dispatch(void);
```

---

# 181. Action API

```c
nova_result_t
nova_action_execute(
    const nova_command_t* command);
```

---

# 182. Prioritäten

Commands besitzen Prioritäten.

```text
Critical

↓

Security

↓

Navigation

↓

UI

↓

Background
```

---

# 183. Sicherheitsrichtlinien

Commands dürfen

niemals

direkt

Speicher

Hardware

Firmware

oder

Grafik

manipulieren.

Alle Zugriffe erfolgen ausschließlich über definierte Runtime-APIs.

---

# 184. Diagnose

Erfasst werden

- erzeugte Commands
- verarbeitete Commands
- fehlerhafte Commands
- Bearbeitungszeit
- Queue-Auslastung

---

# 185. Fehlerbehandlung

Mögliche Fehler

- unbekannter Command
- fehlender Handler
- ungültiger Parameter
- Queue Overflow
- Sicherheitsverletzung

Diese Fehler werden protokolliert.

---

# 186. Anforderungen

Das Command Framework MUSS

- Commands zentral verwalten
- Dispatcher besitzen
- Queue verwenden
- Prioritäten unterstützen
- Actions trennen

---

# 187. SHOULD

Das Command Framework SOLLTE

- Makros vorbereiten
- Undo vorbereiten
- History unterstützen
- geringe Latenz besitzen

---

# 188. MAY

Das Command Framework DARF

- Skripte
- Remote Commands
- Plugins
- OEM Commands
- KI-generierte Commands

unterstützen.

---

# Architekturentscheidung (ADR)

Das Nova Command Framework trennt **Benutzereingaben**, **Commands** und **Programmlogik** vollständig voneinander.

Widgets kennen ausschließlich Commands.

Die eigentliche Verarbeitung erfolgt über Dispatcher und Action Engine.

Dadurch entstehen

- lose Kopplung
- hohe Erweiterbarkeit
- bessere Testbarkeit
- deterministisches Verhalten
- identisches Verhalten auf BIOS und UEFI.

---

# 189. Device Management Framework

Das **Nova Device Management Framework (NDMF)** verwaltet sämtliche Eingabegeräte des Bootloaders.

Es erkennt Geräte automatisch, überwacht deren Status und stellt sie dem Input Framework als logische Geräte zur Verfügung.

---

# 190. Ziele

Das Device Framework soll

- Geräte automatisch erkennen
- Geräte entfernen erkennen
- Hot-Plug unterstützen
- mehrere Geräte verwalten
- Geräte priorisieren
- deterministisch arbeiten

---

# 191. Architektur

```text
Firmware
↓
Platform Layer
↓
Device Detection
↓
Device Manager
├── Keyboard Registry
├── Mouse Registry
├── Touch Registry
├── Driver Manager
├── Device Monitor
└── Diagnostics
```

---

# 192. Gerätepipeline

```text
Hardware

↓

Firmware

↓

Device Discovery

↓

Registry

↓

Driver

↓

Input Runtime
```

---

# 193. Geräte Registry

Alle Geräte werden zentral registriert.

```text
Keyboard

Mouse

Touch

Gamepad

Stylus
```

Jedes Gerät besitzt eine eindeutige ID.

---

# 194. Device Objekt

```c
typedef struct
{
    uint64_t id;

    nova_input_device_type_t type;

    char manufacturer[64];

    char product[64];

    char serial[64];

    bool connected;

    bool active;

} nova_device_t;
```

---

# 195. Gerätezustände

```text
Disconnected

↓

Detected

↓

Initializing

↓

Ready

↓

Active

↓

Disabled

↓

Removed
```

Ungültige Zustandswechsel sind nicht zulässig.

---

# 196. Device Discovery

Das Framework erkennt automatisch

- neue Geräte
- entfernte Geräte
- Statusänderungen

BIOS-Systeme unterstützen Discovery nur eingeschränkt.

UEFI-Systeme können vollständige Discovery bereitstellen.

---

# 197. Hot-Plug

Version 1 unterstützt Hot-Plug dort, wo die Firmware dies zulässt.

Unterstützt

- USB Tastatur
- USB Maus

Vorbereitet

- USB Touch
- USB HID
- Bluetooth HID

---

# 198. Mehrere Tastaturen

Mehrere Tastaturen dürfen gleichzeitig verwendet werden.

Alle Eingaben werden in dieselbe Event Queue übernommen.

Eine Priorisierung einzelner Tastaturen erfolgt nicht.

---

# 199. Mehrere Mäuse

Mehrere Mäuse können gleichzeitig angeschlossen sein.

Intern existiert jedoch genau ein logischer Cursor.

Alle Bewegungen werden zusammengeführt.

---

# 200. Mehrere Touchgeräte

Vorbereitet.

Version 1 verwendet ausschließlich das primäre Touchgerät.

---

# 201. Bluetooth

Bluetooth wird architektonisch vorbereitet.

Version 1 enthält jedoch keinen Bluetooth-Stack.

Spätere Kernelversionen können Bluetooth-Geräte über dieselbe API bereitstellen.

---

# 202. HID-Unterstützung

Vorbereitet

- USB HID
- Composite Devices
- Standard HID Reports

Gerätespezifische Erweiterungen bleiben gekapselt.

---

# 203. Device Prioritäten

Priorität

```text
Interne Tastatur

↓

USB Tastatur

↓

Virtuelle Geräte
```

Bei mehreren identischen Geräten entscheidet die Reihenfolge der Erkennung.

---

# 204. Gerätewechsel

Beim Entfernen eines Geräts

- werden offene Captures aufgehoben
- wird der Fokus überprüft
- wird ein Systemereignis erzeugt

Die UI bleibt weiterhin bedienbar.

---

# 205. Virtuelle Geräte

Vorbereitet.

Virtuelle Eingabegeräte können später

- Tests
- Simulationen
- Automatisierung

unterstützen.

---

# 206. Geräteinformationen

Für jedes Gerät werden gespeichert

- Hersteller
- Produktname
- Typ
- Firmwareinformationen (falls verfügbar)
- Fähigkeiten

---

# 207. Fähigkeiten

Ein Gerät beschreibt seine Fähigkeiten.

Beispiele

```text
Keyboard

↓

Unicode

↓

Key Repeat

↓

Media Keys
```

oder

```text
Touch

↓

Pressure

↓

Hover

↓

Multi Touch
```

---

# 208. Capability Objekt

```c
typedef struct
{
    bool supports_unicode;

    bool supports_touch;

    bool supports_pressure;

    bool supports_hover;

    bool supports_multitouch;

} nova_device_capabilities_t;
```

---

# 209. Device API

```c
nova_result_t
nova_device_initialize(void);

void
nova_device_update(void);

uint32_t
nova_device_count(void);
```

---

# 210. Geräteabfrage

```c
const nova_device_t*
nova_device_get(
    uint32_t index);
```

---

# 211. Device Events

Unterstützt

```text
Device Connected

Device Removed

Device Ready

Device Disabled

Device Error
```

Diese Ereignisse werden an die Event Queue übergeben.

---

# 212. Diagnose

Das Framework protokolliert

- angeschlossene Geräte
- getrennte Geräte
- Initialisierungsfehler
- Gerätewechsel
- Hot-Plug Ereignisse

---

# 213. Fehlerbehandlung

Mögliche Fehler

- unbekanntes Gerät
- Initialisierung fehlgeschlagen
- Gerät entfernt
- Treiber nicht verfügbar
- ungültige Fähigkeiten

Alle Fehler werden im Diagnosesystem protokolliert.

---

# 214. Anforderungen

Das Device Framework MUSS

- Geräte erkennen
- Geräte registrieren
- mehrere Geräte verwalten
- Device Events erzeugen
- Fähigkeiten verwalten

---

# 215. SHOULD

Das Device Framework SOLLTE

- Hot-Plug unterstützen
- Capability Detection besitzen
- Diagnoseinformationen bereitstellen
- virtuelle Geräte vorbereiten

---

# 216. MAY

Das Device Framework DARF

- Bluetooth
- Netzwerkgeräte
- Remote HID
- OEM-Geräte
- Emulatoren

unterstützen.

---

# Architekturentscheidung (ADR)

Das Nova Device Management Framework trennt die **Geräteverwaltung** vollständig von der eigentlichen Eingabeverarbeitung.

Die Input Runtime arbeitet ausschließlich mit registrierten logischen Geräten und kennt weder Firmware noch konkrete Hardware.

Dadurch bleiben Geräteaustausch, zukünftige Eingabegeräte und neue Bussysteme vollständig transparent für die Benutzeroberfläche.

---

# 217. Diagnostics Framework

Das **Nova Input Diagnostics Framework (NIDF)** überwacht sämtliche Komponenten des Input Frameworks.

Es dient

- der Fehleranalyse
- der Performanceüberwachung
- der Geräteüberwachung
- Regressionstests
- Debugging

Das Diagnosesystem arbeitet vollständig passiv und beeinflusst niemals die Eingabeverarbeitung.

---

# 218. Ziele

Das Diagnostics Framework soll

- Eingabelatenz messen
- Fehler erkennen
- Geräte überwachen
- Statistiken sammeln
- reproduzierbare Analysen ermöglichen
- minimale Laufzeitkosten besitzen

---

# 219. Architektur

```text
Input Runtime
↓
Diagnostics Framework
├── Logger
├── Statistics
├── Performance Monitor
├── Device Monitor
├── Event Tracer
├── Error Collector
└── Debug Interface
```

---

# 220. Diagnose Pipeline

```text
Input Event

↓

Statistics

↓

Diagnostics

↓

Logger

↓

Report
```

---

# 221. Statistik

Das Framework sammelt kontinuierlich

- Tastaturereignisse
- Mausereignisse
- Touchereignisse
- Commands
- Fokuswechsel
- Gerätewechsel

---

# 222. Statistik Objekt

```c
typedef struct
{
    uint64_t keyboard_events;

    uint64_t mouse_events;

    uint64_t touch_events;

    uint64_t commands;

    uint64_t focus_changes;

    uint64_t device_changes;

} nova_input_statistics_t;
```

---

# 223. Performance Monitoring

Folgende Werte werden überwacht

- durchschnittliche Eingabelatenz
- maximale Eingabelatenz
- Queue-Auslastung
- Dispatcher-Laufzeit
- Scheduler-Laufzeit

---

# 224. Latenz

Gemessen wird

```text
Hardware

↓

Firmware

↓

Input Runtime

↓

Event Queue

↓

Widget
```

Die gesamte Verarbeitung sollte möglichst unter **2 Millisekunden** bleiben.

---

# 225. Queue Monitoring

Erfasst werden

- aktuelle Auslastung
- maximale Auslastung
- freie Einträge
- verworfene Ereignisse

---

# 226. Geräteüberwachung

Der Device Monitor protokolliert

- Geräteinitialisierung
- Geräteentfernung
- Fehler
- Hot-Plug
- Firmwarefehler

---

# 227. Event Tracing

Optional können sämtliche Ereignisse aufgezeichnet werden.

Beispiel

```text
12:00:01.002

Keyboard

↓

Enter

↓

Button Execute
```

Diese Funktion dient ausschließlich Diagnosezwecken.

---

# 228. Command Tracing

Erfasst werden

```text
Widget

↓

Command

↓

Dispatcher

↓

Action

↓

Ergebnis
```

Dadurch können fehlerhafte Abläufe rekonstruiert werden.

---

# 229. Focus Tracing

Das Framework zeichnet auf

- Fokuswechsel
- Capture
- Dialogwechsel
- Fensteraktivierung

---

# 230. Fehlerklassifikation

Fehler werden eingeteilt in

```text
Info

↓

Warning

↓

Error

↓

Critical
```

Nur kritische Fehler dürfen den Bootvorgang beeinflussen.

---

# 231. Logging

Version 1 unterstützt

- RAM Log
- Bildschirmausgabe (Debug)
- Export zum Kernel

Vorbereitet

- Datei-Logging

---

# 232. Diagnosebericht

Der Diagnosebericht enthält

- Geräte
- Ereignisse
- Latenzen
- Fehler
- Queue-Statistik
- Runtime-Version

---

# 233. Performance Counter

```c
typedef struct
{
    uint64_t average_latency_ns;

    uint64_t maximum_latency_ns;

    uint64_t queue_peak;

    uint64_t events_processed;

} nova_input_performance_t;
```

---

# 234. Debug Interface

Version 1 stellt ein internes Debug Interface bereit.

Unterstützt

- Queue anzeigen
- Geräte anzeigen
- Fokus anzeigen
- Event Trace
- Statistiken

---

# 235. Debug Overlay

Vorbereitet.

Kann anzeigen

```text
FPS

Input

Focus

Queue

Devices
```

Dieses Overlay ist ausschließlich für Entwickler vorgesehen.

---

# 236. Selbsttest

Beim Start kann das Framework einen Selbsttest durchführen.

Geprüft werden

- Queue
- Keyboard
- Maus
- Event Dispatcher
- Scheduler

Der Selbsttest ist optional.

---

# 237. API

```c
void
nova_input_statistics(
    nova_input_statistics_t* stats);

void
nova_input_performance(
    nova_input_performance_t* performance);
```

---

# 238. Diagnose API

```c
void
nova_input_dump(void);

void
nova_input_trace_enable(bool enable);
```

---

# 239. Fehlerbehandlung

Erfasst werden

- Queue Overflow
- verlorene Ereignisse
- Gerätefehler
- Dispatcherfehler
- Synchronisationsfehler
- ungültige Commands

Alle Fehler erhalten einen eindeutigen Diagnosecode.

---

# 240. Performanceziele

| Bereich | Ziel |
|----------|------:|
| Event Queue | < 100 ns |
| Dispatcher | < 200 ns |
| Focus Routing | < 200 ns |
| Command Routing | < 300 ns |
| Gesamte Eingabelatenz | < 2 ms |

---

# 241. Anforderungen

Das Diagnostics Framework MUSS

- Statistiken sammeln
- Fehler protokollieren
- Performance messen
- Geräte überwachen
- Reports erzeugen

---

# 242. SHOULD

Das Diagnostics Framework SOLLTE

- Event Tracing unterstützen
- Debug Overlay vorbereiten
- Selbsttests besitzen
- Queue überwachen

---

# 243. MAY

Das Diagnostics Framework DARF

- Remote Debugging
- Live Performance Graphen
- externe Debug Adapter
- OEM Diagnosemodule

unterstützen.

---

# Architekturentscheidung (ADR)

Das Nova Input Diagnostics Framework arbeitet vollständig **passiv**.

Es verändert niemals die eigentliche Eingabeverarbeitung, sondern beobachtet ausschließlich den Zustand des Systems.

Dadurch können Performanceprobleme und Fehler analysiert werden, ohne das Verhalten des Input Frameworks selbst zu beeinflussen.

---

# 244. Input Security Framework

Das **Nova Input Security Framework (NISF)** schützt sämtliche sicherheitskritischen Eingaben innerhalb des Bootloaders.

Es stellt sicher, dass Passwörter, Wiederherstellungsschlüssel und andere vertrauliche Eingaben ausschließlich von autorisierten Komponenten verarbeitet werden.

---

# 245. Ziele

Das Security Framework soll

- sichere Eingaben ermöglichen
- Eingaben isolieren
- Eingaben validieren
- Spoofing verhindern
- Replay-Angriffe verhindern
- deterministisch arbeiten

---

# 246. Architektur

```text
Input Runtime
↓
Security Layer
├── Secure Input
├── Input Filter
├── Password Manager
├── Secure Focus
├── Secure Commands
├── Validation
└── Audit Log
```

---

# 247. Secure Input

Während einer sicheren Eingabe gelten besondere Regeln.

Beispiele

- Bootpasswort
- TPM PIN
- BitLocker Recovery Key
- LUKS Passwort
- Administratorpasswort

---

# 248. Secure Session

Eine sichere Eingabesitzung besitzt

```text
Begin

↓

Exclusive Focus

↓

Secure Input

↓

Validation

↓

Destroy
```

Nach Abschluss werden alle temporären Daten gelöscht.

---

# 249. Secure Focus

Während einer Secure Session

- besitzt ausschließlich das Passwortfeld den Fokus
- sind Hintergrundfenster blockiert
- werden andere Eingaben verworfen

---

# 250. Secure Widget

Nur speziell markierte Widgets dürfen vertrauliche Eingaben entgegennehmen.

```c
typedef struct
{
    bool secure;

    bool masked;

    bool protected;

} nova_secure_widget_t;
```

---

# 251. Passwortmodus

Der Passwortmodus aktiviert

- Zeichenausblendung
- Zwischenablage deaktivieren
- Screenshot-Sperre
- Ereignisfilter
- Secure Focus

---

# 252. Eingabefilter

Der Security Filter überprüft

- ungültige Zeichen
- maximale Länge
- Wiederholungen
- Manipulationen

---

# 253. Replay Protection

Wiederholte Eingaben dürfen nicht erneut verarbeitet werden.

Jede Secure Session besitzt

- eindeutige Session-ID
- Zeitstempel
- Zufallswert (Nonce)

---

# 254. Secure Command

Nach erfolgreicher Eingabe wird kein Klartext übertragen.

Stattdessen wird ein Secure Command erzeugt.

```text
Password

↓

Validation

↓

Secure Command

↓

Boot Runtime
```

---

# 255. Passwortspeicher

Passwörter werden ausschließlich im Arbeitsspeicher gehalten.

Nach Abschluss

- Überschreiben
- Speicherbereinigung
- Freigabe

Das Passwort darf niemals im Klartext persistiert werden.

---

# 256. Sichere Zeichenpuffer

```c
typedef struct
{
    uint8_t* data;

    uint32_t length;

    bool encrypted;

} nova_secure_buffer_t;
```

Die Implementierung sollte den Speicher nach der Nutzung explizit überschreiben.

---

# 257. Tastatureingaben

Während einer Secure Session

- sind Makros deaktiviert
- sind automatische Wiederholungen optional deaktivierbar
- werden globale Shortcuts ignoriert

---

# 258. Maus und Touch

Während einer Secure Session

- dürfen nur sichere Widgets angeklickt werden
- Hintergrundelemente reagieren nicht
- Pointer Capture wird aufgehoben

---

# 259. Secure Attention

Vorbereitet.

Spätere Versionen können eine Secure Attention Sequence unterstützen.

Beispiele

```text
Ctrl + Alt + Del
```

oder

firmwarespezifische Tastenkombinationen.

---

# 260. Audit Log

Das Audit Log speichert

- Beginn einer Secure Session
- Ende der Session
- erfolgreiche Authentifizierung
- fehlgeschlagene Authentifizierung

Es werden **keine geheimen Eingaben protokolliert**.

---

# 261. Sperrbildschirm

Das Security Framework unterstützt einen Sperrbildschirm.

Eigenschaften

- exklusiver Fokus
- Passwortdialog
- blockierte Hintergrundfenster
- sichere Eingaben

---

# 262. Timeout

Bei Inaktivität

```text
Timeout

↓

Session beenden

↓

Puffer löschen

↓

Dialog schließen
```

Standard

```text
60 Sekunden
```

---

# 263. Manipulationserkennung

Das Framework erkennt

- ungültige Widgets
- manipulierte Commands
- Fokusverletzungen
- doppelte Secure Sessions

---

# 264. Sicherheitsrichtlinien

Während einer Secure Session gilt

- genau ein aktives Passwortfeld
- keine parallelen Dialoge
- keine Hintergrundbefehle
- keine Navigation
- keine Seitenwechsel

---

# 265. Secure Input API

```c
nova_result_t
nova_secure_input_begin(void);

void
nova_secure_input_end(void);

bool
nova_secure_input_active(void);
```

---

# 266. Passwort API

```c
nova_result_t
nova_password_validate(
    const nova_secure_buffer_t* password);
```

---

# 267. Secure Buffer API

```c
void
nova_secure_buffer_clear(
    nova_secure_buffer_t* buffer);
```

Die Implementierung soll den Speicherbereich sicher überschreiben.

---

# 268. Fehlerbehandlung

Mögliche Fehler

- parallele Secure Session
- ungültiger Fokus
- manipuliertes Widget
- Timeout
- Validierungsfehler

Alle sicherheitsrelevanten Fehler werden im Audit Log vermerkt.

---

# 269. Anforderungen

Das Security Framework MUSS

- sichere Eingaben unterstützen
- Fokus sperren
- Passwortspeicher löschen
- Audit Log führen
- Replay-Schutz besitzen

---

# 270. SHOULD

Das Security Framework SOLLTE

- Secure Attention vorbereiten
- Screenshot-Schutz unterstützen
- Timeout verwenden
- Eingabefilter besitzen

---

# 271. MAY

Das Security Framework DARF

- TPM-Integration
- Smartcards
- FIDO2
- biometrische Authentifizierung
- OEM-Sicherheitsmodule

unterstützen.

---

# Architekturentscheidung (ADR)

Das Nova Input Security Framework kapselt sämtliche sicherheitskritischen Eingaben vollständig vom übrigen Eingabesystem.

Während einer Secure Session dürfen ausschließlich speziell gekennzeichnete Widgets Eingaben empfangen.

Alle sensiblen Daten werden ausschließlich im flüchtigen Speicher verarbeitet und nach Abschluss der Sitzung sicher gelöscht.

Dadurch entsteht eine klar definierte Vertrauensgrenze zwischen normaler Benutzerinteraktion und sicherheitskritischen Operationen.

---

# 272. Öffentliche Input API

Das Nova Boot Input Framework stellt eine stabile öffentliche API bereit.

Diese API ist die einzige zulässige Schnittstelle für sämtliche Eingabegeräte des Bootloaders.

```text
Boot Core

↓

Input API

↓

Input Runtime

↓

Device Manager

↓

Hardware
```

Direkte Zugriffe auf BIOS-, UEFI- oder Hardwaretreiber außerhalb der Input Runtime sind nicht zulässig.

---

# 273. API Designprinzipien

Die öffentliche API folgt den Grundsätzen

- ABI-stabil
- plattformneutral
- deterministisch
- dokumentiert
- modular
- erweiterbar
- versionsfähig

---

# 274. Initialisierung

```c
nova_result_t
nova_input_initialize(void);
```

Initialisiert

- Device Manager
- Keyboard Manager
- Mouse Manager
- Touch Manager
- Event Queue
- Focus Manager
- Security Layer
- Diagnostics

---

# 275. Shutdown

```c
nova_result_t
nova_input_shutdown(void);
```

Beendet sämtliche Eingabekomponenten kontrolliert.

Alle Eingabepuffer werden geleert.

---

# 276. Update

```c
void
nova_input_update(
    uint64_t delta_time);
```

Aktualisiert

- Geräte
- Ereignisse
- Fokus
- Gesten
- Queue
- Diagnostics

---

# 277. Device API

```c
uint32_t
nova_input_device_count(void);

const nova_device_t*
nova_input_device_get(
    uint32_t index);
```

---

# 278. Keyboard API

```c
bool
nova_keyboard_poll(
    nova_keyboard_event_t* event);

bool
nova_keyboard_pressed(
    nova_keycode_t key);

bool
nova_keyboard_released(
    nova_keycode_t key);
```

---

# 279. Mouse API

```c
bool
nova_mouse_poll(
    nova_mouse_event_t* event);

void
nova_mouse_position(
    int32_t* x,
    int32_t* y);

uint32_t
nova_mouse_buttons(void);
```

---

# 280. Touch API

```c
bool
nova_touch_poll(
    nova_touch_event_t* event);

uint32_t
nova_touch_contacts(void);
```

---

# 281. Pointer API

```c
void
nova_pointer_position(
    int32_t* x,
    int32_t* y);

bool
nova_pointer_available(void);
```

Der Pointer abstrahiert Maus und Touch.

---

# 282. Event API

```c
bool
nova_event_poll(
    nova_input_event_t* event);

void
nova_event_dispatch(void);

void
nova_event_flush(void);
```

---

# 283. Focus API

```c
nova_widget_t*
nova_focus_widget(void);

nova_window_t*
nova_focus_window(void);

void
nova_focus_set(
    nova_widget_t* widget);
```

---

# 284. Command API

```c
nova_result_t
nova_command_post(
    const nova_command_t* command);

void
nova_command_dispatch(void);
```

---

# 285. Secure Input API

```c
bool
nova_secure_input_active(void);

nova_result_t
nova_secure_input_begin(void);

void
nova_secure_input_end(void);
```

---

# 286. Diagnostics API

```c
void
nova_input_statistics(
    nova_input_statistics_t* statistics);

void
nova_input_dump(void);
```

---

# 287. Runtime Objekt

```c
typedef struct
{
    nova_keyboard_manager_t* keyboard;

    nova_mouse_manager_t* mouse;

    nova_touch_manager_t* touch;

    nova_device_manager_t* devices;

    nova_focus_manager_t* focus;

    nova_event_queue_t* queue;

} nova_input_runtime_t;
```

---

# 288. ABI Header

Alle öffentlichen Strukturen beginnen mit

```c
typedef struct
{
    uint64_t magic;

    uint16_t major;

    uint16_t minor;

    uint32_t size;

} nova_input_abi_header_t;
```

---

# 289. Magic

```text
"NIBI"
```

Nova Input Binary Interface

---

# 290. Versionierung

Semantic Versioning

```text
Major

↓

ABI Änderungen

Minor

↓

Neue Funktionen

Patch

↓

Fehlerkorrekturen
```

---

# 291. Fehlercodes

```c
typedef enum
{
    NOVA_INPUT_SUCCESS = 0,

    NOVA_INPUT_INVALID_DEVICE,

    NOVA_INPUT_INVALID_EVENT,

    NOVA_INPUT_INVALID_COMMAND,

    NOVA_INPUT_INVALID_FOCUS,

    NOVA_INPUT_QUEUE_OVERFLOW,

    NOVA_INPUT_OUT_OF_MEMORY,

    NOVA_INPUT_UNSUPPORTED,

    NOVA_INPUT_FATAL

} nova_input_result_t;
```

---

# 292. Referenzimplementierung

```text
input/
├── runtime/
├── devices/
├── keyboard/
├── mouse/
├── touch/
├── gestures/
├── focus/
├── commands/
├── security/
├── diagnostics/
├── api/
├── tests/
└── docs/
```

---

# 293. Unit Tests

Mindestens folgende Komponenten besitzen eigene Unit Tests

- Device Manager
- Keyboard Manager
- Mouse Manager
- Touch Manager
- Event Queue
- Focus Manager
- Command Framework
- Security Layer

---

# 294. Integrationstests

Pflichttests

- BIOS
- UEFI
- USB Tastatur
- USB Maus
- PS/2 Tastatur
- PS/2 Maus
- verschiedene Bildschirmauflösungen
- Secure Input

---

# 295. Regressionstests

Vor jedem Release werden geprüft

✔ Tastatur

✔ Maus

✔ Touch

✔ Focus

✔ Queue

✔ Dispatcher

✔ Security

✔ ABI

---

# 296. Performanceziele

| Komponente | Ziel |
|------------|------:|
| Geräteerkennung | < 5 ms |
| Event Queue | < 100 ns |
| Dispatcher | < 200 ns |
| Focus Routing | < 200 ns |
| Command Routing | < 300 ns |
| Gesamte Eingabelatenz | < 2 ms |

---

# 297. Speicherziele

| Bereich | Ziel |
|----------|------:|
| Event Queue | < 256 KB |
| Device Registry | < 128 KB |
| Keyboard Runtime | < 64 KB |
| Mouse Runtime | < 64 KB |
| Touch Runtime | < 128 KB |
| Diagnostics | < 256 KB |

---

# 298. Dokumentationsstandard

Jede öffentliche Funktion wird dokumentiert.

```c
/*
Beschreibung

Parameter

Rückgabewert

Fehler

Komplexität

Seiteneffekte

Thread Safety
*/
```

---

# 299. Anforderungen

Die öffentliche Input API MUSS

- ABI-stabil sein
- plattformneutral sein
- dokumentiert sein
- deterministisch arbeiten
- versionsfähig sein

---

# 300. SHOULD

Die API SOLLTE

- zukünftige Geräte unterstützen
- Mehrmonitorbetrieb vorbereiten
- Mehrbenutzerbetrieb vorbereiten
- Interruptbetrieb vorbereiten

---

# 301. MAY

Die API DARF

- OEM-Erweiterungen
- Plugins
- experimentelle Geräte
- Remote Input
- Emulatoren

unterstützen, sofern die öffentliche API unverändert bleibt.

---

# 302. Architekturentscheidung (ADR)

Die öffentliche Input API bildet die einzige offizielle Schnittstelle zwischen Hardware, Firmware und allen höheren Systemschichten.

Alle Geräte, Ereignisse und Eingabemethoden werden über eine gemeinsame Runtime abstrahiert.

Dadurch bleiben Bootloader, Recovery, Kernel und Desktop vollständig unabhängig von konkreten Eingabegeräten und Firmwareimplementierungen.

---

# 303. Referenzarchitektur

Das Nova Boot Input Framework bildet die vollständige Eingabeschicht des Nova Bootloaders.

Alle Eingabegeräte werden über standardisierte Runtime-Komponenten abstrahiert.

```text
                    Boot Core
                         │
                         ▼
                 Nova Input Runtime
                         │
 ┌───────────────────────┼──────────────────────────┐
 │                       │                          │
 ▼                       ▼                          ▼
Device Manager     Event Framework          Security Layer
 │                       │                          │
 ▼                       ▼                          ▼
Keyboard Manager   Focus Framework           Command Framework
 │                       │                          │
 ▼                       ▼                          ▼
Mouse Manager      Gesture Engine              Diagnostics
 │
 ▼
Touch Manager
 │
 ▼
Firmware Layer
 │
 ▼
Hardware
```

Alle Komponenten kommunizieren ausschließlich über öffentliche APIs.

---

# 304. Beziehungen zu anderen NPSPECs

Das Nova Boot Input Framework baut auf folgenden Spezifikationen auf:

- NPSPEC-BOOTPLATFORM-0001
- NPSPEC-BOOTMEMORY-0001
- NPSPEC-BOOTGRAPHICS-0001
- NPSPEC-BOOTUI-0001
- NPSPEC-BOOTSTAGE2-0001

Spätere Kernel-Komponenten übernehmen die Architektur nahezu unverändert.

---

# 305. Gesamter Datenfluss

```text
Hardware

↓

Firmware

↓

Input Driver

↓

Device Manager

↓

Event Queue

↓

Dispatcher

↓

Focus Manager

↓

Command Framework

↓

Boot UI

↓

Graphics Framework
```

Dieser Ablauf ist verbindlich.

---

# 306. Gerätefluss

```text
Gerät

↓

Discovery

↓

Registry

↓

Capabilities

↓

Runtime

↓

Events

↓

UI
```

Alle Geräte werden vor der Verwendung registriert.

---

# 307. Eingabemodell

Nova verwendet ein einheitliches Eingabemodell.

```text
Hardware

↓

Input Device

↓

Input Event

↓

Command

↓

Action
```

Die Benutzeroberfläche kennt ausschließlich Commands.

---

# 308. Pointermodell

Alle Zeigegeräte werden vereinheitlicht.

```text
Mouse

↓

Pointer

↑

Touch

↑

Stylus
```

Dadurch benötigt die UI keine gerätespezifische Logik.

---

# 309. Zustandsmodell

Alle Geräte besitzen definierte Zustände.

```text
Disconnected

↓

Detected

↓

Initializing

↓

Ready

↓

Active

↓

Disabled

↓

Removed
```

Ungültige Zustandswechsel sind unzulässig.

---

# 310. Implementierungsphasen

## Phase 1

- Device Manager
- Keyboard
- Mouse
- Event Queue

---

## Phase 2

- Focus Framework
- Command Framework
- Cursor

---

## Phase 3

- Touch
- Pointer Abstraction
- Gestenerkennung

---

## Phase 4

- Security Layer
- Secure Input
- Diagnostics

---

## Phase 5

- Hot-Plug
- Mehrere Geräte
- Performanceoptimierungen

---

## Phase 6

- Erweiterte Gesten
- Stylus
- Precision Touchpad
- OEM-Erweiterungen

---

# 311. Akzeptanzkriterien

Das Framework gilt als vollständig implementiert, wenn

✓ Tastatur funktioniert

✓ Maus funktioniert

✓ Cursor dargestellt wird

✓ Event Queue arbeitet

✓ Focus Framework korrekt arbeitet

✓ Commands erzeugt werden

✓ Secure Input funktioniert

✓ Diagnostics aktiv sind

✓ Geräte automatisch erkannt werden

✓ sämtliche APIs dokumentiert sind

---

# 312. Qualitätsziele

Das Framework verfolgt dauerhaft

- geringe Latenz
- hohe Stabilität
- deterministische Verarbeitung
- Plattformunabhängigkeit
- Erweiterbarkeit
- Wiederverwendbarkeit
- Wartbarkeit

---

# 313. Performanceziele

| Bereich | Ziel |
|----------|------:|
| Geräteinitialisierung | < 5 ms |
| Event Queue | < 100 ns |
| Dispatcher | < 200 ns |
| Focus Routing | < 200 ns |
| Command Dispatcher | < 300 ns |
| Gesamte Eingabelatenz | < 2 ms |

---

# 314. Architekturprinzipien

Das Nova Input Framework basiert dauerhaft auf folgenden Grundprinzipien.

## Device Abstraction

Alle Geräte werden vollständig abstrahiert.

---

## Event Driven

Alle Eingaben werden als Ereignisse verarbeitet.

---

## Command Driven

Die UI erzeugt ausschließlich Commands.

---

## Deterministic

Identische Eingaben erzeugen identische Ergebnisse.

---

## Platform Independent

BIOS und UEFI bleiben vollständig verborgen.

---

## Secure by Design

Sicherheitskritische Eingaben besitzen eigene Schutzmechanismen.

---

## Separation of Concerns

Jede Komponente besitzt genau eine Aufgabe.

- Device Manager → Geräte
- Keyboard Manager → Tastatur
- Mouse Manager → Maus
- Touch Manager → Touch
- Event Framework → Ereignisse
- Focus Framework → Fokus
- Command Framework → Befehle
- Security Framework → Sichere Eingaben
- Diagnostics → Überwachung

---

## API Stability

Die öffentliche API bleibt innerhalb einer Hauptversion stabil.

---

# 315. Erweiterungsmöglichkeiten

Die Architektur ist vorbereitet für

- Bluetooth HID
- Precision Touchpads
- Mehrere Touchscreens
- Gamepads
- VR-Controller
- Eye Tracking
- Sprachsteuerung
- Gestenerkennung
- Remote Input
- Netzwerkgeräte
- KI-gestützte Eingaben
- Handschrifterkennung

Diese Erweiterungen dürfen die öffentliche API nicht verändern.

---

# 316. Langfristige Vision

Das Nova Input Framework soll langfristig die einzige Eingabeschicht des gesamten Nova-Betriebssystems bilden.

```text
Bootloader

↓

Recovery

↓

Kernel

↓

Login Manager

↓

Desktop

↓

Fenstersystem

↓

Anwendungen

↓

Spiele

↓

Robotik

↓

Embedded Systeme
```

Dadurch existiert über alle Systemschichten hinweg ein einheitliches Eingabemodell.

---

# 317. Roadmap

## Version 1.x

- Tastatur
- Maus
- Event Queue
- Focus
- Secure Input

---

## Version 2.x

- Touch
- Gesten
- Precision Touchpad
- Hot-Plug

---

## Version 3.x

- Bluetooth
- Gamepads
- Stylus
- Mehrmonitorbetrieb

---

## Version 4.x

- Eye Tracking
- Sprachsteuerung
- KI-gestützte Eingaben
- Adaptive Eingabeverarbeitung

---

# 318. Zusammenfassung

Das **Nova Boot Input Framework** definiert die vollständige Eingabearchitektur des Nova Bootloaders.

Es umfasst:

- Device Manager
- Keyboard Manager
- Mouse Manager
- Touch Manager
- Pointer-Abstraktion
- Event Framework
- Focus Framework
- Command Framework
- Security Framework
- Diagnostics
- Öffentliche Input API

Alle Komponenten kommunizieren ausschließlich über definierte Schnittstellen und bilden eine konsistente Grundlage für Bootloader, Recovery, Kernel und Desktop.

---

# 319. Abschluss

Mit der Umsetzung dieser Spezifikation verfügt Nova über ein vollständig plattformunabhängiges, deterministisches und erweiterbares Eingabesystem.

Das Framework bildet die Referenzimplementierung für sämtliche Eingabegeräte innerhalb des Nova-Ökosystems und schafft die Grundlage für zukünftige Desktop-, Embedded- und Robotik-Anwendungen.
