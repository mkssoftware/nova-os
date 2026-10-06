# NPSPEC-COMPAT-PERSONALITY-0001 – Nova Compatibility Personality

## Status

Angenommen

## Kategorie

Compatibility / Personality

## Zweck

NovaOS definiert Compatibility Personalities als isolierte, versionierte Kompatibilitätsumgebungen für Software, die das Verhalten und die Systemkonventionen eines anderen Betriebssystems oder einer älteren Plattform erwartet.

Eine Personality stellt die erwartete Umgebung bereit, ohne NovaOS selbst in das emulierte Systemmodell umzuwandeln.

## Grundprinzipien

```text
Personality ≠ NovaOS
Personality ≠ Virtual Machine
Personality ≠ Authority
Personality ≠ ABI
Personality ≠ Runtime
Compatibility Behavior ≠ Native Behavior
Foreign Privilege ≠ Nova Authority
```

## Modell

```text
Foreign Program
      ↓
Personality Detection
      ↓
Compatibility Personality
├── ABI Provider
├── API Mapping
├── Namespace View
├── Environment
├── Runtime Integration
├── System Behavior
└── Compatibility State
      ↓
NovaOS
```

## Personality-Profil

Eine Personality wird mindestens beschrieben durch:

```text
CompatibilityPersonality
├── PersonalityID
├── Version
├── TargetPlatform
├── SupportedABIs
├── SupportedAPIs
├── NamespaceModel
├── EnvironmentModel
├── RuntimeRequirements
└── CompatibilityLevel
```

## Systemumgebung

Eine Personality darf eine fremde logische Umgebung projizieren:

```text
Filesystem Layout
Path Syntax
Environment Variables
Configuration Model
Process Conventions
Library Locations
Temporary Paths
Known System Locations
```

Diese Umgebung ist eine Compatibility-Projektion und verändert nicht den nativen NovaOS-Namespace.

## Beispiel

```text
NovaOS
   ↓
Windows Personality
├── Windows-compatible paths
├── Environment variables
├── PE integration
├── Win32 API mapping
└── Windows ABI provider
```

oder:

```text
NovaOS
   ↓
POSIX Personality
├── POSIX paths
├── POSIX process semantics
├── POSIX APIs
└── Compatible ABI
```

## API- und ABI-Integration

Eine Personality darf mehrere Compatibility Provider kombinieren:

```text
Personality
├── ABI Provider
├── API Provider
├── Runtime Provider
└── Format Provider
```

Die Personality koordiniert diese Komponenten, ersetzt sie jedoch nicht.

## Prozesskontext

Programme einer Personality erhalten einen eigenen Compatibility-Kontext:

```text
Native Nova Namespace
        ↓
Personality Projection
        ↓
Program Namespace
        ↓
Process Namespace
```

Mehrere Personalities dürfen gleichzeitig auf demselben NovaOS-System aktiv sein.

## Verhalten

Eine Personality darf erwartete Plattformsemantik nachbilden, beispielsweise:

```text
Process Behavior
Thread Behavior
Path Rules
Case Sensitivity
Error Mapping
Environment Semantics
Configuration Access
Library Resolution
```

Abweichungen müssen kontrolliert und diagnostizierbar sein.

## Sicherheit

Fremde Sicherheits- und Privilegmodelle werden nicht direkt übernommen.

```text
Foreign Privilege Request
        ↓
Personality
        ↓
Nova Capability / Policy
        ↓
Effective Authority
```

Begriffe wie `Administrator`, `root` oder ähnliche fremde Privilegstufen erzeugen keine native NovaOS-Authority.

## Isolation

Personality-spezifische Zustände müssen von nativen Systemzuständen getrennt bleiben.

Dazu gehören insbesondere:

```text
Configuration
Runtime State
Temporary Data
Compatibility Metadata
Virtualized System Locations
```

## Versionierung

Mehrere Versionen derselben Personality dürfen parallel existieren:

```text
PersonalityID
├── Version 1
├── Version 2
└── Version 3
```

Programme dürfen an eine kompatible Personality-Version gebunden werden.

## Fehler und Degradation

Nicht unterstützte Funktionen müssen kontrolliert behandelt werden:

```text
Supported
Emulated
PartiallySupported
Unsupported
Unavailable
```

Eine Personality darf fehlende Funktionalität nicht durch Umgehung von NovaOS-Sicherheitsgrenzen simulieren.

## Normative Anforderungen

1. Compatibility Personalities MÜSSEN vom nativen NovaOS-Systemmodell getrennt bleiben.
2. Jede Personality MUSS eine stabile `PersonalityID` besitzen.
3. Personalities MÜSSEN versionierbar sein.
4. Mehrere Personalities MÜSSEN parallel betrieben werden können.
5. ABI-, API- und Runtime-Kompatibilität MÜSSEN getrennte Komponenten bleiben können.
6. Personality-spezifische Namespace-Ansichten MÜSSEN als Projektionen realisierbar sein.
7. Fremde Pfad- und Umgebungskonventionen DÜRFEN den nativen Namespace nicht verändern.
8. Fremde Privilegmodelle DÜRFEN keine native NovaOS-Authority erzeugen.
9. Alle geschützten Operationen MÜSSEN weiterhin Capability- und Policy-Prüfungen durchlaufen.
10. Personality-Zustände MÜSSEN vom nativen Systemzustand isolierbar sein.
11. Nicht unterstützte Funktionen MÜSSEN kontrolliert fehlschlagen oder degradieren.
12. Compatibility Provider DÜRFEN Sicherheitsgrenzen nicht umgehen.
13. Personality, Version, Provider und Compatibility-Zustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-COMPAT-ABI-0001`
- `NPSPEC-PROGRAM-LEGACY-0001`
- `NPSPEC-PROGRAM-COMPATIBILITY-0001`
- `NPSPEC-NAMESPACE-PROCESS-0001`
- `NPSPEC-NAMESPACE-APPLICATION-0001`
- `NPSPEC-NAMESPACE-OVERLAY-0002`
- `NPSPEC-SYSTEM-RUNTIME-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`

## Ergebnis

NovaOS kann fremden Programmen über Compatibility Personalities eine passende Betriebssystemumgebung bereitstellen, ohne das native NovaOS-Systemmodell zu verändern. ABI, APIs, Namespace, Runtime und erwartetes Plattformverhalten werden kontrolliert kombiniert, während NovaOS-Capabilities, Sicherheitsgrenzen und native Systemidentitäten maßgeblich bleiben.