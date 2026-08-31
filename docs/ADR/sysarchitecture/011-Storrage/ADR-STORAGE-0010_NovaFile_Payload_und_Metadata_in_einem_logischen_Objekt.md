# ADR-STORAGE-0010 – NovaFile Payload und Metadata in einem logischen Objekt

## Status

Angenommen

## Kategorie

Storage / NovaFile / Object Model / Metadata / Semantic Storage

## Kontext

`ADR-STORAGE-0009` definiert NovaFile als semantischen Dateicontainer, der Nutzdaten gemeinsam mit Typinformationen, Metadaten, Provenance und weiteren semantischen Eigenschaften transportieren kann.

Dabei darf jedoch keine Architektur entstehen, in der Payload und Metadaten aus Sicht des Systems zwei voneinander unabhängige Ressourcen darstellen.

Ein NovaFile repräsentiert ein logisches Datenobjekt. Payload und Metadaten sind unterschiedliche Bestandteile dieses Objekts, besitzen aber einen gemeinsamen Lebenszyklus und eine gemeinsame semantische Identität.

---

## Entscheidung

NovaOS behandelt **Payload und Metadata eines NovaFile als Bestandteile eines einzigen logischen Objekts**.

```text
NovaFile Object
 ├── Metadata
 │    ├── Semantic Type
 │    ├── Provenance
 │    ├── Relations
 │    └── weitere Eigenschaften
 │
 └── Payload
      └── Content Representation
```

Die interne physische Speicherung darf getrennt erfolgen, diese Trennung darf jedoch nicht die logische Objektsemantik verändern.

---

## Grundprinzip

```text
One logical object.

Multiple internal components.
```

---

## Logische Einheit

Ein NovaFile besitzt eine gemeinsame logische Identität.

Metadata und Payload werden nicht als zwei unabhängige Dateien oder Storage Objects betrachtet, die lediglich über Konventionen miteinander verbunden sind.

Operationen auf dem NovaFile beziehen sich grundsätzlich auf das vollständige logische Objekt.

---

## Physische Trennung

Die logische Einheit erzwingt keine bestimmte physische Speicherung.

Ein Storage Provider darf beispielsweise:

```text
Metadata → Metadata Store
Payload  → CAS
```

verwenden.

Ebenso darf ein portabler NovaFile-Container beide Bestandteile gemeinsam in einer Datei speichern.

Die physische Repräsentation bleibt Implementierungsentscheidung.

---

## Unabhängiger Zugriff

Obwohl Metadata und Payload zu einem logischen Objekt gehören, müssen sie getrennt zugreifbar sein.

Damit bleibt das Metadata-First-Prinzip erhalten:

```text
NovaFile
   ↓
Read Metadata
   ↓
Decide whether Payload is needed
```

Das Lesen von Metadaten darf nicht grundsätzlich das Laden des vollständigen Payloads erfordern.

---

## Konsistenz

Metadata und Payload müssen einen konsistenten Objektzustand bilden.

Eine Änderung, die beide Bereiche betrifft, soll über Transactional Storage gemeinsam veröffentlicht werden.

```text
Old Object State
       ↓
Transaction
       ↓
Metadata + Payload
       ↓
New Object State
```

Teilweise veröffentlichte Zustände müssen vermieden werden.

---

## Versionierung

Versioned Storage behandelt den relevanten Zustand von Metadata und Payload gemeinsam als Objektversion.

```text
Object Version
 ├── Metadata State
 └── Payload / Content Reference
```

Eine reine Metadatenänderung kann eine neue Objektversion erzeugen, ohne zwangsläufig einen neuen Payload oder CAS-Inhalt zu erzeugen.

---

## Content Identity und Object Identity

Die logische NovaFile-Identität bleibt von der Identität seines Payloads getrennt.

```text
NovaFile Object Identity
          ≠
Payload Content Identity
```

Mehrere NovaFile-Versionen oder sogar mehrere logische Objekte dürfen denselben immutable CAS-Payload referenzieren.

Dadurch bleibt Deduplication möglich, ohne logische Objekte zusammenzuführen.

---

## Export und Import

Beim Export eines Storage Objects als portables NovaFile müssen die für das logische Objekt erforderlichen Bestandteile konsistent zusammengeführt werden.

Beim Import wird der Container wieder als zusammengehörendes semantisches Objekt interpretiert.

Dabei darf die Containerstruktur nicht dazu führen, dass Metadata und Payload unbeabsichtigt als voneinander unabhängige Benutzerobjekte erscheinen.

---

## Security und Data Sovereignty

Security- und Data-Sovereignty-Regeln gelten für das logische NovaFile-Objekt und können zusätzlich unterschiedliche Zugriffsrechte für einzelne Bestandteile vorsehen.

Beispielsweise darf Metadata sichtbar sein, während der Payload geschützt bleibt.

Eine solche Zugriffstrennung verändert jedoch nicht die gemeinsame logische Objektzugehörigkeit.

---

## VFS-Projektion

Das VFS projiziert ein NovaFile grundsätzlich als ein logisches Dateiobjekt.

Interne Metadata- oder Payload-Komponenten dürfen nicht automatisch als separate Dateien im normalen Namespace erscheinen.

Spezialisierte APIs und Capabilities dürfen gezielt auf einzelne Bestandteile zugreifen.

---

## Normative Anforderungen

1. NovaOS MUSS Metadata und Payload eines NovaFile als Bestandteile eines logischen Objekts behandeln.
2. Ein NovaFile MUSS eine gemeinsame logische Object Identity besitzen.
3. Metadata und Payload DÜRFEN physisch getrennt gespeichert werden.
4. Physische Trennung DARF die logische Einheit des NovaFile NICHT verändern.
5. Metadata MUSS unabhängig vom vollständigen Payload zugreifbar sein können.
6. Das Lesen von Metadata DARF NICHT grundsätzlich das Laden des Payloads erfordern.
7. Metadata und Payload MÜSSEN einen konsistenten Objektzustand bilden.
8. Gemeinsame Änderungen SOLLEN über Transactional Storage veröffentlicht werden.
9. Versioned Storage MUSS Metadata- und Payload-Zustände einem gemeinsamen Objektversionskontext zuordnen können.
10. NovaFile Object Identity und Payload Content Identity MÜSSEN getrennte Konzepte bleiben.
11. Mehrere logische Objekte DÜRFEN denselben immutable Payload referenzieren.
12. Export und Import MÜSSEN die logische Zusammengehörigkeit von Metadata und Payload erhalten.
13. Unterschiedliche Zugriffsrechte auf Metadata und Payload DÜRFEN unterstützt werden, ohne daraus unabhängige logische Objekte zu machen.
14. Das VFS MUSS NovaFile standardmäßig als ein logisches Dateiobjekt projizieren.

---

## Konsequenzen

### Positive Konsequenzen

- klare Objektsemantik,
- keine fragile Sidecar-Architektur,
- Metadata-First bleibt möglich,
- CAS-basierte Payload-Deduplication bleibt erhalten,
- konsistente Versionierung von Metadata und Payload,
- physische Storage-Optimierungen bleiben möglich.

### Negative Konsequenzen

- Storage Provider müssen logische und physische Struktur unterscheiden,
- getrennte Speicherung benötigt zuverlässige Referenz- und Konsistenzmechanismen,
- partielle Zugriffsrechte erhöhen die Anforderungen an das Authority-Modell.

---

## Verworfene Alternativen

### Metadata als separate Sidecar-Datei

Verworfen.

Payload und Semantik könnten getrennt, inkonsistent oder unabhängig gelöscht werden.

### Metadata vollständig in den Payload einbetten

Verworfen.

Dies würde Metadata-First-Zugriffe erschweren und unnötiges Lesen großer Payloads verursachen.

### Physische und logische Struktur gleichsetzen

Verworfen.

NovaOS soll Metadata, CAS-Payloads und andere Komponenten unabhängig optimieren können, ohne die Objektsemantik zu verändern.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-STORAGE-0002_Natives_Object_Storage_Modell`
- `ADR-STORAGE-0003_Content_Addressed_Storage`
- `ADR-STORAGE-0004_Transactional_Storage`
- `ADR-STORAGE-0005_Versioned_Storage`
- `ADR-STORAGE-0006_Semantic_und_Typed_Storage`
- `ADR-STORAGE-0007_Metadata_First_Storage`
- `ADR-STORAGE-0008_Data_Provenance_im_Storage_Modell`
- `ADR-STORAGE-0009_NovaFile_als_semantischer_Dateicontainer`

---

## Zugehörige NPSPECs

- `NPSPEC-STORAGE-NOVAFILE-OBJECT-0001`
- `NPSPEC-STORAGE-NOVAFILE-PAYLOAD-0001`
- `NPSPEC-STORAGE-NOVAFILE-METADATA-0002`
- `NPSPEC-STORAGE-NOVAFILE-CONSISTENCY-0001`
- `NPSPEC-STORAGE-NOVAFILE-TEST-0002`

---

## Ergebnis

NovaFile besitzt eine logische Objektstruktur, ohne die physische Storage-Implementierung unnötig festzulegen:

```text
          NovaFile
       Logical Object
        /          \
   Metadata       Payload
      ↓              ↓
Metadata Store      CAS
        \          /
       Same Object
```

Damit können Metadata und Payload unabhängig optimiert und geladen werden, bleiben aber semantisch, transaktional und versionstechnisch Bestandteil desselben NovaFile-Objekts.

Die zentrale Architekturregel lautet:

```text
Separate the storage.

Preserve the object.
```