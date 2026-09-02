# ADR-PRIVACY-0001 – Data Minimization

## Status

Angenommen

## Kategorie

Privacy Architecture / Data Protection / Information Flow

## Kontext

NovaOS verarbeitet Daten auf vielen Ebenen des Systems:

- Anwendungen,
- Systemdienste,
- Capabilities,
- Telemetrie,
- Logs und Crash Dumps,
- KI- und Adaptionssysteme,
- Netzwerkkommunikation,
- Cloud- und Remote-Dienste,
- Systemanalyse und Architecture Introspection.

Traditionelle Systeme sammeln häufig mehr Daten als für eine konkrete Funktion tatsächlich erforderlich sind.

Dies erhöht:

- Datenschutzrisiken,
- Auswirkungen von Sicherheitsvorfällen,
- Speicher- und Übertragungskosten,
- Informationsfluss zwischen Systemkomponenten,
- regulatorische Risiken.

NovaOS soll deshalb Datenminimierung nicht ausschließlich Anwendungen überlassen, sondern als systemweites Architekturprinzip behandeln.

---

## Entscheidung

NovaOS etabliert **Data Minimization als verbindliches systemweites Privacy-Prinzip**.

```text
Requested Operation
        ↓
Required Data
        ↓
Minimum Necessary Data
        ↓
Processing
        ↓
Minimum Necessary Result
```

Systemkomponenten sollen nur die Daten erhalten, verarbeiten, übertragen und speichern, die für den jeweiligen Zweck erforderlich sind.

---

## Grundprinzip

```text
Collect less.
Expose less.
Transfer less.
Retain less.
```

Daten, die für eine Operation nicht benötigt werden, sollen der ausführenden Komponente möglichst gar nicht erst zugänglich gemacht werden.

---

## Zweckgebundene Datenbereitstellung

Datenzugriff muss an den tatsächlichen Zweck einer Operation gekoppelt werden können.

Beispiel:

```text
Photo Capability
      ↓
requires image pixels

not automatically:
Location
Owner Identity
Camera Serial Number
Complete Metadata
Other Photos
```

Eine Capability erhält nur die für ihren Contract erforderlichen Daten und Authorities.

---

## Minimierung vor Verarbeitung

Daten sollen möglichst vor dem Übergang in eine andere Trust-, Process- oder Execution-Domain reduziert werden.

```text
Source Data
    ↓
Minimization
    ↓
Required Subset
    ↓
Target Domain
```

Dies reduziert die Menge sensibler Informationen, die eine andere Komponente überhaupt sehen kann.

---

## Semantic Types

Semantic Types können Informationen über Datenschutz- und Datenklassifikation tragen.

Beispielsweise:

```text
Location
Identity
BiometricData
Credential
HealthData
PaymentData
DiagnosticData
```

Dadurch können Systemdienste erkennen, dass bestimmte Daten besondere Minimierungs- oder Information-Flow-Regeln benötigen.

Primitive Datentypen allein reichen dafür nicht aus.

---

## Capability-System

Data Minimization wird mit dem Capability-Modell verbunden.

Eine Capability soll ihren erforderlichen Datenumfang über ihren Contract beschreiben können.

```text
Capability Contract
        ↓
Required Inputs
        ↓
Minimum Data View
        ↓
Provider
```

Capability Delegation und Attenuation können zusätzlich den Zugriff auf Datenbereiche einschränken.

---

## Datenansichten

Wenn sinnvoll, sollen Komponenten begrenzte Views statt vollständiger Datenobjekte erhalten.

```text
Complete Object
     ↓
Restricted View
     ↓
Consumer
```

Beispiele:

```text
Contact → Name only
Photo → Pixel data without location metadata
Document → selected section
Device → required property set
```

Dabei soll unnötige physische Duplikation vermieden werden.

---

## Logs und Telemetrie

Logs, Telemetrie und Diagnosedaten unterliegen ebenfalls Data Minimization.

Bevorzugt werden strukturierte technische Informationen wie:

```text
Component ID
Operation ID
Error Code
Timing
Resource State
Causation ID
```

statt vollständiger:

```text
User Content
Memory Contents
Credentials
Tokens
Documents
Messages
```

Sensitive Inhalte dürfen nur erfasst werden, wenn dies ausdrücklich erforderlich und autorisiert ist.

---

## KI und adaptive Systeme

KI- und Adaptionssysteme erhalten keine Ausnahme von Data Minimization.

Das NovaOS-Adaptionsmodell:

```text
Prediction
    ↓
Actual Decision
    ↓
Prediction Error
    ↓
Model Correction
```

soll möglichst mit den minimal notwendigen Signalen arbeiten.

Rohdaten dürfen nicht allein deshalb dauerhaft gespeichert werden, weil sie möglicherweise später für ein Modell nützlich sein könnten.

---

## Remote Processing

Vor Remote Execution oder Datenübertragung muss geprüft werden, welche Daten tatsächlich übertragen werden müssen.

```text
Local Data
    ↓
Minimization
    ↓
Sovereignty / IFC Check
    ↓
Remote Transfer
```

Data Minimization ergänzt dabei Data Sovereignty und Information Flow Control, ersetzt diese jedoch nicht.

---

## Retention

Datenminimierung betrifft auch die Speicherdauer.

```text
Needed
   ↓
Retained
   ↓
Purpose completed
   ↓
Delete / Aggregate / Anonymize
```

Daten sollen nicht unbegrenzt gespeichert werden, wenn der ursprüngliche Zweck dies nicht verlangt.

Konkrete Retention-Regeln werden durch Policies und NPSPECs definiert.

---

## Derived Data

Auch abgeleitete Daten können sensible Informationen enthalten.

```text
Source Data
    ↓
Processing
    ↓
Derived Data
```

Data Minimization darf deshalb nicht nur auf Rohdaten angewendet werden.

Klassifikation, Provenance und Information-Flow-Regeln müssen auch für relevante abgeleitete Daten erhalten bleiben.

---

## Security und Privacy

Data Minimization ergänzt Capability-Based Security.

```text
Capability
→ Darf auf Daten zugreifen.

Data Minimization
→ Welche dieser Daten werden tatsächlich benötigt?
```

Eine vorhandene Authority ist kein Grund, sämtliche erreichbaren Daten zu verarbeiten.

---

## Architecture Introspection

NovaOS soll kontrolliert nachvollziehbar machen können:

```text
Data Source
Requested Data
Required Data
Released Data
Purpose
Retention Policy
Target Domain
```

Dabei darf die Introspection selbst keine unnötigen sensiblen Daten offenlegen.

---

## Normative Anforderungen

1. NovaOS MUSS Data Minimization als systemweites Privacy-Prinzip behandeln.
2. Komponenten SOLLEN nur die für ihre konkrete Aufgabe erforderlichen Daten erhalten.
3. Daten SOLLEN möglichst vor dem Überschreiten von Trust- oder Execution-Domain-Grenzen minimiert werden.
4. Capability Contracts MÜSSEN den erforderlichen Datenumfang beschreibbar machen können.
5. Semantic Types MÜSSEN datenschutzrelevante Datenklassen ausdrücken können.
6. Begrenzte Datenansichten SOLLEN vollständigen Datenzugriff ersetzen, wenn dies ausreichend ist.
7. Data Minimization MUSS für lokale und entfernte Verarbeitung gelten.
8. Logs, Telemetrie und Crash-Diagnostik MÜSSEN dem Prinzip der Datenminimierung folgen.
9. KI- und Adaptionssysteme DÜRFEN Data Minimization NICHT umgehen.
10. Nicht erforderliche Rohdaten DÜRFEN NICHT allein für hypothetische zukünftige Nutzung unbegrenzt gespeichert werden.
11. Retention MUSS auf den tatsächlichen Zweck begrenzbar sein.
12. Abgeleitete Daten MÜSSEN hinsichtlich ihrer Datenschutzrelevanz berücksichtigt werden.
13. Data Minimization DARF Security-, Information-Flow- oder Data-Sovereignty-Regeln NICHT ersetzen oder abschwächen.
14. Data-Minimization-Entscheidungen MÜSSEN kontrolliert introspektierbar und auditierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- geringere Datenschutzrisiken,
- kleinere Auswirkungen von Sicherheitsvorfällen,
- weniger unnötige Datenübertragung und Speicherung,
- bessere Isolation zwischen Komponenten,
- datenschutzfreundlichere KI- und Diagnosesysteme,
- Privacy wird Teil der Systemarchitektur statt ausschließlich Anwendungspolitik.

### Negative Konsequenzen

- Capability- und Datenverträge müssen benötigte Informationen genauer beschreiben,
- Datenansichten und Minimierung können zusätzliche Planung erfordern,
- spätere Sekundärnutzung von Daten ist nicht automatisch möglich.

---

## Verworfene Alternativen

### Anwendungen allein für Datenminimierung verantwortlich machen

Verworfen.

Viele Datenflüsse entstehen innerhalb von Systemdiensten, Capabilities, Telemetrie und Remote Execution.

### Vollständige Daten bereitstellen und auf korrekte Nutzung vertrauen

Verworfen.

Nicht bereitgestellte Daten können weder versehentlich verarbeitet noch unnötig offengelegt werden.

### Alle Daten vorsorglich speichern

Verworfen.

Hypothetischer zukünftiger Nutzen rechtfertigt keine unbegrenzte Datensammlung.

### Verschlüsselung als Ersatz für Datenminimierung

Verworfen.

Verschlüsselte unnötige Daten bleiben unnötig erhobene Daten.

---

## Abhängigkeiten

- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0009_Unified_Data_Object_Pipelining`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0015_Information_Flow_Control`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-CRYPTO-0010_Privacy_Preserving_Logs_und_Crash_Dumps`
- `ADR-CAPABILITY-0008_Capability_Attenuation`

---

## Zugehörige NPSPECs

- `NPSPEC-PRIVACY-DATA-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-DATA-VIEW-0001`
- `NPSPEC-PRIVACY-DATA-RETENTION-0001`
- `NPSPEC-PRIVACY-DIAGNOSTICS-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-AI-DATA-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-DATA-MINIMIZATION-TEST-0001`

---

## Ergebnis

NovaOS behandelt Datenminimierung als Teil des eigentlichen Datenflusses:

```text
Available Data
      ↓
Purpose + Contract
      ↓
Minimum Required Data
      ↓
Security / IFC / Sovereignty
      ↓
Processing
      ↓
Minimum Required Retention
```

Damit basiert Datenschutz nicht darauf, dass jede Komponente verantwortungsvoll mit allen erreichbaren Daten umgeht.

NovaOS reduziert stattdessen bereits architektonisch, welche Daten eine Komponente überhaupt erhalten, verarbeiten, übertragen und dauerhaft speichern kann.