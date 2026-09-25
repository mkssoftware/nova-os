# NPSPEC-SECURITY-LEASTPRIVILEGE-0001 – Nova Least Privilege

## Status

Angenommen

## Kategorie

Security / Authorization / Least Privilege

## Zweck

NovaOS definiert Least Privilege als systemweites Sicherheitsprinzip.

Jede Komponente erhält nur die Autorität und Ressourcen, die sie für ihre aktuelle Aufgabe tatsächlich benötigt.

```text
Required Task
     ↓
Minimum Authority
     ↓
Minimum Resources
```

## Grundprinzipien

```text
Identity ≠ Authority
Installed ≠ Authorized
Needed Once ≠ Needed Forever
Administrator ≠ Unlimited Authority
More Permission ≠ More Convenience by Default
```

Berechtigungen sollen minimal, explizit, zweckgebunden und möglichst zeitlich begrenzt sein.

## Autoritätsmodell

Die effektive Autorität ergibt sich aus:

```text
Capabilities
+ DAC
+ MAC
+ RBAC
+ ABAC
+ Context
    ↓
Effective Authority
```

Keine einzelne Policy-Schicht darf unnötig umfassende Autorität erzeugen.

## Minimale Capabilities

Komponenten sollen nur die benötigten Capabilities erhalten.

Beispiel:

```text
Image Viewer
├── Read selected image
├── GPU rendering
└── Window output
```

Nicht automatisch:

```text
All Files
Network
Microphone
Camera
System Configuration
```

## Zeitliche Begrenzung

Autorität soll nur so lange bestehen, wie sie benötigt wird.

```text
Acquire
   ↓
Use
   ↓
Release / Revoke
```

Temporäre Aufgaben sollen temporäre Capabilities verwenden können.

## Rechteabsenkung

Komponenten müssen nicht mehr benötigte Rechte freiwillig oder automatisch abgeben können.

```text
Initialization Rights
        ↓
Initialization
        ↓
Drop Rights
        ↓
Runtime Rights
```

Dies ist insbesondere für Systemdienste und Treiber vorgesehen.

## Prozessstart

Neue Prozesse dürfen nicht automatisch sämtliche Rechte ihres Erzeugers übernehmen.

```text
Parent Authority
      ↓
Explicit Selection
      ↓
Child Authority
```

Vererbte Autorität muss kontrolliert und begrenzbar sein.

## Treiber und Dienste

Treiber und Systemdienste erhalten ausschließlich Zugriff auf benötigte Ressourcen.

Beispiel:

```text
Audio Driver
├── Audio Device
├── Required DMA
└── Required Interrupts
```

Ein Treiber erhält dadurch nicht automatisch Zugriff auf andere Geräte, Storage oder Netzwerk.

## Anwendungen

Anwendungen sollen Ressourcen bei Bedarf anfordern.

```text
Application
    ↓
Request Capability
    ↓
Policy / User Decision
    ↓
Limited Access
```

Dauerhafte globale Berechtigungen sollen vermieden werden, wenn eine engere Freigabe möglich ist.

## Privilegienausweitung

Temporäre Privilegienausweitung muss:

```text
Explicit
Authorized
Scoped
Auditable
Reversible
```

sein.

Eine Anwendung darf ihre Autorität nicht selbstständig erweitern.

## Ressourcen

Least Privilege gilt auch für Ressourcen:

```text
Memory
CPU
Storage
Network
IPC
Devices
Handles
```

Autorität und Ressourcenbudget sollen gemeinsam begrenzt werden können.

## Introspection

NovaOS soll mindestens darstellen können:

```text
SubjectID
Granted Authority
Used Authority
Delegated Authority
Temporary Authority
Unused Authority
```

Damit können unnötige Berechtigungen erkannt und reduziert werden.

## Normative Anforderungen

1. NovaOS MUSS Least Privilege als systemweites Sicherheitsprinzip anwenden.
2. Komponenten SOLLEN nur die für ihre Aufgabe erforderliche Autorität erhalten.
3. Neue Prozesse DÜRFEN Rechte NICHT unkontrolliert erben.
4. Nicht mehr benötigte Rechte MÜSSEN abgebbar oder widerrufbar sein.
5. Temporäre Autorität SOLL zeitlich und funktional begrenzt werden.
6. Anwendungen DÜRFEN ihre Autorität NICHT selbstständig erweitern.
7. Treiber und Dienste SOLLEN auf benötigte Ressourcen beschränkt werden.
8. Privilegienausweitungen MÜSSEN explizit autorisiert sein.
9. Least Privilege SOLL auch auf Ressourcenbudgets angewendet werden.
10. Effektive und ungenutzte Autorität SOLL introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-DAC-0001`
- `NPSPEC-SECURITY-MAC-0001`
- `NPSPEC-SECURITY-RBAC-0001`
- `NPSPEC-SECURITY-ABAC-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-SECURITY-0006`

## Ergebnis

```text
Task
 ↓
Required Authority
 ↓
Minimum Capabilities + Policy
 ↓
Limited Lifetime
 ↓
Release
```

NovaOS begrenzt Autorität damit auf das tatsächlich erforderliche Minimum und reduziert die Auswirkungen fehlerhafter oder kompromittierter Komponenten.