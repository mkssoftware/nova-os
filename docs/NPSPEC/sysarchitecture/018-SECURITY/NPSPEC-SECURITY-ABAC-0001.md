# NPSPEC-SECURITY-ABAC-0001 – Nova Attribute-Based Access Control

## Status

Angenommen

## Kategorie

Security / Authorization / Attribute-Based Access Control

## Zweck

NovaOS definiert Attribute-Based Access Control (ABAC) als Policy-Schicht für kontextabhängige Zugriffsentscheidungen.

```text
Subject Attributes
Resource Attributes
Action
Context
      ↓
ABAC Policy
      ↓
Allow / Deny
```

ABAC ergänzt Capability Security, DAC, MAC und RBAC.

## Grundprinzipien

```text
Attribute ≠ Authority
Context ≠ Permission
ABAC Allow ≠ Final Authorization
Unknown Attribute ≠ Allowed
```

ABAC ermöglicht Entscheidungen, die nicht sinnvoll allein über Benutzer, Rollen oder statische Rechte ausgedrückt werden können.

## Attribute

NovaOS darf mindestens folgende Attributklassen verwenden:

```text
Subject
Resource
Action
Environment
Security Context
```

Beispiele:

```text
Subject:
- SecurityDomain
- Role
- TrustLevel

Resource:
- Owner
- Classification
- ResourceType

Environment:
- Network
- DeviceState
- Time
- LocationPolicy
```

Attribute müssen aus vertrauenswürdigen Quellen stammen, wenn sie sicherheitsrelevant sind.

## Policy-Modell

Eine ABAC-Regel besitzt mindestens:

```text
ABACPolicy
├── PolicyID
├── Target
├── Conditions
├── Effect
└── State
```

Optional:

```text
Priority
Validity
Security Domain
Audit Policy
```

Beispiel:

```text
IF
    Resource.Classification = "Internal"
AND Subject.TrustLevel >= Trusted
AND Network.Trusted = true
THEN
    Allow Read
```

## Policy-Auswertung

Die Auswertung muss deterministisch erfolgen.

```text
Request
   ↓
Collect Attributes
   ↓
Validate Attributes
   ↓
Evaluate Policy
   ↓
Allow / Deny
```

Fehlende oder nicht vertrauenswürdige Attribute dürfen bei sicherheitskritischen Regeln nicht implizit als passend behandelt werden.

## Dynamischer Kontext

ABAC darf veränderliche Zustände berücksichtigen:

```text
Network State
Device Trust
Security State
Time
Execution Context
Sovereignty Context
```

Ändert sich ein sicherheitsrelevantes Attribut, darf eine erneute Autorisierung erforderlich werden.

## Integration

Die endgültige Autorisierung darf mehrere Mechanismen kombinieren:

```text
Capability
    +
DAC
    +
MAC
    +
RBAC
    +
ABAC
    ↓
Authorization Decision
```

ABAC darf eine fehlende notwendige Capability oder ein verbindliches MAC-Deny nicht umgehen.

## Attributquellen

Sicherheitsrelevante Attribute müssen eine nachvollziehbare Herkunft besitzen.

```text
Attribute
├── Value
├── Source
├── Trust State
└── Validity
```

Anwendungen dürfen sicherheitskritische Attribute nicht eigenständig manipulieren können.

## Caching

ABAC-Entscheidungen dürfen gecacht werden.

Der Cache muss invalidierbar sein, wenn sich relevante Attribute oder Policies ändern.

Kurzlebige Kontextattribute dürfen keine dauerhaft gültigen Autorisierungsentscheidungen erzeugen.

## Introspection und Audit

NovaOS soll mindestens darstellen können:

```text
PolicyID
SubjectID
ResourceID
Action
Relevant Attributes
Decision
Decision Reason
```

Sensible Attribute müssen dabei geschützt bleiben.

## Normative Anforderungen

1. NovaOS MUSS ABAC als Policy-Schicht unterstützen können.
2. ABAC MUSS Subject-, Resource-, Action- und Context-Attribute verarbeiten können.
3. Sicherheitsrelevante Attribute MÜSSEN aus vertrauenswürdigen Quellen stammen.
4. Fehlende sicherheitskritische Attribute DÜRFEN NICHT implizit zu `Allow` führen.
5. Policy-Auswertung MUSS deterministisch sein.
6. ABAC DARF Capability Security oder MAC NICHT umgehen.
7. Änderungen relevanter Attribute MÜSSEN eine Neubewertung ermöglichen.
8. Anwendungen DÜRFEN sicherheitskritische Attribute NICHT unautorisiert verändern.
9. ABAC-Auswertung MUSS ressourcenbegrenzt sein.
10. Entscheidungen SOLLEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-DAC-0001`
- `NPSPEC-SECURITY-MAC-0001`
- `NPSPEC-SECURITY-RBAC-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-SECURITY-0005`

## Ergebnis

```text
Attributes + Context
        ↓
ABAC Policy
        ↓
Capability + DAC + MAC + RBAC
        ↓
Authorization Decision
```

NovaOS erhält damit eine kontextabhängige Autorisierungsschicht, die Zugriffsentscheidungen anhand vertrauenswürdiger Attribute treffen kann, ohne die grundlegenden Capability- und MAC-Sicherheitsgrenzen zu umgehen.