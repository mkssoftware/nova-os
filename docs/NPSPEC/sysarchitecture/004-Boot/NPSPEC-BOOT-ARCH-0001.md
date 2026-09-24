# NPSPEC-BOOT-ARCH-0001 – Nova Boot Architecture

## Status

Entwurf

## Kategorie

Boot / Architecture / Firmware / Kernel Handoff

## Zweck

NovaOS definiert eine einheitliche Boot-Architektur für den kontrollierten Übergang von Firmware über den Nova-Bootloader zum Kernel.

```text
Firmware
   ↓
Nova Bootloader
   ↓
Boot Environment
   ↓
Kernel Handoff
   ↓
Nova Kernel
```

BIOS und UEFI dürfen unterschiedliche Startmechanismen verwenden, müssen dem Kernel jedoch eine möglichst einheitliche Boot-Umgebung bereitstellen.

## Grundprinzip

Der Kernel soll nicht von der ursprünglichen Firmware-Bootmethode abhängig sein.

```text
BIOS ──┐
       ├→ Nova Boot Architecture → Kernel
UEFI ──┘
```

Firmware- und Plattformdetails werden vor dem Kernelstart validiert, normalisiert und über definierte Boot-Strukturen übergeben.

## Boot-Phasen

Der Bootvorgang wird in klar getrennte Phasen gegliedert:

```text
Firmware Entry
     ↓
Bootloader Initialization
     ↓
Platform Discovery
     ↓
Kernel Selection / Loading
     ↓
Boot Information Construction
     ↓
Kernel Handoff
     ↓
Kernel Initialization
```

Jede Phase besitzt einen definierten Zustand und Fehlerpfad.

## BIOS und UEFI

NovaOS unterstützt unterschiedliche Firmwarepfade:

```text
BIOS
 └── BIOS Bootloader

UEFI
 └── EFI Bootloader
```

Beide Pfade erzeugen vor dem Kernelstart dieselbe logische Nova-Bootbeschreibung.

Firmware-spezifische Unterschiede sollen nach dem Handoff möglichst nicht mehr relevant sein.

## Boot Information

Der Bootloader übergibt dem Kernel eine versionierte Boot Information Block-Struktur.

Diese kann unter anderem enthalten:

```text
Boot Protocol Version
Kernel Build ID
Memory Map
Framebuffer
Firmware Information
ACPI Information
CPU / Platform Information
Boot Volume
Command Line
Boot Mode
Reserved Memory
Boot Status
```

Der Kernel muss die Struktur vor ihrer Verwendung validieren.

## Kernel Loading

Der Bootloader ist verantwortlich für:

```text
Kernel finden
Kernel validieren
Kernel laden
Speicher vorbereiten
Bootinformationen erzeugen
Ausführungsumgebung vorbereiten
Kontrolle übertragen
```

Nach erfolgreichem Handoff übernimmt der Kernel die Verantwortung für das System.

## Kernel Handoff

Der Übergang muss eindeutig definiert sein.

```text
Bootloader
    ↓
Defined CPU State
Defined Memory State
Boot Information Pointer
Kernel Entry Point
    ↓
Kernel
```

Der Kernel darf keine nicht dokumentierten Bootloaderzustände voraussetzen.

## Boot-Modi

Die Architektur soll unterschiedliche Startmodi unterstützen können:

```text
Normal Boot
Recovery Boot
Safe Boot
Diagnostic Boot
Installer Boot
```

Der gewählte Modus wird dem Kernel explizit übergeben.

## Fehlerbehandlung

Fehler müssen möglichst in der Phase behandelt werden, in der sie entstehen.

```text
Boot Failure
    ↓
Diagnostic Information
    ↓
Recovery / Alternative Entry
```

Ein unvollständig vorbereiteter Kernel darf nicht gestartet werden.

## Normative Anforderungen

1. NovaOS MUSS eine definierte Boot-Architektur besitzen.
2. BIOS und UEFI SOLLEN auf ein gemeinsames Kernel-Handoff-Modell normalisiert werden.
3. Der Kernel DARF NICHT unnötig von der ursprünglichen Firmware-Bootmethode abhängen.
4. Bootinformationen MÜSSEN versioniert und validierbar sein.
5. Der Bootloader MUSS den Kernel vor dem Start vollständig laden und vorbereiten.
6. Der CPU- und Speicherzustand beim Kernel-Handoff MUSS definiert sein.
7. Firmwareinformationen MÜSSEN vor ihrer Übergabe soweit möglich validiert werden.
8. Boot-Modi MÜSSEN dem Kernel explizit übergeben werden können.
9. Kritische Bootfehler MÜSSEN einen kontrollierten Kernelstart verhindern.
10. Der Bootpfad SOLL Diagnose- und Recovery-Funktionen unterstützen.
11. Der Kernel DARF keine undokumentierten Bootloaderzustände voraussetzen.
12. Die Boot-Architektur MUSS zukünftige Firmware- und Plattformpfade integrieren können.

## Abhängigkeiten

- `NPSPEC-HAL-0001`
- `NPSPEC-HAL-PLATFORM-0001`
- `NPSPEC-HAL-FIRMWARE-0001`
- `ADR-BOOT-0001`
- `ADR-BOOT-0002`
- `ADR-BOOT-0003`
- `ADR-BOOT-0004`
- `ADR-BOOT-0005`
- `ADR-BOOT-0006`
- `ADR-BOOT-0007`
- `ADR-BOOT-0008`
- `ADR-BOOT-0009`
- `ADR-BOOT-0010`
- `ADR-BOOT-0011`
- `ADR-BOOT-0012`

## Ergebnis

```text
BIOS / UEFI
     ↓
Nova Bootloader
     ↓
Normalized Boot Environment
     ↓
Defined Kernel Handoff
     ↓
Nova Kernel
```

NovaOS erhält damit einen klar definierten und erweiterbaren Bootpfad, bei dem unterschiedliche Firmwaremechanismen vor dem Kernelstart auf eine gemeinsame Nova-Architektur normalisiert werden.