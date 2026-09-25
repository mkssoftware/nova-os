# NPSPEC-NETWORK-SOVEREIGNTY-0001 – Nova Network Sovereignty

## Status

Angenommen

## Kategorie

Network / Sovereignty / Data Control

## Zweck

NovaOS definiert Network Sovereignty als verbindliche Kontrolle darüber, über welche Netzwerkpfade, Regionen, Provider und Vertrauensdomänen Daten übertragen werden dürfen.

```text
Network Flow
     ↓
Sovereignty Policy
     ↓
Allowed Paths
     ↓
Routing / Multipath / VPN
```

Sovereignty-Anforderungen sind Teil der Ausführungsbedingungen eines Netzwerkflows und keine reine Routing-Präferenz.

## Grundprinzip

```text
Reachable ≠ Allowed
Fastest Path ≠ Permitted Path
Encrypted ≠ Sovereign
Location ≠ Trust
```

Ein technisch erreichbarer Netzwerkpfad darf nicht verwendet werden, wenn er die geltenden Sovereignty-Anforderungen verletzt.

## Sovereignty Context

Ein Netzwerkflow darf einen Sovereignty-Kontext besitzen.

```text
NetworkSovereignty
├── FlowID
├── PolicyID
├── Requirements
└── State
```

Optional:

```text
Allowed Regions
Denied Regions
Allowed Providers
Denied Providers
Trust Requirements
Network Classes
Required VPN
ExecutionContract
```

Policies sollen deklarativ beschrieben werden.

## Policy-Beispiele

```text
Only Local Network
Only Trusted Networks
Only EU
Exclude Region X
Require Corporate VPN
Require Specific Provider
No Public Network
Offline Only
```

NovaOS darf weitere Kriterien ergänzen.

## Policy-Hierarchie

Bei mehreren Anforderungen gilt die NovaOS-Constraint-Priorität:

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

Performance-, Energie- oder Komfortoptimierungen dürfen Sovereignty-Regeln nicht überschreiben.

## Routing

Routing muss Sovereignty-Anforderungen berücksichtigen können.

```text
Destination
    ↓
Candidate Routes
    ↓
Sovereignty Filter
    ↓
Allowed Routes
    ↓
Route Selection
```

Nicht erlaubte Routen werden vor der eigentlichen Optimierung ausgeschlossen.

Existiert kein gültiger Pfad, muss die Operation kontrolliert fehlschlagen oder auf eine ausdrücklich erlaubte Alternative warten.

## Multipath

Multipath darf ausschließlich zulässige Pfade verwenden.

```text
Candidate Paths
├── Path A → Allowed
├── Path B → Denied
└── Path C → Allowed
```

Ein Pfadausfall darf nicht automatisch zu einem verbotenen Fallback führen.

## VPN

Sovereignty Policies dürfen VPN-Nutzung verlangen.

```text
Flow
 ↓
VPN Required
 ↓
VPN Available?
├── Yes → Continue
└── No  → Block
```

Ein VPN allein beweist jedoch keine geografische oder organisatorische Sovereignty.

Die Eigenschaften des VPN-Endpunkts und des weiteren Netzwerkpfads müssen berücksichtigt werden können.

## Network Migration

Bei jeder Migration muss der neue Pfad erneut gegen die Sovereignty Policy geprüft werden.

```text
Current Path
     ↓
Migration Candidate
     ↓
Sovereignty Validation
     ↓
Allow / Reject
```

Eine bestehende Verbindung darf durch Migration keine geltenden Einschränkungen verlieren.

## Network Namespaces

Namespaces dürfen eigene Sovereignty Policies besitzen.

```text
Namespace
    ↓
Sovereignty Policy
    ↓
All Network Flows
```

Dadurch können beispielsweise Unternehmens-, Entwicklungs- oder besonders geschützte Umgebungen getrennte Netzwerkregeln erhalten.

## Herkunft von Pfadinformationen

Sovereignty-Entscheidungen dürfen Informationen verwenden wie:

```text
Interface
Network Identity
VPN Endpoint
Provider
Administrative Domain
Configured Region
Verified Network Metadata
```

Nicht verifizierte Netzwerkangaben dürfen nicht automatisch als vertrauenswürdige Sovereignty-Eigenschaften gelten.

## Execution Contracts

Network Sovereignty ist in `Nova.ExecutionContract` integrierbar.

```text
ExecutionContract
├── Security
├── Trust
├── Sovereignty
└── Network Requirements
```

Hard Sovereignty Requirements dürfen nicht stillschweigend degradiert werden.

Soft Preferences dürfen optimiert werden, solange alle Hard Constraints erfüllt bleiben.

## Datenschutz

Sovereignty-Metadaten können sensible Informationen enthalten.

Informationen über:

```text
Network Location
Provider
Organization
Policy
Traffic Destination
```

dürfen nur autorisierten Komponenten zugänglich sein.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Sovereignty Policies
Path Metadata
Validation State
Policy Cache
Route Evaluations
Migration Checks
```

Policy-Prüfungen dürfen keine unkontrollierten Ressourcenzyklen erzeugen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
FlowID
PolicyID
Requirements
Candidate Paths
Rejected Paths
Selected Path
Validation State
Decision Reason
```

Damit muss nachvollziehbar sein, warum ein bestimmter Netzwerkpfad zugelassen oder ausgeschlossen wurde.

## Normative Anforderungen

1. NovaOS MUSS Network Sovereignty als eigenständige Netzwerkpolicy unterstützen.
2. Sovereignty-Anforderungen MÜSSEN vor Performance-Optimierungen berücksichtigt werden.
3. Routing MUSS nicht zulässige Pfade ausschließen können.
4. Multipath DARF keine durch Sovereignty Policy verbotenen Pfade verwenden.
5. Migration MUSS den neuen Pfad erneut validieren.
6. VPN-Nutzung MUSS durch Sovereignty Policies vorgeschrieben werden können.
7. VPN-Nutzung DARF NICHT automatisch als Sovereignty-Nachweis gelten.
8. Network Namespaces SOLLEN eigene Sovereignty Policies besitzen können.
9. Hard Sovereignty Requirements DÜRFEN NICHT stillschweigend degradiert werden.
10. Ein fehlender zulässiger Pfad DARF NICHT automatisch einen unzulässigen Fallback auslösen.
11. Nicht verifizierte Pfadmetadaten DÜRFEN NICHT automatisch als vertrauenswürdig gelten.
12. Sovereignty-Entscheidungen und deren Gründe SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-FLOW-0001`
- `NPSPEC-NETWORK-ROUTING-0001`
- `NPSPEC-NETWORK-MULTIPATH-0001`
- `NPSPEC-NETWORK-NAMESPACE-0001`
- `NPSPEC-NETWORK-VPN-0001`
- `NPSPEC-NETWORK-QOS-0001`
- `NPSPEC-NETWORK-MIGRATION-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-NETWORK-0024`

## Ergebnis

```text
Network Flow
     ↓
Security + Trust + Sovereignty
     ↓
Allowed Path Set
     ↓
Routing + Multipath
     ↓
Controlled Network Path
```

NovaOS erhält damit eine verbindliche Sovereignty-Schicht, durch die Daten nur über Netzwerkpfade übertragen werden, die den festgelegten Sicherheits-, Vertrauens-, Provider- und Standortanforderungen entsprechen.