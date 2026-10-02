# NPSPEC-RESILIENCE-RECOVERYMODE-0001 – Nova Resilience Recovery Mode

## Status

Angenommen

## Kategorie

Resilience / Recovery / Recovery Mode

## Zweck

NovaOS definiert einen kontrollierten Recovery Mode für Situationen, in denen der normale Systembetrieb nicht sicher, zuverlässig oder vollständig wiederhergestellt werden kann.

```text
Normal Operation
      ↓
Critical / Persistent Failure
      ↓
Normal Recovery Failed
      ↓
Recovery Mode
      ↓
Diagnose / Repair / Rollback
      ↓
Verify
      ↓
Normal Operation
```

Der Recovery Mode stellt eine reduzierte, besonders kontrollierte Umgebung für Diagnose und Wiederherstellung bereit.

## Grundprinzipien

```text
Recovery Mode ≠ Safe Mode
Recovery Mode ≠ Normal Mode
Recovery Mode ≠ Automatic Repair
Recovery Mode ≠ Factory Reset
Booted ≠ Recovered
Repair Completed ≠ Healthy
```

Recovery Mode muss möglichst unabhängig von beschädigten Komponenten des normalen Systems funktionieren.

## Recovery Mode Model

```text
RecoveryMode
├── RecoveryID
├── Trigger
├── FailureDomain
├── RecoveryTarget
├── AvailableCapabilities
├── Restrictions
└── State
```

Optional:

```text
FailureID
ClassificationID
CheckpointID
SnapshotID
BootSlot
RecoveryPolicy
IntegrityState
ProvenanceID
```

## Zustände

```text
Inactive
Requested
Entering
Active
Repairing
Verifying
Exiting
Failed
Unknown
```

## Aktivierung

Recovery Mode kann ausgelöst werden durch:

```text
Repeated Boot Failure
Critical Integrity Failure
Recovery Budget Exhausted
Failed Restart
Failed Rollback
Failed Failover
Persistent Storage Failure
Manual User Request
Self-Healing Escalation
```

Die Aktivierung muss nachvollziehbar sein.

## Recovery Environment

Für systemkritische Recovery soll NovaOS eine minimale Recovery-Umgebung bereitstellen.

```text
Bootloader
    ↓
Recovery Path
    ↓
NovaDOS
    ↓
Recovery Capabilities
```

Diese Umgebung soll möglichst wenige Abhängigkeiten vom normalen NovaOS besitzen.

## Minimale Capabilities

Recovery Mode soll abhängig von Hardware und Fehlerzustand mindestens relevante Fähigkeiten bereitstellen können:

```text
Storage Inspection
Filesystem Verification
Integrity Verification
Boot Repair
A/B Slot Management
Rollback
Checkpoint Restore
Snapshot Restore
Configuration Repair
Log / Crash Dump Access
Hardware Diagnostics
Network Recovery
```

Nicht benötigte Dienste sollen deaktiviert bleiben.

## Isolation

Fehlerhafte Systembereiche sollen während Recovery isoliert bleiben.

```text
Recovery Environment
        │
        ├── Healthy Resources
        │
        └── Controlled Access
                 ↓
           Failed System
```

Recovery-Werkzeuge erhalten nur die für ihre Aufgabe notwendigen Capabilities.

## Storage Safety

Beschädigte Datenträger oder Dateisysteme sollen zunächst möglichst nicht verändert werden.

```text
Detect Damage
    ↓
Read-only Inspection
    ↓
Diagnosis
    ↓
Explicit Repair
```

```text
Detected Corruption ≠ Permission to Modify
```

Destruktive Reparaturen benötigen eine entsprechende Policy oder Benutzerentscheidung.

## Recovery Strategies

Recovery Mode kann orchestrieren:

```text
Restart
Rollback
Checkpoint Restore
Snapshot Restore
A/B Rollback
Provider Replacement
Configuration Repair
Filesystem Repair
Boot Repair
Degraded Boot
```

Die kleinste ausreichende Recovery-Maßnahme soll bevorzugt werden.

## Boot Recovery

Bei fehlgeschlagenem Systemstart:

```text
Boot Attempt
    ↓
Health Failure
    ↓
Retry Budget Exhausted
    ↓
Recovery Mode
```

Recovery Mode kann anschließend einen bekannten funktionierenden Boot Slot oder Systemzustand auswählen.

## Security

Recovery Mode darf keine Hintertür in das Capability- und Security-Modell darstellen.

Es gelten weiterhin:

```text
Authentication
Authorization
Capability Control
Integrity Verification
Trust Policy
Encryption
Audit
```

Recovery-Rechte müssen explizit vergeben werden.

## Verschlüsselte Daten

Recovery Mode darf verschlüsselte Daten nicht allein aufgrund seines privilegierten Zustands entschlüsseln.

```text
Recovery Authority ≠ Decryption Authority
```

Notwendige Credentials oder Schlüssel müssen weiterhin gültig bereitgestellt werden.

## Network Recovery

Netzwerkzugriff kann für:

```text
Recovery Packages
Trusted Updates
Remote Diagnostics
Replica Recovery
```

verwendet werden.

Netzwerkzugriff soll standardmäßig auf notwendige Recovery-Funktionen beschränkt bleiben.

## Recovery Provenance

Änderungen müssen nachvollziehbar sein.

```text
What Changed
Why
Recovery Source
Previous State
New State
Verification Result
```

Dies ermöglicht spätere Diagnose und gegebenenfalls erneuten Rollback.

## Verification

Vor Verlassen des Recovery Mode muss geprüft werden:

```text
Boot Integrity
System Integrity
Storage State
Critical Dependencies
Capabilities
Trust State
Required Services
Health
```

```text
System Starts ≠ System Healthy
```

## Recovery Health

Nach Reparatur kann zunächst ein kontrollierter Teststart erfolgen.

```text
Recovery
   ↓
Test Boot
   ↓
Health Verification
   ├── Valid → Accept
   └── Invalid → Recovery Mode
```

Dies integriert sich mit NovaOS Boot Health und A/B Boot.

## Recovery Failure

Kann auch Recovery Mode keine gültige Wiederherstellung durchführen:

```text
Recovery Failed
      ↓
Preserve Data
      ↓
Diagnostic State
      ↓
Manual Intervention
```

Destruktive Maßnahmen wie Neuformatierung oder Factory Reset dürfen nicht automatisch erfolgen.

## Benutzerinteraktion

Recovery Mode soll technische Details verständlich darstellen können.

Beispiel:

```text
NovaOS konnte nicht sicher gestartet werden.

Problem:
Systemdateien konnten nicht verifiziert werden.

Mögliche Aktionen:
- vorherigen Systemzustand starten
- System prüfen
- Reparatur durchführen
- Diagnose öffnen
```

Erweiterte technische Informationen können separat verfügbar sein.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
RecoveryID
Trigger
Failure Domain
Recovery State
Available Strategies
Selected Strategy
Recovery Source
Integrity State
Actions Performed
Verification Result
Boot Health
```

## Normative Anforderungen

1. NovaOS MUSS einen eigenständigen Recovery Mode unterstützen.
2. Recovery Mode MUSS vom normalen Betriebsmodus getrennt sein.
3. Recovery Mode SOLL möglichst wenige Abhängigkeiten vom normalen System besitzen.
4. NovaDOS SOLL als minimale Recovery-Umgebung verwendbar sein.
5. Kritische Recovery-Funktionen MÜSSEN auch bei teilweise beschädigtem NovaOS verfügbar sein können.
6. Fehlerhafte Domains MÜSSEN während Recovery isolierbar bleiben.
7. Recovery-Werkzeuge MÜSSEN Least Privilege und Capability Security respektieren.
8. Beschädigter Storage SOLL zunächst read-only untersuchbar sein.
9. Destruktive Reparaturen DÜRFEN NICHT allein durch Fehlererkennung autorisiert werden.
10. Restart, Rollback, Checkpoint und A/B Recovery MÜSSEN integrierbar sein.
11. Recovery Mode DARF Security-, Trust- oder Sovereignty-Regeln NICHT umgehen.
12. Recovery Authority DARF NICHT automatisch Decryption Authority bedeuten.
13. Netzwerkzugriff SOLL auf notwendige Recovery-Funktionen beschränkbar sein.
14. Recovery-Änderungen MÜSSEN nachvollziehbar sein.
15. Das System MUSS vor Verlassen des Recovery Mode verifiziert werden.
16. Erfolgreicher Boot DARF NICHT automatisch als erfolgreicher Recovery gelten.
17. Recovery MUSS mit Boot Health integrierbar sein.
18. Fehlgeschlagene Recovery MUSS einen stabilen Diagnosezustand ermöglichen.
19. Factory Reset oder Datenlöschung DÜRFEN NICHT automatisch als normale Recovery ausgeführt werden.
20. Recovery-Zustand und ausgeführte Maßnahmen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESILIENCE-ARCH-0001`
- `NPSPEC-RESILIENCE-DETECTION-0001`
- `NPSPEC-RESILIENCE-CLASSIFICATION-0001`
- `NPSPEC-RESILIENCE-ISOLATION-0001`
- `NPSPEC-RESILIENCE-CONTAINMENT-0001`
- `NPSPEC-RESILIENCE-FAILUREDOMAIN-0001`
- `NPSPEC-RESILIENCE-RESTART-0001`
- `NPSPEC-RESILIENCE-FAILOVER-0001`
- `NPSPEC-RESILIENCE-CHECKPOINT-0001`
- `NPSPEC-RESILIENCE-ROLLBACK-0001`
- `NPSPEC-RESILIENCE-DEGRADATION-0001`
- `NPSPEC-AUTONOMY-SELFHEALING-0001`
- `NPSPEC-BOOT-RECOVERY-0001`
- `NPSPEC-BOOT-AB-0001`
- `NPSPEC-BOOT-ROLLBACK-0001`
- `NPSPEC-BOOT-HEALTH-0001`
- `NPSPEC-SECURITY-LEASTPRIVILEGE-0001`
- `ADR-ARCH-0125`

## Ergebnis

```text
Critical Failure
      ↓
Normal Recovery Failed
      ↓
Enter Recovery Mode
      ↓
Minimal Trusted Environment
      ↓
Diagnose
      ↓
Select Recovery Strategy
      ↓
Repair / Restore / Rollback
      ↓
Verify
   ├── Valid → Controlled Normal Boot
   └── Invalid → Remain in Recovery
```

NovaOS erhält damit einen eigenständigen Recovery Mode, der selbst bei schwerwiegenden Systemfehlern eine minimale, kontrollierte und abgesicherte Umgebung für Diagnose, Reparatur, Rollback und Wiederherstellung bereitstellt, ohne Daten oder Sicherheitsgrenzen unnötig zu gefährden.