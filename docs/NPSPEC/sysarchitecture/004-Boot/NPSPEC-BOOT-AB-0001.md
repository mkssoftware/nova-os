# NPSPEC-BOOT-AB-0001 – Nova A/B Boot

## Status

Entwurf

## Kategorie

Boot / A-B Slots / Updates / Recovery / Resilience

## Zweck

NovaOS verwendet ein A/B-Bootmodell, um Systemupdates sicher einzuspielen und bei einem fehlerhaften neuen Systemzustand automatisch auf einen bekannten funktionsfähigen Zustand zurückzufallen.

```text
Slot A ←→ Slot B
   ↓
Boot Control
   ↓
Selected System
```

## Grundprinzip

Es existieren mindestens zwei bootfähige Systemstände:

```text
Slot A = aktuell aktiv
Slot B = Update-Ziel
```

Updates werden bevorzugt auf den inaktiven Slot geschrieben.

```text
Boot A
  ↓
Update B
  ↓
Verify B
  ↓
Boot B
  ↓
Confirm
```

Der bisherige funktionierende Slot bleibt zunächst erhalten.

## Slot-Zustände

Ein Slot besitzt mindestens einen Zustand:

```text
Active
Inactive
BootCandidate
Testing
Confirmed
Failed
Unbootable
```

Zusätzlich können gespeichert werden:

```text
Version
Generation
BootAttempts
LastBootResult
VerificationState
```

## Update-Ablauf

Ein typischer Update-Vorgang erfolgt:

```text
Active Slot A
      ↓
Write Update to B
      ↓
Verify B
      ↓
Mark B as BootCandidate
      ↓
Reboot
      ↓
Boot B
```

Slot A bleibt währenddessen als Rückfalloption erhalten.

## Boot-Versuche

Ein neuer Slot wird zunächst nur testweise gestartet.

```text
BootCandidate
     ↓
Boot Attempt
     ↓
System Startup
     ↓
Health Verification
```

Die Anzahl fehlgeschlagener Startversuche muss begrenzt sein.

## Boot Confirmation

Ein neuer Slot wird erst nach erfolgreichem Systemstart bestätigt.

```text
Testing
   ↓
Kernel Started
   ↓
Critical Services Ready
   ↓
System Healthy
   ↓
Confirmed
```

Das Erreichen des Kernel Entry Points allein gilt nicht als erfolgreicher Systemstart.

## Automatischer Rollback

Kann ein neuer Slot nicht erfolgreich bestätigt werden:

```text
Slot B Failed
     ↓
Boot Control
     ↓
Slot A
```

Der letzte bestätigte Slot wird erneut bevorzugt.

## Boot Control

Die Auswahl des Slots wird durch versionierte Boot-Control-Metadaten gesteuert.

Diese enthalten mindestens:

```text
Slot
Priority
Bootable
Confirmed
BootAttempts
Generation
```

Boot-Control-Daten müssen gegen unvollständige Schreibvorgänge geschützt sein.

## Verifikation

Ein Slot muss vor seiner Aktivierung die geltenden Boot-Prüfungen erfüllen.

```text
Slot
 ↓
Verified Boot
 ↓
Boot Candidate
```

Ein kryptografisch ungültiger Slot darf nicht durch höhere Boot-Priorität erzwungen werden.

## Daten und Systemzustand

A/B-Slots betreffen primär bootkritische Systemkomponenten.

Benutzerdaten und gemeinsam genutzter persistenter Zustand dürfen nicht unkontrolliert zwischen inkompatiblen Systemversionen verändert werden.

Notwendige Migrationen müssen explizit definiert und rollbackfähig oder kompatibel sein.

## Recovery

Sind beide Slots nicht bootfähig:

```text
Slot A Failed
Slot B Failed
      ↓
Nova Recovery
```

Die Recovery-Umgebung bleibt unabhängig vom normalen A/B-Wechsel erreichbar.

## Normative Anforderungen

1. NovaOS SOLL mindestens zwei unabhängig bootfähige System-Slots unterstützen.
2. Updates SOLLEN auf den inaktiven Slot geschrieben werden.
3. Der aktive bestätigte Slot DARF während eines normalen Updates nicht unnötig verändert werden.
4. Ein neuer Slot MUSS vor Aktivierung verifiziert werden.
5. Neue Slots MÜSSEN zunächst als unbestätigt behandelt werden.
6. Boot-Versuche eines unbestätigten Slots MÜSSEN begrenzt sein.
7. Ein Slot DARF erst nach erfolgreicher Health-Verifikation bestätigt werden.
8. Nach wiederholtem Fehlschlag MUSS automatisch auf einen bestätigten Slot zurückgefallen werden können.
9. Boot-Control-Metadaten MÜSSEN gegen inkonsistente Schreibvorgänge geschützt sein.
10. Ein nicht verifizierter Slot DARF NICHT allein aufgrund seiner Priorität gestartet werden.
11. Gemeinsame persistente Daten MÜSSEN bei Versionswechseln konsistent bleiben.
12. Sind keine gültigen Slots verfügbar, MUSS der Recovery-Pfad erreichbar bleiben.

## Abhängigkeiten

- `NPSPEC-BOOT-ARCH-0001`
- `NPSPEC-BOOT-CHAIN-0001`
- `NPSPEC-BOOT-VERIFIED-0001`
- `NPSPEC-BOOT-RECOVERY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `ADR-BOOT-0010`
- `ADR-BOOTCONTROL-0001`
- `ADR-STATE-0008_State_Rollback`

## Ergebnis

```text
Known Good Slot A
       ↓
Update Slot B
       ↓
Verify → Test → Confirm
       ↓
Success → B becomes Known Good
Failure → Return to A
```

NovaOS kann damit Systemupdates testen, ohne den letzten bestätigten bootfähigen Systemzustand sofort aufzugeben.