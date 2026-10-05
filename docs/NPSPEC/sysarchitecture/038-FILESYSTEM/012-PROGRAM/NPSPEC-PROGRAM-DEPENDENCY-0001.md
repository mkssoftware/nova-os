# NPSPEC-PROGRAM-DEPENDENCY-0001 – Nova Program Dependencies

## Status

Angenommen

## Kategorie

Program / Dependencies

## Zweck

NovaOS definiert die Deklaration, Auflösung und Isolation von Abhängigkeiten klassischer Programme.

Programme sollen benötigte Abhängigkeiten bevorzugt selbst mitführen können, ohne globale Systemkomponenten oder andere Programme zu beeinflussen.

## Grundprinzipien

```text
Dependency ≠ Global Installation
Private Dependency ≠ System Dependency
Dependency Identity ≠ Path
Dependency Resolution ≠ Authority
```

## Abhängigkeitsarten

Ein Programm kann unterschiedliche Abhängigkeiten besitzen:

```text
Private
System
Optional
Runtime
```

Private Abhängigkeiten werden innerhalb des Program Packages bereitgestellt.

Systemabhängigkeiten referenzieren vorhandene NovaOS-Komponenten und dürfen diese nicht automatisch installieren oder verändern.

## Deklaration

Abhängigkeiten werden im Program Manifest beschrieben.

```text
Dependency
├── DependencyID
├── Version Requirement
├── Type
└── Constraints
```

Optionale Anforderungen wie Architektur oder benötigte Funktionen dürfen ergänzt werden.

## Private Abhängigkeiten

Private Bibliotheken und Laufzeiten werden bevorzugt unter:

```text
/Apps/<Program>/SYS/
```

bereitgestellt.

Sie können über den SYS Overlay in den effektiven System-Namespace des Programms eingebunden werden.

```text
Private Dependency
       ↓
Program SYS
       ↓
SYS Overlay
       ↓
Effective /System
```

Andere Programme bleiben davon unberührt.

## Auflösung

Die Dependency-Auflösung erfolgt deterministisch:

```text
Manifest
   ↓
Dependency Requirements
   ↓
Resolve Candidates
   ↓
Validate Compatibility
   ↓
Select Dependency
```

Bei mehreren kompatiblen Kandidaten entscheidet eine definierte Resolution Policy.

Nicht erfüllte zwingende Abhängigkeiten verhindern den betroffenen Programmstart.

## Versionen

Unterschiedliche Programme dürfen unterschiedliche Versionen derselben privaten Abhängigkeit verwenden.

```text
Program A → Library 1
Program B → Library 2
```

Ein Update von Program A darf dadurch Program B nicht beeinflussen.

## Globale Abhängigkeiten

Globale Systemkomponenten dürfen nur über dafür vorgesehene Systemmechanismen installiert oder verändert werden.

Ein Program Package darf eine globale Änderung nicht allein durch eine Dependency-Deklaration erzwingen.

## Sicherheit

Abhängigkeiten gehören zum Vertrauens- und Integritätskontext des Program Packages.

Das Auflösen oder Laden einer Abhängigkeit erzeugt keine zusätzliche Authority.

Abhängiger Code arbeitet innerhalb des Sicherheitskontexts des Programms.

## Normative Anforderungen

1. Programme MÜSSEN Abhängigkeiten deklarieren können.
2. Dependency-Auflösung MUSS deterministisch sein.
3. Private Abhängigkeiten SOLLEN innerhalb des Program Packages gespeichert werden.
4. Private Abhängigkeiten DÜRFEN über den Program SYS Overlay eingebunden werden.
5. Unterschiedliche Programme DÜRFEN unterschiedliche Versionen derselben privaten Abhängigkeit verwenden.
6. Private Abhängigkeiten DÜRFEN andere Programme nicht automatisch beeinflussen.
7. Eine Dependency-Deklaration DARF keine globale Installation erzwingen.
8. Globale Systemänderungen MÜSSEN separat autorisiert werden.
9. Zwingende inkompatible oder fehlende Abhängigkeiten MÜSSEN erkannt werden.
10. Dependency-Auflösung DARF keine zusätzliche Authority erzeugen.

## Abhängigkeiten

- `NPSPEC-PROGRAM-PACKAGE-0001`
- `NPSPEC-PROGRAM-MANIFEST-0001`
- `NPSPEC-PROGRAM-SYS-0001`
- `NPSPEC-PROGRAM-SYSOVERLAY-0001`
- `NPSPEC-PROGRAM-NAMESPACE-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`

## Ergebnis

NovaOS ermöglicht Programmen isolierte und deterministisch aufgelöste Abhängigkeiten. Programme können eigene Bibliotheken und Laufzeiten mitführen und unabhängig aktualisieren, ohne globale Systemkomponenten oder die Abhängigkeiten anderer Programme unnötig zu verändern.