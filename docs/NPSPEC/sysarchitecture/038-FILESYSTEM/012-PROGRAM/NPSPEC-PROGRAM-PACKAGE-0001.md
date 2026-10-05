# NPSPEC-PROGRAM-PACKAGE-0001 – Nova Program Package

## Status

Angenommen

## Kategorie

Program / Package

## Zweck

NovaOS definiert ein Program Package als installierbare Einheit eines klassischen monolithischen Programms einschließlich seiner privaten Ressourcen und Abhängigkeiten.

Programme bleiben von NovaOS-Solutions getrennt.

## Grundprinzipien

```text
Program ≠ Solution
Package ≠ Running Process
Package Identity ≠ Installation Path
Private Dependency ≠ Global System Dependency
Installation ≠ Global System Modification
```

Ein Program Package soll möglichst vollständig in seinem eigenen Programmbereich enthalten sein.

## Paketstruktur

Eine typische Installation kann logisch wie folgt aufgebaut sein:

```text
/Apps/<Program>/
├── App/
├── Resources/
├── SYS/
└── Manifest
```

Dabei enthält:

```text
App        → Programmcode
Resources  → programmspezifische Ressourcen
SYS        → private Systemabhängigkeiten
Manifest   → Identität und Paketbeschreibung
```

Die konkrete physische Struktur darf sich ändern, solange das logische Modell erhalten bleibt.

## Paketidentität

Jedes installierbare Programm benötigt eine stabile `ProgramID`.

```text
ProgramID ≠ Name
ProgramID ≠ Path
ProgramID ≠ ProcessID
```

Updates desselben Programms behalten grundsätzlich dieselbe `ProgramID`.

Ein anderes Programm darf nicht allein durch gleichen Namen oder Installationspfad dieselbe Identität erhalten.

## Private Abhängigkeiten

Programme dürfen private Systemabhängigkeiten mitbringen:

```text
/Apps/<Program>/SYS/
```

Diese können im Programmkontext logisch in den System-Namespace eingeblendet werden:

```text
Program SYS
    ↓
SYS Overlay
    ↓
Effective /System
```

Physisch verbleiben diese Komponenten im Program Package.

Eine private `SYS`-Komponente verändert nicht das globale `/System`.

## Globales System

Benötigt ein Programm tatsächlich eine Änderung am globalen System, muss diese separat autorisiert werden.

```text
Private SYS ≠ Global /System
```

Dadurch sollen Programme ihre Abhängigkeiten bevorzugt selbst enthalten, ohne globale Bibliotheken oder Systemkomponenten anderer Programme zu verändern.

## Installation

```text
Package
  ↓
Validate Identity
  ↓
Verify Integrity / Trust
  ↓
Resolve Requirements
  ↓
Authorize
  ↓
Install
  ↓
Register Program
```

Installation und Update sollen transaktional erfolgen können.

## Sicherheit

Das Vorhandensein eines Programms unter `/Apps` erzeugt keine zusätzliche Authority.

Programmberechtigungen werden separat über das Capability- und Userspace-Permission-Modell verwaltet.

Code, Ressourcen und private Abhängigkeiten müssen der Paketidentität eindeutig zuordenbar sein.

## Normative Anforderungen

1. NovaOS MUSS klassische Programme als eigenständige Program Packages verwalten können.
2. Programme und Solutions MÜSSEN getrennte Konzepte bleiben.
3. Jedes Program Package MUSS eine stabile Programmidentität besitzen.
4. Die Programmidentität DARF NICHT vom Installationspfad abhängen.
5. Programme DÜRFEN private Abhängigkeiten in einem eigenen `SYS`-Bereich bereitstellen.
6. Private `SYS`-Abhängigkeiten DÜRFEN das globale `/System` nicht verändern.
7. Änderungen am globalen `/System` MÜSSEN separat autorisiert werden.
8. Program Packages MÜSSEN vor Installation auf Identität und Integrität prüfbar sein.
9. Installation und Update SOLLEN transaktional erfolgen.
10. Die Installation eines Programms DARF keine implizite Laufzeit-Authority erzeugen.

## Abhängigkeiten

- `NPSPEC-USERSPACE-LAYOUT-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`
- `NPSPEC-FILESYSTEM-TRANSACTION-0001`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`

## Ergebnis

NovaOS erhält ein klar abgegrenztes Paketmodell für klassische Programme. Programme können Code, Ressourcen und private Systemabhängigkeiten selbst enthalten, ohne das globale System unnötig zu verändern oder mit dem Solution-Modell vermischt zu werden.