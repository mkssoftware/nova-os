# ADR-ARCH-0020 – KI nicht als Korrektheitsvoraussetzung des Kernels

## Status

Angenommen

## Kategorie

Systemarchitektur / Kernel / KI / Sicherheit / Korrektheit / Resilienz

## Kontext

NovaOS verwendet KI und adaptive Systeme bewusst als wichtigen Bestandteil der Gesamtarchitektur.

KI kann insbesondere unterstützen bei:

- Provider-Auswahl,
- Algorithmus-Auswahl,
- Ressourcenoptimierung,
- Vorhersage von Nutzerverhalten,
- Preloading,
- Scheduling-Hinweisen,
- Fehlerdiagnose,
- Anomalieerkennung,
- Self-Healing,
- Performanceoptimierung,
- Energieoptimierung,
- Systemplanung.

Diese Fähigkeiten dürfen jedoch nicht dazu führen, dass die grundlegende Korrektheit des Kernels von probabilistischen Modellen abhängt.

Ein Kernel muss auch dann korrekt funktionieren, wenn:

```text
AI unavailable
```

```text
AI model corrupted
```

```text
AI prediction wrong
```

```text
AI service disabled
```

```text
AI accelerator unavailable
```

```text
AI model incompatible
```

```text
AI output rejected
```

```text
AI confidence insufficient
```

gilt.

Grundlegende Kernelmechanismen müssen deterministisch spezifizierte und überprüfbare Fallback-Pfade besitzen.

Dazu zählen insbesondere:

- Speicherverwaltung,
- Prozess- und Threadverwaltung,
- Scheduling-Grundmechanismen,
- Interrupt- und Exception-Verarbeitung,
- Synchronisation,
- Capability Enforcement,
- Isolation,
- IPC-Grundmechanismen,
- Ressourcenabrechnung,
- Timer und Zeitbasis,
- Treiberisolation,
- Systemstart,
- Shutdown,
- Fehlerbehandlung,
- Recovery-Grundfunktionen.

KI darf Entscheidungen optimieren.

KI darf aber nicht die Voraussetzung dafür sein, dass diese Mechanismen überhaupt korrekt funktionieren.

---

## Entscheidung

NovaOS MUSS die grundlegende Kernelkorrektheit vollständig unabhängig von KI gewährleisten.

Das zentrale Architekturprinzip lautet:

```text
AI may optimize correctness-preserving choices.

AI must never be required
to establish correctness itself.
```

Der Kernel muss jederzeit über einen:

```text
Non-AI Correctness Path
```

verfügen.

Das Grundmodell lautet:

```text
Kernel Mechanism
      │
      ▼
Deterministic Correct Baseline
      │
      ├── optional AI Advice
      │         ↓
      │    validated optimization
      │
      ▼
Correct Execution
```

Nicht zulässig ist:

```text
Kernel Operation
      │
      ▼
AI Decision
      │
      ▼
Correctness
```

wenn ohne die KI-Entscheidung keine definierte korrekte Ausführung existiert.

---

## Korrektheitsbasis

Der Kernel benötigt für jede fundamentale Operation eine algorithmisch definierte Baseline.

Beispiele:

```text
Scheduler:
    deterministic baseline scheduler
```

```text
Memory Allocation:
    conventional verified allocator
```

```text
Provider Selection:
    static policy fallback
```

```text
Recovery:
    predefined recovery state machine
```

KI darf diese Baseline verbessern oder alternative zulässige Entscheidungen vorschlagen.

---

## KI als Advisor

KI soll im Kernelkontext grundsätzlich nach dem Modell:

```text
Advisor
```

statt:

```text
Authority
```

behandelt werden.

Beispiel:

```text
AI Recommendation:
    migrate task to CPU 3
```

Der Kernel prüft anschließend:

```text
CPU available?
Affinity valid?
Security valid?
Resource budget valid?
Realtime constraints valid?
Determinism valid?
```

Erst danach kann die Empfehlung umgesetzt werden.

---

## Validierung von KI-Ausgaben

KI-Ausgaben müssen vor Verwendung gegen deterministische Regeln und Contracts geprüft werden.

Konzeptionell:

```text
AI Proposal
    ↓
Validation
    ↓
Allowed?
 ┌──┴──┐
 │     │
Yes    No
 │     │
 ▼     ▼
Use   Ignore
```

Eine KI-Ausgabe darf keine bestehende Kernelinvariante außer Kraft setzen.

---

## Kernelinvarianten

Zu den nicht durch KI veränderbaren Invarianten gehören insbesondere:

```text
Memory Safety
Isolation
Capability Enforcement
Privilege Separation
Ownership Rules
Synchronization Correctness
Resource Ownership
Interrupt Safety
Kernel Object Integrity
Security Boundaries
```

KI darf diese Invarianten weder direkt noch indirekt abschwächen.

---

## Scheduling

KI darf Scheduling optimieren.

Beispiele:

- Workload-Prognose,
- Core-Auswahl,
- Cache-Lokalität,
- Energieoptimierung,
- Vorhersage zukünftiger Last.

Der Scheduler muss jedoch ohne KI vollständig funktionsfähig bleiben.

Konzeptionell:

```text
Baseline Scheduler
      +
Optional AI Hint
      ↓
Validated Scheduling Decision
```

Bei Ausfall der KI:

```text
AI unavailable
      ↓
Baseline Scheduler continues
```

---

## Memory Management

KI darf Speicherverhalten analysieren und beispielsweise Hinweise liefern für:

- Preloading,
- Page Placement,
- NUMA Placement,
- Cache-Größen,
- Reclamation-Reihenfolgen.

Die grundlegende Speicherverwaltung darf jedoch nicht davon abhängen.

Insbesondere müssen:

- PMM,
- VMM,
- Page Fault Handling,
- Allocation,
- Mapping,
- Protection,
- Reclamation

auch ohne KI korrekt funktionieren.

---

## Resource Economy

Diese ADR ergänzt `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`.

KI darf Ressourcenbedarf vorhersagen.

Beispiel:

```text
Predicted Memory Need
Predicted CPU Load
Predicted GPU Demand
```

Admission Control und Hard Resource Limits müssen jedoch durch deterministische Regeln durchgesetzt werden.

Eine KI-Prognose darf keine Resource Limits aufheben.

---

## Capability Enforcement

Capability Security darf niemals von KI-Entscheidungen abhängen.

Nicht zulässig:

```text
AI thinks access is probably okay
      ↓
Allow
```

Zulässig:

```text
Capability valid
      ↓
Policy valid
      ↓
Allow
```

KI darf höchstens unterstützen bei:

- Anomalieerkennung,
- Missbrauchserkennung,
- Risikohinweisen.

Diese Hinweise können zusätzliche Einschränkungen auslösen, dürfen aber keine fehlende Autorisierung ersetzen.

---

## Nova.Trust

KI kann Trust-Analysen unterstützen.

Die fundamentale Trust-Validierung muss jedoch auf überprüfbaren Mechanismen beruhen, beispielsweise:

```text
Identity
Signature
Provenance
Policy
Revocation
```

Ein KI-Modell darf nicht allein entscheiden:

```text
This binary looks trustworthy.
```

und daraus Kernelautorität erzeugen.

---

## Interrupts und Exceptions

Interrupt- und Exception-Behandlung muss vollständig deterministisch und ohne KI funktionieren.

KI darf nicht im kritischen Pfad erforderlich sein für:

- Interrupt Dispatch,
- Exception Classification,
- Fault Isolation,
- Timer Handling,
- Context Switches.

---

## Synchronisation

Locking, Atomics, Memory Ordering und andere Synchronisationsmechanismen dürfen keine KI-Entscheidung zur Herstellung ihrer Korrektheit benötigen.

KI darf beispielsweise Lock-Contention analysieren, aber nicht die Semantik von Synchronisationsprimitive verändern.

---

## IPC

Grundlegendes IPC muss ohne KI funktionieren.

KI darf:

- Routing optimieren,
- Batching vorschlagen,
- Providerwahl unterstützen.

Nicht KI-abhängig sein dürfen:

- Message Validation,
- Capability Checks,
- Ownership,
- Lifetime,
- Security,
- Delivery Semantics.

---

## Treiber

Treiber dürfen KI zur Optimierung verwenden, wenn ein funktionaler nicht-KI-basierter Pfad existiert.

Beispiele:

```text
Power Management Prediction
I/O Prefetch
Adaptive Queue Depth
```

Fundamentale Device Control und sichere Fehlerbehandlung müssen jedoch ohne KI funktionieren.

---

## Boot

NovaOS muss ohne KI bootfähig sein.

Der Bootprozess darf nicht voraussetzen:

- ein geladenes ML-Modell,
- einen NPU-Treiber,
- Netzwerkzugriff,
- Cloud-KI,
- einen AI Service.

Falls KI-Komponenten nicht verfügbar sind:

```text
Boot
  ↓
Normal deterministic initialization
  ↓
System operational
```

---

## Recovery

Recovery muss ebenfalls ohne KI funktionieren.

NovaDOS und andere Recovery-Pfade müssen statische, überprüfbare Mechanismen besitzen.

KI kann Diagnosen oder Reparaturvorschläge liefern, aber Recovery darf nicht von diesen abhängen.

---

## Self-Healing

KI kann Self-Healing unterstützen durch:

- Fehlerklassifikation,
- Mustererkennung,
- Ursachenwahrscheinlichkeit,
- Auswahl von Reparaturkandidaten.

Die tatsächlichen Recovery-Aktionen müssen jedoch durch:

- Contracts,
- Policies,
- Capabilities,
- Transaktionen,
- Fail-Safe Defaults

begrenzt werden.

KI darf keine irreversible Reparatur allein aufgrund einer probabilistischen Einschätzung autorisieren.

---

## Graceful Degradation

Wenn KI nicht verfügbar ist, darf NovaOS auf einen nicht-KI-basierten Betriebsmodus degradieren.

Beispiel:

```text
AI Optimized Scheduler
        ↓
AI unavailable
        ↓
Baseline Scheduler
```

oder:

```text
AI Provider Selection
        ↓
unavailable
        ↓
Static Policy Selection
```

Dies ist ein normaler Graceful-Degradation-Pfad und kein Kernel Failure.

---

## Deterministic Mode

Diese ADR ergänzt `ADR-ARCH-0011_Deterministic_Mode`.

Im Deterministic Mode können KI-basierte Optimierungen vollständig deaktiviert werden, sofern sie nicht selbst deterministisch reproduzierbar und ausdrücklich zugelassen sind.

Der Kernel muss auch in diesem Zustand vollständig funktionsfähig bleiben.

---

## Fail-Safe Defaults

Diese ADR ergänzt `ADR-ARCH-0016_Fail_Safe_Defaults`.

Wenn eine KI-Ausgabe:

```text
Unknown
Invalid
Unavailable
LowConfidence
```

ist, gilt:

```text
Use deterministic safe fallback
```

Nicht:

```text
Guess
```

---

## Prediction Error

NovaOS darf Prediction Error verwenden:

```text
Prediction
    ↓
Observed Result
    ↓
Error
    ↓
Model Adjustment
```

Das Lernsignal darf Optimierungsmodelle verändern.

Es darf keine Kernelinvarianten verändern.

---

## AI Confidence

Confidence Scores dürfen für Optimierungsentscheidungen berücksichtigt werden.

Sie dürfen jedoch keine Hard Contracts ersetzen.

Beispiel:

```text
Confidence = 99.9%
```

ist keine Autorisierung und kein Beweis für Korrektheit.

---

## AI Model Lifecycle

KI-Modelle müssen als austauschbare Komponenten behandelt werden.

Ein Modell kann:

```text
Load
Unload
Update
Replace
Disable
Fail
```

ohne dass grundlegende Kernelkorrektheit verloren geht.

---

## Hot Replacement

KI-Modelle dürfen zur Laufzeit ersetzt werden.

Ein fehlgeschlagenes Model Update muss auf:

```text
Previous Model
```

oder:

```text
Non-AI Baseline
```

zurückfallen können.

---

## NPU-Abhängigkeit

Eine NPU darf ein Optimierungsziel sein, aber keine Voraussetzung für Kernelkorrektheit.

```text
NPU unavailable
```

darf nicht bedeuten:

```text
Kernel cannot operate correctly
```

---

## Netzwerk- und Cloud-Unabhängigkeit

Der Kernel darf keine externe KI-Infrastruktur für seine Korrektheit benötigen.

Insbesondere müssen fundamentale Operationen funktionieren bei:

```text
No Network
No Cloud
No AI Service
```

---

## KI und Realtime

KI darf nicht in Hard-Realtime-Pfade eingeführt werden, wenn ihre Worst-Case-Ausführungszeit nicht mit dem Contract vereinbar ist.

Bei Hard Realtime muss eine garantierbare Baseline existieren.

---

## KI und Safety

Safety-kritische Kernelmechanismen dürfen keine probabilistische KI-Ausgabe als alleinige Entscheidungsgrundlage verwenden.

KI kann zusätzliche Diagnoseinformationen bereitstellen.

Die finale Safety-Reaktion muss über deterministische, verifizierbare Regeln abgesichert sein.

---

## KI und Security

Security-kritische Enforcement-Entscheidungen benötigen explizite Regeln.

KI kann beispielsweise erkennen:

```text
Suspicious behavior
```

und eine zusätzliche Prüfung oder Isolation auslösen.

Sie darf jedoch nicht eigenständig fehlende Capabilities erzeugen oder Rechte vergeben.

---

## Architekturtrennung

Konzeptionell trennt NovaOS:

```text
Kernel Correctness Plane
        │
        └── deterministic mechanisms

Optimization Plane
        │
        ├── heuristics
        ├── statistics
        └── AI
```

Das Optimization Plane darf das Correctness Plane beeinflussen, aber nur über validierte Schnittstellen.

---

## Control Boundary

Der Übergang muss explizit sein:

```text
AI / Adaptive System
        ↓
Proposal
        ↓
Kernel Validation Boundary
        ↓
Mechanism
```

Dies verhindert, dass probabilistische Entscheidungen direkt privilegierte Kerneloperationen steuern.

---

## Architecture Introspection

KI-Einfluss auf Kernelentscheidungen muss introspektierbar sein.

Beispiel:

```text
SchedulerDecision {
    baselineCandidate = CPU1
    aiSuggestion = CPU3
    selected = CPU3
    validation = Passed
}
```

oder:

```text
aiSuggestion = GPU1
validation = Rejected
reason = ResourceBudgetViolation
```

---

## Fehlerzustände

KI-bezogene Fehler sollen von normalen Kernelmechanismusfehlern unterscheidbar sein.

Beispiele:

```text
AIUnavailable
AIModelInvalid
AIModelIncompatible
AIInferenceTimeout
AIConfidenceInsufficient
AIProposalRejected
```

Diese Fehler dürfen nicht automatisch zu Kernel Panic führen.

---

## Normative Anforderungen

1. NovaOS MUSS grundlegende Kernelkorrektheit ohne KI gewährleisten.
2. Fundamentale Kernelmechanismen MÜSSEN einen nicht-KI-basierten korrekten Pfad besitzen.
3. KI DARF Kernelentscheidungen optimieren.
4. KI DARF Kernelinvarianten NICHT definieren.
5. KI DARF fehlende Autorisierung NICHT ersetzen.
6. KI-Ausgaben MÜSSEN vor privilegierter Verwendung validiert werden.
7. Memory Safety DARF NICHT von KI abhängen.
8. Isolation DARF NICHT von KI abhängen.
9. Capability Enforcement DARF NICHT von KI abhängen.
10. Synchronisationskorrektheit DARF NICHT von KI abhängen.
11. Interrupt- und Exception-Behandlung DARF NICHT von KI abhängen.
12. IPC-Grundkorrektheit DARF NICHT von KI abhängen.
13. Speicherverwaltung MUSS ohne KI funktionieren.
14. Scheduling MUSS ohne KI funktionieren.
15. Resource Limits MÜSSEN ohne KI durchsetzbar sein.
16. Boot MUSS ohne KI möglich sein.
17. Recovery MUSS ohne KI möglich sein.
18. NovaDOS DARF keine KI für fundamentale Recovery benötigen.
19. Self-Healing DARF KI verwenden, MUSS aber deterministische Sicherheitsgrenzen besitzen.
20. KI DARF keine irreversible Recovery-Aktion allein autorisieren.
21. KI-Ausfall MUSS Graceful Degradation auf einen sicheren Baseline-Pfad ermöglichen.
22. Deterministic Mode MUSS ohne KI vollständig funktionsfähig sein.
23. KI-Confidence DARF Hard Contracts NICHT ersetzen.
24. Prediction Error DARF Optimierungsmodelle ändern, aber keine Kernelinvarianten.
25. KI-Modelle MÜSSEN austauschbar und deaktivierbar sein.
26. Ein fehlerhaftes KI-Modell DARF grundlegende Kernelkorrektheit NICHT beeinträchtigen.
27. Eine NPU DARF keine Voraussetzung für Kernelkorrektheit sein.
28. Netzwerkzugriff DARF keine Voraussetzung für Kernelkorrektheit sein.
29. Cloud-KI DARF keine Voraussetzung für Kernelkorrektheit sein.
30. Hard-Realtime-Pfade MÜSSEN ohne nicht garantierbare KI-Abhängigkeit funktionieren.
31. Safety-kritische Entscheidungen MÜSSEN deterministisch abgesichert sein.
32. Security Enforcement MUSS deterministisch abgesichert sein.
33. KI DARF keine neuen Capabilities oder Privilegien erzeugen.
34. KI-Vorschläge MÜSSEN eine explizite Kernel-Validierungsgrenze passieren.
35. Abgelehnte KI-Vorschläge DÜRFEN die Baseline-Ausführung NICHT verhindern.
36. KI-Timeout MUSS auf einen definierten nicht-KI-Pfad zurückfallen.
37. KI-bezogene Fehler SOLLEN explizit typisiert sein.
38. KI-bezogene Fehler DÜRFEN NICHT standardmäßig Kernel Panic auslösen.
39. Architecture Introspection SOLL KI-Einfluss auf relevante Kernelentscheidungen darstellen können.
40. NovaOS MUSS KI als optionale Optimierungsschicht und nicht als Korrektheitsfundament des Kernels behandeln.

---

## Konsequenzen

### Positive Konsequenzen

- NovaOS bleibt auch ohne KI vollständig funktionsfähig.
- Kernelkorrektheit bleibt überprüfbar.
- KI-Fehler können lokal begrenzt werden.
- NPU- oder Cloud-Ausfälle gefährden den Kernel nicht.
- Deterministic Mode bleibt möglich.
- Recovery bleibt robust.
- KI-Modelle können frei aktualisiert oder ersetzt werden.
- adaptive Optimierung kann aggressiver weiterentwickelt werden, weil eine sichere Baseline existiert.
- Security und Safety bleiben von probabilistischen Entscheidungen getrennt.

### Negative Konsequenzen

- für KI-optimierte Mechanismen muss zusätzlich eine Baseline implementiert werden.
- manche Logik existiert sowohl als statischer als auch adaptiver Pfad.
- KI kann nicht beliebig tief in kritische Kernelmechanismen integriert werden.
- jede KI-Empfehlung benötigt geeignete Validierungsgrenzen.
- maximale Performance kann zugunsten klarer Korrektheitsgrenzen eingeschränkt werden.

---

## Verworfene Alternativen

### KI als zentraler Kernel-Scheduler

Als alleiniger Scheduling-Mechanismus verworfen.

KI darf Scheduling optimieren, aber ein korrekter Baseline-Scheduler muss bestehen bleiben.

### KI als Sicherheitsautorität

Verworfen.

Probabilistische Bewertung ersetzt keine Capability- oder Policy-Prüfung.

### KI als einzige Self-Healing-Logik

Verworfen.

Recovery benötigt statische und überprüfbare Sicherheitsgrenzen.

### Cloud-KI als Kernelbestandteil

Verworfen.

Netzwerk- und Cloud-Verfügbarkeit dürfen keine Voraussetzung für Kernelkorrektheit sein.

### NPU als notwendige Kernelhardware

Verworfen.

NovaOS muss auch auf Systemen ohne NPU korrekt funktionieren.

### KI vollständig aus dem Kernelumfeld ausschließen

Ebenfalls verworfen.

KI kann für Scheduling, Resource Economy, Prefetching, Diagnose und Optimierung erheblichen Nutzen bieten, solange sie nicht zur Korrektheitsvoraussetzung wird.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starken_Isolationsgrenzen`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0010_Structured_Concurrency_als_systemweites_Ausführungsmodell`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts_zwischen_Systemkomponenten`
- `ADR-ARCH-0015_Graceful_Degradation_als_Systemprinzip`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvierbare_Systemarchitektur`
- `ADR-EXECUTION-0001_Nova_ExecutionContract`
- `ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur`
- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`

---

## Zugehörige NPSPECs

- `NPSPEC-KERNEL-AI-BOUNDARY-0001`
- `NPSPEC-KERNEL-NONAI-BASELINE-0001`
- `NPSPEC-KERNEL-AI-VALIDATION-0001`
- `NPSPEC-KERNEL-AI-FALLBACK-0001`
- `NPSPEC-KERNEL-AI-INTROSPECTION-0001`
- `NPSPEC-SCHEDULER-AI-ADVISOR-0001`
- `NPSPEC-SCHEDULER-BASELINE-0001`
- `NPSPEC-MEMORY-AI-ADVISOR-0001`
- `NPSPEC-RESOURCE-AI-ADVISOR-0001`
- `NPSPEC-SECURITY-AI-BOUNDARY-0001`
- `NPSPEC-TRUST-AI-BOUNDARY-0001`
- `NPSPEC-RESILIENCE-AI-ADVISOR-0001`
- `NPSPEC-AI-KERNEL-PROPOSAL-0001`
- `NPSPEC-AI-KERNEL-CONFIDENCE-0001`
- `NPSPEC-AI-KERNEL-FAILURE-0001`
- `NPSPEC-AI-MODEL-LIFECYCLE-0001`
- `NPSPEC-AI-MODEL-HOTREPLACE-0001`
- `NPSPEC-AI-PREDICTIONERROR-0001`

---

## Verifikation

Die Umsetzung muss insbesondere prüfen:

- Boot ohne KI-Komponenten,
- Boot ohne NPU,
- Boot ohne Netzwerk,
- Scheduling bei vollständigem AI-Ausfall,
- Memory Management ohne AI Service,
- Resource Economy ohne Prediction Model,
- Timeout einer KI-Inferenz,
- fehlerhaftes oder inkompatibles KI-Modell,
- abgelehnte AI Scheduler Recommendation,
- AI Proposal mit verletztem Resource Budget,
- AI Proposal mit verletzter Capability Policy,
- Self-Healing ohne KI,
- Recovery ohne Netzwerk und KI,
- Deterministic Mode mit deaktivierter KI,
- Hot Replacement eines KI-Modells,
- Ausfall eines AI Providers während laufender Optimierung.

In allen Fällen muss der Kernel weiterhin einen definierten korrekten Betriebszustand besitzen.

---

## Ergebnis

NovaOS verwendet KI intensiv, aber nicht als Fundament seiner grundlegenden Korrektheit.

Die Architektur lautet:

```text
Correct Kernel Baseline
        │
        ├── Heuristics
        ├── Statistics
        └── AI
             │
             ▼
        Optimization Proposal
             │
             ▼
      Deterministic Validation
             │
             ▼
        Kernel Mechanism
```

Damit gilt:

```text
No AI:
    Kernel remains correct.

Wrong AI:
    Kernel remains correct.

Unavailable AI:
    Kernel remains correct.
```

Die abschließende Architekturregel lautet:

```text
AI may make NovaOS smarter.

AI must not be required
to make the kernel correct.
```