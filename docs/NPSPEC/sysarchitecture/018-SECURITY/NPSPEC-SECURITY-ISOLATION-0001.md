# NPSPEC-SECURITY-ISOLATION-0001 – Nova Security Isolation

## Status

Angenommen

## Kategorie

Security / Isolation / Security Domains

## Zweck

NovaOS definiert Isolation als systemweiten Mechanismus zur Trennung von Komponenten, Ressourcen und Sicherheitsdomänen.

```text
Component A │ Isolation Boundary │ Component B
```

Ein Fehler oder eine Kompromittierung soll möglichst innerhalb der betroffenen Domäne bleiben.

## Grundprinzipien

```text
Isolation ≠ Authorization
Same System ≠ Same Trust
Shared Resource ≠ Shared Authority
Failure in A ≠ Failure in B
Compromise in A ≠ Authority over B
```

Isolation ergänzt Capability Security, MAC, Sandboxing und Prozessisolation.

## Isolationsmodell

Eine Isolationsdomäne besitzt mindestens:

```text
IsolationDomain
├── DomainID
├── Members
├── Resources
├── Boundaries
└── State
```

Optional:

```text
Security Context
Trust Level
Resource Budget
Communication Policy
Failure Policy
```

`DomainID` bleibt unabhängig von physischem Ausführungsort oder Prozess-ID.

## Isolationsbereiche

NovaOS muss Isolation ermöglichen für:

```text
Memory
Processes
Drivers
Storage
IPC
Network
Devices
Credentials
System Services
```

Eine Komponente darf Ressourcen außerhalb ihrer Domäne nur über explizit autorisierte Schnittstellen erreichen.

## Speicherisolation

Speicherbereiche verschiedener Sicherheitsdomänen müssen voneinander geschützt sein.

```text
Domain A Memory
      ║
Protection Boundary
      ║
Domain B Memory
```

Direkter Zugriff darf nur über explizit freigegebene Shared-Memory-Mechanismen erfolgen.

## Prozess- und Dienstisolation

Prozesse und Dienste sollen getrennte Sicherheits- und Ressourcendomänen besitzen können.

Ein kompromittierter Dienst darf dadurch nicht automatisch Autorität über andere Dienste erhalten.

## Treiberisolation

Treiber sollen soweit technisch möglich voneinander und vom restlichen System isoliert werden.

```text
Driver
  ↓
Required Device
  ↓
Required DMA / IRQ / Memory
```

Hardwarezugriff darf nicht automatisch Zugriff auf andere Geräte oder Speicherbereiche erzeugen.

## IPC

Kommunikation über Isolationsgrenzen muss kontrolliert erfolgen.

```text
Domain A
   ↓
Capability-protected IPC
   ↓
Domain B
```

Die Existenz eines IPC-Endpunkts erzeugt keine Kommunikationsberechtigung.

## Netzwerk

Netzwerkisolation darf über:

```text
Network Namespaces
Firewall
Network Capabilities
Network Intent
```

durchgesetzt werden.

Eine isolierte Komponente darf nicht automatisch Zugriff auf das Host-Netzwerk besitzen.

## Gemeinsame Ressourcen

Gemeinsam verwendete Ressourcen müssen explizit freigegeben werden.

```text
Private → Default
Shared  → Explicit
```

Freigaben müssen Rechte, Lebensdauer und beteiligte Domains begrenzen können.

## Fehlerisolation

Fehler sollen möglichst lokal bleiben.

```text
Component Failure
      ↓
Contain
      ↓
Restart / Replace / Terminate
```

Ein Fehler innerhalb einer Domäne soll keine unnötige globale Systembeeinträchtigung verursachen.

## Ressourcenisolation

Domänen müssen Ressourcenbudgets besitzen können:

```text
CPU
Memory
Storage
IPC
Network
Handles
```

Eine Domäne darf andere Domänen nicht durch unkontrollierten Ressourcenverbrauch verdrängen.

## Introspection

NovaOS soll mindestens darstellen können:

```text
DomainID
Members
Resources
Trust Level
Communication Paths
Shared Resources
Violations
State
```

## Normative Anforderungen

1. NovaOS MUSS systemweite Sicherheitsisolation unterstützen.
2. Sicherheitsdomänen MÜSSEN stabile Identitäten besitzen können.
3. Speicher verschiedener Domänen MUSS geschützt werden.
4. Zugriffe über Isolationsgrenzen MÜSSEN explizit autorisiert sein.
5. Gemeinsame Ressourcen MÜSSEN kontrolliert freigegeben werden.
6. Treiber, Dienste und Anwendungen SOLLEN getrennt isolierbar sein.
7. Ressourcenverbrauch MUSS pro Domäne begrenzbar sein.
8. Fehler SOLLEN möglichst innerhalb ihrer Ursprungsdomäne bleiben.
9. Isolation DARF Capability- oder MAC-Prüfungen NICHT ersetzen.
10. Isolationszustände und Grenzüberschreitungen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-MAC-0001`
- `NPSPEC-SECURITY-LEASTPRIVILEGE-0001`
- `NPSPEC-SECURITY-SANDBOX-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-DRIVER-ISOLATION-0001`
- `NPSPEC-MEMORY-PROTECTION-0001`
- `NPSPEC-NETWORK-NAMESPACE-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-SECURITY-0008`

## Ergebnis

```text
Components
    ↓
Isolation Domains
    ↓
Explicit Boundaries
    ↓
Controlled Communication
    ↓
Contained Authority + Failures
```

NovaOS erhält damit ein einheitliches Isolationsmodell, das Sicherheitsgrenzen zwischen Anwendungen, Diensten, Treibern und Ressourcen erzwingt und Fehler sowie Kompromittierungen möglichst lokal begrenzt.