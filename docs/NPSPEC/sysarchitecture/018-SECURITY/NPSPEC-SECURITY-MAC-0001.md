# NPSPEC-SECURITY-MAC-0001 – Nova Mandatory Access Control

## Status

Angenommen

## Kategorie

Security / Authorization / Mandatory Access Control

## Zweck

NovaOS definiert Mandatory Access Control (MAC) als systemweit erzwingbare Sicherheitsschicht für Zugriffe, die weder Benutzer, Eigentümer noch Anwendungen eigenständig umgehen oder abschwächen dürfen.

```text
Subject
   ↓
Capability + DAC
   ↓
MAC Policy
   ↓
Allow / Deny
```

MAC ergänzt Capability Security und DAC um verbindliche Systemregeln.

## Grundprinzip

```text
Ownership ≠ Policy Authority
DAC Allow ≠ MAC Allow
Capability ≠ MAC Bypass
Administrator ≠ Automatic Exemption
Visible ≠ Accessible
```

MAC definiert harte Sicherheitsgrenzen.

Eine Operation, die durch MAC verboten wird, darf nicht allein durch DAC-Rechte oder eine gewöhnliche Capability erlaubt werden.

## Verhältnis zu Capabilities und DAC

NovaOS kombiniert:

```text
Capability
    +
DAC
    +
MAC
    ↓
Authorization Decision
```

Dabei erfüllen die Mechanismen unterschiedliche Aufgaben:

```text
Capability → konkrete delegierte Autorität
DAC        → eigentümer-/benutzerbestimmte Policy
MAC        → verbindliche Systempolicy
```

MAC besitzt gegenüber normalen DAC-Regeln Vorrang.

## MAC-Modell

Eine MAC-Policy arbeitet mindestens mit:

```text
MACContext
├── Subject Label
├── Object Label
├── Operation
├── Policy
└── Decision
```

Optional:

```text
Security Domain
Trust Level
Integrity Level
Confidentiality Level
Compartment
Execution Context
Device Context
Network Context
```

## Security Labels

Subjects und geschützte Objekte dürfen Security Labels besitzen.

```text
Subject
└── SecurityLabel

Object
└── SecurityLabel
```

Labels sind sicherheitsrelevante Metadaten und dürfen nicht von unautorisierten Komponenten verändert werden.

## Subjects

MAC muss mindestens auf folgende Subjects anwendbar sein können:

```text
Process
Service
Application
Driver
User Context
Remote Principal
```

Die Benutzeridentität allein bestimmt nicht die MAC-Berechtigung eines Prozesses.

## Objekte

MAC muss auf unterschiedliche Ressourcen anwendbar sein können:

```text
Storage Object
Memory Object
IPC Endpoint
Service
Device
Driver
Network Resource
Configuration
Security Object
```

Das Modell muss für neue Ressourcentypen erweiterbar bleiben.

## Security Domains

NovaOS darf Ressourcen und Subjects Sicherheitsdomänen zuordnen.

```text
Application Domain
System Service Domain
Driver Domain
User Domain
Recovery Domain
Security Domain
```

Kommunikation zwischen Domains kann durch explizite MAC-Regeln eingeschränkt werden.

## Vertraulichkeit

MAC darf Confidentiality Levels unterstützen.

Beispiel:

```text
Public
Internal
Confidential
Restricted
```

Zusätzliche Compartments dürfen verwendet werden:

```text
Confidential
├── Project A
└── Project B
```

Gleiche Sicherheitsstufe bedeutet damit nicht automatisch gegenseitigen Zugriff.

## Integrität

MAC darf Integritätsstufen getrennt von Vertraulichkeit behandeln.

Beispiel:

```text
Untrusted
User
Trusted
System
Critical
```

Nicht vertrauenswürdige Prozesse dürfen dadurch beispielsweise daran gehindert werden, sicherheitskritische Systemobjekte zu verändern.

```text
Confidentiality ≠ Integrity
```

## Policy-Auswertung

Eine MAC-Entscheidung kann berücksichtigen:

```text
Subject Label
Object Label
Requested Operation
Security Domain
Integrity
Confidentiality
Compartment
System State
```

Beispiel:

```text
Subject
   ↓
MAC Evaluation
   ↓
Allow / Deny
```

`Deny` muss verbindlich durchgesetzt werden.

## Capability-Integration

Eine gültige Capability bleibt notwendig, sofern die Ressource capability-geschützt ist.

```text
Valid Capability
      +
MAC Allow
      ↓
Authorized Operation
```

Eine Capability darf MAC nicht automatisch umgehen.

```text
Capability ≠ MAC Override
```

Spezielle MAC-Verwaltungsrechte müssen selbst explizit autorisiert sein.

## DAC-Integration

Ein Eigentümer darf DAC-Regeln verwalten, aber keine MAC-Regeln abschwächen.

```text
Owner grants Read
      ↓
DAC Allow
      ↓
MAC Deny
      ↓
Access Denied
```

Damit können Benutzer ihre eigenen Ressourcen freigeben, ohne systemweite Sicherheitsgrenzen aufzuheben.

## Prozesse und Anwendungen

Prozesse sollen beim Start einen definierten MAC-Kontext erhalten.

```text
Executable
   +
Launcher Context
   +
Security Policy
   ↓
Process Security Domain
```

Ein Prozess darf seinen eigenen MAC-Kontext nicht eigenständig auf eine privilegiertere Stufe erhöhen.

## IPC

MAC muss auf IPC-Kommunikation anwendbar sein.

```text
Process A
   ↓
IPC Endpoint
   ↓
MAC Check
   ↓
Process B
```

Eine gültige IPC-Capability allein muss keine Kommunikation zwischen verbotenen Sicherheitsdomänen erlauben.

## Storage

Storage-Objekte dürfen MAC-Labels besitzen.

```text
ObjectID
├── Capability Policy
├── DAC Policy
└── MAC Label
```

Labels bleiben an die logische Objektidentität gebunden und nicht lediglich an einen Dateipfad.

```text
Path ≠ Security Identity
```

## Netzwerk

MAC darf Netzwerkzugriffe anhand von Sicherheitsdomänen begrenzen.

Beispiele:

```text
Application → Internet
Service → Local Network
Driver → No Network
Recovery Environment → Restricted Network
```

MAC ergänzt dabei:

```text
Capabilities
Firewall
Network Namespace
Network Intent
Sovereignty
```

Keiner dieser Mechanismen ersetzt die anderen automatisch.

## Geräte und Treiber

Hardwarezugriffe dürfen durch MAC zusätzlich eingeschränkt werden.

```text
Driver Domain
    ↓
MAC Policy
    ↓
Device
```

Ein kompromittierter Treiber darf dadurch nicht automatisch Zugriff auf alle anderen Geräte oder Systemressourcen erhalten.

## Policy-Änderungen

MAC-Policies dürfen nur durch explizit autorisierte Sicherheitskomponenten verändert werden.

```text
Policy Update
    ↓
Authorization
    ↓
Validation
    ↓
Atomic Activation
```

Fehlerhafte oder unvollständige Policies dürfen nicht zu einem unkontrollierten Sicherheitszustand führen.

## Default Policy

NovaOS soll nach dem Prinzip:

```text
Default Deny
```

arbeiten können.

Fehlt für eine sicherheitskritische Operation eine ausreichende Autorisierung, soll sie verweigert werden.

```text
Unknown ≠ Allowed
```

## Recovery und Safe Mode

Recovery- oder Safe-Mode-Umgebungen dürfen spezielle MAC-Policies besitzen.

Diese dürfen jedoch nicht automatisch vollständige uneingeschränkte Autorität erzeugen.

Sicherheitskritische Recovery-Operationen müssen weiterhin explizit autorisiert werden.

## Audit

MAC-Entscheidungen sollen auditierbar sein.

Mindestens erfassbar:

```text
Subject
Object
Operation
Decision
Policy Rule
Timestamp
Reason
```

Sensible Inhalte dürfen dabei nicht unnötig protokolliert werden.

## Ressourcensteuerung

NovaOS muss Grenzen unterstützen für:

```text
MAC Rules
Security Labels
Compartments
Security Domains
Policy Cache
Audit Events
Evaluation Cost
```

Policy-Auswertung muss deterministisch und ausreichend effizient für häufige Sicherheitsprüfungen sein.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
SubjectID
ObjectID
Subject Label
Object Label
Security Domain
Requested Operation
Decision
Matched Policy
Decision Reason
```

Introspection selbst muss wiederum MAC- und Capability-geschützt sein.

## Normative Anforderungen

1. NovaOS MUSS Mandatory Access Control als verbindliche Sicherheitsschicht unterstützen können.
2. MAC MUSS von Capability Security und DAC getrennt modelliert werden.
3. Eine DAC-Allow-Entscheidung DARF ein MAC-Deny NICHT überschreiben.
4. Eine gewöhnliche Capability DARF MAC NICHT umgehen.
5. Subjects und Objekte MÜSSEN Security Labels besitzen können.
6. Confidentiality und Integrity MÜSSEN getrennt modellierbar sein.
7. Sicherheitsdomänen und Compartments SOLLEN unterstützt werden.
8. Prozesse DÜRFEN ihren MAC-Kontext NICHT eigenständig privilegieren.
9. MAC MUSS auf IPC, Storage, Netzwerk und Gerätezugriffe anwendbar sein können.
10. MAC-Policy-Änderungen MÜSSEN explizit autorisiert und kontrolliert aktiviert werden.
11. Unsichere oder unbekannte Zustände DÜRFEN NICHT implizit als erlaubt behandelt werden.
12. MAC-Entscheidungen und ihre Ursachen SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-DAC-0001`
- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-MEMORY-PROTECTION-0001`
- `NPSPEC-DRIVER-ISOLATION-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-NAMESPACE-0001`
- `NPSPEC-NETWORK-FIREWALL-0001`
- `NPSPEC-NETWORK-NAMESPACE-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-SECURITY-0003`

## Ergebnis

```text
Capability Authority
        +
DAC Policy
        +
Mandatory Security Policy
        ↓
MAC Evaluation
        ↓
Allow / Deny
        ↓
Protected Operation
```

NovaOS erhält damit eine verbindliche Mandatory-Access-Control-Schicht, die systemweite Sicherheitsgrenzen unabhängig von Benutzer- und Eigentümerentscheidungen durchsetzt und zusammen mit Capabilities und DAC ein mehrschichtiges Autorisierungsmodell bildet.