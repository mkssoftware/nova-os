# NPSPEC-APP-IDENTITY-0001 – Nova App Identity

## Status

Angenommen

## Kategorie

App / Identity

## Zweck

NovaOS definiert eine stabile Identität für Apps, die unabhängig von Name, Pfad, Installation, Version, Prozess und Gerät bleibt.

Die App Identity bildet die dauerhafte Referenz einer App für Registry, Berechtigungen, Trust, Updates, Zustände und Systemintegration.

## Grundprinzipien

```text
AppID ≠ App Name
AppID ≠ Path
AppID ≠ Package
AppID ≠ Version
AppID ≠ Process
AppID ≠ SandboxID
AppID ≠ Trust
AppID ≠ Authority
```

## Modell

```text
AppIdentity
├── AppID
├── AppType
├── PublisherIdentity
├── IntegrityIdentity
└── Version
```

`AppID` ist die stabile logische Identität der App.

Die übrigen Informationen dienen der Validierung und Einordnung, sind jedoch nicht selbst die AppID.

## Stabilität

Die AppID bleibt erhalten bei:

```text
Rename
Move
Update
Device Migration
Reinstallation
Provider Change
Component Change
UI Change
```

sofern es sich weiterhin um dieselbe logische App handelt.

Eine bewusst unabhängige App erhält eine neue AppID.

## App-Typen

Die App Identity gilt unabhängig vom technischen App-Modell:

```text
AppID
├── Program
├── Solution
├── System App
├── Web App
└── Compatibility App
```

Typ-spezifische Identitäten wie `SolutionID` bleiben erhalten und werden der App Identity zugeordnet.

```text
AppID
   ↓
App Type Identity
   ↓
SolutionID / Program Identity / ...
```

## Instanzen

Laufende Instanzen erhalten eigene Laufzeitidentitäten:

```text
AppID
├── Instance A
├── Instance B
└── Instance C
```

Das Beenden einer Instanz verändert die AppID nicht.

## Integrität

Eine AppID allein beweist weder Integrität noch Vertrauenswürdigkeit.

```text
Validated App Identity
=
AppID
+
Package Integrity
+
Publisher / Provenance
+
Trust State
```

Sicherheitsrelevante Entscheidungen dürfen deshalb nicht ausschließlich auf der AppID beruhen.

## Berechtigungen

Persistente Berechtigungen dürfen an eine validierte App-Identität gebunden werden.

```text
AppID
   +
Validated Integrity
   +
Capability Requirements
      ↓
Permission Decision
```

Sicherheitsrelevante Änderungen können eine erneute Berechtigungsprüfung auslösen.

## Kollisionen

Werden unterschiedliche Inhalte mit derselben AppID erkannt, darf NovaOS diese nicht automatisch als identische vertrauenswürdige App behandeln.

Die Herkunft und Integrität müssen erneut validiert werden.

## Portabilität

Beim Wechsel zwischen kompatiblen Geräten darf die AppID erhalten bleiben.

Lokale:

```text
Handles
Tokens
SandboxIDs
ProcessIDs
Device Bindings
```

werden dadurch nicht portiert.

## Normative Anforderungen

1. Jede registrierte App MUSS eine stabile `AppID` besitzen.
2. AppID MUSS unabhängig von Name, Pfad, Version und Gerät sein.
3. AppID und Laufzeitinstanz MÜSSEN getrennte Identitäten bleiben.
4. AppID und typ-spezifische Identitäten MÜSSEN miteinander verknüpfbar sein.
5. Updates DÜRFEN die AppID nicht allein aufgrund einer Versionsänderung verändern.
6. Eine neue unabhängige App MUSS eine neue AppID erhalten.
7. AppID allein DARF nicht als Integritäts- oder Trust-Nachweis gelten.
8. Sicherheitsrelevante Entscheidungen MÜSSEN zusätzliche Validierungsinformationen berücksichtigen.
9. Persistente Berechtigungen DÜRFEN an validierte App-Identitäten gebunden werden.
10. Sicherheitsrelevante Änderungen MÜSSEN eine Neubewertung bestehender Berechtigungen ermöglichen.
11. Identitätskollisionen MÜSSEN erkannt und sicher behandelt werden.
12. AppID, App-Typ, Integritätsstatus, Herkunft und Trust State MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-APP-MODEL-0001`
- `NPSPEC-APP-PORTABLE-0001`
- `NPSPEC-PROGRAM-TRUST-0001`
- `NPSPEC-SOLUTION-ID-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-REGISTRY-INTEGRITY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine stabile, technische App-Identität, die unabhängig von Darstellung, Speicherort, Version und laufenden Instanzen bleibt. AppID dient als dauerhafte Referenz, während Integrität, Trust, Authority und Laufzeitidentität bewusst getrennt behandelt werden.