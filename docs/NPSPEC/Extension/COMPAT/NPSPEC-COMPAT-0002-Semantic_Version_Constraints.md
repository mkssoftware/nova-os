# NPSPEC-COMPAT-0002 – Semantic Version Constraints

## Status

Angenommen

## Zweck

Diese Spezifikation definiert semantische Versionsbedingungen für NovaOS-Komponenten, Capabilities, Schemas und Semantic Types.

Ziel ist, Versionen maschinenlesbar vergleichen und zulässige Kompatibilitätsbereiche ausdrücken zu können.

## Grundprinzip

```text
Required Version Constraint
    +
Available Version
    ↓
Version Check
    ↓
MATCH / NO_MATCH
```

## Versionsschema

Versionen sollen mindestens folgendes Schema unterstützen:

```text
MAJOR.MINOR.PATCH
```

Beispiel:

```text
3.2.1
```

Bedeutung:

```text
MAJOR:
    inkompatible semantische Änderung

MINOR:
    kompatible Erweiterung

PATCH:
    kompatible Korrektur
```

Diese Bedeutung gilt nur, wenn die jeweilige Komponente semantische Versionierung korrekt verwendet.

## Constraints

Versionsbedingungen müssen mindestens ausdrücken können:

```text
= 2.1.0
>= 2.0.0
< 3.0.0
>= 2.1.0 && < 3.0.0
```

Beispiel:

```text
requires:
    Media.Audio >= 2.0.0 && < 3.0.0
```

## Exakte Bindung

Falls eine konkrete Version zwingend benötigt wird, darf sie exakt gebunden werden.

```text
requires:
    simulation.schema = 4.2.1
```

Exakte Bindungen sollen nur verwendet werden, wenn semantische Kompatibilität mit anderen Versionen nicht garantiert werden kann.

## Pre-Release-Versionen

Vorabversionen müssen eindeutig von stabilen Versionen unterscheidbar sein.

Beispiel:

```text
3.0.0-alpha
3.0.0-beta.2
3.0.0-rc.1
```

Sie dürfen nicht automatisch als gleichwertig mit einer stabilen Version behandelt werden.

## Semantik vor Versionsnummer

Eine passende Versionsnummer garantiert nicht automatisch tatsächliche Kompatibilität.

```text
Version Constraint:
    MATCH

Semantic Compatibility:
    FAILED
```

Die endgültige Kompatibilitätsentscheidung erfolgt daher zusätzlich über Compatibility Descriptor und Validation.

## Beispiel

```text
Available:
    nova.audio.decoder@2.4.1

Required:
    >= 2.1.0 && < 3.0.0
```

Ergebnis:

```text
MATCH
```

Dagegen:

```text
Available:
    3.0.0
```

Ergebnis:

```text
NO_MATCH
```

## Normative Anforderungen

1. NovaOS MUSS maschinenlesbare Versionsbedingungen unterstützen.
2. Exakte Versionen und Versionsbereiche MÜSSEN ausdrückbar sein.
3. MAJOR-, MINOR- und PATCH-Versionen MÜSSEN getrennt vergleichbar sein.
4. Pre-Release-Versionen MÜSSEN eindeutig erkennbar sein.
5. Eine erfüllte Versionsbedingung DARF nicht automatisch als vollständiger Kompatibilitätsnachweis gelten.
6. Zwingende exakte Versionsbindungen MÜSSEN explizit gekennzeichnet sein.
7. Versionsvergleiche MÜSSEN deterministisch sein.

## Abgrenzung

Diese NPSPEC definiert:

- Versionsschema
- Versionsbereiche
- exakte Bindungen
- Pre-Release-Versionen

Nicht Bestandteil sind:

- Compatibility Graph
- Adapterauflösung
- vollständige semantische Kompatibilitätsprüfung
- Versionsmigration

## Zugehörige NPSPECs

- `NPSPEC-COMPAT-0001 – Compatibility Descriptor`
- `NPSPEC-COMPAT-0003 – Compatibility Graph Model`
- `NPSPEC-COMPAT-0004 – Adapter & Transformation Nodes`
- `NPSPEC-COMPAT-0005 – Compatibility Path Resolution`
- `NPSPEC-COMPAT-0006 – Compatibility Validation`
- `NPSPEC-COMPAT-0007 – Compatibility Trust & Security`