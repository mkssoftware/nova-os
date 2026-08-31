# ADR-STORAGE-0006 – Semantic und Typed Storage

## Status

Angenommen

## Kategorie

Kernel / Storage / Semantic Types / Object Storage

## Kontext

Klassische Storage-Systeme behandeln Daten überwiegend als untypisierte Bytefolgen. Ihre Bedeutung wird häufig nur indirekt durch Dateiendungen, MIME Types oder Anwendungen bestimmt.

NovaOS besitzt dagegen Semantic Types als systemweites Architekturprinzip.

Das native Storage-Modell soll diese Semantik erhalten, damit persistente Daten nicht beim Speichern auf untypisierte Bytes reduziert werden.

---

## Entscheidung

NovaOS unterstützt **Semantic und Typed Storage als native Eigenschaft von Storage Objects**.

Ein Storage Object kann neben seinem Inhalt einen systemweit verständlichen Semantic Type besitzen.

```text
Storage Object
    ├── Object Identity
    ├── Semantic Type
    ├── Content
    ├── Metadata
    └── Relations
```

Der Semantic Type beschreibt die Bedeutung des gespeicherten Objekts unabhängig von Dateiname, Dateiendung, Pfad oder konkretem Storage Provider.

---

## Grundprinzip

```text
Storage preserves meaning,
not only bytes.
```

---

## Semantic Type

Ein Semantic Type beschreibt, welche Art von Information ein Objekt repräsentiert.

Beispiele:

```text
Image
Audio
Video
Document
SourceCode
Dataset
Model
```

Diese Typen können durch spezialisierte Untertypen erweitert werden.

Die konkrete Type-Hierarchie und Type-Registry werden separat spezifiziert.

---

## Typidentität

Semantic Types benötigen stabile, systemweit eindeutige Identitäten.

Ein Storage Provider darf die Bedeutung eines Typs nicht anhand lokaler Dateiendungen oder proprietärer Namen neu definieren.

Typidentität und physisches Datenformat bleiben getrennte Konzepte.

```text
Semantic Type
     ≠
Storage Format
```

Ein semantisch identischer Datentyp kann daher mehrere physische Repräsentationen besitzen.

---

## Typed Metadata

Neben dem primären Semantic Type dürfen auch Metadaten typisiert sein.

Beispielsweise kann ein Bild semantisch definierte Eigenschaften besitzen wie:

```text
Dimensions
ColorSpace
CaptureTime
Location
Author
Provenance
```

Damit können System und Capabilities Metadaten interpretieren, ohne ausschließlich an anwendungsspezifische Strukturen gebunden zu sein.

---

## Capabilities

Semantic Types bilden eine Grundlage für die Auswahl geeigneter Capabilities.

```text
Storage Object
      ↓
Semantic Type
      ↓
Compatible Capabilities
```

Eine Capability kann deklarieren, welche Eingabe- und Ausgabetypen sie unterstützt.

Dadurch können Storage Objects direkt in das systemweite Data/Object-Pipelining eingebunden werden.

---

## Format und Semantik

NovaOS trennt:

```text
What the data means
        ↓
Semantic Type

How the data is encoded
        ↓
Storage Format
```

Beispielsweise können PNG, JPEG oder AVIF unterschiedliche Repräsentationen eines semantischen `Image`-Objekts sein.

Konvertierungen zwischen Repräsentationen verändern daher nicht zwingend die übergeordnete semantische Bedeutung.

---

## VFS-Kompatibilität

Bei einer VFS-Projektion können Semantic Types auf klassische Mechanismen wie:

- Dateiendungen,
- MIME Types,
- Extended Attributes

abgebildet werden.

Diese Projektionen dienen der Kompatibilität.

Die native Typidentität bleibt unabhängig davon erhalten.

---

## Provider-Kompatibilität

Nicht jeder Storage Provider kann Semantic Types nativ speichern.

In diesem Fall darf NovaOS geeignete Metadaten- oder Projektionsmechanismen verwenden.

Ein Provider darf jedoch nicht stillschweigend eine andere Semantik vortäuschen.

Ein unvermeidbarer Verlust semantischer Informationen muss als Capability- beziehungsweise Kompatibilitätsgrenze erkennbar sein.

---

## Versionierung und CAS

Semantic Types sind Bestandteil des logischen Objektzustands und können damit versioniert werden.

Content Identity und Semantic Type bleiben getrennt:

```text
Object Identity
      ├── Semantic Type
      └── Content Identity
```

Identische Bytes müssen nicht zwangsläufig dieselbe semantische Bedeutung besitzen.

CAS darf deshalb Content-Gleichheit nicht automatisch mit semantischer Gleichheit gleichsetzen.

---

## Security und Data Sovereignty

Semantic Types dürfen als Eingabe für Security- und Data-Sovereignty-Policies verwendet werden.

Beispielsweise können bestimmte semantische Datenklassen strengere Regeln für:

- Speicherung,
- Übertragung,
- Verarbeitung,
- Replikation

erhalten.

Der Semantic Type selbst ersetzt jedoch keine Authority oder Security Policy.

---

## Architecture Introspection

NovaOS muss kontrolliert introspektierbar machen können:

- Semantic Type,
- Type Identity,
- Storage Format,
- verfügbare typisierte Metadaten,
- kompatible Capabilities,
- Provider-Unterstützung.

Sensitive Metadaten bleiben dabei durch die jeweiligen Zugriffsrechte geschützt.

---

## Normative Anforderungen

1. NovaOS MUSS Semantic Types nativ im Storage-Modell unterstützen.
2. Storage Objects MÜSSEN einen eindeutig identifizierbaren Semantic Type besitzen können.
3. Semantic Type und physisches Storage Format MÜSSEN getrennte Konzepte sein.
4. Semantic Types DÜRFEN NICHT ausschließlich aus Dateiendungen abgeleitet werden.
5. Metadaten MÜSSEN typisiert darstellbar sein.
6. Semantic Types MÜSSEN mit dem systemweiten Semantic-Type-Modell kompatibel sein.
7. Capabilities MÜSSEN Semantic Types für Input-/Output-Contracts verwenden können.
8. Storage Objects MÜSSEN anhand ihrer Semantic Types in Data/Object-Pipelines verwendbar sein.
9. VFS-Projektionen DÜRFEN Semantic Types auf klassische Dateisystemmechanismen abbilden.
10. Fehlende Provider-Unterstützung DARF semantischen Informationsverlust NICHT unsichtbar machen.
11. Content Identity und Semantic Identity MÜSSEN getrennt behandelt werden.
12. Semantic Types MÜSSEN versionierbar sein.
13. Security- und Data-Sovereignty-Policies DÜRFEN Semantic Types als Policy-Information verwenden.
14. Typinformationen und Provider-Fähigkeiten MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- persistente Daten behalten ihre systemweit verständliche Bedeutung,
- Capabilities können direkt anhand von Datentypen ausgewählt werden,
- weniger Abhängigkeit von Dateiendungen,
- einheitliche typisierte Datenpipelines,
- Storage Format und semantische Bedeutung können unabhängig entwickelt werden,
- bessere Grundlage für Automatisierung und intelligente Systemfunktionen.

### Negative Konsequenzen

- Type Registry und Versionierung werden benötigt,
- klassische Dateisysteme können Typinformationen nur eingeschränkt abbilden,
- Typmigration und Kompatibilität müssen langfristig berücksichtigt werden.

---

## Verworfene Alternativen

### Storage ausschließlich als untypisierte Bytes

Verworfen.

Dies würde die systemweite Semantic-Type-Architektur an der Persistenzgrenze verlieren.

### Dateiendung als native Typidentität

Verworfen.

Dateiendungen sind projektionstechnische Konventionen und keine stabile semantische Identität.

### MIME Types als vollständiges Typmodell

Verworfen.

MIME Types bleiben für Kompatibilität relevant, reichen aber für das erweiterbare semantische Typsystem von NovaOS nicht aus.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-STORAGE-0001_VFS_als_Kompatibilitäts_und_Projektionsschicht`
- `ADR-STORAGE-0002_Natives_Object_Storage_Modell`
- `ADR-STORAGE-0003_Content_Addressed_Storage`
- `ADR-STORAGE-0005_Versioned_Storage`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0009_Einheitliches_Daten_und_Objekt_Pipelining`
- `ADR-ARCH-0014_Explicit_Contracts`

---

## Zugehörige NPSPECs

- `NPSPEC-STORAGE-SEMANTIC-0001`
- `NPSPEC-STORAGE-TYPE-0001`
- `NPSPEC-STORAGE-TYPED-METADATA-0001`
- `NPSPEC-STORAGE-TYPE-REGISTRY-0001`
- `NPSPEC-STORAGE-TYPE-PROJECTION-0001`
- `NPSPEC-STORAGE-SEMANTIC-TEST-0001`

---

## Ergebnis

NovaOS speichert nicht nur Daten, sondern erhält deren systemweit verständliche Bedeutung:

```text
Object Identity
      +
Semantic Type
      +
Typed Metadata
      +
Content
      ↓
Typed Storage Object
```

Damit wird die Semantic-Type-Architektur bis in die Persistenzschicht fortgeführt.

Die zentrale Architekturregel lautet:

```text
Persist the meaning
together with the data.
```