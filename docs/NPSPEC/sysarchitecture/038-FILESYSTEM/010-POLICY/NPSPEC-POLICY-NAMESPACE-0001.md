# NPSPEC-POLICY-NAMESPACE-0001 – Nova Namespace Policy

## Status

Angenommen

## Kategorie

Policy / Namespace

## Zweck

NovaOS definiert die Policy für Sichtbarkeit, Projektion und Veränderung von Namespace-Einträgen.

Unterschiedliche Benutzer, Programme, Solutions, Prozesse, Workspaces und Recovery-Kontexte dürfen unterschiedliche logische Sichten auf dieselben zugrunde liegenden Objekte besitzen.

## Grundprinzipien

```text
Namespace Visibility ≠ Authority
Path ≠ Object Identity
Projection ≠ Copy
Mount ≠ Permission
Visible Object ≠ Accessible Object
Hidden Object ≠ Protected Object
Namespace Change ≠ Object Change
```

## Namespace-Kontexte

Policy kann getrennte Namespace-Sichten definieren für:

```text
Global
User
Process
Program
Solution
Workspace
Recovery
```

Die effektive Sicht entsteht aus dem jeweiligen Kontext und den zulässigen Projektionen.

## Entscheidungsmodell

```text
Namespace Request
       ↓
Security Context
       ↓
Namespace Scope
       ↓
Namespace Policy
       ↓
Projection / Visibility Rules
       ↓
Effective Namespace
```

Die resultierende Sicht erzeugt keine zusätzliche Authority auf die sichtbaren Objekte.

## Sichtbarkeit

Policy kann steuern, ob Namespace-Einträge:

```text
Visible
Hidden
Projected
Overlaid
ReadOnly
Unavailable
```

erscheinen.

Die tatsächlichen Zugriffsrechte werden weiterhin separat über das Capability- und Permission-Modell bestimmt.

## Private Program-Namespace

Programme dürfen einen privaten Namespace besitzen.

Insbesondere kann:

```text
/Apps/<Program>/SYS/
```

im Programmkontext als Overlay auf:

```text
/System
```

projiziert werden.

```text
Effective /System
      =
Global /System
      +
Program SYS Overlay
```

Andere Programme und der globale Namespace sehen dieses private Overlay nicht.

## Solutions und Workspaces

Solutions und Workspaces dürfen kontextbezogene Projektionen erhalten.

```text
Global Namespace
       ↓
Context Policy
       ↓
Workspace / Solution View
```

Eine solche Projektion darf keine zusätzliche Objekt-Authority erzeugen.

## Namespace-Änderungen

Operationen wie:

```text
Mount
Unmount
Project
Overlay
Hide
Expose
Redirect
```

müssen policy-kontrolliert sein.

Globale Namespace-Änderungen dürfen strengere Authority verlangen als lokale Kontextänderungen.

## Systembereiche

Kritische Bereiche wie:

```text
/System
/Boot
```

dürfen durch Namespace-Projektionen nicht so überschrieben werden, dass System Protection oder System Write Policy umgangen wird.

Private Overlays verändern ausschließlich die effektive Sicht des jeweiligen Kontexts.

## Dynamische Änderungen

Namespace-Policy darf zur Laufzeit geändert werden.

Bestehende Handles bleiben dabei an ihre bereits autorisierte Objektidentität gebunden und dürfen nicht allein durch eine Namespace-Änderung auf ein anderes Objekt umgelenkt werden.

## Normative Anforderungen

1. NovaOS MUSS Namespace-Sichten policy-basiert steuern können.
2. Namespace-Sichtbarkeit DARF keine Objekt-Authority erzeugen.
3. Namespace-Kontexte MÜSSEN voneinander isolierbar sein.
4. Programme MÜSSEN private Namespace-Projektionen verwenden können.
5. Private `SYS`-Overlays DÜRFEN den globalen `/System`-Namespace nicht verändern.
6. Solutions und Workspaces DÜRFEN kontextbezogene Projektionen besitzen.
7. Globale Namespace-Änderungen MÜSSEN explizit autorisiert sein.
8. Mounts und Projektionen DÜRFEN bestehende Permission-Regeln nicht umgehen.
9. System Protection MUSS auch innerhalb projizierter Namespaces gelten.
10. Namespace-Änderungen DÜRFEN bestehende autorisierte Handles nicht auf andere Objekte umleiten.
11. Namespace-Policy MUSS dynamische und transaktionale Änderungen unterstützen können.
12. Effektiver Namespace, Projektionen und Policy-Entscheidungen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-POLICY-0001`
- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`
- `NPSPEC-PROGRAM-NAMESPACE-0001`
- `NPSPEC-PROGRAM-SYSOVERLAY-0001`
- `NPSPEC-STORAGE-MOUNT-0001`
- `NPSPEC-POLICY-SYSTEMWRITE-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`

## Ergebnis

NovaOS kann für jeden Ausführungskontext eine kontrollierte logische Sicht auf das System erzeugen. Namespace-Projektionen, Overlays und Mounts verändern dabei ausschließlich die sichtbare Struktur und erzeugen weder neue Objektidentitäten noch zusätzliche Authority.