# ADR-CAPABILITY-0005 – Capability Composition

## Status

Angenommen

## Kategorie

Capability Architecture / Composition / Data Pipelines

## Kontext

Einzelne NovaOS-Capabilities sollen nicht nur isoliert verwendet werden.

Komplexe Funktionen entstehen häufig durch die Kombination mehrerer kleiner, spezialisierter Fähigkeiten.

Beispiel:

```text
Load Image
    ↓
Decode
    ↓
Resize
    ↓
Noise Reduction
    ↓
Color Correction
    ↓
Encode
    ↓
Store
```

Würden solche Verarbeitungsketten ausschließlich innerhalb einzelner Anwendungen implementiert, entstünden erneut monolithische Funktionsblöcke und redundante Datenkonvertierungen.

NovaOS benötigt deshalb ein einheitliches Modell zur Komposition von Capabilities.

---

## Entscheidung

NovaOS definiert **Capability Composition als grundlegende Eigenschaft des Capability-Modells**.

Kompatible Capabilities können zu größeren Verarbeitungsketten und neuen zusammengesetzten Fähigkeiten verbunden werden.

```text
Capability A
     ↓
Capability B
     ↓
Capability C
     ↓
Capability D
```

Die Komposition basiert auf expliziten Capability Contracts, Semantic Types und dem systemweiten Data/Object Pipelining.

---

## Grundprinzip

```text
Complex functionality
is composed from reusable capabilities.
```

Capabilities sollen deshalb möglichst orthogonal und kombinierbar entworfen werden.

---

## Typed Composition

Ein Ausgang einer Capability kann mit dem Eingang einer anderen Capability verbunden werden, wenn ihre Verträge kompatibel sind.

```text
Capability A
Output: Image
      ↓
Capability B
Input: Image
```

Semantic Types ermöglichen dabei eine präzisere Prüfung als ausschließlich primitive Datentypen.

Beispielsweise sind:

```text
Image<RGB>
Image<Depth>
Image<Medical>
```

nicht automatisch semantisch austauschbar.

---

## Capability Pipeline

Eine lineare Composition bildet eine Capability Pipeline.

```text
Input
  ↓
Capability A
  ↓
Capability B
  ↓
Capability C
  ↓
Output
```

NovaOS darf eine solche Pipeline als zusammenhängenden Execution Plan behandeln.

Dadurch können systemweite Optimierungen über einzelne Capability-Grenzen hinweg durchgeführt werden.

---

## Graphbasierte Composition

Composition ist nicht auf lineare Pipelines beschränkt.

```text
             ┌→ Capability B ─┐
Input → A ───┤                ├→ D → Output
             └→ Capability C ─┘
```

Damit können Verzweigungen, Zusammenführungen und parallele Verarbeitung modelliert werden.

Die konkreten Graph- und Ausführungsregeln werden in NPSPECs definiert.

---

## Zusammengesetzte Capabilities

Eine Composition kann selbst als Capability veröffentlicht werden.

```text
Capability A
     +
Capability B
     +
Capability C
     ↓
Composite Capability X
```

Für Consumer kann `Composite Capability X` anschließend wie eine normale Capability erscheinen.

Ihre interne Struktur muss nicht Bestandteil der öffentlichen Schnittstelle sein.

---

## Execution Contracts

Constraints müssen durch die gesamte Composition berücksichtigt werden.

```text
Composite Capability
        ↓
Nova.ExecutionContract
        ↓
Execution Planning
        ↓
A → B → C
```

Dazu gehören insbesondere:

- Determinismus,
- Deadlines,
- Resource Budgets,
- Data Sovereignty,
- Trust,
- Locality,
- Security Requirements.

Eine Teil-Capability darf keinen Hard Constraint der Gesamtoperation verletzen.

---

## Provider-Auswahl

Provider können für jede Teil-Capability unabhängig ausgewählt werden.

```text
Decode  → CPU Provider
Resize  → GPU Provider
AI      → NPU Provider
Encode  → Hardware Codec
```

NovaOS darf jedoch auch die Pipeline als Ganzes optimieren.

Eine lokal schlechtere Einzelentscheidung kann zulässig sein, wenn sie für die Gesamtpipeline besser ist, beispielsweise durch weniger Datenbewegung.

---

## Data Movement

Capability Composition wird direkt mit dem Data/Object Pipelining verbunden.

```text
Capability A
     ↓
Shared Object / Buffer
     ↓
Capability B
```

Zero-Copy und Copy Avoidance sollen zwischen kompatiblen Capabilities erhalten bleiben.

Unnötiges Serialisieren, Kopieren oder Materialisieren von Zwischenergebnissen soll vermieden werden.

---

## Locality

Bei der Planung einer Composition berücksichtigt NovaOS die Locality der Daten und Provider.

Beispiel:

```text
GPU Capability A
       ↓
GPU Capability B
       ↓
GPU Capability C
```

kann effizienter sein als:

```text
GPU → CPU → GPU
```

obwohl einzelne CPU-Implementierungen isoliert betrachtet schneller erscheinen könnten.

---

## Security und Authority

Eine Composition vereinigt nicht automatisch die Authorities ihrer Teil-Capabilities.

```text
Capability A Authority
        +
Capability B Authority
        ≠
Unlimited Combined Authority
```

Jede relevante Operation muss weiterhin durch das Security-Modell autorisiert sein.

Capability Composition darf nicht zur Umgehung von Least Privilege, Information Flow Control oder Data Sovereignty führen.

---

## Failure und Cancellation

Fehler, Cancellation und Deadlines müssen entlang der Composition kontrolliert propagiert werden können.

Capability Composition integriert deshalb Structured Concurrency und Backpressure.

Eine fehlgeschlagene Teiloperation darf nicht unkontrolliert abhängige Verarbeitung weiterlaufen lassen.

---

## Introspection

NovaOS muss zusammengesetzte Fähigkeiten introspektierbar machen können.

Beispielsweise:

```text
Composite Capability
Pipeline / Graph
Selected Providers
Data Paths
Execution Contracts
Resource Usage
Failures
```

Damit kann nachvollzogen werden, wie eine komplexe Fähigkeit tatsächlich ausgeführt wurde.

---

## Normative Anforderungen

1. NovaOS MUSS die Composition mehrerer Capabilities unterstützen.
2. Composition MUSS auf expliziten Capability Contracts basieren.
3. Semantic Types MÜSSEN bei der Kompatibilitätsprüfung berücksichtigt werden.
4. Lineare Capability Pipelines MÜSSEN unterstützt werden.
5. Graphbasierte Composition MUSS unterstützt werden können.
6. Eine Composition MUSS selbst als Capability veröffentlicht werden können.
7. `Nova.ExecutionContract` MUSS über Capability-Grenzen hinweg berücksichtigt werden.
8. Hard Constraints MÜSSEN für die gesamte Composition gelten.
9. Provider MÜSSEN pro Teil-Capability auswählbar sein.
10. NovaOS SOLL die Gesamtpipeline statt ausschließlich einzelne Operationen optimieren.
11. Data/Object Pipelining MUSS für Capability Composition verwendet werden können.
12. Zero-Copy und Copy Avoidance SOLLEN über Capability-Grenzen hinweg erhalten bleiben, wenn dies sinnvoll und zulässig ist.
13. Locality MUSS bei der Planung berücksichtigt werden können.
14. Composition DARF Security Authority NICHT implizit erweitern.
15. Information Flow Control und Data Sovereignty MÜSSEN über die gesamte Composition erhalten bleiben.
16. Fehler, Cancellation, Deadlines und Backpressure MÜSSEN kontrolliert propagiert werden können.
17. Capability Compositions MÜSSEN introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- komplexe Funktionen entstehen aus wiederverwendbaren Fähigkeiten,
- weniger redundante Implementierungen,
- systemweite Pipeline-Optimierung,
- bessere Zero-Copy- und Locality-Optimierung,
- Hardwarebeschleuniger können gezielt kombiniert werden,
- Compositions können selbst wiederverwendbare Capabilities werden.

### Negative Konsequenzen

- Vertragskompatibilität muss präzise definiert werden,
- globale Pipeline-Planung ist komplexer als isolierte Provider-Auswahl,
- Fehler- und Lifecycle-Semantik muss über Capability-Grenzen hinweg erhalten bleiben.

---

## Verworfene Alternativen

### Capabilities ausschließlich einzeln aufrufen

Verworfen.

Dadurch könnte NovaOS zusammenhängende Verarbeitungsketten nicht systemweit planen und optimieren.

### Composition ausschließlich innerhalb von Anwendungen

Verworfen.

Dies würde die Capability-Architektur umgehen und systemweite Optimierungen verhindern.

### Automatische Composition ohne Contract-Prüfung

Verworfen.

Syntaktisch kompatible Daten müssen nicht semantisch kompatibel sein.

### Composition vereinigt automatisch alle Authorities

Verworfen.

Dies würde Least Privilege und Capability-Based Security verletzen.

---

## Abhängigkeiten

- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-CAPABILITY-0003_Capability_Registry`
- `ADR-CAPABILITY-0004_Capability_Discovery`
- `ADR-ARCH-0006_Zero_Copy`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0009_Unified_Data_Object_Pipelining`
- `ADR-ARCH-0010_Structured_Concurrency`
- `ADR-ARCH-0017_Composability_and_Orthogonality`
- `ADR-ARCH-0018_Locality_as_Optimization_Objective`

---

## Zugehörige NPSPECs

- `NPSPEC-CAPABILITY-COMPOSITION-0001`
- `NPSPEC-CAPABILITY-PIPELINE-0001`
- `NPSPEC-CAPABILITY-GRAPH-0001`
- `NPSPEC-CAPABILITY-COMPOSITE-0001`
- `NPSPEC-CAPABILITY-COMPOSITION-PLANNING-0001`
- `NPSPEC-CAPABILITY-COMPOSITION-TEST-0001`

---

## Ergebnis

NovaOS behandelt komplexe Funktionalität als Komposition kleinerer Systemfähigkeiten:

```text
Capabilities
     ↓
Composition
     ↓
Execution Plan
     ↓
Provider Selection
     ↓
Optimized Data/Object Pipeline
     ↓
Result
```

Damit wird das Capability-Modell nicht nur zu einem Katalog einzelner Funktionen, sondern zu einer Grundlage für modular aufgebaute und systemweit optimierbare Software.