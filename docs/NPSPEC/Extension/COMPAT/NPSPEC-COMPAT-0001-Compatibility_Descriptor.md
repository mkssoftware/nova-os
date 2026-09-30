# NPSPEC-COMPAT-0001 – Compatibility Descriptor

## Status

Angenommen

## Zweck

Diese Spezifikation definiert den `Compatibility Descriptor` von NovaOS.

Er beschreibt maschinenlesbar, unter welchen Bedingungen eine Komponente, Capability, Ressource oder Datenstruktur mit anderen Systembestandteilen kompatibel ist.

## Grundprinzip

```text
Component A
    +
Compatibility Descriptor
    ↓
Compatibility Check
    ↓
COMPATIBLE / ADAPTABLE / INCOMPATIBLE
```

## Descriptor

Ein Compatibility Descriptor kann enthalten:

```text
CompatibilityDescriptor {
    identity
    version
    interfaces
    semantic_types
    requirements
    constraints
}
```

## Identität und Version

Die beschriebene Komponente muss eindeutig identifizierbar sein.

Beispiel:

```text
identity:
    nova.media.decoder

version:
    3
```

Kompatibilität darf jedoch nicht ausschließlich anhand einer Versionsnummer entschieden werden.

## Interfaces

Der Descriptor beschreibt relevante Schnittstellen.

Beispiel:

```text
provides:
    Media.Audio.Decode

requires:
    Media.Container.Read
```

Dadurch kann NovaOS prüfen, ob benötigte Funktionen vorhanden sind.

## Semantic Types

Unterstützte Ein- und Ausgabetypen müssen beschreibbar sein.

```text
accepts:
    Media.Audio@2

produces:
    Media.PCM@1
```

Kompatibilität hängt dabei von der tatsächlichen Typsemantik ab.

## Anforderungen

Zusätzliche Voraussetzungen dürfen angegeben werden.

Beispiele:

```text
architecture:
    x86

execution_ir:
    >= 2

memory:
    >= 128MiB
```

Nicht erfüllte zwingende Anforderungen verhindern direkte Kompatibilität.

## Kompatibilitätsstatus

Mindestens folgende Zustände müssen unterscheidbar sein:

```text
COMPATIBLE
ADAPTABLE
INCOMPATIBLE
UNKNOWN
```

### `COMPATIBLE`

Direkte Nutzung ist möglich.

### `ADAPTABLE`

Kompatibilität kann durch einen bekannten Adapter oder eine Transformation hergestellt werden.

### `INCOMPATIBLE`

Die Anforderungen können nicht erfüllt werden.

### `UNKNOWN`

Es liegen nicht genügend Informationen für eine sichere Entscheidung vor.

## Adapter

Ein Descriptor darf auf mögliche Adapter hinweisen.

Beispiel:

```text
Media.Audio@1
    ↓ adapter
Media.Audio@2
```

Die Auswahl und Verkettung solcher Adapter wird durch den Compatibility Graph behandelt.

## Beispiel

```text
CompatibilityDescriptor {
    identity:
        nova.audio.decoder

    version:
        3

    accepts:
        Media.Audio.Container@2

    produces:
        Media.Audio.PCM@1

    requirements {
        architecture:
            x86_or_x64
    }
}
```

## Normative Anforderungen

1. Kompatibilitätsrelevante Komponenten MÜSSEN einen maschinenlesbaren Descriptor besitzen können.
2. Identität und Version MÜSSEN eindeutig referenzierbar sein.
3. Interfaces und Semantic Types MÜSSEN getrennt beschreibbar sein.
4. Zwingende Anforderungen MÜSSEN als solche erkennbar sein.
5. NovaOS MUSS mindestens `COMPATIBLE`, `ADAPTABLE`, `INCOMPATIBLE` und `UNKNOWN` unterscheiden können.
6. Versionsnummern DÜRFEN nicht die alleinige Grundlage einer Kompatibilitätsentscheidung sein.
7. Mögliche Adapter MÜSSEN referenzierbar sein können.

## Abgrenzung

Diese NPSPEC definiert:

- Compatibility Descriptor
- Schnittstellen- und Typanforderungen
- Kompatibilitätsstatus
- Adapterreferenzen

Nicht Bestandteil sind:

- Versionsauflösung
- Compatibility Graph
- Adapterausführung
- Pfadsuche

## Zugehörige NPSPECs

- `NPSPEC-COMPAT-0002 – Semantic Version Constraints`
- `NPSPEC-COMPAT-0003 – Compatibility Graph Model`
- `NPSPEC-COMPAT-0004 – Adapter & Transformation Nodes`
- `NPSPEC-COMPAT-0005 – Compatibility Path Resolution`
- `NPSPEC-COMPAT-0006 – Compatibility Validation`
- `NPSPEC-COMPAT-0007 – Compatibility Trust & Security`
- `NPSPEC-CAPSULE-0007 – Capsule Versioning & Compatibility`