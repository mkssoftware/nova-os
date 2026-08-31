# ADR-STORAGE-0011 – Legacy-Dateiprojektion ohne Duplikation

## Status

Angenommen

## Kategorie

Storage / VFS / NovaFile / Compatibility / Projection

## Kontext

NovaOS verwendet native Storage Objects und NovaFile als semantisches Datenmodell. Bestehende Anwendungen erwarten dagegen klassische Dateien mit bekannten Formaten wie:

- PNG,
- JPEG,
- PDF,
- MP3,
- MP4,
- DOCX,
- ZIP.

Ein NovaFile kann solche Formate als Payload beziehungsweise Content Representation enthalten.

Würde NovaOS für Legacy-Anwendungen zusätzlich eine vollständige klassische Datei erzeugen, könnten dieselben Nutzdaten mehrfach gespeichert werden:

```text
NovaFile
 └── Payload: image.png

Legacy Projection
 └── image.png   ← duplicate payload
```

Dies widerspricht Copy Avoidance, CAS-Deduplication und dem einheitlichen Storage-Modell.

---

## Entscheidung

NovaOS stellt Legacy-Dateien grundsätzlich als **Projektionen vorhandener Storage Objects beziehungsweise NovaFile-Payloads ohne dauerhafte Datenduplikation** bereit.

```text
NovaFile Object
      │
      └── Payload / Representation
                 │
                 ├── Native Semantic View
                 │
                 └── VFS Legacy Projection
                          ↓
                     image.png
```

Native und klassische Ansicht referenzieren dabei denselben zugrunde liegenden Inhalt, soweit das benötigte Legacy-Format bereits als geeignete Repräsentation vorhanden ist.

---

## Grundprinzip

```text
One payload.

Multiple views.
```

---

## Legacy-Projektion

Eine Legacy-Projektion stellt eine vorhandene Content Representation über klassische Dateisemantik bereit.

Für eine Legacy-Anwendung kann dadurch beispielsweise:

```text
Photo.nova
```

als:

```text
Photo.jpg
```

erscheinen, wenn das NovaFile bereits eine geeignete JPEG-Repräsentation enthält.

Die projizierte Datei ist keine unabhängige dauerhafte Kopie des Payloads.

---

## Gemeinsames Backing

Native und Legacy-Ansicht sollen dasselbe Storage Backing verwenden.

```text
                 CAS Content
                /           \
       NovaFile View      Legacy View
```

Object Identity, NovaFile Identity und Legacy-Pfad bleiben dabei unterschiedliche Konzepte.

Die gemeinsame Nutzung des Backings führt nicht zu einer Zusammenführung ihrer jeweiligen Semantik.

---

## VFS als Projektionsschicht

Die Legacy-Dateiprojektion wird über die in `ADR-STORAGE-0001` definierte VFS-Projektionsarchitektur bereitgestellt.

Das VFS kann dadurch klassische Eigenschaften wie:

- Dateiname,
- Dateiendung,
- Verzeichnisstruktur,
- Byte-Stream-Zugriff

darstellen, während das native Storage Object unverändert bestehen bleibt.

---

## Schreibzugriffe

Schreibzugriffe einer Legacy-Anwendung dürfen das native Storage-Modell nicht umgehen.

Wird eine projizierte Datei verändert, muss die Änderung kontrolliert in einen neuen konsistenten Objektzustand überführt werden.

```text
Legacy Write
     ↓
Storage Transaction
     ↓
New Content
     ↓
New Object Version
```

Bei CAS-basiertem Content entsteht für geänderte Daten eine neue Content Identity.

---

## Formatkompatibilität

Eine direkte Projektion ohne Konvertierung ist möglich, wenn bereits eine kompatible Content Representation vorhanden ist.

Ist eine benötigte Repräsentation nicht vorhanden, kann eine Capability eine kompatible Darstellung erzeugen.

```text
Native Representation
        ↓
Conversion Capability
        ↓
Legacy Representation
```

Eine solche Konvertierung ist eine tatsächliche Datentransformation und darf nicht fälschlich als reine Projektion bezeichnet werden.

---

## Temporäre und persistente Repräsentationen

Erzeugte Legacy-Repräsentationen können abhängig von Nutzung und Policy:

- temporär,
- gecacht,
- persistent

gespeichert werden.

Persistente zusätzliche Repräsentationen bleiben Bestandteil desselben logischen Storage Objects und werden nicht als unabhängige Benutzerkopien behandelt.

Die Auswahl ist Policy- und Resource-Economy-Entscheidung.

---

## Metadaten

Nicht alle nativen NovaOS-Metadaten können in klassische Dateiformate projiziert werden.

Der Legacy-View darf deshalb eine reduzierte Sicht darstellen.

```text
Native Object
 ├── Semantic Metadata
 ├── Provenance
 ├── Relations
 └── Payload

Legacy File
 ├── Traditional Attributes
 └── Payload
```

Die native Metadatenquelle bleibt autoritativ.

Ein eingeschränkter Legacy-View darf nicht zum Verlust der nativen Metadaten führen.

---

## Security und Data Sovereignty

Eine Legacy-Projektion erzeugt keine zusätzlichen Zugriffsrechte.

Die Bereitstellung der Projektion muss denselben:

- Authority-,
- Security-,
- Trust-,
- Data-Sovereignty-

Regeln unterliegen wie das zugrunde liegende Storage Object.

Insbesondere darf eine Projektion keine geschützten Payloads oder Metadaten verfügbar machen, die der Legacy-Anwendung nicht zugänglich sein dürfen.

---

## Resource Economy

Auch Projektionen verursachen Ressourcenverbrauch, beispielsweise durch:

- Konvertierung,
- Caching,
- temporäre Repräsentationen,
- zusätzliche Metadaten.

Diese Kosten müssen der Resource Economy zurechenbar sein.

Eine dauerhafte zusätzliche Repräsentation soll nur erzeugt werden, wenn sie gegenüber wiederholter Konvertierung sinnvoll ist.

---

## Normative Anforderungen

1. NovaOS MUSS Legacy-Dateiansichten als VFS-Projektionen nativer Storage Objects bereitstellen können.
2. Eine Legacy-Projektion SOLL vorhandenen Payload ohne dauerhafte Duplikation wiederverwenden.
3. Native und Legacy-Ansicht DÜRFEN dasselbe Content Backing referenzieren.
4. Legacy-Pfade DÜRFEN NICHT zur nativen Object Identity werden.
5. Eine vorhandene kompatible Content Representation SOLL direkt projiziert werden.
6. Erforderliche Formatkonvertierungen MÜSSEN als Transformation und nicht als reine Projektion behandelt werden.
7. Legacy-Schreibzugriffe MÜSSEN kontrolliert in das native Storage-Modell zurückgeführt werden.
8. Änderungen an CAS-basiertem Content MÜSSEN eine neue Content Identity erzeugen.
9. Legacy-Schreibzugriffe SOLLEN Transactional und Versioned Storage verwenden.
10. Zusätzliche persistente Repräsentationen MÜSSEN dem logischen Storage Object zugeordnet bleiben.
11. Eine reduzierte Legacy-Metadatenansicht DARF native Metadaten NICHT zerstören.
12. Legacy-Projektionen DÜRFEN Security- oder Data-Sovereignty-Regeln NICHT umgehen.
13. Konvertierungs-, Cache- und Projektionsressourcen MÜSSEN der Resource Economy zurechenbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- klassische Anwendungen können native NovaOS-Daten verwenden,
- keine unnötige doppelte Speicherung identischer Payloads,
- CAS und Copy Avoidance bleiben wirksam,
- native Metadaten und Semantik bleiben erhalten,
- bestehende Dateiformate bleiben vollständig nutzbar.

### Negative Konsequenzen

- VFS-Projektionen benötigen Übersetzungslogik,
- Legacy-Schreibzugriffe müssen in das native Objektmodell zurückgeführt werden,
- fehlende Repräsentationen können Konvertierungskosten verursachen,
- nicht jede native Metadateninformation ist in Legacy-Formaten sichtbar.

---

## Verworfene Alternativen

### Für jede Legacy-Ansicht eine vollständige Dateikopie erzeugen

Verworfen.

Dies würde Speicher verschwenden und unabhängige, potenziell inkonsistente Datenkopien erzeugen.

### NovaFile direkt für jede Legacy-Anwendung sichtbar machen

Verworfen.

Legacy-Anwendungen verstehen die semantische NovaFile-Struktur nicht zwingend.

### Legacy-Kompatibilität nur über Export und Reimport

Verworfen.

Dies würde unnötige Kopien erzeugen und die native Integration klassischer Anwendungen erschweren.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-STORAGE-0001_VFS_als_Kompatibilitäts_und_Projektionsschicht`
- `ADR-STORAGE-0002_Natives_Object_Storage_Modell`
- `ADR-STORAGE-0003_Content_Addressed_Storage`
- `ADR-STORAGE-0004_Transactional_Storage`
- `ADR-STORAGE-0005_Versioned_Storage`
- `ADR-STORAGE-0009_NovaFile_als_semantischer_Dateicontainer`
- `ADR-STORAGE-0010_NovaFile_Payload_und_Metadata_in_einem_logischen_Objekt`
- `ADR-DATAMOVE-0005_Copy_Avoidance_vor_blinder_Zero_Copy_Optimierung`

---

## Zugehörige NPSPECs

- `NPSPEC-STORAGE-LEGACY-PROJECTION-0001`
- `NPSPEC-STORAGE-LEGACY-WRITE-0001`
- `NPSPEC-STORAGE-REPRESENTATION-0001`
- `NPSPEC-STORAGE-REPRESENTATION-CONVERSION-0001`
- `NPSPEC-STORAGE-LEGACY-PROJECTION-TEST-0001`

---

## Ergebnis

NovaOS trennt die logische Darstellung eines Datenobjekts von dessen physischem Payload:

```text
                 Storage Object
                       ↓
                 Shared Content
                  /          \
          NovaFile View    Legacy View
                           image.jpg
```

Dadurch können native NovaOS-Anwendungen und klassische Software dieselben Daten verwenden, ohne dass für jede Darstellung eine unabhängige Kopie entstehen muss.

Die zentrale Architekturregel lautet:

```text
Project the data.

Do not duplicate it.
```