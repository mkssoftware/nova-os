# NPSPEC-INFOFLOW-0004 – Sink Control

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie NovaOS kontrolliert, an welche Ziele Informationen übertragen oder ausgegeben werden dürfen.

Ein `Sink` ist jedes Ziel, an das Daten das aktuelle Schutzgebiet verlassen oder dauerhaft geschrieben werden können.

## Grundprinzip

```text
Information + Labels
        ↓
     Sink
        ↓
Policy Check
        ↓
ALLOW / DENY / REQUIRE_ACTION
```

## Sink-Typen

Typische Sinks sind:

```text
file
network
clipboard
display
printer
device
IPC
remote_service
external_storage
```

Jeder Sink muss anhand seiner sicherheitsrelevanten Eigenschaften beschreibbar sein.

## Sink Descriptor

Ein Sink kann logisch beschrieben werden als:

```text
Sink {
    type
    destination
    trust
    security_context
    capabilities
}
```

Beispiel:

```text
type:
    network

destination:
    external_service

trust:
    third_party
```

## Zugriffskontrolle

Vor einem Informationsfluss muss geprüft werden, ob die Labels der Information mit dem Ziel vereinbar sind.

Beispiel:

```text
Data:
    confidential
    local_only

Sink:
    external_network
```

Ergebnis:

```text
DENY
```

## Kontrollentscheidungen

Eine Prüfung kann mindestens ergeben:

```text
ALLOW
DENY
REQUIRE_ACTION
```

`REQUIRE_ACTION` kann beispielsweise bedeuten:

```text
user_authorization
declassification
sanitization
encryption
```

## Dynamische Sinks

Die Eigenschaften eines Sinks dürfen sich zur Laufzeit ändern.

Beispiel:

```text
trusted_network
    ↓ network changes
untrusted_network
```

Vor sicherheitsrelevanten Übertragungen muss der aktuelle Zustand berücksichtigt werden.

## Indirekte Sinks

Auch Capabilities können als Übergang zu einem Sink wirken.

```text
Document
    ↓
CloudUploader
    ↓
Remote Service
```

NovaOS muss den tatsächlichen Zielpfad berücksichtigen und darf sich nicht nur auf den unmittelbar nächsten Node beschränken.

## Beispiel

```text
Object:
    Document.Report

Labels:
    confidential
    local_only

Target:
    external_cloud
```

Policy:

```text
local_only → external_network denied
```

Ergebnis:

```text
DENY
```

## Normative Anforderungen

1. Sicherheitsrelevante Ausgabeziele MÜSSEN als Sinks klassifizierbar sein.
2. Vor einem Informationsfluss MUSS die Kompatibilität zwischen Information Labels und Sink geprüft werden.
3. NovaOS MUSS mindestens `ALLOW`, `DENY` und `REQUIRE_ACTION` unterscheiden können.
4. Indirekte Datenflüsse zu einem Sink MÜSSEN berücksichtigt werden können.
5. Änderungen des Sink-Sicherheitskontexts MÜSSEN vor relevanten Übertragungen berücksichtigt werden.
6. Ein Sink DARF Information-Flow-Beschränkungen nicht eigenständig umgehen.
7. Declassification oder Sanitization MUSS vor der eigentlichen Sink-Freigabe abgeschlossen sein.

## Abgrenzung

Diese NPSPEC definiert:

- Sink-Klassifikation
- Kontrolle von Informationszielen
- Sink-Entscheidungen
- indirekte Sink-Pfade

Nicht Bestandteil sind:

- Information Labels
- Source Classification
- allgemeine Policy-Sprache
- Declassification und Sanitization

## Zugehörige NPSPECs

- `NPSPEC-INFOFLOW-0001 – Information Labels`
- `NPSPEC-INFOFLOW-0002 – Label Propagation`
- `NPSPEC-INFOFLOW-0003 – Source Classification`
- `NPSPEC-INFOFLOW-0005 – Information Flow Policy`
- `NPSPEC-INFOFLOW-0006 – Declassification & Sanitization`
- `NPSPEC-INFOFLOW-0007 – IPC & Cross-Process Flow Tracking`
- `NPSPEC-INFOFLOW-0008 – Persistent Data Flow Metadata`