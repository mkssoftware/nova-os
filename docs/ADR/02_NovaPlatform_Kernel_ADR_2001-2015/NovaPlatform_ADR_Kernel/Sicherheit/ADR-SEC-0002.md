# ADR-SEC-0002 – Kernel ASLR

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-SEC-0002 |
| Titel | Kernel Address Space Layout Randomization |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-30 |
| Bereich | Sicherheit / Kernel / Speicherverwaltung / Boot |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-SEC-0001, ADR-MM-0001, ADR-MM-0002, ADR-MM-0013, ADR-MM-0015 |
| Nachfolgende Spezifikation | NPSPEC-KASLR-0001 |

---

## 1. Kontext

Der NovaOS-Kernel wird in einem privilegierten virtuellen Adressraum ausgeführt.

Besitzt der Kernel bei jedem Start dasselbe Speicherlayout, kann ein Angreifer bekannte Adressen für folgende Angriffe verwenden:

- Return-Oriented Programming,
- Jump-Oriented Programming,
- Wiederverwendung vorhandener Kernelinstruktionen,
- Überschreiben von Funktionszeigern,
- Manipulation globaler Kernelobjekte,
- gezielte Ausnutzung von Speicherfehlern,
- Umgehung einzelner Kontrollflussmechanismen.

Kernel Address Space Layout Randomization, kurz KASLR, verändert die virtuellen Adressen wichtiger Kernelbereiche bei jedem Systemstart.

Dadurch kann ein Angreifer nicht mehr zuverlässig voraussetzen, dass sich:

- Kernelcode,
- Kerneldaten,
- Kernelmodule,
- Kernelstacks,
- Kernelheap,
- direkte physische Speicherabbildungen,
- CPU-lokale Daten,
- spezielle Sicherheitsbereiche

immer an denselben virtuellen Adressen befinden.

KASLR verhindert Speicherfehler nicht. Es erhöht jedoch den Aufwand, einen vorhandenen Speicherfehler zuverlässig auszunutzen.

---

## 2. Problemstellung

NovaOS benötigt ein KASLR-Modell, das:

- bereits während des Kernelstarts aktiv ist,
- mit BIOS und UEFI funktioniert,
- das eigene Boot-Handoff-Protokoll unterstützt,
- keine ungültigen Speicherüberlappungen erzeugt,
- mit Kernelmodulen zusammenarbeitet,
- Crash-Dumps weiterhin auswertbar macht,
- auf Mehrkern- und NUMA-Systemen funktioniert,
- sowohl 32-Bit- als auch 64-Bit-Ausbaustufen berücksichtigt,
- auch ohne Hardware-Zufallszahlengenerator einen definierten Zustand erreicht.

Die erste NovaOS-Kernelstufe verwendet zunächst eine 32-Bit-Architektur. Der dort verfügbare virtuelle Adressraum begrenzt die mögliche Entropie deutlich.

NovaOS darf deshalb auf 32-Bit-Systemen keine unrealistisch hohe KASLR-Sicherheit behaupten. Die vollständige KASLR-Architektur wird für den späteren 64-Bit-Kernel ausgelegt, während die 32-Bit-Implementierung eine eingeschränkte Teilmenge nutzt.

---

## 3. Entscheidungsziele

Kernel ASLR soll:

1. die virtuelle Kernelbasis pro Boot randomisieren,
2. Kernelmodule unabhängig platzieren,
3. Kernelstacks und dynamische Kernelbereiche randomisieren,
4. starke frühe Entropie verwenden,
5. Bootloader und Kernel gemeinsam einbeziehen,
6. sichere Fallback- und Fehlerzustände definieren,
7. Adresslecks begrenzen,
8. symbolische Crash-Analyse weiterhin ermöglichen,
9. mit Secure Kernel Startup zusammenarbeiten,
10. Architektur- und Plattformunterschiede kapseln,
11. auf 64-Bit-Systemen ausreichende Entropie bereitstellen,
12. KASLR als zusätzliche Schutzschicht und nicht als alleinige Sicherheitsgrenze behandeln.

---

## 4. Begriffsdefinitionen

### 4.1 ASLR

Address Space Layout Randomization verändert die virtuellen Positionen von Code- und Datenbereichen.

---

### 4.2 KASLR

Kernel Address Space Layout Randomization bezeichnet ASLR für den Kerneladressraum.

---

### 4.3 Kernel Slide

Der Kernel Slide ist der zufällig gewählte Abstand zwischen der bevorzugten Linkadresse und der tatsächlichen virtuellen Ladeadresse.

```text
runtime_address =
    link_address + kernel_slide
```

---

### 4.4 Load Bias

Load Bias bezeichnet die Differenz zwischen der in einem Binärabbild verwendeten Basis und der tatsächlichen Ladebasis.

Für den Kernel entspricht er konzeptionell dem Kernel Slide.

---

### 4.5 Entropie

Entropie beschreibt die Unvorhersagbarkeit des gewählten Layouts.

Eine größere Anzahl möglicher, gleich wahrscheinlicher Positionen erhöht die theoretische KASLR-Entropie.

---

### 4.6 Relokation

Eine Relokation passt adressabhängige Werte eines Binärabbilds an dessen tatsächliche Ladeadresse an.

---

### 4.7 Information Leak

Ein Information Leak ist die unbeabsichtigte Offenlegung von Kerneladressen oder anderer layoutrelevanter Informationen.

---

## 5. Bedrohungsmodell

KASLR erschwert Angriffe, bei denen ein Angreifer für eine erfolgreiche Ausnutzung genaue Kerneladressen benötigt.

KASLR schützt insbesondere gegen:

- statische Annahmen über Kerneladressen,
- vorgefertigte ROP- und JOP-Ketten,
- direkte Adressierung globaler Kernelobjekte,
- feste Moduladressen,
- bestimmte Exploits gegen vorhersehbare Kernelstacks.

KASLR schützt nicht vollständig gegen:

- beliebige Kerneladresslecks,
- Brute-Force-Angriffe mit unbegrenzten Versuchen,
- Seitenkanäle,
- physische Angriffe,
- kompromittierte Firmware,
- schreibfähigen Kernelzugriff,
- logische Fehler ohne Adressabhängigkeit,
- Hardwarefehler,
- gestohlene Crash-Dumps mit Layoutinformationen.

KASLR ist daher eine Defense-in-Depth-Maßnahme.

---

## 6. Betrachtete Alternativen

### 6.1 Feste Kerneladresse

Der Kernel wird bei jedem Start an derselben virtuellen Adresse geladen.

#### Vorteile

- einfaches Linker- und Bootmodell,
- leichtes Debugging,
- keine Relokationen,
- geringer Startaufwand.

#### Nachteile

- vollständig vorhersehbares Layout,
- erleichtert Code-Reuse-Angriffe,
- schwache Defense-in-Depth.

Diese Alternative wird für Produktionssysteme verworfen.

---

### 6.2 Nur zufällige physische Ladeadresse

Der Kernel wird physisch zufällig platziert, behält aber eine feste virtuelle Adresse.

#### Vorteile

- erschwert einige physische Speicherangriffe,
- geringer Einfluss auf Kernelcode,
- virtuelle Symbole bleiben konstant.

#### Nachteile

- virtuelle Exploitadressen bleiben vorhersehbar,
- schützt nicht ausreichend gegen ROP und JOP,
- keine Randomisierung von Modulen oder Kernelheap.

Diese Alternative wird allein nicht übernommen.

---

### 6.3 Nur Randomisierung der Kernelbasis

Ausschließlich die virtuelle Kernelbasis wird verschoben.

#### Vorteile

- überschaubare Implementierung,
- wesentliche Adressen ändern sich,
- gute erste Ausbaustufe.

#### Nachteile

- relative Abstände bleiben vollständig vorhersehbar,
- Module und dynamische Bereiche können fest bleiben,
- ein einzelnes Adressleck offenbart große Teile des Layouts.

Diese Alternative wird als erste Implementierungsstufe verwendet, aber nicht als endgültiges Gesamtmodell.

---

### 6.4 Mehrstufige Kerneladressraum-Randomisierung

Kernelbasis, Module, Stacks und dynamische Kernelbereiche werden über getrennte Domänen randomisiert.

#### Vorteile

- größere Gesamtentropie,
- ein einzelner Leak offenbart nicht zwingend das gesamte Layout,
- bessere Trennung unterschiedlicher Kernelbereiche,
- langfristig erweiterbar.

#### Nachteile

- höhere Speicherverwaltungs- und Debuggingkomplexität,
- Fragmentierung des virtuellen Adressraums,
- mehr Relokations- und Mappingaufwand,
- auf 32-Bit-Systemen stark begrenzt.

Diese Alternative wird gewählt.

---

## 7. Entscheidung

NovaOS verwendet ein mehrstufiges KASLR-Modell.

Mindestens folgende Bereiche erhalten kontrollierte Randomisierung:

- Kernelimage,
- Kernelmodule,
- Kernelstacks,
- Kernelheap-Basis,
- CPU-lokale Bereiche,
- `vmalloc`- beziehungsweise dynamische Mappingbereiche,
- direkte physische Speicherabbildung, soweit architektonisch möglich,
- spezielle Sicherheits- und Guard-Bereiche.

Die Randomisierungen verwenden getrennt abgeleitete Zufallswerte.

Ein einzelner Zufallswert darf nicht direkt und unverändert für sämtliche Layoutbereiche verwendet werden.

---

## 8. Grundprinzip

Der Kernel wird als relokierbares Image erzeugt.

Der Bootloader:

1. sammelt frühe Entropie,
2. ermittelt gültige Lade- und Mappingbereiche,
3. wählt eine zulässige Kernelbasis,
4. lädt das Kernelimage,
5. übergibt Randomisierungsinformationen,
6. startet den frühen Kernelcode.

Der Kernel:

1. validiert das Boot-Handoff,
2. validiert die gewählte Basis,
3. führt erforderliche Relokationen aus,
4. baut die endgültigen Seitentabellen auf,
5. randomisiert weitere Kernelbereiche,
6. schützt KASLR-Metadaten,
7. entfernt unnötige temporäre Abbildungen.

---

## 9. Physische und virtuelle Randomisierung

NovaOS behandelt physische und virtuelle Platzierung getrennt.

### 9.1 Physische Randomisierung

Das Kernelimage kann abhängig von Plattform und Speicherkarte an einer variablen physischen Adresse geladen werden.

### 9.2 Virtuelle Randomisierung

Das Kernelimage wird an einer variablen virtuellen Adresse abgebildet.

Die virtuelle Randomisierung ist für den Schutz gegen typische Code-Reuse-Angriffe entscheidender.

Es gilt nicht zwingend:

```text
physical_slide == virtual_slide
```

Beide Werte können unabhängig sein.

---

## 10. Kernelimage

Das Kernelimage wird als positionsunabhängiges oder kontrolliert relokierbares Binärabbild erzeugt.

Das Image muss:

- eine bevorzugte Linkbasis besitzen,
- zulässige Relokationstypen deklarieren,
- keine unkontrollierten absoluten Adressen enthalten,
- architekturkonforme Ausrichtung einhalten,
- getrennte Code-, Daten- und Read-only-Segmente besitzen,
- nach der Initialisierung restriktive Seitenrechte verwenden.

Nicht benötigte Relokationsinformationen werden nach erfolgreicher Initialisierung freigegeben oder schreibgeschützt verborgen.

---

## 11. Relokationsmodell

NovaOS erlaubt nur ausdrücklich unterstützte Kernelrelokationen.

Der frühe Relokator muss:

- Eintragsgrößen validieren,
- Zieladressen auf Kernelbereiche begrenzen,
- Integerüberläufe erkennen,
- unbekannte Relokationstypen ablehnen,
- Schreibzugriffe auf nicht erlaubte Bereiche verhindern,
- die Anzahl der Relokationen begrenzen,
- manipulierte Relokationstabellen erkennen.

Das Kernelimage und seine Relokationsinformationen werden durch Secure Kernel Startup verifiziert.

---

## 12. Kernel Slide

Der Kernel Slide wird aus einer Menge gültiger Kandidaten gewählt.

Ein Kandidat muss:

- korrekt ausgerichtet sein,
- vollständig in den vorgesehenen virtuellen Bereich passen,
- nicht mit reservierten Kernelbereichen überlappen,
- Guard-Bereiche berücksichtigen,
- architekturspezifische Adressierungsgrenzen einhalten,
- das Kernelimage und frühe Tabellen aufnehmen können.

Konzeptionell:

```text
candidate_count =
    available_range / required_alignment

selected_index =
    random_value mod candidate_count

kernel_slide =
    selected_index * required_alignment
```

Zur Vermeidung statistischer Verzerrung muss eine geeignete gleichverteilte Auswahl verwendet werden. Eine einfache Modulo-Operation ist nur zulässig, wenn dadurch keine relevante Bias entsteht oder Rejection Sampling verwendet wird.

---

## 13. Ausrichtung

Die Kernelbasis wird an einer architekturspezifischen Grenze ausgerichtet.

Mögliche Gründe:

- große Speicherseiten,
- Seitentabellenstruktur,
- relative Adressierungsgrenzen,
- Cache- und TLB-Effizienz,
- Bootloaderanforderungen.

Eine größere Ausrichtung reduziert die Anzahl möglicher Positionen und damit die Entropie.

Die gewählte Ausrichtung muss deshalb zwischen:

- Performance,
- Implementierbarkeit,
- Speicherverbrauch,
- Sicherheitsentropie

abwägen.

---

## 14. Entropiequellen

Frühe KASLR-Entropie kann aus mehreren Quellen stammen:

- CPU-Hardwarezufallszahlengenerator,
- UEFI-RNG-Protokoll,
- TPM-Zufallsquelle,
- Bootloader-Seed,
- persistenter geschützter Zufallsseed,
- hochauflösende Timer,
- Geräte- und Interrupttiming,
- plattformspezifische Entropiequellen.

Timingwerte allein gelten nicht als ausreichend starke primäre Quelle.

NovaOS kombiniert mehrere verfügbare Quellen über eine kryptografische Ableitungsfunktion.

---

## 15. Entropievalidierung

Eine Hardwarequelle wird nicht allein aufgrund ihrer Existenz vollständig vertraut.

Der frühe Entropiecode muss:

- Rückgabestatus prüfen,
- offensichtliche konstante Werte erkennen,
- Wiederholungen begrenzen,
- mehrere Quellen kombinieren,
- Quellenausfälle im Trust State erfassen.

NovaOS darf keine erfundene Entropiestärke anzeigen.

Die tatsächlich verfügbaren Quellen und der geschätzte Sicherheitsstatus werden diagnostisch erfasst.

---

## 16. Persistenter Boot-Seed

NovaOS kann einen persistenten, geschützten Boot-Seed verwenden.

Nach erfolgreicher Nutzung wird ein neuer Seed abgeleitet und atomar gespeichert.

Der alte Seed soll nicht erneut als alleinige Grundlage eines späteren Starts dienen.

Ein persistenter Seed muss:

- integritätsgeschützt,
- nach Möglichkeit verschlüsselt,
- an die Systemgeneration oder Trust Policy gebunden,
- gegen einfaches Rollback geschützt

sein.

Fehlt der Seed oder ist er beschädigt, werden andere Entropiequellen verwendet.

---

## 17. Zufallsableitung

Aus dem frühen Master Seed werden getrennte Zufallswerte abgeleitet.

Beispiel:

```text
master_seed
├── "kernel-image"
├── "kernel-modules"
├── "kernel-stacks"
├── "kernel-heap"
├── "physical-map"
├── "per-cpu"
└── "vmalloc"
```

Die Ableitung verwendet Domain Separation.

Dadurch führt die Offenlegung eines Layoutwertes nicht direkt zur Berechnung sämtlicher anderer Layoutwerte.

---

## 18. Entropieausfall

Kann keine ausreichend starke Entropie erzeugt werden, gilt die konfigurierte Sicherheitsrichtlinie.

Mögliche Policies:

| Policy | Verhalten |
|---|---|
| `REQUIRE` | Start wird abgebrochen |
| `DEGRADE` | Start mit eingeschränktem KASLR und Warnstatus |
| `DISABLE` | KASLR wird bewusst deaktiviert |
| `RECOVERY` | verifiziertes Recovery-System wird gestartet |

Der Produktionsstandard ist:

```text
DEGRADE
```

wenn die Plattformrichtlinie KASLR nicht ausdrücklich als zwingend voraussetzt.

Der reduzierte Zustand wird im Trust State vermerkt.

---

## 19. Kein vorhersagbares Standard-Fallback

Bei Entropieausfall darf NovaOS nicht stillschweigend einen bekannten konstanten Slide verwenden und diesen als aktives KASLR melden.

Zulässig sind nur:

- klar als deaktiviert markierter fester Start,
- Startabbruch,
- Recovery,
- eine dokumentierte degradierte Zufallsauswahl.

Die Benutzer- und Systemdiagnose muss den tatsächlichen Zustand anzeigen.

---

## 20. Integration in Secure Kernel Startup

KASLR wird in die Sicherheitsinformationen aus ADR-SEC-0001 integriert.

Der Boot Trust State enthält mindestens:

```text
kaslr_enabled
kaslr_degraded
kernel_virtual_randomized
kernel_physical_randomized
module_randomization_enabled
entropy_sources
entropy_quality
```

Der konkrete Kernel Slide wird nicht über allgemeine Userspace-Schnittstellen offengelegt.

Secure Kernel Startup verifiziert das Kernelimage vor beziehungsweise während seiner randomisierten Platzierung.

---

## 21. Boot-Handoff

Das Boot-Handoff enthält KASLR-relevante Daten.

Eine mögliche Struktur lautet:

```c
typedef struct
{
    uint32_t structure_size;
    uint32_t structure_version;

    uint64_t physical_load_base;
    uint64_t virtual_load_base;
    uint64_t preferred_link_base;

    uint64_t physical_slide;
    uint64_t virtual_slide;

    uint64_t entropy_flags;
    uint64_t kaslr_flags;

    nova_digest_t entropy_commitment;
} nova_boot_kaslr_info_t;
```

Der Kernel validiert sämtliche Werte vor ihrer Verwendung.

---

## 22. Entropy Commitment

Der Bootloader kann statt des vollständigen Master Seeds nur einen kryptografischen Commitment-Wert an den späteren Kernel übergeben.

Der eigentliche Seed wird:

- nur so lange wie erforderlich im Speicher gehalten,
- nach der Ableitung überschrieben,
- nicht in allgemeine Bootlogs aufgenommen,
- nicht an Userspace weitergegeben.

Der Commitment-Wert ermöglicht Diagnose und Konsistenzprüfung, ohne den Seed offenzulegen.

---

## 23. Kernel-Selbstprüfung

Der Kernel prüft beim Start:

- tatsächliche Laufadresse,
- erwartete virtuelle Basis,
- Slide-Ausrichtung,
- erlaubten Adressbereich,
- Segmentgrenzen,
- Relokationsergebnis,
- Überschneidungen,
- Seitenrechte,
- Boot-Handoff-Konsistenz.

Stimmen tatsächliche und angegebene Basis nicht überein, wird der Start abgebrochen.

---

## 24. Übergangsabbildungen

Der Bootloader oder frühe Kernel kann temporäre Identity Mappings oder feste Übergangsabbildungen benötigen.

Diese Abbildungen müssen:

- minimal sein,
- im Handoff beschrieben werden,
- nach Aufbau des endgültigen Adressraums entfernt werden,
- keine unnötigen ausführbaren Bereiche enthalten,
- nicht dauerhaft die randomisierte Kernelbasis umgehen.

Ein dauerhaftes Alias des gesamten Kernels an einer festen Adresse würde KASLR wesentlich schwächen und ist nicht zulässig.

---

## 25. Kerneltext

Der Kerneltext wird als ausführbar und nicht schreibbar abgebildet.

Nach Abschluss der Relokation gilt:

```text
Kerneltext:
    Read = erlaubt
    Write = verboten
    Execute = erlaubt
```

Relokationen dürfen keine dauerhafte Schreibbarkeit des Kerneltexts erfordern.

Temporäre Schreibrechte werden so früh wie möglich entfernt.

---

## 26. Read-only-Kerneldaten

Unveränderliche Kerneldaten werden nach der Initialisierung schreibgeschützt.

Dazu gehören beispielsweise:

- konstante Tabellen,
- bestimmte Funktionszeigertabellen,
- Sicherheitsrichtlinien,
- initialisierte Konfigurationswerte,
- Teile der Symbol- und Metadatenstrukturen.

KASLR und Schreibschutz ergänzen sich:

- KASLR versteckt die Position,
- Schreibschutz verhindert einfache Manipulation.

---

## 27. Nicht ausführbare Datenbereiche

Kernel-Datenbereiche werden standardmäßig nicht ausführbar abgebildet.

Dies betrifft:

- Kernelheap,
- Kernelstacks,
- allgemeine Kerneldaten,
- CPU-lokale Daten,
- Seitentabellen,
- dynamische Mappings ohne Codefreigabe.

KASLR ersetzt keine W^X- oder NX-Policy.

---

## 28. Kernelmodule

Kernelmodule werden unabhängig vom Kernelimage randomisiert.

Für jedes Modul wird eine geeignete Adresse aus einem Modulbereich gewählt.

Der Modul-Loader muss:

- Modulgröße berücksichtigen,
- Ausrichtung einhalten,
- Guard-Bereiche einplanen,
- Überlappungen verhindern,
- Relokationen validieren,
- Signaturen prüfen,
- finale Seitenrechte setzen.

Module werden nicht allein als fester Offset zur Kernelbasis platziert.

---

## 29. Modul-Randomisierung

Der Modulbereich verwendet:

- zufällige Startpositionen,
- optionale zufällige Lücken,
- unabhängige Ableitungswerte,
- wiederverwendbare virtuelle Bereiche erst nach sicherer Freigabe.

Die genaue Platzierung eines Moduls darf nicht aus der Position eines anderen Moduls direkt ableitbar sein.

Die verfügbare Entropie hängt von Größe, Anzahl und Fragmentierung der Module ab.

---

## 30. Kernelstacks

Kernelstacks werden in randomisierten virtuellen Stackregionen oder aus randomisierten Zuweisungsbereichen bereitgestellt.

Jeder Kernelstack besitzt Guard Pages gemäß ADR-MM-0013.

Zu vermeiden sind:

- fortlaufend vorhersehbare Stackadressen,
- direkte Ableitung aus Thread-IDs,
- unmittelbare Platzierung sämtlicher Stacks ohne Lücken.

Die Stackrandomisierung darf die Anforderungen von ADR-MM-0015 nicht verletzen.

---

## 31. Kernelheap

Die Basis des Kernelheaps wird randomisiert.

Zusätzlich kann der Heap-Allocator eigene Randomisierungsmechanismen verwenden:

- zufällige Slab-Auswahl,
- zufällige Freelist-Reihenfolge,
- getrennte Caches,
- Guard-Bereiche,
- Quarantäne freigegebener Objekte.

Die Randomisierung der Heapbasis allein schützt nicht gegen vorhersehbare Objektanordnungen innerhalb des Heaps.

Heap-Hardening wird in einer separaten ADR spezifiziert.

---

## 32. Direkte physische Speicherabbildung

Viele Kernel verwenden einen Bereich, der physischen Speicher direkt virtuell abbildet.

NovaOS randomisiert die Basis dieser Direct Map, soweit die Architektur und frühe Speicherverwaltung dies erlauben.

Die Direct Map:

- ist nicht im Userspace sichtbar,
- verwendet restriktive Seitenrechte,
- darf keine unnötigen ausführbaren Seiten enthalten,
- wird nicht über allgemeine Diagnose-APIs offengelegt.

Auf 32-Bit-Systemen kann der verfügbare Adressraum diese Randomisierung stark begrenzen.

---

## 33. Dynamische Mappingbereiche

Dynamische Kernelabbildungen erhalten einen eigenen randomisierten virtuellen Bereich.

Dazu gehören beispielsweise:

- temporäre physische Mappings,
- Geräte-Mappings,
- Modulbereiche,
- große Kernelallokationen,
- Debug- und Diagnosemappings.

Jeder Bereich besitzt eigene Grenzen und eine eigene Zufallsableitung.

---

## 34. CPU-lokale Bereiche

CPU-lokale Datenbereiche werden randomisiert oder über eine randomisierte Basis referenziert.

Die Adressbildung muss dennoch effizient bleiben, da CPU-lokale Daten häufig verwendet werden.

Mögliche Umsetzung:

- randomisierte globale Per-CPU-Basis,
- randomisierte Bereiche pro CPU,
- architekturspezifisches Basisregister.

CPU-IDs dürfen nicht direkt zur Berechnung einer öffentlich vorhersagbaren Adresse führen.

---

## 35. Seitentabellen

Seitentabellen werden aus geschützten Kernelpools bereitgestellt.

Ihre virtuellen Verwaltungsadressen sollen nicht unnötig vorhersehbar sein.

Seitentabellen:

- sind nicht ausführbar,
- sind nur für erforderliche Kernelkomponenten schreibbar,
- dürfen nicht über normale Userspace-Diagnose offengelegt werden,
- werden nach Möglichkeit außerhalb allgemein vorhersehbarer Objektfolgen platziert.

Die physischen Seitentabellenadressen können durch die Speicherallokation ebenfalls variieren.

---

## 36. Guard-Bereiche

Zwischen wichtigen Kernelregionen können ungemappte Guard-Bereiche liegen.

Beispiele:

```text
Guard
Kerneltext
Guard
Read-only-Daten
Guard
Kerneldaten
Guard
Module
Guard
Kernelheap
```

Guard-Bereiche:

- erschweren lineare Überläufe,
- erkennen ungültige Zugriffe früher,
- reduzieren verfügbare Adressraumkapazität,
- können zusätzliche Layoutentropie liefern.

Ihre Größe kann innerhalb definierter Grenzen variieren.

---

## 37. 32-Bit-Kernel

Der 32-Bit-Kernel besitzt nur einen begrenzten virtuellen Adressraum.

Dadurch sind:

- Anzahl möglicher Kernelbasen,
- Modulrandomisierung,
- Heaprandomisierung,
- Direct-Map-Randomisierung,
- große Guard-Bereiche

stark eingeschränkt.

Für die erste 32-Bit-Implementierung wird mindestens unterstützt:

1. randomisierbare Kernelbasis,
2. randomisierte physische Ladeadresse, soweit möglich,
3. einfacher randomisierter Modulbereich,
4. Kernelstack-Guard-Pages,
5. klare Meldung der geringen Entropie.

Der 32-Bit-Modus gilt nicht als langfristige maximale KASLR-Sicherheitsstufe.

---

## 38. 64-Bit-Kernel

Der spätere 64-Bit-Kernel nutzt den größeren virtuellen Adressraum für getrennte Randomisierungsdomänen.

Dazu gehören:

- Kernelimage,
- Direct Map,
- Module,
- Kernelheap,
- dynamische Mappings,
- CPU-lokale Bereiche,
- Kernelstacks,
- Sanitizer- oder Shadow-Bereiche.

Nicht kanonische und hardwareseitig reservierte Adressbereiche werden ausgeschlossen.

Die genaue Adressraumaufteilung wird durch die Architektur-Speicherspezifikation festgelegt.

---

## 39. Entropieziel

Die tatsächliche Entropie hängt von der Architektur und dem verfügbaren virtuellen Adressraum ab.

NovaOS definiert deshalb keine identische Bitzahl für alle Plattformen.

Die Implementierung muss für jede Plattform dokumentieren:

- Anzahl möglicher Kernelpositionen,
- Anzahl möglicher Modulpositionen,
- Ausrichtung,
- ausgeschlossene Bereiche,
- effektive theoretische Entropie,
- bekannte Reduktionen.

Auf 64-Bit-Systemen soll die Kernelbasis mindestens eine praktisch relevante Zahl zufälliger Positionen besitzen. Die konkrete Mindestanforderung wird in NPSPEC-KASLR-0001 definiert.

---

## 40. Adresslecks

KASLR ist wirkungslos, wenn Kerneladressen unkontrolliert offengelegt werden.

NovaOS begrenzt deshalb Kerneladressinformationen in:

- Logs,
- Fehlermeldungen,
- Telemetrie,
- Debugschnittstellen,
- Prozessinformationen,
- Gerätedaten,
- Crashberichten,
- Objektkennungen.

Kernelpointer dürfen nicht als allgemeine Objekt-IDs verwendet werden.

---

## 41. Pointerdarstellung

Kernelpointer werden in normalen Logs standardmäßig:

- entfernt,
- maskiert,
- gehasht,
- durch symbolische Bezeichnungen ersetzt.

Ein berechtigter Debugmodus kann vollständige Adressen anzeigen.

Dieser Zugriff benötigt eine spezielle Capability und kann den Sicherheitsstatus beeinflussen.

Beispiel:

```text
Nicht privilegiert:
    object=thread:42 address=<hidden>

Privilegiert:
    object=thread:42 address=0xffff...
```

---

## 42. Symboltabellen

Kernel-Symboltabellen dürfen die tatsächliche Laufzeitbasis nicht unkontrolliert offenlegen.

NovaOS trennt:

- Build-Symbole,
- Laufzeitsymbole,
- exportierte Modulsymbole,
- Diagnoseinformationen.

Nicht benötigte vollständige Symbolnamen und Adressen können aus Produktionsimages entfernt oder in geschützten Bereichen gespeichert werden.

Kernelmodule erhalten nur die ausdrücklich exportierten Symbole.

---

## 43. Crash-Dumps

Crash-Dumps benötigen Informationen zur symbolischen Auswertung randomisierter Adressen.

Ein Dump kann deshalb enthalten:

- Build-ID,
- Kernel-Image-ID,
- Systemgeneration,
- verschlüsselten oder geschützten Kernel Slide,
- Modulbasisadressen,
- Modul-Build-IDs,
- Architekturinformationen.

Der Slide darf nicht unkontrolliert in öffentlich lesbaren Dumps gespeichert werden.

Crash-Dumps können:

- verschlüsselt,
- capabilitygeschützt,
- für autorisierte Offlineanalyse exportiert

werden.

---

## 44. Panic-Ausgabe

Eine Kernel-Panic darf standardmäßig keine vollständigen Kerneladressen öffentlich anzeigen.

Die sichtbare Panic-Ausgabe verwendet:

- Fehlercode,
- Build-ID,
- symbolische Funktionsbezeichnungen, soweit sicher,
- anonymisierte oder relative Offsets,
- Diagnose-ID.

Vollständige Adressen können im geschützten Crash-Dump verbleiben.

---

## 45. Debugging

Für Entwicklungssysteme kann KASLR kontrolliert deaktiviert oder reproduzierbar gemacht werden.

Mögliche Entwicklungsoptionen:

```text
kaslr=off
kaslr=seed:<authorized-seed>
kaslr=trace
```

Diese Optionen sind im Produktionsmodus nicht allgemein zulässig.

Werden sie verwendet, wird der Trust State als Development oder Degraded markiert.

Ein fester Debug-Seed darf nicht versehentlich in Produktionsimages übernommen werden.

---

## 46. Deterministische Tests

Kerneltests benötigen teilweise reproduzierbare Layouts.

NovaOS unterstützt hierfür einen autorisierten Testmodus mit:

- festem Test-Seed,
- protokolliertem Layout,
- nicht produktivem Schlüssel,
- klarer Kennzeichnung.

Der Testmodus darf keine Produktionsgeheimnisse freigeben und nicht automatisch aktiviert werden.

Zusätzlich müssen Tests mit zufälligen Seeds durchgeführt werden, um Layoutannahmen im Kernelcode aufzudecken.

---

## 47. Ruhezustand und Wiederaufnahme

Bei einem echten Hibernate-Vorgang wird der bestehende Kernelzustand fortgesetzt.

Das KASLR-Layout bleibt deshalb für die wiederaufgenommene Instanz identisch.

Das Hibernate-Image enthält layoutkritische Daten und muss:

- verschlüsselt,
- authentifiziert,
- an Systemgeneration und Bootpolicy gebunden,
- gegen Rollback geschützt

werden.

Ein Hibernate-Image darf nicht unter einer inkompatiblen Kernelbasis wiederhergestellt werden.

---

## 48. Kexec und Kernelwechsel

Ein direkter Wechsel zu einem neuen Kernel muss eine neue KASLR-Entscheidung treffen.

Der alte Kernel darf nicht ungeprüft einen vorhersehbaren Slide für den neuen Kernel festlegen.

Der neue Kernel benötigt:

- neue Entropieableitung,
- eigenes verifiziertes Manifest,
- neues Boot-Handoff,
- neue KASLR-Metadaten.

Ein Crash-Kernel kann eine gesonderte, vorab reservierte und verifizierte Platzierung verwenden.

---

## 49. Virtualisierung

In virtuellen Maschinen verwendet NovaOS verfügbare virtuelle Entropiequellen.

Der Hypervisor kann:

- virtuelle RNG-Geräte,
- Firmware-RNG,
- virtuelle TPMs

bereitstellen.

NovaOS darf nicht davon ausgehen, dass ein virtueller RNG automatisch vertrauenswürdig ist.

Der Trust State kennzeichnet virtualisierte Entropiequellen entsprechend.

Snapshots können identische Zufallszustände duplizieren. Nach Snapshot-Wiederherstellung muss der Zufallszustand neu diversifiziert werden, soweit dies erkennbar und möglich ist.

---

## 50. Mehrkernsysteme

KASLR wird vor dem Start zusätzlicher CPUs festgelegt.

Alle CPUs verwenden denselben globalen Kerneladressraum, soweit die Architektur nichts anderes vorgibt.

CPU-lokale Bereiche können dennoch unabhängig randomisierte Unterbereiche besitzen.

Zusätzliche CPUs dürfen erst gestartet werden, wenn:

- endgültige Kernelmappings aktiv,
- CPU-lokale Basen festgelegt,
- Seitentabellen konsistent,
- TLB-Regeln definiert

sind.

---

## 51. NUMA-Systeme

Auf NUMA-Systemen bleibt die virtuelle KASLR-Platzierung grundsätzlich unabhängig von der physischen NUMA-Platzierung.

Physische Kernelbereiche können entsprechend der NUMA-Policy verteilt oder repliziert werden.

Die virtuelle Randomisierung darf keine Annahme erzwingen, dass virtuell benachbarte Bereiche physisch oder NUMA-lokal benachbart sind.

---

## 52. KASLR-Metadaten

KASLR-Metadaten umfassen unter anderem:

- Kernel Slide,
- Bereichsbasen,
- Entropieflags,
- Modulpositionen,
- Ableitungszustände,
- Layoutgeneration.

Diese Metadaten werden:

- im Kernel geschützt gespeichert,
- nicht allgemein exportiert,
- nach Möglichkeit schreibgeschützt,
- nur über spezielle Diagnosepfade verfügbar gemacht.

Der ursprüngliche Master Seed wird nach Abschluss der erforderlichen Ableitungen überschrieben.

---

## 53. Laufzeit-Neurandomisierung

NovaOS führt zunächst keine vollständige Laufzeit-Neurandomisierung des Kernelimages durch.

Gründe:

- extrem hohe Komplexität,
- Anpassung laufender Funktionszeiger,
- Unterbrechung von Debug- und Modulzuständen,
- TLB- und Mehrkernkoordination,
- schwer beweisbare Korrektheit.

Dynamische Teilbereiche wie Heapobjekte und Module können jedoch eigene laufende Randomisierungsmechanismen verwenden.

---

## 54. KASLR und Live-Updates

Ein Live-Update darf die KASLR-Garantie nicht durch feste temporäre Aliasadressen umgehen.

Neue Kernelkomponenten müssen:

- verifiziert,
- in zulässigen randomisierten Bereichen platziert,
- mit restriktiven Seitenrechten versehen,
- nach Abschluss von temporären Mappings bereinigt

werden.

Ein vollständiger Wechsel zu einer neuen Kernelgeneration kann eine neue Layoutgeneration erfordern.

---

## 55. Kernel-ABI

Öffentliche Kernel- und Treiberschnittstellen dürfen keine festen Kerneladressen voraussetzen.

Treiber und Module verwenden:

- Symbolauflösung,
- Handles,
- Objekt-IDs,
- versionierte Services,
- relative oder abstrahierte Referenzen.

Ein ABI, das feste Kerneladressen benötigt, ist mit KASLR nicht vereinbar.

---

## 56. API-Grundmodell

KASLR besitzt keine allgemeine Userspace-Steuerungs-API.

Autorisierte Diagnose kann eine reduzierte Statusstruktur abfragen:

```c
nova_status_t nova_security_get_kaslr_status(
    nova_kaslr_status_t* status
);
```

Eine mögliche Struktur lautet:

```c
typedef struct
{
    uint32_t structure_size;
    uint32_t structure_version;

    bool enabled;
    bool degraded;
    bool kernel_image_randomized;
    bool physical_load_randomized;
    bool modules_randomized;
    bool stacks_randomized;
    bool heap_randomized;

    uint32_t entropy_status;
    uint32_t entropy_source_flags;

    uint32_t architecture_mode;
    uint32_t layout_generation;

    uint32_t estimated_kernel_entropy_bits;
    uint32_t estimated_module_entropy_bits;
} nova_kaslr_status_t;
```

Die Struktur enthält keine konkreten Laufzeitadressen oder Slides.

---

## 57. Capability-Modell

Vollständige KASLR-Informationen sind besonders geschützt.

Mögliche Capabilities sind:

```text
CAP_SECURITY_QUERY_KASLR
CAP_SECURITY_READ_KERNEL_LAYOUT
CAP_SECURITY_READ_KERNEL_POINTERS
CAP_DEBUG_KERNEL
CAP_CRASH_DUMP_DECRYPT
CAP_SECURITY_KASLR_ADMIN
```

Das Recht zur Abfrage des KASLR-Status beinhaltet nicht automatisch das Recht, den Kernel Slide zu lesen.

---

## 58. Fehlerbehandlung

KASLR kann fehlschlagen durch:

- fehlende Entropie,
- ungültigen Kandidatenbereich,
- zu wenig virtuellen Adressraum,
- Relokationsfehler,
- Überschneidung von Kernelbereichen,
- ungültige Ausrichtung,
- beschädigte Handoff-Daten,
- nicht unterstützten Relokationstyp,
- inkompatible Architektur,
- fehlgeschlagenen Seitentabellenaufbau.

Mögliche Statuswerte sind:

```text
NOVA_STATUS_OK
NOVA_STATUS_KASLR_NO_ENTROPY
NOVA_STATUS_KASLR_NO_VALID_RANGE
NOVA_STATUS_KASLR_RELOCATION_FAILED
NOVA_STATUS_KASLR_INVALID_SLIDE
NOVA_STATUS_KASLR_ADDRESS_CONFLICT
NOVA_STATUS_KASLR_UNSUPPORTED_RELOCATION
NOVA_STATUS_KASLR_HANDOFF_INVALID
NOVA_STATUS_KASLR_DEGRADED
NOVA_STATUS_KASLR_DISABLED
```

Die Reaktion wird durch die aktive Secure-Startup-Policy bestimmt.

---

## 59. Verhalten bei KASLR-Fehlern

Im Produktionsmodus gelten folgende Standardreaktionen:

| Fehler | Reaktion |
|---|---|
| geringe, aber vorhandene Entropie | degradiert starten und kennzeichnen |
| keine starke Entropie | gemäß Plattformpolicy degradieren oder abbrechen |
| ungültiger Slide | neuen Kandidaten wählen |
| kein gültiger Kandidat | KASLR deaktivieren oder Start abbrechen |
| Relokationsfehler | Start abbrechen |
| Mappingüberlappung | Start abbrechen |
| manipulierte KASLR-Daten | Start abbrechen |
| nicht unterstütztes Kernelimage | Start abbrechen |

Ein beschädigtes oder manipuliertes Layout darf nicht als bloßer Entropiemangel behandelt werden.

---

## 60. Telemetrie und Diagnose

KASLR-Telemetrie erfasst mindestens:

- KASLR aktiviert oder deaktiviert,
- degradierter Zustand,
- verwendete Entropiequellen,
- geschätzte Entropieklasse,
- Anzahl geprüfter Kandidaten,
- Relokationsergebnis,
- randomisierte Bereichstypen,
- Architekturmodus,
- KASLR-Fehlercodes,
- Layoutgeneration.

Konkrete Adressen, Slides und Seeds werden nicht in normale Telemetrie aufgenommen.

---

## 61. Konsequenzen

### 61.1 Positive Konsequenzen

- Kerneladressen ändern sich zwischen Systemstarts,
- statische Code-Reuse-Angriffe werden erschwert,
- Kernelmodule und Stacks werden weniger vorhersehbar,
- klare Integration in Secure Kernel Startup,
- getrennte Randomisierungsdomänen,
- Unterstützung von BIOS und UEFI,
- kontrollierte Crash-Dump-Auswertung,
- Grundlage für weitere Kernel-Hardening-Maßnahmen.

### 61.2 Negative Konsequenzen

- komplexerer Boot- und Linkprozess,
- Relokationen erhöhen die Startzeit,
- Debugging wird aufwendiger,
- Crash-Dumps benötigen zusätzliche Metadaten,
- 32-Bit-Systeme bieten nur geringe Entropie,
- virtueller Adressraum kann stärker fragmentiert werden,
- Kernelmodule benötigen einen komplexeren Loader.

### 61.3 Risiken

- schwache oder vorhersagbare Entropie,
- Kerneladresslecks,
- fehlerhafte Relokationen,
- dauerhaft vorhandene feste Aliasabbildungen,
- Offenlegung des Slides in Logs oder Dumps,
- zu geringe Entropie durch große Ausrichtung,
- identische Seeds nach VM-Snapshots,
- falsche Sicherheitsanzeige auf 32-Bit-Systemen.

Diese Risiken werden begrenzt durch:

- mehrere Entropiequellen,
- kryptografische Zufallsableitung,
- Domain Separation,
- Adressmaskierung,
- geschützte Crash-Dumps,
- Validierung aller Relokationen,
- Entfernung temporärer Mappings,
- ehrliche Trust-State-Darstellung.

---

## 62. Auswirkungen auf andere Subsysteme

### 62.1 Bootloader

Der Bootloader muss Entropie sammeln, gültige Kandidaten auswählen und KASLR-Informationen übergeben.

### 62.2 Secure Kernel Startup

Der KASLR-Zustand wird Teil des Boot Trust State.

### 62.3 Linker und Buildsystem

Der Kernel muss als positionsunabhängiges oder kontrolliert relokierbares Image gebaut werden.

### 62.4 Speicherverwaltung

Die virtuelle Speicherverwaltung muss variable Kernelbasen und getrennte randomisierte Bereiche unterstützen.

### 62.5 Kernelmodulsystem

Module benötigen unabhängige Platzierung, sichere Relokation und finale W^X-Seitenrechte.

### 62.6 Threadverwaltung

Kernelstacks werden aus randomisierten Bereichen mit Guard Pages erzeugt.

### 62.7 Kernelheap

Der Heap erhält eine randomisierte Basis und spätere zusätzliche Hardening-Mechanismen.

### 62.8 Telemetrie

Kerneladressen dürfen in normalen Logs nicht unkontrolliert offengelegt werden.

### 62.9 Debugger

Debugger benötigen autorisierte Symbol-, Slide- und Layoutinformationen.

### 62.10 Crash-Dump-System

Dumps müssen Layoutinformationen geschützt speichern und mit Build-IDs verbinden.

### 62.11 Live-Update-System

Neue Kernelkomponenten dürfen keine festen, vorhersehbaren Aliasadressen einführen.

---

## 63. Implementierungsrichtlinien

1. Die erste Ausbaustufe randomisiert die virtuelle Kernelbasis.
2. Das Kernelimage wird relokierbar gebaut.
3. Der frühe Relokator unterstützt nur explizit erlaubte Relokationstypen.
4. Entropie wird aus mehreren verfügbaren Quellen kombiniert.
5. Alle Layoutdomänen verwenden getrennte Zufallsableitungen.
6. Der Master Seed wird nach der Initialisierung überschrieben.
7. Temporäre feste Kernelabbildungen werden so früh wie möglich entfernt.
8. Kerneltext wird nach Relokation nicht schreibbar abgebildet.
9. Kerneldaten und Stacks werden nicht ausführbar abgebildet.
10. Kernelmodule werden unabhängig vom Kernelimage randomisiert.
11. Kernelpointer werden in normalen Logs verborgen.
12. Crash-Dumps speichern KASLR-Daten nur geschützt.
13. Der 32-Bit-Modus meldet seine begrenzte Entropie ausdrücklich.
14. Die vollständige Mehrbereichsrandomisierung wird für 64 Bit ausgelegt.
15. Ein Entropieausfall wird niemals als erfolgreiches volles KASLR dargestellt.
16. KASLR-Statusabfragen geben keine konkreten Adressen zurück.
17. Debug-Seeds und `kaslr=off` sind im Produktionsmodus nicht allgemein zulässig.
18. Jede neue Architektur dokumentiert ihre tatsächliche KASLR-Entropie.

---

## 64. Testanforderungen

Mindestens folgende Szenarien müssen getestet werden:

- Kernel startet mit unterschiedlichen virtuellen Basen,
- physische Ladeadressrandomisierung,
- gültige Relokationen,
- ungültige Relokationstypen,
- beschädigte Relokationstabellen,
- Integerüberläufe im Relokator,
- ungültiger Kernel Slide,
- Bereichsüberlappungen,
- Ausrichtungsfehler,
- mehrere Entropiequellen,
- Ausfall einer Entropiequelle,
- vollständiger Entropieausfall,
- degradierter KASLR-Start,
- Produktionspolicy `REQUIRE`,
- Boot-Handoff-Manipulation,
- Kernel-Selbstprüfung,
- Entfernung temporärer Identity Mappings,
- Read-only-Kerneltext,
- nicht ausführbare Kerneldaten,
- unabhängige Modulrandomisierung,
- Modulrelokationen,
- Kernelstack-Randomisierung,
- Stack-Guard-Pages,
- Kernelheap-Basisrandomisierung,
- Direct-Map-Randomisierung,
- CPU-lokale Bereiche,
- Mehrkernstart,
- NUMA-Systeme,
- 32-Bit-Adressraum,
- 64-Bit-Adressraum,
- Kerneladressmaskierung in Logs,
- Panic-Ausgabe,
- geschützter Crash-Dump,
- autorisierter Debugzugriff,
- fester Test-Seed,
- VM-Snapshot-Wiederaufnahme,
- Hibernate-Wiederaufnahme,
- direkter Kernelwechsel,
- Live-Update,
- Kernelmodul-Laden unter KASLR,
- statistische Verteilung der Slides,
- Fuzzing des frühen Relokators.

---

## 65. Nicht Bestandteil dieser Entscheidung

Diese ADR legt nicht abschließend fest:

- die konkrete virtuelle Adressraumaufteilung,
- exakte Entropie-Mindestwerte pro Architektur,
- konkrete kryptografische Ableitungsfunktionen,
- vollständiges Kernelheap-Hardening,
- Fine-Grained KASLR auf Funktionsebene,
- laufende Neurandomisierung des Kernelimages,
- Cache- und Branch-Predictor-Schutz,
- vollständige Crash-Dump-Verschlüsselung,
- die endgültige Diagnose-ABI.

Diese Details werden in nachfolgenden ADRs und NPSPEC-Dokumenten definiert.

---

## 66. Verbindliche Invarianten

1. Produktionskernel verwenden KASLR, sofern die aktive Plattformpolicy es nicht ausdrücklich deaktiviert.
2. Das Kernelimage muss positionsunabhängig oder kontrolliert relokierbar sein.
3. Der Kernel Slide muss aus einem validierten Adressbereich stammen.
4. Kernelbereiche dürfen sich nach der Randomisierung nicht überlappen.
5. Alle Relokationseinträge werden vor ihrer Anwendung validiert.
6. Unbekannte Relokationstypen werden abgelehnt.
7. Kerneltext ist nach Abschluss der Relokation nicht schreibbar.
8. Kernel-Datenbereiche und Kernelstacks sind standardmäßig nicht ausführbar.
9. Kernelmodule werden unabhängig von der Kernelbasis platziert.
10. Ein einzelner Zufallswert wird nicht unverändert für sämtliche Layoutdomänen verwendet.
11. Der KASLR-Master-Seed wird nicht an normalen Userspace weitergegeben.
12. Der Master Seed wird nach Abschluss der erforderlichen Ableitungen überschrieben.
13. Konkrete Kerneladressen werden in normalen Logs und Fehlermeldungen verborgen.
14. Kernelpointer dürfen nicht als öffentliche Objektkennungen dienen.
15. Temporäre feste Kernelabbildungen werden nach der Initialisierung entfernt.
16. Ein Entropieausfall darf nicht als vollwertig aktives KASLR dargestellt werden.
17. Der konkrete Kernel Slide wird nicht über allgemeine Status-APIs offengelegt.
18. Crash-Dumps mit Layoutinformationen müssen capabilitygeschützt oder verschlüsselt sein.
19. Ein 32-Bit-Kernel darf keine mit 64-Bit-Systemen vergleichbare KASLR-Entropie behaupten.
20. Eine Capability allein darf KASLR nicht unbemerkt deaktivieren.
21. KASLR ersetzt weder Speichersicherheit noch W^X, NX, Guard Pages oder Kontrollflussschutz.
22. KI- oder lernbasierte Komponenten dürfen KASLR-, Adressschutz-, Debug-, Capability- oder Secure-Startup-Regeln nicht umgehen.

---

## 67. Referenzen

- ADR-SEC-0001 – Secure Kernel Startup
- ADR-BOOT-0001 – BIOS und UEFI parallel unterstützen
- ADR-BOOT-1012 – Eigenes Boot-Handoff-Protokoll
- ADR-MM-0001 – Physical Memory Management
- ADR-MM-0002 – Virtual Memory Management
- ADR-MM-0012 – NUMA Memory Policy
- ADR-MM-0013 – Guard Pages
- ADR-MM-0015 – Kernel Stack Layout
- ADR-KERNEL-0103 – Native Capability-basierte Ressourcenverwaltung
- ADR-KERNEL-0104 – Integrierte Telemetrie- und Diagnoseschnittstellen
- ADR-KERNEL-0105 – ABI-Stabilität über versionierte Kernel-Services
- ADR-PROC-0002 – Thread Lifecycle
- ADR-PROC-0007 – CPU Affinity
- ADR-PROC-0008 – Context Switching
- NPSPEC-KASLR-0001 – Kernel ASLR Specification
- NPSPEC-SECURESTARTUP-0001 – Secure Kernel Startup Specification
- NPSPEC-BOOTHANDOFF-0001 – Boot Handoff Specification
- NPSPEC-KERNELMODULE-0001 – Kernel Module Specification
- NPSPEC-CRASHDUMP-0001 – Kernel Crash Dump Specification

---

## 68. Zusammenfassung der Entscheidung

NovaOS verwendet ein mehrstufiges Kernel-ASLR-Modell.

Der Bootloader sammelt frühe Entropie, wählt eine gültige physische und virtuelle Kernelposition und übergibt die KASLR-Informationen über das versionierte Boot-Handoff. Der Kernel validiert die gewählte Position, führt ausschließlich erlaubte Relokationen aus und baut anschließend den endgültigen randomisierten Kerneladressraum auf.

Neben dem Kernelimage werden langfristig auch folgende Bereiche unabhängig randomisiert:

- Kernelmodule,
- Kernelstacks,
- Kernelheap,
- dynamische Mappingbereiche,
- CPU-lokale Daten,
- direkte physische Speicherabbildung.

Die einzelnen Layoutbereiche verwenden getrennt abgeleitete Zufallswerte. Der ursprüngliche Master Seed wird nach der Initialisierung gelöscht.

Kerneladressen werden in normalen Logs, Panic-Anzeigen und Diagnose-APIs verborgen. Autorisierte Debugger und geschützte Crash-Dumps können die notwendigen Layoutinformationen erhalten.

Der anfängliche 32-Bit-Kernel unterstützt aufgrund seines begrenzten virtuellen Adressraums nur eine reduzierte KASLR-Entropie. Die vollständige Mehrbereichsrandomisierung wird auf den späteren 64-Bit-Kernel ausgerichtet.

KASLR ist eine zusätzliche Schutzschicht. Es ersetzt keine Speichersicherheit, W^X, NX, Guard Pages, Signaturprüfung oder Capability-basierte Isolation.