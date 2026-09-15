# ADR-UPDATE-0005 – Snapshot-Based Rollback

## Status

Angenommen

## Kategorie

Update Architecture / Recovery / Storage

## Kontext

Atomic Updates und die A/B-Systembasis schützen NovaOS vor vielen fehlgeschlagenen Systemupdates.

Nicht jede Aktualisierung benötigt jedoch eine vollständige zweite Systembasis. Für kleinere Systemänderungen, Konfigurationsänderungen, Capability Provider oder andere versionierbare Zustände kann ein Snapshot effizienter sein.

NovaOS benötigt deshalb einen standardisierten Mechanismus, mit dem vor einer kritischen Änderung ein bekannter Zustand erhalten und bei Bedarf wiederhergestellt werden kann.

## Entscheidung

NovaOS unterstützt **Snapshot-Based Rollback** für geeignete versionierbare Systemzustände.

```text
Current State
     ↓
Create Snapshot
     ↓
Apply Update
     ↓
Verify
  ┌──┴───┐
  ↓      ↓
Valid  Failed
  ↓      ↓
Keep   Rollback
         ↓
      Snapshot
```

Snapshots werden bevorzugt über Copy-on-Write oder vergleichbare Storage-Mechanismen erzeugt.

## Grundprinzip

```text
Preserve before change.
Change transactionally.
Verify after change.
Rollback if required.
```

Ein Snapshot ist ein Recovery-Punkt und keine Garantie dafür, dass jede Operation reversibel ist.

## Snapshot Scope

Der Snapshot muss den kleinsten Zustand umfassen, der für einen konsistenten Rollback erforderlich ist.

```text
Update Scope
    ↓
Determine Recovery Scope
    ↓
Create Snapshot
```

Mögliche Scopes sind:

```text
Configuration
Component
Service State
System Volume
System Base
Data Set
```

Unnötig große Snapshots sollen vermieden werden.

## Pre-Update Snapshot

Vor einer rollbackrelevanten Änderung kann ein Snapshot des gültigen Ausgangszustands erzeugt werden.

```text
State N
   ↓
Snapshot N
   ↓
Update
   ↓
State N+1
```

Der Snapshot muss vor dem Commit der davon abhängigen irreversiblen Änderung ausreichend persistent sein.

## Snapshot Identity

Jeder Recovery-Snapshot muss eindeutig identifizierbar sein.

Er muss mindestens logisch mit folgenden Informationen verknüpfbar sein:

```text
Snapshot ID
Source State
Transaction ID
Software State Version
Creation Time
Recovery Scope
```

## Copy-on-Write

NovaOS bevorzugt für Snapshots Copy-on-Write oder vergleichbare Mechanismen.

```text
State N
   ↓
Snapshot
   ↓
Shared Blocks
   ↓
Changes create new blocks
```

Dadurch können Snapshots schnell und speichereffizient erzeugt werden.

Die ADR schreibt jedoch keine konkrete Storage-Implementierung vor.

## Update-Ablauf

Ein snapshotgeschütztes Update folgt grundsätzlich:

```text
Current State
     ↓
Validate
     ↓
Create Snapshot
     ↓
Prepare Update
     ↓
Commit
     ↓
Activate
     ↓
Verify
```

Bei erfolgreicher Verification kann der Snapshot entsprechend der Retention Policy später entfernt werden.

## Rollback

Schlägt die Verification fehl, kann der Snapshot als vorheriger Desired beziehungsweise Actual State wiederhergestellt werden.

```text
Updated State
     ↓
Failure
     ↓
Select Snapshot
     ↓
Rollback Transaction
     ↓
Restore
     ↓
Verify
```

Der Rollback gilt erst nach erfolgreicher Verification als abgeschlossen.

## Atomic Restore

Der Wechsel auf einen Snapshot soll logisch atomar erfolgen.

```text
Broken State
     ↓
Atomic Snapshot Switch
     ↓
Previous State
```

Consumer dürfen keinen undefinierten Mischzustand aus altem und neuem Zustand beobachten.

## Reboot

Bestimmte Snapshot-Rollbacks können einen Neustart benötigen.

```text
Failure
   ↓
Select Snapshot
   ↓
Set Recovery Target
   ↓
Reboot
   ↓
Restore Snapshot
   ↓
Verify
```

Der Recovery Target muss power-failure-safe gespeichert werden.

## Power Failure

Ein Stromausfall während eines Updates oder Rollbacks darf den Snapshot nicht unkontrolliert ungültig machen.

```text
Snapshot
   ↓
Update
   ↓
Power Failure
   ↓
Transaction Recovery
   ↓
Current State or Snapshot
```

Der gültige Zustand muss anhand persistenter Transaction- und Storage-Evidenz bestimmbar sein.

## Datenmigrationen

Snapshots können Datenmigrationen absichern, sofern die betroffenen Daten vollständig im Snapshot Scope enthalten sind.

```text
Data V1
   ↓
Snapshot
   ↓
Migration
   ↓
Data V2
```

Werden externe oder nicht snapshotfähige Daten verändert, reicht ein lokaler Snapshot nicht aus.

Solche Operationen benötigen Compensation, Reconciliation oder eine Rollback Barrier.

## Externe Seiteneffekte

Snapshots können nur Zustände wiederherstellen, die tatsächlich vom Snapshot erfasst werden.

```text
Local Snapshot
      ≠
External Rollback
```

Beispiele:

```text
Remote API Call
External Database
Network Service
Physical Device State
```

dürfen nicht allein aufgrund eines lokalen Snapshots als rollbackfähig betrachtet werden.

## Rollback Barrier

Vor irreversiblen Operationen muss geprüft werden, ob der Snapshot tatsächlich einen vollständigen Rollback ermöglicht.

Ist dies nicht der Fall:

```text
Snapshot
   ↓
Rollback Barrier
   ↓
Irreversible Operation
```

Nach der Barrier gelten Compensation- oder Recovery-Regeln.

## A/B-Systembasis

Snapshot-Based Rollback und A/B-Systembasis ergänzen sich.

```text
A/B
→ kritische bootfähige Systembasis

Snapshot
→ versionierbare Teilzustände
```

NovaOS darf beide Mechanismen kombinieren.

Beispiel:

```text
A/B System Base
      +
Configuration Snapshot
      +
Versioned User Data
```

## Benutzerdaten

Systemupdates dürfen nicht automatisch alle Benutzerdaten in einen System-Snapshot einbeziehen.

System- und Benutzerdaten müssen entsprechend ihrer eigenen Lifecycle-, Privacy- und Retention-Regeln behandelt werden.

Ein Systemrollback darf insbesondere nicht unbeabsichtigt neuere Benutzerdaten überschreiben.

## Snapshot Consistency

Ein Snapshot muss einen definierten Konsistenzzustand besitzen.

Je nach Subsystem kann dies sein:

```text
Crash-Consistent
Application-Consistent
Transaction-Consistent
System-Consistent
```

Der benötigte Konsistenzgrad muss aus dem Recovery Contract hervorgehen.

## Security

Snapshots enthalten möglicherweise sicherheitsrelevante oder vertrauliche Daten.

Für sie gelten deshalb dieselben:

```text
Access Control
Encryption
Integrity
Data Classification
Data Sovereignty
```

wie für die ursprünglichen Daten.

Ein Snapshot darf keine bestehende Security-Grenze umgehen.

## Integrity

Vor einem Rollback muss die Integrität des Snapshots überprüfbar sein.

```text
Snapshot
   ↓
Integrity Verification
   ↓
Restore
```

Ein beschädigter Snapshot darf nicht ungeprüft als Recovery-Zustand aktiviert werden.

## Retention

Snapshots dürfen nicht unbegrenzt aufbewahrt werden.

Retention berücksichtigt mindestens:

```text
Recovery Requirement
Storage Budget
Privacy Policy
Data Retention
Known-Good State
```

Ein Snapshot darf jedoch nicht entfernt werden, solange er für eine laufende Transaction oder erforderliche Recovery benötigt wird.

## Garbage Collection

Nicht mehr benötigte Snapshots können kontrolliert entfernt werden.

```text
Snapshot
   ↓
No active dependency
   ↓
Retention satisfied
   ↓
Garbage Collection
```

Garbage Collection darf keine noch referenzierten Recovery-Zustände zerstören.

## NovaDOS

NovaDOS muss kritische System-Snapshots offline erkennen und prüfen können.

Abhängig vom Recovery Contract kann NovaDOS:

```text
List Snapshots
Inspect Snapshot
Verify Snapshot
Select Snapshot
Restore Snapshot
```

durchführen.

## Self-Healing

Nova.Resilience kann Snapshot Rollback als Recovery Action verwenden.

```text
Failure detected
      ↓
Classify
      ↓
Snapshot suitable?
   ┌──┴───┐
   ↓      ↓
  Yes     No
   ↓      ↓
Rollback Other Recovery
   ↓
Verify
```

Der kleinste sichere Recovery Scope ist zu bevorzugen.

## Normative Anforderungen

1. NovaOS MUSS Snapshot-Based Rollback für geeignete versionierbare Zustände unterstützen können.
2. Ein Recovery-Snapshot MUSS vor der davon abhängigen Zustandsänderung ausreichend persistent sein.
3. Snapshot Scope MUSS dem erforderlichen Recovery Scope entsprechen.
4. NovaOS SOLL den kleinsten sicheren Snapshot Scope verwenden.
5. Snapshots MÜSSEN eindeutig identifizierbar sein.
6. Snapshots MÜSSEN mit dem zugehörigen Software- und Transaction-State verknüpfbar sein.
7. Snapshot Restore SOLL logisch atomar erfolgen.
8. Ein Rollback DARF erst nach erfolgreicher Verification als abgeschlossen gelten.
9. Power Failure DARF keinen undefinierten Snapshot- oder Restore-Zustand erzeugen.
10. Snapshots MÜSSEN vor Restore auf Integrität prüfbar sein.
11. Ein lokaler Snapshot DARF nicht als Rollback-Garantie für externe Seiteneffekte interpretiert werden.
12. Irreversible Operationen MÜSSEN weiterhin Rollback Barriers verwenden, wenn der Snapshot sie nicht vollständig zurücksetzen kann.
13. Snapshot-Based Rollback MUSS mit der Transactional Update Pipeline integrierbar sein.
14. Snapshot-Based Rollback MUSS mit der A/B-Systembasis kombinierbar sein.
15. Systemrollback DARF neuere Benutzerdaten nicht unbeabsichtigt überschreiben.
16. Snapshots MÜSSEN Security-, Privacy- und Data-Sovereignty-Regeln des ursprünglichen Zustands erhalten.
17. Snapshot Retention MUSS kontrollierbar sein.
18. Snapshots DÜRFEN nicht entfernt werden, solange aktive Recovery-Abhängigkeiten bestehen.
19. Nova.Resilience MUSS Snapshots als mögliche Recovery-Evidenz und Recovery Action verwenden können.
20. Kritische System-Snapshots MÜSSEN über NovaDOS offline analysierbar sein können.

## Konsequenzen

### Positive Konsequenzen

- schnelle Rollbacks ohne vollständige Neuinstallation,
- kleinere Updates benötigen nicht zwingend eine vollständige A/B-Kopie,
- Copy-on-Write ermöglicht effiziente Recovery-Punkte,
- Update und Self-Healing können denselben Snapshot-Mechanismus verwenden,
- versionierte Systemzustände werden leichter recoverbar.

### Negative Konsequenzen

- Snapshots benötigen zusätzlichen Storage,
- Retention und Garbage Collection müssen verwaltet werden,
- externe Seiteneffekte können nicht durch lokale Snapshots zurückgesetzt werden,
- Benutzerdaten und Systemzustand müssen sauber getrennt werden.

## Verworfene Alternativen

### Vor jedem Update vollständige Systemkopie

Verworfen, da dies unnötig viel Storage und I/O benötigt.

### Snapshot ersetzt A/B vollständig

Verworfen, da eine separate bootfähige Known-Good-Systembasis für kritische Updates zusätzliche Sicherheit bietet.

### Snapshot ersetzt Compensation

Verworfen, da externe und irreversible Seiteneffekte nicht durch lokale Storage-Snapshots zurückgesetzt werden können.

### Snapshots unbegrenzt behalten

Verworfen, da dies Storage-, Privacy- und Retention-Anforderungen verletzt.

### Rollback ohne Verification

Verworfen, da die erfolgreiche technische Wiederherstellung nicht automatisch einen gültigen Systemzustand beweist.

## Abhängigkeiten

- `ADR-UPDATE-0001_Deklarativer_Softwarezustand`
- `ADR-UPDATE-0002_Transactional_Update_Pipeline`
- `ADR-UPDATE-0003_Atomic_Updates`
- `ADR-UPDATE-0004_A_B_Systembasis`
- `ADR-TRANSACTION-0006_Transaction_Log_und_Power_Failure_Safety`
- `ADR-TRANSACTION-0007_Rollback_und_Compensation`
- `ADR-TRANSACTION-0008_Irreversible_Operationen_mit_Rollback_Barrier`
- `ADR-STORAGE-0005_Versioned_Storage`
- `ADR-STORAGE-0014_Snapshots_und_Rollback`
- `ADR-STORAGE-0015_Checksumming_für_Integrität`
- `ADR-RESILIENCE-0004_Kleinster_sicherer_Recovery_Scope`
- `ADR-RESILIENCE-0010_Rollback_über_Systemgrenzen`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-RESILIENCE-0022_NovaDOS_für_Offline_Recovery`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-SNAPSHOT-0001`
- `NPSPEC-UPDATE-SNAPSHOT-SCOPE-0001`
- `NPSPEC-UPDATE-SNAPSHOT-CONSISTENCY-0001`
- `NPSPEC-UPDATE-SNAPSHOT-ROLLBACK-0001`
- `NPSPEC-UPDATE-SNAPSHOT-INTEGRITY-0001`
- `NPSPEC-UPDATE-SNAPSHOT-RETENTION-0001`
- `NPSPEC-UPDATE-SNAPSHOT-POWER-FAILURE-0001`
- `NPSPEC-UPDATE-SNAPSHOT-RECOVERY-0001`
- `NPSPEC-UPDATE-SNAPSHOT-TEST-0001`

## Ergebnis

NovaOS verwendet Snapshots als effiziente Recovery-Punkte für geeignete Updatezustände:

```text
Known-Good State
       ↓
Snapshot
       ↓
Update
       ↓
Activation
       ↓
Verification
    ┌──┴───┐
    ↓      ↓
 Success  Failure
    ↓      ↓
 Continue Rollback
            ↓
         Snapshot
            ↓
         Verify
```

Snapshots ergänzen Atomic Updates und die A/B-Systembasis, ersetzen jedoch keine Compensation oder Rollback Barriers für externe beziehungsweise irreversible Seiteneffekte.

Der zentrale Grundsatz lautet:

```text
Vor einer kritischen Änderung
wird der kleinste sichere
wiederherstellbare Zustand bewahrt.
```