# ADR-IPC-0007 – Causation und Tracing über IPC-Grenzen

## Status

Angenommen

## Kategorie

IPC / Causation / Tracing / Observability / Structured Concurrency

## Kontext

Ein logischer Vorgang in NovaOS kann mehrere Execution Domains, Services, I/O-Operationen und entfernte Provider durchlaufen.

Ohne gemeinsame Kausalitätsinformationen erscheinen diese Aktivitäten als voneinander unabhängige Ereignisse. Fehler, Latenzen, Ressourcenverbrauch und Abhängigkeiten lassen sich dann nur schwer dem ursprünglichen Auslöser zuordnen.

NovaOS benötigt deshalb eine systemweite Möglichkeit, Ursache-Wirkungs-Beziehungen über IPC-Grenzen hinweg zu erhalten.

---

## Entscheidung

NovaOS propagiert **Causation- und Tracing-Kontext über IPC-Grenzen**.

```text
Originating Operation
        ↓
      Call A
        ↓
      IPC
        ↓
      Call B
        ↓
      I/O
```

Zusammengehörige Operationen bleiben dadurch als kausale Ausführungskette nachvollziehbar.

Tracing ist dabei eine Beobachtungsfunktion; Causation beschreibt die tatsächliche logische Herkunft einer Operation.

---

## Grundprinzip

```text
Execution may cross boundaries.

Causation must not be lost.
```

---

## Causation Context

Eine weitergeleitete Operation kann einen Causation Context besitzen.

Dieser identifiziert die logische Herkunft beziehungsweise Vorgängerbeziehung einer Operation.

Konzeptionell entsteht:

```text
Operation A
    ↓ causes
Operation B
    ↓ causes
Operation C
```

Die Causation Identity darf nicht von Prozess-ID, Thread-ID oder virtueller Adresse abhängig sein.

---

## Trace Context

Zusätzlich kann ein Trace Context verwendet werden, um zusammengehörige Aktivitäten einer Ausführung zu beobachten.

```text
Trace
 ├── Call A
 ├── IPC B
 ├── Service C
 └── I/O D
```

Causation und Trace Context können miteinander verbunden sein, bleiben aber logisch unterschiedliche Konzepte.

---

## Propagation

Causation- und Trace-Kontext sollen automatisch durch die nativen IPC- und Unified-Call-Abstraktionen propagiert werden können.

Dies gilt insbesondere für:

- Typed IPC,
- Unified Calls,
- lokale IPC,
- entfernte Calls,
- asynchrone Operationen,
- nachgelagerte I/O-Requests.

Manuelle Weitergabe durch jede Anwendung soll für normale Systemkommunikation nicht erforderlich sein.

---

## Structured Concurrency

Causation wird mit Structured Concurrency verbunden.

Task Groups, Child Tasks und daraus ausgelöste IPC-Operationen können dadurch einer gemeinsamen Ausführungsstruktur zugeordnet werden.

```text
Task Scope
   ├── Task
   │    └── IPC
   │         └── Service Task
   └── Task
```

Causation ersetzt dabei nicht die Lifecycle- und Cancellation-Semantik von Structured Concurrency.

---

## Asynchrone Kommunikation

Asynchronität darf die Kausalitätsbeziehung nicht aufheben.

Ein Request und seine spätere Completion müssen weiterhin dem auslösenden Vorgang zugeordnet werden können.

Dies gilt auch dann, wenn die Verarbeitung:

- zeitversetzt,
- auf einem anderen Thread,
- in einer anderen Execution Domain,
- auf einem entfernten Provider

erfolgt.

---

## Ressourcen und Performance

Causation Context kann als Grundlage dienen, Ressourcenverbrauch und Latenz einer über mehrere Komponenten verteilten Operation zuzuordnen.

```text
User Action
    ↓
Service Call
    ↓
IPC
    ↓
Storage I/O

Total causal cost
```

Damit ergänzt Causation die systemweite Resource Economy und Performance-Analyse.

---

## Security und Datenschutz

Tracing darf keine neue Authority erzeugen.

Ein übertragener Trace- oder Causation-Identifier berechtigt nicht zum Zugriff auf andere Ressourcen oder Services.

Tracing-Daten können sicherheits- oder datenschutzrelevante Informationen enthalten und müssen deshalb kontrolliert zugänglich sein.

Über Trust- oder Data-Sovereignty-Grenzen dürfen nur zulässige Kontextinformationen propagiert werden.

---

## Fehlerdiagnose

Fehler können mit ihrer kausalen Ausführungskette verbunden werden.

Dadurch kann NovaOS beispielsweise nachvollziehen:

```text
Original Request
      ↓
Service A
      ↓
Service B
      ↓
Failed I/O
```

Dies unterstützt Architecture Introspection, Debugging und systemweite Fehleranalyse.

---

## Normative Anforderungen

1. NovaOS MUSS Causation Context über IPC-Grenzen propagieren können.
2. Causation Identities MÜSSEN unabhängig von Prozess-, Thread- und virtuellen Adressidentitäten sein.
3. NovaOS MUSS zusammengehörige Operationen über einen Trace Context korrelieren können.
4. Causation und Tracing MÜSSEN logisch getrennte Konzepte bleiben.
5. Native IPC- und Unified-Call-Abstraktionen SOLLEN Kontext automatisch propagieren.
6. Asynchrone Ausführung DARF die kausale Zuordnung NICHT verlieren.
7. Request und Completion MÜSSEN demselben kausalen Vorgang zuordenbar sein können.
8. Causation MUSS mit Structured Concurrency integrierbar sein.
9. Causation MUSS über lokale und entfernte Calls hinweg erhalten werden können.
10. Nachgelagerte I/O-Operationen MÜSSEN Causation Context übernehmen können.
11. Causation MUSS für Ressourcen- und Latenzanalyse nutzbar sein können.
12. Trace- oder Causation-Informationen DÜRFEN keine Authority verleihen.
13. Security-, Privacy- und Data-Sovereignty-Regeln MÜSSEN die Propagation einschränken können.
14. Causation- und Trace-Beziehungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- systemweite Ursache-Wirkungs-Analyse,
- bessere Diagnose verteilter Fehler,
- End-to-End-Latenzen werden nachvollziehbar,
- Ressourcenverbrauch kann logischen Vorgängen zugeordnet werden,
- asynchrone und verteilte Ausführung bleibt beobachtbar,
- bessere Grundlage für Architecture Introspection.

### Negative Konsequenzen

- zusätzlicher Kontext muss über Kommunikationsgrenzen propagiert werden,
- Trace-Daten benötigen Zugriffsschutz,
- sehr große Kausalitätsgraphen benötigen kontrollierte Aufbewahrung.

---

## Verworfene Alternativen

### Tracing ausschließlich pro Prozess

Verworfen.

Kausale Vorgänge überschreiten regelmäßig Execution-Domain-Grenzen.

### Korrelation ausschließlich über Zeitstempel

Verworfen.

Zeitliche Nähe beweist keine Ursache-Wirkungs-Beziehung.

### Trace Identifier als Security Authority

Verworfen.

Observability-Kontext und Zugriffsberechtigung müssen strikt getrennt bleiben.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-IPC-0001_Typed_IPC`
- `ADR-IPC-0004_Unified_Call_Abstraction`
- `ADR-IPC-0005_Lokale_und_entfernte_Aufrufe_unter_einem_Contract`
- `ADR-IPC-0006_Backpressure_als_systemweite_IPC_Eigenschaft`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-IO-0001_Completion_Based_Asynchronous_IO`

---

## Zugehörige NPSPECs

- `NPSPEC-IPC-CAUSATION-0001`
- `NPSPEC-IPC-TRACE-CONTEXT-0001`
- `NPSPEC-IPC-CONTEXT-PROPAGATION-0001`
- `NPSPEC-IPC-TRACE-SECURITY-0001`
- `NPSPEC-IPC-TRACE-RESOURCE-0001`
- `NPSPEC-IPC-CAUSATION-TEST-0001`

---

## Ergebnis

NovaOS erhält Ursache-Wirkungs-Beziehungen auch über asynchrone IPC- und Servicegrenzen hinweg:

```text
Origin
  ↓
Task
  ↓
IPC
  ↓
Service
  ↓
I/O
  ↓
Completion
```

Dadurch können verteilte Systemaktivitäten als zusammenhängende kausale Ausführung verstanden, analysiert und kontrolliert introspektiert werden.