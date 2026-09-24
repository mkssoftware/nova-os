# NPSPEC-MEMORY-DEDUP-0001 – Nova Memory Deduplication

## Status

Entwurf

## Kategorie

Memory / Deduplication / Reclamation / Shared Pages

## Zweck

NovaOS definiert Memory Deduplication als optionalen Mechanismus zur Erkennung identischer physischer Speicherseiten und deren kontrollierter Zusammenführung.

```text
Frame A → Data X
Frame B → Data X
Frame C → Data X
        ↓
   Deduplication
        ↓
Shared Frame → Data X
```

Dadurch kann redundanter physischer Speicher freigegeben werden, ohne die logische Isolation der beteiligten Address Spaces aufzugeben.

## Grundprinzip

Identische Speicherinhalte dürfen physisch zusammengeführt werden, solange spätere Änderungen über Copy-on-Write getrennt werden.

```text
VAS A ─┐
       ├→ Shared Read-Only Frame
VAS B ─┘
```

Bei einem Schreibzugriff:

```text
Write
 ↓
COW Fault
 ↓
Private Frame
```

Dabei gilt:

```text
Identical Content ≠ Same Object
Deduplication ≠ Shared-Memory-Semantik
Deduplication ≠ Permission Sharing
```

Deduplication verändert keine logische Ownership oder Berechtigung.

## Deduplication Domain

NovaOS darf Deduplication nur innerhalb explizit zulässiger Domains durchführen.

```text
Dedup Domain
├── Process Group
├── Service
├── Security Domain
├── VM / Compatibility Domain
└── System-approved Domain
```

Cross-Security-Domain-Deduplication darf standardmäßig nicht vorausgesetzt werden.

## Kandidatenauswahl

Nicht jede Page soll permanent verglichen werden.

Geeignete Kandidaten können anhand folgender Eigenschaften ausgewählt werden:

```text
Inactive
Anonymous
Private
Stable
Reclaimable
Not Pinned
Not Device-backed
```

Aktiv veränderte Pages sollen nicht unnötig gescannt werden.

## Erkennung

NovaOS darf zunächst einen Fingerprint verwenden.

```text
Page
 ↓
Hash / Fingerprint
 ↓
Candidate Match
```

Ein gleicher Hash reicht jedoch nicht als Beweis für identischen Inhalt.

```text
Hash Match
    ↓
Full Content Comparison
    ↓
Identical?
```

Nur nach vollständiger Verifikation darf eine Zusammenführung erfolgen.

## Merge

Der Merge erfolgt konzeptionell:

```text
Frame A
Frame B
   ↓
Verify Identical
   ↓
Select Canonical Frame
   ↓
Protect Mappings
   ↓
Redirect Mapping
   ↓
Update References
   ↓
TLB Invalidation
   ↓
Release Duplicate Frame
```

Der Vorgang muss gegenüber konkurrierenden Schreibzugriffen sicher sein.

## Copy-on-Write

Nach dem Merge werden betroffene private Mappings als COW behandelt.

```text
Shared Deduplicated Frame
          ↓
       Write
          ↓
      COW Fault
          ↓
    Private Copy
```

Damit bleibt die ursprüngliche logische Speichersemantik erhalten.

## Permissions

Deduplication darf Berechtigungen nicht vereinigen.

Beispiel:

```text
Mapping A → Read
Mapping B → Read/Write logically
```

Nach dem Merge kann Mapping B technisch zunächst COW-read-only sein, behält aber seine logische Schreibberechtigung.

Ein Teilnehmer darf niemals Rechte eines anderen Teilnehmers übernehmen.

## Sicherheit

Memory Deduplication kann Seitenkanäle erzeugen, wenn ein Prozess erkennen kann, ob seine Daten mit Speicher eines anderen Sicherheitskontexts zusammengeführt wurden.

Daher gilt:

```text
Memory Saving
     <
Security Boundary
```

Cross-User-, Cross-Sandbox- oder Cross-Trust-Domain-Deduplication muss explizit durch Security Policy erlaubt sein.

Sicherheitskritische Domains dürfen Deduplication vollständig deaktivieren.

## Sensitive Memory

Bestimmte Speicherklassen sollen grundsätzlich von Deduplication ausgeschlossen werden können.

Beispiele:

```text
Cryptographic Keys
Credentials
Secrets
Kernel Security State
Device Buffers
Confidential Workloads
```

Die Klassifikation darf über Memory- oder Security-Metadaten erfolgen.

## Concurrent Writes

Zwischen Vergleich und Merge kann sich eine Page verändern.

```text
Compare
  ↓
Concurrent Write
  ↓
Content changed
```

NovaOS muss verhindern, dass dadurch unterschiedliche Inhalte fälschlich zusammengeführt werden.

Merge und Mapping-Änderung benötigen daher geeignete Synchronisation.

## NUMA

Deduplication über NUMA-Nodes hinweg kann Speicher sparen, aber Remote-Zugriffe erzeugen.

```text
Duplicate Frames
Node 0 + Node 1
      ↓
Merge?
```

Die Entscheidung soll berücksichtigen:

```text
Memory Saved
Remote Access Cost
Expected Access Pattern
Memory Pressure
Migration Cost
```

NUMA-lokale Deduplication soll bevorzugt werden, sofern kein ausreichender Grund für nodeübergreifende Zusammenführung besteht.

## Huge Pages

Huge Pages dürfen dedupliziert werden, sofern die Implementierung dies unterstützt.

Alternativ darf eine Huge Page:

```text
Split
 ↓
Deduplicate individual pages
```

werden.

Das Aufteilen darf nur erfolgen, wenn der erwartete Speichergewinn die zusätzlichen Translation- und Verwaltungsaufwände rechtfertigt.

## Memory Pressure

Deduplication ist eine mögliche Reclamation-Strategie.

```text
Memory Pressure
      ↓
Find Duplicate Pages
      ↓
Merge
      ↓
Release Frames
```

Sie darf jedoch nicht vorausgesetzt werden, um kritische Speicheranforderungen erfüllen zu können.

## Hintergrundbetrieb

Deduplication soll bevorzugt als niedrig priorisierte Hintergrundarbeit erfolgen.

```text
Idle / Available Resources
          ↓
Dedup Scan
```

Unter hoher CPU-, Memory- oder Energiebelastung darf die Scan-Aktivität reduziert oder pausiert werden.

## Adaptive Steuerung

NovaOS darf den Nutzen der Deduplication messen.

```text
Scan Cost
    ↓
Pages Found
    ↓
Memory Saved
```

Wird dauerhaft wenig Speicher gewonnen, soll die Scan-Intensität reduziert werden.

```text
Low Dedup Yield
      ↓
Reduce Scanning
```

Adaptive Entscheidungen bleiben Soft Optimizations.

## Realtime

Deduplication darf Hard-Realtime-Pfade nicht beeinträchtigen.

Realtime-geschützter Speicher muss von:

```text
Background Scanning
Unexpected COW Faults
Merge Operations
```

ausgeschlossen werden können.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Scanned Pages
Duplicate Candidates
Merged Pages
Released Frames
Memory Saved
COW Faults after Dedup
Dedup Domains
Scan CPU Cost
Dedup Yield
Rejected Security Domains
```

## Normative Anforderungen

1. NovaOS MUSS auch ohne Memory Deduplication vollständig funktionsfähig sein.
2. Deduplication MUSS als optionale Speicheroptimierung behandelt werden.
3. Ein Hash-Treffer DARF NICHT allein als Beweis identischer Pages verwendet werden.
4. Vor einem Merge MUSS der tatsächliche Seiteninhalt verifiziert werden.
5. Deduplication DARF logische Ownership und Permissions NICHT verändern.
6. Schreibbare private Mappings MÜSSEN nach einem Merge über COW-Semantik getrennt bleiben.
7. Cross-Security-Domain-Deduplication MUSS explizit durch Policy erlaubt werden.
8. Sicherheitskritische Speicherbereiche MÜSSEN von Deduplication ausgeschlossen werden können.
9. Merge-Operationen MÜSSEN gegenüber konkurrierenden Schreibzugriffen sicher sein.
10. NUMA-Kosten SOLLEN bei nodeübergreifender Deduplication berücksichtigt werden.
11. Deduplication DARF garantierte Realtime-Ausführung NICHT beeinträchtigen.
12. Speichergewinn, Scan-Kosten, Merge-Aktivität und Sicherheitsentscheidungen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-MEMORY-PRESSURE-0001`
- `NPSPEC-MEMORY-RECLAIM-0001`
- `NPSPEC-MEMORY-VAS-0001`
- `NPSPEC-MEMORY-PAGING-0001`
- `NPSPEC-MEMORY-TLB-0001`
- `NPSPEC-MEMORY-COW-0001`
- `NPSPEC-MEMORY-MAPPING-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-MEMORY-0020`
- `ADR-VERIFY-0003_Memory_Safety`

## Ergebnis

```text
Duplicate Physical Pages
          ↓
Verify Identical
          ↓
Security / Policy Check
          ↓
Merge to Shared Frame
          ↓
Release Duplicate Frames
          ↓
Write
          ↓
Copy-on-Write
```

NovaOS erhält damit eine optionale Memory-Deduplication-Schicht, die redundante physische Speicherinhalte sicher zusammenführen kann, ohne logische Isolation, Berechtigungen, NUMA-Anforderungen oder Realtime-Garantien aufzugeben.