# ADR-STORAGE-0009 – NovaFile als semantischer Dateicontainer

## Status

Angenommen

## Kategorie

Storage / File Format / Semantic Storage / Object Storage

## Kontext

NovaOS besitzt mit dem nativen Object-Storage-Modell eine wesentlich reichhaltigere Datenarchitektur als klassische Dateisysteme.

Storage Objects können unter anderem enthalten:

- stabile Object Identity,
- Semantic Types,
- typisierte Metadaten,
- Beziehungen,
- Versionen,
- Provenance,
- Security- und Data-Sovereignty-Informationen.

Klassische Dateiformate können diese Informationen häufig nur teilweise oder über externe Sidecar-Dateien abbilden.

NovaOS benötigt deshalb ein natives portables Dateicontainerformat, das die semantischen Eigenschaften eines Storage Objects gemeinsam mit seinen Nutzdaten transportieren kann.

---

## Entscheidung

NovaOS definiert **NovaFile** als nativen semantischen Dateicontainer.

NovaFile ist die portable dateibasierte Repräsentation eines oder mehrerer zusammengehörender NovaOS-Datenobjekte.

```text
NovaFile
 ├── Container Identity
 ├── Semantic Metadata
 ├── Object Metadata
 ├── Relations
 ├── Provenance
 ├── Content
 └── Integrity Information
```

NovaFile ergänzt das native Object-Storage-Modell, ersetzt es jedoch nicht.

---

## Grundprinzip

```text
A NovaFile carries
data + meaning + context.
```

---

## Verhältnis zum Storage Object

Storage Object und NovaFile sind getrennte Konzepte.

```text
Native Storage Object
        ↕
    NovaFile
        ↕
External Storage / Transfer
```

Ein Storage Object kann als NovaFile exportiert und ein NovaFile wieder als Storage Object importiert werden.

Die interne Storage-Implementierung von NovaOS ist nicht an das NovaFile-On-Disk-Format gebunden.

---

## Semantischer Container

NovaFile speichert nicht nur einen untypisierten Byte-Stream.

Der Container kann die zur Interpretation notwendigen semantischen Informationen gemeinsam mit dem Inhalt transportieren.

Dazu gehören insbesondere:

- Semantic Type,
- Content Representation,
- typisierte Metadaten,
- Objektbeziehungen,
- Provenance.

Damit bleibt die Bedeutung eines Objekts auch außerhalb seines ursprünglichen NovaOS-Storage-Providers erhalten.

---

## Mehrere Repräsentationen

Ein semantisches Objekt darf mehrere Repräsentationen enthalten oder referenzieren.

Beispielsweise kann ein Bildobjekt neben dem primären Bildinhalt zusätzliche Ressourcen besitzen.

```text
Image Object
 ├── Primary Representation
 ├── Preview
 ├── Thumbnail
 └── Additional Metadata
```

Die konkrete Containerstruktur und zulässigen Repräsentationsmodelle werden in NPSPECs definiert.

---

## Erweiterbarkeit

NovaFile muss versionierbar und erweiterbar sein.

Neue:

- Semantic Types,
- Metadata Namespaces,
- Container Features,
- Repräsentationen

dürfen hinzugefügt werden können, ohne ältere Implementierungen unnötig inkompatibel zu machen.

Unbekannte optionale Erweiterungen sollen übersprungen oder erhalten werden können, sofern dies sicher möglich ist.

---

## Content-Addressed Storage

NovaFile kann Content Identities für enthaltene oder referenzierte Inhalte verwenden.

```text
NovaFile
    ↓
Content Reference
    ↓
CAS Content
```

Beim Export muss jedoch klar definiert sein, ob referenzierter Content:

- eingebettet,
- extern referenziert,
- oder anderweitig auflösbar

ist.

Ein NovaFile darf nicht stillschweigend von nicht verfügbaren externen Inhalten abhängig sein.

---

## Provenance und Trust

NovaFile kann Provenance-Informationen aus dem nativen Storage-Modell transportieren.

Zusätzlich können Signaturen oder andere Trust-relevante Informationen mitgeführt werden.

Dabei gilt:

```text
Container Integrity
      ≠
Trusted Content
```

Ein technisch intakter NovaFile-Container ist nicht automatisch vertrauenswürdig.

Die Bewertung erfolgt über Nova.Trust.

---

## Security und Data Sovereignty

NovaFile darf Security- und Data-Sovereignty-Metadaten transportieren.

Beim Export oder Import müssen entsprechende Policies geprüft werden.

Das Verpacken eines Objekts in einen NovaFile-Container darf bestehende Informationsfluss- oder Zugriffsregeln nicht umgehen.

---

## VFS

NovaFile wird über das VFS als reguläre Datei projizierbar.

Für klassische Anwendungen erscheint der Container damit als Datei.

NovaOS-fähige Anwendungen und Capabilities können dagegen auf die semantische Containerstruktur zugreifen.

```text
Legacy Application
      ↓
File View

Nova Capability
      ↓
Semantic NovaFile View
```

---

## Fremdformate

NovaFile ersetzt bestehende Formate wie PNG, JPEG, PDF, MP4 oder andere etablierte Datenformate nicht zwingend.

Solche Formate können als Repräsentation innerhalb eines NovaFile-Containers verwendet werden.

```text
NovaFile
 └── Semantic Type: Image
      └── Representation: AVIF
```

Damit bleiben etablierte Codecs und Formate nutzbar, während NovaOS zusätzliche Semantik transportieren kann.

---

## Portabilität

NovaFile ist für die Speicherung und Übertragung außerhalb des nativen NovaOS-Object-Stores vorgesehen.

Der Container darf daher keine Abhängigkeit von:

- virtuellen Adressen,
- physischen Speicheradressen,
- temporären Kernel-Handles,
- lokalen Prozesszuständen

besitzen.

Persistente Referenzen müssen über stabile Identitäten beziehungsweise portable Referenzmechanismen erfolgen.

---

## Normative Anforderungen

1. NovaOS MUSS NovaFile als nativen semantischen Dateicontainer definieren.
2. NovaFile MUSS vom internen Object-Storage-Modell logisch getrennt bleiben.
3. Storage Objects MÜSSEN grundsätzlich in eine NovaFile-Repräsentation exportierbar sein können, sofern ihre Policy dies erlaubt.
4. NovaFile MUSS Semantic Types und typisierte Metadaten transportieren können.
5. NovaFile MUSS Objektbeziehungen und Provenance darstellen können.
6. NovaFile MUSS mehrere zusammengehörende Datenrepräsentationen unterstützen können.
7. Das NovaFile-Format MUSS versionierbar und erweiterbar sein.
8. Unbekannte optionale Erweiterungen SOLLEN sicher übersprungen oder erhalten werden können.
9. NovaFile DARF Content Identities und CAS-Referenzen verwenden.
10. Externe Content-Abhängigkeiten MÜSSEN explizit erkennbar sein.
11. NovaFile MUSS über das VFS als reguläre Datei projizierbar sein.
12. Bestehende Standardformate MÜSSEN als Content-Repräsentationen verwendbar sein können.
13. NovaFile DARF Security- und Data-Sovereignty-Regeln NICHT umgehen.
14. NovaFile DARF keine nicht-portablen Laufzeitreferenzen als persistente Identität verwenden.

---

## Konsequenzen

### Positive Konsequenzen

- Semantik bleibt beim Dateiaustausch erhalten,
- Storage Objects können portabel exportiert werden,
- Metadaten, Provenance und Beziehungen reisen mit den Daten,
- bestehende Medien- und Dokumentformate bleiben weiter nutzbar,
- NovaFile kann als gemeinsames natives Austauschformat für NovaOS dienen.

### Negative Konsequenzen

- zusätzlicher Container-Overhead,
- Formatversionierung muss langfristig gepflegt werden,
- klassische Anwendungen sehen ohne NovaOS-Unterstützung nur den Container,
- externe Referenzen benötigen definierte Portabilitätsregeln.

---

## Verworfene Alternativen

### Eigenes Format für jeden Semantic Type

Verworfen.

Gemeinsame Containerfunktionen wie Provenance, Metadata, Trust und Versionierung würden mehrfach implementiert.

### Bestehende Dateiformate vollständig ersetzen

Verworfen.

Bewährte Formate und Codecs sollen weiterhin als Repräsentationen verwendet werden können.

### Sidecar-Dateien als natives Modell

Verworfen.

Semantik und Nutzdaten könnten getrennt werden, verloren gehen oder inkonsistent werden.

### NovaFile als internes Storage-Format erzwingen

Verworfen.

Das native Object-Storage-Modell muss unabhängig von einem konkreten portablen Containerformat bleiben.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-STORAGE-0001_VFS_als_Kompatibilitäts_und_Projektionsschicht`
- `ADR-STORAGE-0002_Natives_Object_Storage_Modell`
- `ADR-STORAGE-0003_Content_Addressed_Storage`
- `ADR-STORAGE-0005_Versioned_Storage`
- `ADR-STORAGE-0006_Semantic_und_Typed_Storage`
- `ADR-STORAGE-0007_Metadata_First_Storage`
- `ADR-STORAGE-0008_Data_Provenance_im_Storage_Modell`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0019_Evolvable_System_Architecture`

---

## Zugehörige NPSPECs

- `NPSPEC-STORAGE-NOVAFILE-0001`
- `NPSPEC-STORAGE-NOVAFILE-CONTAINER-0001`
- `NPSPEC-STORAGE-NOVAFILE-METADATA-0001`
- `NPSPEC-STORAGE-NOVAFILE-REPRESENTATION-0001`
- `NPSPEC-STORAGE-NOVAFILE-PROVENANCE-0001`
- `NPSPEC-STORAGE-NOVAFILE-INTEGRITY-0001`
- `NPSPEC-STORAGE-NOVAFILE-TEST-0001`

---

## Ergebnis

NovaFile bildet die portable Brücke zwischen dem nativen semantischen Storage-Modell und der klassischen Dateiwelt:

```text
NovaOS Storage Object
        ↓
      NovaFile
        ↓
Data + Type + Metadata
+ Relations + Provenance
        ↓
Portable File
```

Die zentrale Architekturregel lautet:

```text
A file should be able to carry
not only its bytes,
but also their meaning.
```