# ADR-COMPUTE-0001 – Unified Compute Runtime

## Status

Angenommen

## Kategorie

Compute Architecture / Execution

## Kontext

NovaOS kann Berechnungen auf unterschiedlichen Compute-Ressourcen ausführen:

```text
CPU
SIMD
GPU
NPU
Accelerator
Remote Compute
```

Programme und Capabilities sollen nicht für jede Hardwareklasse ein vollständig eigenes Ausführungsmodell benötigen.

NovaOS benötigt daher eine gemeinsame Compute-Abstraktion.

## Entscheidung

NovaOS verwendet eine **Unified Compute Runtime** als gemeinsame Ausführungsschicht für berechnungsorientierte Workloads.

```text
Compute Request
      ↓
Compute Contract
      ↓
Compute Runtime
      ↓
Provider Resolution
      ↓
CPU / GPU / NPU / Remote
      ↓
Execution
```

Der Consumer beschreibt die benötigte Berechnung und ihre Anforderungen. Die Runtime bestimmt einen geeigneten Compute Provider.

## Compute Request

Ein Compute Request beschreibt mindestens:

```text
Operation / Capability
Typed Inputs
Expected Outputs
Execution Constraints
```

Hardwaredetails müssen nur angegeben werden, wenn sie tatsächlich Bestandteil des Contracts sind.

## Compute Provider

Compute Provider repräsentieren verfügbare Ausführungsressourcen.

Beispiele:

```text
CPU Provider
SIMD Provider
GPU Provider
NPU Provider
Hardware Accelerator
Remote Provider
```

Mehrere Provider können dieselbe Compute Capability implementieren.

## Provider Resolution

Die Runtime bestimmt geeignete Provider anhand von:

```text
Capability Compatibility
Semantic Types
Execution Contract
Available Resources
Locality
Trust
Data Sovereignty
```

Erst innerhalb der gültigen Kandidatenmenge dürfen Performance, Energie oder Auslastung optimiert werden.

## Hardware-Unabhängigkeit

Consumer sollen bevorzugt gegen Compute Capabilities programmieren.

```text
Consumer
   ↓
Compute Capability
   ↓
Unified Compute Runtime
   ↓
Hardware Provider
```

Dadurch bleibt die fachliche Berechnung von der konkreten Hardware getrennt.

## Datenbewegung

Die Runtime muss Datenbewegungen bei der Provider-Auswahl berücksichtigen.

```text
Input Location
      +
Compute Location
      ↓
Execution Planning
```

Ein theoretisch schnellerer Provider ist nicht automatisch besser, wenn notwendige Datenübertragungen den Vorteil aufheben.

Zero-Copy soll verwendet werden, wenn dies technisch und semantisch möglich ist.

## Fallback

Ist ein bevorzugter Provider nicht verfügbar, kann ein kompatibler alternativer Provider verwendet werden.

```text
GPU unavailable
      ↓
Compatible CPU Provider
```

Fallback ist nur zulässig, wenn der Execution Contract weiterhin erfüllt wird.

## Determinismus

Compute Provider können unterschiedliche numerische oder zeitliche Eigenschaften besitzen.

Im Deterministic Mode darf nur ein Provider verwendet werden, der die geforderten Determinismus-Garantien erfüllt.

## Remote Compute

Remote Compute wird nicht als separates Compute-Modell behandelt.

```text
Unified Compute Runtime
        ↓
Local Provider
        oder
Remote Provider
```

Remote Provider unterliegen zusätzlich den geltenden Distributed-, Trust- und Sovereignty-Constraints.

## Normative Anforderungen

1. NovaOS MUSS eine gemeinsame Compute Runtime für unterschiedliche Compute Provider bereitstellen.
2. CPU, GPU, NPU und andere Beschleuniger SOLLEN über dasselbe logische Compute-Modell nutzbar sein.
3. Compute Requests MÜSSEN über explizite Contracts beschreibbar sein.
4. Provider MÜSSEN anhand ihrer Capability- und Contract-Kompatibilität ausgewählt werden.
5. Hardwaredetails SOLLEN nicht Bestandteil fachlicher Compute-Logik sein, sofern sie nicht ausdrücklich benötigt werden.
6. Datenlokalität und Transferkosten MÜSSEN bei der Provider-Auswahl berücksichtigt werden können.
7. Zero-Copy SOLL verwendet werden, wenn dies sicher und technisch möglich ist.
8. Fallback DARF Execution Contracts nicht verletzen.
9. Deterministic Mode MUSS bei der Provider-Auswahl berücksichtigt werden.
10. Remote Compute MUSS denselben grundlegenden Compute Contract wie lokale Provider verwenden.

## Konsequenzen

### Positive Konsequenzen

- einheitliches Compute-Modell für unterschiedliche Hardware,
- Capabilities bleiben hardwareunabhängig,
- neue Beschleuniger können als zusätzliche Provider integriert werden,
- automatische Auswahl geeigneter Hardware wird möglich,
- lokale und verteilte Rechenleistung können gemeinsam genutzt werden.

### Negative Konsequenzen

- Provider benötigen einheitliche Contracts,
- unterschiedliche Hardwareeigenschaften müssen durch die Runtime berücksichtigt werden.

## Abhängigkeiten

- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-SEMANTIC-0012_Semantic_Execution`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`
- `ADR-DISTRIBUTED-0001_Distributed_Execution`
- `ADR-DISTRIBUTED-0010_Remote_Execution_unter_Sovereignty_und_Trust_Constraints`

## Zugehörige NPSPECs

- `NPSPEC-COMPUTE-RUNTIME-0001`
- `NPSPEC-COMPUTE-PROVIDER-0001`
- `NPSPEC-COMPUTE-CONTRACT-0001`
- `NPSPEC-COMPUTE-RESOLUTION-0001`

## Ergebnis

NovaOS behandelt unterschiedliche Rechenhardware als austauschbare Compute Provider hinter einer gemeinsamen Runtime:

```text
Compute Requirement
       ↓
Unified Compute Runtime
       ↓
Contract + Constraints
       ↓
Provider Resolution
       ↓
CPU / GPU / NPU / Accelerator / Remote
       ↓
Execution
       ↓
Verified Result
```

Der zentrale Grundsatz lautet:

```text
Die Berechnung beschreibt,
was gerechnet werden soll.

NovaOS entscheidet,
wo und womit sie ausgeführt wird.
```