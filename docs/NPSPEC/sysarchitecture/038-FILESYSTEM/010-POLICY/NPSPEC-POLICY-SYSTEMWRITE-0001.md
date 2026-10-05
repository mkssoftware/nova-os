# NPSPEC-POLICY-SYSTEMWRITE-0001 – Nova System Write Policy

## Status

Angenommen

## Kategorie

Policy / System Write

## Zweck

NovaOS definiert die Policy für Änderungen an geschützten systemweiten Ressourcen.

Schreibzugriff auf normale Daten bedeutet nicht automatisch, dass eine Komponente `/System`, `/Boot`, Sicherheitskonfigurationen oder andere kritische Systembereiche verändern darf.

## Grundprinzipien

```text
Write Authority ≠ System Write Authority
User Authority ≠ System Authority
Administrator ≠ Unrestricted Authority
Private SYS ≠ Global /System
System Write ≠ Runtime Permission
Recovery ≠ System Write Bypass
```

## Geschützte Ziele

Die System Write Policy gilt insbesondere für:

```text
/System
/Boot
Kernel Components
Drivers
System Services
System Libraries
System Runtime
System Registry
Security Configuration
Trust Configuration
Recovery State
```

Weitere Ressourcen können durch Policy als systemgeschützt definiert werden.

## Entscheidungsmodell

```text
Write Request
     ↓
Target Classification
     ↓
Security Context
     ↓
System Write Capability
     ↓
Trust / Protection Policy
     ↓
Transaction
     ↓
Verify
```

Normale Datei- oder Objekt-Schreibrechte reichen für geschützte Systemziele nicht aus.

## Private SYS-Overlays

Programme dürfen private Systemabhängigkeiten unter ihrem eigenen Bereich besitzen:

```text
/Apps/<Program>/SYS/
```

Diese können im Programmkontext logisch nach `/System` projiziert werden.

```text
Program View:

/System
   ↑
Global System
   +
Private SYS Overlay
```

Das private Overlay verändert den globalen `/System`-Zustand nicht.

Eine tatsächliche Änderung von `/System` benötigt separate System-Write-Authority.

## Zulässige Änderungswege

Systemänderungen sollen über kontrollierte Mechanismen erfolgen:

```text
System Update
Component Installation
Driver Management
Configuration Management
Recovery
Authorized System Maintenance
```

Direkte unkontrollierte Manipulation geschützter Systemobjekte soll verhindert werden.

## Kritische Änderungen

Abhängig vom Ziel kann die Policy zusätzlich verlangen:

```text
Elevated Capability
Trusted Component
Verified Package
User Confirmation
Transaction
Recovery Point
Reboot
```

## Transaktionen

Kritische Systemänderungen sollen transaktional erfolgen:

```text
Begin
  ↓
Validate
  ↓
Prepare
  ↓
Write
  ↓
Verify
  ↓
Commit
```

Fehler dürfen keinen undefinierten teilweise geänderten Systemzustand erzeugen.

## Policy-Priorität

```text
Safety
  ↓
Security
  ↓
Sovereignty / Trust
  ↓
Hard System Constraints
  ↓
Explicit User Decisions
  ↓
Soft Preferences
  ↓
Adaptive Optimization
```

Eine Benutzerbestätigung allein darf höhere Sicherheitsgrenzen nicht umgehen.

## Normative Anforderungen

1. NovaOS MUSS System Write Authority von normalen Schreibrechten trennen.
2. Änderungen geschützter Systembereiche MÜSSEN explizit autorisiert sein.
3. Besitz eines Objekts DARF keine System Write Authority erzeugen.
4. Benutzer- oder Administratorstatus DARF keine universelle System Authority erzeugen.
5. Private `SYS`-Overlays DÜRFEN den globalen `/System`-Zustand nicht verändern.
6. Änderungen an `/System` und `/Boot` MÜSSEN gesondert geschützt sein.
7. Kritische Systemänderungen SOLLEN transaktional erfolgen.
8. Trust- und Protection-Regeln MÜSSEN bei sicherheitskritischen Änderungen berücksichtigt werden können.
9. Recovery DARF System Write Policy nicht pauschal umgehen.
10. Fehlgeschlagene Änderungen DÜRFEN keinen undefinierten Systemzustand hinterlassen.
11. Policy-Entscheidungen DÜRFEN keine zusätzliche Runtime-Authority erzeugen.
12. Systemänderung, Authority und Entscheidungsgrund MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-POLICY-0001`
- `NPSPEC-SYSTEM-PROTECTION-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-SYSTEM-TRUST-0001`
- `NPSPEC-PROGRAM-SYS-0001`
- `NPSPEC-PROGRAM-SYSOVERLAY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS trennt normale Schreibrechte strikt von der Authority zur Veränderung des Systems. Programme können private Systemabhängigkeiten über ihre `SYS`-Overlays verwenden, während Änderungen am globalen System ausschließlich über explizit autorisierte, policy-kontrollierte und möglichst transaktionale Systempfade erfolgen.