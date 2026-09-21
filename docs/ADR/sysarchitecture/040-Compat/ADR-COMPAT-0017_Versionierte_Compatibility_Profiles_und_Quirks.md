# ADR-COMPAT-0017 – Versionierte Compatibility Profiles und Quirks

## Status

Angenommen

## Kategorie

Compatibility / Versioning / Profiles / Legacy

## Kontext

Legacy-Software hängt häufig nicht nur von einer dokumentierten API oder ABI ab, sondern auch von:

```text
Bestimmten Betriebssystemversionen
Historischem Verhalten
Undokumentierten Eigenheiten
Fehlern älterer Implementierungen
Anwendungsspezifischen Workarounds
```

Eine globale Nachbildung aller historischen Verhaltensweisen würde die Compatibility-Schichten unnötig komplex machen und könnte neue Anwendungen beeinträchtigen.

## Entscheidung

NovaOS verwendet **versionierte Compatibility Profiles** mit optionalen, expliziten **Quirks**.

```text
Application
    ↓
Compatibility Profile
    ↓
Optional Quirks
    ↓
Compatibility Personality
    ↓
NovaOS
```

Ein Profile beschreibt die grundsätzlich erwartete Compatibility-Umgebung. Quirks ergänzen gezielte Abweichungen für bekannte Sonderfälle.

## Compatibility Profile

Ein Profile definiert beispielsweise:

```text
Target OS / Environment
API / ABI Version
Runtime Version
Filesystem Semantics
Configuration Semantics
Compatibility Features
```

Profiles besitzen eine eindeutige Identität und Version.

Beispiel:

```text
win32.profile.v3
linux.profile.v2
posix.profile.v1
```

## Quirks

Quirks bilden eng begrenzte historische oder anwendungsspezifische Sonderfälle ab.

```text
Compatibility Profile
        +
Application Quirk
        ↓
Effective Compatibility Environment
```

Ein Quirk darf nur das Verhalten verändern, das für den jeweiligen Kompatibilitätsfall erforderlich ist.

## Zuordnung

NovaOS kann Profiles und Quirks anhand von Metadaten bestimmen:

```text
Executable Identity
Binary Hash
Application Version
Declared Requirements
Compatibility Metadata
User / Administrator Override
```

Automatische Erkennung darf keine zusätzlichen Sicherheitsrechte vergeben.

## Versionierung

Compatibility Profiles und Quirks werden unabhängig von der nativen NovaOS-Architektur versioniert.

```text
NovaOS
 ├── Profile v1
 ├── Profile v2
 └── Profile v3
```

Mehrere Versionen dürfen parallel existieren, wenn unterschiedliche Anwendungen sie benötigen.

## Reproduzierbarkeit

Die verwendete Compatibility-Konfiguration muss eindeutig bestimmbar sein.

```text
Application Version
      +
Profile Version
      +
Quirk Set
      =
Compatibility Environment
```

Damit kann ein funktionierender Legacy-Zustand reproduziert und diagnostiziert werden.

## Sicherheitsgrenzen

Profiles und Quirks dürfen ausschließlich Compatibility-Verhalten verändern.

Sie dürfen insbesondere keine:

```text
Capability Checks
Memory Protection
Security Policies
Trust Requirements
Data Sovereignty Rules
Isolation Boundaries
```

deaktivieren oder umgehen.

## Normative Anforderungen

1. Compatibility Profiles MÜSSEN explizit versioniert sein.
2. Mehrere Profile-Versionen MÜSSEN parallel existieren können.
3. Anwendungsspezifische Sonderfälle SOLLEN als Quirks statt als globale Änderungen implementiert werden.
4. Quirks MÜSSEN möglichst eng auf Anwendung und Version begrenzt sein.
5. Profile und aktive Quirks MÜSSEN introspektierbar sein.
6. Die Compatibility-Konfiguration MUSS reproduzierbar beschrieben werden können.
7. Quirks DÜRFEN die native NovaOS-Architektur nicht verändern.
8. Profiles und Quirks DÜRFEN keine zusätzlichen Sicherheitsrechte erzeugen.
9. Veraltete Profiles und Quirks MÜSSEN deaktivierbar und entfernbar sein.
10. Compatibility-Updates DÜRFEN bestehende Profile nicht stillschweigend semantisch verändern.

## Konsequenzen

### Positiv

- historische Software kann gezielt kompatibel gehalten werden,
- Sonderfälle bleiben lokal begrenzt,
- unterschiedliche Compatibility-Versionen können parallel existieren,
- Fehler lassen sich reproduzierbarer diagnostizieren.

### Negativ

- Profile und Quirks müssen langfristig gepflegt werden,
- die Zahl der Compatibility-Konfigurationen kann wachsen,
- anwendungsspezifische Quirks erhöhen den Testaufwand.

## Abhängigkeiten

- `ADR-COMPAT-0003_Compatibility_Personalities`
- `ADR-COMPAT-0008_Legacy_Software_in_Compatibility_Domains`
- `ADR-COMPAT-0010_Legacy_Runtime_side_by_side`
- `ADR-COMPAT-0016_Security_nicht_zugunsten_von_Kompatibilität_absenken`
- `ADR-ARCH-0019_Evolvierbar`
- `ADR-VERIFY-0010_Reproducible_Verification`

## Ergebnis

NovaOS kapselt historische Sonderfälle nach dem Prinzip:

```text
Stable Compatibility Personality
          ↓
Versioned Profile
          ↓
Minimal Explicit Quirks
```

Damit können alte Anwendungen gezielt unterstützt werden, ohne historische Sonderfälle dauerhaft in die allgemeine NovaOS-Architektur einzubauen.