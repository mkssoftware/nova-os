# NPSPEC-SECURITY-AUDIT-0001 – Nova Security Audit

## Status

Angenommen

## Kategorie

Security / Audit / Accountability

## Zweck

NovaOS definiert ein systemweites Security-Audit-Modell zur nachvollziehbaren Aufzeichnung sicherheitsrelevanter Ereignisse.

```text
Security Event
      ↓
Audit Record
      ↓
Protected Audit Log
      ↓
Analysis
```

Audit dient Nachvollziehbarkeit und Beweissicherung, nicht der Autorisierung selbst.

## Grundprinzipien

```text
Audit ≠ Authorization
Logging ≠ Security
Observable ≠ Public
Audit Record ≠ Mutable History
More Logging ≠ Better Security
```

Audit-Daten müssen geschützt, begrenzt und gezielt erfasst werden.

## Audit Record

Ein Audit-Eintrag besitzt mindestens:

```text
AuditRecord
├── EventID
├── Timestamp
├── SubjectID
├── Operation
├── TargetID
├── Result
└── Reason
```

Optional:

```text
Security Domain
CapabilityID
PolicyID
TraceID
ProcessID
Source
Severity
```

## Audit-Ereignisse

Mindestens folgende Ereignisse müssen erfassbar sein:

```text
Authentication
Authorization Failure
Capability Grant / Revocation
Privilege Change
Policy Change
Code Integrity Failure
Sandbox Violation
Security Configuration Change
Sensitive Data Transfer
Recovery Security Action
```

Nicht jedes normale Systemereignis muss Teil des Security Audit sein.

## Integrität

Audit-Daten müssen gegen unautorisierte Veränderung geschützt sein.

```text
Event
 ↓
Append
 ↓
Protected Record
```

Bereits bestätigte Audit-Einträge dürfen nicht unbemerkt verändert oder gelöscht werden.

Kryptografische Verkettung oder Signierung darf für besonders kritische Logs eingesetzt werden.

## Zugriff

Audit-Daten müssen capability- und policygeschützt sein.

```text
Read Audit
Export Audit
Delete Audit
Configure Audit
```

müssen getrennte Rechte besitzen können.

Eine Anwendung darf ihre eigenen sicherheitsrelevanten Audit-Einträge nicht beliebig entfernen.

## Datenschutz

Audit darf keine unnötigen Geheimnisse speichern.

Insbesondere sollen nicht ungefiltert aufgezeichnet werden:

```text
Passwords
Private Keys
Authentication Tokens
Capability Secrets
Sensitive Payloads
```

Personenbezogene Audit-Daten müssen Retention- und Sovereignty-Regeln berücksichtigen können.

## Verfügbarkeit

Audit muss auch unter hoher Last kontrolliert funktionieren.

```text
Bounded Buffers
Retention Limits
Rate Limits
Priority Classes
```

Kritische Audit-Ereignisse dürfen gegenüber rein diagnostischen Ereignissen priorisiert werden.

Ein Audit-Ausfall muss bei sicherheitskritischen Komponenten erkennbar sein.

## Verteilte Systeme

Audit-Ereignisse dürfen über Systeme hinweg korreliert werden.

```text
TraceID
OperationID
SubjectID
TargetID
```

ermöglichen die Zuordnung verteilter Sicherheitsereignisse, ohne eine globale Prozessidentität vorauszusetzen.

## Retention

Audit-Daten müssen kontrollierte Aufbewahrungsregeln besitzen können.

```text
Active
 ↓
Retention Period
 ↓
Archive / Secure Delete
```

Aufbewahrungsdauer darf abhängig von Ereignistyp, Sicherheitsklasse und Policy sein.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
EventID
Timestamp
Subject
Operation
Target
Result
Reason
Severity
Related Trace
```

Die Sichtbarkeit muss selbst autorisiert sein.

## Normative Anforderungen

1. NovaOS MUSS sicherheitsrelevante Ereignisse auditierbar machen.
2. Audit-Einträge MÜSSEN stabile Ereignisidentitäten besitzen.
3. Audit-Daten MÜSSEN gegen unautorisierte Veränderung geschützt sein.
4. Sicherheitsentscheidungen SOLLEN ihren Entscheidungsgrund erfassen können.
5. Zugriff auf Audit-Daten MUSS explizit autorisiert sein.
6. Geheimnisse DÜRFEN NICHT ungefiltert in Audit-Daten geschrieben werden.
7. Kritische Audit-Ereignisse MÜSSEN von normaler Diagnostik unterscheidbar sein.
8. Audit-Ressourcen MÜSSEN begrenzt sein.
9. Verteilte Ereignisse SOLLEN über stabile IDs korrelierbar sein.
10. Retention und sichere Löschung MÜSSEN policygesteuert sein können.

## Abhängigkeiten

- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-MAC-0001`
- `NPSPEC-SECURITY-INFORMATIONFLOW-0001`
- `NPSPEC-SECURITY-DATASOVEREIGNTY-0001`
- `NPSPEC-DISTCOMM-TRACE-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-SECURITY-0013`

## Ergebnis

```text
Security-Relevant Event
          ↓
Protected Audit Record
          ↓
Correlation + Retention
          ↓
Accountability
```

NovaOS erhält damit eine geschützte und ressourcenbegrenzte Audit-Infrastruktur, mit der sicherheitsrelevante Aktionen und Entscheidungen nachvollziehbar werden, ohne Audit mit Autorisierung oder allgemeinem Logging gleichzusetzen.