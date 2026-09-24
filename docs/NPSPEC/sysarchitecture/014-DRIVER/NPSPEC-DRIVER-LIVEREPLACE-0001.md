# NPSPEC-DRIVER-LIVEREPLACE-0001 – Nova Driver Live Replacement

## Status

Entwurf

## Kategorie

Driver / Live Replacement / Live Evolution / Resilience

## Zweck

NovaOS definiert das kontrollierte Ersetzen einer aktiven Treiberimplementierung durch eine andere Implementierung während des laufenden Systems.

```text
Active Driver A
      ↓
Prepare Replacement
      ↓
Driver B
      ↓
Atomic Switch
      ↓
Verify
```

Live Replacement erweitert Hot Reload um den Austausch zwischen unterschiedlichen kompatiblen Treiberimplementierungen.

## Grundprinzip

```text
Driver Identity ≠ Driver Implementation
Replacement ≠ Update
Compatible ≠ Identical
Switch ≠ Verified
```

Ein Gerät darf von einer anderen Treiberimplementierung übernommen werden, sofern Interface, Zustand und Ressourcenübergabe kompatibel sind.

## Replacement Candidate

Ein Ersatztreiber muss vor dem Wechsel validiert werden.

```text
Candidate
├── DriverID
├── Version
├── Supported Device
├── Interfaces
├── State Compatibility
├── Capabilities
└── Trust State
```

Ein Ersatztreiber darf eine neue Version desselben Treibers oder eine alternative Implementierung sein.

## Kompatibilität

Vor dem Austausch muss geprüft werden:

```text
Device Compatibility
Interface Compatibility
State Compatibility
Capability Requirements
Resource Requirements
Security Policy
```

Nicht erfüllte zwingende Anforderungen müssen den Live Replacement verhindern.

## Replacement Lifecycle

```text
Select Candidate
      ↓
Validate
      ↓
Prepare
      ↓
Quiesce Active Driver
      ↓
Transfer / Reconstruct State
      ↓
Transfer Resources
      ↓
Atomic Switch
      ↓
Verify
      ↓
Retire Old Driver
```

Der alte Treiber bleibt bis zum definierten Switch-Punkt verantwortlich.

## State Transfer

Zustand darf übertragen oder durch den neuen Treiber rekonstruiert werden.

```text
Old Driver
   ↓
Portable State
   ↓
New Driver
```

Alternativ:

```text
Device State
    ↓
Re-detect / Reconstruct
    ↓
New Driver
```

Implementationsinterne Speicherstrukturen dürfen nicht als gemeinsame Zustandsrepräsentation vorausgesetzt werden.

## I/O-Übergabe

Laufende Requests müssen eindeutig behandelt werden.

```text
Pending I/O
├── Complete by Old Driver
├── Cancel
└── Transfer
```

Nach dem Switch dürfen neue Requests ausschließlich an den neuen aktiven Treiber gehen.

Ein Request darf nicht doppelt ausgeführt werden.

## Ressourcenübergabe

Folgende Ressourcen müssen kontrolliert übertragen oder neu eingerichtet werden:

```text
Device Ownership
MMIO
Interrupts
DMA Context
Capabilities
Shared Buffers
Provider Registration
```

Es darf keinen undefinierten Zeitraum mit konkurrierendem exklusivem Gerätebesitz geben.

## Atomic Switch

Für höhere Systemschichten soll der Wechsel möglichst als atomarer Provider-Wechsel erscheinen.

```text
Driver A → Switch Point → Driver B
```

Requests müssen eindeutig der alten oder neuen Treiberinstanz zugeordnet werden können.

## Verification

Nach dem Switch muss der neue Treiber geprüft werden.

```text
Driver B
   ↓
Health Check
   ↓
Device Check
   ↓
I/O Verification
   ↓
Healthy
```

`Active` darf nicht automatisch als `Healthy` gelten.

## Rollback

Bis zur erfolgreichen Verifikation soll der vorherige Treiber als Rollback-Kandidat erhalten bleiben, sofern technisch möglich.

```text
Replacement Failed
      ↓
Rollback
      ↓
Previous Driver
```

Falls Rollback nicht möglich ist, darf NovaOS:

```text
Reset Device
Load Fallback Driver
Disable Device
Enter Degraded Mode
```

verwenden.

## Security

Der Ersatztreiber muss sämtliche normalen Prüfungen durchlaufen:

```text
Identity
Integrity
Signature
Trust
Capabilities
Sandbox Policy
```

Live Replacement darf keine Sicherheits- oder Capability-Prüfung umgehen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Device
Current Driver
Replacement Driver
Replacement Phase
Compatibility State
Pending I/O
Verification State
Rollback State
Error
```

## Normative Anforderungen

1. NovaOS SOLL kompatible Treiberimplementierungen zur Laufzeit ersetzen können.
2. Ersatztreiber MÜSSEN vor dem Switch auf Geräte-, Interface- und Sicherheitskompatibilität geprüft werden.
3. Der aktive Treiber MUSS vor der Übergabe kontrolliert quiesziert werden.
4. Laufende I/O-Requests MÜSSEN eindeutig abgeschlossen, abgebrochen oder übertragen werden.
5. Ein Request DARF NICHT durch Live Replacement doppelt ausgeführt werden.
6. Zustandsübertragung MUSS über definierte, kompatible Repräsentationen erfolgen.
7. Ressourcen und Capabilities MÜSSEN kontrolliert übertragen oder neu vergeben werden.
8. Exklusiver Gerätebesitz MUSS zu jedem Zeitpunkt eindeutig sein.
9. Der Provider-Wechsel SOLL für höhere Schichten möglichst atomar erscheinen.
10. Der neue Treiber MUSS nach dem Switch verifiziert werden.
11. Fehlgeschlagene Ersetzung MUSS Rollback, Fallback, Reset oder kontrollierte Deaktivierung ermöglichen.
12. Replacement-, Verification- und Rollback-Zustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DRIVER-MODEL-0001`
- `NPSPEC-DRIVER-FRAMEWORK-0001`
- `NPSPEC-DRIVER-CAPABILITY-0001`
- `NPSPEC-DRIVER-HOTRELOAD-0001`
- `NPSPEC-IO-REQUEST-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `ADR-DRIVER-0009`

## Ergebnis

```text
Driver A
   ↓
Quiesce
   ↓
State + Resource Handover
   ↓
Atomic Switch
   ↓
Driver B
   ↓
Verify
  ├→ Healthy → Retire A
  └→ Failed  → Rollback / Fallback
```

NovaOS erhält damit einen kontrollierten Mechanismus, mit dem ein Gerät während des laufenden Systems zwischen kompatiblen Treiberimplementierungen wechseln kann, ohne I/O-Zuordnung, Ressourcenbesitz oder Sicherheitsgrenzen undefiniert zu lassen.