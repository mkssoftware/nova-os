# NPSPEC-BOOTPLATFORM-0001
# Nova Boot Platform Abstraction

| Feld | Wert |
|------|------|
| Dokument | NPSPEC-BOOTPLATFORM-0001 |
| Titel | Nova Boot Platform Abstraction |
| Version | 1.0.0 |
| Status | Angenommen |
| Kategorie | Boot Platform |
| Bereich | Nova Boot Architecture |
| Übergeordnet | NPSPEC-BOOT-0001 |
| Baut auf | NPSPEC-BOOTSTAGE1-0001, NPSPEC-BOOTSTAGE2-0001 |
| Letzte Änderung | 2026-07-14 |

---

# 1. Zweck

Die **Nova Boot Platform Abstraction (BPA)** bildet die Schnittstelle zwischen der Firmware und dem eigentlichen Nova Boot Core.

Der Boot Core kennt niemals BIOS-, UEFI- oder plattformspezifischen Code.

Alle Firmwarezugriffe erfolgen ausschließlich über diese Abstraktionsschicht.

Damit wird Nova vollständig portierbar.

---

# 2. Ziele

Die Boot Platform soll

- BIOS vereinheitlichen
- UEFI vereinheitlichen
- Coreboot unterstützen
- ARM64 UEFI unterstützen
- OpenSBI unterstützen
- zukünftige Firmware unterstützen

ohne den Boot Core verändern zu müssen.

---

# 3. Grundidee

Der Bootloader besitzt zwei Ebenen.

```text
Nova Boot Core
↓
Nova Boot Platform
↓
Firmware Driver
↓
Firmware
```

Dadurch kennt der Boot Core keine Firmwaredetails.

---

# 4. Motivation

Ohne Plattformabstraktion müsste praktisch jede Bootkomponente unterscheiden zwischen

```text
if(BIOS)

...

else

UEFI
```

Dies würde den gesamten Bootloader unnötig kompliziert machen.

Die Boot Platform verhindert dies vollständig.

---

# 5. Designprinzipien

Die Plattformabstraktion folgt sieben Grundprinzipien.

## Plattformneutralität

Keine Komponente oberhalb des Platform Layers darf BIOS- oder UEFI-Funktionen direkt aufrufen.

---

## Einheitliche API

Jede Plattform implementiert dieselben Funktionen.

Dadurch bleibt der Boot Core unverändert.

---

## Erweiterbarkeit

Neue Plattformen benötigen ausschließlich einen neuen Treiber.

Der Boot Core muss nicht angepasst werden.

---

## Determinismus

Alle Plattformtreiber liefern identische Ergebnisse.

---

## Modularität

Jeder Plattformtreiber befindet sich in einem eigenen Modul.

---

## Testbarkeit

Jede Plattform kann unabhängig getestet werden.

---

## Wartbarkeit

Firmwareänderungen betreffen ausschließlich den jeweiligen Plattformtreiber.

---

# 6. Gesamtarchitektur

```text
Nova Boot Core
├── Memory
├── Graphics
├── Storage
├── Filesystem
├── Input
├── Kernel Loader
├── Security
└── Recovery

↓

Boot Platform
├── BIOS Driver
├── UEFI Driver
├── Coreboot Driver
├── ARM64 Driver
└── OpenSBI Driver

↓

Firmware
```

---

# 7. Schichtenmodell

```text
Layer 5
Boot Core

↓

Layer 4
Platform API

↓

Layer 3
Platform Driver

↓

Layer 2
Firmware Runtime

↓

Layer 1
Firmware
```

Jede Schicht besitzt klar definierte Verantwortlichkeiten.

---

# 8. Plattformtypen

```c
typedef enum
{
    NOVA_PLATFORM_UNKNOWN = 0,

    NOVA_PLATFORM_BIOS,

    NOVA_PLATFORM_UEFI,

    NOVA_PLATFORM_COREBOOT,

    NOVA_PLATFORM_ARM64_UEFI,

    NOVA_PLATFORM_OPENSBI,

    NOVA_PLATFORM_CUSTOM

} nova_platform_type_t;
```

---

# 9. Plattformobjekt

```c
typedef struct
{
    nova_platform_type_t type;

    char firmware_name[64];

    uint32_t major;

    uint32_t minor;

    uint32_t revision;

    bool secure_boot;

    bool runtime_services;

    bool graphics_supported;

    bool mouse_supported;

} nova_platform_t;
```

---

# 10. Platform Manager

Der Platform Manager ist die einzige Instanz, welche Firmwaretreiber verwaltet.

```text
Platform Manager

↓

Platform Driver

↓

Firmware
```

Er entscheidet automatisch,

welcher Treiber geladen wird.

---

# 11. Lebenszyklus

Jeder Plattformtreiber besitzt denselben Ablauf.

```text
Detect

↓

Create

↓

Initialize

↓

Running

↓

Shutdown
```

---

# 12. Plattforminitialisierung

```text
CPU

↓

Detect Firmware

↓

Create Driver

↓

Initialize Driver

↓

Boot Core
```

Danach arbeitet ausschließlich der Boot Core.

---

# 13. Anforderungen

Der Platform Layer MUSS

- automatisch BIOS erkennen
- automatisch UEFI erkennen
- Plattforminformationen bereitstellen
- Firmwarezugriffe abstrahieren
- identische APIs liefern

---

# 14. SHOULD

Der Platform Layer SOLLTE

- weitere Firmware unterstützen
- leicht portierbar sein
- modular aufgebaut sein
- reproduzierbar arbeiten

---

# 15. MAY

Der Platform Layer DARF

- Emulatoren speziell behandeln
- virtuelle Firmware optimieren
- Debugfunktionen bereitstellen

---

# 16. Verzeichnisstruktur

```text
boot/platform/
├── platform.c
├── platform.h
├── manager.c
├── manager.h
├── bios/
├── uefi/
├── coreboot/
├── arm64/
└── opensbi/
```

---

# 17. Architekturziel

Die gesamte Nova-Plattform soll so entworfen werden, dass zukünftige Firmwarearten ausschließlich durch neue Treiber ergänzt werden.

Der Boot Core bleibt dabei unverändert.

Dies ist eine der wichtigsten Designentscheidungen der gesamten Nova-Bootarchitektur.

---

# 18. Firmware Discovery

Nach dem Start von Stage 2 beginnt die automatische Erkennung der Firmware.

Der Platform Manager erkennt selbstständig,

- BIOS
- UEFI
- Coreboot
- ARM64 UEFI
- OpenSBI

ohne Konfigurationsdateien.

Der Boot Core darf niemals selbst nach Firmware suchen.

---

# 19. Plattformerkennung

```text
Stage 2

↓

Platform Manager

↓

Detect Firmware

↓

Create Platform Driver

↓

Initialize Driver

↓

Boot Core
```

Die Plattform wird genau einmal bestimmt.

---

# 20. Platform Factory

Alle Plattformtreiber werden über eine Factory erzeugt.

```c
nova_platform_driver_t*
nova_platform_create(void);
```

Die Factory entscheidet anhand der Firmware,

welcher Treiber verwendet wird.

---

# 21. Platform Registry

Alle verfügbaren Plattformen registrieren sich beim Platform Manager.

```text
Platform Registry
├── BIOS Driver
├── UEFI Driver
├── Coreboot Driver
├── ARM64 Driver
└── OpenSBI Driver
```

Neue Plattformen können registriert werden,

ohne den Boot Core zu verändern.

---

# 22. BIOS Driver

Der BIOS Driver kapselt sämtliche BIOS-Funktionen.

Unterstützt werden

- INT 10h
- INT 13h
- INT 15h
- INT 16h
- INT 19h
- E820
- VBE

Der Boot Core kennt diese Interrupts nicht.

---

# 23. BIOS Driver Aufgaben

Der BIOS Driver übernimmt

- Datenträgerzugriffe
- Grafikinitialisierung
- Speicherkarte
- Tastatur
- Zeitinformationen
- Neustart
- Firmwareinformationen

Alle Ergebnisse werden in Nova-Datenstrukturen übersetzt.

---

# 24. BIOS Driver API

```c
typedef struct
{
    nova_result_t (*initialize)(void);

    nova_result_t (*read_blocks)(
        uint64_t lba,
        uint32_t count,
        void* buffer);

    nova_result_t (*memory_map)(
        nova_memory_map_t* map);

    nova_result_t (*set_video)(
        nova_video_request_t* request);

    nova_result_t (*keyboard_poll)(
        nova_keyboard_event_t* event);

} nova_bios_driver_t;
```

---

# 25. UEFI Driver

Der UEFI Driver kapselt sämtliche UEFI-Protokolle.

Unterstützt

- Boot Services
- Runtime Services
- GOP
- Block IO
- Simple File System
- Loaded Image
- Memory Map
- ExitBootServices()

---

# 26. UEFI Driver Aufgaben

Der UEFI Driver übernimmt

- Datenträger
- Grafik
- Speicherkarte
- Firmwareinformationen
- Maus
- Tastatur
- Timer
- Runtime Services

Die UEFI-Protokolle werden vollständig verborgen.

---

# 27. UEFI Driver API

```c
typedef struct
{
    nova_result_t (*initialize)(void);

    nova_result_t (*block_read)(
        uint64_t lba,
        uint32_t count,
        void* buffer);

    nova_result_t (*graphics_initialize)(void);

    nova_result_t (*memory_map)(
        nova_memory_map_t* map);

    nova_result_t (*exit_boot_services)(void);

} nova_uefi_driver_t;
```

---

# 28. Coreboot Driver

Langfristig unterstützt Nova auch Coreboot.

Aufgaben

- CBMEM erkennen
- Coreboot Tables lesen
- Payload-Informationen übernehmen
- Speicherkarte bereitstellen

Dadurch wird Nova unabhängig von proprietären Firmwarelösungen.

---

# 29. ARM64 Driver

Für ARM64 übernimmt der Plattformtreiber

- UEFI
- Device Tree
- PSCI
- GIC
- MMU
- Framebuffer

Der Boot Core bleibt unverändert.

---

# 30. OpenSBI Driver

Für RISC-V implementiert OpenSBI dieselbe Plattform-API.

Unterstützt

- SBI Calls
- Device Tree
- Timer
- Console
- Hart Discovery

---

# 31. Platform Driver Interface

Alle Plattformtreiber implementieren dieselbe Basisschnittstelle.

```c
typedef struct
{
    nova_result_t (*initialize)(void);

    nova_result_t (*shutdown)(void);

    nova_result_t (*get_platform)(
        nova_platform_t* platform);

} nova_platform_driver_t;
```

---

# 32. Plattformfähigkeiten

Jeder Treiber beschreibt seine Fähigkeiten.

```c
typedef struct
{
    bool graphics;

    bool mouse;

    bool keyboard;

    bool storage;

    bool filesystem;

    bool runtime_services;

    bool secure_boot;

    bool tpm;

} nova_platform_capabilities_t;
```

Der Boot Core entscheidet anhand dieser Fähigkeiten,

welche Funktionen genutzt werden können.

---

# 33. Platform State

```c
typedef enum
{
    NOVA_PLATFORM_CREATED,

    NOVA_PLATFORM_INITIALIZED,

    NOVA_PLATFORM_RUNNING,

    NOVA_PLATFORM_SHUTDOWN

} nova_platform_state_t;
```

Jeder Treiber durchläuft exakt diese Zustände.

---

# 34. Driver Lifecycle

```text
Detect

↓

Create

↓

Initialize

↓

Running

↓

Shutdown

↓

Destroy
```

Fehler führen unmittelbar in den Diagnosepfad.

---

# 35. Fehlerbehandlung

Kann kein passender Treiber gefunden werden,

erfolgt

```text
Unknown Platform

↓

Recovery

↓

Fatal Error
```

Der Kernel wird in diesem Fall nicht gestartet.

---

# 36. Plattforminformationen

Der Platform Manager sammelt

- Firmwaretyp
- Firmwareversion
- Bootmodus
- Plattformname
- Hersteller
- unterstützte Funktionen

Diese Informationen werden Bestandteil von `nova_boot_info_t`.

---

# 37. Driver Registration

Neue Plattformtreiber werden registriert.

```c
nova_result_t
nova_platform_register(
    nova_platform_driver_t* driver);
```

Der Boot Core kennt nur diese Registrierung.

---

# 38. Plattformauswahl

Die Auswahl erfolgt automatisch.

```text
Detect

↓

Match Driver

↓

Initialize

↓

Verify

↓

Running
```

Benutzerinteraktionen sind nicht erforderlich.

---

# 39. Anforderungen

Die Plattformabstraktion MUSS

- BIOS unterstützen
- UEFI unterstützen
- automatisch erkennen
- identische APIs bereitstellen
- den Boot Core vollständig von Firmwaredetails trennen

---

# 40. SHOULD

Die Plattformabstraktion SOLLTE

- Coreboot unterstützen
- ARM64 vorbereiten
- OpenSBI vorbereiten
- neue Plattformen dynamisch ergänzen können

---

# 41. MAY

Die Plattformabstraktion DARF

- Emulatoren speziell behandeln
- Testplattformen integrieren
- Simulationsumgebungen unterstützen
- Diagnose-Hooks bereitstellen

---

# Architekturentscheidung (ADR)

**Wichtige Designentscheidung:**

Der **Nova Boot Core darf niemals BIOS- oder UEFI-spezifischen Code enthalten.**

Alle Firmwareaufrufe müssen ausschließlich über den Platform Layer erfolgen.

Dadurch bleibt Nova langfristig portierbar auf:

- x86 BIOS
- x86 UEFI
- ARM64 UEFI
- Coreboot
- RISC-V OpenSBI
- zukünftige Firmwareplattformen

Diese Trennung ist eine der wichtigsten Architekturentscheidungen der gesamten Nova-Bootplattform.

---

# 42. Platform Services

Der Platform Layer stellt dem Nova Boot Core eine vollständig standardisierte Laufzeitumgebung zur Verfügung.

Alle Firmwarefunktionen werden über **Platform Services** bereitgestellt.

```text
Boot Core

↓

Platform Services

↓

Platform Driver

↓

Firmware
```

Dadurch arbeitet der Boot Core ausschließlich gegen definierte Schnittstellen.

---

# 43. Servicekategorien

Die Platform Services werden in folgende Bereiche unterteilt.

```text
Platform Services
├── Memory Services
├── Storage Services
├── Graphics Services
├── Input Services
├── Timer Services
├── Console Services
├── Firmware Services
├── Configuration Services
└── Runtime Services
```

Jede Kategorie besitzt eine eigene API.

---

# 44. Service Manager

Alle Services werden durch den Platform Manager verwaltet.

```text
Platform Manager

↓

Service Manager

↓

Platform Driver

↓

Firmware
```

Der Boot Core erhält ausschließlich Referenzen auf Services.

---

# 45. Service Registry

Jeder Plattformtreiber registriert seine Services.

```text
Service Registry

↓

Memory

↓

Storage

↓

Graphics

↓

Input

↓

Timer

↓

Console
```

Neue Services können ergänzt werden, ohne bestehende Komponenten zu verändern.

---

# 46. Memory Services

Die Memory Services kapseln sämtliche firmwareabhängigen Speicherfunktionen.

Unterstützt werden

- Speicherkarte
- Speicherreservierung
- Speicherattribute
- Seitenausrichtung
- Speicherprüfung

---

# 47. Memory Service API

```c
typedef struct
{
    nova_result_t (*get_memory_map)(
        nova_memory_map_t* map);

    nova_result_t (*reserve_memory)(
        uint64_t address,
        uint64_t size);

    nova_result_t (*allocate_pages)(
        uint64_t pages,
        void** address);

} nova_memory_service_t;
```

Die Implementierung unterscheidet sich je nach Plattform, die API bleibt identisch.

---

# 48. Storage Services

Storage Services stellen den Zugriff auf Blockgeräte bereit.

Unterstützt

- Datenträger öffnen
- Block lesen
- Geräte erkennen
- Geräteinformationen

Der Boot Core kennt keine BIOS- oder UEFI-Datenträgerprotokolle.

---

# 49. Storage API

```c
typedef struct
{
    nova_result_t (*scan_devices)(void);

    nova_result_t (*read_blocks)(
        uint64_t device,
        uint64_t lba,
        uint32_t count,
        void* buffer);

    nova_result_t (*device_info)(
        uint64_t device,
        nova_storage_info_t* info);

} nova_storage_service_t;
```

---

# 50. Graphics Services

Die Graphics Services initialisieren den Grafikmodus.

Unterstützt werden

BIOS

- VBE

UEFI

- GOP

Später

- ARM Framebuffer
- RISC-V Framebuffer

Alle Plattformen liefern dieselbe Framebufferstruktur.

---

# 51. Graphics API

```c
typedef struct
{
    nova_result_t (*initialize)(
        nova_video_request_t* request);

    nova_result_t (*framebuffer)(
        nova_framebuffer_info_t* info);

    nova_result_t (*available_modes)(
        nova_video_mode_list_t* list);

} nova_graphics_service_t;
```

---

# 52. Input Services

Die Input Services vereinheitlichen sämtliche Eingabegeräte.

Unterstützt werden

- Tastatur
- Maus
- Touch
- Gamepad (optional)

Alle Ereignisse werden in ein gemeinsames Eventmodell überführt.

---

# 53. Input API

```c
typedef struct
{
    nova_result_t (*poll)(
        nova_boot_input_event_t* event);

    nova_result_t (*flush)(void);

} nova_input_service_t;
```

---

# 54. Timer Services

Zeitinformationen werden unabhängig von der Plattform bereitgestellt.

Unterstützte Quellen

- BIOS Tick Counter
- HPET
- PIT
- UEFI Timer
- ARM Generic Timer
- RISC-V Timer

---

# 55. Timer API

```c
typedef struct
{
    uint64_t (*time_ns)(void);

    uint64_t (*ticks)(void);

    void (*delay_ms)(
        uint32_t ms);

} nova_timer_service_t;
```

Alle Zeiten werden in Nanosekunden normiert.

---

# 56. Console Services

Die Console Services stellen Debugausgaben bereit.

Unterstützt

- Textmodus
- VBE-Konsole
- GOP-Konsole
- Serielle Konsole
- Virtuelle Konsole

Diese Services sind ausschließlich für Diagnosezwecke vorgesehen.

---

# 57. Console API

```c
typedef struct
{
    void (*write)(
        const char* text);

    void (*clear)(void);

    void (*set_color)(
        uint32_t fg,
        uint32_t bg);

} nova_console_service_t;
```

---

# 58. Firmware Services

Firmware Services kapseln allgemeine Firmwarefunktionen.

Unterstützt

- Firmwareinformationen
- Neustart
- Ausschalten
- Runtimeinformationen
- Firmwarestatus

---

# 59. Firmware API

```c
typedef struct
{
    nova_result_t (*firmware_info)(
        nova_platform_t* platform);

    nova_result_t (*reboot)(void);

    nova_result_t (*shutdown)(void);

} nova_firmware_service_t;
```

---

# 60. Configuration Services

Konfigurationsdaten werden über einen gemeinsamen Dienst bereitgestellt.

Unterstützt

- Bootkonfiguration
- Sprache
- Theme
- Standardkernel
- Timeout

Alle Konfigurationen werden validiert.

---

# 61. Runtime Services

Runtime Services verwalten den Zustand des Platform Layers.

Aufgaben

- Ressourcenverwaltung
- Dienstregistrierung
- Fehlerverwaltung
- Lebenszyklus
- Ereignisverteilung

Sie bilden das Rückgrat der Plattformabstraktion.

---

# 62. Ressourcenverwaltung

Alle Ressourcen werden zentral verwaltet.

```text
Allocate

↓

Use

↓

Transfer

↓

Release
```

Nach der Kernelübergabe werden nicht mehr benötigte Ressourcen freigegeben oder an den Kernel übergeben.

---

# 63. Synchronisation

Obwohl Stage 2 überwiegend sequentiell arbeitet, müssen Services reentrant ausgelegt sein.

Ziele

- keine globalen Seiteneffekte
- klar definierte Zustände
- deterministisches Verhalten

Dadurch wird eine spätere Parallelisierung erleichtert.

---

# 64. Fehlerbehandlung

Jeder Service liefert einen standardisierten Rückgabewert.

```c
typedef enum
{
    NOVA_OK = 0,

    NOVA_ERROR_NOT_SUPPORTED,

    NOVA_ERROR_NOT_FOUND,

    NOVA_ERROR_IO,

    NOVA_ERROR_INVALID_PARAMETER,

    NOVA_ERROR_OUT_OF_MEMORY,

    NOVA_ERROR_PLATFORM,

    NOVA_ERROR_FATAL

} nova_result_t;
```

Es werden keine firmwareabhängigen Fehlercodes nach außen weitergegeben.

---

# 65. Service Discovery

Der Boot Core kann verfügbare Dienste dynamisch abfragen.

```c
nova_result_t
nova_platform_get_service(
    nova_service_type_t type,
    void** service);
```

Dadurch müssen keine Plattformdetails bekannt sein.

---

# 66. Anforderungen

Die Platform Services MÜSSEN

- vollständig standardisiert sein
- plattformunabhängig arbeiten
- identische Datenstrukturen verwenden
- deterministisch arbeiten
- alle Firmwaredetails kapseln

---

# 67. SHOULD

Die Platform Services SOLLTEN

- leicht erweiterbar sein
- threadfähig vorbereitet sein
- klar dokumentiert sein
- vollständig testbar sein

---

# 68. MAY

Die Platform Services DÜRFEN

- Emulatoroptimierungen
- Performance-Counter
- Debug-Hooks
- zusätzliche Diagnoseinformationen

bereitstellen, solange die öffentliche API unverändert bleibt.

---

# Architekturentscheidung (ADR)

Die **Platform Services** bilden die einzige offizielle Schnittstelle zwischen Boot Core und Firmware.

Es ist **nicht zulässig**, BIOS-Interrupts, UEFI-Protokolle oder andere firmwareabhängige Aufrufe außerhalb des Platform Layers zu verwenden.

Diese strikte Trennung ermöglicht es, dieselbe Bootlogik künftig unverändert auf:

- x86 BIOS
- x86 UEFI
- ARM64 UEFI
- Coreboot
- RISC-V OpenSBI

einzusetzen.

---

# 69. Device Runtime

Die Device Runtime verwaltet sämtliche während des Bootvorgangs verfügbaren Geräte.

Sie stellt eine einheitliche Geräteabstraktion bereit.

Der Boot Core arbeitet ausschließlich mit der Device Runtime.

```text
Boot Core

↓

Device Runtime

↓

Platform Driver

↓

Firmware
```

---

# 70. Ziele

Die Device Runtime soll

- Geräte erkennen
- Geräte klassifizieren
- Geräte initialisieren
- Geräte verwalten
- Ereignisse bereitstellen
- Geräteinformationen sammeln

ohne firmwareabhängigen Code.

---

# 71. Gerätearchitektur

```text
Device Runtime
├── Device Manager
├── Keyboard Runtime
├── Mouse Runtime
├── Touch Runtime
├── Console Runtime
├── Event Queue
└── Device Registry
```

---

# 72. Device Manager

Der Device Manager verwaltet sämtliche Geräte.

Aufgaben

- Registrierung
- Initialisierung
- Status
- Aktivierung
- Deaktivierung
- Ereignisweiterleitung

---

# 73. Geräteobjekt

```c
typedef struct nova_device
{
    uint64_t id;

    nova_device_type_t type;

    char name[64];

    bool available;

    bool initialized;

    void* driver;

} nova_device_t;
```

---

# 74. Gerätetypen

```c
typedef enum
{
    NOVA_DEVICE_UNKNOWN,

    NOVA_DEVICE_KEYBOARD,

    NOVA_DEVICE_MOUSE,

    NOVA_DEVICE_TOUCH,

    NOVA_DEVICE_DISPLAY,

    NOVA_DEVICE_STORAGE,

    NOVA_DEVICE_TIMER,

    NOVA_DEVICE_CONSOLE

} nova_device_type_t;
```

---

# 75. Device Registry

Alle Geräte registrieren sich beim Device Manager.

```text
Registry

↓

Keyboard

↓

Mouse

↓

Display

↓

Storage

↓

Console
```

---

# 76. Keyboard Runtime

Die Keyboard Runtime vereinheitlicht sämtliche Tastatureingaben.

Unterstützt

BIOS

- INT 16h

UEFI

- Simple Text Input
- Simple Text Input Ex

Später

- USB HID
- Bluetooth

---

# 77. Keyboard Event

```c
typedef struct
{
    uint16_t scan_code;

    uint32_t key_code;

    uint32_t modifiers;

    bool pressed;

} nova_keyboard_event_t;
```

---

# 78. Key Codes

Nova verwendet ein eigenes Keycode-System.

Nicht direkt verwendet werden

- BIOS Scancodes
- UEFI Keycodes

Diese werden intern übersetzt.

---

# 79. Mouse Runtime

Die Mouse Runtime unterstützt

BIOS

- PS/2

UEFI

- Simple Pointer Protocol
- Absolute Pointer Protocol

Später

- USB HID
- Bluetooth

---

# 80. Mouse Event

```c
typedef struct
{
    int32_t x;

    int32_t y;

    int32_t wheel;

    uint32_t buttons;

} nova_mouse_event_t;
```

---

# 81. Cursor

Der Cursor wird vollständig durch die Graphics Runtime gezeichnet.

Eigenschaften

- Software Cursor
- Alpha
- Skalierung
- HiDPI
- Animation

---

# 82. Touch Runtime

Optional unterstützt Nova

- Single Touch
- Multi Touch
- Gestenerkennung

Alle Touchereignisse werden in dasselbe Eventsystem integriert.

---

# 83. Gemeinsames Eventmodell

Alle Eingaben besitzen dieselbe Struktur.

```c
typedef enum
{
    NOVA_EVENT_KEYBOARD,

    NOVA_EVENT_MOUSE,

    NOVA_EVENT_TOUCH

} nova_event_type_t;
```

---

# 84. Event Queue

Alle Ereignisse gelangen zunächst in eine zentrale Warteschlange.

```text
Device

↓

Event Queue

↓

UI

↓

Boot Core
```

Die Reihenfolge der Ereignisse bleibt erhalten.

---

# 85. Eventstruktur

```c
typedef struct
{
    uint64_t timestamp;

    nova_event_type_t type;

    union
    {
        nova_keyboard_event_t keyboard;

        nova_mouse_event_t mouse;

        nova_touch_event_t touch;

    };

} nova_boot_event_t;
```

---

# 86. Console Runtime

Die Console Runtime dient ausschließlich

- Debug
- Diagnose
- Wartung

Normale Benutzeroberflächen verwenden ausschließlich die Graphics Runtime.

---

# 87. Konsolentypen

Unterstützt

- BIOS Text Mode
- GOP Console
- VBE Console
- Serial Console
- Recovery Console

---

# 88. Console API

```c
typedef struct
{
    void (*write)(
        const char* text);

    void (*clear)(void);

    void (*set_color)(
        uint32_t foreground,
        uint32_t background);

} nova_console_runtime_t;
```

---

# 89. Device Discovery

Nach der Plattforminitialisierung beginnt automatisch die Gerätesuche.

```text
Platform

↓

Graphics

↓

Keyboard

↓

Mouse

↓

Storage

↓

Console
```

Die Reihenfolge ist verbindlich.

---

# 90. Device Capabilities

Jedes Gerät beschreibt seine Fähigkeiten.

```c
typedef struct
{
    bool input;

    bool output;

    bool removable;

    bool boot_device;

    bool hotplug;

} nova_device_capabilities_t;
```

---

# 91. Gerätezustände

```c
typedef enum
{
    NOVA_DEVICE_CREATED,

    NOVA_DEVICE_DETECTED,

    NOVA_DEVICE_INITIALIZED,

    NOVA_DEVICE_ACTIVE,

    NOVA_DEVICE_FAILED

} nova_device_state_t;
```

---

# 92. Geräteinitialisierung

```text
Detect

↓

Register

↓

Initialize

↓

Verify

↓

Activate
```

Schlägt ein Schritt fehl,

erfolgt keine Aktivierung.

---

# 93. Gerätevalidierung

Vor der Aktivierung wird geprüft

✔ Firmwareantwort gültig

✔ Speicher gültig

✔ Gerät verfügbar

✔ Initialisierung erfolgreich

✔ Ereignisse funktionsfähig

---

# 94. Geräteereignisse

Der Device Manager erzeugt

- Device Connected
- Device Ready
- Device Error
- Device Removed
- Device Failed

Diese Ereignisse können vom Boot Core ausgewertet werden.

---

# 95. Hotplug

Standardmäßig unterstützt Stage 2

keinen vollständigen Hotplug.

Ausnahmen

- USB Recovery Stick

- externe Tastatur

- externe Maus

Alle anderen Geräte werden beim Start erkannt.

---

# 96. Fehlerbehandlung

Typische Fehler

- Tastatur fehlt
- Maus fehlt
- Grafik fehlt
- Konsole nicht verfügbar
- Gerät reagiert nicht

Nicht alle Fehler verhindern den Bootvorgang.

---

# 97. Anforderungen

Die Device Runtime MUSS

- Tastatur unterstützen
- Maus unterstützen
- Event Queue besitzen
- Geräte registrieren
- Geräte validieren
- Console bereitstellen

---

# 98. SHOULD

Die Device Runtime SOLLTE

- Touch unterstützen
- HiDPI-Cursor besitzen
- USB-HID vorbereiten
- Emulatoren unterstützen
- vollständige Geräteinformationen sammeln

---

# 99. MAY

Die Device Runtime DARF

- Bluetooth
- Gamepads
- Grafiktabletts
- Fernbedienungen
- Spezialhardware

unterstützen.

---

# Architekturentscheidung (ADR)

Die Device Runtime bildet die **Hardware Abstraction Layer (HAL)** des Nova-Bootloaders.

Der Boot Core arbeitet ausschließlich mit:

- `nova_device_t`
- `nova_boot_event_t`
- `nova_device_capabilities_t`

und kennt keinerlei BIOS-, UEFI- oder hardwareabhängige Datenstrukturen.

Dadurch bleibt die gesamte Geräteverwaltung vollständig portierbar und kann unverändert auf zukünftigen Plattformen wiederverwendet werden.

---

# 100. Firmware Information Runtime

Die Firmware Information Runtime stellt sämtliche Firmwaretabellen in einer einheitlichen Form bereit.

Der Boot Core kennt weder ACPI noch SMBIOS oder Device Trees direkt.

Alle Firmwaredaten werden über standardisierte Nova-Datenstrukturen bereitgestellt.

```text
Firmware

↓

Platform Driver

↓

Firmware Runtime

↓

Boot Core
```

---

# 101. Ziele

Die Firmware Runtime soll

- ACPI erkennen
- SMBIOS erkennen
- PCI erkennen
- NUMA erkennen
- HPET erkennen
- APIC erkennen
- Device Trees erkennen
- Informationen vereinheitlichen

---

# 102. Firmware Discovery

Nach erfolgreicher Plattforminitialisierung beginnt automatisch die Suche nach Firmwaretabellen.

```text
Platform

↓

ACPI

↓

SMBIOS

↓

PCI

↓

NUMA

↓

HPET

↓

Device Tree

↓

Kernel
```

---

# 103. ACPI Runtime

Die ACPI Runtime verwaltet sämtliche ACPI-Tabellen.

Mindestens unterstützt

- RSDP
- RSDT
- XSDT
- MADT
- FADT
- HPET
- MCFG
- SRAT
- SLIT
- DSDT
- SSDT

---

# 104. ACPI Manager

```text
ACPI Runtime
├── Table Scanner
├── Parser
├── Validator
├── Registry
└── Kernel Export
```

---

# 105. ACPI API

```c
typedef struct
{
    nova_result_t (*initialize)(void);

    nova_result_t (*find_table)(
        const char* signature,
        void** table);

    nova_result_t (*validate)(void);

} nova_acpi_runtime_t;
```

---

# 106. ACPI Informationen

Gesammelt werden

- Version
- OEM
- Revision
- Tabellen
- APIC
- HPET
- NUMA
- PCI-Konfiguration

Diese Informationen werden später Bestandteil von `nova_boot_info_t`.

---

# 107. SMBIOS Runtime

Die SMBIOS Runtime liest sämtliche verfügbaren SMBIOS-Daten.

Unterstützt

- BIOS
- Mainboard
- Hersteller
- Modell
- Prozessor
- Speicher
- Chassis
- Seriennummern (optional)
- UUID (optional)

---

# 108. SMBIOS API

```c
typedef struct
{
    nova_result_t (*initialize)(void);

    nova_result_t (*system)(
        nova_smbios_info_t* info);

} nova_smbios_runtime_t;
```

---

# 109. Datenschutz

Personenbezogene Informationen dürfen nicht automatisch dauerhaft gespeichert werden.

Seriennummern und UUIDs werden nur an den Kernel weitergegeben, wenn dies durch die Sicherheitsrichtlinien erlaubt ist.

---

# 110. PCI Runtime

Die PCI Runtime erkennt sämtliche PCI-Geräte.

Unterstützt

- PCI
- PCI-X
- PCI Express

Es werden noch keine Gerätetreiber geladen.

---

# 111. PCI Discovery

```text
PCI Root

↓

Bus

↓

Device

↓

Function

↓

Device List
```

---

# 112. PCI Device

```c
typedef struct
{
    uint16_t vendor;

    uint16_t device;

    uint8_t class_code;

    uint8_t subclass;

    uint8_t interface;

    uint8_t revision;

} nova_pci_device_t;
```

---

# 113. PCI Klassen

Mindestens

- Display Controller
- Storage Controller
- USB Controller
- Network Controller
- Audio Controller
- Bridge
- Memory Controller
- Multimedia
- Communication
- System Device

---

# 114. HPET Runtime

Falls vorhanden,

wird der High Precision Event Timer erkannt.

Ermittelt werden

- Basisadresse
- Frequenz
- Anzahl Timer
- Revision

Der Kernel entscheidet später,

ob HPET verwendet wird.

---

# 115. APIC Runtime

Die APIC Runtime erkennt

- Local APIC
- IO APIC
- x2APIC

Gesammelt werden

- APIC IDs
- Prozessoranzahl
- Interrupt Controller

---

# 116. NUMA Runtime

Falls NUMA vorhanden ist,

werden erkannt

- NUMA Nodes
- Speicherbereiche
- Prozessorzuordnung
- Distanzen

Diese Informationen werden an den Kernel weitergegeben.

---

# 117. Device Tree Runtime

Auf ARM64 und RISC-V unterstützt Nova zusätzlich Device Trees.

Unterstützt

- Flattened Device Tree (FDT)
- Device Tree Blob (DTB)

Der Boot Core verarbeitet Device Trees niemals direkt.

---

# 118. Hardware Topology

Alle Hardwareinformationen werden in einer gemeinsamen Topologie zusammengeführt.

```text
System
├── CPU
├── Memory
├── PCI
├── Storage
├── Graphics
├── USB
├── Firmware
```

---

# 119. Firmware Registry

Alle Firmwareinformationen werden registriert.

```text
Registry

↓

ACPI

↓

SMBIOS

↓

PCI

↓

NUMA

↓

HPET

↓

Device Tree
```

---

# 120. Firmware Snapshot

Vor dem Kernelstart erzeugt Stage 2 einen vollständigen Snapshot der Firmwareinformationen.

Dieser Snapshot ist unveränderlich und wird Bestandteil von `nova_boot_info_t`.

---

# 121. Firmwarestruktur

```c
typedef struct
{
    nova_acpi_info_t acpi;

    nova_smbios_info_t smbios;

    nova_pci_list_t pci;

    nova_numa_info_t numa;

    nova_hpet_info_t hpet;

    nova_device_tree_t device_tree;

} nova_firmware_info_t;
```

---

# 122. Validierung

Vor der Kernelübergabe werden geprüft

✔ ACPI vollständig

✔ SMBIOS gültig

✔ PCI erfolgreich erkannt

✔ HPET gültig

✔ Firmwaretabellen konsistent

Ungültige Tabellen werden markiert, jedoch nicht verändert.

---

# 123. Übergabe

Alle Firmwareinformationen werden ausschließlich innerhalb von

```text
nova_boot_info_t
```

an den Kernel übergeben.

Es existieren keine plattformspezifischen Übergabemechanismen.

---

# 124. Anforderungen

Die Firmware Runtime MUSS

- ACPI unterstützen
- SMBIOS unterstützen
- PCI unterstützen
- Firmwaretabellen validieren
- alle Informationen vereinheitlichen

---

# 125. SHOULD

Die Firmware Runtime SOLLTE

- NUMA unterstützen
- HPET erkennen
- Device Trees unterstützen
- Topologieinformationen erzeugen
- zukünftige Firmwaretabellen leicht integrieren können

---

# 126. MAY

Die Firmware Runtime DARF

- CXL-Topologien
- ACPI 7.x
- zukünftige PCIe-Erweiterungen
- herstellerspezifische Firmwaretabellen

unterstützen, sofern diese über standardisierte Nova-Datenstrukturen bereitgestellt werden.

---

# Architekturentscheidung (ADR)

Firmwaretabellen werden im Nova-Bootloader **nicht** von einzelnen Komponenten ausgewertet.

Stattdessen erfolgt:

```text
Firmware

↓

Platform Runtime

↓

Nova Firmware Objects

↓

Boot Core

↓

Kernel
```

Dadurch bleibt der Boot Core vollständig unabhängig von ACPI-, SMBIOS- oder Device-Tree-Implementierungen.

---

# 127. Security Runtime

Die **Nova Platform Security Runtime** bildet die Sicherheitsabstraktion zwischen Firmware und Boot Core.

Sie vereinheitlicht sämtliche sicherheitsrelevanten Firmwarefunktionen.

Der Boot Core arbeitet niemals direkt mit

- Secure Boot
- TPM
- PCR
- UEFI Security Protocols
- BIOS-spezifischen Sicherheitsfunktionen

---

# 128. Ziele

Die Security Runtime soll

- Secure Boot erkennen
- TPM erkennen
- Measured Boot unterstützen
- Signaturen prüfen
- Trust Store bereitstellen
- Sicherheitsrichtlinien anwenden
- Firmware Security abstrahieren

---

# 129. Architektur

```text
Boot Core

↓

Security Runtime

↓

Platform Driver

↓

Firmware
```

---

# 130. Komponenten

```text
Security Runtime
├── Secure Boot Manager
├── TPM Manager
├── Trust Store
├── Signature Manager
├── Hash Manager
├── Policy Manager
├── Measurement Manager
└── Runtime Security
```

---

# 131. Secure Boot Manager

Der Secure Boot Manager erkennt automatisch

- aktiviert
- deaktiviert
- benutzerdefiniert
- Audit Mode

Er verändert niemals den Firmwarestatus.

---

# 132. Secure Boot API

```c
typedef struct
{
    bool enabled;

    bool audit_mode;

    bool custom_mode;

    bool user_mode;

} nova_secure_boot_info_t;
```

---

# 133. TPM Runtime

Die TPM Runtime unterstützt

- TPM 2.0
- PCR
- Random Generator
- NV Storage
- Measurements
- Attestation (optional)

---

# 134. TPM API

```c
typedef struct
{
    bool available;

    uint16_t version;

    uint32_t manufacturer;

    uint32_t firmware;

    bool owned;

} nova_tpm_info_t;
```

---

# 135. Hash Runtime

Alle kryptographischen Hashes werden zentral erzeugt.

Unterstützt

- SHA-256
- SHA-384
- SHA-512
- BLAKE3

CRC wird ausschließlich zur Fehlererkennung verwendet.

---

# 136. Signature Runtime

Unterstützte Verfahren

- RSA-3072
- RSA-4096
- ECDSA P-256
- ECDSA P-384
- Ed25519 (zukünftig)

Die verwendeten Algorithmen sind versionsabhängig konfigurierbar.

---

# 137. Nova Trust Store

Nova besitzt einen eigenen plattformunabhängigen Trust Store.

```text
Trust Store
├── Root Keys
├── Intermediate Keys
├── Signing Keys
├── Recovery Keys
└── Test Keys
```

Der Trust Store ist unabhängig von UEFI Secure Boot.

---

# 138. Policy Manager

Alle Sicherheitsrichtlinien werden zentral verwaltet.

```ini
[security]

verify_kernel=true

verify_modules=true

verify_manifest=true

allow_unsigned=false

allow_test_keys=false

allow_debug=false
```

---

# 139. Security Policies

```c
typedef struct
{
    bool verify_kernel;

    bool verify_modules;

    bool verify_manifest;

    bool require_signature;

    bool debug_allowed;

} nova_security_policy_t;
```

---

# 140. Measurement Runtime

Measured Boot erzeugt Messwerte für

- Stage 2
- Kernel
- Module
- Initramfs
- Manifest
- Konfiguration

Die Ergebnisse können in TPM-PCRs geschrieben werden.

---

# 141. PCR Mapping

Empfohlene Zuordnung

```text
PCR0 Firmware

PCR1 Stage1

PCR2 Stage2

PCR3 Manifest

PCR4 Kernel

PCR5 Initramfs

PCR6 Module

PCR7 Configuration
```

Diese Zuordnung bleibt architekturunabhängig.

---

# 142. Runtime Services

Unter UEFI werden Runtime Services bis `ExitBootServices()` unterstützt.

Unter BIOS existieren keine vergleichbaren Runtime Services.

Der Platform Layer stellt beiden Plattformen dieselbe API bereit.

---

# 143. ExitBootServices()

Vor der Übergabe an den Kernel muss unter UEFI zwingend

```text
ExitBootServices()
```

erfolgreich ausgeführt werden.

Ablauf:

```text
GetMemoryMap()

↓

Synchronisation

↓

ExitBootServices()

↓

Firmware deaktiviert

↓

Kernel
```

Nach erfolgreichem Aufruf dürfen keine UEFI-Boot-Services mehr verwendet werden.

---

# 144. BIOS-Übergang

Unter BIOS endet der Plattformkontext mit dem letzten BIOS-Aufruf.

Danach übernimmt ausschließlich der Nova Kernel.

Es werden keine BIOS-Interrupts mehr verwendet.

---

# 145. Chain of Trust

```text
Firmware

↓

Stage 1

↓

Stage 2

↓

Manifest

↓

Kernel

↓

Module

↓

Userspace
```

Jede Ebene validiert die Integrität der nachfolgenden Ebene.

---

# 146. Sicherheitsstatus

Vor dem Kernelstart wird ein gemeinsamer Sicherheitsstatus erzeugt.

```c
typedef struct
{
    bool secure_boot;

    bool tpm;

    bool measured_boot;

    bool trusted_kernel;

    bool trusted_modules;

    bool trusted_manifest;

} nova_platform_security_state_t;
```

---

# 147. Firmware Security Events

Alle sicherheitsrelevanten Ereignisse werden protokolliert.

Beispiele

- Secure Boot deaktiviert
- TPM nicht verfügbar
- PCR erweitert
- Signatur ungültig
- Manifest beschädigt
- Testschlüssel verwendet

---

# 148. Recovery Security

Die Recovery-Umgebung besitzt eigene Sicherheitsregeln.

Sie darf

- Kernel austauschen
- Konfiguration reparieren
- Bootloader aktualisieren

jedoch ausschließlich nach erfolgreicher Authentifizierung.

---

# 149. Kryptographische Bibliothek

Die Security Runtime verwendet eine gemeinsame NovaCrypto-Bibliothek.

Diese stellt bereit

- Hashes
- Signaturen
- Zufallszahlen
- Zertifikate
- Schlüsselspeicher

Alle Plattformen verwenden dieselbe Implementierung.

---

# 150. Anforderungen

Die Security Runtime MUSS

- Secure Boot erkennen
- TPM erkennen
- Trust Store bereitstellen
- Signaturen prüfen
- Richtlinien anwenden
- Sicherheitsstatus erzeugen

---

# 151. SHOULD

Die Security Runtime SOLLTE

- Measured Boot unterstützen
- PCRs verwalten
- mehrere Signaturalgorithmen unterstützen
- Recovery absichern
- zukünftige Sicherheitsstandards integrieren können

---

# 152. MAY

Die Security Runtime DARF

- Remote Attestation
- HSM
- FIDO2
- Smartcards
- Netzwerkbasierte Vertrauensdienste

unterstützen, sofern diese über dieselbe öffentliche API eingebunden werden.

---

# Architekturentscheidung (ADR)

Die Nova Platform Security Runtime stellt eine **plattformunabhängige Sicherheitsarchitektur** bereit.

Der Boot Core kennt weder

- Secure Boot,
- TPM-Befehle,
- UEFI-Sicherheitsprotokolle,
- BIOS-Erweiterungen

noch andere firmwareabhängige Mechanismen.

Alle Sicherheitsfunktionen werden ausschließlich über standardisierte Nova-Datenstrukturen und APIs bereitgestellt.

Dadurch bleibt die Sicherheitsarchitektur konsistent auf:

- x86 BIOS
- x86 UEFI
- ARM64 UEFI
- Coreboot
- RISC-V OpenSBI

und zukünftigen Plattformen.

---

# 153. Öffentliche Platform API

Der Platform Layer stellt dem Nova Boot Core eine vollständig standardisierte API bereit.

Diese API ist die einzige zulässige Kommunikationsschnittstelle zwischen Boot Core und Plattformtreibern.

```text
Boot Core

↓

Platform API

↓

Platform Driver

↓

Firmware
```

Direkte Firmwarezugriffe außerhalb dieser API sind unzulässig.

---

# 154. API Designprinzipien

Die Platform API folgt den Grundsätzen

- stabil
- deterministisch
- threadfähig vorbereitet
- modular
- versionsfähig
- rückwärtskompatibel

Alle Änderungen erfolgen ausschließlich über neue API-Versionen.

---

# 155. Platform Runtime

```c
typedef struct
{
    nova_platform_t platform;

    nova_platform_capabilities_t capabilities;

    nova_platform_state_t state;

    nova_result_t (*initialize)(void);

    nova_result_t (*shutdown)(void);

} nova_platform_runtime_t;
```

---

# 156. Service Locator

Alle Dienste werden über einen Service Locator bereitgestellt.

```c
nova_result_t
nova_platform_get_service(
    nova_service_type_t type,
    void **service);
```

Dadurch muss keine Komponente konkrete Implementierungen kennen.

---

# 157. Service Typen

```c
typedef enum
{
    NOVA_SERVICE_MEMORY,

    NOVA_SERVICE_STORAGE,

    NOVA_SERVICE_GRAPHICS,

    NOVA_SERVICE_INPUT,

    NOVA_SERVICE_TIMER,

    NOVA_SERVICE_CONSOLE,

    NOVA_SERVICE_SECURITY,

    NOVA_SERVICE_FIRMWARE,

    NOVA_SERVICE_CONFIGURATION

} nova_service_type_t;
```

---

# 158. Platform ABI

Die Platform ABI definiert sämtliche Datenstrukturen,

welche zwischen

- Platform Layer
- Boot Core
- Kernel

ausgetauscht werden.

Alle ABI-Strukturen besitzen

- Magic
- Version
- Größe

---

# 159. ABI Header

```c
typedef struct
{
    uint64_t magic;

    uint16_t major;

    uint16_t minor;

    uint32_t size;

} nova_abi_header_t;
```

Alle exportierten Strukturen beginnen mit diesem Header.

---

# 160. Versionsverwaltung

Die ABI folgt Semantic Versioning.

```text
Major

↓

Inkompatible Änderungen

Minor

↓

Neue Funktionen

Patch

↓

Fehlerkorrekturen
```

---

# 161. Capability System

Jede Plattform beschreibt ihre Fähigkeiten.

```c
typedef struct
{
    bool graphics;

    bool storage;

    bool filesystem;

    bool keyboard;

    bool mouse;

    bool touch;

    bool secure_boot;

    bool tpm;

    bool runtime_services;

    bool acpi;

    bool smbios;

    bool pci;

} nova_platform_capabilities_t;
```

---

# 162. Feature Query

```c
bool
nova_platform_has_feature(
    nova_platform_feature_t feature);
```

Der Boot Core verwendet ausschließlich diese Funktion.

---

# 163. Plattformmerkmale

```c
typedef enum
{
    NOVA_FEATURE_VBE,

    NOVA_FEATURE_GOP,

    NOVA_FEATURE_ACPI,

    NOVA_FEATURE_SMBIOS,

    NOVA_FEATURE_TPM,

    NOVA_FEATURE_SECUREBOOT,

    NOVA_FEATURE_MOUSE,

    NOVA_FEATURE_TOUCH,

    NOVA_FEATURE_RUNTIME

} nova_platform_feature_t;
```

---

# 164. Fehlerbehandlung

Alle Plattformfehler besitzen denselben Aufbau.

```c
typedef struct
{
    uint32_t code;

    uint32_t component;

    uint32_t severity;

    char description[256];

} nova_platform_error_t;
```

---

# 165. Fehlerklassen

```text
INFO

↓

WARNING

↓

RECOVERABLE

↓

FATAL
```

Nur RECOVERABLE-Fehler dürfen automatisch behandelt werden.

---

# 166. Fehlercodes

Empfohlene Bereiche

```text
1000 Firmware

2000 Platform

3000 Memory

4000 Storage

5000 Graphics

6000 Input

7000 Security

8000 Runtime

9000 Unknown
```

---

# 167. Platform Logger

Der Platform Layer besitzt einen eigenen Logger.

Er protokolliert

- Firmware
- Plattform
- Services
- Initialisierung
- Fehler
- Warnungen

Alle Einträge werden später in das zentrale Bootlog übernommen.

---

# 168. Diagnoseschnittstelle

```c
nova_result_t
nova_platform_diagnostics(
    nova_platform_report_t *report);
```

Der Bericht enthält sämtliche Plattforminformationen.

---

# 169. Plattformbericht

```c
typedef struct
{
    nova_platform_t platform;

    nova_platform_capabilities_t capabilities;

    nova_platform_security_state_t security;

    uint64_t boot_time;

} nova_platform_report_t;
```

---

# 170. Referenzimplementierung

Empfohlene Struktur

```text
platform/
├── api/
├── manager/
├── runtime/
├── services/
├── bios/
├── uefi/
├── coreboot/
├── arm64/
├── opensbi/
└── tests/
```

---

# 171. Unit Tests

Jeder Plattformtreiber muss Unit Tests besitzen.

Mindestens

- Initialisierung
- Speicher
- Grafik
- Datenträger
- Fehlerfälle
- Recovery
- Security

---

# 172. Integrationstests

Jede Plattform wird getestet auf

- QEMU

- Bochs

- VirtualBox

- VMware

- Hyper-V

- reale Hardware

---

# 173. Regressionstests

Vor jedem Release werden geprüft

✔ ABI

✔ API

✔ Plattformerkennung

✔ Speicherkarte

✔ Grafik

✔ Datenträger

✔ Firmwaretabellen

✔ Sicherheitsstatus


---

# 174. Performance

Der Platform Layer soll

- möglichst wenige Firmwareaufrufe erzeugen

- Daten zwischenspeichern

- unnötige Initialisierungen vermeiden

- Ressourcen effizient verwalten

---

# 175. Dokumentationsstandard

Jeder öffentliche API-Aufruf wird dokumentiert.

Mindestens

```c
/*
 Zweck

 Parameter

 Rückgabewert

 Fehler

 Seiteneffekte

 Thread Safety

 Bemerkungen
*/
```

---

# 176. Kompatibilität

Die Platform API garantiert

- ABI-Kompatibilität

- API-Kompatibilität

- Datenstrukturkompatibilität

zwischen allen Nova-Versionen derselben Hauptversion.

---

# 177. Anforderungen

Die Platform API MUSS

- vollständig dokumentiert sein

- versioniert werden

- ABI-stabil sein

- testbar sein

- deterministisch arbeiten

---

# 178. SHOULD

Die Platform API SOLLTE

- zukünftige Plattformen unterstützen

- dynamisch erweitert werden können

- leicht portierbar bleiben

---

# 179. MAY

Die Platform API DARF

- experimentelle Erweiterungen

- Debug-Schnittstellen

- Emulatorfunktionen

bereitstellen.

Diese dürfen jedoch niemals Bestandteil der stabilen ABI werden.

---

# Architekturentscheidung (ADR)

Die Platform API ist eine der stabilsten Schnittstellen des gesamten Nova-Systems.

Änderungen an dieser API wirken sich unmittelbar auf

- den Bootloader,
- den Kernel,
- Diagnosewerkzeuge,
- Recovery,
- Tests

aus.

Daher sind inkompatible Änderungen ausschließlich mit einer neuen Major-Version zulässig.

---

# 180. Referenzimplementierung

Die Referenzimplementierung beschreibt den empfohlenen Aufbau der gesamten Plattformabstraktion.

Jede unterstützte Plattform implementiert dieselben öffentlichen Schnittstellen.

```text
Nova Boot Core

↓

Platform API

↓

Platform Runtime

↓

Platform Driver

↓

Firmware
```

Dadurch bleibt der Boot Core unabhängig von der zugrunde liegenden Firmware.

---

# 181. Referenzprojektstruktur

```text
boot/
└── platform/
    ├── api/
    │   ├── platform.h
    │   ├── services.h
    │   ├── security.h
    │   ├── firmware.h
    │   └── runtime.h
    │
    ├── core/
    │   ├── manager.c
    │   ├── registry.c
    │   ├── runtime.c
    │   ├── services.c
    │   └── diagnostics.c
    │
    ├── bios/
    │   ├── bios.c
    │   ├── bios_disk.c
    │   ├── bios_video.c
    │   ├── bios_keyboard.c
    │   └── bios_memory.c
    │
    ├── uefi/
    │   ├── uefi.c
    │   ├── gop.c
    │   ├── blockio.c
    │   ├── filesystem.c
    │   └── runtime.c
    │
    ├── coreboot/
    ├── arm64/
    ├── opensbi/
    │
    ├── tests/
    └── docs/
```

Diese Struktur dient als Referenz für alle Nova-Bootplattformen.

---

# 182. Initialisierungsablauf

Der Platform Manager folgt einem festen Ablauf.

```text
Boot Start

↓

Detect Firmware

↓

Create Driver

↓

Register Services

↓

Initialize Services

↓

Validate

↓

Ready

↓

Boot Core
```

Dieser Ablauf ist für alle Plattformen identisch.

---

# 183. Plattformwechsel

Der Boot Core darf niemals erkennen,

welcher Plattformtreiber aktiv ist.

Beispiel

```text
BIOS

↓

Platform API

↓

Boot Core
```

und

```text
UEFI

↓

Platform API

↓

Boot Core
```

führen exakt zum gleichen Verhalten.

---

# 184. Kompatibilitätsmatrix

| Plattform | Status |
|------------|--------|
| BIOS Legacy | Pflicht |
| UEFI x64 | Pflicht |
| UEFI IA32 | Optional |
| ARM64 UEFI | Geplant |
| Coreboot | Geplant |
| OpenSBI | Geplant |
| Emulatoren | Pflicht |

---

# 185. Unterstützte Emulatoren

Die Plattform muss mindestens getestet werden auf

- QEMU
- Bochs
- VirtualBox
- VMware
- Hyper-V

Zusätzlich soll regelmäßig auf echter Hardware getestet werden.

---

# 186. Qualitätsanforderungen

Der Platform Layer MUSS

✔ deterministisch sein

✔ vollständig dokumentiert sein

✔ modular aufgebaut sein

✔ reproduzierbare Builds erzeugen

✔ ABI-stabil bleiben

✔ plattformneutral arbeiten

✔ keine Firmwaredetails nach außen geben

---

# 187. Performanceziele

Die Plattformabstraktion soll

- Initialisierung < 50 ms
- minimale Firmwareaufrufe
- keine unnötigen Speicherkopien
- keine blockierenden Warteschleifen
- deterministische Antwortzeiten

erreichen.

---

# 188. Fehlertoleranz

Bei Fehlern gilt folgende Reihenfolge.

```text
Retry

↓

Fallback

↓

Recovery

↓

Fatal Error
```

Der Platform Layer darf niemals undefiniertes Verhalten erzeugen.

---

# 189. Erweiterbarkeit

Neue Plattformen werden ausschließlich durch neue Treiber ergänzt.

Beispiel

```text
Platform Driver

↓

LoongArch

↓

Boot Core

keine Änderung
```

```text
Platform Driver

↓

PowerPC

↓

Boot Core

keine Änderung
```

---

# 190. Beziehungen zu anderen NPSPECs

Diese Spezifikation bildet die Grundlage für

- NPSPEC-BOOTSTAGE2-0001
- NPSPEC-BOOTMEMORY-0001
- NPSPEC-BOOTGRAPHICS-0001
- NPSPEC-BOOTINPUT-0001
- NPSPEC-BOOTFS-0001
- NPSPEC-KERNELLOADER-0001
- NPSPEC-BOOTHANDOFF-0001
- NPSPEC-BOOTSECURITY-0001
- NPSPEC-BOOTRECOVERY-0001

Alle diese Dokumente verwenden ausschließlich die hier definierten Platform Services.

---

# 191. Implementierungsphasen

## Phase 1

- BIOS Driver
- Platform Manager
- Memory Services
- Storage Services

---

## Phase 2

- UEFI Driver
- GOP
- Runtime Services
- ExitBootServices()

---

## Phase 3

- Security Runtime
- TPM
- Secure Boot
- Trust Store

---

## Phase 4

- Coreboot
- ARM64
- OpenSBI

---

## Phase 5

- Optimierungen
- Diagnose
- Performance
- Regressionstests

---

# 192. Akzeptanzkriterien

Die Spezifikation gilt als umgesetzt, wenn

1. BIOS automatisch erkannt wird.
2. UEFI automatisch erkannt wird.
3. Alle Platform Services verfügbar sind.
4. Boot Core keine Firmwaredetails kennt.
5. Sämtliche Firmwarezugriffe ausschließlich über den Platform Layer erfolgen.
6. ACPI, SMBIOS und PCI erfolgreich erkannt werden.
7. VBE und GOP über dieselbe API bereitgestellt werden.
8. Einheitliche Fehlercodes verwendet werden.
9. Die ABI versioniert ist.
10. Die Plattform auf virtueller und realer Hardware erfolgreich getestet wurde.

---

# 193. Langfristige Roadmap

Version 1.x

- BIOS
- UEFI
- ACPI
- GOP
- TPM

Version 2.x

- ARM64
- Coreboot
- Device Tree
- Erweiterte Sicherheitsfunktionen

Version 3.x

- RISC-V
- OpenSBI
- Mehrplattform-Builds
- Verteilte Bootdiagnose

Version 4.x

- Neue Firmwarestandards
- CXL-Unterstützung
- Erweiterte Hardwaretopologie
- Hotplug-Unterstützung im Bootloader

---

# 194. Zusammenfassung

Die **Nova Boot Platform Abstraction** trennt den Boot Core vollständig von der Firmware.

Dadurch entstehen folgende Vorteile:

- Portierbarkeit
- Wartbarkeit
- Testbarkeit
- Erweiterbarkeit
- Wiederverwendbarkeit
- Plattformunabhängigkeit

Alle zukünftigen Plattformen können durch die Implementierung eines neuen Platform Drivers integriert werden, ohne dass Änderungen am Boot Core erforderlich sind.

---

# 195. Langfristige Vision

Die Platform Abstraction ist nicht nur eine technische Hilfsschicht, sondern eine Kernkomponente der Nova-Architektur.

Sie verfolgt das Ziel, dass Nova Boot Core, Kernel und darüberliegende Systemkomponenten unabhängig von der zugrunde liegenden Firmware oder Prozessorarchitektur arbeiten können.

Langfristig soll dieselbe Plattform-API auf folgenden Architekturen verfügbar sein:

- x86 (BIOS und UEFI)
- ARM64
- RISC-V
- LoongArch
- PowerPC
- zukünftige Plattformen

Dadurch entsteht eine stabile, langlebige Grundlage, auf der Nova OS über viele Hardwaregenerationen hinweg weiterentwickelt werden kann.

---

# 196. Architekturprinzipien

Die Entwicklung der Platform Abstraction folgt dauerhaft diesen Grundsätzen:

- **API vor Implementierung** – Alle Funktionen werden zuerst spezifiziert, anschließend implementiert.
- **Abstraktion statt Spezialfälle** – Plattformabhängigkeiten werden gekapselt, nicht im Boot Core verteilt.
- **Determinismus** – Identische Eingaben erzeugen identische Ergebnisse.
- **Stabile ABI** – Änderungen erfolgen nur kontrolliert und versioniert.
- **Modularität** – Neue Plattformen werden durch Erweiterung, nicht durch Umbau integriert.
- **Testbarkeit** – Jede Plattformkomponente muss isoliert geprüft werden können.

Diese Prinzipien gelten für alle zukünftigen Erweiterungen der Nova Boot Platform.