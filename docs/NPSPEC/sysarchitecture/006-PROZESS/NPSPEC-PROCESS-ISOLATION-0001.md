# NPSPEC-PROCESS-ISOLATION-0001 – Nova Process Isolation

## Status

Entwurf

## Kategorie

Process / Isolation / Security / Memory

## Zweck

NovaOS isoliert Prozesse voneinander und vom Kernel, damit Fehler, Speicherzugriffe und kompromittierte Prozesse möglichst auf ihre eigene Ausführungsdomäne begrenzt bleiben.

```text
Process A     Process B
    │             │
    └──────┬──────┘
           ↓
     Isolation Layer
           ↓
         Kernel
```

## Grundprinzip

Prozesse dürfen ausschließlich auf Ressourcen zugreifen, für die sie explizit autorisiert wurden.

```text
Isolation
+
Capabilities
+
Memory Protection
=
Controlled Access
```

Prozessisolation ist eine Sicherheitsgrenze und darf nicht nur als Fehlervermeidung betrachtet werden.

## Speicherisolation

Normale User-Space-Prozesse besitzen getrennte virtuelle Adressräume.

```text
Process A Virtual Memory
          ≠
Process B Virtual Memory
          ≠
Kernel Memory
```

Direkter Zugriff auf fremden Prozessspeicher ist standardmäßig verboten.

Gemeinsamer Speicher muss explizit eingerichtet werden.

## Kernel-Isolation

User-Space-Prozesse dürfen Kernel-Speicher und privilegierte CPU-Funktionen nicht direkt verwenden.

```text
User Space
    ↓
Defined Kernel Interface
    ↓
Kernel Space
```

Übergänge müssen über kontrollierte Kernel-Schnittstellen erfolgen.

## Ressourcenisolation

Isolation umfasst neben Speicher auch:

```text
Files / Objects
IPC Endpoints
Devices
Network Resources
Compute Resources
Handles
Capabilities
System Services
```

Die Kenntnis einer Ressource allein gewährt keinen Zugriff.

## Capability-basierter Zugriff

Zugriffe werden durch explizite Capabilities autorisiert.

```text
Process
   ↓
Capability
   ↓
Authorized Operation
   ↓
Resource
```

Capabilities müssen hinsichtlich Objekt und erlaubter Operation begrenzbar sein.

## Shared Memory

Gemeinsamer Speicher darf verwendet werden für:

```text
IPC
Zero-Copy
Shared Buffers
Device I/O
Pipelines
```

Die Freigabe muss explizit definieren:

```text
Participants
Memory Range
Permissions
Lifetime
Ownership
```

Shared Memory hebt nur die ausdrücklich definierte Speichergrenze auf.

## IPC

Kommunikation zwischen isolierten Prozessen erfolgt über kontrollierte IPC-Mechanismen.

```text
Process A
   ↓
IPC Endpoint
   ↓
Authorization
   ↓
Process B
```

IPC darf keine implizite Übertragung zusätzlicher Rechte verursachen.

## Gerätezugriff

Direkter Hardwarezugriff aus User Space ist nur über ausdrücklich autorisierte Mechanismen zulässig.

DMA-fähige Geräte müssen zusätzlich durch DMA-/IOMMU-Isolation kontrolliert werden, sofern verfügbar.

```text
Process
   ↓
Driver / Capability
   ↓
DMA Mapping
   ↓
Device
```

## Fehlerisolation

Ein Fehler innerhalb eines Prozesses soll grundsätzlich auf dessen Isolationsdomäne begrenzt bleiben.

```text
Process Failure
      ↓
Contain
      ↓
Terminate / Recover
```

Andere Prozesse und der Kernel dürfen dadurch nicht unmittelbar beschädigt werden.

## Prozessbeziehungen

Parent-, Child-, Service- oder Supervisor-Beziehungen erzeugen keine automatischen Zugriffsrechte.

```text
Relationship ≠ Permission
```

Berechtigungen müssen separat definiert werden.

## Isolation Domains

Für besondere Komponenten dürfen mehrere Prozesse zu einer expliziten Isolation Domain zusammengefasst werden.

Dies kann beispielsweise für Compatibility-Umgebungen oder kontrollierte Legacy-Systeme verwendet werden.

Eine solche Domain muss klare Sicherheits- und Ressourcengrenzen besitzen.

## Normative Anforderungen

1. NovaOS MUSS User-Space-Prozesse voneinander isolieren.
2. Kernel-Speicher MUSS vor normalem User-Space-Zugriff geschützt sein.
3. Fremder Prozessspeicher DARF NICHT ohne explizite Autorisierung zugänglich sein.
4. Shared Memory MUSS explizit eingerichtet und autorisiert werden.
5. Ressourcenbesitz DARF NICHT allein aus Prozessbeziehungen abgeleitet werden.
6. IPC MUSS kontrollierte Sicherheitsgrenzen verwenden.
7. IPC DARF Berechtigungen nicht implizit erweitern.
8. Capability-basierte Zugriffe MÜSSEN auf autorisierte Operationen begrenzt sein.
9. DMA-Zugriffe MÜSSEN kontrolliert und soweit möglich isoliert werden.
10. Prozessfehler SOLLEN auf die jeweilige Isolationsdomäne begrenzt bleiben.
11. Isolation Domains MÜSSEN explizite Grenzen besitzen.
12. Kompatibilitätsanforderungen DÜRFEN die NovaOS-Isolationsregeln nicht stillschweigend abschwächen.

## Abhängigkeiten

- `NPSPEC-PROCESS-MODEL-0001`
- `NPSPEC-PROCESS-LIFECYCLE-0001`
- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `NPSPEC-ARCH-ZEROCOPY-0001`
- `ADR-PROCESS-0004`
- `ADR-VERIFY-0003_Memory_Safety`
- `ADR-VERIFY-0005_Capability_Safety`

## Ergebnis

```text
Process
   ↓
Explicit Authority
   ↓
Isolation Boundary
   ↓
Controlled Resource Access
```

NovaOS erhält damit klar definierte Prozessgrenzen, die Speicher, Ressourcen, Hardwarezugriffe und Kommunikation kontrollieren und Fehler sowie Sicherheitsverletzungen möglichst auf ihre jeweilige Isolationsdomäne begrenzen.