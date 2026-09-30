# NPSPEC-INTENT-0005 – Intent Constraints & Policies

## Status

Angenommen

## Zweck

Diese Spezifikation definiert Constraints und Policies für `Nova.Intent`.

Sie bestimmen, unter welchen technischen, qualitativen und sicherheitsbezogenen Bedingungen ein Intent ausgeführt werden darf.

Grundprinzip:

```text
Intent
    + Constraints
    + Policies
        ↓
gültiger Ausführungsraum
```

## Constraints

Constraints beschreiben Anforderungen an die Ausführung.

Beispiele:

```text
constraints {
    deadline: 500ms
    max_memory: 512MiB
    deterministic: true
    required_accuracy: 99%
    local_only: true
}
```

Typische Constraint-Klassen:

- Zeit und Deadline
- Speicher
- CPU/GPU/NPU-Ressourcen
- Energie
- Genauigkeit
- Präzision
- Determinismus
- Trust-Level
- lokale oder entfernte Ausführung
- Kosten

Constraints können entweder zwingend oder bevorzugt sein.

```text
hard:
    local_only: true

prefer:
    low_energy: true
```

`hard` muss erfüllt werden.

`prefer` beeinflusst die Auswahl, darf aber verletzt werden, wenn keine bessere zulässige Alternative existiert.

## Policies

Policies definieren Regeln darüber, was eine Ausführung tun darf.

Beispiel:

```text
policies {
    network: denied
    remote_execution: denied
    persistent_storage: allowed
    external_data_transfer: denied
}
```

Policies können unter anderem regeln:

- Netzwerkzugriff
- Dateisystemzugriff
- Gerätezugriff
- Remote-Ausführung
- Datenweitergabe
- Persistenz
- Privilegien
- Informationsfluss
- Benutzerinteraktion

Policies sind nicht bloße Optimierungsziele.

Eine verletzte Policy macht einen Ausführungspfad unzulässig.

## Vererbung

Child-Intents erben standardmäßig die relevanten Constraints und Policies ihres Parent-Intents.

Beispiel:

```text
Parent:
    network: denied

Child:
    network: denied
```

Ein Child-Intent darf geerbte Einschränkungen nicht selbstständig abschwächen.

Eine Verschärfung ist zulässig.

```text
Parent:
    network: allowed

Child:
    network: denied
```

## Kombination

Mehrere Constraints und Policies können gleichzeitig gelten.

NovaOS muss Konflikte erkennen.

Beispiel:

```text
constraint:
    remote_only: true

policy:
    remote_execution: denied
```

Ergebnis:

```text
CONFLICT
```

Ein solcher Intent darf nicht ausgeführt werden, bis der Konflikt aufgelöst ist.

## Resolution und Planung

`Intent Resolution`, `Nova.Synthesis` und der Intent Compiler müssen Constraints und Policies bei der Auswahl möglicher Ausführungspfade berücksichtigen.

Beispiel:

```text
Candidates:
    LocalCPU
    LocalNPU
    RemoteService

Policy:
    remote_execution: denied
```

Gültig:

```text
LocalCPU
LocalNPU
```

Ungültig:

```text
RemoteService
```

## Execution Contracts

Ausführungsrelevante Constraints können in einen `Nova.ExecutionContract` überführt werden.

Beispiel:

```text
Intent Constraint:
    max_latency: 50ms

Execution Contract:
    latency <= 50ms
```

Der Intent beschreibt die Anforderung.

Der Execution Contract beschreibt die konkrete Verpflichtung der gewählten Ausführung.

## Laufzeitprüfung

Einige Bedingungen müssen auch während der Ausführung überwacht werden.

Beispiel:

```text
max_memory: 512MiB
```

Wird das Limit überschritten, kann NovaOS abhängig vom Intent:

```text
throttle
replan
pause
fail
```

Policies dürfen während der Ausführung nicht stillschweigend umgangen werden.

## Änderungen

Constraints oder Policies dürfen während eines laufenden Intents nur kontrolliert geändert werden.

Eine Änderung muss:

- autorisiert sein
- nachvollziehbar bleiben
- erneut validiert werden
- gegebenenfalls Re-Resolution oder Replanning auslösen

Zwingende Sicherheitsregeln dürfen nicht durch eine Capability verändert werden.

## Beispiel

```text
Intent {
    type: media.audio.transcribe

    constraints {
        max_memory: 1GiB
        prefer {
            low_energy: true
        }
    }

    policies {
        local_execution: required
        network: denied
    }
}
```

NovaOS darf damit beispielsweise eine lokale CPU- oder NPU-Capability verwenden.

Ein Cloud-Service ist ausgeschlossen.

## Normative Anforderungen

1. Constraints und Policies MÜSSEN getrennt modellierbar sein.
2. Zwingende Constraints MÜSSEN vor und soweit erforderlich während der Ausführung geprüft werden.
3. Policies MÜSSEN für jeden gewählten Ausführungspfad gelten.
4. Child-Intents DÜRFEN geerbte Einschränkungen nicht eigenständig abschwächen.
5. Konflikte zwischen Constraints und Policies MÜSSEN erkannt werden.
6. Capabilities DÜRFEN Constraints oder Policies nicht eigenmächtig verändern.
7. Änderungen während der Laufzeit MÜSSEN autorisiert und nachvollziehbar sein.
8. Ausführungsrelevante Anforderungen MÜSSEN an `Nova.ExecutionContract` übergeben werden können.

## Abgrenzung

Diese NPSPEC definiert das Modell für Intent-bezogene Constraints und Policies.

Nicht Bestandteil sind:

- konkrete Policy-Sprache
- Berechtigungsverwaltung
- Execution-Contract-Implementierung
- Scheduling
- Kosten- und Optimierungsalgorithmen

## Zugehörige NPSPECs

- `NPSPEC-INTENT-0001 – Intent Object Model`
- `NPSPEC-INTENT-0003 – Intent Schema & Semantic Types`
- `NPSPEC-INTENT-0004 – Intent Resolution`
- `NPSPEC-INTENT-0006 – Intent Composition`
- `NPSPEC-EXECIR-0004 – Execution Contract Integration`