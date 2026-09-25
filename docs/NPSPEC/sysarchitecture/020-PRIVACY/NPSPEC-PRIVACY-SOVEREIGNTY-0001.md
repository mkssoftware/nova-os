# NPSPEC-PRIVACY-SOVEREIGNTY-0001 – Nova Privacy Data Sovereignty

## Status

Angenommen

## Kategorie

Privacy / Data Protection / Data Sovereignty

## Zweck

NovaOS definiert Privacy Sovereignty als datenschutzbezogene Kontrolle darüber, wo, durch wen und unter welchen Bedingungen personenbezogene oder sensible Daten gespeichert, verarbeitet und übertragen werden dürfen.

```text
Privacy Data
     ↓
Sovereignty Policy
     ↓
Location + Provider + Purpose
     ↓
Allow / Deny
```

## Grundprinzipien

```text
Reachable ≠ Permitted
Encrypted ≠ Sovereign
Cloud ≠ Trusted
Accessible ≠ Transferable
Fastest Location ≠ Allowed Location
```

## Sovereignty-Modell

Eine Privacy-Sovereignty-Policy besitzt mindestens:

```text
PrivacySovereigntyPolicy
├── PolicyID
├── Target Data
├── Allowed Locations
├── Allowed Processing
└── State
```

Optional:

```text
Allowed Providers
Forbidden Locations
Trust Domains
Purpose
Retention
Replication Policy
Export Policy
Legal Context
```

## Standortkontrolle

Daten müssen auf geografische oder logische Bereiche begrenzbar sein.

```text
Personal Data
     ↓
Allowed Region = EU
     ↓
EU Storage → Allow
Non-EU Storage → Deny
```

Standortregeln dürfen unabhängig vom physischen Netzwerkpfad gelten.

## Providerkontrolle

Policies müssen Verarbeitung auf bestimmte Provider oder Trust Domains begrenzen können.

```text
Allowed:
Local Device
Trusted Organization
Approved Provider

Denied:
Unknown Provider
Untrusted Domain
```

Ein technisch erreichbarer Provider erhält dadurch keine automatische Verarbeitungserlaubnis.

## Verarbeitung

Sovereignty muss nicht nur Speicherung, sondern auch Verarbeitung berücksichtigen.

```text
Storage Location
       +
Processing Location
       +
Processing Provider
       ↓
Sovereignty Decision
```

Daten dürfen nicht automatisch an entfernte Dienste übertragen werden, nur weil diese eine Operation effizienter ausführen könnten.

## Replikation und Backups

Sovereignty Policies müssen gelten für:

```text
Replicas
Snapshots
Backups
Caches
Temporary Copies
Disaster Recovery
```

Eine zulässige Primärspeicherung bedeutet nicht automatisch, dass Replikation in jede Region zulässig ist.

## Privacy Labels

Privacy Labels können Sovereignty-Anforderungen direkt referenzieren.

```text
Privacy Label
├── Allowed Region
├── Allowed Provider
├── Export Policy
└── Processing Restrictions
```

Die Policy bleibt beim Kopieren oder Verschieben der Daten erhalten.

## Remote Processing

Vor Remote Processing muss NovaOS prüfen:

```text
Destination
Provider
Trust Domain
Purpose
Data Classification
Privacy Policy
```

Ist kein zulässiger Ausführungsort verfügbar, muss die Operation lokal ausgeführt, anders geplant oder abgelehnt werden.

## Data Minimization

Sovereignty muss mit Data Minimization kombiniert werden.

```text
Remote Operation
      ↓
Required Data?
      ↓
Minimal Dataset
      ↓
Sovereignty Check
      ↓
Transfer
```

Nur tatsächlich benötigte Daten sollen eine zulässige Grenze überschreiten.

## DLP und Information Flow

Privacy Sovereignty bildet eine Policy-Eingabe für:

```text
DLP
Information Flow
Network Intent
Storage Placement
Remote Execution
```

Strengere Security- oder Privacy-Regeln dürfen dadurch nicht abgeschwächt werden.

## Priorität

Bei Konflikten gilt die NovaOS-Prioritätsordnung:

```text
Safety
  ↓
Security
  ↓
Sovereignty / Trust
  ↓
Hard System Constraints
  ↓
Explicit User Decisions
  ↓
Soft Preferences
  ↓
Optimization
```

Performance darf Sovereignty-Regeln nicht umgehen.

## Introspection

NovaOS soll mindestens darstellen können:

```text
DataID
Allowed Locations
Current Location
Provider
Processing Location
Export Policy
Decision
Reason
```

## Normative Anforderungen

1. NovaOS MUSS Privacy Sovereignty systemweit unterstützen.
2. Speicher- und Verarbeitungsstandort MÜSSEN getrennt kontrollierbar sein.
3. Provider und Trust Domains MÜSSEN policygesteuert begrenzbar sein.
4. Replikate, Backups und Caches MÜSSEN Sovereignty Policies berücksichtigen.
5. Privacy Labels MÜSSEN Sovereignty-Regeln referenzieren können.
6. Remote Processing MUSS vor Datenübertragung geprüft werden.
7. Verschlüsselung DARF eine unzulässige Datenübertragung NICHT legitimieren.
8. Performance-Optimierung DARF Sovereignty-Regeln NICHT umgehen.
9. Data Minimization, DLP und Information Flow MÜSSEN integrierbar sein.
10. Sovereignty-Entscheidungen SOLLEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-LABEL-0001`
- `NPSPEC-PRIVACY-DLP-0001`
- `NPSPEC-SECURITY-DATASOVEREIGNTY-0001`
- `NPSPEC-SECURITY-INFORMATIONFLOW-0001`
- `NPSPEC-NETWORK-SOVEREIGNTY-0001`
- `NPSPEC-NETWORK-INTENT-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-PRIVACY-0008`

## Ergebnis

```text
Privacy Data
     ↓
Location + Provider + Purpose
     ↓
Sovereignty Policy
     ↓
Allowed Placement / Processing
```

NovaOS erhält damit eine datenschutzbezogene Sovereignty-Schicht, die sicherstellt, dass personenbezogene und sensible Daten nur an zulässigen Orten, durch zulässige Provider und für zulässige Zwecke verarbeitet werden.