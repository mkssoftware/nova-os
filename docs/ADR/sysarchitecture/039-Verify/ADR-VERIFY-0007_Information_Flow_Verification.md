# ADR-VERIFY-0007 – Information Flow Verification

## Status

Angenommen

## Kategorie

Verification / Security / Information Flow

## Kontext

Zugriffskontrolle allein verhindert nicht, dass Informationen über unerlaubte Wege zwischen Komponenten, Sicherheitsdomänen oder Vertrauensgrenzen fließen.

Beispiele:

```text
Private Data → Public Output
Secret → Log
Protected Object → Untrusted Service
High-Trust Domain → Low-Trust Domain
```

NovaOS benötigt deshalb eine Möglichkeit, kritische Informationsflüsse unabhängig von einzelnen Zugriffsentscheidungen zu überprüfen.

## Entscheidung

NovaOS verwendet **Information Flow Verification für sicherheits- und datenschutzkritische Datenflüsse**.

```text
Source
  ↓
Classification
  ↓
Processing
  ↓
Flow Validation
  ↓
Destination
```

Ein Informationsfluss ist nur zulässig, wenn Security-, Privacy-, Sovereignty- und Trust-Regeln eingehalten werden.

## Information Labels

Relevante Daten können semantische Sicherheitsinformationen tragen.

Beispiele:

```text
Public
Protected
Private
Secret
Credential
Personal Data
```

Zusätzliche Policies können Herkunft, Zweck oder erlaubte Zielbereiche definieren.

## Flow Rules

Informationsfluss wird als Beziehung zwischen Quelle, Verarbeitung und Ziel betrachtet.

```text
Source
  ↓
Transformation
  ↓
Destination
```

Die Berechtigung zum Lesen einer Quelle bedeutet nicht automatisch, dass deren Inhalt an jedes Ziel weitergegeben werden darf.

## Trust Boundaries

Besondere Prüfung erfolgt beim Übergang zwischen:

```text
Kernel ↔ Userspace
Trusted ↔ Untrusted
Local ↔ Remote
Private ↔ Public
User ↔ Application
Security Domains
```

Grenzübergänge müssen explizite Flow-Regeln respektieren.

## Transformation

Eine Transformation kann die zulässige Informationsklassifikation verändern.

Beispiele:

```text
Anonymization
Aggregation
Redaction
Encryption
Selective Disclosure
```

Eine solche Änderung darf nur angenommen werden, wenn die Transformation dafür ausdrücklich definiert und vertrauenswürdig ist.

## Explizite Freigabe

Bestimmte Informationsflüsse dürfen durch eine autorisierte Declassification erlaubt werden.

```text
Protected Data
      ↓
Authorized Declassification
      ↓
Lower Classification
```

Declassification muss explizit, begrenzt und nachvollziehbar sein.

## Verification

Für kritische Informationsflüsse können kombiniert werden:

```text
Static Analysis
Type / Label Checking
Policy Verification
Model Checking
Runtime Enforcement
Audit
```

Die konkrete Methode richtet sich nach Kritikalität und Systemkomponente.

## Normative Anforderungen

1. Sicherheitskritische Informationsflüsse MÜSSEN überprüfbar sein.
2. Relevante Daten SOLLEN semantische Information-Flow-Labels besitzen können.
3. Lesen und Weitergeben von Informationen MÜSSEN getrennt betrachtet werden.
4. Trust Boundaries MÜSSEN Flow-Regeln erzwingen können.
5. Datenklassifikation DARF nicht implizit abgeschwächt werden.
6. Declassification MUSS explizit autorisiert und nachvollziehbar sein.
7. Transformationen DÜRFEN Labels nur nach definierten Regeln verändern.
8. Data-Sovereignty-Regeln MÜSSEN Bestandteil der Flow-Prüfung sein können.
9. Unbekannte oder nicht überprüfbare Flows MÜSSEN nach Fail-Safe-Default behandelt werden.

## Konsequenzen

### Positiv

- verhindert unerlaubte Datenweitergabe trotz gültigem Einzelzugriff,
- stärkt Datenschutz und Data Sovereignty,
- macht Datenflüsse über Systemgrenzen überprüfbar,
- ergänzt Capability- und Zugriffskontrollen.

### Negativ

- Datenklassifikation und Flow-Regeln erhöhen die Systemkomplexität,
- dynamische Datenflüsse können zusätzliche Runtime-Prüfungen benötigen.

## Abhängigkeiten

- `ADR-VERIFY-0001_Formale_Spezifikation_für_kritische_Komponenten`
- `ADR-VERIFY-0004_Type_Safety`
- `ADR-VERIFY-0005_Capability_Safety`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-PRIVACY-0003`
- `ADR-PRIVACY-0006`
- `ADR-TRUST-0001_Trust_als_Evidence_basiertes_System`

## Ergebnis

NovaOS überprüft nicht nur:

```text
"Wer darf auf Daten zugreifen?"
```

sondern zusätzlich:

```text
"Wohin dürfen diese Informationen fließen?"
```

Damit wird die Kontrolle von Informationsflüssen zu einer eigenständigen überprüfbaren Sicherheitseigenschaft.