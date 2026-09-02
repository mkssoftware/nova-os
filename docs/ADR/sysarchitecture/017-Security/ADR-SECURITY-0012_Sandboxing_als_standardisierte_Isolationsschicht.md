# ADR-SECURITY-0012 – Sandboxing als standardisierte Isolationsschicht

## Status

Angenommen

## Kategorie

Security / Isolation / Sandboxing

## Kontext

NovaOS führt Anwendungen, Systemdienste, Treiber, Plugins, KI-Agenten und Legacy-Software mit unterschiedlichen Vertrauens- und Authority-Niveaus aus.

Diese Komponenten benötigen eine gemeinsame Isolationsarchitektur, statt jeweils eigene Sandbox-Mechanismen zu implementieren.

Sandboxing muss dabei mit Execution Domains, Capabilities, Resource Economy und den systemweiten Security Policies zusammenarbeiten.

---

## Entscheidung

NovaOS definiert **Sandboxing als standardisierte systemweite Isolationsschicht**.

```text
Workload
   ↓
Sandbox Policy
   ↓
Execution Domain
   ├── Authority
   ├── Resource Limits
   ├── Data Access
   ├── Device Access
   └── Communication Boundaries
```

Eine Sandbox beschreibt die zulässige Ausführungsumgebung einer Komponente und wird durch die vorhandenen NovaOS-Sicherheitsmechanismen durchgesetzt.

---

## Grundprinzip

```text
Sandbox = constrained execution environment
```

Sandboxing ist keine separate Sicherheitsarchitektur, sondern eine standardisierte Kombination bestehender Isolation-, Capability- und Policy-Mechanismen.

---

## Sandbox-Profil

Eine Sandbox kann unter anderem festlegen:

- verfügbare Capabilities,
- erlaubte IPC- und `Nova.Call`-Ziele,
- Storage-Zugriffe,
- Netzwerkzugriffe,
- Gerätezugriffe,
- Resource Budgets,
- Trust Requirements,
- Data-Sovereignty-Grenzen,
- erlaubte Systeminterfaces.

Die konkrete Profilstruktur wird in NPSPECs definiert.

---

## Execution Domains

Execution Domains bilden die primäre harte Ausführungsgrenze einer Sandbox.

```text
Sandbox
   ↓
Execution Domain
   ├── Address Space
   ├── Capability Context
   └── Resource Context
```

Mehrere Sandbox-Typen können dieselben grundlegenden Isolationstechniken verwenden, aber unterschiedliche Policies besitzen.

---

## Capability Integration

Eine Sandbox erzeugt keine implizite Authority.

Zugriffe auf geschützte Ressourcen benötigen weiterhin entsprechende Capabilities.

```text
Sandbox permits interface
        +
Capability authorizes operation
        =
Potentially allowed
```

ABAC, MAC und andere Hard Constraints bleiben zusätzlich wirksam.

---

## Standardisierte Sandbox-Klassen

NovaOS soll unterschiedliche Sandbox-Profile auf derselben Architektur abbilden können, beispielsweise für:

```text
Application
System Service
Driver
Plugin
AI Agent
Compatibility Domain
Untrusted Content Handler
```

Diese Klassen sind Policy-Profile und keine voneinander getrennten Sandbox-Systeme.

---

## Ressourcenisolation

Sandboxing umfasst neben Security Authority auch Ressourcenbegrenzung.

Eine Sandbox kann Budgets für beispielsweise:

- CPU,
- Memory,
- Storage,
- Network,
- GPU/NPU,
- offene Handles,
- IPC-Queues

besitzen.

Damit kann eine isolierte Komponente nicht allein durch Ressourcenverbrauch das Gesamtsystem unkontrolliert beeinträchtigen.

---

## Kommunikation

Kommunikation über Sandbox-Grenzen erfolgt ausschließlich über kontrollierte Systemmechanismen.

```text
Sandbox A
    ↓
Typed IPC / Nova.Call
    ↓
Sandbox B
```

Direkte Umgehung der definierten Isolation ist nicht Teil des nativen Modells.

---

## Dynamische Anpassung

Sandbox-Authority kann kontrolliert erweitert oder reduziert werden.

Eine Erweiterung benötigt eine explizite Autorisierungsentscheidung und gegebenenfalls neue Capabilities.

```text
Current Sandbox
      ↓
Authorized Change
      ↓
Updated Sandbox
```

Adaptive Systeme dürfen Sandbox-Grenzen nicht selbständig erweitern.

---

## Legacy Compatibility

Legacy-Sandbox- und Berechtigungsmodelle können innerhalb von Compatibility Domains emuliert werden.

Sie dürfen die native NovaOS-Isolationsschicht jedoch nicht umgehen.

---

## Audit und Introspection

NovaOS muss Sandbox-Konfigurationen introspektierbar machen können.

Insbesondere sollen sichtbar sein:

```text
Sandbox
 ├── Execution Domain
 ├── Capabilities
 ├── Resource Budgets
 ├── Communication Boundaries
 └── Applied Policies
```

---

## Normative Anforderungen

1. NovaOS MUSS eine standardisierte systemweite Sandboxing-Architektur bereitstellen.
2. Sandboxing MUSS auf den nativen Isolation- und Capability-Mechanismen von NovaOS aufbauen.
3. Execution Domains MÜSSEN als harte Sandbox-Grenzen verwendbar sein.
4. Sandbox-Profile MÜSSEN Authority, Ressourcen und Kommunikationsgrenzen beschreiben können.
5. Eine Sandbox DARF keine implizite Authority erzeugen.
6. Geschützte Operationen MÜSSEN weiterhin entsprechende Capabilities erfordern.
7. MAC-, ABAC-, Trust- und Sovereignty-Regeln MÜSSEN innerhalb von Sandboxes weiterhin gelten.
8. NovaOS MUSS unterschiedliche Workload-Typen über dieselbe Sandbox-Architektur isolieren können.
9. Ressourcenverbrauch MUSS pro Sandbox begrenzbar sein.
10. Kommunikation über Sandbox-Grenzen MUSS kontrolliert erfolgen.
11. Sandbox-Erweiterungen MÜSSEN explizit autorisiert werden.
12. Adaptive Systeme DÜRFEN Sandbox-Grenzen NICHT selbständig erweitern.
13. Legacy-Sandbox-Modelle DÜRFEN native NovaOS-Sicherheitsgrenzen NICHT umgehen.
14. Sandbox-Zustände und Policies MÜSSEN kontrolliert auditierbar und introspektierbar sein.
15. Fehlerhafte oder unvollständige Sandbox-Konfigurationen MÜSSEN fail-safe behandelt werden.

---

## Konsequenzen

### Positive Konsequenzen

- einheitliche Isolation für unterschiedliche Workload-Typen,
- keine separaten Sandbox-Systeme pro Subsystem,
- direkte Integration mit Capabilities und Resource Economy,
- geringerer Blast Radius kompromittierter Komponenten,
- konsistente Security Policies über das gesamte System.

### Negative Konsequenzen

- Sandbox-Profile benötigen klare und stabile Semantik,
- zusätzliche Isolation kann Performancekosten verursachen,
- Legacy-Anwendungen können umfangreiche Compatibility-Profile benötigen.

---

## Verworfene Alternativen

### Eigenes Sandbox-System pro Anwendungstyp

Verworfen.

Dies würde mehrere unterschiedliche Sicherheitsarchitekturen erzeugen.

### Sandboxing ausschließlich über Dateisystemrechte

Verworfen.

Isolation umfasst deutlich mehr als Storage-Zugriffe.

### Sandboxing ausschließlich über Virtualisierung

Verworfen.

Eine vollständige VM ist für viele Isolation Domains unnötig schwergewichtig.

### Vertrauenswürdige Anwendungen grundsätzlich ohne Sandbox

Verworfen.

Vertrauen ersetzt keine Isolation und begrenzt keinen Blast Radius.

---

## Abhängigkeiten

- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-PROCESS-0002_Prozesse_als_Isolations_und_Ressourcendomänen`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0003_Least_Privilege_und_Privilege_Separation`
- `ADR-SECURITY-0005_MAC_für_systemweite_harte_Sicherheitsregeln`
- `ADR-SECURITY-0006_ABAC_als_zentrale_Policyentscheidung`

---

## Zugehörige NPSPECs

- `NPSPEC-SECURITY-SANDBOX-0001`
- `NPSPEC-SECURITY-SANDBOX-PROFILE-0001`
- `NPSPEC-SECURITY-SANDBOX-RESOURCE-0001`
- `NPSPEC-SECURITY-SANDBOX-COMMUNICATION-0001`
- `NPSPEC-SECURITY-SANDBOX-TEST-0001`

---

## Ergebnis

NovaOS verwendet Sandboxing als gemeinsame Isolationsschicht für unterschiedliche Workloads:

```text
Workload
   ↓
Sandbox Profile
   ↓
Execution Domain
   ↓
Capabilities + Policies + Resource Budgets
   ↓
Controlled Execution
```

Sandboxing kombiniert damit die vorhandenen NovaOS-Sicherheitsmechanismen zu einer einheitlichen, wiederverwendbaren Isolationsarchitektur.