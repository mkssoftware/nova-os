# NPSPEC-SECURITY-CAPABILITY-0001 – Nova Capability Security

## Status

Angenommen

## Kategorie

Security / Authorization / Capability-Based Security

## Zweck

NovaOS definiert Capabilities als grundlegenden Mechanismus zur Autorisierung von Zugriffen auf Systemressourcen.

```text
Subject
   ↓
Capability
   ↓
Rights + Constraints
   ↓
System Object
```

Eine Capability verbindet die Identität einer Ressource mit explizit delegierter Autorität.

Der Besitz oder die Sichtbarkeit einer Ressource allein erzeugt keine Zugriffsberechtigung.

## Grundprinzip

```text
Identity ≠ Authority
Object Reference ≠ Permission
Visibility ≠ Access
Capability ≠ Resource
Capability Possession ≠ Unlimited Authority
```

NovaOS folgt dem Prinzip der minimal notwendigen Autorität:

```text
Least Authority
```

Komponenten erhalten nur die Rechte, die sie für ihre konkrete Aufgabe benötigen.

## Capability-Modell

Eine Capability besitzt mindestens:

```text
Capability
├── CapabilityID
├── TargetID
├── Rights
├── Holder
├── Issuer
└── State
```

Optional:

```text
Constraints
Expiration
Delegation Policy
Security Context
Resource Budget
Audit Policy
Revocation Information
```

`TargetID` referenziert die logische Identität einer Ressource und bleibt unabhängig von Speicheradresse, Pfad oder physischem Standort.

## Geschützte Ressourcen

Capabilities dürfen unter anderem folgende Ressourcen schützen:

```text
Process
Thread / Task
Memory Object
File / Storage Object
Device
Driver
IPC Endpoint
Network Resource
Service
Capability
System Configuration
```

Neue Ressourcentypen müssen in das Capability-Modell integrierbar sein.

## Rechte

Eine Capability enthält explizite Rechte.

Beispiele:

```text
Read
Write
Execute
Create
Delete
Invoke
Map
Share
Delegate
Administer
```

Ressourcentypen dürfen zusätzliche spezifische Rechte definieren.

Beispiel:

```text
File Capability
├── Read
├── Write
├── MetadataRead
└── MetadataWrite
```

## Kein Ambient Authority

Prozesse sollen nicht automatisch umfassende Rechte aufgrund ihrer bloßen Identität erhalten.

```text
Process
   ↓
Explicit Capabilities
   ↓
Allowed Resources
```

Globale implizite Autorität soll vermieden werden.

Insbesondere soll kein universelles `root`-Prinzip notwendig sein, das automatisch Zugriff auf sämtliche Ressourcen besitzt.

## Capability-Prüfung

Vor einer geschützten Operation muss geprüft werden:

```text
Capability Valid?
      ↓
Target Correct?
      ↓
Required Right Present?
      ↓
Constraints Valid?
      ↓
Security Policy Valid?
      ↓
Allow / Deny
```

Eine Objekt-ID allein darf diese Prüfung nicht ersetzen.

## Delegation

Capabilities dürfen kontrolliert weitergegeben werden.

```text
Holder A
   ↓
Delegate
   ↓
Holder B
```

Delegation darf nur erfolgen, wenn die ursprüngliche Capability dies erlaubt.

Dabei gilt:

```text
Delegated Authority ⊆ Original Authority
```

Delegation darf keine neuen Rechte erzeugen.

## Attenuation

Bei Delegation dürfen Rechte weiter eingeschränkt werden.

```text
Read + Write + Execute
          ↓
        Read
```

Zusätzliche Einschränkungen dürfen umfassen:

```text
Time Limit
Operation Limit
Resource Limit
Usage Count
Namespace Constraint
Security Constraint
```

Eine abgeschwächte Capability darf nicht eigenständig wieder erweitert werden.

## Capability Transfer

Capabilities dürfen über kontrollierte IPC-Mechanismen übertragen werden.

```text
Process A
   ↓
IPC Capability Transfer
   ↓
Kernel Validation
   ↓
Process B
```

Die Übertragung muss explizit sein.

Das Kopieren einer normalen Speicherrepräsentation darf keine gültige Capability erzeugen.

## Capability Forgery

Capabilities müssen gegen Fälschung geschützt sein.

Ein Prozess darf keine gültige Capability erzeugen können, indem er:

```text
CapabilityID errät
Handle manipuliert
Speicher kopiert
Objektadresse kennt
Ressourcenname kennt
```

Die interne Repräsentation muss durch Kernel- oder entsprechend vertrauenswürdige Sicherheitsmechanismen geschützt werden.

## Revocation

Capabilities müssen widerrufbar sein können, wenn die jeweilige Capability-Klasse dies verlangt.

```text
Active
  ↓
Revocation
  ↓
Revoked
```

Mögliche Zustände:

```text
Valid
Expired
Revoked
Invalid
```

Nach erfolgreichem Widerruf dürfen neue geschützte Operationen nicht mehr autorisiert werden.

## Lebensdauer

Capabilities besitzen eine kontrollierte Lebensdauer.

```text
Created
   ↓
Active
   ↓
Delegated / Attenuated
   ↓
Expired / Revoked / Released
```

Freigegebene Capabilities müssen sicher ungültig werden.

Veraltete Handles dürfen nicht unbeabsichtigt auf neue Ressourcen zeigen.

## Prozesse

Neue Prozesse erhalten Capabilities explizit beim Start.

```text
Parent / System
      ↓
Selected Capabilities
      ↓
New Process
```

Ein Child-Prozess darf nicht automatisch sämtliche Rechte seines Erzeugers übernehmen.

## IPC

IPC-Endpunkte müssen capability-geschützt sein können.

```text
EndpointID ≠ Permission

EndpointID
   +
Capability
   ↓
Authorized IPC
```

Dies gilt für lokale und verteilte Kommunikation.

## Dateien und Storage

Dateizugriff soll nicht allein vom Pfad abhängen.

```text
Path / ObjectID
      ≠
Authority
```

Nach erfolgreicher Auflösung und Autorisierung darf ein capability-basiertes Handle für weitere Operationen verwendet werden.

## Geräte und Treiber

Hardwarezugriffe müssen ebenfalls capability-basiert begrenzbar sein.

Beispiele:

```text
Device Access
DMA Access
Interrupt Access
I/O Port Access
Memory Mapping
```

Treiber erhalten nur die für ihre Aufgabe erforderlichen Hardware-Capabilities.

## Netzwerk

Netzwerkerreichbarkeit erzeugt keine Netzwerkautorität.

```text
Reachable ≠ Allowed
```

Capabilities dürfen Netzwerkoperationen begrenzen auf:

```text
Connect
Listen
Bind
Send
Receive
Configure
```

Firewall, Network Intent und Sovereignty bleiben zusätzliche Policy-Schichten.

## Remote Capabilities

Capabilities dürfen kontrolliert über Systemgrenzen erweitert werden.

Dafür gilt:

`NPSPEC-DISTCOMM-REMOTECAPABILITY-0001`

Lokale Capability-Repräsentationen dürfen nicht ungeprüft als Remote Capability verwendet werden.

## Security Context

Capabilities dürfen mit einem Security Context verbunden sein.

```text
Capability
   +
Security Context
   ↓
Authorization Decision
```

Security Context und Capability bleiben getrennte Konzepte.

```text
Identity ≠ Authority
```

## Ressourcensteuerung

NovaOS muss Grenzen unterstützen für:

```text
Capabilities per Process
Delegation Depth
Capability Tables
Revocation State
Capability Transfers
Validation Operations
```

Capability Exhaustion darf nicht zur unkontrollierten Ressourcenbelegung führen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
CapabilityID
TargetID
Holder
Issuer
Rights
Constraints
State
Delegation Relationship
Expiration
Revocation State
```

Geheimes oder fälschungsrelevantes Capability-Material darf nicht über normale Introspection sichtbar sein.

## Normative Anforderungen

1. NovaOS MUSS capability-basierte Autorisierung als grundlegenden Sicherheitsmechanismus unterstützen.
2. Ressourcenidentität und Autorität MÜSSEN getrennt bleiben.
3. Geschützte Operationen MÜSSEN explizite Rechte prüfen können.
4. Capabilities MÜSSEN gegen Fälschung geschützt sein.
5. Capability-Transfer MUSS kontrolliert erfolgen.
6. Delegation DARF keine zusätzlichen Rechte erzeugen.
7. Capability Attenuation MUSS unterstützt werden können.
8. Prozesse SOLLEN nur die für ihre Aufgabe notwendigen Capabilities erhalten.
9. Objektname, Pfad, Adresse oder ID DÜRFEN NICHT automatisch Autorität erzeugen.
10. Capabilities MÜSSEN kontrolliert freigegeben und bei Bedarf widerrufen werden können.
11. Capability-Ressourcen MÜSSEN begrenzt sein.
12. Capability-Beziehungen und Zustände SOLLEN sicher introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-DISTCOMM-REMOTECAPABILITY-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-DRIVER-CAPABILITY-0001`
- `NPSPEC-MEMORY-PROTECTION-0001`
- `NPSPEC-STORAGE-NAMESPACE-0001`
- `NPSPEC-NETWORK-FIREWALL-0001`
- `NPSPEC-NETWORK-NAMESPACE-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-SECURITY-0001`

## Ergebnis

```text
Resource Identity
       +
Explicit Capability
       ↓
Rights
       +
Constraints
       ↓
Authorization
       ↓
Allowed Operation
```

NovaOS erhält damit ein systemweites Capability-Sicherheitsmodell, bei dem Autorität explizit, minimal, delegierbar, einschränkbar und widerrufbar ist, anstatt implizit aus Identität, Pfaden, Netzwerkadressen oder globalen privilegierten Benutzerrollen zu entstehen.