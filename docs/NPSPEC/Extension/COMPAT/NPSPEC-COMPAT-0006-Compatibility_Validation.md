# NPSPEC-COMPAT-0006 – Compatibility Validation

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie NovaOS einen gefundenen Kompatibilitätspfad vor seiner Verwendung validiert.

Ziel ist sicherzustellen, dass der Pfad nicht nur theoretisch erreichbar, sondern unter den aktuellen Bedingungen tatsächlich verwendbar ist.

## Grundprinzip

```text
Compatibility Path
    ↓
Validation
    ↓
VALID / INVALID / CONDITIONAL
```

## Prüfbereiche

Die Validierung berücksichtigt mindestens:

```text
semantic compatibility
version constraints
adapter requirements
lossiness
policies
trust
resource requirements
```

## Pfadvalidierung

Alle Nodes und Übergänge eines Pfades müssen gültig sein.

```text
Source
    ↓ Adapter A
Intermediate
    ↓ Transformation B
Target
```

Ist ein zwingender Übergang ungültig, ist der gesamte Pfad ungültig.

## Ergebnis

Mindestens folgende Zustände müssen unterstützt werden:

```text
VALID
INVALID
CONDITIONAL
UNKNOWN
```

`CONDITIONAL` bedeutet, dass der Pfad nur unter zusätzlichen Bedingungen nutzbar ist.

Beispiele:

```text
required capability missing
authorization required
resource currently unavailable
```

## Lossy Validation

Enthält ein Pfad verlustbehaftete Transformationen, muss geprüft werden, ob der Informationsverlust mit dem Intent und seinen Constraints vereinbar ist.

```text
LOSSY
    +
precision required
    ↓
INVALID
```

falls die geforderte Qualität nicht mehr erreicht werden kann.

## Laufzeitbedingungen

Ein validierter Pfad kann ungültig werden, wenn sich relevante Bedingungen ändern.

Beispiele:

```text
capability removed
policy changed
trust changed
resource unavailable
version changed
```

In diesem Fall muss eine erneute Validation oder Path Resolution möglich sein.

## Beispiel

```text
Path:
    Schema@1
        ↓ Migration
    Schema@2
        ↓ Adapter
    Schema@3
```

Validation:

```text
Version Constraints:
    PASS

Semantic Compatibility:
    PASS

Lossiness:
    NONE

Trust:
    PASS
```

Ergebnis:

```text
VALID
```

## Normative Anforderungen

1. Jeder aufgelöste Kompatibilitätspfad MUSS vor Verwendung validierbar sein.
2. Alle Nodes und Übergänge des Pfades MÜSSEN berücksichtigt werden.
3. NovaOS MUSS mindestens `VALID`, `INVALID`, `CONDITIONAL` und `UNKNOWN` unterscheiden können.
4. Verlustbehaftete Transformationen MÜSSEN gegen Intent- und Qualitätsanforderungen geprüft werden.
5. Hard Constraints und Policies DÜRFEN durch einen Kompatibilitätspfad nicht verletzt werden.
6. Relevante Änderungen an Abhängigkeiten MÜSSEN eine erneute Validation ermöglichen.
7. Ein `UNKNOWN`-Ergebnis DARF nicht automatisch als gültige Kompatibilität behandelt werden.

## Abgrenzung

Diese NPSPEC definiert:

- Validierung von Kompatibilitätspfaden
- Validierungsergebnisse
- Prüfung von Lossiness und Constraints
- erneute Validierung bei Änderungen

Nicht Bestandteil sind:

- Compatibility Graph
- Pfadsuche
- Adapterausführung
- detailliertes Trust-Modell

## Zugehörige NPSPECs

- `NPSPEC-COMPAT-0001 – Compatibility Descriptor`
- `NPSPEC-COMPAT-0002 – Semantic Version Constraints`
- `NPSPEC-COMPAT-0003 – Compatibility Graph Model`
- `NPSPEC-COMPAT-0004 – Adapter & Transformation Nodes`
- `NPSPEC-COMPAT-0005 – Compatibility Path Resolution`
- `NPSPEC-COMPAT-0007 – Compatibility Trust & Security`