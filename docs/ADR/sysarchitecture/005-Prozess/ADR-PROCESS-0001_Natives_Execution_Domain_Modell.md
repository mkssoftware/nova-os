# ADR-PROCESS-0001 – Natives Execution-Domain-Modell

## Status

Angenommen

## Kategorie

Kernel / Prozesse / Ausführung / Isolation / Scheduling / Ressourcen / Sicherheit

## Kontext

Klassische Betriebssysteme behandeln Ausführung meist über die Kategorien:

```text
Process
Thread
Kernel Thread
Service
```

Diese Begriffe sind weiterhin nützlich, reichen für NovaOS jedoch nicht vollständig aus.

NovaOS benötigt ein gemeinsames Modell für unterschiedliche Ausführungsformen, darunter:

- native Anwendungen,
- Systemdienste,
- Treiber,
- isolierte Treiber,
- Kernelkomponenten,
- Capability-Ausführungen,
- Hintergrundaufgaben,
- Sandboxen,
- Kompatibilitätsumgebungen,
- KI-Agenten,
- zukünftige verteilte oder entfernte Ausführung.

Dabei müssen gemeinsame Eigenschaften kontrolliert werden:

- Adressraum,
- Identität,
- Berechtigungen,
- Capabilities,
- Ressourcenbudgets,
- Scheduling,
- Isolation,
- Lifecycle,
- Data-Sovereignty-Regeln,
- Trust,
- Determinismus.

Ein klassisches Prozessobjekt allein soll nicht zum universellen Architekturmodell für alle diese Fälle werden.

---

## Entscheidung

NovaOS führt das **Execution Domain** als primäre logische Einheit für kontrollierte Ausführung ein.

Ein Execution Domain beschreibt eine abgegrenzte Ausführungsumgebung mit:

```text
Identity
Authority
Resources
Memory Context
Execution Contexts
Lifecycle
Policies
```

Konzeptionell:

```text
Execution Domain
      │
      ├── Address Space
      ├── Capability Set
      ├── Resource Budget
      ├── Security Context
      ├── Trust Context
      ├── Data Policy
      ├── Execution Scope
      │
      └── Execution Contexts
             ├── Thread
             ├── Worker
             └── Task
```

Ein klassischer Benutzerprozess wird als eine konkrete Form eines Execution Domains modelliert.

---

## Grundprinzip

```text
A process is one kind of execution domain.

The execution domain is the architectural primitive.
```

---

## Abgrenzung zum Prozess

Ein Prozess bleibt eine sichtbare und praktische Betriebssystemabstraktion.

Er wird jedoch nicht als niedrigste systemweite Ausführungsprimitive verwendet.

Konzeptionell:

```text
Application Process
        ↓
Execution Domain
        ↓
Threads / Tasks
```

Andere Domains müssen kein klassischer Anwendungsprozess sein.

Beispiele:

```text
Driver Domain
Service Domain
Compatibility Domain
Recovery Domain
Agent Domain
```

---

## Execution Domain

Konzeptionell:

```text
ExecutionDomain {
    id
    type
    identity

    memoryContext
    capabilitySet
    securityContext
    trustContext
    dataPolicy

    resourceBudget
    schedulingPolicy

    executionScope
    lifecycleState
}
```

Die konkrete ABI- und Kernelstruktur wird separat spezifiziert.

---

## Domain-Typen

NovaOS kann unterschiedliche Domain-Typen definieren.

Mindestens vorgesehen sind:

```text
Application
SystemService
Driver
KernelService
Compatibility
Sandbox
Agent
Recovery
```

Diese Typen definieren keine vollständig getrennten Prozesssysteme.

Sie konfigurieren dasselbe gemeinsame Domain-Modell mit unterschiedlichen Policies und Privilegien.

---

## Identität

Jedes Execution Domain besitzt eine eindeutige Laufzeitidentität.

Diese ist von:

- Dateiname,
- PID-Anzeige,
- Benutzername,
- UI-Fenster

zu unterscheiden.

Die Identität dient unter anderem für:

- Audit,
- Ressourcenabrechnung,
- Capability-Prüfung,
- IPC,
- Lifecycle,
- Introspection.

---

## PID

NovaOS darf weiterhin klassische Prozess-IDs bereitstellen.

Eine PID ist jedoch eine Prozessdarstellung und nicht zwingend die globale Identität aller Execution Domains.

Konzeptionell:

```text
Process ID
    → compatibility / administration view

ExecutionDomainID
    → native system identity
```

---

## Address Space

Ein Execution Domain kann einen eigenen virtuellen Adressraum besitzen.

Typischer Benutzerprozess:

```text
Execution Domain
      ↓
Private Virtual Address Space
```

Andere Domains können kontrolliert:

- Kerneladressräume,
- Shared Address Spaces,
- eingeschränkte gemeinsame Speicherbereiche

verwenden.

Die konkrete Speichersicherheit wird durch die Memory-Architektur geregelt.

---

## Threads und Execution Contexts

Threads sind ausführbare Kontexte innerhalb eines Execution Domains.

```text
Execution Domain
      │
      ├── Thread 1
      ├── Thread 2
      └── Thread 3
```

Threads erben den grundlegenden:

- Security Context,
- Capability Context,
- Resource Context,
- Address Space

ihres Domains, soweit nicht ausdrücklich weiter eingeschränkt.

---

## Structured Concurrency

Ein Execution Domain besitzt mindestens einen kontrollierten Execution Scope.

Konzeptionell:

```text
Execution Domain
      ↓
Root Execution Scope
      ↓
Child Scopes
      ↓
Tasks / Threads / Operations
```

Damit dürfen Aufgaben nicht unbegrenzt als unverwaltete Hintergrundarbeit entstehen.

Domain-Beendigung kann kontrolliert:

```text
cancel
drain
join
terminate
```

durchführen.

---

## Capability-Modell

Execution Domains erhalten keine implizite globale Autorität allein dadurch, dass sie laufen.

Zugriff auf Systemfunktionen erfolgt über explizite Capabilities.

Konzeptionell:

```text
Execution Domain
      ↓
Capability Set
      ↓
Authorized Operation
```

Beispiele:

```text
File.Read
Network.Connect
Device.Use
Process.Control
System.Configure
```

---

## Least Authority

Ein neues Execution Domain erhält nur die für seine Aufgabe erforderlichen Rechte.

Es gilt:

```text
no capability
    ↓
no authority
```

Ein Domain darf nicht allein aufgrund seiner Herkunft Zugriff auf beliebige Systemressourcen erhalten.

---

## Security Context

Jedes Domain besitzt einen Security Context.

Dieser kann unter anderem enthalten:

```text
principal
capabilities
sandbox policy
integrity level
isolation class
```

Der Security Context ist logisch von der Prozessidentität getrennt.

---

## Trust Context

Trust und Permission sind unterschiedliche Konzepte.

Ein Domain kann beispielsweise:

```text
trusted software
```

sein, ohne dadurch automatisch:

```text
unrestricted permission
```

zu erhalten.

Nova.Trust kann Provenance, Signaturen und Vertrauensstatus des gestarteten Artefakts in den Domain-Kontext übergeben.

---

## Data Sovereignty

Execution Domains müssen Data-Sovereignty- und Information-Flow-Regeln tragen können.

Beispiel:

```text
Domain
  ↓
Data Policy
  ↓
Allowed:
    local processing

Forbidden:
    external network transfer
```

Diese Regeln dürfen nicht allein durch normalen Dateizugriff umgangen werden.

---

## Ressourcen

Jedes Domain besitzt einen Ressourcen-Kontext.

Dieser kann Budgets für:

```text
CPU
Memory
I/O
Network
GPU
NPU
Energy
```

enthalten.

Damit wird Ressourcenverbrauch nicht ausschließlich einzelnen Threads zugerechnet.

---

## Resource Economy

Das Domain ist eine zentrale Abrechnungs- und Kontrollgrenze für die systemweite Ressourcenökonomie.

Konzeptionell:

```text
System Resource Economy
          ↓
Execution Domain Budget
          ↓
Execution Scopes
          ↓
Tasks / Threads
```

Unterbudgets dürfen hierarchisch weitergegeben werden.

---

## Scheduling

Threads innerhalb eines Domains werden regulär durch den Scheduler ausgeführt.

Das Domain selbst kann zusätzliche Schedulingparameter besitzen.

Beispiele:

```text
priority class
CPU set
latency target
deadline policy
resource weight
```

Thread-spezifische Werte dürfen diese weiter verfeinern.

---

## CPU Affinity

Execution Domains können erlaubte oder bevorzugte CPU Sets besitzen.

Beispiel:

```text
Domain:
    Allowed CPUs = {4,5,6,7}
```

Threads dürfen diese Grenzen nicht ohne entsprechende Autorität überschreiten.

---

## Deterministic Mode

Ein Domain kann unter einer Determinism Policy ausgeführt werden.

Dann können beispielsweise festgelegt werden:

```text
scheduler behavior
CPU placement
algorithm selection
time source behavior
random source policy
resource allocation
```

Das Domain bildet damit eine natürliche Grenze für deterministische Ausführung.

---

## Execution Contract

Execution Domains können mit `Nova.ExecutionContract` verknüpft werden.

Relevante Eigenschaften sind unter anderem:

```text
Semantic Requirements
Determinism
Latency / Deadline
Resource Budget
Data Sovereignty
Trust Requirement
Locality
Algorithm Policy
```

Der Contract beschreibt Anforderungen.

Das Execution Domain stellt den kontrollierten Laufzeitkontext bereit.

---

## Lifecycle

Ein Execution Domain besitzt einen expliziten Lifecycle.

Konzeptionell:

```text
Created
   ↓
Prepared
   ↓
Runnable
   ↓
Running
   ↓
Quiescing
   ↓
Terminated
```

Zusätzliche Zustände dürfen definiert werden.

Beispielsweise:

```text
Suspended
Failed
```

---

## Erstellung

Die Erstellung eines Domains erfolgt kontrolliert.

Konzeptionell:

```text
Request
   ↓
Validate Identity
   ↓
Evaluate Trust
   ↓
Resolve Capabilities
   ↓
Allocate Resources
   ↓
Create Memory Context
   ↓
Create Execution Scope
   ↓
Activate
```

Die Aktivierung soll erst erfolgen, wenn notwendige Grundbedingungen erfüllt sind.

---

## Spawn

Ein Domain kann ein weiteres Domain anfordern.

Das Kind erbt nicht automatisch sämtliche Rechte des Eltern-Domains.

Stattdessen wird ein expliziter Authority Transfer durchgeführt.

Beispiel:

```text
Parent Capabilities:
    A B C D

Child requested:
    A C

Child receives:
    A C
```

Neue Rechte können dadurch nicht aus dem Nichts entstehen.

---

## Capability Delegation

Capabilities können kontrolliert an ein anderes Domain delegiert werden.

Die Delegation kann eingeschränkt werden durch:

- Scope,
- Dauer,
- Ressource,
- Operation,
- Weiterdelegierbarkeit.

Damit wird klassische unbeschränkte Rechtevererbung vermieden.

---

## IPC

IPC erfolgt zwischen identifizierten Execution Domains beziehungsweise ihren Endpoints.

Konzeptionell:

```text
Domain A
   ↓
IPC Endpoint
   ↓
Capability Check
   ↓
Domain B
```

Die Existenz eines IPC-Kanals allein erzeugt keine zusätzliche Autorität.

---

## Shared Memory

Domains dürfen kontrollierten Shared Memory verwenden.

Dabei müssen mindestens:

- Ownership,
- Zugriffsrechte,
- Lebensdauer,
- Synchronisation

definiert sein.

Shared Memory darf Isolation nicht implizit aufheben.

---

## Zero-Copy

Das Domain-Modell unterstützt Zero-Copy-Datenübergaben.

Beispiele:

```text
shared memory
page mapping
buffer ownership transfer
```

Dabei müssen Capability-, Data-Sovereignty- und Lifetime-Regeln erhalten bleiben.

---

## Driver Domains

Treiber können abhängig von Sicherheits- und Performanceanforderungen in unterschiedlichen Domains ausgeführt werden.

Beispiele:

```text
Kernel Driver Domain
Isolated Driver Domain
User Driver Domain
```

Das gemeinsame Modell bleibt erhalten.

Damit muss NovaOS nicht für jede Treiberklasse eine vollständig separate Ausführungsarchitektur besitzen.

---

## System Services

Systemdienste laufen ebenfalls in Execution Domains.

Ein Systemdienst erhält nur die für seine Funktion notwendigen Capabilities.

```text
System Service
      ≠
unrestricted system access
```

---

## Kernel Services

Auch interne Kernelkomponenten sollen, soweit sinnvoll, über logisch definierte Ausführungs- und Ressourcenbereiche verfügen.

Nicht jede interne Kerneloperation benötigt dafür einen eigenen virtuellen Adressraum.

Das Domain-Modell kann daher auch logische Isolation darstellen, ohne zwingend Hardwareadressraumisolation zu verlangen.

---

## Compatibility Domains

Kompatibilitätsschichten können als spezielle Execution Domains ausgeführt werden.

Beispiele:

```text
Linux Compatibility Domain
Win32 Compatibility Domain
POSIX Environment
```

Die emulierte beziehungsweise kompatible API erhält dadurch keinen direkten Einfluss auf die native NovaOS-Architektur.

Konzeptionell:

```text
Foreign API
    ↓
Compatibility Domain
    ↓
NovaABI / Capabilities
    ↓
NovaOS
```

---

## Sandbox Domains

Nicht vertrauenswürdige oder stark eingeschränkte Software kann in Sandbox Domains ausgeführt werden.

Dabei können insbesondere begrenzt werden:

- Capabilities,
- Dateizugriffe,
- Netzwerk,
- IPC,
- Ressourcen,
- Gerätezugriffe.

Sandboxing ist damit eine Konfiguration des allgemeinen Domain-Modells.

---

## Agent Domains

KI-Agenten beziehungsweise autonome Softwarekomponenten können in speziellen Agent Domains laufen.

Sie unterliegen denselben grundlegenden Regeln:

```text
Identity
Trust
Capabilities
Resource Budget
Data Policy
Audit
```

Ein Agent erhält keine Sonderautorität aufgrund seiner KI-Funktion.

---

## Recovery Domains

Recovery-Komponenten können ebenfalls als spezielle Domains modelliert werden, sofern sie innerhalb einer laufenden NovaOS-Umgebung ausgeführt werden.

NovaDOS bleibt als separat gebootete Offline-Recovery-Umgebung davon unabhängig.

---

## Isolation Levels

Nicht jedes Domain benötigt dieselbe technische Isolation.

NovaOS kann unterschiedliche Isolation Levels unterstützen.

Beispielsweise:

```text
Logical
AddressSpace
UserMode
HardwareProtected
Virtualized
```

Die gewünschte Isolation wird durch Sicherheits- und Execution-Policy bestimmt.

---

## Keine Hypervisor-Pflicht

Execution Domains benötigen keine Virtual Machine als grundlegende Isolationseinheit.

Hardwarevirtualisierung darf für besonders starke Isolation verwendet werden, ist aber keine Voraussetzung des Domain-Modells.

---

## Failure Containment

Fehler sollen möglichst innerhalb des betroffenen Domains begrenzt bleiben.

Ein abstürzender Benutzerprozess darf beispielsweise nicht den Kernel oder fremde Domains beschädigen.

Bei privilegierten beziehungsweise Kernel Domains hängt die mögliche Isolation von deren technischem Ausführungsmodus ab.

---

## Domain Termination

Bei Beendigung eines Domains müssen zugehörige Ressourcen kontrolliert freigegeben werden.

Dazu gehören:

- Threads,
- Execution Scopes,
- Speicher,
- Handles,
- Capabilities,
- IPC Endpoints,
- Ressourcenreservierungen.

Konzeptionell:

```text
Stop new work
   ↓
Cancel scopes
   ↓
Drain / Join
   ↓
Revoke external authority
   ↓
Release resources
   ↓
Terminate
```

---

## Capability Revocation

Capabilities müssen bei Domain-Beendigung beziehungsweise Policyänderungen widerrufbar sein.

Bereits laufende Operationen müssen entsprechend ihrer Capability- und Lifecycle-Semantik:

- abgeschlossen,
- abgebrochen,
- zurückgewiesen

werden können.

---

## Orphan Prevention

Tasks und Ressourcen sollen nach Domain-Beendigung nicht unbeabsichtigt als Orphans verbleiben.

Structured Concurrency und hierarchische Ressourcenverwaltung dienen als Grundlage dafür.

---

## Crash Handling

Ein Domain-Absturz erzeugt einen expliziten Failure State.

Der Kernel kann abhängig von Policy:

```text
terminate
restart
isolate
report
recover
```

ausführen.

Ein automatischer Restart ist nicht für jede Operation sicher und darf nicht blind erfolgen.

---

## Restartable Services

Systemdienste können explizit als restartfähig definiert werden.

Dabei müssen:

- persistenter Zustand,
- IPC-Rebinding,
- Capability-Wiederherstellung,
- abhängige Dienste

kontrolliert behandelt werden.

Dies kann später mit Hot Replacement und Live Evolution kombiniert werden.

---

## Introspection

Execution Domains sind Bestandteil der Architecture Introspection.

Beispiel:

```text
Execution Domain:
    nova.storage.service

Type:
    SystemService

State:
    Running

Threads:
    6

Memory:
    84 MiB

CPU Budget:
    8%

Capabilities:
    Storage.Manage
    Device.NVMe.Use

Trust:
    Verified
```

---

## Audit

Sicherheitsrelevante Lifecycle- und Authority-Ereignisse müssen auditierbar sein.

Beispiele:

```text
Domain Created
Capability Granted
Capability Delegated
Domain Suspended
Domain Terminated
```

Auditinformationen dürfen nicht mit internem Chain-of-Thought gleichgesetzt werden.

---

## Native ABI

Execution Domains verwenden die native NovaOS-ABI und die zugehörigen Kernelmechanismen.

Kompatibilitäts-APIs werden oberhalb dieses Modells umgesetzt.

Damit bestimmen:

- POSIX,
- Linux,
- Win32

nicht die native Prozessarchitektur.

---

## Prozessdarstellung für Anwendungen

Für normale Anwendungen kann die API weiterhin ein vertrautes Prozessmodell anbieten.

Beispiel:

```text
Process.Start(...)
Process.Id
Process.ExitCode
```

Intern wird dies auf Execution Domains und Execution Contexts abgebildet.

Die native Architektur muss nicht unnötig in jeder High-Level-API sichtbar sein.

---

## Sicherheit

Die Domain-Grenze ist eine zentrale Sicherheitsgrenze.

Es gilt:

```text
Identity
    +
Capability
    +
Policy
    +
Trust
```

bestimmen die zulässige Operation.

Die bloße Existenz eines Prozesses, Threads oder Handles erzeugt keine zusätzliche Autorität.

---

## Fail-Safe Defaults

Kann bei Domain-Erstellung eine notwendige Sicherheits- oder Capability-Entscheidung nicht zuverlässig getroffen werden, wird die betreffende Autorität nicht gewährt.

```text
Unknown
    ≠
Allowed
```

Dies folgt den allgemeinen Fail-Safe-Regeln von NovaOS.

---

## KI-Unabhängigkeit

Das Execution-Domain-System ist grundlegende Kernelinfrastruktur.

Erstellung, Isolation, Scheduling, Capability-Prüfung und Beendigung müssen vollständig ohne KI funktionieren.

KI darf optional:

- Ressourcenbedarf prognostizieren,
- Schedulingpräferenzen vorschlagen,
- Anomalien erkennen.

Sie darf keine zwingenden Sicherheits- oder Isolationseigenschaften ersetzen.

---

## Normative Anforderungen

1. NovaOS MUSS Execution Domains als native logische Einheit kontrollierter Ausführung unterstützen.
2. Ein klassischer Prozess MUSS als eine Form eines Execution Domains modellierbar sein.
3. Execution Domains MÜSSEN eine eindeutige Laufzeitidentität besitzen.
4. Domain Identity und klassische PID MÜSSEN logisch getrennt werden können.
5. Ein Domain MUSS einen expliziten Lifecycle besitzen.
6. Threads und Tasks MÜSSEN einem Domain zugeordnet sein.
7. Ein Domain MUSS mindestens einen kontrollierten Execution Scope besitzen können.
8. Authority MUSS über explizite Capabilities kontrollierbar sein.
9. Neue Domains DÜRFEN Rechte NICHT automatisch vollständig vom Parent übernehmen.
10. Capability Delegation MUSS explizit und einschränkbar sein.
11. Domains MÜSSEN eigene Security- und Trust-Kontexte besitzen können.
12. Trust DARF NICHT automatisch uneingeschränkte Permission bedeuten.
13. Data-Sovereignty-Regeln MÜSSEN an Domains gebunden werden können.
14. Ressourcenbudgets MÜSSEN pro Domain definiert werden können.
15. Ressourcenbudgets MÜSSEN hierarchisch auf untergeordnete Ausführung verteilbar sein.
16. Scheduling- und CPU-Affinity-Regeln MÜSSEN auf Domain-Ebene definierbar sein.
17. Deterministic Mode MUSS auf Domain- beziehungsweise Scope-Ebene anwendbar sein.
18. Execution Contracts MÜSSEN mit Domains verknüpft werden können.
19. IPC MUSS die Identität und Authority der beteiligten Domains berücksichtigen.
20. Shared Memory MUSS explizite Zugriffs- und Lifetime-Regeln besitzen.
21. Zero-Copy-Kommunikation DARF Sicherheits- und Data-Sovereignty-Regeln NICHT umgehen.
22. Treiber MÜSSEN in unterschiedlichen Isolation Levels als Domains ausführbar sein können.
23. Systemdienste DÜRFEN nicht allein aufgrund ihres Typs automatisch Vollzugriff erhalten.
24. Kompatibilitätsumgebungen MÜSSEN auf dem nativen Domain-Modell aufbauen können.
25. Sandbox- und Agent-Ausführung MÜSSEN über dasselbe grundlegende Domain-Modell abbildbar sein.
26. Domain-Beendigung MUSS zugehörige Ausführung und Ressourcen kontrolliert freigeben können.
27. Domains DÜRFEN nach ihrer Beendigung keine unbeabsichtigte Authority behalten.
28. Domain-Abstürze MÜSSEN explizit diagnostizierbar sein.
29. Domain-Zustand, Ressourcen und Authority MÜSSEN kontrolliert introspektierbar sein.
30. Die vollständige grundlegende Domainverwaltung MUSS ohne KI funktionieren.

---

## Konsequenzen

### Positive Konsequenzen

- ein gemeinsames natives Modell für Anwendungen, Dienste, Treiber und Sandboxes,
- klare Trennung zwischen Prozessdarstellung und systemweiter Ausführungsarchitektur,
- direkte Integration von Capabilities, Trust und Data Sovereignty,
- bessere Ressourcen- und Lifecycle-Kontrolle,
- natürliche Grundlage für Structured Concurrency,
- Kompatibilitätsschichten bestimmen nicht die native Architektur,
- zukünftige neue Ausführungsformen können integriert werden, ohne ein separates Prozesssystem zu schaffen.

### Negative Konsequenzen

- das Modell ist umfangreicher als ein klassisches PID-/Prozessmodell,
- Kernel und Runtime benötigen eine klare Trennung zwischen Domain, Thread, Task und Scope,
- Ressourcen-, Capability- und Lifecycle-Systeme müssen eng integriert werden,
- Kompatibilitäts-APIs benötigen Mapping auf das native Modell.

---

## Verworfene Alternativen

### Klassisches POSIX-Prozessmodell als native Grundlage

Verworfen.

Es wäre für NovaOS-Kompatibilität nützlich, würde aber die native Architektur unnötig an historische POSIX-Semantik binden.

### Win32-Prozessmodell als native Grundlage

Verworfen.

NovaOS soll keine bestehende Fremd-ABI zum grundlegenden Systemmodell machen.

### Jeder Diensttyp erhält ein eigenes Ausführungsmodell

Verworfen.

Dies würde Prozesse, Treiber, Services, Agenten und Sandboxes unnötig fragmentieren.

### Jede isolierte Komponente als virtuelle Maschine

Verworfen.

Die Isolation wäre für viele Anwendungsfälle zu schwergewichtig und würde NovaOS unnötig hypervisorzentriert machen.

### Threads als primäre Sicherheitsgrenze

Verworfen.

Threads teilen typischerweise wesentliche Ressourcen und eignen sich nicht als allgemeine Authority- und Isolationseinheit.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starker_Isolation`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvable_System_Architecture`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`

---

## Zugehörige NPSPECs

- `NPSPEC-PROCESS-DOMAIN-0001`
- `NPSPEC-PROCESS-IDENTITY-0001`
- `NPSPEC-PROCESS-LIFECYCLE-0001`
- `NPSPEC-PROCESS-THREAD-0001`
- `NPSPEC-PROCESS-EXECUTION-SCOPE-0001`
- `NPSPEC-PROCESS-CAPABILITY-0001`
- `NPSPEC-PROCESS-SECURITY-CONTEXT-0001`
- `NPSPEC-PROCESS-TRUST-CONTEXT-0001`
- `NPSPEC-PROCESS-RESOURCE-0001`
- `NPSPEC-PROCESS-SPAWN-0001`
- `NPSPEC-PROCESS-IPC-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-PROCESS-COMPATIBILITY-0001`
- `NPSPEC-PROCESS-INTROSPECTION-0001`
- `NPSPEC-PROCESS-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens prüfen:

- Erstellen eines Application Domains,
- mehrere Threads innerhalb eines Domains,
- getrennte Domain- und PID-Identität,
- Capability-Zuweisung,
- eingeschränkte Capability-Delegation,
- Spawn eines Child Domains,
- verweigerte nicht delegierbare Capability,
- eigenes Ressourcenbudget,
- Überschreiten eines CPU- oder Memory-Budgets,
- CPU-Affinity,
- Deterministic Domain,
- Shared Memory zwischen zwei Domains,
- Zero-Copy-Übergabe,
- Domain IPC,
- Sandbox Domain,
- isoliertes Driver Domain,
- System Service Domain,
- Compatibility Domain,
- Agent Domain,
- Domain-Absturz,
- kontrollierte Domain-Beendigung,
- Cancellation aller Child Scopes,
- Capability Revocation,
- keine verwaisten Threads nach Beendigung,
- Introspection und Audit.

---

## Ergebnis

NovaOS verwendet nicht den klassischen Prozess als alleinige systemweite Ausführungsprimitive.

Stattdessen bildet das Execution Domain die gemeinsame Grenze für:

```text
Identity
Memory
Authority
Resources
Scheduling
Lifecycle
Trust
Data Policy
```

Das Architekturmodell lautet:

```text
Execution Domain
      │
      ├── Identity
      ├── Memory Context
      ├── Capability Set
      ├── Security / Trust
      ├── Resource Budget
      ├── Execution Contract
      └── Execution Scope
              ↓
         Threads / Tasks
```

Klassische Prozesse, Systemdienste, Treiber, Sandboxes und zukünftige Ausführungsformen können darauf aufbauen.

Die zentrale Architekturregel lautet:

```text
Model execution by domain,
not by historical process assumptions.

Authority belongs to capabilities.

Resources belong to controlled scopes.

A process is a view.

The execution domain is the native primitive.
```