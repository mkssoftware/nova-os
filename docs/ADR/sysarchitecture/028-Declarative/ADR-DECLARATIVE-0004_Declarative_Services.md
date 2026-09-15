# ADR-DECLARATIVE-0004 – Declarative Services

## Status

Angenommen

## Kategorie

Declarative Architecture / Services

## Kontext

Dienste werden traditionell häufig durch imperative Befehle gesteuert:

```text
start
stop
restart
reload
```

NovaOS soll stattdessen den gewünschten Zustand eines Dienstes beschreiben und dessen Herstellung dem Reconciliation-System überlassen.

## Entscheidung

NovaOS verwendet **Declarative Services**.

Ein Service beschreibt seinen gewünschten Zustand über eine deklarative Service Definition.

```text
Service
 ├── Identity
 ├── Desired State
 ├── Dependencies
 ├── Capabilities
 └── Constraints
```

## Beispiel

```text
Service:
    Identity: network.service

Desired State:
    State: Running
    Startup: Automatic
```

Der Consumer fordert nicht explizit `start()` an, sondern deklariert:

```text
State = Running
```

## Service State

Ein Service kann definierte Zustände besitzen.

Beispiele:

```text
Running
Stopped
Enabled
Disabled
Degraded
```

Der konkrete Zustandsautomat wird durch den Service Contract definiert.

## Actual State

NovaOS beobachtet den tatsächlichen Zustand des Dienstes.

```text
Desired:
    Running

Actual:
    Stopped
```

Der Reconciler bestimmt daraus die notwendige Aktion.

## Reconciliation

```text
Desired Service State
        ↓
Observe Actual State
        ↓
Compare
        ↓
Plan
        ↓
Execute
        ↓
Verify
```

Ein erfolgreicher Startaufruf allein bedeutet nicht, dass der Service tatsächlich gesund läuft.

## Abhängigkeiten

Services deklarieren Voraussetzungen statt feste Startreihenfolgen.

```text
Service A
   ↓ requires
Capability B
```

NovaOS bestimmt daraus die notwendige Ausführungsreihenfolge.

## Capabilities

Services können Capabilities bereitstellen oder benötigen.

```text
Service
 ├── Provides: Capability A
 └── Requires: Capability B
```

Die Abhängigkeit erfolgt bevorzugt gegen Capabilities und nicht gegen konkrete Implementierungen.

## Health

Ein laufender Prozess ist nicht automatisch ein gesunder Service.

```text
Process Running
      +
Health Contract satisfied
      ↓
Service Healthy
```

Health wird getrennt vom reinen Prozesszustand bewertet.

## Restart

Restart ist kein primärer Desired State.

```text
Desired:
    Running
```

Falls der Service nicht dem Desired State entspricht, entscheidet der Reconciler anhand von Operationssemantik, Health State und Recovery Policy über geeignete Maßnahmen.

## Normative Anforderungen

1. NovaOS MUSS Services deklarativ beschreiben können.
2. Services MÜSSEN einen Desired State definieren können.
3. Der Actual State MUSS unabhängig beobachtet werden.
4. Services MÜSSEN durch State Reconciliation verwaltbar sein.
5. Service-Abhängigkeiten SOLLEN gegen Capabilities statt konkrete Provider definiert werden.
6. Services MÜSSEN bereitgestellte und benötigte Capabilities deklarieren können.
7. Prozesszustand und Service Health MÜSSEN getrennt behandelbar sein.
8. Ein Service DARF erst nach Verification als im gewünschten Zustand gelten.
9. Restart DARF nicht als Ersatz für semantische State Reconciliation verwendet werden.

## Konsequenzen

### Positive Konsequenzen

- Services werden über Zielzustände statt Befehlsfolgen gesteuert,
- Abhängigkeiten werden deklarativ,
- Service Provider können austauschbar bleiben,
- Self-Healing kann dasselbe Reconciliation-Modell verwenden.

### Negative Konsequenzen

- Services benötigen State- und Health-Contracts,
- Reconciliation benötigt zuverlässige Service Observation.

## Abhängigkeiten

- `ADR-DECLARATIVE-0001_Desired_State_und_Actual_State`
- `ADR-DECLARATIVE-0002_State_Reconciliation`
- `ADR-DECLARATIVE-0003_Declarative_Resources`
- `ADR-RESILIENCE-0005_Retry_nur_nach_expliziter_Operationssemantik`
- `ADR-RESILIENCE-0008_Health_State_Model`
- `ADR-SEMANTIC-0006_Typed_Capabilities`

## Zugehörige NPSPECs

- `NPSPEC-DECLARATIVE-SERVICE-0001`
- `NPSPEC-DECLARATIVE-SERVICE-STATE-0001`
- `NPSPEC-DECLARATIVE-SERVICE-HEALTH-0001`

## Ergebnis

NovaOS steuert Services über ihren gewünschten Zustand:

```text
Desired Service State
        ↓
Reconciliation
        ↓
Service Provider
        ↓
Health Verification
        ↓
Actual State
```

Der zentrale Grundsatz lautet:

```text
Nicht:
Starte diesen Dienst.

Sondern:
Dieser Dienst soll laufen.
```