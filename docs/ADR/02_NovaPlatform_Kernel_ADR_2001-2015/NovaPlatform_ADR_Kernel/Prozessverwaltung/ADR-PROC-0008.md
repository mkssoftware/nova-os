# ADR-PROC-0008 – Context Switching

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-PROC-0008 |
| Titel | Context Switching |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-30 |
| Bereich | Kernel / Prozesse / Threads / Scheduling / CPU-Architektur |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-PROC-0001, ADR-PROC-0002, ADR-PROC-0003, ADR-PROC-0004, ADR-PROC-0005, ADR-PROC-0006, ADR-PROC-0007 |
| Nachfolgende Spezifikation | NPSPEC-CONTEXTSWITCH-0001 |

---

## 1. Kontext

NovaOS führt mehrere Prozesse und Threads scheinbar gleichzeitig aus.

Auf jeder logischen CPU kann zu einem Zeitpunkt jedoch nur ein Thread aktiv sein. Der Scheduler muss deshalb zwischen ausführbaren Threads wechseln.

Bei einem Context Switch wird der Ausführungskontext des aktuell laufenden Threads gespeichert und der Kontext des nächsten Threads wiederhergestellt.

Dieser Kontext kann umfassen:

- allgemeine CPU-Register,
- Stack Pointer,
- Instruction Pointer,
- Status- und Kontrollregister,
- Kernelstack,
- Adressraum,
- Thread Local Storage,
- FPU- und SIMD-Zustand,
- Debugregister,
- Sicherheitszustände,
- CPU-lokale Threadreferenzen,
- Scheduler- und Abrechnungsdaten.

Context Switching ist einer der kritischsten Kernelpfade.

Fehler können zu folgenden Problemen führen:

- beschädigte Register,
- falsche Adressräume,
- Zugriff auf fremde Prozessdaten,
- verlorene FPU-Zustände,
- ungültige Kernelstacks,
- falsche Rückkehr in den Userspace,
- Sicherheitslücken,
- nicht reproduzierbare Systemabstürze.

Der Context-Switch-Pfad muss deshalb minimal, deterministisch, architekturgekapselt und umfassend getestet sein.

---

## 2. Problemstellung

NovaOS benötigt ein einheitliches Context-Switch-Modell für:

- Userspace-Threads,
- Kernelthreads,
- Echtzeitthreads,
- CPU-lokale Idle Threads,
- Threadmigrationen,
- Prozesswechsel,
- Rückkehr aus Interrupts,
- Rückkehr aus Systemaufrufen,
- präemptive und freiwillige Kontextwechsel.

Dabei müssen unterschiedliche Fälle unterstützt werden:

```text
Thread A → Thread B im selben Prozess
Thread A → Thread C in einem anderen Prozess
Userspace → Kernelthread
Kernelthread → Userspace
normaler Thread → Idle Thread
Idle Thread → normaler Thread
```

Nicht jeder Wechsel benötigt dieselben Operationen.

Ein Threadwechsel innerhalb desselben Prozesses benötigt beispielsweise nicht zwingend einen Wechsel des Adressraums. Ein Wechsel zwischen zwei Prozessen kann dagegen eine Aktualisierung der Seitentabellen, Adressraumkennungen und TLB-Behandlung erfordern.

Das Modell muss diese Unterschiede nutzen, ohne mehrere widersprüchliche Context-Switch-Implementierungen einzuführen.

---

## 3. Entscheidungsziele

Das Context-Switch-Modell soll:

1. alle erforderlichen Threadzustände vollständig sichern,
2. eine sichere Wiederaufnahme jedes Threads ermöglichen,
3. Thread- und Prozesswechsel unterscheiden,
4. unnötige Adressraumwechsel vermeiden,
5. Mehrkernsysteme unterstützen,
6. Echtzeitanforderungen berücksichtigen,
7. Architekturdetails kapseln,
8. FPU-, SIMD- und Erweiterungszustände sicher verwalten,
9. Sicherheitsgrenzen zwischen Prozessen erhalten,
10. Kontextwechsel korrekt abrechnen und diagnostizieren,
11. einen möglichst kleinen kritischen Pfad besitzen,
12. zukünftige CPU-Erweiterungen über versionierte Zustandsformate unterstützen.

---

## 4. Begriffsdefinitionen

### 4.1 Thread Context

Der Thread Context enthält alle Zustände, die benötigt werden, um die Ausführung eines Threads später korrekt fortzusetzen.

---

### 4.2 Context Switch

Ein Context Switch ist der Wechsel der aktiven CPU-Ausführung von einem Thread zu einem anderen Thread.

---

### 4.3 Process Switch

Ein Process Switch ist ein Context Switch zwischen Threads unterschiedlicher Prozesse beziehungsweise unterschiedlicher Adressräume.

Jeder Process Switch ist ein Context Switch, aber nicht jeder Context Switch ist ein Process Switch.

---

### 4.4 Voluntary Context Switch

Ein freiwilliger Context Switch entsteht, wenn ein Thread selbst nicht weiterlaufen kann oder möchte.

Beispiele:

- Warten auf ein Ereignis,
- Warten auf I/O,
- Warten auf eine Sperre,
- freiwillige Abgabe der CPU,
- Threadbeendigung.

---

### 4.5 Involuntary Context Switch

Ein erzwungener Context Switch wird durch den Kernel ausgelöst.

Beispiele:

- Ablauf einer Zeitscheibe,
- Aufwecken eines höher priorisierten Threads,
- Echtzeit-Deadline,
- Prioritätsänderung,
- CPU-Hotplug,
- Affinitätsänderung.

---

### 4.6 Switch Frame

Der Switch Frame ist die architekturspezifische Speicherstruktur, in der der für einen Context Switch erforderliche CPU-Zustand abgelegt wird.

---

### 4.7 Trap Frame

Der Trap Frame enthält den CPU-Zustand, der beim Eintritt in den Kernel durch Interrupt, Exception oder Systemaufruf gesichert wird.

Trap Frame und Switch Frame können sich teilweise überschneiden, bleiben aber konzeptionell getrennte Strukturen.

---

## 5. Betrachtete Alternativen

### 5.1 Vollständiges Speichern aller CPU-Zustände bei jedem Wechsel

Bei jedem Context Switch werden alle verfügbaren Register und Erweiterungszustände gespeichert.

#### Vorteile

- einfaches Zustandsmodell,
- kein verlorener CPU-Zustand,
- geringe Anzahl spezieller Fälle.

#### Nachteile

- hohe Kosten bei jedem Context Switch,
- große Speicherzugriffe,
- unnötiges Speichern ungenutzter Erweiterungszustände,
- schlechte Skalierung bei großen Vektor- und Matrixzuständen.

Diese Alternative wird für den allgemeinen Pfad verworfen.

---

### 5.2 Minimaler Kontext ohne erweiterten CPU-Zustand

Es werden nur allgemeine Register und Stackinformationen gespeichert.

#### Vorteile

- sehr schneller Context Switch,
- geringer Speicherbedarf,
- einfache Implementierung.

#### Nachteile

- FPU-, SIMD- und Erweiterungszustände könnten verloren gehen,
- ungeeignet für moderne Anwendungen,
- Sicherheits- und Datenintegritätsrisiko.

Diese Alternative wird verworfen.

---

### 5.3 Vollständig verzögertes Speichern erweiterter Zustände

FPU- und SIMD-Zustände werden erst gespeichert, wenn ein anderer Thread die entsprechende Einheit verwendet.

#### Vorteile

- weniger Speicheroperationen bei Threads ohne FPU-Nutzung,
- potenziell schnellere Kontextwechsel.

#### Nachteile

- komplexe Fault- und Besitzermodelle,
- zusätzliche Sicherheitsrisiken,
- schwer vorhersehbare Latenzen,
- problematisch für Echtzeit,
- anfällig für Seitenkanäle und Zustandslecks.

Diese Alternative wird nicht als allgemeiner Standard verwendet.

---

### 5.4 Hybrides architekturabhängiges Context-Switch-Modell

Der Kernkontext wird bei jedem Wechsel gespeichert. Erweiterte Zustände werden über explizite Nutzungsmasken und architekturspezifische Mechanismen kontrolliert gesichert.

#### Vorteile

- vollständige Korrektheit,
- kontrollierbare Performance,
- Unterstützung zukünftiger CPU-Erweiterungen,
- klare Echtzeitregeln,
- gute Sicherheitskontrolle.

#### Nachteile

- höhere Implementierungskomplexität,
- architekturspezifische Zustandsverwaltung erforderlich,
- Zustandsmasken und Speicherbereiche müssen versioniert werden.

Diese Alternative wird gewählt.

---

## 6. Entscheidung

NovaOS verwendet ein hybrides, threadbasiertes und architekturgekapseltes Context-Switch-Modell.

Bei jedem Threadwechsel wird ein obligatorischer Kernkontext gesichert.

Dieser enthält mindestens:

```text
stack_pointer
instruction_pointer oder return_state
callee_saved_registers
processor_status
kernel_stack_reference
thread_local_state
address_space_reference
```

Architekturspezifische und erweiterte Zustände werden über zusätzliche Kontextbereiche verwaltet.

Der Context Switch wird konzeptionell in zwei Ebenen getrennt:

1. generische Scheduler- und Threadverwaltung,
2. architekturspezifisches Sichern und Wiederherstellen des CPU-Zustands.

Die generische Ebene darf keine direkten Annahmen über konkrete Register oder Seitentabellenformate treffen.

---

## 7. Planbare Einheit

Die direkt planbare Einheit bleibt der Thread gemäß ADR-PROC-0002.

Der Scheduler wechselt ausschließlich zwischen Threads.

Ein Prozesswechsel entsteht indirekt, wenn der nächste Thread einem anderen Prozess oder Adressraum angehört.

```text
Scheduler
   ↓
wählt Thread
   ↓
Thread bestimmt Prozess
   ↓
Prozess bestimmt Adressraum
```

Ein Prozess besitzt keinen eigenständigen speicherbaren CPU-Registerkontext.

---

## 8. Context-Switch-Arten

NovaOS unterscheidet mindestens folgende Wechselarten:

| Wechselart | Beschreibung |
|---|---|
| Threadwechsel im selben Prozess | unterschiedlicher Thread, gleicher Adressraum |
| Prozesswechsel | unterschiedlicher Thread und anderer Adressraum |
| Wechsel zu Kernelthread | Ziel besitzt keinen Userspace-Kontext |
| Wechsel zum Idle Thread | keine reguläre Arbeit vorhanden |
| Wechsel vom Idle Thread | reguläre Arbeit wurde verfügbar |
| CPU-Migration | Thread wird auf einer anderen CPU fortgesetzt |
| Selbstwechsel | Scheduler wählt erneut denselben Thread |

Ein Selbstwechsel soll nach Möglichkeit erkannt und ohne vollständige Context-Switch-Sequenz beendet werden.

---

## 9. Auslöser eines Context Switch

Ein Context Switch kann ausgelöst werden durch:

- Ablauf einer Zeitscheibe,
- freiwilliges Yield,
- Blockierung,
- Warten auf I/O,
- Warten auf IPC,
- Warten auf Synchronisationsobjekte,
- Aufwecken eines höher priorisierten Threads,
- Deadline Scheduling,
- Budgetüberschreitung,
- Threadbeendigung,
- Affinitätsänderung,
- CPU-Hotplug,
- Lastverteilung,
- Energie- oder Temperaturpolitik,
- Rückkehr aus einem Interrupt,
- Rückkehr aus einem Systemaufruf.

Der eigentliche Wechsel erfolgt nur an einem sicheren Scheduling-Punkt.

---

## 10. Sichere Scheduling-Punkte

Ein Context Switch darf nur erfolgen, wenn die aktuelle CPU einen konsistenten Kernelzustand besitzt.

Ein sicherer Scheduling-Punkt erfordert mindestens:

- gültigen aktuellen Thread,
- gültigen Kernelstack,
- konsistenten Trap- oder Switch Frame,
- keine unzulässigen gehaltenen CPU-lokalen Sperren,
- erlaubten Präemptionszustand,
- korrekt gesicherte Interruptverschachtelung,
- gültigen Schedulerzustand.

Ist Präemption deaktiviert, wird eine Rescheduling-Anforderung vorgemerkt.

Der Wechsel erfolgt, sobald der nächste sichere Punkt erreicht wird.

---

## 11. Obligatorischer Kernkontext

Der obligatorische Kernkontext enthält alle Zustände, die für den Wechsel zwischen Kernel-Ausführungskontexten erforderlich sind.

Typische Bestandteile sind:

- Stack Pointer,
- Rücksprungadresse oder Instruction Pointer,
- callee-saved Register,
- Statusregister,
- Kernelstackgrenzen,
- Threadzeiger,
- architekturspezifische Kontrollwerte.

Caller-saved Register müssen bereits durch den Aufrufpfad, Trap Frame oder die ABI gesichert sein, wenn sie nach dem Wechsel noch benötigt werden.

Die genaue Aufteilung wird pro Architektur festgelegt.

---

## 12. Trap Frame und Switch Frame

Trap Frame und Switch Frame werden getrennt definiert.

### 12.1 Trap Frame

Der Trap Frame entsteht typischerweise bei:

- Interrupts,
- Exceptions,
- Systemaufrufen,
- Übergängen aus dem Userspace.

Er enthält den Zustand, der für die Rückkehr aus dem Kernel benötigt wird.

### 12.2 Switch Frame

Der Switch Frame entsteht beim tatsächlichen Schedulerwechsel.

Er enthält den minimalen Zustand, der benötigt wird, um die Kernel-Ausführung des Threads wieder aufzunehmen.

Ein Thread kann gleichzeitig einen Trap Frame auf seinem Kernelstack und einen Switch Frame als aktuellen Schedulerkontext besitzen.

---

## 13. Kernelstack-Wechsel

Jeder Thread besitzt gemäß ADR-MM-0015 einen eigenen Kernelstack.

Beim Context Switch wird:

1. der aktuelle Kernel-Stack-Pointer im Threadobjekt gespeichert,
2. der Zielthread als aktuell markiert,
3. der Kernel-Stack-Pointer des Zielthreads geladen,
4. die CPU-lokale Kernelstackreferenz aktualisiert,
5. gegebenenfalls der privilegierte Stackeintrag der CPU aktualisiert.

Nach dem Stackwechsel darf der alte Stack nicht mehr ohne explizite Referenz verwendet werden.

Lokale Variablen der Switch-Funktion können sich auf dem alten Stack befinden. Der architekturspezifische Code muss deshalb exakt definieren, an welchem Punkt der Stackwechsel erfolgt.

---

## 14. Adressraumwechsel

Besitzen alter und neuer Thread unterschiedliche Adressräume, führt der Kernel einen Adressraumwechsel aus.

Der Wechsel umfasst je nach Architektur:

- Laden der Seitentabellenwurzel,
- Wechsel der Adressraumkennung,
- TLB-Behandlung,
- Aktualisierung CPU-lokaler Adressraumreferenzen,
- Sicherheitsbarrieren,
- Aktualisierung architekturspezifischer Kontrollregister.

Besitzen beide Threads denselben Adressraum, wird der Wechsel vermieden.

```text
previous.address_space == next.address_space
    → kein vollständiger Adressraumwechsel
```

---

## 15. Kernelthreads

Kernelthreads besitzen keinen normalen Userspace-Adressraum.

NovaOS verwendet für Kernelthreads einen definierten Kerneladressraum beziehungsweise eine sichere Adressraumreferenz.

Beim Wechsel:

```text
Userspace-Thread → Kernelthread
```

wird der Userspace-Adressraum deaktiviert oder durch den Kerneladressraum ersetzt, soweit die Architektur dies erfordert.

Ein Kernelthread darf nicht unbeabsichtigt auf den Userspace-Adressraum des zuvor laufenden Threads zugreifen.

---

## 16. TLB-Verwaltung

Ein Adressraumwechsel kann Auswirkungen auf den Translation Lookaside Buffer besitzen.

NovaOS verwendet nach Möglichkeit hardwareseitige Adressraumkennungen wie:

- PCID,
- ASID,
- vergleichbare architekturspezifische Kennungen.

Dadurch können TLB-Einträge verschiedener Adressräume gleichzeitig erhalten bleiben.

Ein vollständiger TLB-Flush erfolgt nur, wenn er tatsächlich erforderlich ist.

Gründe können sein:

- keine Adressraumkennungen verfügbar,
- Wiederverwendung einer Kennung,
- globale Änderung der Seitentabellen,
- Sicherheitsanforderung,
- architekturspezifische Einschränkung.

---

## 17. Adressraumgenerationen

Jeder Adressraum besitzt eine Generation.

Die Generation wird erhöht, wenn Änderungen eine erneute TLB-Synchronisation erfordern.

Jede CPU speichert, welche Generation eines Adressraums sie zuletzt verwendet hat.

Beim Context Switch prüft der Kernel:

```text
cpu_seen_generation != address_space_generation
    → erforderliche TLB-Synchronisation durchführen
```

Dadurch werden unnötige globale TLB-Flushes vermieden.

---

## 18. FPU- und SIMD-Zustand

Moderne Prozessoren besitzen umfangreiche erweiterte Registerzustände.

Dazu gehören beispielsweise:

- FPU,
- MMX,
- SSE,
- AVX,
- AVX-512,
- ARM NEON,
- SVE,
- Matrix- oder KI-Erweiterungen.

NovaOS speichert für jeden Thread einen eigenen erweiterten CPU-Zustand, sobald der Thread entsprechende Funktionen verwendet.

Der Zustand darf niemals zwischen nicht autorisierten Threads sichtbar bleiben.

---

## 19. Eager Save als Sicherheitsstandard

NovaOS verwendet für aktiv genutzte FPU-, SIMD- und Vektorzustände standardmäßig ein sicheres Eager-Save/Eager-Restore-Modell.

Das bedeutet:

- der Zustand des alten Threads wird beim Wechsel gesichert,
- der Zustand des neuen Threads wird vor dessen Nutzung wiederhergestellt,
- Registerinhalte werden nicht unkontrolliert zwischen Threads weitergereicht.

Diese Entscheidung reduziert:

- Zustandslecks,
- komplexe Fault-Pfade,
- unvorhersehbare Echtzeitlatenzen,
- Seitenkanalrisiken.

Architekturspezifische Optimierungen bleiben zulässig, wenn sie dieselben Sicherheits- und Determinismusanforderungen erfüllen.

---

## 20. Dynamische Zustandsgröße

Der erweiterte CPU-Zustand kann je nach Hardware unterschiedlich groß sein.

Der Thread Context darf deshalb nicht von einer festen universellen Zustandsgröße ausgehen.

Jeder erweiterte Kontext enthält mindestens:

```text
format
version
size
feature_mask
alignment
storage
```

Der Speicherbereich wird entsprechend den Anforderungen der CPU ausgerichtet.

Ein Thread darf nur auf CPUs migriert werden, die seinen erforderlichen Erweiterungszustand unterstützen oder eine definierte kompatible Darstellung bereitstellen.

---

## 21. FPU-Nutzung im Kernel

Kernelcode darf FPU-, SIMD- oder Vektorregister nicht unkontrolliert verwenden.

Ein Kernelabschnitt, der solche Register benötigt, muss eine explizite Kernel-FPU-Schnittstelle verwenden.

Beispiel:

```c
nova_status_t status = nova_kernel_fpu_begin();

if (status == NOVA_STATUS_OK)
{
    /* begrenzte FPU- oder SIMD-Arbeit */

    nova_kernel_fpu_end();
}
```

Diese Schnittstelle muss:

- den Threadzustand schützen,
- Präemption entsprechend kontrollieren,
- Verschachtelung definieren,
- Interruptnutzung begrenzen,
- Echtzeitkosten berücksichtigen.

---

## 22. Thread Local Storage

Beim Context Switch muss der Thread-Local-Storage-Zustand des Zielthreads aktiviert werden.

Dazu können gehören:

- TLS-Basisregister,
- Userspace-Threadzeiger,
- Kernel-Threadzeiger,
- architekturspezifische Segment- oder Pointerregister.

Userspace-TLS und Kernel-TLS werden getrennt behandelt.

Der aktuelle Kernelthread muss über eine CPU-lokale oder architekturspezifisch sichere Referenz erreichbar sein.

---

## 23. CPU-lokaler Current-Thread-Zeiger

Jede CPU besitzt eine Referenz auf ihren aktuell laufenden Thread.

Beim Context Switch wird diese Referenz atomar beziehungsweise in einer architekturspezifisch sicheren Reihenfolge aktualisiert.

Konzeptionell:

```c
cpu->previous_thread = previous;
cpu->current_thread = next;
```

Nach der Veröffentlichung von `current_thread` müssen alle für den Zielthread erforderlichen CPU-lokalen Zustände konsistent sein.

Interrupt- und Exceptionpfade dürfen niemals einen teilweise aktualisierten Threadzustand beobachten.

---

## 24. Debugregister

Hardware-Debugregister werden nur für Threads aktiviert, die entsprechende Debugzustände besitzen.

Beim Wechsel können erforderlich sein:

- Speichern alter Breakpoints,
- Laden neuer Breakpoints,
- Deaktivieren nicht benötigter Debugregister,
- Löschen sicherheitsrelevanter Restzustände.

Ein Thread ohne Debugkontext darf keine Breakpoints des vorherigen Threads übernehmen.

Debugzustände unterliegen dem Capability- und Debugger-Sicherheitsmodell.

---

## 25. Sicherheitszustände

Architekturspezifische Sicherheitsmechanismen können Teil des Context Switch sein.

Beispiele:

- Control-flow Enforcement,
- Shadow Stacks,
- Pointer Authentication,
- Memory Protection Keys,
- Tagging-Zustände,
- Branch-Prediction-Schutz,
- spekulative Ausführungsbarrieren,
- Userspace-Zugriffssteuerung.

Die Architekturkomponente muss deklarieren, welche Zustände:

- pro Thread,
- pro Prozess,
- pro CPU,
- pro Sicherheitsdomäne

verwaltet werden.

Ein Context Switch zwischen Sicherheitsdomänen kann zusätzliche Schutzmaßnahmen erfordern.

---

## 26. Spekulative Ausführung

Bei einem Wechsel zwischen nicht vertrauensgleichen Sicherheitsdomänen können architekturspezifische Maßnahmen gegen spekulative Seitenkanäle erforderlich sein.

Mögliche Maßnahmen sind:

- Branch-Predictor-Barrieren,
- indirekte Branch-Schutzmechanismen,
- kontrollierte Cache- oder TLB-Behandlung,
- Core-Scheduling,
- SMT-Trennung,
- architekturspezifische Mitigations.

Diese Maßnahmen werden nur entsprechend der aktiven Sicherheitsrichtlinie und Hardwareanfälligkeit aktiviert.

Die generische Context-Switch-Schnittstelle stellt dafür einen Domänenwechselhinweis bereit.

---

## 27. Präemption

NovaOS unterstützt präemptive Kontextwechsel.

Ein Thread kann verdrängt werden, wenn:

- ein höher priorisierter Thread lauffähig wird,
- seine Zeitscheibe endet,
- sein Echtzeitbudget verbraucht ist,
- eine Scheduler-Richtlinie einen Wechsel verlangt.

Präemption ist nur zulässig, wenn der Präemptionszähler der CPU beziehungsweise des Threads dies erlaubt.

Konzeptionell:

```text
preempt_disable_count == 0
    → Präemption erlaubt

preempt_disable_count > 0
    → Rescheduling vormerken
```

Ein Präemptionszähler darf nicht als Ersatz für korrekte Synchronisation missbraucht werden.

---

## 28. Interruptkontext

Ein normaler Threadwechsel erfolgt nicht mitten in einem unvollständigen unmittelbaren Interruptpfad.

Der Interruptpfad kann eine Rescheduling-Anforderung setzen.

Der eigentliche Wechsel erfolgt:

- am Ende des äußersten Interrupts,
- an einem sicheren Rückkehrpunkt,
- nach Wiederherstellung eines konsistenten Kernelzustands.

Verschachtelte Interrupts müssen vollständig berücksichtigt werden.

Ein Interruptstack darf nicht versehentlich als dauerhafter Threadstack gespeichert werden, wenn die Architektur getrennte Interruptstacks verwendet.

---

## 29. Systemaufrufkontext

Ein Systemaufruf kann blockieren und dadurch einen Context Switch auslösen.

Der Userspace-Zustand bleibt im Trap Frame des Threads gespeichert.

Nach dem erneuten Einplanen setzt der Thread seine Kernel-Ausführung fort und kehrt anschließend über den kontrollierten Systemaufruf-Rückkehrpfad in den Userspace zurück.

Vor der Rückkehr werden mindestens geprüft:

- Zieladressraum,
- Userspace Instruction Pointer,
- Userspace Stack Pointer,
- Statusregister,
- ausstehende Signale oder Ereignisse,
- Sicherheitszustand,
- Rückkehrmodus.

---

## 30. Freiwilliger Context Switch

Ein freiwilliger Context Switch erfolgt beispielsweise durch:

```c
nova_thread_yield();
nova_wait_object(...);
nova_ipc_receive(...);
nova_mutex_lock(...);
```

Der Thread muss vor dem Wechsel einen konsistenten Zustand besitzen.

Eine blockierende Operation führt konzeptionell folgende Schritte aus:

1. Wartebedingung registrieren,
2. Threadzustand auf blockiert setzen,
3. Run Queue aktualisieren,
4. Scheduler aufrufen,
5. Context Switch durchführen.

Die Registrierung der Wartebedingung und die Zustandsänderung müssen verlorene Wake-ups verhindern.

---

## 31. Erzwungener Context Switch

Bei einem erzwungenen Context Switch bleibt der Thread grundsätzlich lauffähig.

Der Scheduler:

1. beendet die aktuelle Laufzeitabrechnung,
2. setzt den Thread gegebenenfalls zurück in `READY`,
3. reiht ihn nach Klassen- und Prioritätsregeln ein,
4. wählt den nächsten Thread,
5. führt den Context Switch aus.

Ein erzwungener Wechsel darf keine unvollständigen Threadzustände hinterlassen.

---

## 32. Selbstwechsel

Wählt der Scheduler denselben Thread erneut aus, wird kein vollständiger Context Switch ausgeführt.

Stattdessen werden lediglich erforderliche Scheduler- und Abrechnungsdaten aktualisiert.

```text
previous == next
    → Context Switch überspringen
```

Ausstehende Zustandsarbeiten, etwa eine TLB-Generation oder Sicherheitsaktualisierung, dürfen dadurch nicht übergangen werden.

---

## 33. Wechsel zum Idle Thread

Ist kein anderer zulässiger Thread verfügbar, wählt der Scheduler den CPU-lokalen Idle Thread gemäß ADR-PROC-0006.

Der Wechsel zum Idle Thread kann beinhalten:

- Abschluss der Abrechnung des vorherigen Threads,
- Aktivierung des Kerneladressraums,
- Laden des Idle-Kernelstacks,
- Aktualisierung des Current-Thread-Zeigers,
- Vorbereitung der CPU-Energieverwaltung.

Der Idle Thread besitzt keinen Userspace-Kontext.

---

## 34. Wechsel vom Idle Thread

Wird ein regulärer Thread lauffähig, wird der Idle Thread verdrängt.

Der Wechsel umfasst:

- Verlassen des Hardware-Idle-Zustands,
- Aktualisierung der Idle-Zeit,
- Auswahl des Zielthreads,
- Laden seines Kernel- und gegebenenfalls Userspace-Kontexts,
- Wiederherstellung seines Adressraums,
- Start der Laufzeitabrechnung.

Der Idle Thread wird nicht in eine normale Run Queue zurückgestellt.

---

## 35. CPU-Migration

Bei einer Migration wird ein Thread auf einer anderen CPU fortgesetzt.

Vor der Migration müssen:

- alle CPU-lokalen Zustände neutral gespeichert sein,
- die Affinitätsgeneration geprüft werden,
- der Thread von der alten Run Queue entfernt sein,
- architekturspezifische Zustände migrierbar sein,
- Ziel-CPU und CPU-Fähigkeiten kompatibel sein.

Nach der Migration werden:

- der Thread auf der Ziel-CPU eingereiht,
- CPU-lokale Referenzen aktualisiert,
- notwendige TLB- oder Adressraumzustände hergestellt,
- Migrationsstatistiken aktualisiert.

Ein Thread darf niemals gleichzeitig auf zwei CPUs laufen.

---

## 36. Migration zwischen unterschiedlichen CPU-Typen

Auf heterogenen Systemen können CPUs unterschiedliche Register- oder Erweiterungsfähigkeiten besitzen.

Vor einer Migration prüft der Kernel:

```text
thread.required_cpu_features
    ⊆
target_cpu.available_features
```

Ist dies nicht erfüllt, ist die CPU kein gültiges Migrationsziel.

Alternativ kann ein Prozess eine kontrollierte kompatible Codevariante aktivieren und nicht mehr benötigte erweiterte Zustände explizit ablegen. Dies darf nicht automatisch und unbemerkt während eines Context Switch erfolgen.

---

## 37. Echtzeit-Context-Switch

Für Echtzeitthreads gelten zusätzliche Anforderungen:

- begrenzte maximale Switch-Latenz,
- keine unbeschränkte Speicherallokation,
- vorab reservierter Kontextspeicher,
- kein unvorhersehbarer Lazy-FPU-Fault,
- kontrollierte TLB-Kosten,
- definierte CPU-Fähigkeiten,
- messbare Interrupt- und Schedulerlatenzen.

Die Admission Control muss die erwarteten Context-Switch-Kosten in die Echtzeitkapazität einbeziehen.

Ein Wechsel zwischen großen Vektorzuständen kann die Echtzeitkosten erheblich erhöhen und muss entsprechend berücksichtigt werden.

---

## 38. Kontextspeicher

Alle für einen Context Switch erforderlichen Speicherbereiche werden vorab bereitgestellt.

Der Context-Switch-Pfad darf keine normale dynamische Speicherallokation ausführen.

Vorab reserviert werden mindestens:

- Kernelstack,
- Kernkontext,
- Trap-Frame-Bereich,
- erforderlicher erweiterter CPU-Zustand,
- Sicherheitskontext,
- Debugkontext, falls aktiviert.

Kann ein benötigter Kontextspeicher bei Threaderzeugung nicht bereitgestellt werden, wird die Threaderzeugung abgelehnt.

---

## 39. Kontextformat und Versionierung

Architekturspezifische Kontextformate werden versioniert.

Ein Kontextkopf kann enthalten:

```c
typedef struct
{
    uint32_t format;
    uint32_t version;
    uint32_t size;
    uint32_t alignment;

    uint64_t feature_mask;
    uint64_t flags;
} nova_context_header_t;
```

Dadurch können zukünftige CPU-Erweiterungen ergänzt werden, ohne alte gespeicherte Kontextformate unkontrolliert falsch zu interpretieren.

Innerhalb eines laufenden Kernels müssen aktive Kontextformate vollständig unterstützt werden.

---

## 40. Scheduler-Schnittstelle

Die generische Schedulerlogik verwendet eine architekturunabhängige Switch-Schnittstelle.

Konzeptionell:

```c
nova_thread_t* nova_scheduler_switch(
    nova_thread_t* previous,
    nova_thread_t* next
);
```

Die architekturspezifische Ebene kann eine Schnittstelle wie folgt bereitstellen:

```c
void nova_arch_context_switch(
    nova_arch_context_t* previous,
    const nova_arch_context_t* next
);
```

Der genaue Rückgabepunkt einer Low-Level-Switch-Funktion muss dokumentiert sein, da sie nach dem Stackwechsel im Kontext des neuen Threads zurückkehren kann.

---

## 41. Switch-Reihenfolge

Ein Context Switch folgt konzeptionell dieser Reihenfolge:

```text
Schedulerentscheidung fixieren
        ↓
Laufzeit des alten Threads abrechnen
        ↓
alten Threadzustand aktualisieren
        ↓
erweiterten Zustand sichern
        ↓
Adressraumwechsel vorbereiten
        ↓
Current-Thread-Zeiger aktualisieren
        ↓
Kernelstack wechseln
        ↓
Adressraum des Zielthreads aktivieren
        ↓
erweiterten Zustand wiederherstellen
        ↓
Laufzeit des Zielthreads starten
        ↓
Ausführung des Zielthreads fortsetzen
```

Die exakte Reihenfolge kann architekturbedingt abweichen.

Sicherheits- und Sichtbarkeitsinvarianten müssen jedoch jederzeit eingehalten werden.

---

## 42. Sperren während des Context Switch

Der Scheduler benötigt CPU-lokale oder Run-Queue-Sperren zur Auswahl und Zustandsänderung.

Diese Sperren dürfen nicht unkontrolliert in den Zielthread übertragen werden.

Es muss eindeutig definiert sein:

- welche Sperre vor dem Switch gehalten wird,
- auf welchem Stack sie freigegeben wird,
- ob der Zielthread die Freigabe abschließt,
- welche Interruptzustände gelten.

Der Context-Switch-Pfad darf keine Sperre halten, auf die der Zielthread unmittelbar blockierend angewiesen ist.

---

## 43. Interruptzustand

Der Interruptzustand muss während des Context Switch kontrolliert sein.

Der Low-Level-Switch erfolgt typischerweise mit lokal deaktivierten Interrupts oder unter einem äquivalent geschützten Zustand.

Vor der Fortsetzung des Zielthreads wird dessen erwarteter Kernel-Interruptzustand hergestellt.

Ein Thread darf nicht versehentlich den temporären Interruptzustand eines anderen Threads übernehmen.

Der endgültige Userspace-Interruptzustand wird ausschließlich über den validierten Trap Frame wiederhergestellt.

---

## 44. Speicherbarrieren

Auf Mehrkernsystemen sind Speicherbarrieren erforderlich, damit Änderungen an:

- Threadzuständen,
- Run Queues,
- Current-Thread-Zeigern,
- Affinitäten,
- Adressräumen,
- Migrationszuständen

in der korrekten Reihenfolge sichtbar werden.

Der generische Code beschreibt die benötigte Ordnungssemantik.

Die Architekturkomponente setzt diese mit geeigneten Barrieren und atomaren Operationen um.

Compilerbarrieren allein sind nicht ausreichend, wenn Hardware-Reordering möglich ist.

---

## 45. Laufzeitabrechnung

Vor dem Context Switch wird die Laufzeit des alten Threads abgeschlossen.

Nach dem Switch beginnt die Abrechnung des neuen Threads.

Erfasst werden mindestens:

- Userspace-Zeit,
- Kernelzeit,
- Echtzeitbudget,
- Scheduler-Klassenzeit,
- Idle-Zeit,
- freiwillige Kontextwechsel,
- erzwungene Kontextwechsel,
- Migrationen,
- Switch-Latenz.

Die Zeitmessung verwendet eine monotone CPU- oder Systemzeitquelle.

---

## 46. Context-Switch-Zähler

Jeder Thread besitzt mindestens folgende Zähler:

```text
voluntary_switch_count
involuntary_switch_count
migration_count
address_space_switch_count
extended_state_switch_count
```

Jede CPU besitzt zusätzlich:

```text
total_switch_count
idle_switch_count
process_switch_count
same_process_switch_count
failed_migration_count
```

Diese Zähler dienen der Diagnose und Leistungsanalyse.

---

## 47. Telemetrie

NovaOS kann Context-Switch-Ereignisse in einem CPU-lokalen Ringpuffer erfassen.

Ein Ereignis kann enthalten:

```text
timestamp
cpu_id
previous_thread_id
next_thread_id
previous_process_id
next_process_id
switch_reason
previous_state
scheduler_class
priority
address_space_changed
extended_state_saved
migration
duration
```

Die vollständige Ereignisaufzeichnung kann konfigurierbar sein, da Context Switches sehr häufig auftreten.

Im Standardbetrieb werden bevorzugt aggregierte Zähler verwendet.

---

## 48. Performanceanforderungen

Der Context-Switch-Pfad muss:

- ohne normale Heap-Allokation arbeiten,
- möglichst wenige globale Sperren verwenden,
- CPU-lokale Daten bevorzugen,
- unnötige Adressraumwechsel vermeiden,
- unnötige TLB-Flushes vermeiden,
- nur erforderliche Erweiterungszustände sichern,
- keine blockierende Telemetrie ausführen.

Performanceoptimierungen dürfen niemals:

- Registerzustände verlieren,
- Sicherheitsbarrieren umgehen,
- fremde FPU-Daten sichtbar machen,
- Adressraumgrenzen verletzen,
- Echtzeitinvarianten brechen.

---

## 49. Fehlerbehandlung

Folgende Zustände gelten als Fehler:

- fehlender Zielthread,
- ungültiger Zielzustand,
- ungültiger Kernelstack,
- nicht ausgerichteter Kontextspeicher,
- inkompatibler CPU-Erweiterungszustand,
- ungültiger Adressraum,
- Migration auf eine unzulässige CPU,
- Thread gleichzeitig auf mehreren CPUs,
- beschädigter Trap Frame,
- unbekanntes Kontextformat,
- inkonsistente Präemptions- oder Interruptzähler.

Wird ein ungültiger Userspace-Rückkehrzustand erkannt, darf der Kernel:

- die Rückkehr verweigern,
- den Thread beenden,
- den Prozess beenden,
- ein Diagnoseereignis erzeugen.

Ein Kernel-Panic ist zulässig, wenn der Kernel keinen sicheren Ausführungskontext mehr herstellen kann.

---

## 50. Sicherheitsmodell

Context Switching ist ausschließlich eine Kerneloperation.

Userspace darf:

- keinen Switch Frame direkt verändern,
- keine Kernelregisterwerte festlegen,
- keinen fremden Kontext laden,
- keine Seitentabellenwurzel direkt bestimmen,
- keinen erweiterten Zustand eines anderen Threads lesen,
- keine Sicherheitsbarrieren deaktivieren.

Userspace darf nur dokumentierte Teile seines eigenen Rückkehrkontexts über kontrollierte APIs verändern.

Alle Werte werden vor der Rückkehr validiert.

---

## 51. Konsequenzen

### 51.1 Positive Konsequenzen

- klare Trennung zwischen generischem und architekturspezifischem Code,
- sichere Wiederherstellung von Threadkontexten,
- optimierte Wechsel innerhalb desselben Prozesses,
- kontrollierte TLB-Verwaltung,
- sichere FPU- und SIMD-Zustände,
- Unterstützung moderner CPU-Erweiterungen,
- gute Mehrkern- und Echtzeitintegration,
- vollständige Diagnosefähigkeit,
- stabile Grundlage für mehrere CPU-Architekturen.

### 51.2 Negative Konsequenzen

- hohe Implementierungskomplexität,
- umfangreiche architekturspezifische Assembly-Anteile,
- große Erweiterungszustände können Context Switches verteuern,
- Sicherheitsmitigationen erhöhen die Switch-Kosten,
- heterogene CPUs erschweren Migrationen,
- Fehler sind häufig schwer reproduzierbar.

### 51.3 Risiken

- beschädigte Kernelstacks,
- Registerverlust,
- falscher Adressraum,
- FPU- oder SIMD-Datenlecks,
- TLB-Inkonsistenzen,
- verlorene Rescheduling-Ereignisse,
- doppeltes Ausführen eines Threads,
- fehlerhafte Rückkehr in den Userspace,
- unvollständige Sicherheitsbarrieren.

Diese Risiken werden begrenzt durch:

- klare Kontextformate,
- Guard Pages,
- Generationen,
- CPU-lokale Zustände,
- Eager Save für aktive Erweiterungszustände,
- atomare Schedulerübergänge,
- umfangreiche Invarianten- und Stresstests.

---

## 52. Auswirkungen auf andere Subsysteme

### 52.1 Scheduler

Der Scheduler bestimmt alten und neuen Thread und ruft den Context-Switch-Pfad an einem sicheren Punkt auf.

### 52.2 Threadverwaltung

Jeder Thread benötigt Kernkontext, Kernelstack und gegebenenfalls erweiterte Zustandsbereiche.

### 52.3 Prozessverwaltung

Der Prozess liefert den Adressraum und prozessbezogene Sicherheitszustände.

### 52.4 Speicherverwaltung

Die Speicherverwaltung muss Adressraumwechsel, TLB-Generationen und Seitentabellenwurzeln bereitstellen.

### 52.5 CPU-Affinität

Migrationen dürfen nur auf CPUs erfolgen, die mit Affinität und benötigten CPU-Fähigkeiten vereinbar sind.

### 52.6 Echtzeitscheduler

Switch-Kosten müssen begrenzt, messbar und bei der Kapazitätsplanung berücksichtigt werden.

### 52.7 Interruptsystem

Interrupt- und Exceptionpfade müssen gültige Trap Frames bereitstellen und sichere Rescheduling-Punkte definieren.

### 52.8 Debugger

Debugger benötigen kontrollierten Zugriff auf gespeicherte Register- und Erweiterungszustände.

### 52.9 Sicherheitssystem

Sicherheitsdomänenwechsel können zusätzliche Barrieren oder Mitigations auslösen.

### 52.10 Telemetrie

Context-Switch-Ereignisse müssen CPU-lokal und nicht blockierend erfasst werden.

---

## 53. Implementierungsrichtlinien

1. Zunächst wird ein minimaler architekturspezifischer Kernkontext implementiert.
2. Jeder Thread erhält vor seiner ersten Ausführung einen vollständig initialisierten Kontext.
3. Der Context-Switch-Pfad führt keine Heap-Allokation aus.
4. Wechsel innerhalb desselben Adressraums vermeiden unnötige Seitentabellenwechsel.
5. FPU- und SIMD-Unterstützung wird erst nach korrektem Kernkontext ergänzt.
6. Aktive erweiterte Zustände werden standardmäßig sicher und deterministisch gespeichert.
7. TLB-Optimierungen werden erst nach funktionierender Generationenverwaltung aktiviert.
8. Der Current-Thread-Zeiger ist CPU-lokal.
9. Ein Thread darf niemals gleichzeitig auf mehreren CPUs als `RUNNING` markiert sein.
10. Selbstwechsel werden ohne vollständigen Low-Level-Switch behandelt.
11. Architektur-Assembly bleibt klein und wird durch C-kompatible Strukturen beschrieben.
12. Strukturgrößen und Offsets werden zur Build-Zeit geprüft.
13. Context-Switch-Code wird mit deaktivierter Instrumentierung kompiliert, wenn Instrumentierung Rekursion verursachen könnte.
14. Sicherheitsoptimierungen dürfen erst nach nachgewiesener Gleichwertigkeit aktiviert werden.
15. Jede unterstützte CPU-Architektur erhält eigene Context-Switch-Tests.

---

## 54. Testanforderungen

Mindestens folgende Szenarien müssen getestet werden:

- Wechsel zwischen zwei Kernelthreads,
- Wechsel zwischen zwei Threads desselben Prozesses,
- Wechsel zwischen unterschiedlichen Prozessen,
- Userspace zu Kernelthread,
- Kernelthread zu Userspace,
- Wechsel zum Idle Thread,
- Wechsel vom Idle Thread,
- freiwilliger Context Switch,
- erzwungener Context Switch,
- Selbstwechsel,
- Blockierung im Systemaufruf,
- Präemption am Ende eines Interrupts,
- verschachtelte Interrupts,
- Threadmigration,
- Affinitätsänderung während der Ausführung,
- CPU-Hotplug,
- CPU-Ausfall,
- allgemeine Registermuster,
- Stack Pointer und Instruction Pointer,
- FPU-Zustand,
- SIMD- und Vektorzustand,
- Thread Local Storage,
- Debugregister,
- unterschiedliche Adressräume,
- TLB-Generationen,
- Adressraumkennungen,
- große Anzahl schneller Context Switches,
- Echtzeit-Switch-Latenz,
- Wechsel unter hoher Interruptlast,
- Kernelstack-Guard-Page-Auslösung,
- ungültiger Trap Frame,
- inkompatible Ziel-CPU,
- parallele Migration desselben Threads,
- Sicherheitsdomänenwechsel,
- korrekte Laufzeitabrechnung,
- korrekte Context-Switch-Zähler,
- Virtualisierung und emulierte CPUs.

---

## 55. Nicht Bestandteil dieser Entscheidung

Diese ADR legt nicht abschließend fest:

- konkrete Registerlayouts einzelner CPU-Architekturen,
- genaue Assembly-Sequenzen,
- konkrete TLB-Flush-Befehle,
- vollständige Sicherheitsmitigations,
- genaue FPU- und Vektorzustandsformate,
- Context Switching virtueller Maschinen,
- persistente Checkpoint- oder Prozessmigration,
- die endgültige öffentliche Debug-ABI,
- exakte maximale Echtzeit-Switch-Latenzen.

Diese Details werden in architekturspezifischen ADRs und NPSPEC-Dokumenten definiert.

---

## 56. Verbindliche Invarianten

1. Der Thread ist die direkt planbare und umschaltbare Einheit.
2. Jede aktive CPU besitzt genau einen aktuellen Thread.
3. Ein Thread darf niemals gleichzeitig auf mehreren CPUs ausgeführt werden.
4. Jeder Thread besitzt vor seiner ersten Ausführung einen gültigen Kontext.
5. Jeder Context Switch erfolgt an einem sicheren Scheduling-Punkt.
6. Der Kernelstack des Zielthreads muss vor seiner Verwendung vollständig gültig sein.
7. Ein Prozesswechsel aktiviert den korrekten Zieladressraum.
8. Ein Threadwechsel im selben Adressraum darf unnötige Adressraumwechsel vermeiden.
9. Erweiterte CPU-Zustände dürfen nicht zwischen Threads sichtbar werden.
10. Der Context-Switch-Pfad darf keine normale Heap-Allokation ausführen.
11. Der Current-Thread-Zeiger muss auf jeder CPU konsistent sein.
12. Der Idle Thread darf nur durch den Idle Thread derselben CPU repräsentiert werden.
13. Eine Migration darf nur auf eine nach Affinität und CPU-Fähigkeiten zulässige CPU erfolgen.
14. Userspace darf keine Kernelkontexte oder fremden Threadkontexte direkt verändern.
15. Trap Frames werden vor der Rückkehr in den Userspace validiert.
16. Context-Switch-Abrechnung verwendet eine monotone Zeitquelle.
17. Ausstehende Sicherheits- oder TLB-Aktualisierungen dürfen durch einen Selbstwechsel nicht übersprungen werden.
18. Architekturabhängige Kontextformate müssen versioniert und größenvalidiert sein.
19. Fehlerhafte Kontextzustände dürfen nicht zu einer unkontrollierten Userspace-Rückkehr führen.
20. KI- oder lernbasierte Schedulerkomponenten dürfen Context-Switch-, Sicherheits-, Affinitäts- oder Echtzeitinvarianten nicht umgehen.

---

## 57. Referenzen

- ADR-PROC-0001 – Process Lifecycle
- ADR-PROC-0002 – Thread Lifecycle
- ADR-PROC-0003 – Scheduler Classes
- ADR-PROC-0004 – Prioritäten
- ADR-PROC-0005 – Echtzeitprozesse
- ADR-PROC-0006 – Idle Thread
- ADR-PROC-0007 – CPU Affinity
- ADR-KERNEL-0100 – Kernel Object Graph statt isolierter Subsysteme
- ADR-KERNEL-0101 – Ereignisgesteuerter Kernel-Core
- ADR-KERNEL-0102 – Einheitliches Objektmodell für Kernel und Userspace
- ADR-KERNEL-0103 – Native Capability-basierte Ressourcenverwaltung
- ADR-KERNEL-0104 – Integrierte Telemetrie- und Diagnoseschnittstellen
- ADR-KERNEL-0105 – ABI-Stabilität über versionierte Kernel-Services
- ADR-MM-0012 – NUMA Memory Policy
- ADR-MM-0013 – Guard Pages
- ADR-MM-0015 – Kernel Stack Layout
- NPSPEC-SCHEDULER-0001 – Scheduler Specification
- NPSPEC-SCHEDULER-AFFINITY-0001 – CPU Affinity Specification
- NPSPEC-CONTEXTSWITCH-0001 – Context Switching Specification
- NPSPEC-REALTIME-0001 – Realtime Process and Thread Specification
- NPSPEC-PROCESS-0001 – Process Specification
- NPSPEC-THREAD-0001 – Thread Specification

---

## 58. Zusammenfassung der Entscheidung

NovaOS verwendet ein hybrides, threadbasiertes und architekturgekapseltes Context-Switch-Modell.

Bei jedem Threadwechsel wird ein obligatorischer Kernkontext gesichert und der Kontext des Zielthreads wiederhergestellt. Erweiterte CPU-Zustände wie FPU, SIMD, Vektor-, Debug- und Sicherheitszustände werden über versionierte architekturspezifische Kontextbereiche verwaltet.

Ein Wechsel zwischen Threads desselben Prozesses vermeidet nach Möglichkeit einen unnötigen Adressraumwechsel. Bei einem Prozesswechsel werden Seitentabellen, Adressraumkennungen, TLB-Generationen und Sicherheitszustände kontrolliert aktualisiert.

Der Context-Switch-Pfad verwendet vorab reservierten Speicher, führt keine normale Heap-Allokation aus und stellt sicher, dass ein Thread niemals gleichzeitig auf mehreren CPUs ausgeführt wird.

Aktiv genutzte FPU- und SIMD-Zustände werden standardmäßig sicher und deterministisch gespeichert. Context Switches erfolgen ausschließlich an konsistenten Scheduling-Punkten und werden über CPU-lokale Zähler und optionale Telemetrie diagnostizierbar gemacht.