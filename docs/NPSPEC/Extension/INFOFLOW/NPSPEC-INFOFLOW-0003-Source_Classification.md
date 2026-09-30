# NPSPEC-INFOFLOW-0003 – Source Classification

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie eingehende Informationen beim Eintritt in NovaOS klassifiziert und mit passenden Information Labels versehen werden.

Ziel ist, Schutzanforderungen möglichst früh festzulegen.

## Grundprinzip

```text
Information Source
    ↓
Source Classification
    ↓
Information Labels
    ↓
weitere Verarbeitung
```

## Informationsquellen

Quellen können beispielsweise sein:

```text
file
user_input
sensor
network
device
database
remote_service
generated_data
```

Jede Quelle darf eigene Klassifikationsregeln besitzen.

## Klassifikation

Die Klassifikation kann anhand von:

- Herkunft
- Semantic Type
- Besitzer
- Sicherheitskontext
- vorhandenen Metadaten
- System- oder Benutzer-Policies

erfolgen.

Beispiel:

```text
Source:
    microphone

Context:
    private_session

Result:
    private
```

## Vorhandene Labels

Bereits vorhandene gültige Labels müssen berücksichtigt werden.

```text
Imported Object:
    confidential
```

darf durch einen Import nicht automatisch zu:

```text
public
```

werden.

## Vertrauenswürdigkeit

Labels externer oder nicht vertrauenswürdiger Quellen dürfen nicht ungeprüft als verbindlich übernommen werden.

NovaOS darf sie abhängig von der Policy:

```text
accept
augment
override
restrict
```

## Standardklassifikation

Kann keine eindeutige Klassifikation bestimmt werden, muss eine sichere Standardregel angewendet werden.

```text
unknown source
    ↓
policy default
```

Die konkrete Default-Klasse wird durch die Information-Flow-Policy festgelegt.

## Reklassifikation

Eine spätere Reklassifikation ist zulässig, wenn neue Informationen verfügbar werden.

Eine Abschwächung bestehender Schutzanforderungen erfordert eine autorisierte Declassification oder Reklassifikation.

## Beispiel

```text
Source:
    external USB device

Semantic Type:
    Document.Report

Existing Label:
    internal

Policy:
    external_source → restricted
```

Ergebnis:

```text
labels {
    internal
    restricted
}
```

## Normative Anforderungen

1. Neue Informationsquellen MÜSSEN klassifizierbar sein.
2. Source Classification MUSS vor sicherheitsrelevanter Weiterverarbeitung möglich sein.
3. Vorhandene gültige Labels MÜSSEN berücksichtigt werden.
4. Labels nicht vertrauenswürdiger Quellen MÜSSEN durch lokale Policies überprüfbar sein.
5. Für nicht eindeutig klassifizierbare Quellen MUSS eine sichere Default-Regel existieren.
6. Eine spätere Abschwächung von Labels MUSS autorisiert erfolgen.
7. Die Klassifikation MUSS unabhängig vom konkreten Dateiformat möglich sein.

## Abgrenzung

Diese NPSPEC definiert:

- Klassifikation eingehender Informationen
- Source-basierte Labels
- Umgang mit bestehenden Labels
- Default-Klassifikation

Nicht Bestandteil sind:

- Label Propagation
- Sink Control
- Declassification
- allgemeine Information-Flow-Policies

## Zugehörige NPSPECs

- `NPSPEC-INFOFLOW-0001 – Information Labels`
- `NPSPEC-INFOFLOW-0002 – Label Propagation`
- `NPSPEC-INFOFLOW-0004 – Sink Control`
- `NPSPEC-INFOFLOW-0005 – Information Flow Policy`
- `NPSPEC-INFOFLOW-0006 – Declassification & Sanitization`
- `NPSPEC-INFOFLOW-0007 – IPC & Cross-Process Flow Tracking`
- `NPSPEC-INFOFLOW-0008 – Persistent Data Flow Metadata`