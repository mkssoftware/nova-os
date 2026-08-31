# ADR-STORAGE-0007 – Metadata-First Storage

## Status

Angenommen

## Kategorie

Kernel / Storage / Metadata / Object Storage / Semantics

## Kontext

Im nativen Storage-Modell von NovaOS besitzen persistente Objekte neben ihren eigentlichen Nutzdaten auch strukturierte Informationen über Identität, Typ, Herkunft, Beziehungen, Sicherheit und Speicherung.

In klassischen Dateisystemen sind solche Metadaten häufig sekundär. Anwendungen müssen Dateien öffnen oder ihren Inhalt analysieren, um ihre Bedeutung und Eigenschaften festzustellen.

Für NovaOS sollen Metadaten dagegen ein grundlegender Bestandteil des Storage-Modells sein und für Discovery, Policy-Entscheidungen und Capability-Auswahl genutzt werden können, ohne zunächst den vollständigen Objektinhalt laden zu müssen.

---

## Entscheidung

NovaOS verwendet ein **Metadata-First Storage Model**.

Jedes native Storage Object besitzt einen strukturierten Metadatenzustand, der unabhängig vom eigentlichen Payload adressierbar und auswertbar ist.

```text
Storage Object
    ├── Identity
    ├── Metadata
    │    ├── Semantic Type
    │    ├── Provenance
    │    ├── Relations
    │    ├── Security
    │    ├── Sovereignty
    │    └── Storage Properties
    │
    └── Content
```

Metadaten sind damit kein optionaler Zusatz zu einer Datei, sondern Bestandteil des nativen Objektzustands.

---

## Grundprinzip

```text
Understand the object
before loading its content.
```

---

## Metadaten und Payload

NovaOS trennt logisch zwischen:

```text
Metadata
```

und:

```text
Payload
```

Viele Operationen sollen ausschließlich anhand der Metadaten ausgeführt werden können.

Dazu gehören beispielsweise:

- Discovery,
- Suche,
- Capability-Auswahl,
- Policy-Prüfung,
- Storage-Planung,
- Provenance-Auswertung.

Der Payload muss dafür nicht zwangsläufig gelesen werden.

---

## Strukturierte und typisierte Metadaten

Metadaten sollen nicht lediglich als beliebige Key-Value-Sammlung behandelt werden.

Sie können über definierte Namespaces und Semantic Types strukturiert werden.

Dadurch können Systemkomponenten gemeinsame Metadaten verstehen, während Anwendungen und Capabilities eigene Erweiterungen ergänzen können.

Die konkrete Metadata-Schema- und Namespace-Architektur wird separat spezifiziert.

---

## Metadaten als Policy-Input

Systementscheidungen dürfen Metadaten als Eingabe verwenden.

Beispielsweise können folgende Eigenschaften Storage- und Ausführungsentscheidungen beeinflussen:

```text
Semantic Type
Provenance
Trust
Data Classification
Sovereignty Policy
Retention Policy
Storage Requirements
```

Metadaten ersetzen dabei keine Authority oder Policy Engine.

Sie liefern die Informationen, auf deren Grundlage Policies entscheiden.

---

## Beziehungen und Provenance

Objektbeziehungen und Provenance werden als native Metadaten behandelt.

Dadurch kann NovaOS beispielsweise darstellen:

```text
Object B
   ↓ derived-from
Object A
```

oder:

```text
Document
   ↓ created-by
Capability X
```

Diese Informationen bleiben unabhängig von Dateipfaden und VFS-Projektionen erhalten.

---

## VFS-Projektion

Klassische Dateisysteme können native Metadaten nur teilweise darstellen.

Das VFS darf geeignete Metadaten auf Mechanismen wie:

- Dateinamen,
- Dateiendungen,
- Extended Attributes,
- klassische Dateiattribute

projizieren.

Die VFS-Darstellung ist jedoch nicht die autoritative Definition des nativen Metadatenzustands.

---

## Transaktionen und Versionierung

Metadatenänderungen sind Teil des Storage-Zustands und müssen mit Transactional Storage kombinierbar sein.

```text
Metadata A
    ↓
Transaction
    ↓
Metadata B
```

Bei versionierten Storage Objects können relevante Metadaten ebenfalls Bestandteil einer Version sein.

Damit bleiben frühere Objektzustände einschließlich ihrer damaligen Semantik und Beziehungen nachvollziehbar.

---

## Metadaten und Content Identity

Metadata Identity und Content Identity bleiben getrennt.

Eine reine Metadatenänderung muss nicht zwangsläufig einen neuen CAS-Inhalt erzeugen.

```text
Object Version
    ├── Metadata State
    └── Content ID
```

Mehrere Objektversionen können deshalb denselben Content Identifier verwenden.

---

## Security und Privacy

Metadaten können selbst sensible Informationen enthalten.

Metadata-First bedeutet daher nicht, dass sämtliche Metadaten für jeden Teilnehmer sichtbar sind.

Zugriff auf Metadaten unterliegt denselben grundlegenden:

- Authority-,
- Security-,
- Trust-,
- Data-Sovereignty-

Regeln wie andere Systemdaten.

Discovery und Introspection dürfen geschützte Metadaten nicht unautorisiert offenlegen.

---

## Performance

Häufig benötigte Metadaten sollen effizient zugreifbar sein, ohne den vollständigen Payload eines Objekts laden zu müssen.

Die konkrete physische Speicherung, Indexierung und Caching-Strategie bleibt Provider- und Implementierungsentscheidung.

Metadata-First schreibt keine bestimmte Datenbank oder On-Disk-Struktur vor.

---

## Normative Anforderungen

1. NovaOS MUSS Metadaten als nativen Bestandteil jedes Storage Objects modellieren können.
2. Metadaten und Payload MÜSSEN logisch getrennt adressierbar sein.
3. Relevante Metadaten MÜSSEN ohne vollständiges Laden des Payloads auswertbar sein.
4. Metadaten MÜSSEN strukturiert und typisiert darstellbar sein.
5. Das Metadatenmodell MUSS erweiterbare Namespaces unterstützen können.
6. Semantic Types, Provenance und Objektbeziehungen MÜSSEN über native Metadaten repräsentierbar sein.
7. Storage- und System-Policies DÜRFEN Metadaten als Entscheidungsgrundlage verwenden.
8. Metadaten DÜRFEN Authority oder Security Policies NICHT ersetzen.
9. Metadatenänderungen MÜSSEN mit Transactional Storage kombinierbar sein.
10. Relevante Metadaten MÜSSEN mit Storage-Versionen versionierbar sein.
11. Metadata State und CAS Content Identity MÜSSEN getrennte Konzepte bleiben.
12. VFS-Projektionen DÜRFEN native Metadaten auf klassische Dateisystemmechanismen abbilden.
13. Metadaten MÜSSEN denselben Security- und Data-Sovereignty-Grundsätzen wie andere Daten unterliegen.
14. Metadatenzugriffe MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Objekte können ohne vollständiges Laden ihres Inhalts verstanden werden,
- effizientere Suche und Discovery,
- direkte Integration von Semantic Types und Provenance,
- bessere Capability- und Policy-Auswahl,
- konsistentes Metadatenmodell unabhängig vom Dateisystem,
- bessere Grundlage für Automatisierung und adaptive Systemfunktionen.

### Negative Konsequenzen

- Metadaten benötigen eigene Persistenz- und Indexierungsmechanismen,
- Schema- und Namespace-Evolution muss langfristig berücksichtigt werden,
- sensible Metadaten benötigen eigene Zugriffskontrolle.

---

## Verworfene Alternativen

### Metadaten nur als Extended Attributes

Verworfen.

Extended Attributes sind eine mögliche VFS-Projektion, aber keine ausreichende Grundlage für das native Storage-Modell.

### Bedeutung ausschließlich aus dem Payload ableiten

Verworfen.

Dies würde unnötige I/O- und Parsing-Kosten verursachen und systemweite Semantik verlieren.

### Unstrukturierte Key-Value-Metadaten als einziges Modell

Verworfen.

NovaOS benötigt systemweit verständliche, typisierte und erweiterbare Metadaten.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-STORAGE-0001_VFS_als_Kompatibilitäts_und_Projektionsschicht`
- `ADR-STORAGE-0002_Natives_Object_Storage_Modell`
- `ADR-STORAGE-0003_Content_Addressed_Storage`
- `ADR-STORAGE-0004_Transactional_Storage`
- `ADR-STORAGE-0005_Versioned_Storage`
- `ADR-STORAGE-0006_Semantic_und_Typed_Storage`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0013_Architecture_Introspection`

---

## Zugehörige NPSPECs

- `NPSPEC-STORAGE-METADATA-0001`
- `NPSPEC-STORAGE-METADATA-SCHEMA-0001`
- `NPSPEC-STORAGE-METADATA-NAMESPACE-0001`
- `NPSPEC-STORAGE-METADATA-INDEX-0001`
- `NPSPEC-STORAGE-METADATA-SECURITY-0001`
- `NPSPEC-STORAGE-METADATA-TEST-0001`

---

## Ergebnis

NovaOS behandelt Metadaten als gleichwertigen Bestandteil persistenter Objekte:

```text
Object Identity
      ↓
Metadata
      ↓
Understand / Discover / Decide
      ↓
Content only when required
```

Die Storage-Architektur kann dadurch Bedeutung, Beziehungen und Policies eines Objekts verarbeiten, bevor dessen eigentliche Nutzdaten gelesen werden.

Die zentrale Architekturregel lautet:

```text
Metadata describes the object.

Payload carries its content.
```