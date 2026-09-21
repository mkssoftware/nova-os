# ADR-AUTONOMY-0003 – Self-Configuration

## Status

Angenommen

## Kategorie

Autonomy / Configuration / System Management

## Kontext

NovaOS soll sich weitgehend selbst konfigurieren können, ohne dass Nutzer für normale Hardware-, Ressourcen- oder Systemänderungen manuell Konfigurationsdateien bearbeiten müssen.

Änderungen können beispielsweise entstehen durch:

```text
New Hardware
Removed Hardware
New Capability
Provider Changes
Network Changes
Storage Changes
Resource Changes
Software Updates
Changed Workloads
```

Manuelle Konfiguration soll weiterhin möglich sein, aber nicht Voraussetzung für einen funktionsfähigen Systemzustand sein.

## Entscheidung

NovaOS unterstützt **Self-Configuration** als policy- und constraint-gesteuerte autonome Systemfunktion.

```text
System Discovery
      ↓
Actual State
      ↓
Desired State
      ↓
Policies + Constraints
      ↓
Configuration Plan
      ↓
Apply
      ↓
Verify
```

NovaOS darf selbstständig Konfigurationen erzeugen oder anpassen, solange diese innerhalb der gültigen Autonomy Boundary liegen.

## Deklarative Konfiguration

Self-Configuration basiert auf Desired State und Actual State.

```text
Desired State
      ↕
Actual State
      ↓
Difference
      ↓
Configuration Plan
```

Das System beschreibt bevorzugt den gewünschten Zustand und nicht eine feste Folge manueller Konfigurationsschritte.

## Discovery

NovaOS darf verfügbare Ressourcen und Fähigkeiten automatisch erkennen.

Beispiele:

```text
CPU
Memory
Storage
Network Interfaces
GPU / NPU
Devices
Drivers
Capabilities
Compute Providers
Services
```

Discovery allein erzeugt jedoch noch keine Berechtigung zur Nutzung.

## Configuration Planning

Aus erkannten Ressourcen und gültigen Anforderungen erstellt NovaOS einen Configuration Plan.

```text
Available Resources
       +
Desired State
       +
Policies
       +
Constraints
       ↓
Valid Configuration Space
       ↓
Configuration Plan
```

Nur gültige Konfigurationen dürfen angewendet werden.

## Automatische Defaults

Fehlt eine explizite Konfiguration, darf NovaOS sichere Defaults verwenden.

Beispiele:

```text
Safe Driver Selection
Default Resource Limits
Default Network Policy
Default Storage Placement
Default Service Configuration
```

Defaults müssen Fail-Safe-Prinzipien folgen.

## Nutzerkonfiguration

Explizite Nutzerkonfiguration besitzt innerhalb gültiger Hard Constraints Vorrang vor automatisch erzeugter Konfiguration.

```text
Hard Constraints
      ↓
Explicit Configuration
      ↓
Automatic Configuration
```

Self-Configuration darf explizite Einstellungen nicht stillschweigend überschreiben.

## Adaptive Configuration

Adaptive Modelle dürfen Self-Configuration optimieren.

Beispiele:

```text
Resource Allocation
Provider Preference
Cache Size
Power Policy
Storage Placement
Network Strategy
```

Adaptive Optimierung bleibt innerhalb der bestehenden Hard Constraints.

## Configuration Changes

Autonome Konfigurationsänderungen sollen bevorzugt transaktional erfolgen.

```text
Current Configuration
        ↓
Prepare Change
        ↓
Validate
        ↓
Apply
        ↓
Verify
        ↓
Commit
```

Schlägt die Änderung fehl, soll der vorherige gültige Zustand erhalten oder wiederhergestellt werden.

## Hot Configuration

Soweit technisch möglich, dürfen Konfigurationen ohne Neustart geändert werden.

```text
Detect Change
     ↓
Plan
     ↓
Hot Reconfigure
     ↓
Verify
```

Ein Neustart darf erforderlich bleiben, wenn Hardware oder Sicherheitsgrenzen dies verlangen.

## Hardware Changes

Neue oder entfernte Hardware darf automatisch in die Systemkonfiguration integriert werden.

```text
Hardware Change
      ↓
Discovery
      ↓
Capability / Driver Resolution
      ↓
Constraint Check
      ↓
Configuration
```

Neue Hardware erhält dadurch nicht automatisch zusätzliche Rechte.

## Configuration Drift

NovaOS soll Abweichungen zwischen Desired State und Actual State erkennen.

```text
Desired State
      ≠
Actual State
      ↓
Configuration Drift
      ↓
Reconciliation
```

Ob eine Abweichung automatisch korrigiert werden darf, hängt von der jeweiligen Autonomy Policy ab.

## Security

Self-Configuration darf Security-, Trust- oder Authorization-Regeln nicht umgehen.

Insbesondere gilt:

```text
Detected
    ≠
Trusted
    ≠
Authorized
```

Neue Komponenten müssen dieselben Sicherheitsprüfungen durchlaufen wie manuell konfigurierte Komponenten.

## Reversible Configuration

Autonome Änderungen sollen möglichst reversibel sein.

Bevorzugt werden:

```text
Transactional Changes
Versioned Configuration
Snapshots
Rollback
Compensating Actions
```

Irreversible Änderungen können einen höheren Autonomy Level oder explizite Bestätigung verlangen.

## Verification

Nach einer Konfigurationsänderung muss NovaOS den resultierenden Zustand prüfen können.

```text
Configuration Change
        ↓
Actual State
        ↓
Verification
   ├── Valid   → Commit
   └── Invalid → Recovery / Rollback
```

Eine erfolgreich ausgeführte Änderung ist nicht automatisch eine erfolgreich erreichte Konfiguration.

## Failure und Unknown

Kann NovaOS keine gültige Konfiguration bestimmen, darf es keine unsichere Konfiguration erzwingen.

```text
No Valid Configuration
        ↓
Safe State
        ↓
Degrade / Request Decision
```

Dabei gilt:

```text
Unknown ≠ Valid
```

## Explainability

NovaOS soll relevante automatische Konfigurationsentscheidungen erklären können.

Beispiele:

```text
Warum wurde dieser Provider gewählt?

Warum wurde dieser Treiber aktiviert?

Warum wurde eine Einstellung geändert?

Warum wurde eine Änderung nicht durchgeführt?

Warum wurde eine Konfiguration zurückgesetzt?
```

## Deterministic Mode

Self-Configuration muss im Deterministic Mode reproduzierbar sein können.

Dazu müssen relevante:

```text
System State
Hardware State
Policies
Constraints
Configuration Rules
Adaptive State
```

fixiert, versioniert oder kontrolliert werden können.

## Observability

Self-Configuration muss mit Decision Tracing und System State Graph korrelierbar sein.

Relevante Informationen sind:

```text
Configuration ID
Previous State
Desired State
Configuration Plan
Decision Reason
Applied Changes
Verification Result
Rollback Result
```

## Normative Anforderungen

1. NovaOS SOLL Self-Configuration als autonome Systemfunktion unterstützen.
2. Self-Configuration MUSS auf Policies und Constraints basieren.
3. Desired State und Actual State MÜSSEN getrennt betrachtet werden.
4. Hardware, Provider und Capabilities DÜRFEN automatisch erkannt werden.
5. Discovery DARF keine Trust- oder Authorization-Entscheidung ersetzen.
6. Automatische Konfiguration MUSS ausschließlich innerhalb des gültigen Constraint Space erfolgen.
7. Fehlende explizite Konfiguration DARF durch sichere Defaults ergänzt werden.
8. Explizite Nutzerkonfiguration MUSS Vorrang vor adaptiver oder automatischer Konfiguration besitzen.
9. Adaptive Modelle DÜRFEN Konfigurationen optimieren, aber keine Hard Constraints verändern.
10. Konfigurationsänderungen SOLLEN transaktional und reversibel erfolgen.
11. Configuration Drift MUSS erkennbar sein.
12. Automatische Drift-Korrektur MUSS der jeweiligen Autonomy Policy unterliegen.
13. Security-, Trust- und Authorization-Regeln DÜRFEN durch Self-Configuration nicht umgangen werden.
14. Konfigurationsänderungen MÜSSEN nach ihrer Anwendung verifizierbar sein.
15. Fehlgeschlagene Änderungen SOLLEN rollback- oder recoveryfähig sein.
16. `Unknown` DARF nicht als gültige Konfiguration interpretiert werden.
17. Relevante automatische Konfigurationsentscheidungen SOLLEN erklärbar sein.
18. Deterministic Mode MUSS Self-Configuration reproduzierbar machen können.
19. Self-Configuration MUSS mit Observability und Decision Tracing korrelierbar sein.
20. NovaOS MUSS auch ohne adaptive Optimierung eine sichere Grundkonfiguration herstellen können.

## Konsequenzen

### Positive Konsequenzen

- NovaOS benötigt deutlich weniger manuelle Systemkonfiguration,
- Hardware- und Systemänderungen können automatisch integriert werden,
- Configuration Drift kann selbstständig erkannt und korrigiert werden,
- Konfiguration bleibt mit Desired State und Contracts konsistent,
- fehlerhafte Änderungen können kontrolliert zurückgesetzt werden.

### Negative Konsequenzen

- automatische Konfiguration benötigt zuverlässige Discovery- und Verification-Mechanismen,
- Konflikte zwischen expliziter und automatisch erzeugter Konfiguration müssen eindeutig aufgelöst werden,
- dynamische Hardware- und Systemänderungen erhöhen den Planungsaufwand.

## Abhängigkeiten

- `ADR-AUTONOMY-0001_Policy_Driven_Autonomy`
- `ADR-AUTONOMY-0002_Constraint_Driven_Autonomy`
- `ADR-ADAPTIVE-0012_Policy_Learning_mit_harten_Grenzen`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-DECLARATIVE-0001_Desired_State_und_Actual_State`
- `ADR-DECLARATIVE-0002_State_Reconciliation`
- `ADR-DECLARATIVE-0003_Declarative_Resources`
- `ADR-DECLARATIVE-0004_Declarative_Services`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-OBSERVABILITY-0005_System_State_Graph`
- `ADR-RESILIENCE-0008_Recovery_Verification`

## Zugehörige NPSPECs

- `NPSPEC-AUTONOMY-SELF-CONFIGURATION-0001`
- `NPSPEC-AUTONOMY-CONFIGURATION-PLANNING-0001`
- `NPSPEC-AUTONOMY-CONFIGURATION-DRIFT-0001`
- `NPSPEC-AUTONOMY-CONFIGURATION-VERIFICATION-0001`
- `NPSPEC-AUTONOMY-CONFIGURATION-ROLLBACK-0001`

## Ergebnis

NovaOS behandelt Konfiguration als autonomen Reconciliation-Prozess:

```text
Discover
   ↓
Observe Actual State
   ↓
Compare Desired State
   ↓
Apply Policies + Constraints
   ↓
Plan Configuration
   ↓
Apply
   ↓
Verify
   ↓
Reconcile
```

Der zentrale Grundsatz lautet:

```text
NovaOS soll sich selbst
sinnvoll konfigurieren können.

Der Nutzer beschreibt möglichst,
was erreicht werden soll –
nicht jeden technischen Schritt.

Das System darf den Weg
selbstständig bestimmen,
aber nur innerhalb seiner
Policies und Constraints.
```