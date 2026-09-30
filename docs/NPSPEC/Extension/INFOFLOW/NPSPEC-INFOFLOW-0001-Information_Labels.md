# NPSPEC-INFOFLOW-0001 – Information Labels

## Status

Angenommen

## Zweck

Diese Spezifikation definiert Information Labels für Daten innerhalb von NovaOS.

Labels beschreiben sicherheits- und datenflussrelevante Eigenschaften einer Information und bilden die Grundlage für systemweite Information-Flow-Regeln.

## Grundprinzip

```text
Information
    +
Information Label
    ↓
Policy Enforcement
```

Das Label gehört logisch zur Information und bleibt unabhängig von Anwendung, Prozess oder Speicherort erhalten.

## Label-Modell

Ein Label kann logisch enthalten:

```text
InformationLabel {
    classification
    owner
    origin
    restrictions
}
```

Beispiel:

```text
classification:
    confidential

restrictions:
    local_only
```

## Klassifikation

NovaOS muss unterschiedliche Schutzklassen unterstützen können.

Beispiele:

```text
public
internal
private
confidential
restricted
```

Die konkreten Klassen dürfen durch Policies erweitert werden.

## Mehrere Labels

Eine Information darf mehrere gleichzeitig gültige Labels besitzen.

Beispiel:

```text
private
project_alpha
local_only
```

Die Kombination aller Labels bestimmt die zulässigen Datenflüsse.

## Bindung an Information

Labels müssen an die logische Information gebunden sein.

Sie dürfen daher auch bei:

```text
copy
move
serialization
IPC
TaskCapsule migration
```

erhalten bleiben.

## Vererbung

Aus geschützten Daten erzeugte Informationen müssen relevante Labels übernehmen können.

Beispiel:

```text
Confidential Document
    ↓ summarize
Confidential Summary
```

Die genauen Regeln für die Weitergabe definiert `NPSPEC-INFOFLOW-0002`.

## Labels und Semantic Types

Information Labels und Semantic Types sind getrennte Konzepte.

```text
Semantic Type:
    Was ist die Information?

Information Label:
    Wie darf sie verwendet werden?
```

Beispiel:

```text
Semantic Type:
    Document.Transcript

Label:
    confidential
```

## Integrität

Labels müssen gegen unautorisierte Änderung geschützt sein.

Eine Capability darf ein Label nicht eigenständig entfernen oder abschwächen.

## Beispiel

```text
object:document:42 {
    semantic_type:
        Document.Report

    labels {
        confidential
        local_only
    }
}
```

Damit kann NovaOS beispielsweise verhindern, dass das Dokument an einen externen Dienst übertragen wird.

## Normative Anforderungen

1. Informationsobjekte MÜSSEN Information Labels tragen können.
2. Mehrere Labels MÜSSEN gleichzeitig unterstützt werden.
3. Labels MÜSSEN unabhängig vom konkreten Prozess oder Speicherort erhalten bleiben können.
4. Labels und Semantic Types MÜSSEN getrennt behandelt werden.
5. Abgeleitete Informationen MÜSSEN relevante Labels übernehmen können.
6. Unautorisierte Entfernung oder Abschwächung von Labels MUSS verhindert werden.
7. Information-Flow-Policies MÜSSEN Labels als Entscheidungsgrundlage verwenden können.

## Abgrenzung

Diese NPSPEC definiert:

- Information Labels
- Klassifikation
- Label-Bindung
- grundlegende Vererbung

Nicht Bestandteil sind:

- Label Propagation im Detail
- Source Classification
- Sink Control
- Declassification

## Zugehörige NPSPECs

- `NPSPEC-INFOFLOW-0002 – Label Propagation`
- `NPSPEC-INFOFLOW-0003 – Source Classification`
- `NPSPEC-INFOFLOW-0004 – Sink Control`
- `NPSPEC-INFOFLOW-0005 – Information Flow Policy`
- `NPSPEC-INFOFLOW-0006 – Declassification & Sanitization`
- `NPSPEC-INFOFLOW-0007 – IPC & Cross-Process Flow Tracking`
- `NPSPEC-INFOFLOW-0008 – Persistent Data Flow Metadata`