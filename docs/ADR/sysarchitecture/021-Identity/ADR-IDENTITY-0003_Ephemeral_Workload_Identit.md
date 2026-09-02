# ADR-IDENTITY-0003 – Ephemeral Workload Identity

## Status

Angenommen

## Kategorie

Identity Architecture / Workload Identity / Runtime Identity

## Kontext

Nicht jede NovaOS-Entität benötigt eine langlebige, persistente Systemidentität.

Kurzlebige Workloads wie:

- Prozesse,
- Tasks,
- Capability Provider Instances,
- Sandbox-Instanzen,
- temporäre Dienste,
- KI-Agent-Instanzen,
- Jobs und Worker

benötigen während ihrer Ausführung dennoch eine eindeutig überprüfbare Identität.

Eine stabile Service- oder Application-Identity allein reicht dafür nicht aus, da mehrere gleichzeitig laufende Instanzen voneinander unterscheidbar sein müssen.

## Entscheidung

NovaOS führt **Ephemeral Workload Identities** für kurzlebige Ausführungsinstanzen ein.

```text
Stable Identity
      ↓
Workload Instance
      ↓
Ephemeral Workload Identity
```

Die Ephemeral Workload Identity existiert nur innerhalb des definierten Workload-Lifecycles und wird nach dessen Ende ungültig.

## Grundprinzip

```text
Stable Identity = logical continuity

Ephemeral Identity = current execution instance
```

Beide Identitäten können miteinander verbunden sein, bleiben aber semantisch getrennt.

## Identity Binding

Eine Ephemeral Workload Identity kann an relevante Laufzeitkontexte gebunden werden:

```text
Application Identity
Service Identity
Execution Domain
Sandbox
Task Group
Session
Device Context
```

Die Bindung muss systemverifizierbar sein.

Ein Workload darf nicht selbstständig behaupten, zu einer bestimmten stabilen Identity zu gehören.

## Lifecycle

Die Identity folgt dem Lifecycle des Workloads.

```text
Workload Created
      ↓
Identity Issued
      ↓
Workload Active
      ↓
Workload Terminated
      ↓
Identity Invalid
```

Ein neuer Workload erhält grundsätzlich eine neue Ephemeral Identity, auch wenn er dieselbe logische Anwendung oder denselben Dienst ausführt.

## Authority

Eine Ephemeral Workload Identity erzeugt keine automatische Authority.

```text
Workload Identity ≠ Capability
```

Capabilities können jedoch gezielt an eine bestimmte Workload Identity gebunden werden.

Dadurch kann Authority automatisch mit dem Ende des Workloads ihre Gültigkeit verlieren.

## Delegation und Isolation

Ephemeral Workload Identities ermöglichen eine präzise Zuordnung delegierter Authority.

```text
Service
   ↓
Attenuated Capability
   ↓
Workload Identity
```

Damit kann ein temporärer Worker genau die Authority erhalten, die er für seine Aufgabe benötigt, ohne die vollständige Authority des übergeordneten Dienstes zu übernehmen.

## Lokale und entfernte Workloads

Das Modell gilt sowohl für lokale als auch für entfernte Workloads.

Eine Remote Workload Identity muss zusätzlich über geeignete Identity-, Trust- und kryptographische Mechanismen verifizierbar sein.

Netzwerkadresse oder Verbindung dürfen nicht als Ersatz für die Workload Identity verwendet werden.

## Audit und Causation

Ephemeral Workload Identities sollen Operationen eindeutig einer konkreten Ausführungsinstanz zuordnen können.

```text
Stable Service Identity
        ↓
Workload Identity
        ↓
Operation / Nova.Call
```

Dadurch kann zwischen verschiedenen Instanzen desselben Dienstes unterschieden werden.

## Normative Anforderungen

1. NovaOS MUSS Ephemeral Workload Identities für kurzlebige Ausführungsinstanzen unterstützen.
2. Ephemeral Workload Identity MUSS von stabiler Systemidentität getrennt sein.
3. Jede gleichzeitig relevante Workload-Instanz MUSS eindeutig identifizierbar sein können.
4. Die Identity MUSS an den Lifecycle des Workloads gebunden sein.
5. Eine beendete Workload Identity DARF nicht für neue Operationen weiterverwendet werden.
6. Neue Workload-Instanzen MÜSSEN grundsätzlich neue Ephemeral Identities erhalten.
7. Die Bindung an Application-, Service- oder andere stabile Identities MUSS systemverifizierbar sein.
8. Ephemeral Identity DARF keine automatische Authority erzeugen.
9. Capabilities MÜSSEN an Ephemeral Workload Identities bindbar sein können.
10. Delegierte Authority MUSS mit dem Workload-Lifecycle begrenzbar sein.
11. Netzwerkadresse, Process ID oder Runtime Handle DÜRFEN die Workload Identity NICHT ersetzen.
12. Workload Identity und relevante Bindungen MÜSSEN kontrolliert auditierbar und introspektierbar sein.

## Konsequenzen

### Positive Konsequenzen

- eindeutige Identifikation einzelner Workload-Instanzen,
- präzisere Capability-Delegation,
- Authority kann automatisch an Workload-Lifecycles gekoppelt werden,
- bessere Isolation und Auditierbarkeit,
- stabile Service Identity und konkrete Ausführungsinstanz bleiben unterscheidbar.

### Negative Konsequenzen

- Workload Identities müssen effizient erzeugt und verwaltet werden,
- verteilte Workloads benötigen zusätzliche Verifikation.

## Verworfene Alternativen

### Process ID als Workload Identity

Verworfen, da Process IDs lokale, wiederverwendbare Runtime-Kennungen sind.

### Stabile Service Identity für alle Instanzen verwenden

Verworfen, da einzelne Workloads dadurch nicht eindeutig unterscheidbar wären.

### Workload Identity dauerhaft weiterverwenden

Verworfen, da dies die Bindung zwischen Identity und konkretem Lifecycle aufheben würde.

## Abhängigkeiten

- `ADR-IDENTITY-0001_Identity_getrennt_von_Display_Name_Credential_Process_und_Address`
- `ADR-IDENTITY-0002_Stabile_Systemidentitäten`
- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-SECURITY-0003_Least_Privilege_und_Privilege_Separation`
- `ADR-SECURITY-0011_Capability_Leases_und_Epochs`
- `ADR-CAPABILITY-0007_Capability_Delegation`

## Zugehörige NPSPECs

- `NPSPEC-IDENTITY-WORKLOAD-0001`
- `NPSPEC-IDENTITY-WORKLOAD-BINDING-0001`
- `NPSPEC-IDENTITY-WORKLOAD-LIFECYCLE-0001`
- `NPSPEC-IDENTITY-WORKLOAD-AUTHORITY-0001`
- `NPSPEC-IDENTITY-WORKLOAD-TEST-0001`

## Ergebnis

NovaOS unterscheidet zwischen langlebiger logischer Identität und kurzlebiger Ausführungsidentität:

```text
Stable Identity
      │
      ├── Workload A → Ephemeral Identity A
      ├── Workload B → Ephemeral Identity B
      └── Workload C → Ephemeral Identity C
```

Damit können einzelne Workloads eindeutig identifiziert, isoliert und mit lifecyclegebundener Authority ausgestattet werden, ohne die stabile Identität der übergeordneten Anwendung oder des Dienstes zu verändern.