# ADR-DISTRIBUTED-0001 – Distributed Execution

## Status

Angenommen

## Kategorie

Distributed Architecture / Execution

## Kontext

NovaOS soll Capabilities und Aufgaben nicht grundsätzlich an das lokale Gerät binden.

Eine Operation kann abhängig von verfügbaren Ressourcen, Hardware, Security, Data Sovereignty oder Performance auf unterschiedlichen Execution Locations ausgeführt werden.

```text
Application
    ↓
Capability
    ↓
Execution Location
```

Für den Consumer soll dabei die gewünschte Funktion im Vordergrund stehen, nicht der physische Ausführungsort.

## Entscheidung

NovaOS unterstützt **Distributed Execution** als systemweite Architektur­fähigkeit.

Eine Operation kann auf einem geeigneten Execution Provider ausgeführt werden:

```text
Execution Request
       ↓
Planner
       ↓
 ┌─────┼─────┬──────────┐
Local  Device  Cluster  Remote
CPU/GPU
```

Die Auswahl erfolgt anhand des Execution Contracts und der verfügbaren Provider.

## Location Transparency

Der Consumer soll grundsätzlich dieselbe Capability verwenden können, unabhängig davon, wo sie ausgeführt wird.

```text
Capability Request
       ↓
Execution Contract
       ↓
Provider Resolution
       ↓
Execution
```

Location Transparency darf jedoch Kosten, Fehler, Security-Grenzen oder Netzwerkabhängigkeiten nicht verbergen.

## Execution Contract

Distributed Execution muss relevante Anforderungen berücksichtigen.

Beispiele:

```text
Semantic Types
Determinism
Latency / Deadline
Resource Budget
Trust
Data Sovereignty
Locality
Required Hardware
```

Nur Provider, die den Contract erfüllen, dürfen berücksichtigt werden.

## Datenlokalität

Die Ausführung soll nach Möglichkeit zu den Daten verschoben werden, wenn dies effizienter oder sicherer ist.

```text
Data
 ↓
Execution
```

statt zwingend:

```text
Data
 ─────────→
Remote Execution
```

Unnötige Datenbewegungen sollen vermieden werden.

## Security und Trust

Remote Execution benötigt eine ausreichende Vertrauensbasis.

```text
Identity
   ↓
Trust
   ↓
Authorization
   ↓
Execution
```

Ein technisch erreichbarer Provider ist nicht automatisch ein zulässiger Provider.

## Failure Semantics

Verteilte Ausführung besitzt zusätzliche Fehlerzustände:

```text
Unavailable
Timeout
Connection Lost
Remote Failure
Unknown Result
```

Insbesondere gilt:

```text
Connection Lost
     ≠
Execution Failed
```

Nach Kommunikationsverlust kann der tatsächliche Ausführungszustand unbekannt sein.

`Unknown` muss daher als eigener Zustand behandelt werden.

## Fallback

Falls ein Provider ausfällt, kann NovaOS einen anderen Provider verwenden, sofern:

```text
Contract Compatible
Security Valid
Operation Semantics Allow Retry
```

Ein automatischer Retry oder Providerwechsel ist nicht für jede Operation zulässig.

## Determinismus

Distributed Execution muss den Deterministic Mode respektieren.

Wenn reproduzierbare Ausführung gefordert ist, dürfen nur Provider und Ausführungswege verwendet werden, die den entsprechenden Contract erfüllen.

## Normative Anforderungen

1. NovaOS MUSS Distributed Execution für geeignete Capabilities unterstützen können.
2. Consumer SOLLEN nicht unnötig an einen physischen Execution Provider gebunden werden.
3. Provider Selection MUSS den Execution Contract berücksichtigen.
4. Security-, Trust- und Data-Sovereignty-Constraints MÜSSEN vor Remote Execution geprüft werden.
5. Datenbewegungen SOLLEN minimiert werden.
6. Locality SOLL bei der Planung berücksichtigt werden.
7. Remote Failure und Unknown Execution State MÜSSEN unterschieden werden.
8. Retry oder Providerwechsel DARF nur bei geeigneter Operationssemantik erfolgen.
9. Distributed Execution MUSS Deterministic Mode respektieren.
10. Location Transparency DARF reale Kosten, Fehler oder Sicherheitsgrenzen nicht semantisch verbergen.

## Konsequenzen

### Positive Konsequenzen

- Capabilities können lokale und entfernte Ressourcen nutzen,
- spezielle Hardware kann systemweit verfügbar gemacht werden,
- Berechnung kann näher an Daten oder Ressourcen verschoben werden,
- Anwendungen bleiben weitgehend vom Ausführungsort entkoppelt.

### Negative Konsequenzen

- zusätzliche Fehler- und Netzwerkzustände entstehen,
- Trust, Locality und Data Sovereignty werden Teil der Ausführungsplanung,
- exakt-einmalige Ausführung kann nicht allgemein vorausgesetzt werden.

## Abhängigkeiten

- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`
- `ADR-SEMANTIC-0012_Semantic_Execution`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-RESILIENCE-0005_Retry_nur_nach_expliziter_Operationssemantik`

## Zugehörige NPSPECs

- `NPSPEC-DISTRIBUTED-EXECUTION-0001`
- `NPSPEC-DISTRIBUTED-EXECUTION-CONTRACT-0001`
- `NPSPEC-DISTRIBUTED-EXECUTION-FAILURE-0001`

## Ergebnis

NovaOS behandelt den Ausführungsort als planbare Eigenschaft einer Operation:

```text
Execution Intent
       ↓
Execution Contract
       ↓
Constraint Solving
       ↓
Provider Resolution
       ↓
Local / Remote Execution
       ↓
Verified Result
```

Der zentrale Grundsatz lautet:

```text
Die Capability beschreibt,
was ausgeführt werden soll.

Der Execution Contract bestimmt,
wo und unter welchen Bedingungen
es ausgeführt werden darf.
```