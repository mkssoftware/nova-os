# ADR-BOOT-0001 – Parallele BIOS- und UEFI-Bootunterstützung

## Status

Angenommen

## Kategorie

Boot / Bootloader / Firmware / Plattformkompatibilität / Recovery / Hardwareabstraktion

## Kontext

NovaOS soll sowohl auf älteren klassischen PC-Systemen als auch auf modernen Rechnern bootfähig sein.

Im PC-Umfeld existieren dafür zwei wesentliche Firmware- und Bootmodelle:

```text
Legacy BIOS
```

und:

```text
UEFI
```

Beide Modelle unterscheiden sich grundlegend.

BIOS verwendet typischerweise:

- MBR beziehungsweise klassische Bootsektoren,
- Real Mode beim initialen Start,
- BIOS Interrupt Services,
- klassische Disk Services,
- optional VESA/VBE für Grafik,
- meist keine standardisierte moderne Runtime-Schnittstelle.

UEFI verwendet dagegen:

- EFI Executables,
- UEFI Boot Manager,
- GPT-nahe moderne Plattformmodelle,
- Boot Services,
- Runtime Services,
- Graphics Output Protocol,
- Configuration Tables,
- standardisierte Firmwareprotokolle.

NovaOS soll nicht zwei voneinander unabhängige Betriebssystem-Startarchitekturen entwickeln.

Stattdessen sollen BIOS und UEFI lediglich unterschiedliche **Boot Entry Paths** darstellen, die möglichst früh in einen gemeinsamen NovaOS-Bootpfad übergehen.

---

## Entscheidung

NovaOS unterstützt BIOS und UEFI parallel.

Die beiden Firmwarepfade besitzen separate minimale Einstiegskomponenten, konvergieren jedoch vor dem Start des Kernels in ein gemeinsames Bootmodell.

Das Grundmodell lautet:

```text
                  Firmware
                     │
          ┌──────────┴──────────┐
          ▼                     ▼
       BIOS                  UEFI
          │                     │
          ▼                     ▼
   BIOS Boot Path         UEFI Boot Path
          │                     │
          └──────────┬──────────┘
                     ▼
            Nova Boot Environment
                     │
                     ▼
              Common BootInfo
                     │
                     ▼
                 Kernel
```

Damit gilt:

```text
Different firmware entry.

Common NovaOS boot semantics.
```

---

## Grundprinzip

BIOS und UEFI sind Bootmechanismen, nicht zwei unterschiedliche NovaOS-Plattformarchitekturen.

Der Kernel soll möglichst nicht wissen müssen, ob er ursprünglich über BIOS oder UEFI gestartet wurde.

Firmwareabhängige Informationen werden vor Kernelübergabe normalisiert.

---

## Boot Entry Paths

NovaOS besitzt mindestens zwei Boot Entry Paths:

```text
BIOS Boot Entry
UEFI Boot Entry
```

Diese dürfen technisch unterschiedlich implementiert sein.

Gemeinsam müssen sie jedoch dieselben grundlegenden Ziele erreichen:

- Kernel lokalisieren,
- Kernel validieren,
- benötigte Bootressourcen laden,
- Speicherinformationen erfassen,
- Grafik initialisieren, sofern verfügbar,
- Plattforminformationen erfassen,
- Common BootInfo erzeugen,
- kontrolliert an den Kernel übergeben.

---

## BIOS Boot Path

Der BIOS-Pfad startet über einen klassischen BIOS-kompatiblen Bootmechanismus.

Konzeptionell:

```text
BIOS
  ↓
Boot Sector / Stage 1
  ↓
Boot Stage 2
  ↓
Nova Boot Environment
  ↓
Kernel
```

Der initiale Stage-1-Code soll möglichst klein bleiben.

Komplexere Funktionen gehören in spätere Bootstufen.

---

## BIOS Stage 1

Stage 1 ist primär dafür verantwortlich:

- Kontrolle von der Firmware zu übernehmen,
- minimale CPU- und Speicherannahmen herzustellen,
- die nächste Bootstufe zu laden,
- bei nicht behebbaren Fehlern einen minimalen Fehlerpfad anzubieten.

Stage 1 soll keine umfangreiche Bootpolicy oder UI implementieren.

---

## BIOS Stage 2

Stage 2 kann unter anderem übernehmen:

- Speichererkennung,
- Dateisystemzugriff,
- Kernel-Loading,
- Grafikinitialisierung,
- Bootmenü,
- Recovery-Auswahl,
- Signaturprüfung,
- Plattformvorbereitung,
- Protected-/Long-Mode-Übergang.

Die genaue Aufteilung darf sich mit der Architektur weiterentwickeln.

---

## UEFI Boot Path

Unter UEFI wird NovaOS über eine EFI-Anwendung gestartet.

Konzeptionell:

```text
UEFI Firmware
      ↓
NovaOS EFI Bootloader
      ↓
UEFI Boot Services
      ↓
Nova Boot Environment
      ↓
ExitBootServices
      ↓
Kernel
```

Der UEFI Loader nutzt standardisierte UEFI-Protokolle, solange Boot Services verfügbar sind.

---

## ExitBootServices

Vor der endgültigen Kernelübergabe muss der UEFI-Pfad die Nutzung von Boot Services kontrolliert beenden.

Das typische Modell lautet:

```text
Acquire final memory map
      ↓
Prepare BootInfo
      ↓
ExitBootServices
      ↓
No further Boot Service calls
      ↓
Enter kernel
```

Nach erfolgreichem `ExitBootServices()` dürfen Boot Services nicht weiter verwendet werden.

---

## Gemeinsame Nova Boot Environment

BIOS und UEFI sollen möglichst früh eine gemeinsame logische Bootumgebung erreichen.

Diese übernimmt firmwareunabhängige Funktionen wie:

- Bootkonfiguration,
- Bootmenü,
- Kernel-Auswahl,
- Recovery-Auswahl,
- Kernelvalidierung,
- Modul-Loading,
- gemeinsame Bootressourcen,
- Bootdiagnose,
- BootInfo-Erzeugung.

Dadurch muss Bootlogik nicht doppelt implementiert werden.

---

## Gemeinsames BootInfo

Beide Bootpfade erzeugen dieselbe semantische BootInfo-Struktur.

Beispiel:

```text
BootInfo {
    version
    architecture
    platform
    firmwareType

    memoryMap
    framebuffer
    bootDevice
    kernelImage
    modules

    firmwareReferences
    commandLine

    recoveryMode
    securityState
}
```

Die exakte binäre Struktur wird in einer NPSPEC definiert.

---

## Firmware Type

Die ursprüngliche Firmware darf als Information erhalten bleiben.

Beispiel:

```text
FirmwareType = BIOS
```

oder:

```text
FirmwareType = UEFI
```

Der Kernel darf diese Information für Diagnose und Plattformprovider verwenden.

Sie darf jedoch nicht dazu führen, dass zentrale Kernelkomponenten zwei vollständig getrennte Codepfade benötigen.

---

## Firmware References

Firmware-spezifische Daten werden nur dort weitergegeben, wo sie später noch benötigt werden.

Beispiele:

```text
ACPI RSDP
SMBIOS entry
UEFI system table reference
Device Tree
```

Diese Informationen werden anschließend gemäß:

```text
ADR-HAL-0003_Firmware_über_Platform_Provider_abstrahieren
```

durch Platform Provider verarbeitet.

---

## Kernel-Format

Beide Bootpfade laden dasselbe native Kernel-Image-Format.

BIOS und UEFI dürfen nicht unterschiedliche Kernelbinaries allein aufgrund des Firmwaretyps erfordern.

Architekturspezifische Kernelvarianten bleiben davon getrennt.

Beispiel:

```text
x86_64 kernel
ARM64 kernel
```

ist legitim.

Nicht gewünscht ist:

```text
x86_64 BIOS kernel
x86_64 UEFI kernel
```

wenn beide dieselbe Architektur verwenden.

---

## ELF Kernel

Für den nativen NovaOS-Kernel wird ein strukturiertes ausführbares Kernelformat verwendet, bevorzugt ELF für die jeweils unterstützte Architektur.

Der Bootloader muss mindestens:

- Header validieren,
- Load Segments bestimmen,
- Speicherbereiche reservieren,
- Segmente korrekt laden,
- Entry Point bestimmen

können.

---

## Gemeinsame Kernel Entry Convention

BIOS und UEFI müssen den Kernel über dieselbe definierte NovaOS Kernel Entry Convention starten.

Konzeptionell:

```text
KernelEntry(BootInfo*)
```

Die tatsächliche ABI hängt von der Zielarchitektur ab.

Firmwarebedingte Unterschiede müssen vor diesem Punkt normalisiert sein.

---

## CPU Mode

Der Bootloader ist dafür verantwortlich, den für den jeweiligen Kernel erforderlichen CPU-Modus herzustellen.

Für einen 64-Bit-x86-Kernel bedeutet dies beispielsweise:

```text
Real Mode
   ↓
Protected Mode
   ↓
Long Mode
   ↓
Kernel
```

Der UEFI-Pfad kann abhängig von der Plattform bereits in einem geeigneten Modus laufen.

Trotzdem muss die gemeinsame Kernel Entry Convention dieselben Voraussetzungen garantieren.

---

## Memory Map

Beide Bootpfade müssen eine normalisierte Speicherkarte erzeugen.

Mögliche Quellen:

```text
BIOS E820
UEFI Memory Map
```

werden in ein gemeinsames NovaOS-Modell übersetzt.

Beispiel:

```text
BootMemoryRegion {
    base
    length
    type
    attributes
}
```

---

## Speichertypen

Die normalisierte Boot-Speicherkarte muss mindestens unterscheiden können:

```text
Usable
Reserved
Firmware
Bootloader
Kernel
Framebuffer
MMIO
ACPI
Persistent
Unknown
```

Firmware-spezifische Details dürfen als zusätzliche Attribute erhalten bleiben.

---

## Sicherheitsregel für Speicher

Unbekannte oder widersprüchliche Speicherbereiche dürfen nicht als frei verfügbar angenommen werden.

Es gilt:

```text
Unknown memory
    ↓
Reserved by default
```

bis eine vertrauenswürdige spätere Analyse anderes bestätigt.

---

## Grafik

Beide Bootpfade sollen einen linearen Framebuffer bereitstellen können, sofern die Plattform dies unterstützt.

Mögliche Mechanismen:

```text
BIOS:
    VBE

UEFI:
    GOP
```

Beide werden auf ein gemeinsames Framebuffer-Modell abgebildet.

---

## Gemeinsames Framebuffer-Modell

Beispiel:

```text
FramebufferInfo {
    physicalAddress
    width
    height
    stride
    pixelFormat
    bitsPerPixel
}
```

Boot-UI und Kernel müssen dadurch nicht separat gegen VBE oder GOP entwickelt werden.

---

## Kein verpflichtender Grafikmodus

NovaOS muss auch dann bootfähig bleiben, wenn kein geeigneter grafischer Modus verfügbar ist.

Mögliche Fallbacks:

```text
Text Mode
Serial Console
Minimal Error Output
Headless Boot
```

Grafik ist ein Bootfeature, aber keine Voraussetzung für Kernelkorrektheit.

---

## Boot Device

Der Bootpfad muss das verwendete Bootmedium soweit möglich identifizieren.

Beispiele:

```text
NVMe
SATA
USB
Virtual Disk
Network Boot
```

Firmwareabhängige Device Handles werden auf eine normalisierte Boot Device Description abgebildet.

---

## Dateisystemzugriff

BIOS und UEFI dürfen unterschiedliche Low-Level-Methoden für den Zugriff auf Bootdateien verwenden.

Die höhere Bootlogik soll jedoch über einen gemeinsamen Boot Storage Contract arbeiten.

Beispiel:

```text
BootFileSystem
    ↓
Open
Read
Enumerate
```

---

## Boot Partition

NovaOS darf ein gemeinsames Bootlayout definieren.

Auf modernen Systemen kann beispielsweise eine EFI System Partition für UEFI verwendet werden.

BIOS darf dieselbe physische Installation über einen separaten BIOS Boot Entry erreichen, sofern das Partitions- und Bootlayout dies unterstützt.

---

## GPT

NovaOS soll GPT als modernes Partitionsmodell unterstützen.

BIOS-Unterstützung darf nicht bedeuten, dass das gesamte System auf MBR beschränkt wird.

BIOS-Boot von GPT-basierten Datenträgern kann über geeignete Bootstrukturen unterstützt werden.

---

## Secure Boot

UEFI Secure Boot kann unterstützt werden.

NovaOS darf sich jedoch nicht ausschließlich auf Secure Boot als Vertrauensmodell verlassen.

Nova.Trust und die eigene Boot-Vertrauenskette bleiben übergeordnet relevant.

---

## BIOS und Vertrauensprüfung

Auch BIOS-Systeme müssen Kernel- und Bootartefakte kryptografisch validieren können.

Damit gilt:

```text
Firmware Secure Boot available
```

ist eine zusätzliche Schutzschicht.

Nicht:

```text
No UEFI Secure Boot
=
No verified boot
```

---

## Gemeinsame Verified-Boot-Logik

Signatur- und Integritätsprüfung soll möglichst im gemeinsamen Bootcode erfolgen.

Beispiel:

```text
Load Artifact
      ↓
Verify Identity
      ↓
Verify Signature
      ↓
Verify Integrity
      ↓
Check Policy
      ↓
Use Artifact
```

Dies reduziert Unterschiede zwischen BIOS und UEFI.

---

## Boot Chain

Die Bootkette muss nachvollziehbar sein.

Beispiel:

```text
Firmware
    ↓
Nova Boot Entry
    ↓
Nova Boot Environment
    ↓
Kernel
    ↓
System
```

Für jeden relevanten Schritt sollen Identität, Version und Integritätsstatus nachvollziehbar sein.

---

## Recovery

BIOS und UEFI müssen dieselbe logische Recovery-Umgebung erreichen können.

Dazu gehört insbesondere NovaDOS beziehungsweise die definierte Offline-Recovery-Umgebung.

Beispiel:

```text
BIOS ─┐
      ├── Nova Boot Manager
UEFI ─┘
          │
          ├── NovaOS
          ├── Recovery
          └── NovaDOS
```

---

## Bootmenü

Das Bootmenü gehört zur gemeinsamen Nova Boot Environment und soll nicht zweimal vollständig implementiert werden.

Firmwareabhängig ist nur die technische Initialisierung, nicht die höhere Navigation.

---

## Bootoptionen

Mögliche Bootoptionen sind beispielsweise:

```text
Normal Boot
Recovery
NovaDOS
Safe Mode
Diagnostic Mode
Memory Test
Previous Known-Good State
```

Die genaue Liste wird separat spezifiziert.

---

## Boot Configuration

Bootkonfiguration muss firmwareunabhängig modelliert werden.

Beispiel:

```text
BootEntry {
    id
    kernel
    modules
    commandLine
    mode
    trustPolicy
}
```

BIOS und UEFI interpretieren dieselbe NovaOS-Konfiguration.

---

## Known-Good Boot

NovaOS kann mehrere bootfähige Systemgenerationen oder Known-Good-Stände verwalten.

Beide Firmwarepfade müssen diese auf dieselbe Weise auswählen können.

---

## Boot Failure

Bootfehler werden typisiert behandelt.

Beispiele:

```text
KernelNotFound
KernelInvalid
SignatureInvalid
MemoryMapInvalid
UnsupportedArchitecture
FramebufferUnavailable
BootDeviceUnavailable
FirmwareError
ConfigurationInvalid
```

Firmwaretypische Fehler dürfen als Detail erhalten bleiben.

---

## Fallback

Wo möglich, soll der Bootloader sichere Fallbacks verwenden.

Beispiele:

```text
Preferred Graphics Mode
    ↓ fails
Fallback Graphics Mode
    ↓ fails
Text / Serial Output
```

oder:

```text
Current System Generation
    ↓ invalid
Known-Good Generation
```

Sicherheitsanforderungen dürfen durch Fallback nicht abgeschwächt werden.

---

## Fail-Safe Defaults

Bei unsicheren Bootzuständen gilt:

```text
Do not boot unverified code
```

und:

```text
Do not treat unknown memory as usable
```

und:

```text
Do not silently downgrade security policy
```

---

## BIOS- und UEFI-Parität

BIOS und UEFI müssen nicht exakt dieselben Firmwarefeatures besitzen.

Erforderlich ist funktionale Parität für die grundlegenden NovaOS-Bootfunktionen.

Beispiel:

```text
Kernel Loading
Verified Boot
Recovery
Boot Menu
BootInfo
Framebuffer where possible
```

Ein Feature, das ausschließlich durch eine bestimmte Firmware verfügbar ist, darf optional bleiben.

---

## Keine künstliche Firmwareparität

NovaOS soll UEFI-Funktionen nicht künstlich auf BIOS nachbilden, wenn dies keinen architektonischen Mehrwert besitzt.

Beispiel:

UEFI Runtime Services müssen nicht emuliert werden, nur damit BIOS und UEFI formal identisch erscheinen.

---

## UEFI bevorzugen

Wenn ein System sowohl UEFI als auch Legacy-BIOS-Kompatibilität anbietet, soll NovaOS standardmäßig den nativen UEFI-Pfad bevorzugen.

Gründe:

- moderneres Plattformmodell,
- bessere Firmwareintegration,
- moderner Grafikzugriff,
- bessere Partitionierungsintegration,
- weniger Legacy-Abhängigkeiten.

BIOS bleibt echter unterstützter Fallback und Kompatibilitätspfad.

---

## Kein CSM als Architekturvoraussetzung

NovaOS darf nicht voraussetzen, dass moderne UEFI-Systeme ein Compatibility Support Module besitzen.

Der UEFI-Pfad muss vollständig nativ funktionieren.

---

## BIOS bleibt eigenständig

Der BIOS-Pfad darf nicht lediglich auf CSM-Verfügbarkeit moderner UEFI-Firmware angewiesen sein.

Er soll auf tatsächlichen Legacy-BIOS-Systemen funktionieren können.

---

## Plattformprovider

Nach Kernelstart werden Firmwareunterschiede durch Platform Provider übernommen.

Beispiel:

```text
BootInfo
    ↓
PC Platform Provider
    ├── ACPI
    ├── UEFI information
    └── Legacy BIOS-derived information
```

Der Kernelkern bleibt davon weitgehend unabhängig.

---

## Boot und HAL

Der Bootloader initialisiert nur die Hardware, die für einen zuverlässigen Kernelstart erforderlich ist.

Die vollständige Hardwareinitialisierung gehört anschließend in:

```text
HAL
Platform Provider
Driver Infrastructure
```

Der Bootloader soll kein zweites vollständiges Treibersystem entwickeln.

---

## Minimal Boot Hardware

Typischerweise erforderlich sind nur:

- CPU-Basiskonfiguration,
- Speicherzugriff,
- Bootmedium,
- Kernel-Loading,
- minimale Ausgabe,
- Firmware-/Plattforminformationen.

Andere Hardware wird später vom Kernel übernommen.

---

## Übergabe von Hardwarezuständen

Vom Bootloader aktivierte Hardwarezustände müssen entweder:

- dokumentiert an den Kernel übergeben,
- kontrolliert deaktiviert,
- oder durch definierte Kernelinitialisierung übernommen

werden.

Unsichtbare Bootloaderzustände sollen vermieden werden.

---

## Boot Resource Ownership

Alle vom Bootloader belegten Speicherressourcen müssen in BootInfo markiert sein.

Der Kernel darf sie erst wiederverwenden, nachdem die jeweilige Bootresource freigegeben wurde.

---

## Boot Services als temporäre Ressourcen

UEFI Boot Services sind temporäre Firmware-Ressourcen.

Nach `ExitBootServices()` gehen entsprechende Zuständigkeiten an NovaOS über.

Das Ownership-Modell muss diesen Übergang explizit behandeln.

---

## Bootloader Lifecycle

Der Bootloader selbst besitzt einen klaren Lebenszyklus:

```text
Firmware Entry
      ↓
Initialization
      ↓
Load / Verify
      ↓
Prepare Handoff
      ↓
Transfer Ownership
      ↓
Kernel Entry
      ↓
Bootloader Retired
```

Nach erfolgreicher Kernelübergabe darf keine verdeckte aktive Bootloaderlogik zurückbleiben.

---

## Deterministischer Kernelübergang

Kernel Entry muss einen wohldefinierten Zustand garantieren.

Dazu gehören je Architektur unter anderem:

- CPU Mode,
- Interrupt State,
- Stack,
- Page Tables,
- Registerkonvention,
- BootInfo Pointer,
- Speicherreservierungen.

Der Kernel darf keine undocumented firmware-spezifischen Voraussetzungen benötigen.

---

## Architekturportabilität

Diese ADR betrifft primär PC-BIOS und UEFI.

Das gemeinsame Bootmodell soll jedoch so gestaltet sein, dass weitere Bootprotokolle später ebenfalls in dieselbe Kernel Entry Convention münden können.

Beispiel:

```text
Future Firmware
      ↓
Future Boot Entry
      ↓
Common Nova Boot Environment
      ↓
Common Kernel Entry
```

Damit bleibt die Bootarchitektur mit `ADR-HAL-0002` vereinbar.

---

## 32-Bit und 64-Bit

Firmwarebitness und Kernelbitness müssen explizit behandelt werden.

NovaOS darf nicht implizit voraussetzen, dass:

```text
Firmware architecture
==
Kernel architecture
```

ist.

Unterstützte Kombinationen werden separat spezifiziert.

---

## Boot-Architekturidentität

Der Bootloader muss sicherstellen, dass ein Kernel nur auf einer kompatiblen Zielarchitektur gestartet wird.

Ein inkompatibles Image wird abgelehnt.

---

## Boot Introspection

Bootpfad und relevante Bootentscheidungen sollen später introspektierbar sein.

Beispiel:

```text
Boot Firmware:
    UEFI

Boot Loader:
    Nova Boot 1.4

Kernel:
    NovaOS Kernel 0.8

Boot Mode:
    Normal

Verified:
    Yes
```

---

## Boot Decision Trace

Relevante Bootentscheidungen sollen nachvollziehbar sein.

Beispiel:

```text
Requested:
    Current Generation

Rejected:
    signature invalid

Selected:
    Known-Good Generation
```

Dies unterstützt Diagnose und Self-Healing.

---

## Boot Logging

Der Bootloader soll einen kompakten strukturierten Bootlog führen können.

Dieser kann an den Kernel übergeben werden.

Beispiele:

```text
Firmware detected
Memory map acquired
Graphics initialized
Kernel verified
Kernel loaded
Boot services exited
Kernel entered
```

---

## Recovery bei Bootloaderfehler

Wenn der primäre Start nicht fortgesetzt werden kann, soll der Bootloader – sofern noch möglich – eine Recoveryoption anbieten.

Fehler dürfen jedoch keine automatische unsichere Ausführung auslösen.

---

## Transaktionale Updates

Bootloader- und Kernelupdates sollen so gestaltet werden, dass ein Stromausfall oder abgebrochenes Update nicht automatisch das einzige bootfähige System zerstört.

Geeignete Mechanismen können sein:

```text
A/B generations
staging
atomic activation
known-good fallback
```

Die konkrete Updatearchitektur wird separat spezifiziert.

---

## Bootloader-Komponenten

Die Firmware Entry Layer sollen klein gehalten werden.

Konzeptionell:

```text
BIOS Entry
UEFI Entry
     │
     ▼
Shared Boot Core
     │
     ├── Storage
     ├── Filesystem
     ├── Verification
     ├── Boot Configuration
     ├── UI
     ├── Recovery
     └── Handoff
```

Dadurch liegt der Großteil der Funktionalität im gemeinsamen Code.

---

## Code Sharing

Soweit technisch sinnvoll, sollen BIOS und UEFI gemeinsame Implementierungen verwenden für:

- ELF Parsing,
- Signaturprüfung,
- Bootkonfiguration,
- Bootmenülogik,
- Boot Resource Manager,
- Boot UI,
- Kernel Handoff Preparation.

Firmwarezugriff selbst bleibt backendabhängig.

---

## Boot Provider

Firmwareabhängige Bootmechanismen können intern als Provider modelliert werden.

Beispiel:

```text
BootFirmwareProvider
├── BiosBootProvider
└── UefiBootProvider
```

Sie liefern gemeinsame Boot Services an den Shared Boot Core.

---

## Beispiel eines Boot Provider Contracts

Konzeptionell:

```text
BootFirmwareProvider {
    GetMemoryMap()
    GetFramebuffer()
    ReadBootDevice()
    GetFirmwareTables()
    PrepareKernelHandoff()
    ExitFirmwareBootPhase()
}
```

Die konkrete API wird in NPSPECs festgelegt.

---

## Keine Firmwarelecks

Der Shared Boot Core soll nicht überall Abfragen enthalten wie:

```text
if BIOS ...
if UEFI ...
```

Firmwareunterschiede sollen möglichst hinter Boot Provider Contracts gekapselt werden.

---

## Security und Trust

Alle Bootpfade müssen dieselben übergeordneten Trust-Anforderungen erfüllen.

Die Vertrauenskette darf nicht davon abhängen, ob BIOS oder UEFI verwendet wird.

Konzeptionell:

```text
Boot Artifact
    ↓
Identity
    ↓
Integrity
    ↓
Signature
    ↓
Trust Policy
    ↓
Execution
```

---

## TPM

Ein vorhandenes TPM darf unabhängig vom BIOS-/UEFI-Pfad in die Boot-Vertrauenskette integriert werden.

Measured Boot kann damit separat von UEFI Secure Boot unterstützt werden.

---

## Measured Boot

Bootkomponenten dürfen relevante Artefakte messen.

Beispiele:

```text
Bootloader
Configuration
Kernel
Modules
Recovery Image
```

Die konkrete TPM-/Attestation-Architektur wird separat definiert.

---

## NovaDOS

NovaDOS soll aus beiden Bootpfaden erreichbar sein.

Das bedeutet:

```text
BIOS
  └── Nova Boot Environment
          └── NovaDOS

UEFI
  └── Nova Boot Environment
          └── NovaDOS
```

NovaDOS darf nicht nur auf einem der beiden Firmwaremodelle verfügbar sein.

---

## Deterministic Mode

Der Bootloader kann deterministische Bootparameter für den Kernel vorbereiten.

Der Firmwarepfad darf diese Semantik nicht verändern.

Identische Bootkonfiguration und identisches Kernelimage sollen nach Möglichkeit denselben semantischen Kernelstart erzeugen.

---

## KI-Unabhängigkeit

Der vollständige Bootprozess muss ohne KI funktionieren.

KI darf nicht benötigt werden für:

- Firmwareerkennung,
- Kernelwahl im Normalfall,
- Kernelvalidierung,
- Speichererkennung,
- Grafikinitialisierung,
- Recovery-Auswahl,
- Kernelübergabe.

Adaptive oder KI-gestützte Diagnose darf optional hinzukommen.

---

## Normative Anforderungen

1. NovaOS MUSS parallele BIOS- und UEFI-Bootunterstützung besitzen.
2. BIOS und UEFI MÜSSEN als getrennte Firmware Entry Paths implementierbar sein.
3. Beide Bootpfade MÜSSEN in einen gemeinsamen NovaOS-Bootpfad konvergieren.
4. Der Kernel SOLL keine getrennte BIOS- und UEFI-Kernarchitektur benötigen.
5. Beide Bootpfade MÜSSEN dieselbe semantische BootInfo erzeugen.
6. Der Firmwaretyp MUSS in BootInfo erkennbar bleiben.
7. Firmware-spezifische Informationen MÜSSEN hinter definierten Strukturen gekapselt werden.
8. Beide Bootpfade MÜSSEN dasselbe Kernel-Image-Format laden können.
9. Firmwaretyp DARF NICHT allein unterschiedliche Kernelimages erzwingen.
10. Der Kernel Entry Contract MUSS firmwareunabhängig definiert sein.
11. Der BIOS-Pfad MUSS einen klassischen Legacy-BIOS-Start unterstützen können.
12. Der UEFI-Pfad MUSS als native EFI-Anwendung funktionieren.
13. Der UEFI-Pfad DARF NICHT von CSM abhängig sein.
14. Der BIOS-Pfad DARF NICHT ausschließlich auf CSM angewiesen sein.
15. Auf Systemen mit nativer UEFI-Unterstützung SOLL UEFI bevorzugt werden.
16. BIOS MUSS als eigenständiger unterstützter Bootpfad erhalten bleiben.
17. UEFI Boot Services MÜSSEN vor Kernelübergabe kontrolliert beendet werden.
18. Nach `ExitBootServices()` DÜRFEN UEFI Boot Services NICHT weiter verwendet werden.
19. Beide Bootpfade MÜSSEN eine normalisierte Speicherkarte bereitstellen.
20. Unbekannte Speicherbereiche DÜRFEN NICHT als frei nutzbar markiert werden.
21. BIOS E820 SOLL als Speicherquelle unterstützt werden.
22. UEFI Memory Map MUSS als Speicherquelle im UEFI-Pfad verwendet werden können.
23. Beide Bootpfade SOLLEN einen gemeinsamen Framebuffer Contract erzeugen.
24. BIOS SOLL VBE verwenden können, wenn verfügbar.
25. UEFI SOLL GOP verwenden können.
26. Grafik DARF NICHT Voraussetzung für grundlegende Bootfähigkeit sein.
27. Headless- oder Text-Fallback MUSS grundsätzlich möglich sein.
28. Die höhere Bootlogik SOLL firmwareunabhängig implementiert werden.
29. Bootmenülogik SOLL zwischen BIOS und UEFI geteilt werden.
30. Recovery-Logik SOLL zwischen BIOS und UEFI geteilt werden.
31. NovaDOS MUSS über beide Bootpfade erreichbar sein.
32. Verified Boot MUSS unabhängig vom Firmwaretyp möglich sein.
33. Fehlendes UEFI Secure Boot DARF NICHT bedeuten, dass Kernelvalidierung deaktiviert wird.
34. UEFI Secure Boot DARF als zusätzliche Vertrauensschicht integriert werden.
35. BIOS-Boot MUSS eigene kryptografische Artefaktvalidierung unterstützen können.
36. Bootartefakte MÜSSEN vor Verwendung validierbar sein.
37. Sicherheits-Fallbacks DÜRFEN Signatur- oder Trust-Anforderungen NICHT stillschweigend abschwächen.
38. GPT SOLL auch mit BIOS-Boot unterstützt werden können.
39. BIOS-Unterstützung DARF das NovaOS-Disklayout NICHT auf MBR beschränken.
40. Boot Resource Ownership MUSS explizit an den Kernel übergeben werden.
41. Der Kernel DARF Bootloader-Speicher erst nach kontrollierter Freigabe wiederverwenden.
42. Der Bootloader MUSS einen wohldefinierten CPU-Zustand beim Kernel Entry herstellen.
43. Der Bootloader MUSS einen gültigen Kernel Stack beziehungsweise Entry-Kontext bereitstellen.
44. Firmwarebedingte Unterschiede MÜSSEN vor dem gemeinsamen Kernel Entry soweit möglich normalisiert werden.
45. Der Bootloader SOLL nur die für den Start erforderliche Hardware initialisieren.
46. Vollständige Hardwareinitialisierung MUSS anschließend durch HAL und Treiber erfolgen.
47. Vom Bootloader hinterlassene Hardwarezustände MÜSSEN dokumentiert oder kontrolliert übernommen werden.
48. Bootfehler MÜSSEN typisiert behandelbar sein.
49. Sichere Bootfallbacks SOLLEN möglich sein.
50. Known-Good-Bootstände SOLLEN über beide Firmwarepfade auswählbar sein.
51. Bootkonfiguration SOLL firmwareunabhängig sein.
52. Firmwareabhängige Bootmechanismen SOLLEN über Boot Provider abstrahiert werden.
53. Der Shared Boot Core SOLL möglichst wenige direkte BIOS-/UEFI-Sonderfälle enthalten.
54. BIOS- und UEFI-spezifischer Code SOLL auf notwendige Entry- und Firmwareintegration begrenzt werden.
55. Der Bootpfad MUSS später introspektierbar sein.
56. Relevante Bootentscheidungen SOLLEN protokollierbar sein.
57. Ein strukturierter Bootlog SOLL an den Kernel übergeben werden können.
58. Bootupdates SOLLEN Known-Good- und Recovery-Mechanismen berücksichtigen.
59. TPM und Measured Boot SOLLEN firmwareunabhängig integrierbar sein.
60. Die native Bootarchitektur MUSS für zukünftige Firmwaremodelle erweiterbar bleiben.
61. BIOS und UEFI MÜSSEN dieselbe übergeordnete NovaOS-Trust-Policy verwenden können.
62. Die Bootarchitektur MUSS ohne KI vollständig funktionieren.
63. KI DARF Bootdiagnose unterstützen, aber keine fundamentale Bootkorrektheit ersetzen.
64. NovaOS MUSS BIOS und UEFI als unterschiedliche Einstiegspfade in dieselbe Systemarchitektur behandeln.

---

## Konsequenzen

### Positive Konsequenzen

- Unterstützung älterer und moderner PC-Hardware.
- kein Zwang zu zwei unterschiedlichen Kernelvarianten.
- gemeinsame Bootmenü-, Recovery- und Verified-Boot-Logik.
- UEFI kann modern genutzt werden, ohne Legacy-Systeme auszuschließen.
- BIOS-Unterstützung zwingt NovaOS nicht auf MBR.
- Bootcode wird langfristig besser wartbar.
- spätere Firmwaremodelle können in dasselbe Handoff-Modell integriert werden.
- NovaDOS und Recovery bleiben unabhängig vom Firmwaretyp verfügbar.

### Negative Konsequenzen

- zwei Low-Level-Firmwarepfade müssen gepflegt und getestet werden.
- BIOS benötigt zusätzliche Legacy-Initialisierung.
- unterschiedliche Firmwarequellen müssen normalisiert werden.
- Bootloaderarchitektur wird komplexer als ein reiner UEFI-Loader.
- BIOS-Hardware besitzt teilweise schlechtere oder uneinheitliche Firmwareinformationen.

---

## Verworfene Alternativen

### Nur BIOS unterstützen

Verworfen.

BIOS ist für moderne Systeme keine ausreichende langfristige Grundlage.

### Nur UEFI unterstützen

Verworfen.

Dies würde ältere oder spezielle BIOS-Systeme unnötig ausschließen.

### Zwei vollständig unabhängige Bootloaderarchitekturen

Verworfen.

Dies würde Bootmenü, Recovery, Validierung und Kernel-Handoff doppelt implementieren.

### BIOS-Unterstützung ausschließlich über UEFI-CSM

Verworfen.

Nicht jedes UEFI-System besitzt CSM, und echte Legacy-BIOS-Systeme sollen unterstützt bleiben.

### Separater Kernel für BIOS und UEFI

Verworfen.

Firmware ist kein ausreichender Grund für unterschiedliche Kernelarchitekturen.

### MBR als einheitliches Partitionsmodell

Verworfen.

NovaOS soll GPT und moderne Speicherlayouts vollständig unterstützen.

### UEFI Secure Boot als einzige Kernelvalidierung

Verworfen.

NovaOS benötigt ein eigenes firmwareunabhängiges Trust- und Verified-Boot-Modell.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-HAL-0001_Einheitliche_Hardware_Abstraction_Layer`
- `ADR-HAL-0002_Plattformabstraktion_für_x86_ARM_und_zukünftige_Architekturen`
- `ADR-HAL-0003_Firmware_über_Platform_Provider_abstrahieren`
- `ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starken_Isolationsgrenzen`
- `ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage`
- `ADR-ARCH-0004_Transaktionale_Systemoperationen`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts_zwischen_Systemkomponenten`
- `ADR-ARCH-0015_Graceful_Degradation_als_Systemprinzip`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvierbare_Systemarchitektur`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`
- `ADR-TRUST-0001_Nova_Trust`

---

## Zugehörige NPSPECs

- `NPSPEC-BOOT-ARCH-0001`
- `NPSPEC-BOOT-BIOS-0001`
- `NPSPEC-BOOT-BIOS-STAGE1-0001`
- `NPSPEC-BOOT-BIOS-STAGE2-0001`
- `NPSPEC-BOOT-UEFI-0001`
- `NPSPEC-BOOT-UEFI-ENTRY-0001`
- `NPSPEC-BOOT-UEFI-EXITBOOTSERVICES-0001`
- `NPSPEC-BOOT-PROVIDER-0001`
- `NPSPEC-BOOT-SHAREDCORE-0001`
- `NPSPEC-BOOT-BOOTINFO-0001`
- `NPSPEC-BOOT-KERNEL-HANDOFF-0001`
- `NPSPEC-BOOT-KERNEL-ELF-0001`
- `NPSPEC-BOOT-MEMORYMAP-0001`
- `NPSPEC-BOOT-FRAMEBUFFER-0001`
- `NPSPEC-BOOT-STORAGE-0001`
- `NPSPEC-BOOT-FILESYSTEM-0001`
- `NPSPEC-BOOT-CONFIG-0001`
- `NPSPEC-BOOT-MENU-0001`
- `NPSPEC-BOOT-RECOVERY-0001`
- `NPSPEC-BOOT-VERIFICATION-0001`
- `NPSPEC-BOOT-KNOWNGOOD-0001`
- `NPSPEC-BOOT-LOG-0001`
- `NPSPEC-BOOT-TPM-0001`
- `NPSPEC-BOOT-MEASURED-0001`

---

## Verifikation

Die Umsetzung muss mindestens folgende Szenarien prüfen:

- Boot auf echtem Legacy-BIOS-System,
- BIOS-Boot unter QEMU,
- nativer UEFI-Boot,
- UEFI-Boot unter QEMU/OVMF,
- UEFI-System ohne CSM,
- BIOS-Boot von GPT-Datenträger,
- Laden desselben Kernelimages über BIOS und UEFI,
- identische Common-BootInfo-Semantik über beide Pfade,
- BIOS-E820-Speicherkarte,
- UEFI-Memory-Map,
- VBE-Framebuffer,
- GOP-Framebuffer,
- Boot ohne grafischen Modus,
- Verified Boot über BIOS,
- Verified Boot über UEFI,
- UEFI Secure Boot als zusätzliche Schutzschicht,
- ungültiges Kernelimage,
- fehlgeschlagene Signaturprüfung,
- Known-Good-Fallback,
- Recovery-Start,
- NovaDOS-Start,
- kontrollierter `ExitBootServices()`-Pfad,
- Bootlogübergabe an den Kernel,
- korrekte Freigabe von Bootloader-Ressourcen.

---

## Ergebnis

NovaOS besitzt zwei Firmware Entry Paths, aber nur eine gemeinsame höhere Bootarchitektur.

Das Modell lautet:

```text
             ┌── BIOS ──┐
Firmware ────┤          ├── Nova Boot Core
             └── UEFI ──┘         │
                                  ▼
                              Common BootInfo
                                  │
                                  ▼
                                Kernel
```

Damit gilt:

```text
BIOS and UEFI are entry mechanisms.

They are not different operating systems.
```

Die zentrale Architekturregel lautet:

```text
Support both firmware worlds.

Normalize early.

Share the boot core.

Enter one NovaOS kernel architecture.
```