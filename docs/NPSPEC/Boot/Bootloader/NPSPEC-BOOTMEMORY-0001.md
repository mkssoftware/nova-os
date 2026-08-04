# NPSPEC-BOOTMEMORY-0001
# Nova Boot Memory Framework

| Feld | Wert |
|------|------|
| Dokument | NPSPEC-BOOTMEMORY-0001 |
| Titel | Nova Boot Memory Framework |
| Version | 1.0.0 |
| Status | Angenommen |
| Kategorie | Boot Specification |
| Bereich | Nova Boot Platform |
| Übergeordnet | NPSPEC-BOOTSTAGE2-0001 |
| Letzte Änderung | 2026-07-14 |

---

# 1. Zweck

Das **Nova Boot Memory Framework (NBMF)** definiert sämtliche Speichermechanismen des Nova-Bootloaders.

Es stellt dem gesamten Bootloader eine einheitliche, deterministische und plattformunabhängige Speicherverwaltung zur Verfügung.

Alle Speicheroperationen erfolgen ausschließlich über dieses Framework.

---

# 2. Ziele

Das Framework soll

- physikalischen Speicher erkennen
- Speicherkarten vereinheitlichen
- Boot-Heap bereitstellen
- Seitenverwaltung vorbereiten
- Speicher reservieren
- Speicher validieren
- Framebuffer reservieren
- Kernelbereiche reservieren
- Module verwalten
- Initramfs verwalten
- Speicherinformationen an den Kernel übergeben

---

# 3. Designprinzipien

Das Boot Memory Framework folgt folgenden Grundsätzen:

## Determinismus

Identische Hardware muss immer dieselbe Speicherkarte erzeugen.

---

## Plattformunabhängigkeit

BIOS E820

und

UEFI Memory Map

werden intern in dieselben Nova-Strukturen überführt.

---

## Einfache Verwaltung

Während des Bootvorgangs existiert ausschließlich physischer Speicher.

Virtuelle Speicherverwaltung beginnt erst im Kernel.

---

## Keine Fragmentierung

Speicher wird ausschließlich linear vergeben.

Es erfolgt keine Freigabe während Stage 2.

---

## Hohe Nachvollziehbarkeit

Jede Speicherreservierung wird protokolliert.

---

# 4. Architektur

```text
Boot Core
↓
Memory Runtime
├── Memory Map
├── Boot Heap
├── Frame Allocator
├── Page Allocator
├── Reservation Manager
├── Validation
└── Kernel Export
```

---

# 5. Lebenszyklus

```text
Detect Memory

↓

Normalize

↓

Reserve

↓

Allocate

↓

Validate

↓

Kernel Export
```

---

# 6. Aufgaben

Die Memory Runtime übernimmt

- Speichererkennung
- Vereinheitlichung
- Reservierungen
- Boot Heap
- Page Allocation
- Frame Allocation
- Speicherattribute
- Kernelübergabe

---

# 7. Nicht-Aufgaben

Die Memory Runtime verwaltet NICHT

- Virtuellen Speicher
- Demand Paging
- Swap
- Copy-on-Write
- NUMA-Zuweisung
- Heap des Kernels

Diese Aufgaben beginnen erst nach Kernelstart.

---

# 8. Speicherquellen

Unterstützt

## BIOS

- INT 15h E820

---

## UEFI

- GetMemoryMap()

---

## ARM64

- UEFI Memory Map

---

## RISC-V

- Device Tree

---

# 9. Vereinheitlichung

Alle Plattformen liefern intern dieselbe Struktur.

```text
Firmware

↓

Platform Driver

↓

Memory Runtime

↓

Nova Memory Map
```

---

# 10. Speicherregion

```c
typedef struct
{
    uint64_t start;

    uint64_t length;

    nova_memory_type_t type;

    uint64_t attributes;

} nova_memory_region_t;
```

---

# 11. Speicherkarte

```c
typedef struct
{
    uint32_t version;

    uint32_t count;

    nova_memory_region_t* regions;

} nova_memory_map_t;
```

Diese Struktur wird später Bestandteil von

```text
nova_boot_info_t
```

---

# 12. Speicherklassen

```c
typedef enum
{
    NOVA_MEMORY_FREE,

    NOVA_MEMORY_RESERVED,

    NOVA_MEMORY_BOOTLOADER,

    NOVA_MEMORY_KERNEL,

    NOVA_MEMORY_MODULE,

    NOVA_MEMORY_INITRAMFS,

    NOVA_MEMORY_FRAMEBUFFER,

    NOVA_MEMORY_ACPI,

    NOVA_MEMORY_MMIO,

    NOVA_MEMORY_FIRMWARE,

    NOVA_MEMORY_RUNTIME,

    NOVA_MEMORY_BAD

} nova_memory_type_t;
```

---

# 13. Speicherattribute

Jede Region besitzt Attribute.

```c
typedef enum
{
    NOVA_MEMORY_READ       = 1 << 0,

    NOVA_MEMORY_WRITE      = 1 << 1,

    NOVA_MEMORY_EXECUTE    = 1 << 2,

    NOVA_MEMORY_CACHEABLE  = 1 << 3,

    NOVA_MEMORY_RUNTIME    = 1 << 4

} nova_memory_attributes_t;
```

---

# 14. Normalisierung

Alle Plattformen werden auf dieselben Nova-Typen abgebildet.

Beispiel

```text
E820 Usable

↓

NOVA_MEMORY_FREE
```

```text
UEFI Conventional

↓

NOVA_MEMORY_FREE
```

Dadurch kennt der Boot Core keine Firmwaretypen.

---

# 15. Anforderungen

Die Memory Runtime MUSS

- BIOS unterstützen
- UEFI unterstützen
- eine einheitliche Memory Map erzeugen
- deterministisch arbeiten
- versionierte Datenstrukturen verwenden

---

# 16. SHOULD

Die Memory Runtime SOLLTE

- zukünftige Plattformen unterstützen
- reproduzierbar arbeiten
- vollständig dokumentiert sein

---

# 17. MAY

Die Memory Runtime DARF

- Diagnoseinformationen
- Debugausgaben
- Performance-Messungen

bereitstellen.

---

# 18. Memory Discovery

Nach der Initialisierung des Platform Layers beginnt die automatische Speichererkennung.

```text
Platform Driver

↓

Firmware Memory Map

↓

Memory Runtime

↓

Nova Memory Map
```

Der Boot Core arbeitet ausschließlich mit der Nova Memory Map.

---

# 19. Speicherquellen

Je nach Plattform werden unterschiedliche Quellen verwendet.

## BIOS

```text
INT 15h

↓

E820
```

---

## UEFI

```text
GetMemoryMap()
```

---

## ARM64

```text
UEFI Memory Map
```

oder

```text
Device Tree
```

---

## RISC-V

```text
Device Tree
```

---

# 20. Discovery Workflow

```text
Detect Platform

↓

Read Firmware Memory

↓

Normalize

↓

Validate

↓

Sort

↓

Merge

↓

Reserve

↓

Boot Runtime
```

---

# 21. BIOS E820

Bei BIOS wird die vollständige E820-Speicherkarte gelesen.

Unterstützte Typen

- Usable RAM
- Reserved
- ACPI Reclaim
- ACPI NVS
- Bad Memory

Alle weiteren Typen werden als Reserved behandelt.

---

# 22. UEFI Memory Map

Unter UEFI werden sämtliche Memory Descriptors verarbeitet.

Mindestens

- EfiConventionalMemory
- EfiLoaderCode
- EfiLoaderData
- EfiBootServicesCode
- EfiBootServicesData
- EfiRuntimeServicesCode
- EfiRuntimeServicesData
- EfiACPIReclaimMemory
- EfiACPIMemoryNVS
- EfiMemoryMappedIO

---

# 23. Device Tree Memory

Auf ARM64 und RISC-V werden Speicherbereiche aus dem Device Tree gelesen.

Unterstützt

- memory nodes
- reserved-memory
- initrd
- framebuffer

---

# 24. Plattformmapping

Alle Plattformtypen werden auf Nova Memory Types abgebildet.

```text
E820 Usable

↓

FREE
```

```text
UEFI Conventional

↓

FREE
```

```text
Device Tree memory

↓

FREE
```

---

# 25. Sortierung

Nach der Erkennung werden alle Regionen sortiert.

Sortierkriterium

```text
Startadresse

↓

aufsteigend
```

Dies erleichtert spätere Optimierungen.

---

# 26. Zusammenführen

Benachbarte Regionen desselben Typs werden automatisch zusammengeführt.

Beispiel

```text
0x100000

↓

2 MB FREE
```

```text
0x300000

↓

2 MB FREE
```

↓

```text
0x100000

↓

4 MB FREE
```

Dadurch wird die Speicherkarte übersichtlicher.

---

# 27. Validierung

Vor der Verwendung werden geprüft

✔ Überlappungen

✔ ungültige Größen

✔ Nullbereiche

✔ falsche Ausrichtung

✔ Firmwarefehler

Ungültige Regionen werden verworfen.

---

# 28. Überlappungen

Überlappen sich Regionen,

gilt folgende Priorität

```text
Bad Memory

↓

Reserved

↓

Firmware

↓

ACPI

↓

Kernel

↓

Free
```

Dadurch überschreibt niemals freier Speicher reservierte Bereiche.

---

# 29. Speichergrenzen

Unterstützt werden

- unter 1 MB
- High Memory
- \> 4 GB
- 64 Bit Adressen

Der Bootloader arbeitet intern ausschließlich mit 64-Bit-Adressen.

---

# 30. Reserved Memory

Folgende Bereiche werden grundsätzlich reserviert

- BIOS
- Firmware
- Stage 1
- Stage 2
- Boot Heap
- Framebuffer
- ACPI
- MMIO
- Kernel
- Module
- Initramfs

Diese Bereiche dürfen niemals erneut vergeben werden.

---

# 31. Speicherstatistik

Während der Erkennung werden Statistiken erzeugt.

```c
typedef struct
{
    uint64_t total_memory;

    uint64_t usable_memory;

    uint64_t reserved_memory;

    uint64_t firmware_memory;

    uint64_t bad_memory;

} nova_memory_statistics_t;
```

---

# 32. Diagnose

Die Diagnose zeigt

- Gesamtspeicher
- Nutzbarer Speicher
- Reservierter Speicher
- Anzahl Regionen
- Größte freie Region

Diese Informationen stehen Recovery und Kernel zur Verfügung.

---

# 33. Memory Report

```text
Memory Report

↓

Firmware

↓

Free

↓

Reserved

↓

Framebuffer

↓

Kernel

↓

Modules

↓

Heap
```

---

# 34. Fehlerbehandlung

Mögliche Fehler

- ungültige E820
- beschädigte UEFI Memory Map
- fehlender Device Tree
- Speicherüberlappungen
- ungültige Adressen

Alle Fehler werden protokolliert.

---

# 35. Speicherlimits

Mindestens unterstützt

- 16 MB
- 32 MB
- 64 MB
- 128 MB
- 256 MB
- 512 MB
- 1 GB
- 2 GB
- 4 GB
- >4 GB

Nova besitzt keine künstliche Obergrenze.

---

# 36. Fragmentierung

Die Memory Runtime analysiert

- Anzahl Regionen
- freie Bereiche
- Fragmentierung

Diese Informationen dienen später dem Kernel.

---

# 37. Konsistenzprüfung

Vor Abschluss der Erkennung wird geprüft

✔ sortiert

✔ keine Überlappung

✔ gültige Größen

✔ gültige Typen

✔ gültige Attribute

Erst danach wird die Memory Map freigegeben.

---

# 38. Anforderungen

Die Memory Discovery MUSS

- BIOS unterstützen
- UEFI unterstützen
- Device Trees unterstützen
- Regionen sortieren
- Regionen validieren
- Regionen zusammenführen

---

# 39. SHOULD

Die Memory Discovery SOLLTE

- Firmwarefehler erkennen
- Statistiken erzeugen
- Diagnose unterstützen
- große Speichermengen effizient verarbeiten

---

# 40. MAY

Die Memory Discovery DARF

- Herstellerfehler automatisch korrigieren
- Warnungen ausgeben
- Debugberichte erzeugen

solange die ursprünglichen Firmwaredaten dokumentiert bleiben.

---

# Architekturentscheidung (ADR)

Die **Nova Memory Map** ist die einzige offizielle Darstellung des physischen Speichers innerhalb des Bootloaders.

Firmware-spezifische Formate wie **BIOS E820**, **UEFI Memory Descriptors** oder **Device Trees** werden unmittelbar nach der Erkennung in die Nova Memory Map überführt und anschließend verworfen.

Dadurch arbeiten alle weiteren Komponenten – einschließlich Boot Heap, Page Allocator, Kernel Loader und `nova_boot_info_t` – ausschließlich mit einer plattformunabhängigen Speicherrepräsentation.

---

# 41. Boot Heap

Der **Boot Heap** stellt den allgemeinen Arbeitsspeicher des Bootloaders bereit.

Alle dynamischen Datenstrukturen von Stage 2 werden ausschließlich aus diesem Heap reserviert.

Dazu gehören beispielsweise

- Listen
- Konfigurationsdaten
- Dateisystemstrukturen
- ACPI-Daten
- SMBIOS-Daten
- PCI-Geräte
- Bootmodule
- UI-Objekte

---

# 42. Designziele

Der Boot Heap soll

- extrem schnell arbeiten
- deterministisch sein
- keine Fragmentierung erzeugen
- keine Speicherfreigabe benötigen
- einfach überprüfbar sein

---

# 43. Architektur

```text
Memory Runtime
↓
Boot Heap
├── Linear Allocator
├── Alignment Manager
├── Reservation Manager
├── Validation
└── Statistics
```

---

# 44. Speicherbereich

Der Boot Heap wird einmalig angelegt.

```text
Memory Map

↓

größte freie Region

↓

Heap reservieren

↓

Boot Runtime
```

Die Größe ist konfigurierbar.

Standard

```text
16 MB
```

Empfohlen

```text
32–64 MB
```

---

# 45. Heapstruktur

```c
typedef struct
{
    void* start;

    void* current;

    void* end;

    uint64_t size;

    uint64_t used;

} nova_boot_heap_t;
```

---

# 46. Linear Allocator

Stage 2 verwendet standardmäßig einen **Linear Allocator**.

```text
Allocate

↓

Current Pointer

↓

Current + Size

↓

Return Pointer
```

Es existiert keine Freiliste.

---

# 47. Keine Freigabe

Während Stage 2 wird Speicher grundsätzlich **nicht** freigegeben.

Gründe

- einfacher Code
- deterministisch
- keine Fragmentierung
- maximale Geschwindigkeit

Nach dem Kernelstart wird der gesamte Heap verworfen.

---

# 48. Boot Allocate

```c
void*
nova_boot_allocate(
    uint64_t size);
```

Eigenschaften

- linear
- O(1)
- deterministisch

---

# 49. Alignment

Alle Speicherreservierungen erfolgen ausgerichtet.

Standard

```text
16 Byte
```

Unterstützt

- 8 Byte
- 16 Byte
- 32 Byte
- 64 Byte
- 4 KB
- 2 MB

---

# 50. Aligned Allocate

```c
void*
nova_boot_allocate_aligned(
    uint64_t size,
    uint64_t alignment);
```

Diese Funktion wird insbesondere für

- Seitentabellen
- Framebuffer
- DMA
- Kernelabbilder

verwendet.

---

# 51. Reservierungsmanager

Der Reservierungsmanager verhindert doppelte Belegungen.

```text
Allocate

↓

Reserve Region

↓

Validate

↓

Return
```

---

# 52. Speicherblock

```c
typedef struct
{
    void* address;

    uint64_t size;

    uint32_t flags;

} nova_memory_block_t;
```

---

# 53. Speicherflags

```c
typedef enum
{
    NOVA_BLOCK_HEAP       = 1 << 0,

    NOVA_BLOCK_KERNEL     = 1 << 1,

    NOVA_BLOCK_MODULE     = 1 << 2,

    NOVA_BLOCK_FRAMEBUFFER= 1 << 3,

    NOVA_BLOCK_PAGE_TABLE = 1 << 4,

    NOVA_BLOCK_RESERVED   = 1 << 5

} nova_memory_block_flags_t;
```

---

# 54. Speicherprüfung

Vor jeder Reservierung wird geprüft

✔ Heap vorhanden

✔ genügend Speicher

✔ Alignment gültig

✔ keine Überlappung

---

# 55. Heap Overflow

Reicht der Speicher nicht aus,

wird

```text
Allocate

↓

Out of Memory

↓

Boot Panic
```

ausgelöst.

Es erfolgt keine automatische Erweiterung.

---

# 56. Speicherlayout

```text
Low Memory

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

Free Memory
```

---

# 57. Heap Statistik

```c
typedef struct
{
    uint64_t total;

    uint64_t used;

    uint64_t free;

    uint64_t allocations;

} nova_boot_heap_statistics_t;
```

---

# 58. Speicherverbrauch

Während des Bootens wird protokolliert

- Anzahl Reservierungen
- größte Reservierung
- freier Heap
- Peak-Nutzung

Diese Informationen werden im Diagnosebericht gespeichert.

---

# 59. Lebenszyklus

```text
Create

↓

Reserve

↓

Allocate

↓

Kernel Start

↓

Destroy
```

Nach dem Kernelstart existiert der Heap nicht mehr.

---

# 60. Reservierungsreihenfolge

Empfohlene Reihenfolge

```text
Page Tables

↓

Framebuffer

↓

Kernel

↓

Module

↓

Boot Info

↓

ACPI

↓

SMBIOS

↓

Heap Objekte
```

Dadurch bleiben große Speicherblöcke möglichst zusammenhängend.

---

# 61. Anforderungen

Der Boot Heap MUSS

- linear arbeiten
- deterministisch sein
- Alignment unterstützen
- Überläufe erkennen
- Statistiken erzeugen

---

# 62. SHOULD

Der Boot Heap SOLLTE

- Cachefreundlich sein
- SIMD-Ausrichtung unterstützen
- Diagnoseinformationen liefern
- reproduzierbar arbeiten

---

# 63. MAY

Der Boot Heap DARF

- Guard Pages vorbereiten
- Speichervergiftung (Memory Poisoning) für Debug-Builds nutzen
- Canary-Werte einsetzen
- zusätzliche Validierungsprüfungen durchführen

Diese Funktionen dürfen in Release-Builds deaktiviert werden.

---

# Architekturentscheidung (ADR)

Der Nova Boot Heap verwendet bewusst einen **Linear Allocator (Bump Allocator)**.

Auf komplexe Allokatoren mit Freilisten, Buddy-System oder Slab-Caches wird im Bootloader verzichtet, da die Lebensdauer aller Objekte auf den Zeitraum zwischen Start von Stage 2 und dem Kernel-Handoff begrenzt ist.

Dies führt zu:

- O(1)-Allokationen
- keiner Fragmentierung
- minimalem Codeumfang
- einfacher Verifizierbarkeit
- hoher Performance

Der eigentliche Kernel übernimmt nach dem Bootvorgang die vollständige Speicherverwaltung.

---

# 64. Physical Frame Allocator

Der **Physical Frame Allocator (PFA)** verwaltet den gesamten freien physischen Arbeitsspeicher.

Er stellt Speicher ausschließlich in Form physischer Speicherframes bereit.

Der Boot Heap verwendet den PFA als Speicherquelle.

---

# 65. Ziele

Der Frame Allocator soll

- freie Speicherframes verwalten
- Frames reservieren
- Frames freigeben (optional)
- Kernelbereiche markieren
- Framebuffer reservieren
- DMA unterstützen
- deterministisch arbeiten

---

# 66. Architektur

```text
Memory Runtime
↓
Frame Allocator
├── Frame Bitmap
├── Reservation Manager
├── Validation
├── Statistics
└── Kernel Export
```

---

# 67. Speicherframe

Nova verwendet standardmäßig

```text
4 KB
```

als kleinste physische Speichereinheit.

```text
Physical Memory

↓

4 KB Frames
```

Alle physischen Speicherbereiche werden intern in Frames zerlegt.

---

# 68. Framenummer

```text
Frame Number

=

Physical Address

/

4096
```

Beispiel

```text
0x00100000

↓

Frame 256
```

---

# 69. Frame Status

```c
typedef enum
{
    NOVA_FRAME_FREE,

    NOVA_FRAME_RESERVED,

    NOVA_FRAME_BOOT,

    NOVA_FRAME_KERNEL,

    NOVA_FRAME_MODULE,

    NOVA_FRAME_FRAMEBUFFER,

    NOVA_FRAME_ACPI,

    NOVA_FRAME_MMIO,

    NOVA_FRAME_BAD

} nova_frame_state_t;
```

---

# 70. Frame Bitmap

Die Verwaltung erfolgt über eine Bitmap.

```text
Bit = 0

↓

frei
```

```text
Bit = 1

↓

belegt
```

Dies ermöglicht eine sehr kompakte Darstellung.

---

# 71. Bitmapstruktur

```c
typedef struct
{
    uint64_t frame_count;

    uint64_t free_frames;

    uint64_t used_frames;

    uint8_t* bitmap;

} nova_frame_bitmap_t;
```

---

# 72. Initialisierung

Die Bitmap wird direkt aus der Nova Memory Map erzeugt.

```text
Memory Map

↓

Frame Bitmap

↓

Frame Allocator
```

Alle reservierten Regionen werden unmittelbar markiert.

---

# 73. Reservierte Frames

Standardmäßig reserviert

- Stage 1
- Stage 2
- Boot Heap
- Kernel
- Initramfs
- Module
- Framebuffer
- ACPI
- MMIO
- Firmware

Diese Frames dürfen niemals erneut vergeben werden.

---

# 74. Allocate Frame

```c
uint64_t
nova_frame_allocate(void);
```

Rückgabe

```text
Physische Adresse
```

oder

```text
INVALID_FRAME
```

---

# 75. Mehrere Frames

```c
uint64_t
nova_frame_allocate_contiguous(
    uint64_t frame_count);
```

Wird verwendet für

- Kernel
- DMA
- große Buffer
- Seitentabellen

---

# 76. Freigabe

Optional

```c
void
nova_frame_release(
    uint64_t address);
```

Standardmäßig wird während Stage 2 keine Freigabe benötigt.

Die API existiert bereits für spätere Erweiterungen.

---

# 77. Page Allocator

Der Page Allocator arbeitet oberhalb des Frame Allocators.

Er verwaltet

- 4-KB-Seiten
- 2-MB-Seiten
- 1-GB-Seiten

Die tatsächlichen Frames stammen immer aus dem PFA.

---

# 78. Unterstützte Seitengrößen

```text
4 KB

Standard
```

```text
2 MB

Huge Pages
```

```text
1 GB

Gigantic Pages
```

Welche Größen verfügbar sind, hängt von den CPU-Fähigkeiten ab.

---

# 79. Seitenausrichtung

Alle Seiten müssen korrekt ausgerichtet sein.

| Seitengröße | Ausrichtung |
|-------------|------------:|
| 4 KB | 4 KB |
| 2 MB | 2 MB |
| 1 GB | 1 GB |

---

# 80. Page Descriptor

```c
typedef struct
{
    uint64_t physical;

    uint64_t virtual;

    uint64_t flags;

} nova_page_descriptor_t;
```

---

# 81. Identity Mapping

Während des Bootvorgangs verwendet Nova standardmäßig Identity Mapping.

```text
Physisch

0x100000

↓

Virtuell

0x100000
```

Dadurch bleibt der Übergang einfach nachvollziehbar.

---

# 82. Higher Half Mapping

Optional bereitet Stage 2 bereits den späteren Higher-Half-Kernel vor.

Beispiel

```text
Physisch

0x00100000

↓

Virtuell

0xFFFFFFFF80000000
```

Die eigentliche Aktivierung erfolgt erst unmittelbar vor oder während des Kernelstarts.

---

# 83. Kernel Mapping

Für das Kernelabbild werden zusammenhängende Seiten reserviert.

Eigenschaften

- korrekt ausgerichtet
- schreibgeschützt nach dem Laden (wenn möglich)
- ausführbar nur für Codebereiche
- Datenbereiche nicht ausführbar (NX, sofern verfügbar)

---

# 84. Framebuffer Mapping

Der Framebuffer erhält eine eigene Speicherklasse.

Eigenschaften

- nicht verschiebbar
- Write Combining (wenn unterstützt)
- Kernelzugriff vorbereitet

---

# 85. Page Tables

Stage 2 erzeugt die ersten Seitentabellen.

Mindestens

- PML4 (x86-64)
- PDPT
- Page Directory
- Page Table

Bei 32-Bit-Systemen entsprechend:

- Page Directory
- Page Table

---

# 86. Paging Workflow

```text
Memory Map

↓

Frame Allocator

↓

Page Allocator

↓

Page Tables

↓

Kernel
```

---

# 87. Speicherschutz

Der Page Allocator unterstützt vorbereitend folgende Attribute:

- Read
- Write
- Execute
- No Execute (NX)
- Global
- Cache Disable
- Write Combining

Die tatsächliche Aktivierung hängt von Architektur und CPU-Unterstützung ab.

---

# 88. Speicherstatistik

```c
typedef struct
{
    uint64_t total_frames;

    uint64_t free_frames;

    uint64_t reserved_frames;

    uint64_t kernel_frames;

    uint64_t framebuffer_frames;

} nova_frame_statistics_t;
```

---

# 89. Anforderungen

Der Physical Frame Allocator MUSS

- 4-KB-Frames verwalten
- Bitmap verwenden
- zusammenhängende Frames reservieren können
- Kernelbereiche markieren
- deterministisch arbeiten

---

# 90. SHOULD

Der Physical Frame Allocator SOLLTE

- Huge Pages unterstützen
- Speicherstatistiken erzeugen
- Alignment validieren
- große Speichermengen effizient verwalten

---

# 91. MAY

Der Physical Frame Allocator DARF

- NUMA-Knoten berücksichtigen
- Memory Coloring vorbereiten
- Debug-Markierungen
- Guard Frames
- Poison Pages

unterstützen.

---

# Architekturentscheidung (ADR)

Der Nova Bootloader verwendet für den **Physical Frame Allocator** bewusst eine **Bitmap-basierte Verwaltung**.

Gründe:

- sehr geringer Speicherverbrauch
- O(1)-Zugriff auf den Status einzelner Frames
- einfache Implementierung
- deterministisches Verhalten
- leicht auf den Kernel übertragbar

Der spätere Nova-Kernel kann diese Datenstruktur direkt übernehmen oder in eine komplexere Speicherverwaltung (z. B. Buddy Allocator oder Slab Allocator) überführen.

---

# 92. Memory Reservation Manager

Der **Memory Reservation Manager (MRM)** verwaltet sämtliche reservierten Speicherbereiche während des Bootvorgangs.

Er stellt sicher, dass kein Speicherbereich mehrfach vergeben wird.

Alle Reservierungen erfolgen ausschließlich über den MRM.

---

# 93. Ziele

Der Reservation Manager soll

- Reservierungen verwalten
- Konflikte erkennen
- Überlappungen verhindern
- Prioritäten berücksichtigen
- Reservierungen dokumentieren
- Kernelinformationen erzeugen

---

# 94. Architektur

```text
Memory Runtime
↓
Reservation Manager
├── Reservation Table
├── Conflict Detection
├── Validator
├── Statistics
└── Kernel Export
```

---

# 95. Reservierungsobjekt

```c
typedef struct
{
    uint64_t address;

    uint64_t size;

    nova_memory_type_t type;

    uint64_t flags;

    char owner[64];

} nova_memory_reservation_t;
```

---

# 96. Reservierungstabelle

```c
typedef struct
{
    uint32_t count;

    nova_memory_reservation_t* entries;

} nova_reservation_table_t;
```

Alle Reservierungen werden zentral gespeichert.

---

# 97. Standardreservierungen

Unmittelbar nach der Speichererkennung werden reserviert

- Stage 1
- Stage 2
- Boot Heap
- Platform Runtime
- Graphics Runtime
- Reservation Table
- Boot Information

---

# 98. Kernelreservierung

Vor dem Laden des Kernels wird ein zusammenhängender Speicherbereich reserviert.

Eigenschaften

- physisch zusammenhängend
- korrekt ausgerichtet
- überschreibgeschützt
- reserviert bis Kernelstart

---

# 99. Kernelobjekt

```c
typedef struct
{
    uint64_t physical;

    uint64_t virtual;

    uint64_t size;

    uint64_t alignment;

} nova_kernel_memory_t;
```

---

# 100. Modulreservierungen

Alle Kernelmodule erhalten eigene Reservierungen.

```text
Module

↓

Reservation Manager

↓

Memory Map
```

Module dürfen sich niemals gegenseitig überlappen.

---

# 101. Initramfs

Für Initramfs wird ein eigener Speicherbereich reserviert.

Eigenschaften

- physisch zusammenhängend
- lesbar
- unveränderlich bis Kernelübergabe

---

# 102. Boot Assets

Auch grafische Ressourcen werden reserviert.

Dazu gehören

- PNG
- Icons
- Fonts
- Themes
- Animationstabellen

Diese Bereiche werden als `BOOTLOADER` markiert.

---

# 103. DMA-Bereiche

Für zukünftige DMA-Anforderungen können reserviert werden

- DMA32
- DMA64
- Gerätepuffer

Der Reservation Manager berücksichtigt entsprechende Adressgrenzen.

---

# 104. MMIO

Memory Mapped IO wird grundsätzlich reserviert.

Typische Bereiche

- PCI BARs
- APIC
- IOAPIC
- HPET
- Framebuffer
- Firmwareregister

Diese Bereiche dürfen niemals allokiert werden.

---

# 105. ACPI

Alle ACPI-Bereiche werden reserviert.

Unterstützt

- ACPI Reclaim
- ACPI NVS
- zukünftige ACPI-Erweiterungen

Der Kernel erhält diese Informationen unverändert.

---

# 106. Firmwarebereiche

Reserviert werden außerdem

- BIOS ROM
- UEFI Runtime
- UEFI Boot Services (bis ExitBootServices)
- Firmwaretabellen
- SMBIOS
- Device Tree

---

# 107. Framebuffer

Der Framebuffer besitzt höchste Priorität.

Eigenschaften

- nicht verschiebbar
- keine Überschneidung
- dauerhaft reserviert
- Übergabe an Kernel

---

# 108. Konflikterkennung

Vor jeder Reservierung erfolgt

```text
Neue Region

↓

Vergleich

↓

Überlappung?

↓

Ja → Fehler

Nein → Reservieren
```

---

# 109. Prioritäten

Bei Konflikten gilt folgende Reihenfolge

```text
Bad Memory

↓

Firmware

↓

MMIO

↓

Framebuffer

↓

Kernel

↓

Module

↓

Initramfs

↓

Heap

↓

Free
```

Höher priorisierte Bereiche können nicht überschrieben werden.

---

# 110. Reservierungsflags

```c
typedef enum
{
    NOVA_RESERVE_READ       = 1 << 0,

    NOVA_RESERVE_WRITE      = 1 << 1,

    NOVA_RESERVE_EXECUTE    = 1 << 2,

    NOVA_RESERVE_RUNTIME    = 1 << 3,

    NOVA_RESERVE_PERMANENT  = 1 << 4

} nova_reservation_flags_t;
```

---

# 111. Reservierungs-API

```c
nova_result_t
nova_memory_reserve(
    uint64_t address,
    uint64_t size,
    nova_memory_type_t type);
```

---

# 112. Automatische Reservierung

Während des Bootens reserviert der Manager automatisch

- Boot Heap
- Kernel
- Module
- Initramfs
- Bootinformationen
- Seitentabellen
- Diagnosebereiche

Der Boot Core muss diese Bereiche nicht manuell markieren.

---

# 113. Reservierungsstatistik

```c
typedef struct
{
    uint64_t reserved_bytes;

    uint64_t reservation_count;

    uint64_t largest_region;

    uint64_t conflicts;

} nova_reservation_statistics_t;
```

---

# 114. Validierung

Vor Abschluss der Initialisierung wird geprüft

✔ keine Überlappungen

✔ gültige Adressen

✔ gültige Größen

✔ eindeutige Eigentümer

✔ gültige Typen

---

# 115. Export

Alle Reservierungen werden Bestandteil von

```text
nova_boot_info_t
```

Der Kernel übernimmt diese Informationen unverändert.

---

# 116. Anforderungen

Der Reservation Manager MUSS

- sämtliche Reservierungen zentral verwalten
- Konflikte erkennen
- Prioritäten beachten
- Überlappungen verhindern
- Reservierungen exportieren

---

# 117. SHOULD

Der Reservation Manager SOLLTE

- Reservierungen protokollieren
- Diagnoseinformationen erzeugen
- zukünftige Speicherklassen unterstützen
- reproduzierbar arbeiten

---

# 118. MAY

Der Reservation Manager DARF

- Debug-Reservierungen
- temporäre Reservierungen
- Performance-Marker
- Speicher-Tags
- zukünftige CXL-Speicherbereiche

unterstützen.

---

# 119. Architekturentscheidung (ADR)

Der **Memory Reservation Manager** ist die zentrale Instanz für sämtliche Speicherreservierungen.

Es ist **nicht zulässig**, Speicherbereiche direkt in der Memory Map oder Bitmap als reserviert zu markieren.

Alle Reservierungen müssen ausschließlich über den Reservation Manager erfolgen.

Dadurch wird gewährleistet:

- vollständige Nachvollziehbarkeit
- konsistente Speicherkarten
- konfliktfreie Allokation
- einfache Diagnose
- reproduzierbares Verhalten

Der Reservation Manager bildet damit die verbindliche Quelle für alle belegten Speicherbereiche während des gesamten Bootvorgangs.

---

# 120. Memory Validation Engine

Die **Memory Validation Engine (MVE)** überprüft sämtliche Speicherinformationen vor der Übergabe an den Kernel.

Sie stellt sicher, dass

- die Memory Map konsistent ist
- Reservierungen korrekt sind
- Speichergrenzen eingehalten werden
- Firmwarefehler erkannt werden
- keine ungültigen Bereiche existieren

Erst nach erfolgreicher Validierung darf der Kernel gestartet werden.

---

# 121. Ziele

Die Validation Engine soll

- Speicher validieren
- Firmwarefehler erkennen
- Überlappungen erkennen
- Bad Memory markieren
- Alignment prüfen
- Speicherdiagnosen erzeugen
- Kernelinformationen vorbereiten

---

# 122. Architektur

```text
Memory Runtime
↓
Validation Engine
├── Map Validator
├── Reservation Validator
├── Alignment Validator
├── Bad Memory Scanner
├── Statistics
└── Diagnostic Logger
```

---

# 123. Validierungsablauf

```text
Memory Discovery

↓

Normalize

↓

Reservations

↓

Validate

↓

Repair (optional)

↓

Export
```

---

# 124. Map Validator

Der Map Validator prüft

- Regionen sortiert
- keine Überlappungen
- gültige Größen
- gültige Typen
- gültige Attribute

Nur eine vollständig gültige Memory Map darf exportiert werden.

---

# 125. Address Validation

Jede Region wird geprüft.

```text
Start < Ende

↓

Größe > 0

↓

64 Bit gültig

↓

Ausrichtung gültig
```

Ungültige Regionen werden verworfen.

---

# 126. Alignment Validation

Standardprüfungen

| Objekt | Alignment |
|---------|----------:|
| Heap | 16 Byte |
| Pages | 4096 Byte |
| Huge Pages | 2 MB |
| Gigantic Pages | 1 GB |
| DMA | Plattformabhängig |

---

# 127. Region Validation

Für jede Region wird geprüft

✔ Typ gültig

✔ Attribute gültig

✔ Startadresse gültig

✔ Endadresse gültig

✔ Größe gültig

✔ keine Überlappung

---

# 128. Bad Memory Detection

Defekte Speicherbereiche werden automatisch erkannt.

Quellen

- BIOS E820
- UEFI Memory Map
- Firmwaretabellen
- Diagnose

Alle Bad-Memory-Bereiche werden dauerhaft reserviert.

---

# 129. Bad Memory Runtime

```c
typedef struct
{
    uint64_t address;

    uint64_t size;

    uint32_t reason;

} nova_bad_memory_region_t;
```

---

# 130. Firmware Validation

Firmwareinformationen werden geprüft auf

- doppelte Regionen
- ungültige Typen
- inkonsistente Attribute
- fehlerhafte Größen
- ungültige Adressen

Firmwarefehler werden protokolliert.

---

# 131. Reservation Validation

Alle Reservierungen werden erneut überprüft.

```text
Reservation

↓

Memory Map

↓

Conflict Check

↓

Valid
```

Konflikte verhindern den Kernelstart.

---

# 132. Duplicate Detection

Doppelte Reservierungen werden erkannt.

```text
Kernel

Kernel

↓

Duplicate

↓

Error
```

---

# 133. Memory Statistics Validation

Statistiken werden geprüft.

```text
Total

=

Reserved

+

Free

+

Bad

+

Firmware
```

Inkonsistenzen erzeugen Diagnoseeinträge.

---

# 134. ECC-Unterstützung

Falls verfügbar,

werden ECC-Informationen übernommen.

Unterstützt

- ECC aktiv
- ECC Fehler
- korrigierbare Fehler
- nicht korrigierbare Fehler

Die Auswertung erfolgt zunächst nur informativ.

---

# 135. Memory Poisoning

Für Debug-Builds unterstützt Nova

Memory Poisoning.

Beispiel

```text
0xCC

oder

0xDEADBEEF
```

Nicht initialisierte Speicherbereiche können damit erkannt werden.

---

# 136. Guard Regions

Optional können Guard Regions reserviert werden.

Sie dienen

- Debugging
- Speicherüberlauf-Erkennung
- Heap-Validierung

Diese Bereiche dürfen niemals verwendet werden.

---

# 137. Konsistenzprüfung

Vor Abschluss gilt

```text
Memory Map

↓

Reservations

↓

Bitmap

↓

Heap

↓

Kernel

↓

Valid?
```

Alle Komponenten müssen konsistent sein.

---

# 138. Diagnosebericht

Die Validation Engine erzeugt

- Warnungen
- Fehler
- Statistiken
- Speicherberichte
- Firmwarefehler
- Bad Memory Report

---

# 139. Logging

Alle Fehler werden in das zentrale Bootlog geschrieben.

Beispiele

- ungültige Adresse
- Alignmentfehler
- Firmwarefehler
- Überlappung
- Bad Memory

---

# 140. Reparaturmodus

Optional kann die Validation Engine harmlose Fehler korrigieren.

Beispiele

- Regionen zusammenführen
- Sortierung korrigieren
- doppelte freie Bereiche entfernen

Kritische Fehler werden niemals automatisch geändert.

---

# 141. Validation API

```c
nova_result_t
nova_memory_validate(
    nova_memory_map_t* map);
```

---

# 142. Diagnostic API

```c
nova_result_t
nova_memory_diagnostics(
    nova_memory_report_t* report);
```

---

# 143. Validation Report

```c
typedef struct
{
    bool valid;

    uint32_t warnings;

    uint32_t errors;

    uint64_t usable_memory;

    uint64_t reserved_memory;

} nova_memory_validation_report_t;
```

---

# 144. Anforderungen

Die Validation Engine MUSS

- Memory Map validieren
- Reservierungen prüfen
- Überlappungen erkennen
- Bad Memory markieren
- Diagnoseberichte erzeugen

---

# 145. SHOULD

Die Validation Engine SOLLTE

- ECC auswerten
- Debugfunktionen unterstützen
- harmlose Firmwarefehler korrigieren
- Statistiken erzeugen

---

# 146. MAY

Die Validation Engine DARF

- Memory Poisoning
- Guard Regions
- Performance-Messungen
- Hersteller-Workarounds
- erweiterte Integritätsprüfungen

unterstützen.

---

# Architekturentscheidung (ADR)

Die **Memory Validation Engine** ist die letzte Instanz vor dem Kernel-Handoff.

Keine Komponente darf den Kernel starten, solange

- die Memory Map ungültig ist,
- Reservierungen Konflikte enthalten,
- Bad-Memory-Bereiche nicht markiert wurden oder
- schwerwiegende Konsistenzfehler vorliegen.

Damit garantiert Nova, dass der Kernel ausschließlich mit einer vollständig geprüften und konsistenten Speicherrepräsentation startet.

---

# 147. Memory Export

Nach erfolgreicher Initialisierung und Validierung exportiert die Memory Runtime sämtliche relevanten Speicherinformationen an den Kernel.

Ab diesem Zeitpunkt gilt die Memory Runtime als abgeschlossen.

```text
Memory Runtime

↓

Export

↓

nova_boot_info_t

↓

Kernel
```

---

# 148. Ziele

Der Export soll

- vollständig sein
- deterministisch sein
- ABI-stabil sein
- versionsfähig sein
- architekturunabhängig sein

---

# 149. Exportierte Informationen

Mindestens werden exportiert

- Memory Map
- Reservation Table
- Frame Bitmap
- Heap Information
- Boot Statistics
- Validation Report
- Firmware Memory
- Kernel Memory
- Initramfs
- Module

---

# 150. Exportstruktur

```text
Memory Runtime
├── Memory Map
├── Reservations
├── Statistics
├── Frame Bitmap
├── Heap
├── Validation
└── Diagnostics
↓
nova_boot_info_t
```

---

# 151. nova_boot_info_t

Die Memory Runtime ergänzt folgende Informationen.

```c
typedef struct
{
    nova_memory_map_t memory_map;

    nova_reservation_table_t reservations;

    nova_frame_bitmap_t frame_bitmap;

    nova_boot_heap_statistics_t heap;

    nova_memory_statistics_t statistics;

    nova_memory_validation_report_t validation;

} nova_boot_memory_info_t;
```

Diese Struktur wird Bestandteil von

```text
nova_boot_info_t
```

---

# 152. Export API

```c
nova_result_t
nova_memory_export(
    nova_boot_memory_info_t* info);
```

---

# 153. Memory Map Export

Die vollständige Memory Map wird unverändert exportiert.

Eigenschaften

- sortiert
- validiert
- normalisiert
- versioniert

Der Kernel darf die Struktur übernehmen.

---

# 154. Reservation Export

Exportiert werden

- Kernel
- Heap
- Framebuffer
- Module
- Initramfs
- Firmware
- ACPI
- MMIO

Dadurch kennt der Kernel sämtliche belegten Speicherbereiche.

---

# 155. Frame Bitmap Export

Optional kann die Bitmap direkt übernommen werden.

```text
Bootloader Bitmap

↓

Kernel Bitmap
```

Dadurch entfällt eine erneute Initialisierung.

---

# 156. Heap Export

Der Kernel erhält Informationen über

- Startadresse
- Größe
- Peak-Nutzung
- Anzahl Reservierungen

Der Heap selbst wird nach dem Kernelstart verworfen.

---

# 157. Validation Export

Der Kernel erhält

- Warnungen
- Fehler
- Bad Memory
- Firmwarefehler
- Diagnoseinformationen

Dadurch kann später entschieden werden,

ob weitere Prüfungen notwendig sind.

---

# 158. Statistikexport

Exportiert werden

```text
Gesamtspeicher

Freier Speicher

Reservierter Speicher

Bad Memory

Firmware

Kernel

Framebuffer
```

---

# 159. ABI Header

Jede exportierte Struktur beginnt mit

```c
typedef struct
{
    uint64_t magic;

    uint16_t major;

    uint16_t minor;

    uint32_t size;

} nova_memory_abi_header_t;
```

---

# 160. Magic

```text
"NMBI"
```

Nova Memory Boot Information

Dadurch erkennt der Kernel ungültige Daten sofort.

---

# 161. Versionsverwaltung

```text
Major

↓

ABI Änderung

Minor

↓

Neue Felder

Patch

↓

Bugfix
```

---

# 162. Speicherkompatibilität

Die ABI garantiert

- Little Endian

- 64 Bit Adressen

- natürliche Alignmentregeln

- reproduzierbare Strukturgrößen

---

# 163. Lebensdauer

Die exportierten Informationen bleiben gültig,

bis der Kernel sie vollständig übernommen hat.

Danach darf Stage 2 freigegeben werden.

---

# 164. Kernelübernahme

```text
Stage 2

↓

Memory Export

↓

Kernel Initialize

↓

Memory Manager

↓

Boot Runtime Ende
```

---

# 165. Übergabeprinzip

Der Bootloader verändert exportierte Daten nach dem Export nicht mehr.

Der Kernel besitzt danach die vollständige Kontrolle.

---

# 166. Fehlerbehandlung

Schlägt der Export fehl,

erfolgt

```text
Export

↓

Fatal Error

↓

Boot Panic
```

Ein Kernelstart ist in diesem Fall nicht zulässig.

---

# 167. Anforderungen

Der Memory Export MUSS

- vollständig sein
- ABI-stabil sein
- versioniert sein
- validierte Daten exportieren
- deterministisch arbeiten

---

# 168. SHOULD

Der Memory Export SOLLTE

- Diagnoseinformationen enthalten
- Bitmap exportieren
- Statistiken exportieren
- zukünftige Erweiterungen ermöglichen

---

# 169. MAY

Der Memory Export DARF

- Debuginformationen
- Performancewerte
- Herstellerinformationen
- zusätzliche Diagnosedaten

exportieren.

---

# 170. Architekturentscheidung (ADR)

Die **Memory Runtime** ist nach dem Export vollständig abgeschlossen.

Der Kernel übernimmt sämtliche Speicherinformationen unverändert und startet seine eigene Speicherverwaltung auf Basis der exportierten Nova-Datenstrukturen.

Es erfolgt **keine erneute Firmwareabfrage** und **keine zweite Speichererkennung**.

Dadurch wird sichergestellt, dass Bootloader und Kernel exakt dieselbe Sicht auf den physikalischen Speicher besitzen.

---

# 171. Memory Diagnostics Framework

Das **Memory Diagnostics Framework (MDF)** überwacht und analysiert sämtliche Speicheroperationen während des Bootvorgangs.

Es dient

- der Fehlersuche
- der Performanceanalyse
- der Speicherdiagnose
- der Validierung
- dem Recovery-System
- dem Kernel

Alle Diagnosedaten verbleiben standardmäßig lokal.

---

# 172. Ziele

Das Diagnostics Framework soll

- Speicherereignisse protokollieren
- Speicherverbrauch analysieren
- Performance messen
- Fragmentierung analysieren
- Diagnoseberichte erzeugen
- Kernelinformationen bereitstellen

---

# 173. Architektur

```text
Memory Runtime
↓
Diagnostics Runtime
├── Logger
├── Profiler
├── Statistics
├── Fragmentation Analyzer
├── Benchmark
├── Timeline
└── Export
```

---

# 174. Logging

Jede relevante Speicheroperation erzeugt einen Logeintrag.

Beispiele

- Memory Discovery
- Heap Allocation
- Frame Allocation
- Reservierung
- Validation
- Export

---

# 175. Memory Log Entry

```c
typedef struct
{
    uint64_t timestamp;

    uint32_t component;

    uint32_t event;

    uint64_t address;

    uint64_t size;

    nova_result_t result;

} nova_memory_log_entry_t;
```

---

# 176. Boot Timeline

Der Speicherinitialisierungsvorgang wird vollständig aufgezeichnet.

```text
Firmware

↓

Memory Discovery

↓

Normalize

↓

Validation

↓

Heap

↓

Frames

↓

Reservations

↓

Export

↓

Kernel
```

---

# 177. Performance Profiler

Der Profiler misst

- Discovery-Zeit
- Heap-Zeit
- Bitmap-Erzeugung
- Reservierungszeit
- Exportzeit
- Gesamtzeit

Alle Zeiten werden in Nanosekunden gespeichert.

---

# 178. Performance Report

```c
typedef struct
{
    uint64_t discovery_ns;

    uint64_t normalize_ns;

    uint64_t validation_ns;

    uint64_t heap_ns;

    uint64_t export_ns;

    uint64_t total_ns;

} nova_memory_performance_report_t;
```

---

# 179. Speicherstatistik

Die Runtime sammelt kontinuierlich

- Gesamtspeicher
- Freier Speicher
- Reservierter Speicher
- Heapverbrauch
- Anzahl Frames
- Anzahl Reservierungen

---

# 180. Speicherfragmentierung

Die Fragmentierung wird analysiert.

Kennzahlen

- Anzahl freier Bereiche
- größte freie Region
- durchschnittliche Region
- Fragmentierungsgrad

---

# 181. Fragmentierungsbericht

```c
typedef struct
{
    uint64_t free_regions;

    uint64_t largest_region;

    uint64_t smallest_region;

    uint64_t average_region;

    float fragmentation;

} nova_fragmentation_report_t;
```

---

# 182. Diagnosemodi

```text
Normal

↓

Verbose

↓

Debug

↓

Developer
```

Je nach Modus werden unterschiedlich viele Informationen gesammelt.

---

# 183. Memory Monitor

Während Stage 2 werden überwacht

- Heap Peak
- Frameverbrauch
- Reservierungen
- Speicherfehler
- Überläufe
- Alignmentfehler

---

# 184. Speicherwarnungen

Warnungen entstehen beispielsweise bei

- ungewöhnlich hoher Fragmentierung
- fast vollem Heap
- ungewöhnlich vielen Reservierungen
- inkonsistenten Firmwaredaten

Warnungen verhindern den Bootvorgang nicht.

---

# 185. Kritische Fehler

Kritisch sind

- ungültige Memory Map
- Heap Overflow
- doppelte Kernelreservierung
- beschädigte Bitmap
- fehlerhafte Exportstruktur

Diese Fehler führen unmittelbar zum Boot Panic.

---

# 186. Benchmark Runtime

Optional kann Stage 2 einfache Benchmarks ausführen.

Beispiele

- Speicherbandbreite
- Heap-Performance
- Frame Allocation
- Bitmap-Zugriffe

Diese Benchmarks sind standardmäßig deaktiviert.

---

# 187. Diagnosebericht

```c
typedef struct
{
    nova_memory_statistics_t statistics;

    nova_fragmentation_report_t fragmentation;

    nova_memory_performance_report_t performance;

    nova_memory_validation_report_t validation;

} nova_memory_report_t;
```

---

# 188. Export

Der Diagnosebericht wird Bestandteil von

```text
nova_boot_info_t
```

Der Kernel kann diese Informationen später anzeigen oder weiterverarbeiten.

---

# 189. Debug Runtime

Im Entwicklermodus werden zusätzlich angezeigt

- komplette Memory Map
- Frame Bitmap
- Heap Layout
- Reservierungstabelle
- Fragmentierung
- Speicherattribute

Diese Informationen können direkt im Recovery-Modus dargestellt werden.

---

# 190. Bootprofiling

Das Framework erzeugt ein vollständiges Bootprofil.

```text
Memory Discovery

↓

Heap

↓

Frames

↓

Reservations

↓

Validation

↓

Export
```

Dadurch können spätere Optimierungen objektiv bewertet werden.

---

# 191. Diagnose-API

```c
nova_result_t
nova_memory_get_report(
    nova_memory_report_t* report);
```

---

# 192. Log-API

```c
nova_result_t
nova_memory_get_log(
    nova_memory_log_entry_t* entries,
    uint32_t* count);
```

---

# 193. Anforderungen

Das Diagnostics Framework MUSS

- Speicheroperationen protokollieren
- Performance messen
- Fragmentierung analysieren
- Diagnoseberichte erzeugen
- Daten an den Kernel exportieren

---

# 194. SHOULD

Das Diagnostics Framework SOLLTE

- Bootprofile erzeugen
- Debuginformationen bereitstellen
- Benchmarkfunktionen besitzen
- Warnungen klassifizieren

---

# 195. MAY

Das Diagnostics Framework DARF

- grafische Speicherkarten
- Heatmaps
- Live-Diagnose
- Telemetrie (optional)
- JSON-Export

unterstützen.

---

# 196. Architekturentscheidung (ADR)

Die Speicherdiagnose ist kein separates Debugwerkzeug, sondern ein integraler Bestandteil des Nova Boot Memory Frameworks.

Alle Speicheroperationen können nachvollzogen werden, wodurch Fehler bereits während des Bootvorgangs erkannt und später vom Kernel oder der Recovery-Umgebung ausgewertet werden können.

Dies verbessert die Wartbarkeit und erleichtert die Entwicklung zukünftiger Speicherkomponenten erheblich.

---

# 197. Öffentliche Boot Memory API

Das Nova Boot Memory Framework stellt eine stabile öffentliche API bereit.

Diese API ist die einzige zulässige Schnittstelle für sämtliche Speicheroperationen innerhalb des Bootloaders.

```text
Boot Core

↓

Memory API

↓

Memory Runtime

↓

Platform Layer
```

Direkte Zugriffe auf interne Datenstrukturen sind nicht zulässig.

---

# 198. API Designprinzipien

Die Memory API folgt den Grundsätzen

- deterministisch
- ABI-stabil
- modular
- versionsfähig
- plattformneutral
- threadfähig vorbereitet

Alle Änderungen erfolgen ausschließlich über neue API-Versionen.

---

# 199. Initialisierung

```c
nova_result_t
nova_memory_initialize(void);
```

Initialisiert

- Memory Discovery
- Memory Map
- Reservation Manager
- Boot Heap
- Frame Allocator
- Validation Engine
- Diagnostics Runtime

---

# 200. Shutdown

```c
nova_result_t
nova_memory_shutdown(void);
```

Schließt die Memory Runtime kontrolliert ab.

Diese Funktion wird unmittelbar vor dem Kernel-Handoff aufgerufen.

---

# 201. Speicherallokation

```c
void*
nova_memory_allocate(
    uint64_t size);
```

Reserviert Speicher aus dem Boot Heap.

---

# 202. Ausgerichtete Allokation

```c
void*
nova_memory_allocate_aligned(
    uint64_t size,
    uint64_t alignment);
```

Unterstützt

- 8 Byte
- 16 Byte
- 32 Byte
- 64 Byte
- 4 KB
- 2 MB
- 1 GB

---

# 203. Frame API

```c
uint64_t
nova_frame_allocate(void);

uint64_t
nova_frame_allocate_contiguous(
    uint64_t frames);
```

Diese Funktionen arbeiten direkt mit dem Physical Frame Allocator.

---

# 204. Reservierungs-API

```c
nova_result_t
nova_memory_reserve(
    uint64_t address,
    uint64_t size,
    nova_memory_type_t type);
```

Jede Reservierung wird validiert und protokolliert.

---

# 205. Memory Map API

```c
const nova_memory_map_t*
nova_memory_map(void);
```

Liefert die vollständige, validierte Nova Memory Map.

---

# 206. Statistik-API

```c
const nova_memory_statistics_t*
nova_memory_statistics(void);
```

Stellt aktuelle Speicherstatistiken bereit.

---

# 207. Diagnose-API

```c
nova_result_t
nova_memory_report(
    nova_memory_report_t* report);
```

Erzeugt einen vollständigen Diagnosebericht.

---

# 208. Export API

```c
nova_result_t
nova_memory_export(
    nova_boot_memory_info_t* info);
```

Bereitet sämtliche Informationen für den Kernel-Handoff vor.

---

# 209. Fehlercodes

```c
typedef enum
{
    NOVA_MEMORY_SUCCESS = 0,

    NOVA_MEMORY_OUT_OF_MEMORY,

    NOVA_MEMORY_INVALID_REGION,

    NOVA_MEMORY_ALIGNMENT_ERROR,

    NOVA_MEMORY_CONFLICT,

    NOVA_MEMORY_CORRUPTED,

    NOVA_MEMORY_UNSUPPORTED,

    NOVA_MEMORY_FATAL

} nova_memory_result_t;
```

Alle Funktionen liefern ausschließlich standardisierte Fehlercodes.

---

# 210. ABI

Die Memory Runtime besitzt eine eigene ABI.

```text
Magic

↓

Version

↓

Größe

↓

Daten
```

Alle exportierten Strukturen beginnen mit demselben ABI-Header.

---

# 211. ABI Header

```c
typedef struct
{
    uint64_t magic;

    uint16_t major;

    uint16_t minor;

    uint32_t size;

} nova_memory_abi_header_t;
```

---

# 212. Versionsverwaltung

Semantic Versioning

```text
Major

↓

Inkompatible Änderung

Minor

↓

Neue Funktionen

Patch

↓

Fehlerkorrekturen
```

---

# 213. Referenzimplementierung

Empfohlene Projektstruktur

```text
memory/
├── api/
├── discovery/
├── normalize/
├── validation/
├── heap/
├── frames/
├── pages/
├── reservations/
├── diagnostics/
├── export/
├── tests/
└── docs/
```

---

# 214. Unit Tests

Jede Komponente besitzt eigene Unit Tests.

Mindestens

- Heap
- Bitmap
- Allocator
- Validation
- Reservations
- Export
- Diagnostics

---

# 215. Integrationstests

Getestet werden

- BIOS
- UEFI
- ARM64
- Device Tree
- große Speichermengen
- kleine Speichermengen

---

# 216. Regressionstests

Vor jedem Release werden geprüft

✔ Memory Discovery

✔ Normalisierung

✔ Heap

✔ Reservierungen

✔ Frame Allocator

✔ Validation

✔ Export

✔ ABI

---

# 217. Belastungstests

Es werden Tests durchgeführt für

- 16 MB RAM
- 64 MB RAM
- 256 MB RAM
- 1 GB RAM
- 4 GB RAM
- 64 GB RAM
- 256 GB RAM
- 1 TB RAM

Das Verhalten muss in allen Fällen deterministisch bleiben.

---

# 218. Emulatoren

Pflichttests

- QEMU
- Bochs
- VirtualBox
- VMware
- Hyper-V

Zusätzlich reale Hardware.

---

# 219. Qualitätsanforderungen

Die Memory Runtime MUSS

✔ deterministisch arbeiten

✔ vollständig dokumentiert sein

✔ reproduzierbare Ergebnisse liefern

✔ ABI-stabil bleiben

✔ keine Speicherüberläufe erzeugen

✔ alle Firmwarequellen vereinheitlichen

---

# 220. Performanceziele

Empfohlene Zielwerte

| Operation | Ziel |
|-----------|-----:|
| Memory Discovery | < 10 ms |
| Normalisierung | < 5 ms |
| Bitmap-Erzeugung | < 5 ms |
| Heap Initialisierung | < 2 ms |
| Export | < 2 ms |

Die Werte dienen als Referenz und können je nach Hardware variieren.

---

# 221. Dokumentationsstandard

Jede öffentliche Funktion besitzt mindestens

```c
/*
Beschreibung

Parameter

Rückgabewert

Fehler

Seiteneffekte

Komplexität

Thread Safety

Bemerkungen
*/
```

---

# 222. Anforderungen

Die öffentliche API MUSS

- vollständig dokumentiert sein
- versioniert werden
- ABI-stabil bleiben
- testbar sein
- reproduzierbar arbeiten

---

# 223. SHOULD

Die API SOLLTE

- leicht erweiterbar sein
- zukünftige Architekturen unterstützen
- klar dokumentiert bleiben
- einfache Migrationen ermöglichen

---

# 224. MAY

Die API DARF

- experimentelle Erweiterungen
- zusätzliche Diagnosefunktionen
- Debug-Hooks
- optionale Profiler

bereitstellen.

Diese Erweiterungen dürfen die stabile ABI nicht verändern.

---

# Architekturentscheidung (ADR)

Die **Boot Memory API** bildet die einzige offizielle Schnittstelle zur Speicherverwaltung des Bootloaders.

Boot-Komponenten dürfen ausschließlich diese API verwenden und niemals direkt auf interne Datenstrukturen wie Bitmap, Heap oder Reservation Table zugreifen.

Dadurch bleibt die Implementierung austauschbar, ohne dass sich die öffentliche Schnittstelle ändert.

---

# 225. Referenzimplementierung

Die Referenzimplementierung beschreibt den empfohlenen Aufbau des Nova Boot Memory Frameworks.

Alle Komponenten arbeiten ausschließlich über die öffentliche Memory API.

```text
Boot Core

↓

Memory API

↓

Memory Runtime

↓

Platform Layer
```

---

# 226. Referenzprojektstruktur

```text
boot/
└── memory/
    ├── api/
    │   ├── memory.h
    │   ├── allocator.h
    │   ├── frames.h
    │   ├── pages.h
    │   ├── reservations.h
    │   ├── validation.h
    │   └── diagnostics.h
    │
    ├── core/
    │   ├── manager.c
    │   ├── runtime.c
    │   ├── export.c
    │   └── statistics.c
    │
    ├── discovery/
    │   ├── e820.c
    │   ├── uefi.c
    │   ├── devicetree.c
    │   └── normalize.c
    │
    ├── heap/
    │   ├── heap.c
    │   ├── linear_allocator.c
    │   └── alignment.c
    │
    ├── frames/
    │   ├── bitmap.c
    │   ├── allocator.c
    │   └── statistics.c
    │
    ├── pages/
    │   ├── page_allocator.c
    │   ├── page_tables.c
    │   ├── identity_map.c
    │   └── higher_half.c
    │
    ├── reservations/
    │   ├── manager.c
    │   ├── validator.c
    │   └── conflicts.c
    │
    ├── validation/
    │   ├── validator.c
    │   ├── badmemory.c
    │   ├── alignment.c
    │   └── repair.c
    │
    ├── diagnostics/
    │   ├── logger.c
    │   ├── profiler.c
    │   ├── reports.c
    │   └── timeline.c
    │
    ├── tests/
    └── docs/
```

---

# 227. Initialisierungsreihenfolge

Die Initialisierung erfolgt immer in derselben Reihenfolge.

```text
Platform

↓

Memory Discovery

↓

Normalize

↓

Validation

↓

Reservation Manager

↓

Frame Allocator

↓

Boot Heap

↓

Page Allocator

↓

Diagnostics

↓

Export

↓

Kernel
```

Eine Änderung dieser Reihenfolge ist nicht zulässig.

---

# 228. Kernel-Handoff

Vor dem Start des Kernels muss Folgendes abgeschlossen sein:

- Memory Discovery
- Memory Validation
- Reservation Manager
- Frame Allocator
- Boot Heap
- Page Tables
- Export
- Diagnostics

Erst danach erfolgt die Übergabe an den Kernel.

---

# 229. Beziehungen zu anderen NPSPECs

Das Boot Memory Framework ist Grundlage für:

- NPSPEC-BOOTSTAGE2-0001
- NPSPEC-BOOTPLATFORM-0001
- NPSPEC-BOOTGRAPHICS-0001
- NPSPEC-KERNELLOADER-0001
- NPSPEC-BOOTHANDOFF-0001
- NPSPEC-KERNELENTRY-0001
- NPSPEC-KERNELMEMORY-0001 (geplant)
- NPSPEC-VMM-0001 (geplant)

Der Kernel übernimmt die hier definierten Datenstrukturen und erweitert sie um virtuelle Speicherverwaltung.

---

# 230. Implementierungsphasen

## Phase 1

- Memory Discovery
- E820
- UEFI Memory Map
- Normalisierung

---

## Phase 2

- Boot Heap
- Linear Allocator
- Alignment
- Reservation Manager

---

## Phase 3

- Physical Frame Allocator
- Bitmap
- Page Allocator
- Identity Mapping

---

## Phase 4

- Higher-Half-Vorbereitung
- Seitentabellen
- Kernel Mapping
- Framebuffer Mapping

---

## Phase 5

- Validation Engine
- Diagnostics
- Performance Analyzer
- Export

---

## Phase 6

- Optimierungen
- NUMA-Vorbereitung
- Huge Pages
- Erweiterte Debugfunktionen

---

# 231. Akzeptanzkriterien

Das Framework gilt als umgesetzt, wenn

1. BIOS E820 korrekt gelesen wird.
2. UEFI Memory Maps vollständig unterstützt werden.
3. Device-Tree-Speicher erkannt wird.
4. Alle Regionen korrekt normalisiert werden.
5. Die Nova Memory Map erzeugt wird.
6. Der Boot Heap funktionsfähig ist.
7. Der Physical Frame Allocator arbeitet.
8. Reservierungen konfliktfrei verwaltet werden.
9. Die Validation Engine alle Prüfungen erfolgreich durchführt.
10. Eine vollständige `nova_boot_memory_info_t` erzeugt wird.
11. Alle Daten erfolgreich über `nova_boot_info_t` an den Kernel übergeben werden.

---

# 232. Kompatibilitätsziele

Das Framework unterstützt langfristig:

| Plattform | Status |
|------------|--------|
| BIOS x86 | Pflicht |
| UEFI x64 | Pflicht |
| ARM64 UEFI | Geplant |
| RISC-V Device Tree | Geplant |
| Coreboot | Geplant |
| Emulatoren | Pflicht |

Die öffentliche API bleibt plattformunabhängig.

---

# 233. Qualitätsziele

Das Nova Boot Memory Framework verfolgt folgende Qualitätsmerkmale:

- deterministische Speicherverwaltung
- reproduzierbare Ergebnisse
- keine Fragmentierung im Bootloader
- geringe Komplexität
- hohe Performance
- vollständige Diagnosefähigkeit
- klare Trennung zwischen Bootloader und Kernel

---

# 234. Architekturprinzipien

Das Framework basiert dauerhaft auf folgenden Prinzipien:

## Platform First

Alle Firmwarequellen werden zunächst vereinheitlicht.

---

## Memory First

Jede Bootkomponente verwendet ausschließlich die zentrale Memory Runtime.

---

## Single Source of Truth

Es existiert genau eine gültige Memory Map.

Alle Komponenten verwenden dieselbe Datenbasis.

---

## Explicit Reservations

Jede Speicherbelegung wird explizit registriert.

Implizite Reservierungen sind unzulässig.

---

## Validation Before Execution

Der Kernel darf erst gestartet werden, wenn sämtliche Speicherstrukturen erfolgreich validiert wurden.

---

## Deterministic Allocation

Alle Allokationen müssen reproduzierbar sein.

Identische Hardware muss zu identischen Speicherlayouts führen.

---

## Separation of Responsibilities

Die Zuständigkeiten sind klar getrennt:

- Discovery erkennt Speicher.
- Reservation verwaltet belegte Bereiche.
- Frame Allocator verwaltet physische Frames.
- Boot Heap stellt temporären Speicher bereit.
- Validation prüft die Konsistenz.
- Export übergibt die Informationen an den Kernel.

---

# 235. Langfristige Vision

Das Nova Boot Memory Framework soll die Grundlage für die gesamte Speicherverwaltung der Nova-Plattform bilden.

Die gleiche Architektur soll sich durch alle Ebenen des Systems ziehen:

```text
Firmware

↓

Boot Memory Runtime

↓

Kernel Physical Memory Manager

↓

Virtual Memory Manager

↓

Kernel Heap

↓

User Space Allocator

↓

Application Memory
```

Dadurch entsteht eine konsistente Speicherarchitektur vom Einschalten des Rechners bis zur Ausführung von Benutzeranwendungen.

---

# 236. Zukünftige Erweiterungen

Für spätere Versionen sind unter anderem geplant:

- Buddy Allocator für den Kernel
- Slab Allocator
- NUMA-Awareness
- CXL Memory Support
- Persistent Memory (NVDIMM)
- Memory Compression
- Transparent Huge Pages
- Memory Hotplug
- Memory Encryption (z. B. AMD SME/SEV, Intel TME)
- Speicherisolation für Virtualisierung

Diese Erweiterungen bauen auf den in dieser Spezifikation definierten Grundlagen auf.

---

# 237. Zusammenfassung

Das **Nova Boot Memory Framework** stellt eine vollständige, plattformunabhängige Speicherverwaltung für den Bootloader bereit.

Es übernimmt:

- Erkennung des physischen Speichers
- Vereinheitlichung verschiedener Firmwareformate
- Verwaltung aller Reservierungen
- Boot Heap
- Physical Frame Allocator
- Page Allocator
- Validierung
- Diagnose
- Export der Speicherinformationen an den Kernel

Das Framework bildet damit die Grundlage für alle weiteren Speicherkomponenten innerhalb von Nova OS.