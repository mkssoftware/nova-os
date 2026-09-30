# NPSPEC-COMPAT-0005 – Compatibility Path Resolution

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie NovaOS im `Compatibility Graph` einen gültigen Pfad zwischen nicht direkt kompatiblen Komponenten, Versionen oder Semantic Types findet.

## Grundprinzip

```text
Source
    ↓
Compatibility Graph
    ↓
Path Resolution
    ↓
Target
```

Ein Pfad kann direkte Kompatibilität oder mehrere Adapter, Transformationen und Migrationen enthalten.

## Pfadsuche

Ausgangspunkt und Ziel müssen eindeutig beschrieben sein.

Beispiel:

```text
Source:
    Document.Schema@1

Target:
    Document.Schema@3
```

Möglicher Pfad:

```text
Schema@1
    ↓ Migration
Schema@2
    ↓ Migration
Schema@3
```

## Kandidaten

Es dürfen mehrere gültige Pfade existieren.

```text
Path A:
    direct adapter

Path B:
    adapter → transformation

Path C:
    migration → adapter
```

Ungültige Pfade müssen verworfen werden.

## Pfadbedingungen

Bei der Auflösung müssen mindestens berücksichtigt werden:

```text
semantic compatibility
version constraints
requirements
lossiness
policies
trust
```

Ein technisch erreichbarer Pfad ist nicht automatisch zulässig.

## Pfadbewertung

Mehrere gültige Pfade dürfen anhand von Eigenschaften verglichen werden.

Beispiele:

```text
number_of_steps
cost
latency
lossiness
resource_usage
```

Hard Constraints haben Vorrang vor Optimierungspräferenzen.

## Zyklen

Die Pfadsuche muss unbeabsichtigte Endlosschleifen verhindern.

Bereits besuchte Zustände oder definierte Suchgrenzen müssen berücksichtigt werden.

## Kein gültiger Pfad

Kann kein zulässiger Pfad gefunden werden, lautet das Ergebnis:

```text
NO_COMPATIBLE_PATH
```

NovaOS darf in diesem Fall keine nicht verifizierte Transformation erzwingen.

## Beispiel

```text
Media.Audio@1
    ↓ Adapter
Media.Audio@2
    ↓ Transformation
Media.Audio@3
```

Wenn beide Übergänge gültig sind, kann der gesamte Pfad als Kandidat verwendet werden.

## Normative Anforderungen

1. NovaOS MUSS mehrstufige Kompatibilitätspfade auflösen können.
2. Versions-, Semantic-Type- und Node-Anforderungen MÜSSEN bei der Pfadsuche berücksichtigt werden.
3. Ungültige oder nicht zulässige Pfade MÜSSEN ausgeschlossen werden.
4. Mehrere gültige Pfade MÜSSEN unterstützt werden können.
5. Hard Constraints DÜRFEN durch Pfadoptimierung nicht verletzt werden.
6. Die Pfadsuche MUSS gegen unbeabsichtigte Zyklen und unbeschränkte Suche geschützt sein.
7. Fehlt ein gültiger Pfad, MUSS dies eindeutig erkennbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Compatibility Path Resolution
- Kandidatenpfade
- Pfadbedingungen
- grundlegende Pfadbewertung

Nicht Bestandteil sind:

- Definition der Adapter
- eigentliche Transformation
- abschließende Compatibility Validation
- Trust-Modell im Detail

## Zugehörige NPSPECs

- `NPSPEC-COMPAT-0001 – Compatibility Descriptor`
- `NPSPEC-COMPAT-0002 – Semantic Version Constraints`
- `NPSPEC-COMPAT-0003 – Compatibility Graph Model`
- `NPSPEC-COMPAT-0004 – Adapter & Transformation Nodes`
- `NPSPEC-COMPAT-0006 – Compatibility Validation`
- `NPSPEC-COMPAT-0007 – Compatibility Trust & Security`