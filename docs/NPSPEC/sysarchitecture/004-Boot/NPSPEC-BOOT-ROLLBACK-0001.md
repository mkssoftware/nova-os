# NPSPEC-BOOT-ROLLBACK-0001 – Nova Boot Rollback

## Status

Entwurf

## Kategorie

Boot / Rollback / Recovery / Updates / Resilience

## Zweck

NovaOS stellt einen kontrollierten Boot-Rollback bereit, um nach einem fehlgeschlagenen Update oder nicht bootfähigen Systemzustand auf einen zuvor bestätigten Zustand zurückzukehren.

```text
New System State
      ↓
Boot Failure
      ↓
Rollback Decision
      ↓
Known Good State
```

## Grundprinzip

Rollback erfolgt nur auf einen bekannten und verifizierten Zustand.

```text
Current State
     ↓
Failure
     ↓
Previous Confirmed State
     ↓
Verify
     ↓
Boot
```

Rollback ist eine Zustandsänderung und muss kontrolliert sowie nachvollziehbar erfolgen.

## Rollback-Auslöser

Ein Rollback kann ausgelöst werden durch:

```text
Repeated Boot Failure
Failed Health Check
Failed Update
Invalid Boot Component
Unconfirmed A/B Slot
Explicit Recovery Request
```

## Known Good State

Ein Rollback-Ziel muss als gültiger Zustand bekannt sein.

Es kann beispielsweise sein:

```text
Confirmed A/B Slot
Previous System Version
System Snapshot
Recovery Image
```

Ein lediglich vorhandener älterer Zustand gilt nicht automatisch als sicherer Rollback-Kandidat.

## Automatischer Rollback

NovaOS kann nach definierten fehlgeschlagenen Boot-Versuchen automatisch zurückrollen.

```text
Boot Candidate
     ↓
Failure Counter
     ↓
Threshold Reached
     ↓
Rollback
```

Die Anzahl der Versuche muss begrenzt sein, um Boot-Schleifen zu verhindern.

## A/B-Integration

Bei A/B-Systemen erfolgt Rollback bevorzugt durch Wechsel auf den letzten bestätigten Slot.

```text
Slot B Testing
     ↓
Boot Failed
     ↓
Slot B → Failed
     ↓
Slot A → Active
```

Der fehlerhafte Slot bleibt für Diagnosezwecke identifizierbar.

## Systemzustand

Rollback von Boot-Komponenten und Rollback von persistentem Systemzustand müssen getrennt betrachtet werden.

```text
Boot Rollback
      ≠
Data Rollback
```

Persistente Daten dürfen nicht automatisch auf einen älteren Stand zurückgesetzt werden, sofern dies nicht ausdrücklich Teil der Transaktion ist.

## Datenmigration

Hat eine neue Systemversion persistente Daten verändert, muss definiert sein, ob diese Änderungen:

```text
Backward Compatible
Reversible
Migratable
Irreversible
```

sind.

Irreversible Migrationen dürfen nicht durchgeführt werden, solange ein Rollback auf eine inkompatible ältere Version noch erforderlich sein kann.

## Verifikation

Vor dem Start eines Rollback-Ziels muss dessen Integrität erneut geprüft werden.

```text
Rollback Target
      ↓
Verified Boot
      ↓
Valid
      ↓
Boot
```

Ein früher bestätigter Zustand darf nicht ungeprüft gestartet werden, wenn er inzwischen verändert wurde.

## Fehlerfall

Scheitert auch der Rollback:

```text
Primary Boot Failed
      ↓
Rollback Failed
      ↓
Nova Recovery
```

NovaOS darf nicht unbegrenzt zwischen fehlerhaften Zuständen wechseln.

## Normative Anforderungen

1. NovaOS MUSS einen kontrollierten Boot-Rollback unterstützen können.
2. Rollback-Ziele MÜSSEN eindeutig identifizierbar sein.
3. Automatischer Rollback DARF nur auf definierte zulässige Zustände erfolgen.
4. Rollback-Ziele MÜSSEN vor dem Start erneut verifiziert werden.
5. Wiederholte Bootfehler SOLLEN einen automatischen Rollback auslösen können.
6. Boot-Versuche MÜSSEN begrenzt sein.
7. A/B-Boot SOLL auf den letzten bestätigten Slot zurückfallen können.
8. Boot-Rollback DARF persistente Benutzerdaten nicht automatisch zurücksetzen.
9. Datenmigrationen MÜSSEN hinsichtlich Rollback-Kompatibilität bewertet werden.
10. Irreversible Änderungen MÜSSEN explizit gekennzeichnet sein.
11. Fehlgeschlagene Rollbacks MÜSSEN in den Recovery-Pfad führen können.
12. Rollback-Entscheidungen SOLLEN diagnostizierbar und nachvollziehbar sein.

## Abhängigkeiten

- `NPSPEC-BOOT-ARCH-0001`
- `NPSPEC-BOOT-VERIFIED-0001`
- `NPSPEC-BOOT-RECOVERY-0001`
- `NPSPEC-BOOT-AB-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `ADR-BOOT-0011`
- `ADR-BOOTCONTROL-0001`
- `ADR-STATE-0008_State_Rollback`

## Ergebnis

```text
Failed Boot State
      ↓
Select Known Good State
      ↓
Verify
      ↓
Rollback
      ↓
Boot / Recovery
```

NovaOS kann damit nach fehlgeschlagenen Systemänderungen kontrolliert zu einem bestätigten bootfähigen Zustand zurückkehren, ohne persistente Daten unkontrolliert zurückzusetzen.