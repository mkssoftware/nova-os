# NPSPEC-PROGRAM-LAYOUT-0001 – Nova Program Layout

## Status

Angenommen

## Kategorie

Program / Layout

## Zweck

NovaOS definiert eine einheitliche logische Struktur für klassische monolithische Programme unter `/Apps`.

Programmcode, Ressourcen und private Systemabhängigkeiten bleiben dabei klar vom globalen System getrennt.

## Grundprinzipien

```text
Program Layout ≠ Program Identity
Program Directory ≠ Authority
Private SYS ≠ Global /System
Logical Layout ≠ Physical Storage Requirement
```

Die stabile `ProgramID` bleibt unabhängig vom Installationspfad.

## Standardstruktur

Ein installiertes Programm wird logisch nach folgendem Schema organisiert:

```text
/Apps/<Program>/
├── App/
├── Resources/
├── SYS/
└── Manifest
```

### App

`App/` enthält die eigentlichen Programmkomponenten:

```text
App/
├── Executables
├── Libraries
└── Program Components
```

Welche Komponenten ausführbar sind, wird durch das Program Manifest bestimmt.

### Resources

`Resources/` enthält programmspezifische Ressourcen, beispielsweise:

```text
Icons
Images
Localization
Templates
Static Data
UI Resources
```

Ressourcen gehören zum Program Package und sind von Benutzerdaten zu unterscheiden.

### SYS

`SYS/` enthält private Systemabhängigkeiten des Programms:

```text
SYS/
├── Libraries/
├── Runtime/
└── Dependencies/
```

Diese Komponenten bleiben physisch im Programmbereich.

NovaOS kann sie über einen programmspezifischen Overlay logisch in den System-Namespace einblenden:

```text
Global /System
      +
Program SYS
      ↓
Effective /System
```

Dieser effektive System-Namespace gilt ausschließlich im Kontext des jeweiligen Programms.

### Manifest

Das Manifest beschreibt das Program Package gemäß:

```text
NPSPEC-PROGRAM-MANIFEST-0001
```

Es enthält unter anderem Programmidentität, Version, Einstiegspunkte, Abhängigkeiten und Capability-Anforderungen.

## Isolation

Programme dürfen ihre privaten Dateien nicht ungefragt in andere Programmbereiche oder das globale `/System` verteilen.

```text
/Apps/ProgramA/
/Apps/ProgramB/
```

bilden getrennte Programmbereiche.

Gemeinsam benötigte globale Systemkomponenten werden über dafür vorgesehene Systemmechanismen verwaltet.

## Benutzerdaten

Persistente Benutzerdaten gehören nicht in das Program Package.

Programme greifen über autorisierte Userspace- und Filesystem-Schnittstellen auf Benutzerdaten zu.

Programmspezifische Benutzereinstellungen werden ebenfalls über das NovaOS-Settings-Modell verwaltet.

## Normative Anforderungen

1. Klassische Programme MÜSSEN logisch unter `/Apps` integrierbar sein.
2. Program Packages SOLLEN `App`, `Resources`, `SYS` und `Manifest` klar trennen.
3. `App` MUSS Programmcode und programminterne Komponenten aufnehmen können.
4. `Resources` MUSS programmspezifische Ressourcen aufnehmen können.
5. `SYS` MUSS private Laufzeit- und Systemabhängigkeiten aufnehmen können.
6. Private `SYS`-Komponenten DÜRFEN das globale `/System` nicht verändern.
7. Der programmspezifische `SYS`-Overlay DARF nur im jeweiligen Programmkontext wirksam sein.
8. Persistente Benutzerdaten DÜRFEN NICHT zwingend innerhalb des Program Packages gespeichert werden.
9. Der Installationspfad DARF NICHT die stabile Programmidentität darstellen.
10. Der Programmbereich DARF keine implizite Authority erzeugen.

## Abhängigkeiten

- `NPSPEC-PROGRAM-PACKAGE-0001`
- `NPSPEC-PROGRAM-MANIFEST-0001`
- `NPSPEC-USERSPACE-LAYOUT-0001`
- `NPSPEC-USERSPACE-SETTINGS-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`

## Ergebnis

NovaOS erhält eine klare und isolierte Struktur für klassische Programme. Programmcode, Ressourcen und private Systemabhängigkeiten bleiben innerhalb des Programmbereichs, während globale Systemkomponenten und Benutzerdaten davon getrennt verwaltet werden.