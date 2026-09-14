# ADR-TRANSACTION-0006 – Transaction Log und Power Failure Safety

## Status

Angenommen

## Kategorie

Transaction Architecture / Persistence / Crash Consistency

## Kontext

NovaOS muss sicherstellen, dass System Transactions auch bei unerwarteten Unterbrechungen konsistent bleiben.

Mögliche Unterbrechungen sind:

- Stromausfall,
- Hardware-Reset,
- Kernel Panic,
- Systemabsturz,
- Watchdog Reset,
- unerwartete Abschaltung,
- Storage- oder I/O-Fehler.

Ohne persistente Transaction-Information kann nach einem Absturz unklar sein, welche Änderungen:

```text
geplant
vorbereitet
committed
angewendet
verifiziert
```

wurden.

Besonders kritisch ist ein Zustand, bei dem die persistierte Systemstruktur teilweise aktualisiert wurde.

## Entscheidung

NovaOS verwendet einen **persistenten Transaction Log** für alle Transaction-Operationen, deren Recovery über einen Neustart hinweg erforderlich ist.

```text
Transaction
     ↓
Transaction Log
     ↓
Prepare
     ↓
Commit Record
     ↓
Apply
     ↓
Verification
```

Der Log dient als persistente Recovery-Evidenz und nicht als allgemeiner Ersatz für das Storage-Modell.

## Grundprinzip

```text
Never rely on volatile state
to determine transaction outcome
after an unexpected restart.
```

Nach einem Crash muss NovaOS aus persistenten Transaction- und Storage-Zuständen einen sicheren und nachvollziehbaren Zustand bestimmen können.

## Transaction States

Persistente Transactions müssen mindestens logisch zwischen relevanten Zuständen unterscheiden können:

```text
Created
Prepared
Committed
Applying
Completed
Aborted
RecoveryRequired
```

Die konkrete Zustandsmaschine wird in der zugehörigen NPSPEC definiert.

## Write-Ahead-Prinzip

Für persistente Transactions muss die notwendige Recovery-Evidenz vor einem nicht sicher rekonstruierbaren Commit beziehungsweise Zustandwechsel persistent gemacht werden.

```text
Transaction Intent
       ↓
Durable Log Record
       ↓
Prepare / Commit
       ↓
State Change
```

Damit kann Recovery feststellen, welche Entscheidung vor einem Power Failure bereits dauerhaft dokumentiert war.

## Commit-Evidenz

Ein Commit gilt für Recovery erst dann als erfolgt, wenn die dafür erforderliche persistente Commit-Evidenz entsprechend dem jeweiligen Storage Contract dauerhaft hergestellt wurde.

```text
Commit Decision
      ↓
Durable Commit Record
      ↓
Commit becomes recoverable
```

Ein flüchtiger Commit-Zustand darf nach einem Neustart nicht als dauerhafter Commit angenommen werden.

## Atomic Persistence

Persistente Änderungen müssen so organisiert werden, dass Recovery keinen undefinierten Mischzustand akzeptieren muss.

Je nach Storage Provider können dafür beispielsweise verwendet werden:

```text
Journal
Copy-on-Write
Atomic Object Replacement
Versioned Objects
Checksums
Commit Markers
```

Die konkrete Technik bleibt providerabhängig.

## Power Failure

Bei einem Stromausfall kann eine Transaction an jeder Stelle der Persistierung unterbrochen werden.

```text
Prepare
   ↓
Power Failure
```

oder:

```text
Commit
   ↓
Power Failure
```

oder:

```text
Apply
   ↓
Power Failure
```

NovaOS muss diese Fälle unterscheiden und anhand der verfügbaren persistenten Evidenz recovern können.

## Recovery

Nach einem Neustart wird der Transaction Log auf unvollständige Transactions geprüft.

```text
Boot
 ↓
Load Transaction Log
 ↓
Detect Incomplete Transactions
 ↓
Determine Durable State
 ↓
Recover
 ↓
Verify
```

Recovery darf nicht allein anhand des letzten flüchtigen Runtime-Zustands entscheiden.

## Unknown Outcome

Kann nach einem Power Failure nicht sicher festgestellt werden, ob eine Operation ausgeführt wurde, bleibt ihr Ergebnis zunächst:

```text
Unknown
```

Ein unbekannter Zustand darf nicht blind als `Failed` oder `Succeeded` interpretiert werden.

Reconciliation oder eine explizite idempotente Wiederholung muss den tatsächlichen Zustand bestimmen.

## Transaction Log und Idempotenz

Transaction- und Operation-IDs müssen die Wiedererkennung bereits bearbeiteter Operationen ermöglichen.

```text
Transaction ID
Operation ID
Commit State
```

Dadurch kann Recovery vermeiden, dass eine bereits erfolgreich ausgeführte idempotente Operation unbeabsichtigt erneut wirksam wird.

## Log Integrity

Transaction Logs müssen gegen unbeabsichtigte Beschädigung geschützt werden.

Dafür müssen Integritätsmechanismen des Storage-Systems verwendet werden, insbesondere:

```text
Checksums
Versioning
Sequence Information
Integrity Validation
```

Ein beschädigter oder widersprüchlicher Log-Eintrag darf nicht automatisch als vertrauenswürdige Recovery-Evidenz gelten.

## Log Ordering

Für Recovery relevante Log-Einträge müssen eine eindeutige Reihenfolge beziehungsweise Versionierung besitzen.

```text
Record 100
Record 101
Record 102
```

Damit können unvollständige oder widersprüchliche Sequenzen erkannt werden.

## Truncation und Garbage Collection

Abgeschlossene Transaction-Einträge dürfen nach Erfüllung ihrer Retention- und Recovery-Anforderungen entfernt oder kompakt dargestellt werden.

Das Entfernen darf erst erfolgen, wenn die entsprechende Transaction nicht mehr für Recovery oder Audit benötigt wird.

## Resource Transactions

Resource Transactions können persistente Recovery-Evidenz benötigen, wenn ihre Reservation oder Commit-Entscheidung einen Neustart überlebt.

```text
Resource Reservation
        ↓
Durable Transaction State
        ↓
Power Failure
        ↓
Recover / Release / Reconcile
```

Nicht mehr gültige Reservations dürfen nach einem Neustart nicht dauerhaft Ressourcen blockieren.

## Configuration Transactions

Persistente Configuration Transactions müssen nach einem Power Failure eindeutig zwischen gültigen Konfigurationsversionen unterscheiden können.

```text
Configuration N
      ↓
Transaction
      ↓
Configuration N+1
      ↓
Power Failure
```

Recovery muss entweder den gültigen alten oder den gültigen neuen Zustand herstellen beziehungsweise eine explizite Recovery-Semantik anwenden.

## Multi-Service Transactions

Bei Multi-Service Transactions kann der lokale Transaction Log nur den lokalen Zustand beweisen.

```text
Local Log
    ↓
Local Commit Evidence
```

Er beweist nicht automatisch den Commit eines entfernten Participants.

Unbekannte Remote-Zustände müssen daher über Reconciliation behandelt werden.

## Storage Failure

Power Failure Safety ersetzt keine Storage-Integrität.

Wenn persistente Daten beschädigt oder nicht mehr verfügbar sind, gelten die entsprechenden Storage- und Resilience-Mechanismen.

```text
Transaction Recovery
        ↓
Storage Integrity Check
        ↓
Valid Evidence?
```

Ohne ausreichende Evidenz darf NovaOS keinen erfolgreichen Zustand vortäuschen.

## Security

Transaction Logs können sicherheitsrelevante Informationen enthalten.

Daher gelten für sie:

```text
Access Control
Data Minimization
Integrity Protection
Retention Policy
Privacy Classification
```

Secrets dürfen nicht unnötig als Klartext im Transaction Log gespeichert werden.

## Audit

Transaction Recovery und relevante Commit-Entscheidungen müssen nachvollziehbar sein.

Dabei muss zwischen:

```text
Transaction Recovery Evidence
```

und

```text
Security / Administrative Audit
```

unterschieden werden können.

Beide können dieselben Ereignisse referenzieren, haben aber unterschiedliche Retention- und Zugriffseigenschaften.

## Deterministic Mode

Im Deterministic Mode muss die Transaction-Recovery-Semantik deterministisch definiert sein.

Bei gleicher persistenter Ausgangslage muss dieselbe Recovery-Entscheidung entstehen, sofern keine externen Zustandsänderungen vorliegen.

## Recovery Verification

Eine rekonstruierte Transaction gilt nicht allein durch das Vorhandensein eines Log-Eintrags als erfolgreich.

```text
Transaction Evidence
       ↓
Recovery
       ↓
Verification
       ↓
Valid State
```

Dies folgt der systemweiten Regel:

```text
Recover → Verify → Restore/Continue
```

## Normative Anforderungen

1. NovaOS MUSS für reboot-relevante Transactions persistente Transaction-Evidenz unterstützen.
2. Der Transaction Log MUSS zur Recovery unvollständiger Transactions verwendet werden können.
3. Recovery MUSS zwischen mindestens vorbereitetem, committedem und abgeschlossenem Zustand unterscheiden können.
4. Für einen dauerhaften Commit MUSS die erforderliche Commit-Evidenz persistent hergestellt werden.
5. Power Failures DÜRFEN keinen undefinierten persistenten Transaction-Zustand erzeugen.
6. Unbekannte Operationsergebnisse MÜSSEN als `Unknown` behandelbar sein.
7. Recovery DARF einen unbekannten Zustand nicht blind als erfolgreich oder fehlgeschlagen interpretieren.
8. Transaction- und Operation-IDs MÜSSEN zur Recovery und Deduplication verwendet werden können.
9. Transaction Logs MÜSSEN gegen unbeabsichtigte Beschädigung auf Integrität prüfbar sein.
10. Log-Einträge MÜSSEN eine Recovery-relevante Reihenfolge oder Versionierung besitzen.
11. Abgeschlossene Log-Einträge DÜRFEN erst nach erfüllten Recovery- und Retention-Anforderungen entfernt werden.
12. Resource Reservations MÜSSEN nach einem Neustart recoverbar oder sicher freigebbar sein.
13. Configuration Transactions MÜSSEN nach Power Failure auf einen gültigen Konfigurationszustand zurückführbar sein.
14. Lokale Transaction-Evidenz DARF nicht als Beweis für den Commit eines entfernten Participants interpretiert werden.
15. Security-, Privacy- und Data-Sovereignty-Anforderungen MÜSSEN auch für Transaction Logs gelten.
16. Secrets SOLLEN nicht unnötig im Transaction Log persistiert werden.
17. Recovery MUSS vor einer erfolgreichen Zustandsfeststellung eine geeignete Verification durchführen können.
18. Deterministic Mode MUSS eine deterministische Recovery-Semantik für identische persistente Ausgangslagen gewährleisten.
19. Transaction Recovery MUSS mit Nova.Resilience integrierbar sein.
20. Kritische Transaction-Evidenz MUSS bei Bedarf über NovaDOS offline analysierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Power Failures können kontrolliert behandelt werden,
- Transaction-Zustände bleiben über Neustarts rekonstruierbar,
- unbekannte Commit-Zustände werden nicht falsch interpretiert,
- Resource Reservations können nach Abstürzen sicher bereinigt werden,
- Konfigurationen bleiben auf gültige Versionen zurückführbar,
- Recovery erhält eine belastbare persistente Evidenzbasis.

### Negative Konsequenzen

- Persistente Log-Einträge verursachen zusätzlichen Storage- und I/O-Aufwand,
- Log-Management und Retention müssen definiert werden,
- Recovery kann bei beschädigter Evidenz zusätzliche Reconciliation benötigen.

## Verworfene Alternativen

### Recovery ausschließlich aus Runtime-State

Verworfen, da Runtime-State bei Power Failure vollständig verloren gehen kann.

### Commit ohne persistente Commit-Evidenz

Verworfen, da nach einem Neustart nicht zuverlässig festgestellt werden könnte, ob der Commit erfolgt war.

### Jede Transaction vollständig synchron auf alle Storage-Geräte schreiben

Verworfen, da dies unnötige Latenz und Ressourcenbelastung erzeugen würde und nicht für jeden Provider erforderlich ist.

### Unbekannten Zustand automatisch als Failure behandeln

Verworfen, da dadurch bereits ausgeführte Operationen fälschlich wiederholt werden könnten.

### Transaction Log als vollständiges Audit-Log verwenden

Verworfen, da Recovery-Evidenz und langfristiges Audit unterschiedliche Anforderungen an Zugriff, Retention und Datenschutz besitzen.

## Abhängigkeiten

- `ADR-TRANSACTION-0001_System_Transactions`
- `ADR-TRANSACTION-0002_Atomic_Desired_State_Changes`
- `ADR-TRANSACTION-0003_Configuration_Transactions`
- `ADR-TRANSACTION-0004_Resource_Transactions`
- `ADR-TRANSACTION-0005_Multi-Service_Transactions`
- `ADR-STORAGE-0004_Transactional_Storage`
- `ADR-STORAGE-0005_Versioned_Storage`
- `ADR-STORAGE-0013_Transaktionale_Dateioperationen`
- `ADR-STORAGE-0014_Snapshots_und_Rollback`
- `ADR-STORAGE-0015_Checksumming_für_Integrität`
- `ADR-RESILIENCE-0010_Rollback_über_Systemgrenzen`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-RESILIENCE-0022_NovaDOS_für_Offline_Recovery`
- `ADR-CRYPTO-0010_Privacy-Preserving_Logs_und_Crash_Dumps`

## Zugehörige NPSPECs

- `NPSPEC-TRANSACTION-LOG-0001`
- `NPSPEC-TRANSACTION-LOG-RECORD-0001`
- `NPSPEC-TRANSACTION-LOG-COMMIT-0001`
- `NPSPEC-TRANSACTION-LOG-RECOVERY-0001`
- `NPSPEC-TRANSACTION-LOG-INTEGRITY-0001`
- `NPSPEC-TRANSACTION-LOG-RETENTION-0001`
- `NPSPEC-TRANSACTION-POWER-FAILURE-0001`
- `NPSPEC-TRANSACTION-POWER-FAILURE-TEST-0001`

## Ergebnis

NovaOS verwendet persistente Transaction-Evidenz, um System Transactions auch nach Power Failure oder ungeplanten Neustarts rekonstruieren zu können:

```text
Transaction
      ↓
Durable Evidence
      ↓
Prepare / Commit
      ↓
Power Failure
      ↓
Recovery
      ↓
Reconciliation
      ↓
Verification
      ↓
Valid System State
```

Damit ist der Ausgang einer persistenten Transaction nicht vom flüchtigen Zustand vor dem Absturz abhängig, sondern kann aus überprüfbarer persistenter Evidenz und dem tatsächlichen Systemzustand sicher rekonstruiert werden.