# NPSPEC-CAPABILITY-SANDBOX-0001 – Nova Capability Sandbox

## Status

Angenommen

## Kategorie

Capability / Sandbox / Security

## Zweck

NovaOS definiert Capability Sandboxing als Ausführungsmodell, bei dem Anwendungen, Dienste, Agents und andere Workloads ausschließlich über explizit bereitgestellte Capabilities auf Systemressourcen zugreifen können.

```text
Workload
   ↓
Sandbox
   ↓
Capability Set
   ↓
Explicitly Authorized Resources
```

Die Sandbox begrenzt damit den maximal erreichbaren Autoritätsumfang eines Workloads.

## Grundprinzipien

```text
Sandbox ≠ Trust
Sandbox ≠ Capability
Sandbox ≠ Authorization
Installed ≠ Unsandboxed
Trusted Software ≠ Unlimited Authority
Filesystem Visibility ≠ Filesystem Authority
Network Available ≠ Network Authority
Sandbox Escape ≠ Acceptable Failure Mode
```

## Sandbox-Modell

Eine Capability Sandbox besitzt mindestens:

```text
CapabilitySandbox
├── SandboxID
├── Workload Identity
├── Security Domain
├── Capability Set
├── Resource Limits
└── State
```

Optional:

```text
ExecutionContract
Trust Requirements
Network Policy
Filesystem Policy
Device Policy
IPC Policy
Resource Budget
Sovereignty Policy
```

## Default Authority

Neue Sandboxes starten grundsätzlich mit minimaler Autorität.

```text
Create Sandbox
      ↓
Minimal Runtime Authority
      ↓
Explicit Capability Injection
      ↓
Execute
```

Es darf keine implizite vollständige Benutzer- oder Systemautorität geben.

## Capability Injection

Benötigte Rechte werden explizit bereitgestellt.

```text
Application
├── Read(Document A)
├── Write(Document B)
└── Use(Render Service)
```

Die Anwendung erhält dadurch keinen allgemeinen Zugriff auf:

```text
All Files
All Devices
All Network
Other Applications
System Configuration
User Credentials
```

## Dateisystem

Dateisystemzugriff erfolgt über Capabilities.

```text
File Capability
Directory Capability
NovaFile Capability
Volume Capability
```

Ein sichtbarer Pfad erzeugt keine Autorität.

```text
Path Known ≠ File Accessible
```

## Netzwerk

Netzwerkzugriff muss separat autorisiert werden.

```text
Sandbox
   ↓
Network Capability
   ↓
Allowed Endpoint / Service / Intent
```

Eine allgemeine Netzwerkfähigkeit soll vermieden werden, wenn eine engere Capability ausreicht.

## IPC

Kommunikation mit anderen Komponenten benötigt explizite IPC- oder Service-Capabilities.

```text
Sandbox A
   ↓
IPC Capability
   ↓
Sandbox B
```

Die Existenz eines Services darf keinen automatischen Zugriff erzeugen.

## Geräte

Hardwarezugriff erfolgt ausschließlich über kontrollierte Device Capabilities.

```text
Camera
Microphone
GPU
USB Device
Storage Device
```

Eine Capability für ein Gerät darf keinen Zugriff auf andere Geräte implizieren.

## Dynamische Rechte

Eine Sandbox kann während der Laufzeit zusätzliche Capabilities erhalten.

```text
Operation Request
      ↓
Capability Discovery
      ↓
Policy / User Decision
      ↓
Attenuated Capability
      ↓
Sandbox
```

Neue Autorität muss explizit nachvollziehbar sein.

## Child Workloads

Von einer Sandbox gestartete Workloads erben nicht automatisch sämtliche Capabilities.

```text
Parent Sandbox
      ↓
Explicit Selection
      ↓
Attenuation
      ↓
Child Sandbox
```

Dabei gilt:

```text
Authority(Child) ⊆ Authority(Parent)
```

sofern keine unabhängige Autoritätsquelle ausdrücklich zusätzliche Rechte gewährt.

## Resource Isolation

Capability Sandboxing wird mit Ressourcenbegrenzung kombiniert.

```text
CPU
Memory
Storage
Network
GPU
I/O
```

Resource Budgets sind jedoch keine Capabilities und bleiben ein separater Kontrollmechanismus.

## Kompromittierung

Wird ein Workload kompromittiert, soll der erreichbare Schaden auf dessen Capability Set begrenzt bleiben.

```text
Compromised Application
        ↓
Available Capabilities
        ↓
Maximum Reachable Authority
```

Dies bildet eine zentrale Sicherheitsgrenze von NovaOS.

## Revocation

Capabilities innerhalb einer laufenden Sandbox müssen widerrufbar bleiben.

```text
Capability Revoked
       ↓
Sandbox Capability Set
       ↓
Authority Removed
```

Ein Neustart der Sandbox darf widerrufene Autorität nicht automatisch wiederherstellen.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
SandboxID
Workload Identity
Security Domain
Granted Capabilities
Resource Limits
Delegation Relations
Revoked Capabilities
State
```

Geheime Tokens dürfen dabei nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Workloads in capability-basierten Sandboxes ausführen können.
2. Sandboxes MÜSSEN standardmäßig minimale Autorität besitzen.
3. Ressourcen-Zugriff MUSS über explizite Capabilities kontrollierbar sein.
4. Benutzerautorität DARF NICHT automatisch vollständig an Anwendungen übertragen werden.
5. Dateisystem-, Netzwerk-, IPC- und Geräteautorität MÜSSEN getrennt kontrollierbar sein.
6. Child Workloads DÜRFEN Parent-Autorität nicht automatisch vollständig erben.
7. Dynamisch gewährte Capabilities MÜSSEN kontrolliert und nachvollziehbar sein.
8. Revocation MUSS auch innerhalb laufender Sandboxes wirksam werden können.
9. Eine kompromittierte Sandbox SOLL auf ihre explizit verfügbare Autorität begrenzt bleiben.
10. Sandbox-Zustände und Capability Sets SOLLEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-DELEGATION-0001`
- `NPSPEC-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-CAPABILITY-ISOLATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-SANDBOX-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-CAPABILITY-0011`

## Ergebnis

```text
Workload
   ↓
Capability Sandbox
   ↓
Minimal Explicit Authority
   ↓
Controlled Resources
   ↓
Bounded Security Impact
```

NovaOS erhält damit ein capability-basiertes Sandbox-Modell, bei dem Anwendungen und andere Workloads nur die ausdrücklich benötigte Autorität erhalten und eine Kompromittierung möglichst auf diesen begrenzten Autoritätsraum beschränkt bleibt.