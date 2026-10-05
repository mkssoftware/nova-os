# NPSPEC-SYSTEM-LIBRARIES-0001 – Nova System Libraries

## Status

Angenommen

## Kategorie

System / Libraries

## Zweck

NovaOS definiert System Libraries als gemeinsam bereitgestellte Bibliotheken für Systemkomponenten, Services, Runtimes und Programme.

System Libraries besitzen stabile Identitäten und versionierte Schnittstellen. Programmspezifische Bibliotheken verbleiben im privaten `SYS` des jeweiligen Programms.

## Grundprinzipien

```text
Library ≠ Program
Library ≠ Service
Library ≠ Runtime
Library Path ≠ Library Identity
Private Library ≠ System Library
Library Access ≠ Authority
```

## Struktur

Systemweit bereitgestellte Bibliotheken befinden sich logisch unter:

```text
/System/Libraries/
```

Programmspezifische Bibliotheken liegen dagegen unter:

```text
/Apps/<Program>/SYS/Libraries/
```

Sie werden über den Program SYS Overlay in dessen effektive Systemsicht eingebunden.

## Identität

Eine Bibliothek besitzt eine stabile Identität:

```text
Library
├── LibraryID
├── Version
├── ABI / Interface
├── Architecture
└── Compatibility
```

`LibraryID` ist unabhängig von Dateiname, Pfad oder physischem Speicherort.

## Auflösung

Bibliotheken werden über das Dependency-Modell aufgelöst:

```text
Library Requirement
       ↓
Dependency Resolution
       ↓
Private SYS
       ↓
Global /System/Libraries
       ↓
Compatible Library
```

Die konkrete Priorität wird durch die definierte Overlay- und Dependency-Policy bestimmt.

## Versionierung

Mehrere Bibliotheksversionen dürfen parallel existieren, sofern dies erforderlich ist.

```text
LibraryID
├── Version 1
├── Version 2
└── Version 3
```

Programme dürfen unterschiedliche private Versionen derselben Bibliothek verwenden, ohne globale Versionskonflikte zu erzeugen.

## ABI und Kompatibilität

Vor Verwendung müssen relevante Anforderungen geprüft werden können:

```text
Architecture
ABI Version
Library Version
Required Features
Runtime Requirements
```

Eine vorhandene Bibliothek gilt nicht automatisch als kompatibel.

## Updates

System Libraries werden über kontrollierte Systemupdates aktualisiert.

Ein Programm darf seine private Bibliothek aktualisieren, ohne dadurch die globale System Library zu verändern.

Laufende Komponenten dürfen nicht unkontrolliert auf eine inkompatible Bibliotheksversion umgestellt werden.

## Sicherheit

Bibliotheken laufen innerhalb des Sicherheitskontexts der Komponente, die sie verwendet.

```text
Program Authority
      ↓
Library Code
      ↓
System Operation
```

Eine Bibliothek erzeugt keine zusätzliche Authority.

Sicherheitskritische System Libraries müssen auf Integrität und Trust prüfbar sein.

## Normative Anforderungen

1. NovaOS MUSS systemweite Libraries bereitstellen können.
2. System Libraries SOLLEN logisch unter `/System/Libraries/` verfügbar sein.
3. Libraries MÜSSEN stabile Identitäten besitzen können.
4. Library-Versionen MÜSSEN eindeutig unterscheidbar sein.
5. ABI- und Kompatibilitätsanforderungen MÜSSEN prüfbar sein.
6. Programme MÜSSEN private Libraries über `SYS` bereitstellen können.
7. Private Libraries DÜRFEN globale System Libraries nicht verändern.
8. Mehrere Library-Versionen MÜSSEN parallel unterstützt werden können.
9. Library-Auflösung MUSS deterministisch erfolgen.
10. Library-Code DARF keine zusätzliche Authority erzeugen.
11. System-Library-Updates MÜSSEN kontrolliert erfolgen.
12. Identität, Version, ABI und Herkunft einer Library MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-LAYOUT-0001`
- `NPSPEC-SYSTEM-FOUNDATION-0001`
- `NPSPEC-SYSTEM-RUNTIME-0001`
- `NPSPEC-PROGRAM-DEPENDENCY-0002`
- `NPSPEC-PROGRAM-SYS-0001`
- `NPSPEC-PROGRAM-SYSOVERLAY-0001`
- `NPSPEC-PROGRAM-COMPATIBILITY-0001`

## Ergebnis

NovaOS besitzt ein versioniertes und konfliktfreies Bibliotheksmodell. Systemweit bereitgestellte Libraries können gemeinsam genutzt werden, während Programme inkompatible oder spezielle Versionen isoliert über ihren privaten `SYS`-Bereich bereitstellen können.