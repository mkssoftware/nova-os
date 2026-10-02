# NPSPEC-UPDATE-BOOTLOADER-0001 – Nova Bootloader Update

## Status

Angenommen

## Kategorie

Update / Bootloader / Boot Integrity

## Zweck

NovaOS definiert einen besonders abgesicherten Update-Lebenszyklus für Bootloader-Komponenten.

Da ein fehlerhafter Bootloader das Starten von NovaOS und seiner Recovery-Umgebung verhindern kann, dürfen Bootloader-Updates nur mit validiertem Fallback- und Recovery-Pfad durchgeführt werden.

```text
Current Bootloader
        ↓
Validate Update
        ↓
Prepare Recovery
        ↓
Stage New Bootloader
        ↓
Activate
        ↓
Boot Test
        ↓
Verify
```

## Grundprinzipien

```text
Bootloader Update ≠ Normal File Update
Written ≠ Bootable
Bootable ≠ Trusted
Trusted ≠ Compatible
Started ≠ Healthy
Newer ≠ Safer
Bootloader Rollback ≠ Security Rollback
Recovery Path ≠ Same Failure Domain
```

## Update Model

```text
BootloaderUpdate
├── UpdateID
├── BootloaderID
├── CurrentVersion
├── TargetVersion
├── BuildID
├── ContentID
├── BootMode
├── Platform
└── State
```

Optional:

```text
BIOSPayload
UEFIPayload
BootConfiguration
RecoveryTarget
FallbackTarget
MinimumSecureVersion
VerificationPlan
RollbackPlan
ProvenanceID
```

## Unterstützte Boot-Pfade

NovaOS muss unterschiedliche Plattformpfade berücksichtigen können:

```text
BIOS
UEFI
```

Beide dürfen unterschiedliche Update-Artefakte besitzen, müssen aber in das gemeinsame NovaOS-Bootmodell überführt werden.

## Target Validation

Vor Installation müssen mindestens geprüft werden:

```text
Architecture
Firmware Interface
Boot Mode
Partition Layout
Boot Medium
Bootloader ABI
NBHP/BIB Compatibility
Secure Boot Policy
Recovery Compatibility
```

Ein Bootloader darf nicht allein aufgrund passender Versionsnummer installiert werden.

## Staging

Neue Bootloader-Artefakte werden vor Aktivierung vollständig vorbereitet.

```text
Acquire
 ↓
Verify ContentID
 ↓
Verify Signature
 ↓
Validate Compatibility
 ↓
Stage
```

Der aktuell funktionsfähige Bootpfad soll währenddessen unverändert bleiben.

## BIOS Update

Bei BIOS-Systemen können beispielsweise betroffen sein:

```text
MBR / Boot Sector
Stage 1
Stage 2
Boot Metadata
Boot Configuration
Recovery References
```

Schreibreihenfolge und On-Disk-Konsistenz müssen so gestaltet werden, dass ein unterbrochenes Update möglichst keinen undefinierten Bootzustand erzeugt.

## UEFI Update

Bei UEFI-Systemen können betroffen sein:

```text
EFI System Partition
Nova EFI Loader
Boot Entries
Fallback Loader
Secure Boot Metadata
Boot Configuration
```

NovaOS muss UEFI-Fallback-Pfade berücksichtigen können.

## Recovery First

Vor Änderung kritischer Bootstrukturen muss ein unabhängiger Recovery-Pfad verfügbar oder explizit als nicht verfügbar erkannt sein.

```text
Validate Recovery
      ↓
Update Bootloader
```

Mögliche Recovery-Pfade:

```text
Secondary Bootloader
A/B Boot Environment
Fallback EFI Loader
NovaDOS
Recovery Partition
External Recovery Medium
Firmware Recovery
```

## A/B Bootloader

Wo technisch möglich, soll ein A/B-Modell verwendet werden.

```text
Bootloader A
    ↓
Prepare B
    ↓
Verify B
    ↓
Select B
    ↓
Trial Boot
```

Der bekannte Bootloader bleibt erhalten, bis die neue Version erfolgreich bestätigt wurde.

## Atomic Activation

Die Umschaltung auf einen neuen Bootloader muss soweit technisch möglich atomar erfolgen.

```text
Old Boot Target
      ↓
Atomic Selection Change
      ↓
New Boot Target
```

Große Bootloader-Artefakte sollen nicht durch direktes Überschreiben des aktiven Exemplars atomar simuliert werden.

## Trial Boot

Ein neuer Bootloader gilt nach dem ersten Start noch nicht automatisch als Known-Good.

```text
Pending
  ↓
Trial Boot
  ↓
Kernel Entry
  ↓
Boot Health Verification
  ↓
Known-Good
```

Die erfolgreiche Übergabe an den Kernel allein muss nicht für vollständige Verifikation ausreichen.

## NBHP/BIB

Der neue Bootloader muss einen gültigen Nova Boot Hand-off / Boot Information Block erzeugen können.

Zu prüfen sind insbesondere:

```text
BIB Version
Memory Information
Framebuffer Information
Firmware Information
Boot Mode
Build Information
Required Boot Metadata
```

Inkompatible Übergabe an den Kernel muss als Bootfehler behandelt werden.

## Secure Boot

Bei aktiviertem Secure Boot müssen Bootloader-Artefakte die aktuelle Trust Policy erfüllen.

```text
Bootloader
   ↓
Signature
   ↓
Trust Chain
   ↓
Boot Authorization
```

Ein Rollback darf widerrufene Schlüssel oder verbotene Bootloader-Versionen nicht reaktivieren.

## Measured Boot

Bootloader-Version und relevante Bootartefakte sollen in die Measured-Boot-Kette einbezogen werden können.

Dadurch kann der tatsächlich gestartete Bootzustand attestierbar bleiben.

## Anti-Rollback

NovaOS muss Minimum-Secure-Versionen berücksichtigen können.

```text
TargetVersion
<
MinimumSecureVersion
→ Reject
```

Known-Good bedeutet nicht automatisch, dass eine alte Version weiterhin sicherheitspolitisch zulässig ist.

## Transaction Integration

```text
Begin
 ↓
Validate
 ↓
Stage
 ↓
Prepare Recovery
 ↓
Revalidate
 ↓
Activate
 ↓
Reboot
 ↓
Verify
 ↓
Commit
```

Bootloader-Updates können eine Transaktion über einen Neustart hinweg fortsetzen.

## Persistent Transaction State

Der Updatezustand muss den Neustart überleben.

```text
PendingBootloaderUpdate
BootAttempt
CandidateBootloader
FallbackBootloader
VerificationPending
```

Nach Crash oder Stromverlust muss bestimmbar bleiben, welcher Bootpfad verwendet werden soll.

## Rollback

Schlägt der neue Bootloader fehl:

```text
Candidate Bootloader
        ↓
Boot Failure
        ↓
Fallback Bootloader
        ↓
Verify
```

Rollback muss aktuelle Secure-Boot-, Trust- und Anti-Rollback-Regeln berücksichtigen.

## Failure Handling

Mögliche Fehler:

```text
Write Failure
Integrity Failure
Signature Failure
Invalid Boot Metadata
Boot Failure
Kernel Handoff Failure
Health Verification Failure
Power Loss
```

Mögliche Reaktionen:

```text
Retry
Fallback Bootloader
A/B Switch
NovaDOS Recovery
Firmware Recovery
External Recovery
```

## Boot Loop Protection

Wiederholte Fehlstarts müssen erkannt werden.

```text
Boot Candidate
      ↓
Failure
      ↓
Attempt Counter
      ↓
Threshold
      ↓
Fallback / Recovery
```

Ein defekter Candidate darf nicht unbegrenzt erneut gestartet werden.

## Provenance

NovaOS soll nachvollziehen können:

```text
UpdateID
BootloaderID
OldVersion
NewVersion
BuildID
ContentID
BootMode
Activation Result
Boot Attempts
Verification Result
Fallback Result
Failure Reason
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Active Bootloader
Candidate Bootloader
Fallback Bootloader
Bootloader Version
BuildID
Integrity State
Trust State
Boot Attempts
Verification State
Rollback Availability
Recovery Availability
```

## Normative Anforderungen

1. NovaOS MUSS Bootloader-Updates kontrolliert verwalten können.
2. Bootloader-Updates MÜSSEN Plattform und Boot Mode berücksichtigen.
3. BIOS- und UEFI-Artefakte MÜSSEN eindeutig unterscheidbar sein.
4. Bootloader-Artefakte MÜSSEN vor Aktivierung auf Integrität geprüft werden.
5. Kritische Bootloader-Artefakte MÜSSEN Signatur-, Trust- und Authorization-Prüfungen durchlaufen.
6. Bootloader-Kompatibilität mit Kernel und NBHP/BIB MUSS geprüft werden können.
7. Neue Bootloader SOLLEN vor Aktivierung gestaged werden.
8. Vor kritischer Aktivierung MUSS ein Recovery-Pfad geprüft werden.
9. Fehlende Recovery-Möglichkeit MUSS explizit erkennbar sein.
10. Der bestehende Known-Good-Bootpfad SOLL bis zur erfolgreichen Verifikation erhalten bleiben.
11. A/B-Bootloader SOLLEN unterstützt werden, wenn die Plattform dies ermöglicht.
12. Die Bootloader-Aktivierung MUSS soweit technisch möglich atomar erfolgen.
13. Ein neuer Bootloader MUSS als Candidate beziehungsweise Pending behandelt werden können.
14. `Booted` DARF NICHT automatisch als `Known-Good` gelten.
15. Bootloader-Updates MÜSSEN über Neustarts hinweg transaktional fortsetzbar sein.
16. Der Updatezustand MUSS persistent rekonstruierbar sein.
17. Boot-Versuche MÜSSEN begrenzbar sein.
18. Wiederholte Fehlstarts MÜSSEN Fallback oder Recovery auslösen können.
19. Secure-Boot-Regeln MÜSSEN während Update und Rollback erhalten bleiben.
20. Measured Boot SOLL den neuen Bootloaderzustand erfassen können.
21. Minimum-Secure-Versionen MÜSSEN Rollback blockieren können.
22. Widerrufene Schlüssel DÜRFEN durch Rollback NICHT reaktiviert werden.
23. Stromausfall DARF keinen undefinierten Bootzustand erzeugen, soweit die Plattform dies technisch verhindern kann.
24. Fehlgeschlagene Bootloader-Updates MÜSSEN einen definierten Recovery-Pfad besitzen.
25. Bootloader-Update-Entscheidungen MÜSSEN nachvollziehbare Provenance besitzen.
26. Bootloader-Update-State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-SIGNING-0001`
- `NPSPEC-UPDATE-ATOMIC-0001`
- `NPSPEC-UPDATE-TRANSACTIONAL-0001`
- `NPSPEC-UPDATE-AB-0001`
- `NPSPEC-UPDATE-STAGED-0001`
- `NPSPEC-UPDATE-ROLLBACK-0001`
- `NPSPEC-UPDATE-FIRMWARE-0001`
- `NPSPEC-BOOT-ARCH-0001`
- `NPSPEC-BOOT-BIOS-0001`
- `NPSPEC-BOOT-UEFI-0001`
- `NPSPEC-BOOT-SECURE-0001`
- `NPSPEC-BOOT-MEASURED-0001`
- `NPSPEC-BOOT-RECOVERY-0001`
- `NPSPEC-BOOT-AB-0001`
- `NPSPEC-BOOT-ROLLBACK-0001`
- `NPSPEC-BOOT-HEALTH-0001`
- `ADR-ARCH-0183`

## Ergebnis

```text
Known-Good Bootloader
         ↓
Stage Candidate
         ↓
Verify Content + Trust
         ↓
Validate Recovery
         ↓
Atomic Activation
         ↓
Trial Boot
         ↓
Verify Boot Chain
        ↙   ↘
     Healthy Failed
        ↓       ↓
    Known-Good Fallback
                ↓
             Recovery
```

NovaOS erhält damit einen besonders abgesicherten Update-Pfad für seinen Bootloader, bei dem ein neuer Bootpfad erst nach erfolgreicher Integritäts-, Trust-, Kompatibilitäts- und Boot-Verifikation als Known-Good gilt und ein fehlerhaftes Update möglichst nicht den unabhängigen Recovery-Zugang zum System zerstören kann.