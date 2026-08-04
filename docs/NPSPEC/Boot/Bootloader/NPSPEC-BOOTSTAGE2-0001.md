### **NPSPEC-BOOTSTAGE2-0001**
# Nova Boot Stage 2 Framework

| Feld | Wert |
|------|------|
| Dokument | NPSPEC-BOOTSTAGE2-0001 |
| Titel | Nova Boot Stage 2 Framework |
| Version | 1.0.0 |
| Status | Angenommen |
| Kategorie | Boot Specification |
| Bereich | Nova Boot Platform |
| Übergeordnet | NPSPEC-BOOT-0001 |
| Baut auf | NPSPEC-BOOTSTAGE1-0001 |
| Letzte Änderung | 2026-07-14 |

---

# 1. Zweck

Der **Nova Stage-2-Bootloader** bildet den eigentlichen Kern der Nova Boot Platform.

Während Stage 1 ausschließlich das Laden von Stage 2 übernimmt, stellt Stage 2 eine vollständige Laufzeitumgebung bereit, die den Übergang zwischen Firmware und Kernel organisiert.

Stage 2 übernimmt die komplette Hardware- und Systeminitialisierung, die für den Kernelstart erforderlich ist.

Er bildet damit die erste "Mini-Betriebssystemschicht" von Nova.

---

# 2. Ziele

Stage 2 soll

- BIOS und UEFI vereinheitlichen
- Hardware erkennen
- Speicher analysieren
- Grafik initialisieren
- Eingabegeräte initialisieren
- Bootmenü anzeigen
- Kernel laden
- Module laden
- Recovery bereitstellen
- Diagnose ermöglichen
- Selbstheilung unterstützen
- Bootinformationen erzeugen
- den Kernel sicher starten

---

# 3. Designphilosophie

Stage 2 folgt sechs Grundprinzipien.

## Modularität

Alle Funktionen werden als eigenständige Komponenten implementiert.

Beispielsweise:

```text
Boot Core

├── Graphics

├── Storage

├── Filesystem

├── Security

├── Recovery

├── Kernel Loader

├── Diagnostics

└── UI
```

Jede Komponente kann unabhängig getestet werden.

---

## Plattformunabhängigkeit

Der eigentliche Boot Core kennt keine BIOS- oder UEFI-spezifischen Details.

Alle Firmwarefunktionen werden über eine Plattformabstraktion bereitgestellt.

---

## Determinismus

Unter identischen Voraussetzungen muss Stage 2 immer denselben Ablauf besitzen.

Es dürfen keine zufälligen Entscheidungen getroffen werden.

---

## Erweiterbarkeit

Neue Funktionen werden ausschließlich als Module ergänzt.

Der bestehende Boot Core bleibt möglichst unverändert.

---

## Sicherheit

Alle geladenen Daten werden vor der Nutzung validiert.

Ungültige oder beschädigte Komponenten dürfen niemals an den Kernel übergeben werden.

---

## Diagnosefähigkeit

Jeder Schritt des Bootvorgangs muss nachvollziehbar sein.

Fehler werden protokolliert und können im Recovery-System ausgewertet werden.

---

# 4. Verantwortlichkeiten

Stage 2 ist verantwortlich für

✔ Plattforminitialisierung

✔ Speichererkennung

✔ Grafik

✔ Tastatur

✔ Maus

✔ Datenträger

✔ Dateisystem

✔ Bootkonfiguration

✔ Kernel Loader

✔ Modul Loader

✔ Initramfs

✔ ACPI

✔ SMBIOS

✔ Recovery

✔ Diagnose

✔ Logging

✔ Bootinformationen

✔ Übergabe an den Kernel

---

# 5. Nicht-Aufgaben

Stage 2 ist **nicht** verantwortlich für

- Prozessverwaltung
- Scheduler
- Virtuellen Speicher des Kernels
- Gerätetreiber des Betriebssystems
- Benutzerverwaltung
- Desktop
- Netzwerkdienste
- Anwendungen

Diese Aufgaben beginnen erst nach dem Kernelstart.

---

# 6. Gesamtarchitektur

```text
Firmware

↓

Stage 1

↓

Stage 2

├── Platform Layer

├── Boot Core

├── Graphics

├── Storage

├── Filesystem

├── Configuration

├── Security

├── Recovery

├── Diagnostics

├── Kernel Loader

├── Module Loader

└── Handoff Builder

↓

Kernel
```

Stage 2 bildet die vollständige Vermittlungsschicht zwischen Firmware und Betriebssystem.

---

# 7. Boot Core

Der Boot Core koordiniert sämtliche Komponenten.

```text
Boot Core

├── Scheduler

├── Event Dispatcher

├── Module Manager

├── Progress Manager

├── Logger

├── Diagnostics

└── Boot Context
```

Er besitzt keine Firmwareabhängigkeiten.

---

# 8. Boot Context

Alle Bootinformationen werden in einem gemeinsamen Kontext gesammelt.

```c
typedef struct nova_boot_context
{
    nova_platform_t platform;

    nova_memory_map_t memory;

    nova_storage_list_t storage;

    nova_filesystem_list_t filesystems;

    nova_framebuffer_info_t framebuffer;

    nova_input_state_t input;

    nova_boot_configuration_t configuration;

    nova_kernel_image_t kernel;

    nova_module_list_t modules;

    nova_boot_security_state_t security;

    nova_boot_log_t log;

} nova_boot_context_t;
```

Dieser Kontext wird während des gesamten Stage-2-Lebenszyklus erweitert.

---

# 9. Lebenszyklus

Der Bootloader arbeitet nach einem festen Zustandsmodell.

```text
Start

↓

Initialize Platform

↓

Detect Hardware

↓

Initialize Graphics

↓

Load Configuration

↓

Load Kernel

↓

Load Modules

↓

Security Verification

↓

Prepare Boot Info

↓

Kernel Entry
```

Jede Phase besitzt klar definierte Ein- und Ausgänge.

---

# 10. Bootphasen

```c
typedef enum
{
    NOVA_STAGE2_START,

    NOVA_STAGE2_PLATFORM,

    NOVA_STAGE2_MEMORY,

    NOVA_STAGE2_STORAGE,

    NOVA_STAGE2_FILESYSTEM,

    NOVA_STAGE2_CONFIGURATION,

    NOVA_STAGE2_GRAPHICS,

    NOVA_STAGE2_INPUT,

    NOVA_STAGE2_SECURITY,

    NOVA_STAGE2_KERNEL,

    NOVA_STAGE2_MODULES,

    NOVA_STAGE2_HANDOFF,

    NOVA_STAGE2_DONE,

    NOVA_STAGE2_ERROR

} nova_stage2_phase_t;
```

Alle Komponenten melden ihren Fortschritt an den Boot Core.

---

# 11. Zustandsdiagramm

```text
Initialize

↓

Platform

↓

Memory

↓

Storage

↓

Filesystem

↓

Configuration

↓

Graphics

↓

Input

↓

Kernel

↓

Modules

↓

Verify

↓

Handoff

↓

Kernel
```

Bei Fehlern erfolgt ein definierter Übergang in den Recovery- oder Diagnosemodus.

---

# 12. Komponentenmodell

Jede Stage-2-Komponente besitzt denselben Lebenszyklus.

```text
Create

↓

Initialize

↓

Run

↓

Shutdown
```

Dadurch können Komponenten unabhängig voneinander entwickelt und getestet werden.

---

# 13. Bootmodule

Stage 2 besteht intern aus logisch getrennten Modulen.

Mindestens:

- Platform
- Memory
- Graphics
- Storage
- Filesystem
- Configuration
- Input
- Security
- Kernel Loader
- Module Loader
- Recovery
- Diagnostics
- Logging
- Progress
- Handoff

Neue Module können später ergänzt werden, ohne den Boot Core grundlegend zu ändern.

---

# 14. Anforderungen

Stage 2 MUSS

- Stage 1 vollständig ersetzen
- BIOS und UEFI abstrahieren
- den Kernel laden
- Module laden
- Fehler behandeln
- Recovery unterstützen
- eine versionierte `nova_boot_info_t` erzeugen
- reproduzierbar arbeiten

---

# 15. SHOULD

Stage 2 SOLLTE

- vollständig modular aufgebaut sein
- klar dokumentiert werden
- Hardware abstrahieren
- Erweiterungen über Schnittstellen integrieren
- Diagnosedaten sammeln

---

# 16. MAY

Stage 2 DARF

- Animationen anzeigen
- Themes verwenden
- Hintergrundbilder laden
- Mehrsprachigkeit unterstützen
- Plugins laden
- Erweiterungsmodule einbinden

Diese Funktionen dürfen den Kernelstart jedoch niemals blockieren.

---

# 17. CPU-Initialisierung

Nach erfolgreichem Laden übernimmt Stage 2 die vollständige Initialisierung der CPU.

Zu den Aufgaben gehören:

- CPU identifizieren
- CPU-Fähigkeiten ermitteln
- Register vorbereiten
- A20 überprüfen
- GDT erzeugen
- Protected Mode vorbereiten
- Long Mode vorbereiten
- Interruptsystem vorbereiten

Stage 2 übernimmt dabei sämtliche Aufgaben, die bewusst nicht in Stage 1 enthalten sind.

---

# 18. CPU-Erkennung

Zu Beginn wird die CPU analysiert.

Ermittelt werden mindestens:

- Hersteller
- Modell
- Family
- Stepping
- Microcode-Version (optional)
- logische Prozessoren
- physische Kerne (falls verfügbar)
- Hyperthreading
- APIC-Unterstützung

---

# 19. CPUID

Stage 2 verwendet die CPUID-Instruktion.

Mindestens werden abgefragt:

```text
Vendor String

↓

Family

↓

Model

↓

Stepping

↓

Feature Bits

↓

Extended Feature Bits

↓

Extended Functions
```

Diese Informationen werden später an den Kernel weitergegeben.

---

# 20. CPU-Features

Mindestens sollen erkannt werden:

- FPU
- MMX
- SSE
- SSE2
- SSE3
- SSSE3
- SSE4.1
- SSE4.2
- AVX
- AVX2
- XSAVE
- AES
- RDRAND
- RDSEED
- NX
- PAE
- Long Mode
- APIC
- x2APIC
- TSC
- HPET (über ACPI)

---

# 21. CPU-Informationsstruktur

```c
typedef struct nova_cpu_info
{
    char vendor[16];

    uint32_t family;

    uint32_t model;

    uint32_t stepping;

    uint64_t feature_flags;

    uint64_t extended_features;

    bool long_mode;

    bool pae;

    bool nx;

    bool apic;

} nova_cpu_info_t;
```

Diese Struktur wird später Bestandteil von `nova_boot_info_t`.

---

# 22. A20 Gate

Vor dem Wechsel in den Protected Mode MUSS die A20-Leitung aktiviert sein.

Unterstützte Verfahren:

- BIOS
- Keyboard Controller
- Fast A20
- automatische Prüfung

Der Bootloader soll selbst erkennen, welches Verfahren erfolgreich war.

---

# 23. Global Descriptor Table

Stage 2 erzeugt eine vollständige GDT.

Mindestens:

```text
Null Descriptor

32 Bit Code

32 Bit Data

64 Bit Code

64 Bit Data

TSS (optional)
```

Alle Selektoren werden zentral definiert.

---

# 24. Beispiel-GDT

```text
Index

0

Null
```

```text
1

Kernel Code 32
```

```text
2

Kernel Data 32
```

```text
3

Kernel Code 64
```

```text
4

Kernel Data 64
```

Spätere Versionen ergänzen TSS und Benutzersegmente.

---

# 25. Protected Mode

Der Protected Mode wird ausschließlich durch Stage 2 aktiviert.

Ablauf

```text
CLI

↓

LGDT

↓

CR0 setzen

↓

Far Jump

↓

Protected Mode
```

Stage 1 darf den Protected Mode niemals aktivieren.

---

# 26. Long Mode Vorbereitung

Falls ein 64-Bit-Kernel geladen wird,

bereitet Stage 2 zusätzlich vor:

- PAE
- Long-Mode-GDT
- EFER
- Paging
- Identity Mapping

Der eigentliche Wechsel erfolgt unmittelbar vor dem Kernelstart.

---

# 27. Paging

Stage 2 erzeugt bei Bedarf erste Seitentabellen.

Unterstützt werden:

- Identity Mapping
- Higher Half Mapping
- Framebuffer Mapping
- Kernel Mapping
- Modul Mapping

Alle Tabellen werden später an den Kernel übergeben.

---

# 28. Speichererkennung

Der physische Speicher wird vollständig erkannt.

Unterstützt:

- BIOS E820
- UEFI Memory Map

Ergebnis:

```text
Available

Reserved

ACPI

Framebuffer

MMIO

Firmware

Bootloader

Kernel

Module
```

---

# 29. Speicherverwaltung

Stage 2 besitzt einen einfachen Boot-Allocator.

Eigenschaften:

- nur physischer Speicher
- keine Freigabe
- lineare Allokation
- deterministisch
- keine Fragmentierung

---

# 30. Boot Heap

```text
Boot Heap

↓

Allocate()

↓

Pointer
```

Der Heap dient ausschließlich Stage 2.

Nach dem Kernelstart wird er nicht weiter verwendet.

---

# 31. Memory Map

Alle Speicherbereiche werden gesammelt.

```c
typedef struct
{
    uint64_t start;

    uint64_t length;

    nova_boot_memory_type_t type;

} nova_memory_region_t;
```

Diese Struktur wird unverändert an den Kernel weitergereicht.

---

# 32. Framebuffer

Stage 2 initialisiert den Grafikmodus.

Unterstützt:

BIOS

- VBE

UEFI

- GOP

Beide liefern dieselbe Framebufferstruktur.

---

# 33. Framebufferstruktur

```c
typedef struct
{
    void* address;

    uint32_t width;

    uint32_t height;

    uint32_t pitch;

    uint32_t bpp;

    uint32_t pixel_format;

} nova_framebuffer_info_t;
```

Diese Struktur wird Bestandteil von `nova_boot_info_t`.

---

# 34. Video-Modi

Mindestens unterstützt:

- 800×600
- 1024×768
- 1280×720
- 1280×800
- 1366×768
- 1600×900
- 1920×1080

Automatisch wird der beste Modus gewählt.

---

# 35. Speicherlayout nach Initialisierung

```text
00000000

↓

BIOS

↓

Stage 1

↓

Stage 2

↓

Boot Heap

↓

Kernel

↓

Module

↓

Framebuffer

↓

freier Speicher
```

Alle Bereiche werden dokumentiert.

---

# 36. Interrupts

Während der Initialisierung bleiben Interrupts deaktiviert.

Erst unmittelbar vor dem Kernelstart entscheidet Stage 2,

ob Interrupts aktiviert oder deaktiviert übergeben werden.

Der Standard lautet:

```text
Interrupts deaktiviert
```

---

# 37. Bootzeit

Stage 2 beginnt eine monotone Zeitmessung.

Quellen:

- BIOS Tick Counter
- PIT
- HPET
- UEFI Timer

Die Zeit wird später Bestandteil der Bootinformationen.

---

# 38. Initialisierungsreihenfolge

```text
CPU

↓

CPUID

↓

A20

↓

Memory

↓

GDT

↓

Paging

↓

Framebuffer

↓

Boot Heap

↓

Weiter zur Plattforminitialisierung
```

Diese Reihenfolge ist verbindlich.

---

# 39. Anforderungen

Stage 2 MUSS:

- CPU identifizieren
- Speicher erkennen
- GDT erzeugen
- Framebuffer initialisieren
- Boot Heap erzeugen
- Paging vorbereiten
- Datenstrukturen für den Kernel aufbauen

---

# 40. SHOULD

Stage 2 SOLLTE:

- automatisch Long Mode vorbereiten
- verfügbare CPU-Features erkennen
- Speicher konsistent validieren
- ungültige Bereiche markieren
- mehrere Grafikmodi unterstützen

---

# 41. Boot Platform Layer

Der **Boot Platform Layer (BPL)** abstrahiert sämtliche Firmware- und Plattformdetails.

Der Boot Core darf niemals direkt auf

- BIOS
- UEFI
- Coreboot
- zukünftige Firmware

zugreifen.

Stattdessen erfolgt jeder Zugriff ausschließlich über den BPL.

```text
Firmware

↓

Platform Driver

↓

Boot Platform Layer

↓

Nova Boot Core
```

Dadurch bleibt der Boot Core vollständig plattformunabhängig.

---

# 42. Plattformtreiber

Jede Firmware besitzt einen eigenen Treiber.

```text
Platform Driver

├── BIOS Driver

├── UEFI Driver

├── Coreboot Driver

└── Future Driver
```

Alle Treiber implementieren dieselbe API.

---

# 43. Plattforminitialisierung

Die Initialisierung erfolgt unmittelbar nach der CPU-Initialisierung.

```text
CPU

↓

Platform Detect

↓

Platform Driver

↓

Platform Initialize

↓

Boot Core
```

Danach kennt der Boot Core keine Firmwaredetails mehr.

---

# 44. Plattformstruktur

```c
typedef struct nova_platform
{
    nova_platform_type_t type;

    char firmware_name[64];

    uint32_t version_major;

    uint32_t version_minor;

    uint64_t firmware_base;

    bool secure_boot;

    bool tpm_available;

} nova_platform_t;
```

---

# 45. Plattformtypen

```c
typedef enum
{
    NOVA_PLATFORM_UNKNOWN = 0,

    NOVA_PLATFORM_BIOS,

    NOVA_PLATFORM_UEFI,

    NOVA_PLATFORM_COREBOOT,

    NOVA_PLATFORM_CUSTOM

} nova_platform_type_t;
```

---

# 46. Plattform-API

Alle Plattformen implementieren dieselben Routinen.

```c
typedef struct
{
    nova_result_t (*initialize)(void);

    nova_result_t (*memory_map)(nova_memory_map_t*);

    nova_result_t (*read_blocks)(
        uint64_t lba,
        uint32_t sectors,
        void* buffer);

    nova_result_t (*set_video)(
        nova_video_request_t* request);

    nova_result_t (*input_poll)(
        nova_boot_input_event_t* event);

    nova_result_t (*shutdown_firmware)(void);

} nova_platform_driver_t;
```

---

# 47. BIOS-Treiber

Der BIOS-Treiber kapselt

- INT 13h
- INT 10h
- INT 16h
- E820
- VBE
- BIOS Timer

Der Boot Core kennt diese Interrupts nicht.

---

# 48. UEFI-Treiber

Der UEFI-Treiber kapselt

- Boot Services
- Runtime Services
- GOP
- Block IO
- Simple File System
- Memory Map
- Loaded Image
- ExitBootServices()

---

# 49. Plattformzustand

Während der Initialisierung werden gesammelt

- Firmwaretyp
- Firmwareversion
- Bootmodus
- Secure Boot Status
- TPM
- Zeitquelle
- Grafiksystem

Diese Informationen werden später an den Kernel übergeben.

---

# 50. Hardware Discovery

Nach der Plattforminitialisierung beginnt die Hardwareerkennung.

```text
CPU

↓

Memory

↓

Graphics

↓

Storage

↓

Input

↓

ACPI

↓

SMBIOS

↓

PCI

↓

Boot Device
```

---

# 51. Speichererkennung

Es werden erkannt

- RAM
- reservierter Speicher
- ACPI
- MMIO
- Framebuffer
- Firmware
- Bootloader

Alle Bereiche werden klassifiziert.

---

# 52. Datenträgererkennung

Stage 2 erkennt automatisch

- IDE
- SATA
- AHCI (Firmware)
- NVMe (Firmware)
- USB
- CD/DVD
- virtuelle Laufwerke

Dabei wird ausschließlich die Firmware verwendet.

Eigene Treiber existieren noch nicht.

---

# 53. Boot Device

Das Bootgerät wird beschrieben durch

```c
typedef struct
{
    uint64_t id;

    uint64_t size;

    nova_storage_type_t type;

    bool removable;

    char name[64];

} nova_boot_device_t;
```

---

# 54. Gerätekategorien

```c
typedef enum
{
    STORAGE_UNKNOWN,

    STORAGE_IDE,

    STORAGE_SATA,

    STORAGE_NVME,

    STORAGE_USB,

    STORAGE_CDROM,

    STORAGE_NETWORK

} nova_storage_type_t;
```

---

# 55. ACPI

Stage 2 sucht

- RSDP
- RSDT
- XSDT
- MADT
- FADT
- HPET
- MCFG

Die Tabellen werden nicht vollständig interpretiert.

Sie werden für den Kernel vorbereitet.

---

# 56. ACPI-Struktur

```c
typedef struct
{
    void* rsdp;

    void* rsdt;

    void* xsdt;

    uint32_t revision;

} nova_acpi_info_t;
```

---

# 57. SMBIOS

Es werden gesammelt

- Hersteller
- Produkt
- BIOS-Version
- Board
- Speicher
- Seriennummern (optional)
- UUID (optional)

Alle personenbezogenen Daten können durch Richtlinien ausgeblendet werden.

---

# 58. SMBIOS-Struktur

```c
typedef struct
{
    char vendor[64];

    char product[64];

    char bios_version[64];

    char board[64];

} nova_smbios_info_t;
```

---

# 59. PCI Discovery

Optional scannt Stage 2 den PCI-Bus.

Es werden mindestens erkannt

- GPU
- Storage Controller
- Netzwerkcontroller
- USB Controller
- Audio Controller

Es werden noch keine Gerätetreiber geladen.

---

# 60. PCI-Gerät

```c
typedef struct
{
    uint16_t vendor;

    uint16_t device;

    uint8_t class_code;

    uint8_t subclass;

    uint8_t prog_if;

} nova_pci_device_t;
```

---

# 61. Geräteklassifizierung

Alle Geräte werden logisch eingeordnet.

```text
Graphics

Storage

Network

Audio

USB

Bridge

Input

Other
```

Dadurch muss der Kernel keine vollständige Erkennung erneut durchführen.

---

# 62. Firmwaredienste

Bis zum Kernelstart dürfen Firmwaredienste verwendet werden.

Vor der Kernelübergabe gilt:

BIOS

```text
keine weiteren BIOS-Aufrufe
```

UEFI

```text
ExitBootServices()
```

Danach übernimmt ausschließlich der Kernel.

---

# 63. Plattformvalidierung

Vor dem Kernelstart prüft Stage 2

✔ gültige Memory Map

✔ gültiger Framebuffer

✔ gültiges Bootgerät

✔ gültige Firmware

✔ erfolgreiche Hardwareinitialisierung

Fehler führen in den Diagnosemodus.

---

# 64. Plattformdiagnose

Die Diagnose sammelt

- Firmwareinformationen
- Bootmodus
- Speichergröße
- CPU
- Grafikmodus
- Datenträger
- ACPI
- SMBIOS

Diese Daten können im Recovery-System angezeigt werden.

---

# 65. Anforderungen

Der Platform Layer MUSS

- BIOS kapseln
- UEFI kapseln
- Hardware erkennen
- ACPI vorbereiten
- SMBIOS vorbereiten
- Speicher bereitstellen
- den Boot Core unabhängig halten

---

# 66. SHOULD

Der Platform Layer SOLLTE

- zukünftige Firmware unterstützen
- leicht portierbar sein
- klar dokumentiert sein
- deterministisch arbeiten
- Diagnoseinformationen sammeln

---

# 67. MAY

Der Platform Layer DARF

- Coreboot unterstützen
- Open Firmware unterstützen
- Emulatoren speziell behandeln
- virtuelle Hardware optimieren

Diese Erweiterungen dürfen die Standardarchitektur nicht beeinflussen.

---

# 68. Storage Runtime

Die **Storage Runtime** stellt sämtliche Datenträgerzugriffe innerhalb von Stage 2 bereit.

Der Boot Core arbeitet niemals direkt mit BIOS- oder UEFI-Aufrufen.

Alle Datenträger werden über eine einheitliche Runtime angesprochen.

```text
Boot Core

↓

Storage Runtime

↓

Platform Driver

↓

Firmware
```

---

# 69. Ziele

Die Storage Runtime soll

- alle Bootlaufwerke erkennen
- Partitionen analysieren
- Dateisysteme bereitstellen
- Blockzugriffe abstrahieren
- Dateicaching ermöglichen
- Kerneldateien laden
- Recovery-Daten lesen
- zukünftige Dateisysteme unterstützen

---

# 70. Speichergeräte

Unterstützt werden:

- IDE
- SATA
- AHCI (Firmware)
- NVMe (Firmware)
- USB
- CD/DVD
- virtuelle Laufwerke
- RAM-Disk (später)

---

# 71. Block Device

Alle Laufwerke implementieren dieselbe Schnittstelle.

```c
typedef struct nova_block_device
{
    uint64_t device_id;

    uint64_t block_size;

    uint64_t block_count;

    bool removable;

    nova_storage_type_t type;

    nova_result_t (*read)(
        uint64_t lba,
        uint32_t count,
        void* buffer);

} nova_block_device_t;
```

---

# 72. Block Layer

Die Block Layer arbeitet ausschließlich mit

```text
Logical Block Address (LBA)
```

CHS wird ausschließlich innerhalb des BIOS-Treibers behandelt.

Der Boot Core kennt keine CHS-Adressen.

---

# 73. Partitionsverwaltung

Nach der Laufwerkserkennung erfolgt automatisch die Analyse der Partitionstabellen.

Unterstützt werden

- MBR
- GPT

---

# 74. Master Boot Record

Bei klassischen Datenträgern wird zunächst der MBR gelesen.

Ermittelt werden

- Bootflag
- Partitionstyp
- Start-LBA
- Größe

---

# 75. GPT

Falls eine GPT erkannt wird,

liest Stage 2

- GPT Header
- Partition Entries
- GUIDs
- CRC
- Backup GPT

Beschädigte GPTs werden erkannt.

---

# 76. Partition

```c
typedef struct nova_partition
{
    uint64_t first_lba;

    uint64_t last_lba;

    uint64_t size;

    nova_partition_type_t type;

    char label[64];

} nova_partition_t;
```

---

# 77. Partitionstypen

```c
typedef enum
{
    NOVA_PARTITION_UNKNOWN,

    NOVA_PARTITION_FAT,

    NOVA_PARTITION_ISO9660,

    NOVA_PARTITION_EXT2,

    NOVA_PARTITION_NOVAFS,

    NOVA_PARTITION_RECOVERY

} nova_partition_type_t;
```

---

# 78. Filesystem Layer

Alle Dateisysteme implementieren dieselbe API.

```text
Filesystem

↓

Open

↓

Read

↓

Close
```

Der Kernel Loader arbeitet ausschließlich mit dieser API.

---

# 79. Unterstützte Dateisysteme

Pflicht

- FAT12
- FAT16
- FAT32

Empfohlen

- ISO9660

Später

- ext2
- NovaFS

---

# 80. FAT Runtime

Die FAT Runtime unterstützt

- BPB
- FAT
- Cluster Chains
- Root Directory
- Unterverzeichnisse
- Long File Names
- Dateilesen

Schreibzugriffe erfolgen im Bootloader grundsätzlich nicht.

---

# 81. ISO9660 Runtime

Für Rettungs-CDs wird ISO9660 unterstützt.

Mindestens

- Primary Volume Descriptor
- Directory Records
- Dateizugriff
- El Torito

Später optional

- Joliet
- Rock Ridge

---

# 82. NovaFS

Langfristig erhält Nova ein eigenes Boot-Dateisystem.

Eigenschaften

- Journaling
- Prüfsummen
- Snapshots
- Kompression
- Signaturen

Während der ersten Version wird NovaFS jedoch noch nicht vorausgesetzt.

---

# 83. Filesystem API

```c
typedef struct nova_filesystem_driver
{
    nova_result_t (*mount)(
        nova_partition_t* partition);

    nova_result_t (*open)(
        const char* path,
        nova_file_t* file);

    nova_result_t (*read)(
        nova_file_t* file,
        void* buffer,
        uint64_t bytes);

    nova_result_t (*close)(
        nova_file_t* file);

} nova_filesystem_driver_t;
```

---

# 84. Dateistruktur

```c
typedef struct nova_file
{
    uint64_t size;

    uint64_t position;

    uint64_t first_cluster;

    uint32_t attributes;

    char path[260];

} nova_file_t;
```

---

# 85. Bootkonfiguration

Die Bootkonfiguration wird als normale Datei gelesen.

Standardpfad

```text
/boot/nova.cfg
```

oder

```text
/nova/boot.cfg
```

Der Pfad ist konfigurierbar.

---

# 86. Bootkonfigurationsformat

```ini
[boot]

default=nova

timeout=2

theme=dark

resolution=auto

language=de-DE

[entry.nova]

kernel=/boot/kernel.elf

initramfs=/boot/initramfs.img

arguments=quiet splash
```

---

# 87. Kerneldateien

Mindestens werden geladen

```text
kernel.elf
```

optional

```text
kernel64.elf
```

```text
kernel.debug
```

---

# 88. Moduldateien

Zusätzlich können geladen werden

- initramfs
- Treibermodule
- Fonts
- Themes
- Recovery
- Bootanimationen
- Konfigurationsdateien

---

# 89. Block Cache

Stage 2 besitzt einen kleinen Blockcache.

Eigenschaften

- Read Only
- FIFO oder LRU
- feste Größe
- deterministisch

Der Cache dient ausschließlich zur Beschleunigung des Bootvorgangs.

---

# 90. File Cache

Zusätzlich existiert ein Dateicache.

Geeignet für

- Kernel
- Initramfs
- Konfiguration
- Fonts
- Themes

Dadurch werden doppelte Lesezugriffe vermieden.

---

# 91. Fehlerbehandlung

Dateisystemfehler werden klassifiziert.

```text
Filesystem Not Found

↓

Partition Missing

↓

Read Error

↓

Invalid Format

↓

Recovery
```

---

# 92. Recovery Integration

Falls Kerneldateien fehlen,

kann automatisch

- Recovery gestartet
- alternativer Kernel geladen
- Backup verwendet
- Snapshot gewählt

werden.

---

# 93. Sicherheitsprüfung

Vor der Weitergabe an den Kernel werden geprüft

- Dateigröße
- Hash
- Signatur
- Dateiformat
- Ladeadresse

Ungültige Dateien dürfen nicht gestartet werden.

---

# 94. Anforderungen

Die Storage Runtime MUSS

- MBR unterstützen
- GPT unterstützen
- FAT lesen
- Kernel laden
- Initramfs laden
- Module laden
- Konfiguration lesen
- Caching unterstützen

---

# 95. SHOULD

Die Storage Runtime SOLLTE

- ISO9660 unterstützen
- Blockcache besitzen
- Dateicache besitzen
- zukünftige Dateisysteme leicht integrieren können
- Recovery automatisch unterstützen

---

# 96. MAY

Die Storage Runtime DARF

- ext2
- NovaFS
- Netzwerkboot
- verschlüsselte Dateisysteme
- Snapshot-Dateisysteme

unterstützen.

---

# 97. Kernel Loader

Der **Kernel Loader** ist die zentrale Komponente von Stage 2.

Er ist verantwortlich für

- Kerneldatei öffnen
- Dateiformat erkennen
- Integrität prüfen
- Speicher reservieren
- Segmente laden
- BSS initialisieren
- Module vorbereiten
- Handoff erzeugen
- Kernel starten

Er stellt die letzte Stufe vor der Kontrolle des Betriebssystems dar.

---

# 98. Unterstützte Kernel-Formate

Pflicht

- ELF32
- ELF64

Optional

- PE/COFF
- Nova Kernel Image (NKI)
- komprimierte Kernel

---

# 99. Ladeablauf

```text
Open Kernel

↓

Read Header

↓

Validate

↓

Allocate Memory

↓

Load Segments

↓

Initialize BSS

↓

Load Modules

↓

Build Boot Info

↓

Transfer Control
```

Jeder Schritt muss erfolgreich abgeschlossen sein.

---

# 100. ELF-Erkennung

Vor dem Laden wird geprüft

```text
Magic Number

↓

Architecture

↓

Endianess

↓

Version

↓

Entry Point

↓

Program Header
```

Ungültige ELF-Dateien dürfen nicht ausgeführt werden.

---

# 101. ELF Loader

Der Loader unterstützt

- ELF Header
- Program Header
- Load Segments
- BSS
- Alignment
- Entry Point
- Symbolinformationen (optional)

Section Header werden für den Bootvorgang nicht benötigt.

---

# 102. Speicherreservierung

Für jedes Segment wird

```text
Allocate Physical Memory

↓

Copy Segment

↓

Zero BSS

↓

Protect Region
```

durchgeführt.

Überlappende Segmente sind unzulässig.

---

# 103. Segmenttypen

Unterstützt

```text
PT_LOAD

PT_DYNAMIC (optional)

PT_NOTE (optional)

PT_TLS (optional)
```

Nicht unterstützte Segmenttypen werden ignoriert, sofern sie für den Kernelstart nicht erforderlich sind.

---

# 104. Kernelstruktur

```c
typedef struct nova_kernel_image
{
    void* entry;

    uint64_t image_base;

    uint64_t image_size;

    uint64_t physical_address;

    bool is_64bit;

    bool relocatable;

} nova_kernel_image_t;
```

---

# 105. Kernelprüfung

Vor dem Start werden mindestens geprüft

- ELF Magic
- Entry Point
- Segmentgrößen
- Segmentgrenzen
- Speicherüberlappungen
- Hash
- Signatur (optional)

---

# 106. Initramfs

Stage 2 unterstützt optional ein Initramfs.

Eigenschaften

- komprimiert oder unkomprimiert
- Read Only
- frühes Dateisystem
- Treiber
- Konfiguration
- Recovery-Dateien

---

# 107. Modul Loader

Zusätzlich zum Kernel können Module geladen werden.

Mögliche Module

- Initramfs
- Schriftarten
- Themes
- Bootanimation
- Microcode
- Treiber
- Recovery
- Sprachpakete

---

# 108. Modulstruktur

```c
typedef struct nova_boot_module
{
    char name[64];

    char path[256];

    void* address;

    uint64_t size;

    uint8_t hash[32];

    nova_module_type_t type;

} nova_boot_module_t;
```

---

# 109. Modularten

```c
typedef enum
{
    NOVA_MODULE_KERNEL,

    NOVA_MODULE_INITRAMFS,

    NOVA_MODULE_FONT,

    NOVA_MODULE_THEME,

    NOVA_MODULE_DRIVER,

    NOVA_MODULE_RECOVERY,

    NOVA_MODULE_CONFIGURATION,

    NOVA_MODULE_MICROCODE

} nova_module_type_t;
```

---

# 110. Boot Manifest

Alle Bootdateien werden über ein Manifest beschrieben.

Beispiel

```text
boot.manifest

↓

Kernel

↓

Initramfs

↓

Fonts

↓

Themes

↓

Recovery

↓

Signaturen
```

---

# 111. Manifeststruktur

```c
typedef struct nova_boot_manifest
{
    uint32_t version;

    uint32_t module_count;

    uint8_t manifest_hash[32];

    nova_boot_module_t* modules;

} nova_boot_manifest_t;
```

---

# 112. Boot Information

Vor dem Kernelstart erzeugt Stage 2 die vollständige

```text
nova_boot_info_t
```

Sie ersetzt sämtliche temporären Übergaben.

---

# 113. Aufbau von nova_boot_info_t

```c
typedef struct nova_boot_info
{
    uint64_t signature;

    uint32_t version;

    uint32_t size;

    nova_platform_t platform;

    nova_cpu_info_t cpu;

    nova_memory_map_t memory;

    nova_framebuffer_info_t framebuffer;

    nova_boot_device_t boot_device;

    nova_acpi_info_t acpi;

    nova_smbios_info_t smbios;

    nova_kernel_image_t kernel;

    nova_boot_module_list_t modules;

    nova_boot_security_state_t security;

    nova_boot_log_t log;

} nova_boot_info_t;
```

Diese Struktur ist die wichtigste Schnittstelle zwischen Bootloader und Kernel.

---

# 114. Bootinformationen

Mindestens enthalten

- Plattform
- CPU
- RAM
- Framebuffer
- ACPI
- SMBIOS
- Bootgerät
- Kernel
- Module
- Sicherheitsstatus
- Bootlog

---

# 115. ABI

Der Kernel erhält ausschließlich

```text
Pointer auf nova_boot_info_t
```

Alle zukünftigen Erweiterungen erfolgen innerhalb dieser Struktur.

Dadurch bleibt die ABI stabil.

---

# 116. Kernel Entry

Vor dem Kernelstart gilt

```text
Interrupts deaktiviert
```

```text
Paging vorbereitet
```

```text
Framebuffer gültig
```

```text
Memory Map gültig
```

```text
Module geladen
```

```text
Firmware beendet
```

Danach übernimmt ausschließlich der Kernel.

---

# 117. Übergabe

```text
Stage 2

↓

Build Boot Info

↓

Exit Firmware

↓

Jump Kernel Entry
```

Es existiert kein Rücksprung.

---

# 118. Übergabeparameter

Für x86-32

```text
EAX

Magic
```

```text
EBX

nova_boot_info
```

Für x86-64

```text
RDI

nova_boot_info
```

---

# 119. Kernel Magic

```c
#define NOVA_BOOT_MAGIC \
0x4E4F5641424F4F54ULL
```

Der Kernel prüft

- Magic
- Version
- Größe

bevor weitere Felder verwendet werden.

---

# 120. Anforderungen

Der Kernel Loader MUSS

- ELF32 laden
- ELF64 laden
- Module laden
- Initramfs laden
- Bootinformationen erzeugen
- sichere Übergabe durchführen
- Firmware korrekt verlassen

---

# 121. SHOULD

Der Kernel Loader SOLLTE

- Relocations unterstützen
- komprimierte Kernel unterstützen
- Kernelversion prüfen
- Signaturen prüfen
- Rollback unterstützen

---

# 122. MAY

Der Kernel Loader DARF

- mehrere Kernel verwalten
- Debugkernel laden
- Testkernel laden
- Recoverykernel laden
- Kernelbenchmarking durchführen

---

# 123. Graphics Runtime

Die **Nova Graphics Runtime** stellt sämtliche grafischen Funktionen des Bootloaders bereit.

Sie bildet die Grundlage für

- Boot Splash
- Bootmenü
- Recovery UI
- Diagnose
- Setup
- Dateimanager
- Speichertest
- Verschlüsselungsdialoge

Alle grafischen Komponenten verwenden ausschließlich diese Runtime.

---

# 124. Ziele

Die Graphics Runtime soll

- BIOS und UEFI vereinheitlichen
- Hardware abstrahieren
- Framebuffer verwalten
- Themes unterstützen
- hochauflösende Oberflächen ermöglichen
- Animationen bereitstellen
- möglichst wenig Speicher benötigen
- unabhängig vom Kernel funktionieren

---

# 125. Architektur

```text
Graphics Runtime

├── Framebuffer Manager
├── Renderer
├── Font Engine
├── Image Decoder
├── Theme Manager
├── Window Manager
├── Cursor Manager
├── Animation Engine
├── Widget Library
└── UI Manager
```

---

# 126. Grafik-Backends

Unterstützt werden

## BIOS

- VBE 2.0
- VBE 3.0
- Linear Framebuffer

---

## UEFI

- Graphics Output Protocol (GOP)

---

Alle Backends liefern dieselbe Framebufferstruktur.

---

# 127. Framebuffer Manager

Der Framebuffer Manager verwaltet

- Framebufferadresse
- Auflösung
- Farbtiefe
- Pitch
- Pixelformat
- Backbuffer

```c
typedef struct
{
    void* framebuffer;

    void* backbuffer;

    uint32_t width;

    uint32_t height;

    uint32_t pitch;

    uint32_t bpp;

} nova_framebuffer_t;
```

---

# 128. Unterstützte Auflösungen

Mindestens

- 800×600
- 1024×768
- 1280×720
- 1280×800
- 1366×768
- 1600×900
- 1920×1080
- 2560×1440
- 3840×2160 (UEFI)

Die höchste kompatible Auflösung wird automatisch gewählt.

---

# 129. Renderer

Der Renderer stellt primitive Zeichenfunktionen bereit.

Mindestens

- Pixel
- Linie
- Rechteck
- Kreis
- Polygon
- Text
- Bild
- Verlauf
- Alpha Blending

Alle höheren UI-Komponenten bauen darauf auf.

---

# 130. Zeichenprimitive

```c
void draw_pixel();

void draw_line();

void draw_rect();

void fill_rect();

void draw_circle();

void draw_image();

void draw_text();

void present();
```

---

# 131. Double Buffering

Standardmäßig verwendet Nova

```text
Backbuffer

↓

Render

↓

Present

↓

Framebuffer
```

Dadurch entstehen keine sichtbaren Flackereffekte.

---

# 132. Dirty Regions

Zur Performanceoptimierung werden nur geänderte Bereiche neu gezeichnet.

```text
Invalidate()

↓

Dirty Rectangle

↓

Redraw

↓

Present
```

Dadurch sinkt die CPU-Last deutlich.

---

# 133. Font Engine

Die Font Engine unterstützt

- Bitmap Fonts
- PSF
- BDF
- TrueType (TTF)
- OpenType (OTF)

Alle Schriftarten werden in Glyph-Caches umgewandelt.

---

# 134. Unicode

Alle Texte verwenden intern

```text
UTF-8
```

Unterstützt werden

- ASCII
- Unicode BMP
- Emoji (optional)
- mathematische Zeichen
- technische Symbole

---

# 135. Glyph Cache

Beim Laden einer Schrift werden Glyphen zwischengespeichert.

```text
Load Font

↓

Rasterize

↓

Glyph Cache

↓

Renderer
```

Dadurch müssen Zeichen nur einmal berechnet werden.

---

# 136. Image Runtime

Unterstützte Formate

Pflicht

- BMP
- PNG

Optional

- JPEG
- WebP

Später

- AVIF
- HEIF

---

# 137. SVG

SVG-Dateien werden nicht direkt gerendert.

Beim Build erfolgt

```text
SVG

↓

Nova Vector Format

↓

Boot Assets
```

Dadurch bleibt der Bootloader klein.

---

# 138. Theme Manager

Ein Theme definiert

- Farben
- Schriften
- Icons
- Animationen
- Hintergrundbilder
- Fensterstil
- Widgets

```text
Theme

↓

Renderer

↓

UI
```

---

# 139. Standardthemes

Mindestens

- Nova Dark
- Nova Light
- High Contrast
- Recovery

Später

- Benutzerdefinierte Themes

---

# 140. Window Manager

Obwohl Stage 2 kein Desktop besitzt,

verwaltet der Window Manager

- Dialoge
- Fenster
- Panels
- Modale Fenster
- Popupfenster

Dadurch bleibt die UI konsistent.

---

# 141. Widget Library

Verfügbare Widgets

- Button
- Label
- Checkbox
- Radio Button
- Slider
- Progressbar
- Textfeld
- Listen
- Tabellen
- Icons
- Scrollbereiche

Alle Widgets sind vollständig tastaturbedienbar.

---

# 142. Cursor Manager

Unterstützt

- Softwarecursor
- Hardwarecursor (wenn verfügbar)
- animierte Cursor
- Skalierung
- HiDPI

---

# 143. Animation Engine

Animationen werden zeitbasiert ausgeführt.

Unterstützt

- Fade
- Slide
- Zoom
- Scale
- Opacity
- Rotation (optional)

Animationen dürfen den Bootvorgang niemals blockieren.

---

# 144. Boot Splash

Die Boot Splash Engine unterstützt

- Hintergrundbild
- Logo
- Fortschrittsanzeige
- Statusmeldungen
- Animationen
- Übergänge

Die Anzeige ist an reale Bootphasen gekoppelt.

---

# 145. Progress Engine

Fortschritt basiert auf

```text
Boot Phase

↓

Teilfortschritt

↓

Gesamtfortschritt

↓

Progress Bar
```

Es werden keine künstlichen Ladebalken verwendet.

---

# 146. UI Layout Engine

Die Oberfläche verwendet ein flexibles Layoutsystem.

Unterstützt

- Anchors
- Docking
- Zentrierung
- Raster
- Padding
- Margin
- Skalierung

Dadurch passt sich die Oberfläche an verschiedene Auflösungen an.

---

# 147. Rendering Pipeline

```text
UI

↓

Layout

↓

Widgets

↓

Renderer

↓

Backbuffer

↓

Present
```

Diese Pipeline gilt für sämtliche Bootoberflächen.

---

# 148. Performance

Die Graphics Runtime soll

- SIMD verwenden (später)
- Dirty Regions nutzen
- Double Buffering verwenden
- Alpha Blending optimieren
- Speicherzugriffe minimieren

---

# 149. Anforderungen

Die Graphics Runtime MUSS

- VBE unterstützen
- GOP unterstützen
- PNG laden
- TTF darstellen
- Double Buffering besitzen
- Progress anzeigen
- Themes unterstützen

---

# 150. SHOULD

Die Graphics Runtime SOLLTE

- SVG unterstützen (vorverarbeitet)
- Unicode vollständig darstellen
- HiDPI unterstützen
- mehrere Themes besitzen
- GPU-Beschleunigung später ermöglichen

---

# 151. MAY

Die Graphics Runtime DARF

- Videos
- Lottie-Animationen
- Shader
- 3D-Effekte
- Partikelsysteme

unterstützen, sofern diese den Systemstart nicht verzögern.

---

# Designhinweis – Nova Boot UI

Für Nova OS sollte die Bootoberfläche nicht wie klassische Bootloader (GRUB, Windows Boot Manager) wirken, sondern bereits die Designsprache des späteren Desktops übernehmen.

Das bedeutet:

- Fluent-/Acrylic-Design
- Glas- und Transparenzeffekte (soweit softwareseitig möglich)
- konsistente Icons und Typografie
- dieselbe Farbpalette wie Nova OS
- identische UI-Komponenten (Buttons, Dialoge, Listen)
- fließender Übergang vom Bootloader zur Desktopoberfläche

Dadurch entsteht für den Benutzer der Eindruck eines durchgängigen Systems ohne sichtbaren Wechsel zwischen Firmware, Bootloader und Betriebssystem.

---

# 152. Boot User Interface

Die **Nova Boot UI** bildet die komplette Benutzerschnittstelle des Bootloaders.

Sie ersetzt klassische textbasierte Bootloader vollständig.

Die Benutzeroberfläche wird für sämtliche Bootfunktionen verwendet:

- Normaler Systemstart
- Recovery
- Diagnose
- Speicherprüfung
- Backup
- Wiederherstellung
- Bootmanager
- Datenträgerverwaltung
- Verschlüsselung
- Einstellungen

---

# 153. Designprinzipien

Die Bootoberfläche folgt denselben Gestaltungsrichtlinien wie Nova OS.

Grundprinzipien:

- Fluent Design
- Glass Design
- wenige Farben
- große Bedienelemente
- hohe Lesbarkeit
- vollständige Maussteuerung
- vollständige Tastatursteuerung
- Controller-Unterstützung (optional)

Der Benutzer soll keinen sichtbaren Übergang zwischen Bootloader und Betriebssystem wahrnehmen.

---

# 154. Boot Workflow

```text
Firmware

↓

Stage 1

↓

Stage 2

↓

Nova Splash

↓

Bootmenü

↓

Systemstart

↓

Kernel

↓

Nova Desktop
```

Optional wird das Bootmenü übersprungen.

---

# 155. Startbildschirm

Beim Start erscheint zunächst

```text
Nova Logo

↓

Systeminitialisierung

↓

Bootfortschritt

↓

Statusmeldungen
```

Der Bildschirm verwendet ausschließlich reale Fortschrittsdaten.

---

# 156. Bootmenü

Das Bootmenü unterstützt beliebig viele Einträge.

Beispiel

```text
Nova OS

Nova Recovery

Nova Diagnostics

Memory Test

Disk Manager

Backup Manager

Firmware Setup

Power Off

Restart
```

Die Reihenfolge ist konfigurierbar.

---

# 157. Booteintrag

```c
typedef struct
{
    char title[64];

    char description[256];

    char icon[64];

    char kernel_path[260];

    char initramfs[260];

    char arguments[512];

    uint32_t flags;

} nova_boot_entry_t;
```

---

# 158. Timeout

Das Bootmenü besitzt einen konfigurierbaren Timeout.

```ini
timeout=2
```

Unterstützt

- kein Timeout
- Sekunden
- automatische Auswahl

---

# 159. Navigation

Unterstützt

- Tastatur
- Maus
- Touch
- Gamepad (optional)

Tastatur

```text
↑ ↓

Auswahl
```

```text
Enter

Start
```

```text
ESC

Zurück
```

---

# 160. Maus

Die Bootoberfläche unterstützt

- Zeiger
- Klick
- Doppelklick
- Scrollrad
- Drag & Drop (Recovery)

Die Maus wird bereits im Bootloader vollständig initialisiert.

---

# 161. Touch

Optional

- Single Touch
- Multi Touch
- Gesten

Touch wird über dieselbe Event-API verarbeitet.

---

# 162. Eventsystem

Alle Eingaben werden vereinheitlicht.

```text
Keyboard

↓

Mouse

↓

Touch

↓

Gamepad

↓

Event Queue

↓

UI
```

---

# 163. Dialogsystem

Der Bootloader besitzt ein eigenes Dialogsystem.

Unterstützt

- Meldungen
- Warnungen
- Fehler
- Bestätigungen
- Kennwortabfragen
- Dateiauswahl

---

# 164. Recovery UI

Die Recovery-Oberfläche besitzt dieselbe Designsprache wie Nova OS.

Module

- Dateimanager
- Terminal
- Snapshot
- Wiederherstellung
- Datenträgerprüfung
- Bootreparatur
- Netzwerkeinstellungen (optional)

---

# 165. Dateimanager

Der integrierte Dateimanager unterstützt

- FAT
- ISO9660
- NovaFS
- ext2 (später)

Funktionen

- Kopieren
- Verschieben
- Löschen
- Umbenennen
- Vorschau
- Eigenschaften

---

# 166. Backup Manager

Der Backup Manager unterstützt

- komplette Laufwerke
- Partitionen
- einzelne Dateien
- Snapshots
- automatische Sicherungen

---

# 167. Wiederherstellung

Unterstützt

- Snapshot Restore
- Image Restore
- Datei Restore
- Bootloader Restore
- Konfigurations Restore

---

# 168. Speichertest

Ein integrierter Speichertest prüft

- RAM
- Speicherfehler
- Adressleitungen
- Datenleitungen
- Muster
- ECC-Status (wenn verfügbar)

---

# 169. Datenträgerdiagnose

Unterstützt

- SMART (später)
- Lesetest
- Oberflächentest
- Dateisystemprüfung
- Partitionserkennung
- Bootsektorprüfung

---

# 170. Verschlüsselungsdialog

Falls eine verschlüsselte Systempartition erkannt wird,

erscheint automatisch ein Entsperrdialog.

Unterstützt

- Passwort
- TPM
- Recovery Key
- Smartcard (später)

---

# 171. Bootoptionen

Beim Start können optionale Modi gewählt werden.

Beispiele

- Normal
- Safe Mode
- Recovery
- Debug
- Verbose
- Single User
- Memory Test

---

# 172. Einstellungen

Bereits im Bootloader können geändert werden

- Sprache
- Auflösung
- Theme
- Timeout
- Standardbetriebssystem
- Mausgeschwindigkeit
- Barrierefreiheit

Alle Änderungen werden in der Bootkonfiguration gespeichert.

---

# 173. Mehrsprachigkeit

Alle Texte stammen aus Sprachdateien.

Unterstützt

- Deutsch
- Englisch

Später

- weitere Sprachen

Unicode wird vollständig unterstützt.

---

# 174. Barrierefreiheit

Die Bootoberfläche unterstützt

- große Schrift
- hohen Kontrast
- Screenreader-Hooks (später)
- Tastaturnavigation
- Farbfehlsichtigkeitsprofile

---

# 175. Boot-Workflows

Der Bootloader definiert standardisierte Abläufe.

Beispiel

```text
Normal Boot

↓

Kernel

↓

Desktop
```

```text
Recovery

↓

Recovery UI

↓

Reparatur

↓

Neustart
```

```text
Diagnose

↓

Hardwaretests

↓

Bericht

↓

Benutzerentscheidung
```

---

# 176. Sitzungsverwaltung

Die Bootoberfläche merkt sich optional

- letzten Bootmodus
- letzte Sprache
- letzte Auflösung
- letzte Fehlermeldung

Diese Informationen können im Recovery-Modus angezeigt werden.

---

# 177. Anforderungen

Die Boot UI MUSS

- vollständig grafisch sein
- Maus unterstützen
- Tastatur unterstützen
- Recovery integrieren
- Bootmenü anzeigen
- Dateimanager besitzen
- Diagnose unterstützen

---

# 178. SHOULD

Die Boot UI SOLLTE

- Touch unterstützen
- Themes besitzen
- Animationen nutzen
- HiDPI unterstützen
- Mehrsprachigkeit ermöglichen

---

# 179. MAY

Die Boot UI DARF

- Gamepad unterstützen
- Bildschirmtastatur besitzen
- Sprachausgabe integrieren
- Netzwerkfunktionen für Remote-Recovery bereitstellen
- Plugin-basierte Werkzeuge laden

---

# Designhinweis – Nova Boot Experience

Der Bootloader soll nicht wie ein technisches Werkzeug wirken, sondern wie der erste Bildschirm eines modernen Betriebssystems.

Das Ziel ist, dass der Benutzer bereits während des Bootvorgangs dieselbe visuelle Identität erlebt wie später auf dem Desktop.

Deshalb sollen Farben, Icons, Animationen, Typografie und Bedienkonzepte zwischen Bootloader und Nova OS konsistent sein.

---

# 180. Sicherheitsarchitektur

Die Sicherheitsarchitektur von Stage 2 bildet die **Root of Trust** für Nova OS.

Ab Stage 2 beginnt die vollständige Vertrauenskette.

```text
Firmware

↓

Stage 1

↓

Stage 2

↓

Kernel

↓

Kernel Module

↓

Systemdienste

↓

Benutzeranwendungen
```

Jede Komponente validiert die nachfolgende Komponente.

---

# 181. Sicherheitsziele

Stage 2 MUSS

- Integrität prüfen
- Authentizität prüfen
- Bootmanipulation erkennen
- beschädigte Komponenten erkennen
- Rollback ermöglichen
- Recovery unterstützen
- Auditinformationen erzeugen

---

# 182. Chain of Trust

```text
Firmware

↓

Stage 1

↓

Stage 2

↓

Boot Manifest

↓

Kernel

↓

Module

↓

Initramfs
```

Jede Stufe überprüft die Integrität der nächsten.

---

# 183. Integritätsmodell

Für jede Bootdatei werden mindestens geprüft

- Größe
- Hash
- Format
- Version
- Ladeadresse

Optional zusätzlich

- digitale Signatur
- Zertifikatskette
- TPM Measurements

---

# 184. Hashalgorithmen

Pflicht

- SHA-256

Optional

- SHA-384
- SHA-512
- BLAKE3

CRC dient ausschließlich der Fehlererkennung und ersetzt keine kryptographische Prüfung.

---

# 185. Digitale Signaturen

Nova unterstützt signierte Bootdateien.

Signierbar sind

- Kernel
- Initramfs
- Module
- Themes
- Recovery
- Konfigurationsdateien
- Boot Manifest

---

# 186. Nova Trust Store

Stage 2 besitzt einen eigenen Vertrauensspeicher.

```text
Nova Trust Store

↓

Root Keys

↓

Intermediate Keys

↓

Signing Keys
```

Der Trust Store ist unabhängig von der Firmware implementiert.

---

# 187. Secure Boot

Wenn Secure Boot aktiv ist,

werden folgende Informationen erfasst

- aktiv/inaktiv
- Plattformschlüssel
- Firmwaremodus
- Richtlinien

Stage 2 kann sowohl mit aktiviertem als auch deaktiviertem Secure Boot arbeiten.

---

# 188. TPM 2.0

Falls vorhanden,

werden erkannt

- TPM Version
- Hersteller
- PCRs
- Verfügbarkeit
- Ownership Status

Die eigentliche TPM-Nutzung wird durch den Security Manager gesteuert.

---

# 189. Measured Boot

Optional misst Stage 2

- Bootloader
- Manifest
- Kernel
- Module
- Initramfs

Die Messwerte können in TPM-PCRs geschrieben werden.

---

# 190. Sicherheitsrichtlinien

Die Bootkonfiguration kann Richtlinien definieren.

Beispiel

```ini
[security]

verify=true

require_signature=true

allow_debug=false

allow_unsigned=false

allow_recovery=true
```

---

# 191. Verschlüsselung

Unterstützt werden

- LUKS (später)
- BitLocker-Erkennung
- NovaCrypt (geplant)

Stage 2 muss verschlüsselte Bootpartitionen entsperren können.

---

# 192. Authentifizierung

Mögliche Verfahren

- Passwort
- TPM
- Recovery Key
- USB Key
- Smartcard (später)
- FIDO2 (später)

Mehrere Verfahren können kombiniert werden.

---

# 193. Boot Policies

```c
typedef struct
{
    bool require_signature;

    bool require_manifest;

    bool allow_debug;

    bool allow_recovery;

    bool allow_test_kernel;

} nova_boot_policy_t;
```

Diese Richtlinien werden vor jedem Boot ausgewertet.

---

# 194. Boot Manifest

Das Manifest beschreibt

- Dateien
- Versionen
- Größen
- Hashes
- Signaturen
- Abhängigkeiten

Nur vollständig validierte Manifeste dürfen verwendet werden.

---

# 195. Rollback Protection

Stage 2 erkennt veraltete Systemstände.

Optional können

- Versionsnummern
- Buildnummern
- Signaturgenerationen

verglichen werden.

Dadurch wird verhindert, dass manipulierte ältere Kernel geladen werden.

---

# 196. Recovery Security

Die Recovery-Umgebung besitzt eigene Sicherheitsregeln.

Standardmäßig gilt

- keine automatische Datenlöschung
- keine automatische Entschlüsselung
- keine Änderung ohne Benutzerbestätigung

---

# 197. Audit Log

Jede sicherheitsrelevante Aktion wird protokolliert.

Beispiele

- Signaturfehler
- TPM-Status
- Kernelprüfung
- Recovery-Start
- Rollback
- Bootmodus

---

# 198. Auditstruktur

```c
typedef struct
{
    uint64_t timestamp;

    uint32_t event_id;

    uint32_t severity;

    char description[256];

} nova_boot_audit_event_t;
```

Alle Ereignisse können an den Kernel übergeben werden.

---

# 199. Sicherheitsfehler

Mögliche Fehler

- ungültige Signatur
- beschädigtes Manifest
- Hashfehler
- Kernel beschädigt
- Modul beschädigt
- TPM nicht verfügbar
- Richtlinienverletzung

Für jeden Fehler existiert ein definierter Recoverypfad.

---

# 200. Sicherheitsstatus

Vor dem Kernelstart wird ein Gesamtstatus erzeugt.

```c
typedef enum
{
    NOVA_BOOT_SECURE,

    NOVA_BOOT_WARNING,

    NOVA_BOOT_RECOVERY,

    NOVA_BOOT_UNSAFE,

    NOVA_BOOT_FATAL

} nova_boot_security_status_t;
```

Der Kernel erhält diesen Status in `nova_boot_info_t`.

---

# 201. Selbstheilung

Falls zulässig,

kann Stage 2

- beschädigte Konfigurationen ersetzen
- Backupkernel laden
- Recovery starten
- Snapshot auswählen

Automatische Reparaturen dürfen niemals Benutzerdaten überschreiben.

---

# 202. Sicherheitsereignisse

Jedes Ereignis besitzt

- Zeit
- Quelle
- Komponente
- Fehlercode
- Schweregrad
- Benutzeraktion (optional)

Dies erleichtert spätere Diagnosen.

---

# 203. Anforderungen

Die Sicherheitsarchitektur MUSS

- Hashes prüfen
- Richtlinien auswerten
- Recovery unterstützen
- Auditinformationen erzeugen
- Sicherheitsstatus an den Kernel übergeben

---

# 204. SHOULD

Die Sicherheitsarchitektur SOLLTE

- TPM unterstützen
- Secure Boot erkennen
- Signaturen prüfen
- Rollback verhindern
- mehrere Vertrauensschlüssel verwalten

---

# 205. MAY

Die Sicherheitsarchitektur DARF

- Netzwerkattestierung
- Remote Trust
- HSM-Unterstützung
- Hardware Security Module
- Mehrfaktor-Authentifizierung

unterstützen.

---

# Architekturhinweis

Die Nova-Sicherheitsarchitektur soll **nicht ausschließlich von UEFI Secure Boot abhängig sein**.

Nova besitzt einen eigenen **plattformunabhängigen Trust Store** und eine eigene Signaturinfrastruktur. Dadurch kann dieselbe Sicherheitsarchitektur auf BIOS-, UEFI-, ARM64-, RISC-V- und Embedded-Systemen verwendet werden.

UEFI Secure Boot wird als zusätzliche Sicherheitsschicht betrachtet, nicht als alleinige Vertrauensquelle.

---

# 206. Boot Logging Runtime

Die **Boot Logging Runtime** zeichnet sämtliche relevanten Ereignisse während des Bootvorgangs auf.

Sie dient

- der Fehlersuche
- der Diagnose
- dem Recovery-System
- dem Kernel
- der Qualitätsanalyse

Das Logsystem ist bereits aktiv, bevor der Kernel gestartet wird.

---

# 207. Ziele

Das Bootlog soll

- deterministisch sein
- möglichst wenig Speicher benötigen
- den Bootvorgang nicht verlangsamen
- nach dem Kernelstart weiterverwendet werden können
- Recovery unterstützen

---

# 208. Architektur

```text
Component

↓

Logger

↓

Ring Buffer

↓

Boot Log

↓

Kernel

↓

System Log
```

Der Kernel übernimmt den Ringbuffer nach dem Start.

---

# 209. Log Level

```c
typedef enum
{
    NOVA_LOG_TRACE,

    NOVA_LOG_DEBUG,

    NOVA_LOG_INFO,

    NOVA_LOG_WARNING,

    NOVA_LOG_ERROR,

    NOVA_LOG_FATAL

} nova_log_level_t;
```

---

# 210. Boot Log Entry

```c
typedef struct
{
    uint64_t timestamp_ns;

    nova_log_level_t level;

    uint32_t component;

    uint32_t event;

    uint32_t error;

    char message[192];

} nova_boot_log_entry_t;
```

Alle Komponenten verwenden dieses Format.

---

# 211. Ring Buffer

Stage 2 verwendet einen festen Ringbuffer.

Eigenschaften

- keine dynamische Speicherverwaltung
- konfigurierbare Größe
- FIFO
- lockfrei
- deterministisch

---

# 212. Boot Log Manager

Der Manager unterstützt

- Schreiben
- Lesen
- Export
- Suche
- Filter
- Übergabe an den Kernel

---

# 213. Ereigniskategorien

Mindestens

- Plattform
- CPU
- Speicher
- Grafik
- Datenträger
- Dateisystem
- Kernel Loader
- Module
- Sicherheit
- Recovery
- Benutzeraktionen

---

# 214. Diagnosesystem

Die Diagnose sammelt

- Hardwareinformationen
- Bootdauer
- Firmwaredaten
- CPU-Features
- Speichernutzung
- Fehler
- Warnungen
- Recovery-Ereignisse

---

# 215. Diagnosebericht

```c
typedef struct
{
    nova_cpu_info_t cpu;

    nova_memory_map_t memory;

    nova_platform_t platform;

    nova_boot_security_status_t security;

    uint64_t boot_time_ns;

} nova_boot_diagnostic_report_t;
```

---

# 216. Performance-Monitor

Während des Bootens misst Stage 2

- CPU-Zeit
- Bootdauer
- Ladezeiten
- Dateisystemzugriffe
- Kernelladezeit
- Modulladezeit

Diese Werte helfen bei der Optimierung.

---

# 217. Boot Timeline

```text
Firmware

↓

Stage 1

↓

Stage 2

↓

Graphics

↓

Filesystem

↓

Kernel

↓

Desktop
```

Für jede Phase wird die benötigte Zeit gespeichert.

---

# 218. Fehlerdiagnose

Jeder Fehler erhält

- Fehlercode
- Komponente
- Zeitpunkt
- Schweregrad
- Recovery-Empfehlung

---

# 219. Fehlerstruktur

```c
typedef struct
{
    uint32_t code;

    uint32_t component;

    uint32_t severity;

    char description[256];

} nova_boot_error_t;
```

---

# 220. Fehlercodes

Empfohlene Gruppen

```text
1000 Plattform

2000 Speicher

3000 Grafik

4000 Storage

5000 Filesystem

6000 Kernel

7000 Module

8000 Security

9000 Recovery
```

Dadurch lassen sich Fehler schnell zuordnen.

---

# 221. Recovery Manager

Der Recovery Manager entscheidet,

ob ein Fehler automatisch behandelt werden kann.

Mögliche Aktionen

- Retry
- Backupkernel
- Snapshot
- Recoverymodus
- Benutzerdialog

---

# 222. Selbstheilung

Stage 2 unterstützt

- Bootkonfiguration reparieren
- Backupkernel laden
- Recoverypartition verwenden
- beschädigte Module ersetzen
- Bootmanifest neu erzeugen

Alle Aktionen werden protokolliert.

---

# 223. Entwicklermodus

Der Entwicklermodus aktiviert

- ausführliche Logs
- Debug Overlay
- Registeranzeige
- Speicherkarte
- Framebufferinformationen
- Bootphasen

Er darf standardmäßig deaktiviert sein.

---

# 224. Wartungsmodus

Der Wartungsmodus stellt Werkzeuge bereit

- Bootloader aktualisieren
- Recovery testen
- Dateisystem prüfen
- Datenträger prüfen
- Bootkonfiguration bearbeiten
- Diagnoseberichte exportieren

---

# 225. Update-System

Stage 2 soll sich selbst aktualisieren können.

Voraussetzungen

- Signaturprüfung
- Backup
- Rollback
- Versionsprüfung

Fehlgeschlagene Updates dürfen den Rechner nicht unbootbar machen.

---

# 226. Boot-Telemetrie

Standardmäßig werden **keine** Daten an externe Systeme übertragen.

Alle Diagnosedaten verbleiben lokal.

Optionale Telemetrie muss

- explizit aktiviert werden
- transparent dokumentiert sein
- DSGVO-konform arbeiten

---

# 227. Recovery-Berichte

Recovery kann Berichte erzeugen.

Inhalte

- Hardware
- Fehler
- Logdateien
- Bootdauer
- Speicherkarte
- Sicherheitsstatus

Die Berichte können später vom Betriebssystem übernommen werden.

---

# 228. Persistente Logs

Optional können Bootlogs

- auf einer Recoverypartition
- auf einem USB-Stick
- in einer Diagnosedatei

gespeichert werden.

Dabei gelten dieselben Sicherheitsrichtlinien wie für andere Bootdaten.

---

# 229. Anforderungen

Das Logging MUSS

- Ringbuffer verwenden
- Fehler protokollieren
- Recovery unterstützen
- an den Kernel übergeben werden
- deterministisch arbeiten

---

# 230. SHOULD

Das Logging SOLLTE

- Performance messen
- Bootphasen aufzeichnen
- Filter unterstützen
- Entwicklerwerkzeuge integrieren

---

# 231. MAY

Das Logging DARF

- JSON-Export
- Binärformat
- Kompression
- Netzwerkexport (Recovery)
- Fernwartung

unterstützen.

---

# Architekturhinweis

Das Bootlog ist **kein separates Debugwerkzeug**, sondern ein Bestandteil der Nova-Plattform.

Jede Komponente – vom Bootloader bis zum Kernel – verwendet dieselben Datenstrukturen und dieselbe Ereignislogik.

Dadurch entsteht eine durchgängige Diagnosekette vom Einschalten des Rechners bis zum vollständig gestarteten Betriebssystem.

---

# 232. Öffentliche Boot API

Stage 2 stellt eine stabile öffentliche API für alle Bootkomponenten bereit.

Alle Komponenten kommunizieren ausschließlich über definierte Schnittstellen.

Dadurch wird verhindert, dass interne Implementierungen voneinander abhängig werden.

---

# 233. Initialisierungs-API

```c
nova_result_t
nova_stage2_initialize(void);
```

Initialisiert

- Boot Core
- Platform Layer
- Memory Runtime
- Graphics Runtime
- Storage Runtime
- Filesystem Runtime
- Security Runtime
- UI Runtime

---

# 234. Platform API

```c
nova_result_t
nova_platform_initialize(void);

nova_result_t
nova_platform_shutdown(void);

nova_platform_t*
nova_platform_get(void);
```

---

# 235. Memory API

```c
void*
nova_boot_allocate(uint64_t size);

void*
nova_boot_allocate_aligned(
    uint64_t size,
    uint64_t alignment);

void
nova_boot_free_all(void);
```

Da Stage 2 kurzlebig ist, erfolgt keine klassische Speicherfreigabe.

---

# 236. Storage API

```c
nova_result_t
nova_storage_read(
    uint64_t device,
    uint64_t lba,
    uint32_t sectors,
    void* buffer);

nova_result_t
nova_storage_scan(void);
```

---

# 237. Filesystem API

```c
nova_result_t
nova_fs_mount(
    nova_partition_t* partition);

nova_result_t
nova_fs_open(
    const char* path,
    nova_file_t* file);

nova_result_t
nova_fs_read(
    nova_file_t* file,
    void* buffer,
    uint64_t bytes);
```

---

# 238. Graphics API

```c
nova_result_t
nova_graphics_initialize(void);

void
nova_present(void);

void
nova_clear(
    uint32_t color);

void
nova_draw_text(
    int x,
    int y,
    const char* text);
```

---

# 239. Kernel Loader API

```c
nova_result_t
nova_kernel_load(
    const char* path,
    nova_kernel_image_t* kernel);

nova_result_t
nova_kernel_verify(
    nova_kernel_image_t* kernel);

NOVA_NORETURN
void
nova_kernel_enter(
    const nova_boot_info_t* info);
```

---

# 240. Recovery API

```c
nova_result_t
nova_recovery_start(void);

nova_result_t
nova_recovery_backup(void);

nova_result_t
nova_recovery_restore(void);
```

---

# 241. Security API

```c
nova_result_t
nova_security_verify_kernel(void);

nova_result_t
nova_security_verify_modules(void);

nova_result_t
nova_security_measure_boot(void);
```

---

# 242. Boot ABI

Zwischen Stage 2 und Kernel existiert genau eine offizielle ABI.

```text
Stage 2

↓

nova_boot_info_t

↓

Kernel
```

Es dürfen keine weiteren versteckten Übergaben existieren.

---

# 243. ABI-Kompatibilität

Jede Version besitzt

```text
Magic

Version

Größe

Feature Flags
```

Der Kernel muss unbekannte Versionen erkennen können.

---

# 244. Versionsverwaltung

```c
typedef struct
{
    uint16_t major;

    uint16_t minor;

    uint16_t patch;

} nova_boot_version_t;
```

Änderungen an der ABI dürfen niemals stillschweigend erfolgen.

---

# 245. Boot Feature Flags

```c
typedef enum
{
    NOVA_BOOT_FEATURE_ACPI        = 1 << 0,
    NOVA_BOOT_FEATURE_GOP         = 1 << 1,
    NOVA_BOOT_FEATURE_VBE         = 1 << 2,
    NOVA_BOOT_FEATURE_TPM         = 1 << 3,
    NOVA_BOOT_FEATURE_SECUREBOOT  = 1 << 4,
    NOVA_BOOT_FEATURE_RECOVERY    = 1 << 5,
    NOVA_BOOT_FEATURE_NETWORKBOOT = 1 << 6,
    NOVA_BOOT_FEATURE_DEBUG       = 1 << 7
} nova_boot_feature_flags_t;
```

---

# 246. Referenzimplementierung

Die empfohlene Reihenfolge lautet

```text
Stage 1

↓

Platform

↓

CPU

↓

Memory

↓

Graphics

↓

Storage

↓

Filesystem

↓

Configuration

↓

UI

↓

Kernel Loader

↓

Module Loader

↓

Security

↓

Boot Info

↓

Kernel
```

Alle Implementierungen sollen diesem Ablauf folgen.

---

# 247. Teststrategie

Jede Änderung an Stage 2 muss automatisiert getestet werden.

Mindestens:

## Plattform

- BIOS
- UEFI
- QEMU
- VirtualBox
- VMware
- reale Hardware

---

## Grafik

- VBE
- GOP
- verschiedene Auflösungen
- HiDPI
- Mehrmonitor (später)

---

## Dateisysteme

- FAT12
- FAT16
- FAT32
- GPT
- ISO9660

---

## Kernel Loader

- ELF32
- ELF64
- ungültige ELF
- beschädigte ELF
- große Kernel

---

## Recovery

- Recoverystart
- Snapshot
- Backup
- Restore
- Boot Repair

---

## Sicherheit

- Signaturprüfung
- TPM
- Secure Boot
- Manifest
- Hashprüfung

---

# 248. Qualitätsanforderungen

Stage 2 MUSS

✔ modular aufgebaut sein

✔ reproduzierbar gebaut werden

✔ dokumentiert sein

✔ deterministisch arbeiten

✔ vollständig testbar sein

✔ ohne undefiniertes Verhalten arbeiten

✔ auf echter Hardware validiert werden

---

# 249. Projektstruktur

Empfohlene Struktur

```text
boot/

├── stage2/

│   ├── core/

│   ├── platform/

│   ├── memory/

│   ├── graphics/

│   ├── renderer/

│   ├── fonts/

│   ├── image/

│   ├── storage/

│   ├── filesystem/

│   ├── kernel/

│   ├── modules/

│   ├── ui/

│   ├── recovery/

│   ├── security/

│   ├── diagnostics/

│   ├── logging/

│   ├── config/

│   ├── input/

│   ├── animation/

│   └── handoff/
```

Diese Struktur dient als Referenz und kann bei Bedarf erweitert werden.

---

# 250. Beziehungen zu anderen NPSPECs

Stage 2 bildet die Grundlage für

- NPSPEC-BOOTGRAPHICS-0001
- NPSPEC-BOOTUI-0001
- NPSPEC-BOOTINPUT-0001
- NPSPEC-BOOTFS-0001
- NPSPEC-KERNELLOADER-0001
- NPSPEC-BOOTSECURITY-0001
- NPSPEC-BOOTRECOVERY-0001
- NPSPEC-BOOTHANDOFF-0001
- NPSPEC-KERNELENTRY-0001

Alle nachfolgenden Spezifikationen konkretisieren einzelne Teilbereiche dieser Architektur.

---

# 251. Implementierungsphasen

## Phase 1

- Stage-2-Grundgerüst
- Protected Mode
- Speicherverwaltung
- VBE

---

## Phase 2

- FAT32
- Kernel Loader
- ELF Loader
- Initramfs

---

## Phase 3

- Grafische Oberfläche
- Bootmenü
- Maus
- Fonts
- PNG
- SVG

---

## Phase 4

- Recovery
- Dateimanager
- Backup
- Diagnose
- Selbstheilung

---

## Phase 5

- UEFI
- Secure Boot
- TPM
- Signaturen
- Measured Boot

---

## Phase 6

- ARM64
- RISC-V
- NovaFS
- Netzwerkboot
- Remote Recovery

---

# 252. Akzeptanzkriterien

NPSPEC-BOOTSTAGE2-0001 gilt als umgesetzt, wenn:

1. Stage 2 BIOS und UEFI unterstützt.
2. Die Plattformabstraktion vollständig funktioniert.
3. Speicher korrekt erkannt wird.
4. VBE und GOP unterstützt werden.
5. GPT und FAT32 gelesen werden können.
6. Der Kernel als ELF32 oder ELF64 geladen wird.
7. Module und Initramfs geladen werden.
8. `nova_boot_info_t` vollständig aufgebaut wird.
9. Der Kernel über eine stabile ABI gestartet wird.
10. Recovery, Logging und Diagnose funktionsfähig sind.
11. Sicherheitsprüfungen vor dem Kernelstart erfolgen.
12. Der Bootloader auf virtueller und realer Hardware erfolgreich arbeitet.

---

# 253. Zusammenfassung

Der **Nova Stage-2-Bootloader** ist weit mehr als ein klassischer Second-Stage-Loader.

Er stellt eine vollständige Bootplattform bereit, die Hardware erkennt, Firmware abstrahiert, Dateisysteme verwaltet, grafische Oberflächen darstellt, Sicherheitsprüfungen durchführt und den Kernel kontrolliert startet.

Stage 2 bildet damit das technische Fundament der gesamten Nova-Plattform.

---

# 254. Langfristige Vision

Langfristig soll Stage 2 nicht nur ein Bootloader sein, sondern eine eigenständige, modulare Laufzeitumgebung mit denselben Qualitätsmerkmalen wie das eigentliche Betriebssystem.

Die Architektur soll:

- auf BIOS, UEFI, ARM64 und RISC-V identisch funktionieren,
- über Jahrzehnte ABI-kompatibel bleiben,
- eine einheitliche Benutzererfahrung vom Einschalten bis zum Desktop bieten,
- Recovery, Diagnose und Selbstheilung als Kernfunktionen integrieren,
- eine sichere und nachvollziehbare Bootkette bereitstellen,
- vollständig modular und erweiterbar sein.

Stage 2 wird damit zur zentralen Brücke zwischen Firmware und Nova Kernel und ist einer der wichtigsten Bausteine der gesamten Nova-Architektur.
