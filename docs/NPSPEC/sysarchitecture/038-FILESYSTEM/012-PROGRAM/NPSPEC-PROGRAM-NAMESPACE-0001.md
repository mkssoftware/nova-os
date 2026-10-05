# NPSPEC-PROGRAM-NAMESPACE-0001 – Nova Program Namespace

## Status

Angenommen

## Kategorie

Program / Namespace

## Zweck

NovaOS definiert für jedes klassische Programm einen eigenen logischen Namespace.

Dieser stellt dem Programm eine konsistente Sicht auf Programmdateien, Ressourcen, private Systemabhängigkeiten und autorisierte externe Ressourcen bereit, ohne den globalen Namespace zu verändern.

## Grundprinzipien

```text
Program Namespace ≠ Global Namespace
Namespace Visibility ≠ Authority
Path ≠ Object Identity
Projection ≠ Copy
Program Context ≠ Physical Storage
```

## Namespace-Modell

Der Program Namespace wird aus mehreren vorhandenen Bereichen zusammengesetzt:

```text
Global Namespace
      +
Program Package
      +
Program SYS Overlay
      +
Authorized Resources
      ↓
Effective Program Namespace
```

Die zugrunde liegenden Objekte bleiben an ihren tatsächlichen Speicherorten.

## Programmbereich

Das eigene Program Package ist im Program Namespace verfügbar:

```text
/Apps/<Program>/
├── App/
├── Resources/
└── SYS/
```

Die stabile Programmidentität bleibt unabhängig von diesen Pfaden.

## Effektives System

Der globale `/System`-Namespace wird mit dem privaten `SYS` des Programms kombiniert:

```text
Global /System
      +
Private SYS
      ↓
Effective /System
```

Dieser effektive `/System`-Namespace gilt ausschließlich für das jeweilige Programm.

## Externe Ressourcen

Benutzerdaten, Workspace-Ressourcen, Volumes oder andere externe Objekte dürfen nur eingebunden werden, wenn eine entsprechende Authority besteht.

```text
Capability
    ↓
Authorized Object
    ↓
Program Projection
```

Die Projektion einer Ressource erzeugt keine zusätzliche Berechtigung.

## Isolation

Programme besitzen voneinander getrennte Namespace-Kontexte:

```text
Program A → Namespace A
Program B → Namespace B
```

Änderungen an einer programmspezifischen Projektion dürfen den Namespace anderer Programme nicht automatisch verändern.

## Auflösung

Namespace-Auflösung erfolgt deterministisch:

```text
Program Path
    ↓
Namespace Resolution
    ↓
Projection / ObjectID
    ↓
Capability Check
    ↓
Authorized Handle
```

Nach erfolgreicher Auflösung sollen weitere Zugriffe über stabile Objektidentitäten beziehungsweise autorisierte Handles erfolgen.

## Lebenszyklus

Der Program Namespace wird beim Erzeugen des Programmkontexts aufgebaut und beim Beenden kontrolliert freigegeben.

Persistente Änderungen an eigentlichen Objekten bleiben davon unberührt.

## Normative Anforderungen

1. Jedes gestartete Programm MUSS einen eigenen Namespace-Kontext besitzen können.
2. Der Program Namespace DARF den globalen Namespace nicht verändern.
3. Private `SYS`-Abhängigkeiten MÜSSEN über den programmspezifischen SYS Overlay integrierbar sein.
4. Externe Ressourcen DÜRFEN nur mit gültiger Authority eingebunden werden.
5. Namespace-Sichtbarkeit DARF keine zusätzliche Authority erzeugen.
6. Programme MÜSSEN voneinander isolierte Namespace-Sichten besitzen können.
7. Namespace-Auflösung MUSS deterministisch sein.
8. Projektionen DÜRFEN keine unnötigen Kopien erzeugen.
9. Pfade DÜRFEN NICHT als stabile Objektidentität behandelt werden.
10. Nach der Auflösung SOLLEN Zugriffe über `ObjectID` und autorisierte Handles erfolgen.

## Abhängigkeiten

- `NPSPEC-PROGRAM-LAYOUT-0001`
- `NPSPEC-PROGRAM-SYS-0001`
- `NPSPEC-PROGRAM-SYSOVERLAY-0001`
- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`

## Ergebnis

Jedes klassische NovaOS-Programm erhält eine eigene kontrollierte Sicht auf das System. Programmdateien, private Abhängigkeiten und autorisierte externe Ressourcen können zu einem effektiven Namespace zusammengesetzt werden, ohne globale Namespace-Strukturen oder Sicherheitsgrenzen zu verändern.