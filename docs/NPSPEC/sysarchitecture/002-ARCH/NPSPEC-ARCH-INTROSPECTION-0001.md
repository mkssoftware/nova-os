# NPSPEC-ARCH-INTROSPECTION-0001 – Nova System Introspection

## Status

Angenommen

## Kategorie

Architecture / Introspection / Observability / System Model

## Zweck

NovaOS stellt eine einheitliche Introspection-Schnittstelle bereit, über die autorisierte Komponenten den strukturierten Zustand und die Beziehungen des Systems untersuchen können.

```text
System Model
     ↓
Introspection
     ↓
Tools / UI / Diagnostics / Recovery
```

## Grundprinzip

Introspection basiert auf strukturierten Systeminformationen statt auf systemspezifischer Interpretation von Logs, Dateien oder Speicherstrukturen.

Abfragbar sind unter anderem:

```text
Objects
Processes
Services
Capabilities
Resources
Devices
Relationships
State
Health
Execution
```

## Introspection API

Abfragen erfolgen über stabile IDs, semantische Typen und Beziehungen.

Beispiele:

```text
Get(EntityID)
Query(Type)
Relations(EntityID)
State(EntityID)
Capabilities(EntityID)
ResourceUsage(EntityID)
```

## Beziehungen

Introspection muss Zusammenhänge sichtbar machen können.

```text
Process
   └── uses → Capability
                 └── provided-by → Service
                                      └── runs-on → CPU
```

Dadurch können Fragen beantwortet werden wie:

```text
Warum läuft dieser Prozess?
Welche Ressource verwendet er?
Welche Capability stellt dieser Service bereit?
Wovon hängt diese Komponente ab?
```

## Live State

Introspection kann sowohl aktuelle Zustände als auch verfügbare historische Informationen bereitstellen.

```text
Current State
History
Version
Health
Resource Usage
```

## Architektur-Introspection

NovaOS soll seine eigene Architektur untersuchbar machen.

```text
Component
Provider
Dependency
Capability
Contract
Resource
Relationship
```

Dadurch können Diagnose- und Entwicklertools dieselbe Systembeschreibung verwenden.

## Sicherheit

Introspection erzeugt keine zusätzlichen Berechtigungen.

```text
Introspection Request
        ↓
Security Context
        ↓
Capability / Policy Check
        ↓
Filtered Result
```

Sensible Informationen müssen entsprechend dem aufrufenden Kontext gefiltert werden.

## Performance

Introspection darf kritische Systempfade nicht unnötig blockieren.

Teure Abfragen sollen begrenzt, gefiltert oder asynchron ausgeführt werden können.

## Normative Anforderungen

1. NovaOS MUSS eine strukturierte System-Introspection bereitstellen.
2. Introspection MUSS auf dem Nova System Model aufbauen können.
3. Entitäten MÜSSEN über stabile IDs abfragbar sein.
4. Semantische Typen und Beziehungen MÜSSEN untersuchbar sein.
5. Ressourcen- und Capability-Beziehungen SOLLEN sichtbar gemacht werden können.
6. Introspection MUSS capability- und policy-basiert geschützt werden.
7. Eine Introspection-Anfrage DARF keine zusätzlichen Berechtigungen erzeugen.
8. Sensible Informationen MÜSSEN entsprechend dem Security Context gefiltert werden.
9. Introspection SOLL Diagnose, Debugging, Recovery und System-UI unterstützen.
10. Introspection DARF kritische Systempfade nicht unkontrolliert blockieren.

## Abhängigkeiten

- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-ARCH-0013_Introspection`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-OBSERVABILITY-0009_Architecture_Introspection_as_system_function`

## Ergebnis

```text
Nova System
     ↓
Structured Introspection
     ↓
Understand
Diagnose
Explain
Debug
Recover
```

NovaOS wird dadurch nicht zu einer Black Box, sondern kann seinen eigenen Zustand und seine internen Beziehungen kontrolliert und maschinenlesbar sichtbar machen.