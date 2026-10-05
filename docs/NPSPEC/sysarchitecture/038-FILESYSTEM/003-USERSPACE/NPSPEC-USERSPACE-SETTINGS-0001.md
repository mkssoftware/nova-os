# NPSPEC-USERSPACE-SETTINGS-0001 – Nova Userspace Settings

## Status

Angenommen

## Kategorie

Userspace / Settings

## Zweck

NovaOS definiert ein einheitliches Modell für Benutzer-, Programm- und Solution-Einstellungen.

Einstellungen werden strukturiert gespeichert und nicht als beliebige, über das System verteilte Konfigurationsdateien behandelt.

## Grundprinzipien

```text
Settings ≠ Program Files
Settings ≠ System State
User Settings ≠ Global Settings
Setting Identity ≠ Storage Path
```

Einstellungen gehören eindeutig zu einem Scope und einer verantwortlichen Komponente.

## Scopes

Mindestens folgende Bereiche werden unterschieden:

```text
System
User
Program
Solution
Workspace
```

Beispiele:

```text
User
└── Darstellung

Program
└── Editor Preferences

Solution
└── Solution Configuration
```

## Settings-Modell

```text
Setting
├── SettingID
├── OwnerID
├── Scope
├── Key
├── Type
└── Value
```

Optional:

```text
DefaultValue
Constraints
Version
Policy
```

Einstellungen sollen typisiert sein.

Beispiele:

```text
Boolean
Integer
String
Enum
List
Object
```

## Speicherung

Der physische Speicherort einer Einstellung ist Implementierungsdetail.

Programme sollen Einstellungen über die NovaOS-Settings-Schnittstelle verwenden und keine festen Konfigurationspfade voraussetzen.

```text
Application
    ↓
Settings API
    ↓
Settings Store
```

## Vererbung

Einstellungen dürfen hierarchisch aufgelöst werden.

Beispiel:

```text
System Default
      ↓
User Setting
      ↓
Program Setting
      ↓
Workspace Setting
```

Eine spezifischere Einstellung darf eine allgemeinere Einstellung überschreiben, sofern die jeweilige Policy dies erlaubt.

## Transaktionen

Zusammengehörige Änderungen sollen transaktional gespeichert werden können.

```text
Change Settings
      ↓
Validate
      ↓
Commit
```

Dadurch dürfen keine teilweise übernommenen Konfigurationen entstehen.

## Sicherheit

Nicht jede Einstellung darf von jedem Prozess verändert werden.

System- oder sicherheitsrelevante Einstellungen benötigen entsprechende Capabilities.

```text
Settings Request
      ↓
Permission Check
      ↓
Read / Modify
```

Benutzereinstellungen dürfen keine System-Authority erzeugen.

## Synchronisation

Benutzereinstellungen dürfen zwischen NovaOS-Systemen synchronisiert werden.

Geräteabhängige oder sicherheitskritische Einstellungen können davon ausgeschlossen werden.

Die lokale Funktion des Systems darf nicht zwingend von einer Cloud- oder Netzwerkverbindung abhängen.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
SettingID
Owner
Scope
Type
Effective Value
Source
```

## Normative Anforderungen

1. NovaOS MUSS ein einheitliches Settings-Modell bereitstellen.
2. Einstellungen MÜSSEN einem eindeutigen Scope zugeordnet sein.
3. Einstellungen SOLLEN typisiert sein.
4. Programme SOLLEN keine festen physischen Settings-Pfade voraussetzen.
5. System-, User-, Program-, Solution- und Workspace-Einstellungen MÜSSEN unterscheidbar sein.
6. Settings-Vererbung MUSS deterministisch sein.
7. Zusammengehörige Änderungen SOLLEN transaktional gespeichert werden.
8. Sicherheitsrelevante Einstellungen MÜSSEN capability-basiert geschützt werden.
9. Benutzereinstellungen DÜRFEN keine System-Authority erzeugen.
10. Synchronisation DARF für die lokale Nutzung nicht zwingend erforderlich sein.

## Abhängigkeiten

- `NPSPEC-USERSPACE-LAYOUT-0001`
- `NPSPEC-FILESYSTEM-TRANSACTION-0001`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`
- `NPSPEC-STORAGE-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-CAPABILITY-APPLICATION-0001`

## Ergebnis

NovaOS erhält ein einheitliches, typisiertes und scope-basiertes Settings-System. Programme, Solutions und Benutzer können Einstellungen unabhängig von festen Speicherpfaden verwalten, während Sicherheit, Vererbung und transaktionale Änderungen zentral geregelt bleiben.