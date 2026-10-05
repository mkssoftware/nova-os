# NPSPEC-PROGRAM-INSTALL-0001 – Nova Program Installation

## Status

Angenommen

## Kategorie

Program / Installation

## Zweck

NovaOS definiert die sichere, transaktionale und reproduzierbare Installation klassischer Program Packages.

Eine Installation registriert ein Program Package innerhalb eines definierten Install Scope, ohne automatisch globale Systemänderungen oder Laufzeit-Authority zu erzeugen.

## Grundprinzipien

```text
Install ≠ Execute
Install ≠ Grant Runtime Authority
Install ≠ Modify Global /System
Package Validation ≠ Trust
Prepared ≠ Installed
```

## Installationsablauf

```text
Package
  ↓
Validate Manifest
  ↓
Verify Integrity / Trust
  ↓
Resolve Dependencies
  ↓
Evaluate Install Scope
  ↓
Check Install Authority
  ↓
Prepare
  ↓
Commit
  ↓
Register
  ↓
Verify
```

Ein Fehler vor dem Commit darf keine unvollständige Installation sichtbar hinterlassen.

## Validierung

Vor der Installation müssen mindestens geprüft werden:

```text
ProgramID
Manifest Version
Program Version
Package Structure
Integrity
Compatibility
Dependencies
Install Scope
```

Ungültige oder nicht erfüllbare zwingende Anforderungen verhindern die Installation.

## Installation

Das Program Package wird entsprechend dem Program Layout bereitgestellt:

```text
/Apps/<Program>/
├── App/
├── Resources/
├── SYS/
└── Manifest
```

Die konkrete physische Storage Location ist nicht Bestandteil der Programmidentität.

## Abhängigkeiten

Private Dependencies werden gemeinsam mit dem Program Package installiert und verbleiben innerhalb dessen `SYS`-Bereich.

```text
Program
└── SYS
    └── Private Dependencies
```

Sie dürfen keine globale Installation ihrer Abhängigkeiten erzwingen.

Globale Systemabhängigkeiten müssen über separate Systemmechanismen verwaltet werden.

## Install Scope

Die Installation erfolgt innerhalb eines expliziten Install Scope:

```text
User
System
```

Eine User-Installation darf keine System-Installation oder globale Systemänderung implizieren.

## Transaktion

Die Installation soll als zusammengehörige Transaktion behandelt werden:

```text
Begin
  ↓
Stage Package
  ↓
Validate
  ↓
Prepare Registration
  ↓
Commit
  ↓
Verify
```

Schlägt die Installation fehl, muss NovaOS einen konsistenten vorherigen Zustand erhalten oder wiederherstellen können.

## Berechtigungen

Installations-Authority und spätere Runtime-Authority bleiben getrennt.

```text
Install Permission
       ≠
Program Runtime Capability
```

Capability-Anforderungen des Program Manifests dürfen während der Installation registriert werden, gelten dadurch jedoch nicht automatisch als gewährt.

## Registrierung

Nach erfolgreichem Commit wird das Programm mit mindestens folgenden Informationen registriert:

```text
ProgramID
Version
Install Scope
Package Reference
Manifest Reference
State
```

Erst eine erfolgreich verifizierte Installation darf regulär als installiert veröffentlicht werden.

## Normative Anforderungen

1. NovaOS MUSS Program Packages vor der Installation validieren.
2. Integrität und Vertrauensstatus MÜSSEN prüfbar sein.
3. Zwingende Dependencies MÜSSEN vor dem Commit auflösbar sein.
4. Der Install Scope MUSS vor der Installation feststehen.
5. Installation SOLL transaktional erfolgen.
6. Fehlgeschlagene Installationen DÜRFEN keinen inkonsistenten sichtbaren Zustand hinterlassen.
7. Private Dependencies MÜSSEN innerhalb des Program Packages isoliert bleiben können.
8. Eine Installation DARF das globale `/System` nicht implizit verändern.
9. Installations-Authority DARF keine Runtime-Authority erzeugen.
10. Ein Programm DARF erst nach erfolgreichem Commit und Registrierung als installiert gelten.

## Abhängigkeiten

- `NPSPEC-PROGRAM-PACKAGE-0001`
- `NPSPEC-PROGRAM-MANIFEST-0001`
- `NPSPEC-PROGRAM-LAYOUT-0001`
- `NPSPEC-PROGRAM-DEPENDENCY-0002`
- `NPSPEC-PROGRAM-INSTALLSCOPE-0001`
- `NPSPEC-FILESYSTEM-TRANSACTION-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`

## Ergebnis

NovaOS installiert klassische Programme als validierte und transaktionale Program Packages. Install Scope, private Abhängigkeiten, Registrierung und Berechtigungen bleiben klar getrennt, sodass Installationen konsistent erfolgen, ohne das globale System oder die spätere Runtime-Authority unbeabsichtigt zu verändern.