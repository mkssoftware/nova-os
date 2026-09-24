# NPSPEC-MEMORY-COW-0001 – Nova Copy-on-Write

## Status

Entwurf

## Kategorie

Memory / Copy-on-Write / Virtual Memory / Shared Pages

## Zweck

NovaOS definiert Copy-on-Write (COW) als Mechanismus, mit dem mehrere Virtual Address Spaces zunächst denselben physischen Speicher verwenden können, solange keine schreibende Änderung erfolgt.

```text
VAS A ─┐
       ├→ Shared Physical Frame
VAS B ─┘
       Read-Only
```

Erst beim ersten Schreibzugriff wird für den schreibenden Kontext eine private Kopie erzeugt.

```text
Write
 ↓
COW Fault
 ↓
Allocate Frame
 ↓
Copy
 ↓
Remap Writer
```

Dadurch werden unnötige Speicherduplikationen vermieden.

## Grundprinzip

Mehrere Mappings dürfen denselben physischen Frame referenzieren.

```text
Virtual Page A ─┐
                ├→ Physical Frame X
Virtual Page B ─┘
```

Die COW-Mappings werden zunächst nicht schreibbar eingerichtet.

Dabei gilt:

```text
Shared Read ≠ Shared Write
COW ≠ Immediate Copy
```

Die Kopie entsteht erst, wenn ein Teilnehmer tatsächlich schreiben möchte.

## COW-Zustand

Ein COW-Mapping benötigt mindestens:

```text
Backing Frame
COW Flag
Permissions
Reference State
```

Der physische Frame muss wissen beziehungsweise über seine Ownership-Struktur erkennen lassen, wie viele gültige Referenzen auf ihn bestehen.

## Erstellung

COW kann beim Duplizieren oder Ableiten eines Speicherzustands eingerichtet werden.

```text
Original Mapping
      ↓
Create COW Mapping
      ↓
Both Read-Only
      ↓
Shared Frame
```

Bestehende schreibbare Mappings müssen dabei kontrolliert auf COW-Semantik umgestellt werden.

Erforderliche TLB-Invalidierungen müssen durchgeführt werden.

## Lesezugriff

Lesezugriffe benötigen keine Kopie.

```text
Read
 ↓
Shared Frame
 ↓
Continue
```

Solange kein Schreibzugriff erfolgt, können beliebig viele zulässige Address Spaces denselben Frame verwenden.

## Schreibzugriff

Ein Schreibzugriff auf ein COW-Mapping erzeugt einen Protection Fault.

```text
Write Access
     ↓
Read-Only COW Mapping
     ↓
Page Fault
```

Der Fault Handler erkennt:

```text
Valid Mapping
+
Write Access
+
COW Flag
```

und startet die COW-Auflösung.

## COW-Auflösung

Der normale Ablauf lautet:

```text
COW Fault
   ↓
Validate Mapping
   ↓
Allocate New Frame
   ↓
Copy Old Content
   ↓
Install Private Mapping
   ↓
Enable Write
   ↓
Update References
   ↓
Invalidate TLB
   ↓
Resume Task
```

Der ursprüngliche Frame bleibt für andere Teilnehmer erhalten.

## Single-Owner Optimization

Besitzt der COW-Frame zum Zeitpunkt des Schreibzugriffs nur noch einen relevanten Besitzer, kann eine Kopie unnötig sein.

```text
Reference Count = 1
        ↓
Remove COW
        ↓
Enable Write
```

Diese Optimierung darf nur erfolgen, wenn keine weiteren gültigen Referenzen existieren.

## Referenzverwaltung

COW benötigt zuverlässige Referenzverwaltung.

```text
Shared Frame
├── Mapping A
├── Mapping B
└── Mapping C
```

Ein Frame darf erst freigegeben werden, wenn keine gültige Referenz mehr besteht.

```text
Reference Count = 0
        ↓
Frame Reclaimable
```

## Concurrent COW Faults

Mehrere Tasks können gleichzeitig auf dieselbe COW-Seite schreiben.

```text
Task A ─┐
        ├→ COW Frame
Task B ─┘
```

Die Auflösung muss verhindern:

```text
Lost Mapping Updates
Incorrect Reference Counts
Double Free
Unauthorized Shared Writes
```

Jeder unabhängige schreibende Address Space muss nach erfolgreicher Auflösung den korrekten privaten Zustand besitzen.

## Memory Ordering

Änderungen an:

```text
Page Tables
COW Metadata
Frame References
Permissions
```

müssen mit geeigneter Synchronisation und Memory Ordering erfolgen.

Ein COW-Fault darf keinen teilweise aktualisierten Mapping-Zustand sichtbar machen.

## TLB

Beim Einrichten oder Auflösen von COW können sich Page Permissions ändern.

```text
Writable
   ↓
COW Read-Only
```

oder:

```text
COW Read-Only
      ↓
Private Writable
```

Betroffene TLB-Einträge müssen entsprechend `NPSPEC-MEMORY-TLB-0001` invalidiert werden.

## NUMA

Ein neuer COW-Frame darf entsprechend der NUMA-Policy des schreibenden Tasks oder Address Spaces platziert werden.

```text
COW Fault
   ↓
Writer Locality
   ↓
NUMA Allocation
```

Dadurch kann eine COW-Auflösung gleichzeitig die Speicherlokalität verbessern.

## Memory Pressure

Eine COW-Auflösung benötigt möglicherweise zusätzlichen physischen Speicher.

```text
COW Fault
   ↓
Allocate Frame
   ↓
Memory Pressure / OOM?
```

Kann kein Frame bereitgestellt werden, muss der Fault kontrolliert fehlschlagen.

COW darf nicht voraussetzen, dass Speicher für eine spätere Kopie garantiert verfügbar ist.

## Große Pages

COW auf Huge Pages darf unterschiedliche Strategien verwenden.

```text
Copy Entire Huge Page
```

oder:

```text
Split Huge Page
      ↓
COW Smaller Pages
```

Die Auswahl darf von Kosten, Hardwareunterstützung, Memory Pressure und Zugriffsmuster abhängen.

## Backing Objects

COW darf auch für datei- oder objektbasierte private Mappings verwendet werden.

```text
Backing Object
      ↓
Shared Original Page
      ↓
Private Write
      ↓
COW Frame
```

Die private Änderung darf das ursprüngliche Backing Object nicht verändern, sofern dessen Mapping-Semantik dies nicht ausdrücklich vorsieht.

## Sicherheit

COW darf Isolation nicht abschwächen.

Nach der COW-Auflösung gilt:

```text
Writer → Private Frame
Others → Original Frame
```

Schreibzugriffe eines Prozesses dürfen nicht unbeabsichtigt den Speicherzustand eines anderen isolierten Prozesses verändern.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
COW Mappings
Shared Frame References
COW Fault Count
COW Copies
Single-Owner Optimizations
COW Allocation Failures
Huge-Page Splits
Copied Bytes
```

## Normative Anforderungen

1. NovaOS MUSS Copy-on-Write für geeignete virtuelle Mappings unterstützen können.
2. COW-Mappings MÜSSEN bis zur erfolgreichen Auflösung gegen direkten Schreibzugriff geschützt sein.
3. Lesezugriffe DÜRFEN keine unnötige physische Kopie erzeugen.
4. Ein COW-Schreibzugriff MUSS als eindeutig klassifizierbarer Page Fault behandelbar sein.
5. Vor dem privaten Schreibzugriff MUSS ein unabhängiger Frame bereitgestellt oder Single-Owner-Semantik nachgewiesen werden.
6. Frame-Referenzen MÜSSEN während Erstellung, Auflösung und Unmapping korrekt verwaltet werden.
7. Ein gemeinsam verwendeter Frame DARF NICHT freigegeben werden, solange gültige Referenzen bestehen.
8. Gleichzeitige COW-Faults MÜSSEN SMP-sicher behandelt werden.
9. Permission-Änderungen MÜSSEN mit korrekter TLB-Invalidierung verbunden sein.
10. NUMA-Policies SOLLEN bei der Allokation neuer COW-Frames berücksichtigt werden.
11. COW-Allokationsfehler und OOM MÜSSEN kontrolliert behandelbar sein.
12. COW-Mappings, Faults, Kopien und Referenzzustände SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-FRAMEALLOCATOR-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-MEMORY-OOM-0001`
- `NPSPEC-MEMORY-VAS-0001`
- `NPSPEC-MEMORY-PAGING-0001`
- `NPSPEC-MEMORY-PAGETABLE-0001`
- `NPSPEC-MEMORY-TLB-0001`
- `NPSPEC-MEMORY-DEMANDPAGING-0001`
- `NPSPEC-SYNC-ATOMIC-0001`
- `ADR-MEMORY-0015`
- `ADR-VERIFY-0003_Memory_Safety`

## Ergebnis

```text
Shared Physical Frame
       ↑       ↑
     VAS A   VAS B
       ↓
Write by VAS A
       ↓
COW Fault
       ↓
Allocate + Copy
       ↓
VAS A → Private Frame
VAS B → Original Frame
```

NovaOS erhält damit einen sicheren und speichereffizienten Copy-on-Write-Mechanismus, der gemeinsame Speicherzustände erst bei tatsächlichen Änderungen trennt und dabei Paging, TLB, NUMA, Memory Pressure und Prozessisolation berücksichtigt.