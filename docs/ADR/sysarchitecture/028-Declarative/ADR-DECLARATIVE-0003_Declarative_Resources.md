# ADR-DECLARATIVE-0003 – Declarative Resources

## Status

Angenommen

## Kategorie

Declarative Architecture / Resources

## Kontext

NovaOS verwaltet Ressourcen wie Dienste, Geräte, Speicher, Netzwerkendpunkte und Capabilities über ein gemeinsames Objektmodell.

Für deklarative Systemsteuerung darf nicht nur der Gesamtzustand des Systems beschreibbar sein. Auch einzelne Ressourcen müssen ihren gewünschten Zustand deklarativ ausdrücken können.

## Entscheidung

NovaOS verwendet **Declarative Resources**.

Eine Ressource kann ihren gewünschten Zustand über eine deklarative Resource Definition beschreiben.

```text
Resource
 ├── Identity
 ├── Type
 ├── Desired State
 └── Constraints
```

Die Definition beschreibt das gewünschte Ergebnis und nicht die dafür notwendigen Einzelschritte.

## Beispiel

```text
Resource:
    Type: Service
    Identity: network.service

Desired State:
    State: Running
    Startup: Automatic
```

NovaOS bestimmt selbst den notwendigen Übergang vom Actual State zum Desired State.

## Resource Identity

Declarative Resources verwenden stabile Object Identities.

```text
Resource Definition
       ↓
Object ID
       ↓
Actual Resource
```

Name, Pfad oder Speicherort sind nicht die primäre Identität.

## Desired State

Der Desired State kann Eigenschaften einer Ressource beschreiben.

Beispiele:

```text
State
Configuration
Version
Availability
Permissions
Relationships
Required Capabilities
```

Welche Eigenschaften zulässig sind, wird durch den Resource Contract bestimmt.

## Actual State

Der tatsächliche Zustand wird unabhängig vom Desired State beobachtet.

```text
Desired Resource State
          ↓
       Compare
          ↑
Actual Resource State
```

Eine deklarierte Eigenschaft gilt erst dann als erreicht, wenn sie im Actual State verifiziert wurde.

## Reconciliation

Declarative Resources werden durch State Reconciliation verwaltet.

```text
Resource Definition
       ↓
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

## Abhängigkeiten

Ressourcen können deklarative Abhängigkeiten besitzen.

```text
Service A
   ↓ requires
Capability B
   ↓ requires
Device C
```

Abhängigkeiten beschreiben Voraussetzungen und nicht imperative Startreihenfolgen.

## Constraints

Resource Definitions können Constraints enthalten.

Beispiele:

```text
Minimum Version
Required Trust
Resource Budget
Locality
Availability
Security Policy
```

Der Reconciler muss diese beim Erstellen eines Plans berücksichtigen.

## Lifecycle

Der gewünschte Lifecycle einer Ressource kann deklarativ beschrieben werden.

```text
Present
Absent
Running
Stopped
Enabled
Disabled
```

Die konkrete Umsetzung bleibt Aufgabe des zuständigen Providers.

## Normative Anforderungen

1. NovaOS MUSS Ressourcen deklarativ beschreiben können.
2. Declarative Resources MÜSSEN stabile Object Identities verwenden können.
3. Resource Definitions MÜSSEN Desired State und Constraints beschreiben können.
4. Resource Contracts MÜSSEN zulässige Zustände und Eigenschaften definieren.
5. Actual State MUSS unabhängig beobachtet werden.
6. Declarative Resources MÜSSEN durch State Reconciliation verwaltbar sein.
7. Abhängigkeiten MÜSSEN deklarativ ausdrückbar sein.
8. Resource Definitions DÜRFEN keine konkrete Implementierung erzwingen, sofern der Contract dies nicht verlangt.
9. Ein Desired Resource State DARF erst nach Verification als erreicht gelten.

## Konsequenzen

### Positive Konsequenzen

- Ressourcen werden einheitlich deklarativ steuerbar,
- Implementierung und gewünschter Zustand bleiben getrennt,
- Self-Healing kann auf denselben Resource Definitions aufbauen,
- Abhängigkeiten und Constraints werden maschinenlesbar.

### Negative Konsequenzen

- Resource Contracts müssen klar definiert werden,
- Provider benötigen zuverlässige Observation- und Reconciliation-Schnittstellen.

## Abhängigkeiten

- `ADR-ARCH-0003_Deklaratives_Systemmodell`
- `ADR-OBJECT-0001_Unified_System_Object_Model`
- `ADR-OBJECT-0002_Stabile_Object_Identity`
- `ADR-DECLARATIVE-0001_Desired_State_und_Actual_State`
- `ADR-DECLARATIVE-0002_State_Reconciliation`
- `ADR-ARCH-0014_Explizite_Contracts`

## Zugehörige NPSPECs

- `NPSPEC-DECLARATIVE-RESOURCE-0001`
- `NPSPEC-DECLARATIVE-RESOURCE-CONTRACT-0001`
- `NPSPEC-DECLARATIVE-RESOURCE-STATE-0001`

## Ergebnis

NovaOS beschreibt Ressourcen über ihren gewünschten Zustand:

```text
Declarative Resource
        ↓
Desired State
        ↓
Reconciliation
        ↓
Actual Resource
        ↓
Verification
```

Der zentrale Grundsatz lautet:

```text
Eine Ressource beschreibt,
wie sie sein soll.

NovaOS bestimmt,
wie sie diesen Zustand erreicht.
```