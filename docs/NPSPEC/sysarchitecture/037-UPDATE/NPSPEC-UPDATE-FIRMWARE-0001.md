# NPSPEC-UPDATE-FIRMWARE-0001 – Nova Firmware Update

## Status

Angenommen

## Kategorie

Update / Firmware / Hardware Lifecycle

## Zweck

NovaOS definiert einen kontrollierten Update-Lebenszyklus für Firmware von Plattformen, Geräten und Hardwarekomponenten.

Firmware-Updates werden wegen ihres erhöhten Ausfallrisikos besonders streng validiert, vorbereitet und verifiziert.

```text
Current Firmware
       ↓
Validate Update
       ↓
Prepare Recovery
       ↓
Stage Firmware
       ↓
Apply
       ↓
Hardware Restart / Reset
       ↓
Verify
```

## Grundprinzipien

```text
Firmware ≠ Driver
Firmware Update ≠ Normal File Update
Signed ≠ Compatible
Compatible ≠ Authorized
Flashed ≠ Active
Active ≠ Healthy
Newer ≠ Safer
Rollback Available ≠ Rollback Safe
Firmware Failure ≠ Undefined Recovery
```

## Firmware Update Model

```text
FirmwareUpdate
├── UpdateID
├── DeviceID
├── FirmwareID
├── CurrentVersion
├── TargetVersion
├── HardwareCompatibility
├── UpdateMethod
└── State
```

Optional:

```text
BuildID
ContentID
VendorID
DeviceRevision
MinimumVersion
RollbackPolicy
RecoveryImage
PowerRequirements
DriverDependencies
VerificationPlan
ProvenanceID
```

## Firmware Types

NovaOS kann unter anderem verwalten:

```text
UEFI / Platform Firmware
Device Firmware
Storage Firmware
Network Firmware
GPU Firmware
Controller Firmware
Peripheral Firmware
Embedded Controller Firmware
```

Nicht jede Firmware muss durch NovaOS direkt aktualisierbar sein.

## Hardware Binding

Firmware muss eindeutig an kompatible Hardware gebunden sein.

```text
Firmware
   ↓
VendorID
DeviceID
Revision
Platform
   ↓
Compatibility Match
```

Ein ähnlicher Gerätename reicht nicht als Identifikation.

## Trust Verification

Vor Installation müssen mindestens geprüft werden:

```text
Content Integrity
Signature
Signer Trust
Firmware Identity
Hardware Compatibility
Security Policy
Minimum Secure Version
Revocation State
```

Firmware aus unbekannter oder nicht autorisierter Quelle darf nicht automatisch installiert werden.

## Staging

Firmware soll vor der eigentlichen Installation vollständig vorbereitet werden.

```text
Acquire
 ↓
Verify
 ↓
Stage
 ↓
Revalidate
 ↓
Flash
```

Damit darf ein Downloadfehler nicht während eines kritischen Flash-Vorgangs auftreten müssen.

## Power Safety

Firmware-Updates können durch Stromverlust Hardware unbrauchbar machen.

Vor kritischen Updates müssen deshalb relevante Bedingungen geprüft werden können:

```text
Stable Power
Battery Level
External Power
Recovery Capability
Device Availability
```

Bei unzureichenden Voraussetzungen muss das Update verschoben oder blockiert werden können.

## Update Methods

Abhängig von Hardware und Firmware können unterschiedliche Verfahren verwendet werden:

```text
Runtime Update
Device Reset
System Restart
Next Boot
UEFI Capsule
Recovery Environment
Vendor-Specific Protocol
```

Der Update Manager wählt keinen unsicheren Runtime-Pfad allein zur Vermeidung eines Neustarts.

## Atomic Firmware

Unterstützt die Hardware Dual-Bank- oder A/B-Firmware:

```text
Firmware Bank A
      ↓
Write Bank B
      ↓
Verify B
      ↓
Switch
```

soll diese Möglichkeit bevorzugt werden.

```text
Firmware A/B ≠ NovaOS System A/B
```

Beide Mechanismen können jedoch koordiniert werden.

## Driver Coordination

Treiber und Firmware können voneinander abhängen.

```text
Driver v3
    ↓
Requires Firmware ≥ v5
```

Der Update Manager muss solche Abhängigkeiten gemeinsam planen können.

Mögliche Reihenfolgen:

```text
Firmware → Restart → Driver

Driver Preparation
      ↓
Firmware
      ↓
Driver Activation
```

## Device Quiescence

Vor Firmware-Aktualisierung muss ein Gerät gegebenenfalls stillgelegt werden.

```text
Stop New I/O
    ↓
Drain Requests
    ↓
Stop DMA
    ↓
Quiesce Device
    ↓
Firmware Update
```

Offene Operationen dürfen nicht unkontrolliert verloren gehen.

## Flash Process

Der eigentliche Flash-Vorgang ist eine kritische Phase.

```text
Enter Update Mode
      ↓
Write Firmware
      ↓
Verify Written Content
      ↓
Finalize
      ↓
Reset / Activate
```

NovaOS darf einen begonnenen kritischen Flash-Vorgang nicht unnötig unterbrechen.

## Rollback

Firmware Rollback ist nur zulässig, wenn Hardware und Security Policy ihn unterstützen.

```text
Firmware v6
    ↓
Failure
    ↓
Firmware v5
```

Vor Rollback müssen geprüft werden:

```text
Hardware Support
Rollback Image
Security Policy
Minimum Secure Version
State Compatibility
Anti-Rollback Counter
```

## Anti-Rollback

Hardware kann monotone Versionszähler oder Anti-Rollback-Mechanismen besitzen.

NovaOS muss diese respektieren.

```text
TargetVersion
<
MinimumSecureVersion
→ Reject
```

Ein Recovery-Vorgang darf Anti-Rollback-Schutz nicht umgehen.

## Recovery

Firmware-Updates sollen verfügbare Hardware-Recovery-Mechanismen berücksichtigen.

Beispiele:

```text
Dual Firmware Bank
Recovery Partition
Boot ROM
Recovery Mode
Fallback Firmware
External Recovery
```

Existiert kein sicherer Recovery-Pfad, muss das höhere Risiko vor Installation berücksichtigt werden.

## Transaction Integration

Firmware kann Bestandteil einer größeren Update-Transaktion sein.

```text
System Update
├── Firmware
├── Driver
└── System Component
```

Da Firmwareänderungen nicht immer vollständig rollbackfähig sind, muss die Transaktion irreversible Schritte explizit kennzeichnen.

## Verification

Nach Aktivierung muss der Firmwarezustand erneut geprüft werden.

```text
Read Firmware Version
      ↓
Check Device Identity
      ↓
Check Device Health
      ↓
Verify Driver Communication
      ↓
Operational Test
```

```text
Flash Successful ≠ Device Healthy
```

## Failure Handling

Mögliche Reaktionen:

```text
Retry
Device Reset
Firmware Rollback
Fallback Bank
Disable Device
Degraded Mode
Recovery Environment
Manual Recovery
```

Ein nicht essenzielles Gerät darf deaktiviert werden, wenn dadurch das restliche System stabil weiterarbeiten kann.

## Provenance

NovaOS soll nachvollziehen können:

```text
UpdateID
FirmwareID
DeviceID
OldVersion
NewVersion
ContentID
Signer
UpdateMethod
ActivationResult
VerificationResult
RollbackResult
FailureReason
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Current Firmware
Available Firmware
Hardware Compatibility
Minimum Secure Version
Update Capability
Update State
Recovery Capability
Rollback Capability
Verification State
Failure Reason
```

## Normative Anforderungen

1. NovaOS MUSS Firmware-Updates kontrolliert verwalten können.
2. Firmware MUSS eindeutig an kompatible Hardware gebunden werden können.
3. Firmware-Updates MÜSSEN auf Integrität geprüft werden.
4. Kritische Firmware MUSS Signatur-, Trust- und Authorization-Prüfungen durchlaufen.
5. Hardware-Revision und Firmware-Kompatibilität MÜSSEN berücksichtigt werden.
6. Minimum-Secure-Version-Regeln MÜSSEN durchsetzbar sein.
7. Revoked Firmware oder Signer MÜSSEN blockierbar sein.
8. Firmware SOLL vor Installation vollständig gestaged werden.
9. Kritische Power-Anforderungen MÜSSEN vor Flash-Vorgängen prüfbar sein.
10. Unsichere Update-Bedingungen MÜSSEN Installation blockieren können.
11. Geräte MÜSSEN bei Bedarf vor Firmware-Updates quieszierbar sein.
12. Aktive I/O- und DMA-Operationen MÜSSEN berücksichtigt werden.
13. Unterstützte Dual-Bank-/A/B-Verfahren SOLLEN bevorzugt werden.
14. Driver- und Firmware-Abhängigkeiten MÜSSEN gemeinsam planbar sein.
15. Firmware-Updates DÜRFEN einen Neustart oder Device Reset verlangen.
16. Ein Neustart DARF NICHT allein zur Komfortoptimierung durch einen unsicheren Runtime-Pfad vermieden werden.
17. Firmware Rollback MUSS Hardware- und Security-Regeln berücksichtigen.
18. Anti-Rollback-Mechanismen DÜRFEN NICHT umgangen werden.
19. Irreversible Firmware-Schritte MÜSSEN explizit gekennzeichnet werden.
20. Verfügbare Recovery-Mechanismen MÜSSEN vor kritischen Updates berücksichtigt werden.
21. Firmware MUSS nach Aktivierung verifiziert werden.
22. `Flashed` DARF NICHT als `Healthy` interpretiert werden.
23. Fehlgeschlagene Firmware-Updates MÜSSEN in einen definierten Recovery-Zustand übergehen können.
24. Firmware-Update-Entscheidungen MÜSSEN nachvollziehbare Provenance besitzen.
25. Firmware-Update-State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-PACKAGE-0001`
- `NPSPEC-UPDATE-SIGNING-0001`
- `NPSPEC-UPDATE-STAGED-0001`
- `NPSPEC-UPDATE-TRANSACTIONAL-0001`
- `NPSPEC-UPDATE-ROLLBACK-0001`
- `NPSPEC-UPDATE-DRIVER-0001`
- `NPSPEC-BOOT-UEFI-0001`
- `NPSPEC-BOOT-SECURE-0001`
- `NPSPEC-BOOT-RECOVERY-0001`
- `NPSPEC-DRIVER-LIVEREPLACE-0001`
- `NPSPEC-HAL-FIRMWARE-0001`
- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-TRUST-SOFTWARE-0001`
- `ADR-ARCH-0182`

## Ergebnis

```text
Firmware Update
      ↓
Identify Hardware
      ↓
Verify Firmware
      ↓
Check Security + Compatibility
      ↓
Prepare Recovery
      ↓
Check Power + Device State
      ↓
Stage
      ↓
Quiesce Device
      ↓
Flash
      ↓
Reset / Activate
      ↓
Verify
     ↙   ↘
 Healthy Failed
    ↓       ↓
 Complete  Fallback /
           Rollback /
           Recovery
```

NovaOS erhält damit einen sicheren Firmware-Update-Lebenszyklus, der die besonderen Risiken hardwarenaher und teilweise irreversibler Aktualisierungen berücksichtigt und Firmware, Treiber, Boot, Security und Recovery kontrolliert miteinander verbindet.