# NPSPEC-INFOFLOW-0006 – Declassification & Sanitization

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie Information Labels kontrolliert abgeschwächt oder Daten so bereinigt werden können, dass bestimmte Schutzbeschränkungen entfallen dürfen.

## Grundprinzip

```text
Protected Information
    ↓
Authorized Transformation
    ↓
Verification
    ↓
Declassified / Sanitized Information
```

Eine normale Datenverarbeitung darf Schutzlabels nicht entfernen.

## Declassification

`Declassification` bedeutet die autorisierte Abschwächung einer bestehenden Informationsklassifikation.

Beispiel:

```text
confidential
    ↓ authorized declassification
internal
```

Eine Declassification benötigt eine explizite Policy oder Berechtigung.

## Sanitization

`Sanitization` entfernt oder verändert schützenswerte Inhalte, sodass ein weniger restriktiver Informationsfluss zulässig werden kann.

Beispiel:

```text
Document {
    name
    address
    statistics
}
    ↓ sanitize
Document {
    statistics
}
```

Nach erfolgreicher Sanitization dürfen betroffene Labels neu bewertet werden.

## Autorisierung

Declassification und Sanitization dürfen nur durch ausdrücklich dafür autorisierte Capabilities oder Systemmechanismen erfolgen.

Beispiel:

```text
Capability:
    document.anonymize

Permission:
    information.declassify
```

## Verifikation

Vor einer Label-Abschwächung muss NovaOS prüfen, ob die notwendige Transformation erfolgreich durchgeführt wurde.

Mögliche Ergebnisse:

```text
VERIFIED
FAILED
UNCERTAIN
```

`UNCERTAIN` darf nicht automatisch als erfolgreiche Sanitization gelten.

## Teilweise Declassification

Nur betroffene Labels dürfen entfernt oder abgeschwächt werden.

Beispiel:

```text
labels {
    confidential
    project_alpha
    local_only
}
```

Nach erlaubter Declassification von `confidential`:

```text
labels {
    project_alpha
    local_only
}
```

Andere Einschränkungen bleiben erhalten.

## Nachvollziehbarkeit

Sicherheitsrelevante Declassification soll nachvollziehbar bleiben.

Mindestens referenzierbar sein sollen:

```text
source
operation
authorization
result
```

## Beispiel

```text
Input:
    customer_dataset

Labels:
    confidential
    personal_data

Operation:
    anonymize

Verification:
    VERIFIED
```

Ergebnis:

```text
Output:
    anonymized_dataset

Labels:
    confidential
```

Das Label `personal_data` darf entfernt werden, wenn die entsprechende Policy dies erlaubt.

## Normative Anforderungen

1. Information Labels DÜRFEN nur durch autorisierte Declassification oder Sanitization abgeschwächt werden.
2. Sanitization MUSS vor einer daraus resultierenden Label-Abschwächung verifizierbar sein.
3. `FAILED` oder `UNCERTAIN` DÜRFEN nicht als erfolgreiche Sanitization behandelt werden.
4. Nicht betroffene Labels MÜSSEN erhalten bleiben.
5. Declassification MUSS mit den geltenden Information-Flow-Policies vereinbar sein.
6. Sicherheitsrelevante Declassification SOLL nachvollziehbar protokolliert werden.
7. Normale Capabilities DÜRFEN Schutzlabels nicht eigenständig entfernen.

## Abgrenzung

Diese NPSPEC definiert:

- Declassification
- Sanitization
- autorisierte Label-Abschwächung
- Verifikation von Sanitization

Nicht Bestandteil sind:

- allgemeine Label Propagation
- Source Classification
- Sink Control
- konkrete Anonymisierungsalgorithmen

## Zugehörige NPSPECs

- `NPSPEC-INFOFLOW-0001 – Information Labels`
- `NPSPEC-INFOFLOW-0002 – Label Propagation`
- `NPSPEC-INFOFLOW-0003 – Source Classification`
- `NPSPEC-INFOFLOW-0004 – Sink Control`
- `NPSPEC-INFOFLOW-0005 – Information Flow Policy`
- `NPSPEC-INFOFLOW-0007 – IPC & Cross-Process Flow Tracking`
- `NPSPEC-INFOFLOW-0008 – Persistent Data Flow Metadata`