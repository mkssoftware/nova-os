# ADR-PRIVACY-0002 – Data Retention und Expiration

## Status

Angenommen

## Kategorie

Privacy Architecture / Data Lifecycle / Retention

## Kontext

Datenminimierung endet nicht mit der Erhebung oder Verarbeitung von Daten.

Auch Daten, die ursprünglich erforderlich waren, können ihren Zweck später verlieren. Werden solche Daten unbegrenzt gespeichert, entstehen unnötige Datenschutz-, Sicherheits- und Ressourcenrisiken.

NovaOS benötigt deshalb ein systemweites Modell für:

- Aufbewahrungsdauer,
- Ablaufbedingungen,
- automatische Löschung,
- rechtlich oder funktional notwendige Aufbewahrung,
- temporäre Daten,
- abgeleitete Daten,
- Logs und Diagnosedaten,
- Backups und Snapshots.

---

## Entscheidung

NovaOS behandelt **Data Retention und Expiration als native Eigenschaften des Daten-Lifecycles**.

Schützenswerte oder temporäre Daten können mit einer expliziten Retention Policy verbunden werden.

```text
Data Created
    ↓
Active Use
    ↓
Retention Period / Condition
    ↓
Expiration
    ↓
Delete / Cryptographic Erase / Approved Transformation
```

Daten sollen nicht länger aufbewahrt werden als für ihren definierten Zweck erforderlich.

---

## Grundprinzip

```text
Retention must have a reason.

Expiration must have an effect.
```

Unbegrenzte Aufbewahrung darf nicht der implizite Standard für temporäre oder datenschutzrelevante Daten sein.

---

## Retention Policy

Eine Retention Policy kann beispielsweise festlegen:

```text
Purpose
Creation Time
Expiration Time
Maximum Lifetime
Retention Condition
Required Minimum Retention
Deletion Policy
```

Die konkrete Policy-Struktur wird in NPSPECs definiert.

---

## Arten der Retention

NovaOS muss unterschiedliche Retention-Semantiken unterstützen können.

Beispiele:

```text
Session Lifetime
Task Lifetime
Application Lifetime
Fixed Duration
Until Purpose Completed
Until Explicit User Deletion
Legally Required Retention
Persistent Until Replaced
```

Die Retention-Semantik muss zum jeweiligen Datentyp und Zweck passen.

---

## Expiration

Expiration bedeutet, dass Daten nach Ablauf ihrer zulässigen Lebensdauer nicht mehr regulär verwendet werden dürfen.

```text
Valid
  ↓
Expired
  ↓
Unavailable for normal use
```

Die physische oder kryptographische Entfernung kann anschließend entsprechend der Storage- und Crypto-Policy erfolgen.

Expiration und physische Löschung sind damit unterscheidbare Lifecycle-Ereignisse.

---

## Automatische Löschung

Wo die Policy dies vorsieht, muss NovaOS abgelaufene Daten automatisch entfernen können.

Mögliche Verfahren sind:

```text
Logical Deletion
Physical Deletion
Cryptographic Erasure
Secure Storage Cleanup
```

Das geeignete Verfahren hängt von Storage-Technologie, Verschlüsselung, Snapshots, Backups und Replikation ab.

---

## Mindestaufbewahrung

Einige Daten dürfen nicht vor Ablauf einer vorgeschriebenen Mindestdauer gelöscht werden.

```text
Minimum Retention
      ↓
Protected Period
      ↓
Expiration permitted
```

Eine Maximum Retention begrenzt dagegen, wie lange Daten höchstens gespeichert werden dürfen.

Beide Konzepte müssen getrennt modellierbar sein.

---

## Temporäre Daten

Temporäre Daten müssen einen klar definierten Lifecycle besitzen.

Beispiele:

```text
Temporary Files
Caches
Intermediate Pipeline Objects
Session Data
Clipboard Data
Transient AI Context
```

Temporäre Daten sollen bevorzugt an den Lifecycle ihrer erzeugenden Operation, Session oder Execution Domain gebunden werden.

---

## Abgeleitete Daten

Retention gilt auch für abgeleitete Daten.

```text
Source Data
    ↓
Processing
    ↓
Derived Data
```

Eine Transformation darf bestehende Retention- oder Privacy-Anforderungen nicht automatisch entfernen.

Neue Retention-Regeln dürfen entstehen, wenn dies semantisch und policyseitig zulässig ist.

---

## Logs und Diagnosedaten

Logs, Telemetrie und Crash Dumps benötigen eigene begrenzte Retention Policies.

```text
Diagnostic Data
      ↓
Retention Window
      ↓
Expiration
      ↓
Deletion / Aggregation
```

Diagnosedaten dürfen nicht allein deshalb unbegrenzt gespeichert werden, weil ihre zukünftige Verwendung theoretisch möglich wäre.

---

## KI und adaptive Systeme

Trainings-, Adaptions- und Prediction-Daten unterliegen denselben Retention-Prinzipien.

Das adaptive NovaOS-Modell soll bevorzugt notwendige Erkenntnisse oder Modellzustände behalten, statt unbegrenzt vollständige historische Rohdaten zu speichern.

```text
Observation
    ↓
Learning Signal
    ↓
Model Update
    ↓
Raw Data expires when no longer required
```

---

## Snapshots, Backups und Replikate

Expiration muss relevante Kopien berücksichtigen.

```text
Primary Data
 ├── Snapshot
 ├── Backup
 └── Replica
```

Das Löschen des primären Objekts allein darf nicht als vollständige Datenentfernung dargestellt werden, wenn weiterhin nutzbare Kopien existieren.

Retention Policies müssen deshalb mit Storage-Versionierung, Backup und Replikation integrierbar sein.

---

## Legal Hold und Policy Override

Eine autorisierte Policy kann die reguläre Expiration vorübergehend blockieren, beispielsweise aufgrund einer gesetzlichen Aufbewahrungspflicht.

```text
Expiration Reached
      +
Authorized Hold
      ↓
Retention continues
```

Ein solcher Hold muss explizit, autorisiert und auditierbar sein.

Er darf nicht als allgemeiner Mechanismus zur unbegrenzten Aufbewahrung verwendet werden.

---

## Data Sovereignty und Information Flow

Retention ändert keine anderen Schutzanforderungen.

Gespeicherte Daten bleiben während ihrer gesamten Lebensdauer den geltenden Regeln für:

```text
Security
Information Flow
Data Sovereignty
Encryption
Access Authority
```

unterworfen.

---

## Introspection und Audit

NovaOS soll kontrolliert nachvollziehbar machen können:

```text
Data Identity
Retention Policy
Purpose
Creation Time
Expiration
Hold State
Deletion State
Remaining Copies
```

Die Introspection darf dabei selbst keine geschützten Inhalte offenlegen.

---

## Normative Anforderungen

1. NovaOS MUSS Data Retention und Expiration als systemweite Lifecycle-Eigenschaften unterstützen.
2. Datenschutzrelevante und temporäre Daten MÜSSEN mit begrenzbaren Retention Policies versehen werden können.
3. Minimum und Maximum Retention MÜSSEN getrennt modellierbar sein.
4. Expiration MUSS von physischer Datenlöschung unterscheidbar sein.
5. Abgelaufene Daten DÜRFEN nicht regulär weiterverwendet werden.
6. Automatische Löschung oder kryptographische Entfernung MUSS policygesteuert möglich sein.
7. Temporäre Daten SOLLEN an den Lifecycle ihrer Session, Task Group, Operation oder Execution Domain gebunden werden können.
8. Retention-Regeln MÜSSEN auf relevante abgeleitete Daten anwendbar sein.
9. Logs, Telemetrie und Crash Dumps MÜSSEN begrenzbare Retention Policies besitzen.
10. KI- und Adaptionssysteme DÜRFEN Retention Policies NICHT umgehen.
11. Snapshots, Backups und Replikate MÜSSEN bei der tatsächlichen Datenentfernung berücksichtigt werden.
12. Legal Holds oder vergleichbare Ausnahmen MÜSSEN explizit autorisiert und auditierbar sein.
13. Retention DARF Security-, Information-Flow-, Sovereignty- oder Encryption-Anforderungen NICHT abschwächen.
14. Retention-, Expiration- und Löschzustände MÜSSEN kontrolliert introspektierbar und auditierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Daten werden nicht unnötig dauerhaft gespeichert,
- geringeres Datenschutz- und Sicherheitsrisiko,
- temporäre Daten erhalten einen definierten Lifecycle,
- Backups und Snapshots werden in Löschkonzepte einbezogen,
- KI-, Diagnose- und Telemetriedaten folgen denselben Privacy-Regeln.

### Negative Konsequenzen

- Storage-Systeme müssen Retention-Metadaten berücksichtigen,
- Backup und Snapshot Handling wird komplexer,
- gesetzliche Mindestaufbewahrung und maximale Retention können miteinander koordiniert werden müssen.

---

## Verworfene Alternativen

### Daten bleiben gespeichert, bis Speicher benötigt wird

Verworfen.

Ressourcendruck ist kein geeignetes Datenschutz-Lifecycle-Modell.

### Retention ausschließlich Anwendungen überlassen

Verworfen.

Systemdienste, Logs, Capabilities, Backups und KI-Komponenten erzeugen ebenfalls Daten.

### Expiration bedeutet automatisch physisches Überschreiben

Verworfen.

Bei modernen Storage-Systemen, Snapshots und Verschlüsselung ist dies nicht immer möglich oder sinnvoll.

### Backups von Retention ausnehmen

Verworfen.

Eine weiterhin entschlüsselbare Backup-Kopie bleibt eine existierende Kopie der Daten.

---

## Abhängigkeiten

- `ADR-PRIVACY-0001_Data_Minimization`
- `ADR-SECURITY-0015_Information_Flow_Control`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-CRYPTO-0006_Secure_Erase_als_Key_Destruction_wo_möglich`
- `ADR-CRYPTO-0010_Privacy_Preserving_Logs_und_Crash_Dumps`
- `ADR-STORAGE-0005_Versioned_Storage`
- `ADR-STORAGE-0014_Snapshots_und_Rollback`

---

## Zugehörige NPSPECs

- `NPSPEC-PRIVACY-RETENTION-0001`
- `NPSPEC-PRIVACY-EXPIRATION-0001`
- `NPSPEC-PRIVACY-TEMPORARY-DATA-0001`
- `NPSPEC-PRIVACY-RETENTION-HOLD-0001`
- `NPSPEC-PRIVACY-BACKUP-RETENTION-0001`
- `NPSPEC-PRIVACY-RETENTION-TEST-0001`

---

## Ergebnis

NovaOS behandelt Daten nicht nur anhand ihres Speicherortes, sondern anhand ihres vollständigen Lifecycles:

```text
Creation
   ↓
Purpose
   ↓
Retention Policy
   ↓
Active Lifetime
   ↓
Expiration
   ↓
Deletion / Cryptographic Erasure
```

Damit wird verhindert, dass ursprünglich notwendige Daten allein aufgrund fehlender Lifecycle-Regeln dauerhaft im System verbleiben.