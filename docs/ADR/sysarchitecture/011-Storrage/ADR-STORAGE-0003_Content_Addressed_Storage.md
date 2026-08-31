# ADR-STORAGE-0003 – Content-Addressed Storage

## Status

Angenommen

## Kategorie

Kernel / Storage / Object Model / Integrity / Deduplication

## Kontext

Das native Object-Storage-Modell von NovaOS trennt die Identität eines Storage Objects bereits von seinem Pfad und physischen Speicherort.

Für unveränderliche Dateninhalte ist zusätzlich eine adressierbare Identität auf Basis ihres Inhalts sinnvoll.

Content-Addressed Storage (CAS) ermöglicht:

- Integritätsprüfung,
- sichere Wiederverwendung identischer Inhalte,
- Deduplication,
- effiziente Snapshots und Versionierung,
- verlässliche Referenzierung unveränderlicher Daten.

CAS darf jedoch nicht die logische Identität eines Storage Objects ersetzen.

---

## Entscheidung

NovaOS unterstützt **Content-Addressed Storage als native Storage-Funktion für geeignete immutable beziehungsweise versionierte Daten**.

```text
Content
   ↓
Content Identifier
   ↓
CAS Object
```

Der Content Identifier wird deterministisch aus dem Inhalt abgeleitet.

Dabei werden zwei Identitäten unterschieden:

```text
Object Identity
→ Identität des logischen Storage Objects

Content Identity
→ Identität eines konkreten Inhalts
```

Ein Storage Object kann dadurch im Laufe seiner Lebensdauer auf unterschiedliche Content-Versionen verweisen.

---

## Grundprinzip

```text
Object identity describes what it is.

Content identity describes what it contains.
```

---

## Immutable Content

Ein bereits adressierter CAS-Inhalt wird nicht nachträglich verändert.

Ändert sich der Inhalt, entsteht eine neue Content Identity.

```text
Content A
   ↓ modification
Content B
   ↓
New Content Identifier
```

Dadurch bleibt eine Content-Adresse dauerhaft an exakt denselben Inhalt gebunden.

---

## Content Identifier

Content Identifier basieren auf einem kryptographisch geeigneten Content-Digest.

Die konkrete Hashfunktion wird nicht dauerhaft auf ADR-Ebene festgelegt.

Das Format muss:

- algorithmusidentifizierbar,
- versionierbar,
- erweiterbar

sein.

Damit kann NovaOS Hashverfahren zukünftig austauschen oder parallel unterstützen.

---

## Verhältnis zum Object Storage

CAS ergänzt `ADR-STORAGE-0002`, ersetzt es aber nicht.

```text
Storage Object
      ↓
Object Metadata
      ↓
Content Reference
      ↓
CAS Content
```

Mehrere Storage Objects dürfen auf denselben CAS-Inhalt verweisen.

Dadurch kann physisches Backing wiederverwendet werden, ohne logische Objektidentitäten zusammenzuführen.

---

## Deduplication

Identische CAS-Inhalte dürfen dasselbe physische Backing verwenden.

```text
Object A ─┐
          ├── Content X
Object B ─┘
```

Dies ermöglicht deterministische Deduplication ohne nachträgliches Scannen beliebiger Speicherinhalte.

Ownership, Authority und Accounting der Storage Objects bleiben trotzdem getrennt.

---

## Integrität

Beim Lesen eines CAS-Inhalts kann seine Content Identity zur Integritätsprüfung verwendet werden.

Eine Abweichung zwischen erwartetem Identifier und tatsächlichem Inhalt muss als Integritätsfehler behandelt werden.

CAS ersetzt jedoch keine:

- Signatur,
- Provenance,
- Trust-Bewertung,
- Zugriffsberechtigung.

Ein korrekter Hash beweist nur die Identität des Inhalts.

---

## Security und Data Sovereignty

Die Kenntnis eines Content Identifiers gewährt keine Authority auf den Inhalt.

```text
Content ID ≠ Capability
```

CAS darf außerdem nicht dazu führen, dass Inhalte unterschiedlicher Security- oder Sovereignty-Domänen unkontrolliert gemeinsam gespeichert oder sichtbar gemacht werden.

Physische Deduplication bleibt daher policy- und trustabhängig.

---

## Versionierung

CAS eignet sich als Grundlage für versionierte Storage Objects.

```text
Object
 ├── Version 1 → Content A
 ├── Version 2 → Content B
 └── Version 3 → Content C
```

Die konkrete Versionierungs- und Snapshot-Semantik wird separat definiert.

---

## Location Transparency

Ein CAS-Inhalt kann auf unterschiedlichen Storage Providern liegen oder repliziert sein.

Die Content Identity bleibt dabei unverändert.

```text
Same Content ID
    ↓
Local / Remote / Replicated Storage
```

Location, Kosten und Verfügbarkeit bleiben weiterhin sichtbar, wenn sie für eine Operation relevant sind.

---

## Garbage Collection

CAS-Inhalte können von mehreren Objekten und Versionen referenziert werden.

Ihre physische Lebensdauer darf deshalb nicht allein an eine einzelne VFS-Datei oder ein einzelnes Storage Object gekoppelt werden.

Nicht mehr erreichbare CAS-Inhalte müssen kontrolliert reclaimbar sein.

Die konkrete Garbage-Collection-Strategie wird in einer NPSPEC definiert.

---

## Architecture Introspection

NovaOS muss kontrolliert introspektierbar machen können:

- Object Identity,
- Content Identity,
- verwendeten Digest-Algorithmus,
- Referenzen,
- Storage Provider,
- Integritätsstatus.

Sensitive Informationen und nicht autorisierte Inhalte dürfen dadurch nicht offengelegt werden.

---

## Normative Anforderungen

1. NovaOS MUSS Content-Addressed Storage für geeignete persistente Daten unterstützen.
2. Object Identity und Content Identity MÜSSEN getrennte Konzepte bleiben.
3. Ein Content Identifier MUSS deterministisch aus dem Inhalt ableitbar sein.
4. Bereits adressierter CAS-Inhalt MUSS immutable behandelt werden.
5. Geänderter Inhalt MUSS eine neue Content Identity erhalten.
6. Das Content-Identifier-Format MUSS unterschiedliche und zukünftige Digest-Algorithmen unterstützen können.
7. Mehrere Storage Objects DÜRFEN denselben CAS-Inhalt referenzieren.
8. CAS DARF zur physischen Deduplication verwendet werden.
9. Content Identifier DÜRFEN NICHT als Zugriffsberechtigung behandelt werden.
10. Deduplication über Security- oder Sovereignty-Grenzen MUSS policygesteuert sein.
11. CAS-Inhalte MÜSSEN unabhängig von ihrem physischen Storage Provider identifizierbar bleiben.
12. Nicht mehr referenzierte CAS-Inhalte MÜSSEN kontrolliert reclaimbar sein.
13. CAS-Identität und Integritätszustand MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- eindeutige Identifikation unveränderlicher Inhalte,
- integrierte Integritätsprüfung,
- natürliche Deduplication,
- effiziente Grundlage für Versionierung und Snapshots,
- Wiederverwendung identischer Inhalte über mehrere Storage Objects,
- unabhängige Replikation und Migration von Inhalten.

### Negative Konsequenzen

- Content-Digests müssen berechnet und verwaltet werden,
- Garbage Collection benötigt Referenz- beziehungsweise Reachability-Informationen,
- mutable Daten benötigen neue Content-Versionen,
- Deduplication muss Security- und Sovereignty-Grenzen berücksichtigen.

---

## Verworfene Alternativen

### Content Identity ersetzt Object Identity

Verworfen.

Logisch unterschiedliche Objekte können denselben Inhalt besitzen.

### CAS für sämtliche mutable Daten erzwingen

Verworfen.

CAS ist besonders für immutable und versionierte Inhalte geeignet und darf nicht unnötig jedes Storage-Modell bestimmen.

### Content Identifier als Capability

Verworfen.

Die Kenntnis eines Identifiers darf keinen Zugriff auf den zugehörigen Inhalt gewähren.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-STORAGE-0001_VFS_als_Kompatibilitäts_und_Projektionsschicht`
- `ADR-STORAGE-0002_Natives_Object_Storage_Modell`
- `ADR-ARCH-0004_Transactionale_Systemoperationen`
- `ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`

---

## Zugehörige NPSPECs

- `NPSPEC-STORAGE-CAS-0001`
- `NPSPEC-STORAGE-CONTENT-ID-0001`
- `NPSPEC-STORAGE-CAS-INTEGRITY-0001`
- `NPSPEC-STORAGE-CAS-DEDUP-0001`
- `NPSPEC-STORAGE-CAS-GC-0001`
- `NPSPEC-STORAGE-CAS-TEST-0001`

---

## Ergebnis

NovaOS ergänzt seine stabile logische Objektidentität um eine separate Identität für konkrete Inhalte:

```text
Storage Object
      ↓
Object Identity
      +
Content Reference
      ↓
Content Identity
      ↓
Immutable CAS Content
```

Damit können Integrität, Deduplication, Versionierung und Replikation auf einer gemeinsamen Storage-Grundlage aufgebaut werden, ohne logische Objekte mit ihren konkreten Datenversionen gleichzusetzen.

Die zentrale Architekturregel lautet:

```text
Objects have identity.

Content has identity.

Do not confuse the two.
```