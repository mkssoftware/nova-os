# NPSPEC-CAPABILITY-PERMISSION-0001 – Nova Capability Permission

## Status

Angenommen

## Kategorie

Capability / Permission

## Zweck

NovaOS definiert das Berechtigungsmodell für die Nutzung von Capabilities.

Eine Capability darf nur verwendet werden, wenn der aktuelle Sicherheitskontext über die dafür erforderliche Authority verfügt und die geltende Policy die konkrete Nutzung erlaubt.

## Grundprinzipien

```text
CapabilityID ≠ Permission
Discovery ≠ Permission
Request ≠ Grant
Trust ≠ Permission
Declared Capability ≠ Granted Capability
Possession ≠ Delegation Right
Permission ≠ Universal Authority
```

## Modell

Eine Capability-Berechtigung kann mindestens gebunden sein an:

```text
PrincipalID
CapabilityID
Operations
Target
Scope
Constraints
Lifetime
Delegation
SecurityContext
```

Die Berechtigung beschreibt die maximal zulässige Authority.

## Vergabe

```text
Capability Request
       ↓
Requester Identity
       ↓
Required Authority
       ↓
Policy Evaluation
       ↓
Existing Permissions
       ↓
Allow / Restrict / Ask / Deny
       ↓
Capability Token / Handle
```

Nur eine positive Entscheidung darf tatsächliche Capability-Authority erzeugen.

## Least Authority

NovaOS vergibt ausschließlich die für die konkrete Aufgabe erforderliche Authority.

Beispiel:

```text
Requested:
Storage Access

Effective:
Read
Target: ObjectID A
Lifetime: Current Task
```

Eine allgemeinere Capability darf eingeschränkt werden auf:

```text
Operations
Objects
Devices
Resources
Namespace Scope
Time
Resource Budget
```

## Programme

Programme erhalten nicht automatisch die vollständige Authority des ausführenden Benutzers.

```text
User Authority
      ≠
Program Authority
```

Benötigte Capabilities werden separat autorisiert.

## Solutions

Bei Solutions entstehen Berechtigungen ausschließlich aus den explizit verwendeten Capabilities des Logic Graph.

```text
Capability Node
      ↓
Permission Evaluation
      ↓
Custom Script
```

Custom-NovaLang-Code darf keine zusätzliche Capability-Authority selbst anfordern oder erzeugen.

## Persistente Entscheidungen

Berechtigungsentscheidungen dürfen an stabile Identitäten gebunden werden:

```text
PrincipalID
+
CapabilityID
+
Security Context
+
Integrity State
```

Bei sicherheitsrelevanten Änderungen muss eine erneute Bewertung möglich sein.

## Delegation

Eine Capability darf nur delegiert werden, wenn dies ausdrücklich erlaubt ist.

```text
Original Authority
       ↓
Attenuation
       ↓
Delegated Authority
```

Delegation darf Authority niemals erweitern.

## Lifetime

Berechtigungen können zeitlich oder kontextuell begrenzt sein:

```text
Single Call
Task
Process
Session
Workspace
Persistent
Explicit Expiration
```

Temporäre Authority muss nach Ende ihres Scopes ungültig werden können.

## Revocation

Capability-Berechtigungen müssen widerrufbar sein.

Widerruf kann ausgelöst werden durch:

```text
User Decision
Policy Change
Trust Change
Security Event
Identity Change
Expiration
```

## Execution Contract

Ein Execution Contract darf aus bereits vorhandener Authority die minimal erforderliche effektive Authority ableiten:

```text
Existing Authority
       ∩
Contract Requirements
       ∩
Policy
       ↓
Effective Capability Authority
```

Der Contract selbst erzeugt keine Berechtigung.

## Normative Anforderungen

1. Capability-Nutzung MUSS explizite Authority erfordern.
2. `CapabilityID` und Capability-Permission MÜSSEN getrennt bleiben.
3. Discovery DARF keine Permission erzeugen.
4. Trust DARF keine Permission automatisch erzeugen.
5. Programme DÜRFEN nicht automatisch vollständige Benutzer-Authority erben.
6. Solutions DÜRFEN Authority ausschließlich über autorisierte Capabilities erhalten.
7. Custom-NovaLang-Code DARF keine zusätzliche System-Authority erzeugen.
8. Capability-Permissions MÜSSEN auf Operationen, Targets und Scopes begrenzbar sein.
9. NovaOS MUSS Least Authority unterstützen.
10. Delegation DARF Authority ausschließlich erhalten oder reduzieren.
11. Temporäre Authority MUSS eine definierte Lifetime besitzen.
12. Capability-Permissions MÜSSEN widerrufbar sein.
13. Persistente Entscheidungen MÜSSEN an validierbare Identitäten und Sicherheitskontexte gebunden sein.
14. Execution Contracts DÜRFEN keine neue Authority erzeugen.
15. Effektive Permission, Constraints und Entscheidungsgrund MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-POLICY-CAPABILITY-0001`
- `NPSPEC-TRUST-CAPABILITY-0001`
- `NPSPEC-REGISTRY-CAPABILITY-0001`
- `NPSPEC-PROGRAM-PERMISSION-0001`
- `NPSPEC-WORKSPACE-CAPABILITY-0001`
- `NPSPEC-WORKSPACE-PERMISSION-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`

## Ergebnis

NovaOS trennt Capability-Identität, Berechtigungsentscheidung und tatsächliche Authority konsequent voneinander. Programme, Solutions und andere Ausführungskontexte erhalten nur die minimal erforderlichen, begrenzten und widerrufbaren Rechte für die konkrete Nutzung einer Capability.