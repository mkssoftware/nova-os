# NPSPEC-SECURITY-DAC-0001 – Nova Discretionary Access Control

## Status

Angenommen

## Kategorie

Security / Authorization / Discretionary Access Control

## Zweck

NovaOS definiert Discretionary Access Control (DAC) als ergänzende Policy-Schicht für benutzer- und eigentümerbasierte Zugriffssteuerung.

```text
Subject
   ↓
Identity / Ownership
   ↓
DAC Policy
   ↓
Requested Operation
   ↓
Allow / Deny
```

DAC ergänzt das capability-basierte Sicherheitsmodell, ersetzt dieses jedoch nicht.

## Grundprinzip

```text
Ownership ≠ Unlimited Authority
DAC Permission ≠ Capability
Identity ≠ Authority
Visible ≠ Accessible
DAC Allow ≠ Final Authorization
```

Eine positive DAC-Entscheidung darf keine fehlende Capability oder höherrangige Sicherheitsregel umgehen.

## Verhältnis zu Capabilities

NovaOS verwendet Capabilities als grundlegenden Autorisierungsmechanismus.

DAC bildet darüber eine Policy-Schicht:

```text
Subject
   ↓
DAC Policy
   ↓
Capability / Authorized Handle
   ↓
Resource
```

Vereinfacht:

```text
DAC → Wer darf grundsätzlich zugreifen?
Capability → Welche konkrete Autorität besitzt der Aufrufer?
```

Beide Mechanismen müssen gemeinsam nutzbar sein.

## DAC-Modell

Eine DAC-Regel besitzt mindestens:

```text
DACEntry
├── ResourceID
├── SubjectID
├── Rights
├── Effect
└── State
```

Optional:

```text
OwnerID
GroupID
Inheritance
Constraints
Expiration
Audit Policy
```

`ResourceID` und `SubjectID` müssen stabile logische Identitäten verwenden.

## Subjects

DAC-Regeln dürfen sich beziehen auf:

```text
User
Group
Service
Application
Process Context
Security Principal
```

Die konkrete Prozessidentität darf von der dauerhaften Benutzer- oder Dienstidentität getrennt bleiben.

## Rechte

DAC muss ressourcenspezifische Rechte unterstützen können.

Allgemeine Rechte:

```text
Read
Write
Execute
Create
Delete
ModifyPermissions
Share
```

Storage kann beispielsweise zusätzliche Rechte besitzen:

```text
MetadataRead
MetadataWrite
Enumerate
Rename
```

## Eigentümer

Ressourcen dürfen einen logischen Eigentümer besitzen.

```text
Resource
├── OwnerID
└── DAC Policy
```

Der Eigentümer darf innerhalb der geltenden Systempolicy Zugriffsrechte vergeben oder entziehen.

Eigentum darf jedoch keine übergeordneten Sicherheitsregeln außer Kraft setzen.

```text
Owner ≠ Root
```

## Gruppen

Mehrere Subjects dürfen über Gruppen zusammengefasst werden.

```text
Group
├── User A
├── User B
└── Service C
```

DAC-Regeln dürfen Gruppen direkt referenzieren.

Gruppenmitgliedschaft muss über die Security-Infrastruktur kontrolliert werden.

## Allow und Deny

Regeln dürfen mindestens unterstützen:

```text
Allow
Deny
```

Konflikte müssen deterministisch aufgelöst werden.

Explizite Sicherheitsverbote dürfen nicht durch allgemeinere Allow-Regeln aufgehoben werden.

## Vererbung

Hierarchische Ressourcen dürfen DAC-Regeln vererben.

Beispiel:

```text
Folder
├── DAC Policy
│
├── File A
├── File B
└── Folder C
```

Vererbung muss explizit kontrollierbar sein.

Eine Ressource darf:

```text
Inherit
Override
Block Inheritance
```

unterstützen.

## Dateien und Storage

DAC soll insbesondere für benutzerorientierte Storage-Ressourcen verwendet werden können.

```text
Storage Object
├── Owner
├── Group
└── Access Policy
```

Die Sicherheitsentscheidung darf jedoch nicht allein vom Dateipfad abhängen.

```text
Path ≠ Identity
```

Die stabile `ObjectID` bleibt die relevante Ressourcenidentität.

## Anwendungen

Benutzer dürfen Anwendungen gezielt Zugriff auf eigene Ressourcen gewähren.

Beispiel:

```text
User
  ↓
Grant Read
  ↓
Application
  ↓
Document
```

Die resultierende Autorität soll nach Möglichkeit als eingeschränkte Capability an die Anwendung übertragen werden.

## Sharing

DAC darf benutzerfreundliche Freigaben ermöglichen.

```text
Owner
  ↓
Share
  ↓
User / Group
  ↓
Selected Rights
```

Freigaben dürfen zeitlich oder funktional begrenzt werden.

Eine Freigabe darf keine stärkeren Rechte übertragen als der Freigebende delegieren darf.

## Sicherheitsentscheidung

Eine Zugriffsentscheidung darf mehrere Ebenen kombinieren.

```text
Capability
   +
DAC
   +
Security Policy
   +
Resource Policy
   ↓
Authorization Decision
```

Dabei gilt grundsätzlich:

```text
Safety
  ↓
Security
  ↓
Mandatory Restrictions
  ↓
Capability Authority
  ↓
DAC Policy
```

DAC darf höherrangige Einschränkungen nicht überschreiben.

## Änderung von Berechtigungen

Nur entsprechend autorisierte Subjects dürfen DAC-Regeln verändern.

Beispielsweise:

```text
Owner
Authorized Administrator
Delegated Permission Manager
```

Das Recht `ModifyPermissions` muss selbst autorisiert sein.

## Default Policy

Neu erzeugte Ressourcen müssen eine definierte Ausgangspolicy erhalten.

Diese darf aus folgenden Quellen entstehen:

```text
Creator Context
Parent Policy
Application Policy
System Defaults
Explicit User Decision
```

Unsichere globale Freigaben dürfen nicht implizit erzeugt werden.

## Revocation

DAC-Rechte müssen entzogen werden können.

```text
Allowed
   ↓
Permission Removed
   ↓
Denied
```

Bereits delegierte Capabilities müssen entsprechend ihrer eigenen Revocation-Semantik behandelt werden.

Eine DAC-Änderung bedeutet daher nicht automatisch, dass jede zuvor ausgegebene Capability sofort ungültig wird.

## Caching

DAC-Entscheidungen dürfen gecacht werden.

```text
Subject + Resource + Operation
             ↓
        Cached Decision
```

Änderungen an:

```text
Permissions
Ownership
Group Membership
Security Policy
Resource State
```

müssen relevante Cache-Einträge invalidieren können.

## Sicherheit

DAC-Metadaten dürfen nicht durch unautorisierte Prozesse verändert werden.

Manipulation von:

```text
OwnerID
GroupID
Rights
Inheritance
Policy Metadata
```

muss verhindert werden.

Anwendungen dürfen ihre eigenen Berechtigungen nicht eigenständig erweitern.

## Ressourcensteuerung

NovaOS muss Grenzen unterstützen für:

```text
DAC Entries
Groups
Group Memberships
Inheritance Depth
Policy Cache
Policy Evaluation Cost
```

Komplexe Berechtigungsstrukturen dürfen keinen unbegrenzten Ressourcenverbrauch verursachen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
ResourceID
OwnerID
SubjectID
GroupID
Rights
Effect
Inheritance
Decision
Decision Reason
```

Ein Benutzer oder Administrator soll nachvollziehen können, warum ein Zugriff erlaubt oder verweigert wurde.

## Normative Anforderungen

1. NovaOS MUSS DAC als ergänzende Autorisierungs-Policy unterstützen können.
2. DAC DARF das Capability-Modell NICHT ersetzen.
3. Eine DAC-Allow-Entscheidung DARF fehlende notwendige Autorität NICHT erzeugen.
4. Ressourcen und Subjects MÜSSEN über stabile Identitäten referenzierbar sein.
5. Eigentümer MÜSSEN Zugriffsrechte innerhalb ihrer delegierten Autorität verwalten können.
6. Gruppenbasierte Berechtigungen SOLLEN unterstützt werden.
7. Allow- und Deny-Regeln MÜSSEN deterministisch ausgewertet werden.
8. Vererbung MUSS explizit kontrollierbar sein.
9. Anwendungen DÜRFEN ihre eigenen Rechte NICHT selbstständig erweitern.
10. Berechtigungsänderungen MÜSSEN Cache- und Autorisierungszustände aktualisieren können.
11. DAC-Strukturen MÜSSEN ressourcenbegrenzt sein.
12. Zugriffsentscheidungen und ihre Ursachen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-DISTCOMM-REMOTECAPABILITY-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-NAMESPACE-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-SECURITY-0002`

## Ergebnis

```text
Identity / Ownership
        ↓
DAC Policy
        ↓
Capability Authority
        ↓
Security Constraints
        ↓
Authorization Decision
        ↓
Resource Access
```

NovaOS erhält damit eine benutzerfreundliche DAC-Schicht für Eigentum, Gruppen, Freigaben und klassische Zugriffsrechte, während Capabilities weiterhin die konkrete Autorität repräsentieren und DAC keine globale oder implizite Macht über das System erzeugt.