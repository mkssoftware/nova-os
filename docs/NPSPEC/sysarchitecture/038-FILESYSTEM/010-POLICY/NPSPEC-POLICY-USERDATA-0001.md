# NPSPEC-POLICY-USERDATA-0001 – Nova User Data Policy

## Status

Angenommen

## Kategorie

Policy / User Data

## Zweck

NovaOS definiert die Policy für Zugriff, Veränderung, Weitergabe und Löschung von Benutzerdaten.

Benutzerdaten bleiben unter Kontrolle des Benutzers und dürfen nicht allein aufgrund von Sichtbarkeit, Dateipfad, Programmausführung oder Benutzerkontext zugänglich werden.

## Grundprinzipien

```text
Visibility ≠ Authority
User Context ≠ Full User Data Access
Path Knowledge ≠ Permission
Ownership ≠ Unrestricted Access
Trust ≠ Data Authority
Installation ≠ Data Access
Workspace Membership ≠ Data Access
```

## Geschützte Daten

Die Policy gilt insbesondere für:

```text
User Files
NovaFiles
Documents
Media
Settings
Credentials
Personal Metadata
Workspace Data
Application Data
Solution Data
Cloud / Remote Data
```

## Entscheidungsmodell

```text
Data Request
     ↓
Requester Identity
     ↓
Target ObjectID
     ↓
Requested Operation
     ↓
Security Context
     ↓
User Data Policy
     ↓
Capability Authority
     ↓
Allow / Restrict / Ask / Deny
```

## Operationen

Zugriffe müssen nach Operation unterscheidbar sein:

```text
Discover
ReadMetadata
Read
Create
Write
Append
Rename
Move
Delete
Relate
Share
Export
```

Leseberechtigung erzeugt beispielsweise keine Schreib-, Lösch- oder Weitergabeberechtigung.

## Programme und Solutions

Programme erhalten nicht automatisch Zugriff auf sämtliche Daten des ausführenden Benutzers.

Solutions erhalten ausschließlich Zugriff über explizit autorisierte Capabilities ihres Logic Graph.

```text
Program / Solution
       ↓
Authorized Capability
       ↓
Specific User Data
```

Authority soll möglichst auf konkrete Objekte, Datentypen, Bereiche oder Operationen beschränkt werden.

## Workspace

Die Aufnahme eines Objekts in einen Workspace erzeugt keine neue Authority.

```text
Workspace Membership ≠ Permission
```

Der effektive Zugriff ergibt sich aus vorhandener Authority und dem Workspace-Sicherheitskontext.

## Weitergabe

Die Weitergabe von Benutzerdaten an andere Benutzer, Programme, Solutions, Services oder entfernte Systeme benötigt eine dafür geeignete Authority.

```text
Read Authority ≠ Share Authority
```

## Recovery und Updates

Systemupdates und System-Recovery dürfen Benutzerdaten grundsätzlich nicht verändern oder zurücksetzen.

Eine Datenwiederherstellung muss als separater autorisierter Vorgang behandelt werden.

## Policy-Priorität

```text
Safety
  ↓
Security
  ↓
Sovereignty / Trust
  ↓
Hard System Constraints
  ↓
Explicit User Decisions
  ↓
Soft Preferences
  ↓
Adaptive Optimization
```

## Normative Anforderungen

1. NovaOS MUSS Benutzerdaten policy-basiert schützen.
2. Programme DÜRFEN nicht automatisch vollständigen Zugriff auf Benutzerdaten erhalten.
3. Solutions DÜRFEN Benutzerdaten nur über autorisierte Capabilities verwenden.
4. Sichtbarkeit oder Kenntnis einer `ObjectID` DARF keine Authority erzeugen.
5. Datenzugriffe MÜSSEN nach Operation einschränkbar sein.
6. Authority SOLL auf den minimal erforderlichen Datenbereich beschränkt werden.
7. Workspace-Mitgliedschaft DARF keine zusätzliche Daten-Authority erzeugen.
8. Lesezugriff DARF keine implizite Weitergabe-Authority erzeugen.
9. Remote-Zugriff und Export MÜSSEN separat kontrollierbar sein.
10. Systemupdates DÜRFEN Benutzerdaten nicht standardmäßig verändern.
11. System-Recovery DARF Benutzerdaten nicht standardmäßig zurücksetzen.
12. Datenzugriff, effektive Authority und Entscheidungsgrund MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-POLICY-0001`
- `NPSPEC-POLICY-CAPABILITY-0001`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`
- `NPSPEC-USERSPACE-DATA-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`
- `NPSPEC-WORKSPACE-PERMISSION-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-SYSTEM-RECOVERY-0001`

## Ergebnis

NovaOS behandelt Benutzerdaten als explizit geschützte Ressourcen. Programme, Solutions, Workspaces und Systemkomponenten erhalten nur die minimal erforderliche Authority für konkrete Daten und Operationen, während Sichtbarkeit, Benutzerkontext oder Trust niemals automatisch vollständigen Datenzugriff erzeugen.