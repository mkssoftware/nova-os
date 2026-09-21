# ADR-OBSERVABILITY-0008 – Provenance Inspection

## Status

Angenommen

## Kategorie

Observability Architecture / Provenance

## Kontext

NovaOS verwendet Provenance, um die Herkunft und Entstehung von Software, Objekten, Daten und Ergebnissen nachvollziehbar zu machen.

Für Diagnose, Security, Reproduzierbarkeit und Introspection muss diese Provenance nicht nur gespeichert, sondern auch systemweit inspizierbar sein.

Beispiele:

```text
Woher stammt dieses Objekt?
Welche Version wurde verwendet?
Welche Capability erzeugte es?
Welcher Algorithmus wurde ausgeführt?
Welcher Provider führte ihn aus?
Welche Eingaben waren beteiligt?
Welche Transformationen fanden statt?
```

## Entscheidung

NovaOS führt **Provenance Inspection** als systemweite Observability- und Introspection-Fähigkeit ein.

```text
Object / Result / Software
          ↓
Provenance Evidence
          ↓
Provenance Inspection
          ↓
Origin + Transformation + Dependencies
```

Provenance Inspection erzeugt keine Herkunftsinformationen, sondern macht vorhandene Provenance strukturiert zugänglich.

## Provenance-Modell

Provenance kann Beziehungen zwischen folgenden Elementen darstellen:

```text
Object
Version
Execution
Capability
Algorithm
Provider
Software Component
Input
Output
Transformation
```

Beispiel:

```text
Input Object
     ↓
Execution
     ├── Capability
     ├── Algorithm + Version
     └── Provider
     ↓
Output Object
```

## Objekt-Provenance

Für ein Objekt soll nachvollziehbar sein können:

```text
Object ID
Version ID
Origin
Creation Execution
Input Objects
Transformations
Producer
Relevant Software Version
```

Provenance ist dabei versionsbezogen.

```text
Object ID
   ├── Version 1 → Provenance A
   └── Version 2 → Provenance B
```

## Execution-Provenance

Execution-Provenance verbindet ein Ergebnis mit seiner tatsächlichen Ausführung.

Relevant können sein:

```text
Execution ID
Capability ID
Algorithm ID + Version
Provider ID
Execution Location
Input Versions
Output Versions
Execution Contract Reference
```

Damit kann ein Ergebnis auf seine Erzeugung zurückgeführt werden.

## Software-Provenance

Auch ausführende Software kann inspiziert werden.

Beispiele:

```text
Component Identity
Version
Build Identity
Package Identity
Signature
Supply-Chain Provenance
Installation Source
```

Damit kann nachvollzogen werden, welche konkrete Software an einer Ausführung beteiligt war.

## Provenance Graph

Provenance kann als gerichteter Graph dargestellt werden.

```text
Object A ─┐
Object B ─┼→ Execution X → Object C
          │
Algorithm ┤
Provider ─┘
```

Mehrere Verarbeitungsschritte bilden eine Provenance Chain:

```text
Object A
   ↓
Execution 1
   ↓
Object B
   ↓
Execution 2
   ↓
Object C
```

## System State Graph

Provenance und aktueller Systemzustand bleiben getrennte Sichtweisen.

```text
System State Graph
    → Was existiert und wie hängt es aktuell zusammen?

Provenance Graph
    → Woher stammt etwas und wie ist es entstanden?
```

Beide Graphen müssen über stabile Identitäten korrelierbar sein.

## Distributed Provenance

Bei Distributed Execution darf die Provenance Chain nicht an Node-Grenzen abbrechen.

```text
Object
  ↓
Node A
  ↓
Remote Execution
  ↓
Node B
  ↓
Result Object
```

Remote Provenance muss ihre Herkunft und vorhandene Evidence kenntlich machen.

## Provenance und Trust

Provenance ist Evidence, aber nicht automatisch Trust.

```text
Known Origin
     ≠
Trusted Origin
     ≠
Authorized Origin
```

Provenance Inspection darf daher nicht selbst entscheiden, ob ein Objekt oder eine Komponente vertrauenswürdig ist.

Diese Bewertung bleibt Aufgabe der Trust- und Security-Policies.

## Provenance und Decision Tracing

Decision Tracing und Provenance ergänzen sich.

```text
Decision Tracing
    → Warum wurde diese Ausführung gewählt?

Provenance
    → Was wurde tatsächlich verwendet und erzeugt?
```

Beide Informationen können über Execution IDs verbunden werden.

## Reproduzierbarkeit

Provenance Inspection kann relevante Informationen für reproduzierbare Ausführungen bereitstellen.

Beispiele:

```text
Input Version
Algorithm Version
Provider
Software Version
Execution Parameters
Deterministic State
```

Provenance allein garantiert jedoch keine Reproduzierbarkeit.

## Unvollständige Provenance

Provenance kann vollständig, teilweise oder unbekannt sein.

```text
Verified
Partial
Unknown
```

Fehlende Informationen dürfen nicht erfunden oder als verifiziert dargestellt werden.

Es gilt:

```text
Unknown Provenance ≠ Trusted Provenance
```

## Privacy und Security

Provenance kann sensible Beziehungen und Informationen enthalten.

Der Zugriff unterliegt deshalb:

```text
Authorization
Data Classification
Privacy
Security Policy
Data Sovereignty
```

Secrets, Credentials und unnötige Nutzdaten dürfen nicht Bestandteil der Provenance Inspection sein.

## Normative Anforderungen

1. NovaOS MUSS vorhandene Provenance strukturiert inspizierbar machen können.
2. Provenance MUSS über stabile Objekt-, Versions- und Execution-Identitäten korrelierbar sein.
3. Objekt-Provenance MUSS versionsbezogen darstellbar sein.
4. Input- und Output-Beziehungen MÜSSEN nachvollziehbar sein können.
5. Capability-, Algorithmus- und Provider-Informationen SOLLEN mit Execution-Provenance verknüpfbar sein.
6. Software-Provenance MUSS integrierbar sein.
7. Mehrstufige Provenance Chains MÜSSEN als Graph darstellbar sein.
8. Distributed Execution DARF Provenance Chains nicht grundsätzlich unterbrechen.
9. Provenance und System State Graph MÜSSEN korrelierbar sein.
10. Provenance und Decision Tracing MÜSSEN über Execution-Identitäten korrelierbar sein.
11. Provenance DARF nicht automatisch als Trust oder Authorization interpretiert werden.
12. Unvollständige Provenance MUSS als `Partial` oder `Unknown` erkennbar sein.
13. Fehlende Provenance DARF nicht synthetisch als verifiziert dargestellt werden.
14. Provenance Inspection MUSS für Diagnose, Reproduzierbarkeit, Security und Introspection nutzbar sein.
15. Zugriff auf Provenance MUSS Privacy-, Security-, Authorization- und Data-Sovereignty-Regeln einhalten.
16. Secrets und Credentials DÜRFEN nicht durch Provenance Inspection offengelegt werden.

## Konsequenzen

### Positive Konsequenzen

- Herkunft von Objekten und Ergebnissen wird nachvollziehbar,
- Verarbeitungsketten können über mehrere Executions verfolgt werden,
- Software- und Daten-Provenance werden miteinander verbindbar,
- Reproduzierbarkeit und Fehleranalyse werden verbessert,
- Security- und Trust-Systeme erhalten überprüfbare Evidence.

### Negative Konsequenzen

- Provenance benötigt zusätzliche Metadaten,
- lange Verarbeitungsketten können große Graphen erzeugen,
- Zugriff auf Provenance benötigt differenzierte Berechtigungen.

## Abhängigkeiten

- `ADR-OBSERVABILITY-0001_Unified_Logging_Metrics_Tracing`
- `ADR-OBSERVABILITY-0002_Distributed_Tracing`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-OBSERVABILITY-0005_System_State_Graph`
- `ADR-ARCH-0013_Introspection`
- `ADR-OBJECT-0002_Stabile_Object_Identity`
- `ADR-OBJECT-0003_Object_Versioning`
- `ADR-OBJECT-0004_Provenance_als_versionsbezogene_Evidence`
- `ADR-OBJECT-0009_Daten_und_Objekt_Pipelines`
- `ADR-TRUST-0001_Trust_als_Evidence_basiertes_System`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-DISTRIBUTED-0001_Distributed_Execution`

## Zugehörige NPSPECs

- `NPSPEC-OBSERVABILITY-PROVENANCE-INSPECTION-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-GRAPH-0001`
- `NPSPEC-OBSERVABILITY-EXECUTION-PROVENANCE-0001`
- `NPSPEC-OBSERVABILITY-OBJECT-PROVENANCE-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-QUERY-0001`

## Ergebnis

NovaOS kann die Herkunft und Entstehung eines Ergebnisses systemweit nachvollziehen:

```text
Inputs
   ↓
Execution
   ├── Capability
   ├── Algorithm
   ├── Provider
   └── Software
   ↓
Output
   ↓
Provenance Graph
   ↓
Inspection
```

Der zentrale Grundsatz lautet:

```text
NovaOS soll nicht nur wissen,
was ein Objekt ist.

Es soll nachvollziehen können,
woher es stammt,
welche Versionen beteiligt waren
und durch welche Ausführungen
es entstanden ist.
```