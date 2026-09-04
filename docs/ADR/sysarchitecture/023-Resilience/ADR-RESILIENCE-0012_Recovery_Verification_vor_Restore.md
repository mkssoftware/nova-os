# ADR-RESILIENCE-0012 – Recovery Verification vor Restore

## Status

Angenommen

## Kategorie

Resilience Architecture / Recovery Verification / Restore

## Kontext

Eine ausgeführte Recovery-Aktion bedeutet nicht automatisch, dass der betroffene Systemzustand tatsächlich wieder gültig ist.

Ein Prozess kann erfolgreich neu gestartet worden sein, obwohl seine Abhängigkeiten weiterhin fehlen. Ein Provider kann wieder antworten, aber falsche Ergebnisse liefern. Ein Checkpoint kann geladen worden sein, obwohl dessen Invarianten nicht mehr erfüllt sind.

Würde NovaOS unmittelbar nach einer Recovery den normalen Betrieb wieder aufnehmen, könnten fehlerhafte oder inkonsistente Zustände erneut in das System gelangen.

## Entscheidung

Nova.Resilience verlangt eine **explizite Recovery Verification vor dem Restore des normalen Betriebs**.

```text
Recover
   ↓
Verify
   ↓
Valid?
 ├── Yes → Restore
 └── No  → Replan / Escalate
```

`Recover`, `Verify` und `Restore` bleiben getrennte Phasen.

## Grundprinzip

```text
Recovery Action completed
≠
Recovery successful
```

Eine Recovery gilt erst dann als erfolgreich, wenn der resultierende Zustand ausreichend verifiziert wurde.

## Recover

`Recover` führt die geplanten Maßnahmen zur Wiederherstellung eines gültigen Zustands aus.

Beispiele:

```text
Restart
Rollback
Checkpoint Restore
Provider Replacement
State Reconstruction
Resource Reallocation
Compensation
Reconciliation
```

Der erfolgreiche Abschluss dieser Aktion bestätigt zunächst nur, dass die Recovery-Maßnahme technisch durchgeführt wurde.

## Verify

`Verify` prüft den resultierenden Zustand.

Abhängig von der Komponente können dazu gehören:

```text
Health Checks
Invariant Checks
Integrity Checks
State Validation
Dependency Validation
Contract Validation
Capability Validation
Trust Validation
Progress Validation
```

Die Verification muss zum jeweiligen Recovery Scope und zur Fehlerklasse passen.

## Restore

Erst nach erfolgreicher Verification darf die betroffene Komponente wieder für den vorgesehenen Betrieb freigegeben werden.

```text
Recovered
    ↓
Verified
    ↓
Restore
    ↓
Operational
```

Restore kann beispielsweise bedeuten:

```text
Resume Workload
Enable Provider
Restore Routing
Reconnect Dependencies
Resume Data Flow
Accept New Requests
Release Isolation
```

## Isolation bis zur Verification

Während der Verification soll die betroffene Komponente soweit erforderlich isoliert bleiben.

```text
Failure
   ↓
Contain
   ↓
Recover
   ↓
Verify
   ↓
Release Containment
```

Dadurch wird verhindert, dass ein noch nicht bestätigter Zustand bereits wieder Auswirkungen auf andere Failure Domains besitzt.

## Verification Scope

Die Verification muss mindestens die für den Recovery Scope relevanten Invarianten prüfen.

```text
Recovery Scope
      ↓
Required Invariants
      ↓
Verification
```

Eine lokale Task-Recovery benötigt andere Prüfungen als die Wiederherstellung eines kompletten Storage-Subsystems.

## Dependency Verification

Eine Komponente kann selbst gesund erscheinen, obwohl notwendige Abhängigkeiten weiterhin fehlerhaft sind.

```text
Recovered Service
      ↓
Dependencies
   ├── Storage
   ├── Provider
   └── Network
      ↓
Verification
```

Deshalb müssen relevante Dependencies in die Verification einbezogen werden können.

## Semantic Verification

Nova.Resilience bevorzugt semantische Verification gegenüber rein technischen Erfolgssignalen.

```text
Process running
≠
Service correct
```

Beispielsweise kann ein erfolgreich gestarteter Dienst zusätzlich prüfen müssen, ob:

- sein Zustand konsistent ist,
- notwendige Daten verfügbar sind,
- Invarianten erfüllt sind,
- erforderliche Provider erreichbar sind,
- seine Capability Contracts erfüllt werden.

## Provider Replacement

Nach dem Austausch eines Capability Providers muss der neue Provider vor regulärer Nutzung überprüft werden.

```text
Provider A Failed
      ↓
Provider B Selected
      ↓
Contract + Health + Trust Verification
      ↓
Restore Routing
```

Provider Replacement allein gilt nicht als erfolgreiche Recovery.

## Checkpoint Recovery

Nach dem Laden eines Semantic Checkpoints müssen dessen Invarianten erneut geprüft werden.

```text
Checkpoint
    ↓
Restore State
    ↓
Validate Version
    ↓
Validate Authority
    ↓
Validate Invariants
    ↓
Restore Operation
```

Ein technisch lesbarer Checkpoint ist nicht automatisch semantisch gültig.

## Cross-System Recovery

Bei systemübergreifender Recovery kann Verification auch Reconciliation mit entfernten Komponenten erfordern.

```text
Local Recovery
      +
Remote State
      ↓
Reconciliation
      ↓
Verification
```

Unbekannter Remote-Zustand darf nicht ohne ausreichende Evidence als erfolgreich wiederhergestellt gelten.

## Verification Failure

Schlägt die Verification fehl, darf kein normaler Restore erfolgen.

```text
Verify
  ↓
Failed
  ├── Replan
  ├── Retry Recovery
  ├── Expand Recovery Scope
  ├── Alternative Provider
  └── Escalate
```

Die weitere Aktion wird durch Nova.Resilience bestimmt.

## Degraded Restore

Nicht jede erfolgreiche Recovery muss den vollständigen ursprünglichen Zustand wiederherstellen.

Wenn die geltenden Contracts dies erlauben, kann ein verifizierter degradierter Zustand freigegeben werden.

```text
Recovery
   ↓
Verification
   ↓
Degraded but Valid
   ↓
Degraded Restore
```

Der degradierte Zustand muss explizit im Health State sichtbar bleiben.

## Verification Failure Loops

Wiederholte Zyklen aus Recovery und fehlgeschlagener Verification müssen begrenzt werden.

```text
Recover
   ↓
Verify
   ↓ failed
Recover
   ↓
Verify
   ↓ failed
   ↓
Escalate
```

Nova.Resilience darf nicht unbegrenzt denselben erfolglosen Recovery-Pfad wiederholen.

## Security und Trust

Verification darf nicht ausschließlich technische Funktionsfähigkeit prüfen, wenn Security- oder Trust-Zustände Teil der Recovery-Anforderungen sind.

Ein technisch funktionsfähiger, aber nicht mehr vertrauenswürdiger Provider darf nicht allein aufgrund erfolgreicher Health Checks wieder freigegeben werden.

Aktuelle Security-, Trust-, Capability- und Data-Sovereignty-Entscheidungen haben Vorrang.

## Normative Anforderungen

1. Nova.Resilience MUSS Recovery Verification vor Restore unterstützen.
2. `Recover`, `Verify` und `Restore` MÜSSEN semantisch getrennte Phasen bleiben.
3. Der technische Abschluss einer Recovery-Aktion DARF nicht automatisch als erfolgreiche Recovery gelten.
4. Eine Komponente DARF erst nach ausreichender Verification wieder für den normalen Betrieb freigegeben werden.
5. Die Verification MUSS zum Recovery Scope und zur Fehlerklasse passen.
6. Relevante Invarianten MÜSSEN während der Verification geprüft werden können.
7. Relevante Dependencies MÜSSEN in die Verification einbezogen werden können.
8. Semantic Verification SOLL gegenüber rein technischen Liveness-Signalen bevorzugt werden.
9. Provider Replacement MUSS vor regulärer Nutzung des neuen Providers verifiziert werden.
10. Semantic Checkpoints MÜSSEN nach ihrer Wiederherstellung erneut validiert werden.
11. Systemübergreifende Recovery MUSS unbekannte relevante Remote-Zustände vor Restore reconciliieren können.
12. Fehlgeschlagene Verification MUSS Replanning, alternative Recovery oder Eskalation auslösen können.
13. Fehlgeschlagene Verification DARF keinen normalen Restore auslösen.
14. Ein verifizierter degradierter Zustand DARF wiederhergestellt werden, wenn die geltenden Contracts dies zulassen.
15. Ein degradierter Restore MUSS im Health State sichtbar bleiben.
16. Wiederholte Recovery-Verification-Schleifen MÜSSEN begrenzt werden.
17. Verification MUSS aktuelle Security-, Trust-, Capability-, Integrity- und Data-Sovereignty-Constraints berücksichtigen.
18. Verification-Ergebnisse und Restore-Entscheidungen MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- fehlerhafte Recovery wird nicht mit erfolgreicher Wiederherstellung verwechselt,
- Komponenten gelangen erst nach Prüfung zurück in den normalen Betrieb,
- Checkpoints und Provider Replacement werden semantisch validiert,
- degradierte, aber gültige Recovery bleibt möglich,
- Cascading Failures durch verfrühten Restore werden reduziert.

### Negative Konsequenzen

- Recovery benötigt zusätzliche Verification-Schritte,
- die Wiederaufnahme des Betriebs kann sich dadurch geringfügig verzögern,
- Komponenten müssen geeignete Invarianten und Verification-Schnittstellen bereitstellen.

## Verworfene Alternativen

### Erfolgreiche Recovery-Aktion gilt automatisch als erfolgreich

Verworfen, da die technische Durchführung einer Recovery nichts über die Gültigkeit des resultierenden Zustands garantiert.

### Health Check allein reicht immer aus

Verworfen, da Health, semantische Correctness, Integrity und Trust unterschiedliche Eigenschaften darstellen.

### Restore parallel zur Verification

Verworfen, da dadurch ein noch nicht bestätigter Zustand bereits wieder Auswirkungen auf andere Systembereiche haben könnte.

### Unbegrenzte Recovery-Verification-Schleifen

Verworfen, da dadurch Ressourcen verbraucht und dauerhafte Fehler verschleiert würden.

## Abhängigkeiten

- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0002_Detect_Classify_Contain_Preserve_Plan_Recover_Verify_Restore_Learn`
- `ADR-RESILIENCE-0003_Hierarchische_Failure_Domains`
- `ADR-RESILIENCE-0004_Kleinster_sicherer_Recovery_Scope`
- `ADR-RESILIENCE-0008_Health_State_Model`
- `ADR-RESILIENCE-0009_Semantic_Checkpointing`
- `ADR-RESILIENCE-0010_Rollback_über_Systemgrenzen`
- `ADR-RESILIENCE-0011_Progress_Based_Watchdogs`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-RECOVERY-VERIFICATION-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-INVARIANT-0001`
- `NPSPEC-RESILIENCE-DEPENDENCY-VERIFICATION-0001`
- `NPSPEC-RESILIENCE-RESTORE-GATE-0001`
- `NPSPEC-RESILIENCE-DEGRADED-RESTORE-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-LOOP-0001`
- `NPSPEC-RESILIENCE-RECOVERY-VERIFICATION-TEST-0001`

## Ergebnis

Nova.Resilience trennt die technische Wiederherstellung konsequent von der Freigabe für den normalen Betrieb:

```text
Recover
   ↓
Verify
   ├── Invalid
   │      ↓
   │   Replan / Escalate
   │
   └── Valid
          ↓
       Restore
          ↓
       Operational
```

Damit gilt eine Recovery in NovaOS erst dann als erfolgreich, wenn der resultierende Zustand nachweislich die für seinen Scope erforderlichen Invarianten, Contracts und Hard Constraints erfüllt.