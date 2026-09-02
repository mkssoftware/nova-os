# ADR-PRIVACY-0008 – Public, Protected und Private NovaFile Metadata

## Status

Angenommen

## Kategorie

Privacy Architecture / NovaFile / Metadata

## Kontext

NovaFile speichert Payload und Metadaten als ein logisches Objekt.

Metadaten können jedoch selbst sensible Informationen enthalten, beispielsweise:

- Autor oder Besitzer,
- Provenance,
- Standortinformationen,
- Semantic Data Labels,
- interne Beziehungen,
- Bearbeitungshistorie,
- Security- oder Sovereignty-Informationen.

Nicht alle Metadaten dürfen deshalb für jeden Consumer gleichermaßen sichtbar sein.

## Entscheidung

NovaFile-Metadaten werden in drei grundlegende Sichtbarkeitsklassen eingeteilt:

```text
NovaFile
 ├── Public Metadata
 ├── Protected Metadata
 ├── Private Metadata
 └── Payload
```

Die Klassifikation beschreibt die zulässige Offenlegung der Metadaten und ist unabhängig von ihrer physischen Speicherung.

## Public Metadata

Public Metadata enthält Informationen, die ohne besondere Datenzugriffs-Authority sichtbar sein dürfen.

Beispiele können sein:

```text
File Type
Public Display Name
Format Version
öffentliche Capability-Hinweise
```

`Public` bedeutet nicht automatisch, dass das gesamte NovaFile öffentlich zugänglich ist.

## Protected Metadata

Protected Metadata ist nur innerhalb eines autorisierten Kontextes zugänglich.

Dazu können beispielsweise gehören:

```text
Semantic Data Labels
Provenance
Data Sovereignty Context
Retention Policy
interne Objektbeziehungen
```

Der Zugriff wird über die normalen NovaOS-Security- und Privacy-Mechanismen kontrolliert.

## Private Metadata

Private Metadata enthält Informationen, deren Offenlegung besonders restriktiv behandelt wird.

Beispiele können sein:

```text
Private User Metadata
Sensitive Provenance
Internal Application Metadata
Security-sensitive Metadata
```

Der Zugriff benötigt eine explizite passende Authority.

```text
Metadata exists
      ≠
Metadata is visible
```

## Unabhängigkeit von der Speicherung

Die drei Klassen sind logische Schutzklassen.

Sie erzwingen nicht drei physisch getrennte Metadatenbereiche.

Storage Provider dürfen geeignete Repräsentationen wählen, solange die Schutzsemantik erhalten bleibt.

## Selective Disclosure

NovaOS darf nur den für eine Operation erforderlichen Teil der Metadaten offenlegen.

```text
NovaFile Metadata
       ↓
Authority + Purpose + Policy
       ↓
Metadata View
       ↓
Consumer
```

Damit muss beispielsweise eine Thumbnail-Capability nicht automatisch Provenance- oder Benutzerinformationen erhalten.

## Propagation

Beim Kopieren, Exportieren, Konvertieren oder Ableiten eines NovaFile dürfen geschützte Metadaten nicht unbeabsichtigt öffentlich werden.

Notwendige Schutzklassifikationen müssen über relevante Transformationen hinweg erhalten bleiben.

Eine Herabstufung benötigt eine explizit zulässige Policy oder autorisierte Declassification.

## Legacy-Dateiformate

Bei der Projektion eines NovaFile auf ein Legacy-Dateiformat muss NovaOS kontrollieren, welche Metadaten exportiert werden können.

Metadaten dürfen nicht allein deshalb offengelegt werden, weil das Zielformat entsprechende Metadatenfelder unterstützt.

## Normative Anforderungen

1. NovaFile MUSS Metadaten als `Public`, `Protected` oder `Private` klassifizieren können.
2. Die Klassifikation MUSS unabhängig von der physischen Storage-Repräsentation sein.
3. `Public` Metadata DARF keine Authority auf Payload oder andere Metadaten erzeugen.
4. `Protected` Metadata MUSS einer Policy- und Authority-Prüfung unterliegen.
5. `Private` Metadata MUSS explizit autorisierten Zugriff erfordern.
6. NovaOS MUSS selektive Metadata Views unterstützen können.
7. Metadatenklassifikationen MÜSSEN bei relevanten Transformationen erhalten bleiben.
8. Eine Schutzklassifikation DARF nicht durch Formatkonvertierung oder Legacy-Projektion implizit abgeschwächt werden.
9. Declassification MUSS explizit autorisiert sein.
10. Metadata-Zugriffe MÜSSEN mit Data Minimization, IFC und Semantic Data Labels integrierbar sein.
11. Legacy-Export MUSS die Metadata-Sichtbarkeit respektieren.
12. Schutzklassifikation und relevante Zugriffe MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- NovaFile-Metadaten erhalten ein einheitliches Privacy-Modell.
- sensible Metadaten können im Objekt verbleiben, ohne jedem Consumer sichtbar zu sein.
- Selective Disclosure wird auch auf Metadaten anwendbar.
- Legacy-Export kann unbeabsichtigte Metadatenlecks verhindern.

### Negative Konsequenzen

- Metadata-Zugriffe benötigen zusätzliche Policy-Auswertung.
- Provider und Dateikonverter müssen Metadata-Klassifikationen korrekt erhalten.

## Verworfene Alternativen

### Alle NovaFile-Metadaten öffentlich

Verworfen, da Metadaten selbst sensible Informationen enthalten können.

### Alle Metadaten vollständig privat

Verworfen, da bestimmte Informationen für Discovery, Dateiverwaltung und Interoperabilität sichtbar sein müssen.

### Schutz ausschließlich über den Payload-Zugriff

Verworfen, da Metadata und Payload unterschiedliche Offenlegungsanforderungen besitzen können.

## Abhängigkeiten

- `ADR-STORAGE-0009_NovaFile_als_semantischer_Dateicontainer`
- `ADR-STORAGE-0010_NovaFile_Payload_und_Metadata_in_einem_logischen_Objekt`
- `ADR-STORAGE-0011_Legacy_Dateiprojektion_ohne_Duplikation`
- `ADR-PRIVACY-0001_Data_Minimization`
- `ADR-PRIVACY-0003_Semantic_Data_Labels`
- `ADR-PRIVACY-0005_Selective_Disclosure`
- `ADR-SECURITY-0015_Information_Flow_Control`

## Zugehörige NPSPECs

- `NPSPEC-PRIVACY-NOVAFILE-METADATA-0001`
- `NPSPEC-PRIVACY-METADATA-CLASSIFICATION-0001`
- `NPSPEC-PRIVACY-METADATA-VIEW-0001`
- `NPSPEC-PRIVACY-METADATA-EXPORT-0001`
- `NPSPEC-PRIVACY-METADATA-TEST-0001`

## Ergebnis

NovaFile trennt die Sichtbarkeit seiner Metadaten semantisch:

```text
Public
Protected
Private
```

Dadurch können notwendige Metadaten verfügbar bleiben, während sensible Informationen nach Authority, Purpose und Privacy Policy geschützt oder selektiv offengelegt werden.