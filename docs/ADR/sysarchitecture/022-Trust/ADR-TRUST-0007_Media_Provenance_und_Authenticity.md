# ADR-TRUST-0007 – Media Provenance und Authenticity

## Status

Angenommen

## Kategorie

Trust Architecture / Media Trust / Provenance

## Kontext

Digitale Bilder, Videos und Audiodaten können aufgenommen, bearbeitet, konvertiert oder vollständig synthetisch erzeugt werden.

Eine gültige Signatur beweist dabei nicht automatisch, dass ein Medium unverändert, authentisch aufgenommen oder inhaltlich wahr ist.

NovaOS benötigt deshalb ein Modell, das Herkunft und Bearbeitungshistorie von Medien nachvollziehbar macht, ohne daraus unbegründete Aussagen über deren Inhalt abzuleiten.

## Entscheidung

Nova.Trust behandelt **Media Provenance und Authenticity als native Trust-Eigenschaften**.

```text
Capture / Generation
        ↓
Transformations
        ↓
Current Media
        ↓
Provenance Evidence
        ↓
Nova.Trust
```

NovaOS kann damit Herkunft, Erzeugungsart und relevante Transformationen eines Mediums nachvollziehen und separat bewerten.

## Grundprinzip

```text
Authentic provenance
≠
True content
```

Nova.Trust unterscheidet insbesondere zwischen:

```text
Provenance   → Woher stammt das Medium?
Integrity    → Wurde es unbemerkt verändert?
Authenticity → Sind Herkunftsbehauptungen verifizierbar?
Content      → Was zeigt oder behauptet das Medium?
```

## Media Provenance

Provenance kann relevante Stationen eines Mediums miteinander verbinden:

```text
Camera / Generator
        ↓
Original Media
        ↓
Edit
        ↓
Conversion
        ↓
Published Media
```

Bearbeitungen zerstören Provenance nicht zwangsläufig, sondern können als neue nachvollziehbare Transformationen ergänzt werden.

## Aufnahme und Generierung

Nova.Trust muss zwischen unterschiedlichen Ursprüngen unterscheiden können, beispielsweise:

- physische Aufnahme,
- Software-Rendering,
- KI-generierter Inhalt,
- zusammengesetzte Medien,
- unbekannte Herkunft.

Eine KI-generierte Datei darf daher eine vollständig verifizierbare Provenance besitzen, ohne als physische Aufnahme dargestellt zu werden.

## Provenance Evidence

Media Provenance kann durch geeignete Evidence unterstützt werden, beispielsweise:

```text
Device Identity
Creator Identity
Signature
Trusted Timestamp
Transformation Record
Content Binding
```

Nova.Trust bleibt dabei unabhängig von einem einzelnen Medienformat oder Provenance-Standard.

## Transformationen

Medienbearbeitung muss als normaler Bestandteil der Provenance Chain behandelt werden können.

```text
Original
   ↓
Crop
   ↓
Color Correction
   ↓
Export
```

Eine Bearbeitung bedeutet nicht automatisch Manipulation im sicherheitsrelevanten Sinn.

Entscheidend ist, ob die relevante Transformation nachvollziehbar und mit den behaupteten Eigenschaften vereinbar ist.

## Fehlende Provenance

Legacy-Medien oder externe Inhalte können keine oder nur unvollständige Provenance besitzen.

```text
Unknown Provenance ≠ False Content
Unknown Provenance ≠ Verified Provenance
```

NovaOS darf fehlende Provenance weder als Beweis für Manipulation noch als bestätigte Authentizität behandeln.

## NovaFile Integration

NovaFile kann Media-Provenance als geschützte semantische Metadaten mit dem Medium verbinden.

Bei Export in Legacy-Formate muss die Provenance soweit möglich erhalten oder kontrolliert projiziert werden können.

## Normative Anforderungen

1. Nova.Trust MUSS Media Provenance als native Trust Evidence unterstützen.
2. Provenance, Integrity, Authenticity und inhaltliche Wahrheit MÜSSEN getrennte Konzepte bleiben.
3. NovaOS MUSS physisch aufgenommene, synthetische und unbekannte Medienherkunft unterscheiden können.
4. KI-generierte Medien MÜSSEN als solche Provenance besitzen können.
5. Transformationen MÜSSEN Bestandteil einer Provenance Chain sein können.
6. Bearbeitung DARF nicht automatisch als Vertrauensverlust interpretiert werden.
7. Media Provenance MUSS kryptographisch verifizierbare Evidence integrieren können.
8. Nova.Trust DARF nicht von einem einzelnen Provenance-Standard abhängig sein.
9. Fehlende Provenance DARF weder als bestätigte Authentizität noch als Beweis für Fälschung gelten.
10. Provenance MUSS mit NovaFile-Metadaten integrierbar sein.
11. Provenance oder Authenticity DARF keine Capability Authority erzeugen.
12. Relevante Media-Provenance-Beziehungen MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- nachvollziehbare Herkunft von Bildern, Videos und Audio,
- KI-generierte und physisch aufgenommene Medien können unterschieden werden,
- Bearbeitungshistorien können erhalten bleiben,
- Medienauthentizität wird Teil von Nova.Trust,
- keine falsche Gleichsetzung von Provenance und inhaltlicher Wahrheit.

### Negative Konsequenzen

- ältere Medien besitzen häufig keine verifizierbare Provenance,
- externe Formate können Provenance nur eingeschränkt transportieren.

## Verworfene Alternativen

### Signiertes Medium bedeutet authentischer Inhalt

Verworfen, da eine Signatur keine Aussage über die inhaltliche Wahrheit eines Mediums garantiert.

### Bearbeitete Medien grundsätzlich als nicht vertrauenswürdig behandeln

Verworfen, da legitime Transformationen Bestandteil einer nachvollziehbaren Provenance Chain sein können.

### Ein bestimmter Provenance-Standard als NovaOS-Grundmodell

Verworfen, da die Architektur standard- und formatunabhängig bleiben soll.

## Abhängigkeiten

- `ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur`
- `ADR-TRUST-0003_Signatur_ist_nicht_Vertrauen`
- `ADR-TRUST-0005_Trust_Graph_statt_globalem_Trust_Score`
- `ADR-STORAGE-0008_Data_Provenance_im_Storage_Modell`
- `ADR-STORAGE-0009_NovaFile_als_semantischer_Dateicontainer`
- `ADR-PRIVACY-0008_Public_Protected_Private_NovaFile_Metadata`
- `ADR-IDENTITY-0004_Device_Identity`

## Zugehörige NPSPECs

- `NPSPEC-TRUST-MEDIA-PROVENANCE-0001`
- `NPSPEC-TRUST-MEDIA-AUTHENTICITY-0001`
- `NPSPEC-TRUST-MEDIA-TRANSFORMATION-0001`
- `NPSPEC-TRUST-MEDIA-GENERATION-0001`
- `NPSPEC-TRUST-MEDIA-PROVENANCE-TEST-0001`

## Ergebnis

Nova.Trust behandelt die Herkunft digitaler Medien als nachvollziehbare Provenance Chain:

```text
Capture / Generation
        ↓
Transformations
        ↓
Media Artifact
        ↓
Provenance Evidence
        ↓
Authenticity Evaluation
```

Damit kann NovaOS nachvollziehen, **woher ein Medium stammt und wie es entstanden oder verändert worden ist**, ohne Provenance, Authentizität und inhaltliche Wahrheit miteinander gleichzusetzen.