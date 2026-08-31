# ADR-ARCH-0016 – Fail-Safe Defaults

## Status

Angenommen

## Kategorie

Systemarchitektur / Sicherheit / Resilienz / Policy / Fehlerbehandlung / Recovery

## Kontext

NovaOS ist als capability-basiertes, adaptives, deklaratives und stark automatisiertes Betriebssystem konzipiert.

Viele Systementscheidungen werden nicht ausschließlich statisch durch Anwendungen getroffen, sondern entstehen aus dem Zusammenspiel von:

- Declarative System Model,
- Execution Contracts,
- Component Contracts,
- Capability Resolution,
- Provider Selection,
- Resource Economy,
- Security Policies,
- Trust Policies,
- Data Sovereignty,
- Information Flow,
- Location Policies,
- Graceful Degradation,
- Self-Healing,
- Hot Replacement,
- automatischer Algorithmusauswahl,
- adaptiver Optimierung.

Dadurch entstehen Situationen, in denen das System keine vollständig sichere oder eindeutige Entscheidung treffen kann.

Beispiele:

```text
Policy unavailable
```

```text
Trust state unknown
```

```text
Capability permission ambiguous
```

```text
Configuration corrupted
```

```text
Contract cannot be validated
```

```text
Provider identity cannot be verified
```

```text
Resource state unknown
```

```text
Transaction recovery incomplete
```

```text
Security metadata missing
```

```text
Data sovereignty classification unknown
```

```text
Execution location cannot be verified
```

Ein gefährliches System könnte in solchen Situationen nach dem Prinzip handeln:

```text
Unknown
   ↓
Assume Allowed
```

oder:

```text
Policy unavailable
   ↓
Continue with previous assumptions
```

oder:

```text
Verification failed
   ↓
Ignore verification
```

Für NovaOS ist dieses Verhalten nicht akzeptabel.

Die Architektur benötigt deshalb ein systemweites Prinzip:

```text
Fail-Safe Defaults
```

Das bedeutet:

> Wenn NovaOS nicht sicher feststellen kann, dass eine sicherheits-, integritäts- oder policyrelevante Operation zulässig ist, wird standardmäßig der sicherere Zustand gewählt.

Das klassische Sicherheitsprinzip:

```text
Default Deny
```

ist dabei ein wichtiger Teil, aber nicht die vollständige Definition.

Nicht jede Systementscheidung ist eine Autorisierungsentscheidung.

Beispielsweise kann ein Grafiktreiber ausfallen.

Die sichere Reaktion ist nicht zwingend:

```text
Stop System
```

sondern möglicherweise:

```text
Fallback to Safe Framebuffer
```

Ebenso kann ein Performance-Provider ausfallen.

Die sichere Reaktion kann sein:

```text
Use slower verified provider
```

Fail-Safe Defaults bedeutet deshalb für NovaOS:

```text
Unknown or invalid state
        ↓
Select predefined safe state
```

und nicht pauschal:

```text
Unknown
   ↓
Shutdown
```

---

# Entscheidung

NovaOS MUSS Fail-Safe Defaults als systemweites Architekturprinzip verwenden.

Wenn eine sicherheits-, integritäts-, vertrauens-, sovereignty-, policy- oder korrektheitsrelevante Entscheidung nicht zuverlässig getroffen werden kann, MUSS das System auf einen vorher definierten sicheren Zustand zurückfallen.

Das Grundmodell lautet:

```text
Requested Operation
        ↓
Resolve Context
        ↓
Validate Contracts
        ↓
Validate Authorization
        ↓
Validate Trust
        ↓
Validate Integrity
        ↓
Validate Policy
        ↓
Decision
     ┌──┴──┐
     │     │
   Valid  Unknown/Invalid
     │     │
     ▼     ▼
 Execute  Safe Default
```

Das zentrale Prinzip lautet:

```text
Permission requires proof.

Uncertainty does not grant authority.
```

Für allgemeine Systemzustände gilt:

```text
When uncertain,
move toward the safest valid state.
```

---

# Fail-Safe ist nicht gleich Fail-Stop

NovaOS unterscheidet ausdrücklich zwischen:

```text
Fail-Safe
```

und:

```text
Fail-Stop
```

Fail-Safe bedeutet:

```text
Failure
   ↓
Safe State
```

Fail-Stop bedeutet:

```text
Failure
   ↓
Stop Operation
```

Fail-Stop kann ein Fail-Safe-Verhalten sein, ist aber nicht die einzige Möglichkeit.

---

# Safe State

Ein Safe State ist ein Zustand, bei dem die relevanten Hard Constraints weiterhin eingehalten werden.

Dazu gehören insbesondere:

```text
Safety
Security
Data Integrity
Data Sovereignty
Information Flow
Trust Requirements
Correctness
Hard Realtime Constraints
Determinism Requirements
Explicit Hard Contracts
```

---

# Kein universeller Safe State

NovaOS definiert keinen einzelnen universellen Systemzustand namens:

```text
Safe
```

Der sichere Zustand ist abhängig von:

- Komponente,
- Capability,
- Operation,
- Daten,
- Contract,
- Failure Domain,
- Security Context,
- Trust Context,
- Ressourcen,
- Hardware,
- aktuellem Systemzustand.

---

# Safe Default

Jede relevante Komponente SOLL einen expliziten Safe Default definieren.

Beispiele:

```text
Authorization:
    Deny
```

```text
Unknown executable:
    Do not execute
```

```text
Unknown remote provider:
    Do not delegate
```

```text
Unknown data sovereignty:
    Keep local
```

```text
Unknown write state:
    Do not overwrite
```

```text
Graphics acceleration unavailable:
    Safe framebuffer
```

```text
Optional AI unavailable:
    deterministic non-AI fallback
```

---

# Default Deny

Für Autorisierung gilt grundsätzlich:

```text
No explicit authorization
        ↓
Deny
```

Nicht:

```text
No explicit denial
        ↓
Allow
```

---

# Capability Security

NovaOS verwendet Capability-Based Security.

Der Besitz oder die erfolgreiche Delegation einer gültigen Capability stellt die positive Autorisierung dar.

Fehlt diese:

```text
Capability Missing
       ↓
Access Denied
```

---

# Unknown Capability

Eine unbekannte oder nicht validierbare Capability darf nicht als gültige Berechtigung interpretiert werden.

```text
Unknown Capability
       ↓
Invalid for Authorization
```

---

# Invalid Capability

Eine:

- abgelaufene,
- widerrufene,
- beschädigte,
- falsch signierte,
- nicht verifizierbare

Capability ist nicht autorisierend.

---

# Capability Attenuation

Wenn Capability-Rechte reduziert werden können, ist bei unsicherer Interpretation die restriktivere gültige Interpretation zu bevorzugen.

---

# Least Privilege

Fail-Safe Defaults ergänzen das Least-Privilege-Prinzip.

```text
Grant only what is proven necessary and authorized.
```

---

# Permission Inheritance

Berechtigungen dürfen nicht aufgrund unklarer Vererbung erweitert werden.

Bei Ambiguität gilt:

```text
More Restrictive
```

vor:

```text
More Permissive
```

---

# Explicit Allow

Sicherheitskritische Aktionen benötigen einen explizit positiven Entscheidungsweg.

Konzeptionell:

```text
Allowed =
    IdentityValid
    AND CapabilityValid
    AND PolicyAllows
    AND TrustSufficient
    AND ContractValid
```

---

# Keine Negativlogik als alleinige Autorisierung

Nicht:

```text
if not denied:
    allow
```

sondern:

```text
if explicitly authorized:
    allow
else:
    deny
```

---

# Nova.Trust

Diese ADR integriert:

```text
Nova.Trust
```

mit dem Modell:

```text
Identity
   ↓
Signature
   ↓
Provenance
   ↓
Permission
   ↓
Capability
   ↓
Audit
```

---

# Unknown Trust

Wenn ein Contract ein bestimmtes Trust-Level verlangt und der Trust-Status nicht bestimmt werden kann:

```text
Trust = Unknown
```

gilt nicht als:

```text
Trust = Sufficient
```

---

# Trust Minimum

Beispiel:

```text
TrustRequirement {
    minimum = Verified
}
```

Bei:

```text
ProviderTrust = Unknown
```

darf der Provider nicht verwendet werden.

---

# Trust Failure

Trust Verification Failure muss von normalen Providerfehlern unterscheidbar sein.

---

# Kein Trust Downgrade

NovaOS darf bei Trust-Problemen nicht automatisch:

```text
Verified Provider
       ↓
Unknown Provider
```

wechseln, wenn der Contract `Verified` verlangt.

---

# Identity

Kann eine Identität nicht zuverlässig bestimmt werden, darf keine Identität mit höheren Rechten angenommen werden.

---

# Anonymous Default

Wo anonyme Nutzung zulässig ist, kann:

```text
Unknown Identity
       ↓
Anonymous / Unprivileged
```

ein Safe Default sein.

---

# Privilege Default

Unbekannte Privilegien werden als:

```text
No Privilege
```

behandelt.

---

# Data Sovereignty

Data Sovereignty besitzt Fail-Safe-Semantik.

Kann NovaOS nicht feststellen, ob Daten einen Standort verlassen dürfen:

```text
Unknown Sovereignty
        ↓
Keep Data Local
```

---

# Keine automatische Cloud-Freigabe

Nicht:

```text
Sovereignty metadata missing
        ↓
Upload to Cloud
```

---

# Unknown Residency Policy

Wenn keine gültige Residency Policy bestimmt werden kann, muss eine konservative lokale Policy verwendet werden.

---

# Local-First Safe Default

Für unbekannte Datenrestriktionen ist:

```text
Local Processing
```

gegenüber:

```text
Remote Processing
```

zu bevorzugen.

---

# Information Flow

Unklare Information-Flow-Berechtigungen dürfen keinen neuen Datenfluss autorisieren.

```text
Unknown Flow Permission
        ↓
Block Flow
```

---

# Existing Flow

Bereits etablierte Flows dürfen nur fortgeführt werden, wenn ihre Autorisierung weiterhin gültig ist.

---

# Revocation

Nach bestätigtem Capability- oder Policy-Widerruf dürfen neue Operationen nicht weiter autorisiert werden.

---

# Revocation Uncertainty

Kann der Revocation State bei einem sicherheitskritischen Vorgang nicht zuverlässig geprüft werden, muss der Contract festlegen, ob:

```text
Fail Closed
```

erforderlich ist.

Für hochprivilegierte Operationen ist dies der Standard.

---

# Fail Closed

`Fail Closed` bedeutet:

```text
Security mechanism unavailable
        ↓
Access remains blocked
```

---

# Fail Open

`Fail Open` bedeutet:

```text
Security mechanism unavailable
        ↓
Access continues
```

Fail Open ist in NovaOS kein allgemeiner Default.

---

# Explizites Fail Open

Fail Open darf nur verwendet werden, wenn:

- der Contract dies ausdrücklich erlaubt,
- Safety dadurch nicht gefährdet wird,
- Security-Anforderungen dies zulassen,
- die Failure Semantics dokumentiert sind.

---

# Safety vs Security

Safety und Security können in seltenen Fällen unterschiedliche sichere Zustände verlangen.

Beispiel:

```text
Emergency Exit
```

darf möglicherweise nicht durch einen ausgefallenen digitalen Autorisierungsdienst blockiert werden.

Solche Konflikte müssen explizit im Systemdesign modelliert werden.

---

# Kein implizites Priorisieren

NovaOS darf solche Safety-vs-Security-Konflikte nicht spontan anhand generischer Heuristiken entscheiden.

---

# Safety Contract

Safety-kritische Komponenten benötigen explizite Failure Policies.

---

# Safety Default

Der Safe Default einer Safety-Komponente muss domainspezifisch definiert sein.

---

# Data Integrity

Bei Unsicherheit über Datenintegrität darf NovaOS Daten nicht automatisch als gültig behandeln.

```text
Integrity Unknown
      ↓
Untrusted / Unverified
```

---

# Corrupted Data

Erkannte beschädigte Daten dürfen nicht stillschweigend als korrekte Daten weiterverarbeitet werden.

---

# Recovery Copy

Wenn eine verifizierte Recovery-Kopie existiert:

```text
Primary Invalid
      ↓
Verified Recovery Copy
```

kann dies der Safe Default sein.

---

# Read-Only Fallback

Bei Unsicherheit über Schreibkonsistenz kann:

```text
Read-Only
```

ein sicherer Fallback sein.

---

# Storage Example

```text
Filesystem state uncertain
        ↓
Mount Read-Only
        ↓
Verify / Repair
        ↓
Enable Write
```

---

# Kein automatisches destruktives Repair

Self-Healing darf bei Unsicherheit nicht automatisch irreversible Änderungen durchführen.

---

# Destructive Operations

Destruktive Operationen benötigen besonders starke positive Autorisierung.

Beispiele:

```text
Delete
Format
Overwrite
Key Destruction
Factory Reset
Partition Change
Firmware Update
```

---

# Confirmation

Eine fehlende erforderliche Bestätigung gilt als:

```text
Not Authorized
```

---

# Timeout bei Bestätigung

Wenn eine erforderliche Bestätigung abläuft:

```text
Timeout
   ↓
Cancel
```

Nicht:

```text
Timeout
   ↓
Assume Yes
```

---

# Transactions

Diese ADR integriert:

```text
ADR-ARCH-0004
Transaktionale Systemoperationen
```

Bei unklarem Transaktionszustand muss NovaOS zwischen:

```text
Committed
Aborted
Prepared
Unknown
```

unterscheiden.

---

# Unknown Transaction Outcome

Ein:

```text
UnknownOutcome
```

darf nicht automatisch als:

```text
Failed, retry from beginning
```

interpretiert werden.

Dadurch könnten Operationen doppelt ausgeführt werden.

---

# Mutating Operations

Bei mutierenden Operationen muss ein unbekannter Outcome konservativ behandelt werden.

---

# Transaction Recovery

Beispiel:

```text
Unknown Outcome
      ↓
Query Transaction State
      ↓
Recover Journal
      ↓
Determine Commit State
      ↓
Continue
```

---

# Keine blinde Wiederholung

Eine nicht idempotente Operation darf nach unbekanntem Outcome nicht blind wiederholt werden.

---

# Idempotency

Idempotente Operationen können sicherere Retry-Strategien ermöglichen.

---

# Commit Point

Nach einem Commit Point muss Recovery die bereits erfolgte Änderung berücksichtigen.

---

# Rollback

Rollback darf nur erfolgen, wenn die Operation tatsächlich rollbackfähig ist.

---

# Forward Recovery

Wenn Rollback nicht sicher möglich ist, kann Forward Recovery der sichere Pfad sein.

---

# Declarative System Model

Diese ADR integriert:

```text
ADR-ARCH-0003
Deklaratives Systemmodell als Architekturgrundlage
```

Wenn der Desired State nicht valide ist:

```text
Invalid Desired State
       ↓
Do Not Apply
```

---

# Previous Known-Good State

Ein gültiger bestehender Zustand soll erhalten bleiben, wenn eine neue Konfiguration nicht validiert werden kann.

```text
Current Known-Good State
          │
          ├── New State Valid
          │       ↓
          │     Apply
          │
          └── New State Invalid
                  ↓
              Keep Current
```

---

# Configuration Failure

Eine beschädigte Konfiguration darf nicht automatisch mit permissiven Werten ersetzt werden.

---

# Safe Configuration Defaults

Defaults müssen explizit als sichere Defaults entworfen werden.

---

# Missing Configuration

Fehlende Konfiguration bedeutet nicht automatisch:

```text
Enable Everything
```

---

# Feature Default

Sicherheitsrelevante Features sollen standardmäßig den restriktiveren Zustand verwenden.

---

# Mechanism / Policy Separation

Diese ADR integriert:

```text
ADR-ARCH-0002
Mechanism / Policy Separation
```

Ein Mechanismus darf bei Ausfall der Policy-Schicht nicht eigenständig eine permissive Policy erfinden.

---

# Policy Failure

```text
Policy unavailable
      ↓
Use validated safe policy
```

oder:

```text
Deny / Suspend operation
```

---

# Cached Policy

Eine gecachte Policy darf nur verwendet werden, wenn:

- ihre Gültigkeit bekannt ist,
- ihre Version zulässig ist,
- ihr Ablaufzeitpunkt nicht überschritten wurde,
- ihre Integrität bestätigt ist,
- der Contract Offline-Verwendung erlaubt.

---

# Stale Security Policy

Eine abgelaufene Security Policy darf nicht automatisch weiterverwendet werden.

---

# Policy Versioning

Policies müssen versionierbar sein.

---

# Policy Validation

Neue Policies müssen vor Aktivierung validiert werden.

---

# Policy Hot Replacement

Diese ADR integriert:

```text
ADR-ARCH-0012
Hot Replacement und Live Evolution
```

Eine neue Policy-Version darf erst aktiviert werden, wenn ihre Kompatibilität und Integrität bestätigt sind.

---

# Failed Policy Replacement

Bei fehlgeschlagenem Replacement:

```text
New Policy Invalid
       ↓
Keep Previous Known-Good Policy
```

sofern diese weiterhin gültig ist.

---

# Kein Downgrade auf unsichere Policy

Eine ältere Policy darf nur als Fallback verwendet werden, wenn sie weiterhin die erforderlichen Hard Constraints erfüllt.

---

# Execution Contracts

`Nova.ExecutionContract` muss Fail-Safe-Anforderungen berücksichtigen können.

Beispiel:

```text
ExecutionContract {
    trust {
        minimum = Verified
    }

    location {
        remote = Forbidden
    }

    degradation {
        allowed = true
    }

    failure {
        onTrustUnknown = Fail
        onRemoteUnavailable = UseLocal
    }
}
```

---

# Failure Policy

Contracts können Failure Policies definieren.

Beispiele:

```text
Fail
Fallback
Degrade
Retry
Rollback
ForwardRecover
ReadOnly
Suspend
AskUser
```

---

# Safe Fallback

Ein Fallback ist nur dann zulässig, wenn er weiterhin alle relevanten Hard Contracts erfüllt.

---

# Graceful Degradation

Diese ADR integriert:

```text
ADR-ARCH-0015
Graceful Degradation als Systemprinzip
```

Graceful Degradation darf ausschließlich auf gültige sichere Zustände wechseln.

---

# Safe Degradation

```text
Preferred State
      ↓
Unavailable
      ↓
Safe Degraded State
```

---

# Unsafe Degradation

Nicht zulässig:

```text
Verified Local Provider
        ↓
Unavailable
        ↓
Untrusted Remote Provider
```

wenn Trust und LocalOnly Hard Requirements sind.

---

# Degradation Exhaustion

Wenn keine sichere Degradationsstufe existiert:

```text
DegradationExhausted
        ↓
Fail-Safe Action
```

---

# Resource Economy

Diese ADR integriert:

```text
ADR-ARCH-0005
Systemweite Ressourcenökonomie
```

Ressourcenknappheit darf keine Hard Constraints außer Kraft setzen.

---

# Resource Failure

Wenn eine sichere Operation nicht ausreichend Ressourcen erhält:

```text
Insufficient Resources
        ↓
Degrade if safe
        ↓
Otherwise fail
```

---

# Emergency Reserve

Emergency Resources dürfen für Recovery und kritische Systemoperationen reserviert werden.

---

# Memory Allocation Failure

Eine fehlgeschlagene Speicherallokation darf nicht zu:

- Null-Pointer-Nutzung,
- uninitialisierten Daten,
- Buffer Overflows,
- inkonsistentem Zustand

führen.

---

# Allocation Result

Allocation Failure muss explizit behandelbar sein.

---

# Integer Overflow

Overflow darf nicht stillschweigend zu einer kleineren unsicheren Ressourcenallokation führen.

---

# Size Validation

Größenberechnungen müssen vor Allokation auf Overflow geprüft werden.

---

# Resource Accounting Failure

Wenn Resource Accounting für eine unprivilegierte Operation nicht zuverlässig durchgeführt werden kann, darf die Operation keine unbegrenzten Ressourcen erhalten.

---

# Budget Unknown

```text
Budget Unknown
      ↓
Conservative Limit
```

oder:

```text
Reject
```

abhängig vom Contract.

---

# CPU Scheduling

Ein Fehler in adaptiver Prioritätsberechnung darf nicht automatisch maximale Priorität vergeben.

---

# Priority Default

Unbekannte Priorität erhält eine definierte konservative Standardpriorität.

---

# Realtime

Realtime-Rechte benötigen explizite Autorisierung.

```text
Realtime Permission Unknown
        ↓
No Realtime Privilege
```

---

# Deadline

Kann eine Hard Deadline nicht garantiert werden, darf NovaOS nicht behaupten, sie sei garantiert.

---

# Deterministic Mode

Diese ADR integriert:

```text
ADR-ARCH-0011
Deterministic Mode
```

Kann eine deterministische Ausführung nicht gewährleistet werden:

```text
Determinism Required
        +
Cannot Guarantee
        ↓
Fail
```

oder ein explizit zugelassener deterministischer Fallback muss verwendet werden.

---

# Keine stille Nichtdeterministik

Nicht:

```text
Deterministic Mode unavailable
        ↓
Run nondeterministically
```

---

# Structured Concurrency

Diese ADR integriert:

```text
ADR-ARCH-0010
Structured Concurrency als systemweites Ausführungsmodell
```

Fehler und Cancellation müssen innerhalb definierter Scopes propagiert werden.

---

# Orphan Tasks

Ein Parent Failure darf nicht unbeabsichtigt privilegierte Orphan Tasks erzeugen.

---

# Cancellation Default

Bei Scope-Abbruch werden abhängige Child Tasks standardmäßig kontrolliert gecancelt, sofern ihr Contract keine explizite unabhängige Lifetime definiert.

---

# Cleanup

Cleanup muss auch auf Fehlerpfaden ausgeführt werden können.

---

# Resource Release

Fail-Safe-Pfade müssen:

- Locks,
- Handles,
- Memory,
- Buffers,
- Capabilities,
- Leases,
- Device Resources

kontrolliert freigeben.

---

# Capability Leases

Abgelaufene Leases dürfen nicht als weiterhin gültig angenommen werden.

---

# Network

Netzwerkfehler besitzen keine implizite Erfolgssemantik.

---

# Connection Failure

```text
Connection Lost
```

ist nicht:

```text
Remote Operation Failed
```

wenn der tatsächliche Remote Outcome unbekannt ist.

---

# Remote Mutations

Remote Mutations benötigen:

- Transaction IDs,
- Idempotency Keys,
- Durable Operation IDs

wo dies erforderlich ist.

---

# TLS / Transport Security

Kann erforderliche Transport Security nicht aufgebaut werden:

```text
Secure Transport Required
        ↓
Handshake Failed
        ↓
Do Not Connect
```

---

# Kein automatisches HTTP-Fallback

Nicht:

```text
HTTPS failed
    ↓
HTTP
```

wenn sichere Übertragung erforderlich ist.

---

# Certificate Validation

Fehlgeschlagene Zertifikatsprüfung darf nicht automatisch ignoriert werden.

---

# Unknown Certificate Status

Unknown ist nicht automatisch Trusted.

---

# DNS / Naming

Unsichere oder widersprüchliche Namensauflösung darf keine privilegierte Identitätsannahme erzeugen.

---

# Location Transparency

Diese ADR integriert:

```text
ADR-ARCH-0007
Location Transparency mit sichtbaren Kosten und Fehlern
```

Kann der zulässige Ausführungsort nicht bestimmt werden, darf nicht automatisch auf einen beliebigen Remote-Standort ausgewichen werden.

---

# Unknown Location

```text
Location Unknown
      ↓
Do Not Assume Allowed
```

---

# Local Safe Fallback

Wenn semantisch möglich:

```text
Remote Location Unknown
        ↓
Verified Local Provider
```

---

# Remote Provider

Remote Provider benötigen weiterhin:

- Identity,
- Trust,
- Authorization,
- Location Policy,
- Sovereignty Policy.

---

# Distributed Systems

Netzwerkpartitionen müssen explizit behandelt werden.

---

# Partition

Eine Partition darf nicht automatisch als:

```text
Remote node definitely failed
```

interpretiert werden.

---

# Split Brain

Bei Systemen mit exklusiver Ownership muss Split Brain verhindert werden.

---

# Lease Expiration

Eine abgelaufene Ownership Lease darf nicht stillschweigend verlängert angenommen werden.

---

# Fencing

Distributed Ownership soll Fencing verwenden, wo erforderlich.

---

# Storage

Bei unsicherem Storage State ist Data Integrity wichtiger als Availability.

---

# Filesystem

Ein beschädigtes Dateisystem kann in:

```text
Read-Only Recovery Mode
```

wechseln.

---

# Write Protection

Bei unklarer Metadatenintegrität kann Write Protection aktiviert werden.

---

# Journal Recovery

Journal Recovery muss vor erneuten Writes abgeschlossen werden können.

---

# Unknown Block Ownership

Unklare Blockzuordnung darf nicht durch blindes Überschreiben gelöst werden.

---

# Encryption

Kann ein Schlüssel nicht sicher bestimmt oder validiert werden, dürfen verschlüsselte Daten nicht mit einem vermuteten Schlüssel überschrieben werden.

---

# Key Management

Unknown Key State muss explizit sein.

---

# Key Revocation

Widerrufene Schlüssel dürfen nicht weiterverwendet werden.

---

# Key Destruction

Key Destruction muss besonders gegen unbeabsichtigte Ausführung geschützt werden.

---

# Backup

Ein Backup gilt erst dann als verwendbar, wenn erforderliche Integritätsprüfungen bestanden wurden.

---

# Recovery

Recovery darf nicht automatisch eine beschädigte Recovery-Version über einen bekannten guten Zustand schreiben.

---

# Boot

Fail-Safe Defaults gelten bereits während des Bootprozesses.

---

# Boot Verification

Wenn Secure-Boot-/Trust-Anforderungen gelten und ein Bootartefakt nicht verifiziert werden kann:

```text
Do Not Execute
```

---

# Boot Fallback

Ein verifizierter älterer Bootpfad kann verwendet werden, wenn Policy und Rollback-Schutz dies erlauben.

---

# Rollback Protection

Ein älteres, aber verwundbares Image darf nicht allein deshalb gestartet werden, weil das aktuelle Image beschädigt ist.

---

# Recovery Environment

Wenn normaler Boot nicht sicher möglich ist:

```text
Normal Boot
    ↓
Verified Recovery
    ↓
NovaDOS
```

kann der Fail-Safe-Pfad sein.

---

# NovaDOS

NovaDOS soll fundamentale Recovery-Funktionen unabhängig vom normalen NovaOS bereitstellen.

---

# Boot UI

Wenn GPU-Beschleunigung nicht sicher initialisiert werden kann:

```text
Accelerated Renderer
        ↓
Framebuffer Backend
```

---

# Graphics

Ein Grafikfehler darf nicht unnötig die Integrität anderer Systemkomponenten gefährden.

---

# Driver Isolation

Fehlerhafte oder nicht vertrauenswürdige Treiber können in stärkere Isolation verschoben werden.

---

# Driver Trust

Ein unbekannter Treiber darf nicht automatisch mit maximalen Kernelrechten geladen werden.

---

# Driver Default

Bevorzugt:

```text
Unknown / Lower Trust Driver
        ↓
Isolated Driver Domain
```

wenn der Driver Contract dies zulässt.

Andernfalls:

```text
Do Not Load
```

---

# DMA

DMA-Zugriff muss über IOMMU oder gleichwertige Schutzmechanismen begrenzt werden, wenn dies für die Plattform vorgesehen ist.

---

# Unknown DMA Mapping

Ein Gerät darf bei unbekanntem Mapping nicht automatisch Zugriff auf den gesamten physischen Speicher erhalten.

---

# Device Failure

Gerätefehler sollen auf die kleinste mögliche Failure Domain begrenzt werden.

---

# USB / External Devices

Neue externe Geräte erhalten nicht automatisch privilegierten Zugriff.

---

# Removable Media

Automatische Ausführung unbekannter Programme von Wechselmedien ist kein Safe Default.

---

# Executables

Unbekannte Binärdateien werden nicht allein aufgrund ihrer Dateiendung als vertrauenswürdig behandelt.

---

# Semantic Types

Diese ADR integriert:

```text
ADR-ARCH-0008
Semantic Types als systemweites Typsystem
```

---

# Unknown Type

Ein unbekannter Semantic Type darf nicht automatisch als kompatibler Typ interpretiert werden.

---

# Unsafe Cast

Ein unchecked cast muss explizit als unsafe gekennzeichnet sein.

---

# Type Validation

Unvalidierte externe Daten bleiben:

```text
Unvalidated<T>
```

bis eine erfolgreiche Validierung erfolgt.

---

# Type Confusion

Fail-Safe Defaults müssen Type Confusion verhindern.

---

# Serialization

Unbekannte oder inkompatible Schema-Versionen dürfen nicht stillschweigend als bekannte Version interpretiert werden.

---

# Unknown Fields

Unknown Fields können erhalten oder ignoriert werden, wenn das Schema dies erlaubt.

Sie dürfen nicht automatisch privilegierte Semantik erhalten.

---

# Data/Object Pipeline

Diese ADR integriert:

```text
ADR-ARCH-0009
Einheitliches Daten- und Objekt-Pipelining
```

---

# Pipeline Stage Failure

Eine fehlgeschlagene Required Stage darf nicht stillschweigend übersprungen werden.

---

# Optional Stage

Nur explizit optionale Stages dürfen bei Fehlern übersprungen werden.

---

# Pipeline Output

Ein Pipeline Output darf erst als erfolgreich gelten, wenn alle Required Contracts erfüllt sind.

---

# Zero-Copy

Diese ADR integriert:

```text
ADR-ARCH-0006
Zero-Copy als Architekturprinzip
```

---

# Zero-Copy Safety

Zero-Copy darf nicht verwendet werden, wenn Ownership-, Lifetime-, Security- oder Coherency-Anforderungen nicht sicher erfüllt werden können.

---

# Copy Fallback

Wenn:

```text
Zero-Copy unsafe
```

und:

```text
Copy allowed
```

gilt:

```text
Safe Copy
```

ist gegenüber unsicherem Shared Memory zu bevorzugen.

---

# Copy Forbidden

Wenn der Contract Kopieren verbietet und Zero-Copy nicht sicher möglich ist:

```text
Fail
```

---

# Buffer Bounds

Unsichere Buffergrößen dürfen nicht angenommen werden.

---

# Shared Memory

Shared Memory benötigt explizite Zugriffsrechte.

---

# Ownership Unknown

Bei unklarem Ownership State darf kein Schreibzugriff angenommen werden.

---

# Read-Only View

Ein Read-Only View kann ein Safe Default sein.

---

# AI

KI darf niemals die einzige Instanz sein, die sicherheitskritische Fail-Safe-Entscheidungen bestimmt.

---

# AI Uncertainty

Ein KI-Modell kann Ergebnisse mit Unsicherheit liefern.

Unsicherheit darf nicht automatisch in privilegierte Aktionen übersetzt werden.

---

# AI Suggestions

AI Output ist zunächst:

```text
Suggestion
```

und nicht:

```text
Authorization
```

---

# AI Agents

AI Agents benötigen explizite Capabilities.

---

# Agent Permission

Ein Agent darf keine Operation durchführen, nur weil er sie sprachlich vorgeschlagen oder geplant hat.

---

# Tool Invocation

Jeder Tool-Aufruf muss weiterhin durch die normalen Capability- und Policy-Prüfungen laufen.

---

# AI Failure

Wenn AI nicht verfügbar ist, müssen fundamentale Sicherheitsmechanismen weiter funktionieren.

---

# AI Hallucination

Halluzinierte Capability IDs, Ressourcen, Benutzerrechte oder Systemzustände dürfen keine Autorität erzeugen.

---

# Adaptive Systems

Adaptive Systeme dürfen Fail-Safe Defaults optimieren, aber nicht entfernen.

---

# Prediction Error

Prediction Error darf Präferenzen verbessern.

Er darf keine Hard Security Policy abschwächen.

---

# Automatic Algorithm Selection

NovaOS darf automatisch Algorithmen auswählen.

Die Auswahl darf jedoch keine Hard Contracts verletzen.

---

# Algorithm Unknown

Wenn die Eigenschaften eines Algorithmus nicht ausreichend bekannt sind, darf er nicht für einen Contract verwendet werden, der diese Eigenschaften garantiert benötigt.

---

# Forced Algorithm

Ein Forced Algorithm darf nur ausgeführt werden, wenn seine Ausführung weiterhin Safety-, Security- und System-Hard-Constraints erfüllt.

---

# Expliziter Override

Ein Benutzer- oder Entwickler-Override kann Soft Policies überstimmen.

Er kann keine fundamentalen Hard Safety- oder Security-Grenzen umgehen, sofern nicht ein expliziter privilegierter Unsafe-Mechanismus dafür vorgesehen und autorisiert ist.

---

# Unsafe Operations

NovaOS kann explizite Unsafe-Operationen unterstützen.

Diese müssen:

- sichtbar,
- autorisiert,
- auditierbar,
- begrenzt,
- nicht implizit

sein.

---

# Architecture Introspection

Diese ADR integriert:

```text
ADR-ARCH-0013
Architecture Introspection
```

Fail-Safe-Entscheidungen müssen introspektierbar sein.

---

# Introspection Example

```text
Operation:
    de.nova.storage.write

Requested:
    Write

Decision:
    Denied

Reason:
    IntegrityStateUnknown

SafeDefault:
    ReadOnly

Policy:
    storage.integrity.default
```

---

# Decision Trace

Für bedeutende Fail-Safe-Entscheidungen soll ein Decision Trace verfügbar sein.

---

# Explainability

Das System soll beantworten können:

```text
Why was this operation denied?
```

```text
Why was this provider not selected?
```

```text
Why did the filesystem become read-only?
```

```text
Why was remote execution blocked?
```

---

# Explicit Contracts

Diese ADR integriert:

```text
ADR-ARCH-0014
Explizite Contracts zwischen Systemkomponenten
```

Fail-Safe-Verhalten soll Teil der Contracts sein.

---

# Failure Contract

Ein Component Contract kann beispielsweise definieren:

```text
FailurePolicy {
    authorizationUnknown = Deny
    trustUnknown         = Deny
    providerUnavailable  = Fallback
    integrityUnknown     = ReadOnly
    resourceExhausted    = Degrade
}
```

---

# Keine versteckten Defaults

Sicherheitsrelevante Defaults müssen dokumentiert und introspektierbar sein.

---

# Default Registry

NovaOS kann systemweit definierte Safe Defaults für standardisierte Failure Classes besitzen.

---

# Component Override

Komponenten dürfen einen systemweiten Safe Default nur überschreiben, wenn:

- der Contract dies erlaubt,
- der Override nicht weniger sicher als zulässig ist,
- die Änderung explizit ist.

---

# Hierarchie

Konzeptionell:

```text
System Safety Policy
        ↓
Security Policy
        ↓
Domain Policy
        ↓
Component Contract
        ↓
Operation Contract
```

Ein untergeordneter Contract darf höhere Hard Constraints nicht aufheben.

---

# Unknown Contract

Kann ein erforderlicher Contract nicht geladen oder validiert werden:

```text
Contract Unknown
       ↓
Do Not Assume Permission
```

---

# Contract Version

Unbekannte inkompatible Contract-Versionen dürfen nicht automatisch als kompatibel behandelt werden.

---

# Contract Negotiation

Wenn zwei Komponenten keinen kompatiblen Contract finden:

```text
Negotiation Failed
       ↓
Do Not Communicate
```

oder ein explizit definierter kompatibler Fallback wird verwendet.

---

# IPC

Typed IPC muss ungültige oder nicht autorisierte Nachrichten ablehnen können.

---

# Unknown Message

Eine unbekannte Nachricht darf nicht als privilegierter bekannter Befehl interpretiert werden.

---

# Invalid Message

Fehlgeschlagene Validierung führt zu:

```text
Reject
```

nicht:

```text
Best Guess
```

bei sicherheitsrelevanter IPC.

---

# Parser

Parser müssen sichere Fehlerzustände besitzen.

---

# Partial Parse

Ein teilweise geparster privilegierter Befehl darf nicht automatisch ausgeführt werden.

---

# Input Validation

Externe Eingaben gelten grundsätzlich als untrusted, bis sie validiert wurden.

---

# Missing Fields

Fehlende sicherheitsrelevante Felder dürfen keine permissiven Defaults erhalten.

---

# Unknown Enum Values

Unbekannte Enum-Werte müssen explizit behandelt werden.

---

# Future Compatibility

Forward Compatibility darf nicht durch permissive Interpretation unbekannter Security-Semantik erkauft werden.

---

# API Evolution

Neue API-Versionen müssen sichere Defaults für alte Clients definieren.

---

# New Permission

Eine neu eingeführte Permission darf bei alten Clients nicht automatisch als gewährt gelten.

---

# New Feature

Neue privilegierte Features sollen standardmäßig deaktiviert bleiben, bis sie explizit autorisiert werden.

---

# Hot Evolution

Live Evolution darf neue sicherheitsrelevante Semantik nicht automatisch auf alte Contracts projizieren.

---

# Kernel

Kernelmechanismen müssen besonders konservative Fail-Safe Defaults verwenden.

---

# Kernel Pointer

Ungültige Kernelpointer dürfen nicht dereferenziert werden.

---

# User Pointer

User-Pointer müssen vor Zugriff validiert werden.

---

# Syscalls

Ungültige Syscall-Parameter müssen abgelehnt werden.

---

# Unknown Syscall

Unbekannte Syscall-Nummern dürfen keine zufällige Funktion auslösen.

---

# Kernel Object

Ungültige Kernel Object Handles dürfen keine implizite Referenz auf andere Objekte erzeugen.

---

# Handle Generation

Generation Counters oder gleichwertige Mechanismen sollen stale handles erkennen.

---

# Capability Handle

Ein ungültiger Capability Handle besitzt keine Rechte.

---

# Privilege Boundary

Fehler dürfen Privilegien niemals erhöhen.

Das zentrale Invariant lautet:

```text
Failure must not create authority.
```

---

# Isolation Failure

Kann eine geforderte Isolation nicht hergestellt werden:

```text
Required Isolation
       +
Isolation Setup Failed
       ↓
Do Not Execute
```

---

# Sandbox

Ein Sandbox-Ausfall darf nicht automatisch zur Ausführung außerhalb der Sandbox führen.

---

# Driver Domain

Kann ein Treiber nur in einem isolierten Domain ausgeführt werden und dessen Isolation nicht aufgebaut werden:

```text
Do Not Load Driver
```

---

# Compatibility Layer

Ein Fehler in WINE-, Win32-, POSIX-, Linux- oder anderen Compatibility Layers darf keine zusätzlichen nativen NovaOS-Rechte erzeugen.

---

# Legacy Permission Mapping

Unbekannte Legacy-Berechtigungen werden nicht automatisch auf umfassende NovaOS-Capabilities abgebildet.

---

# ABI

Ungültige ABI-Daten müssen abgelehnt werden.

---

# FFI

FFI-Grenzen benötigen explizite Validierung.

---

# NovaLang

NovaLang soll sichere Sprachdefaults unterstützen.

Beispiele:

```text
Bounds Checking
Checked Conversions
Explicit Unsafe
Definite Initialization
Structured Errors
```

---

# Uninitialized State

Uninitialisierte Variablen dürfen nicht als zufällige gültige Werte interpretiert werden.

---

# Nullability

Nullability muss explizit modellierbar sein.

---

# Error Handling

Fehler dürfen nicht automatisch als erfolgreiche Default-Werte verschluckt werden, wenn dies Semantik verändern würde.

---

# Result Types

Operationen sollen Fehler explizit darstellen können.

Konzeptionell:

```text
Result<T, Error>
```

---

# Panic

Ein lokaler Fehler soll möglichst innerhalb der kleinsten sinnvollen Failure Domain bleiben.

---

# Panic != System Compromise

Nicht jeder interne Fehler soll das gesamte System stoppen.

---

# Failure Containment

Fail-Safe Defaults und starke Isolation ergänzen sich:

```text
Failure
   ↓
Contain
   ↓
Safe Local State
   ↓
Recover
```

---

# Self-Healing

Self-Healing darf erst nach sicherer Fehlerklassifikation handeln.

---

# Unknown Failure

Ein unbekannter Fehler darf keine destruktive automatische Reparatur autorisieren.

---

# Repair Plan

Ein Repair Plan muss vor Ausführung validiert werden können.

---

# Repair Capability

Self-Healing benötigt nur die minimal erforderlichen Capabilities.

---

# Recovery Privilege

Recovery-Komponenten dürfen nicht allein aufgrund ihrer Rolle unbegrenzte Rechte besitzen.

---

# Audit

Sicherheitsrelevante Fail-Safe-Entscheidungen sollen auditierbar sein.

---

# Audit Failure

Ein Audit-System-Ausfall darf unterschiedlich behandelt werden:

```text
AuditRequired = Hard
    ↓
Operation may fail closed
```

oder:

```text
Audit = BestEffort
    ↓
Operation may continue
```

Die Semantik muss explizit im Contract stehen.

---

# Audit Buffer

Kritische Audit-Daten können reservierte Ressourcen verwenden.

---

# Audit Overflow

Audit Overflow darf nicht stillschweigend sicherheitsrelevante Events verlieren, wenn vollständiges Audit ein Hard Requirement ist.

---

# Observability

Fail-Safe Events sollen beobachtbar sein.

Metriken können umfassen:

```text
FailSafeActivations
AuthorizationDenials
TrustFailures
IntegrityFailures
PolicyFailures
ReadOnlyFallbacks
ProviderFallbacks
ContractFailures
IsolationFailures
UnsafeFallbacksPrevented
RecoveryActivations
```

---

# Keine sensiblen Daten in Diagnose

Observability darf selbst keine Security- oder Privacy-Grenzen verletzen.

---

# User Experience

Fail-Safe Defaults sollen nicht unnötig als kryptische Fehler erscheinen.

---

# User Message

Beispiel:

```text
Die Datei wurde schreibgeschützt geöffnet,
weil ihre Integrität nicht vollständig
überprüft werden konnte.
```

---

# Developer Message

Zusätzlich:

```text
SafeDefaultActivated {
    subsystem = Storage
    reason = MetadataIntegrityUnknown
    action = MountReadOnly
}
```

---

# Silent Safe Defaults

Einige interne Safe Defaults können ohne Benutzerinteraktion erfolgen, wenn:

- kein Datenverlust entsteht,
- keine relevante Funktion dauerhaft verloren geht,
- keine Benutzerentscheidung erforderlich ist.

---

# User Confirmation

Wenn mehrere sichere Alternativen existieren und die Entscheidung semantisch vom Nutzer abhängt, kann NovaOS den Nutzer fragen.

---

# Timeout

Ein Timeout bei sicherheitsrelevanter Auswahl darf keine privilegierte Option automatisch auswählen.

---

# Safe UI Default

Dialoge für destruktive Operationen sollen standardmäßig die nicht-destruktive Option fokussieren.

---

# Confirmation Semantics

Beispiel:

```text
[Abbrechen] [Löschen]
```

Der Default ist:

```text
Abbrechen
```

---

# No Accidental Confirmation

Gefährliche Aktionen dürfen nicht allein durch das Schließen oder Timeout eines Dialogs bestätigt werden.

---

# Accessibility

Fail-Safe UI muss auch mit Accessibility-Funktionen korrekt bedienbar sein.

---

# Time

Zeit kann sicherheitsrelevant sein.

---

# Clock Unknown

Wenn Zertifikats-, Token- oder Lease-Gültigkeit von vertrauenswürdiger Zeit abhängt und die Zeit unbekannt ist, darf NovaOS nicht automatisch annehmen, dass das Objekt gültig ist.

---

# Clock Domains

Monotonic Time und Wall Clock müssen semantisch getrennt bleiben.

---

# Token Expiration

Ein Token mit nicht verifizierbarer Ablaufzeit darf nicht automatisch unbegrenzt gültig werden.

---

# Randomness

Wenn kryptographisch sichere Zufallswerte benötigt werden und keine ausreichende Entropie verfügbar ist:

```text
Do Not Generate Weak Secret
```

---

# Cryptographic RNG

Ein schwacher PRNG darf nicht stillschweigend als CSPRNG verwendet werden.

---

# Cryptographic Failure

Fehler in kryptographischen Operationen müssen explizit propagiert werden.

---

# Signature Verification

Verification Error ist nicht:

```text
Signature Valid
```

---

# Hash Verification

Fehlende oder falsche Hashwerte dürfen nicht als erfolgreiche Integritätsprüfung gelten.

---

# Updates

Systemupdates müssen Fail-Safe Defaults verwenden.

---

# Update Verification

Nicht verifizierte Updates dürfen nicht installiert werden.

---

# Interrupted Update

Ein unterbrochenes Update soll auf:

```text
Previous Known-Good Generation
```

oder:

```text
Recoverable Staged State
```

zurückfallen.

---

# A/B Updates

Wo sinnvoll können A/B-Generationen verwendet werden.

---

# Boot Health

Eine neue Generation wird erst als Known-Good markiert, nachdem definierte Health Checks bestanden wurden.

---

# Failed New Generation

```text
New Generation
      ↓
Health Check Failed
      ↓
Rollback / Previous Known-Good
```

---

# Anti-Rollback

Security Policy kann Rollback auf verwundbare Versionen verbieten.

---

# Update Conflict

Wenn weder neue noch alte Generation die Hard Security Requirements erfüllt:

```text
Recovery Required
```

statt unsicher weiterzulaufen.

---

# Configuration Transactions

Konfigurationsänderungen sollen:

```text
Validate
Stage
Activate
Verify
Commit
```

verwenden.

---

# Partial Configuration

Eine teilweise aktivierte Konfiguration darf nicht als vollständig gültig behandelt werden.

---

# Architecture Principle

Die zentrale Architekturregel lautet:

```text
Failure must not increase privilege.
```

ergänzt durch:

```text
Failure must not weaken hard guarantees.
```

und:

```text
Unknown must remain unknown
until positively resolved.
```

---

# Normative Festlegungen

1. NovaOS MUSS Fail-Safe Defaults als systemweites Architekturprinzip verwenden.

2. Unsicherheit DARF keine zusätzliche Autorität erzeugen.

3. Fehler DÜRFEN Privilegien NICHT erhöhen.

4. Fehler DÜRFEN Hard Security Guarantees NICHT stillschweigend abschwächen.

5. Fehler DÜRFEN Hard Safety Guarantees NICHT stillschweigend abschwächen.

6. Fehler DÜRFEN Data Sovereignty NICHT stillschweigend abschwächen.

7. Fehler DÜRFEN Information Flow Restrictions NICHT stillschweigend abschwächen.

8. Fehler DÜRFEN Data Integrity NICHT stillschweigend aufgeben.

9. Fail-Safe MUSS von Fail-Stop unterschieden werden.

10. Fail-Stop DARF als Fail-Safe-Strategie verwendet werden.

11. Safe States MÜSSEN domainspezifisch definierbar sein.

12. NovaOS DARF keinen universellen Safe State voraussetzen.

13. Relevante Komponenten SOLLEN explizite Safe Defaults definieren.

14. Autorisierung MUSS grundsätzlich positive Autorisierung verlangen.

15. Fehlende Autorisierung MUSS als Deny behandelt werden.

16. Unbekannte Autorisierung MUSS als Deny behandelt werden.

17. Ungültige Capabilities DÜRFEN keine Autorität verleihen.

18. Abgelaufene Capabilities DÜRFEN keine Autorität verleihen.

19. Widerrufene Capabilities DÜRFEN keine Autorität verleihen.

20. Nicht verifizierbare Capabilities DÜRFEN keine Autorität verleihen.

21. Capability Attenuation MUSS restriktive Interpretation ermöglichen.

22. Least Privilege MUSS mit Fail-Safe Defaults kombiniert werden.

23. Unklare Permission Inheritance DARF keine Rechte erweitern.

24. Privilegierte Operationen MÜSSEN einen positiven Autorisierungsweg besitzen.

25. Trust Unknown DARF ein Trust Minimum NICHT erfüllen.

26. Trust Verification Failure MUSS explizit behandelbar sein.

27. Trust Failure DARF NICHT automatisch zu Trust Downgrade führen.

28. Unbekannte Identität DARF keine privilegierte Identität erzeugen.

29. Unbekannte Privilegien MÜSSEN als nicht vorhanden behandelt werden.

30. Data Sovereignty Unknown MUSS konservativ behandelt werden.

31. Unbekannte Sovereignty DARF keine Remote-Übertragung autorisieren.

32. Local Processing SOLL bei unbekannter Residency Policy bevorzugt werden.

33. Unknown Information Flow Permission MUSS neue Flows blockieren.

34. Widerrufene Berechtigungen DÜRFEN neue Operationen NICHT autorisieren.

35. Revocation Uncertainty MUSS Contract-basiert behandelt werden.

36. Hochprivilegierte Operationen SOLLEN bei nicht prüfbarer Revocation fail closed arbeiten.

37. Fail Open DARF NICHT allgemeiner NovaOS-Default sein.

38. Fail Open MUSS explizit erlaubt werden.

39. Fail Open DARF Safety NICHT verletzen.

40. Fail Open DARF Security Contracts NICHT verletzen.

41. Safety-vs-Security-Konflikte MÜSSEN explizit modelliert werden.

42. Solche Konflikte DÜRFEN NICHT implizit heuristisch entschieden werden.

43. Safety-kritische Komponenten MÜSSEN explizite Failure Policies besitzen.

44. Unknown Integrity MUSS als unverified behandelt werden.

45. Corrupted Data DARF NICHT stillschweigend als gültig behandelt werden.

46. Verifizierte Recovery Copies DÜRFEN als sichere Fallbacks verwendet werden.

47. Read-Only MUSS als möglicher Safe State unterstützt werden.

48. Unsicherer Filesystem State SOLL Read-Only Recovery ermöglichen.

49. Self-Healing DARF bei unbekanntem Zustand NICHT automatisch destruktive Reparaturen ausführen.

50. Destruktive Operationen MÜSSEN starke positive Autorisierung besitzen.

51. Fehlende erforderliche Confirmation MUSS als Cancel beziehungsweise Not Authorized behandelt werden.

52. Confirmation Timeout DARF NICHT als Zustimmung interpretiert werden.

53. Unknown Transaction Outcome MUSS explizit modelliert werden.

54. Unknown Transaction Outcome DARF NICHT automatisch als fehlgeschlagene Operation interpretiert werden.

55. Nicht idempotente Mutationen DÜRFEN nach Unknown Outcome NICHT blind wiederholt werden.

56. Transaction IDs SOLLEN für relevante Mutationen verwendet werden.

57. Idempotency Keys SOLLEN unterstützt werden.

58. Commit State MUSS rekonstruierbar sein können, wenn Recovery dies erfordert.

59. Rollback DARF nur bei rollbackfähigen Operationen verwendet werden.

60. Forward Recovery MUSS unterstützt werden können.

61. Ungültiger Desired State DARF NICHT aktiviert werden.

62. Ein bestehender Known-Good State SOLL bei ungültiger neuer Konfiguration erhalten bleiben.

63. Beschädigte Konfiguration DARF NICHT automatisch durch permissive Defaults ersetzt werden.

64. Security Defaults MÜSSEN konservativ entworfen werden.

65. Fehlende Konfiguration DARF NICHT implizit alle Features aktivieren.

66. Mechanismen DÜRFEN bei Policy-Ausfall keine permissive Policy erfinden.

67. Policy Failure MUSS einen definierten Safe Default besitzen.

68. Cached Policies DÜRFEN nur innerhalb ihrer Gültigkeitsbedingungen verwendet werden.

69. Abgelaufene Security Policies DÜRFEN NICHT automatisch weiterverwendet werden.

70. Policies MÜSSEN versionierbar sein.

71. Policies MÜSSEN vor Aktivierung validierbar sein.

72. Policy Hot Replacement MUSS atomar oder transaktional erfolgen können.

73. Fehlgeschlagenes Policy Replacement SOLL den vorherigen Known-Good State erhalten.

74. Ein Policy Fallback MUSS weiterhin alle Hard Constraints erfüllen.

75. Execution Contracts MÜSSEN Failure Policies ausdrücken können.

76. Failure Policies SOLLEN `Fail` ausdrücken können.

77. Failure Policies SOLLEN `Fallback` ausdrücken können.

78. Failure Policies SOLLEN `Degrade` ausdrücken können.

79. Failure Policies SOLLEN `Retry` ausdrücken können.

80. Failure Policies SOLLEN `Rollback` ausdrücken können.

81. Failure Policies SOLLEN `ForwardRecover` ausdrücken können.

82. Failure Policies SOLLEN `ReadOnly` ausdrücken können.

83. Failure Policies SOLLEN `Suspend` ausdrücken können.

84. Failure Policies SOLLEN `AskUser` ausdrücken können.

85. Fallbacks MÜSSEN Hard Contracts erfüllen.

86. Graceful Degradation DARF nur auf sichere gültige Zustände wechseln.

87. Degradation DARF Security NICHT unter Hard Minimum reduzieren.

88. Degradation DARF Trust NICHT unter Hard Minimum reduzieren.

89. Degradation DARF Sovereignty NICHT verletzen.

90. Bei Degradation Exhaustion MUSS eine Fail-Safe Action existieren können.

91. Resource Pressure DARF Hard Constraints NICHT aufheben.

92. Insufficient Resources SOLL zunächst sichere Degradation ermöglichen.

93. Ist keine sichere Degradation möglich, MUSS Failure zulässig sein.

94. Emergency Resources SOLLEN reservierbar sein.

95. Memory Allocation Failure MUSS explizit behandelbar sein.

96. Allocation Failure DARF NICHT zu uninitialisiertem Zugriff führen.

97. Integer Overflow MUSS bei sicherheitsrelevanten Größenberechnungen erkannt werden.

98. Overflow DARF NICHT zu kleinerer unsicherer Allokation führen.

99. Resource Accounting Failure DARF NICHT unbegrenzte Ressourcen gewähren.

100. Unknown Budget MUSS konservativ behandelt werden.

101. Adaptive Priority Failure DARF NICHT maximale Priorität erzeugen.

102. Realtime Privilege MUSS explizit autorisiert sein.

103. Unknown Realtime Permission MUSS Realtime Privilege verweigern.

104. Nicht garantierbare Hard Deadlines DÜRFEN NICHT als garantiert gemeldet werden.

105. Required Determinism MUSS bei fehlender Garantie fehlschlagen oder sicheren deterministischen Fallback verwenden.

106. NovaOS DARF NICHT stillschweigend von deterministischer zu nichtdeterministischer Ausführung wechseln.

107. Structured Concurrency MUSS Fehlerpropagation unterstützen.

108. Parent Failure DARF NICHT unbeabsichtigt privilegierte Orphan Tasks erzeugen.

109. Cancellation MUSS definierte Scope-Semantik besitzen.

110. Cleanup MUSS auf Failure Paths möglich sein.

111. Ressourcen MÜSSEN auf Failure Paths kontrolliert freigegeben werden können.

112. Abgelaufene Leases DÜRFEN NICHT als gültig behandelt werden.

113. Network Failure DARF NICHT automatisch Remote Failure bedeuten.

114. Connection Loss MUSS Unknown Remote Outcome darstellen können.

115. Remote Mutations SOLLEN Durable Operation IDs unterstützen.

116. Erforderliche Transport Security DARF NICHT bei Handshake Failure abgeschwächt werden.

117. Secure Transport Failure DARF NICHT automatisch unverschlüsselten Transport aktivieren.

118. Certificate Validation Failure MUSS Verbindung blockieren können.

119. Unknown Certificate Status DARF NICHT als Trusted gelten.

120. Unsichere Namensauflösung DARF keine privilegierte Identität erzeugen.

121. Location Unknown DARF keine beliebige Remote-Ausführung autorisieren.

122. Verified Local Fallback SOLL bevorzugt werden können.

123. Remote Provider MÜSSEN Identity Requirements erfüllen.

124. Remote Provider MÜSSEN Trust Requirements erfüllen.

125. Remote Provider MÜSSEN Authorization Requirements erfüllen.

126. Remote Provider MÜSSEN Sovereignty Requirements erfüllen.

127. Network Partition MUSS von bestätigtem Node Failure unterscheidbar sein.

128. Split Brain MUSS bei exklusiver Ownership verhindert werden können.

129. Abgelaufene Ownership Leases DÜRFEN NICHT stillschweigend verlängert werden.

130. Fencing SOLL für relevante Distributed Ownership verwendet werden.

131. Data Integrity SOLL bei unsicherem Storage State gegenüber Availability priorisiert werden.

132. Beschädigte Dateisysteme SOLLEN Read-Only Recovery unterstützen.

133. Unklare Storage Metadata SOLL Write Protection auslösen können.

134. Journal Recovery SOLL vor normalen Writes abgeschlossen werden können.

135. Unknown Block Ownership DARF NICHT durch blindes Überschreiben gelöst werden.

136. Unknown Key State MUSS explizit modellierbar sein.

137. Widerrufene Keys DÜRFEN NICHT weiterverwendet werden.

138. Key Destruction MUSS besonders geschützt sein.

139. Backup Integrity MUSS vor sicherheitskritischer Recovery validierbar sein.

140. Recovery DARF NICHT automatisch einen Known-Good State mit unverified Data überschreiben.

141. Fail-Safe Defaults MÜSSEN während des Bootprozesses gelten.

142. Nicht verifizierbare Bootartefakte DÜRFEN bei erforderlichem Trust NICHT ausgeführt werden.

143. Boot Fallback MUSS Trust Policies respektieren.

144. Boot Fallback MUSS Anti-Rollback Policies respektieren.

145. Recovery Environment MUSS als Safe Fallback verwendbar sein können.

146. NovaDOS MUSS unabhängig vom normalen NovaOS Recovery ermöglichen können.

147. Grafikbeschleunigung DARF NICHT Voraussetzung für fundamentale Boot-Diagnose sein.

148. Framebuffer Rendering SOLL als Safe Graphics Fallback verfügbar sein.

149. Unbekannte Treiber DÜRFEN NICHT automatisch maximale Kernelrechte erhalten.

150. Treiber SOLLEN entsprechend Trust und Capability isoliert werden.

151. Kann erforderliche Driver Isolation nicht aufgebaut werden, SOLL der Treiber nicht geladen werden.

152. DMA DARF NICHT automatisch gesamten physischen Speicher adressieren dürfen.

153. IOMMU SOLL DMA entsprechend Device Capabilities begrenzen.

154. Device Failure SOLL auf kleine Failure Domains begrenzt werden.

155. Externe Geräte DÜRFEN NICHT automatisch privilegierten Zugriff erhalten.

156. Wechselmedien DÜRFEN NICHT standardmäßig unbekannte Programme automatisch ausführen.

157. Unbekannte Executables DÜRFEN NICHT allein aufgrund ihrer Dateiendung als vertrauenswürdig gelten.

158. Unknown Semantic Type DARF NICHT automatisch als kompatibel gelten.

159. Unchecked Casts MÜSSEN explizit unsafe sein.

160. Externe Daten SOLLEN als `Unvalidated<T>` darstellbar sein.

161. Type Confusion MUSS verhindert werden.

162. Inkompatible Schema-Versionen DÜRFEN NICHT stillschweigend gleichgesetzt werden.

163. Unknown Fields DÜRFEN keine privilegierte Semantik erhalten.

164. Required Pipeline Stages DÜRFEN NICHT stillschweigend übersprungen werden.

165. Nur explizit optionale Pipeline Stages DÜRFEN bei Fehler übersprungen werden.

166. Pipeline Success MUSS alle Required Contracts erfüllen.

167. Zero-Copy DARF NICHT verwendet werden, wenn Ownership unsicher ist.

168. Zero-Copy DARF NICHT verwendet werden, wenn Lifetime unsicher ist.

169. Zero-Copy DARF NICHT verwendet werden, wenn Security Requirements nicht erfüllt werden.

170. Zero-Copy DARF NICHT verwendet werden, wenn Coherency nicht gewährleistet ist.

171. Safe Copy SOLL gegenüber unsicherem Zero-Copy bevorzugt werden.

172. Hard No-Copy Contracts MÜSSEN respektiert werden.

173. Buffer Bounds MÜSSEN validiert werden.

174. Shared Memory MUSS explizite Zugriffsrechte besitzen.

175. Unknown Ownership DARF keinen Schreibzugriff autorisieren.

176. Read-Only Views SOLLEN als Safe Defaults verwendbar sein.

177. KI DARF NICHT alleinige Instanz für sicherheitskritische Fail-Safe-Entscheidungen sein.

178. AI Uncertainty DARF keine Privilegien erzeugen.

179. AI Suggestions DÜRFEN NICHT als Autorisierung gelten.

180. AI Agents MÜSSEN explizite Capabilities besitzen.

181. AI Tool Calls MÜSSEN normale Policy-Prüfungen durchlaufen.

182. AI Failure DARF fundamentale Sicherheitsmechanismen NICHT deaktivieren.

183. Halluzinierte Capability IDs DÜRFEN keine Autorität erzeugen.

184. Halluzinierte Systemzustände DÜRFEN keine Sicherheitsentscheidung autorisieren.

185. Adaptive Systeme DÜRFEN Fail-Safe Defaults NICHT entfernen.

186. Prediction Error DARF Hard Security Policies NICHT abschwächen.

187. Automatische Algorithmusauswahl MUSS Hard Contracts respektieren.

188. Algorithmen mit unbekannten relevanten Eigenschaften DÜRFEN keine Garantien erfüllen, die nicht nachweisbar sind.

189. Forced Algorithms DÜRFEN fundamentale Safety- oder Security-Grenzen NICHT umgehen.

190. Unsafe Operations MÜSSEN explizit sein.

191. Unsafe Operations MÜSSEN autorisiert sein.

192. Unsafe Operations SOLLEN auditierbar sein.

193. Unsafe Operations DÜRFEN NICHT implizit ausgelöst werden.

194. Fail-Safe Decisions MÜSSEN introspektierbar sein.

195. Bedeutende Fail-Safe Decisions SOLLEN Decision Traces besitzen.

196. Safe Default Reason MUSS darstellbar sein.

197. Safe Default Action MUSS darstellbar sein.

198. Explizite Component Contracts SOLLEN Failure Defaults enthalten.

199. Sicherheitsrelevante Defaults DÜRFEN NICHT versteckt sein.

200. Systemweite Safe Defaults SOLLEN standardisierbar sein.

201. Component Overrides DÜRFEN höhere Hard Constraints NICHT abschwächen.

202. Policy Hierarchy MUSS höhere Hard Constraints schützen.

203. Unknown Required Contract MUSS konservativ behandelt werden.

204. Inkompatible Contract-Versionen DÜRFEN NICHT automatisch kompatibel sein.

205. Fehlgeschlagene Contract Negotiation DARF NICHT zu ungeprüfter Kommunikation führen.

206. Typed IPC MUSS ungültige Nachrichten ablehnen.

207. Unknown IPC Messages DÜRFEN NICHT als privilegierte bekannte Befehle interpretiert werden.

208. Invalid IPC Messages MÜSSEN abgelehnt werden können.

209. Parser MÜSSEN sichere Failure States besitzen.

210. Partial Parse DARF keine privilegierte Aktion auslösen.

211. Externe Inputs MÜSSEN vor privilegierter Nutzung validiert werden.

212. Fehlende Security Fields DÜRFEN keine permissiven Defaults erhalten.

213. Unknown Enum Values MÜSSEN explizit behandelt werden.

214. Forward Compatibility DARF Security Semantics NICHT permissiv erraten.

215. Neue Permissions DÜRFEN für alte Clients NICHT automatisch gewährt werden.

216. Neue privilegierte Features SOLLEN standardmäßig deaktiviert sein.

217. Live Evolution DARF neue Security Semantics NICHT ungeprüft auf alte Contracts übertragen.

218. Kernelmechanismen MÜSSEN konservative Fail-Safe Defaults verwenden.

219. Ungültige Kernelpointer DÜRFEN NICHT dereferenziert werden.

220. User Pointer MÜSSEN vor privilegiertem Zugriff validiert werden.

221. Ungültige Syscall-Parameter MÜSSEN abgelehnt werden.

222. Unknown Syscalls DÜRFEN keine zufällige Operation auslösen.

223. Ungültige Kernel Object Handles DÜRFEN keine Rechte erzeugen.

224. Stale Handles SOLLEN erkannt werden können.

225. Ungültige Capability Handles MÜSSEN Null Authority besitzen.

226. Isolation Failure DARF NICHT zu weniger isolierter Ausführung führen, wenn Isolation Hard Requirement ist.

227. Sandbox Failure DARF NICHT automatisch unsandboxed Execution erzeugen.

228. Compatibility Layers DÜRFEN bei Fehlern keine nativen Privilegien erzeugen.

229. Unknown Legacy Permissions DÜRFEN NICHT auf umfassende NovaOS-Rechte abgebildet werden.

230. ABI Inputs MÜSSEN validierbar sein.

231. FFI Boundaries SOLLEN explizite Validierung besitzen.

232. NovaLang SOLL sichere Sprachdefaults unterstützen.

233. Uninitialisierte Variablen DÜRFEN NICHT als gültige Zufallswerte verwendet werden.

234. Nullability SOLL explizit modelliert werden.

235. Fehler DÜRFEN NICHT automatisch in semantisch falsche Erfolgswerte umgewandelt werden.

236. Result-or-Error-Semantik SOLL unterstützt werden.

237. Lokale Fehler SOLLEN innerhalb kleiner Failure Domains bleiben.

238. Nicht jeder lokale Fehler DARF einen System-Panic erzwingen.

239. Failure Containment MUSS mit Fail-Safe Defaults kombiniert werden.

240. Self-Healing MUSS Fehler vor destruktiver Reparatur klassifizieren.

241. Unknown Failure DARF keine irreversible automatische Reparatur autorisieren.

242. Repair Plans SOLLEN vor Ausführung validiert werden.

243. Recovery-Komponenten SOLLEN Least Privilege verwenden.

244. Audit Requirements MÜSSEN als Hard oder BestEffort klassifizierbar sein.

245. Hard Audit Failure DARF eine Operation fail closed behandeln.

246. BestEffort Audit Failure DARF entsprechend Contract fortgesetzt werden.

247. Kritische Audit-Daten SOLLEN reservierte Ressourcen verwenden können.

248. Audit Overflow MUSS explizit behandelt werden.

249. Fail-Safe Events SOLLEN beobachtbar sein.

250. Observability DARF Privacy NICHT verletzen.

251. Observability DARF Data Sovereignty NICHT verletzen.

252. Benutzerrelevante Safe Defaults SOLLEN verständlich erklärt werden.

253. Entwicklerdiagnose SOLL strukturierte technische Gründe bereitstellen.

254. Interne Safe Defaults DÜRFEN ohne Benutzerinteraktion erfolgen, wenn keine relevante Entscheidung erforderlich ist.

255. Mehrere sichere Alternativen DÜRFEN Benutzerentscheidung erfordern.

256. Security-relevanter UI Timeout DARF NICHT privilegierte Zustimmung bedeuten.

257. Destruktive UI-Aktionen SOLLEN einen nichtdestruktiven Default besitzen.

258. Dialogschließen DARF NICHT als Zustimmung zu destruktiver Aktion gelten.

259. Fail-Safe UI MUSS Accessibility berücksichtigen.

260. Unknown Trusted Time MUSS konservativ behandelt werden.

261. Token Expiration DARF bei unbekannter Zeit NICHT automatisch ignoriert werden.

262. Wall Clock und Monotonic Time MÜSSEN semantisch getrennt werden.

263. Fehlende kryptographische Entropie DARF NICHT durch schwache Secrets kompensiert werden.

264. CSPRNG-Anforderungen MÜSSEN Hard Requirements sein können.

265. Crypto Failure MUSS explizit propagiert werden.

266. Signature Verification Error DARF NICHT als Valid gelten.

267. Hash Verification Error DARF NICHT als erfolgreiche Integritätsprüfung gelten.

268. Nicht verifizierte Systemupdates DÜRFEN NICHT aktiviert werden.

269. Unterbrochene Updates MÜSSEN recoverable sein.

270. Known-Good Generations SOLLEN unterstützt werden.

271. Neue Generationen SOLLEN erst nach Health Verification als Known-Good markiert werden.

272. Failed New Generation SOLL Rollback ermöglichen.

273. Rollback MUSS Anti-Rollback Security respektieren.

274. Unsichere alte Versionen DÜRFEN NICHT allein wegen Verfügbarkeit automatisch gestartet werden.

275. Wenn keine sichere Generation verfügbar ist, MUSS Recovery möglich sein.

276. Konfigurationsänderungen SOLLEN transaktional erfolgen.

277. Partial Configuration DARF NICHT als vollständig gültig behandelt werden.

278. Parallel auftretende Fehler MÜSSEN ohne Privilegieneskalation behandelbar sein.

279. Fail-Safe Defaults MÜSSEN auch bei Resource Pressure gelten.

280. Fail-Safe Defaults MÜSSEN auch im Brownout Mode gelten.

281. Fail-Safe Defaults MÜSSEN auch während Self-Healing gelten.

282. Fail-Safe Defaults MÜSSEN auch während Hot Replacement gelten.

283. Fail-Safe Defaults MÜSSEN auch während Recovery gelten.

284. Fail-Safe Defaults MÜSSEN auch während Boot gelten.

285. Fail-Safe Defaults MÜSSEN auch bei Remote Execution gelten.

286. Fail-Safe Defaults MÜSSEN auch bei Offline-Betrieb gelten.

287. Fail-Safe Defaults MÜSSEN ohne KI funktionieren.

288. Fail-Safe Defaults MÜSSEN ohne Cloud funktionieren.

289. Fail-Safe Defaults DÜRFEN NICHT von adaptiven Diensten abhängig sein.

290. Fundamentale Fail-Safe-Pfade SOLLEN statisch implementierbar sein.

291. Fail-Safe Policies MÜSSEN testbar sein.

292. Safe States SOLLEN automatisch validierbar sein.

293. Safe Defaults SOLLEN versionierbar sein.

294. Safe Default Changes SOLLEN auditierbar sein.

295. Safe Defaults SOLLEN über Architecture Introspection sichtbar sein.

296. Failure Injection MUSS Fail-Safe-Pfade testen können.

297. Recovery Tests MÜSSEN Known-Good-State-Erhaltung prüfen.

298. Security Tests MÜSSEN prüfen, dass Fehler keine Authority erzeugen.

299. Concurrency Tests MÜSSEN Race Conditions auf Fail-Safe-Pfaden prüfen.

300. Resource Exhaustion Tests MÜSSEN sichere Failure Semantics prüfen.

301. Corruption Tests MÜSSEN sichere Recovery States prüfen.

302. Network Partition Tests MÜSSEN Unknown Outcome korrekt prüfen.

303. Policy Failure Tests MÜSSEN Default-Deny-Verhalten prüfen.

304. Trust Failure Tests MÜSSEN Unknown != Trusted prüfen.

305. Sovereignty Failure Tests MÜSSEN Local-Safe-Defaults prüfen.

306. Transaction Tests MÜSSEN doppelte Mutation verhindern.

307. Boot Tests MÜSSEN nicht verifizierbare Artefakte ablehnen.

308. Driver Tests MÜSSEN Isolation Failure korrekt behandeln.

309. IPC Tests MÜSSEN unbekannte privilegierte Nachrichten ablehnen.

310. Parser Tests MÜSSEN malformed Input sicher behandeln.

311. AI Tests MÜSSEN zeigen, dass AI Output keine Authority erzeugt.

312. Update Tests MÜSSEN fehlgeschlagene Generationen sicher zurücksetzen.

313. Long-Running Tests MÜSSEN stale Security State erkennen.

314. Fail-Safe-Pfade DÜRFEN keine Ressourcenlecks verursachen.

315. Fail-Safe-Pfade DÜRFEN keine Capability-Leaks verursachen.

316. Fail-Safe-Pfade DÜRFEN keine Lock-Leaks verursachen.

317. Fail-Safe-Pfade DÜRFEN keine privilegierten Orphan Tasks erzeugen.

318. Ein Safe Default MUSS mindestens so restriktiv wie der erforderliche Hard Contract sein.

319. Ein Fehler in einem Optimierungsmechanismus DARF NICHT die Sicherheitssemantik verändern.

320. NovaOS MUSS nach dem Invariant arbeiten: `Failure must not create authority.`

---

# Konsequenzen

## Positive Konsequenzen

- Sicherheitsfehler führen nicht automatisch zu zusätzlicher Autorität.
- Fehlende Policies führen nicht zu permissiven Systemzuständen.
- Capability-Ausfälle erzeugen keine impliziten Rechte.
- Trust-Unklarheiten werden konservativ behandelt.
- Data Sovereignty bleibt auch bei Fehlern erhalten.
- Information Flow Restrictions bleiben bei Fehlern erhalten.
- beschädigte Dateisysteme können sicher Read-Only weiterbetrieben werden.
- fehlerhafte Updates können auf Known-Good-Generationen zurückfallen.
- Bootprozesse können auf verifizierte Recovery-Pfade wechseln.
- Graceful Degradation bleibt innerhalb sicherer Grenzen.
- Self-Healing kann keine beliebigen destruktiven Reparaturen aus Unsicherheit ableiten.
- Hot Replacement behält bekannte gute Zustände.
- Remote Execution wird bei unbekannter Location oder Trust nicht unkontrolliert aktiviert.
- Zero-Copy wird nicht auf Kosten von Ownership oder Memory Safety erzwungen.
- KI-Systeme erhalten keine implizite Autorität.
- NovaOS bleibt auch bei Teilfehlern nachvollziehbar.
- Architecture Introspection kann Safe-Default-Entscheidungen erklären.
- Recovery wird systematisch von unsicheren Best-Effort-Reaktionen getrennt.
- Failure Containment wird gestärkt.
- das System verhält sich unter ungewöhnlichen Fehlerbedingungen vorhersehbarer.

---

# Negative Konsequenzen

- bestimmte Operationen können häufiger abgelehnt werden.
- temporäre Policy-Ausfälle können Funktionen blockieren.
- Trust-Infrastruktur muss hochverfügbar oder mit sicheren Offline-Mechanismen versehen sein.
- Offline-Systeme benötigen gecachte, validierte Policies.
- Fail-Closed kann Availability reduzieren.
- Recovery-Logik wird komplexer.
- Safe States müssen für viele Subsysteme explizit definiert werden.
- Safety-vs-Security-Konflikte benötigen domainspezifische Entscheidungen.
- Distributed Transactions benötigen Unknown-Outcome-Semantik.
- Anwendungen müssen Fehler expliziter behandeln.
- Entwickler können sich weniger auf implizite Defaults verlassen.
- Tests müssen wesentlich mehr Failure Paths abdecken.
- einige Performance-Optimierungen müssen bei Unsicherheit deaktiviert werden.
- Legacy-Kompatibilität kann restriktiver ausfallen.
- ein konservativer Safe Default kann zeitweise Funktionalität reduzieren.

---

# Verworfene Alternativen

## Fail Open als allgemeiner Default

Verworfen.

```text
Security mechanism failed
        ↓
Allow
```

würde Fehler in Autorisierung verwandeln.

---

## Best Effort bei Security

Verworfen.

Security Hard Requirements dürfen nicht aufgrund von Verfügbarkeit aufgegeben werden.

---

## Unknown = Allowed

Verworfen.

```text
Unknown
```

ist ein eigener Zustand und keine positive Autorisierung.

---

## Unknown = Denied für sämtliche Systemzustände

Als universelles Modell verworfen.

Für Autorisierung ist Default Deny richtig.

Für andere Subsysteme kann ein anderer sicherer Zustand sinnvoll sein:

```text
ReadOnly
Fallback
Suspend
Recovery
```

---

## Jeder Fehler führt zum System-Shutdown

Verworfen.

Fail-Safe ist nicht identisch mit Fail-Stop.

---

## Immer vorherigen Zustand verwenden

Verworfen.

Der vorherige Zustand kann:

- abgelaufen,
- widerrufen,
- kompromittiert,
- inkompatibel

sein.

---

## Immer neueste Konfiguration verwenden

Verworfen.

Neu bedeutet nicht automatisch valide oder sicher.

---

## Automatische Cloud-Auslagerung bei lokalem Fehler

Verworfen.

Kann Sovereignty, Trust, Privacy und Kostenanforderungen verletzen.

---

## Automatisches Security Downgrade

Verworfen.

Beispiel:

```text
TLS unavailable
    ↓
plaintext
```

ist kein zulässiger allgemeiner Fallback.

---

## KI entscheidet Safe State

Verworfen.

KI kann unterstützen, aber keine fundamentale Sicherheitsautorität ersetzen.

---

## Self-Healing darf alles reparieren

Verworfen.

Recovery muss weiterhin Capability-, Trust- und Transaction-Regeln erfüllen.

---

## Fehler verschlucken

Verworfen.

Fehler müssen semantisch sichtbar bleiben.

---

## Alle unbekannten Typen akzeptieren

Verworfen.

Dies würde Type Confusion und Contract-Verletzungen ermöglichen.

---

## Zero-Copy um jeden Preis

Verworfen.

Memory Safety, Ownership und Security haben Vorrang.

---

## Availability vor Integrity

Als allgemeines Prinzip verworfen.

Domains können unterschiedliche Anforderungen besitzen, aber NovaOS darf Integrität nicht stillschweigend für Verfügbarkeit opfern.

---

# Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starken_Isolationsgrenzen`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage`
- `ADR-ARCH-0004_Transaktionale_Systemoperationen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern`
- `ADR-ARCH-0008_Semantic_Types_als_systemweites_Typsystem`
- `ADR-ARCH-0009_Einheitliches_Daten_und_Objekt_Pipelining`
- `ADR-ARCH-0010_Structured_Concurrency_als_systemweites_Ausführungsmodell`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0012_Hot_Replacement_und_Live_Evolution`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts_zwischen_Systemkomponenten`
- `ADR-ARCH-0015_Graceful_Degradation_als_Systemprinzip`
- `ADR-EXECUTION-0001_Nova_ExecutionContract`
- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur`
- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-DATA-0001_Einheitliches_Nova_Datenobjektmodell`

---

# Zugehörige NPSPECs

- `NPSPEC-ARCH-FAILSAFE-0001`
- `NPSPEC-FAILSAFE-MODEL-0001`
- `NPSPEC-FAILSAFE-STATE-0001`
- `NPSPEC-FAILSAFE-DEFAULT-0001`
- `NPSPEC-FAILSAFE-POLICY-0001`
- `NPSPEC-FAILSAFE-DECISION-0001`
- `NPSPEC-FAILSAFE-FALLBACK-0001`
- `NPSPEC-FAILSAFE-FAILCLOSED-0001`
- `NPSPEC-FAILSAFE-FAILOPEN-0001`
- `NPSPEC-FAILSAFE-RECOVERY-0001`
- `NPSPEC-FAILSAFE-INTROSPECTION-0001`
- `NPSPEC-FAILSAFE-DECISIONTRACE-0001`
- `NPSPEC-FAILSAFE-AUDIT-0001`
- `NPSPEC-FAILSAFE-TESTING-0001`
- `NPSPEC-FAILSAFE-FAULTINJECTION-0001`
- `NPSPEC-SECURITY-DEFAULTDENY-0001`
- `NPSPEC-SECURITY-LEASTPRIVILEGE-0001`
- `NPSPEC-SECURITY-FAILCLOSED-0001`
- `NPSPEC-SECURITY-AUTHORIZATION-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-SECURITY-ISOLATION-FAILURE-0001`
- `NPSPEC-CAPABILITY-VALIDATION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-CAPABILITY-LEASE-0001`
- `NPSPEC-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-TRUST-FAILURE-0001`
- `NPSPEC-TRUST-UNKNOWN-0001`
- `NPSPEC-TRUST-VERIFICATION-0001`
- `NPSPEC-DATA-INTEGRITY-0001`
- `NPSPEC-DATA-SOVEREIGNTY-0001`
- `NPSPEC-DATA-INFORMATIONFLOW-0001`
- `NPSPEC-DATA-VALIDATION-0001`
- `NPSPEC-STORAGE-READONLY-FALLBACK-0001`
- `NPSPEC-STORAGE-INTEGRITY-RECOVERY-0001`
- `NPSPEC-STORAGE-JOURNAL-RECOVERY-0001`
- `NPSPEC-TRANSACTION-UNKNOWNOUTCOME-0001`
- `NPSPEC-TRANSACTION-IDEMPOTENCY-0001`
- `NPSPEC-TRANSACTION-RECOVERY-0001`
- `NPSPEC-CONFIG-KNOWNGOOD-0001`
- `NPSPEC-CONFIG-SAFEDEFAULT-0001`
- `NPSPEC-POLICY-VALIDATION-0001`
- `NPSPEC-POLICY-CACHE-0001`
- `NPSPEC-POLICY-FAILURE-0001`
- `NPSPEC-POLICY-KNOWNGOOD-0001`
- `NPSPEC-EXECUTION-FAILUREPOLICY-0001`
- `NPSPEC-EXECUTION-SAFEFALLBACK-0001`
- `NPSPEC-EXECUTION-UNKNOWNOUTCOME-0001`
- `NPSPEC-RESOURCE-SAFEFAILURE-0001`
- `NPSPEC-RESOURCE-ALLOCATION-FAILURE-0001`
- `NPSPEC-RESOURCE-BUDGET-UNKNOWN-0001`
- `NPSPEC-LOCATION-SAFEFALLBACK-0001`
- `NPSPEC-NETWORK-SECUREFALLBACK-0001`
- `NPSPEC-NETWORK-UNKNOWNOUTCOME-0001`
- `NPSPEC-DISTRIBUTED-FENCING-0001`
- `NPSPEC-DISTRIBUTED-LEASE-0001`
- `NPSPEC-BOOT-FAILSAFE-0001`
- `NPSPEC-BOOT-KNOWNGOOD-0001`
- `NPSPEC-BOOT-ROLLBACKPROTECTION-0001`
- `NPSPEC-DRIVER-SAFELOAD-0001`
- `NPSPEC-DRIVER-ISOLATION-FAILURE-0001`
- `NPSPEC-DMA-SAFEDEFAULT-0001`
- `NPSPEC-IPC-VALIDATION-0001`
- `NPSPEC-IPC-UNKNOWNMESSAGE-0001`
- `NPSPEC-TYPE-UNVALIDATED-0001`
- `NPSPEC-TYPE-SAFECAST-0001`
- `NPSPEC-SERIALIZATION-SAFEDEFAULT-0001`
- `NPSPEC-PIPELINE-FAILUREPOLICY-0001`
- `NPSPEC-MEMORY-SAFEFAILURE-0001`
- `NPSPEC-MEMORY-OWNERSHIP-FAILURE-0001`
- `NPSPEC-AI-AUTHORITY-0001`
- `NPSPEC-AI-SAFEFAILURE-0001`
- `NPSPEC-AGENT-CAPABILITY-0001`
- `NPSPEC-UPDATE-KNOWNGOOD-0001`
- `NPSPEC-UPDATE-FAILSAFE-0001`
- `NPSPEC-UPDATE-ROLLBACKPROTECTION-0001`
- `NPSPEC-CRYPTO-FAILSAFE-0001`
- `NPSPEC-CRYPTO-RNG-0001`
- `NPSPEC-CRYPTO-VERIFICATION-0001`
- `NPSPEC-OBSERVABILITY-FAILSAFE-0001`

---

# Verifikation und Tests

Die Umsetzung dieser ADR MUSS insbesondere durch Security-, Failure-, Corruption-, Fault-Injection-, Recovery-, Transaction-, Concurrency-, Resource-Exhaustion- und Integrationstests überprüft werden.

## Default-Deny-Test

Ohne gültige Capability:

```text
Access
   ↓
Denied
```

---

## Unknown-Permission-Test

Bei:

```text
Permission = Unknown
```

darf kein Zugriff gewährt werden.

---

## Invalid-Capability-Test

Beschädigte Capabilities dürfen keine Autorität erzeugen.

---

## Revocation-Test

Widerrufene Capabilities dürfen nicht weiterverwendet werden.

---

## Trust-Test

Bei:

```text
Required = Verified
Actual   = Unknown
```

muss der Provider abgelehnt werden.

---

## Sovereignty-Test

Bei unbekannter Data-Sovereignty-Klassifikation darf kein automatischer Cloud-Upload erfolgen.

---

## Information-Flow-Test

Unbekannte Flow Permission muss den neuen Flow blockieren.

---

## Integrity-Test

Daten mit unbekannter Integrität dürfen nicht als verifiziert behandelt werden.

---

## Read-Only-Test

Ein beschädigtes Dateisystem muss auf einen definierten Read-Only-Recovery-Pfad wechseln können.

---

## Transaction-Test

Unknown Outcome darf keine doppelte nicht-idempotente Mutation erzeugen.

---

## Confirmation-Test

Ein Confirmation Timeout darf keine destruktive Aktion bestätigen.

---

## Policy-Failure-Test

Ausfall des Policy Providers darf keine permissive Policy erzeugen.

---

## Cached-Policy-Test

Abgelaufene gecachte Security Policy darf nicht weiterverwendet werden.

---

## Known-Good-Test

Eine ungültige neue Konfiguration darf den bestehenden Known-Good State nicht zerstören.

---

## Hot-Replacement-Test

Fehlerhafte neue Policy-Generation muss auf eine weiterhin gültige Known-Good-Version zurückfallen können.

---

## Graceful-Degradation-Test

Ein Fallback darf ausschließlich auf eine Contract-kompatible sichere Degradationsstufe wechseln.

---

## Degradation-Exhaustion-Test

Existiert kein sicherer Fallback:

```text
DegradationExhausted
```

muss zu einem definierten sicheren Fehlerpfad führen.

---

## Memory-Allocation-Test

Allocation Failure darf keinen Zugriff auf uninitialisierten Speicher erzeugen.

---

## Overflow-Test

Integer Overflow bei Größenberechnung muss vor Allokation erkannt werden.

---

## Realtime-Test

Unknown Realtime Permission darf keine Realtime-Priorität erhalten.

---

## Determinism-Test

Required Determinism darf nicht stillschweigend durch nondeterministische Ausführung ersetzt werden.

---

## Structured-Concurrency-Test

Parent Failure darf keine privilegierten Orphan Tasks erzeugen.

---

## Lease-Test

Abgelaufene Leases dürfen nicht weiter als gültig gelten.

---

## Network-Unknown-Outcome-Test

Connection Loss nach Remote Mutation muss Unknown Outcome darstellen können.

---

## TLS-Test

TLS Failure darf nicht automatisch auf unsicheren Transport zurückfallen.

---

## Certificate-Test

Certificate Validation Failure muss Verbindung blockieren können.

---

## Location-Test

Unknown Location darf keine automatische Remote Execution auslösen.

---

## Partition-Test

Network Partition darf nicht automatisch als bestätigter Node Failure interpretiert werden.

---

## Split-Brain-Test

Abgelaufene Ownership Lease muss Fencing beziehungsweise Verlust der Schreibautorität auslösen.

---

## Storage-Test

Unklare Metadatenintegrität muss Write Protection aktivieren können.

---

## Boot-Test

Nicht verifizierbares Bootartefakt darf bei Required Trust nicht ausgeführt werden.

---

## Anti-Rollback-Test

Ein bekannt verwundbares altes Image darf nicht als einfacher Fallback gestartet werden.

---

## Driver-Test

Unknown Driver darf nicht automatisch im Kernel mit maximalen Rechten ausgeführt werden.

---

## Driver-Isolation-Test

Kann erforderliche Isolation nicht aufgebaut werden, darf der Treiber nicht weniger isoliert gestartet werden.

---

## DMA-Test

Fehlende DMA-Zuordnung darf keinen Full-Memory-Zugriff erzeugen.

---

## Semantic-Type-Test

Unknown Semantic Type darf nicht automatisch als kompatibel behandelt werden.

---

## IPC-Test

Unknown IPC Message darf keine privilegierte Operation auslösen.

---

## Parser-Test

Malformed Input darf keinen Partial-Parse-Befehl ausführen.

---

## Pipeline-Test

Required Pipeline Stage darf bei Fehler nicht stillschweigend übersprungen werden.

---

## Zero-Copy-Test

Unklare Ownership muss unsicheres Zero-Copy verhindern.

---

## Copy-Fallback-Test

Wenn Copy erlaubt ist, muss bei unsicherem Zero-Copy auf sichere Kopie gewechselt werden können.

---

## AI-Test

AI Output darf ohne Capability keine privilegierte Operation auslösen.

---

## AI-Hallucination-Test

Halluzinierte Capability IDs dürfen keine Rechte erzeugen.

---

## Update-Test

Nicht verifiziertes Update darf nicht aktiviert werden.

---

## Interrupted-Update-Test

Ein unterbrochenes Update muss auf einen recoverable State zurückfallen.

---

## Known-Good-Boot-Test

Eine neue Generation darf erst nach erfolgreichem Health Check als Known-Good gelten.

---

## Crypto-RNG-Test

Fehlende ausreichende Entropie darf nicht zur Erzeugung schwacher Secrets führen.

---

## Signature-Test

Verification Error muss als Failure und nicht als Valid behandelt werden.

---

## Audit-Test

Bei Hard Audit Requirement muss Audit Failure entsprechend fail closed behandelt werden können.

---

## Resource-Exhaustion-Test

Extremer Resource Pressure darf keine Security Policies deaktivieren.

---

## Brownout-Test

Brownout darf Hard Security Requirements nicht reduzieren.

---

## Self-Healing-Test

Unknown Failure darf keine irreversible automatische Reparatur auslösen.

---

## Architecture-Introspection-Test

Für eine Fail-Safe-Entscheidung müssen mindestens darstellbar sein:

```text
Operation
RequestedState
Decision
Reason
SafeDefault
RelevantContract
RelevantPolicy
```

---

## Fault-Injection-Test

Gezielt ausfallen müssen unter anderem:

- Policy Service,
- Trust Service,
- Capability Registry,
- Storage Metadata,
- Network,
- DNS,
- Time Source,
- RNG,
- GPU Driver,
- Filesystem,
- Update Service,
- Audit Service,
- Resource Accounting,
- Contract Registry.

Für jeden Ausfall muss geprüft werden, dass keine zusätzliche Autorität entsteht.

---

## Concurrency-Test

Race Conditions zwischen:

```text
Grant
Revoke
Use
```

müssen sicher behandelt werden.

---

## Long-Running-Test

Langzeitbetrieb darf keine:

- stale Capabilities,
- stale Leases,
- stale Trust Decisions,
- stale Policies,
- unbounded Audit Queues,
- Recovery State Leaks,
- Capability Leaks,
- Handle Leaks

erzeugen.

---

# Ergebnis

Mit `ADR-ARCH-0016` wird Fail-Safe Defaults zu einem fundamentalen NovaOS-Architekturprinzip.

Das System verwendet nicht:

```text
No explicit denial
       ↓
Allow
```

sondern:

```text
Positive authorization
       ↓
Allow
```

Für nicht autorisierungsbezogene Fehler gilt entsprechend:

```text
Failure / Unknown
        ↓
Determine Safe State
        ↓
Validate Hard Contracts
        ↓
Safe Fallback
```

Die Architektur verbindet damit:

```text
Explicit Contracts
        │
        ▼
Requested Operation
        │
        ▼
Capability Validation
        │
        ▼
Trust Validation
        │
        ▼
Policy Validation
        │
        ▼
Integrity Validation
        │
        ▼
Hard Constraint Validation
        │
        ├──────── Valid ────────► Execute
        │
        └──────── Invalid/Unknown
                         │
                         ▼
                  Fail-Safe Default
                         │
              ┌──────────┼──────────┐
              ▼          ▼          ▼
             Deny     Safe Fallback  Read-Only
                         │
                         ▼
                 Graceful Degradation
                         │
                         ▼
                     Recovery
                         │
                         ▼
                    Verification
                         │
                         ▼
                  Known-Good State
```

Dadurch entsteht eine zentrale NovaOS-Invariante:

```text
Failure must never create authority.
```

ergänzt durch:

```text
Unknown is not permission.
```

```text
Unknown is not trust.
```

```text
Unknown is not integrity.
```

```text
Unknown is not success.
```

und schließlich:

```text
When the preferred state cannot
be proven safe,

choose a state that can.
```