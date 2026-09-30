# NPSPEC-INFOFLOW-0005 – Information Flow Policy

## Status

Angenommen

## Zweck

Diese Spezifikation definiert die systemweiten Regeln, nach denen NovaOS Informationsflüsse erlaubt, einschränkt oder blockiert.

Die Policy verbindet Information Labels, Quelle, Ziel und Sicherheitskontext zu einer verbindlichen Entscheidung.

## Grundprinzip

```text
Source
    +
Information Labels
    +
Sink
    +
Security Context
    ↓
Information Flow Policy
    ↓
ALLOW / DENY / REQUIRE_ACTION
```

## Policy-Regel

Eine Regel kann logisch beschrieben werden als:

```text
FlowRule {
    source
    labels
    sink
    context
    action
}
```

Beispiel:

```text
labels:
    confidential

sink:
    external_network

action:
    DENY
```

## Entscheidungen

Mindestens folgende Entscheidungen müssen unterstützt werden:

```text
ALLOW
DENY
REQUIRE_ACTION
```

`REQUIRE_ACTION` kann beispielsweise verlangen:

```text
user_authorization
sanitization
declassification
encryption
```

## Regelpriorität

Bei mehreren passenden Regeln muss eine eindeutige Auswertung möglich sein.

Grundsätzlich gilt:

```text
explicit DENY
    >
restrictive rule
    >
ALLOW
```

Spezifischere Regeln dürfen allgemeinere Regeln überschreiben, sofern dadurch keine höher priorisierte Sicherheitsregel verletzt wird.

## Kontext

Policies dürfen den aktuellen Kontext berücksichtigen.

Beispiele:

```text
user
workspace
device
network
trust
location_class
execution_context
```

Dadurch kann derselbe Informationsfluss unter unterschiedlichen Bedingungen unterschiedlich bewertet werden.

## Default Policy

Existiert keine passende Regel, muss ein definierter sicherer Standard gelten.

Beispiel:

```text
unknown flow
    ↓
DENY
```

Die konkrete Default-Policy kann durch den Sicherheitskontext bestimmt werden.

## Policy-Vererbung

Policies dürfen auf mehreren Ebenen definiert werden:

```text
system
organization
user
workspace
intent
```

Untergeordnete Policies dürfen übergeordnete zwingende Einschränkungen nicht abschwächen.

## Dynamische Neubewertung

Ändert sich ein relevanter Kontext, muss ein Informationsfluss erneut bewertet werden können.

Beispiel:

```text
trusted network
    ↓
network changes
    ↓
untrusted network
    ↓
policy re-evaluation
```

## Beispiel

```text
Information:
    Document.Report

Labels:
    confidential

Sink:
    external_cloud

Policy:
    confidential → external_cloud = DENY
```

Ergebnis:

```text
DENY
```

## Normative Anforderungen

1. Informationsflüsse MÜSSEN durch maschinenlesbare Policies kontrollierbar sein.
2. Policies MÜSSEN Information Labels, Source, Sink und Kontext berücksichtigen können.
3. NovaOS MUSS mindestens `ALLOW`, `DENY` und `REQUIRE_ACTION` unterstützen.
4. Konfligierende Regeln MÜSSEN deterministisch ausgewertet werden.
5. Übergeordnete zwingende Einschränkungen DÜRFEN durch untergeordnete Policies nicht abgeschwächt werden.
6. Für nicht abgedeckte Informationsflüsse MUSS eine sichere Default-Regel existieren.
7. Relevante Kontextänderungen MÜSSEN eine erneute Policy-Bewertung ermöglichen.

## Abgrenzung

Diese NPSPEC definiert:

- Information-Flow-Policies
- Policy-Regeln
- Entscheidungen
- Priorität und Vererbung
- Kontextbewertung

Nicht Bestandteil sind:

- Information Labels
- Source Classification
- Sink Control
- Declassification und Sanitization

## Zugehörige NPSPECs

- `NPSPEC-INFOFLOW-0001 – Information Labels`
- `NPSPEC-INFOFLOW-0002 – Label Propagation`
- `NPSPEC-INFOFLOW-0003 – Source Classification`
- `NPSPEC-INFOFLOW-0004 – Sink Control`
- `NPSPEC-INFOFLOW-0006 – Declassification & Sanitization`
- `NPSPEC-INFOFLOW-0007 – IPC & Cross-Process Flow Tracking`
- `NPSPEC-INFOFLOW-0008 – Persistent Data Flow Metadata`