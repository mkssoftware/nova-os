# ADR-PRIVACY-0004 – Data Loss Prevention über semantische Informationen

## Status

Angenommen

## Kategorie

Privacy Architecture / Data Loss Prevention / Information Flow

## Kontext

Klassische Data-Loss-Prevention-Systeme erkennen schützenswerte Informationen häufig erst beim Verlassen eines Systems durch nachträgliche Inhaltsanalyse, Mustererkennung oder statische Regeln.

NovaOS besitzt mit:

- Semantic Types,
- Semantic Data Labels,
- Data Provenance,
- Capability Contracts,
- Information Flow Control,
- Data Sovereignty

bereits semantische Informationen über Daten und ihren Kontext.

Diese Informationen sollen genutzt werden, um unerlaubte oder unbeabsichtigte Datenabflüsse bereits innerhalb des System-Datenflusses zu verhindern.

---

## Entscheidung

NovaOS integriert **Data Loss Prevention als semantikbasierte systemweite Policy-Funktion**.

DLP-Entscheidungen sollen bevorzugt auf bereits vorhandenen semantischen Informationen basieren.

```text
Data
 ├── Semantic Type
 ├── Semantic Data Labels
 ├── Provenance
 └── Policy Context
        ↓
Requested Data Flow
        ↓
DLP Policy Evaluation
        ↓
Allow / Transform / Require Approval / Deny
```

DLP wird damit nicht ausschließlich als nachgeschalteter Scanner an Netzwerk- oder Storage-Grenzen implementiert.

---

## Grundprinzip

```text
Understand data before controlling its flow.
```

NovaOS soll Datenabfluss möglichst verhindern, bevor geschützte Informationen eine nicht zulässige Grenze überschreiten.

---

## Semantische DLP-Entscheidung

Eine DLP-Entscheidung kann Informationen berücksichtigen wie:

```text
Semantic Type
Semantic Data Labels
Data Classification
Provenance
Source
Destination
Requested Operation
Caller Identity
Capability Context
Trust Domain
Data Sovereignty
Information Flow Policy
```

Dadurch kann dieselbe technische Operation abhängig von der Bedeutung der Daten unterschiedlich behandelt werden.

---

## Beispiel

Ein Exportvorgang kann technisch identisch sein:

```text
Copy(Object, ExternalDevice)
```

Die Policy kann jedoch unterscheiden:

```text
PublicDocument
    → Allow

InternalDocument
    → Policy Check

ConfidentialDocument
    → Require Approval

Credential
    → Deny
```

Die Entscheidung basiert auf semantischem Kontext und nicht ausschließlich auf Dateiendung oder Byte-Mustern.

---

## Systemweite Enforcement Points

DLP muss an relevanten Datenflussgrenzen integrierbar sein.

Dazu gehören insbesondere:

```text
Storage Export
Clipboard
IPC
Nova.Call
Network Transfer
Remote Execution
External Devices
Application Sharing
Backup / Replication
Printing
Capability Pipelines
```

DLP ist dabei keine separate Datenbewegungsarchitektur, sondern verwendet bestehende NovaOS-Enforcement-Mechanismen.

---

## Information Flow Control

DLP baut auf Information Flow Control auf.

```text
Source Data
    ↓
Information Flow Context
    ↓
DLP Policy
    ↓
Destination
```

IFC beschreibt zulässige Informationsflüsse.

DLP ergänzt darauf aufbauend Regeln zur Verhinderung unerwünschter Offenlegung, Exporte oder Weitergabe.

Beide Systeme müssen konsistente semantische Dateninformationen verwenden.

---

## Data Minimization

Wenn eine vollständige Übertragung nicht zulässig oder erforderlich ist, kann DLP mit Data Minimization kombiniert werden.

```text
Original Object
      ↓
Minimization
      ↓
Permitted View
      ↓
Destination
```

Beispielsweise können vor einem Export:

```text
Location Metadata
Personal Identifiers
Internal Metadata
Credentials
```

entfernt werden, sofern die Transformation ausdrücklich zulässig ist.

---

## Kontrollierte Transformation

DLP darf statt einer vollständigen Blockierung eine definierte Transformation verlangen.

Beispiele:

```text
Redaction
Pseudonymization
Metadata Removal
Aggregation
Restricted Data View
Approved Format Conversion
```

Eine solche Transformation muss selbst autorisiert sein und darf keine Hard Constraints abschwächen.

---

## Capability-System

Capability Contracts können DLP-relevante Eigenschaften deklarieren.

Beispielsweise:

```text
Input Classification
Output Classification
Label Preservation
Permitted Destinations
Declassification Behavior
Data Export Properties
```

Capability Resolution kann dadurch Provider ausschließen, die erforderliche DLP- oder Information-Flow-Regeln nicht erfüllen können.

---

## Provenance und abgeleitete Daten

DLP muss auch abgeleitete Daten berücksichtigen.

```text
Protected Data
      ↓
Processing
      ↓
Derived Result
```

Ein Ergebnis verliert seinen Schutzstatus nicht automatisch dadurch, dass es transformiert, zusammengefasst oder in ein anderes Format konvertiert wurde.

Semantic Data Labels und Provenance dienen dazu, relevante Schutzanforderungen weiterzuführen.

---

## Inhaltsanalyse als Ergänzung

Semantische Metadaten ersetzen Inhaltsanalyse nicht vollständig.

Wenn Daten:

```text
unbekannt,
unlabelled,
extern importiert,
inkonsistent klassifiziert
```

sind, kann zusätzliche Inhaltsklassifikation erforderlich sein.

```text
Semantic Information
        +
Optional Content Inspection
        ↓
DLP Decision
```

Inhaltsanalyse ist damit ein ergänzender Mechanismus und nicht das alleinige Fundament der DLP-Architektur.

---

## Unknown ist nicht automatisch unkritisch

Fehlende semantische Informationen dürfen nicht automatisch als:

```text
Public
```

interpretiert werden.

Für sicherheits- oder datenschutzkritische Datenflüsse gilt:

```text
Unknown ≠ Safe
```

Die Policy entscheidet, ob zusätzliche Klassifikation, Benutzerfreigabe oder Ablehnung erforderlich ist.

---

## Benutzerinteraktion

Policies können für bestimmte Datenflüsse eine explizite Benutzerentscheidung verlangen.

```text
Requested Export
      ↓
DLP Policy
      ↓
User Approval Required
      ↓
Allow / Deny
```

Eine Benutzerfreigabe darf jedoch keine nicht übersteuerbaren Security-, IFC- oder Sovereignty-Hard-Constraints außer Kraft setzen.

---

## KI und adaptive Systeme

KI kann bei der Klassifikation unbekannter Daten oder bei der Erkennung möglicher Datenabflüsse unterstützen.

Ihre Ergebnisse sind jedoch Policy-Input und keine eigenständige Sicherheitsautorität.

```text
AI Classification
      ↓
Confidence / Evidence
      ↓
Policy Evaluation
```

Unsichere Vorhersagen dürfen nicht automatisch Hard Constraints abschwächen.

---

## Security und Authority

DLP ersetzt keine Autorisierung.

```text
Capability Authorization
        +
DLP / IFC Policy
        ↓
Operation permitted
```

Eine gültige Capability kann eine Operation grundsätzlich autorisieren, während DLP den konkreten Informationsfluss dennoch verbietet.

Umgekehrt erzeugt eine positive DLP-Entscheidung keine fehlende Capability.

---

## Data Sovereignty

DLP muss Data-Sovereignty-Anforderungen berücksichtigen.

Beispielsweise kann ein Datenobjekt:

```text
PersonalData
Sovereignty=EU
```

für einen bestimmten Remote Provider funktional geeignet sein, dessen Nutzung aber aufgrund der zulässigen Verarbeitungs- oder Transitregion ausgeschlossen werden.

Sovereignty bleibt ein Hard Constraint.

---

## Audit und Introspection

Relevante DLP-Entscheidungen müssen nachvollziehbar sein.

NovaOS soll kontrolliert darstellen können:

```text
Data Identity
Semantic Classification
Requested Flow
Source
Destination
Applicable Policy
Decision
Required Transformation
Approval
Reason
```

Geschützte Inhalte selbst dürfen dadurch nicht unnötig offengelegt werden.

---

## Normative Anforderungen

1. NovaOS MUSS semantikbasierte Data Loss Prevention systemweit unterstützen.
2. DLP MUSS Semantic Types, Semantic Data Labels und relevante Provenance verwenden können.
3. DLP MUSS mit Information Flow Control integriert sein.
4. DLP MUSS an relevanten Datenflussgrenzen durchsetzbar sein.
5. Fehlende Klassifikation DARF nicht automatisch als unkritisch interpretiert werden.
6. DLP MUSS lokale und entfernte Datenflüsse berücksichtigen können.
7. DLP MUSS Data Minimization und kontrollierte Transformation unterstützen können.
8. Transformation oder Declassification MUSS explizit autorisiert sein.
9. Abgeleitete Daten DÜRFEN ihren Schutzstatus nicht allein durch Transformation verlieren.
10. Inhaltsanalyse MUSS als ergänzender Klassifikationsmechanismus integrierbar sein.
11. Capability Authorization und DLP-Entscheidung MÜSSEN getrennt bleiben.
12. DLP DARF Security-, IFC-, Trust- oder Data-Sovereignty-Hard-Constraints NICHT abschwächen.
13. KI-basierte Klassifikation DARF keine eigenständige Sicherheitsautorität besitzen.
14. DLP-Entscheidungen MÜSSEN kontrolliert auditierbar und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Datenabfluss kann bereits innerhalb des Datenflusses verhindert werden,
- weniger Abhängigkeit von reiner Muster- und Inhaltsanalyse,
- Schutzregeln folgen Daten über Anwendungen und Formate hinweg,
- DLP integriert sich in Capabilities, IFC und Data Sovereignty,
- kontrollierte Minimierung kann unnötige Komplettblockierungen vermeiden.

### Negative Konsequenzen

- Qualität semantischer Labels beeinflusst die Qualität der DLP-Entscheidung,
- unbekannte Daten können zusätzliche Klassifikation benötigen,
- Policy-Auswertung muss an mehreren Datenflussgrenzen effizient verfügbar sein.

---

## Verworfene Alternativen

### DLP ausschließlich am Netzwerk-Gateway

Verworfen.

Daten können auch über Storage, Clipboard, externe Geräte, IPC, Remote Execution oder andere Systemwege abfließen.

### DLP ausschließlich durch Inhaltsanalyse

Verworfen.

NovaOS besitzt bereits reichhaltige semantische Informationen, die präzisere Entscheidungen ermöglichen.

### Jeder autorisierte Datenzugriff erlaubt automatisch Export

Verworfen.

Authority zum Verwenden von Daten ist nicht automatisch Authority zur Weitergabe.

### Unklassifizierte Daten grundsätzlich freigeben

Verworfen.

Dies würde fehlende Metadaten zu einem einfachen Umgehungsweg machen.

---

## Abhängigkeiten

- `ADR-PRIVACY-0001_Data_Minimization`
- `ADR-PRIVACY-0002_Data_Retention_und_Expiration`
- `ADR-PRIVACY-0003_Semantic_Data_Labels`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-SECURITY-0015_Information_Flow_Control`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-STORAGE-0008_Data_Provenance_im_Storage_Modell`
- `ADR-CAPABILITY-0005_Capability_Composition`

---

## Zugehörige NPSPECs

- `NPSPEC-PRIVACY-DLP-0001`
- `NPSPEC-PRIVACY-DLP-POLICY-0001`
- `NPSPEC-PRIVACY-DLP-ENFORCEMENT-0001`
- `NPSPEC-PRIVACY-DLP-CLASSIFICATION-0001`
- `NPSPEC-PRIVACY-DLP-TRANSFORMATION-0001`
- `NPSPEC-PRIVACY-DLP-AUDIT-0001`
- `NPSPEC-PRIVACY-DLP-TEST-0001`

---

## Ergebnis

NovaOS behandelt Data Loss Prevention als Teil seiner semantischen Datenarchitektur:

```text
Semantic Data
      ↓
Classification + Provenance
      ↓
Requested Information Flow
      ↓
DLP + IFC + Sovereignty
      ↓
Allow / Transform / Approval / Deny
```

Dadurch versucht NovaOS nicht erst an der letzten Systemgrenze zu erkennen, ob Daten schützenswert sind.

Die Schutzsemantik begleitet die Daten durch das System und kann bereits vor einem unerlaubten oder unbeabsichtigten Informationsfluss durchgesetzt werden.