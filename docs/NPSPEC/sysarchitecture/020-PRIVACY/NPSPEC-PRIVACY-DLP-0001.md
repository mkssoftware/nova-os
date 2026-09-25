# NPSPEC-PRIVACY-DLP-0001 – Nova Data Loss Prevention

## Status

Angenommen

## Kategorie

Privacy / Data Protection / Data Loss Prevention

## Zweck

NovaOS definiert Data Loss Prevention (DLP) als systemweiten Mechanismus, der unerlaubte oder unbeabsichtigte Weitergabe geschützter Daten erkennt und verhindert.

```text
Protected Data
      ↓
Transfer Attempt
      ↓
DLP Policy
      ↓
Allow / Restrict / Deny
```

DLP ergänzt Zugriffskontrolle und Information Flow, ersetzt diese jedoch nicht.

## Grundprinzipien

```text
Can Read ≠ Can Export
Can Copy ≠ Can Share
Encrypted ≠ Allowed Transfer
DLP ≠ Authorization
Destination Reachable ≠ Destination Allowed
```

## DLP-Modell

Eine DLP-Entscheidung besitzt mindestens:

```text
DLPContext
├── DataID
├── Privacy Label
├── Operation
├── Destination
└── Decision
```

Optional:

```text
Purpose
Security Domain
Recipient
Sovereignty Policy
Application
Device
Network Context
```

## Kontrollierte Datenwege

DLP muss mindestens integrierbar sein mit:

```text
Filesystem
Clipboard
IPC
Network
Messaging
Removable Storage
Printing
Export
Remote Processing
```

Die Kontrolle soll möglichst an zentralen Systemgrenzen erfolgen, statt ausschließlich durch Anwendungen.

## Policy-Auswertung

Eine DLP-Entscheidung kann berücksichtigen:

```text
Privacy Label
Data Classification
Purpose
Destination
Recipient
Security Domain
Sovereignty
Operation
```

Beispiel:

```text
Sensitive Personal Data
        +
External Destination
        ↓
DLP Policy
        ↓
Deny
```

## Reaktionen

DLP Policies müssen unterschiedliche Reaktionen erlauben:

```text
Allow
Allow with Audit
Require Confirmation
Redact
Restrict
Quarantine
Deny
```

Sicherheitskritische Regeln dürfen nicht durch einfache Benutzerbestätigung umgangen werden.

## Datenminimierung

Wenn eine vollständige Übertragung nicht erforderlich ist, darf DLP eine reduzierte Darstellung ermöglichen.

```text
Full Dataset
     ↓
Minimization / Redaction
     ↓
Allowed Subset
```

Dies muss semantisch kontrolliert erfolgen und darf Daten nicht still verfälschen.

## Privacy Labels

Privacy Labels bilden eine zentrale Grundlage für DLP.

```text
Data
 ↓
Privacy Label
 ↓
DLP Policy
 ↓
Transfer Decision
```

Labels dürfen beim Kopieren, Exportieren oder Übertragen nicht automatisch verloren gehen.

## Information Flow

DLP muss mit Nova Information Flow Security zusammenarbeiten.

```text
Access Control
      +
Information Flow
      +
DLP
      ↓
Effective Transfer Decision
```

DLP darf bestehende strengere Sicherheitsentscheidungen nicht abschwächen.

## Data Sovereignty

DLP muss geografische und organisatorische Datenregeln berücksichtigen können.

```text
Data → EU Only
Destination → Non-EU
        ↓
Deny
```

Verschlüsselung allein macht einen unzulässigen Transfer nicht zulässig.

## Umgehungsschutz

DLP-relevante Systemgrenzen dürfen nicht durch alternative Datenwege einfach umgangen werden.

Dazu gehören insbesondere:

```text
Raw Device Access
Direct Network Access
Shared Memory
Clipboard
Temporary Files
Custom Protocols
```

Capabilities und Sandboxing müssen solche Pfade kontrollierbar machen.

## Audit

Relevante DLP-Ereignisse sollen auditierbar sein:

```text
DataID
Operation
Destination
Policy
Decision
Reason
Timestamp
```

Sensible Inhalte selbst sollen nicht unnötig im Audit-Log gespeichert werden.

## Introspection

NovaOS soll mindestens darstellen können:

```text
DataID
Privacy Label
Operation
Destination
DLP Policy
Decision
Reason
```

## Normative Anforderungen

1. NovaOS MUSS systemweite DLP Policies unterstützen können.
2. DLP MUSS von normaler Zugriffskontrolle getrennt behandelt werden.
3. Privacy Labels MÜSSEN als Grundlage für DLP verwendbar sein.
4. DLP MUSS zentrale Datenwege wie Netzwerk, IPC, Clipboard und Storage kontrollieren können.
5. DLP DARF strengere Security- oder Information-Flow-Regeln NICHT abschwächen.
6. Data Sovereignty MUSS in DLP-Entscheidungen integrierbar sein.
7. Verschlüsselung DARF unzulässige Datenübertragung NICHT automatisch erlauben.
8. DLP-Regeln MÜSSEN gegen triviale Umgehung über alternative Datenwege geschützt werden.
9. DLP-Ereignisse SOLLEN auditierbar sein.
10. DLP-Entscheidungen SOLLEN sicher introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-LABEL-0001`
- `NPSPEC-SECURITY-INFORMATIONFLOW-0001`
- `NPSPEC-SECURITY-DATASOVEREIGNTY-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-SANDBOX-0001`
- `NPSPEC-SECURITY-AUDIT-0001`
- `ADR-PRIVACY-0005`

## Ergebnis

```text
Protected Data
      ↓
Privacy Label
      ↓
Information Flow + DLP Policy
      ↓
Destination Evaluation
      ↓
Allow / Restrict / Deny
```

NovaOS erhält damit eine systemweite DLP-Schicht, die verhindert, dass geschützte Daten trotz vorhandener Leseberechtigung unkontrolliert exportiert, übertragen oder an unzulässige Ziele weitergegeben werden.