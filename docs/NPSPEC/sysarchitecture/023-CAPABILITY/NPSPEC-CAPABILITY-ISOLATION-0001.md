# NPSPEC-CAPABILITY-ISOLATION-0001 – Nova Capability Isolation

## Status

Angenommen

## Kategorie

Capability / Isolation / Security

## Zweck

NovaOS definiert Capability Isolation als Trennung von Autorität zwischen Prozessen, Workloads, Anwendungen, Services, Agents und Security Domains.

```text
Security Domain A        Security Domain B
├── Capability A         ├── Capability C
└── Capability B         └── Capability D
        │                        │
        └──── Isolation ─────────┘
```

Eine Capability darf ausschließlich innerhalb des autorisierten Kontexts nutzbar sein und nicht unbeabsichtigt über Isolationsgrenzen gelangen.

## Grundprinzipien

```text
Isolation ≠ Authorization
Isolation ≠ Trust
Visibility ≠ Authority
Shared Object ≠ Shared Authority
Shared Memory ≠ Shared Capabilities
Same Identity ≠ Same Capability Set
Same Application ≠ Same Security Domain
Isolation Boundary ≠ Automatic Trust Boundary
```

## Capability Domain

Jeder Security Context besitzt eine explizite Capability-Menge.

```text
CapabilityDomain
├── DomainID
├── Principal Identity
├── Capability Set
├── Security Policy
└── State
```

Dabei gilt:

```text
Authority(Process A)
≠
Authority(Process B)
```

auch wenn beide Prozesse zur selben Anwendung gehören.

## Capability Tables

Capability Handles werden in isolierten Capability Tables verwaltet.

```text
Process A
└── Handle 10 → Capability X

Process B
└── Handle 10 → Capability Y
```

Handle-Werte besitzen außerhalb ihrer Capability Table keine Bedeutung.

## Prozessisolation

Neue Prozesse oder Workloads erhalten keine automatische Kopie aller Parent-Capabilities.

```text
Parent Capabilities
       ↓
Spawn Contract
       ↓
Explicit Selection
       ↓
Child Capabilities
```

Damit wird Ambient Authority vermieden.

## IPC

IPC darf Isolation nur durch explizite Capability-Übertragung überschreiten.

```text
Domain A
   ↓
Explicit Transfer
   ↓
Validation
   ↓
Attenuation
   ↓
Domain B
```

Normale Nachrichten dürfen keine versteckte Capability-Übertragung verursachen.

## Shared Memory

Shared Memory und Capability Authority bleiben getrennt.

```text
Shared Buffer
     ≠
Shared Capability Table
```

Das gemeinsame Mapping eines Buffers darf keine weiteren Capabilities der beteiligten Prozesse offenlegen.

## Services

Services erhalten ausschließlich Capabilities, die sie für ihre Aufgabe benötigen.

```text
Application
    ↓
Service Request
    ↓
Minimal Delegated Capability
    ↓
Service
```

Ein Service darf seine eigenen Systemrechte nicht automatisch auf den Client übertragen.

## Agents

Agents und Sub-Agents besitzen isolierte Capability Domains.

```text
User
 ↓ Delegation
Agent A
 ↓ Attenuation
Sub-Agent B
```

Ein Agent darf weder auf nicht delegierte Benutzerrechte noch auf Capabilities anderer Agents zugreifen.

## Geräte und Treiber

Treiber sollen Capability-isoliert auf Hardware zugreifen.

```text
Driver
├── Device Capability
├── DMA Capability
├── Interrupt Capability
└── Memory Capability
```

Der Besitz einer Device Capability darf keinen allgemeinen Zugriff auf andere Hardware erzeugen.

IOMMU und Memory Protection sollen Hardware-Isolation unterstützen.

## Remote Isolation

Remote Capabilities müssen lokale und entfernte Authority Domains getrennt halten.

```text
Local Domain
    ↓
Controlled Export
    ↓
Remote Capability
    ↓
Remote Domain
```

Eine Remote-Verbindung darf keine implizite Erweiterung lokaler Autorität bewirken.

## Fehlerisolation

Der Absturz oder die Kompromittierung einer Komponente soll auf deren Capability Domain begrenzt bleiben.

```text
Compromised Workload
       ↓
Accessible Authority
       ↓
Only Explicit Capabilities
```

Dies begrenzt den möglichen Schadensumfang.

## Revocation

Isolation muss mit Revocation zusammenarbeiten.

```text
Capability Revoked
       ↓
All Isolated References
       ↓
Invalid
```

Ein anderer Security Domain darf eine widerrufene Capability nicht über alte Handles oder Tokens weiterverwenden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
DomainID
Principal Identity
Capability Count
Capability Types
Delegation Relations
Isolation Boundaries
State
```

Verwendbare Tokens oder fremde geheime Capability-Daten dürfen dabei nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Capability Authority zwischen Security Domains isolieren.
2. Capability Tables MÜSSEN voneinander getrennt sein.
3. Handles DÜRFEN außerhalb ihres gültigen Kontexts keine Autorität besitzen.
4. Prozesse und Workloads DÜRFEN Capabilities nur explizit erhalten.
5. IPC DARF Isolation nur durch kontrollierte Capability-Übertragung überschreiten.
6. Shared Memory DARF keine implizite Capability-Freigabe erzeugen.
7. Services, Agents und Treiber MÜSSEN nach Least Privilege isolierbar sein.
8. Remote Capability Domains MÜSSEN von lokalen Authority Domains getrennt bleiben.
9. Revocation MUSS über Isolationsgrenzen hinweg wirksam bleiben.
10. Kompromittierung einer Komponente SOLL auf deren explizit verfügbare Autorität begrenzt werden.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-CAPABILITY-TOKEN-0001`
- `NPSPEC-CAPABILITY-DELEGATION-0001`
- `NPSPEC-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-ISOLATION-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-DRIVER-ISOLATION-0001`
- `ADR-CAPABILITY-0010`

## Ergebnis

```text
Principal
    ↓
Isolated Capability Domain
    ↓
Explicit Authority
    ↓
Controlled Transfer
    ↓
Minimal Cross-Domain Authority
```

NovaOS erhält damit eine durchgängige Capability-Isolation, bei der jede Komponente ausschließlich auf explizit bereitgestellte Autorität zugreifen kann und Fehler oder Kompromittierungen möglichst innerhalb ihrer Security Domain begrenzt bleiben.