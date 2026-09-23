# NPSPEC-ARCH-SYSTEMMODEL-0001 – Nova System Model

## Status

Entwurf

## Kategorie

Architecture / System Model / Declarative State / Resources

## Zweck

Das Nova System Model beschreibt NovaOS als zusammenhängendes, deklaratives System aus Objekten, Ressourcen, Capabilities, Beziehungen und Zuständen.

Statt Systemzustände ausschließlich aus verstreuten Prozessen, Dateien, Handles und Konfigurationen abzuleiten, besitzt NovaOS ein explizites Modell seines aktuellen und gewünschten Zustands.

```text
Desired State
     ↓
Nova System Model
     ↕
Reconciliation
     ↕
Actual State
```

Das System Model bildet damit die gemeinsame Grundlage für:

```text
Konfiguration
Ressourcen
Services
Hardware
Storage
Network
Security
Capabilities
UI
Execution
Recovery
Distributed Resources
```

## Grundprinzip

NovaOS unterscheidet zwischen:

```text
Desired State
```

und:

```text
Actual State
```

Beispiel:

```text
Desired:
Network.Interface = Enabled

Actual:
Network.Interface = Disabled
```

Das System versucht kontrolliert:

```text
Actual State
      ↓
Desired State
```

anzugleichen.

## Architektur

```text
User Intent
System Policy
Configuration
Applications
Capabilities
        ↓
Desired State
        ↓
Nova System Model
        ↓
Reconciliation Engine
        ↓
System Operations
        ↓
Actual State
        ↑
Observation
```

## System State Graph

Das System Model wird als verknüpfter Zustandsgraph betrachtet.

```text
System
 ├── Hardware
 ├── Devices
 ├── Services
 ├── Processes
 ├── Objects
 ├── Storage
 ├── Network
 ├── Capabilities
 ├── Security
 ├── Sessions
 └── UI
```

Elemente werden über stabile Identitäten und semantische Beziehungen miteinander verbunden.

Beispiel:

```text
User Session
     │
     ├── owns → Workspace
     │
     ├── uses → Display
     │
     └── runs → Process
                    │
                    └── provides → Capability
```

## System Entity

Eine Systementität besitzt mindestens:

```text
EntityID
Type
State
Properties
Relationships
Version
```

Optional:

```text
Capabilities
Security Context
Resource Information
Health
Provenance
Location
```

## Stabile Identität

Identität wird von Name, Pfad oder physischem Standort getrennt.

```text
EntityID
   ≠
Name
   ≠
Path
   ≠
Location
```

Dadurch kann eine Ressource verschoben oder umbenannt werden, ohne ihre Systemidentität zu verlieren.

## Semantische Typen

Entitäten besitzen semantische Typen.

Beispiele:

```text
Storage.Volume
Network.Interface
Display.Output
Compute.GPU
Service
User.Session
Document
Capability
```

Dadurch kann NovaOS verstehen, welche Funktion eine Ressource besitzt, ohne ausschließlich ihre Implementierung kennen zu müssen.

## Beziehungen

Beziehungen sind explizit und typisiert.

Beispiele:

```text
owns
uses
depends-on
provides
contains
connected-to
runs-on
stored-on
authorized-by
derived-from
```

Beispiel:

```text
Application
    │
    └── uses → Capability
                  │
                  └── provided-by → Service
```

## Desired State

Der Desired State beschreibt den gewünschten Systemzustand.

Beispiele:

```text
Service.State = Running

Network.Interface.State = Enabled

Display.Mode = 2560x1440

Backup.Policy = Enabled
```

Desired State beschreibt das Ziel und nicht zwingend die notwendigen Ausführungsschritte.

## Actual State

Der Actual State beschreibt den tatsächlich beobachteten Zustand.

```text
Hardware
Drivers
Services
Processes
Resources
Devices
Storage
Network
```

Der Zustand wird durch autorisierte Systemkomponenten beobachtet.

## Reconciliation

NovaOS gleicht Desired und Actual State kontrolliert ab.

```text
Observe
   ↓
Compare
   ↓
Plan
   ↓
Execute
   ↓
Verify
```

Beispiel:

```text
Desired: Service = Running
Actual:  Service = Stopped

        ↓

Plan:
Start Service

        ↓

Verify

        ↓

Actual: Service = Running
```

## Abhängigkeiten

Systementitäten können explizite Abhängigkeiten besitzen.

```text
Service A
   ↓ depends-on
Service B
   ↓ depends-on
Device C
```

Der Reconciler kann daraus eine geeignete Ausführungsreihenfolge bestimmen.

Zirkuläre oder nicht erfüllbare Abhängigkeiten müssen erkannt werden.

## Constraints

Systemzustände können durch Constraints begrenzt werden.

Beispiele:

```text
Security
Trust
Data Sovereignty
Resource Budget
Realtime
Energy
Hardware
User Policy
```

Priorität:

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

Adaptive Mechanismen dürfen harte Constraints nicht überschreiben.

## Transaktionale Änderungen

Zusammengehörige Änderungen sollen als konsistente Systemtransaktionen ausgeführt werden können.

```text
Current State
     ↓
Prepare
     ↓
Validate
     ↓
Commit
     ↓
New State
```

Fehler dürfen nicht unnötig zu teilweise angewendeten Systemzuständen führen.

## Versionierung

Relevante Zustände können versioniert werden.

```text
State v1
   ↓
State v2
   ↓
State v3
```

Dadurch werden:

```text
History
Comparison
Rollback
Diagnostics
Recovery
```

ermöglicht.

## Snapshots

Das System Model kann konsistente Snapshots relevanter Zustände erzeugen.

```text
System State
     ↓
Snapshot
     ↓
Recovery / Analysis / Comparison
```

Ein Snapshot muss nicht zwingend sämtliche Nutzdaten enthalten, sondern beschreibt den für seinen Zweck notwendigen Zustand.

## Ressourcen

Ressourcen werden als Systementitäten modelliert.

Beispiele:

```text
CPU
Memory
GPU
Storage
Network
Energy
Accelerators
Devices
```

Dadurch können Ressourcenplanung und Systemzustand dieselbe gemeinsame Modellbasis verwenden.

## Capabilities

Capabilities sind Teil des System Models.

```text
Capability
 ├── CapabilityID
 ├── Provider
 ├── State
 ├── Constraints
 └── Relationships
```

Beispiel:

```text
Image.Edit
    ↓
provided-by
    ↓
Provider A
```

Provider können dynamisch erscheinen oder verschwinden.

## Location Transparency

Eine Entität kann lokal oder remote bereitgestellt werden.

```text
EntityID
   ↓
Location
   ├── Local
   └── Remote
```

Die Identität bleibt von der physischen Ausführungsposition getrennt.

Location Transparency darf Security-, Trust- oder Sovereignty-Regeln nicht umgehen.

## UI-Integration

Die NovaOS-Oberfläche kann Teile des System Models als semantische Projektion darstellen.

```text
System Model
     ↓
Semantic UI
     ↓
Desktop / Dashboard / Settings
```

Beispiel:

```text
Network.Interface
      ↓
Settings UI
```

Die UI ist damit eine Darstellung des Systemzustands und nicht dessen alleinige Quelle.

## Settings

Systemeinstellungen verändern bevorzugt Desired State.

```text
User changes Setting
        ↓
Desired State Update
        ↓
Reconciliation
        ↓
System Change
```

Dadurch müssen Einstellungen nicht direkt hardware- oder implementierungsspezifische Operationen ausführen.

## Self-Healing

Das System Model bildet eine Grundlage für Self-Healing.

```text
Expected State
      ↓
Observed Deviation
      ↓
Diagnosis
      ↓
Recovery Plan
      ↓
Reconciliation
```

Self-Healing darf nur innerhalb definierter Sicherheits- und Autonomiegrenzen handeln.

## Adaptive Systeme

Adaptive Komponenten dürfen Vorschläge oder Optimierungen erzeugen.

```text
Observation
    ↓
Prediction
    ↓
Optimization Proposal
    ↓
Constraint Validation
    ↓
System Model
```

Sie verändern das System nicht außerhalb ihrer autorisierten Policy.

## Introspection

NovaOS muss seinen Systemzustand strukturiert untersuchen können.

Beispiele:

```text
Warum läuft dieser Service?

Welche Capability verwendet diese Anwendung?

Welche Ressource blockiert diese Operation?

Warum wurde dieser Provider gewählt?

Welche Abhängigkeit ist ausgefallen?
```

Das System Model stellt dafür die Beziehungen und Zustände bereit.

## Observability

Änderungen am System Model sollen nachvollziehbar sein.

```text
Entity
Change
Previous State
New State
Reason
Initiator
Timestamp
Transaction
```

Nicht jede Information muss dauerhaft gespeichert werden; Retention und Privacy gelten weiterhin.

## Fehlerzustände

Das System Model unterscheidet mindestens:

```text
Known
Unknown
Unavailable
Failed
Pending
```

`Unknown` darf nicht automatisch als:

```text
Healthy
Authorized
Satisfied
```

interpretiert werden.

## Recovery

Nach Absturz oder Neustart kann NovaOS:

```text
Persisted Desired State
        +
Observed Actual State
        ↓
Reconciliation
        ↓
Recovered System State
```

verwenden.

Dadurch muss nicht jeder vorherige Laufzeitzustand blind rekonstruiert werden.

## Verteilte Systeme

Das System Model kann verteilte Ressourcen abbilden.

```text
Local System
     │
     ├── Local Resource
     │
     └── Remote Resource
              ↓
          Remote Node
```

Netzwerkausfall oder unbekannter Remote-Zustand muss explizit als solcher dargestellt werden.

## Sicherheit

Das System Model ist kein global frei zugänglicher Datenbestand.

Zugriff erfolgt entsprechend:

```text
Identity
Security Context
Capability
Policy
Information Flow
Privacy
```

Eine Komponente erhält nur die für ihre Aufgabe notwendigen Ausschnitte und Operationen.

## Normative Anforderungen

1. NovaOS MUSS Desired State und Actual State logisch unterscheiden.
2. Systementitäten MÜSSEN über stabile Identitäten referenzierbar sein.
3. Identität DARF NICHT ausschließlich von Name, Pfad oder physischem Standort abhängen.
4. Systementitäten SOLLEN semantische Typen besitzen.
5. Beziehungen zwischen Systementitäten SOLLEN explizit und typisiert sein.
6. Desired und Actual State MÜSSEN durch einen kontrollierten Reconciliation-Prozess abgeglichen werden können.
7. Reconciliation MUSS dem Ablauf `Observe → Compare → Plan → Execute → Verify` folgen können.
8. Harte Safety-, Security-, Trust- und Sovereignty-Constraints DÜRFEN NICHT durch adaptive Optimierungen überschrieben werden.
9. Zusammengehörige Zustandsänderungen SOLLEN transaktional ausführbar sein.
10. Relevante Systemzustände SOLLEN versionierbar und snapshotfähig sein.
11. Abhängigkeiten zwischen Systementitäten MÜSSEN darstellbar sein.
12. Nicht erfüllbare und zirkuläre Abhängigkeiten MÜSSEN erkennbar sein.
13. Capabilities und Ressourcen MÜSSEN in das System Model integrierbar sein.
14. Lokale und entfernte Ressourcen SOLLEN über dasselbe grundlegende Modell beschreibbar sein.
15. Location Transparency DARF NICHT Security-, Trust- oder Sovereignty-Regeln umgehen.
16. System-UI SOLL den Systemzustand aus dem System Model projizieren können.
17. Einstellungen SOLLEN bevorzugt Desired State verändern statt direkte Implementierungsoperationen auszuführen.
18. `Unknown` DARF NICHT als erfolgreicher, sicherer oder autorisierter Zustand interpretiert werden.
19. Der Zugriff auf das System Model MUSS capability- und policy-basiert kontrolliert werden.
20. Das System Model MUSS Introspection, Observability, Recovery und Self-Healing unterstützen können.

## Abhängigkeiten

- `ADR-ARCH-0003_Deklaratives_Systemmodell`
- `ADR-ARCH-0002_Mechanism_und_Policy`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0013_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-STATE-0001_Globaler_System_State_Graph`
- `ADR-STATE-0002_Desired_und_Actual_State`
- `ADR-STATE-0003_State_Machines`
- `ADR-STATE-0004_State_Reconciliation`
- `ADR-STATE-0005_State_Snapshots`
- `ADR-STATE-0006_State_Versioning_und_History`
- `ADR-STATE-0007_Transactional_State`
- `ADR-STATE-0008_State_Rollback`

## Ergebnis

Das Nova System Model bildet die gemeinsame strukturelle Sicht auf NovaOS:

```text
Objects
Resources
Capabilities
Services
Hardware
Security
Configuration
      ↓
System State Graph
      ↓
Desired ↔ Actual
      ↓
Reconciliation
      ↓
Controlled System Operations
```

NovaOS wird dadurch nicht nur als Sammlung laufender Programme und Dateien betrachtet, sondern als explizit modelliertes, beobachtbares und kontrolliert veränderbares Gesamtsystem.