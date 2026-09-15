# ADR-UPDATE-0022 – Boot Health vor Update Commit

## Status

Angenommen

## Kategorie

Update / Boot / Recovery

## Kontext

Ein erfolgreich installiertes Update ist nicht automatisch ein funktionsfähiger Systemzustand.

Insbesondere bei Änderungen an Kernel, Treibern oder Systembasis kann das System zwar starten, aber kritische Funktionen nicht erreichen.

NovaOS darf einen neuen Updatezustand deshalb erst dauerhaft akzeptieren, nachdem dessen Boot Health bestätigt wurde.

## Entscheidung

NovaOS verlangt für bootrelevante Updates einen erfolgreichen **Boot Health Check vor dem finalen Update Commit**.

```text
Install Candidate
      ↓
Boot Candidate
      ↓
Health Milestones
      ↓
Healthy?
  ┌───┴───┐
  ↓       ↓
 Yes      No
  ↓       ↓
Commit  Fallback
```

Bis zum erfolgreichen Commit bleibt der vorherige Known-Good-Zustand erhalten.

## Health Milestones

Ein erfolgreicher Boot wird anhand definierter Milestones bewertet.

Beispiele:

```text
Bootloader completed
Kernel initialized
Storage available
Critical Services running
System UI reachable
```

Die konkreten Milestones werden durch den jeweiligen Update- und Systemzustand bestimmt.

## Commit

Erst nach Erreichen der erforderlichen Health Milestones wird der Candidate zum neuen Known-Good-State.

```text
Candidate
   ↓
Boot Healthy
   ↓
Commit
   ↓
Known-Good
```

## Failure

Werden die erforderlichen Milestones nicht erreicht, gilt der Candidate als fehlgeschlagen.

```text
Candidate
   ↓
Boot Failure
   ↓
Fallback
   ↓
Previous Known-Good
```

Wiederholte Bootversuche müssen begrenzt sein, damit keine Boot-Schleife entsteht.

## A/B-Systembasis

Bei einer A/B-Systembasis bleibt der vorherige Slot Known-Good, bis der neue Slot erfolgreich bestätigt wurde.

```text
A = Known-Good
B = Candidate
      ↓
Boot B
      ↓
Healthy
      ↓
B = Known-Good
```

## Normative Anforderungen

1. Bootrelevante Updates MÜSSEN vor dem finalen Commit einen Boot Health Check bestehen.
2. Erforderliche Health Milestones MÜSSEN explizit definiert sein.
3. Der vorherige Known-Good-State MUSS bis zum erfolgreichen Commit erhalten bleiben.
4. Ein Candidate DARF nicht allein aufgrund eines gestarteten Kernels als erfolgreich gelten.
5. Fehlgeschlagene Bootversuche MÜSSEN erkannt und begrenzt werden.
6. Bei fehlgeschlagenem Health Check MUSS ein Fallback auf einen zulässigen Known-Good-State möglich sein.
7. Der Commit MUSS power-failure-safe gespeichert werden.
8. Boot Health MUSS mit A/B-, Recovery- und Anti-Rollback-Mechanismen integrierbar sein.

## Konsequenzen

### Positive Konsequenzen

- fehlerhafte Updates werden nicht vorschnell als Known-Good markiert,
- Boot-Loops können automatisch erkannt werden,
- A/B-Fallback wird zuverlässig steuerbar,
- Recovery basiert auf tatsächlich gestarteten Systemzuständen.

### Negative Konsequenzen

- Updates benötigen eine zusätzliche Bestätigungsphase,
- Health Milestones müssen gepflegt werden.

## Abhängigkeiten

- `ADR-UPDATE-0002_Transactional_Update_Pipeline`
- `ADR-UPDATE-0004_A_B_Systembasis`
- `ADR-UPDATE-0016_Bootloader_Update_Schutz`
- `ADR-UPDATE-0020_Anti_Rollback_Minimum_Version`
- `ADR-RESILIENCE-0008_Health_State_Model`
- `ADR-TRANSACTION-0006_Transaction_Log_und_Power_Failure_Safety`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-BOOT-HEALTH-0001`
- `NPSPEC-UPDATE-BOOT-MILESTONES-0001`
- `NPSPEC-UPDATE-BOOT-COMMIT-0001`

## Ergebnis

Ein Update wird erst nach einem nachweislich gesunden Start dauerhaft übernommen:

```text
Candidate
   ↓
Boot
   ↓
Health Milestones
   ↓
Verify
   ↓
Commit as Known-Good
```

Der zentrale Grundsatz lautet:

```text
Installiert bedeutet nicht erfolgreich.

Erst ein gesunder Boot
bestätigt das Update.
```