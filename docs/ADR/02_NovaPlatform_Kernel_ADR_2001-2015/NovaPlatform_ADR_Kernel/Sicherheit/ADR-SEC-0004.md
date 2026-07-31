# ADR-SEC-0004 – NX Memory

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-SEC-0004 |
| Titel | NX Memory |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Sicherheit / Kernel / Virtuelle Speicherverwaltung / Ausführungsschutz |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-SEC-0001, ADR-SEC-0002, ADR-SEC-0003, ADR-MM-0002, ADR-MM-0013, ADR-MM-0014, ADR-MM-0015 |
| Nachfolgende Spezifikation | NPSPEC-NXMEMORY-0001 |

---

## 1. Kontext

Moderne Prozessoren können für einzelne Speicherseiten festlegen, ob darin enthaltene Daten als Programmcode ausgeführt werden dürfen.

Diese Eigenschaft wird abhängig von der Architektur bezeichnet als:

- NX – No Execute,
- XD – Execute Disable,
- XN – Execute Never,
- PXN – Privileged Execute Never,
- UXN – Unprivileged Execute Never.

Ohne Ausführungsschutz können Speicherbereiche wie:

- Kernelstacks,
- Userspace-Stacks,
- Kernelheap,
- Userspace-Heap,
- Datenabschnitte,
- DMA-Puffer,
- gemeinsam genutzter Speicher,
- Gerätedaten,
- Netzwerkpuffer

möglicherweise direkt als Code ausgeführt werden.

Ein Angreifer könnte einen Speicherfehler verwenden, um eigenen Maschinencode in einen beschreibbaren Puffer einzubringen und die CPU anschließend zu diesem Puffer umzuleiten.

NovaOS benötigt deshalb eine verbindliche Richtlinie, nach der Speicher standardmäßig nicht ausführbar ist und nur ausdrücklich als Code vorgesehene Bereiche Ausführungsrechte erhalten.

---

## 2. Problemstellung

NovaOS soll sowohl auf moderner als auch auf älterer Hardware funktionieren.

Die erste Kernelimplementierung verwendet zunächst einen 32-Bit-x86-Kernel. Auf dieser Plattform ist hardwarebasierter Seitenausführungsschutz typischerweise nur verfügbar, wenn:

- die CPU das NX-Bit unterstützt,
- Extended Page Tables beziehungsweise PAE-Seitentabellen verwendet werden,
- das NXE-Bit in einem architekturspezifischen Kontrollregister aktiviert wird.

Ohne PAE besitzt klassisches 32-Bit-Paging auf x86 kein per-page NX-Bit.

Der spätere 64-Bit-Kernel kann NX als reguläre Seitentabelleneigenschaft verwenden.

Die Architektur muss deshalb:

- NX-fähige Hardware erkennen,
- PAE auf unterstützten 32-Bit-Systemen berücksichtigen,
- klare Fallback-Regeln für ältere Hardware definieren,
- keine vollständige NX-Garantie behaupten, wenn die Hardware sie nicht bereitstellt,
- Bootloader, Kernel, Module, JIT und Userspace gemeinsam behandeln.

---

## 3. Entscheidungsziele

NX Memory soll:

1. Datenbereiche standardmäßig nicht ausführbar machen,
2. Kernelstacks und Kernelheap vor Codeausführung schützen,
3. Userspace-Datenbereiche ebenfalls als NX markieren,
4. W^X als verbindliche Grundregel durchsetzen,
5. ausführbaren Speicher nur über kontrollierte APIs erlauben,
6. Kernelmodule sicher laden und finalisieren,
7. JIT-Compiler ohne dauerhaftes RWX unterstützen,
8. die Fähigkeiten der Hardware korrekt erkennen,
9. 32-Bit- und 64-Bit-Architekturen berücksichtigen,
10. DMA- und MMIO-Bereiche niemals unbeabsichtigt ausführbar machen,
11. Verstöße diagnostizierbar machen,
12. mit Secure Startup, KASLR und Stack Protector zusammenarbeiten.

---

## 4. Begriffsdefinitionen

### 4.1 NX

NX bedeutet, dass die CPU Instruktionsabrufe aus einer bestimmten Speicherseite verweigert.

---

### 4.2 Executable Memory

Executable Memory ist Speicher, aus dem die CPU Programminstruktionen abrufen darf.

---

### 4.3 Writable Memory

Writable Memory ist Speicher, dessen Inhalt durch die zugriffsberechtigte Ausführung verändert werden darf.

---

### 4.4 W^X

W^X bedeutet:

> Eine Speicherseite darf nicht gleichzeitig beschreibbar und ausführbar sein.

Formal:

```text
Write XOR Execute
```

Zulässige Kombinationen sind beispielsweise:

```text
Read + Execute
Read + Write
Read only
No Access
```

Nicht zulässig ist im normalen Betrieb:

```text
Read + Write + Execute
```

---

### 4.5 Code Finalization

Code Finalization ist der kontrollierte Übergang eines Speicherbereichs von einem beschreibbaren Zustand in einen ausführbaren, nicht mehr beschreibbaren Zustand.

---

### 4.6 Execute Fault

Ein Execute Fault ist eine CPU-Exception, die beim Versuch entsteht, Code aus einer nicht ausführbaren Seite abzurufen.

---

## 5. Bedrohungsmodell

NX Memory erschwert insbesondere:

- Ausführung eingeschleusten Codes auf dem Stack,
- Ausführung eingeschleusten Codes im Heap,
- Ausführung manipulierter Netzwerk- oder Dateipuffer,
- direkte Ausführung aus DMA-Puffern,
- Ausführung aus gemeinsam genutzten Datenbereichen,
- bestimmte Formen von Code Injection,
- Missbrauch beschreibbarer Kernelbereiche als Code.

NX schützt nicht vollständig gegen:

- Return-Oriented Programming,
- Jump-Oriented Programming,
- Wiederverwendung bereits ausführbaren Codes,
- Kontrollflussfehler innerhalb gültiger Codeseiten,
- beliebige Schreibzugriffe auf beschreibbare Daten,
- falsch als ausführbar markierte Seiten,
- Hardware ohne Ausführungsschutz,
- kompromittierte Seitentabellen,
- physische Angriffe.

NX ist deshalb eine Defense-in-Depth-Maßnahme und muss mit KASLR, Stack Protector, W^X, Kontrollflussschutz und Speichersicherheit kombiniert werden.

---

## 6. Betrachtete Alternativen

### 6.1 Alle gemappten Seiten sind ausführbar

NovaOS verwendet keine NX-Markierungen.

#### Vorteile

- einfache Speicherverwaltung,
- maximale Kompatibilität mit älterer Hardware,
- keine besonderen Codegenerierungsregeln.

#### Nachteile

- Stack und Heap können Code enthalten,
- Code-Injection-Angriffe werden erleichtert,
- keine klare Trennung zwischen Code und Daten.

Diese Alternative wird verworfen.

---

### 6.2 NX nur für Userspace

Userspace-Daten werden als NX markiert, der Kerneladressraum bleibt ausführbar.

#### Vorteile

- Schutz vieler Anwendungsprozesse,
- einfachere Kernelimplementierung.

#### Nachteile

- Kernelstacks und Kernelheap bleiben ausführbar,
- Kernel-Speicherfehler können eingeschleusten Code ausführen,
- unzureichend für ein sicherheitsorientiertes Kernelmodell.

Diese Alternative wird verworfen.

---

### 6.3 NX nur für Stackbereiche

Nur Kernel- und Userspace-Stacks werden als nicht ausführbar markiert.

#### Vorteile

- Schutz gegen klassische Stack-Code-Injection,
- geringer Implementierungsaufwand.

#### Nachteile

- Heap-, DMA- und Datenpuffer bleiben ausführbar,
- Angreifer können auf andere beschreibbare Bereiche ausweichen,
- kein allgemeines W^X-Modell.

Diese Alternative wird verworfen.

---

### 6.4 Default-NX mit verbindlichem W^X

Alle neuen Mappings sind standardmäßig nicht ausführbar. Ausführungsrechte werden ausschließlich ausdrücklich und kontrolliert vergeben.

#### Vorteile

- klare Sicherheitsgrundlage,
- Schutz von Stack, Heap, DMA und Datenbereichen,
- kontrollierte Unterstützung für Module und JIT,
- leicht prüfbare Invarianten,
- gute Integration mit Capability- und Speichermodell.

#### Nachteile

- höhere Komplexität der Mapping-APIs,
- ältere Software kann Anpassungen benötigen,
- Hardware ohne NX benötigt einen degradierten Modus,
- JIT-Compiler benötigen spezielle Schnittstellen.

Diese Alternative wird gewählt.

---

## 7. Entscheidung

NovaOS verwendet Default-NX und setzt W^X als verbindliche Speicherschutzregel durch.

Jede neue virtuelle Speicherabbildung ist standardmäßig:

```text
Read = entsprechend der Anforderung
Write = entsprechend der Anforderung
Execute = verboten
```

Ausführungsrechte müssen ausdrücklich angefordert und durch den Kernel geprüft werden.

Eine Seite darf im normalen Produktionsbetrieb niemals gleichzeitig:

```text
Write = erlaubt
Execute = erlaubt
```

besitzen.

---

## 8. Standard-Seitenrechte

NovaOS verwendet folgende typische Seitenrechte:

| Bereich | Lesen | Schreiben | Ausführen |
|---|---:|---:|---:|
| Kerneltext | Ja | Nein | Ja |
| Kernel-Rodata | Ja | Nein | Nein |
| Kerneldaten | Ja | Ja | Nein |
| Kernelheap | Ja | Ja | Nein |
| Kernelstack | Ja | Ja | Nein |
| Kernelmodule während des Ladens | Ja | Ja | Nein |
| finalisierter Modultext | Ja | Nein | Ja |
| Modul-Rodata | Ja | Nein | Nein |
| Moduldaten | Ja | Ja | Nein |
| Userspace-Code | Ja | Nein | Ja |
| Userspace-Rodata | Ja | Nein | Nein |
| Userspace-Heap | Ja | Ja | Nein |
| Userspace-Stack | Ja | Ja | Nein |
| DMA-Puffer | Ja | Ja | Nein |
| MMIO | Geräteabhängig | Geräteabhängig | Nein |
| Seitentabellen | Ja | kontrolliert | Nein |

---

## 9. Default-NX

Die virtuelle Speicherverwaltung behandelt fehlende Ausführungsangaben immer als nicht ausführbar.

Eine Mapping-API darf nicht standardmäßig Ausführungsrechte vergeben.

Beispiel:

```c
nova_vm_map(
    address_space,
    virtual_address,
    physical_address,
    size,
    NOVA_VM_READ | NOVA_VM_WRITE
);
```

Das Fehlen von `NOVA_VM_EXECUTE` bedeutet zwingend NX.

---

## 10. Explizite Ausführungsrechte

Ausführungsrechte werden ausschließlich über explizite Flags vergeben.

```c
NOVA_VM_EXECUTE
```

Der Kernel prüft dabei:

- aufrufende Capability,
- Zieladressraum,
- Mappingtyp,
- aktuelle Schreibrechte,
- Dateiintegrität,
- Signatur- oder Loaderstatus,
- Sicherheitsdomäne,
- W^X-Konflikte.

Ein beliebiger Userspace-Prozess darf nicht automatisch jeden beschreibbaren Speicher in ausführbaren Speicher umwandeln.

---

## 11. W^X-Regel

Für normale Seiten gilt verbindlich:

```text
Writable(page) → not Executable(page)
Executable(page) → not Writable(page)
```

Versucht ein Aufrufer, beide Rechte gleichzeitig zu setzen, wird die Operation abgelehnt.

Möglicher Statuswert:

```text
NOVA_STATUS_WX_VIOLATION
```

Ausnahmen sind ausschließlich streng begrenzte, interne Übergangszustände während des frühen Bootstraps oder spezieller Architekturvorgänge.

---

## 12. Temporäre W^X-Übergänge

Beim Laden oder Erzeugen von Code sind zwei getrennte Zustände zulässig:

```text
Zustand 1:
    Read + Write
    Execute verboten

Zustand 2:
    Read + Execute
    Write verboten
```

Der Übergang lautet:

```text
Speicher zuweisen
        ↓
Code schreiben oder laden
        ↓
Code prüfen
        ↓
Daten- und Instruktionscache synchronisieren
        ↓
Schreibrecht entfernen
        ↓
Ausführungsrecht setzen
        ↓
TLB synchronisieren
```

Ein RWX-Zwischenzustand ist nicht erforderlich und nicht zulässig.

---

## 13. Kerneltext

Der Kerneltext wird nach Abschluss von Laden und Relokation als:

```text
Read + Execute
```

abgebildet.

Schreibzugriffe sind anschließend verboten.

Temporäre Schreibrechte während Relokation oder Patchen müssen:

- lokal begrenzt,
- zeitlich begrenzt,
- vollständig protokollierbar,
- vor Freigabe weiterer CPUs beendet

sein.

Der Kerneltext darf nicht dauerhaft über eine zweite schreibbare Aliasabbildung erreichbar bleiben.

---

## 14. Kernel-Rodata

Unveränderliche Kerneldaten werden als:

```text
Read only
Execute verboten
```

abgebildet.

Dazu gehören beispielsweise:

- konstante Tabellen,
- bestimmte Sicherheitsrichtlinien,
- schreibgeschützte Funktionsmetadaten,
- unveränderliche Konfigurationswerte,
- Teile der Symbolinformationen.

Read-only-Daten dürfen nicht ausgeführt werden, sofern die Architektur keine untrennbare Seitengranularität erzwingt.

---

## 15. Kerneldaten

Beschreibbare Kerneldaten werden als:

```text
Read + Write
Execute verboten
```

abgebildet.

Dies betrifft:

- globale Variablen,
- CPU-lokale Daten,
- Schedulerzustände,
- Objektlisten,
- Cache- und Verwaltungsdaten,
- dynamisch veränderbare Tabellen.

Ausführungsrechte werden nicht aufgrund räumlicher Nähe zum Kerneltext übernommen.

---

## 16. Kernelheap

Alle Kernelheap-Seiten sind standardmäßig NX.

Der Heapallocator darf keine ausführbaren Seiten aus allgemeinen Heap-Pools zurückgeben.

Komponenten, die ausführbaren Speicher benötigen, verwenden einen separaten kontrollierten Code-Allocator.

Dadurch wird verhindert, dass ein normaler Heapfehler direkt ausführbaren Speicher erzeugt oder manipuliert.

---

## 17. Kernelstacks

Alle Kernelstacks sind:

```text
Read + Write
Execute verboten
```

Dies gilt für:

- normale Thread-Kernelstacks,
- Kernelthreadstacks,
- Idle-Threadstacks,
- Interruptstacks,
- Exceptionstacks,
- Notfallstacks,
- Panic- und Double-Fault-Stacks.

Guard Pages gemäß ADR-MM-0013 bleiben zusätzlich aktiv.

---

## 18. Userspace-Stacks

Userspace-Stacks sind standardmäßig nicht ausführbar.

Der Prozessloader markiert sie als:

```text
Read + Write
Execute verboten
```

Eine Binärdatei darf keinen ausführbaren Stack allein durch eine unbestätigte Metadatenangabe verlangen.

Ausführbare Userspace-Stacks sind im Produktionsmodus standardmäßig nicht erlaubt.

---

## 19. Userspace-Heap

Userspace-Heaps sind standardmäßig:

```text
Read + Write
Execute verboten
```

Ein Prozess darf Heapseiten nicht gleichzeitig beschreibbar und ausführbar markieren.

JIT-Anwendungen verwenden eine spezielle Codegenerierungsschnittstelle.

---

## 20. Programmloader

Der Programmloader leitet Seitenrechte aus den verifizierten Segmenten einer ausführbaren Datei ab.

Typische Zuordnung:

```text
Textsegment:
    Read + Execute

Rodata:
    Read only

Datensegment:
    Read + Write

BSS:
    Read + Write
```

Der Loader lehnt Segmente ab, die gleichzeitig Schreib- und Ausführungsrechte verlangen, sofern keine ausdrücklich autorisierte Kompatibilitätspolicy existiert.

---

## 21. Segmentüberlappungen

Ausführbare Dateien dürfen keine überlappenden Segmente verwenden, durch die dieselbe Seite gleichzeitig Schreib- und Ausführungsrechte erhält.

Der Loader prüft:

- virtuelle Bereiche,
- Seitenausrichtung,
- Dateioffsets,
- Segmentgrenzen,
- zusammengeführte Seitenrechte,
- Integerüberläufe.

Widersprüchliche Rechte führen zur Ablehnung des Programms oder Moduls.

---

## 22. Kernelmodule

Kernelmodule werden in mehreren Phasen geladen.

### 22.1 Ladephase

```text
Modultext:
    Read + Write
    Execute verboten
```

### 22.2 Prüfphase

Der Loader:

- prüft Signatur,
- prüft ABI,
- validiert Relokationen,
- löst erlaubte Symbole auf,
- prüft Abschnittsgrenzen.

### 22.3 Finalisierung

```text
Text:
    Read + Execute

Rodata:
    Read only

Daten:
    Read + Write
```

Erst nach Finalisierung darf der Modulcode ausgeführt werden.

---

## 23. Modulrelokationen

Relokationen werden ausgeführt, solange der Modultext noch nicht ausführbar ist.

Nach Abschluss:

1. Relokationstabellen validieren,
2. Codecache synchronisieren,
3. Schreibrechte entfernen,
4. Ausführungsrechte setzen,
5. TLB aktualisieren,
6. Modul als ausführbar veröffentlichen.

Ein teilweise finalisiertes Modul darf für andere CPUs oder Threads nicht sichtbar werden.

---

## 24. Modulentladung

Bei der Entladung eines Moduls werden zuerst seine Ausführungsrechte entfernt.

Der Ablauf lautet:

```text
neue Aufrufe verhindern
        ↓
aktive Aufrufe beenden
        ↓
Ausführungsrechte entfernen
        ↓
TLB und CPU-Caches synchronisieren
        ↓
Modulspeicher freigeben
```

Freigegebene Modulseiten dürfen nicht weiter ausführbar bleiben.

---

## 25. JIT-Compiler

NovaOS unterstützt JIT-Compiler über eine kontrollierte Codegenerierungs-API.

Ein JIT darf keine dauerhaft RWX-gemappte Seite erhalten.

Der bevorzugte Ablauf lautet:

```text
JIT-Codeobjekt erzeugen
        ↓
beschreibbare NX-Ansicht erhalten
        ↓
Code erzeugen
        ↓
Codeobjekt finalisieren
        ↓
beschreibbare Ansicht verlieren
        ↓
ausführbare RX-Ansicht erhalten
```

---

## 26. Dual Mapping

Eine Implementierung kann dasselbe physische Codeobjekt über zwei virtuelle Ansichten abbilden:

```text
Writer View:
    Read + Write
    Execute verboten

Executor View:
    Read + Execute
    Write verboten
```

Beide Ansichten dürfen nicht gleichzeitig für denselben unkontrollierten Prozess vollständig nutzbar sein.

Der Kernel muss verhindern, dass:

- über die Writer View Code verändert wird,
- während andere Threads ihn über die Executor View ausführen.

Dazu verwendet NovaOS Finalisierung, Generationen und Synchronisationsbarrieren.

---

## 27. JIT-Capability

JIT-Codegenerierung benötigt eine spezielle Capability.

```text
CAP_MEMORY_JIT
```

Diese Capability kann begrenzt werden auf:

- einen bestimmten Prozess,
- maximale Codegröße,
- bestimmte Codeobjekte,
- erlaubte Compiler- oder Laufzeitkomponenten,
- Sicherheitsdomäne,
- Gültigkeitsdauer.

Eine normale Speicher-Mapping-Capability berechtigt nicht automatisch zur JIT-Codeerzeugung.

---

## 28. JIT-Finalisierung

Bei der Finalisierung eines JIT-Codeobjekts prüft NovaOS mindestens:

- Objektzustand,
- Größe,
- Schreibansichten,
- aktive Writer,
- Zieladressraum,
- Sicherheitspolicy,
- Cache-Synchronisation,
- W^X-Invarianten.

Optional können zusätzliche Prüfungen erfolgen:

- Kontrollflussvalidierung,
- Signatur eines Codecaches,
- Bytecode-Herkunft,
- erlaubte Instruktionsklassen,
- Sandboxpolicy.

---

## 29. JIT-Neubearbeitung

Soll finalisierter Code geändert werden, wird er nicht direkt wieder beschreibbar und gleichzeitig ausführbar gemacht.

Stattdessen wird:

1. eine neue beschreibbare Generation erzeugt oder
2. die Ausführung der alten Generation vollständig gestoppt,
3. die Ausführungsabbildung entfernt,
4. die Schreibabbildung aktiviert,
5. der Code geändert,
6. erneut finalisiert.

Die neue Generation erhält eine eigene Identität.

---

## 30. DMA-Speicher

DMA-Puffer sind immer nicht ausführbar.

Dies gilt unabhängig davon, ob der Puffer:

- vom Kernel,
- von einem Treiber,
- von einem Gerät,
- über eine IOMMU,
- physisch zusammenhängend

bereitgestellt wird.

Die CPU-Abbildung eines DMA-Puffers erhält kein Ausführungsrecht.

Ein Gerät darf über DMA keinen direkt ausführbaren Kernelcode erzeugen.

---

## 31. MMIO

Memory-Mapped I/O wird niemals als ausführbarer Speicher abgebildet.

MMIO-Bereiche werden mit architekturspezifischen Cache- und Ordering-Eigenschaften markiert.

Standard:

```text
Read = geräteabhängig
Write = geräteabhängig
Execute = verboten
```

Ein Instruktionsabruf aus MMIO ist immer ein Sicherheits- oder Kernelprogrammierfehler.

---

## 32. Framebuffer

Framebuffer-Speicher ist nicht ausführbar.

Dies gilt sowohl für:

- Bootloader-Framebuffer,
- Kernel-Grafikpuffer,
- Shared Graphics Buffers,
- GPU-Mappings.

Ein Framebuffer darf nicht aufgrund seiner großen zusammenhängenden Speicherabbildung als Codebereich missbraucht werden.

---

## 33. Gemeinsam genutzter Speicher

Shared Memory ist standardmäßig nicht ausführbar.

Soll ein ausführbares Codeobjekt zwischen Prozessen geteilt werden, muss es über einen kontrollierten Loader- oder Codeobjektpfad erzeugt werden.

Schreibende und ausführende Parteien werden getrennt autorisiert.

Ein Prozess darf keine beschreibbare Shared-Memory-Seite erzeugen und sie unkontrolliert in einem privilegierteren Prozess ausführbar abbilden lassen.

---

## 34. Dateibasierte Mappings

Dateibasierte Mappings sind standardmäßig NX.

Ausführungsrechte werden nur vergeben, wenn:

- die Datei als ausführbares Artefakt geöffnet wurde,
- der Prozess entsprechende Rechte besitzt,
- Mount- und Dateisystempolicy Ausführung erlaubt,
- Integritäts- und Signaturregeln erfüllt sind,
- keine W^X-Verletzung entsteht.

Eine Datei auf einem `noexec`-Dateisystem darf nicht ausführbar gemappt werden.

---

## 35. Anonymer Speicher

Anonymer Speicher ist standardmäßig nicht ausführbar.

Ausführbare anonyme Mappings sind nur über:

- JIT-Codeobjekte,
- kontrollierte Loaderpfade,
- spezielle Compatibility Services

zulässig.

Ein allgemeiner `mmap`-ähnlicher Aufruf mit `WRITE | EXECUTE` wird abgelehnt.

---

## 36. Copy-on-Write

Copy-on-Write darf keine W^X-Verletzung erzeugen.

Wird eine ausführbare, schreibgeschützte Seite durch eine Schreibanforderung kopiert, erhält die neue private Seite zunächst:

```text
Read + Write
Execute verboten
```

Sie wird nicht automatisch ausführbar.

Eine spätere Ausführungsfreigabe benötigt einen neuen kontrollierten Finalisierungsschritt.

---

## 37. Seitentabellen

Seitentabellen selbst sind nicht ausführbar.

Sie sollen außerdem nur über kontrollierte Kernelpfade beschreibbar sein.

NovaOS verhindert, dass normale Kernelkomponenten:

- beliebige Page Table Entries verändern,
- NX ohne Policyprüfung entfernen,
- RWX-Seiten erzeugen,
- Seitentabellen als allgemeinen Speicher verwenden.

Änderungen erfolgen über die virtuelle Speicherverwaltung.

---

## 38. Direkte physische Speicherabbildung

Die Direct Map des physischen Speichers ist standardmäßig NX.

Andernfalls könnten physische Seiten, die über ihre normale Kernelabbildung nicht ausführbar sind, über ein ausführbares Direct-Map-Alias umgangen werden.

Für Kernelcode gilt deshalb:

- ausführbare Hauptabbildung,
- kein ausführbares beschreibbares Direct-Map-Alias.

Codephysikseiten müssen in der Direct Map entweder:

- NX und nicht für Codeausführung verwendbar oder
- vollständig aus der allgemeinen Direct Map entfernt

sein.

---

## 39. Aliasabbildungen

W^X wird pro physischer Seite und nicht nur pro einzelner virtueller Adresse betrachtet.

Es ist nicht ausreichend, eine Seite so abzubilden:

```text
virtuelle Adresse A:
    Read + Execute

virtuelle Adresse B:
    Read + Write
```

wenn beide Ansichten gleichzeitig unkontrolliert aktiv sind.

Der Kernel führt deshalb eine Alias- und Codeobjektverwaltung für ausführbare physische Seiten.

---

## 40. Temporäre Kernelmappings

Temporäre Mappings sind standardmäßig NX.

Auch wenn sie physischen Speicher abbilden, der an anderer Stelle Code enthält, erhalten sie nicht automatisch Ausführungsrechte.

Ausführbare temporäre Mappings benötigen eine interne, streng begrenzte API und müssen nach Gebrauch sofort entfernt werden.

---

## 41. Bootloader

Bereits der Bootloader soll, soweit der aktuelle CPU-Modus dies unterstützt, Code- und Datenbereiche unterscheiden.

Während sehr früher Bootphasen können vorübergehend gröbere Rechte gelten.

Der Bootloader muss diese Übergangszustände dokumentieren und so früh wie möglich reduzieren.

Vor dem Kernel-Handoff sollen mindestens folgende Bereiche getrennt sein:

- Bootloadercode,
- Bootloaderdaten,
- Kernelcode,
- Kernelrodata,
- Kerneldata,
- Stack,
- Handoff-Daten.

---

## 42. Früher Kernelstart

Während des frühen Kernelstarts kann der Kernel zunächst mit temporären Seitentabellen arbeiten.

Der Übergang lautet:

```text
minimale Bootmappings
        ↓
CPU-Fähigkeiten erkennen
        ↓
NX aktivieren
        ↓
endgültige Seitentabellen aufbauen
        ↓
Code- und Datenrechte trennen
        ↓
temporäre breite Mappings entfernen
        ↓
weitere CPUs starten
```

Normale Userspace-Prozesse dürfen erst gestartet werden, nachdem die endgültige NX-Policy aktiv ist.

---

## 43. 32-Bit-x86-Unterstützung

Auf 32-Bit-x86 benötigt hardwarebasiertes NX typischerweise PAE-Paging.

NovaOS prüft:

- CPUID-Unterstützung für PAE,
- CPUID-Unterstützung für NX,
- Aktivierbarkeit von NXE,
- kompatible Seitentabellen,
- korrekte physische Adressbreite.

Ist PAE und NX verfügbar, verwendet der 32-Bit-Kernel PAE-Seitentabellen zur Durchsetzung von NX.

---

## 44. 32-Bit ohne NX

Unterstützt eine 32-Bit-CPU kein NX, kann NovaOS keine vollständige per-page Ausführungsverbotsgarantie bereitstellen.

Mögliche Reaktionen sind:

| Policy | Verhalten |
|---|---|
| `REQUIRE_NX` | normaler Start wird abgebrochen |
| `DEGRADED` | Start mit deutlich gekennzeichnetem reduziertem Schutz |
| `RECOVERY_ONLY` | nur eingeschränktes Recovery-System starten |
| `LEGACY_MODE` | kontrollierter Altgerätemodus |

Der allgemeine Kompatibilitätsstandard für ältere Geräte ist `DEGRADED`, sofern die Gerätesicherheitsrichtlinie NX nicht zwingend voraussetzt.

---

## 45. Segmentbasierter Fallback

Auf bestimmten 32-Bit-x86-Systemen können Segmentierungsmechanismen begrenzte Code- und Datentrennung unterstützen.

Dieser Fallback:

- ist kein vollständiger Ersatz für per-page NX,
- schützt nicht alle Kernelmappings,
- ist mit einem flachen Speichermodell nur eingeschränkt vereinbar,
- wird nicht als vollständiges NX gemeldet.

NovaOS kann solche Mechanismen ergänzend verwenden, kennzeichnet das System jedoch weiterhin als NX-degradiert.

---

## 46. 64-Bit-x86

Auf x86-64 ist NX Bestandteil des vorgesehenen Seitentabellenmodells, sofern die CPU die entsprechende Fähigkeit bereitstellt.

NovaOS aktiviert NXE vor der Freigabe normaler Kernel- und Userspace-Ausführung.

Ein 64-Bit-Produktionssystem ohne funktionsfähiges NX wird standardmäßig nicht als vollständig sicher unterstützt.

---

## 47. ARM und andere Architekturen

Auf ARM verwendet NovaOS je nach Architektur:

- XN,
- PXN,
- UXN,
- entsprechende Seitentabellenattribute.

Andere Architekturen werden über eine generische VM-Rechtesemantik eingebunden:

```text
NOVA_VM_EXECUTE
NOVA_VM_USER_EXECUTE
NOVA_VM_KERNEL_EXECUTE
```

Die Architekturkomponente muss dokumentieren, welche Trennung tatsächlich durch Hardware erzwungen wird.

---

## 48. Benutzer- und Kernelausführung

Wenn die Hardware getrennte Ausführungsrechte für Kernel und Userspace unterstützt, verwendet NovaOS diese.

Beispiele:

- Userspace-Code darf im Userspace ausführbar sein,
- Kernel darf Userspace-Code nicht unbeabsichtigt als privilegierten Code ausführen,
- Kernelcode darf niemals im Userspace ausführbar sein.

Diese Trennung ergänzt klassische User/Supervisor-Seitenrechte.

---

## 49. Kernel darf Userspace-Speicher nicht ausführen

Der Kernel führt keinen Code direkt aus einem Userspace-Mapping aus.

Systemaufrufe übergeben Daten und Befehlsparameter, aber keine direkt im Kernelmodus auszuführenden Funktionszeiger.

Callbacks aus Userspace werden durch:

- IPC,
- Ereignisse,
- Rückkehr in den Userspace,
- kontrollierte Upcalls

realisiert.

Eine Userspace-Adresse darf nicht als Kernel-Instruction-Pointer verwendet werden.

---

## 50. Cache-Synchronisation

Auf Architekturen mit getrennten Daten- und Instruktionscaches muss die Codefinalisierung beide Caches synchronisieren.

Der Ablauf umfasst je nach Architektur:

- Daten-Cache bereinigen,
- Instruktions-Cache invalidieren,
- Speicherbarrieren,
- Instruction Synchronization Barrier,
- TLB-Aktualisierung.

Code darf erst nach abgeschlossener Synchronisation ausgeführt werden.

---

## 51. Mehrkernsysteme

Änderungen von Ausführungsrechten müssen auf allen betroffenen CPUs sichtbar werden.

Die virtuelle Speicherverwaltung muss:

- Page Table Entries aktualisieren,
- Speicherbarrieren ausführen,
- TLB Shootdowns senden,
- Instruktionscaches synchronisieren,
- aktive Codeausführung koordinieren.

Ein Kern darf Code nicht weiter ausführen, nachdem dessen Ausführungsrechte global entfernt wurden.

---

## 52. Echtzeitprozesse

NX bleibt auch für Echtzeitprozesse aktiv.

Codefinalisierung und TLB-Synchronisation können unvorhersehbare Latenzen erzeugen und sind innerhalb harter Echtzeitabschnitte standardmäßig nicht zulässig.

Echtzeitcode muss:

- vor Eintritt in den Echtzeitabschnitt geladen,
- verifiziert,
- finalisiert,
- in Caches vorbereitet

werden.

Dynamische JIT-Neugenerierung innerhalb eines harten Echtzeitabschnitts ist nicht zulässig.

---

## 53. Virtualisierung

Unter einem Hypervisor verwendet NovaOS sowohl Gastseitentabellen als auch verfügbare Second-Level-Translation-Rechte.

Der Hypervisor kann zusätzliche NX- beziehungsweise Execute-Rechte erzwingen.

NovaOS darf sich jedoch nicht ausschließlich auf den Hypervisor verlassen und setzt seine eigene Seitentabellenpolicy um.

Virtuelle Maschinen, die NovaOS selbst hostet, erhalten eigene getrennte Ausführungsrichtlinien.

---

## 54. Hibernate und Resume

Beim Wiederaufnehmen aus einem Hibernate-Image werden sämtliche Seitenrechte erneut validiert.

Das Image darf keine manipulierten RWX-Mappings wiederherstellen.

Vor der Wiederaufnahme normaler Threads prüft der Kernel mindestens:

- NX-Aktivierung,
- W^X-Invarianten,
- Kerneltextrechte,
- Modulrechte,
- Stackrechte,
- Codeobjektgenerationen.

Ein inkompatibler CPU-Wechsel kann die Wiederaufnahme verhindern.

---

## 55. Live-Updates

Live-Updates müssen W^X einhalten.

Neue Codegenerationen werden:

1. in NX-Speicher geladen,
2. verifiziert,
3. relokiert,
4. finalisiert,
5. atomar veröffentlicht.

Alte Codegenerationen verlieren zuerst ihre Erreichbarkeit und anschließend ihre Ausführungsrechte, bevor sie freigegeben werden.

Ein Live-Update darf keine dauerhafte RWX-Patchzone hinterlassen.

---

## 56. Debugger

Ein Kerneldebugger kann kontrollierte Codepatches benötigen.

Im Produktionsmodus gelten:

- spezielle Debug-Capability,
- expliziter Debugzustand,
- zeitlich begrenzte Schreibphase,
- keine gleichzeitige Schreib- und Ausführungsfreigabe,
- Cache- und TLB-Synchronisation,
- vollständige lokale Telemetrie.

Ein Debugger darf W^X nicht durch ein dauerhaftes schreibbares Alias umgehen.

---

## 57. Breakpoints

Software-Breakpoints verändern ausführbaren Code.

Der sichere Ablauf lautet:

```text
betroffene Ausführung stoppen
        ↓
Ausführungsrecht entfernen
        ↓
kontrollierte Schreibabbildung herstellen
        ↓
Breakpoint schreiben
        ↓
Schreibabbildung entfernen
        ↓
Ausführungsrecht wiederherstellen
        ↓
Caches und TLB synchronisieren
        ↓
Ausführung fortsetzen
```

Hardware-Breakpoints werden bevorzugt, wenn sie verfügbar und ausreichend sind.

---

## 58. Security State

Der NX-Status wird im Systemsicherheitszustand dokumentiert.

Mögliche Flags:

```text
NX_SUPPORTED
NX_ENABLED
KERNEL_WX_ENFORCED
USER_WX_ENFORCED
JIT_POLICY_ACTIVE
MODULE_WX_ENFORCED
DIRECT_MAP_NX
NX_DEGRADED
NX_UNAVAILABLE
```

Der Status muss die tatsächlich durch Hardware erzwungenen Eigenschaften widerspiegeln.

---

## 59. API-Grundmodell

Eine mögliche generische Mapping-API lautet:

```c
typedef enum
{
    NOVA_VM_NONE         = 0,
    NOVA_VM_READ         = 1u << 0,
    NOVA_VM_WRITE        = 1u << 1,
    NOVA_VM_EXECUTE      = 1u << 2,
    NOVA_VM_USER         = 1u << 3,
    NOVA_VM_GLOBAL       = 1u << 4,
    NOVA_VM_DEVICE       = 1u << 5,
    NOVA_VM_NO_CACHE     = 1u << 6
} nova_vm_protection_t;

nova_status_t nova_vm_protect(
    nova_handle_t address_space,
    uintptr_t virtual_address,
    size_t size,
    nova_vm_protection_t protection
);
```

Die virtuelle Speicherverwaltung lehnt `WRITE | EXECUTE` standardmäßig ab.

---

## 60. Codeobjekt-API

Für JIT und kontrollierte dynamische Codeerzeugung wird eine gesonderte API verwendet:

```c
nova_status_t nova_code_object_create(
    const nova_code_object_create_info_t* create_info,
    nova_handle_t* code_object
);

nova_status_t nova_code_object_map_writer(
    nova_handle_t code_object,
    nova_handle_t address_space,
    void** writable_address
);

nova_status_t nova_code_object_finalize(
    nova_handle_t code_object
);

nova_status_t nova_code_object_map_executor(
    nova_handle_t code_object,
    nova_handle_t address_space,
    const void** executable_address
);

nova_status_t nova_code_object_revoke(
    nova_handle_t code_object
);
```

Ein Codeobjekt besitzt einen expliziten Lebenszyklus.

---

## 61. Codeobjekt-Zustände

Ein Codeobjekt kann folgende Zustände durchlaufen:

```text
CREATED
   ↓
WRITABLE
   ↓
VALIDATING
   ↓
FINALIZED
   ↓
EXECUTABLE
   ↓
REVOKED
   ↓
RELEASED
```

| Zustand | Bedeutung |
|---|---|
| `CREATED` | Objekt wurde erzeugt |
| `WRITABLE` | Code kann geschrieben werden, ist aber NX |
| `VALIDATING` | Finalisierungsprüfungen laufen |
| `FINALIZED` | Inhalt ist unveränderlich festgelegt |
| `EXECUTABLE` | RX-Abbildungen sind zulässig |
| `REVOKED` | neue und bestehende Ausführung wird beendet |
| `RELEASED` | Speicher wurde freigegeben |

Ein Codeobjekt darf nicht gleichzeitig in `WRITABLE` und `EXECUTABLE` sein.

---

## 62. Capability-Modell

Mögliche Capabilities sind:

```text
CAP_MEMORY_EXECUTE
CAP_MEMORY_JIT
CAP_CODE_OBJECT_CREATE
CAP_CODE_OBJECT_FINALIZE
CAP_KERNEL_MODULE_LOAD
CAP_KERNEL_TEXT_PATCH
CAP_DEBUG_KERNEL
CAP_SECURITY_WX_ADMIN
```

Capabilities können begrenzt werden auf:

- einen bestimmten Prozess,
- einen bestimmten Adressraum,
- maximale Codegröße,
- bestimmte Codeobjekte,
- bestimmte Dateien,
- bestimmte Sicherheitsdomänen,
- zeitliche Gültigkeit.

Eine Capability ersetzt keine W^X- und Integritätsprüfung.

---

## 63. Telemetrie und Diagnose

NovaOS erfasst mindestens:

- NX-Unterstützung der CPU,
- aktivierter NX-Modus,
- W^X-Status für Kernel und Userspace,
- Anzahl ausführbarer Mappings,
- Anzahl von Codeobjekten,
- abgelehnte RWX-Anforderungen,
- abgelehnte Execute-Anforderungen,
- Execute Faults,
- Modulfinalisierungen,
- JIT-Finalisierungen,
- temporäre privilegierte Codepatches,
- degradierter Hardwarezustand.

Konkrete Kerneladressen werden in normaler Telemetrie verborgen.

---

## 64. Execute Faults

Ein Execute Fault wird anhand des betroffenen Privilegienbereichs behandelt.

### 64.1 Userspace Execute Fault

Versucht ein Prozess Code aus einer NX-Seite auszuführen:

1. Exception erfassen,
2. Adresse gegen den Prozessadressraum prüfen,
3. Sicherheitsereignis erzeugen,
4. Prozess nach Exceptionpolicy benachrichtigen oder beenden.

### 64.2 Kernel Execute Fault

Versucht der Kernel Code aus einer NX-Seite auszuführen, liegt ein schwerer Kernel- oder Sicherheitsfehler vor.

Standardreaktion:

- sicheren Diagnosezustand herstellen,
- Crash-Kontext sichern,
- Kernel-Panic auslösen,
- Recovery-Policy anwenden.

---

## 65. Fehlercodes

Mögliche Status- und Fehlercodes sind:

```text
NOVA_STATUS_OK
NOVA_STATUS_EXECUTE_NOT_ALLOWED
NOVA_STATUS_WX_VIOLATION
NOVA_STATUS_NX_UNSUPPORTED
NOVA_STATUS_NX_NOT_ENABLED
NOVA_STATUS_CODE_OBJECT_NOT_FINALIZED
NOVA_STATUS_CODE_OBJECT_ALREADY_EXECUTABLE
NOVA_STATUS_CODE_OBJECT_REVOKED
NOVA_STATUS_EXECUTABLE_MAPPING_DENIED
NOVA_STATUS_MODULE_FINALIZATION_FAILED
NOVA_STATUS_CACHE_SYNC_FAILED
NOVA_STATUS_TLB_SYNC_FAILED
NOVA_STATUS_SECURITY_POLICY_CONFLICT
```

Eine ungültige Userspace-Anforderung darf keinen Kernel-Panic auslösen.

---

## 66. Verhalten auf Hardware ohne NX

Fehlt hardwarebasierter Ausführungsschutz, muss NovaOS:

- den Zustand eindeutig erkennen,
- ihn im Trust State dokumentieren,
- sicherheitskritische Funktionen gegebenenfalls einschränken,
- Anwendungen über reduzierte Garantien informieren,
- eine Gerätepolicy anwenden.

Mögliche Einschränkungen:

- keine hochsensiblen Sicherheitsdomänen,
- kein unprivilegiertes JIT,
- strengere Modulpolicy,
- reduzierte Netzwerkdienste,
- Warnung im Sicherheitszentrum,
- Recovery- oder Legacy-Modus.

NovaOS darf softwareseitige Markierungen ohne Hardwaredurchsetzung nicht als vollständiges NX ausweisen.

---

## 67. Performance

NX selbst verursacht auf moderner Hardware gewöhnlich nur geringe laufende Kosten.

Zusätzliche Kosten entstehen vor allem durch:

- TLB-Synchronisation bei Rechteänderungen,
- Cache-Synchronisation bei dynamischem Code,
- getrennte Codeobjektverwaltung,
- W^X-Prüfungen,
- Modul- und JIT-Finalisierung.

NovaOS akzeptiert diese Kosten als notwendige Sicherheitsmaßnahme.

Ausführungsrechte sollen selten geändert werden. Code wird bevorzugt einmal geladen und anschließend dauerhaft finalisiert.

---

## 68. Konsequenzen

### 68.1 Positive Konsequenzen

- Stack-, Heap- und Datenpuffer sind nicht ausführbar,
- Code-Injection-Angriffe werden erschwert,
- Kernel- und Userspace-W^X wird verbindlich,
- sichere Modul- und JIT-Lebenszyklen,
- DMA- und MMIO-Bereiche bleiben NX,
- klare Hardware- und Legacy-Erkennung,
- gute Integration mit KASLR und Stack Protector,
- Diagnose unerlaubter Ausführungsversuche.

### 68.2 Negative Konsequenzen

- 32-Bit-x86 benötigt PAE für vollständiges NX,
- sehr alte Hardware bietet nur reduzierten Schutz,
- JIT-Compiler benötigen spezielle APIs,
- Debugger und Live-Patching werden komplexer,
- Rechteänderungen benötigen TLB- und Cache-Synchronisation,
- Loader und Modulverwaltung benötigen strenge Segmenttrennung.

### 68.3 Risiken

- unbeabsichtigte RWX-Aliasabbildungen,
- ausführbare Direct Map,
- fehlende Cache-Synchronisation,
- fehlerhafte PAE- oder NXE-Aktivierung,
- unsicherer Legacy-Fallback,
- dauerhaft schreibbare Kerneltext-Aliase,
- JIT-Race-Conditions,
- falsche Seitenrechte nach Hibernate oder Live-Update.

Diese Risiken werden begrenzt durch:

- zentrale Mapping-APIs,
- physische Aliasverfolgung,
- Codeobjekte,
- atomare Finalisierung,
- TLB- und Cache-Shootdowns,
- Trust-State-Prüfung,
- umfangreiche W^X-Invariantentests.

---

## 69. Auswirkungen auf andere Subsysteme

### 69.1 Bootloader

Der Bootloader muss Code- und Datenbereiche soweit möglich getrennt abbilden und NX-Fähigkeiten übergeben.

### 69.2 Secure Kernel Startup

NX-Unterstützung und W^X-Status werden Teil des Boot Trust State.

### 69.3 KASLR

Randomisierte Bereiche erhalten zusätzlich ihren jeweiligen minimalen Seitenrechtssatz.

### 69.4 Stack Protector

Stack Protector und NX ergänzen sich beim Schutz gegen stackbasierte Angriffe.

### 69.5 Virtuelle Speicherverwaltung

Die VM-Schicht setzt Default-NX, W^X, Aliasprüfung und Rechteänderungen durch.

### 69.6 Kernel Stack Layout

Sämtliche Kernelstacks werden NX abgebildet.

### 69.7 DMA-Verwaltung

DMA-Puffer und ihre CPU-Mappings erhalten niemals Ausführungsrechte.

### 69.8 Kernelmodulsystem

Module werden zunächst NX geladen und erst nach Prüfung als RX finalisiert.

### 69.9 Programmloader

Der Loader validiert Segmente und verhindert W^X-Überlappungen.

### 69.10 JIT-Laufzeiten

JITs verwenden kontrollierte Codeobjekte und Finalisierungs-APIs.

### 69.11 Debugger und Live-Update

Codepatches benötigen atomare W^X-konforme Übergänge.

### 69.12 Telemetrie

Execute Faults und abgelehnte Rechteänderungen werden lokal diagnostizierbar.

---

## 70. Implementierungsrichtlinien

1. Alle neuen Mappings sind standardmäßig NX.
2. Ausführungsrechte müssen ausdrücklich angefordert werden.
3. `WRITE | EXECUTE` wird durch die zentrale VM-Schicht abgelehnt.
4. Kerneltext wird nach Relokation als RX abgebildet.
5. Kernel-Rodata wird read-only und NX abgebildet.
6. Kerneldata, Heap und Stacks werden RW und NX abgebildet.
7. DMA-, MMIO- und Framebufferbereiche bleiben immer NX.
8. Kernelmodule werden in NX-Speicher geladen und erst nach Finalisierung ausführbar.
9. JITs verwenden Codeobjekte statt allgemeiner RWX-Mappings.
10. W^X wird auch über virtuelle Aliasabbildungen derselben physischen Seite durchgesetzt.
11. Die Direct Map darf kein ausführbares Alias beschreibbarer Codeseiten bereitstellen.
12. 32-Bit-x86 verwendet PAE, wenn dies für NX erforderlich und hardwareseitig verfügbar ist.
13. Fehlendes NX wird eindeutig als degradierter Sicherheitszustand gemeldet.
14. Andere CPUs werden erst nach Aktivierung der endgültigen Seitenrechte gestartet.
15. Rechteänderungen führen zu erforderlicher TLB- und Cache-Synchronisation.
16. Kerneldebugger und Live-Updates verwenden keine dauerhaften RWX-Patchbereiche.
17. Hibernate-Wiederaufnahme validiert sämtliche Ausführungsrechte erneut.
18. Der Build- und Testprozess prüft die endgültigen Segmentrechte des Kernelimages.
19. Userspace kann keine ausführbaren Mappings ohne Policy- und Capability-Prüfung erzeugen.
20. Vollständiges NX wird nur gemeldet, wenn die Hardware es tatsächlich erzwingt.

---

## 71. Testanforderungen

Mindestens folgende Szenarien müssen getestet werden:

- NX-Fähigkeitserkennung,
- PAE-Erkennung auf 32-Bit-x86,
- NXE-Aktivierung,
- Start auf NX-fähiger Hardware,
- Start auf Hardware ohne NX,
- `REQUIRE_NX`-Policy,
- degradierter Legacy-Modus,
- Ausführung aus Kernelstack,
- Ausführung aus Userspace-Stack,
- Ausführung aus Kernelheap,
- Ausführung aus Userspace-Heap,
- Ausführung aus DMA-Puffer,
- Ausführung aus MMIO,
- Ausführung aus Framebuffer,
- Kerneltext ist nicht schreibbar,
- Kernel-Rodata ist nicht schreibbar und nicht ausführbar,
- Kerneldata ist NX,
- RWX-Mapping wird abgelehnt,
- RWX-Aliasabbildung wird erkannt,
- Direct Map ist NX,
- gültiger Programmloader,
- W^X-Segmentüberlappung,
- gültiges Kernelmodul,
- Modulrelokation in NX-Speicher,
- Modulfinalisierung zu RX,
- Modulentladung und Execute-Revoke,
- JIT-Codeobjekt,
- JIT-Finalisierung,
- JIT-Neubearbeitung,
- parallele Writer und Executor,
- Copy-on-Write einer Codeseite,
- Shared-Memory-Codeobjekt,
- `noexec`-Dateisystem,
- anonymer ausführbarer Speicher,
- TLB Shootdown,
- Instruktionscache-Synchronisation,
- Mehrkern-Codefinalisierung,
- Echtzeitprozess mit vorbereitetem Code,
- Hibernate-Wiederaufnahme,
- Live-Update,
- Software-Breakpoint,
- Kernel Execute Fault,
- Userspace Execute Fault,
- KASLR und NX gemeinsam,
- Stack Protector und NX gemeinsam,
- Virtualisierung,
- ARM-XN-Backend,
- Fuzzing von Mapping- und Loaderrechten.

---

## 72. Nicht Bestandteil dieser Entscheidung

Diese ADR legt nicht abschließend fest:

- die vollständige PAE-Speicherarchitektur,
- konkrete x86-, ARM- oder RISC-V-Seitentabellenbits,
- vollständige Control-Flow Integrity,
- Shadow Stacks,
- Kernelheap-Hardening,
- die genaue JIT-Codevalidierung,
- vollständige Modul-Signaturregeln,
- Memory Protection Keys,
- die endgültige Codeobjekt-ABI,
- konkrete Einschränkungen des Legacy-Modus.

Diese Details werden in nachfolgenden ADRs und NPSPEC-Dokumenten definiert.

---

## 73. Verbindliche Invarianten

1. Alle neuen virtuellen Speicherabbildungen sind standardmäßig nicht ausführbar.
2. Ausführungsrechte werden ausschließlich ausdrücklich vergeben.
3. Eine Seite darf im normalen Produktionsbetrieb nicht gleichzeitig schreibbar und ausführbar sein.
4. W^X gilt auch über Aliasabbildungen derselben physischen Seite.
5. Kerneltext ist nach der Initialisierung ausführbar, aber nicht schreibbar.
6. Kernel-Rodata ist weder schreibbar noch ausführbar.
7. Kerneldata, Kernelheap und Kernelstacks sind nicht ausführbar.
8. Userspace-Heaps und Userspace-Stacks sind standardmäßig nicht ausführbar.
9. DMA-, MMIO- und Framebufferbereiche sind niemals ausführbar.
10. Seitentabellen sind nicht ausführbar.
11. Die Direct Map darf W^X und NX nicht über ein Alias umgehen.
12. Kernelmodule werden vor der Ausführung vollständig verifiziert und finalisiert.
13. JIT-Code wird zuerst als NX geschrieben und anschließend als nicht beschreibbares RX finalisiert.
14. Ein Codeobjekt darf nicht gleichzeitig beschreibbar und ausführbar sein.
15. Rechteänderungen werden auf allen betroffenen CPUs durch TLB- und Cache-Synchronisation sichtbar gemacht.
16. Der Kernel führt keinen Code direkt aus Userspace-Speicher im Kernelmodus aus.
17. Ein Kernel Execute Fault führt nicht zu einem normalen Weiterbetrieb.
18. Fehlende Hardware-NX-Unterstützung muss ausdrücklich als degradierter Zustand sichtbar sein.
19. Softwaremarkierungen ohne Hardwaredurchsetzung dürfen nicht als vollständiges NX bezeichnet werden.
20. Eine Capability ersetzt keine W^X-, Integritäts- oder Loaderprüfung.
21. Debugger und Live-Updates dürfen keine dauerhaften RWX-Mappings erzeugen.
22. Hibernate und Kernelwechsel müssen die Ausführungsrechte erneut validieren.
23. NX ersetzt weder KASLR, Stack Protector, Guard Pages, W^X noch Speichersicherheit.
24. KI- oder lernbasierte Komponenten dürfen NX-, W^X-, JIT-, Modul-, Debug-, Capability- oder Secure-Startup-Regeln nicht umgehen.

---

## 74. Referenzen

- ADR-SEC-0001 – Secure Kernel Startup
- ADR-SEC-0002 – Kernel ASLR
- ADR-SEC-0003 – Stack Protector
- ADR-MM-0002 – Virtual Memory Management
- ADR-MM-0013 – Guard Pages
- ADR-MM-0014 – DMA Memory
- ADR-MM-0015 – Kernel Stack Layout
- ADR-PROC-0002 – Thread Lifecycle
- ADR-PROC-0005 – Echtzeitprozesse
- ADR-PROC-0008 – Context Switching
- ADR-KERNEL-0103 – Native Capability-basierte Ressourcenverwaltung
- ADR-KERNEL-0104 – Integrierte Telemetrie- und Diagnoseschnittstellen
- ADR-KERNEL-0105 – ABI-Stabilität über versionierte Kernel-Services
- NPSPEC-NXMEMORY-0001 – NX Memory Specification
- NPSPEC-SECURESTARTUP-0001 – Secure Kernel Startup Specification
- NPSPEC-KASLR-0001 – Kernel ASLR Specification
- NPSPEC-STACKPROTECTOR-0001 – Stack Protector Specification
- NPSPEC-VIRTUALMEMORY-0001 – Virtual Memory Specification
- NPSPEC-KERNELMODULE-0001 – Kernel Module Specification
- NPSPEC-CODEOBJECT-0001 – Executable Code Object Specification

---

## 75. Zusammenfassung der Entscheidung

NovaOS verwendet Default-NX und setzt W^X als verbindliche Speicherschutzregel durch.

Alle neuen Speicherabbildungen sind standardmäßig nicht ausführbar. Ausführungsrechte müssen ausdrücklich angefordert und durch die virtuelle Speicherverwaltung, das Capability-System und die aktive Sicherheitsrichtlinie geprüft werden.

Kerneltext und finalisierter Modulcode werden als `Read + Execute` abgebildet. Kernel-Rodata ist schreibgeschützt und nicht ausführbar. Kerneldata, Heap, Stacks, DMA-Puffer, MMIO und Framebuffer sind nicht ausführbar.

Eine physische Seite darf auch über mehrere virtuelle Aliasabbildungen nicht gleichzeitig beschreibbar und ausführbar sein. Die Direct Map darf W^X daher nicht durch ein beschreibbares Alias von Codeseiten umgehen.

Kernelmodule und JIT-Code werden zunächst in beschreibbaren NX-Speicher geladen oder erzeugt. Erst nach Prüfung, Cache-Synchronisation und Entfernung der Schreibrechte werden sie ausführbar.

Der anfängliche 32-Bit-x86-Kernel verwendet PAE, sofern dies für NX notwendig und von der Hardware unterstützt wird. Auf älterer Hardware ohne NX kann NovaOS in einem deutlich gekennzeichneten degradierten Modus starten. Dieser Zustand darf nicht als vollständiger NX-Schutz dargestellt werden.

NX ergänzt KASLR, Stack Protector, Guard Pages, W^X und weitere Kontrollflussschutzmechanismen, ersetzt diese aber nicht.# ADR-SEC-0005 – SMEP/SMAP

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-SEC-0005 |
| Titel | Supervisor Mode Execution Prevention und Supervisor Mode Access Prevention |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Sicherheit / Kernel / Speicherzugriff / CPU-Schutzfunktionen |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-SEC-0001, ADR-SEC-0002, ADR-SEC-0003, ADR-SEC-0004, ADR-MM-0002, ADR-PROC-0008 |
| Nachfolgende Spezifikation | NPSPEC-SMEP-SMAP-0001 |

---

## 1. Kontext

NovaOS trennt den privilegierten Kerneladressraum vom nicht privilegierten Userspace.

Trotz dieser Trennung muss der Kernel regelmäßig Daten mit Userspace-Prozessen austauschen.

Beispiele:

- Systemaufrufparameter lesen,
- Ergebnisse in Userspace-Puffer schreiben,
- Zeichenketten aus Userspace übernehmen,
- IPC-Daten kopieren,
- Dateisystemdaten übertragen,
- Netzwerkpakete bereitstellen,
- Prozesskontexte und Debugdaten verarbeiten.

Ohne zusätzliche Hardwaremechanismen kann Kernelcode versehentlich oder durch einen Angriff:

- Code aus einer Userspace-Seite im Kernelmodus ausführen,
- Userspace-Daten direkt dereferenzieren,
- manipulierte Userspace-Zeiger verwenden,
- während eines privilegierten Vorgangs auf ausgetauschte Daten zugreifen,
- eine kompromittierte Funktionsadresse in den Userspace umleiten.

x86-Prozessoren stellen dafür zwei Schutzfunktionen bereit:

- SMEP – Supervisor Mode Execution Prevention,
- SMAP – Supervisor Mode Access Prevention.

SMEP verhindert im Supervisor Mode die Ausführung von Code aus Userspace-Seiten.

SMAP verhindert im Supervisor Mode standardmäßig das Lesen und Schreiben von Userspace-Seiten.

Andere CPU-Architekturen besitzen vergleichbare Mechanismen, beispielsweise:

- PXN – Privileged Execute Never,
- PAN – Privileged Access Never,
- UAO – User Access Override,
- entsprechende privilegierte Zugriffskontrollen.

NovaOS benötigt ein einheitliches Architekturmodell, das diese Funktionen nutzt und alle Kernelzugriffe auf Userspace-Speicher über kontrollierte Schnittstellen führt.

---

## 2. Problemstellung

Der Kernel darf Userspace-Speicher nicht wie normalen Kernel-Speicher behandeln.

Ein Userspace-Zeiger kann:

- auf eine nicht gemappte Seite zeigen,
- außerhalb des erlaubten Adressbereichs liegen,
- während der Operation geändert werden,
- auf einen schreibgeschützten Bereich zeigen,
- mit einem anderen Mapping überlappen,
- einen Seitenfehler auslösen,
- kurz vor dem Zugriff ungültig werden,
- absichtlich auf Kerneladressen gesetzt werden,
- eine Integerüberlaufprüfung umgehen.

Eine einmalige Zeigerprüfung reicht nicht aus.

Zwischen Prüfung und Nutzung kann sich der Userspace-Adressraum ändern. Außerdem muss der Kernel manche Seitenfehler beim Kopieren kontrolliert behandeln können.

SMEP und SMAP müssen daher mit:

- der virtuellen Speicherverwaltung,
- Systemaufrufen,
- Exception Handling,
- Context Switching,
- Interrupts,
- CPU-Hotplug,
- Debugging,
- Echtzeitpfaden

zusammenarbeiten.

---

## 3. Entscheidungsziele

Die SMEP/SMAP-Architektur soll:

1. Kernelcodeausführung aus Userspace-Speicher verhindern,
2. direkte Kernelzugriffe auf Userspace-Daten standardmäßig verhindern,
3. kontrollierte Copy-in- und Copy-out-Schnittstellen bereitstellen,
4. ungültige Userspace-Zeiger sicher behandeln,
5. Zugriffsfaults ohne Kernelkompromittierung abfangen,
6. Mehrkern- und Context-Switch-Semantik unterstützen,
7. Interrupt- und Exceptionpfade absichern,
8. TOCTOU-Risiken begrenzen,
9. architekturübergreifende Schutzmechanismen abstrahieren,
10. fehlende Hardwareunterstützung klar kennzeichnen,
11. Diagnose ermöglichen, ohne Kerneladressen offenzulegen,
12. mit NX, KASLR und Capability-Isolation zusammenarbeiten.

---

## 4. Begriffsdefinitionen

### 4.1 Supervisor Mode

Supervisor Mode ist der privilegierte CPU-Ausführungsmodus des Kernels.

---

### 4.2 Userspace-Seite

Eine Userspace-Seite ist eine virtuelle Speicherseite, die durch die Seitentabellen als für nicht privilegierte Ausführung zugänglich markiert ist.

---

### 4.3 SMEP

SMEP verhindert die Ausführung von Instruktionen aus Userspace-Seiten, während die CPU im Supervisor Mode läuft.

---

### 4.4 SMAP

SMAP verhindert standardmäßig Datenzugriffe des Kernels auf Userspace-Seiten.

Kontrollierte Zugriffsfenster müssen ausdrücklich geöffnet und anschließend wieder geschlossen werden.

---

### 4.5 User Access Window

Ein User Access Window ist ein kurzer, kontrollierter Kernelabschnitt, in dem ein definierter Zugriff auf Userspace-Speicher erlaubt wird.

---

### 4.6 Copy-in

Copy-in überträgt Daten aus Userspace-Speicher in einen Kernelpuffer.

---

### 4.7 Copy-out

Copy-out überträgt Daten aus einem Kernelpuffer in Userspace-Speicher.

---

### 4.8 Uaccess

Uaccess bezeichnet die Gesamtheit der kontrollierten Kernelmechanismen zum Zugriff auf Userspace-Speicher.

---

### 4.9 Fault Fixup

Ein Fault Fixup ist ein kontrollierter Wiederherstellungspfad für einen erwartbaren Speicherfehler während eines Uaccess-Vorgangs.

---

## 5. Bedrohungsmodell

SMEP erschwert insbesondere:

- Kernelkontrollfluss in eine Userspace-Seite,
- Ausführung von durch den Angreifer bereitgestelltem Userspace-Code im Kernelmodus,
- bestimmte Return-to-User- und Code-Injection-Angriffe.

SMAP erschwert insbesondere:

- unbeabsichtigte Dereferenzierung von Userspace-Zeigern,
- Confused-Deputy-Angriffe über manipulierte Userspace-Puffer,
- Ausnutzung fehlerhafter Kernelpointerprüfungen,
- direkte Nutzung von Userspace-Daten als vertrauenswürdige Kernelstrukturen.

SMEP und SMAP schützen nicht vollständig gegen:

- Code-Reuse innerhalb des Kernels,
- gültige Kernelpointer auf manipulierte Kerneldaten,
- Fehler innerhalb bewusst geöffneter Uaccess-Fenster,
- TOCTOU-Probleme bei mehrfacher Datennutzung,
- Hardware ohne entsprechende Schutzfunktionen,
- kompromittierte Seitentabellen,
- physische Angriffe,
- logische Berechtigungsfehler.

---

## 6. Betrachtete Alternativen

### 6.1 Keine Hardwaredurchsetzung

Der Kernel prüft Userspace-Zeiger ausschließlich per Software.

#### Vorteile

- funktioniert auf jeder Hardware,
- einfache frühe Implementierung,
- keine architekturspezifischen Kontrollregister.

#### Nachteile

- versehentliche direkte Zugriffe bleiben möglich,
- Kernelcode kann in Userspace umgeleitet werden,
- Softwareprüfungen können vergessen werden,
- geringere Defense-in-Depth.

Diese Alternative wird für unterstützte Hardware verworfen.

---

### 6.2 Nur SMEP

Der Kernel verhindert Userspace-Codeausführung, erlaubt aber weiterhin direkte Datenzugriffe.

#### Vorteile

- Schutz gegen Kernelkontrollfluss in Userspace-Code,
- geringere Komplexität als vollständiges SMAP.

#### Nachteile

- manipulierte Userspace-Zeiger bleiben gefährlich,
- direkte Dereferenzierungen werden nicht verhindert,
- unzureichender Schutz für Systemaufrufpfade.

Diese Alternative wird als mögliche Zwischenstufe, aber nicht als Zielarchitektur akzeptiert.

---

### 6.3 Nur SMAP

Der Kernel verhindert direkte Datenzugriffe, aber nicht die Ausführung von Userspace-Code.

#### Vorteile

- starke Trennung von Kernel- und Userspace-Datenzugriffen,
- erzwingt kontrollierte Copy-Schnittstellen.

#### Nachteile

- Kernelkontrollfluss könnte weiterhin in Userspace-Speicher gelangen,
- unvollständiger Schutz.

Diese Alternative wird verworfen.

---

### 6.4 SMEP und SMAP mit kontrollierter Uaccess-API

NovaOS aktiviert beide Mechanismen und erlaubt Userspace-Datenzugriffe ausschließlich über kleine kontrollierte Zugriffsfenster.

#### Vorteile

- starke Hardwaredurchsetzung,
- verhindert Kernelcodeausführung aus Userspace-Speicher,
- erzwingt zentrale Copy- und Validierungspfade,
- gut diagnostizierbare Verstöße,
- architekturübergreifend abstrahierbar.

#### Nachteile

- höhere Komplexität,
- Uaccess-Fault-Recovery notwendig,
- Interrupt- und Context-Switch-Regeln müssen exakt sein,
- ältere Hardware benötigt einen degradierten Modus.

Diese Alternative wird gewählt.

---

## 7. Entscheidung

NovaOS aktiviert SMEP und SMAP beziehungsweise architektonisch gleichwertige Mechanismen, sobald:

- die CPU sie unterstützt,
- die Seitentabellen korrekt eingerichtet sind,
- die Exceptionpfade bereitstehen,
- die Uaccess-API initialisiert ist.

Im normalen Kernelbetrieb gilt:

```text
Kernel darf Userspace-Code nicht ausführen.
Kernel darf Userspace-Daten nicht direkt lesen oder schreiben.
```

Zugriffe auf Userspace-Daten erfolgen ausschließlich über kontrollierte Uaccess-Funktionen.

---

## 8. Schutzmodell

NovaOS trennt vier grundlegende Fälle:

| Operation | Standard |
|---|---|
| Kernel führt Kernelcode aus | erlaubt |
| Kernel führt Userspace-Code aus | verboten |
| Kernel greift auf Kerneldata zu | erlaubt |
| Kernel greift direkt auf Userspace-Daten zu | verboten |
| kontrollierter Uaccess | zeitlich begrenzt erlaubt |
| Userspace führt Kernelcode aus | durch Seitentabellen verboten |
| Userspace greift auf Kerneldata zu | durch Seitentabellen verboten |

SMEP und SMAP ergänzen die normalen User/Supervisor-Seitenrechte.

---

## 9. SMEP-Aktivierung

Auf x86 wird SMEP aktiviert, wenn:

- CPUID die Fähigkeit meldet,
- Paging aktiv ist,
- Kernel- und Userspace-Seiten korrekt markiert sind,
- frühe Übergangsseiten überprüft wurden.

Die Aktivierung erfolgt über das architekturspezifische Kontrollregister.

Konzeptionell:

```c
if (nova_cpu_has_feature(NOVA_CPU_FEATURE_SMEP))
{
    nova_arch_enable_smep();
}
```

Die generische Sicherheitsschicht verwendet keine direkten x86-Kontrollregister.

---

## 10. SMAP-Aktivierung

SMAP wird aktiviert, nachdem die Uaccess- und Fault-Fixup-Infrastruktur einsatzbereit ist.

Konzeptionell:

```c
if (nova_cpu_has_feature(NOVA_CPU_FEATURE_SMAP))
{
    nova_arch_enable_smap();
}
```

Nach Aktivierung sind Userspace-Datenzugriffe aus dem Kernel standardmäßig gesperrt.

Eine versehentliche direkte Dereferenzierung löst einen Schutzfehler aus.

---

## 11. Aktivierungsreihenfolge

Die sichere Aktivierungsreihenfolge lautet:

```text
CPU-Fähigkeiten erkennen
        ↓
Kernel- und Userseiten korrekt markieren
        ↓
Exceptiontabellen initialisieren
        ↓
Uaccess-Fault-Fixups bereitstellen
        ↓
SMEP aktivieren
        ↓
SMAP aktivieren
        ↓
Schutzstatus prüfen
        ↓
normalen Userspace starten
```

Userspace-Prozesse dürfen nicht gestartet werden, bevor das aktive Schutzmodell festgelegt wurde.

---

## 12. Aktivierung auf allen CPUs

SMEP und SMAP sind CPU-lokale Hardwarezustände.

Jede neu gestartete CPU muss die Schutzfunktionen unabhängig aktivieren.

Eine CPU darf erst als online und schedulerfähig gelten, wenn:

- die erforderlichen Seitentabellen aktiv sind,
- SMEP entsprechend der Systempolicy gesetzt ist,
- SMAP entsprechend der Systempolicy gesetzt ist,
- die CPU-lokale Uaccess-Infrastruktur bereitsteht.

---

## 13. Einheitliche Systempolicy

NovaOS verhindert einen unkontrollierten Mischbetrieb, bei dem einige CPUs SMEP/SMAP verwenden und andere nicht, sofern Threads zwischen diesen CPUs migrieren können.

Mögliche Policies:

| Policy | Verhalten |
|---|---|
| `REQUIRE_UNIFORM` | nur CPUs mit erforderlichen Funktionen werden online geschaltet |
| `RESTRICT_CPUS` | sicherheitskritische Threads laufen nur auf geschützten CPUs |
| `SYSTEM_DEGRADED` | gesamtes System wird als degradiert markiert |
| `DISABLE_GLOBALLY` | Funktion wird systemweit deaktiviert |

Der Produktionsstandard ist `REQUIRE_UNIFORM`, soweit die Plattform dies unterstützt.

---

## 14. Architekturabstraktion

NovaOS definiert eine generische Privileged-Access-Schutzschicht.

Mögliche Architekturfunktionen:

```c
typedef struct nova_arch_user_access_ops
{
    nova_status_t (*initialize_cpu)(
        nova_cpu_t* cpu
    );

    void (*enable_execution_protection)(void);
    void (*enable_access_protection)(void);

    void (*begin_user_access)(void);
    void (*end_user_access)(void);

    bool (*is_user_access_enabled)(void);
    bool (*is_execution_protection_enabled)(void);
    bool (*is_access_protection_enabled)(void);
} nova_arch_user_access_ops_t;
```

Auf x86 werden diese Operationen durch SMEP, SMAP, `STAC` und `CLAC` umgesetzt.

Auf ARM können PXN, PAN und vergleichbare Mechanismen verwendet werden.

---

## 15. Uaccess-Grundregel

Kernelcode darf Userspace-Zeiger nicht direkt dereferenzieren.

Nicht zulässig:

```c
void syscall_write(const char* user_buffer, size_t size)
{
    kernel_output(user_buffer, size);
}
```

Zulässig:

```c
void syscall_write(const char* user_buffer, size_t size)
{
    char kernel_buffer[256];

    nova_status_t status = nova_copy_from_user(
        kernel_buffer,
        user_buffer,
        size
    );

    if (status != NOVA_STATUS_OK)
    {
        return;
    }

    kernel_output(kernel_buffer, size);
}
```

---

## 16. Uaccess-API

NovaOS stellt mindestens folgende Funktionen bereit:

```c
nova_status_t nova_copy_from_user(
    void* kernel_destination,
    const void* user_source,
    size_t size
);

nova_status_t nova_copy_to_user(
    void* user_destination,
    const void* kernel_source,
    size_t size
);

nova_status_t nova_clear_user(
    void* user_destination,
    size_t size
);

nova_status_t nova_copy_string_from_user(
    char* kernel_destination,
    size_t kernel_capacity,
    const char* user_source,
    size_t* copied_size
);

nova_status_t nova_user_range_validate(
    const void* user_address,
    size_t size,
    nova_user_access_t access
);
```

Die endgültige ABI wird in NPSPEC-SMEP-SMAP-0001 definiert.

---

## 17. Copy-in

`nova_copy_from_user()`:

1. validiert Adresse und Größe,
2. prüft Integerüberläufe,
3. prüft die Userspace-Adressgrenze,
4. öffnet ein kontrolliertes Lesezugriffsfenster,
5. kopiert die Daten,
6. behandelt erwartbare Seitenfehler,
7. schließt das Zugriffsfenster,
8. gibt den Status zurück.

Der Kernel verwendet anschließend ausschließlich die Kernelkopie.

---

## 18. Copy-out

`nova_copy_to_user()`:

1. validiert Zieladresse und Größe,
2. prüft Schreibberechtigung,
3. öffnet ein kontrolliertes Schreibzugriffsfenster,
4. kopiert die Daten,
5. behandelt erwartbare Seitenfehler,
6. schließt das Zugriffsfenster,
7. gibt den Status zurück.

Sensible Kernelpuffer müssen vor dem Copy-out vollständig initialisiert sein, damit keine nicht initialisierten Daten in den Userspace gelangen.

---

## 19. Bereichsvalidierung

Eine Userspace-Adresse wird nur als Bereich validiert, wenn:

```text
start liegt im Userspace
end liegt im Userspace
start + size erzeugt keinen Überlauf
gesamter Bereich liegt innerhalb einer zulässigen Grenze
```

Konzeptionell:

```c
bool nova_user_range_is_valid(
    uintptr_t address,
    size_t size
)
{
    if (size == 0)
    {
        return true;
    }

    if (address > NOVA_USER_ADDRESS_MAX)
    {
        return false;
    }

    if (size - 1 > NOVA_USER_ADDRESS_MAX - address)
    {
        return false;
    }

    return true;
}
```

Diese Prüfung ersetzt nicht den tatsächlichen zugriffsgeschützten Kopiervorgang.

---

## 20. Time-of-Check-to-Time-of-Use

Eine erfolgreiche Bereichsprüfung garantiert nicht, dass der Speicher beim späteren Zugriff unverändert bleibt.

Der Userspace kann möglicherweise:

- das Mapping ändern,
- Seiten freigeben,
- Schutzrechte ändern,
- Daten parallel verändern,
- einen anderen Thread schreiben lassen.

NovaOS verwendet deshalb die Regel:

> Userspace-Daten werden einmal kontrolliert in Kernelspeicher kopiert und anschließend dort validiert und verarbeitet.

Mehrfaches Nachlesen sicherheitsrelevanter Strukturen aus Userspace-Speicher ist zu vermeiden.

---

## 21. Verschachtelte Zeiger

Enthält eine kopierte Userspace-Struktur weitere Zeiger, werden diese nicht automatisch vertrauenswürdig.

Der Kernel muss:

1. äußere Struktur kopieren,
2. Feldgrößen validieren,
3. enthaltene Userspace-Zeiger einzeln prüfen,
4. referenzierte Daten separat kopieren,
5. eine vollständige Kernelrepräsentation erzeugen.

Beispiel:

```text
Userspace-Struktur
├── Länge
├── Flags
└── Zeiger auf Daten

Kernel:
1. Struktur kopieren
2. Länge und Flags prüfen
3. Daten über Zeiger getrennt kopieren
4. nur Kernelkopie weiterverwenden
```

---

## 22. Zeichenketten

Userspace-Zeichenketten werden immer mit einer maximalen Länge kopiert.

`nova_copy_string_from_user()` muss:

- einen Terminator innerhalb der Grenze verlangen,
- die Zielkapazität berücksichtigen,
- fehlenden Terminator als Fehler melden,
- partielle Ergebnisse klar behandeln,
- keine unbeschränkte Suche im Userspace durchführen.

Möglicher Fehler:

```text
NOVA_STATUS_STRING_TOO_LONG
```

---

## 23. Große Datenmengen

Große Datenmengen müssen nicht zwingend vollständig auf dem Kernelstack zwischengespeichert werden.

Zulässige Strategien:

- begrenzte Kernelheap-Puffer,
- gepinnte Seiten,
- kontrollierte Scatter/Gather-Listen,
- seitenweise Kopie,
- Shared-Memory-Objekte,
- asynchrone I/O-Objekte.

Auch diese Mechanismen müssen die Uaccess- und Capability-Regeln einhalten.

---

## 24. Gepinnte Userspace-Seiten

Für DMA oder asynchrone I/O können Userspace-Seiten vorübergehend gepinnt werden.

Der Ablauf muss:

1. Bereich validieren,
2. Seiten auflösen,
3. Zugriffsrechte prüfen,
4. Seiten referenzieren,
5. Änderungen oder Freigabe kontrollieren,
6. IOMMU- und DMA-Regeln anwenden,
7. Seiten nach Abschluss freigeben.

Das Pinnen erlaubt dem Kernel nicht, beliebige Userspace-Zeiger außerhalb kontrollierter APIs direkt zu verwenden.

---

## 25. User Access Window

Ein User Access Window ist immer:

- so kurz wie möglich,
- auf eine konkrete Operation begrenzt,
- nicht über allgemeine Funktionsaufrufe hinweg offen,
- nicht über einen Context Switch hinweg offen,
- nicht über eine Rückkehr in Userspace hinweg offen,
- nicht unbegrenzt verschachtelt.

Der bevorzugte Aufbau lautet:

```c
nova_user_access_begin();

result = nova_arch_copy_user(...);

nova_user_access_end();
```

Direkte Nutzung dieser Low-Level-Funktionen ist nur innerhalb der Uaccess-Implementierung zulässig.

---

## 26. x86-STAC/CLAC

Auf x86 verwendet NovaOS bei aktiviertem SMAP:

```text
STAC → kontrollierten Userspace-Datenzugriff erlauben
CLAC → Userspace-Datenzugriff wieder sperren
```

Die Sequenz muss sicherstellen, dass `CLAC` in allen Kontrollpfaden ausgeführt wird:

- Erfolg,
- Fehler,
- Seitenfehler,
- Exception Fixup,
- frühe Rückkehr.

Ein offenes Access-Flag darf nicht versehentlich bestehen bleiben.

---

## 27. Keine offenen Uaccess-Fenster bei Context Switch

Ein Thread darf keinen Context Switch ausführen, während sein User Access Window geöffnet ist.

Vor einem Schedulerwechsel muss gelten:

```text
user_access_depth == 0
access_override_disabled == true
```

Ein Verstoß stellt eine Kernelinvariante dar.

Blockierende Operationen innerhalb eines geöffneten Uaccess-Fensters sind verboten.

---

## 28. Präemption

Während eines Low-Level-Uaccess-Abschnitts kann Präemption vorübergehend deaktiviert werden, wenn die Architektur den Zugriffszustand CPU-lokal und nicht threadbezogen verwaltet.

Der Abschnitt muss sehr kurz bleiben.

Alternativ kann der Zugriffszustand im Threadkontext gespeichert und bei jedem Context Switch sicher behandelt werden. NovaOS bevorzugt jedoch:

```text
kein Context Switch bei offenem Uaccess-Fenster
```

Dadurch bleibt die Sicherheitssemantik einfacher und besser überprüfbar.

---

## 29. Interrupts

Ein Interrupt kann während eines Uaccess-Vorgangs auftreten.

Der Interrupt-Entry-Pfad muss sicherstellen, dass normaler Interruptcode nicht unbeabsichtigt mit geöffnetem Userspace-Zugriff läuft.

Auf unterstützten Architekturen gilt:

1. Zugriffszustand beim Eintritt sichern,
2. Userspace-Zugriff für den Interruptpfad sperren,
3. Interrupt behandeln,
4. vorherigen Zustand nur für den kontrollierten Uaccess-Pfad wiederherstellen.

Ein Interrupt-Handler darf nicht davon ausgehen, dass SMAP bereits gesperrt war.

---

## 30. Exceptions

Eine Exception während eines Uaccess-Vorgangs kann erwartet oder unerwartet sein.

Erwartbar sind beispielsweise:

- Seite nicht vorhanden,
- Userspace-Ziel schreibgeschützt,
- Mapping während der Kopie verändert.

Solche Fehler können über Fault Fixups in einen kontrollierten Status umgewandelt werden.

Unerwartete Kernelzugriffe außerhalb eines Uaccess-Bereichs werden als Kernel- oder Sicherheitsfehler behandelt.

---

## 31. Fault-Fixup-Tabellen

NovaOS verwendet architekturspezifische Exception- beziehungsweise Fixup-Tabellen.

Ein Eintrag verbindet:

- fehleranfällige Uaccess-Instruktion,
- sicheren Fortsetzungspunkt,
- zurückzugebenden Fehlerstatus.

Konzeptionell:

```text
fault_instruction → fixup_instruction
```

Die Tabellen:

- sind schreibgeschützt,
- werden durch Secure Startup verifiziert,
- enthalten nur kontrollierte Uaccess-Bereiche,
- dürfen nicht beliebige Kernel-Faults verschlucken.

---

## 32. Seitenfehler während Uaccess

Ein Seitenfehler während Uaccess kann abhängig vom Kontext:

- eine Seite regulär einlagern,
- Copy-on-Write auflösen,
- kontrolliert fehlschlagen,
- einen Teiltransfer melden.

Nicht zulässig ist eine unbeschränkte Fehlerbehandlung in:

- Interruptkontext,
- atomaren Kernelabschnitten,
- gehaltenen Spinlocks,
- harten Echtzeitabschnitten.

Die Uaccess-API muss angeben, ob sie Fehlerbehandlung mit potenzieller Blockierung erlaubt.

---

## 33. Atomare Uaccess-Varianten

NovaOS kann nicht blockierende Uaccess-Varianten bereitstellen.

Beispiel:

```c
nova_status_t nova_copy_from_user_atomic(
    void* kernel_destination,
    const void* user_source,
    size_t size
);
```

Diese Variante:

- löst keine blockierenden Seitenfehler auf,
- gibt bei nicht sofort zugreifbarem Speicher einen Fehler zurück,
- ist für kurze atomare Kernelpfade vorgesehen,
- darf keine normale Heap-Allokation durchführen.

---

## 34. Echtzeit-Uaccess

In harten Echtzeitabschnitten sind unvorhersehbare Seitenfehler unzulässig.

Userspace-Puffer müssen vorab:

- validiert,
- gepinnt,
- in den Speicher geladen,
- mit geeigneten Zugriffsrechten versehen

werden.

Der Echtzeitpfad verwendet ausschließlich vorab vorbereitete Uaccess-Objekte oder gebundene Shared-Memory-Bereiche.

SMEP und SMAP bleiben dabei aktiv.

---

## 35. Systemaufrufe

Der Systemaufruf-Dispatcher behandelt alle Pointerargumente zunächst als nicht vertrauenswürdige Userspace-Adressen.

Ein Systemaufruf darf:

- keinen Userspace-Zeiger als Kernelzeiger interpretieren,
- keine Userspace-Struktur ohne Kopie weiterreichen,
- keine Userspace-Funktionsadresse im Kernelmodus aufrufen,
- keine unvalidierte Länge verwenden.

Die Systemaufrufimplementierung verwendet ausschließlich die Uaccess-API.

---

## 36. IPC

IPC-Nachrichten werden entweder:

- in Kernelpuffer kopiert,
- über kontrollierte Shared-Memory-Objekte übertragen,
- über gepinnte Seiten beschrieben.

Ein IPC-Dienst darf keine rohen Zeiger aus dem Adressraum eines anderen Prozesses direkt dereferenzieren.

Prozessübergreifende Speicherübertragung erfolgt über explizite Speicherobjekte und Capabilities.

---

## 37. Kernelthreads

Kernelthreads besitzen keinen normalen Userspace-Adressraum.

Sie dürfen standardmäßig keine Uaccess-Operationen ausführen.

Benötigt ein Kernelthread Zugriff auf den Adressraum eines Prozesses, muss er:

- eine explizite Adressraumreferenz besitzen,
- die erforderliche Capability besitzen,
- den Zieladressraum kontrolliert aktivieren oder über Remote-Copy-APIs zugreifen,
- dieselben Uaccess-Regeln einhalten.

Der zuletzt auf der CPU aktive Userspace-Adressraum darf nicht stillschweigend weiterverwendet werden.

---

## 38. Context Switching

Beim Context Switch wird geprüft:

```text
user_access_depth == 0
SMAP override ist deaktiviert
kein offener Fault-Fixup-Zustand
```

Der Zielthread startet immer mit gesperrtem privilegiertem Userspace-Datenzugriff.

SMEP bleibt dauerhaft aktiv, sobald es auf der CPU eingeschaltet wurde.

Ein Userspace-Zugriffsfenster wird niemals von einem Thread an einen anderen vererbt.

---

## 39. Rückkehr in Userspace

Vor der Rückkehr in den Userspace prüft der Kernel mindestens:

- Uaccess-Fenster geschlossen,
- Access Override deaktiviert,
- SMEP aktiv,
- SMAP beziehungsweise Äquivalent aktiv,
- gültiger Userspace Instruction Pointer,
- gültiger Userspace Stack Pointer,
- Trap Frame validiert.

Ein offenes Uaccess-Fenster verhindert die Rückkehr und löst einen Kernelintegritätsfehler aus.

---

## 40. Kernel-Funktionszeiger

Alle Kernel-Funktionszeiger müssen auf ausführbare Kernelcodebereiche zeigen.

Vor besonders kritischen indirekten Aufrufen kann der Kernel optional prüfen:

```text
target liegt in:
    Kerneltext
    oder finalisiertem Modultext
    oder autorisiertem Codeobjekt
```

Eine Adresse in einer Userspace-Seite ist niemals ein gültiges Kernel-Aufrufziel.

SMEP bildet dafür die Hardware-Rückfallebene.

---

## 41. Upcalls und Callbacks

NovaOS führt Userspace-Callbacks nicht im Kernelmodus aus.

Ein Callback wird realisiert durch:

1. Ereignis im Kernel erzeugen,
2. Userspace-Thread vorbereiten oder aufwecken,
3. validierten Userspace-Kontext setzen,
4. kontrolliert in den Userspace zurückkehren.

Der Kernel springt niemals direkt im Supervisor Mode zu einer Userspace-Adresse.

---

## 42. KASLR-Integration

SMEP verhindert, dass ein Angreifer KASLR umgeht, indem der Kernelkontrollfluss einfach auf bekannten Userspace-Code umgeleitet wird.

KASLR schützt die Position des Kernelcodes.

SMEP stellt sicher, dass Userspace-Speicher kein Ersatz für Kernelcode sein kann.

Beide Mechanismen ergänzen sich, ersetzen aber keine Kontrollflussvalidierung.

---

## 43. NX-Integration

NX und SMEP erfüllen unterschiedliche Aufgaben.

| Mechanismus | Schutz |
|---|---|
| NX | verhindert Ausführung aus als NX markierten Seiten |
| SMEP | verhindert Kernel-Ausführung aus Userspace-Seiten, auch wenn diese im Userspace ausführbar sind |
| SMAP | verhindert Kernel-Datenzugriff auf Userspace-Seiten |
| User/Supervisor-Bit | verhindert Userspace-Zugriff auf Kernelseiten |

Eine Userspace-Codeseite kann für Userspace ausführbar sein und muss dennoch für den Kernel durch SMEP gesperrt bleiben.

---

## 44. Stack Protector

Ein Stack Overflow könnte versuchen, eine Kernel-Rücksprungadresse auf Userspace-Code umzulenken.

Der Stack Protector soll die Überschreibung erkennen.

SMEP verhindert zusätzlich die privilegierte Ausführung des Userspace-Ziels.

Beide Schutzmechanismen werden gemeinsam eingesetzt.

---

## 45. Capability-System

Eine Capability für ein Userspace-Speicherobjekt erlaubt nicht automatisch direkte Kernel-Dereferenzierung.

Capabilities autorisieren:

- welche Operation zulässig ist,
- welcher Bereich betroffen ist,
- welcher Zugriffstyp erlaubt ist.

Der tatsächliche Datentransfer erfolgt weiterhin über Uaccess- oder Shared-Memory-Schnittstellen.

Eine Capability darf SMEP oder SMAP nicht deaktivieren.

---

## 46. Copy statt dauerhafter Zugriff

Für kleine sicherheitsrelevante Strukturen gilt:

> kopieren, validieren, intern verwenden.

Beispiele:

- Systemaufrufparameter,
- Dateipfade,
- Sicherheitsbeschreibungen,
- Capability-Anforderungen,
- Modulmetadaten,
- Netzwerkadressstrukturen.

Dadurch verhindert NovaOS, dass Userspace-Daten nach ihrer Validierung unbemerkt verändert werden.

---

## 47. Shared-Memory-Objekte

Für große oder häufig verwendete Daten kann NovaOS Shared-Memory-Objekte einsetzen.

Der Kernel behandelt solche Objekte nicht automatisch als vertrauenswürdig.

Mögliche Sicherheitsmodelle:

- unveränderlicher Snapshot,
- Producer-/Consumer-Ring mit atomaren Protokollen,
- explizite Eigentumsübergabe,
- versiegelte Seiten,
- generationengesicherte Puffer.

SMAP bleibt für direkte Userspace-Mappings aktiv.

---

## 48. Debugger

Ein Kerneldebugger darf SMEP oder SMAP nicht dauerhaft deaktivieren.

Benötigt er Userspace-Speicherzugriff, verwendet er dieselben kontrollierten Uaccess-Schnittstellen.

Temporäre Debug-Ausnahmen benötigen:

- spezielle Capability,
- aktive Debugpolicy,
- lokale Telemetrie,
- garantierte Wiederaktivierung.

Ein Debugfehler darf keine CPU unbemerkt mit deaktiviertem Schutz zurücklassen.

---

## 49. CPU-Hotplug

Beim Online-Schalten einer CPU wird geprüft:

- SMEP-Unterstützung,
- SMAP-Unterstützung,
- Architekturäquivalente,
- Systempolicy,
- erfolgreiche Aktivierung.

Eine CPU, die die erforderliche Produktionspolicy nicht erfüllt, wird:

- nicht online geschaltet,
- einem eingeschränkten CPU-Pool zugeordnet oder
- führt zu einem degradierten Systemzustand.

Das Verhalten wird vor der Threadmigration festgelegt.

---

## 50. Suspend und Resume

Nach Suspend oder Hibernate müssen SMEP und SMAP auf jeder CPU erneut geprüft und aktiviert werden.

Vor Wiederaufnahme normaler Threads gelten mindestens:

```text
SMEP entsprechend Policy aktiv
SMAP entsprechend Policy aktiv
Uaccess Override geschlossen
Seitentabellen gültig
```

Ein manipuliertes oder inkompatibles Resume darf den Schutz nicht stillschweigend deaktivieren.

---

## 51. Virtualisierung

Unter einem Hypervisor kann SMEP/SMAP:

- direkt virtualisiert,
- emuliert,
- ausgeblendet,
- fehlerhaft implementiert

sein.

NovaOS prüft die tatsächlich sichtbaren CPU-Fähigkeiten und führt Selbsttests aus.

Das System verlässt sich nicht allein auf eine Hypervisorangabe.

Der Trust State kennzeichnet virtualisierte Schutzfunktionen.

---

## 52. Plattform ohne SMEP

Fehlt SMEP, verwendet NovaOS weiterhin:

- NX,
- KASLR,
- Kerneladressprüfung,
- Kontrollflussvalidierung,
- getrennte User/Supervisor-Seiten,
- Stack Protector.

Das System wird jedoch als SMEP-degradiert gekennzeichnet.

Eine Plattformpolicy kann den Produktionsstart ohne SMEP ablehnen.

---

## 53. Plattform ohne SMAP

Fehlt SMAP, bleiben die Uaccess-APIs trotzdem verpflichtend.

Softwareprüfungen umfassen:

- Userspace-Adressgrenzen,
- Zugriffsrechte,
- Exception Fixups,
- Copy-in/Copy-out,
- TOCTOU-Vermeidung.

Der Schutzstatus wird als SMAP-degradiert markiert.

Kernelcode darf fehlende Hardwareunterstützung nicht als Erlaubnis für direkte Userspace-Dereferenzierungen behandeln.

---

## 54. Sicherheitsmodi

Mögliche Policies sind:

| Modus | SMEP | SMAP | Verhalten |
|---|---:|---:|---|
| `ENFORCING` | erforderlich, wenn Plattformprofil dies verlangt | erforderlich, wenn Plattformprofil dies verlangt | Start oder CPU-Online kann abgelehnt werden |
| `STANDARD` | verwenden, wenn verfügbar | verwenden, wenn verfügbar | fehlende Funktionen werden als degradiert markiert |
| `LEGACY` | optional | optional | Software-Uaccess bleibt verpflichtend |
| `DEVELOPMENT` | kontrolliert testbar | kontrolliert testbar | Abweichungen werden deutlich markiert |

Ein stilles automatisches Abschalten nach einem Schutzfehler ist nicht zulässig.

---

## 55. Trust State

Der Sicherheitsstatus enthält mindestens:

```text
SMEP_SUPPORTED
SMEP_ENABLED
SMAP_SUPPORTED
SMAP_ENABLED
UACCESS_ENFORCED
FAULT_FIXUPS_ACTIVE
PRIVILEGED_ACCESS_DEGRADED
VIRTUALIZED_PROTECTION
```

Die Statuswerte werden pro System und gegebenenfalls pro CPU erfasst.

---

## 56. API-Grundmodell

Eine mögliche Uaccess-API lautet:

```c
typedef enum
{
    NOVA_USER_ACCESS_READ  = 1u << 0,
    NOVA_USER_ACCESS_WRITE = 1u << 1
} nova_user_access_t;

nova_status_t nova_user_range_validate(
    const void* user_address,
    size_t size,
    nova_user_access_t access
);

nova_status_t nova_copy_from_user(
    void* kernel_destination,
    const void* user_source,
    size_t size
);

nova_status_t nova_copy_to_user(
    void* user_destination,
    const void* kernel_source,
    size_t size
);

nova_status_t nova_copy_string_from_user(
    char* kernel_destination,
    size_t kernel_capacity,
    const char* user_source,
    size_t* copied_size
);

nova_status_t nova_clear_user(
    void* user_destination,
    size_t size
);
```

Low-Level-Funktionen zum Öffnen eines Zugriffsfeldes werden nicht als allgemeine Kernel-API veröffentlicht.

---

## 57. Uaccess-Ergebnis

Eine Copy-Funktion gibt einen eindeutigen Status und optional die tatsächlich kopierte Größe zurück.

```c
typedef struct
{
    nova_status_t status;
    size_t bytes_completed;
} nova_user_copy_result_t;
```

Mögliche Semantiken:

- vollständig oder Fehler,
- definierter Teiltransfer,
- kein Teiltransfer bei sicherheitskritischen Strukturen.

Die jeweilige API muss ihre Semantik eindeutig dokumentieren.

---

## 58. Kernelinterner Uaccess-Zustand

Jede CPU beziehungsweise jeder aktuelle Thread enthält mindestens:

```c
typedef struct
{
    uint32_t depth;
    uint32_t flags;

    uintptr_t active_start;
    size_t active_size;

    nova_user_access_t access;
    nova_status_t fault_status;
} nova_uaccess_state_t;
```

Die konkrete Implementierung darf kleiner sein, muss aber:

- Verschachtelung erkennen,
- offene Fenster vor Context Switch erkennen,
- Fault Fixups zuordnen,
- Diagnose ermöglichen.

---

## 59. Verschachtelung

Uaccess-Fenster sollen grundsätzlich nicht verschachtelt werden.

Falls eine interne Architekturimplementierung Verschachtelung benötigt, wird ein begrenzter Zähler verwendet.

Es gilt:

```text
begin:
    depth += 1

end:
    depth -= 1

depth == 0:
    Hardwarezugriff wieder vollständig sperren
```

Unterlauf, Überlauf oder Rückkehr mit einem nicht null gesetzten Zähler gilt als Kernelintegritätsfehler.

---

## 60. Compilerbarrieren

Der Compiler darf Speicherzugriffe nicht außerhalb des kontrollierten Uaccess-Fensters verschieben.

`begin_user_access()` und `end_user_access()` benötigen deshalb geeignete:

- Compilerbarrieren,
- Hardwarebarrieren,
- architekturspezifische Instruktionssemantik.

Eine reine Funktionskonvention ohne Barrieren ist nicht ausreichend.

---

## 61. Spekulative Ausführung

SMAP und Bereichsprüfung allein verhindern nicht jede spekulative Seiteneffektmöglichkeit.

Uaccess-Funktionen müssen bei Bedarf:

- Bounds Checks absichern,
- Spekulationsbarrieren verwenden,
- maskierte Indizes einsetzen,
- geheime Kernelwerte nicht abhängig von Userspace-Indizes laden.

Die konkrete Spectre-Hardening-Policy wird in einer gesonderten ADR spezifiziert.

---

## 62. Fehlerbehandlung

Mögliche Fehlercodes sind:

```text
NOVA_STATUS_OK
NOVA_STATUS_INVALID_USER_ADDRESS
NOVA_STATUS_USER_RANGE_OVERFLOW
NOVA_STATUS_USER_READ_DENIED
NOVA_STATUS_USER_WRITE_DENIED
NOVA_STATUS_USER_PAGE_NOT_PRESENT
NOVA_STATUS_USER_COPY_PARTIAL
NOVA_STATUS_USER_ACCESS_FAULT
NOVA_STATUS_USER_STRING_TOO_LONG
NOVA_STATUS_UACCESS_IN_ATOMIC_CONTEXT
NOVA_STATUS_UACCESS_WINDOW_OPEN
NOVA_STATUS_SMEP_UNSUPPORTED
NOVA_STATUS_SMAP_UNSUPPORTED
NOVA_STATUS_PROTECTION_DEGRADED
```

Ungültige Userspace-Eingaben führen nicht zu einer Kernel-Panic.

Eine Kernel-Panic ist jedoch zulässig bei:

- SMEP-Verstoß im Kernel,
- unerwartetem SMAP-Verstoß außerhalb Uaccess,
- offenem Uaccess-Fenster bei Context Switch,
- beschädigter Fixup-Tabelle,
- deaktiviertem Schutz trotz Enforcing Policy.

---

## 63. Verhalten bei SMEP-Verstoß

Versucht der Kernel, Code aus einer Userspace-Seite auszuführen:

1. CPU-Fault erfassen,
2. Instruction Pointer und Seitentyp prüfen,
3. andere CPUs bei Bedarf stoppen,
4. geschützten Crash-Kontext sichern,
5. Sicherheitsereignis erzeugen,
6. Kernel-Panic auslösen.

Ein SMEP-Verstoß ist keine normale Userspace-Exception.

---

## 64. Verhalten bei SMAP-Verstoß

Ein SMAP-Verstoß wird unterschieden nach:

### 64.1 Innerhalb eines gültigen Uaccess-Fixups

Der Zugriff wird kontrolliert abgebrochen und als Uaccess-Fehler zurückgegeben.

### 64.2 Außerhalb eines Uaccess-Bereichs

Es liegt ein Kernelprogrammier- oder Sicherheitsfehler vor.

Standardreaktion:

- Diagnose sichern,
- Kernel-Panic auslösen,
- Recovery-Policy anwenden.

---

## 65. Telemetrie und Diagnose

NovaOS erfasst mindestens:

- SMEP-Unterstützung und Aktivierung,
- SMAP-Unterstützung und Aktivierung,
- Architekturäquivalent,
- Anzahl erfolgreicher Uaccess-Operationen,
- Anzahl fehlgeschlagener Copy-ins,
- Anzahl fehlgeschlagener Copy-outs,
- Anzahl partieller Transfers,
- Anzahl Fault Fixups,
- Anzahl unerwarteter Schutzverletzungen,
- degradierter Schutzstatus,
- CPU-Hotplug-Abweisungen.

Userspace-Adressen werden entsprechend der Datenschutz- und Adressschutzpolicy maskiert.

---

## 66. Performance

SMEP verursacht im normalen Betrieb geringe zusätzliche Kosten.

SMAP erzeugt Kosten durch:

- Öffnen und Schließen kurzer Zugriffsfelder,
- zusätzliche Bereichsprüfungen,
- Copy-in und Copy-out,
- Fault-Fixup-Infrastruktur.

NovaOS akzeptiert diese Kosten für die klare Trennung von Kernel- und Userspace-Speicher.

Optimierungen dürfen erfolgen durch:

- architekturoptimierte Copy-Routinen,
- blockweise Kopie,
- gepinnte Seiten für große Transfers,
- Shared-Memory-Protokolle,
- Vermeidung mehrfacher Kopien.

Optimierungen dürfen Uaccess- und Sicherheitsgrenzen nicht umgehen.

---

## 67. Konsequenzen

### 67.1 Positive Konsequenzen

- Kernel kann keinen Userspace-Code im Supervisor Mode ausführen,
- direkte Userspace-Dereferenzierungen werden verhindert,
- zentrale und überprüfbare Uaccess-Schnittstellen,
- Schutz gegen viele Return-to-User-Angriffe,
- klare Fehlerbehandlung für ungültige Zeiger,
- bessere Trennung von Kernel und Userspace,
- gute Integration mit NX, KASLR und Stack Protector,
- architekturübergreifendes Schutzmodell.

### 67.2 Negative Konsequenzen

- höhere Komplexität der Copy- und Fault-Pfade,
- zusätzliche Laufzeitkosten,
- Interrupt- und Context-Switch-Integration erforderlich,
- ältere Hardware bietet nur degradierten Schutz,
- Treiber und Kernelkomponenten müssen konsequent angepasst werden.

### 67.3 Risiken

- vergessenes Schließen eines Uaccess-Fensters,
- Context Switch mit geöffnetem SMAP-Override,
- Interruptcode läuft mit offenem Zugriff,
- fehlerhafte Fixup-Tabelle,
- direkte Userspace-Dereferenzierung auf Hardware ohne SMAP,
- TOCTOU bei mehrfach gelesenen Strukturen,
- unvollständige CPU-Hotplug-Aktivierung,
- unsichere Debugausnahmen.

Diese Risiken werden begrenzt durch:

- kleine zentrale Uaccess-API,
- automatische Scope-Helfer,
- Context-Switch-Invarianten,
- Interrupt-Entry-Hardening,
- schreibgeschützte Fixup-Tabellen,
- Buildprüfungen,
- Laufzeitselbsttests,
- vollständige Telemetrie.

---

## 68. Auswirkungen auf andere Subsysteme

### 68.1 Virtuelle Speicherverwaltung

Die VM-Schicht muss User/Supervisor-Seiten korrekt markieren und sichere Bereichsprüfungen bereitstellen.

### 68.2 Systemaufrufe

Alle Pointerargumente werden ausschließlich über Uaccess-Funktionen verarbeitet.

### 68.3 Context Switching

Offene Uaccess-Fenster sind bei einem Context Switch verboten.

### 68.4 Interruptsystem

Interrupt-Entry muss privilegierten Userspace-Zugriff unabhängig vom unterbrochenen Zustand sperren.

### 68.5 Exception Handling

Erwartbare Uaccess-Faults benötigen kontrollierte Fixup-Pfade.

### 68.6 Prozessverwaltung

Prozessadressräume müssen klare Userspace-Grenzen und Lebenszyklusregeln besitzen.

### 68.7 IPC

Prozessübergreifende Daten werden kopiert oder über explizite Shared-Memory-Objekte übertragen.

### 68.8 Echtzeitsystem

Echtzeit-Uaccess benötigt vorab gepinnte und validierte Speicherbereiche.

### 68.9 Capability-System

Capabilities autorisieren Speicherobjekte, deaktivieren aber niemals SMEP oder SMAP.

### 68.10 Debugger

Debugger verwenden dieselben kontrollierten Speicherzugriffsschnittstellen.

### 68.11 Secure Startup

Der aktive SMEP/SMAP-Status wird in den System-Trust-State übernommen.

---

## 69. Implementierungsrichtlinien

1. Kernel- und Userspace-Seiten werden eindeutig über Seitentabellenattribute getrennt.
2. SMEP wird aktiviert, sobald sichere Seitentabellen aktiv sind.
3. SMAP wird aktiviert, sobald Uaccess und Fault Fixups bereitstehen.
4. Jede online geschaltete CPU muss die aktive Systempolicy erfüllen.
5. Direkte Userspace-Zeiger-Dereferenzierung ist im Kernel verboten.
6. Alle Systemaufrufpointer werden über Uaccess verarbeitet.
7. Kleine sicherheitsrelevante Strukturen werden einmal in Kernelspeicher kopiert.
8. Uaccess-Fenster bleiben möglichst kurz.
9. Uaccess darf nicht über einen Context Switch hinweg offen bleiben.
10. Normale Interruptpfade laufen mit gesperrtem Userspace-Datenzugriff.
11. Fault-Fixup-Tabellen sind schreibgeschützt und auf Uaccess-Bereiche begrenzt.
12. Kernelthreads verwenden keine impliziten Userspace-Adressräume.
13. Hardware ohne SMAP verwendet weiterhin verpflichtende Software-Uaccess-APIs.
14. Fehlende Hardwarefunktionen werden im Trust State sichtbar gemacht.
15. Ein SMEP-Verstoß führt zu einer Kernel-Panic.
16. Ein unerwarteter SMAP-Verstoß außerhalb von Uaccess führt zu einer Kernel-Panic.
17. Compiler- und Speicherbarrieren verhindern das Verschieben von Zugriffen aus dem Uaccess-Fenster.
18. Debug- und Development-Ausnahmen dürfen den Schutz nicht dauerhaft deaktivieren.
19. Suspend und CPU-Hotplug prüfen die Schutzfunktionen erneut.
20. Uaccess-Code wird durch statische Analyse, Fuzzing und Laufzeittests besonders geprüft.

---

## 70. Testanforderungen

Mindestens folgende Szenarien müssen getestet werden:

- SMEP-Fähigkeitserkennung,
- SMAP-Fähigkeitserkennung,
- Aktivierung auf der Boot-CPU,
- Aktivierung auf zusätzlichen CPUs,
- CPU-Hotplug ohne erforderliche Funktionen,
- Kernelversuch, Userspace-Code auszuführen,
- direkter Kernelzugriff auf Userspace-Daten,
- gültiger `copy_from_user`,
- gültiger `copy_to_user`,
- ungültige Userspace-Adresse,
- Kerneladresse als Userspace-Zeiger,
- Bereichsüberlauf,
- nicht gemappte Seite,
- schreibgeschützte Zielseite,
- Mappingänderung während Copy-in,
- Mappingänderung während Copy-out,
- partielle Kopie,
- begrenzte Zeichenkette,
- fehlender Stringterminator,
- verschachtelte Pointerstruktur,
- großer gepinnter Puffer,
- atomarer Uaccess,
- Uaccess in hartem Echtzeitabschnitt,
- Page Fault mit gültigem Fixup,
- Page Fault ohne Fixup,
- beschädigte Fixup-Tabelle,
- Interrupt während Uaccess,
- verschachtelter Interrupt während Uaccess,
- Context-Switch-Versuch mit offenem Fenster,
- Rückkehr in Userspace mit offenem Fenster,
- Kernelthread ohne Userspace-Adressraum,
- CPU-Migration,
- Suspend und Resume,
- Hibernate,
- Virtualisierung,
- Hardware ohne SMEP,
- Hardware ohne SMAP,
- Legacy-Modus,
- Debuggerzugriff,
- NX und SMEP gemeinsam,
- Stack Protector und SMEP gemeinsam,
- TOCTOU-Fuzzing,
- Systemaufruf-Fuzzing,
- Telemetrie und Fault-Zähler.

---

## 71. Nicht Bestandteil dieser Entscheidung

Diese ADR legt nicht abschließend fest:

- die konkrete x86-Assembly der Uaccess-Routinen,
- die vollständige ARM-PAN/PXN-Implementierung,
- Spectre- und Meltdown-Mitigationen,
- vollständige Control-Flow Integrity,
- die genaue Seitentabellenarchitektur,
- Prozessübergreifende Remote-Copy-APIs,
- vollständige Shared-Memory-Protokolle,
- die endgültige Uaccess-ABI,
- konkrete CPU-Pool-Policies für heterogene Schutzfunktionen.

Diese Details werden in nachfolgenden ADRs und NPSPEC-Dokumenten definiert.

---

## 72. Verbindliche Invarianten

1. Kernelcode darf niemals im Supervisor Mode aus Userspace-Seiten ausgeführt werden.
2. Direkte Kernelzugriffe auf Userspace-Daten sind standardmäßig verboten.
3. Userspace-Datenzugriffe erfolgen ausschließlich über kontrollierte Uaccess-Schnittstellen.
4. SMEP bleibt nach seiner Aktivierung während des normalen Kernelbetriebs aktiv.
5. SMAP beziehungsweise sein Architekturäquivalent bleibt außerhalb kurzer Uaccess-Fenster aktiv.
6. Ein Uaccess-Fenster darf nicht über einen Context Switch hinweg offen bleiben.
7. Ein Uaccess-Fenster darf nicht über eine Rückkehr in den Userspace hinweg offen bleiben.
8. Normaler Interruptcode darf nicht unbeabsichtigt mit geöffnetem Userspace-Zugriff laufen.
9. Jeder Userspace-Bereich wird auf Grenzen und Integerüberläufe geprüft.
10. Eine erfolgreiche Bereichsprüfung ersetzt nicht den faultgeschützten Zugriff.
11. Sicherheitsrelevante Userspace-Strukturen werden nach Möglichkeit einmal kopiert und anschließend als Kernelkopie verarbeitet.
12. Verschachtelte Userspace-Zeiger werden einzeln validiert und kopiert.
13. Fault Fixups dürfen nur bekannte Uaccess-Instruktionen behandeln.
14. Kernelthreads dürfen nicht implizit den Userspace-Adressraum des zuvor laufenden Threads verwenden.
15. Ein Kernel-Funktionszeiger darf niemals auf eine Userspace-Seite zeigen.
16. Userspace-Callbacks werden nur durch kontrollierte Rückkehr in den Userspace ausgeführt.
17. Eine Capability darf SMEP, SMAP oder die Uaccess-Prüfung nicht umgehen.
18. Fehlende Hardwareunterstützung muss im Sicherheitsstatus sichtbar sein.
19. Software-Uaccess-Regeln bleiben auch ohne SMAP verpflichtend.
20. Ein SMEP-Verstoß erlaubt keinen normalen Kernelweiterbetrieb.
21. Ein unerwarteter SMAP-Verstoß außerhalb eines gültigen Uaccess-Pfads erlaubt keinen normalen Kernelweiterbetrieb.
22. Jede online geschaltete CPU muss den für sie gemeldeten Schutz tatsächlich aktiviert haben.
23. SMEP/SMAP ersetzen weder NX, KASLR, Stack Protector, Capability-Prüfung noch Speichersicherheit.
24. KI- oder lernbasierte Komponenten dürfen SMEP-, SMAP-, Uaccess-, Fault-, Capability-, Debug- oder Secure-Startup-Regeln nicht umgehen.

---

## 73. Referenzen

- ADR-SEC-0001 – Secure Kernel Startup
- ADR-SEC-0002 – Kernel ASLR
- ADR-SEC-0003 – Stack Protector
- ADR-SEC-0004 – NX Memory
- ADR-MM-0002 – Virtual Memory Management
- ADR-MM-0013 – Guard Pages
- ADR-MM-0015 – Kernel Stack Layout
- ADR-PROC-0001 – Process Lifecycle
- ADR-PROC-0002 – Thread Lifecycle
- ADR-PROC-0005 – Echtzeitprozesse
- ADR-PROC-0007 – CPU Affinity
- ADR-PROC-0008 – Context Switching
- ADR-KERNEL-0103 – Native Capability-basierte Ressourcenverwaltung
- ADR-KERNEL-0104 – Integrierte Telemetrie- und Diagnoseschnittstellen
- ADR-KERNEL-0105 – ABI-Stabilität über versionierte Kernel-Services
- NPSPEC-SMEP-SMAP-0001 – SMEP/SMAP Specification
- NPSPEC-NXMEMORY-0001 – NX Memory Specification
- NPSPEC-VIRTUALMEMORY-0001 – Virtual Memory Specification
- NPSPEC-UACCESS-0001 – Userspace Access Specification
- NPSPEC-SYSCALL-0001 – System Call Specification
- NPSPEC-EXCEPTION-0001 – Exception Handling Specification

---

## 74. Zusammenfassung der Entscheidung

NovaOS aktiviert SMEP und SMAP beziehungsweise architektonisch gleichwertige Schutzmechanismen, sobald Seitentabellen, Exception Handling und Uaccess-Infrastruktur bereitstehen.

SMEP verhindert, dass der Kernel im privilegierten CPU-Modus Code aus Userspace-Seiten ausführt. SMAP verhindert, dass der Kernel Userspace-Daten außerhalb eines ausdrücklich geöffneten Zugriffsfeldes liest oder schreibt.

Kernelkomponenten dürfen Userspace-Zeiger nicht direkt dereferenzieren. Alle Datenübertragungen erfolgen über kontrollierte Funktionen wie:

- `nova_copy_from_user()`,
- `nova_copy_to_user()`,
- `nova_copy_string_from_user()`,
- `nova_clear_user()`.

Uaccess-Fenster bleiben kurz, dürfen keinen Context Switch überdauern und werden in Interruptpfaden standardmäßig geschlossen. Erwartbare Seitenfehler werden ausschließlich über schreibgeschützte Fault-Fixup-Tabellen behandelt.

Fehlt SMEP oder SMAP auf älterer Hardware, bleiben die Software-Uaccess-Regeln vollständig verbindlich. Das System wird jedoch als sicherheitstechnisch degradiert gekennzeichnet.

SMEP, SMAP, NX, KASLR, Stack Protector, User/Supervisor-Seitenrechte und das Capability-System bilden gemeinsam eine mehrschichtige Trennung zwischen Kernel und Userspace.