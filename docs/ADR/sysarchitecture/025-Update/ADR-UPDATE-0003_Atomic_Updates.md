# ADR-UPDATE-0003 – Atomic Updates

## Status

Angenommen

## Kategorie

Update Architecture / Atomicity / System State

## Kontext

Ein NovaOS-Update kann mehrere zusammengehörige Komponenten verändern.

Beispiele:

- Kernel,
- Treiber,
- Systemdienste,
- Capability Provider,
- Bibliotheken,
- Konfigurationen,
- Boot-Komponenten,
- Datenformate.

Werden diese Komponenten einzeln aktiviert, kann ein Mischzustand entstehen:

```text
Kernel V2
Driver V1
Service V2
Configuration V1
```

Ein solcher Zustand kann inkompatibel oder nicht bootfähig sein.

NovaOS benötigt deshalb atomare Updates, bei denen zusammengehörige Änderungen als eine logische Einheit aktiviert werden.

## Entscheidung

NovaOS unterstützt **Atomic Updates**.

Ein Update Change Set wird vollständig vorbereitet und anschließend als eine logische Zustandsänderung aktiviert.

```text
Active State N
      ↓
Prepare Update
      ↓
Validate
      ↓
Atomic Switch
      ↓
Active State N+1
```

Consumer dürfen keinen undefinierten Mischzustand zwischen den beiden Softwarezuständen beobachten.

## Grundprinzip

```text
Old State
   or
New State

never an undefined mixture
```

Atomarität bezieht sich auf den logisch sichtbaren Softwarezustand.

Sie bedeutet nicht, dass sämtliche physischen Schreiboperationen gleichzeitig stattfinden müssen.

## Update Change Set

Zusammengehörige Änderungen werden als gemeinsames Update Change Set behandelt.

```text
Update Change Set
 ├── Component A
 ├── Component B
 ├── Configuration
 └── Dependencies
```

Der Transaction Scope muss alle Komponenten umfassen, deren Versionen oder Zustände gemeinsam konsistent sein müssen.

## Vorbereitung

Neue Komponenten werden nach Möglichkeit außerhalb des aktiven Zustands vorbereitet.

```text
Active State N
      │
      ├──────────────┐
      │              ↓
      │        Prepared State N+1
      │              ↓
      └──────── Atomic Switch
```

Der bisher aktive Zustand bleibt währenddessen unverändert nutzbar.

## Validierung

Vor dem Atomic Switch muss der vorbereitete Zustand validiert werden.

Dabei werden mindestens relevante:

```text
Dependencies
Compatibility
Integrity
Trust
Capabilities
Resources
Configuration
Data Sovereignty
Execution Contracts
```

geprüft.

Hard Constraints müssen vor Aktivierung erfüllt sein.

## Atomic Switch

Die Aktivierung erfolgt über einen eindeutig definierten Umschaltpunkt.

```text
State N
   ↓
Atomic Switch
   ↓
State N+1
```

Der konkrete Mechanismus kann abhängig vom Subsystem unterschiedlich sein.

Beispiele:

```text
Version Pointer Switch
Root Snapshot Switch
Provider Switch
Boot Target Switch
Namespace Switch
Configuration Version Switch
```

Die technische Implementierung muss die logische Atomarität erhalten.

## Laufende Prozesse

Nicht jeder laufende Prozess muss im selben Moment beendet werden.

NovaOS darf einen kontrollierten Übergang unterstützen:

```text
New Requests → New Version
Old Requests → Old Version until completion
```

Dabei dürfen inkompatible Zustände nicht unkontrolliert miteinander interagieren.

Die konkrete Übergangssemantik muss explizit definiert sein.

## Hot Updates

Hot-updatefähige Komponenten können ohne Neustart atomar ausgetauscht werden.

```text
Provider A active
      ↓
Prepare Provider B
      ↓
Validate
      ↓
Atomic Switch
      ↓
Provider B active
```

Der alte Provider wird erst freigegeben, wenn keine erforderlichen Referenzen mehr bestehen.

## Reboot Updates

Updates von Kernel, Boot-Komponenten oder anderen nicht sicher hot-austauschbaren Komponenten können beim Neustart aktiviert werden.

```text
Current Boot State
       ↓
Prepare New State
       ↓
Set Boot Target
       ↓
Reboot
       ↓
Boot New State
       ↓
Verify
```

Der Boot Target Switch muss power-failure-safe erfolgen.

## A/B-Systemzustände

Für kritische Systemupdates kann ein A/B-Modell verwendet werden.

```text
Slot A = Active
Slot B = Prepared

       ↓

Atomic Boot Target Switch

       ↓

Slot B = Candidate
```

Nach erfolgreicher Health Verification wird der neue Zustand als stabil markiert.

Bei Fehlschlag kann der vorherige Zustand erneut aktiviert werden.

## Versionierte Zustände

Atomic Updates sollen auf versionierten Softwarezuständen aufbauen.

```text
Software State 41
Software State 42
Software State 43
```

Dadurch kann NovaOS eindeutig feststellen:

```text
Current State
Candidate State
Previous State
Recovery State
```

## Datenmigrationen

Eine Datenmigration darf die Atomicity-Garantie nicht stillschweigend brechen.

Wenn alte und neue Software unterschiedliche Datenformate verwenden, muss eine geeignete Strategie definiert sein.

Beispiele:

```text
Backward-Compatible Migration
Copy-on-Write Migration
Versioned Data
Dual-Format Transition
Rollback Barrier
```

Irreversible Migrationen müssen explizit gekennzeichnet werden.

## Power Failure

Ein Stromausfall während eines Atomic Updates darf keinen undefinierten Mischzustand erzeugen.

```text
State N
   ↓
Update
   ↓
Power Failure
   ↓
Recovery
   ↓
State N
   or
State N+1
```

Der Transaction Log und die Storage-Mechanismen müssen feststellen können, welcher Zustand gültig ist.

## Rollback

Solange keine Rollback Barrier überschritten wurde, soll ein vorheriger gültiger Softwarezustand wieder aktivierbar bleiben.

```text
State N
   ↓
State N+1
   ↓
Verification Failure
   ↓
Rollback
   ↓
State N
```

Rollback ist selbst eine kontrollierte Zustandsänderung.

## Rollback Barrier

Irreversible Update-Schritte müssen explizit gekennzeichnet werden.

```text
Atomic Update
     ↓
Rollbackfähiger Zustand
     ↓
Rollback Barrier
     ↓
Irreversible Operation
```

NovaOS darf nach Überschreiten dieser Grenze keinen vollständigen Rollback garantieren.

## Verification

Ein erfolgreicher Atomic Switch bedeutet noch nicht, dass das Update erfolgreich abgeschlossen ist.

```text
Atomic Switch
      ↓
Activation
      ↓
Health Verification
      ↓
Complete
```

Schlägt die Verification fehl, muss Recovery entsprechend dem Update Contract erfolgen.

## Health Milestones

Kritische Updates können mehrere Health Milestones besitzen.

```text
Boot
 ↓
Kernel healthy
 ↓
Storage healthy
 ↓
Core services healthy
 ↓
Security services healthy
 ↓
User environment healthy
```

Erst nach Erreichen der notwendigen Milestones darf der neue Zustand als stabil gelten.

## Multi-Service Updates

Mehrere Services können Teil desselben Atomic Update Change Sets sein.

```text
Atomic Update
 ├── Service A V2
 ├── Service B V4
 └── Service C V3
```

Die logische Aktivierung muss über die Multi-Service-Transaction-Semantik koordiniert werden.

Globale physische Gleichzeitigkeit wird dabei nicht vorausgesetzt.

## Capability Provider

Ein Provider-Wechsel kann atomarer Bestandteil eines Updates sein.

```text
Capability
   ↓
Provider A
   ↓
Atomic Update
   ↓
Provider B
```

Bestehende Capability Contracts müssen während des Übergangs erhalten bleiben.

## Ressourcen

Ein Atomic Update kann vorübergehend alte und neue Versionen gleichzeitig speichern oder ausführen.

```text
Old State
   +
Candidate State
```

Der zusätzliche Ressourcenbedarf muss vor Commit über Resource Transactions berücksichtigt werden.

## Security und Trust

Atomicity darf keine Security- oder Trust-Prüfung ersetzen.

Vor Aktivierung müssen weiterhin die relevanten:

```text
Signature
Provenance
Trust Policy
Authorization
Capability
Integrity
Data Sovereignty
```

geprüft werden.

## Recovery

Nova.Resilience muss erkennen können, welcher Updatezustand aktiv, vorbereitet oder unvollständig ist.

```text
Failure
   ↓
Inspect Transaction State
   ↓
Determine Valid Software State
   ↓
Recover
   ↓
Verify
```

Recovery soll den kleinsten sicheren Scope verwenden.

## NovaDOS

Wenn das normale System nach einem Update nicht mehr gestartet werden kann, muss NovaDOS den gültigen Softwarezustand bestimmen können.

NovaDOS kann abhängig vom Recovery Contract:

```text
Inspect Update State
Verify Candidate
Select Previous State
Rollback Boot Target
Repair Transaction State
```

durchführen.

## Cleanup

Der vorherige Zustand darf erst entfernt werden, wenn er nicht mehr für:

```text
Rollback
Recovery
Retention
Audit
```

benötigt wird.

Der erfolgreiche Atomic Switch allein ist daher kein Grund für sofortiges Cleanup.

## Normative Anforderungen

1. NovaOS MUSS atomare Softwareupdates unterstützen.
2. Zusammengehörige Update-Komponenten MÜSSEN als gemeinsames Change Set behandelbar sein.
3. Der bisher aktive Zustand MUSS während der Vorbereitung konsistent bleiben.
4. Der Candidate State MUSS vor Aktivierung validiert werden.
5. Die Aktivierung MUSS einen eindeutig definierten logischen Atomic Switch besitzen.
6. Consumer DÜRFEN keinen undefinierten Mischzustand zwischen alter und neuer Version beobachten.
7. Logische Atomarität DARF nicht mit physisch gleichzeitigen Schreiboperationen gleichgesetzt werden.
8. Hot Updates MÜSSEN kontrollierte atomare Provider- oder Versionswechsel unterstützen können.
9. Reboot-basierte Updates MÜSSEN einen power-failure-sicheren Boot Target Switch verwenden können.
10. Kritische Systemupdates MÜSSEN A/B- oder vergleichbare versionierte Zustände unterstützen können.
11. Datenmigrationen MÜSSEN ihre Kompatibilitäts- und Rollback-Semantik deklarieren.
12. Irreversible Update-Schritte MÜSSEN Rollback Barriers verwenden.
13. Power Failure DARF keinen undefinierten persistenten Mischzustand hinterlassen.
14. Der vorherige gültige Zustand SOLL bis zur erfolgreichen Verification rollbackfähig bleiben.
15. Ein Atomic Switch DARF nicht allein als erfolgreicher Update-Abschluss gelten.
16. Kritische Updates MÜSSEN Health Verification unterstützen.
17. Multi-Service Updates MÜSSEN mit der Multi-Service-Transaction-Semantik integrierbar sein.
18. Resource Requirements für parallelen alten und neuen Zustand MÜSSEN vor Commit berücksichtigt werden.
19. Atomic Updates DÜRFEN Security-, Trust-, Capability- oder Data-Sovereignty-Prüfungen nicht umgehen.
20. Updatezustände MÜSSEN über den Transaction Log recoveryfähig rekonstruierbar sein.
21. Kritische Atomic Updates MÜSSEN über NovaDOS offline recoverbar sein können.
22. Alte Zustände DÜRFEN nicht entfernt werden, solange sie noch für notwendige Recovery benötigt werden.

## Konsequenzen

### Positive Konsequenzen

- keine undefinierten Mischversionen,
- Updates können vollständig vorbereitet werden, bevor sie aktiv werden,
- Power Failure Safety wird deutlich verbessert,
- kritische Updates können auf bekannte Zustände zurückfallen,
- Hot Update und Reboot Update verwenden dasselbe logische Modell,
- Recovery kann mit klar identifizierbaren Softwarezuständen arbeiten.

### Negative Konsequenzen

- alte und neue Version können temporär zusätzlichen Storage benötigen,
- Datenmigrationen benötigen explizite Kompatibilitätsregeln,
- Hot Updates erfordern saubere Lebenszyklus- und Referenzsemantik,
- kritische Komponenten benötigen zusätzliche Recovery-Metadaten.

## Verworfene Alternativen

### Dateien einzeln im aktiven System ersetzen

Verworfen, da dadurch Mischzustände entstehen können.

### Update-Komponenten unabhängig aktivieren

Verworfen, wenn zwischen ihnen Versions- oder Konsistenzabhängigkeiten bestehen.

### Atomicity nur für den Kernel

Verworfen, da auch Services, Provider, Konfiguration und andere Systemkomponenten konsistente Change Sets benötigen.

### Sofortiges Löschen des alten Zustands

Verworfen, da dadurch Rollback und Recovery unnötig verloren gehen.

### Atomic Switch ohne Verification

Verworfen, da technische Aktivierung nicht beweist, dass der neue Zustand funktionsfähig ist.

## Abhängigkeiten

- `ADR-UPDATE-0001_Deklarativer_Softwarezustand`
- `ADR-UPDATE-0002_Transactional_Update_Pipeline`
- `ADR-TRANSACTION-0001_System_Transactions`
- `ADR-TRANSACTION-0002_Atomic_Desired_State_Changes`
- `ADR-TRANSACTION-0004_Resource_Transactions`
- `ADR-TRANSACTION-0005_Multi-Service_Transactions`
- `ADR-TRANSACTION-0006_Transaction_Log_und_Power_Failure_Safety`
- `ADR-TRANSACTION-0007_Rollback_und_Compensation`
- `ADR-TRANSACTION-0008_Irreversible_Operationen_mit_Rollback_Barrier`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-RESILIENCE-0022_NovaDOS_für_Offline_Recovery`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-ATOMIC-0001`
- `NPSPEC-UPDATE-CHANGESET-0001`
- `NPSPEC-UPDATE-ATOMIC-SWITCH-0001`
- `NPSPEC-UPDATE-STATE-VERSIONING-0001`
- `NPSPEC-UPDATE-AB-STATE-0001`
- `NPSPEC-UPDATE-HOT-SWITCH-0001`
- `NPSPEC-UPDATE-DATA-MIGRATION-0001`
- `NPSPEC-UPDATE-ATOMIC-RECOVERY-0001`
- `NPSPEC-UPDATE-ATOMIC-POWER-FAILURE-0001`
- `NPSPEC-UPDATE-ATOMIC-TEST-0001`

## Ergebnis

NovaOS aktiviert zusammengehörige Softwareänderungen als atomare Zustandswechsel:

```text
Active State N
      ↓
Prepare Candidate N+1
      ↓
Validate
      ↓
Atomic Switch
      ↓
Activate
      ↓
Verify
   ┌──┴───┐
   ↓      ↓
Valid   Invalid
   ↓      ↓
Stable  Rollback / Recovery
```

Damit ist für NovaOS nicht das einzelne Ersetzen von Dateien die atomare Einheit, sondern der **konsistente Softwarezustand als Ganzes**.