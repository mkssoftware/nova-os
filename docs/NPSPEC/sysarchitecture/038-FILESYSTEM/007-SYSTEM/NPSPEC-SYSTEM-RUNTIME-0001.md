# NPSPEC-SYSTEM-RUNTIME-0001 – Nova System Runtime

## Status

Angenommen

## Kategorie

System / Runtime

## Zweck

NovaOS definiert die System Runtime als standardisierte Laufzeitumgebung für native Systemkomponenten, Programme und Solutions.

Die Runtime stellt gemeinsame Laufzeitmechanismen oberhalb der System Foundation bereit und verhindert, dass jede Komponente grundlegende Funktionen selbst implementieren muss.

## Grundprinzipien

```text
Runtime ≠ Kernel
Runtime ≠ System Foundation
Runtime ≠ System Framework
Runtime ≠ Program
Runtime Access ≠ Authority
```

## Architektur

```text
Programs / Solutions
        ↓
System Framework
        ↓
System Runtime
        ↓
System Foundation
        ↓
Kernel
```

Die Runtime kann auch direkt von Systemkomponenten verwendet werden, wenn keine höherwertige Framework-Abstraktion erforderlich ist.

## Verantwortlichkeiten

Die Runtime stellt insbesondere gemeinsame Laufzeitfunktionen bereit für:

```text
Memory Management Helpers
Object Lifetime
Task / Async Runtime
Error Handling
Type Information
Module Loading
Runtime Contracts
Serialization
Resource Handles
Diagnostics
```

Sprachspezifische Funktionen dürfen durch zusätzliche Runtime-Komponenten ergänzt werden.

## Runtime-Instanzen

NovaOS darf mehrere Runtime-Versionen parallel unterstützen.

```text
Runtime
├── Version A
├── Version B
└── Version C
```

Programme können dadurch an eine kompatible Runtime gebunden werden, ohne andere Programme zu beeinflussen.

## Private Runtime

Benötigt ein Programm eine eigene Runtime-Version, kann diese über den privaten `SYS`-Bereich bereitgestellt werden:

```text
/Apps/<Program>/SYS/Runtime/
```

Sie wird über den Program SYS Overlay in dessen effektive Systemumgebung eingebunden.

```text
Program
   ↓
Effective /System
   ↓
Private Runtime / System Runtime
```

Die globale Runtime wird dadurch nicht verändert.

## Runtime Resolution

Die Auswahl einer Runtime erfolgt deterministisch:

```text
Runtime Requirement
       ↓
Dependency Resolution
       ↓
Compatibility Check
       ↓
Runtime Selection
       ↓
Execution
```

Fehlende oder inkompatible Runtime-Versionen müssen eindeutig erkannt werden.

## Runtime Contracts

Runtime-Komponenten können überprüfbare Verträge für:

```text
Inputs
Outputs
Types
Resources
Lifetime
Errors
Security Requirements
```

bereitstellen.

Vertragsverletzungen müssen kontrolliert erkannt und behandelt werden können.

## Sicherheit

Runtime-Code läuft innerhalb des Sicherheitskontexts der aufrufenden Komponente.

```text
Program Authority
      ↓
Runtime
      ↓
System Operation
```

Die Runtime darf keine zusätzlichen Capabilities erzeugen oder Sicherheitsgrenzen umgehen.

## Live Evolution

Runtime-Komponenten sollen versioniert und kontrolliert aktualisierbar sein.

Ein Austausch darf laufende Programme nicht unkontrolliert auf eine inkompatible Runtime umstellen.

## Normative Anforderungen

1. NovaOS MUSS eine gemeinsame System Runtime bereitstellen können.
2. Runtime und Kernel MÜSSEN logisch getrennt bleiben.
3. Runtime und System Foundation MÜSSEN getrennte Verantwortlichkeiten besitzen.
4. Mehrere Runtime-Versionen MÜSSEN parallel unterstützt werden können.
5. Programme MÜSSEN private Runtime-Versionen über `SYS` bereitstellen können.
6. Private Runtimes DÜRFEN die globale Runtime nicht verändern.
7. Runtime-Auflösung MUSS deterministisch erfolgen können.
8. Fehlende oder inkompatible Runtimes MÜSSEN erkannt werden.
9. Runtime-Komponenten SOLLEN überprüfbare Contracts unterstützen.
10. Runtime-Code DARF keine zusätzliche Authority erzeugen.
11. Runtime-Versionen MÜSSEN kontrolliert aktualisierbar sein.
12. Runtime-Zustand, Version und verfügbare Funktionen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-FOUNDATION-0001`
- `NPSPEC-SYSTEM-FRAMEWORK-0001`
- `NPSPEC-PROGRAM-DEPENDENCY-0002`
- `NPSPEC-PROGRAM-SYS-0001`
- `NPSPEC-PROGRAM-SYSOVERLAY-0001`
- `NPSPEC-PROGRAM-COMPATIBILITY-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`

## Ergebnis

NovaOS besitzt eine gemeinsame, versionierbare System Runtime für grundlegende Laufzeitmechanismen. Mehrere Runtime-Versionen können parallel existieren, Programme können private Runtimes isoliert bereitstellen und sämtliche Laufzeitfunktionen bleiben dem Capability- und Sicherheitsmodell von NovaOS untergeordnet.