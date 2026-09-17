# ADR-DISTRIBUTED-0004 – Remote Capabilities

## Status

Angenommen

## Kategorie

Distributed Architecture / Capabilities

## Kontext

NovaOS verwendet Capabilities als primäre Abstraktion für Systemfunktionen.

Eine benötigte Capability kann lokal verfügbar sein oder durch einen anderen vertrauenswürdigen NovaOS-Knoten bereitgestellt werden.

Consumer sollen dabei nicht grundsätzlich zwischen lokalen und entfernten Capabilities unterscheiden müssen.

## Entscheidung

NovaOS unterstützt **Remote Capabilities** als reguläre Capability Provider.

```text
Consumer
   ↓
Capability Requirement
   ↓
Resolver
   ↓
┌───────────────┐
│ Local Provider│
│ Remote Provider
└───────────────┘
   ↓
Execution
```

Remote Capabilities verwenden denselben semantischen Capability Contract wie lokale Provider.

## Capability Discovery

Remote Capabilities können über das normale Discovery-Modell gefunden werden.

```text
Capability Requirement
        ↓
Discovery
        ↓
Local + Remote Providers
```

Discovery bedeutet noch keine Berechtigung zur Nutzung.

## Remote Provider

Ein Remote Provider muss mindestens beschreiben können:

```text
Capability ID
Contract Version
Semantic Input Types
Semantic Output Types
Execution Properties
Trust Information
Availability
```

Provider-spezifische Eigenschaften dürfen den Capability Contract ergänzen, aber nicht dessen Semantik verändern.

## Authorization

Vor jeder Nutzung muss geprüft werden, ob die Remote Capability verwendet werden darf.

```text
Remote Capability
       ↓
Trust
       ↓
Authorization
       ↓
Execution
```

Die bloße Kenntnis oder Erreichbarkeit einer Capability gewährt keine Zugriffsrechte.

## Execution Contract

Remote Capabilities müssen relevante Execution Constraints erfüllen.

Beispiele:

```text
Trust
Data Sovereignty
Latency
Deadline
Determinism
Resource Budget
Locality
```

Kann ein Remote Provider einen Hard Constraint nicht erfüllen, ist er kein gültiger Kandidat.

## Datenübertragung

Inputs und Outputs werden über Typed IPC beziehungsweise Distributed Communication übertragen.

```text
Typed Input
    ↓
Remote Capability
    ↓
Typed Output
```

Semantic Types und relevante Metadaten müssen über Systemgrenzen erhalten bleiben.

## Provider-Austauschbarkeit

Lokale und entfernte Provider können austauschbar sein, wenn sie denselben Contract erfüllen.

```text
Capability Contract
       ↓
 ┌─────┴─────┐
Local      Remote
Provider    Provider
```

Location Transparency darf Unterschiede bei Kosten, Latenz und Fehlersemantik nicht verbergen.

## Fehler

Remote Capabilities besitzen zusätzliche mögliche Fehlerzustände:

```text
Provider Unreachable
Connection Lost
Timeout
Trust Changed
Remote Failure
Execution State Unknown
```

Ein Kommunikationsabbruch bedeutet nicht automatisch, dass die Operation nicht ausgeführt wurde.

## Replanning

Wird ein Remote Provider ungültig oder fällt aus, kann NovaOS einen alternativen Provider auswählen.

```text
Remote Provider Failed
        ↓
Replanning
        ↓
Alternative Provider
```

Dies ist nur zulässig, wenn Operationssemantik und Execution Contract eine erneute Ausführung erlauben.

## Normative Anforderungen

1. NovaOS MUSS Remote Capabilities als Capability Provider unterstützen können.
2. Lokale und entfernte Provider MÜSSEN denselben Capability Contract verwenden können.
3. Remote Capabilities MÜSSEN über das Capability-Discovery-Modell auffindbar sein können.
4. Discovery DARF keine automatische Authorization bedeuten.
5. Remote Provider MÜSSEN vor der Nutzung Trust- und Authorization-Prüfungen bestehen.
6. Security- und Data-Sovereignty-Constraints MÜSSEN eingehalten werden.
7. Semantic Types MÜSSEN über Systemgrenzen erhalten bleiben.
8. Location Transparency DARF relevante Fehler- und Kostenunterschiede nicht verbergen.
9. Kommunikationsverlust MUSS einen `Unknown` Execution State ermöglichen.
10. Replanning und Retry MÜSSEN die Operationssemantik berücksichtigen.
11. Remote Provider DÜRFEN die Semantik eines Capability Contracts nicht verändern.

## Konsequenzen

### Positive Konsequenzen

- Capabilities können systemübergreifend genutzt werden,
- spezialisierte Hardware und Dienste werden gemeinsam verfügbar,
- Consumer bleiben weitgehend unabhängig vom Provider-Standort,
- lokale und entfernte Provider verwenden dasselbe Capability-Modell.

### Negative Konsequenzen

- Remote Execution erzeugt zusätzliche Fehlerzustände,
- Discovery, Trust und Authorization müssen systemübergreifend funktionieren.

## Abhängigkeiten

- `ADR-DISTRIBUTED-0001_Distributed_Execution`
- `ADR-DISTRIBUTED-0003_Distributed_Scheduling`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-SEMANTIC-0005_Typed_IPC`
- `ADR-SEMANTIC-0006_Typed_Capabilities`
- `ADR-SEMANTIC-0011_Semantic_Discovery`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`

## Zugehörige NPSPECs

- `NPSPEC-DISTRIBUTED-REMOTE-CAPABILITY-0001`
- `NPSPEC-DISTRIBUTED-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-DISTRIBUTED-CAPABILITY-EXECUTION-0001`

## Ergebnis

Remote Capabilities erweitern das normale Capability-Modell über Gerätegrenzen hinweg:

```text
Capability Requirement
        ↓
Discovery
        ↓
Contract + Constraints
        ↓
Local / Remote Provider
        ↓
Authorized Execution
        ↓
Typed Result
```

Der zentrale Grundsatz lautet:

```text
Eine Capability beschreibt,
was verfügbar ist.

Ob ihr Provider lokal oder entfernt läuft,
ist eine kontrollierte Ausführungsentscheidung.
```