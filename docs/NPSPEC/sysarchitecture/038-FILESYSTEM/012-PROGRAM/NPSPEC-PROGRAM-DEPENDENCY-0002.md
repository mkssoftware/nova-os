# NPSPEC-PROGRAM-DEPENDENCY-0002 – Nova Program Dependencies

## Status

Angenommen

## Kategorie

Program / Dependencies

## Zweck

NovaOS definiert die versionierte Deklaration, deterministische Auflösung und Isolation von Abhängigkeiten klassischer Programme.

Version 0002 konkretisiert insbesondere Identität, Versionsbindung, Konfliktfreiheit und reproduzierbare Dependency-Auflösung.

## Grundprinzipien

```text
DependencyID ≠ Path
Dependency ≠ Global Installation
Private Dependency ≠ Global Dependency
Version Requirement ≠ Selected Version
Resolution ≠ Authority
```

Private Abhängigkeiten sollen bevorzugt Bestandteil des jeweiligen Program Packages sein.

## Dependency-Modell

```text
Dependency
├── DependencyID
├── VersionConstraint
├── Type
├── Required
└── Constraints
```

Optional:

```text
Architecture
ABI
Features
Integrity
Provider
```

`DependencyID` identifiziert die Abhängigkeit unabhängig von Dateiname oder Speicherpfad.

## Typen

```text
Private
System
Runtime
Optional
```

### Private

Wird vom Program Package bereitgestellt und liegt typischerweise unter:

```text
/Apps/<Program>/SYS/
```

### System

Referenziert eine von NovaOS bereitgestellte Systemkomponente.

Eine solche Deklaration darf das globale `/System` nicht automatisch verändern.

## Versionsauflösung

Das Manifest beschreibt Anforderungen, nicht zwingend eine konkrete physische Datei.

```text
Dependency Requirement
        ↓
Candidate Discovery
        ↓
Constraint Validation
        ↓
Deterministic Selection
        ↓
Resolved Dependency
```

Die Auswahl muss reproduzierbar sein, wenn dieselben Eingaben und verfügbaren Komponenten vorliegen.

## Private Versionen

Programme dürfen unabhängig voneinander verschiedene Versionen derselben Dependency verwenden.

```text
Program A
└── Runtime X 2.1

Program B
└── Runtime X 3.0
```

Es entsteht dadurch kein globaler Versionskonflikt.

## SYS-Integration

Private Dependencies können über den programmspezifischen SYS Overlay bereitgestellt werden:

```text
Program SYS
    +
Global /System
    ↓
Effective /System
```

Der Overlay gilt ausschließlich für den jeweiligen Programmkontext.

## Konflikte

Nicht erfüllbare Anforderungen müssen explizit erkannt werden.

NovaOS darf nicht stillschweigend eine inkompatible Dependency verwenden.

Mögliche Ergebnisse:

```text
Resolved
Missing
Incompatible
Ambiguous
Invalid
```

Bei zwingenden Dependencies verhindern `Missing`, `Incompatible`, `Ambiguous` oder `Invalid` den betroffenen Start.

## Update und Entfernung

Updates eines Programms dürfen dessen private Dependencies gemeinsam aktualisieren.

Das Entfernen eines Programms soll dessen ausschließlich private Dependencies ebenfalls entfernen können.

Gemeinsam genutzte globale Komponenten dürfen dadurch nicht gelöscht werden.

## Sicherheit und Integrität

Dependencies müssen in die Integritäts- und Vertrauensprüfung des Program Packages beziehungsweise des jeweiligen Systemproviders einbezogen werden.

Das Laden einer Dependency erweitert nicht die Authority des Programms.

```text
Dependency Code
      ↓
Program Security Context
```

## Normative Anforderungen

1. Jede Dependency MUSS über eine stabile `DependencyID` identifizierbar sein.
2. Dependencies MÜSSEN Versionsanforderungen deklarieren können.
3. Dependency-Auflösung MUSS deterministisch sein.
4. Inkompatible oder mehrdeutige Auflösungen MÜSSEN erkannt werden.
5. Private Dependencies SOLLEN im Program Package verbleiben.
6. Unterschiedliche Programme DÜRFEN unterschiedliche Versionen derselben privaten Dependency verwenden.
7. Private Dependencies DÜRFEN keine globalen Versionskonflikte erzeugen.
8. System-Dependencies DÜRFEN das globale `/System` nicht automatisch verändern.
9. Dependency-Integrität MUSS prüfbar sein.
10. Dependency-Auflösung und Laden DÜRFEN keine zusätzliche Authority erzeugen.

## Abhängigkeiten

- `NPSPEC-PROGRAM-PACKAGE-0001`
- `NPSPEC-PROGRAM-MANIFEST-0001`
- `NPSPEC-PROGRAM-SYS-0001`
- `NPSPEC-PROGRAM-SYSOVERLAY-0001`
- `NPSPEC-PROGRAM-NAMESPACE-0001`

## Ergebnis

NovaOS erhält eine deterministische und versionssichere Dependency-Auflösung. Private Abhängigkeiten bleiben vollständig programmspezifisch, wodurch mehrere Versionen parallel existieren können, ohne globale Dependency-Konflikte oder unbeabsichtigte Änderungen am System zu verursachen.