# ADR-OBSERVABILITY-0009 – Architecture Introspection als Systemfunktion

## Status

Angenommen

## Kategorie

Observability Architecture / Introspection

## Kontext

NovaOS besitzt eine stark dynamische Architektur aus:

```text
Objects
Capabilities
Services
Algorithms
Providers
Resources
Executions
Dependencies
Policies
Contracts
```

Für Diagnose, Administration, Entwicklung und Self-Healing muss das System seine eigene Architektur zur Laufzeit beschreiben können.

Architecture Introspection darf deshalb keine reine Debug-Funktion sein.

## Entscheidung

NovaOS stellt **Architecture Introspection als native Systemfunktion** bereit.

```text
Running System
      ↓
Architecture Introspection
      ↓
Structured Architecture Model
      ↓
Query / Inspection / Analysis
```

Die Introspection beschreibt die tatsächlich vorhandene und beobachtbare Systemarchitektur.

## Introspection Model

NovaOS muss insbesondere Informationen über folgende Elemente bereitstellen können:

```text
Components
Services
Capabilities
Objects
Algorithms
Providers
Resources
Interfaces
Contracts
Dependencies
Relationships
Versions
Locations
Health States
```

Die Informationen werden über stabile Systemidentitäten miteinander verbunden.

## Architekturbeziehungen

Architecture Introspection muss Beziehungen explizit darstellen können.

Beispiele:

```text
Service ─────uses─────→ Capability
Capability ─provided_by→ Provider
Execution ───uses─────→ Algorithm
Algorithm ───runs_on──→ Compute Provider
Service ───depends_on─→ Service
Object ─────stored_on─→ Storage Provider
Component ─implements─→ Interface
```

Beziehungen müssen semantisch typisiert sein.

## Statische und dynamische Architektur

NovaOS unterscheidet zwischen:

```text
Declared Architecture
Observed Architecture
```

Declared Architecture beschreibt definierte Komponenten, Contracts und Abhängigkeiten.

Observed Architecture beschreibt den tatsächlich beobachteten Laufzeitzustand.

```text
Declared
   ↓
Comparison
   ↑
Observed
```

Abweichungen müssen erkennbar sein.

## System State Graph

Der `System State Graph` bildet die zentrale Laufzeitsicht für Architecture Introspection.

```text
Architecture Metadata
        +
Observed State
        +
Relationships
        ↓
System State Graph
        ↓
Architecture Introspection
```

Architecture Introspection ist jedoch die Systemfunktion und nicht mit einer bestimmten internen Graphimplementierung gleichzusetzen.

## Contracts

Contracts müssen introspektierbar sein.

Beispiele:

```text
Execution Contracts
Capability Contracts
Interface Contracts
Resource Contracts
Storage Contracts
Security Constraints
```

Dabei muss zwischen Contract-Definition und aktuellem Erfüllungszustand unterschieden werden.

## Execution Architecture

Eine konkrete Execution soll architektonisch nachvollziehbar sein.

```text
Execution
 ├── Capability
 ├── Algorithm
 ├── Provider
 ├── Resources
 ├── Input Objects
 ├── Output Objects
 └── Execution Contract
```

Damit kann NovaOS erklären, aus welchen Systemelementen eine Ausführung tatsächlich zusammengesetzt wurde.

## Location

Location Transparency darf physische Architekturinformationen nicht vollständig verbergen.

Introspection kann deshalb unterscheiden zwischen:

```text
Logical Identity
Logical Relationship
Physical Location
Execution Location
Data Location
```

So bleiben lokale und verteilte Architekturen analysierbar.

## Versionen

Introspection muss relevante Versionen sichtbar machen können.

Beispiele:

```text
Component Version
Interface Version
Capability Version
Algorithm Version
Provider Version
Contract Version
Policy Version
```

Dies ist insbesondere für Reproduzierbarkeit, Updates und Fehleranalyse erforderlich.

## Abhängigkeiten

Abhängigkeiten müssen in beide Richtungen untersuchbar sein.

```text
What does this component depend on?

What depends on this component?
```

Dadurch können Impact Analysis und Recovery Scope bestimmt werden.

## Observability

Architecture Introspection wird mit Observability korreliert.

```text
Architecture
    ↓
Executions
    ↓
Logs
Metrics
Traces
Decisions
Resources
```

Damit kann ein Observability-Ereignis auf die betroffene Architektur zurückgeführt werden.

## Provenance

Architecture Introspection und Provenance Inspection ergänzen sich.

```text
Architecture Introspection
    → Was existiert und wie hängt es zusammen?

Provenance Inspection
    → Woher stammt es und wie entstand es?
```

Beide verwenden stabile Identitäten zur Korrelation.

## Self-Healing

`Nova.Resilience` darf Architecture Introspection für Recovery Planning verwenden.

Beispiel:

```text
Failed Provider
      ↓
Architecture Introspection
      ↓
Dependent Executions
      ↓
Affected Capabilities
      ↓
Affected Services
      ↓
Recovery Scope
```

Introspection selbst führt keine Recovery Policy aus.

## Zugriff

Architecture Introspection ist keine uneingeschränkt öffentliche Sicht auf das gesamte System.

Abfragen unterliegen:

```text
Authorization
Security Policy
Privacy
Data Classification
Data Sovereignty
```

Ein Consumer sieht nur die Architekturinformationen, für die er eine zulässige Sicht besitzt.

## Maschinenlesbarkeit

Architecture Introspection muss primär strukturiert und maschinenlesbar sein.

```text
Structured Architecture Data
        ↓
CLI
GUI
Developer Tools
Diagnostics
Self-Healing
Automation
```

Menschenlesbare Darstellungen werden aus diesem Modell erzeugt.

## Normative Anforderungen

1. NovaOS MUSS Architecture Introspection als native Systemfunktion bereitstellen.
2. Introspection MUSS strukturierte und maschinenlesbare Informationen liefern.
3. Komponenten, Services, Capabilities, Objects, Algorithms, Providers und Resources MÜSSEN introspektierbar sein können.
4. Architekturbeziehungen MÜSSEN semantisch typisiert darstellbar sein.
5. Bestehende stabile Systemidentitäten SOLLEN zur Korrelation verwendet werden.
6. Declared und Observed Architecture MÜSSEN unterscheidbar sein.
7. Abweichungen zwischen deklarierter und beobachteter Architektur MÜSSEN erkennbar sein können.
8. Contracts und deren relevanter Zustand MÜSSEN introspektierbar sein.
9. Execution Architecture MUSS über Execution IDs nachvollziehbar sein.
10. Logische Identität und physische Location MÜSSEN unterscheidbar bleiben.
11. Relevante Versionen MÜSSEN introspektierbar sein.
12. Abhängigkeiten MÜSSEN für Impact Analysis untersuchbar sein.
13. Architecture Introspection MUSS mit dem System State Graph korrelierbar sein.
14. Architecture Introspection MUSS mit Logs, Metrics, Traces und Decision Traces korrelierbar sein.
15. Provenance Inspection MUSS über gemeinsame Identitäten integrierbar sein.
16. `Nova.Resilience` MUSS Introspection für Impact- und Recovery-Analyse verwenden können.
17. Introspection DARF keine allgemeine Recovery Policy selbst ausführen.
18. Zugriff auf Introspection MUSS Authorization-, Privacy-, Security- und Data-Sovereignty-Regeln einhalten.

## Konsequenzen

### Positive Konsequenzen

- NovaOS kann seine eigene Architektur zur Laufzeit beschreiben,
- Abhängigkeiten und Auswirkungen von Fehlern werden sichtbar,
- Debugging und Diagnose werden systemweit vereinheitlicht,
- Developer Tools benötigen keine proprietären Architekturmodelle,
- Self-Healing kann auf tatsächlichen Systembeziehungen arbeiten.

### Negative Konsequenzen

- Komponenten müssen relevante Architekturinformationen bereitstellen,
- dynamische Zustände können kurzfristig veraltet sein,
- Introspection benötigt Zugriffskontrolle und Datenfilterung.

## Abhängigkeiten

- `ADR-ARCH-0013_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-OBSERVABILITY-0001_Unified_Logging_Metrics_Tracing`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-OBSERVABILITY-0005_System_State_Graph`
- `ADR-OBSERVABILITY-0008_Provenance_Inspection`
- `ADR-OBJECT-0001_Unified_System_Object_Model`
- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-ALGORITHM-0008_Algorithm_Registry`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-DISTRIBUTED-0009_Location_Transparent_Execution`
- `ADR-RESILIENCE-0020_Explainable_Self_Healing`

## Zugehörige NPSPECs

- `NPSPEC-OBSERVABILITY-ARCHITECTURE-INTROSPECTION-0001`
- `NPSPEC-OBSERVABILITY-ARCHITECTURE-QUERY-0001`
- `NPSPEC-OBSERVABILITY-ARCHITECTURE-RELATIONSHIP-0001`
- `NPSPEC-OBSERVABILITY-ARCHITECTURE-STATE-0001`
- `NPSPEC-OBSERVABILITY-ARCHITECTURE-ACCESS-0001`

## Ergebnis

Architecture Introspection wird zu einer grundlegenden NovaOS-Systemfunktion:

```text
Running NovaOS
      ↓
Architecture Introspection
      ↓
Components + Relationships + State
      ↓
System State Graph
      ↓
Diagnosis
Developer Tools
Impact Analysis
Self-Healing
```

Der zentrale Grundsatz lautet:

```text
Die Architektur von NovaOS
darf keine Black Box sein.

Das System muss strukturiert
beschreiben können,
woraus es besteht,
wie seine Teile zusammenhängen
und welchen Zustand sie besitzen.
```