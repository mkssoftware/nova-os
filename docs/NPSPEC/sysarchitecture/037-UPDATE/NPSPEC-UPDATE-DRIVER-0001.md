# NPSPEC-UPDATE-DRIVER-0001 – Nova Driver Update

## Status

Angenommen

## Kategorie

Update / Driver / Hardware Lifecycle

## Zweck

NovaOS definiert einen kontrollierten Update-Lebenszyklus für Gerätetreiber. Treiber sollen abhängig von Isolation, Hardwarezustand und Update-Fähigkeit live ersetzt, gestaged oder beim nächsten Boot aktiviert werden können.

```text
Running Driver
      ↓
Prepare New Driver
      ↓
Validate
      ↓
Quiesce Device
      ↓
Transfer / Preserve State
      ↓
Switch Driver
      ↓
Verify Device
```

## Grundprinzipien

```text
Driver Update ≠ File Replacement
Loaded ≠ Bound
Bound ≠ Operational
Operational ≠ Healthy
Signed ≠ Compatible
Compatible ≠ Authorized
Driver State ≠ Device State
Driver Restart ≠ Device Reset
Update Failure ≠ System Failure
```

## Driver Update Model

```text
DriverUpdate
├── UpdateID
├── DriverID
├── DeviceID
├── OldVersion
├── NewVersion
├── TargetHardware
├── UpdateMode
└── State
```

Optional:

```text
OldBuildID
NewBuildID
ContentID
ABIRequirements
CapabilityRequirements
DeviceStateSchema
FirmwareRequirements
RollbackPlan
VerificationPlan
ProvenanceID
```

## Update Modes

NovaOS unterstützt abhängig vom Treiber:

```text
Live Replace
Driver Restart
Device Rebind
Next Boot
A/B System Update
Recovery Update
```

Der sicherste geeignete Modus wird anhand von Driver Contract, Hardwarezustand und System Policy gewählt.

## Compatibility

Vor Aktivierung müssen mindestens geprüft werden:

```text
Architecture
Kernel ABI
Driver ABI
Device ID
Hardware Revision
Firmware Version
Required Capabilities
State Schema
Dependencies
Security Policy
```

```text
Hardware Detected ≠ Driver Compatible
```

## Staging

Der neue Treiber wird vor Aktivierung vorbereitet.

```text
Acquire
 ↓
Verify Content
 ↓
Verify Signature
 ↓
Validate Compatibility
 ↓
Stage Driver
```

Der laufende Treiber bleibt währenddessen unverändert.

## Device Quiescence

Vor Live Replacement muss das Gerät in einen kontrollierten Zustand gebracht werden.

```text
Stop New Requests
      ↓
Drain I/O
      ↓
Complete / Cancel DMA
      ↓
Stabilize Device
      ↓
Switch Driver
```

Der erforderliche Quiescence-Scope soll möglichst klein bleiben.

## DMA und IOMMU

Vor Entfernung eines Treibers müssen aktive DMA-Operationen kontrolliert behandelt werden.

```text
Driver v1
   ↓
Stop DMA
   ↓
Revoke DMA Mapping
   ↓
Validate IOMMU State
   ↓
Driver v2
```

Ein alter Treiber darf nach Ablösung keinen fortbestehenden unautorisierten Gerätezugriff besitzen.

## Capability Revalidation

Capabilities werden nicht automatisch vom alten Treiber übernommen.

```text
Driver v1 Authority
        ↓
Revalidate
        ↓
Driver v2 Authority
```

Die neue Treiberversion erhält nur aktuell benötigte und autorisierte Rechte.

## State Transfer

Falls der Gerätetreiber laufenden Zustand besitzt:

```text
Driver State v1
      ↓
Export
      ↓
Validate / Migrate
      ↓
Import
      ↓
Driver State v2
```

State Transfer muss explizit definiert sein.

```text
Memory Copy ≠ State Migration
```

## Device State

Hardware kann eigenen Zustand besitzen:

```text
Registers
Queues
DMA State
Buffers
Firmware State
Connection State
```

NovaOS muss unterscheiden zwischen:

```text
Preserve
Reconstruct
Reinitialize
Reset
```

Ein Device Reset darf nicht automatisch vorausgesetzt werden.

## Live Replacement

Geeignete isolierte Treiber können live ersetzt werden.

```text
Driver v1
    ↓
Quiesce
    ↓
Prepare v2
    ↓
Atomic Rebind
    ↓
Driver v2
    ↓
Verify
```

User-Mode Driver Domains sollen Live Replacement bevorzugen, wenn Hardware und Driver Contract dies zulassen.

## Kernel-Mode Driver

Kernel-Treiber benötigen strengere Anforderungen.

Kann sichere Live-Ablösung nicht gewährleistet werden:

```text
Stage Driver
     ↓
Activate Next Boot
```

Ein unsicherer Live Update darf nicht allein zur Vermeidung eines Neustarts erzwungen werden.

## I/O Handling

Während des Updates müssen offene I/O-Requests eindeutig behandelt werden.

Mögliche Strategien:

```text
Complete
Drain
Cancel
Retry
Replay
Fail Explicitly
```

Requests dürfen nicht zwischen alter und neuer Treiberinstanz verloren gehen oder doppelt ausgeführt werden.

## Rollback

Schlägt die neue Treiberversion fehl:

```text
Driver v2
    ↓
Contain Failure
    ↓
Rollback / Rebind
    ↓
Driver v1
    ↓
Verify Device
```

Ist der alte Treiber mit dem aktuellen Hardwarezustand nicht mehr kompatibel, muss Recovery oder Device Reinitialization verwendet werden.

## Driver Isolation

Treiberfehler sollen möglichst innerhalb der Driver Domain verbleiben.

```text
Driver Failure
     ↓
Contain
     ↓
Restart / Replace / Rollback
```

```text
Driver Failure ≠ Kernel Failure
```

Dies gilt insbesondere für isolierte User-Mode-Treiber.

## Firmware-Abhängigkeiten

Treiberupdates können bestimmte Firmware-Versionen voraussetzen.

```text
Driver v2
   ↓
Requires Firmware ≥ X
```

Treiber- und Firmware-Update müssen bei harter Abhängigkeit gemeinsam geplant werden.

## Transaction Integration

```text
Begin
 ↓
Stage Driver
 ↓
Validate
 ↓
Quiesce Device
 ↓
Prepare State
 ↓
Switch / Rebind
 ↓
Verify
 ↓
Commit / Rollback
```

## Verification

Nach Aktivierung müssen mindestens relevante Eigenschaften geprüft werden:

```text
Driver Loaded
Device Bound
Device Responsive
I/O Functional
DMA Valid
Capabilities Valid
No Critical Errors
Contract Satisfied
```

```text
Driver Loaded ≠ Device Healthy
```

## Recovery

Kann weder neue noch vorherige Treiberversion verwendet werden:

```text
Disable Device
      ↓
Degraded Mode
      ↓
Recovery Driver /
Recovery Environment
```

Ein nicht essenzielles Gerät darf deaktiviert werden, statt die Stabilität des gesamten Systems zu gefährden.

## Provenance

NovaOS soll nachvollziehen können:

```text
UpdateID
DriverID
DeviceID
OldVersion
NewVersion
BuildID
ContentID
UpdateMode
StateMigration
VerificationResult
RollbackResult
FailureReason
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Driver Version
Driver Build
Bound Device
Update Capability
Update State
Isolation Domain
Capability Set
DMA State
Device Health
Rollback Availability
Failure Reason
```

## Normative Anforderungen

1. NovaOS MUSS kontrollierte Driver Updates unterstützen.
2. Treiberupdates MÜSSEN Hardware- und ABI-Kompatibilität prüfen.
3. Treiberupdates MÜSSEN Integritäts-, Signatur-, Trust- und Authorization-Prüfungen durchlaufen.
4. Neue Treiber SOLLEN vor Aktivierung gestaged werden.
5. Laufende Geräteoperationen MÜSSEN vor Live Replacement kontrolliert behandelt werden.
6. Aktive DMA-Operationen MÜSSEN vor Treiberablösung berücksichtigt werden.
7. Alte DMA-Berechtigungen MÜSSEN widerrufbar sein.
8. Capabilities MÜSSEN für die neue Treiberinstanz revalidiert werden.
9. State Transfer MUSS explizit definiert sein.
10. Driver State und Device State MÜSSEN getrennt behandelt werden.
11. Device Reset DARF NICHT automatisch vorausgesetzt werden.
12. Offene I/O-Requests MÜSSEN eindeutig behandelt werden.
13. I/O DARF durch Driver Replacement NICHT unkontrolliert doppelt ausgeführt werden.
14. Geeignete isolierte Treiber SOLLEN live ersetzbar sein.
15. Unsicher live ersetzbare Treiber MÜSSEN einen Boot-basierten Update-Pfad verwenden können.
16. Treiberfehler SOLLEN auf die kleinste mögliche Failure Domain begrenzt werden.
17. Fehlgeschlagene Updates MÜSSEN Rollback, Restart, Rebind oder Recovery ermöglichen.
18. Rollback MUSS aktuellen Device State berücksichtigen.
19. Firmware-Abhängigkeiten MÜSSEN explizit modelliert werden.
20. Treiberupdates MÜSSEN transaktional integrierbar sein.
21. Der neue Treiber MUSS nach Aktivierung verifiziert werden.
22. `Loaded` DARF NICHT als `Healthy` interpretiert werden.
23. Nicht essenzielle fehlerhafte Geräte MÜSSEN deaktivierbar sein.
24. Driver-Update-Entscheidungen MÜSSEN nachvollziehbare Provenance besitzen.
25. Driver-Update-State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-STAGED-0001`
- `NPSPEC-UPDATE-TRANSACTIONAL-0001`
- `NPSPEC-UPDATE-ROLLBACK-0001`
- `NPSPEC-UPDATE-LIVE-0001`
- `NPSPEC-DRIVER-FRAMEWORK-0001`
- `NPSPEC-DRIVER-KERNELMODE-0001`
- `NPSPEC-DRIVER-USERMODE-0001`
- `NPSPEC-DRIVER-ISOLATION-0001`
- `NPSPEC-DRIVER-CAPABILITY-0001`
- `NPSPEC-DRIVER-HOTRELOAD-0001`
- `NPSPEC-DRIVER-LIVEREPLACE-0001`
- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `ADR-ARCH-0181`

## Ergebnis

```text
Running Driver v1
       ↓
Stage Driver v2
       ↓
Validate Hardware + ABI
       ↓
Quiesce Device
       ↓
Drain I/O + DMA
       ↓
Revalidate Capabilities
       ↓
Transfer / Rebuild State
       ↓
Switch Driver
       ↓
Verify Device
      ↙   ↘
   Healthy Failed
      ↓       ↓
   Commit   Rollback /
            Rebind /
            Recovery
```

NovaOS erhält damit einen sicheren Driver-Update-Lebenszyklus, der Treiber und Hardwarezustand getrennt behandelt, Live Replacement für geeignete isolierte Treiber ermöglicht und Fehler möglichst auf die betroffene Driver Domain beziehungsweise das einzelne Gerät begrenzt.