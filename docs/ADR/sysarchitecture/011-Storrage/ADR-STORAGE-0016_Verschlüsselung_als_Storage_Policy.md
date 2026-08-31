# ADR-STORAGE-0016 – Verschlüsselung als Storage Policy

## Status

Angenommen

## Kategorie

Storage / Encryption / Security / Data Sovereignty / Policy

## Kontext

NovaOS muss persistente Daten gegen unautorisierten Zugriff schützen können.

Klassische Systeme behandeln Verschlüsselung häufig als Eigenschaft eines gesamten Datenträgers, eines Dateisystems oder einer einzelnen Anwendung. Das ist für das native NovaOS-Storage-Modell zu unflexibel.

Storage Objects können unterschiedliche Schutzanforderungen besitzen. Diese Anforderungen können sich aus Security, Data Sovereignty, Semantic Type, Trust, Benutzerentscheidung oder Systemrolle ergeben.

Verschlüsselung soll deshalb nicht an ein bestimmtes Dateisystem oder physisches Medium gekoppelt werden.

---

## Entscheidung

NovaOS behandelt **Verschlüsselung als deklarative Storage Policy**.

```text
Storage Object
      ↓
Storage Policy
      ↓
Encryption Requirement
      ↓
Storage Provider
```

Die Policy beschreibt die erforderliche Schutzwirkung. Der Storage Provider beziehungsweise die Security-Infrastruktur bestimmt innerhalb dieser Anforderungen die konkrete technische Umsetzung.

---

## Grundprinzip

```text
Encryption is a storage requirement,
not a filesystem special case.
```

---

## Deklarative Verschlüsselung

Ein Storage Object, Volume oder definierter Storage Scope kann eine Encryption Policy besitzen.

Diese kann beispielsweise ausdrücken:

```text
Encryption
 ├── Required
 ├── Preferred
 └── Not Required
```

Zusätzliche Anforderungen können über entsprechende Security Contracts definiert werden.

Die konkrete Algorithmus-, Schlüssel- und Formatdefinition gehört in die zugehörigen NPSPECs.

---

## Hard Constraint

Ist Verschlüsselung als zwingend erforderlich definiert, wird sie zu einem Hard Constraint.

```text
Storage Request
      ↓
Encryption Required
      ↓
Eligible Providers
```

Ein Provider, der die geforderte Schutzwirkung nicht bereitstellen kann, darf für diesen Storage Request nicht ausgewählt werden.

NovaOS darf eine verpflichtende Encryption Policy nicht stillschweigend abschwächen.

---

## Objekt-, Volume- und Provider-Ebene

Verschlüsselung kann auf unterschiedlichen Ebenen technisch umgesetzt werden.

Beispiele:

- einzelnes Storage Object,
- Gruppe von Objekten,
- Named Volume,
- Storage Provider,
- physisches Medium.

Die logische Storage Policy bleibt von der konkreten Implementierungsebene getrennt.

Dadurch kann ein Objekt seine Schutzanforderungen auch bei Migration auf einen anderen Provider beibehalten.

---

## Migration und Location Transparency

Bei Migration oder Replikation muss die Encryption Policy erhalten bleiben.

```text
Encrypted Object
      ↓
Migration
      ↓
Policy Evaluation
      ↓
Compatible Destination
```

Location Transparency darf nicht dazu führen, dass Daten auf einem Ziel mit schwächerem Schutz gespeichert werden.

Die Policy reist logisch mit dem Storage Object.

---

## Data Sovereignty

Encryption Policies sind Bestandteil der Data-Sovereignty-Architektur.

Eine Data-Sovereignty Policy kann beispielsweise verlangen, dass bestimmte Daten ausschließlich verschlüsselt gespeichert werden.

Verschlüsselung allein ersetzt jedoch keine Regeln über:

- zulässige Storage Locations,
- Informationsflüsse,
- Zugriffsrechte,
- Trust Domains.

---

## Schlüsselverwaltung

Die Storage-Schicht darf Schlüssel nicht als gewöhnliche Storage-Metadaten behandeln.

Schlüsselidentität, Bereitstellung, Rotation und Schutz werden durch die Security- und Trust-Infrastruktur kontrolliert.

```text
Storage Policy
      ↓
Key Reference / Security Context
      ↓
Protected Key Infrastructure
```

Storage Objects sollen auf erforderliche Schlüsselkontexte verweisen können, ohne geheimes Schlüsselmaterial selbst ungeschützt zu enthalten.

---

## Metadata und Payload

Encryption Policies müssen unterscheiden können, welche Bestandteile eines Storage Objects geschützt werden müssen.

Je nach Policy können dies sein:

```text
Payload
Metadata
oder
Payload + Metadata
```

Metadata-First Storage darf nicht dazu führen, dass sensible Metadaten unbeabsichtigt unverschlüsselt verfügbar werden.

---

## Content-Addressed Storage

Verschlüsselung muss mit Content-Addressed Storage kombinierbar sein.

Dabei sind Content Identity und verschlüsselte physische Repräsentation getrennte Architekturkonzepte.

Deduplication oder CAS-Optimierungen dürfen Encryption-, Trust- oder Data-Sovereignty-Grenzen nicht umgehen.

Die konkrete Reihenfolge von Hashing, Verschlüsselung und Content-Adressierung wird in den NPSPECs definiert.

---

## Snapshots und Versionen

Snapshots und historische Versionen müssen die für ihre Daten geltenden Verschlüsselungsanforderungen erhalten.

Ein Rollback darf keine frühere Version in einem Zustand veröffentlichen, der aktuelle zwingende Security Policies verletzt.

---

## Legacy-Projektion

Legacy-Dateiprojektionen dürfen die Encryption Policy nicht umgehen.

Eine Anwendung kann eine entschlüsselte autorisierte Sicht erhalten, ohne dass deshalb eine dauerhaft unverschlüsselte Kopie erzeugt werden muss.

Temporäre Klartextrepräsentationen unterliegen weiterhin den entsprechenden Security- und Resource-Policies.

---

## Architecture Introspection

NovaOS muss den Verschlüsselungszustand kontrolliert introspektierbar machen.

Dabei dürfen keine geheimen Schlüssel oder vergleichbare Sicherheitsinformationen offengelegt werden.

Relevant sind insbesondere:

- Encryption Requirement,
- aktueller Schutzstatus,
- verwendete Schutzklasse,
- zuständiger Provider,
- Policy-Konformität.

---

## Normative Anforderungen

1. NovaOS MUSS Verschlüsselung als native Storage Policy unterstützen.
2. Encryption Policy und konkrete Verschlüsselungsimplementierung MÜSSEN getrennt bleiben.
3. Zwingend erforderliche Verschlüsselung MUSS als Hard Constraint behandelt werden.
4. Eine verpflichtende Encryption Policy DARF NICHT stillschweigend abgeschwächt werden.
5. Storage Provider MÜSSEN ihre relevanten Encryption Capabilities deklarieren.
6. Encryption Policies MÜSSEN bei Migration und Replikation erhalten bleiben.
7. Verschlüsselung MUSS mit Data-Sovereignty-Policies kombinierbar sein.
8. Schlüsselmaterial DARF NICHT als gewöhnliche ungeschützte Storage-Metadata gespeichert werden.
9. Policies MÜSSEN Payload und sensible Metadata schützen können.
10. Verschlüsselung MUSS mit CAS, Versioned Storage und Snapshots kombinierbar sein.
11. Storage-Optimierungen DÜRFEN Encryption- oder Trust-Grenzen NICHT umgehen.
12. Legacy-Projektionen DÜRFEN keine unkontrollierten persistenten Klartextkopien erzeugen.
13. Rollback MUSS weiterhin geltende zwingende Security Constraints berücksichtigen.
14. Encryption Requirement und Policy-Konformität MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Verschlüsselung wird unabhängig vom konkreten Dateisystem,
- unterschiedliche Daten können unterschiedliche Schutzanforderungen besitzen,
- Migration und Replikation erhalten Security-Anforderungen,
- Data Sovereignty kann Verschlüsselung direkt erzwingen,
- zukünftige Verschlüsselungstechniken können integriert werden.

### Negative Konsequenzen

- Provider müssen Encryption Capabilities deklarieren,
- Schlüsselverwaltung benötigt eine separate sichere Infrastruktur,
- CAS, Deduplication und Verschlüsselung benötigen abgestimmte Regeln,
- verschlüsselte Metadaten können Metadata-First-Optimierungen begrenzen.

---

## Verworfene Alternativen

### Verschlüsselung ausschließlich pro Volume

Verworfen.

Einzelne Storage Objects können unterschiedliche Schutzanforderungen besitzen.

### Verschlüsselung ausschließlich durch Anwendungen

Verworfen.

Systemweite Security- und Data-Sovereignty-Policies wären dadurch nicht zuverlässig durchsetzbar.

### Verschlüsselungsalgorithmus direkt im Storage Object festschreiben

Verworfen.

Die Architektur soll Schutzanforderungen deklarieren und langfristige Algorithmusagilität ermöglichen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-STORAGE-0002_Natives_Object_Storage_Modell`
- `ADR-STORAGE-0003_Content_Addressed_Storage`
- `ADR-STORAGE-0007_Metadata_First_Storage`
- `ADR-STORAGE-0012_Named_Volumes_statt_Laufwerksbuchstaben`
- `ADR-STORAGE-0014_Snapshots_und_Rollback`
- `ADR-STORAGE-0015_Checksumming_für_Integrität`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`

---

## Zugehörige NPSPECs

- `NPSPEC-STORAGE-ENCRYPTION-0001`
- `NPSPEC-STORAGE-ENCRYPTION-POLICY-0001`
- `NPSPEC-STORAGE-ENCRYPTION-KEY-0001`
- `NPSPEC-STORAGE-ENCRYPTION-METADATA-0001`
- `NPSPEC-STORAGE-ENCRYPTION-MIGRATION-0001`
- `NPSPEC-STORAGE-ENCRYPTION-TEST-0001`

---

## Ergebnis

Verschlüsselung wird in NovaOS als deklarative Anforderung des Storage Objects beziehungsweise Storage Scopes behandelt:

```text
Data
 ↓
Encryption Policy
 ↓
Hard Constraints
 ↓
Compatible Storage Provider
 ↓
Protected Storage
```

Die konkrete technische Umsetzung kann sich weiterentwickeln, während die Sicherheitsanforderung stabil bleibt.

Die zentrale Architekturregel lautet:

```text
Declare the required protection.

Let the storage architecture enforce it.
```