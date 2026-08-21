# NPSPEC-COMPUTE-PACS-0001 — PACS Compute

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Medical Imaging / PACS  
**Komponente:** NovaOS PACS Compute  
**Bezeichner:** `NPSPEC-COMPUTE-PACS-0001`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`, `NPSPEC-COMPUTE-DICOM-0001`  
**Verwandte Spezifikationen:** `NPSPEC-COMPUTE-RADIOLOGY-0001`, `NPSPEC-COMPUTE-RUNTIME-0007`, `NPSPEC-COMPUTE-HPC-0001`  
**Kurzname:** PACS Compute  

---

## 1. Zweck

Diese Spezifikation definiert die gemeinsame PACS-Compute-Infrastruktur der NovaOS-Compute-Architektur.

PACS steht für:

`Picture Archiving and Communication System`

PACS Compute stellt die technische Infrastruktur für:

- Aufnahme,
- Identifikation,
- Validierung,
- Indexierung,
- Speicherung,
- Suche,
- Abruf,
- Übertragung,
- Replikation,
- Archivierung,
- Lebenszyklusverwaltung

medizinischer Bilddaten bereit.

PACS Compute baut auf:

`NPSPEC-COMPUTE-DICOM-0001`

auf.

Die grundlegende Architektur lautet:

```text
Imaging Device
      |
      v
 DICOM Input
      |
      v
 PACS Compute
      |
 +----+-----+--------+--------+
 |          |        |        |
 v          v        v        v
Index     Store    Query    Retrieve
                     |
                     v
                Consumer
```

PACS Compute ist keine radiologische Bildverarbeitungs- oder Diagnosekomponente.

Bildanalyse wird durch:

`NPSPEC-COMPUTE-RADIOLOGY-0001`

und darauf aufbauende Komponenten durchgeführt.

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> PACS Compute trennt medizinische Objekte, Metadatenindex, physische Speicherung, Übertragung, Zugriffskontrolle, Replikation und Lebenszyklusverwaltung voneinander.

Ein medizinisches Objekt DARF nicht ausschließlich über seinen physischen Speicherpfad identifiziert werden.

Die logische Identität MUSS unabhängig von:

- Dateiname,
- Verzeichnis,
- Storage Node,
- Storage Backend,
- physischem Datenträger

bestehen.

---

## 3. Ziele

PACS Compute MUSS architektonisch mindestens folgende Bereiche unterstützen:

1. DICOM-Objekte,
2. Studies,
3. Series,
4. SOP Instances,
5. DICOM-Metadaten,
6. Object Ingestion,
7. Validation,
8. Indexing,
9. Storage,
10. Query,
11. Retrieve,
12. Transfer,
13. Caching,
14. Replikation,
15. Archivierung,
16. Tiered Storage,
17. Integritätsprüfung,
18. Deduplication,
19. Lifecycle Management,
20. Transaktionssicherheit,
21. Zugriffskontrolle,
22. Auditierbarkeit,
23. De-Identification-Integration,
24. DICOM-Netzwerkdienste,
25. DICOMweb,
26. Streaming,
27. parallele Verarbeitung,
28. verteilte Speicherung,
29. Distributed Compute,
30. High Availability,
31. Backup,
32. Recovery,
33. Skalierbarkeit,
34. Observability,
35. Erweiterbarkeit.

---

## 4. Nicht-Ziele

Diese Spezifikation definiert nicht vollständig:

- radiologische Diagnose,
- Befunderstellung,
- Krankenhausverwaltung,
- Patientenverwaltung,
- Terminplanung,
- Abrechnung,
- RIS,
- HIS,
- elektronische Patientenakte,
- konkrete Viewer,
- konkrete Benutzeroberflächen,
- konkrete medizinische KI-Modelle.

Diese Funktionen MÜSSEN separat spezifiziert werden.

---

## 5. Sicherheitsgrundsatz

PACS-Systeme verarbeiten hochsensible medizinische Daten.

PACS Compute MUSS deshalb nach dem Prinzip:

`Least Privilege`

arbeiten.

Zugriffe auf medizinische Daten MÜSSEN durch die zuständige NovaOS-Sicherheits- und Berechtigungsinfrastruktur kontrollierbar sein.

---

## 6. Zero-Trust-Grundsatz

Netzwerkquellen, externe Systeme und eingehende medizinische Objekte MÜSSEN grundsätzlich als nicht vertrauenswürdig behandelt werden.

Eine Netzwerkverbindung allein DARF keine Vertrauensstellung erzeugen.

---

## 7. Object Model

Die grundlegende logische Hierarchie lautet:

```text
Patient
  |
  +-- Study
       |
       +-- Series
            |
            +-- Instance
```

---

## 8. PACS Object

Ein gespeichertes medizinisches Objekt wird logisch als:

`PACSObject`

repräsentiert.

---

## 9. PACS Object Identity

Ein PACS-Objekt MUSS eine stabile technische Identität besitzen.

---

## 10. DICOM SOP Instance

Für DICOM-Objekte MUSS die:

`SOPInstanceUID`

als wesentliche DICOM-Objektidentität berücksichtigt werden.

---

## 11. Study Identity

Eine Studie wird über:

`StudyInstanceUID`

identifiziert.

---

## 12. Series Identity

Eine Serie wird über:

`SeriesInstanceUID`

identifiziert.

---

## 13. SOP Class

Die:

`SOPClassUID`

MUSS gespeichert und indexierbar sein.

---

## 14. Internal Object ID

Zusätzlich zu DICOM-UIDs SOLL PACS Compute eine interne Objekt-ID verwenden können.

---

## 15. Physical Location

Die physische Speicherposition MUSS von der logischen Objektidentität getrennt sein.

---

## 16. Storage Reference

Ein Objekt SOLL eine oder mehrere:

`StorageReference`

besitzen können.

---

## 17. Multiple Storage Copies

Dasselbe logische Objekt DARF mehrere physische Kopien besitzen.

---

## 18. Storage Copy State

Eine physische Kopie SOLL mindestens einen Zustand besitzen können:

```text
AVAILABLE
VERIFYING
MIGRATING
DEGRADED
CORRUPT
OFFLINE
DELETING
```

---

## 19. Object State

Ein logisches PACS-Objekt SOLL mindestens folgende Zustände unterstützen:

```text
RECEIVING
VALIDATING
INDEXING
AVAILABLE
QUARANTINED
ARCHIVED
DELETING
DELETED
ERROR
```

---

## 20. State Transition

Zustandsübergänge MÜSSEN kontrolliert und nachvollziehbar erfolgen.

---

## 21. Ingestion

Das Einbringen neuer medizinischer Daten wird als:

`Ingestion`

bezeichnet.

---

## 22. Ingestion Pipeline

Die grundlegende Pipeline lautet:

```text
Receive
  |
  v
Identify
  |
  v
Validate
  |
  v
Security Check
  |
  v
Store Temporary
  |
  v
Index
  |
  v
Commit
  |
  v
Available
```

---

## 23. Atomic Ingestion

Ein Objekt DARF erst als verfügbar markiert werden, wenn die notwendigen Ingestion-Schritte erfolgreich abgeschlossen wurden.

---

## 24. Partial Ingestion

Unvollständig empfangene Objekte DÜRFEN nicht als vollständig verfügbar veröffentlicht werden.

---

## 25. Temporary Storage

Während der Ingestion SOLL temporärer Speicher verwendet werden können.

---

## 26. Validation

Eingehende DICOM-Objekte MÜSSEN über `NPSPEC-COMPUTE-DICOM-0001` validierbar sein.

---

## 27. Validation Levels

Mindestens folgende Validierungsstufen SOLLEN unterstützt werden:

```text
BASIC
STANDARD
STRICT
```

---

## 28. Invalid Object

Ein ungültiges Objekt DARF nicht stillschweigend als gültiges medizinisches Objekt gespeichert werden.

---

## 29. Quarantine

Nicht eindeutig gültige oder sicherheitskritische Objekte SOLLEN in Quarantäne verschoben werden können.

---

## 30. Quarantine Isolation

Quarantäneobjekte DÜRFEN normalen Clients nicht automatisch bereitgestellt werden.

---

## 31. Duplicate Detection

PACS Compute MUSS doppelte SOP Instance UIDs erkennen können.

---

## 32. Exact Duplicate

Identische Objekte mit identischer SOP Instance UID SOLLEN als exakte Duplikate erkannt werden können.

---

## 33. Conflicting Duplicate

Unterschiedliche Daten mit identischer SOP Instance UID MÜSSEN als Konflikt erkannt werden.

---

## 34. Duplicate Policy

Der Umgang mit Duplikaten MUSS durch eine Policy steuerbar sein.

---

## 35. Duplicate Policies

Mögliche Policies umfassen:

```text
REJECT
KEEP_EXISTING
KEEP_BOTH_INTERNAL
QUARANTINE
MANUAL_REVIEW
```

---

## 36. Silent Replacement

Ein bestehendes Objekt DARF nicht ohne explizite Policy stillschweigend überschrieben werden.

---

## 37. Object Fingerprint

PACS Compute SOLL einen kryptographischen Fingerprint des gespeicherten Objekts erzeugen.

---

## 38. Content Hash

Ein Content Hash SOLL zur Integritätsprüfung verwendet werden können.

---

## 39. Hash Algorithm

Der verwendete Hashalgorithmus MUSS versioniert beziehungsweise identifizierbar sein.

---

## 40. Index

PACS Compute MUSS einen Metadatenindex bereitstellen.

---

## 41. Index Separation

Der Index MUSS von der eigentlichen Objektspeicherung logisch getrennt sein.

---

## 42. Index Entry

Ein Indexeintrag wird als:

`PACSIndexEntry`

repräsentiert.

---

## 43. Indexed Fields

Mindestens folgende Informationen SOLLEN indexierbar sein:

```text
PatientID
StudyInstanceUID
SeriesInstanceUID
SOPInstanceUID
SOPClassUID
StudyDate
StudyTime
Modality
SeriesNumber
InstanceNumber
AccessionNumber
StudyDescription
SeriesDescription
NumberOfFrames
TransferSyntaxUID
StorageState
```

---

## 44. Sensitive Index Fields

Patientenbezogene Indexfelder MÜSSEN den gleichen Datenschutzanforderungen wie die zugrunde liegenden medizinischen Daten unterliegen.

---

## 45. Index Extension

Zusätzliche DICOM-Tags SOLLEN konfigurierbar indexiert werden können.

---

## 46. Private Tag Indexing

Private Tags DÜRFEN nur über explizite Policy indexiert werden.

---

## 47. Index Transaction

Indexänderungen MÜSSEN mit dem Objektzustand konsistent gehalten werden.

---

## 48. Index Failure

Ein Indexierungsfehler DARF ein Objekt nicht fälschlich als vollständig verfügbar markieren.

---

## 49. Reindexing

PACS Compute MUSS eine Neuindexierung gespeicherter Objekte ermöglichen.

---

## 50. Reindex Without Rewrite

Eine Neuindexierung SOLL möglich sein, ohne die gespeicherten Originalobjekte zu verändern.

---

## 51. Query

PACS Compute MUSS strukturierte Abfragen unterstützen.

---

## 52. Query Levels

Mindestens folgende logische Ebenen SOLLEN unterstützt werden:

```text
PATIENT
STUDY
SERIES
INSTANCE
```

---

## 53. Query Filter

Abfragen SOLLEN mehrere Filter kombinieren können.

---

## 54. Exact Match

Exakte Übereinstimmung MUSS unterstützt werden.

---

## 55. Range Query

Bereichsabfragen SOLLEN unterstützt werden.

---

## 56. Date Range

Datumsbereiche SOLLEN unterstützt werden.

---

## 57. Modality Query

Abfragen nach Modalität SOLLEN unterstützt werden.

---

## 58. UID Query

Abfragen nach Study-, Series- und SOP-UID MÜSSEN effizient möglich sein.

---

## 59. Pagination

Große Ergebnismengen MÜSSEN paginiert beziehungsweise gestreamt werden können.

---

## 60. Query Limit

Abfragen SOLLEN konfigurierbare Ergebnisgrenzen besitzen.

---

## 61. Query Timeout

Lang laufende Abfragen SOLLEN zeitlich begrenzt werden können.

---

## 62. Query Cancellation

Abfragen MÜSSEN kontrolliert abbrechbar sein.

---

## 63. Query Authorization

Eine Abfrage DARF nur Ergebnisse liefern, auf die der aufrufende Sicherheitskontext zugreifen darf.

---

## 64. Metadata Leakage Protection

Nicht autorisierte Objekte DÜRFEN auch nicht über Metadatenabfragen offengelegt werden.

---

## 65. Retrieve

Der Abruf gespeicherter Objekte wird als:

`Retrieve`

bezeichnet.

---

## 66. Retrieve Object

Ein einzelnes Objekt MUSS abrufbar sein.

---

## 67. Retrieve Series

Eine vollständige Serie SOLL abrufbar sein.

---

## 68. Retrieve Study

Eine vollständige Studie SOLL abrufbar sein.

---

## 69. Partial Retrieve

Teilabrufe SOLLEN unterstützt werden können.

---

## 70. Frame Retrieve

Bei geeigneten Multiframe-Objekten SOLLEN einzelne Frames abrufbar sein.

---

## 71. Metadata Retrieve

Metadaten SOLLEN ohne vollständigen Abruf der Pixel Data verfügbar sein.

---

## 72. Streaming Retrieve

Große Objekte MÜSSEN streamingfähig übertragen werden können.

---

## 73. Range Read

Storage Backends SOLLEN Teilbereichszugriffe unterstützen können.

---

## 74. Retrieve Authorization

Jeder Abruf MUSS autorisierbar sein.

---

## 75. Retrieve Audit

Abrufe SOLLEN auditierbar sein.

---

## 76. Storage Architecture

PACS Compute MUSS unterschiedliche Speicherbackends unterstützen können.

---

## 77. Storage Backend

Ein Speicherbackend wird als:

`PACSStorageBackend`

repräsentiert.

---

## 78. Backend Interface

Ein Storage Backend SOLL mindestens folgende Operationen bereitstellen:

```text
Put
Get
ReadRange
Exists
Delete
Verify
GetMetadata
```

---

## 79. Local Storage

Lokaler Dateispeicher SOLL unterstützt werden können.

---

## 80. Block Storage

Blockbasierte Speichersysteme SOLLEN integrierbar sein.

---

## 81. Object Storage

Object Storage SOLL unterstützt werden können.

---

## 82. Distributed Storage

Verteilte Speichersysteme SOLLEN integrierbar sein.

---

## 83. Offline Archive

Offline- beziehungsweise Nearline-Archive SOLLEN integrierbar sein.

---

## 84. Storage Independence

Höhere PACS-Schichten DÜRFEN nicht von einem konkreten physischen Storage Backend abhängig sein.

---

## 85. Storage URI

Physische Speicherreferenzen SOLLEN über eine abstrahierte Storage-URI oder äquivalente Referenz dargestellt werden.

---

## 86. Original Object Preservation

Original empfangene DICOM-Objekte SOLLEN unverändert gespeichert werden können.

---

## 87. Derived Objects

Abgeleitete medizinische Objekte MÜSSEN getrennt vom Original identifizierbar sein.

---

## 88. Immutable Storage

PACS Compute SOLL unveränderliche Speicherung unterstützen.

---

## 89. Write Once

Für bestimmte Storage Policies SOLL ein einmal geschriebenes Objekt nicht in-place veränderbar sein.

---

## 90. Object Replacement

Änderungen SOLLEN als neues Objekt beziehungsweise neue Version behandelt werden können.

---

## 91. Versioning

Storage Backends DÜRFEN Objektversionierung unterstützen.

---

## 92. Version Identity

Versionen MÜSSEN eindeutig unterscheidbar sein.

---

## 93. Storage Tier

PACS Compute SOLL mehrere Storage Tiers unterstützen.

---

## 94. Hot Storage

Häufig verwendete Daten SOLLEN auf schnellem Speicher gehalten werden können.

---

## 95. Warm Storage

Weniger häufig verwendete Daten SOLLEN auf kostengünstigerem Speicher gehalten werden können.

---

## 96. Cold Storage

Langzeitdaten SOLLEN auf Cold Storage verschoben werden können.

---

## 97. Archive Storage

Langfristige Archivspeicherung SOLL unterstützt werden.

---

## 98. Tier Policy

Die Zuordnung zu Storage Tiers MUSS über Policies steuerbar sein.

---

## 99. Access Frequency

Zugriffshäufigkeit DARF als Kriterium für Tiering verwendet werden.

---

## 100. Study Age

Alter einer Studie DARF als Tiering-Kriterium verwendet werden.

---

## 101. Modality

Modalität DARF als Tiering-Kriterium verwendet werden.

---

## 102. Legal Retention

Aufbewahrungspflichten MÜSSEN von normalen Cache- oder Tiering-Regeln unterscheidbar sein.

---

## 103. Migration

Objekte SOLLEN zwischen Storage Tiers migriert werden können.

---

## 104. Online Migration

Eine Migration SOLL möglich sein, ohne das logische Objekt unzugänglich zu machen.

---

## 105. Migration Verification

Nach Migration MUSS die Integrität der Zielkopie überprüfbar sein.

---

## 106. Source Deletion

Die Quellkopie DARF erst nach erfolgreicher Verifikation entsprechend der Policy entfernt werden.

---

## 107. Replication

PACS Compute SOLL mehrere physische Kopien eines Objekts verwalten können.

---

## 108. Replication Factor

Die gewünschte Anzahl physischer Kopien SOLL konfigurierbar sein.

---

## 109. Replica Placement

Replikate SOLLEN auf unterschiedliche Failure Domains verteilt werden können.

---

## 110. Failure Domain

Eine Failure Domain DARF repräsentieren:

```text
Disk
Node
Rack
Datacenter
Region
```

---

## 111. Replica Verification

Replikate MÜSSEN auf Integrität überprüfbar sein.

---

## 112. Replica Repair

Fehlende oder beschädigte Replikate SOLLEN aus gültigen Kopien wiederhergestellt werden können.

---

## 113. Replica Selection

Beim Lesen SOLL ein geeignetes verfügbares Replikat gewählt werden.

---

## 114. Read Locality

Datenlokalität SOLL bei der Replikatauswahl berücksichtigt werden.

---

## 115. Integrity

PACS Compute MUSS gespeicherte Daten auf Integrität prüfen können.

---

## 116. Integrity Check

Ein Integrity Check SOLL mindestens vergleichen:

```text
Stored Size
Content Hash
Storage Metadata
```

---

## 117. Periodic Scrubbing

Gespeicherte Daten SOLLEN regelmäßig überprüft werden können.

---

## 118. Scrubbing

Die periodische Integritätsprüfung wird als:

`Storage Scrubbing`

bezeichnet.

---

## 119. Corruption Detection

Beschädigte Kopien MÜSSEN als solche markiert werden.

---

## 120. Corruption Isolation

Eine bekannte beschädigte Kopie DARF nicht bevorzugt ausgeliefert werden.

---

## 121. Self-Healing

Wenn eine gültige Replik vorhanden ist, SOLL PACS Compute eine beschädigte Kopie automatisch wiederherstellen können.

---

## 122. Repair Audit

Automatische Reparaturen MÜSSEN nachvollziehbar sein.

---

## 123. Deduplication

PACS Compute DARF physische Deduplication unterstützen.

---

## 124. Logical Identity

Deduplication DARF die logische Identität medizinischer Objekte nicht zerstören.

---

## 125. Deduplication Hash

Content-basierte Deduplication SOLL kryptographische Hashes verwenden.

---

## 126. Hash Collision Protection

Ein Hash allein DARF bei sicherheitskritischen Entscheidungen nicht zwingend als vollständiger Identitätsnachweis verwendet werden.

---

## 127. Cache

PACS Compute SOLL Cache-Schichten unterstützen.

---

## 128. Metadata Cache

Metadaten DÜRFEN gecacht werden.

---

## 129. Object Cache

Objektdaten DÜRFEN gecacht werden.

---

## 130. Frame Cache

Dekodierte Frames DÜRFEN über DICOM Compute gecacht werden.

---

## 131. Volume Cache

Rekonstruierte Volumen DÜRFEN über Radiology Compute gecacht werden.

---

## 132. Cache Separation

Cache-Daten MÜSSEN von autoritativen Originaldaten unterscheidbar sein.

---

## 133. Cache Eviction

Das Entfernen eines Cache-Eintrags DARF das autoritative PACS-Objekt nicht löschen.

---

## 134. Cache Policy

Cache Policies SOLLEN berücksichtigen können:

```text
LRU
LFU
Size
Age
AccessFrequency
Priority
```

---

## 135. Prefetch

PACS Compute SOLL Daten vorausschauend laden können.

---

## 136. Study Prefetch

Bei Abruf einer Serie DÜRFEN weitere relevante Serien derselben Studie vorgeladen werden.

---

## 137. Prefetch Policy

Prefetch MUSS durch Policy und Ressourcenlimits kontrolliert werden.

---

## 138. Network Architecture

PACS Compute SOLL mehrere medizinische Kommunikationsprotokolle integrieren können.

---

## 139. DICOM Networking

Klassische DICOM-Netzwerkdienste SOLLEN unterstützt werden können.

---

## 140. Association

DICOM Associations SOLLEN verwaltet werden können.

---

## 141. Application Entity

Eine DICOM Application Entity wird abgekürzt:

`AE`

---

## 142. AE Title

AE Titles SOLLEN konfigurierbar sein.

---

## 143. C-ECHO

DICOM Verification über:

`C-ECHO`

SOLL unterstützt werden.

---

## 144. C-STORE

DICOM Storage über:

`C-STORE`

SOLL unterstützt werden.

---

## 145. C-FIND

DICOM Query über:

`C-FIND`

SOLL unterstützt werden.

---

## 146. C-MOVE

DICOM Retrieve über:

`C-MOVE`

SOLL unterstützt werden können.

---

## 147. C-GET

DICOM Retrieve über:

`C-GET`

SOLL unterstützt werden können.

---

## 148. Storage Commitment

DICOM Storage Commitment SOLL unterstützt werden können.

---

## 149. Storage Commitment Meaning

Eine Storage-Commitment-Bestätigung DARF erst erfolgen, wenn die durch die konfigurierte Policy geforderten Persistenzbedingungen erfüllt sind.

---

## 150. Presentation Context

DICOM Presentation Contexts MÜSSEN bei Associations berücksichtigt werden.

---

## 151. Transfer Syntax Negotiation

Transfer Syntaxes MÜSSEN ausgehandelt werden können.

---

## 152. SOP Class Negotiation

Unterstützte SOP Classes MÜSSEN deklarierbar sein.

---

## 153. Association Limits

Gleichzeitige Associations MÜSSEN begrenzbar sein.

---

## 154. Network Timeout

DICOM-Netzwerkoperationen MÜSSEN Timeouts unterstützen.

---

## 155. Network Cancellation

Lang laufende Netzwerkoperationen SOLLEN abbrechbar sein.

---

## 156. DICOMweb

PACS Compute SOLL DICOMweb unterstützen können.

---

## 157. QIDO-RS

Query based on ID for DICOM Objects:

`QIDO-RS`

SOLL unterstützt werden.

---

## 158. WADO-RS

Web Access to DICOM Objects:

`WADO-RS`

SOLL unterstützt werden.

---

## 159. STOW-RS

Store Over the Web:

`STOW-RS`

SOLL unterstützt werden.

---

## 160. DICOMweb Authentication

DICOMweb-Zugriffe MÜSSEN mit der Sicherheitsinfrastruktur integrierbar sein.

---

## 161. HTTPS

DICOMweb SOLL über geschützte Transportverbindungen betrieben werden.

---

## 162. Bulk Data

DICOMweb Bulk Data SOLL streamingfähig verarbeitet werden.

---

## 163. Metadata Representation

DICOMweb-Metadaten SOLLEN standardkonform bereitgestellt werden können.

---

## 164. Network Gateway

PACS Compute SOLL als Gateway zwischen klassischen DICOM-Diensten und DICOMweb fungieren können.

---

## 165. Protocol Independence

Die interne PACS-Objektverwaltung DARF nicht von einem einzelnen Netzwerkprotokoll abhängig sein.

---

## 166. Routing

PACS Compute SOLL regelbasiertes Routing unterstützen.

---

## 167. Routing Rule

Eine Routing Rule SOLL Bedingungen und Ziele definieren.

---

## 168. Routing Conditions

Mögliche Bedingungen umfassen:

```text
Modality
SOPClass
StudyDescription
SeriesDescription
SourceAE
Destination
Institution
```

---

## 169. Routing Destination

Ein Ziel DARF sein:

```text
LocalStorage
RemotePACS
DICOMNode
DICOMwebEndpoint
ProcessingPipeline
Archive
```

---

## 170. Routing Loop Protection

Routing MUSS gegen unbeabsichtigte Schleifen geschützt sein.

---

## 171. Routing Audit

Routingentscheidungen SOLLEN nachvollziehbar sein.

---

## 172. Store-and-Forward

PACS Compute SOLL Store-and-Forward unterstützen.

---

## 173. Offline Destination

Ist ein Ziel vorübergehend nicht erreichbar, SOLLEN Objekte lokal gepuffert werden können.

---

## 174. Retry

Fehlgeschlagene Übertragungen SOLLEN wiederholt werden können.

---

## 175. Retry Policy

Eine Retry Policy SOLL definieren:

```text
MaxAttempts
InitialDelay
Backoff
MaximumDelay
```

---

## 176. Exponential Backoff

Exponentieller Backoff SOLL unterstützt werden können.

---

## 177. Dead Letter Queue

Dauerhaft fehlgeschlagene Übertragungen SOLLEN in eine kontrollierte Fehlerwarteschlange verschoben werden können.

---

## 178. Transfer Queue

Ausgehende Transfers SOLLEN über Warteschlangen verwaltet werden.

---

## 179. Transfer Priority

Transfers SOLLEN Prioritäten besitzen können.

---

## 180. Emergency Priority

Zeitkritische Transfers DÜRFEN höhere Priorität erhalten, sofern die Policy dies erlaubt.

---

## 181. Fair Scheduling

Niedrig priorisierte Transfers DÜRFEN nicht unbegrenzt verhungern.

---

## 182. Bandwidth Control

Netzwerkbandbreite SOLL pro Ziel oder Transferklasse begrenzt werden können.

---

## 183. Concurrent Transfers

Die Anzahl paralleler Transfers MUSS konfigurierbar sein.

---

## 184. Scheduler Integration

PACS Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0002` integrieren.

---

## 185. PACS Tasks

Typische Tasks umfassen:

```text
Receive
Validate
Hash
Index
Store
Replicate
Verify
Route
Transfer
Retrieve
Migrate
Archive
Scrub
Repair
Delete
```

---

## 186. Task Dependencies

Abhängigkeiten zwischen Tasks MÜSSEN explizit darstellbar sein.

---

## 187. Priority Scheduling

PACS-Aufgaben SOLLEN priorisiert werden können.

---

## 188. Background Tasks

Nicht zeitkritische Aufgaben wie Scrubbing SOLLEN mit niedriger Priorität ausgeführt werden können.

---

## 189. Foreground Tasks

Benutzerinitiierte Abrufe SOLLEN gegenüber Hintergrundarbeiten priorisiert werden können.

---

## 190. Resource Isolation

Hintergrundaufgaben DÜRFEN interaktive Abrufe nicht unkontrolliert verdrängen.

---

## 191. Compute Runtime Integration

PACS Compute MUSS mit `NPSPEC-COMPUTE-RUNTIME-0001` integrierbar sein.

---

## 192. Memory Model

PACS Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0003` für große I/O-Puffer und Streaming verwenden können.

---

## 193. Object Model Integration

PACS Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0004` integrieren.

---

## 194. Distributed Compute

PACS Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0007` für verteilte Aufgaben verwenden können.

---

## 195. Distributed PACS

Eine PACS-Installation SOLL aus mehreren Nodes bestehen können.

---

## 196. PACS Node

Ein Node wird als:

`PACSNode`

repräsentiert.

---

## 197. Node Capabilities

Ein Node SOLL Fähigkeiten deklarieren können:

```text
INGEST
INDEX
QUERY
STORAGE
RETRIEVE
ROUTING
ARCHIVE
COMPUTE
```

---

## 198. Stateless Service

Geeignete PACS-Dienste SOLLEN zustandslos skalierbar sein.

---

## 199. Stateful Service

Zustandsbehaftete Komponenten MÜSSEN ihre Konsistenz- und Recovery-Eigenschaften explizit definieren.

---

## 200. Cluster

Mehrere PACS Nodes SOLLEN einen Cluster bilden können.

---

## 201. Cluster Membership

Cluster-Mitgliedschaft MUSS kontrolliert verwaltet werden.

---

## 202. Node Failure

Der Ausfall eines einzelnen Nodes SOLL nicht automatisch den Verlust gespeicherter Daten verursachen.

---

## 203. High Availability

Kritische PACS-Dienste SOLLEN redundant betreibbar sein.

---

## 204. Failover

Bei Ausfall eines Dienstes SOLL ein anderer geeigneter Node Aufgaben übernehmen können.

---

## 205. Split-Brain Protection

Verteilte PACS-Komponenten MÜSSEN gegen inkonsistente konkurrierende Zustände geschützt werden.

---

## 206. Metadata Consistency

Der Metadatenindex MUSS definierte Konsistenzgarantien besitzen.

---

## 207. Object Availability

Ein Objekt DARF nur als verfügbar gemeldet werden, wenn mindestens eine gültige lesbare Kopie entsprechend der Policy existiert.

---

## 208. Replication Consistency

Der gewünschte und tatsächliche Replikationszustand MÜSSEN unterscheidbar sein.

---

## 209. Desired State

Der gewünschte Speicherzustand wird als:

`DesiredStorageState`

repräsentiert.

---

## 210. Actual State

Der tatsächliche Speicherzustand wird als:

`ActualStorageState`

repräsentiert.

---

## 211. Reconciliation

PACS Compute SOLL Desired State und Actual State regelmäßig abgleichen.

---

## 212. Self-Healing Reconciliation

Abweichungen SOLLEN automatisch korrigiert werden können.

---

## 213. Backup

PACS Compute MUSS Backup-Strategien unterstützen können.

---

## 214. Backup Scope

Backups SOLLEN mindestens berücksichtigen:

```text
Objects
Index
Configuration
Policies
Audit Metadata
```

---

## 215. Backup Independence

Backup DARF nicht mit normaler Replikation gleichgesetzt werden.

---

## 216. Point-in-Time Recovery

Index- und Konfigurationsdaten SOLLEN Point-in-Time-Recovery unterstützen können.

---

## 217. Restore

Backups MÜSSEN kontrolliert wiederherstellbar sein.

---

## 218. Restore Validation

Wiederhergestellte Objekte MÜSSEN auf Integrität überprüfbar sein.

---

## 219. Disaster Recovery

PACS Compute SOLL Disaster-Recovery-Szenarien unterstützen.

---

## 220. Recovery Site

Ein alternatives Recovery-System DARF bereitgestellt werden.

---

## 221. Recovery Point Objective

Eine Installation SOLL ein:

`RPO`

definieren können.

---

## 222. Recovery Time Objective

Eine Installation SOLL ein:

`RTO`

definieren können.

---

## 223. Lifecycle Management

PACS Compute MUSS Objektlebenszyklen verwalten können.

---

## 224. Lifecycle Policy

Eine Lifecycle Policy SOLL Regeln definieren können für:

```text
Retention
Tiering
Archiving
Replication
Deletion
```

---

## 225. Retention

Aufbewahrungszeiten MÜSSEN definierbar sein.

---

## 226. Retention Lock

Bestimmte Objekte SOLLEN gegen vorzeitige Löschung geschützt werden können.

---

## 227. Legal Hold

Ein:

`LegalHold`

SOLL die normale Löschlogik überschreiben können.

---

## 228. Expiration

Nach Ablauf einer Retention Policy DARF ein Objekt zur Löschung freigegeben werden.

---

## 229. Deletion

Löschung MUSS als kontrollierter Prozess erfolgen.

---

## 230. Logical Delete

Ein Objekt SOLL zunächst logisch gelöscht beziehungsweise zur Löschung markiert werden können.

---

## 231. Physical Delete

Physische Kopien DÜRFEN erst entsprechend der definierten Policy entfernt werden.

---

## 232. Referential Integrity

Objekte mit notwendigen Referenzbeziehungen SOLLEN nicht unkontrolliert entfernt werden.

---

## 233. Delete Audit

Löschvorgänge MÜSSEN auditierbar sein.

---

## 234. Secure Deletion

Wo Storage Backend und Policy dies erfordern, SOLL sichere Löschung unterstützt werden können.

---

## 235. De-Identification

PACS Compute MUSS mit der De-Identification-Infrastruktur von `NPSPEC-COMPUTE-DICOM-0001` integrierbar sein.

---

## 236. De-Identified Repository

De-identifizierte Daten SOLLEN in logisch getrennten Repositories gespeichert werden können.

---

## 237. Original Separation

Originale und de-identifizierte Objekte MÜSSEN eindeutig unterscheidbar sein.

---

## 238. UID Remapping

De-Identification-Pipelines MÜSSEN konsistentes UID-Remapping unterstützen.

---

## 239. Export

PACS Compute SOLL kontrollierten Export unterstützen.

---

## 240. Export Job

Ein Export wird als:

`PACSExportJob`

repräsentiert.

---

## 241. Export Selection

Exportdaten SOLLEN nach:

```text
Study
Series
Instance
Query
```

ausgewählt werden können.

---

## 242. Export Validation

Vor Export SOLL geprüft werden, ob alle erwarteten Objekte verfügbar sind.

---

## 243. Export De-Identification

Ein Export DARF optional eine De-Identification-Pipeline verwenden.

---

## 244. Export Manifest

Ein Export SOLL ein Manifest erzeugen können.

---

## 245. Manifest Fields

Ein Manifest SOLL mindestens technische Informationen enthalten können:

```text
ExportId
ObjectCount
TotalSize
ObjectIdentifiers
Hashes
CreationTime
```

---

## 246. Import

PACS Compute SOLL kontrollierte Batch-Imports unterstützen.

---

## 247. Import Manifest

Importe DÜRFEN über Manifeste validiert werden.

---

## 248. Import Resume

Große Importe SOLLEN nach Unterbrechungen fortgesetzt werden können.

---

## 249. Export Resume

Große Exporte SOLLEN fortgesetzt werden können.

---

## 250. Checkpointing

Lang laufende PACS-Aufgaben SOLLEN Checkpoints unterstützen.

---

## 251. Job Object

Lang laufende Aufgaben werden als:

`PACSJob`

repräsentiert.

---

## 252. Job State

Ein Job SOLL mindestens besitzen:

```text
QUEUED
RUNNING
PAUSED
COMPLETED
FAILED
CANCELLED
```

---

## 253. Job Progress

Ein Job SOLL Fortschritt bereitstellen können.

---

## 254. Job Persistence

Lang laufende Jobs SOLLEN Neustarts des Systems überstehen können.

---

## 255. Job Resume

Unterbrochene kompatible Jobs SOLLEN fortgesetzt werden können.

---

## 256. Job Cancellation

Jobs MÜSSEN kontrolliert abbrechbar sein.

---

## 257. Cancellation Safety

Ein Abbruch DARF keine unvollständigen Daten als vollständig markieren.

---

## 258. Security Context

Jede PACS-Operation SOLL einem Sicherheitskontext zugeordnet sein.

---

## 259. Authentication

Identitäten MÜSSEN über die zuständige NovaOS-Sicherheitsinfrastruktur authentifizierbar sein.

---

## 260. Authorization

Operationen MÜSSEN autorisierbar sein.

---

## 261. Access Control

Zugriffskontrolle SOLL mindestens berücksichtigen können:

```text
User
Service
Role
Application
Purpose
Resource
Operation
```

---

## 262. Study-Level Access

Zugriffsrechte SOLLEN auf Studienebene beschränkt werden können.

---

## 263. Series-Level Access

Zugriffsrechte SOLLEN auf Serienebene beschränkt werden können.

---

## 264. Instance-Level Access

Falls erforderlich, SOLLEN Rechte auf Instanzebene durchsetzbar sein.

---

## 265. Service Identity

Automatisierte PACS-Dienste MÜSSEN eigene technische Identitäten verwenden können.

---

## 266. No Shared Superuser

Dienste SOLLEN nicht auf ein gemeinsames unbeschränktes PACS-Konto angewiesen sein.

---

## 267. Audit

PACS Compute MUSS sicherheitsrelevante Operationen auditierbar machen.

---

## 268. Audit Event

Ein Audit Event wird als:

`PACSAuditEvent`

repräsentiert.

---

## 269. Audit Operations

Mindestens folgende Ereignisse SOLLEN auditierbar sein:

```text
STORE
QUERY
RETRIEVE
EXPORT
IMPORT
DELETE
DEIDENTIFY
ROUTE
REPLICATE
RESTORE
POLICY_CHANGE
AUTHORIZATION_FAILURE
```

---

## 270. Audit Fields

Ein Audit Event SOLL mindestens enthalten können:

```text
EventId
Timestamp
Actor
Operation
Resource
Result
Source
Destination
```

---

## 271. Audit Privacy

Auditdaten SOLLEN nur die für Nachvollziehbarkeit erforderlichen personenbezogenen Informationen enthalten.

---

## 272. Audit Integrity

Auditinformationen SOLLEN gegen unbemerkte Manipulation geschützt werden.

---

## 273. Audit Retention

Auditdaten SOLLEN eigene Retention Policies besitzen können.

---

## 274. Encryption in Transit

Medizinische Daten SOLLEN bei Übertragung geschützt werden.

---

## 275. Encryption at Rest

Storage Backends SOLLEN Verschlüsselung ruhender Daten unterstützen können.

---

## 276. Key Separation

Verschlüsselungsschlüssel SOLLEN getrennt von den verschlüsselten medizinischen Daten verwaltet werden.

---

## 277. Key Rotation

Schlüsselrotation SOLL unterstützt werden können.

---

## 278. Secrets

Passwörter, Schlüssel und Tokens DÜRFEN nicht als normale PACS-Konfiguration im Klartext behandelt werden.

---

## 279. Resource Limits

PACS Compute MUSS Ressourcenlimits unterstützen.

---

## 280. Ingestion Limit

Maximale parallele Ingestion-Vorgänge SOLLEN begrenzbar sein.

---

## 281. Query Limit

Gleichzeitige Abfragen SOLLEN begrenzbar sein.

---

## 282. Retrieve Limit

Gleichzeitige Abrufe SOLLEN begrenzbar sein.

---

## 283. Transfer Limit

Ausgehende Transfers SOLLEN begrenzbar sein.

---

## 284. Storage Quota

Storage Quotas SOLLEN definierbar sein.

---

## 285. Tenant Quota

Bei Multi-Tenant-Betrieb SOLLEN getrennte Quotas unterstützt werden können.

---

## 286. Multi-Tenancy

PACS Compute DARF mehrere logisch getrennte Mandanten unterstützen.

---

## 287. Tenant Isolation

Mandantendaten MÜSSEN voneinander isoliert werden können.

---

## 288. Tenant Index

Indexdaten MÜSSEN mandantenbezogen trennbar sein.

---

## 289. Tenant Storage

Speicherbereiche SOLLEN mandantenbezogen getrennt werden können.

---

## 290. Cross-Tenant Access

Mandantenübergreifender Zugriff DARF nur über explizite Policy erfolgen.

---

## 291. Observability

PACS Compute MUSS Betriebsmetriken bereitstellen können.

---

## 292. Metrics

Mindestens folgende Metriken SOLLEN verfügbar sein:

```text
ObjectsReceived
ObjectsStored
ObjectsRejected
ObjectsQuarantined
BytesStored
QueriesPerSecond
RetrievesPerSecond
TransferQueueDepth
FailedTransfers
StorageLatency
QueryLatency
RetrieveLatency
ReplicationLag
CorruptReplicas
```

---

## 293. Storage Metrics

Pro Storage Backend SOLLEN Kapazitäts- und Leistungsdaten verfügbar sein.

---

## 294. Node Metrics

PACS Nodes SOLLEN ihren Zustand melden.

---

## 295. Health State

Ein Node SOLL mindestens folgende Zustände besitzen:

```text
HEALTHY
DEGRADED
UNAVAILABLE
MAINTENANCE
```

---

## 296. Health Check

PACS-Dienste SOLLEN maschinenlesbare Health Checks bereitstellen.

---

## 297. Readiness

Ein Dienst SOLL getrennt melden können, ob er läuft und ob er neue Arbeit annehmen kann.

---

## 298. Backpressure

Bei Ressourcenüberlastung MUSS PACS Compute Backpressure anwenden können.

---

## 299. Queue Growth Protection

Unbegrenztes Wachstum von Warteschlangen MUSS verhindert werden.

---

## 300. Load Shedding

Nicht kritische Aufgaben DÜRFEN bei extremer Überlastung verzögert oder abgewiesen werden.

---

## 301. Priority Protection

Kritische medizinische Abrufe SOLLEN gegenüber Hintergrundarbeiten priorisierbar sein.

---

## 302. Performance

PACS Compute SOLL für große Datenmengen skalieren.

---

## 303. Horizontal Scaling

Geeignete PACS-Dienste SOLLEN horizontal skalierbar sein.

---

## 304. Partitioning

Index- und Storage-Systeme SOLLEN partitioniert werden können.

---

## 305. Study Locality

Daten einer Studie SOLLEN nach Möglichkeit lokalitätsbewusst gespeichert beziehungsweise verarbeitet werden können.

---

## 306. Metadata Locality

Häufig benötigte Metadaten SOLLEN nahe an Query-Diensten verfügbar sein.

---

## 307. Compute Locality

Radiology-Compute-Aufgaben SOLLEN nach Möglichkeit auf Nodes ausgeführt werden, die bereits Zugriff auf die benötigten Daten besitzen.

---

## 308. Zero-Copy

Geeignete lokale Datenpfade SOLLEN unnötige Kopien vermeiden.

---

## 309. Streaming Pipeline

PACS-Objekte SOLLEN direkt aus Storage in Verarbeitungs- oder Netzwerkpipelines gestreamt werden können.

---

## 310. Radiology Compute Integration

PACS Compute SOLL `NPSPEC-COMPUTE-RADIOLOGY-0001` integrieren können.

---

## 311. Processing Pipeline

Gespeicherte Serien SOLLEN an Radiology-Compute-Pipelines übergeben werden können.

---

## 312. Derived Result Ingestion

Von Radiology Compute erzeugte DICOM-Ergebnisse SOLLEN erneut in PACS aufgenommen werden können.

---

## 313. Derived Result Link

Abgeleitete Ergebnisse SOLLEN ihre Quelldaten referenzieren.

---

## 314. Processing Queue

PACS Compute SOLL medizinische Verarbeitungsaufgaben an Compute-Pipelines übergeben können.

---

## 315. Automatic Processing

Automatische Verarbeitung DARF durch Routing- oder Ingestion-Regeln ausgelöst werden.

---

## 316. Processing Failure

Ein Fehler einer optionalen Compute-Pipeline DARF das unveränderte Originalobjekt nicht beschädigen.

---

## 317. AI Integration

PACS Compute DARF Studien an autorisierte KI-Verarbeitungspipelines übergeben.

---

## 318. AI Result Separation

KI-Ergebnisse MÜSSEN als abgeleitete Daten identifizierbar bleiben.

---

## 319. AI Authorization

KI-Dienste DÜRFEN nur auf Daten zugreifen, für die sie autorisiert wurden.

---

## 320. External System Integration

PACS Compute SOLL Integrationsschnittstellen für andere medizinische Systeme bereitstellen können.

---

## 321. Event Architecture

PACS Compute SOLL strukturierte Ereignisse erzeugen können.

---

## 322. PACS Event

Ein Ereignis wird als:

`PACSEvent`

repräsentiert.

---

## 323. Event Types

Mindestens folgende Ereignisse SOLLEN möglich sein:

```text
OBJECT_RECEIVED
OBJECT_AVAILABLE
OBJECT_REJECTED
STUDY_COMPLETE
SERIES_COMPLETE
OBJECT_RETRIEVED
OBJECT_REPLICATED
OBJECT_ARCHIVED
OBJECT_DELETED
STORAGE_DEGRADED
REPLICA_CORRUPT
JOB_FAILED
```

---

## 324. Event Consumers

Autorisierte Komponenten SOLLEN PACS-Ereignisse abonnieren können.

---

## 325. Event Delivery

Die Ereignisübertragung SOLL definierte Zustellgarantien besitzen.

---

## 326. Idempotency

Event Consumer SOLLEN Ereignisse idempotent verarbeiten können.

---

## 327. Idempotent Ingestion

Wiederholtes Einspielen desselben Objekts SOLL keinen unkontrollierten mehrfachen Zustand erzeugen.

---

## 328. Request ID

Netzwerk- und Joboperationen SOLLEN eine Request ID besitzen.

---

## 329. Correlation ID

Zusammengehörige Operationen SOLLEN über eine Correlation ID verbunden werden können.

---

## 330. Trace

Eine Verarbeitung SOLL über mehrere PACS-Komponenten hinweg nachvollziehbar sein.

---

## 331. Diagnostics

PACS Compute MUSS strukturierte Diagnosen bereitstellen.

---

## 332. Diagnostic Classes

Mindestens folgende Klassen SOLLEN existieren:

```text
INVALID_OBJECT
DICOM_VALIDATION_FAILED
DUPLICATE_OBJECT
CONFLICTING_DUPLICATE
INDEX_FAILURE
STORAGE_FAILURE
STORAGE_FULL
OBJECT_NOT_FOUND
OBJECT_CORRUPT
REPLICA_CORRUPT
REPLICATION_FAILED
QUERY_FAILED
RETRIEVE_FAILED
TRANSFER_FAILED
ROUTING_FAILED
AUTHENTICATION_FAILED
AUTHORIZATION_FAILED
RETENTION_BLOCKED
LEGAL_HOLD
BACKUP_FAILED
RESTORE_FAILED
NODE_UNAVAILABLE
CLUSTER_DEGRADED
TIMEOUT
OUT_OF_MEMORY
CANCELLED
```

---

## 333. Diagnostic Severity

Mindestens:

```text
INFO
WARNING
ERROR
FATAL
```

SOLLTEN unterstützt werden.

---

## 334. Diagnostic Context

Diagnosen SOLLEN technische Kontextinformationen enthalten können:

```text
RequestId
JobId
NodeId
StudyInstanceUID
SeriesInstanceUID
SOPInstanceUID
StorageBackend
RemoteEndpoint
```

---

## 335. Privacy-Aware Diagnostics

Diagnosen DÜRFEN keine unnötigen personenbezogenen Informationen enthalten.

---

## 336. Testing — Ingestion

Mindestens folgende Fälle MÜSSEN getestet werden:

```text
ValidObject
InvalidObject
PartialObject
ExactDuplicate
ConflictingDuplicate
StorageFailure
IndexFailure
Cancellation
```

---

## 337. Testing — Query

Mindestens:

```text
StudyQuery
SeriesQuery
InstanceQuery
UIDQuery
DateRange
ModalityFilter
Pagination
AuthorizationFilter
```

MÜSSEN getestet werden.

---

## 338. Testing — Retrieve

Mindestens:

```text
SingleObject
Series
Study
MetadataOnly
Streaming
MissingObject
UnauthorizedObject
```

MÜSSEN getestet werden.

---

## 339. Testing — Storage

Mindestens:

```text
Put
Get
RangeRead
Verify
Delete
BackendFailure
StorageFull
```

MÜSSEN getestet werden.

---

## 340. Testing — Integrity

Mindestens:

```text
ValidHash
CorruptObject
CorruptReplica
ReplicaRepair
MigrationVerification
```

MÜSSEN getestet werden.

---

## 341. Testing — Replication

Mindestens:

```text
ReplicaCreation
NodeFailure
ReplicaLoss
ReplicaRepair
ReadFromAlternateReplica
```

SOLLTEN getestet werden.

---

## 342. Testing — Network

Für implementierte Protokolle SOLLEN mindestens getestet werden:

```text
C-ECHO
C-STORE
C-FIND
C-MOVE
C-GET
QIDO-RS
WADO-RS
STOW-RS
```

---

## 343. Testing — Security

Mindestens:

```text
UnauthorizedQuery
UnauthorizedRetrieve
UnauthorizedStore
UnauthorizedDelete
TenantIsolation
InvalidCredentials
ExpiredCredentials
```

MÜSSEN getestet werden.

---

## 344. Testing — Lifecycle

Mindestens:

```text
Retention
Expiration
LegalHold
Archive
Migration
LogicalDelete
PhysicalDelete
```

SOLLTEN getestet werden.

---

## 345. Testing — Recovery

Mindestens:

```text
NodeRestart
ServiceRestart
IndexRecovery
InterruptedIngestion
InterruptedMigration
InterruptedTransfer
BackupRestore
```

SOLLTEN getestet werden.

---

## 346. Testing — Distributed

Mindestens:

```text
NodeLoss
NetworkPartition
ReplicaLag
Failover
Reconciliation
```

SOLLTEN getestet werden.

---

## 347. Stress Tests

Mindestens:

```text
ManySmallObjects
VeryLargeObjects
LargeMultiframeObjects
LargeStudy
ManyConcurrentStores
ManyConcurrentQueries
ManyConcurrentRetrieves
TransferQueuePressure
StoragePressure
IndexPressure
NodeFailureUnderLoad
```

SOLLTEN getestet werden.

---

## 348. Fuzz Testing

Netzwerkparser, DICOM-Eingabeschichten und externe Protokollschnittstellen SOLLEN Fuzz Testing unterzogen werden.

---

## 349. Performance Tests

Mindestens folgende Größen SOLLEN messbar sein:

```text
StoreThroughput
QueryLatency
RetrieveLatency
StreamingThroughput
IndexThroughput
ReplicationThroughput
MigrationThroughput
```

---

## 350. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens unterstützen:

```text
DICOM Object Ingestion
DICOM Validation

Study
Series
SOP Instance

StudyInstanceUID
SeriesInstanceUID
SOPInstanceUID
SOPClassUID

Metadata Index

Study Query
Series Query
Instance Query

Object Retrieve
Series Retrieve
Study Retrieve

Streaming

Local Storage Backend
Storage Abstraction

Object Fingerprint
Integrity Verification

Duplicate Detection
Conflicting Duplicate Detection

Atomic Ingestion
Temporary Storage

Access Control Integration
Audit Integration

Retention Architecture
Controlled Deletion

Scheduler Integration
Memory Model Integration
Object Model Integration

Diagnostics
Metrics
Cancellation
```

---

## 351. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

```text
DICOM Networking

C-ECHO
C-STORE
C-FIND
C-MOVE
C-GET
Storage Commitment

DICOMweb

QIDO-RS
WADO-RS
STOW-RS

Distributed Storage
Object Storage
Tiered Storage

Hot Storage
Warm Storage
Cold Storage
Archive

Replication
Automatic Replica Repair
Storage Scrubbing

High Availability
Failover
Cluster Operation

Routing
Store-and-Forward
Transfer Queues

Backup
Point-in-Time Recovery
Disaster Recovery

Multi-Tenancy

De-Identification Integration

Radiology Compute Integration
AI Pipeline Integration

Distributed Compute
HPC Integration
```

---

## 352. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-PACS-0001` MUSS:

1. DICOM-Objekte aufnehmen können.
2. DICOM Compute integrieren.
3. eingehende Objekte validieren können.
4. unvollständige Objekte erkennen.
5. ungültige Objekte kontrolliert behandeln.
6. Study Instance UID speichern.
7. Series Instance UID speichern.
8. SOP Instance UID speichern.
9. SOP Class UID speichern.
10. logische und physische Objektidentität trennen.
11. einen Metadatenindex bereitstellen.
12. Studien indexieren.
13. Serien indexieren.
14. Instanzen indexieren.
15. UID-basierte Abfragen ermöglichen.
16. Studien abfragen können.
17. Serien abfragen können.
18. Instanzen abfragen können.
19. einzelne Objekte abrufen können.
20. Serien abrufen können.
21. Studien abrufen können.
22. große Objekte streamingfähig abrufen können.
23. Metadaten ohne vollständige Pixelübertragung bereitstellen können.
24. Storage Backends abstrahieren.
25. mindestens ein lokales Storage Backend unterstützen.
26. Objektintegrität prüfen können.
27. Content Fingerprints erzeugen können.
28. exakte Duplikate erkennen können.
29. widersprüchliche Duplikate erkennen können.
30. bestehende Objekte nicht stillschweigend überschreiben.
31. atomare Ingestion ermöglichen.
32. temporäre unvollständige Daten von verfügbaren Daten trennen.
33. Storage- und Indexzustand konsistent halten.
34. Objekte neu indexieren können.
35. Zugriffe autorisieren können.
36. nicht autorisierte Objekte auch vor Metadatenabfragen schützen.
37. Abrufe auditierbar machen.
38. Löschungen auditierbar machen.
39. Retention Policies architektonisch unterstützen.
40. kontrollierte Löschung ermöglichen.
41. Originaldaten und abgeleitete Daten unterscheiden.
42. DICOM-De-Identification integrieren können.
43. Netzwerkdaten als nicht vertrauenswürdig behandeln.
44. Ressourcenlimits unterstützen.
45. Timeouts unterstützen.
46. kontrollierten Abbruch unterstützen.
47. unvollständige Ergebnisse nicht als vollständig markieren.
48. strukturierte Diagnosen bereitstellen.
49. datenschutzfreundliche Diagnostik ermöglichen.
50. Betriebsmetriken bereitstellen.
51. Storage-Kapazität überwachen können.
52. Fehlerzustände von Storage Backends erkennen.
53. Jobs beziehungsweise lang laufende Operationen modellieren können.
54. Jobfortschritt bereitstellen können.
55. den Compute Scheduler integrieren.
56. das Compute Memory Model integrieren.
57. das Compute Object Model integrieren.
58. verteilte Erweiterung architektonisch ermöglichen.
59. Radiology Compute integrieren können.
60. Erweiterungen ohne Änderung des grundlegenden PACS-Objektmodells ermöglichen.

---

## 353. Empfohlene interne Komponenten

Eine vollständige Implementierung SOLL logisch folgende Komponenten besitzen:

```text
NovaPACSCompute
 |
 +-- PACSRuntime
 |
 +-- PACSIngestion
 |
 +-- PACSValidator
 |
 +-- PACSIndex
 |
 +-- PACSQueryEngine
 |
 +-- PACSRetrieveEngine
 |
 +-- PACSStorageManager
 |
 +-- PACSReplicationManager
 |
 +-- PACSIntegrityManager
 |
 +-- PACSCache
 |
 +-- PACSLifecycleManager
 |
 +-- PACSRoutingEngine
 |
 +-- PACSTransferManager
 |
 +-- PACSNetworkServices
 |
 +-- PACSDICOMweb
 |
 +-- PACSJobManager
 |
 +-- PACSSecurityBridge
 |
 +-- PACSAuditBridge
 |
 +-- PACSRecoveryManager
 |
 +-- PACSDiagnostics
 |
 +-- PACSProfiler
```

Diese Struktur ist logisch und schreibt keine konkrete Datei-, Klassen- oder Modulstruktur vor.

---

## 354. Beziehung zu `NPSPEC-COMPUTE-DICOM-0001`

DICOM Compute stellt die Interpretation medizinischer DICOM-Objekte bereit.

```text
Raw DICOM
    |
    v
DICOM Compute
    |
    +--> Parse
    +--> Validate
    +--> Metadata
    +--> Pixel Structure
    |
    v
PACS Compute
    |
    +--> Index
    +--> Store
    +--> Query
    +--> Retrieve
    +--> Archive
```

PACS Compute DARF die DICOM-Parserlogik nicht unnötig duplizieren.

---

## 355. Beziehung zu `NPSPEC-COMPUTE-RADIOLOGY-0001`

Radiology Compute verarbeitet medizinische Bilddaten.

PACS Compute verwaltet deren Speicherung und Bereitstellung.

```text
PACS
 |
 v
Study / Series
 |
 v
Radiology Compute
 |
 v
Derived Result
 |
 v
PACS
```

---

## 356. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

Die Compute Runtime stellt gemeinsame Ressourcen für:

```text
Validation
Hashing
Indexing
Compression
Streaming
Background Processing
```

bereit.

---

## 357. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Scheduler verwaltet:

```text
Ingestion
Indexing
Replication
Transfer
Migration
Scrubbing
Repair
Archiving
```

---

## 358. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Das Memory Model verwaltet:

```text
Network Buffers
File Buffers
Streaming Buffers
Metadata Buffers
Temporary Object Buffers
```

---

## 359. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0004`

PACS-Objekte SOLLEN auf dem gemeinsamen Compute Object Model aufbauen.

---

## 360. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0007`

Distributed Compute stellt Infrastruktur für verteilte PACS-Aufgaben bereit.

---

## 361. Beziehung zu `NPSPEC-COMPUTE-HPC-0001`

HPC Compute DARF für große Batch-Verarbeitungs-, Reindexierungs-, Migrations- und medizinische Compute-Workloads verwendet werden.

---

## 362. Grundlegender Datenfluss

Der grundlegende PACS-Datenfluss lautet:

```text
Imaging Device
      |
      v
Receive
      |
      v
Validate
      |
      v
Fingerprint
      |
      v
Temporary Store
      |
      v
Index
      |
      v
Commit
      |
      v
Authoritative Storage
      |
      +----------------+
      |                |
      v                v
Replication         Query
      |                |
      v                v
Archive            Retrieve
```

---

## 363. Abrufmodell

```text
Client
  |
  v
Query
  |
  v
Authorization
  |
  v
Index
  |
  v
Object Resolution
  |
  v
Replica Selection
  |
  v
Storage
  |
  v
Streaming Retrieve
```

---

## 364. Replikationsmodell

```text
Logical Object
      |
      +------------------+
      |                  |
      v                  v
Replica A            Replica B
Node A               Node B
      |
      +---------+
                |
                v
          Integrity State
```

Der Zustand des logischen Objekts ist von einzelnen Replikaten getrennt.

---

## 365. Self-Healing-Modell

```text
Periodic Scrub
      |
      v
Verify Replica
      |
      +-- Valid --------> Continue
      |
      +-- Corrupt
            |
            v
      Find Valid Replica
            |
            v
         Rebuild
            |
            v
         Verify
            |
            v
        Available
```

---

## 366. Lifecycle-Modell

```text
Ingest
  |
  v
Hot Storage
  |
  v
Warm Storage
  |
  v
Cold Storage
  |
  v
Archive
  |
  v
Retention Expired
  |
  v
Deletion Eligibility
  |
  v
Controlled Delete
```

Legal Hold oder andere Policies DÜRFEN diesen Ablauf unterbrechen.

---

## 367. Sicherheitsmodell

```text
Request
  |
  v
Authenticate
  |
  v
Authorize
  |
  v
Policy Check
  |
  v
PACS Operation
  |
  v
Audit
```

---

## 368. Compute-Integrationsmodell

```text
PACS Study
    |
    v
Retrieve / Stream
    |
    v
DICOM Compute
    |
    v
Radiology Compute
    |
    v
Derived DICOM Object
    |
    v
PACS Ingestion
```

---

## 369. Performance-Grundsatz

PACS Compute SOLL medizinische Objekte möglichst ohne unnötige vollständige Kopien zwischen:

```text
Network
Storage
DICOM Compute
Radiology Compute
```

bewegen.

Streaming und Range Reads SOLLEN bevorzugt werden, wenn eine vollständige Materialisierung nicht erforderlich ist.

---

## 370. Zuverlässigkeitsgrundsatz

Eine PACS-Implementierung DARF die Existenz einer Datei nicht mit garantierter Datenintegrität gleichsetzen.

Ein Objekt gilt nur dann als verlässlich verfügbar, wenn seine Speicher- und Integritätsbedingungen entsprechend der aktiven Policy erfüllt sind.

---

## 371. Datenhoheitsgrundsatz

Der Metadatenindex ist nicht die alleinige autoritative Kopie des medizinischen Objekts.

Ebenso ist ein physischer Storage-Pfad nicht die logische Identität des Objekts.

Die Architektur trennt:

```text
Object Identity
Metadata
Index
Storage Location
Replica State
Lifecycle State
Access Policy
```

---

## 372. Erweiterbarkeit

Neue:

```text
Storage Backends
Index Backends
DICOM Services
DICOMweb Services
Routing Rules
Archive Systems
Replication Strategies
Lifecycle Policies
Compute Pipelines
```

SOLLTEN integrierbar sein, ohne das grundlegende PACS-Objektmodell zu verändern.

---

## 373. Empfohlene Folge-Spezifikationen

Auf dieser Basisspezifikation SOLLEN weitere Detail-Spezifikationen aufbauen.

Empfohlen:

```text
NPSPEC-COMPUTE-PACS-INGEST-0001
PACS Ingestion

NPSPEC-COMPUTE-PACS-INDEX-0001
PACS Metadata Index

NPSPEC-COMPUTE-PACS-QUERY-0001
PACS Query Engine

NPSPEC-COMPUTE-PACS-RETRIEVE-0001
PACS Retrieve Engine

NPSPEC-COMPUTE-PACS-STORAGE-0001
PACS Storage Architecture

NPSPEC-COMPUTE-PACS-REPLICATION-0001
PACS Replication

NPSPEC-COMPUTE-PACS-INTEGRITY-0001
PACS Data Integrity

NPSPEC-COMPUTE-PACS-CACHE-0001
PACS Cache

NPSPEC-COMPUTE-PACS-ROUTING-0001
PACS Routing

NPSPEC-COMPUTE-PACS-DICOMNET-0001
DICOM Network Services

NPSPEC-COMPUTE-PACS-DICOMWEB-0001
DICOMweb Services

NPSPEC-COMPUTE-PACS-LIFECYCLE-0001
PACS Lifecycle Management

NPSPEC-COMPUTE-PACS-ARCHIVE-0001
PACS Archive

NPSPEC-COMPUTE-PACS-SECURITY-0001
PACS Security

NPSPEC-COMPUTE-PACS-AUDIT-0001
PACS Audit

NPSPEC-COMPUTE-PACS-HA-0001
PACS High Availability

NPSPEC-COMPUTE-PACS-RECOVERY-0001
PACS Backup and Recovery

NPSPEC-COMPUTE-PACS-DISTRIBUTED-0001
Distributed PACS

NPSPEC-COMPUTE-PACS-TEST-0001
PACS Conformance Tests
```

---

## 374. Zusammenfassung

`NPSPEC-COMPUTE-PACS-0001` definiert die gemeinsame PACS-Infrastruktur der NovaOS-Compute-Architektur.

PACS Compute behandelt medizinische Daten als logisch identifizierte und verwaltete Objekte.

Die grundlegende Schichtung lautet:

```text
DICOM Objects
      |
      v
DICOM Compute
      |
      v
PACS Compute
      |
 +----+----+----+----+----+
 |         |    |    |    |
 v         v    v    v    v
Index    Store Query Get Archive
```

Die Architektur unterstützt:

```text
Ingestion
Validation
Indexing
Storage
Query
Retrieve
Streaming
Routing
Replication
Integrity Checking
Self-Healing
Tiered Storage
Archiving
Lifecycle Management
Backup
Recovery
```

Netzwerkzugriffe können über:

```text
DICOM
C-ECHO
C-STORE
C-FIND
C-MOVE
C-GET
Storage Commitment

DICOMweb
QIDO-RS
WADO-RS
STOW-RS
```

bereitgestellt werden.

Die Architektur trennt konsequent:

```text
Object Identity
DICOM Metadata
Index
Physical Storage
Replica State
Lifecycle State
Security Policy
```

voneinander.

PACS Compute kann von einer lokalen Einzelplatzinstallation bis zu einem verteilten medizinischen Archiv skalieren:

```text
Single Node
    |
    v
Multi-Storage
    |
    v
Cluster
    |
    v
Distributed PACS
    |
    v
Multi-Site Archive
```

Das grundlegende Architekturprinzip lautet:

> NovaOS PACS Compute behandelt medizinische Bilddaten nicht als gewöhnliche Dateien in Verzeichnissen, sondern als dauerhaft identifizierte, indexierte, validierte, integritätsgeschützte und policygesteuerte medizinische Objekte. Die logische Identität eines Objekts bleibt unabhängig von seinem physischen Speicherort. Dadurch können Speicherung, Suche, Abruf, Replikation, Selbstheilung, Archivierung und Compute-Verarbeitung unabhängig voneinander skalieren, während Originaldaten, abgeleitete Daten, Zugriffsrechte und Lebenszykluszustände eindeutig nachvollziehbar bleiben.