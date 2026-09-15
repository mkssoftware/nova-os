# ADR-OBJECT-0004 – Object Provenance

## Status

Angenommen

## Kategorie

Object Architecture / Provenance / Trust

## Kontext

NovaOS-Objekte können erzeugt, importiert, kopiert, konvertiert, bearbeitet oder aus anderen Objekten abgeleitet werden.

Für Trust, Nachvollziehbarkeit und Reproduzierbarkeit muss erkennbar sein, woher ein Objekt oder eine konkrete Objektversion stammt.

## Entscheidung

NovaOS unterstützt **Object Provenance** als Bestandteil des Unified System Object Model.

Provenance wird versionsbezogen erfasst.

```text
Object Version
     ↓
Provenance
 ├── Origin
 ├── Creator
 ├── Operation
 └── Source Objects
```

## Versionsbezug

Provenance gehört zu einer konkreten Object Version.

```text
Object ID
 ├── V1 → Provenance A
 ├── V2 → Provenance B
 └── V3 → Provenance C
```

Dadurch bleibt nachvollziehbar, wie sich ein Objekt entwickelt hat.

## Ableitung

Wird ein Objekt aus anderen Objekten erzeugt, können diese als Quellen referenziert werden.

```text
Object A
    \
     → Operation → Object C
    /
Object B
```

Die Referenzen verwenden stabile Object IDs und bei Bedarf konkrete Version IDs.

## Provenance Chain

Mehrere Ableitungen bilden eine Provenance Chain.

```text
Object A
   ↓
Object B
   ↓
Object C
```

NovaOS muss diese Kette nachvollziehen können, soweit entsprechende Provenance-Daten vorhanden sind.

## Operation

Die erzeugende oder verändernde Operation kann referenziert werden.

```text
Source Object
      ↓
Capability / Operation
      ↓
Result Object
```

Damit kann nachvollzogen werden, wodurch eine neue Version entstanden ist.

## Trust

Provenance ist Trust Evidence, aber kein automatischer Vertrauensbeweis.

```text
Known Provenance
      ≠
Trusted Object
```

Trust Policies entscheiden separat, wie Provenance bewertet wird.

## Externe Objekte

Bei importierten Objekten kann die Provenance unvollständig oder unbekannt sein.

```text
Provenance:
    Unknown
```

NovaOS darf fehlende Herkunftsinformationen nicht erfinden.

## Normative Anforderungen

1. NovaOS MUSS Provenance für Objektversionen unterstützen können.
2. Provenance MUSS einer konkreten Object Version zugeordnet werden können.
3. Source Objects MÜSSEN über stabile Object IDs referenzierbar sein.
4. Konkrete Source Versions MÜSSEN referenzierbar sein können.
5. Erzeugende oder verändernde Operationen MÜSSEN beschreibbar sein.
6. Provenance Chains MÜSSEN nachvollziehbar sein können.
7. Unbekannte Provenance MUSS explizit darstellbar sein.
8. Provenance DARF nicht automatisch Trust implizieren.

## Konsequenzen

### Positive Konsequenzen

- Herkunft und Ableitung von Objekten werden nachvollziehbar.
- Objektversionen können reproduzierbarer werden.
- Trust-Entscheidungen erhalten zusätzliche Evidence.
- Data Linking und Versionshistorie werden miteinander verbunden.

### Negative Konsequenzen

- Provenance-Daten benötigen zusätzlichen Speicher.
- lange Ableitungsketten müssen effizient verwaltet werden.

## Abhängigkeiten

- `ADR-OBJECT-0001_Unified_System_Object_Model`
- `ADR-OBJECT-0002_Stabile_Object_Identity`
- `ADR-OBJECT-0003_Object_Versioning`
- `ADR-SEMANTIC-0008_Semantic_Relationships`
- `ADR-TRUST-0001_Trust_als_Evidence_basiertes_System`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`

## Zugehörige NPSPECs

- `NPSPEC-OBJECT-PROVENANCE-0001`
- `NPSPEC-OBJECT-PROVENANCE-CHAIN-0001`
- `NPSPEC-OBJECT-PROVENANCE-REFERENCE-0001`

## Ergebnis

NovaOS kann für eine Objektversion nachvollziehen, woher sie stammt und wodurch sie entstanden ist:

```text
Source Objects
      ↓
Operation
      ↓
Object Version
      ↓
Provenance Record
```

Der zentrale Grundsatz lautet:

```text
Ein Objekt besitzt nicht nur
eine Identität und Version,

sondern kann auch beschreiben,
woher dieser Zustand stammt.
```