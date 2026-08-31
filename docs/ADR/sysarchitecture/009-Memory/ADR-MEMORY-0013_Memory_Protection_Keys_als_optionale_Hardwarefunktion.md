# ADR-MEMORY-0013 – Memory Protection Keys als optionale Hardwarefunktion

## Status

Angenommen

## Kategorie

Kernel / Memory Management / Security / Hardwareunterstützung

## Kontext

Klassische Page-Table-Berechtigungen wie Read, Write und Execute bilden die grundlegende Speicherisolation von NovaOS.

Einige Prozessorarchitekturen bieten zusätzlich **Memory Protection Keys** oder vergleichbare Mechanismen. Damit können Gruppen von Pages einem Protection Key zugeordnet und ihre Zugriffsrechte anschließend mit geringerem Aufwand verändert werden, ohne jedes einzelne Page-Table-Mapping anzupassen.

Dies kann insbesondere für:

- Laufzeit-Isolation innerhalb einer Execution Domain,
- Sandboxing,
- JIT-Runtimes,
- temporär geschützte Speicherbereiche,
- Security-sensitive Libraries

nützlich sein.

Protection Keys sind jedoch nicht auf jeder Zielarchitektur verfügbar und dürfen deshalb keine Voraussetzung für die Korrektheit oder grundlegende Isolation von NovaOS sein.

---

## Entscheidung

NovaOS unterstützt Memory Protection Keys und funktional vergleichbare Hardwaremechanismen als **optionale Beschleunigung zusätzlicher Speicherprotektion**.

```text
Virtual Memory Protection
        │
        ├── Page-Table Protection    ← verbindliche Basis
        │
        └── Protection Keys          ← optionale Hardwareoptimierung
```

Die grundlegende Security-Semantik muss immer über die normale Virtual-Memory- und Capability-Architektur durchsetzbar bleiben.

Protection Keys dürfen diese Mechanismen ergänzen, aber nicht ersetzen.

---

## Grundprinzip

```text
Security semantics are mandatory.

Protection-key acceleration is optional.

Hardware availability must never
define correctness.
```

---

## Abstraktion

Protection Keys werden durch das architekturspezifische Memory Backend beziehungsweise die HAL abstrahiert.

Der generische Memory Manager darf nicht direkt von:

- x86-spezifischen Protection Keys,
- ARM-spezifischen Mechanismen,
- oder einer bestimmten Registerstruktur

abhängen.

Die Plattform meldet stattdessen ihre verfügbaren Protection-Fähigkeiten.

---

## Protection Domains

Geeignete Memory Regions können logisch einer Protection Domain zugeordnet werden.

```text
Memory Region
      ↓
Protection Domain
      ↓
Hardware Protection Key
```

Die logische Protection Domain bleibt von der konkreten Hardwareimplementierung getrennt.

Besitzt die Plattform keine Protection Keys, muss NovaOS dieselbe erforderliche Sicherheitssemantik mit anderen zulässigen Mechanismen herstellen oder die betreffende optionale Optimierung deaktivieren.

---

## Verhältnis zu Page Permissions

Protection Keys ersetzen nicht:

```text
Read
Write
Execute
```

der Page Tables.

Die effektive Berechtigung ergibt sich aus allen relevanten Schutzmechanismen.

Protection Keys dürfen bestehende Page-Table-Rechte nicht erweitern.

Insbesondere kann ein Protection Key kein NX-Mapping ausführbar oder ein Read-only-Mapping beschreibbar machen.

---

## Einsatzbereich

Protection Keys sollen vor allem verwendet werden, wenn Zugriffsrechte auf größere Gruppen bereits gemappter Pages häufig oder sehr schnell geändert werden müssen.

Beispiele:

```text
Runtime Isolation
Sandbox Memory
JIT State
Sensitive Runtime Data
Temporary Access Windows
```

Ob Protection Keys tatsächlich eingesetzt werden, entscheidet die Memory- und Security-Policy anhand der Hardwarefähigkeiten und Anforderungen.

---

## Execution Domains

Protection Keys erzeugen zusätzliche Isolation **innerhalb** einer Execution Domain.

Sie ersetzen nicht die Isolation zwischen Execution Domains.

```text
Execution Domain A
    ├── Protection Region 1
    └── Protection Region 2

Execution Domain B
    └── separate VAS
```

Die VAS-Isolation bleibt die primäre Grenze zwischen Execution Domains.

---

## Begrenzte Hardwareressource

Die Anzahl verfügbarer Protection Keys kann hardwareseitig stark begrenzt sein.

NovaOS darf deshalb nicht voraussetzen, dass jede:

- Allocation,
- Library,
- Capability,
- Memory Region

einen eigenen Hardware-Key erhält.

Protection Keys müssen als begrenzte Hardwareressource verwaltet werden.

Die Zuordnung zwischen logischen Protection Domains und Hardware Keys darf policygesteuert erfolgen.

---

## Security

Protection-Key-Zustände sind sicherheitsrelevant.

NovaOS muss verhindern, dass nicht autorisierter Code:

- fremde Protection Domains verändert,
- Protection Keys neu zuordnet,
- oder Schutzgrenzen dauerhaft deaktiviert.

Protection-Key-Verwaltung muss daher in das bestehende Authority- und Security-Modell integriert werden.

---

## Scheduler und Context Switch

Falls Protection-Key-Zustände execution-context-spezifisch sind, müssen sie beim Scheduling korrekt erhalten beziehungsweise wiederhergestellt werden.

Ein Task darf keine Protection-Key-Konfiguration eines anderen nicht autorisierten Execution Context übernehmen.

Die konkrete Hardwarebehandlung ist Aufgabe der architekturspezifischen Implementierung.

---

## Deterministic Mode

Protection Keys dürfen im Deterministic Mode verwendet werden, sofern ihr Verhalten reproduzierbar ist.

Adaptive Zuordnung oder dynamisches Recycling von Keys kann eingeschränkt werden, wenn dies für reproduzierbares Verhalten erforderlich ist.

---

## Fail-Safe Verhalten

Fehlt die benötigte Hardwarefunktion oder stehen keine Protection Keys mehr zur Verfügung, darf NovaOS eine erforderliche Sicherheitsgrenze nicht still entfernen.

Es gilt:

```text
Protection Key unavailable
        ↓
Secure fallback
or
Reject operation
```

Performanceverlust ist zulässig.

Eine Abschwächung verbindlicher Security-Anforderungen ist nicht zulässig.

---

## Architecture Introspection

NovaOS muss autorisierten Komponenten Informationen über die Nutzung von Protection Keys bereitstellen können.

Dazu gehören insbesondere:

- verfügbare Hardwareunterstützung,
- verwendete Protection Domains,
- Hardware-Key-Belegung,
- Fallback-Nutzung,
- relevante Protection-Verletzungen.

Hardwaredetails sollen dabei hinter der allgemeinen Memory-Protection-Abstraktion bleiben.

---

## Normative Anforderungen

1. NovaOS DARF Hardware Memory Protection Keys als optionale Funktion verwenden.
2. Protection Keys DÜRFEN NICHT Voraussetzung für grundlegende Memory Isolation oder Systemkorrektheit sein.
3. Protection Keys MÜSSEN über eine architekturunabhängige Abstraktion bereitgestellt werden.
4. Page-Table-Protection MUSS die verbindliche Basis der Speicherprotektion bleiben.
5. Protection Keys DÜRFEN bestehende Page-Table-Rechte NICHT erweitern.
6. Protection Keys SOLLEN für zusätzliche Protection Domains innerhalb einer Execution Domain nutzbar sein.
7. Die Isolation zwischen Execution Domains DARF NICHT durch Protection Keys ersetzt werden.
8. Protection Keys MÜSSEN als begrenzte Hardwareressource behandelt werden.
9. Protection-Key-Zuordnung und -Änderung MÜSSEN autorisiert sein.
10. Context-spezifische Protection-Zustände MÜSSEN beim Scheduling korrekt isoliert werden.
11. Fehlende oder erschöpfte Protection-Key-Ressourcen MÜSSEN über sicheren Fallback oder kontrolliertes Scheitern behandelt werden.
12. Security-Anforderungen DÜRFEN bei fehlender Hardwareunterstützung NICHT abgeschwächt werden.
13. Deterministic Mode MUSS dynamische Protection-Key-Policies kontrollieren können.
14. Protection-Key-Nutzung und Fallbacks MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- schnelle Änderung von Zugriffsrechten auf Speichergruppen,
- zusätzliche Isolation innerhalb einer Execution Domain,
- effizientere Sandboxing- und Runtime-Mechanismen,
- Nutzung moderner CPU-Sicherheitsfunktionen ohne Architekturabhängigkeit,
- sichere Portabilität auf Plattformen ohne Protection Keys.

### Negative Konsequenzen

- zusätzliche Zustände im Memory- und Scheduler-Modell,
- hardwareseitig nur begrenzte Anzahl von Keys,
- plattformspezifisch unterschiedliche Fähigkeiten,
- sichere Fallback-Pfade bleiben weiterhin erforderlich.

---

## Verworfene Alternativen

### Protection Keys als verpflichtende NovaOS-Hardwareanforderung

Verworfen.

Dies würde Portabilität einschränken und grundlegende Security von optionalen CPU-Funktionen abhängig machen.

### Protection Keys ersetzen Page-Table-Protection

Verworfen.

Protection Keys sind eine zusätzliche Schutzebene und keine vollständige Alternative zu MMU-Rechten.

### Ein Hardware-Key pro Memory Object

Verworfen.

Die verfügbare Anzahl von Hardware Keys ist typischerweise zu gering und architekturabhängig.

### Fehlende Protection Keys durch schwächere Isolation kompensieren

Verworfen.

NovaOS folgt Fail-Safe Defaults; Performanceoptimierungen dürfen Security nicht abschwächen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-MEMORY-0005_Virtual_Address_Spaces_pro_Execution_Domain`
- `ADR-MEMORY-0012_ASLR_Guard_Pages_und_Non_Executable_Memory`
- `ADR-HAL-0002_Plattformabstraktion_für_x86_ARM_und_zukünftige_Architekturen`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-MEMORY-PROTECTION-KEY-0001`
- `NPSPEC-MEMORY-PROTECTION-DOMAIN-0001`
- `NPSPEC-MEMORY-PROTECTION-KEY-ALLOC-0001`
- `NPSPEC-MEMORY-PROTECTION-KEY-CONTEXT-0001`
- `NPSPEC-MEMORY-PROTECTION-KEY-TEST-0001`

---

## Ergebnis

Memory Protection Keys werden in NovaOS als optionale Hardwarebeschleunigung einer allgemeineren Protection-Architektur verwendet:

```text
Security Requirement
        ↓
Logical Protection Domain
        ↓
Page-Table Protection
        +
Optional Hardware Protection Key
```

Damit kann NovaOS moderne Hardwaremechanismen für effiziente feingranulare Isolation nutzen, ohne seine Security-Semantik von einer bestimmten Prozessorarchitektur abhängig zu machen.

Die zentrale Regel lautet:

```text
Protection semantics are portable.

Hardware protection keys are optional.

Security must remain correct
with or without them.
```