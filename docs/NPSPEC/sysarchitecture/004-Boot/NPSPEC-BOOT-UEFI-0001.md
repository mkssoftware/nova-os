# NPSPEC-BOOT-UEFI-0001 – Nova UEFI Boot

## Status

Entwurf

## Kategorie

Boot / UEFI / Firmware / Bootloader

## Zweck

NovaOS definiert einen UEFI-Bootpfad, der UEFI-Dienste zur Vorbereitung des Systems verwendet und anschließend in die gemeinsame Nova-Bootarchitektur übergeht.

```text
UEFI Firmware
     ↓
Nova EFI Application
     ↓
UEFI Boot Services
     ↓
Normalized Boot Environment
     ↓
Kernel Handoff
```

## Grundprinzip

UEFI dient ausschließlich als Boot- und Firmwareumgebung.

```text
UEFI
  ↓
Nova UEFI Loader
  ↓
Nova Boot Model
  ↓
Kernel
```

Der Kernel soll nach dem Handoff nicht von UEFI Boot Services abhängig sein.

## EFI-Anwendung

Der Nova UEFI Bootloader wird als EFI-Anwendung gestartet.

Er übernimmt mindestens:

```text
Boot Volume Access
Kernel Selection
Kernel Loading
Memory Map
Framebuffer Setup
Firmware Discovery
Boot Information Construction
ExitBootServices()
Kernel Handoff
```

## Boot Services

Vor `ExitBootServices()` dürfen UEFI Boot Services verwendet werden für:

```text
Memory Allocation
File Access
Protocol Discovery
Graphics
Device Information
Memory Map
```

Nach erfolgreichem `ExitBootServices()` dürfen diese Dienste nicht mehr verwendet werden.

## Speicher

Der UEFI-Bootloader übernimmt die UEFI Memory Map und normalisiert sie für NovaOS.

```text
UEFI Memory Map
      ↓
Validation
      ↓
Nova Memory Map
      ↓
Boot Information
```

Kernel, Bootinformationen und weitere benötigte Bereiche müssen vor `ExitBootServices()` korrekt reserviert sein.

## ExitBootServices

Vor dem Aufruf muss die aktuelle Memory Map einschließlich gültigem Map Key vorliegen.

```text
GetMemoryMap()
      ↓
Finalize Boot State
      ↓
ExitBootServices()
      ↓
Kernel Handoff
```

Schlägt `ExitBootServices()` aufgrund einer veränderten Memory Map fehl, muss die Memory Map erneut abgefragt und der Vorgang kontrolliert wiederholt werden.

## Graphics

Der Bootloader kann das UEFI Graphics Output Protocol verwenden, um einen Framebuffer für die frühe Anzeige bereitzustellen.

An den Kernel werden mindestens relevante Informationen übergeben:

```text
Framebuffer Address
Resolution
Stride
Pixel Format
```

Der spätere Grafikstack übernimmt die Kontrolle unabhängig von GOP.

## Firmwareinformationen

Der Bootloader ermittelt relevante Firmwareinformationen und übergibt Referenzen über die Nova-Bootstruktur.

Dazu können gehören:

```text
ACPI RSDP
UEFI System Information
Runtime Services
Configuration Tables
```

Diese Informationen müssen vor ihrer Verwendung validiert werden.

## Runtime Services

UEFI Runtime Services dürfen nach dem Boot nur über kontrollierte Firmware-Schnittstellen verwendet werden.

```text
Kernel
  ↓
Nova Firmware Abstraction
  ↓
UEFI Runtime Services
```

Direkte Abhängigkeiten allgemeiner Kernel-Komponenten von UEFI sind zu vermeiden.

## Secure Boot

Der UEFI-Pfad soll mit Secure Boot integrierbar sein.

```text
Firmware Trust
      ↓
Bootloader Verification
      ↓
Kernel Verification
      ↓
Boot
```

Secure Boot ersetzt jedoch nicht die eigenen NovaOS-Integritäts- und Trust-Prüfungen.

## Kernel Handoff

Nach `ExitBootServices()` erzeugt der UEFI-Pfad denselben logischen Kernel-Handoff wie andere Bootpfade.

```text
UEFI-specific State
        ↓
Normalization
        ↓
Nova Boot Information
        ↓
Kernel Entry
```

Der Kernel soll keinen separaten UEFI-spezifischen Startpfad benötigen.

## Fehlerbehandlung

Kritische Fehler müssen den Kernelstart verhindern.

Beispiele:

```text
Kernel Not Found
Kernel Invalid
Memory Map Invalid
Unsupported Platform
ExitBootServices Failed
Boot Information Invalid
```

Fehler sollen Diagnose- oder Recovery-Pfade ermöglichen.

## Normative Anforderungen

1. NovaOS MUSS einen definierten UEFI-Bootpfad unterstützen.
2. Der Nova UEFI Bootloader MUSS als EFI-Anwendung ausführbar sein.
3. UEFI Boot Services DÜRFEN nur vor `ExitBootServices()` verwendet werden.
4. Die finale UEFI Memory Map MUSS vor dem Kernel-Handoff erfasst werden.
5. Änderungen der Memory Map vor `ExitBootServices()` MÜSSEN korrekt behandelt werden.
6. Kernel- und Bootdaten MÜSSEN vor `ExitBootServices()` reserviert sein.
7. GOP SOLL für einen frühen Framebuffer verwendet werden können.
8. Firmwareinformationen MÜSSEN in die gemeinsame Nova-Bootstruktur normalisiert werden.
9. UEFI Runtime Services MÜSSEN über die Firmware-Abstraktion kontrolliert werden.
10. Der UEFI-Pfad SOLL Secure Boot unterstützen können.
11. Kritische Bootfehler MÜSSEN einen unkontrollierten Kernelstart verhindern.
12. BIOS und UEFI SOLLEN denselben Nova-Kernel über dasselbe logische Handoff-Modell starten können.

## Abhängigkeiten

- `NPSPEC-BOOT-ARCH-0001`
- `NPSPEC-HAL-PLATFORM-0001`
- `NPSPEC-HAL-FIRMWARE-0001`
- `ADR-BOOT-0001`
- `ADR-BOOT-0002`
- `ADR-BOOT-0004`

## Ergebnis

```text
UEFI Firmware
     ↓
Nova EFI Bootloader
     ↓
ExitBootServices()
     ↓
Normalized Nova Boot Information
     ↓
Common Nova Kernel
```

NovaOS erhält damit einen modernen UEFI-Bootpfad, ohne die allgemeine Kernelarchitektur dauerhaft an UEFI zu binden.