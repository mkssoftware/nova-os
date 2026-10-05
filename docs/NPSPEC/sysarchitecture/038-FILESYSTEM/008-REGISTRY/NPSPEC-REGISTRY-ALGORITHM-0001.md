# NPSPEC-REGISTRY-ALGORITHM-0001 – Nova Algorithm Registry

## Status

Angenommen

## Kategorie

Registry / Algorithm

## Zweck

NovaOS definiert die Algorithm Registry als systemweites Verzeichnis verfügbarer Algorithmen und ihrer Implementierungen.

Sie ermöglicht die Auswahl geeigneter Algorithmen anhand von Anforderungen, Execution Contracts und System Policy, ohne Aufrufer an eine konkrete Implementierung zu binden.

## Grundprinzipien

```text
AlgorithmID ≠ Implementation
Algorithm ≠ Provider
Algorithm ≠ Capability
Discovery ≠ Authority
Selection ≠ Execution
Preferred Algorithm ≠ Forced Algorithm
```

## Registry-Modell

Ein Eintrag kann enthalten:

```text
AlgorithmRegistryEntry
├── AlgorithmID
├── Version
├── Operation
├── SemanticInput
├── SemanticOutput
├── Implementations
└── State
```

Optional:

```text
PerformanceProfile
ResourceRequirements
Determinism
Accuracy
HardwareRequirements
TrustRequirements
Compatibility
```

Die `AlgorithmID` bildet die stabile Identität des Algorithmus unabhängig von seiner Implementierung.

## Implementierungen

Ein Algorithmus darf mehrere Implementierungen besitzen:

```text
AlgorithmID
├── CPU Implementation
├── SIMD Implementation
├── GPU Implementation
└── Accelerator Implementation
```

Die Implementierungen müssen denselben definierten funktionalen Vertrag erfüllen.

## Discovery

```text
Operation Requirement
        ↓
Algorithm Registry
        ↓
Candidate Algorithms
        ↓
Candidate Implementations
        ↓
Constraint Evaluation
```

Discovery erzeugt keine Authority und führt den Algorithmus nicht automatisch aus.

## Auswahl

Die Auswahl kann Eigenschaften des Execution Contracts berücksichtigen:

```text
Semantic Input / Output
Latency
Deadline
Resource Budget
Determinism
Energy
Trust
Security
Hardware Availability
User Preference
```

Die Auswahl erfolgt gemäß System Policy.

## Preferred und Forced

Ein Aufrufer kann einen Algorithmus bevorzugen oder ausdrücklich erzwingen:

```text
Preferred Algorithm
        ↓
Use if suitable

Forced Algorithm
        ↓
Use or Fail
```

Ein erzwungener Algorithmus darf dennoch keine Safety-, Security- oder Hard Constraints verletzen.

## Adaptive Auswahl

NovaOS darf zwischen kompatiblen Implementierungen dynamisch wählen.

```text
Algorithm
   ↓
Current System State
   ↓
Policy
   ↓
Best Valid Implementation
```

Adaptive Optimierung darf explizite Benutzerentscheidungen und höhere Constraints nicht überschreiben.

## Versionierung

Algorithmen und Implementierungen müssen versionierbar sein.

Inkompatible Änderungen dürfen nicht stillschweigend unter derselben Version veröffentlicht werden.

## Sicherheit

Die Registry speichert keine Capability-Tokens.

Benötigt eine Implementierung geschützte Ressourcen wie GPU, Netzwerk oder spezielle Hardware, müssen diese separat autorisiert werden.

## Normative Anforderungen

1. NovaOS MUSS eine Algorithm Registry bereitstellen.
2. Algorithmen MÜSSEN stabile `AlgorithmID`s besitzen können.
3. Algorithmus und Implementierung MÜSSEN getrennt behandelt werden.
4. Ein Algorithmus DARF mehrere Implementierungen besitzen.
5. Implementierungen MÜSSEN gegen den Algorithmusvertrag validierbar sein.
6. Algorithmusauswahl MUSS Execution Contracts berücksichtigen können.
7. Preferred und Forced Algorithms MÜSSEN unterscheidbar sein.
8. Forced Algorithms DÜRFEN Hard Constraints nicht umgehen.
9. Adaptive Auswahl MUSS System Policy berücksichtigen.
10. Discovery und Auswahl DÜRFEN keine Authority erzeugen.
11. Benötigte Ressourcen MÜSSEN separat capability-basiert autorisiert werden.
12. Algorithmus, Version, Implementierungen und Auswahlgrund MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-REGISTRY-0001`
- `NPSPEC-REGISTRY-CAPABILITY-0001`
- `NPSPEC-REGISTRY-TYPE-0001`
- `NPSPEC-SYSTEM-POLICY-0001`
- `NPSPEC-SYSTEM-RESOURCES-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann Algorithmen unabhängig von ihrer konkreten Implementierung registrieren, finden und anhand von Execution Contract, Ressourcen, Hardware, Trust und Policy auswählen. Dadurch können unterschiedliche CPU-, GPU- oder Accelerator-Implementierungen transparent genutzt und optimiert werden, ohne Anwendungen fest an eine bestimmte Implementierung zu koppeln.