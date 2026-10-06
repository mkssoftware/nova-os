# NPSPEC-COMPAT-VERSIONING-0001 – Nova Compatibility Versioning

## Status

Angenommen

## Kategorie

Compatibility / Versioning

## Zweck

NovaOS definiert ein einheitliches Versionierungsmodell für Compatibility-Komponenten.

Dadurch können unterschiedliche Plattform-, ABI-, API-, Syscall-, Runtime- und Emulationsversionen parallel unterstützt werden, ohne die native NovaOS-Architektur an eine bestimmte Fremdplattform zu binden.

## Grundprinzipien

```text
Compatibility Version ≠ NovaOS Version
Platform Version ≠ ABI Version
ABI Version ≠ API Version
Provider Version ≠ Compatibility Version
Newer ≠ Compatible
Version Match ≠ Authority
```

## Versionsmodell

Compatibility-Komponenten versionieren ihre relevanten Ebenen unabhängig:

```text
CompatibilityVersion
├── PersonalityVersion
├── ABIVersion
├── APIVersion
├── SyscallVersion
├── RuntimeVersion
├── ProtocolVersion
├── ProviderVersion
└── TranslationVersion
```

Nicht jede Compatibility-Umgebung muss alle Ebenen verwenden.

## Compatibility Profile

Die für eine konkrete Software benötigte Umgebung wird als Compatibility Profile beschrieben:

```text
CompatibilityProfile
├── TargetPlatform
├── Architecture
├── Personality
├── ABIRequirements
├── APIRequirements
├── SyscallRequirements
├── RuntimeRequirements
└── AdditionalRequirements
```

## Auflösung

Beim Start fremder Software bestimmt NovaOS die benötigte Compatibility-Konfiguration:

```text
Software Requirements
        ↓
Compatibility Discovery
        ↓
Version Matching
        ↓
Compatibility Validation
        ↓
Provider Selection
        ↓
Execution
```

Die höchste verfügbare Version darf nicht automatisch gewählt werden. Maßgeblich ist die kompatible Version.

## Parallele Versionen

Mehrere Versionen derselben Compatibility-Komponente dürfen gleichzeitig vorhanden sein:

```text
Linux Personality
├── Version A
├── Version B
└── Version C

Legacy Runtime
├── Runtime 1
└── Runtime 2
```

Programme dürfen dadurch weiterhin mit ihrer benötigten Umgebung ausgeführt werden.

## Kompatibilitätsregeln

Versionsbeziehungen können beschrieben werden als:

```text
Exact
Minimum
Maximum
Range
Compatible
Incompatible
Deprecated
```

Kompatibilität muss explizit bestimmt werden und darf nicht allein aus Versionsnummern abgeleitet werden.

## Updates

Compatibility-Komponenten dürfen unabhängig vom NovaOS-Basissystem aktualisiert werden.

Ein Update darf bestehende Compatibility Profiles nicht unkontrolliert verändern.

Neue Versionen können parallel zur bisherigen Version installiert und vor einer Umschaltung validiert werden.

## Entfernung

Eine Compatibility-Version darf erst entfernt werden, wenn Abhängigkeiten geprüft wurden.

```text
Version
   ↓
Dependency Check
   ↓
In Use?
├── Yes → Preserve / Migrate
└── No  → Remove
```

## Sicherheit

Eine ältere Version darf trotz technischer Kompatibilität durch Security Policy blockiert werden.

```text
Compatible
    +
Trust
    +
Security Policy
    =
Usable
```

Versionskompatibilität erzeugt keine Authority und überschreibt keine Sicherheitsanforderungen.

## Normative Anforderungen

1. Compatibility-Versionen MÜSSEN unabhängig von der NovaOS-Version verwaltet werden können.
2. Personality-, ABI-, API-, Syscall-, Runtime- und Provider-Versionen MÜSSEN getrennt versionierbar sein.
3. Mehrere Versionen derselben Compatibility-Komponente MÜSSEN parallel unterstützt werden können.
4. Software MUSS ihre benötigten Compatibility-Anforderungen beschreiben können.
5. Version Matching DARF nicht allein auf numerischen Versionsvergleichen beruhen.
6. Kompatibilität MUSS explizit validiert werden.
7. Updates DÜRFEN bestehende Compatibility-Umgebungen nicht unkontrolliert überschreiben.
8. Vor dem Entfernen einer Version MÜSSEN bestehende Abhängigkeiten geprüft werden.
9. Unsichere oder widerrufene Versionen MÜSSEN durch Policy blockierbar sein.
10. Versionierung DARF keine Authority oder Permission erzeugen.
11. Fallback auf ältere Versionen DARF zwingende Sicherheitsanforderungen nicht umgehen.
12. Verwendete Compatibility-Versionen und Auflösungsentscheidungen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-COMPAT-ABI-0001`
- `NPSPEC-COMPAT-PERSONALITY-0001`
- `NPSPEC-COMPAT-API-0001`
- `NPSPEC-COMPAT-SYSCALL-0001`
- `NPSPEC-COMPAT-LEGACYRUNTIME-0001`
- `NPSPEC-COMPAT-PROTOCOL-0001`
- `NPSPEC-COMPAT-SANDBOX-0001`
- `NPSPEC-CAPABILITY-COMPATIBILITY-0001`
- `NPSPEC-CAPABILITY-TRUST-0001`

## Ergebnis

NovaOS kann mehrere Generationen und Versionen von Compatibility-Komponenten parallel verwalten. Software wird gezielt mit der passenden Personality, ABI, API, Runtime und weiteren Compatibility-Komponenten verbunden, während Updates, Sicherheit und die native NovaOS-Version unabhängig davon bleiben.