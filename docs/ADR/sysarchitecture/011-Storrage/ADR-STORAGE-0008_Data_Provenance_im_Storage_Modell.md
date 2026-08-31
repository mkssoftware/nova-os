# ADR-STORAGE-0008 – Data Provenance im Storage-Modell

## Status

Angenommen

## Kategorie

Kernel / Storage / Provenance / Trust / Object Storage

## Kontext

NovaOS behandelt persistente Daten als Storage Objects mit stabiler Identität, Semantic Types, Metadaten und Versionen.

Für viele Systemfunktionen reicht es jedoch nicht zu wissen, **was** ein Objekt ist. Es muss auch nachvollziehbar sein, **woher** Daten stammen und durch welche Verarbeitungsschritte sie entstanden sind.

Dies ist insbesondere relevant für:

- Nova.Trust,
- KI-generierte oder KI-veränderte Inhalte,
- Dokumente und Medien,
- wissenschaftliche und technische Daten,
- Systemartefakte,
- Auditing,
- Reproduzierbarkeit.

Provenance soll deshalb nicht ausschließlich von einzelnen Anwendungen verwaltet werden.

---

## Entscheidung

NovaOS integriert **Data Provenance als native Eigenschaft des Storage-Modells**.

Storage Objects und ihre Versionen können strukturierte Herkunfts- und Transformationsinformationen besitzen.

```text
Source Object
      ↓
Capability / Transformation
      ↓
Derived Object
      ↓
Further Transformation
      ↓
Result Object
```

Provenance bildet damit einen nachvollziehbaren Graphen der Entstehung und Veränderung von Daten.

---

## Grundprinzip

```text
Data should be able to describe
where it came from
and how it was transformed.
```

---

## Provenance-Modell

Provenance kann insbesondere Beziehungen zwischen folgenden Elementen beschreiben:

```text
Object
Version
Source
Capability
Transformation
Identity
Timestamp
Trust Information
```

Nicht jedes Objekt muss sämtliche Informationen besitzen.

Fehlende oder unbekannte Provenance muss explizit von verifizierter Provenance unterscheidbar sein.

---

## Objekt- und Versionsbezug

Provenance wird grundsätzlich an konkrete Objektzustände beziehungsweise Versionen gebunden.

```text
Object A / Version 1
        ↓
Transformation X
        ↓
Object B / Version 1
```

Eine spätere Änderung von Object B darf die Provenance seiner vorherigen Version nicht rückwirkend verändern.

---

## Provenance-Graph

Provenance darf mehrere Quellen und mehrere Verarbeitungsschritte abbilden.

```text
Object A ─┐
          ├── Transformation → Object C
Object B ─┘
```

Damit können auch zusammengesetzte oder abgeleitete Daten nachvollzogen werden.

Die Provenance-Struktur ist deshalb als Graph und nicht ausschließlich als lineare Historie zu betrachten.

---

## Integration mit Nova.Trust

Provenance liefert Informationen für das systemweite Trust-Modell.

```text
Identity
   ↓
Signature
   ↓
Provenance
   ↓
Permission
   ↓
Capability
   ↓
Audit
```

Provenance selbst stellt jedoch keinen Vertrauensbeweis dar.

Nova.Trust entscheidet anhand von Provenance, Signaturen, Identitäten und weiteren Informationen über den resultierenden Trust-Zustand.

---

## Capabilities und Transformationen

Capabilities können bei der Erzeugung oder Transformation von Storage Objects Provenance-Einträge erzeugen.

Ein Eintrag kann beispielsweise festhalten:

```text
Input Objects
      ↓
Capability
      ↓
Output Object
```

Damit kann nachvollzogen werden, welche Systemfähigkeit an der Entstehung eines Objekts beteiligt war.

---

## KI-generierte Inhalte

KI-generierte oder KI-transformierte Inhalte verwenden dasselbe allgemeine Provenance-Modell.

NovaOS benötigt dafür keine separate, isolierte KI-Provenance-Architektur.

Soweit verfügbar, können beispielsweise Informationen über:

- verwendete Capability,
- Modellidentität,
- Eingabeobjekte,
- Transformation,

in die Provenance aufgenommen werden.

Die konkrete KI-Metadatenstruktur wird separat spezifiziert.

---

## Externe Daten

Importierte Daten können Provenance-Informationen aus externen Formaten oder Trust-Systemen übernehmen.

Dabei muss unterschieden werden zwischen:

```text
Verified Provenance
Declared Provenance
Derived Provenance
Unknown Provenance
```

Externe Angaben dürfen nicht automatisch als verifiziert gelten.

---

## Unveränderlichkeit

Bereits bestätigte Provenance-Einträge dürfen nicht stillschweigend umgeschrieben werden.

Korrekturen oder zusätzliche Erkenntnisse müssen nachvollziehbar ergänzt beziehungsweise über neue Objektversionen dargestellt werden.

Damit bleibt die Historie auditierbar.

---

## Security und Data Sovereignty

Provenance kann selbst sensible Informationen enthalten.

Beispielsweise können Herkunft, beteiligte Personen, Systeme oder Speicherorte schützenswert sein.

Deshalb unterliegt Provenance denselben:

- Authority-,
- Security-,
- Privacy-,
- Data-Sovereignty-

Regeln wie andere Metadaten.

Ein Teilnehmer darf aus einem Provenance-Graphen keine Informationen erhalten, für die keine entsprechende Authority besteht.

---

## VFS und externe Formate

Bei VFS-Projektionen darf Provenance auf geeignete externe Metadatenformate abgebildet werden.

Ein klassisches Dateisystem muss jedoch nicht die vollständige native Provenance-Semantik unterstützen.

Die native Provenance bleibt Bestandteil des Storage Objects und nicht des Dateipfads.

---

## Architecture Introspection

NovaOS muss Provenance kontrolliert introspektierbar machen.

Soweit autorisiert, sollen insbesondere sichtbar sein:

- Quellen,
- Versionen,
- Transformationen,
- beteiligte Capabilities,
- Provenance-Status,
- Trust-Bezug.

---

## Normative Anforderungen

1. NovaOS MUSS Data Provenance nativ im Storage-Modell unterstützen.
2. Provenance MUSS an Storage Objects beziehungsweise konkrete Objektversionen bindbar sein.
3. Provenance MUSS mehrere Quellen und Transformationen als Graph darstellen können.
4. Provenance-Einträge MÜSSEN strukturierte und typisierte Informationen unterstützen.
5. Verifizierte, deklarierte, abgeleitete und unbekannte Provenance MÜSSEN unterscheidbar sein.
6. Capabilities MÜSSEN Provenance für erzeugte oder transformierte Objekte bereitstellen können.
7. KI-generierte Inhalte SOLLEN das allgemeine Provenance-Modell verwenden.
8. Externe Provenance DARF NICHT automatisch als vertrauenswürdig gelten.
9. Bereits bestätigte Provenance DARF NICHT unbemerkt rückwirkend verändert werden.
10. Provenance MUSS mit Versioned Storage und Transactional Storage kombinierbar sein.
11. Provenance MUSS als Eingabe für Nova.Trust verwendbar sein.
12. Provenance DARF Authority oder Trust-Entscheidungen NICHT selbst ersetzen.
13. Provenance-Daten MÜSSEN Security- und Data-Sovereignty-Regeln unterliegen.
14. Provenance MUSS kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- nachvollziehbare Datenherkunft,
- gemeinsame Provenance-Architektur für Anwendungen, System und KI,
- bessere Grundlage für Nova.Trust,
- verbesserte Auditierbarkeit und Reproduzierbarkeit,
- Herkunft bleibt über Dateipfade und Storage Provider hinweg erhalten.

### Negative Konsequenzen

- zusätzlicher Metadata- und Storage-Aufwand,
- Provenance-Graphen können langfristig wachsen,
- externe Provenance besitzt unterschiedliche Vertrauensqualität,
- Provenance selbst kann sensible Informationen enthalten.

---

## Verworfene Alternativen

### Provenance ausschließlich durch Anwendungen

Verworfen.

Dadurch würden inkompatible und nicht systemweit nutzbare Provenance-Modelle entstehen.

### Provenance nur als einfache Änderungszeitlinie

Verworfen.

Daten können mehrere Quellen und verzweigte Transformationsketten besitzen.

### Provenance automatisch als Trust-Beweis behandeln

Verworfen.

Herkunftsinformation und Vertrauensbewertung sind getrennte Konzepte.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-STORAGE-0002_Natives_Object_Storage_Modell`
- `ADR-STORAGE-0004_Transactional_Storage`
- `ADR-STORAGE-0005_Versioned_Storage`
- `ADR-STORAGE-0006_Semantic_und_Typed_Storage`
- `ADR-STORAGE-0007_Metadata_First_Storage`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explicit_Contracts`

---

## Zugehörige NPSPECs

- `NPSPEC-STORAGE-PROVENANCE-0001`
- `NPSPEC-STORAGE-PROVENANCE-GRAPH-0001`
- `NPSPEC-STORAGE-PROVENANCE-TRANSFORM-0001`
- `NPSPEC-STORAGE-PROVENANCE-TRUST-0001`
- `NPSPEC-STORAGE-PROVENANCE-SECURITY-0001`
- `NPSPEC-STORAGE-PROVENANCE-TEST-0001`

---

## Ergebnis

NovaOS macht die Herkunft persistenter Daten zu einem Bestandteil des nativen Storage-Modells:

```text
Sources
   ↓
Transformations
   ↓
Object Versions
   ↓
Provenance Graph
   ↓
Trust / Audit / Reproducibility
```

Damit kann NovaOS nicht nur feststellen, welches Objekt vorliegt, sondern auch nachvollziehen, wie dieses Objekt entstanden ist.

Die zentrale Architekturregel lautet:

```text
Store the data.

Preserve its history.
```