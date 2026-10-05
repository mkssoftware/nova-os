# NPSPEC-PROGRAM-INSTALLSCOPE-0001 – Nova Program Install Scope

## Status

Angenommen

## Kategorie

Program / Installation

## Zweck

NovaOS definiert den Install Scope eines klassischen Programms.

Der Install Scope bestimmt, für welchen Benutzer- oder Systemkontext ein Program Package installiert und sichtbar gemacht wird, ohne die stabile Programmidentität zu verändern.

## Grundprinzipien

```text
Install Scope ≠ Program Identity
Installation ≠ Authority
Visibility ≠ Permission
User Install ≠ System Install
Install Location ≠ Install Scope
```

## Install Scopes

NovaOS unterstützt mindestens:

```text
User
System
```

### User

Eine User-Installation gilt ausschließlich für einen bestimmten Benutzer.

```text
User A → Program verfügbar
User B → Program nicht automatisch verfügbar
```

Für eine reine User-Installation sind keine globalen Systemänderungen erforderlich.

### System

Eine System-Installation stellt das Programm systemweit bereit.

```text
System Install
├── User A
├── User B
└── User C
```

Die systemweite Installation benötigt entsprechende Installations-Authority.

Systemweite Verfügbarkeit bedeutet nicht, dass das Programm automatisch Zugriff auf Daten aller Benutzer erhält.

## Programmidentität

Der Install Scope ist nicht Bestandteil der stabilen `ProgramID`.

```text
ProgramID
   +
InstallScope
   ↓
Program Installation
```

Dieselbe Programmidentität kann dadurch in unterschiedlichen Installationskontexten erkannt werden.

## Program Package

Unabhängig vom Install Scope bleibt das Paketmodell erhalten:

```text
Program
├── App/
├── Resources/
├── SYS/
└── Manifest
```

Private `SYS`-Abhängigkeiten bleiben Bestandteil der jeweiligen Programminstallation und verändern nicht das globale `/System`.

## Sichtbarkeit

Der Install Scope beeinflusst, in welchen Benutzerkontexten das Programm entdeckt und gestartet werden kann.

```text
Installed Program
      ↓
Install Scope
      ↓
Userspace Projection
      ↓
Visible Program
```

Die konkrete physische Speicherung darf davon unabhängig sein.

## Berechtigungen

Installation und Programmausführung besitzen getrennte Berechtigungsmodelle.

```text
Install Authority
      ≠
Runtime Authority
```

Eine systemweite Installation gewährt dem Programm keine zusätzlichen Laufzeit-Capabilities.

## Updates und Entfernung

Updates müssen den bestehenden Install Scope berücksichtigen.

Eine User-Installation darf nicht ohne explizite Autorisierung in eine System-Installation umgewandelt werden.

Das Entfernen einer User-Installation darf Installationen anderer Benutzer oder eine vorhandene System-Installation nicht unbeabsichtigt entfernen.

## Normative Anforderungen

1. NovaOS MUSS mindestens `User`- und `System`-Installationen unterstützen.
2. Der Install Scope DARF die stabile `ProgramID` nicht verändern.
3. User-Installationen MÜSSEN auf den jeweiligen Benutzerkontext begrenzbar sein.
4. System-Installationen MÜSSEN explizite Installations-Authority erfordern.
5. Systemweite Sichtbarkeit DARF keine zusätzliche Laufzeit-Authority erzeugen.
6. Der Install Scope DARF NICHT vom physischen Installationspfad abhängig sein.
7. Private `SYS`-Abhängigkeiten MÜSSEN auch bei unterschiedlichen Install Scopes isoliert bleiben.
8. Updates MÜSSEN den Install Scope der Installation berücksichtigen.
9. Änderungen des Install Scope MÜSSEN explizit autorisiert werden.
10. Entfernung einer Installation DARF andere Install Scopes nicht unbeabsichtigt beeinflussen.

## Abhängigkeiten

- `NPSPEC-PROGRAM-PACKAGE-0001`
- `NPSPEC-PROGRAM-MANIFEST-0001`
- `NPSPEC-PROGRAM-LAYOUT-0001`
- `NPSPEC-PROGRAM-SYS-0001`
- `NPSPEC-PROGRAM-NAMESPACE-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`

## Ergebnis

NovaOS trennt die Identität eines Programms von dessen Installationsumfang. Programme können benutzerspezifisch oder systemweit bereitgestellt werden, ohne dadurch globale Systemänderungen, andere Installationen oder zusätzliche Laufzeitberechtigungen zu erzeugen.