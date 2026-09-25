# NPSPEC-BOOT-HEALTH-0001 – Nova Boot Health

## Status

Angenommen

## Kategorie

Boot / Health / Boot Control / Recovery / Resilience

## Zweck

NovaOS bewertet nach einem Systemstart, ob ein gestarteter Systemzustand tatsächlich funktionsfähig ist.

```text
Boot
  ↓
Kernel
  ↓
Critical Services
  ↓
Health Check
  ↓
Confirm / Fail
```

Das Erreichen des Kernel Entry Points allein gilt nicht als erfolgreicher Boot.

## Grundprinzip

Ein Boot gilt erst als erfolgreich, wenn definierte kritische Systemfunktionen betriebsbereit sind.

```text
Started ≠ Healthy
```

Der Health-Zustand beeinflusst insbesondere:

```text
A/B Confirmation
Rollback
Boot Failure Counter
Recovery
```

## Health-Zustände

NovaOS unterscheidet mindestens:

```text
Starting
Checking
Healthy
Degraded
Failed
Unknown
```

Dabei gilt:

```text
Unknown ≠ Healthy
```

## Health-Kriterien

Die Boot-Policy definiert die für einen erfolgreichen Start erforderlichen Kriterien.

Beispiele:

```text
Kernel operational
Memory management operational
Storage available
System volume available
Critical services running
Security services operational
System state consistent
```

Nicht jede optionale Komponente muss für `Healthy` verfügbar sein.

## Health Check

Nach dem Kernelstart beginnt eine definierte Prüfphase.

```text
Kernel Start
    ↓
System Initialization
    ↓
Health Checks
    ↓
Boot Health Decision
```

Die Prüfung muss innerhalb eines definierten Zeitfensters abgeschlossen werden.

## Boot Confirmation

Ein neuer A/B-Slot oder Systemstand darf erst nach erfolgreicher Health-Prüfung bestätigt werden.

```text
BootCandidate
     ↓
Boot
     ↓
Health = Healthy
     ↓
Confirmed
```

Erst danach darf der vorherige bestätigte Zustand als Rollback-Sicherung ersetzt werden.

## Degraded State

Ein System kann trotz nichtkritischer Fehler betriebsfähig sein.

```text
Critical Requirements erfüllt
Optional Component failed
        ↓
Degraded
```

Ob `Degraded` für eine Boot-Bestätigung ausreichend ist, muss durch Policy definiert sein.

## Fehler

Schlägt ein kritischer Health Check fehl:

```text
Health = Failed
      ↓
Boot not confirmed
      ↓
Failure Counter
      ↓
Retry / Rollback / Recovery
```

Ein fehlgeschlagener Health Check darf nicht als erfolgreicher Boot gespeichert werden.

## Persistenz

Der Boot-Health-Zustand muss so gespeichert werden, dass der Bootloader beim nächsten Start erkennen kann:

```text
Last Boot Attempt
Last Confirmed Boot
Failure Count
Slot Health
Boot Generation
```

Unvollständige Schreibvorgänge dürfen keinen falschen erfolgreichen Zustand erzeugen.

## Watchdog

Ein Watchdog kann verwendet werden, um Startvorgänge zu erkennen, die hängen bleiben, bevor eine Health-Bestätigung möglich ist.

```text
Boot Start
   ↓
Watchdog Armed
   ↓
Health Confirmed
   ↓
Watchdog Cleared
```

## Normative Anforderungen

1. NovaOS MUSS zwischen gestartetem und gesundem Systemzustand unterscheiden.
2. Ein Boot DARF NICHT allein durch Erreichen des Kernel Entry Points als erfolgreich gelten.
3. Kritische Health-Kriterien MÜSSEN durch Policy definierbar sein.
4. `Unknown` DARF NICHT als `Healthy` behandelt werden.
5. Health Checks MÜSSEN innerhalb eines definierten Zeitfensters erfolgen.
6. Ein A/B-Bootkandidat DARF erst nach erfolgreicher Health-Prüfung bestätigt werden.
7. Fehlgeschlagene Health Checks MÜSSEN den Boot-Fehlerzustand aktualisieren.
8. Wiederholte Fehler MÜSSEN Rollback oder Recovery auslösen können.
9. `Degraded` MUSS von `Healthy` und `Failed` unterscheidbar sein.
10. Boot-Health-Daten MÜSSEN gegen inkonsistente Schreibvorgänge geschützt sein.
11. Watchdog-basierte Fehlererkennung SOLL unterstützt werden.
12. Health-Entscheidungen SOLLEN diagnostizierbar und nachvollziehbar sein.

## Abhängigkeiten

- `NPSPEC-BOOT-ARCH-0001`
- `NPSPEC-BOOT-AB-0001`
- `NPSPEC-BOOT-ROLLBACK-0001`
- `NPSPEC-BOOT-RECOVERY-0001`
- `ADR-BOOT-0012`
- `ADR-BOOTCONTROL-0001`

## Ergebnis

```text
Boot Attempt
    ↓
System Initialization
    ↓
Health Evaluation
    ↓
Healthy → Confirm
Failed  → Retry / Rollback / Recovery
```

NovaOS kann damit zuverlässig unterscheiden, ob ein System lediglich gestartet wurde oder tatsächlich einen bestätigten betriebsfähigen Zustand erreicht hat.