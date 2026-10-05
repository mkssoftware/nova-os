# NPSPEC-SYSTEM-UPDATES-0001 – Nova System Updates

## Status

Angenommen

## Kategorie

System / Updates

## Zweck

NovaOS definiert den kontrollierten Updateprozess für systemweite Komponenten.

Updates müssen atomar, überprüfbar und rückrollbar sein und dürfen ein funktionierendes System nicht durch teilweise installierte Zustände ersetzen.

## Grundprinzipien

```text
Update ≠ Installation
Downloaded ≠ Trusted
Prepared ≠ Active
Installed ≠ Verified
Update Failure ≠ System Failure
Rollback ≠ Neuinstallation
```

## Updateumfang

Systemupdates können insbesondere betreffen:

```text
Kernel
HAL
Drivers
System Foundation
Framework
Runtime
Modules
Services
Libraries
Security Components
Boot Components
System Configuration
```

Programme besitzen einen davon getrennten Update-Lifecycle.

## Update-Ablauf

```text
Discover
  ↓
Download
  ↓
Verify Package
  ↓
Resolve Dependencies
  ↓
Compatibility Check
  ↓
Prepare
  ↓
Commit
  ↓
Activate
  ↓
Verify
```

Erst nach erfolgreicher Verifikation gilt ein Update als vollständig übernommen.

## Integrität und Trust

Vor der Aktivierung müssen mindestens relevante Eigenschaften geprüft werden können:

```text
Package Identity
Version
Integrity
Signature
Provenance
Trust
Compatibility
Dependencies
```

Eine gültige Signatur allein reicht nicht automatisch für eine positive Trust-Entscheidung.

## Transaktionale Updates

Zusammengehörige Systemänderungen werden als kontrollierte Update-Transaktion behandelt.

```text
Current State
     ↓
Prepared Update
     ↓
Atomic Switch
     ↓
New State
```

Teilweise aktualisierte Systemzustände dürfen nicht als gültiger Endzustand veröffentlicht werden.

## A/B und Snapshots

Für kritische Systembereiche können verwendet werden:

```text
A/B System State
Snapshots
Immutable Versions
Previous Known-Good State
```

Der bisherige funktionsfähige Zustand soll erhalten bleiben, bis der neue Zustand erfolgreich verifiziert wurde.

## Live Updates

Komponenten dürfen ohne Neustart aktualisiert werden, wenn sie Live Evolution unterstützen.

```text
Old Component
      ↓
Prepare New Version
      ↓
Transfer State
      ↓
Switch
      ↓
Verify
      ↓
Retire Old Version
```

Kernel-, Boot- oder andere kritische Änderungen dürfen einen Neustart verlangen.

## Fehler und Rollback

```text
Update Failure
      ↓
Abort / Rollback
      ↓
Known-Good State
      ↓
Verify
```

Ein fehlgeschlagenes Update darf nicht automatisch einen bereits bekannten funktionsfähigen Zustand zerstören.

## Berechtigungen

Systemupdates benötigen explizite Update-Authority.

Ein Update darf keine neuen Runtime-Capabilities für Programme oder andere Komponenten allein durch seine Installation erzeugen.

## Normative Anforderungen

1. Systemupdates MÜSSEN vor Aktivierung auf Integrität, Identität und Kompatibilität geprüft werden.
2. Sicherheitsrelevante Updates MÜSSEN einer Trust-Prüfung unterliegen.
3. Zusammengehörige Änderungen MÜSSEN transaktional aktualisierbar sein.
4. Teilweise installierte Zustände DÜRFEN nicht als gültiger Systemzustand veröffentlicht werden.
5. Kritische Updates SOLLEN einen bekannten funktionsfähigen Zustand erhalten.
6. NovaOS MUSS Rollback oder einen vergleichbaren Recovery-Pfad unterstützen.
7. A/B- und Snapshot-Verfahren MÜSSEN integrierbar sein.
8. Live Updates SOLLEN verwendet werden können, wenn die betroffene Komponente dies unterstützt.
9. Update-Installation MUSS von Runtime-Authority getrennt bleiben.
10. Systemupdates MÜSSEN explizit autorisiert sein.
11. Ein neuer Systemzustand SOLL nach Aktivierung verifiziert werden.
12. Updatezustand, Version, Fehler und Rollbackstatus MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-BOOT-0001`
- `NPSPEC-SYSTEM-KERNEL-0001`
- `NPSPEC-SYSTEM-TRUST-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-SYSTEM-RECOVERY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS aktualisiert Systemkomponenten über einen verifizierten, transaktionalen und rückrollbaren Prozess. Neue Systemzustände werden erst nach erfolgreicher Prüfung aktiviert, während bekannte funktionsfähige Zustände für Recovery und Rollback erhalten bleiben können.