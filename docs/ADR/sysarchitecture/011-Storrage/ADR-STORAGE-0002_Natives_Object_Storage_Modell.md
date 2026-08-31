# ADR-STORAGE-0002 – Natives Object-Storage-Modell

## Status

Angenommen

## Kategorie

Kernel / Storage / Object Model / Data Architecture

## Kontext

Klassische Dateisysteme modellieren persistente Daten primär als Dateien und Verzeichnisse. Für NovaOS reicht dieses Modell nicht aus.

Das System benötigt persistente Datenobjekte mit Eigenschaften wie:

- stabiler Identität,
- Semantic Type,
- Metadaten,
- Provenance,
- Beziehungen zu anderen Objekten,
- Security- und Data-Sovereignty-Informationen,
- unterschiedlichen Storage-Locations.

Das klassische Datei-/Verzeichnismodell bleibt für Kompatibilität wichtig, soll aber nicht das native Storage-Modell bestimmen.

---

## Entscheidung

NovaOS verwendet ein **natives Object-Storage-Modell** als grundlegende logische Storage-Abstraktion.

```text
Storage Object
    ├── Object Identity
    ├── Semantic Type
    ├── Data
    ├── Metadata
    ├── Relations
    ├── Authority
    └── Storage Policy
```

Ein Storage Object ist unabhängig davon definiert, wie und wo seine Daten physisch gespeichert werden.

Das VFS aus `ADR-STORAGE-0001` projiziert geeignete Storage Objects bei Bedarf als Dateien und Verzeichnisse.

---

## Grundprinzip

```text
Objects are the native storage abstraction.

Files are one possible projection.
```

---

## Stabile Objektidentität

Jedes persistente Storage Object besitzt eine stabile Identität.

Diese Identität ist unabhängig von:

- Dateiname,
- Pfad,
- Volume,
- physischer Speicheradresse,
- konkretem Storage Provider.

```text
Object ID
   ↓
Storage Object
```

Umbenennen oder Verschieben verändert daher nicht automatisch die Identität des Objekts.

---

## Semantic Types

Storage Objects können einen Semantic Type besitzen.

Beispiele:

```text
Image
Document
Audio
Video
SourceCode
Model
Dataset
```

Semantic Types ermöglichen es NovaOS und seinen Capabilities, Daten nach ihrer Bedeutung statt ausschließlich anhand von Dateiendungen zu behandeln.

Die konkrete Type-Hierarchie wird separat spezifiziert.

---

## Daten und Metadaten

Ein Storage Object trennt logisch zwischen:

```text
Object Data
Object Metadata
```

Metadaten können unter anderem enthalten:

- Semantic Type,
- Provenance,
- Version,
- Beziehungen,
- Storage-Eigenschaften,
- Data-Sovereignty-Informationen.

Das native Modell darf nicht voraussetzen, dass alle Metadaten in Dateinamen oder Sidecar-Dateien gespeichert werden.

---

## Objektbeziehungen

Storage Objects dürfen explizite Beziehungen zu anderen Objekten besitzen.

```text
Object A
   │
   ├── references → Object B
   └── derived-from → Object C
```

Damit können Datenabhängigkeiten und Provenance unabhängig von Pfadstrukturen dargestellt werden.

---

## Storage Provider

Die physische Speicherung erfolgt über Storage Provider.

```text
Storage Object
      ↓
Storage Policy
      ↓
Storage Provider
```

Provider können unterschiedliche Technologien und Eigenschaften besitzen.

Das Object Model bleibt davon unabhängig.

---

## Location Transparency

Die Objektidentität bleibt unabhängig vom aktuellen Speicherort.

Ein Objekt kann beispielsweise:

- lokal,
- auf einem anderen Volume,
- remote,
- repliziert,
- migriert

gespeichert sein.

Location Transparency darf relevante Kosten, Verfügbarkeit oder Fehler nicht verbergen.

---

## VFS-Projektion

Geeignete Storage Objects können über das VFS als klassische Dateien dargestellt werden.

```text
Storage Object
      ↓
VFS Projection
      ↓
File / Directory View
```

Pfad und Dateiname sind dabei Projektionseigenschaften und nicht die primäre Objektidentität.

Dadurch bleiben klassische Anwendungen kompatibel, ohne das native Storage-Modell zu bestimmen.

---

## Security und Data Sovereignty

Authority wird an das Objekt beziehungsweise seine kontrollierten Zugriffsmechanismen gebunden und nicht ausschließlich an einen Pfad.

Storage Policies können zusätzlich Data-Sovereignty-Anforderungen enthalten.

Diese können beispielsweise festlegen, auf welchen Storage Domains oder Providern ein Objekt gespeichert werden darf.

---

## Transaktionale Operationen

Storage Objects müssen mit den transaktionalen Systemoperationen von NovaOS kombinierbar sein.

Operationen wie:

- Erzeugen,
- Aktualisieren,
- Verschieben,
- Verknüpfen,
- Löschen

sollen Bestandteil größerer Systemtransaktionen sein können.

Die konkrete Persistenz- und Commit-Semantik wird separat spezifiziert.

---

## Data/Object Pipelining

Storage Objects integrieren sich direkt in das einheitliche Data/Object-Pipelining von NovaOS.

```text
Storage Object
      ↓
Capability
      ↓
Transformation
      ↓
Storage Object
```

Damit müssen persistente Daten nicht grundsätzlich zuerst in ein separates dateibasiertes Zwischenmodell übersetzt werden.

---

## Architecture Introspection

NovaOS muss Storage Objects kontrolliert introspektierbar machen.

Relevant sind insbesondere:

- Object Identity,
- Semantic Type,
- Storage Location,
- Provider,
- Beziehungen,
- Provenance,
- Security- und Sovereignty-Eigenschaften.

Sensitive Metadaten bleiben dabei durch Authority geschützt.

---

## Normative Anforderungen

1. NovaOS MUSS ein natives persistentes Object-Storage-Modell bereitstellen.
2. Storage Objects MÜSSEN eine stabile, pfadunabhängige Identität besitzen.
3. Storage Objects MÜSSEN unabhängig vom konkreten Storage Provider modelliert sein.
4. Storage Objects MÜSSEN Semantic Types unterstützen können.
5. Daten und Metadaten MÜSSEN logisch getrennt modellierbar sein.
6. Beziehungen zwischen Storage Objects MÜSSEN explizit darstellbar sein.
7. Pfade und Dateinamen DÜRFEN NICHT die primäre Objektidentität bilden.
8. Storage Objects MÜSSEN über geeignete Provider persistent gespeichert werden können.
9. Storage Location DARF die logische Objektidentität NICHT verändern.
10. Geeignete Storage Objects MÜSSEN über das VFS als Dateien beziehungsweise Verzeichnisse projizierbar sein.
11. Security- und Data-Sovereignty-Policies MÜSSEN auf Storage Objects anwendbar sein.
12. Storage Objects SOLLEN in transaktionale Systemoperationen integrierbar sein.
13. Storage Objects MÜSSEN mit dem systemweiten Data/Object-Pipelining kombinierbar sein.
14. Objektidentität, Typ, Beziehungen und Storage-Eigenschaften MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- stabile Datenidentität unabhängig von Pfaden,
- Semantic Types werden nativ nutzbar,
- bessere Provenance- und Relationship-Modelle,
- Storage Provider können ausgetauscht oder erweitert werden,
- direkte Integration in Capability- und Datenpipelines,
- klassische Dateisysteme bleiben über das VFS kompatibel.

### Negative Konsequenzen

- zusätzliches Object- und Metadata-Management,
- klassische Dateisysteme können nicht jede native Semantik direkt speichern,
- VFS-Projektionen benötigen Übersetzungsregeln,
- Storage Provider benötigen definierte Capability-Profile.

---

## Verworfene Alternativen

### Dateien als einziges natives Storage-Objekt

Verworfen.

Dies würde Identität, Semantik und Beziehungen unnötig an klassische Dateisystemstrukturen koppeln.

### Pfad als dauerhafte Objektidentität

Verworfen.

Verschieben oder Umbenennen würde dadurch die Identität eines Objekts verändern.

### Object Storage nur als zusätzliche Datenbank

Verworfen.

Das Object Model soll Teil der nativen Storage-Architektur und keine isolierte Zusatzschicht sein.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-STORAGE-0001_VFS_als_Kompatibilitäts_und_Projektionsschicht`
- `ADR-ARCH-0003_Declarative_System_Model`
- `ADR-ARCH-0004_Transactionale_Systemoperationen`
- `ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0009_Einheitliches_Daten_und_Objekt_Pipelining`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explicit_Contracts`

---

## Zugehörige NPSPECs

- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-OBJECT-ID-0001`
- `NPSPEC-STORAGE-OBJECT-METADATA-0001`
- `NPSPEC-STORAGE-OBJECT-RELATION-0001`
- `NPSPEC-STORAGE-OBJECT-PROVIDER-0001`
- `NPSPEC-STORAGE-OBJECT-PROJECTION-0001`
- `NPSPEC-STORAGE-OBJECT-TEST-0001`

---

## Ergebnis

NovaOS trennt die native Identität persistenter Daten von ihrer klassischen Dateisystemdarstellung:

```text
Storage Object
      ↓
Stable Identity + Semantics
      ↓
Storage Provider
       │
       └── VFS Projection → File View
```

Damit werden Dateien zu einer wichtigen kompatiblen Darstellung persistenter Daten, während Storage Objects das native logische Storage-Modell von NovaOS bilden.

Die zentrale Architekturregel lautet:

```text
Identity belongs to the object,
not to its path.
```