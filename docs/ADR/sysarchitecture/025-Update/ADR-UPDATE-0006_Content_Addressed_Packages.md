# ADR-UPDATE-0006 – Content-Addressed Packages

## Status

Angenommen

## Kategorie

Update Architecture / Package Management / Integrity

## Kontext

NovaOS benötigt für Updates eine eindeutige und unveränderliche Identifikation von Softwarepaketen.

Klassische Paketidentitäten wie:

```text
Name + Version
```

reichen dafür nicht aus, da unterschiedliche Inhalte dieselbe Versionsbezeichnung tragen könnten.

Für:

- reproduzierbare Updates,
- Integritätsprüfung,
- Deduplizierung,
- Caching,
- Rollback,
- Supply-Chain-Provenance

muss der tatsächliche Paketinhalt eindeutig referenzierbar sein.

## Entscheidung

NovaOS verwendet **Content-Addressed Packages**.

Die Identität eines konkreten Paketartefakts wird aus seinem kryptografischen Inhaltshash abgeleitet.

```text
Package Content
      ↓
Cryptographic Hash
      ↓
Content ID
```

Ändert sich der relevante Paketinhalt, entsteht eine neue Content ID.

## Grundprinzip

```text
Same Content
    ↓
Same Content ID

Different Content
    ↓
Different Content ID
```

Paketname und Version bleiben semantische Metadaten, sind jedoch nicht die kryptografische Identität des Artefakts.

## Content ID

Die Content ID basiert auf einem zugelassenen kryptografischen Hashverfahren.

```text
hash:<algorithm>:<digest>
```

Die konkrete Hash-Suite und Kodierung werden durch eine NPSPEC definiert.

## Immutable Packages

Ein Artefakt mit bestehender Content ID ist unveränderlich.

```text
Package
Content ID = H1

Content changes
      ↓

New Package
Content ID = H2
```

Inhalt darf niemals unter derselben Content ID ersetzt werden.

## Name und Version

Menschenlesbare Namen und Versionen bleiben erhalten.

```text
nova.graphics
Version 4.2
      ↓
Content ID H7
```

Sie dienen der Auswahl und Darstellung.

Nach der Auflösung verwendet die Update Pipeline die konkrete Content ID.

## Manifest

Ein Paketmanifest beschreibt mindestens relevante Eigenschaften wie:

```text
Name
Version
Architecture
Dependencies
Capabilities
Content References
Execution Requirements
Provenance
```

Das Manifest muss selbst integritätsgeschützt und eindeutig dem Paketinhalt zugeordnet sein.

## Abhängigkeiten

Semantische Abhängigkeiten werden durch den Resolver auf konkrete Artefakte aufgelöst.

```text
Package A
   ↓
Dependency B >= 2
   ↓
Resolver
   ↓
Package B
Content ID H42
```

Der resultierende Update Plan referenziert konkrete Content IDs.

## Integritätsprüfung

Nach dem Erwerb eines Artefakts wird dessen Content ID erneut berechnet.

```text
Artifact
   ↓
Hash
   ↓
Expected Content ID?
   ├── Yes → Accept
   └── No  → Reject
```

Ein Hash-Mismatch führt zur Ablehnung des Artefakts.

## Signatur und Vertrauen

Content Addressing beweist die Identität und Integrität des referenzierten Inhalts, aber nicht dessen Vertrauenswürdigkeit.

```text
Content ID
    ≠
Signature
    ≠
Trust
```

Trust wird separat anhand von:

```text
Signature
Provenance
Trust Anchors
Supply Chain Evidence
Policy
```

bewertet.

Eine korrekte Content ID darf daher keine Trust-Prüfung ersetzen.

## Provenance

Provenance wird mit der konkreten Content ID verknüpft.

```text
Content ID
   ↓
Provenance
   ↓
Source / Build / Signer
```

Dadurch beziehen sich Herkunftsnachweise auf einen exakt definierten Inhalt.

## Reproducible Builds

Reproduzierbare Builds können durch Content IDs verglichen werden.

```text
Build A → H1
Build B → H1

H1 == H1
```

Identische Content IDs liefern starke Evidenz dafür, dass die verglichenen Artefaktbytes identisch sind.

Die Vertrauensentscheidung bleibt davon getrennt.

## Deduplizierung

Identische Artefakte müssen nicht mehrfach gespeichert werden.

```text
Reference A ─┐
Reference B ─┼──> Content H1
Reference C ─┘
```

Dies ermöglicht Deduplizierung über:

- Updates,
- Softwarezustände,
- Snapshots,
- A/B-Systembasen,
- lokale Caches.

## Cache

Content-addressed Artefakte können sicher über ihre Content ID gecacht werden.

```text
Need H42
   ↓
Cache contains H42?
   ├── Yes → Verify / Use
   └── No  → Acquire
```

Der Cache darf keine andere Nutzlast für dieselbe Content ID liefern.

## Mirrors

Ein Paket kann von unterschiedlichen Quellen bezogen werden.

```text
Mirror A ─┐
Mirror B ─┼──> Content ID H42
Mirror C ─┘
```

Die Quelle bestimmt nicht die Identität des Artefakts.

Trust- und Provenance-Regeln gelten weiterhin unabhängig davon.

## Update Plan

Ein vollständig aufgelöster Update Plan verwendet konkrete Content IDs.

```text
Desired State
     ↓
Resolver
     ↓
Resolved Update Plan
 ├── H10
 ├── H24
 └── H91
```

Damit bleibt der geplante Inhalt während der Transaction eindeutig bestimmt.

## Atomic Updates

Content IDs ermöglichen die eindeutige Referenzierung aller Artefakte eines Atomic Update Change Sets.

```text
Change Set
 ├── Kernel     → H1
 ├── Driver     → H2
 └── Service    → H3
```

Ein nachträglicher Austausch eines Artefakts unter derselben Identität ist ausgeschlossen.

## A/B-Systembasis

Beide Systembasen können auf gemeinsame unveränderliche Inhalte referenzieren.

```text
Base A ─┐
        ├── Content H1
Base B ─┘
```

Dadurch muss identischer Inhalt nicht zwangsläufig physisch dupliziert werden.

Die logische Trennung der Systembasen bleibt erhalten.

## Snapshot und Rollback

Snapshots und frühere Softwarezustände können konkrete Content IDs referenzieren.

```text
Software State 41
      ↓
H1 H2 H3

Software State 42
      ↓
H1 H4 H3
```

Dadurch kann NovaOS exakt bestimmen, welche Artefakte für einen Rollback benötigt werden.

## Garbage Collection

Ein Artefakt darf entfernt werden, wenn keine relevante Referenz mehr darauf besteht.

Zu berücksichtigen sind mindestens:

```text
Active State
Candidate State
Rollback State
Snapshots
Recovery
Retention
Transactions
```

Content Addressing ersetzt daher keine Referenz- und Lifecycle-Verwaltung.

## Hash-Algorithmus

NovaOS darf die Architektur nicht dauerhaft an einen einzelnen Hash-Algorithmus binden.

Content IDs müssen den verwendeten Algorithmus eindeutig kennzeichnen.

```text
algorithm:digest
```

Dadurch wird eine kontrollierte Hash-Agility ermöglicht.

## Hash Collision

Eine Hash-Kollision darf nicht als normaler Gleichheitsfall behandelt werden, wenn zusätzliche Evidenz einen Inhaltskonflikt zeigt.

Ein solcher Zustand muss als Integritäts- beziehungsweise Security-Ereignis behandelt werden.

## Security

Content IDs dürfen nicht als Berechtigungsnachweis verwendet werden.

```text
Know Content ID
      ≠
Authority
```

Der Zugriff auf Artefakte unterliegt weiterhin den normalen Security- und Capability-Regeln.

## Privacy

Content IDs können Informationen über bekannte Inhalte offenlegen und als Korrelationsmerkmal wirken.

Sie dürfen deshalb nicht unnötig über Trust- oder Privacy-Grenzen hinweg veröffentlicht werden.

## Recovery

Nach einem Crash kann NovaOS anhand der im Transaction State gespeicherten Content IDs feststellen, welche konkreten Artefakte zu einer Update Transaction gehören.

```text
Transaction
    ↓
Content IDs
    ↓
Verify Availability
    ↓
Continue / Rollback / Recover
```

## Normative Anforderungen

1. NovaOS MUSS konkrete Paketartefakte über kryptografische Content IDs identifizieren können.
2. Unterschiedlicher Paketinhalt MUSS eine unterschiedliche Content ID erzeugen, vorbehaltlich der Sicherheit des verwendeten Hashverfahrens.
3. Inhalt unter einer bestehenden Content ID MUSS unveränderlich behandelt werden.
4. Paketname und Version DÜRFEN die Content ID nicht ersetzen.
5. Aufgelöste Update Plans MÜSSEN konkrete Content IDs referenzieren können.
6. Erworbene Artefakte MÜSSEN vor Verwendung gegen ihre erwartete Content ID geprüft werden.
7. Ein Hash-Mismatch MUSS zur Ablehnung des Artefakts führen.
8. Content Addressing DARF Signatur-, Provenance- oder Trust-Prüfungen nicht ersetzen.
9. Provenance MUSS mit konkreten Artefakten beziehungsweise Content IDs verknüpfbar sein.
10. Identische Content IDs SOLLEN für Deduplizierung und Caching nutzbar sein.
11. Content-addressed Artefakte MÜSSEN unabhängig von ihrem Transportweg identifizierbar bleiben.
12. Atomic Update Change Sets MÜSSEN konkrete Artefaktidentitäten festlegen können.
13. Rollback- und Snapshot-Zustände MÜSSEN konkrete benötigte Artefakte referenzieren können.
14. Garbage Collection DARF keine noch für Active State, Rollback, Recovery oder Transactions benötigten Artefakte entfernen.
15. Content IDs MÜSSEN den verwendeten Hash-Algorithmus eindeutig bestimmen.
16. Die Architektur MUSS Hash-Agility ermöglichen.
17. Ein erkannter Inhaltskonflikt bei gleicher Content ID MUSS als Security- beziehungsweise Integrity-Ereignis behandelt werden.
18. Eine Content ID DARF keine Authority erzeugen.
19. Content IDs MÜSSEN entsprechend geltender Privacy-Regeln behandelt werden.
20. Die Transactional Update Pipeline MUSS Content IDs für Recovery verwenden können.

## Konsequenzen

### Positive Konsequenzen

- konkrete Paketartefakte sind eindeutig identifizierbar,
- Manipulation und Übertragungsfehler werden erkennbar,
- Update Plans werden reproduzierbarer,
- Deduplizierung und sichere Caches werden möglich,
- A/B, Snapshots und Rollback können Inhalte gemeinsam nutzen,
- Provenance kann exakt an Artefakte gebunden werden.

### Negative Konsequenzen

- Hashes und Referenzen müssen verwaltet werden,
- Garbage Collection benötigt Referenzinformationen,
- Hash-Agility muss von Anfang an berücksichtigt werden,
- Content IDs können Privacy-relevante Korrelationsmerkmale darstellen.

## Verworfene Alternativen

### Name und Version als eindeutige Paketidentität

Verworfen, da derselbe Name und dieselbe Version unterschiedliche Inhalte bezeichnen könnten.

### Downloadquelle als Identität

Verworfen, da Mirrors und Provider austauschbar sein sollen.

### Signatur als Paketidentität

Verworfen, da Signatur, Inhaltsidentität und Vertrauen getrennte Konzepte sind.

### Veränderbare Pakete unter stabiler ID

Verworfen, da dadurch Reproduzierbarkeit, Caching und Rollback unsicher würden.

### Fest verdrahteter einzelner Hash-Algorithmus

Verworfen, da kryptografische Algorithmen langfristig austauschbar bleiben müssen.

## Abhängigkeiten

- `ADR-UPDATE-0001_Deklarativer_Softwarezustand`
- `ADR-UPDATE-0002_Transactional_Update_Pipeline`
- `ADR-UPDATE-0003_Atomic_Updates`
- `ADR-UPDATE-0004_A_B_Systembasis`
- `ADR-UPDATE-0005_Snapshot_Based_Rollback`
- `ADR-STORAGE-0003_Content_Addressed_Storage`
- `ADR-STORAGE-0005_Versioned_Storage`
- `ADR-STORAGE-0015_Checksumming_für_Integrität`
- `ADR-TRUST-0003_Signatur_ist_nicht_Vertrauen`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`
- `ADR-TRUST-0011_Reproducible_Builds_als_Trust_Evidence`
- `ADR-TRANSACTION-0006_Transaction_Log_und_Power_Failure_Safety`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-CONTENT-PACKAGE-0001`
- `NPSPEC-UPDATE-CONTENT-ID-0001`
- `NPSPEC-UPDATE-PACKAGE-MANIFEST-0001`
- `NPSPEC-UPDATE-PACKAGE-RESOLUTION-0001`
- `NPSPEC-UPDATE-PACKAGE-INTEGRITY-0001`
- `NPSPEC-UPDATE-PACKAGE-CACHE-0001`
- `NPSPEC-UPDATE-PACKAGE-GC-0001`
- `NPSPEC-UPDATE-HASH-AGILITY-0001`
- `NPSPEC-UPDATE-CONTENT-PACKAGE-TEST-0001`

## Ergebnis

NovaOS identifiziert konkrete Softwareartefakte anhand ihres tatsächlichen Inhalts:

```text
Package Content
      ↓
Cryptographic Hash
      ↓
Content ID
      ↓
Verify
      ↓
Update / Cache / Rollback
```

Name und Version beschreiben ein Paket.

Die Content ID bestimmt dagegen eindeutig das konkrete Artefakt, das NovaOS verwenden soll.

Der zentrale Grundsatz lautet:

```text
Nicht der Name bestimmt,
welches Paket vorliegt,
sondern sein verifizierter Inhalt.
```