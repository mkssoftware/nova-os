# NPSPEC-FILESYSTEM-PERMISSION-0001 – Nova Filesystem Permissions

## Status

Angenommen

## Kategorie

Filesystem / Security / Permissions

## Zweck

NovaOS definiert ein capability-basiertes Berechtigungsmodell für Filesystem-Ressourcen.

Berechtigungen werden nicht aus Pfaden, Dateiendungen oder bloßer Sichtbarkeit abgeleitet, sondern beziehen sich auf stabile Ressourcenidentitäten und explizit autorisierte Operationen.

```text
Principal
   ↓
Capability
   ↓
ObjectID
   ↓
Allowed Operation
```

## Grundprinzipien

```text
Visibility ≠ Authority
Path Knowledge ≠ Permission
ObjectID Knowledge ≠ Permission
Ownership ≠ Unlimited Authority
Permission ≠ Capability Token
Namespace Access ≠ Object Access
Read ≠ Write
Write ≠ Delete
Delete ≠ Administrative Control
```

## Permission Model

```text
FilesystemPermission
├── SubjectID
├── TargetID
├── Rights
├── Scope
└── State
```

Optional:

```text
CapabilityID
DelegationSource
Expiration
Conditions
SecurityLabel
TrustRequirement
ProvenanceID
```

`TargetID` soll bevorzugt eine stabile `ObjectID`, `VolumeID` oder andere stabile Ressourcenidentität sein.

## Rechte

Filesystem-Rechte müssen granular darstellbar sein.

Beispiele:

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
Execute
Project
Relate
ModifyMetadata
ModifyPermissions
Delegate
```

NovaOS darf weitere semantisch spezialisierte Rechte definieren.

## Keine pfadbasierte Authority

Ein Pfad ist keine Sicherheitsidentität.

```text
/Benutzer/Dokumente/datei.nf
        ↓
Resolve
        ↓
ObjectID
        ↓
Permission Check
```

Rename oder Move dürfen eine bestehende Berechtigungsentscheidung nicht allein aufgrund des neuen Pfades verändern.

Policies dürfen den Namespace-Kontext berücksichtigen, die eigentliche Zielidentität bleibt jedoch stabil.

## Capability Integration

Filesystem-Zugriffe sollen über autorisierte Handles erfolgen.

```text
Request
   ↓
ObjectID
   ↓
Capability / Policy Check
   ↓
Authorized Handle
   ↓
Operation
```

Es gilt:

```text
ObjectID ≠ Capability
CapabilityID ≠ Handle
Handle ≠ Object
```

Ein Handle enthält beziehungsweise repräsentiert nur die für den jeweiligen Zugriff gewährte Authority.

## Least Privilege

NovaOS soll immer die kleinste notwendige Authority vergeben.

Benötigt ein Prozess ausschließlich:

```text
Read
```

darf daraus nicht automatisch entstehen:

```text
Write
Delete
ModifyPermissions
```

## Delegation

Filesystem-Authority darf kontrolliert delegiert werden.

```text
Authority A
    ↓
Attenuation
    ↓
Authority B
```

Delegierte Rechte dürfen nicht stärker sein als die Ausgangsrechte.

```text
DelegatedAuthority
⊆
OriginalAuthority
```

## Revocation

Berechtigungen müssen widerrufbar sein.

```text
Valid
 ↓
Revoked
```

Weitere Zustände:

```text
Expired
Unknown
```

`Unknown` darf nicht als `Valid` interpretiert werden.

Bei sicherheitskritischen Operationen muss die aktuelle Authority revalidierbar sein.

## Namespace und Projection

Namespace- oder Projection-Sichtbarkeit erzeugt keine Berechtigung.

```text
Projection
   ↓
Visible Object
   ↓
Capability Check
```

Ein Objekt kann sichtbar, aber nicht lesbar sein.

Ebenso darf ein Objekt über mehrere Projections erscheinen, ohne unterschiedliche grundlegende Objektberechtigungen zu erhalten.

## Private SYS Overlay

Ein Programm darf auf sein privates `SYS`-Overlay entsprechend seiner Programmberechtigungen zugreifen.

```text
/Apps/Example/SYS
```

Diese Authority gilt nicht automatisch für:

```text
/System
```

Es gilt:

```text
Private SYS Write
≠
Global System Write
```

Eine Änderung des globalen Systembereichs benötigt eine gesonderte Capability.

## Metadaten

Metadaten besitzen eigene Zugriffsrechte.

```text
ReadMetadata
ModifyMetadata
```

`ReadMetadata` darf nicht automatisch `Read` auf den Payload bedeuten.

Umgekehrt muss Payload-Zugriff nicht zwingend Zugriff auf alle geschützten Metadaten gewähren.

## Relations

Das Erstellen oder Ändern einer Relation benötigt entsprechende Authority.

```text
Source Object
      ↓
Relate Permission
      ↓
Relation
      ↓
Target ObjectID
```

Die Kenntnis einer Relation gewährt keinen Zugriff auf das Target.

```text
Relation Visible
≠
Target Authorized
```

## Projections

Das Recht, ein Objekt zu verwenden, bedeutet nicht automatisch, dass der Prozess neue globale Projections dafür erzeugen darf.

```text
Read
≠
Project
```

Globale Namespace-Veränderungen können stärkere Berechtigungen erfordern als private oder processlokale Projections.

## Verzeichnisse

Rechte auf Container und enthaltene Objekte bleiben unterscheidbar.

```text
Directory Permission
≠
Automatic Unlimited Child Permission
```

Vererbungsregeln dürfen existieren, müssen jedoch explizit und nachvollziehbar sein.

Implizite unbegrenzte Vererbung ist nicht vorauszusetzen.

## Transaktionen

Filesystem-Transaktionen verleihen keine zusätzliche Authority.

Alle enthaltenen Operationen müssen autorisiert sein.

```text
Begin Transaction
      ↓
Permission Validation
      ↓
Stage
      ↓
Revalidation
      ↓
Commit
```

Eine bei Beginn gültige Berechtigung ist nicht automatisch bis zum Commit gültig.

## TOCTOU-Schutz

Zwischen Auflösung und Operation soll NovaOS stabile autorisierte Handles verwenden.

```text
Path
 ↓
Resolve ObjectID
 ↓
Authorize
 ↓
Handle
 ↓
Operate on Handle
```

Dadurch soll verhindert werden, dass ein Pfad zwischen Prüfung und Zugriff auf ein anderes Objekt umgebogen wird.

## Security Labels

Filesystem-Objekte dürfen zusätzliche Security Labels besitzen.

```text
ObjectID
├── Permission State
└── Security Labels
```

Capability-, DAC-, MAC-, RBAC- oder ABAC-Policies können gemeinsam zur Entscheidung beitragen.

Eine Capability darf dabei nur innerhalb der geltenden übergeordneten Sicherheitsregeln wirksam sein.

## Systembereiche

Kritische Bereiche wie:

```text
/System
/Boot
Recovery Resources
Security Configuration
```

dürfen strengere Policies besitzen.

Normale Benutzer- oder Programmberechtigungen dürfen daraus keine implizite System-Authority ableiten.

## Introspection

Autorisierte Komponenten sollen feststellen können:

```text
TargetID
Effective Rights
Capability Source
Delegation Chain
Expiration
Revocation State
Security Constraints
```

Dabei dürfen Capability Tokens oder andere übertragbare Credentials nicht ungeschützt offengelegt werden.

## Provenance

Sicherheitsrelevante Änderungen sollen nachvollziehbar sein.

Beispiele:

```text
Permission Granted
Permission Changed
Permission Delegated
Permission Revoked
Global System Modification
```

Die Aufzeichnung selbst darf keine zusätzliche Authority erzeugen.

## Normative Anforderungen

1. NovaOS MUSS Filesystem-Berechtigungen auf stabile Ressourcenidentitäten beziehen können.
2. Pfade DÜRFEN NICHT als alleinige Sicherheitsidentität verwendet werden.
3. Sichtbarkeit DARF NICHT als Zugriffsberechtigung interpretiert werden.
4. Kenntnis einer `ObjectID` DARF KEINE Authority verleihen.
5. Filesystem-Rechte MÜSSEN granular darstellbar sein.
6. Read, Write, Delete, Execute und Permission Management MÜSSEN unterscheidbar sein.
7. Metadatenrechte MÜSSEN von Payload-Rechten unterscheidbar sein.
8. NovaOS SOLL Filesystem-Zugriffe über autorisierte Handles durchführen.
9. Least Privilege MUSS unterstützt werden.
10. Delegierte Authority DARF die ursprüngliche Authority NICHT überschreiten.
11. Filesystem-Authority MUSS widerrufbar sein können.
12. `Unknown` DARF NICHT als gültige Authority interpretiert werden.
13. Namespace-Projections DÜRFEN KEINE zusätzliche Authority erzeugen.
14. Mehrere Projections desselben Objekts DÜRFEN die Objektidentität NICHT umgehen.
15. Private `SYS`-Berechtigungen DÜRFEN NICHT automatisch für das globale `/System` gelten.
16. Globale Systemänderungen MÜSSEN gesondert autorisiert werden.
17. Relation-Sichtbarkeit DARF KEINE Authority über das Relation Target verleihen.
18. `Read` DARF NICHT automatisch `Project` bedeuten.
19. Globale und private Projection-Rechte MÜSSEN unterscheidbar sein.
20. Verzeichnisrechte und Rechte auf enthaltene Objekte MÜSSEN unterscheidbar bleiben.
21. Berechtigungsvererbung MUSS explizit definiert sein.
22. Filesystem-Transaktionen DÜRFEN KEINE zusätzliche Authority erzeugen.
23. Sicherheitskritische Authority MUSS vor Commit revalidierbar sein.
24. Autorisierte Handles SOLLEN TOCTOU-Risiken bei Pfadauflösung reduzieren.
25. Capability-, DAC-, MAC-, RBAC- und ABAC-Policies MÜSSEN kombinierbar sein können.
26. Kritische Systembereiche MÜSSEN strengere Policies verwenden können.
27. Sicherheitsrelevante Permission-Änderungen SOLLEN nachvollziehbare Provenance besitzen.
28. Effective Permissions MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-FILESYSTEM-SEMANTIC-0001`
- `NPSPEC-FILESYSTEM-RELATION-0001`
- `NPSPEC-FILESYSTEM-TRANSACTION-0001`
- `NPSPEC-OBJECT-PERMISSION-0001`
- `NPSPEC-CAPABILITY-TOKEN-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-CAPABILITY-DELEGATION-0001`
- `NPSPEC-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-DAC-0001`
- `NPSPEC-SECURITY-MAC-0001`
- `NPSPEC-SECURITY-RBAC-0001`
- `NPSPEC-SECURITY-ABAC-0001`
- `NPSPEC-SECURITY-LEASTPRIVILEGE-0001`

## Ergebnis

```text
Namespace / Projection
         ↓
      ObjectID
         ↓
 Requested Operation
         ↓
Capability + Security Policy
         ↓
    Authorization
      ↙       ↘
   Deny       Allow
                ↓
        Authorized Handle
                ↓
         Filesystem Operation
```

NovaOS erhält damit ein capability-basiertes Filesystem-Berechtigungsmodell, bei dem Objektidentität, Namespace-Sichtbarkeit und Authority konsequent getrennt bleiben. Programme erhalten ausschließlich die für ihre Aufgabe benötigten Rechte, während Rename, Move, Projections und private `SYS`-Overlays die Sicherheitsgrenzen nicht umgehen können.