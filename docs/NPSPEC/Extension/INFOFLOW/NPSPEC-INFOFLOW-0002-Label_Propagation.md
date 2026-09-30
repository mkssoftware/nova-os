# NPSPEC-INFOFLOW-0002 – Label Propagation

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie Information Labels bei Verarbeitung, Kopieren, Umwandlung und Zusammenführung von Daten weitergegeben werden.

Ziel ist, dass Schutzinformationen nicht verloren gehen, wenn neue Daten aus bereits klassifizierten Informationen entstehen.

## Grundprinzip

```text
Input + Label
    ↓
Processing
    ↓
Output + abgeleitetes Label
```

Eine Capability darf relevante Schutzinformationen nicht unbeabsichtigt entfernen.

## Propagation

Bei direkter Verarbeitung werden relevante Labels grundsätzlich übernommen.

Beispiel:

```text
Document.Report
    label: confidential
        ↓ summarize
Document.Summary
    label: confidential
```

## Zusammenführung

Werden mehrere Informationsquellen kombiniert, muss das Ergebnis die notwendigen Einschränkungen aller relevanten Inputs berücksichtigen.

Beispiel:

```text
Input A:
    internal

Input B:
    confidential
```

Ergebnis:

```text
internal
confidential
```

Die konkrete Policy entscheidet, wie kombinierte Labels interpretiert werden.

## Ableitungen

Auch indirekt erzeugte Informationen können Labels übernehmen müssen.

Beispiele:

```text
document → summary
audio → transcript
dataset → analysis
image → extracted text
```

Entscheidend ist der Informationsfluss, nicht das Dateiformat.

## Transformationen

Eine Formatänderung entfernt keine Schutzklassifikation.

```text
Document.PDF
    ↓ convert
Document.Text
```

Relevante Labels bleiben erhalten.

## Kontrollierte Änderung

Labels dürfen nur durch ausdrücklich autorisierte Mechanismen verändert oder entfernt werden.

Beispiele:

```text
declassification
sanitization
authorized reclassification
```

Eine normale Capability darf solche Änderungen nicht eigenständig durchführen.

## Fehlende Propagation-Regel

Ist NovaOS unsicher, wie ein Label weitergegeben werden muss, gilt die restriktivere Behandlung.

```text
unknown propagation
    ↓
preserve restriction
```

## Beispiel

```text
Audio {
    label:
        private
}
    ↓
SpeechRecognition
    ↓
Transcript {
    label:
        private
}
```

Der Wechsel von Audio zu Text verändert nicht automatisch die Schutzanforderung.

## Normative Anforderungen

1. Relevante Information Labels MÜSSEN bei Datenverarbeitung propagiert werden.
2. Format- oder Semantic-Type-Änderungen DÜRFEN Labels nicht automatisch entfernen.
3. Bei Datenzusammenführung MÜSSEN die Einschränkungen aller relevanten Inputs berücksichtigt werden.
4. Abgeleitete Informationen MÜSSEN abhängig vom Informationsfluss passende Labels erhalten.
5. Nicht autorisierte Capabilities DÜRFEN Labels nicht abschwächen oder entfernen.
6. Bei unklarer Propagation MUSS die restriktivere Behandlung gewählt werden.
7. Autorisierte Label-Änderungen MÜSSEN von normaler Propagation unterscheidbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Label Propagation
- Label-Vererbung
- Zusammenführung von Labels
- Verhalten bei Transformationen

Nicht Bestandteil sind:

- Source Classification
- Sink Control
- allgemeine Information-Flow-Policies
- Declassification und Sanitization

## Zugehörige NPSPECs

- `NPSPEC-INFOFLOW-0001 – Information Labels`
- `NPSPEC-INFOFLOW-0003 – Source Classification`
- `NPSPEC-INFOFLOW-0004 – Sink Control`
- `NPSPEC-INFOFLOW-0005 – Information Flow Policy`
- `NPSPEC-INFOFLOW-0006 – Declassification & Sanitization`
- `NPSPEC-INFOFLOW-0007 – IPC & Cross-Process Flow Tracking`
- `NPSPEC-INFOFLOW-0008 – Persistent Data Flow Metadata`