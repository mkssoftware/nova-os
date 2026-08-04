# NPSPEC-BOOT-0001
# Nova Boot Architecture

| Feld | Wert |
|---|---|
| Dokument | NPSPEC-BOOT-0001 |
| Titel | Nova Boot Architecture |
| Version | 1.1.0 |
| Status | Angenommen |
| Kategorie | System Specification |
| Bereich | Nova Boot Platform |
| Übergeordnet | Nova System Architecture |
| Zielarchitekturen | x86, x86-64, später ARM64 und RISC-V |
| Letzte Änderung | 2026-07-14 |

---

# 1. Zweck

Diese Spezifikation definiert die vollständige Boot-Architektur von Nova OS.

Sie beschreibt den Systemstart vom Übergang der Firmware an den ersten Nova-Code bis zur kontrollierten Übergabe an den Kernel.

Die Boot-Architektur umfasst:

- Legacy-BIOS-Boot
- UEFI-Boot
- Stage-1- und Stage-2-Bootloader
- Hardware- und Plattforminitialisierung
- Speichererkennung
- Grafikinitialisierung
- Eingabegeräte
- Datenträger- und Dateisystemzugriff
- Kernel- und Modulladen
- Sicherheitsprüfung
- Recovery
- Selbstheilung
- Bootprotokollierung
- Kernel-Handoff

Diese Spezifikation definiert die übergeordneten Anforderungen. Detailanforderungen werden in nachgeordneten NPSPECs beschrieben.

---

# 2. Ziele

Die Nova Boot Architecture soll:

- BIOS und UEFI über eine gemeinsame Abstraktion unterstützen
- einen stabilen, versionierten Boot-to-Kernel-Vertrag bereitstellen
- 32-Bit- und 64-Bit-Kernel laden können
- langfristig mehrere CPU-Architekturen unterstützen
- grafische und textbasierte Bootoberflächen bereitstellen
- Recovery- und Selbstheilungsfunktionen integrieren
- Kernel, Module und Konfigurationen verifizieren
- deterministisch und reproduzierbar arbeiten
- sichere Fallback-Pfade besitzen
- Debugging und Protokollierung bereits vor dem Kernel ermöglichen
- modular erweiterbar bleiben

---

# 3. Nichtziele

Diese Spezifikation definiert nicht:

- die vollständige Kernelarchitektur
- Gerätetreiber des laufenden Kernels
- Benutzeranmeldung
- Desktop- oder Shell-Initialisierung
- vollständige Dateisystemtreiber für den Kernel
- vollständige Netzwerkunterstützung im Bootloader
- allgemeine Anwendungsruntimes

Der Bootloader darf nur die Funktionen bereitstellen, die für Start, Diagnose, Recovery und Handoff erforderlich sind.

---

# 4. Leitprinzipien

## 4.1 Kleine vertrauenswürdige Basis

Der frühe Bootpfad muss möglichst klein, überprüfbar und deterministisch bleiben.

## 4.2 Trennung von Plattform und Logik

BIOS- und UEFI-spezifischer Code wird hinter einer gemeinsamen Plattformabstraktion gekapselt.

## 4.3 Versionierte Übergabe

Alle Daten, die an den Kernel übergeben werden, müssen über das versionierte, TLV-basierte Nova Boot Handoff Protocol mit Boot Information Block (NBHP/BIB) definiert sein.

## 4.4 Sichere Fallbacks

Jede kritische Bootphase muss einen definierten Fehler- und Wiederherstellungspfad besitzen.

## 4.5 Keine unnötigen Abhängigkeiten

Der Bootloader darf keine vollständige Betriebssystemumgebung voraussetzen.

## 4.6 Recovery als Systemfunktion

Recovery ist kein nachträgliches Zusatzprogramm, sondern ein fester Bestandteil der Boot-Architektur.

## 4.7 Benutzerkontrolle

Kritische Aktionen wie Löschen, Entschlüsseln, Wiederherstellen oder Überschreiben benötigen klare Benutzerfreigaben.

---

# 5. Gesamtarchitektur

```text
Firmware
├── Legacy BIOS
└── UEFI
      │
      ▼
Nova Platform Entry
├── BIOS Stage 1
├── BIOS Stage 2
└── UEFI Application
      │
      ▼
Boot Platform Abstraction
├── Firmware Services
├── Memory Discovery
├── Storage Access
├── Filesystem Access
├── Graphics
├── Input
├── Time
└── Logging
      │
      ▼
Nova Boot Core
├── Configuration
├── Boot Menu
├── Security
├── Recovery
├── Diagnostics
├── Kernel Loader
├── Module Loader
└── Handoff Builder
      │
      ▼
Kernel Entry
```

---

# 6. Bootphasen

```c
typedef enum nova_boot_phase
{
    NOVA_BOOT_PHASE_FIRMWARE = 0,
    NOVA_BOOT_PHASE_ENTRY,
    NOVA_BOOT_PHASE_PLATFORM,
    NOVA_BOOT_PHASE_MEMORY,
    NOVA_BOOT_PHASE_STORAGE,
    NOVA_BOOT_PHASE_FILESYSTEM,
    NOVA_BOOT_PHASE_GRAPHICS,
    NOVA_BOOT_PHASE_INPUT,
    NOVA_BOOT_PHASE_CONFIGURATION,
    NOVA_BOOT_PHASE_SECURITY,
    NOVA_BOOT_PHASE_RECOVERY_CHECK,
    NOVA_BOOT_PHASE_KERNEL_LOAD,
    NOVA_BOOT_PHASE_MODULE_LOAD,
    NOVA_BOOT_PHASE_KERNEL_VERIFY,
    NOVA_BOOT_PHASE_HANDOFF_PREPARE,
    NOVA_BOOT_PHASE_FIRMWARE_EXIT,
    NOVA_BOOT_PHASE_KERNEL_ENTRY,
    NOVA_BOOT_PHASE_COMPLETE,
    NOVA_BOOT_PHASE_FAILED
} nova_boot_phase_t;
```

Jede Phase muss:

- einen eindeutigen Status besitzen
- protokollierbar sein
- Fehlercodes liefern
- Fortschrittsinformationen bereitstellen
- sichere Abbruch- oder Recovery-Pfade definieren

---

# 7. Bootpfade

## 7.1 Legacy-BIOS-Pfad

```text
BIOS
↓
MBR / Boot Sector
↓
Stage 1
↓
Stage 2
↓
Protected Mode / Long Mode Vorbereitung
↓
Nova Boot Core
↓
Kernel Handoff
```

Der BIOS-Pfad muss mindestens unterstützen:

- Bootsektor mit Signatur `0xAA55`
- CHS-Fallback
- INT-13h-LBA-Erweiterungen
- E820-Speicherkarte
- VBE oder Textmodus
- A20-Aktivierung
- GDT
- Protected Mode
- optional Long Mode

## 7.2 UEFI-Pfad

```text
UEFI Firmware
↓
EFI System Partition
↓
Nova EFI Application
↓
UEFI Protocol Discovery
↓
Nova Boot Core
↓
ExitBootServices()
↓
Kernel Handoff
```

Der UEFI-Pfad muss mindestens unterstützen:

- PE/COFF EFI Application
- Simple File System Protocol
- Block I/O Protocol
- Graphics Output Protocol
- UEFI Memory Map
- Loaded Image Protocol
- Secure Boot Status
- `ExitBootServices()`

---

# 8. Boot Platform Abstraction

```c
typedef enum nova_boot_platform_type
{
    NOVA_BOOT_PLATFORM_UNKNOWN = 0,
    NOVA_BOOT_PLATFORM_BIOS,
    NOVA_BOOT_PLATFORM_UEFI,
    NOVA_BOOT_PLATFORM_COREBOOT,
    NOVA_BOOT_PLATFORM_FIRMWARE_GENERIC
} nova_boot_platform_type_t;
```

```c
typedef struct nova_boot_platform
{
    nova_boot_platform_type_t type;

    nova_result_t (*initialize)(void);
    nova_result_t (*get_memory_map)(nova_boot_memory_map_t* map);
    nova_result_t (*read_blocks)(
        uint64_t device,
        uint64_t lba,
        uint64_t count,
        void* buffer);

    nova_result_t (*load_file)(
        const char* path,
        void** data,
        uint64_t* size);

    nova_result_t (*set_video_mode)(
        const nova_video_request_t* request,
        nova_framebuffer_info_t* framebuffer);

    nova_result_t (*poll_input)(
        nova_boot_input_event_t* event);

    uint64_t (*get_monotonic_time_ns)(void);

    nova_result_t (*exit_firmware_services)(void);
} nova_boot_platform_t;
```

Höhere Bootkomponenten dürfen nicht direkt von BIOS-Interrupts oder UEFI-Protokollen abhängen.

---

# 9. Stage-Modell

## 9.1 Stage 1

Aufgaben:

- Prozessorzustand stabilisieren
- Stack initialisieren
- Bootlaufwerk sichern
- Stage 2 lokalisieren
- Stage 2 laden
- Ladefehler behandeln
- Kontrolle übergeben

Stage 1 soll keine komplexe Benutzeroberfläche, Dateisystemlogik oder Sicherheitsprüfung enthalten.

## 9.2 Stage 2

Aufgaben:

- A20 aktivieren
- Speicherkarte lesen
- Grafik initialisieren
- Eingabe initialisieren
- Dateisystem laden
- Konfiguration lesen
- Bootmenü anzeigen
- Kernel und Module laden
- Sicherheitsprüfungen ausführen
- Handoff-Struktur aufbauen
- CPU-Modus wechseln
- Kernel starten

## 9.3 UEFI Application

Im UEFI-Pfad ersetzt die EFI-Anwendung Stage 1 und Stage 2 als Firmware-Entry, muss intern jedoch dieselben logischen Bootphasen durchlaufen.

---

# 10. Bootkonfiguration

Die Bootkonfiguration muss mindestens enthalten:

- Booteinträge
- Standard-Bootziel
- Timeout
- Kernelpfad
- Initramfs-Pfad
- Modulpfade
- Kernelargumente
- Grafikmodus
- Sprache
- Theme
- Sicherheitsrichtlinie
- Recovery-Einstellungen
- Diagnoseoptionen

Beispiel:

```ini
[boot]
default=nova
timeout=2
resolution=auto
language=de-DE
theme=nova-dark

[entry.nova]
kernel=/nova/kernel.nki
initramfs=/nova/initramfs.img
arguments=quiet graphical

[entry.recovery]
kernel=/nova/kernel.nki
initramfs=/nova/recovery.img
arguments=recovery safe
```

NKI ist das bevorzugte Produktionsformat. Bootkonfigurationen dürfen alternativ direkt auf gültige ELF32- oder ELF64-Kernel verweisen.

Ungültige Konfigurationen dürfen den Start nicht unkontrolliert abbrechen. Es muss ein eingebautes Standardprofil existieren.

---

# 11. Speicherarchitektur

Der Bootloader muss folgende Speicherklassen unterscheiden:

- Firmware-reserviert
- Bootloader-Code
- Bootloader-Daten
- Bootloader-Heap
- Bootloader-Stack
- Kernel-Code
- Kernel-Daten
- Kernel-Stack
- Initramfs
- Module
- Framebuffer
- ACPI
- SMBIOS
- MMIO
- verfügbarer physischer Speicher

```c
typedef enum nova_boot_memory_type
{
    NOVA_BOOT_MEMORY_UNKNOWN = 0,
    NOVA_BOOT_MEMORY_AVAILABLE,
    NOVA_BOOT_MEMORY_RESERVED,
    NOVA_BOOT_MEMORY_ACPI_RECLAIMABLE,
    NOVA_BOOT_MEMORY_ACPI_NVS,
    NOVA_BOOT_MEMORY_MMIO,
    NOVA_BOOT_MEMORY_FIRMWARE,
    NOVA_BOOT_MEMORY_BOOTLOADER,
    NOVA_BOOT_MEMORY_KERNEL,
    NOVA_BOOT_MEMORY_MODULE,
    NOVA_BOOT_MEMORY_FRAMEBUFFER,
    NOVA_BOOT_MEMORY_BAD
} nova_boot_memory_type_t;
```

Der Bootloader muss Speicherüberschneidungen erkennen und verhindern.

---

# 12. Kernel Image Loader

Der Kernel Loader muss mindestens unterstützen:

- NKI als bevorzugtes Produktionsformat
- ELF32
- ELF64
- direkte Formaterkennung über Magic-Werte
- Segmentprüfung
- Entry-Point-Prüfung
- Ladeadressprüfung
- Alignment
- BSS-Initialisierung
- Prüfsummen
- digitale Signaturen
- optionale Kompression
- Kernel-Fallback

Optional können später unterstützt werden:

- PE/COFF
- Nova Kernel Image Format
- relocierbare Kernel
- Higher-Half-Kernel
- KASLR

---

# 13. Bootmodule

Bootmodule umfassen:

- Initramfs
- Treibermodule
- Microcode
- Fonts
- Themes
- Recovery-Komponenten
- Konfiguration
- Sicherheitsdaten
- Diagnosemodule

```c
typedef struct nova_boot_module
{
    uint64_t physical_address;
    uint64_t size;

    nova_boot_module_type_t type;
    uint32_t flags;

    char name[64];
    char path[256];

    uint8_t hash[32];
} nova_boot_module_t;
```

Jedes Modul muss hinsichtlich Größe, Adresse und Integrität validiert werden.

---

# 14. Boot-to-Kernel Handoff

```c
#define NOVA_BIB_MAGIC 0x004249425048424EULL /* "NBHPBIB\0" */

typedef struct nova_bib_header
{
    uint64_t magic;
    uint16_t version_major;
    uint16_t version_minor;
    uint16_t version_patch;
    uint16_t header_size;
    uint32_t total_size;
    uint32_t checksum;
    uint32_t architecture;
    uint32_t flags;
} nova_bib_header_t;

typedef struct nova_bib_tlv_header
{
    uint16_t type;
    uint16_t flags;
    uint32_t length;
} nova_bib_tlv_header_t;
```

Direkt auf den festen BIB-Header folgt eine Folge ausgerichteter TLV-Einträge. Version 1 definiert mindestens die Typen Firmware, Memory, Graphics, Kernel, Security und Boot Options.

Der Kernel erhält genau einen Zeiger auf den BIB und muss Magic, Protokollversion, Headergröße, Gesamtgröße, Prüfsumme, TLV-Längen und Alignment prüfen, bevor er Nutzdaten verwendet. Unbekannte optionale TLV-Einträge werden anhand ihrer Länge übersprungen. Unbekannte erforderliche Einträge führen zu einem kontrollierten Bootabbruch.

---

# 15. Kernel Entry Contract

Beim Kernel-Eintritt müssen folgende Bedingungen definiert sein:

- CPU-Modus eindeutig festgelegt
- Interrupts deaktiviert
- gültiger Stack vorhanden
- Direction Flag gelöscht
- Segmentzustand dokumentiert
- Paging-Zustand dokumentiert
- Handoff-Zeiger in definiertem Register oder Parameter
- Firmwaredienste beendet oder ausdrücklich verfügbar
- Framebuffer nicht mehr durch Firmware verändert
- Bootloader-Speicher als reserviert markiert

Für x86-32:

```text
EAX = Boot Magic
EBX = Zeiger auf den NBHP/BIB
ESP = Kernel-Stack
```

Für x86-64:

```text
RDI = Zeiger auf den NBHP/BIB
RSP = 16-Byte-ausgerichteter Kernel-Stack
```

---

# 16. Grafik

Die Bootarchitektur muss unterstützen:

- BIOS VBE
- UEFI GOP
- Textmodus-Fallback
- automatische Auflösungswahl
- sichere Pixelformat-Erkennung
- 24- und 32-Bit-Framebuffer
- Pitch-Unterstützung
- Double Buffering optional
- HiDPI-Skalierung
- Software-Rendering
- Übergabe des Framebuffers an den Kernel

Die grafische Oberfläche darf den Kernelstart nicht verhindern. Bei Fehlern muss Textmodus oder Minimalgrafik verwendet werden.

---

# 17. Eingabe

Unterstützt werden:

- BIOS-Tastatur
- PS/2-Tastatur
- PS/2-Maus
- UEFI Simple Text Input
- UEFI Simple Pointer
- UEFI Absolute Pointer
- später USB-HID über eigene Treiber

```c
typedef struct nova_boot_input_event
{
    nova_boot_input_type_t type;
    uint64_t timestamp_ns;

    union
    {
        nova_boot_key_event_t key;
        nova_boot_pointer_event_t pointer;
        nova_boot_touch_event_t touch;
    };
} nova_boot_input_event_t;
```

---

# 18. Boot User Interface

Die Bootoberfläche unterstützt:

- Startmenü
- Countdown
- Standard-Boot
- Recovery-Menü
- Diagnose
- Speicherprüfung
- Backup
- Wiederherstellung
- sicheres Löschen
- Verschlüsselungsfunktionen
- Tastatur- und Mausbedienung
- Barrierefreiheitsoptionen
- Safe-Mode-Darstellung

Animationen und Glass-Effekte sind optional und müssen bei schwacher Hardware deaktiviert werden können.

---

# 19. Sicherheit

```text
Firmware Trust
↓
Bootloader Verification
↓
Configuration Verification
↓
Kernel Verification
↓
Module Verification
↓
Handoff Security State
```

Unterstützte Sicherheitsfunktionen:

- Hashprüfung
- digitale Signaturen
- Secure Boot
- eigener Nova Trust Store
- Schlüsselrotation
- Sperrlisten
- Measured Boot
- TPM
- verschlüsselte Systempartitionen
- Boot-Authentifizierung
- Entwickler- und Wartungsmodus
- Auditprotokoll

Unsichere oder nicht verifizierte Starts müssen im Handoff markiert werden.

---

# 20. Recovery

Recovery-Funktionen:

- Dateisicherung
- Dateiwiederherstellung
- Partitionsprüfung
- Dateisystemprüfung
- Bootloader-Reparatur
- Kernel-Fallback
- Snapshot-Rollback
- Konfigurationsrücksetzung
- Speicherprüfung
- Laufwerksdiagnose
- sicheres Löschen
- Logexport

Recovery darf unabhängig vom Hauptsystem startbar sein.

---

# 21. Selbstheilung

Prüfbare Komponenten:

- Stage 2
- UEFI-Anwendung
- Bootkonfiguration
- Kernel
- Initramfs
- kritische Module
- Recovery-Image
- Boot-Assets

Mögliche Reaktionen:

- erneute Prüfung
- Fallback-Kopie laden
- Recovery starten
- Snapshot wiederherstellen
- Benutzer informieren
- Reparatur protokollieren

Automatische Reparaturen dürfen keine Benutzerdaten überschreiben, sofern keine explizite Freigabe oder Policy vorliegt.

---

# 22. Protokollierung

Unterstützte Ziele:

- Ringbuffer im RAM
- Framebuffer-Konsole
- Textmodus
- serieller Port
- persistente Logdatei
- Übergabe an Kernel
- Recovery-Export

```c
typedef struct nova_boot_log_entry
{
    uint64_t timestamp_ns;
    nova_log_level_t level;
    nova_boot_phase_t phase;
    uint32_t component_id;
    uint32_t error_code;
    char message[192];
} nova_boot_log_entry_t;
```

---

# 23. Fehlerbehandlung

```c
typedef enum nova_boot_error_severity
{
    NOVA_BOOT_ERROR_INFO = 0,
    NOVA_BOOT_ERROR_WARNING,
    NOVA_BOOT_ERROR_RECOVERABLE,
    NOVA_BOOT_ERROR_CRITICAL,
    NOVA_BOOT_ERROR_FATAL
} nova_boot_error_severity_t;
```

Mögliche Aktionen:

- fortsetzen
- erneut versuchen
- Fallback laden
- Safe Mode
- Recovery starten
- Diagnose ausführen
- kontrolliert neu starten
- anhalten

Ein schwarzer Bildschirm ohne Fehlerausgabe ist nicht zulässig, sofern Grafik- oder Textausgabe technisch möglich ist.

---

# 24. Bootfortschritt

```c
typedef struct nova_boot_progress
{
    nova_boot_phase_t phase;
    uint32_t phase_progress;
    uint32_t total_progress;

    char status_text[128];
} nova_boot_progress_t;
```

Eine reine Zeitanimation darf nicht als tatsächlicher Systemfortschritt ausgegeben werden.

---

# 25. Hardwareerkennung

Der Bootloader sammelt mindestens:

- CPU-Hersteller und Features
- Speichermenge
- ACPI-Tabellen
- SMBIOS
- Bootgerät
- Grafikmodus
- Firmwaretyp
- Secure-Boot-Status
- TPM-Status
- grundlegende PCI-Informationen optional

Die Hardwareerkennung darf nicht den vollständigen Kernel-Gerätetreiber ersetzen.

---

# 26. Portabilität

## 26.1 x86

- BIOS
- UEFI
- 16-Bit-Real-Mode
- 32-Bit-Protected-Mode
- 64-Bit-Long-Mode

## 26.2 ARM64

- UEFI
- Device Tree
- Exception Levels
- MMU
- PSCI
- Framebuffer

## 26.3 RISC-V

- OpenSBI
- SBI
- Device Tree
- Supervisor Mode
- Paging

---

# 27. Build- und Artefaktmodell

## BIOS

```text
boot1.bin
boot2.bin
kernel.nki
initramfs.img
disk.img
```

## UEFI

```text
BOOTX64.EFI
kernel.nki
initramfs.img
EFI System Partition
disk.img
```

Jedes Artefakt muss:

- reproduzierbar erzeugt werden können
- eine Version besitzen
- eine Prüfsumme besitzen
- optional signiert werden können
- im Buildmanifest aufgeführt sein

---

# 28. Bootmanifest

```c
typedef struct nova_boot_manifest
{
    uint32_t version;
    uint32_t entry_count;

    nova_boot_manifest_entry_t* entries;

    uint8_t manifest_hash[32];
    nova_signature_t signature;
} nova_boot_manifest_t;
```

Das Manifest beschreibt:

- Bootloader-Komponenten
- Kernel
- Module
- Assets
- Versionen
- Größen
- Hashes
- Signaturen
- Abhängigkeiten

---

# 29. Kompatibilität

Die Bootarchitektur soll langfristig kompatibel sein zu:

- BIOS
- UEFI
- GPT
- MBR
- FAT32
- ISO 9660
- El Torito
- ELF32
- ELF64
- ACPI
- SMBIOS
- TPM 2.0

---

# 30. Testanforderungen

Pflichttests:

- BIOS-Boot
- UEFI-Boot
- Stage-1-Ladevorgang
- Stage-2-Ladevorgang
- beschädigter Bootloader
- beschädigter Kernel
- fehlende Konfiguration
- ungültige Signatur
- Speicherüberschneidung
- ungültige Memory Map
- Grafik-Fallback
- Tastaturbetrieb ohne Maus
- Recovery-Boot
- Kernel-Handoff
- 32-Bit-Kernel
- 64-Bit-Kernel
- QEMU
- reale Hardware
- Secure-Boot-Modus
- Debug- und Release-Build

---

# 31. Referenz-Testmatrix

| Plattform | Firmware | Datenträger | Grafik | Ziel |
|---|---|---|---|---|
| QEMU x86 | BIOS | IDE Image | VBE | Pflicht |
| QEMU x86-64 | UEFI | GPT/ESP | GOP | Pflicht |
| VirtualBox | BIOS | SATA | VBE | Pflicht |
| VirtualBox | UEFI | GPT/ESP | GOP | empfohlen |
| VMware | UEFI | NVMe virtuell | GOP | empfohlen |
| reale ältere Hardware | BIOS | SATA/USB | VBE | empfohlen |
| reale moderne Hardware | UEFI | NVMe/USB | GOP | Pflicht |

---

# 32. Öffentliche Boot-API

```c
nova_result_t nova_boot_initialize(void);

nova_result_t nova_boot_platform_detect(
    nova_boot_platform_t* platform);

nova_result_t nova_boot_load_configuration(
    nova_boot_configuration_t* configuration);

nova_result_t nova_boot_select_entry(
    nova_boot_configuration_t* configuration,
    nova_boot_entry_t* entry);

nova_result_t nova_boot_load_kernel(
    const nova_boot_entry_t* entry,
    nova_loaded_kernel_t* kernel);

nova_result_t nova_boot_build_handoff(
    const nova_loaded_kernel_t* kernel,
    nova_bib_builder_t* bib_builder);

NOVA_NORETURN
void nova_boot_enter_kernel(
    const nova_loaded_kernel_t* kernel,
    const nova_bib_header_t* bib);
```

---

# 33. Nachgeordnete Spezifikationen

- NPSPEC-BOOTLOADER-0001
- NPSPEC-BOOTSTAGE1-0001
- NPSPEC-BOOTSTAGE2-0001
- NPSPEC-BIOSBOOT-0001
- NPSPEC-UEFIBOOT-0001
- NPSPEC-BOOTPLATFORM-0001
- NPSPEC-BOOTMEMORY-0001
- NPSPEC-KERNELLOADER-0001
- NPSPEC-BOOTHANDOFF-0001
- NPSPEC-KERNELENTRY-0001
- NPSPEC-BOOTGRAPHICS-0001
- NPSPEC-BOOTUI-0001
- NPSPEC-BOOTINPUT-0001
- NPSPEC-BOOTSTORAGE-0001
- NPSPEC-BOOTFS-0001
- NPSPEC-BOOTCONFIG-0001
- NPSPEC-BOOTSECURITY-0001
- NPSPEC-BOOTRECOVERY-0001
- NPSPEC-BOOTSELFHEAL-0001
- NPSPEC-BOOTLOG-0001
- NPSPEC-BOOTERROR-0001

---

# 34. Implementierungsstufen

## Phase 0 – Minimaler BIOS-Boot

- Stage 1
- Stage 2
- Kernel laden
- VBE
- einfacher Handoff
- Fehlerausgabe

## Phase 1 – Stabiles Bootprotokoll

- versionierte Boot-Info
- vollständige Speicherkarte
- Module
- ACPI
- SMBIOS
- Logging

## Phase 2 – UEFI

- EFI-Anwendung
- GOP
- UEFI-Dateisystem
- ExitBootServices
- gemeinsamer Boot Core

## Phase 3 – Grafisches Bootsystem

- Bootmenü
- Maus
- Fonts
- Assets
- Animationen
- Recovery UI

## Phase 4 – Sicherheit

- Signaturen
- Secure Boot
- Measured Boot
- TPM
- Verschlüsselung

## Phase 5 – Recovery und Selbstheilung

- Diagnose
- Snapshot-Rollback
- Bootreparatur
- Backup
- Wiederherstellung
- sicheres Löschen

## Phase 6 – Weitere Architekturen

- ARM64
- RISC-V
- Device Tree
- plattformübergreifender Boot Core

---

# 35. Akzeptanzkriterien

NPSPEC-BOOT-0001 gilt als implementiert, wenn:

1. Nova unter BIOS und UEFI starten kann.
2. Beide Pfade denselben Boot Core verwenden.
3. Kernel und Module aus einem Dateisystem geladen werden.
4. Der Kernel genau einen Zeiger auf einen gültigen, versionierten NBHP/BIB erhält.
5. Memory Map, Framebuffer, Bootgerät und Module korrekt übergeben werden.
6. Fehler sichtbar protokolliert werden.
7. Ein Recovery-Pfad verfügbar ist.
8. beschädigte Kernel erkannt werden.
9. ein definierter Fallback ausgeführt wird.
10. der Bootvorgang in QEMU und auf mindestens einer realen Plattform erfolgreich validiert wurde.

---

# 36. Zusammenfassung

Die **Nova Boot Architecture** definiert den vollständigen Startpfad von Nova OS.

Sie vereinheitlicht BIOS und UEFI, trennt plattformspezifische Firmwarefunktionen von der eigentlichen Bootlogik und definiert einen stabilen Vertrag zwischen Bootloader und Kernel.

Durch die Integration von Grafik, Eingabe, Sicherheit, Recovery, Selbstheilung, Logging und dem versionierten TLV-basierten NBHP/BIB entsteht eine langfristig wartbare Bootplattform, die sowohl ältere Rechner als auch moderne Systeme unterstützt.

---

# Anhang A – Bezug zum aktuellen Nova-Prototyp

Der bestehende Nova-Prototyp bildet bereits Teile der frühen Implementierungsstufe ab:

```text
boot1.asm
├── BIOS Entry
├── Stackinitialisierung
├── Bootlaufwerk
├── Sektorlesen
└── Sprung zu Stage 2

boot2.asm
├── VBE
├── Kernel laden
├── GDT
├── Protected Mode
└── Kernel Entry

kernel_entry.asm
├── Stack
├── Bootparameter
└── kernel_main()
```

Eine eigenständige Framebuffer-Übergabe über eine feste Speicheradresse ist nicht Teil des öffentlichen Handoffs. Framebufferdaten werden als Graphics-TLV im NBHP/BIB übergeben.

---

# Anhang B – Langfristige Vision

Die Nova Boot Platform soll langfristig mehr sein als ein klassischer Bootloader.

Sie soll:

- ältere und moderne Hardware vereinheitlichen
- Diagnose und Recovery ohne externes Betriebssystem ermöglichen
- beschädigte Systemkomponenten erkennen und reparieren
- sichere Bootketten bereitstellen
- grafisch modern, aber technisch minimal bleiben
- dieselbe Architektur auf PC, Embedded-Systemen und Robotern verwenden
- einen stabilen Bootvertrag über viele Kernelgenerationen hinweg gewährleisten

Damit bildet NPSPEC-BOOT-0001 das Fundament für den gesamten Nova-Systemstart und für alle nachfolgenden Boot-, Kernel- und Recovery-Spezifikationen.
