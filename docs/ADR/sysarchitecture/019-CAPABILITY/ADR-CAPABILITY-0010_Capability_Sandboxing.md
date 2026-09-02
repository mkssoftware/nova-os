# ADR-CAPABILITY-0010 – Capability Sandboxing

## Status

Angenommen

## Kategorie

Capability Architecture / Sandboxing / Isolation / Security

## Kontext

NovaOS-Capabilities können von unterschiedlichen und unterschiedlich vertrauenswürdigen Providern implementiert werden.

Dazu gehören beispielsweise:

- Systemkomponenten,
- Anwendungen,
- Plugins,
- Treiber,
- KI-Agenten,
- Compatibility Components,
- Remote Provider.

Die Bereitstellung einer funktionalen Capability darf nicht automatisch bedeuten, dass deren Provider uneingeschränkten Zugriff auf den aufrufenden Prozess, dessen Daten oder andere Systemressourcen erhält.

NovaOS benötigt deshalb eine direkte Verbindung zwischen Capability-Ausführung und der standardisierten System-Sandboxing-Architektur.

---

## Entscheidung

NovaOS unterstützt **Capability Sandboxing als native Möglichkeit, Capability Provider innerhalb explizit begrenzter Execution Domains auszuführen**.

```text
Capability Request
        ↓
Provider Resolution
        ↓
Sandbox Policy
        ↓
Isolated Execution Domain
        ↓
Capability Execution
```

Die Isolation wird aus den bestehenden NovaOS-Sicherheitsmechanismen zusammengesetzt und bildet keine separate Sicherheitsarchitektur.

---

## Grundprinzip

```text
A capability exposes functionality,
not unrestricted access to its caller.
```

---

## Sandbox Context

Ein Capability Provider kann innerhalb eines definierten Sandbox Context ausgeführt werden.

Dieser kann insbesondere begrenzen:

```text
Security Capabilities
Storage Access
Network Access
Device Access
IPC / Nova.Call
Memory Access
Resource Budgets
Data Flow
Trust Context
```

Der Provider erhält nur die für seine Aufgabe erforderlichen Ressourcen und Authorities.

---

## Provider-Isolation

Capability Identity und Provider-Isolation bleiben getrennt.

```text
Capability
   ↓
Provider A → System Domain
Provider B → Application Sandbox
Provider C → Driver Domain
Provider D → Remote Domain
```

Dieselbe funktionale Capability kann somit durch Provider mit unterschiedlichen Isolationseigenschaften bereitgestellt werden.

---

## Caller-Daten

Ein Provider erhält keinen impliziten Zugriff auf den vollständigen Adressraum oder Security Context des Callers.

Daten werden über kontrollierte Mechanismen übergeben:

```text
Typed IPC
Shared Objects
Shared Buffers
Nova.Call
Data/Object Pipeline
```

Zero-Copy darf Isolation nicht umgehen.

---

## Delegierte Authority

Benötigt ein Provider Zugriff auf Ressourcen des Callers, muss die erforderliche Authority explizit delegiert werden.

```text
Caller Authority
      ↓
Attenuation
      ↓
Delegation
      ↓
Sandboxed Provider
```

Dadurch erhält der Provider nur die für die konkrete Capability-Ausführung benötigte Authority.

---

## Sandbox Profile

Provider können deklarieren, welche Sandbox-Anforderungen sie besitzen.

NovaOS kann standardisierte Profile verwenden, beispielsweise:

```text
System Capability
Application Capability
Plugin Capability
Driver Capability
AI Capability
Untrusted Capability
Compatibility Capability
```

Die konkreten Profile und deren technische Grenzen werden in NPSPECs definiert.

---

## Resource Isolation

Capability Sandboxing umfasst neben Security Authority auch Ressourcenbegrenzung.

```text
CPU
Memory
GPU / NPU
Storage I/O
Network
Handles
Queues
Pinned Memory
```

Die Limits werden mit der systemweiten Resource Economy und dem `Nova.ExecutionContract` verbunden.

---

## Capability Composition

Bei Capability Composition können einzelne Teil-Capabilities in unterschiedlichen Sandboxes ausgeführt werden.

```text
Capability A
   ↓
Sandbox A
   ↓
Capability B
   ↓
Sandbox B
   ↓
Capability C
```

Die Übergänge bleiben kontrollierte Data- und Authority-Grenzen.

Eine Composition darf Sandboxing nicht implizit aufheben.

---

## Trust und Provider-Auswahl

Sandboxing kann Bestandteil der Provider Resolution sein.

Ein weniger vertrauenswürdiger Provider kann beispielsweise nur dann zulässig sein, wenn eine ausreichend starke Isolation verfügbar ist.

```text
Provider Trust
      +
Sandbox Strength
      +
Execution Requirements
      ↓
Provider Eligibility
```

Sandboxing ersetzt jedoch keine Trust-Prüfung.

---

## Fehler und Kompromittierung

Ein Fehler oder eine Kompromittierung eines sandboxed Capability Providers soll möglichst auf dessen Execution Domain begrenzt bleiben.

NovaOS kann den Provider abhängig vom Contract:

- beenden,
- neu starten,
- ersetzen,
- isolieren,
- dessen Capabilities widerrufen.

Andere Systemkomponenten sollen dadurch nicht unnötig beeinträchtigt werden.

---

## Hot Replacement

Sandboxed Provider müssen mit Hot Replacement und Capability Registry integrierbar sein.

```text
Provider A
    ↓ failure / update
Provider B
    ↓
New Sandbox
    ↓
Capability continues
```

Die funktionale Capability Identity kann dabei unverändert bleiben.

---

## Introspection

NovaOS muss kontrolliert nachvollziehbar machen können:

```text
Capability
Provider
Execution Domain
Sandbox Profile
Effective Authority
Delegated Authority
Resource Budgets
Communication Boundaries
```

Sensitive Sandbox- oder Security-Informationen bleiben entsprechend vorhandener Authority geschützt.

---

## Normative Anforderungen

1. NovaOS MUSS Capability Provider innerhalb isolierter Execution Domains ausführen können.
2. Capability Sandboxing MUSS auf der allgemeinen NovaOS-Sandboxing-Architektur basieren.
3. Eine funktionale Capability DARF ihrem Provider NICHT automatisch Zugriff auf den Caller gewähren.
4. Caller-Daten MÜSSEN über kontrollierte Daten- und Kommunikationsmechanismen übertragen werden.
5. Zero-Copy DARF Sandbox-Grenzen NICHT umgehen.
6. Benötigte Caller-Authority MUSS explizit delegiert und soweit möglich attenuiert werden.
7. Sandbox Contexts MÜSSEN Security- und Resource-Grenzen definieren können.
8. Unterschiedliche Provider derselben Capability MÜSSEN unterschiedliche Isolationseigenschaften besitzen können.
9. Capability Composition DARF bestehende Sandbox-Grenzen NICHT implizit aufheben.
10. Sandboxing MUSS bei der Provider Resolution berücksichtigt werden können.
11. Sandboxing DARF Trust- oder Authority-Prüfungen NICHT ersetzen.
12. Provider-Fehler SOLLEN möglichst auf die jeweilige Isolation Domain begrenzt bleiben.
13. Capability Sandboxing MUSS mit Revocation und Hot Replacement integrierbar sein.
14. Sandbox Context und effektive Authority MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Drittanbieter-Capabilities können sicherer integriert werden,
- Provider erhalten nur notwendige Authority,
- Fehler und Kompromittierungen werden besser begrenzt,
- Plugins, Treiber und KI-Agenten können dasselbe Isolationmodell nutzen,
- Capability Composition bleibt auch über Trust-Grenzen möglich.

### Negative Konsequenzen

- Sandbox-Übergänge können zusätzlichen Ausführungsaufwand erzeugen,
- Provider benötigen explizite Daten- und Authority-Schnittstellen,
- Provider Resolution muss Isolationseigenschaften berücksichtigen.

---

## Verworfene Alternativen

### Alle Capability Provider im Caller-Prozess ausführen

Verworfen.

Fehler oder Kompromittierungen hätten direkten Zugriff auf den Caller.

### Separate Sandbox-Architektur nur für Capabilities

Verworfen.

NovaOS besitzt bereits eine systemweite standardisierte Sandboxing- und Execution-Domain-Architektur.

### Sandboxing ersetzt Capability-Based Security

Verworfen.

Isolation und Authority sind unterschiedliche Sicherheitsmechanismen.

### Zero-Copy durch direkte Pointer über Sandbox-Grenzen

Verworfen.

Performanceoptimierungen dürfen Isolation nicht umgehen.

---

## Abhängigkeiten

- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-CAPABILITY-0005_Capability_Composition`
- `ADR-CAPABILITY-0007_Capability_Delegation`
- `ADR-CAPABILITY-0008_Capability_Attenuation`
- `ADR-CAPABILITY-0009_Capability_Revocation`
- `ADR-SECURITY-0003_Least_Privilege_und_Privilege_Separation`
- `ADR-SECURITY-0012_Sandboxing_als_standardisierte_Isolationsschicht`

---

## Zugehörige NPSPECs

- `NPSPEC-CAPABILITY-SANDBOX-0001`
- `NPSPEC-CAPABILITY-SANDBOX-PROFILE-0001`
- `NPSPEC-CAPABILITY-SANDBOX-AUTHORITY-0001`
- `NPSPEC-CAPABILITY-SANDBOX-RESOURCE-0001`
- `NPSPEC-CAPABILITY-SANDBOX-COMMUNICATION-0001`
- `NPSPEC-CAPABILITY-SANDBOX-TEST-0001`

---

## Ergebnis

NovaOS trennt die angebotene Funktion einer Capability konsequent von den Rechten ihres Providers:

```text
Capability
    ↓
Provider
    ↓
Sandboxed Execution Domain
    ↓
Explicit Authority + Resource Limits
    ↓
Controlled Execution
```

Damit können auch weniger vertrauenswürdige oder externe Capability Provider in das gemeinsame Fähigkeitenmodell integriert werden, ohne ihnen unnötigen Zugriff auf Caller, Daten oder das Gesamtsystem zu gewähren.