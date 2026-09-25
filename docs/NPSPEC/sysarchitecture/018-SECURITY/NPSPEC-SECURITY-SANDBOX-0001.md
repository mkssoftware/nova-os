# NPSPEC-SECURITY-SANDBOX-0001 – Nova Security Sandbox

## Status

Angenommen

## Kategorie

Security / Isolation / Sandbox

## Zweck

NovaOS definiert Sandboxing als kontrollierte Ausführungsumgebung für Anwendungen, Dienste und andere potenziell nicht vertrauenswürdige Komponenten.

```text
Component
    ↓
Sandbox
    ↓
Explicit Capabilities
    ↓
Allowed Resources
```

Die Sandbox begrenzt den möglichen Schaden bei Fehlern oder Kompromittierung.

## Grundprinzipien

```text
Running ≠ Trusted
Installed ≠ Privileged
Sandbox ≠ Permission
Isolation ≠ Authorization
Escape ≠ Acceptable Failure
```

Eine Sandbox folgt grundsätzlich `Least Privilege` und `Default Deny`.

## Sandbox-Modell

Eine Sandbox besitzt mindestens:

```text
Sandbox
├── SandboxID
├── SubjectID
├── Security Domain
├── Capabilities
├── Resource Limits
└── State
```

Optional:

```text
Filesystem View
Network Policy
Device Access
IPC Policy
ExecutionContract
Lifetime
```

## Isolation

Eine Sandbox muss Zugriffe begrenzen können auf:

```text
Memory
Storage
IPC
Network
Devices
System Services
Configuration
```

Nicht explizit freigegebene Ressourcen sollen nicht erreichbar sein.

## Capability-Integration

Zugriffe erfolgen über explizite Capabilities.

```text
Sandbox
   ↓
Capability
   ↓
Authorized Resource
```

Die Kenntnis eines Pfades, Dienstnamens, Handles oder einer Netzwerkadresse erzeugt keine Autorität.

## Storage

Eine Anwendung soll standardmäßig nur ihre eigenen Daten und explizit freigegebene Objekte sehen.

```text
Application Data
Selected User Files
Explicit Shared Resources
```

Eine vollständige Sicht auf das Benutzerdateisystem soll nicht automatisch bestehen.

## Netzwerk

Netzwerkzugriff muss separat kontrollierbar sein.

```text
No Network
Local Only
Selected Services
Internet
```

Network Intent, Firewall, Namespace und Sovereignty bleiben zusätzliche Sicherheitsmechanismen.

## IPC und Dienste

Eine Sandbox darf nur autorisierte IPC-Endpunkte und Systemdienste verwenden.

```text
Sandbox
   ↓
Allowed IPC Capabilities
   ↓
System Service
```

Andere laufende Prozesse dürfen nicht allein aufgrund ihrer Sichtbarkeit erreichbar sein.

## Geräte

Zugriff auf Geräte wie:

```text
Camera
Microphone
GPU
USB
Sensors
```

muss explizit freigegeben werden können.

Gerätezugriff soll möglichst auf die benötigte Funktion beschränkt werden.

## Ressourcenbegrenzung

Eine Sandbox muss begrenzbar sein hinsichtlich:

```text
Memory
CPU
Storage
Processes / Tasks
IPC
Network
Handles
```

Damit darf eine Anwendung nicht unbegrenzt Systemressourcen verbrauchen.

## Privilegienänderung

Eine Sandbox darf ihre eigenen Sicherheitsgrenzen nicht selbstständig erweitern.

Zusätzliche Rechte benötigen:

```text
Explicit Request
      ↓
Policy / User Decision
      ↓
Capability Grant
```

Temporäre Rechte sollen nach ihrer Verwendung wieder entzogen werden können.

## Fehler und Beendigung

Eine Sandbox muss unabhängig beendet werden können.

```text
Running
   ↓
Violation / Failure / User Action
   ↓
Terminate
   ↓
Resource Cleanup
```

Fehler innerhalb einer Sandbox sollen andere Sicherheitsdomänen möglichst nicht beeinträchtigen.

## Introspection

NovaOS soll mindestens darstellen können:

```text
SandboxID
SubjectID
Security Domain
Granted Capabilities
Resource Limits
Network Access
Device Access
State
Violations
```

## Normative Anforderungen

1. NovaOS MUSS isolierte Sandboxes unterstützen.
2. Sandboxes MÜSSEN nach `Default Deny` betreibbar sein.
3. Ressourcenzugriffe SOLLEN über explizite Capabilities erfolgen.
4. Eine Sandbox DARF ihre eigenen Rechte NICHT selbstständig erweitern.
5. Speicher-, Storage-, IPC-, Netzwerk- und Gerätezugriffe MÜSSEN begrenzbar sein.
6. Ressourcenverbrauch MUSS pro Sandbox begrenzbar sein.
7. Sandboxes MÜSSEN unabhängig beendet und bereinigt werden können.
8. Sandbox-Grenzen MÜSSEN mit MAC und Least Privilege kombinierbar sein.
9. Sicherheitsverletzungen SOLLEN auditierbar sein.
10. Sandbox-Zustand und effektive Autorität SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-MAC-0001`
- `NPSPEC-SECURITY-LEASTPRIVILEGE-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-DRIVER-SANDBOX-0001`
- `NPSPEC-NETWORK-NAMESPACE-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-SECURITY-0007`

## Ergebnis

```text
Untrusted Component
        ↓
Isolated Sandbox
        ↓
Minimum Capabilities
        +
Resource Limits
        ↓
Controlled Execution
```

NovaOS erhält damit eine systemweite Sandbox-Infrastruktur, die Anwendungen und Dienste auf explizit freigegebene Ressourcen und minimale Autorität beschränkt.