# ADR-IPC-0004 – Unified Call Abstraction

## Status

Angenommen

## Kategorie

IPC / Communication / Location Transparency / Typed IPC / Call Model

## Kontext

NovaOS-Komponenten können Funktionen innerhalb derselben Execution Domain, über IPC-Grenzen oder über andere Systemprovider aufrufen.

Wenn für jeden Kommunikationspfad ein vollständig anderes Programmiermodell verwendet wird, müssen Anwendungen unnötig zwischen lokalen Funktionsaufrufen, IPC, Services und möglicherweise entfernten Providern unterscheiden.

NovaOS besitzt bereits Typed IPC, Capability-Based IPC und Location Transparency. Darauf aufbauend soll eine gemeinsame logische Aufrufabstraktion entstehen, ohne die tatsächlichen Kosten und Fehlergrenzen verschiedener Ausführungsorte zu verstecken.

---

## Entscheidung

NovaOS definiert eine **Unified Call Abstraction** für typisierte, contract-basierte Aufrufe.

```text
Caller
  ↓
Typed Call Contract
  ↓
Call Resolution
  ├── Local
  ├── IPC
  └── Remote / Provider
  ↓
Callee
```

Der logische Call Contract bleibt unabhängig vom konkreten Transport.

Die Ausführungssemantik darf jedoch Unterschiede bei Latenz, Fehlern, Ressourcenverbrauch und Verfügbarkeit nicht verschleiern.

---

## Grundprinzip

```text
Unify the call model,
not the physical cost model.
```

---

## Call Contract

Ein Unified Call wird durch einen expliziten Contract beschrieben.

Dieser kann insbesondere festlegen:

- Operation Identity,
- Input Types,
- Output Types,
- Error Semantics,
- Authority Requirements,
- Deadline,
- Cancellation,
- Resource Budget.

Damit bleibt die Bedeutung eines Calls unabhängig davon erhalten, wo die Implementierung ausgeführt wird.

---

## Lokale und entfernte Ausführung

Eine Capability kann lokal oder über eine andere Execution Domain bereitgestellt werden.

```text
Call
 ↓
Resolver
 ├── Same Domain
 ├── Other Execution Domain
 └── External Provider
```

Der Aufrufer soll nicht für jede Platzierung eine vollständig andere API benötigen.

Die tatsächliche Platzierung bleibt jedoch introspektierbar.

---

## Typed IPC Integration

Calls über Execution-Domain-Grenzen verwenden das Typed-IPC-Modell.

```text
Unified Call
     ↓
Typed IPC Contract
     ↓
IPC Transport
```

Input-, Output- und Fehlersemantik bleiben dadurch über die Isolationsgrenze hinweg erhalten.

---

## Capability-Based Authority

Ein Call darf nur ausgeführt werden, wenn der Aufrufer die erforderliche Authority besitzt.

```text
Call Contract
      +
Capability
      ↓
Authorized Call
```

Call Resolution darf keine Security-Grenzen umgehen.

Eine lokale Implementierung erhält nicht automatisch schwächere Autorisierungsanforderungen als eine entfernte.

---

## Asynchronität

Die Unified Call Abstraction muss mit dem nativen Async-/Await- und Structured-Concurrency-Modell zusammenarbeiten.

Ein Call kann unabhängig von seiner aktuellen Platzierung asynchron ausführbar sein.

Cancellation und Deadlines sollen entlang des Calls propagiert werden können.

---

## Location Transparency

NovaOS unterstützt Location Transparency, aber keine Illusion identischer Kosten.

Ein entfernter Call kann zusätzliche Eigenschaften besitzen:

- höhere Latenz,
- zusätzliche Ressourcenbindung,
- Transportfehler,
- Provider-Ausfall,
- eingeschränkte Verfügbarkeit.

Diese Unterschiede müssen über Contracts, Fehlersemantik und Introspection sichtbar bleiben.

---

## Data Movement

Die Unified Call Abstraction schreibt keinen bestimmten Datentransport vor.

Je nach Call können Daten über:

- direkte Werte,
- Typed Messages,
- Shared Objects,
- Shared Buffers,
- Zero-Copy IPC

übertragen werden.

Die Data-Movement-Strategie kann unabhängig vom logischen Call Contract optimiert werden.

---

## Provider Resolution

Mehrere Provider können denselben logischen Contract implementieren.

```text
Call Contract
      ↓
Eligible Providers
      ↓
Policy / Constraints
      ↓
Selected Provider
```

Hard Constraints müssen vor Optimierungszielen ausgewertet werden.

Eine explizite Provider-Auswahl darf möglich sein, sofern sie keine Hard Constraints verletzt.

---

## Normative Anforderungen

1. NovaOS MUSS eine einheitliche Call Abstraction für geeignete System- und Serviceaufrufe bereitstellen.
2. Calls MÜSSEN über explizite typisierte Contracts beschreibbar sein.
3. Der logische Call Contract SOLL unabhängig vom konkreten Transport bleiben.
4. Lokale und IPC-basierte Implementierungen SOLLEN denselben logischen Contract bereitstellen können.
5. Unified Calls über Execution-Domain-Grenzen MÜSSEN in Typed IPC integrierbar sein.
6. Call Authority MUSS über das Capability-Modell kontrollierbar sein.
7. Lokale Ausführung DARF Security-Anforderungen NICHT umgehen.
8. Cancellation und Deadlines MÜSSEN über Calls propagierbar sein.
9. Unified Calls MÜSSEN mit Structured Concurrency und Async/Await kombinierbar sein.
10. Location Transparency DARF unterschiedliche Kosten und Fehlergrenzen NICHT verschleiern.
11. Data-Movement-Strategien MÜSSEN unabhängig vom logischen Call Contract optimierbar sein.
12. Mehrere Provider MÜSSEN denselben Call Contract implementieren können.
13. Provider-Auswahl MUSS Hard Constraints vor Soft Constraints und Optimierungszielen berücksichtigen.
14. Tatsächlicher Provider, Ausführungsort und relevante Call-Eigenschaften MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- einheitliches Programmiermodell für lokale und IPC-basierte Services,
- geringere Kopplung zwischen API und Ausführungsort,
- natürliche Integration mit Location Transparency,
- Provider können ausgetauscht oder verschoben werden,
- Typed IPC und Capability Security bleiben erhalten.

### Negative Konsequenzen

- lokale und entfernte Calls dürfen semantisch nicht fälschlich als kostenidentisch erscheinen,
- Provider Resolution benötigt klare Contracts,
- Fehlersemantik muss unterschiedliche Ausführungsorte berücksichtigen.

---

## Verworfene Alternativen

### Separate APIs für lokale und IPC-Aufrufe

Verworfen.

Dies würde Anwendungen unnötig an die aktuelle Platzierung einer Implementierung koppeln.

### Vollständige Transparenz aller Remote-Eigenschaften

Verworfen.

Latenz, Fehlergrenzen und Ressourcenverbrauch dürfen nicht verborgen werden.

### Unified Calls ohne explizite Contracts

Verworfen.

Ohne Typ-, Fehler- und Authority-Semantik würde die Abstraktion wesentliche Systeminformationen verlieren.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-IPC-0001_Typed_IPC`
- `ADR-IPC-0002_Capability_Based_IPC`
- `ADR-IPC-0003_Zero_Copy_IPC`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0018_Locality_als_Optimierungsziel`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-IPC-UNIFIED-CALL-0001`
- `NPSPEC-IPC-CALL-CONTRACT-0001`
- `NPSPEC-IPC-CALL-RESOLUTION-0001`
- `NPSPEC-IPC-CALL-PROVIDER-0001`
- `NPSPEC-IPC-CALL-ASYNC-0001`
- `NPSPEC-IPC-UNIFIED-CALL-TEST-0001`

---

## Ergebnis

NovaOS trennt den logischen Aufruf von dessen konkretem Ausführungsort:

```text
Typed Call
    ↓
Contract + Capability
    ↓
Resolution
 ┌────┼─────┐
Local IPC Provider
```

Dadurch können Implementierungen verschoben, ersetzt oder unterschiedlich bereitgestellt werden, ohne für jeden Ausführungsort ein eigenes Call-Modell einzuführen.

Die Unterschiede bei Kosten, Latenz und Fehlern bleiben dennoch explizit sichtbar.