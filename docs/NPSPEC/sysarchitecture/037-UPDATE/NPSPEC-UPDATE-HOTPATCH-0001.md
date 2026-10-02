# NPSPEC-UPDATE-HOTPATCH-0001 – Nova Hotpatch

## Status

Angenommen

## Kategorie

Update / Hotpatch / Runtime Patching

## Zweck

NovaOS definiert Hotpatching für kleine, gezielte Änderungen an bereits laufenden Komponenten, ohne die gesamte Komponente ersetzen oder das System neu starten zu müssen.

```text
Running Code
    ↓
Prepare Patch
    ↓
Validate
    ↓
Reach Safe Patch Point
    ↓
Atomic Patch
    ↓
Verify
```

Hotpatching ist insbesondere für dringende Sicherheits- und Fehlerkorrekturen vorgesehen.

## Grundprinzipien

```text
Hotpatch ≠ Live Update
Hotpatch ≠ Arbitrary Memory Modification
Patch Loaded ≠ Patch Active
Patch Active ≠ Patch Verified
Signed Patch ≠ Authorized Patch
Compatible Patch ≠ Safe Patch
Patch Success ≠ Component Healthy
```

## Hotpatch Model

```text
Hotpatch
├── PatchID
├── TargetID
├── TargetBuildID
├── TargetContentID
├── PatchVersion
├── PatchPayload
├── PatchPoints
└── VerificationState
```

Optional:

```text
RequiredState
Architecture
Dependencies
SecurityPolicy
RollbackData
ExecutionContract
ProvenanceID
```

## Target Binding

Ein Hotpatch muss eindeutig an den erwarteten Zielcode gebunden sein.

```text
Expected BuildID
Expected ContentID
Expected Patch Point
        ↓
Actual Runtime Target
        ↓
Match?
```

Versionsnummern allein reichen nicht aus.

```text
Same Version ≠ Same Binary
```

## Patch Points

Hotpatchbare Komponenten sollen definierte Patch Points bereitstellen.

```text
Original Function
      ↓
Patch Point
      ↓
Replacement Function
```

Patch Points können beispielsweise sein:

```text
Function Entry
Dispatch Table
Indirection Stub
Versioned Call Target
Explicit Patch Hook
```

Unkontrollierte Änderungen beliebiger Instruktionen sollen vermieden werden.

## Safe Patch State

Vor Aktivierung muss NovaOS sicherstellen, dass der betroffene Code sicher geändert werden kann.

```text
Locate Patch Point
      ↓
Check Active Execution
      ↓
Synchronize CPUs / Tasks
      ↓
Reach Safe State
      ↓
Apply Patch
```

Kein Thread oder CPU darf einen inkonsistenten Zwischenzustand ausführen.

## Atomic Activation

Die sichtbare Umschaltung muss atomar erfolgen.

```text
Calls → Old Code

Atomic Switch

Calls → Patched Code
```

Andere CPUs dürfen niemals teilweise aktualisierte Instruktionsfolgen beobachten.

## SMP Synchronization

Auf Mehrkernsystemen muss Hotpatching CPU-übergreifend synchronisiert werden.

Zu berücksichtigen sind:

```text
Instruction Cache
Memory Ordering
Active Stack Frames
Concurrent Execution
Interrupt Context
Preemption
CPU Affinity
```

## Active Stack Frames

Bereits laufende Aufrufe können weiterhin alten Code verwenden.

Mögliche Strategien:

```text
Drain
Wait
Redirect
Versioned Execution
Reject Patch
```

Ein Patch darf keine Rücksprungadresse oder laufende Ausführung ungültig machen.

## State Compatibility

Hotpatches sollen bestehende Datenstrukturen möglichst unverändert weiterverwenden.

Falls State geändert werden muss:

```text
Old State
   ↓
Explicit Migration
   ↓
New State
```

Komplexe State-Migrationen sollen bevorzugt über `NPSPEC-UPDATE-LIVE-0001` erfolgen.

## Security

Hotpatches besitzen dieselben Sicherheitsanforderungen wie andere systemkritische Updates.

```text
Patch
 ↓
Integrity
 ↓
Signature
 ↓
Trust
 ↓
Authorization
 ↓
Compatibility
```

Hotpatching darf Code Integrity nicht umgehen.

## Capability Safety

Ein Hotpatch darf die Autorität einer Komponente nicht implizit erweitern.

```text
Authority After Patch
⊆
Authorized Authority
```

Neue Capability-Anforderungen müssen explizit validiert werden.

## Kernel Hotpatching

Kernel-Hotpatches benötigen besonders strenge Anforderungen.

Geeignete Ziele können sein:

```text
Isolated Functions
Bug Fixes
Security Fixes
Small Logic Changes
```

Besonders kritisch sind:

```text
Scheduler
Interrupt Handling
Memory Management
Synchronization
Capability Enforcement
Syscall Paths
```

Kann sichere Laufzeitänderung nicht bewiesen oder ausreichend validiert werden, muss ein Live-, A/B-, Immutable- oder Neustart-Update verwendet werden.

## Patch Stacking

Mehrere Hotpatches können voneinander abhängen.

```text
Base Build
   ↓
Patch P1
   ↓
Patch P2
```

NovaOS muss Patch-Reihenfolge und Abhängigkeiten nachvollziehen können.

Ein neuer vollständiger Build darf mehrere vorherige Hotpatches konsolidieren.

## Rollback

Reversible Hotpatches sollen deaktivierbar sein.

```text
Patched Code
     ↓
Rollback
     ↓
Original Code
```

Vor Rollback müssen erneut Runtime-, State- und Security-Bedingungen geprüft werden.

```text
Patch Reversible ≠ Rollback Currently Safe
```

## Transaction Integration

Hotpatching verwendet einen kompakten transaktionalen Ablauf:

```text
Begin
 ↓
Validate Target
 ↓
Prepare Patch
 ↓
Synchronize
 ↓
Atomic Activate
 ↓
Verify
 ↓
Commit / Rollback
```

## Verification

Nach Aktivierung müssen relevante Eigenschaften geprüft werden.

```text
Patch Integrity
Target Binding
Runtime Health
Contract Compliance
Security Invariants
```

Bei kritischen Fehlern muss Rollback oder Recovery ausgelöst werden können.

## Realtime

Hotpatching darf harte Realtime-Garantien nicht unkontrolliert verletzen.

Synchronisations- oder Stop-Zeiten müssen für entsprechende Komponenten begrenzt sein.

Kann dies nicht garantiert werden, muss der Patch verschoben oder abgelehnt werden.

## Provenance

NovaOS soll nachvollziehen können:

```text
PatchID
TargetBuildID
TargetContentID
Patch Version
Patch Points
Activation Time
Previous Patch State
Verification Result
Rollback Result
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Installed Hotpatches
Active Hotpatches
Target Component
Target Build
Patch Dependencies
Patch State
Verification State
Rollback Availability
Failure Reason
```

## Normative Anforderungen

1. NovaOS MUSS Hotpatching für geeignete Komponenten unterstützen können.
2. Hotpatches MÜSSEN eindeutig an ihr Zielartefakt gebunden sein.
3. Versionsnummern allein DÜRFEN NICHT als Target Binding verwendet werden.
4. Kritische Hotpatches MÜSSEN BuildID oder ContentID validieren.
5. Patch Points MÜSSEN eindeutig bestimmbar sein.
6. Beliebige unkontrollierte Runtime-Codeänderungen DÜRFEN NICHT als reguläres Hotpatching gelten.
7. Hotpatch-Aktivierung MUSS atomar erfolgen.
8. Andere CPUs DÜRFEN keinen partiell gepatchten Code ausführen.
9. Aktive Ausführung am Patch Point MUSS berücksichtigt werden.
10. SMP-Systeme MÜSSEN CPU-übergreifende Synchronisation unterstützen.
11. Instruction-Cache-Kohärenz MUSS sichergestellt werden.
12. Hotpatches MÜSSEN Integritäts-, Signatur-, Trust- und Authorization-Prüfungen durchlaufen.
13. Hotpatching DARF Code Integrity NICHT umgehen.
14. Ein Patch DARF Capabilities NICHT implizit erweitern.
15. State-Änderungen MÜSSEN explizit modelliert werden.
16. Komplexe State-Migration SOLL einen vollständigen Live-Update-Pfad verwenden.
17. Patch-Abhängigkeiten MÜSSEN nachvollziehbar sein.
18. Patch Stacking MUSS kontrolliert werden.
19. Reversible Hotpatches SOLLEN Rollback unterstützen.
20. Rollback MUSS vor Ausführung erneut validiert werden.
21. Hotpatches MÜSSEN in das transaktionale Update-Modell integrierbar sein.
22. Aktive Hotpatches MÜSSEN nach Aktivierung verifiziert werden.
23. Unsicher hotpatchbare Änderungen MÜSSEN einen alternativen Update-Pfad verwenden.
24. Harte Realtime-Garantien DÜRFEN NICHT unkontrolliert verletzt werden.
25. Hotpatch-Aktivierungen MÜSSEN nachvollziehbare Provenance besitzen.
26. Hotpatch-State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-SIGNING-0001`
- `NPSPEC-UPDATE-CONTENTADDRESS-0001`
- `NPSPEC-UPDATE-TRANSACTIONAL-0001`
- `NPSPEC-UPDATE-ROLLBACK-0001`
- `NPSPEC-UPDATE-LIVE-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-REALTIME-DEADLINE-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `ADR-ARCH-0177`

## Ergebnis

```text
Running Component
       ↓
Acquire Hotpatch
       ↓
Verify Target + Trust
       ↓
Prepare Patch
       ↓
Reach Safe Patch Point
       ↓
Synchronize Execution
       ↓
Atomic Activation
       ↓
Verify
      ↙   ↘
   Healthy Failed
      ↓       ↓
   Commit   Rollback /
            Live Update /
            Recovery
```

NovaOS erhält damit einen gezielten Runtime-Patching-Mechanismus für kleine und dringende Änderungen. Hotpatches können insbesondere Sicherheits- und Fehlerkorrekturen mit minimaler Unterbrechung aktivieren, ohne dafür die Sicherheits-, Konsistenz- und Verifikationsregeln des normalen NovaOS-Update-Modells zu umgehen.