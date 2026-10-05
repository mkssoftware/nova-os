# NPSPEC-NAMESPACE-PERMISSION-0001 – Nova Namespace Permission

## Status

Angenommen

## Kategorie

Namespace / Permission

## Zweck

NovaOS definiert die Berechtigungsregeln für Namespace-Operationen.

Namespace Permission kontrolliert, wer Namespace-Strukturen sehen, durchsuchen oder verändern darf, ohne Namespace-Berechtigungen mit der Authority auf die referenzierten Objekte gleichzusetzen.

## Grundprinzipien

```text
Namespace Permission ≠ Object Permission
Visibility ≠ Authority
Discover ≠ Read
Path Access ≠ Object Access
Mount Authority ≠ Object Authority
Projection Authority ≠ Target Authority
Namespace Ownership ≠ Unrestricted Authority
```

## Berechtigungsmodell

Namespace-Rechte können mindestens umfassen:

```text
Discover
Enumerate
Resolve
CreateEntry
RemoveEntry
RenameEntry
Project
Overlay
Mount
Unmount
ModifyMapping
```

Die Rechte können auf einen bestimmten:

```text
NamespaceID
Scope
Namespace Entry
Subtree
Projection
Overlay
Mount
```

begrenzt werden.

## Auflösung und Objektzugriff

Namespace-Zugriff und Objektzugriff erfolgen getrennt:

```text
Path
 ↓
Namespace Permission
 ↓
Resolution
 ↓
ObjectID
 ↓
Object Capability / Permission
 ↓
Authorized Handle
```

Die Berechtigung zur Auflösung eines Pfades erzeugt keine Berechtigung auf das Zielobjekt.

## Sichtbarkeit

Ein Kontext kann einen Namespace-Eintrag sehen, ohne auf dessen Inhalt zugreifen zu dürfen.

```text
Visible
  ≠
Readable
  ≠
Writable
  ≠
Executable
```

Ebenso kann ein autorisiertes Handle auf ein Objekt bestehen, obwohl das Objekt im aktuellen Namespace nicht sichtbar ist.

## Kontext

Namespace-Berechtigungen können für folgende Scopes gelten:

```text
System
User
Process
Program
Solution
Workspace
Recovery
```

Rechte eines übergeordneten Kontextes werden nicht automatisch vollständig an untergeordnete Kontexte weitergegeben.

## Projektionen und Overlays

Das Recht, eine Projektion oder ein Overlay anzulegen, gewährt keine zusätzliche Authority auf dessen Zielobjekte.

```text
Projection Authority
        +
Target Authority
        ↓
Usable Projection
```

Private Overlays dürfen keine globalen Namespace-Rechte erzeugen.

## Globale Änderungen

Änderungen am globalen System Namespace benötigen gesonderte Authority.

Dies betrifft insbesondere:

```text
Global Mount
Global Projection
/System Mapping
/Boot Mapping
Global Namespace Structure
```

Für geschützte Systembereiche gelten zusätzlich System Protection und System Write Policy.

## Delegation

Namespace-Rechte dürfen delegiert werden, sofern die ursprüngliche Authority dies erlaubt.

Delegation darf Rechte nur erhalten oder reduzieren:

```text
Original Authority
       ↓
Attenuation
       ↓
Delegated Authority
```

## Widerruf

Namespace-Authority muss widerrufbar sein.

Ein Widerruf verhindert zukünftige Namespace-Operationen, darf jedoch bereits autorisierte Objekt-Handles nicht stillschweigend auf andere Objekte umleiten.

## Normative Anforderungen

1. NovaOS MUSS Namespace- und Objektberechtigungen getrennt behandeln.
2. Namespace-Sichtbarkeit DARF keine Objekt-Authority erzeugen.
3. Discover-, Resolve- und Änderungsrechte MÜSSEN unterscheidbar sein.
4. Namespace-Rechte MÜSSEN auf konkrete Scopes und Ressourcen begrenzbar sein.
5. Projektionen und Overlays DÜRFEN keine Target-Authority erzeugen.
6. Mount Authority DARF keine automatische Authority auf gemountete Objekte erzeugen.
7. Globale Namespace-Änderungen MÜSSEN explizit autorisiert sein.
8. Geschützte Systembereiche MÜSSEN zusätzliche Schutzregeln berücksichtigen.
9. Untergeordnete Kontexte DÜRFEN Authority nicht automatisch erweitern.
10. Delegierte Namespace-Authority DARF die ursprüngliche Authority nicht überschreiten.
11. Namespace-Authority MUSS widerrufbar sein.
12. Namespace-Berechtigungen und Entscheidungsgründe MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FILESYSTEM-PERMISSION-0001`
- `NPSPEC-NAMESPACE-RESOLUTION-0001`
- `NPSPEC-NAMESPACE-SYSTEM-0001`
- `NPSPEC-NAMESPACE-OVERLAY-0002`
- `NPSPEC-NAMESPACE-VIRTUAL-0001`
- `NPSPEC-POLICY-NAMESPACE-0001`
- `NPSPEC-POLICY-CAPABILITY-0001`
- `NPSPEC-POLICY-SYSTEMWRITE-0001`
- `NPSPEC-SYSTEM-PROTECTION-0001`

## Ergebnis

NovaOS trennt die Authority zur Nutzung und Veränderung eines Namespace strikt von der Authority auf die darin referenzierten Objekte. Sichtbarkeit, Auflösung, Projektionen, Overlays und Mounts können dadurch unabhängig kontrolliert werden, ohne bestehende Capability- oder Objektberechtigungen zu umgehen.