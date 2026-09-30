# NPSPEC-INFOFLOW-0008 – Persistent Data Flow Metadata

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie Information-Flow-Metadaten dauerhaft zusammen mit Informationen gespeichert werden.

Ziel ist, dass Schutzklassifikation und relevante Herkunft auch nach Speicherung, Neustart, Kopieren oder Migration erhalten bleiben.

## Grundprinzip

```text
Information
    +
Flow Metadata
    ↓
Persistent Storage
    ↓
Reload
    ↓
Information + Flow Metadata
```

## Persistente Metadaten

Zu einer Information können mindestens gespeichert werden:

```text
FlowMetadata {
    labels
    origin
    owner
    policy_references
    provenance_reference
}
```

Nicht jede Information muss alle Felder besitzen.

## Bindung

Flow-Metadaten müssen logisch an die zugehörige Information gebunden bleiben.

Dies gilt auch bei:

```text
save
copy
move
archive
TaskCapsule migration
```

Ein Wechsel des Speicherorts darf Schutzinformationen nicht automatisch entfernen.

## Integrität

Flow-Metadaten müssen gegen unautorisierte Änderung geschützt werden.

NovaOS muss erkennen können, wenn:

```text
labels removed
metadata modified
metadata corrupted
```

wurden.

## Fehlende Metadaten

Werden Daten geladen, deren erwartete Flow-Metadaten fehlen oder nicht verifiziert werden können, darf NovaOS sie nicht automatisch als unklassifiziert oder öffentlich behandeln.

Mögliche Reaktion:

```text
RECLASSIFY
RESTRICT
BLOCK
```

## Kopien

Eine Kopie einer Information übernimmt grundsätzlich deren relevante Flow-Metadaten.

```text
Object A:
    confidential
        ↓ copy
Object B:
    confidential
```

Eine autorisierte Declassification bleibt davon getrennt.

## Externe Formate

Kann ein externes Dateiformat die Flow-Metadaten nicht selbst aufnehmen, darf NovaOS alternative Speicherung verwenden.

Beispiele:

```text
filesystem metadata
NovaFile metadata
sidecar metadata
system metadata store
```

Die logische Bindung zur Information muss erhalten bleiben.

## Lebensdauer

Flow-Metadaten bleiben mindestens so lange erhalten wie die zugehörige Information, sofern keine autorisierte Änderung oder Löschung erfolgt.

Gelöschte Daten dürfen keine dauerhaft verwaisten sicherheitsrelevanten Metadaten hinterlassen müssen.

## Beispiel

```text
object:report:42 {
    semantic_type:
        Document.Report

    flow_metadata {
        labels {
            confidential
            local_only
        }

        origin:
            object:dataset:17
    }
}
```

Nach Neustart oder Migration müssen diese Labels weiterhin verfügbar sein.

## Normative Anforderungen

1. Information-Flow-Metadaten MÜSSEN persistent speicherbar sein.
2. Persistente Labels MÜSSEN logisch an die zugehörige Information gebunden bleiben.
3. Kopieren, Verschieben oder Migration DÜRFEN relevante Labels nicht automatisch entfernen.
4. Flow-Metadaten MÜSSEN gegen unautorisierte Änderung prüfbar sein.
5. Fehlende oder beschädigte Metadaten DÜRFEN nicht automatisch als fehlende Schutzanforderung interpretiert werden.
6. Externe Formate ohne eigene Metadatenunterstützung MÜSSEN durch alternative persistente Zuordnung unterstützt werden können.
7. Autorisierte Änderungen MÜSSEN von normaler Metadatenpersistenz unterscheidbar bleiben.

## Abgrenzung

Diese NPSPEC definiert:

- persistente Information-Flow-Metadaten
- Bindung an Informationen
- Erhaltung bei Speicherung und Migration
- Verhalten bei fehlenden Metadaten

Nicht Bestandteil sind:

- Label Propagation
- Source Classification
- Declassification
- konkrete Dateisystem-Metadatenformate

## Zugehörige NPSPECs

- `NPSPEC-INFOFLOW-0001 – Information Labels`
- `NPSPEC-INFOFLOW-0002 – Label Propagation`
- `NPSPEC-INFOFLOW-0003 – Source Classification`
- `NPSPEC-INFOFLOW-0004 – Sink Control`
- `NPSPEC-INFOFLOW-0005 – Information Flow Policy`
- `NPSPEC-INFOFLOW-0006 – Declassification & Sanitization`
- `NPSPEC-INFOFLOW-0007 – IPC & Cross-Process Flow Tracking`
- `NPSPEC-CAPSULE-0004 – Resource Packaging & References`