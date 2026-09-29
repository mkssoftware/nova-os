# NPSPEC-CAUSAL-0004 – Causality Propagation

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie kausale Herkunftsinformationen bei Transformationen, Ableitungen und Weiterverarbeitung automatisch fortgeführt werden.

Ziel ist, dass ein Ergebnis seine relevante Herkunft behält, ohne dass jede Capability den vollständigen Causality Graph selbst verwalten muss.

## Grundprinzip

```text
Input
    ↓
Operation
    ↓
Output
```

Besitzt der Input bereits Lineage, wird diese über die neue Operation erweitert.

Beispiel:

```text
Audio.raw
    ↓
NoiseReduction
    ↓
Audio.cleaned
    ↓
SpeechRecognition
    ↓
Transcript
```

Die Lineage des `Transcript` enthält dadurch mittelbar auch `Audio.raw`.

## Propagation

Bei einer kausal relevanten Operation erzeugt NovaOS Beziehungen zwischen:

```text
Inputs
Operation
Outputs
```

Beispiel:

```text
Input A ─┐
         ├─ Operation X ─→ Output C
Input B ─┘
```

Daraus entstehen mindestens:

```text
Operation X USED Input A
Operation X USED Input B
Operation X PRODUCED Output C
Output C DERIVED_FROM Input A
Output C DERIVED_FROM Input B
```

Transitive Beziehungen müssen nicht vollständig materialisiert gespeichert werden.

Sie dürfen aus dem Graph berechnet werden.

## Propagation-Arten

NovaOS muss mindestens unterscheiden zwischen:

```text
REFERENCE
COPY
TRANSFORM
COMBINE
EXTRACT
GENERATE
```

### `REFERENCE`

Das Objekt wird nur referenziert.

```text
Object A
    ↓ reference
Object A
```

Es entsteht keine neue Objektidentität.

### `COPY`

Ein neues Objekt enthält semantisch dieselben Daten.

```text
Object A
    ↓ copy
Object B
```

`Object B` erhält eine Lineage zu `Object A`.

### `TRANSFORM`

Ein Objekt wird verändert.

```text
Image.Raw
    ↓ encode
Image.PNG
```

Das Ergebnis erhält eine `DERIVED_FROM`-Beziehung zum Ursprung.

### `COMBINE`

Mehrere Quellen werden kombiniert.

```text
A ─┐
   ├─ combine ─→ C
B ─┘
```

Das Ergebnis erhält Lineage zu allen kausal relevanten Quellen.

### `EXTRACT`

Nur ein Teil eines Objekts wird verwendet.

```text
Video
    ↓ extract_audio
Audio
```

Die Lineage soll die verwendete Teilkomponente beschreiben können.

### `GENERATE`

Ein neues Objekt wird aus einem Intent, Modell, Algorithmus oder anderen Quellen erzeugt.

Beispiel:

```text
Prompt
Model
Parameters
    ↓
Generation
    ↓
Image
```

Alle tatsächlich relevanten Ursachen sollen referenzierbar bleiben.

## Relevanz

Nicht jeder gelesene Wert ist automatisch eine kausale Quelle.

Beispiel:

```text
Datei wird geöffnet
    ↓
Metadaten werden angezeigt
```

Eine lediglich technisch gelesene Information muss nicht zwingend Teil der Result Lineage werden.

Capabilities müssen deshalb kennzeichnen können, welche Inputs:

```text
causal
contextual
incidental
```

sind.

Nur kausal relevante Eingaben werden regulär propagiert.

## Mehrstufige Propagation

Propagation erfolgt schrittweise.

```text
A
↓
B
↓
C
↓
D
```

Die direkte Lineage von `D` kann nur `C` enthalten.

Die transitive Lineage ergibt:

```text
D
← C
← B
← A
```

Dadurch muss nicht bei jeder Operation die vollständige Herkunft kopiert werden.

## Parent- und Child-Intents

Kausale Informationen müssen über zusammengesetzte Intents hinweg erhalten bleiben.

Beispiel:

```text
Parent Intent
    ├── Child A → Result A
    └── Child B uses Result A → Result B
```

`Result B` behält die Lineage zu den relevanten Quellen von `Result A`.

## Fehler und Abbruch

Fehlgeschlagene Operationen dürfen Causal Events erzeugen, aber keine erfolgreichen Result-Beziehungen vortäuschen.

Beispiel:

```text
Operation
    ↓ Failed
```

Es darf kein:

```text
PRODUCED valid_result
```

entstehen, sofern kein gültiges Ergebnis bestätigt wurde.

Teilresultate dürfen separat markiert werden.

## Propagation-Grenzen

Propagation darf durch definierte Regeln begrenzt werden.

Beispiele:

- Datenschutz
- Retention
- Geheimhaltung
- externe Systemgrenzen
- Graph-Compaction

Eine Begrenzung darf nicht fälschlich behaupten, dass keine frühere Ursache existierte.

Stattdessen kann beispielsweise gespeichert werden:

```text
upstream_lineage:
    restricted
```

oder:

```text
upstream_lineage:
    compacted
```

## Beispiel

```text
object:recording
    ↓
operation:noise_reduction
    ↓
object:clean_audio
    ↓
operation:speech_recognition
    ↓
object:transcript
```

Direkte Beziehungen:

```text
clean_audio
    DERIVED_FROM recording

transcript
    DERIVED_FROM clean_audio
```

Transitiv:

```text
transcript
    DERIVED_FROM recording
```

## Normative Anforderungen

1. Kausal relevante Inputs MÜSSEN auf erzeugte Outputs propagiert werden können.
2. NovaOS MUSS zwischen Referenzierung, Kopie, Transformation, Kombination, Extraktion und Erzeugung unterscheiden können.
3. Mehrere kausale Inputs MÜSSEN gemeinsam propagiert werden können.
4. Transitive Lineage MUSS berechenbar sein, ohne vollständig materialisiert gespeichert werden zu müssen.
5. Nicht kausal relevante technische Zugriffe DÜRFEN nicht automatisch als Herkunft propagiert werden.
6. Fehlgeschlagene Operationen DÜRFEN keine erfolgreichen Result-Beziehungen erzeugen.
7. Propagation-Grenzen MÜSSEN erkennbar bleiben und dürfen fehlende Herkunft nicht vortäuschen.

## Abgrenzung

Diese NPSPEC definiert:

- automatische Weitergabe von Lineage
- Propagation-Arten
- kausale Relevanz
- mehrstufige Ableitung

Nicht Bestandteil sind:

- Causal Event Format
- Query-Schnittstellen
- Graph-Compaction
- Information-Flow-Labels
- Evidence-Bundles

## Zugehörige NPSPECs

- `NPSPEC-CAUSAL-0001 – Causality Graph Model`
- `NPSPEC-CAUSAL-0002 – Causal Event Records`
- `NPSPEC-CAUSAL-0003 – Object & Result Lineage`
- `NPSPEC-CAUSAL-0005 – Causal Query Interface`
- `NPSPEC-CAUSAL-0006 – Causal Integrity & Graph Compaction`
- `NPSPEC-INFOFLOW-0002 – Label Propagation`