# ADR-DISTRIBUTED-0010 – Remote Execution unter Sovereignty und Trust Constraints

## Status

Angenommen

## Kategorie

Distributed Architecture / Security / Execution

## Kontext

Remote Execution ermöglicht NovaOS, Capabilities auf anderen Geräten oder Cluster-Nodes auszuführen.

Dabei können Daten, Metadaten und Ausführungskontext die lokale Systemgrenze verlassen.

Eine technisch erreichbare Remote-Ressource darf deshalb nicht automatisch als zulässiger Execution Provider betrachtet werden.

## Entscheidung

NovaOS erlaubt **Remote Execution nur innerhalb expliziter Sovereignty- und Trust-Constraints**.

```text
Execution Request
        ↓
Sovereignty Check
        ↓
Trust Check
        ↓
Authorization
        ↓
Execution Contract
        ↓
Remote Execution
```

Kann eine verpflichtende Bedingung nicht verifiziert werden, darf der Remote Provider nicht verwendet werden.

## Sovereignty Constraints

Data Sovereignty definiert, wo Daten verarbeitet oder gespeichert werden dürfen.

Beispiele:

```text
LocalOnly
DeviceOnly
TrustedCluster
OrganizationOnly
RegionRestricted
NoRemoteProcessing
```

Diese Anforderungen gelten als Hard Constraints.

## Trust Constraints

Remote Provider müssen das erforderliche Trust Level nachweisen können.

```text
Provider Identity
       ↓
Trust Evidence
       ↓
Required Trust
       ↓
Accepted / Rejected
```

Trust darf nicht allein aus Netzwerkstandort oder Erreichbarkeit abgeleitet werden.

## Datenklassifikation

Die Zulässigkeit einer Remote Execution kann von den verwendeten Daten abhängen.

```text
Input Objects
     ↓
Semantic Metadata
     ↓
Privacy / Sovereignty Policy
     ↓
Allowed Execution Locations
```

Unterschiedliche Objekte derselben Operation können unterschiedliche Einschränkungen besitzen.

## Provider Selection

Nur Provider, die alle Hard Constraints erfüllen, gelangen in die gültige Kandidatenmenge.

```text
Remote Providers
       ↓
Sovereignty Filter
       ↓
Trust Filter
       ↓
Authorization Filter
       ↓
Valid Providers
```

Performance, Energie oder Auslastung werden erst danach berücksichtigt.

## Trust Changes

Trust ist kein dauerhaft garantierter Zustand.

```text
Trusted Provider
       ↓
Trust Evidence Changed
       ↓
Reevaluation
```

Ändert sich der Trust State, müssen neue Ausführungen erneut bewertet werden.

Laufende Operationen müssen entsprechend ihrem Contract behandelt werden.

## Kein automatischer Fallback

Wird Remote Execution durch Sovereignty oder Trust blockiert, darf NovaOS nicht automatisch auf einen anderen unzulässigen Remote Provider ausweichen.

```text
Remote Provider rejected
        ↓
Local Provider available?
   ├── Yes → Replan
   └── No  → Requirement unsatisfied
```

Constraints dürfen für einen Fallback nicht abgeschwächt werden.

## Datenminimierung

Bei zulässiger Remote Execution sollen nur die tatsächlich benötigten Daten übertragen werden.

```text
Required Input
      ↓
Minimum Disclosure
      ↓
Remote Provider
```

Unnötige Objekt-, Metadata- oder Credential-Übertragung ist zu vermeiden.

## Verification

Die Zulässigkeit muss sowohl bei der Planung als auch unmittelbar vor relevanter Remote Execution prüfbar sein.

```text
Plan
 ↓
Constraint Verification
 ↓
Execute
 ↓
Result Verification
```

Ein zuvor gültiger Plan darf nicht blind verwendet werden, wenn sich relevante Trust- oder Policy-Zustände geändert haben.

## Normative Anforderungen

1. NovaOS MUSS Sovereignty und Trust als Hard Constraints für Remote Execution behandeln.
2. Remote Provider MÜSSEN eine überprüfbare Identität besitzen.
3. Erreichbarkeit DARF nicht als Trust-Nachweis gelten.
4. Data-Sovereignty-Policies MÜSSEN vor Remote Execution geprüft werden.
5. Trust-Anforderungen MÜSSEN vor Remote Execution geprüft werden.
6. Nicht verifizierbare Hard Constraints MÜSSEN zur Ablehnung des Providers führen.
7. Performance- oder Adaptive Policies DÜRFEN Sovereignty- oder Trust-Constraints nicht überschreiben.
8. Trust-Änderungen MÜSSEN eine erneute Bewertung ermöglichen.
9. Fallback und Replanning DÜRFEN Constraints nicht abschwächen.
10. Remote Execution SOLL nur die minimal erforderlichen Daten offenlegen.
11. Ergebnisse einer Remote Execution MÜSSEN entsprechend ihrem Contract verifiziert werden.

## Konsequenzen

### Positive Konsequenzen

- Remote Execution bleibt mit Data Sovereignty vereinbar,
- nicht vertrauenswürdige Provider werden ausgeschlossen,
- Optimierungen können Sicherheitsgrenzen nicht überschreiben,
- sensible Daten bleiben kontrollierbar.

### Negative Konsequenzen

- mögliche Remote Provider werden stärker eingeschränkt,
- Trust- und Sovereignty-Zustände müssen für die Planung verfügbar sein.

## Abhängigkeiten

- `ADR-DISTRIBUTED-0001_Distributed_Execution`
- `ADR-DISTRIBUTED-0004_Remote_Capabilities`
- `ADR-DISTRIBUTED-0009_Location_Transparent_Execution`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-TRUST-0001_Trust_als_Evidence_basiertes_System`
- `ADR-PRIVACY-0007_Data_Sovereignty_Pipeline`

## Zugehörige NPSPECs

- `NPSPEC-DISTRIBUTED-REMOTE-TRUST-0001`
- `NPSPEC-DISTRIBUTED-REMOTE-SOVEREIGNTY-0001`
- `NPSPEC-DISTRIBUTED-REMOTE-AUTHORIZATION-0001`

## Ergebnis

NovaOS behandelt Remote Execution nicht als reine Ressourcenoptimierung:

```text
Execution Requirement
        ↓
Sovereignty
        ↓
Trust
        ↓
Authorization
        ↓
Execution Contract
        ↓
Valid Remote Providers
        ↓
Execution
```

Der zentrale Grundsatz lautet:

```text
Remote Execution ist nur zulässig,
wenn sowohl der Ort als auch der Provider
den geltenden Vertrauens- und
Souveränitätsanforderungen entsprechen.
```