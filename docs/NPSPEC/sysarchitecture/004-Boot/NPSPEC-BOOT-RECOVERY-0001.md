# NPSPEC-BOOT-RECOVERY-0001 – Nova Boot Recovery

## Status

Angenommen

## Kategorie

Boot / Recovery / Resilience / Diagnostics

## Zweck

NovaOS stellt einen unabhängigen Recovery-Bootpfad bereit, wenn der normale Systemstart nicht sicher oder erfolgreich durchgeführt werden kann.

```text
Normal Boot
    ↓
Failure / Recovery Request
    ↓
Nova Recovery
    ↓
Diagnose / Repair / Restore
```

## Grundprinzip

Recovery ist ein eigener kontrollierter Bootpfad und keine Umgehung der normalen Sicherheitsarchitektur.

```text
Detect
  ↓
Diagnose
  ↓
Recover
  ↓
Verify
  ↓
Continue / Restore
```

## Recovery-Auslöser

Recovery kann ausgelöst werden durch:

```text
Boot Failure
Kernel Verification Failure
Damaged System State
Failed Update
Repeated Boot Failure
Explicit User Request
Self-Healing Request
```

## Recovery Environment

NovaOS verwendet eine minimale Recovery-Umgebung mit den für Diagnose und Wiederherstellung notwendigen Funktionen.

Diese kann beispielsweise bereitstellen:

```text
NovaDOS
Storage Access
Filesystem Check
Boot Repair
System Verification
Rollback
Backup / Restore
Diagnostics
Memory Test
Encryption Management
```

Die Recovery-Umgebung soll möglichst unabhängig vom beschädigten Hauptsystem funktionieren.

## Recovery Boot

Der Recovery-Pfad wird über die normale Bootarchitektur gestartet.

```text
Firmware
   ↓
Nova Bootloader
   ↓
Recovery Selection
   ↓
Verified Recovery Environment
```

Das Hauptsystem muss dafür nicht erfolgreich startfähig sein.

## Boot Failure Detection

NovaOS soll wiederholte fehlgeschlagene Starts erkennen können.

```text
Boot Attempt
    ↓
Success Marker?
    ↓
No
    ↓
Failure Counter
    ↓
Recovery
```

Ein erfolgreicher Systemstart setzt den entsprechenden Fehlerzustand zurück.

## Wiederherstellung

Recovery kann abhängig vom Fehler folgende Maßnahmen durchführen:

```text
Retry Boot
Repair Boot Configuration
Restore Boot Component
Rollback System State
Restore Snapshot
Start Safe Mode
Start NovaDOS
```

Automatische Maßnahmen dürfen nur innerhalb definierter Sicherheitsgrenzen erfolgen.

## Rollback

Bei fehlgeschlagenen Updates oder Systemänderungen kann ein vorheriger gültiger Zustand wiederhergestellt werden.

```text
Current State
     ↓
Recovery
     ↓
Known Good State
     ↓
Verify
     ↓
Boot
```

Rollback muss mit dem transaktionalen Systemzustand und der Versionsverwaltung abgestimmt sein.

## Verifikation

Nach einer Reparatur muss der resultierende Zustand geprüft werden.

```text
Repair
  ↓
Verify
  ↓
Valid → Boot
Invalid → Continue Recovery
```

Eine lediglich ausgeführte Reparatur darf nicht automatisch als erfolgreich gelten.

## Sicherheit

Recovery darf Security-, Trust- oder Verschlüsselungsregeln nicht unkontrolliert umgehen.

Insbesondere gilt:

```text
Recovery Access ≠ Unlimited Access
```

Geschützte Daten benötigen weiterhin die erforderlichen Schlüssel oder Berechtigungen.

## Fehlerzustände

Recovery unterscheidet mindestens:

```text
Recovered
Failed
NotRecoverable
Unknown
```

Dabei gilt:

```text
Unknown ≠ Recovered
```

## Normative Anforderungen

1. NovaOS MUSS einen definierten Recovery-Bootpfad besitzen.
2. Recovery SOLL unabhängig vom regulären Hauptsystem startfähig sein.
3. Wiederholte Bootfehler SOLLEN automatisch erkannt werden können.
4. Recovery MUSS manuell startbar sein.
5. Recovery-Komponenten MÜSSEN vor ihrer Verwendung verifizierbar sein.
6. Reparaturen MÜSSEN nach ihrer Durchführung überprüft werden.
7. Recovery SOLL Rollback auf einen bekannten gültigen Zustand unterstützen.
8. Recovery DARF Security-, Trust- oder Verschlüsselungsregeln nicht unkontrolliert umgehen.
9. Automatische Reparaturen MÜSSEN innerhalb definierter Policies erfolgen.
10. Fehlgeschlagene Recovery-Versuche DÜRFEN das System nicht weiter beschädigen.
11. `Unknown` DARF NICHT als erfolgreiche Wiederherstellung behandelt werden.
12. NovaDOS SOLL als unabhängige Offline-Recovery-Umgebung integrierbar sein.

## Abhängigkeiten

- `NPSPEC-BOOT-ARCH-0001`
- `NPSPEC-BOOT-CHAIN-0001`
- `NPSPEC-BOOT-SECURE-0001`
- `NPSPEC-BOOT-VERIFIED-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `ADR-BOOT-0009`
- `ADR-STATE-0008_State_Rollback`

## Ergebnis

```text
Boot Failure
    ↓
Verified Recovery Environment
    ↓
Diagnose
    ↓
Repair / Rollback
    ↓
Verify
    ↓
Known Good Boot State
```

NovaOS erhält damit einen unabhängigen und kontrollierten Wiederherstellungspfad, der auch bei einem beschädigten Hauptsystem Diagnose, Reparatur und Rollback ermöglicht.