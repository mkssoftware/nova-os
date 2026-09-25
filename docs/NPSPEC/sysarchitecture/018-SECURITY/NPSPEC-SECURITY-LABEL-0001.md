# NPSPEC-SECURITY-LABEL-0001 – Nova Security Labels

## Status

Angenommen

## Kategorie

Security / Policy / Security Labels

## Zweck

NovaOS definiert ein einheitliches Modell für Security Labels zur Kennzeichnung sicherheitsrelevanter Eigenschaften von Subjects, Ressourcen und Daten.

```text
Object / Subject
      ↓
Security Label
      ↓
Security Policy
      ↓
Decision
```

Security Labels liefern Policy-Informationen, erzeugen aber selbst keine Autorität.

## Grundprinzipien

```text
Label ≠ Capability
Label ≠ Permission
Label ≠ Identity
Classification ≠ Authorization
Unknown Label ≠ Trusted
```

## Label-Modell

Ein Security Label besitzt mindestens:

```text
SecurityLabel
├── LabelID
├── Type
├── Value
└── State
```

Optional:

```text
Security Domain
Integrity Level
Confidentiality Level
Compartments
Sovereignty Class
Provenance
```

`LabelID` muss eine stabile logische Identität besitzen.

## Label-Typen

NovaOS muss unterschiedliche Label-Klassen unterstützen können:

```text
Confidentiality
Integrity
Trust
Security Domain
Data Classification
Sovereignty
Custom Policy
```

Beispiel:

```text
Confidentiality = Sensitive
Integrity       = Trusted
Domain          = System
```

## Zuweisung

Labels dürfen durch vertrauenswürdige Komponenten oder Policies zugewiesen werden.

```text
Create / Import / Receive
          ↓
Label Assignment
          ↓
Protected Object
```

Eine Anwendung darf sicherheitsrelevante Labels nicht eigenständig auf eine günstigere Sicherheitsstufe ändern.

## Bindung

Labels müssen an die logische Identität eines Objekts gebunden werden können.

```text
ObjectID → SecurityLabel
```

Umbenennen, Verschieben oder Ändern des physischen Speicherorts darf ein Label nicht automatisch entfernen.

## Vererbung und Propagation

Labels dürfen kontrolliert propagiert werden.

```text
Sensitive Input
      ↓
Processing
      ↓
Derived Output
      ↓
Sensitive
```

Vererbung muss policygesteuert sein und darf Sicherheitsklassifikationen nicht unkontrolliert abschwächen.

## Relabeling

Änderungen sicherheitsrelevanter Labels müssen explizit autorisiert sein.

```text
Current Label
     ↓
Authorized Relabel
     ↓
New Label
```

Eine Herabstufung kann zusätzliche Declassification-Rechte erfordern.

## Policy-Integration

Security Labels müssen mindestens integrierbar sein mit:

```text
MAC
ABAC
Information Flow
Data Sovereignty
Sandboxing
Audit
```

Beispiel:

```text
Subject Label
     +
Object Label
     ↓
MAC Policy
     ↓
Allow / Deny
```

## Schutz

Labels müssen gegen unautorisierte Manipulation geschützt sein.

Ungültige, fehlende oder unbekannte Labels müssen bei sicherheitskritischen Entscheidungen explizit behandelt werden.

## Introspection

NovaOS soll mindestens darstellen können:

```text
LabelID
Type
Value
TargetID
State
Source
Policy
```

Die Sichtbarkeit sensibler Labels muss selbst autorisiert sein.

## Normative Anforderungen

1. NovaOS MUSS ein einheitliches Security-Label-Modell bereitstellen.
2. Labels DÜRFEN NICHT als Capabilities oder Berechtigungen behandelt werden.
3. Labels MÜSSEN an stabile Ressourcenidentitäten bindbar sein.
4. Sicherheitsrelevante Labels MÜSSEN gegen unautorisierte Änderung geschützt sein.
5. Confidentiality und Integrity MÜSSEN getrennt darstellbar sein.
6. Label-Propagation MUSS policygesteuert sein.
7. Relabeling MUSS explizit autorisiert werden.
8. Sicherheitsklassifikationen DÜRFEN NICHT unkontrolliert abgeschwächt werden.
9. MAC, ABAC und Information Flow SOLLEN Security Labels gemeinsam verwenden können.
10. Label-Zustände SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-SECURITY-MAC-0001`
- `NPSPEC-SECURITY-ABAC-0001`
- `NPSPEC-SECURITY-INFORMATIONFLOW-0001`
- `NPSPEC-SECURITY-DATASOVEREIGNTY-0001`
- `NPSPEC-SECURITY-AUDIT-0001`
- `ADR-SECURITY-0014`

## Ergebnis

```text
Subject / Resource / Data
          ↓
     Security Label
          ↓
MAC / ABAC / Information Flow
          ↓
    Security Decision
```

NovaOS erhält damit ein gemeinsames Label-Modell, über das Sicherheitsklassifikation, Integrität, Vertrauensdomänen und Informationsfluss konsistent beschrieben und durchgesetzt werden können.