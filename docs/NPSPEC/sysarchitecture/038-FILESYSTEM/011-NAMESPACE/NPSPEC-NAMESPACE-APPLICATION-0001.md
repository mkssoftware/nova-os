# NPSPEC-NAMESPACE-APPLICATION-0001 – Nova Application Namespace

## Status

Angenommen

## Kategorie

Namespace / Application

## Zweck

NovaOS definiert den logischen Namespace klassischer monolithischer Programme.

Der Application Namespace stellt einem Programm eine konsistente Sicht auf globale Systemressourcen, eigene Dateien, private Abhängigkeiten und autorisierte Ressourcen bereit, ohne den globalen Namespace zu verändern.

## Grundprinzipien

```text
Application Namespace ≠ Global Namespace
Application Namespace ≠ Process Namespace
Application Visibility ≠ Authority
Private SYS ≠ Global /System
Path ≠ ObjectID
Overlay ≠ Physical Modification
```

## Modell

```text
ApplicationNamespace
├── NamespaceID
├── ProgramID
├── InstallScope
├── BaseNamespace
├── PrivateSYS
├── Projections
├── Policy
└── Version
```

Der Application Namespace ist an die stabile `ProgramID` gebunden, nicht an eine einzelne Prozessinstanz.

## Effektive Sicht

Die Programmsicht kann gebildet werden aus:

```text
Global Namespace
      +
User Context
      +
Application Namespace
      +
Private SYS Overlay
      ↓
Effective Application Namespace
```

Prozesse des Programms können diese Sicht als Grundlage ihres Process Namespace verwenden.

## Programmbereich

Das installierte Programm besitzt seinen eigenen Bereich:

```text
/Apps/<Program>/
├── App/
├── Resources/
├── SYS/
└── Manifest
```

Dieser physische Bereich bleibt von seiner effektiven logischen Sicht getrennt.

## Private SYS-Abhängigkeiten

Private Systemabhängigkeiten liegen unter:

```text
/Apps/<Program>/SYS/
```

und können im Application Namespace logisch auf `/System` projiziert werden.

```text
Global /System
      +
Private SYS
      ↓
Effective /System
```

Dadurch kann ein Programm private Libraries, Runtimes oder andere Abhängigkeiten verwenden, ohne globale Systemkomponenten zu ersetzen.

## Auflösung

```text
Application Path
      ↓
Application Namespace
      ↓
Overlay / Projection Resolution
      ↓
ObjectID
      ↓
Capability / Permission Check
      ↓
Authorized Handle
```

Namespace-Auflösung und Autorisierung bleiben getrennte Vorgänge.

## Install Scope

User- und Systeminstallation dürfen unterschiedliche physische oder logische Einbindungen besitzen.

```text
ProgramID
   +
InstallScope
   ↓
Application Namespace
```

Der Install Scope erzeugt keine zusätzliche Runtime-Authority.

## Prozessintegration

Beim Start eines Programms:

```text
Application Namespace
        ↓
Process Creation
        ↓
Process Namespace
        ↓
Optional Process-specific Restrictions
```

Ein Process Namespace darf die Application-Sicht weiter einschränken oder um autorisierte prozessspezifische Projektionen ergänzen.

## Dynamik

Application-Projektionen und private Overlays dürfen versioniert aktualisiert werden.

Bereits ausgegebene Handles bleiben an ihre aufgelöste `ObjectID` gebunden.

## Sicherheit

Der Application Namespace darf keine:

```text
Capability Authority
Filesystem Permission
System Write Authority
Trust
```

erzeugen.

Insbesondere gewährt ein privates `/System`-Overlay keine Berechtigung zur Änderung des globalen `/System`.

## Normative Anforderungen

1. NovaOS MUSS einen programmspezifischen Application Namespace unterstützen.
2. Der Application Namespace MUSS an die stabile `ProgramID` gebunden sein.
3. Application Namespace und Process Namespace MÜSSEN getrennte Konzepte bleiben.
4. Programme MÜSSEN private `SYS`-Abhängigkeiten verwenden können.
5. Private `SYS`-Inhalte MÜSSEN physisch im Programmbereich verbleiben können.
6. Private `SYS`-Overlays DÜRFEN den globalen `/System`-Zustand nicht verändern.
7. Namespace-Sichtbarkeit DARF keine Authority erzeugen.
8. Install Scope und Runtime Authority MÜSSEN getrennt bleiben.
9. Prozesse MÜSSEN den Application Namespace als Basis ihrer Namespace-Sicht verwenden können.
10. Process Namespaces DÜRFEN die Application-Sicht weiter einschränken.
11. Bestehende Handles DÜRFEN durch Namespace-Änderungen nicht auf andere Objekte umgebunden werden.
12. Application Namespace, Overlays und effektive Auflösung MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-NAMESPACE-OVERLAY-0002`
- `NPSPEC-NAMESPACE-PROCESS-0001`
- `NPSPEC-POLICY-NAMESPACE-0001`
- `NPSPEC-PROGRAM-NAMESPACE-0001`
- `NPSPEC-PROGRAM-SYS-0001`
- `NPSPEC-PROGRAM-SYSOVERLAY-0001`
- `NPSPEC-PROGRAM-INSTALLSCOPE-0001`

## Ergebnis

NovaOS stellt jedem klassischen Programm einen stabilen Application Namespace bereit. Private Abhängigkeiten können transparent in die Programmsicht eingebunden werden, während globaler Systemzustand, Objektidentität und Authority vollständig davon getrennt bleiben.