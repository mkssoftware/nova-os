# NPSPEC-UPDATE-AB-0001 – Nova A/B Update

## Status

Angenommen

## Kategorie

Update / A/B Update / Boot Resilience

## Zweck

NovaOS definiert A/B-Updates für boot- und systemkritische Komponenten. Eine aktive, bekannte funktionsfähige Systemgeneration bleibt verfügbar, während eine neue Generation getrennt vorbereitet und anschließend kontrolliert aktiviert wird.

```text
Slot A = Active / Known-Good
Slot B = Inactive

Update
  ↓
Prepare B
  ↓
Validate B
  ↓
Switch Boot Target
  ↓
Boot B
  ↓
Verify
 ↙   ↘
OK   Failed
↓       ↓
B       A
```

## Grundprinzipien

```text
A/B ≠ Backup
A/B ≠ Snapshot
Inactive ≠ Invalid
Booted ≠ Healthy
Active ≠ Known-Good
Switch ≠ Successful Update
Rollback ≠ Restore Old Security State
Slot Identity ≠ System Identity
```

## Slot Model

```text
ABSlot
├── SlotID
├── Generation
├── SystemVersion
├── BuildID
├── State
├── IntegrityState
├── VerificationState
└── BootState
```

Slot-Zustände:

```text
Inactive
Staging
Ready
PendingBoot
Booting
Active
Healthy
Failed
RollbackTarget
Unknown
```

## Slot-Trennung

A und B müssen logisch getrennte Update-Ziele bilden.

```text
Active Slot A
      │
      └── remains operational

Inactive Slot B
      ↓
Update + Validation
```

Das Update des inaktiven Slots darf den aktiven Slot nicht unkontrolliert verändern.

## Update-Ablauf

```text
Determine Active Slot
        ↓
Select Inactive Slot
        ↓
Stage Update
        ↓
Verify Package
        ↓
Resolve Dependencies
        ↓
Validate Slot
        ↓
Mark PendingBoot
        ↓
Atomic Boot Switch
```

Der bisherige Slot bleibt als Recovery-Ziel erhalten.

## Boot Selection

Die Boot-Auswahl muss persistent und crash-sicher gespeichert werden.

```text
PreferredSlot
FallbackSlot
BootAttempts
HealthState
Generation
```

Der Wechsel des bevorzugten Slots muss atomar erfolgen.

## Trial Boot

Eine neue Generation wird zunächst als noch nicht bestätigt behandelt.

```text
PendingBoot
    ↓
Trial Boot
    ↓
Health Verification
```

```text
Boot Success ≠ Update Success
```

Erst erfolgreiche Systemverifikation darf die neue Generation als `Healthy` beziehungsweise `Known-Good` bestätigen.

## Health Verification

Die Verifikation kann umfassen:

```text
Kernel Started
Critical Drivers Ready
Storage Available
Required Services Ready
Integrity Valid
Contracts Satisfied
State Migration Valid
Security State Valid
```

## Boot Attempts

NovaOS muss fehlgeschlagene Startversuche begrenzen können.

```text
Boot B
 ↓
Failure
 ↓
Attempt Counter
 ↓
Threshold reached?
├── No  → Retry B
└── Yes → Fallback A
```

Dadurch werden permanente Boot-Loops vermieden.

## Fallback

Kann die neue Generation nicht erfolgreich verifiziert werden:

```text
Slot B Failed
      ↓
Select Slot A
      ↓
Boot A
      ↓
Verify A
```

Der vorherige Slot darf nur verwendet werden, wenn er weiterhin als zulässiger Recovery-Zustand gilt.

## State Separation

System- und Benutzerdaten müssen vom Slot-Modell getrennt betrachtet werden.

```text
System A/B
    +
Persistent User State
    +
Shared System State
```

Shared State darf nicht blind zwischen inkompatiblen Generationen verwendet werden.

## State Migration

Migrationen müssen A/B-Fallback berücksichtigen.

```text
System A
State v1
   ↓
System B
State v2
```

Ist `State v2` nicht rückwärtskompatibel, muss eine Strategie existieren:

```text
Dual-Compatible State
Snapshot
Copy-on-Write Migration
Reverse Migration
Compensation
Recovery
```

```text
System Rollback Possible
≠
State Rollback Possible
```

## Security State

Monotone Sicherheitszustände bleiben außerhalb unsicherer Rollback-Semantik.

Beispiele:

```text
Revoked Keys
Revoked Certificates
Minimum Secure Version
Compromise State
Security Counter
```

Ein Fallback auf Slot A darf diese Informationen nicht zurücksetzen.

## Atomic Integration

Der Wechsel zwischen Slots nutzt das atomare Update-Modell.

```text
Prepare B
   ↓
Validate B
   ↓
Atomic Boot Selection
```

Ein Stromausfall während des Wechsels darf keinen Zustand erzeugen, in dem kein gültiger Boot-Pfad bestimmbar ist.

## Transaction Integration

A/B ist Bestandteil der Update-Transaktion:

```text
Begin
 ↓
Stage Inactive Slot
 ↓
Validate
 ↓
Prepare Boot Switch
 ↓
Commit
 ↓
Boot Candidate
 ↓
Verify
 ↓
Finalize / Fallback
```

## Live Evolution

Komponenten, die sicher live aktualisiert werden können, benötigen nicht zwingend einen vollständigen A/B-Systemwechsel.

A/B bleibt insbesondere für Änderungen geeignet, deren Fehler den Boot- oder Recovery-Pfad gefährden könnten.

## Recovery

Sind beide Slots nicht nutzbar:

```text
A Failed
+
B Failed
   ↓
Nova Recovery
   ↓
Repair / Reinstall / Restore
```

A/B ersetzt daher nicht NovaDOS oder andere Recovery-Mechanismen.

## Slot Cleanup

Der vorherige Known-Good-Slot darf nicht unmittelbar nach dem ersten erfolgreichen Start überschrieben werden.

Erst nach definierter erfolgreicher Verifikation und Finalisierung darf er wieder als zukünftiges Update-Ziel freigegeben werden.

## Provenance

NovaOS soll nachvollziehen können:

```text
SlotID
Generation
BuildID
UpdateID
TransactionID
PreviousSlot
ActivationTime
BootAttempts
HealthResult
VerificationResult
FallbackReason
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Active Slot
Inactive Slot
Preferred Slot
Fallback Slot
Slot Generations
System Versions
Health States
Boot Attempts
Verification State
Rollback Availability
```

## Normative Anforderungen

1. NovaOS MUSS A/B-Updates für bootkritische Systemänderungen unterstützen können.
2. Aktiver und inaktiver Slot MÜSSEN eindeutig unterscheidbar sein.
3. Der aktive Slot DARF während des Staging nicht unkontrolliert verändert werden.
4. Der inaktive Slot MUSS vor Aktivierung validierbar sein.
5. Der Boot-Slot-Wechsel MUSS atomar und persistent sein.
6. Nach Stromausfall MUSS ein gültiger Boot-Pfad bestimmbar bleiben.
7. Eine neue Generation MUSS zunächst als unbestätigt behandelt werden können.
8. `Booted` DARF NICHT automatisch als `Healthy` gelten.
9. Die neue Generation MUSS vor Known-Good-Markierung verifiziert werden.
10. Boot-Versuche MÜSSEN begrenzbar sein.
11. Wiederholtes Boot-Versagen MUSS automatischen Fallback ermöglichen.
12. Der Fallback-Slot MUSS vor Verwendung weiterhin zulässig sein.
13. Shared State MUSS auf Versionskompatibilität geprüft werden.
14. State Migration MUSS einen möglichen A/B-Fallback berücksichtigen.
15. System-Rollback DARF NICHT automatisch State-Rollback voraussetzen.
16. Monotone Security States DÜRFEN durch Fallback NICHT zurückgesetzt werden.
17. A/B MUSS mit dem transaktionalen Update-Modell integrierbar sein.
18. Der vorherige Known-Good-Slot DARF NICHT vor erfolgreicher Finalisierung unnötig zerstört werden.
19. Fehler beider Slots MÜSSEN in einen unabhängigen Recovery-Pfad führen können.
20. A/B DARF NICHT als Ersatz für Backup oder Recovery interpretiert werden.
21. Slot- und Boot-Zustände MÜSSEN persistent rekonstruierbar sein.
22. A/B-Entscheidungen MÜSSEN nachvollziehbare Provenance besitzen.
23. Slot-, Health- und Fallback-State MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-ATOMIC-0001`
- `NPSPEC-UPDATE-TRANSACTIONAL-0001`
- `NPSPEC-STATE-VERSIONING-0001`
- `NPSPEC-STATE-SNAPSHOT-0001`
- `NPSPEC-STATE-ROLLBACK-0001`
- `NPSPEC-BOOT-AB-0001`
- `NPSPEC-BOOT-ROLLBACK-0001`
- `NPSPEC-BOOT-HEALTH-0001`
- `NPSPEC-BOOT-RECOVERY-0001`
- `NPSPEC-RESILIENCE-RECOVERYMODE-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `ADR-ARCH-0170`

## Ergebnis

```text
Known-Good A
     ↓
Prepare B
     ↓
Validate B
     ↓
Atomic Boot Switch
     ↓
Trial Boot B
     ↓
Verify B
    ↙   ↘
Healthy  Failed
   ↓       ↓
Confirm B Fallback A
   ↓       ↓
B becomes Verify A
Known-Good
```

NovaOS erhält damit ein robustes A/B-Update-Modell, bei dem eine neue Systemgeneration vollständig getrennt vorbereitet und getestet werden kann, während die vorherige bekannte funktionsfähige Generation als unmittelbarer Fallback erhalten bleibt.