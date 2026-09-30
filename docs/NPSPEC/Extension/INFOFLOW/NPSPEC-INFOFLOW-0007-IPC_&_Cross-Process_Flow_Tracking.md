# NPSPEC-INFOFLOW-0007 – IPC & Cross-Process Flow Tracking

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie NovaOS Informationsflüsse über Prozess- und IPC-Grenzen hinweg verfolgt.

Ziel ist, dass Information Labels auch bei Kommunikation zwischen Prozessen, Capabilities und Diensten erhalten und kontrollierbar bleiben.

## Grundprinzip

```text
Process A
    ↓
IPC
    ↓
Process B
```

Die Prozessgrenze darf Information-Flow-Metadaten nicht verlieren.

## IPC-Übertragung

Bei einer IPC-Nachricht müssen relevante Labels zusammen mit den Daten übertragen oder eindeutig referenziert werden.

Beispiel:

```text
Message {
    payload
    information_labels
}
```

Die konkrete Transportrepräsentation bleibt dem IPC-System überlassen.

## Flow Tracking

NovaOS muss nachvollziehen können:

```text
source
sender
receiver
data_object
labels
transfer
```

Dabei wird der logische Informationsfluss verfolgt, nicht nur der technische Speichertransfer.

## Gemeinsamer Speicher

Auch bei Shared Memory müssen Information-Flow-Regeln gelten.

```text
Process A
    ↓
Shared Object
    ↓
Process B
```

Das gemeinsame Objekt behält seine Labels unabhängig davon, welcher Prozess darauf zugreift.

## Transformation

Verarbeitet ein empfangender Prozess die Daten weiter, gelten die Regeln aus `Label Propagation`.

Beispiel:

```text
confidential audio
    ↓ IPC
Speech Process
    ↓ transcription
confidential transcript
```

## Prozessübergreifende Policies

Vor einer Übertragung muss geprüft werden, ob der empfangende Prozess oder Capability-Kontext die Information erhalten darf.

Beispiel:

```text
Data:
    restricted

Receiver:
    untrusted
```

Policy:

```text
DENY
```

## Indirekte Weitergabe

Weiterleitungen über mehrere Prozesse müssen berücksichtigt werden können.

```text
Process A
    ↓
Process B
    ↓
Process C
```

Ein erlaubter Transfer von A nach B bedeutet nicht automatisch, dass B die Information an C weitergeben darf.

## Prozessende

Das Ende eines Prozesses darf persistente Information-Flow-Metadaten nicht zerstören.

Labels gehören zur Information, nicht zum Prozess.

## Beispiel

```text
Process A:
    reads confidential document

    ↓ IPC

Process B:
    creates summary

    ↓ IPC

Process C:
    attempts external upload
```

Die ursprüngliche Klassifikation bleibt erhalten.

```text
confidential
    ↓
external upload
    ↓
DENY
```

## Normative Anforderungen

1. IPC MUSS relevante Information Labels erhalten können.
2. Prozessgrenzen DÜRFEN Information-Flow-Klassifikationen nicht entfernen.
3. Sender und Empfänger MÜSSEN vor sicherheitsrelevanten Transfers policyprüfbar sein.
4. Shared-Memory-Kommunikation MUSS denselben Information-Flow-Regeln unterliegen wie Nachrichten-IPC.
5. Indirekte Weitergabe über mehrere Prozesse MUSS kontrollierbar sein.
6. Empfangene Daten MÜSSEN weiterhin der normalen Label Propagation unterliegen.
7. Prozessende DARF persistente Labels der zugrunde liegenden Informationen nicht entfernen.

## Abgrenzung

Diese NPSPEC definiert:

- Information Flow über IPC
- prozessübergreifende Label-Erhaltung
- Shared-Memory-Flows
- indirekte Weitergabe

Nicht Bestandteil sind:

- IPC-Protokolle
- allgemeine IPC-Sicherheit
- Label-Definition
- persistente Speicherung von Flow-Metadaten

## Zugehörige NPSPECs

- `NPSPEC-INFOFLOW-0001 – Information Labels`
- `NPSPEC-INFOFLOW-0002 – Label Propagation`
- `NPSPEC-INFOFLOW-0004 – Sink Control`
- `NPSPEC-INFOFLOW-0005 – Information Flow Policy`
- `NPSPEC-INFOFLOW-0006 – Declassification & Sanitization`
- `NPSPEC-INFOFLOW-0008 – Persistent Data Flow Metadata`