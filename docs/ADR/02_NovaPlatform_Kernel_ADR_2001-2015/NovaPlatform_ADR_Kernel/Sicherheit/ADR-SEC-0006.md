# ADR-SEC-0006 – Kernel Isolation

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-SEC-0006 |
| Titel | Kernel Isolation |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Sicherheit / Kernel / Virtuelle Speicherverwaltung / Prozessisolation |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-SEC-0001, ADR-SEC-0002, ADR-SEC-0003, ADR-SEC-0004, ADR-SEC-0005, ADR-MM-0002, ADR-PROC-0008 |
| Nachfolgende Spezifikation | NPSPEC-KERNELISOLATION-0001 |

---

## 1. Kontext

Der NovaOS-Kernel besitzt Zugriff auf:

- sämtliche physischen Speicherbereiche,
- Prozessadressräume,
- Kernelobjekte,
- Seitentabellen,
- Geräteregister,
- Sicherheits- und Capability-Zustände,
- kryptografische Schlüssel,
- Dateisystemmetadaten,
- privilegierte CPU-Zustände.

Viele Betriebssysteme bilden den vollständigen Kerneladressraum auch während der Userspace-Ausführung in den Seitentabellen eines Prozesses ab.

Die Kernelseiten sind dabei zwar als Supervisor-only markiert, bleiben aber virtuell vorhanden.

Dieses Modell vereinfacht:

- Systemaufrufe,
- Interrupts,
- Context Switching,
- TLB-Verwaltung,
- Kernelzugriffe auf Prozessdaten.

Es besitzt jedoch Sicherheitsnachteile.

Bestimmte spekulative CPU-Angriffe können versuchen, Daten aus privilegierten, aber im aktuellen Adressraum vorhandenen Mappings indirekt sichtbar zu machen.

Zusätzlich können:

- Seitenkanäle,
- Kerneladresslecks,
- Seitentabellenfehler,
- falsch gesetzte User/Supervisor-Bits,
- kompromittierte Userspace-Page-Table-Verwaltung

von einem vollständig gemappten Kerneladressraum profitieren.

NovaOS benötigt deshalb eine stärkere Trennung zwischen dem während der Userspace-Ausführung sichtbaren Adressraum und dem vollständigen Kerneladressraum.

---

## 2. Problemstellung

Eine vollständige Kernelisolation muss mehrere widersprüchliche Anforderungen erfüllen:

- Userspace darf den Kerneladressraum nicht sehen.
- Interrupts und Systemaufrufe müssen weiterhin sicher in den Kernel wechseln.
- Der Wechsel darf keinen ungültigen Stack oder Adressraum verwenden.
- Kernelstacks dürfen nicht im normalen Userspace-Adressraum sichtbar sein.
- Mehrkernsysteme benötigen CPU-lokale Eintrittsdaten.
- KASLR darf nicht über feste Aliasabbildungen umgangen werden.
- SMEP, SMAP und NX müssen weiterhin aktiv bleiben.
- TLB-Flushes dürfen die Systemleistung nicht unnötig zerstören.
- Echtzeitpfade benötigen messbare Eintritts- und Austrittslatenzen.
- 32-Bit-Systeme besitzen nur begrenzten virtuellen Adressraum.
- Hardware ohne PCID oder ASID verursacht höhere Wechselkosten.
- Debugging, Crash-Dumps und Hibernate müssen die Trennung berücksichtigen.

NovaOS darf keine vollständige Isolation behaupten, wenn der gesamte Kernel weiterhin dauerhaft im Userspace-Seitentabellensatz abgebildet ist.

---

## 3. Entscheidungsziele

Kernel Isolation soll:

1. den vollständigen Kerneladressraum aus normalen Userspace-Seitentabellen entfernen,
2. nur einen minimalen Eintrittsbereich gemeinsam abbilden,
3. Systemaufrufe, Interrupts und Exceptions sicher behandeln,
4. Kernelstacks vor Userspace-Mappings verbergen,
5. spekulative Seitenkanäle gegen gemappte Kerneldaten reduzieren,
6. KASLR gegen einfache Seitentabellenbeobachtung stärken,
7. PCID-, ASID- und TLB-Optimierungen nutzen,
8. Mehrkern-, NUMA- und CPU-Hotplug-Systeme unterstützen,
9. kontrollierte Uaccess-Pfade erhalten,
10. auf 32-Bit-Systemen eine realistische Teilumsetzung ermöglichen,
11. Echtzeitkosten messbar und policyfähig machen,
12. den tatsächlichen Isolationsstatus diagnostizierbar machen.

---

## 4. Begriffsdefinitionen

### 4.1 Kernel Isolation

Kernel Isolation bezeichnet die Trennung des vollständigen Kerneladressraums von den während der Userspace-Ausführung aktiven Seitentabellen.

---

### 4.2 Kernel Page-Table Isolation

Kernel Page-Table Isolation, kurz KPTI, verwendet getrennte Seitentabellensichten für Userspace- und Kernel-Ausführung.

---

### 4.3 User Page Table

Die User Page Table enthält:

- den erlaubten Userspace-Adressraum,
- einen minimalen Kernel-Entry-Bereich,
- keine allgemeine vollständige Kernelabbildung.

---

### 4.4 Kernel Page Table

Die Kernel Page Table enthält:

- den vollständigen Kerneladressraum,
- benötigte Kernelobjekte und Gerätebereiche,
- den kontrolliert zugänglichen Userspace-Adressraum oder Uaccess-Mappings,
- CPU-lokale Kerneldaten.

---

### 4.5 Entry Trampoline

Der Entry Trampoline ist ein minimaler Codebereich, der beim Eintritt aus dem Userspace den Wechsel in den vollständigen Kerneladressraum durchführt.

---

### 4.6 Entry Stack

Der Entry Stack ist ein kleiner CPU-lokaler Stack, der für die ersten Instruktionen eines Systemaufrufs, Interrupts oder einer Exception verwendet werden kann, bevor der vollständige Thread-Kernelstack verfügbar ist.

---

### 4.7 Shared Entry Area

Die Shared Entry Area ist der minimale, kontrollierte Bereich, der sowohl in User- als auch in Kernel-Seitentabellen abgebildet sein muss.

---

### 4.8 Address Space Identifier

PCID, ASID oder vergleichbare Kennungen ermöglichen es der CPU, TLB-Einträge verschiedener Adressräume zu unterscheiden, ohne bei jedem Wechsel alle Übersetzungseinträge zu verwerfen.

---

## 5. Bedrohungsmodell

Kernel Isolation erschwert insbesondere:

- spekulatives Lesen gemappter Kerneldaten aus dem Userspace,
- Ermittlung des Kernelmappings über Seitentabellenverhalten,
- Ausnutzung falsch gesetzter Zugriffsbits im vollständigen Kernelmapping,
- bestimmte KASLR-Offenlegungen,
- Missbrauch unnötig gemeinsam gemappter Kernelbereiche,
- direkte Abhängigkeit von einem gemeinsamen Adressraum.

Kernel Isolation schützt nicht vollständig gegen:

- kompromittierten Kernelcode,
- beliebige Schreibzugriffe auf Seitentabellen,
- Hardwarefehler,
- physische Angriffe,
- DMA-Angriffe ohne IOMMU,
- Seitenkanäle ohne Mappingabhängigkeit,
- Fehler in der minimalen Shared Entry Area,
- falsch implementierte Uaccess-Pfade,
- gestohlene privilegierte Schlüssel nach Kernelkompromittierung.

Kernel Isolation ist eine Defense-in-Depth-Maßnahme.

---

## 6. Betrachtete Alternativen

### 6.1 Vollständig gemeinsamer Kerneladressraum

Der vollständige Kernel bleibt in jedem Prozessadressraum gemappt.

#### Vorteile

- einfache Systemaufrufpfade,
- schnelle Kernelübergänge,
- geringe TLB-Kosten,
- einfache Context-Switch-Implementierung.

#### Nachteile

- vollständiges Kernelmapping ist während Userspace-Ausführung vorhanden,
- erhöhtes Risiko spekulativer Seitenkanäle,
- größere Angriffsoberfläche bei Seitentabellenfehlern,
- schwächere KASLR-Isolation.

Diese Alternative wird für vollständig geschützte Systeme verworfen.

---

### 6.2 Eigener Kerneladressraum ohne Userspace-Mappings

Beim Kerneleintritt wird auf einen Kerneladressraum gewechselt, der keinerlei Userspace-Seiten enthält.

#### Vorteile

- sehr starke Adressraumtrennung,
- Kernel kann Userspace-Speicher nicht versehentlich direkt adressieren,
- kleinere gemeinsame Mappingmenge.

#### Nachteile

- Uaccess benötigt temporäre Remote-Mappings,
- hohe Mapping- und TLB-Kosten,
- große Komplexität für Systemaufrufe und I/O,
- schwierige Copy-in/Copy-out-Implementierung.

Diese Alternative wird nicht als allgemeines Anfangsmodell gewählt.

Sie bleibt als spätere strengere Isolationsstufe möglich.

---

### 6.3 Getrennte User- und Kernel-Seitentabellen mit minimaler Shared Entry Area

Userspace und Kernel verwenden getrennte Seitentabellensichten. Ein kleiner gemeinsamer Eintrittsbereich ermöglicht den sicheren Wechsel.

#### Vorteile

- vollständiger Kernel ist im Userspace nicht gemappt,
- sichere Systemaufruf- und Interruptübergänge,
- vorhandene Uaccess-Modelle bleiben nutzbar,
- gute PCID-/ASID-Optimierung,
- bewährtes Grundmodell.

#### Nachteile

- zusätzlicher Seitentabellenwechsel,
- höhere Systemaufruf- und Interruptlatenz,
- komplexe Entry- und Exit-Pfade,
- Shared Entry Area wird sicherheitskritisch.

Diese Alternative wird gewählt.

---

### 6.4 Vollständige Hardwarevirtualisierung des Kernels

Der Kernel läuft in einer getrennten Virtualisierungsebene.

#### Vorteile

- zusätzliche Hardwareisolation,
- starke Trennung möglich,
- Grundlage für Safety- oder Hypervisorarchitektur.

#### Nachteile

- erhebliche Architekturkomplexität,
- höhere Laufzeitkosten,
- nicht auf allen Zielsystemen verfügbar,
- ersetzt keine korrekte Kerneladressraumverwaltung.

Diese Alternative wird für die erste Kernelarchitektur verworfen.

---

## 7. Entscheidung

NovaOS verwendet Kernel Page-Table Isolation mit getrennten User- und Kernel-Seitentabellensichten.

Während normaler Userspace-Ausführung ist aktiv:

```text
User Page Table
├── Userspace-Code
├── Userspace-Daten
├── Userspace-Stack
├── kontrollierte Shared-Memory-Bereiche
└── minimale Shared Entry Area
```

Während Kernel-Ausführung ist aktiv:

```text
Kernel Page Table
├── Kerneltext
├── Kernel-Rodata
├── Kerneldata
├── Kernelheap
├── Kernelstacks
├── Kernelmodule
├── Direct Map
├── MMIO
├── CPU-lokale Daten
└── kontrolliert zugänglicher Userspace
```

Der vollständige Kerneladressraum wird nicht in der normalen User Page Table abgebildet.

---

## 8. Grundprinzip

Der Eintritt aus dem Userspace erfolgt konzeptionell:

```text
Userspace
   ↓
CPU wechselt Privilegienstufe
   ↓
Shared Entry Trampoline
   ↓
minimalen CPU-lokalen Zustand sichern
   ↓
Kernel Page Table aktivieren
   ↓
vollständigen Thread-Kernelstack aktivieren
   ↓
normalen Kernel-Entry ausführen
```

Die Rückkehr lautet:

```text
Kernelarbeit abschließen
   ↓
Userspace-Rückkehrzustand validieren
   ↓
Entry Stack vorbereiten
   ↓
User Page Table aktivieren
   ↓
Kernelgeheimnisse aus Registern entfernen
   ↓
kontrolliert in Userspace zurückkehren
```

---

## 9. Adressraumansichten

Jeder Prozess besitzt logisch mindestens zwei Seitentabellenwurzeln:

```text
process.user_page_table
process.kernel_page_table
```

Die Kernel Page Table kann aus:

- einer globalen Kernelhälfte,
- einer prozessspezifischen Userspace-Hälfte,
- gemeinsamen Kernelmappingstrukturen

zusammengesetzt werden.

Die konkrete Seitentabellenstruktur ist architekturabhängig.

---

## 10. User Page Table

Die User Page Table enthält ausschließlich:

- den gültigen Prozess-Userspace,
- minimale Shared Entry Mappings,
- architekturspezifisch notwendige Metadaten,
- gegebenenfalls read-only Zeit- oder Systeminformationsseiten.

Nicht enthalten sind:

- vollständiger Kerneltext,
- Kernelheap,
- Kernelstacks,
- Kernelmodule,
- Direct Map,
- allgemeine MMIO-Bereiche,
- Seitentabellenverwaltungsbereiche,
- Kernelobjekte,
- kryptografische Kernelgeheimnisse.

---

## 11. Kernel Page Table

Die Kernel Page Table enthält den vollständigen Kerneladressraum.

Sie kann den Userspace des aktuell ausgeführten Prozesses weiterhin als User-markierte Seiten enthalten, damit kontrollierte Uaccess-Funktionen arbeiten können.

SMAP beziehungsweise ein Architekturäquivalent verhindert dabei normale direkte Zugriffe.

Eine spätere strengere Variante kann Userspace-Seiten aus der Kernel Page Table entfernen und nur temporär kontrolliert einblenden.

---

## 12. Shared Entry Area

Die Shared Entry Area wird in beiden Seitentabellensichten an derselben virtuellen Adresse abgebildet.

Sie enthält nur das zwingend Notwendige:

- Entry-Trampoline-Code,
- Exit-Trampoline-Code,
- minimale CPU-lokale Eintrittsdaten,
- Entry Stack,
- gegebenenfalls architekturspezifische Deskriptoren.

Sie enthält nicht:

- allgemeine Kernelobjekte,
- vollständige Threadstrukturen,
- kryptografische Schlüssel,
- Kernelheapdaten,
- unnötige Funktionsbibliotheken,
- vollständige Symboltabellen.

---

## 13. Rechte der Shared Entry Area

Die Shared Entry Area verwendet minimale Seitenrechte.

Typische Rechte:

| Teilbereich | User Read | User Write | User Execute | Kernel Write | Kernel Execute |
|---|---:|---:|---:|---:|---:|
| Entry-Trampoline-Code | Nein | Nein | Nein | Nein | Ja |
| Exit-Trampoline-Code | Nein | Nein | Nein | Nein | Ja |
| Entry Stack | Nein | Nein | Nein | Ja | Nein |
| CPU-Entry-Daten | Nein | Nein | Nein | kontrolliert | Nein |

Die Seiten bleiben Supervisor-only, auch wenn sie in der User Page Table vorhanden sind.

---

## 14. Entry Trampoline

Der Entry Trampoline muss möglichst klein und frei von komplexen Abhängigkeiten sein.

Er darf vor Aktivierung der Kernel Page Table nur auf Daten zugreifen, die in der Shared Entry Area vorhanden sind.

Er führt mindestens aus:

1. minimalen Registerzustand sichern,
2. Zugriffszustände normalisieren,
3. SMAP-Override schließen,
4. gegebenenfalls Spekulationsschutz anwenden,
5. Kernel-Seitentabellenwurzel laden,
6. TLB-/PCID-Regeln anwenden,
7. Thread-Kernelstack aktivieren,
8. zum normalen Kernel-Entry springen.

---

## 15. Exit Trampoline

Der Exit Trampoline führt den kontrollierten Wechsel zurück zur User Page Table aus.

Er muss:

1. Userspace-Kontext validiert erhalten,
2. sensitive Kernelregister bereinigen,
3. offenen Uaccess-Zustand ausschließen,
4. User Page Table aktivieren,
5. notwendige Barrieren ausführen,
6. den privilegierten Rückkehrbefehl verwenden.

Nach Aktivierung der User Page Table darf der Exit-Code nur noch auf die Shared Entry Area zugreifen.

---

## 16. Entry Stack

Jede aktive logische CPU besitzt mindestens einen eigenen Entry Stack.

Der Entry Stack:

- ist klein,
- besitzt Guard Pages, soweit architektonisch möglich,
- ist in User- und Kernel-Seitentabellen als Supervisor-only gemappt,
- ist nicht ausführbar,
- enthält nur kurzlebige Eintrittsdaten,
- wird nicht als normaler Thread-Kernelstack verwendet.

Nach Aktivierung der Kernel Page Table wechselt der Kernel auf den vollständigen Kernelstack des aktuellen Threads.

---

## 17. Keine vollständigen Kernelstacks in User Page Tables

Thread-Kernelstacks werden ausschließlich in der Kernel-Seitentabellensicht abgebildet.

Die User Page Table enthält weder:

- den vollständigen Stack,
- seine Guard Pages als sichtbare Kernelstruktur,
- eine schreibbare Aliasabbildung,
- Stack-Metadaten.

Dadurch bleiben Kernelstackpositionen während normaler Userspace-Ausführung verborgen.

---

## 18. CPU-lokale Entry-Daten

Die Shared Entry Area enthält nur minimale CPU-lokale Informationen.

Beispiele:

```text
kernel_page_table_root
entry_stack_pointer
cpu_id
entry_flags
temporary_register_storage
```

Nicht enthalten sein sollen:

- vollständiger Current-Thread-Control-Block,
- Schedulerlisten,
- Prozesslisten,
- Sicherheitsschlüssel,
- allgemeine Kernelpointer.

Wo möglich, werden Pointer vermieden oder gegen KASLR-Leaks maskiert.

---

## 19. Systemaufrufe

Ein Systemaufruf durchläuft folgende Phasen:

```text
Userspace-Systemaufruf
        ↓
Hardware-Entry
        ↓
Entry Trampoline
        ↓
Kernel Page Table aktivieren
        ↓
Thread-Kernelstack aktivieren
        ↓
Trap Frame vervollständigen
        ↓
Systemaufruf dispatchen
```

Der Dispatcher darf erst ausgeführt werden, wenn:

- vollständiger Kerneladressraum aktiv,
- gültiger Kernelstack vorhanden,
- SMEP und SMAP aktiv,
- Entry-Zustand konsistent

sind.

---

## 20. Interrupts aus Userspace

Ein Interrupt während Userspace-Ausführung beginnt ebenfalls in der Shared Entry Area.

Der Entry-Code bestimmt anhand des gespeicherten Privilegienzustands, ob ein Seitentabellenwechsel erforderlich ist.

```text
Interrupt aus Userspace:
    User → Kernel Page Table wechseln

Interrupt aus Kernel:
    Kernel Page Table bereits aktiv
```

Die beiden Pfade müssen eindeutig unterschieden werden.

---

## 21. Exceptions aus Userspace

Exceptions wie:

- Page Fault,
- General Protection Fault,
- Invalid Opcode,
- Debugexception

verwenden denselben sicheren Entry-Grundmechanismus.

Die vollständige Exceptionbehandlung beginnt erst nach Aktivierung der Kernel Page Table.

Der frühe Entry-Code darf keine komplexe Exception auslösen, die von nicht gemappten Kernelkomponenten abhängig ist.

---

## 22. Exceptions im Entry Trampoline

Ein Fehler innerhalb des Entry Trampolines ist besonders kritisch.

NovaOS benötigt dafür einen minimalen Notfallpfad.

Mögliche Reaktionen:

- Wechsel auf einen CPU-lokalen Notfallstack,
- Aktivierung einer bekannten Kernel-Seitentabelle,
- minimale Fehlerdiagnose,
- Kernel-Panic.

Der Entry-Trampoline-Pfad darf nicht rekursiv unbegrenzt fehlschlagen.

---

## 23. Interrupts im Kernel

Läuft die CPU bereits mit der Kernel Page Table, ist kein weiterer KPTI-Seitentabellenwechsel erforderlich.

Verschachtelte Interrupts bleiben im Kerneladressraum.

Der Interrupt-Exit prüft, ob die Rückkehr:

- in den Kernel oder
- in den Userspace

erfolgt.

Nur bei Userspace-Rückkehr wird auf die User Page Table gewechselt.

---

## 24. Context Switching

Ein Context Switch im Kernel wechselt den aktuellen Thread und gegebenenfalls den Prozess.

Der Kernel aktualisiert:

- neue Kernel-Seitentabellenreferenz,
- neue User-Seitentabellenreferenz,
- Entry-Area-Daten,
- PCID oder ASID,
- Kernelstack,
- Uaccess-Kontext,
- TLB-Generation.

Die User Page Table wird erst unmittelbar vor der Rückkehr zum Zielprozess aktiviert.

---

## 25. Wechsel zwischen Threads desselben Prozesses

Besitzen alter und neuer Thread denselben Prozessadressraum, können beide dieselben User- und Kernel-Seitentabellenreferenzen verwenden.

Der Kernelstack und Threadkontext wechseln dennoch.

Unnötige Seitentabellen- oder PCID-Wechsel werden vermieden.

---

## 26. Wechsel zwischen Prozessen

Bei einem Prozesswechsel werden beide Seitentabellensichten des Zielprozesses berücksichtigt.

Konzeptionell:

```text
previous.kernel_page_table
        ↓
next.kernel_page_table
        ↓
Thread im Kernel ausführen
        ↓
next.user_page_table
        ↓
in Userspace zurückkehren
```

TLB-Generationen und Adressraumkennungen werden vor jeder Aktivierung geprüft.

---

## 27. Kernelthreads

Kernelthreads verwenden ausschließlich eine Kernel-Seitentabellensicht.

Sie besitzen keine normale User Page Table.

Ein Kernelthread darf bei seinem Start nicht den Useradressraum des zuvor laufenden Prozesses übernehmen.

Benötigt er Zugriff auf Prozessspeicher, verwendet er:

- explizite Adressraumhandles,
- Uaccess-Dienste,
- Remote-Copy-APIs,
- Shared-Memory-Objekte.

---

## 28. Idle Threads

Idle Threads verwenden ausschließlich den Kerneladressraum.

Der CPU-lokale Idle Thread ist keinem Userspace-Adressraum zugeordnet.

Beim Wechsel vom Userspace zum Idle Thread wird der Kerneladressraum aktiviert und bleibt aktiv, bis ein neuer Userspace-Thread kontrolliert zurückkehrt.

---

## 29. Uaccess

Im gewählten Grundmodell kann die Kernel Page Table die Userseiten des aktuellen Prozesses enthalten.

Direkte Zugriffe bleiben durch SMAP verboten.

Ein Uaccess-Vorgang benötigt:

- gültigen Prozessadressraum,
- Kernel Page Table des Prozesses,
- kontrolliertes Uaccess-Fenster,
- Fault Fixup,
- keine Rückkehr in User Page Table während der Operation.

Uaccess darf nicht stattfinden, während die User Page Table aktiv ist und der Kernel nur im Entry Trampoline läuft.

---

## 30. Strenger Uaccess-Modus

NovaOS kann später einen strengeren Modus unterstützen, in dem die Kernel Page Table keine dauerhaften Userspace-Mappings enthält.

Uaccess erfolgt dann über:

- temporäre Fenster,
- dedizierte Copy-Mappings,
- physische Seitenreferenzen,
- kontrollierte Remote-Adressraumoperationen.

Dieser Modus bietet stärkere Trennung, verursacht aber höhere Kosten.

Er ist nicht Voraussetzung der ersten KPTI-Implementierung.

---

## 31. SMEP und SMAP

Kernel Isolation ergänzt SMEP und SMAP.

| Mechanismus | Aufgabe |
|---|---|
| KPTI | entfernt vollständige Kernelseiten aus der User Page Table |
| SMEP | verhindert Kernelausführung aus Userseiten |
| SMAP | verhindert Kernel-Datenzugriff auf Userseiten |
| NX | verhindert Ausführung aus Datenseiten |
| User/Supervisor-Bit | verhindert Userspace-Zugriff auf Kernelseiten |

Keiner dieser Mechanismen ersetzt die anderen vollständig.

---

## 32. KASLR

Kernel Isolation verstärkt KASLR, da die vollständigen Kernelseiten während Userspace-Ausführung nicht gemappt sind.

Die Shared Entry Area bleibt jedoch ein potenzieller Layoutindikator.

Deshalb gilt:

- Shared Entry Area minimal halten,
- keine unnötigen Kernelpointer speichern,
- feste Adressen nur verwenden, wenn architektonisch erforderlich,
- Entry-Basis soweit möglich randomisieren,
- keine vollständigen Symbole oder Adresslisten bereitstellen.

---

## 33. NX und W^X

Die Shared Entry Area folgt NX und W^X.

```text
Trampoline-Code:
    Read + Execute
    nicht schreibbar

Entry-Daten:
    Read oder Read + Write
    nicht ausführbar

Entry Stack:
    Read + Write
    nicht ausführbar
```

Es existiert keine RWX-Shared-Entry-Seite.

---

## 34. Stack Protector

Entry Trampoline und früheste Stackwechselroutinen können teilweise nicht regulär compilerinstrumentiert werden.

Sie müssen:

- klein,
- überwiegend in überprüftem Assembly,
- ohne komplexe lokale Puffer,
- durch Guard Pages und Notfallpfade geschützt

sein.

Sobald der vollständige Thread-Kernelstack aktiv ist, gilt der normale Stack Protector aus ADR-SEC-0003.

---

## 35. TLB-Verwaltung

Der Wechsel zwischen User- und Kernel-Seitentabellen kann TLB-Flushes verursachen.

NovaOS verwendet nach Möglichkeit:

- PCID auf x86,
- ASID auf ARM,
- vergleichbare Architekturkennungen,
- globale Kernelmappinggenerationen,
- gezielte Invalidierungen.

Das Ziel ist:

```text
User Page Table + eigene Kennung
Kernel Page Table + eigene Kennung
```

Dadurch können gültige TLB-Einträge beider Sichten teilweise erhalten bleiben.

---

## 36. PCID-Zuordnung

Eine mögliche x86-PCID-Zuordnung verwendet pro Prozess zwei Kennungen:

```text
user_pcid
kernel_pcid
```

Die Kennungen können aus einer gemeinsamen Adressraum-ID und einem Sichtbit abgeleitet werden.

Vor Wiederverwendung einer PCID muss die zugehörige Generation geprüft und gegebenenfalls invalidiert werden.

PCID-Wiederverwendung darf keine TLB-Einträge eines früheren Prozesses sichtbar machen.

---

## 37. ASID-Zuordnung

Auf Architekturen mit ASIDs gelten dieselben Grundprinzipien:

- getrennte Identität der User- und Kernelsicht,
- generationengesicherte Wiederverwendung,
- gezielte TLB-Invalidierung,
- CPU-lokale Seen-Generations.

Die generische Kernelisolation hängt nicht von PCID-spezifischen Annahmen ab.

---

## 38. Hardware ohne PCID oder ASID

Fehlen Adressraumkennungen, muss NovaOS beim Wechsel möglicherweise größere TLB-Bereiche leeren.

Kernel Isolation bleibt funktional, kann aber höhere Kosten verursachen.

Die Plattformpolicy kann abhängig von:

- Sicherheitsbedarf,
- Performanceanforderungen,
- Echtzeitanforderungen,
- CPU-Leistung

zwischen aktivierter Isolation und einem degradierten gemeinsamen Mappingmodell wählen.

Die Entscheidung wird im Trust State sichtbar gemacht.

---

## 39. Global Pages

Globale TLB-Seiten müssen mit KPTI sorgfältig behandelt werden.

Eine als global markierte Kernelmappingseite darf nicht versehentlich in der User Page Table wirksam bleiben.

Globale Mappings werden deshalb:

- auf notwendige Shared-Entry-Seiten begrenzt oder
- während KPTI nicht für vollständige Kernelbereiche verwendet.

Die genaue Architekturpolicy wird in der VM-Spezifikation festgelegt.

---

## 40. TLB Shootdowns

Ändern sich:

- Shared Entry Area,
- Kerneltextrechte,
- Prozess-Usermappings,
- Entry-Stack-Mappings,
- Trampoline-Code,

müssen alle betroffenen User- und Kernel-PCIDs beziehungsweise ASIDs invalidiert werden.

Eine Änderung gilt erst als abgeschlossen, wenn keine CPU mehr veraltete sicherheitsrelevante TLB-Einträge verwenden kann.

---

## 41. Mehrkernsysteme

Jede CPU besitzt eigene:

- Entry Stacks,
- Entry-Daten,
- aktuelle Seitentabellenreferenzen,
- PCID-/ASID-Zustände,
- TLB-Generationen,
- Notfallkontexte.

Gemeinsam genutzte Trampoline-Codeseiten sind schreibgeschützt.

CPU-lokale Entry-Daten dürfen nicht durch andere CPUs unkontrolliert verändert werden.

---

## 42. CPU-Hotplug

Vor dem Online-Schalten einer CPU werden angelegt:

- Entry Stack,
- Guard Pages,
- CPU-Entry-Daten,
- Notfallstack,
- Trampoline-Mappings,
- Kernel-Seitentabellenreferenz.

Die CPU führt einen Isolationstest aus, bevor sie als online gilt.

Beim Offlining werden:

- neue Eintritte verhindert,
- Threads migriert,
- TLB-Zustände bereinigt,
- Entry-Daten invalidiert,
- CPU-lokale Mappingreferenzen freigegeben.

---

## 43. NUMA-Systeme

CPU-lokale Entry Stacks und Entry-Daten sollen auf dem zugehörigen NUMA-Knoten liegen.

Trampoline-Code kann gemeinsam schreibgeschützt verwendet werden.

Kernel-Seitentabellenstrukturen können NUMA-lokal replizierte Verwaltungsseiten nutzen, müssen aber logisch konsistente Kernelmappings bereitstellen.

---

## 44. 32-Bit-x86

Der 32-Bit-Adressraum begrenzt die mögliche Trennung stark.

NovaOS verwendet zunächst ein Split-Address-Space-Modell und prüft, ob zwei Seitentabellensichten praktikabel umgesetzt werden können.

Mögliche Einschränkungen:

- höhere Seitentabellenkosten,
- geringere Userspace-Größe,
- begrenzter Kerneladressraum,
- höhere TLB-Flush-Kosten,
- weniger verfügbare PCID-Funktionen,
- komplexere PAE-Integration.

Die 32-Bit-Implementierung kann deshalb eine reduzierte Kernelisolation verwenden.

---

## 45. Reduzierte 32-Bit-Isolation

Die reduzierte 32-Bit-Stufe umfasst mindestens:

1. strikte User/Supervisor-Trennung,
2. NX über PAE, soweit verfügbar,
3. SMEP und SMAP, soweit verfügbar,
4. minimale Userspace-sichtbare Kernelbereiche,
5. keine Userspace-lesbaren Kernelmappings,
6. isolierte Kernelstacks,
7. getrennte Entry-Bereiche.

Kann vollständiges KPTI nicht effizient oder korrekt umgesetzt werden, wird dies als reduzierte Isolation gekennzeichnet.

---

## 46. 64-Bit-Kernel

Der spätere 64-Bit-Kernel verwendet vollständige getrennte User- und Kernel-Seitentabellensichten.

Der größere virtuelle Adressraum erlaubt:

- klar getrennte Kernelregionen,
- kleine Shared Entry Area,
- getrennte PCID-/ASID-Nutzung,
- starke KASLR,
- isolierte Kernelstacks,
- umfangreiche Guard-Bereiche.

Vollständige Kernelisolation ist ein verbindliches Ziel der 64-Bit-Produktionsarchitektur.

---

## 47. ARM und andere Architekturen

Auf ARM kann NovaOS abhängig von der Architektur verwenden:

- TTBR-Trennung,
- ASIDs,
- PAN,
- PXN,
- UXN,
- Exception-Level-spezifische Mappings.

Andere Architekturen implementieren dieselbe generische Semantik:

```text
User View:
    kein vollständiges Kernelmapping

Kernel View:
    vollständiger Kernelzugriff
```

Die konkrete Register- und Seitentabellensequenz bleibt architekturspezifisch.

---

## 48. Spekulative Ausführung

Kernel Isolation ist insbesondere eine Maßnahme gegen bestimmte Mapping-basierte spekulative Angriffe.

Beim Wechsel können zusätzliche Barrieren erforderlich sein:

- nach Seitentabellenwechsel,
- vor Nutzung sensibler Daten,
- vor Rückkehr in Userspace,
- bei Sicherheitsdomänenwechseln.

KPTI allein behebt nicht sämtliche spekulativen Seitenkanäle.

Weitere Mitigations werden in einer gesonderten ADR festgelegt.

---

## 49. Registerbereinigung

Vor der Rückkehr in Userspace müssen Register, die Kernelgeheimnisse enthalten könnten, bereinigt oder mit definierten Userspace-Werten überschrieben werden.

Dazu können gehören:

- temporäre allgemeine Register,
- interne Pointer,
- Kernel-Stack-Adressen,
- Seitentabellenreferenzen,
- Spekulationsmetadaten.

Die ABI definiert, welche Register ohnehin als Rückgabewerte oder gespeicherter Userspace-Kontext verwendet werden.

---

## 50. Seitentabellenschutz

User- und Kernel-Seitentabellen sind Kernelobjekte und niemals direkt im Userspace beschreibbar.

Änderungen erfolgen ausschließlich über die virtuelle Speicherverwaltung.

Seitentabellen:

- sind NX,
- besitzen kontrollierte Schreibrechte,
- werden nicht über öffentliche physische Mappings zugänglich,
- werden gegen Use-after-free und Generationenkonflikte geschützt.

Ein Prozess darf seine User Page Table nicht direkt manipulieren.

---

## 51. Direct Map

Die physische Direct Map existiert ausschließlich in der Kernel Page Table.

Sie wird nicht in User Page Tables übernommen.

Dadurch werden:

- physische Speicheradressen,
- Kernelobjekte,
- Seitentabellen,
- DMA-Puffer

während normaler Userspace-Ausführung nicht virtuell sichtbar.

Die Direct Map bleibt zusätzlich NX gemäß ADR-SEC-0004.

---

## 52. MMIO

MMIO-Bereiche existieren ausschließlich in dafür vorgesehenen Kernel- oder Treiberadressräumen.

Sie werden nicht allgemein in User Page Tables übernommen.

Userspace-Gerätezugriffe benötigen:

- explizite Geräte-Capability,
- kontrolliertes Mapping,
- IOMMU- und Treiberpolicy,
- eigene nicht privilegierte Abbildung.

Ein solches Mapping ist kein Teil des Kernel-MMIO-Bereichs.

---

## 53. Kernelmodule

Kernelmodule werden ausschließlich in der Kernel Page Table ausführbar abgebildet.

Die User Page Table enthält:

- keinen Modultext,
- keine Moduldaten,
- keine Modulrelokationen,
- keine Modul-Symboltabellen.

Damit kann Userspace Modulpositionen nicht direkt aus seinen Seitentabellen ableiten.

---

## 54. Gemeinsame Informationsseiten

NovaOS kann kleine read-only Informationsseiten in Userspace-Prozesse mappen.

Beispiele:

- monotone Zeitbasis,
- Systemaufrufmetadaten,
- CPU-Funktionsinformationen.

Diese Seiten:

- enthalten keine Kernelpointer,
- enthalten keine Geheimnisse,
- werden als normale Userspace-Seiten behandelt,
- gehören nicht zum Kernelmapping,
- besitzen minimale Rechte.

---

## 55. Signal- und Upcall-Rückkehr

Vor einem kontrollierten Userspace-Upcall wird der Zielkontext vollständig validiert.

Der Kernel:

1. bereitet Userspace-Stack und Trap Frame vor,
2. validiert Instruction Pointer,
3. bereinigt Kernelregister,
4. wechselt auf die User Page Table,
5. kehrt über den Exit Trampoline zurück.

Der Upcall-Code liegt im normalen Userspace-Adressraum und wird niemals im Kernelmodus ausgeführt.

---

## 56. Echtzeitprozesse

KPTI erhöht die Kosten von:

- Systemaufrufen,
- Interrupts,
- Exceptions,
- Context Switches,
- Uaccess.

Diese Kosten müssen in Echtzeitanalysen berücksichtigt werden.

Für harte Echtzeit gilt:

- PCID/ASID bevorzugt,
- Seitentabellen vorab vorbereitet,
- Entry Stacks vorab reserviert,
- keine dynamische Allokation im Entry-Pfad,
- Latenzen gemessen und begrenzt.

KPTI darf nicht unbemerkt pro Thread deaktiviert werden, nur um eine Deadline leichter einzuhalten.

---

## 57. Isolierte Echtzeit-CPU

Eine isolierte Echtzeit-CPU kann eine speziell vorbereitete Kernelisolation verwenden.

Dabei bleiben alle Sicherheitsinvarianten erhalten.

Zulässige Optimierungen:

- vorab geladene TLB-Einträge,
- feste PCID-/ASID-Zuordnung,
- reduzierte, aber nicht unsichere Entry-Pfade,
- begrenzte Interruptquellen.

Ein gemeinsamer vollständiger Kerneladressraum im Userspace bleibt keine zulässige Echtzeitoptimierung im Enforcing Mode.

---

## 58. Virtualisierung

Unter einem Hypervisor kann Kernel Isolation mit Second-Level Translation kombiniert werden.

Der Gastkernel setzt weiterhin seine eigenen getrennten Seitentabellensichten um.

Ein Hypervisor schützt nicht automatisch vor:

- Gastkernel-Mappingfehlern,
- Gast-Userspace-Seitenkanälen,
- fehlerhaften Gast-Uaccess-Pfaden.

Virtuelle CPU-Fähigkeiten wie PCID oder ASID werden geprüft und nicht nur angenommen.

---

## 59. Suspend und Hibernate

Beim Resume müssen für jede CPU erneut hergestellt werden:

- Kernel-Seitentabellenreferenz,
- User-Seitentabellenreferenzen,
- Entry Stack,
- Shared Entry Area,
- PCID-/ASID-Generationen,
- SMEP/SMAP/NX-Zustände.

Vor Wiederaufnahme eines Userspace-Threads wird die Isolation vollständig validiert.

Ein Hibernate-Image muss Seitentabellen- und Entry-Daten authentifizieren.

---

## 60. Live-Update

Live-Updates des Entry Trampolines sind besonders kritisch.

Eine Aktualisierung benötigt:

1. neuen Code verifizieren,
2. neue RX-Seiten vorbereiten,
3. auf allen CPUs einen sicheren Kernelzustand herstellen,
4. User- und Kernel-Seitentabellen atomar aktualisieren,
5. TLB und Instruktionscaches synchronisieren,
6. alte Trampoline erst danach entfernen.

Ein teilweise aktualisierter Entry-Pfad ist nicht zulässig.

---

## 61. Debugger

Ein Kerneldebugger darf Kernel Isolation nicht dauerhaft deaktivieren.

Debuggerzugriff auf:

- User Page Tables,
- Kernel Page Tables,
- Entry Stacks,
- Trampoline-Code,
- PCID-/ASID-Zustände

benötigt spezielle Capabilities.

Debugänderungen werden lokal protokolliert und beeinflussen gegebenenfalls den Trust State.

---

## 62. Crash-Dumps

Crash-Dumps müssen beide Seitentabellensichten berücksichtigen.

Ein Dump kann enthalten:

- Kernel-Seitentabellenwurzel,
- betroffene User-Seitentabellenwurzel,
- PCID-/ASID-Generationen,
- Entry-Stack-Zustand,
- Trampoline-Version,
- KASLR-Build-ID.

Diese Informationen sind sicherheitsrelevant und müssen capabilitygeschützt oder verschlüsselt gespeichert werden.

---

## 63. Sicherheitsmodi

NovaOS definiert folgende Kernelisolationsmodi:

| Modus | Bedeutung |
|---|---|
| `FULL` | getrennte User- und Kernel-Seitentabellen |
| `REDUCED` | teilweise Isolation mit dokumentierten Plattformgrenzen |
| `LEGACY` | gemeinsames Mapping mit anderen Schutzmechanismen |
| `REQUIRED` | Start ohne vollständige Isolation wird abgelehnt |
| `DEVELOPMENT` | kontrollierte Diagnoseabweichungen möglich |

Der 64-Bit-Produktionsstandard ist:

```text
FULL
```

Der anfängliche 32-Bit-Kernel kann abhängig von Hardware und Pagingmodell `REDUCED` verwenden.

---

## 64. Kein stilles Fallback

Schlägt die Aktivierung vollständiger Kernelisolation fehl, darf NovaOS nicht stillschweigend in ein gemeinsames Mappingmodell wechseln.

Zulässige Reaktionen:

- Start abbrechen,
- Recovery starten,
- nach expliziter Policy degradiert starten,
- Legacy-Modus mit deutlicher Kennzeichnung verwenden.

Der Trust State muss den tatsächlich aktiven Modus wiedergeben.

---

## 65. Trust State

Der Kernelisolationsstatus enthält mindestens:

```text
KERNEL_ISOLATION_FULL
KERNEL_ISOLATION_REDUCED
KERNEL_ISOLATION_LEGACY
KPTI_ENABLED
SHARED_ENTRY_MINIMAL
PCID_ACTIVE
ASID_ACTIVE
ENTRY_STACKS_ISOLATED
DIRECT_MAP_HIDDEN_FROM_USER
KERNEL_ISOLATION_DEGRADED
```

Zusätzlich kann der Status pro CPU abgefragt werden.

Konkrete Seitentabellenadressen werden nicht allgemein offengelegt.

---

## 66. API-Grundmodell

Kernel Isolation besitzt keine allgemeine Userspace-Steuerungs-API.

Eine autorisierte Statusabfrage kann lauten:

```c
nova_status_t nova_security_get_kernel_isolation_status(
    nova_kernel_isolation_status_t* status
);
```

Eine mögliche Struktur:

```c
typedef struct
{
    uint32_t structure_size;
    uint32_t structure_version;

    uint32_t isolation_mode;
    uint32_t flags;

    bool separate_user_page_tables;
    bool separate_kernel_page_tables;
    bool shared_entry_minimal;
    bool entry_stacks_isolated;

    bool pcid_active;
    bool asid_active;

    uint32_t online_cpu_count;
    uint32_t protected_cpu_count;

    uint64_t entry_generation;
    uint64_t page_table_generation;
} nova_kernel_isolation_status_t;
```

Die Struktur enthält keine Kerneladressen oder Seitentabellenwurzeln.

---

## 67. Capability-Modell

Mögliche Capabilities sind:

```text
CAP_SECURITY_QUERY_KERNEL_ISOLATION
CAP_VM_INSPECT_PAGE_TABLES
CAP_VM_MODIFY_KERNEL_MAPPINGS
CAP_DEBUG_KERNEL
CAP_CRASH_DUMP_DECRYPT
CAP_SECURITY_KERNEL_ISOLATION_ADMIN
```

Das Recht zur Statusabfrage berechtigt nicht zum Lesen von:

- Seitentabellenwurzeln,
- Kernelmappingadressen,
- Entry-Stack-Inhalten,
- KASLR-Slides.

Eine Capability darf die Isolation nicht unbemerkt deaktivieren.

---

## 68. Selbsttests

NovaOS führt beim Start Kernelisolations-Selbsttests aus.

Mindestens geprüft werden:

- Kerneltext fehlt in User Page Table,
- Kernelheap fehlt in User Page Table,
- Kernelstacks fehlen in User Page Table,
- Direct Map fehlt in User Page Table,
- Shared Entry Area besitzt minimale Rechte,
- Entry Stack ist NX,
- Trampoline-Code ist nicht schreibbar,
- User-/Kernelwechsel funktioniert,
- SMEP/SMAP bleiben aktiv,
- PCID-/ASID-Generationen sind konsistent.

Fehlgeschlagene kritische Tests verhindern den normalen Enforcing-Start.

---

## 69. Fehlerbehandlung

Mögliche Fehlercodes sind:

```text
NOVA_STATUS_OK
NOVA_STATUS_KERNEL_ISOLATION_UNSUPPORTED
NOVA_STATUS_KERNEL_ISOLATION_DEGRADED
NOVA_STATUS_USER_PAGE_TABLE_INVALID
NOVA_STATUS_KERNEL_PAGE_TABLE_INVALID
NOVA_STATUS_SHARED_ENTRY_INVALID
NOVA_STATUS_ENTRY_STACK_INVALID
NOVA_STATUS_PAGE_TABLE_SWITCH_FAILED
NOVA_STATUS_PCID_EXHAUSTED
NOVA_STATUS_ASID_EXHAUSTED
NOVA_STATUS_TLB_GENERATION_MISMATCH
NOVA_STATUS_KERNEL_MAPPING_EXPOSED
NOVA_STATUS_ENTRY_INVARIANT_VIOLATION
NOVA_STATUS_SECURITY_POLICY_CONFLICT
```

Eine ungültige Userspace-Anforderung darf keinen Kernel-Panic auslösen.

Ein Kernel-Panic ist zulässig bei:

- Ausführung mit falscher Seitentabellensicht,
- sichtbarem vollständigem Kernelmapping entgegen Enforcing Policy,
- beschädigtem Entry Stack,
- unmöglicher Rückkehr in einen sicheren Adressraum,
- inkonsistenter PCID-/ASID-Wiederverwendung.

---

## 70. Telemetrie und Diagnose

NovaOS erfasst mindestens:

- aktiven Kernelisolationsmodus,
- KPTI-Aktivierungsstatus,
- PCID-/ASID-Unterstützung,
- Anzahl User-zu-Kernel-Wechsel,
- Anzahl Kernel-zu-User-Wechsel,
- TLB-Flushes,
- PCID-/ASID-Wiederverwendungen,
- Entry- und Exit-Latenzen,
- fehlgeschlagene Selbsttests,
- CPU-Hotplug-Abweisungen,
- degradierte Plattformzustände.

Konkrete Kerneladressen und Seitentabellenwurzeln werden in normaler Telemetrie verborgen.

---

## 71. Performance

Kernel Isolation erhöht die Kosten von:

- Systemaufrufen,
- Interrupts aus Userspace,
- Exceptions aus Userspace,
- Context Switches zwischen Prozessen,
- TLB-Verwaltung.

NovaOS reduziert diese Kosten durch:

- PCID oder ASID,
- kleine Shared Entry Area,
- minimale Entry- und Exit-Pfade,
- wiederverwendete Kernelmappingstrukturen,
- generationengesicherte TLB-Einträge,
- keine unnötigen Seitentabellenwechsel bei Kernelinterrupts.

Sicherheit hat Vorrang vor einer geringfügig geringeren Systemaufruflatenz.

---

## 72. Konsequenzen

### 72.1 Positive Konsequenzen

- vollständiger Kerneladressraum ist im Userspace nicht gemappt,
- stärkere Trennung zwischen Kernel und Prozessen,
- bessere Abwehr bestimmter spekulativer Angriffe,
- KASLR wird weniger leicht über Mappings offengelegt,
- Kernelstacks und Direct Map bleiben aus User Page Tables entfernt,
- klare Entry- und Exit-Invarianten,
- gute Integration mit SMEP, SMAP, NX und Capabilities,
- hardwarebeschleunigte Optimierung über PCID und ASID.

### 72.2 Negative Konsequenzen

- höhere Systemaufruf- und Interruptkosten,
- komplexere Entry- und Exit-Assembly,
- zusätzliche Seitentabellen pro Prozess,
- Shared Entry Area wird hochgradig sicherheitskritisch,
- 32-Bit-Systeme bieten nur begrenzte Umsetzungsmöglichkeiten,
- Debugging und Crash-Analyse werden komplexer.

### 72.3 Risiken

- vollständige Kernelseiten versehentlich in User Page Table,
- fehlerhafte Shared-Entry-Rechte,
- falscher Stack beim Seitentabellenwechsel,
- PCID-/ASID-Wiederverwendung ohne Invalidierung,
- Rückkehr mit falscher Seitentabellensicht,
- KASLR-Leak über Entry-Daten,
- inkompatibler CPU-Hotplug,
- unvollständige Resume-Wiederherstellung.

Diese Risiken werden begrenzt durch:

- minimale Shared Entry Area,
- automatische Seitentabellentests,
- Generationen,
- Guard Pages,
- PCID-/ASID-Validierung,
- atomare Entry-Updates,
- Trust-State-Überwachung,
- umfangreiche Mehrkern- und Fault-Injection-Tests.

---

## 73. Auswirkungen auf andere Subsysteme

### 73.1 Virtuelle Speicherverwaltung

Jeder Prozess benötigt getrennte User- und Kernel-Seitentabellensichten.

### 73.2 Context Switching

Context Switches aktualisieren beide Seitentabellenreferenzen und ihre Generationen.

### 73.3 Systemaufrufe

Systemaufrufe durchlaufen einen minimalen Entry Trampoline vor dem normalen Dispatcher.

### 73.4 Interrupt- und Exceptionsystem

Entry-Pfade müssen zwischen User- und Kernelursprung unterscheiden.

### 73.5 Kernel Stack Layout

Thread-Kernelstacks bleiben aus User Page Tables entfernt; CPU-Entry-Stacks werden separat bereitgestellt.

### 73.6 KASLR

Shared Entry Area und Seitentabellen dürfen keine unnötigen Kerneladressen offenlegen.

### 73.7 NX Memory

Trampoline-Code ist RX, Entry-Daten und Entry-Stacks sind NX.

### 73.8 SMEP/SMAP

SMEP und SMAP bleiben trotz Seitentabellenwechsel aktiv und ergänzen KPTI.

### 73.9 CPU-Hotplug

Jede CPU benötigt vollständig initialisierte Entry- und Isolationseigenschaften.

### 73.10 Echtzeitscheduler

Entry-, Exit- und TLB-Kosten werden in Latenz- und Budgetberechnungen einbezogen.

### 73.11 Crash-Dump-System

Dumps müssen beide Seitentabellensichten und sensible Mappinginformationen geschützt behandeln.

### 73.12 Secure Startup

Der Kernelisolationsmodus und die Selbsttestergebnisse werden Teil des Trust State.

---

## 74. Implementierungsrichtlinien

1. Der vollständige Kerneladressraum wird nicht in normalen User Page Tables abgebildet.
2. Jede User Page Table enthält nur eine minimale Shared Entry Area.
3. Trampoline-Code wird RX und niemals schreibbar abgebildet.
4. Entry-Daten und Entry-Stacks sind NX.
5. Vollständige Thread-Kernelstacks bleiben aus User Page Tables entfernt.
6. Systemaufrufe, Interrupts und Exceptions wechseln vor normaler Kernelarbeit auf die Kernel Page Table.
7. Die Rückkehr in Userspace aktiviert erst nach vollständiger Validierung die User Page Table.
8. Uaccess-Fenster müssen vor der Rückkehr geschlossen sein.
9. SMEP, SMAP und NX bleiben zusätzlich aktiv.
10. PCID oder ASID werden verwendet, wenn sicher verfügbar.
11. Kennungen werden generationengesichert wiederverwendet.
12. Die Direct Map existiert ausschließlich in Kernel Page Tables.
13. Jede CPU erhält eigene Entry- und Notfallstacks.
14. Zusätzliche CPUs werden erst nach bestandenem Isolationstest online geschaltet.
15. Der 32-Bit-Modus darf reduzierte Isolation verwenden, muss dies aber sichtbar kennzeichnen.
16. Der 64-Bit-Produktionskernel verwendet vollständiges KPTI.
17. Fehlgeschlagene Isolation darf nicht stillschweigend zu einem Legacy-Mapping führen.
18. Shared-Entry-Änderungen erfolgen atomar auf allen CPUs.
19. Der Kernel führt beim Start automatische Mapping- und Rechtestests aus.
20. Allgemeine Diagnose-APIs geben keine Seitentabellenwurzeln oder Kerneladressen zurück.

---

## 75. Testanforderungen

Mindestens folgende Szenarien müssen getestet werden:

- Erzeugung getrennter User- und Kernel-Seitentabellen,
- Kerneltext fehlt in User Page Table,
- Kernelheap fehlt in User Page Table,
- Kernelstack fehlt in User Page Table,
- Direct Map fehlt in User Page Table,
- Modulbereiche fehlen in User Page Table,
- gültige Shared Entry Area,
- Trampoline-Code ist RX,
- Entry Stack ist RW und NX,
- Systemaufruf aus Userspace,
- schneller Systemaufrufpfad,
- Interrupt aus Userspace,
- Interrupt aus Kernel,
- Exception aus Userspace,
- Exception im Entry Trampoline,
- verschachtelte Interrupts,
- Rückkehr in Userspace,
- ungültiger Userspace-Rückkehrzustand,
- Context Switch im selben Prozess,
- Context Switch zwischen Prozessen,
- Kernelthread,
- Idle Thread,
- Uaccess unter KPTI,
- SMAP-Fault während Uaccess,
- offenes Uaccess-Fenster bei Exit,
- PCID-Unterstützung,
- PCID-Wiederverwendung,
- ASID-Unterstützung,
- ASID-Wiederverwendung,
- Hardware ohne PCID/ASID,
- TLB Shootdown,
- Mehrkernsystem,
- CPU-Hotplug,
- CPU-Offlining,
- NUMA-System,
- 32-Bit-Kernel,
- 32-Bit-PAE,
- reduzierte 32-Bit-Isolation,
- vollständiger 64-Bit-Modus,
- ARM-Backend,
- Echtzeitlatenz,
- Suspend und Resume,
- Hibernate,
- Virtualisierung,
- Live-Update des Trampolines,
- Kerneldebugger,
- Crash-Dump,
- KASLR-Leak-Prüfung,
- Mapping-Fuzzing,
- Seitentabellen-Fault-Injection,
- automatische Isolation-Selbsttests.

---

## 76. Nicht Bestandteil dieser Entscheidung

Diese ADR legt nicht abschließend fest:

- konkrete x86- oder ARM-Assemblysequenzen,
- die vollständige Seitentabellenstruktur,
- genaue virtuelle Adressen der Shared Entry Area,
- PCID- und ASID-Bitaufteilungen,
- sämtliche Spectre- und Meltdown-Mitigationen,
- strikten Kerneladressraum ohne dauerhafte Userspace-Mappings,
- Hypervisorbasierte Kernelisolation,
- vollständige Memory-Encryption,
- die endgültige Diagnose-ABI,
- konkrete Performancegrenzen.

Diese Details werden in nachfolgenden ADRs und NPSPEC-Dokumenten definiert.

---

## 77. Verbindliche Invarianten

1. Der vollständige Kerneladressraum darf in einem vollständig isolierten System nicht in normalen User Page Tables gemappt sein.
2. User Page Tables enthalten ausschließlich den Userspace und eine minimale Shared Entry Area.
3. Kerneltext, Kernelheap, Kernelstacks, Kernelmodule und Direct Map fehlen in User Page Tables.
4. Die Shared Entry Area enthält nur zwingend erforderlichen Entry- und Exit-Code sowie minimale CPU-lokale Daten.
5. Shared-Entry-Code ist ausführbar, aber nicht schreibbar.
6. Shared-Entry-Daten und Entry-Stacks sind nicht ausführbar.
7. Entry-Seiten bleiben Supervisor-only.
8. Normale Kernelarbeit beginnt erst nach Aktivierung der Kernel Page Table.
9. Die Rückkehr in Userspace erfolgt erst nach vollständiger Validierung des Rückkehrkontexts.
10. Vor User-Rückkehr müssen sämtliche Uaccess-Fenster geschlossen sein.
11. Jeder Thread startet im Kernel mit einer gültigen Kernel-Seitentabellensicht.
12. Kernelthreads und Idle Threads besitzen keine implizite User Page Table.
13. SMEP, SMAP und NX bleiben unabhängig von KPTI verbindlich.
14. PCID- und ASID-Kennungen dürfen nur generationengesichert wiederverwendet werden.
15. Ein veralteter TLB-Eintrag darf keine Kernelabbildung in einer User Page Table sichtbar machen.
16. Jede online geschaltete CPU besitzt vollständig initialisierte Entry- und Isolationseigenschaften.
17. Ein fehlgeschlagener Enforcing-Isolationsstart darf nicht stillschweigend in den Legacy-Modus wechseln.
18. Reduzierte oder fehlende Isolation muss im Trust State sichtbar sein.
19. Der 32-Bit-Modus darf keine vollständige 64-Bit-Isolationsgarantie behaupten.
20. Der 64-Bit-Produktionskernel verwendet vollständige getrennte User- und Kernel-Seitentabellensichten.
21. Kerneladressraum- und Seitentabelleninformationen werden nicht über allgemeine Diagnose-APIs offengelegt.
22. Eine Capability allein darf Kernel Isolation nicht unbemerkt deaktivieren.
23. Kernel Isolation ersetzt weder KASLR, NX, SMEP, SMAP, Stack Protector, Capability-Prüfung noch Speichersicherheit.
24. KI- oder lernbasierte Komponenten dürfen Kernelisolation, Seitentabellen-, Entry-, TLB-, Capability-, Debug- oder Secure-Startup-Regeln nicht umgehen.

---

## 78. Referenzen

- ADR-SEC-0001 – Secure Kernel Startup
- ADR-SEC-0002 – Kernel ASLR
- ADR-SEC-0003 – Stack Protector
- ADR-SEC-0004 – NX Memory
- ADR-SEC-0005 – SMEP/SMAP
- ADR-MM-0002 – Virtual Memory Management
- ADR-MM-0012 – NUMA Memory Policy
- ADR-MM-0013 – Guard Pages
- ADR-MM-0015 – Kernel Stack Layout
- ADR-PROC-0002 – Thread Lifecycle
- ADR-PROC-0005 – Echtzeitprozesse
- ADR-PROC-0006 – Idle Thread
- ADR-PROC-0007 – CPU Affinity
- ADR-PROC-0008 – Context Switching
- ADR-KERNEL-0103 – Native Capability-basierte Ressourcenverwaltung
- ADR-KERNEL-0104 – Integrierte Telemetrie- und Diagnoseschnittstellen
- ADR-KERNEL-0105 – ABI-Stabilität über versionierte Kernel-Services
- NPSPEC-KERNELISOLATION-0001 – Kernel Isolation Specification
- NPSPEC-VIRTUALMEMORY-0001 – Virtual Memory Specification
- NPSPEC-SMEP-SMAP-0001 – SMEP/SMAP Specification
- NPSPEC-NXMEMORY-0001 – NX Memory Specification
- NPSPEC-CONTEXTSWITCH-0001 – Context Switching Specification
- NPSPEC-SYSCALL-0001 – System Call Specification
- NPSPEC-EXCEPTION-0001 – Exception Handling Specification

---

## 79. Zusammenfassung der Entscheidung

NovaOS verwendet Kernel Page-Table Isolation mit getrennten User- und Kernel-Seitentabellensichten.

Während normaler Userspace-Ausführung enthält die aktive User Page Table ausschließlich:

- den Prozessadressraum,
- eine minimale Shared Entry Area,
- notwendige Supervisor-only-Trampoline- und Entry-Seiten.

Der vollständige Kerneladressraum mit Kerneltext, Heap, Stacks, Modulen, Direct Map und MMIO ist dort nicht abgebildet.

Bei Systemaufrufen, Interrupts und Exceptions wechselt ein kleiner Entry Trampoline zunächst auf die Kernel Page Table und anschließend auf den vollständigen Thread-Kernelstack. Vor der Rückkehr werden der Userspace-Kontext validiert, Kernelregister bereinigt und die User Page Table wieder aktiviert.

SMEP, SMAP, NX und User/Supervisor-Seitenrechte bleiben zusätzlich aktiv. PCID, ASID und generationengesicherte TLB-Verwaltung reduzieren die Kosten der Seitentabellenwechsel.

Der anfängliche 32-Bit-Kernel kann aufgrund des begrenzten Adressraums und älterer Hardware eine reduzierte Isolation verwenden. Dieser Zustand wird ausdrücklich gekennzeichnet. Der spätere 64-Bit-Produktionskernel verwendet vollständige getrennte User- und Kernel-Seitentabellen.