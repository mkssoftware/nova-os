# NPSPEC-BOOT-BIOS-0001 – Nova BIOS Boot

## Status

Entwurf

## Kategorie

Boot / BIOS / Legacy Firmware / Bootloader

## Zweck

NovaOS definiert einen BIOS-Bootpfad für Systeme ohne UEFI und normalisiert diesen auf dieselbe Boot-Architektur wie andere Firmwarepfade.

```text
BIOS
  ↓
Boot Sector
  ↓
Nova BIOS Bootloader
  ↓
Normalized Boot Environment
  ↓
Kernel Handoff
```

## Grundprinzip

BIOS-spezifische Mechanismen dürfen nur innerhalb des BIOS-Bootpfads verwendet werden.

```text
BIOS Mechanisms
      ↓
Nova BIOS Loader
      ↓
Nova Boot Model
      ↓
Kernel
```

Der Kernel soll nach dem Handoff nicht mehr von BIOS-Funktionen abhängig sein.

## Boot-Stufen

Der BIOS-Bootpfad darf aus mehreren Stufen bestehen:

```text
BIOS
 ↓
Stage 1
 ↓
Stage 2
 ↓
Kernel Loader
 ↓
Kernel
```

Frühe Stufen sollen nur die Funktionen enthalten, die zum Laden der nächsten Stufe erforderlich sind.

## Stage 1

Stage 1 wird durch das BIOS geladen und gestartet.

Aufgaben sind mindestens:

```text
Initial CPU State übernehmen
Bootmedium identifizieren
Stage 2 finden
Stage 2 laden
Kontrolle übertragen
```

Stage 1 soll möglichst klein und einfach bleiben.

## Stage 2

Stage 2 übernimmt die erweiterte Bootvorbereitung.

Dazu können gehören:

```text
Memory Discovery
Video / Framebuffer Setup
Boot Volume Access
Kernel Loading
Platform Discovery
Protected / Long Mode Preparation
Boot Information Construction
```

## BIOS-Dienste

BIOS-Interrupts dürfen während der frühen Bootphase verwendet werden.

Beispiele:

```text
Disk I/O
Memory Information
Video Services
Keyboard Input
```

Nach dem Wechsel in die endgültige Kernel-Ausführungsumgebung darf der Kernel nicht von BIOS-Interrupts abhängig sein.

## Speicher

Der BIOS-Bootloader muss eine gültige Speicherbeschreibung ermitteln und reservierte Bereiche berücksichtigen.

```text
BIOS Memory Map
      ↓
Validation
      ↓
Nova Memory Map
      ↓
Boot Information
```

Bootloader, Kernel, Bootinformationen und temporäre Daten dürfen sich nicht gegenseitig überschreiben.

## CPU-Modus

Der Bootloader stellt den für den Nova-Kernel erforderlichen CPU-Zustand her.

Der Übergang kann beispielsweise umfassen:

```text
Real Mode
   ↓
Protected Mode
   ↓
Kernel Entry
```

Der konkrete Zielmodus wird durch die Kernelarchitektur definiert.

## Datenträgerzugriff

Der BIOS-Bootloader muss den Kernel und erforderliche Bootdaten vom Bootmedium laden können.

BIOS-spezifischer Datenträgerzugriff bleibt auf die Bootphase beschränkt.

Nach Kernelstart übernimmt das reguläre NovaOS-Treiber- und Storage-System.

## Kernel Handoff

Vor dem Handoff muss der BIOS-Pfad dieselbe logische Bootbeschreibung erzeugen wie andere Bootpfade.

```text
BIOS-specific State
        ↓
Normalization
        ↓
Nova Boot Information
        ↓
Kernel Entry
```

Damit muss der Kernel keine separate BIOS-Kernelarchitektur besitzen.

## Fehlerbehandlung

Fehler während des BIOS-Boots müssen kontrolliert behandelt werden.

```text
Load Failure
Invalid Memory Map
Invalid Kernel
Unsupported CPU
Invalid Boot Information
        ↓
Diagnostic / Recovery
```

Ein Kernel darf nicht mit unvollständiger oder ungültiger Bootumgebung gestartet werden.

## Normative Anforderungen

1. NovaOS MUSS einen definierten BIOS-Bootpfad unterstützen können.
2. BIOS-spezifische Mechanismen MÜSSEN auf die Bootschicht begrenzt bleiben.
3. Stage 1 SOLL nur minimal notwendige Funktionen enthalten.
4. Spätere Bootstufen MÜSSEN die vollständige Kernelvorbereitung übernehmen können.
5. Die BIOS-Speicherkarte MUSS vor Verwendung validiert werden.
6. Bootloader- und Kernel-Speicherbereiche DÜRFEN sich nicht unkontrolliert überschneiden.
7. Der erforderliche CPU-Ausführungsmodus MUSS vor dem Kernel-Handoff hergestellt werden.
8. Der Kernel DARF nach dem Handoff nicht von BIOS-Interrupts abhängig sein.
9. Der BIOS-Pfad MUSS eine normalisierte Nova-Bootbeschreibung erzeugen.
10. Kritische Lade- oder Validierungsfehler MÜSSEN den Kernelstart verhindern.
11. BIOS-Boot MUSS mit den allgemeinen Nova-Recovery- und Diagnosemechanismen integrierbar sein.
12. BIOS und UEFI SOLLEN denselben Kernel ohne firmwareabhängige Kernelvarianten starten können.

## Abhängigkeiten

- `NPSPEC-BOOT-ARCH-0001`
- `NPSPEC-HAL-PLATFORM-0001`
- `NPSPEC-HAL-FIRMWARE-0001`
- `ADR-BOOT-0001`
- `ADR-BOOT-0002`
- `ADR-BOOT-0003`

## Ergebnis

```text
Legacy BIOS
     ↓
Nova BIOS Bootloader
     ↓
Firmware Normalization
     ↓
Nova Boot Information
     ↓
Common Nova Kernel
```

NovaOS kann damit auch auf klassischen BIOS-Systemen starten, ohne BIOS-spezifische Abhängigkeiten in die allgemeine Kernelarchitektur zu übernehmen.