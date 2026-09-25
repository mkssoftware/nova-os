# NPSPEC-MEMORY-ASLR-0001 – Nova Address Space Layout Randomization

## Status

Angenommen

## Kategorie

Memory / ASLR / Virtual Memory / Security / Address Space

## Zweck

NovaOS definiert Address Space Layout Randomization (ASLR) als Sicherheitsmechanismus zur zufälligen Platzierung geeigneter virtueller Speicherbereiche.

```text
Process Start
     ↓
Generate Layout Entropy
     ↓
Place Memory Regions
     ↓
Randomized Address Space
```

ASLR soll verhindern, dass ein Angreifer zuverlässig vorhersagen kann, an welchen virtuellen Adressen sich Code, Daten, Stacks oder andere relevante Speicherbereiche befinden.

## Grundprinzip

Die virtuelle Adresse eines Objekts soll keine dauerhaft vorhersagbare Eigenschaft sein.

```text
Object Identity ≠ Virtual Address
```

Zwischen unterschiedlichen Instanzen desselben Programms können Speicherbereiche unterschiedliche Adressen besitzen.

```text
Run A:
Executable → 0x...

Run B:
Executable → 0x...

Run C:
Executable → 0x...
```

Dabei gilt:

```text
ASLR ≠ Memory Isolation
ASLR ≠ Memory Safety
ASLR ≠ Access Control
```

ASLR ergänzt andere Schutzmechanismen, ersetzt sie jedoch nicht.

## Randomisierbare Bereiche

NovaOS soll geeignete Speicherbereiche randomisieren können.

Dazu gehören insbesondere:

```text
Executable Images
Shared Libraries
Stacks
Heap Regions
Memory-Mapped Objects
Shared Memory Mappings
Thread Stacks
JIT Regions
```

Kernelbereiche können durch Kernel-ASLR separat randomisiert werden.

## Entropie

ASLR benötigt nicht vorhersehbare Zufallswerte.

```text
Secure Entropy
      ↓
ASLR Seed
      ↓
Layout Decisions
```

Für sicherheitsrelevante Randomisierung muss eine geeignete kryptographisch sichere Entropiequelle verwendet werden.

Vorhersagbare Werte wie:

```text
Timestamp only
PID only
Boot Counter only
```

reichen nicht als alleinige ASLR-Quelle aus.

## Layout-Erzeugung

Die Layout-Erzeugung erfolgt konzeptionell:

```text
VAS Constraints
      ↓
Available Regions
      ↓
Alignment Requirements
      ↓
Security Constraints
      ↓
Random Placement
      ↓
Validate Layout
```

Randomisierung darf keine ungültigen oder überlappenden Mappings erzeugen.

## Alignment

Randomisierte Adressen müssen weiterhin alle erforderlichen Alignment-Regeln erfüllen.

```text
Random Address
      ↓
Alignment
      ↓
Valid Mapping Address
```

Dies betrifft insbesondere:

```text
Page Alignment
Huge Page Alignment
Device Requirements
Architecture Constraints
```

## Entropieraum

Die verfügbare ASLR-Entropie hängt von:

```text
Address Width
VAS Size
Region Size
Alignment
Reserved Areas
Architecture
```

ab.

NovaOS soll unnötig feste virtuelle Bereiche vermeiden, wenn dadurch ohne funktionalen Grund ASLR-Entropie verloren geht.

## Per-Process Randomization

Jeder neu erzeugte Prozess soll grundsätzlich einen eigenen randomisierten Layout-Zustand erhalten.

```text
Process A → Layout Seed A
Process B → Layout Seed B
```

Ein kompromittierter Layout-Wert eines Prozesses darf nicht automatisch das Layout anderer Prozesse offenlegen.

## Stack Randomization

Stacks sollen randomisierte Startpositionen besitzen können.

```text
VAS
 ↓
Random Gap
 ↓
Guard Page
 ↓
Stack
```

Guard Pages und ASLR sollen gemeinsam eingesetzt werden können.

## Heap Randomization

Heap-Regionen sollen nicht dauerhaft an einer festen virtuellen Basisadresse beginnen müssen.

```text
Process
   ↓
Randomized Heap Region
```

Interne Heap-Allokator-Randomisierung darf zusätzlich eingesetzt werden, ist jedoch nicht Bestandteil der grundlegenden VAS-ASLR-Semantik.

## Mapping Randomization

Mappings ohne explizit angeforderte feste Adresse sollen innerhalb zulässiger VAS-Bereiche randomisiert platziert werden können.

```text
Map Object
    ↓
No Fixed Address Required
    ↓
Randomized Placement
```

Eine explizit erzwungene Adresse darf nur verwendet werden, wenn API, Capability und Sicherheitsregeln dies zulassen.

## Shared Memory

Dasselbe Shared Memory Object darf in unterschiedlichen Address Spaces an unterschiedlichen virtuellen Adressen erscheinen.

```text
Shared Object
├── VAS A → 0x...
└── VAS B → 0x...
```

Die gemeinsame Objektidentität darf nicht von identischen virtuellen Adressen abhängen.

## Kernel ASLR

NovaOS darf Kernelcode und geeignete Kernelbereiche randomisieren.

```text
Boot
 ↓
Entropy
 ↓
Kernel Layout
 ↓
Randomized Kernel VAS
```

Kernel-ASLR muss mit Boot, Paging, Interrupts, Debugging und Crash Reporting kompatibel sein.

## Reproduzierbarkeit

Deterministische oder reproduzierbare Ausführungsmodi können mit ASLR in Konflikt stehen.

NovaOS muss diesen Konflikt explizit behandeln.

```text
Security Mode
    ↓
Fresh Randomization

Deterministic Mode
    ↓
Controlled Reproducible Layout
```

Eine Abschwächung von ASLR für reproduzierbare Ausführung muss durch entsprechende Policy erlaubt sein.

## Compatibility

Legacy-Anwendungen können feste Adressen voraussetzen.

Solche Anforderungen dürfen innerhalb einer Compatibility Personality unterstützt werden.

```text
Native Nova Application
        ↓
ASLR by default

Legacy Compatibility
        ↓
Controlled Exception
```

Legacy-Anforderungen dürfen die native NovaOS-Sicherheitsarchitektur nicht global abschwächen.

## Information Leakage

ASLR verliert an Wirksamkeit, wenn virtuelle Adressen unkontrolliert offengelegt werden.

Daher müssen:

```text
Logs
Diagnostics
Crash Reports
Introspection
APIs
```

Adressinformationen entsprechend dem Security Context behandeln.

ASLR darf jedoch nicht als Ersatz für die Verhinderung von Information Leaks betrachtet werden.

## Fork / Clone / Checkpoint

Bei duplizierten oder wiederhergestellten Address Spaces muss definiert sein, ob das bestehende Layout erhalten oder neu randomisiert wird.

```text
Checkpoint Restore
       ↓
Preserve Layout?
or
Re-randomize?
```

Die Entscheidung muss mit Pointer-Semantik, COW, Kompatibilität und Wiederherstellungsmodell vereinbar sein.

## Performance

ASLR darf Paging- und Mapping-Performance nicht unnötig verschlechtern.

Randomisierung soll insbesondere berücksichtigen:

```text
VAS Fragmentation
Huge Page Opportunities
Mapping Search Cost
Guard Regions
```

Sicherheit hat gegenüber reinem Layout-Komfort Vorrang.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
ASLR Enabled
Randomized Region Classes
Entropy Status
Kernel ASLR State
Compatibility Exceptions
Deterministic Exceptions
```

Konkrete randomisierte Adressen dürfen nur für entsprechend autorisierte Diagnosekontexte sichtbar sein.

## Normative Anforderungen

1. NovaOS MUSS ASLR für geeignete User-Space-Address-Spaces unterstützen.
2. Native Prozesse SOLLEN standardmäßig mit randomisiertem Speicherlayout gestartet werden.
3. ASLR MUSS eine für Sicherheitszwecke geeignete Entropiequelle verwenden.
4. Prozessinstanzen SOLLEN voneinander unabhängige Layout-Randomisierung erhalten.
5. Executables, Libraries, Stacks, Heap und dynamische Mappings SOLLEN randomisierbar sein.
6. Randomisierte Adressen MÜSSEN alle Architektur- und Alignment-Anforderungen erfüllen.
7. Shared Memory DARF NICHT von identischen virtuellen Adressen in allen Teilnehmern abhängen.
8. NovaOS SOLL Kernel-ASLR unterstützen.
9. Feste Adressanforderungen MÜSSEN explizit angefordert und kontrolliert behandelt werden.
10. Compatibility-Ausnahmen DÜRFEN ASLR NICHT systemweit abschalten.
11. Sicherheitsrelevante Adressinformationen MÜSSEN bei Introspection und Logging geschützt werden.
12. ASLR-Zustand und kontrollierte Ausnahmen SOLLEN diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-VAS-0001`
- `NPSPEC-MEMORY-PAGING-0001`
- `NPSPEC-MEMORY-PAGETABLE-0001`
- `NPSPEC-MEMORY-MAPPING-0001`
- `NPSPEC-MEMORY-PROTECTION-0001`
- `NPSPEC-PROCESS-SPAWN-0001`
- `NPSPEC-PROCESS-CHECKPOINT-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-MEMORY-0022`
- `ADR-VERIFY-0003_Memory_Safety`

## Ergebnis

```text
Executable / Stack / Heap / Mappings
                ↓
        Security Constraints
                ↓
          ASLR Placement
                ↓
       Randomized Address Space
                ↓
      Hardware Memory Protection
```

NovaOS erhält damit eine systematisch in die VAS- und Mapping-Architektur integrierte ASLR-Schicht, die virtuelle Speicherpositionen schwerer vorhersagbar macht, ohne Objektidentität, Kompatibilität, Determinismus oder Memory Protection miteinander zu vermischen.