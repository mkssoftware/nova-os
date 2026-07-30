# ADR-PROC-0006 – Idle Thread

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-PROC-0006 |
| Titel | Idle Thread |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-30 |
| Bereich | Kernel / Threads / Scheduling / Energieverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-PROC-0002, ADR-PROC-0003, ADR-PROC-0004, ADR-PROC-0005 |
| Nachfolgende Spezifikation | NPSPEC-SCHEDULER-IDLE-0001 |

---

## 1. Kontext

Der NovaOS-Scheduler muss auf jeder aktiven logischen CPU jederzeit einen gültigen Thread auswählen können.

Normalerweise wählt er einen ausführbaren Thread aus einer Scheduler-Klasse:

```text
CRITICAL
REALTIME
INTERACTIVE
NORMAL
BATCH
BACKGROUND
```

Es kann jedoch vorkommen, dass auf einer CPU kein regulärer Thread ausführbar ist.

Beispiele:

- alle Threads warten auf Ereignisse,
- alle Threads sind blockiert,
- eine CPU wurde gerade gestartet,
- eine CPU ist von bestimmten Arbeitslasten isoliert,
- alle zur CPU passenden Threads laufen bereits auf anderen CPUs,
- das System befindet sich in einer Phase geringer Aktivität.

Der Scheduler darf in diesem Zustand weder einen ungültigen Thread auswählen noch ohne definierten Ausführungskontext weiterarbeiten.

Deshalb benötigt jede aktive logische CPU einen eigenen Idle Thread.

Der Idle Thread stellt sicher, dass der Scheduler immer einen gültigen Ausführungskontext besitzt. Gleichzeitig ist er die zentrale Schnittstelle zwischen Scheduler und CPU-Energieverwaltung, wenn auf einer CPU keine reguläre Arbeit vorhanden ist.

---

## 2. Problemstellung

Ohne einen definierten Idle Thread entstehen mehrere Architekturprobleme:

- Der Scheduler könnte eine leere Run Queue nicht einheitlich behandeln.
- Es gäbe keinen gültigen Threadkontext für eine unbeschäftigte CPU.
- CPU-lokale Threaddaten wären während des Leerlaufs nicht eindeutig.
- Energiesparzustände müssten als Sonderfall außerhalb des Threadmodells behandelt werden.
- Kontextwechsel in und aus dem Leerlauf wären schwer diagnostizierbar.
- CPU-Start und CPU-Hotplug benötigten separate Ausnahmepfade.
- Race Conditions zwischen Leerlaufprüfung und neu eintreffender Arbeit könnten zu verlorenen Wake-ups führen.
- Statistiken könnten Leerlaufzeit und tatsächliche Arbeitszeit nicht zuverlässig unterscheiden.

Der Idle Thread darf gleichzeitig nicht wie ein normaler Hintergrundthread behandelt werden.

Ein normaler Thread kann:

- blockieren,
- beendet werden,
- seine Priorität ändern,
- Ressourcen besitzen,
- Systemaufrufe ausführen,
- auf eine andere CPU migrieren.

Diese Eigenschaften sind für einen Idle Thread größtenteils unzulässig.

---

## 3. Entscheidungsziele

Die Idle-Thread-Architektur soll:

1. auf jeder aktiven CPU immer einen gültigen Ausführungskontext bereitstellen,
2. leere Run Queues ohne Sonderzustand behandeln,
3. einen sicheren Übergang in CPU-Energiesparzustände ermöglichen,
4. verlorene Wake-ups verhindern,
5. CPU-Start, CPU-Hotplug und CPU-Offlining unterstützen,
6. Leerlaufzeit präzise erfassen,
7. keine normale Hintergrundarbeit im Idle Thread ausführen,
8. keine unnötige CPU-Zeit verbrauchen,
9. Mehrkern- und heterogene Prozessoren unterstützen,
10. den Idle-Pfad möglichst klein und überprüfbar halten.

---

## 4. Begriffsdefinitionen

### 4.1 Idle Thread

Der Idle Thread ist ein spezieller Kernelthread, der ausgeführt wird, wenn auf seiner CPU kein anderer zulässiger Thread lauffähig ist.

---

### 4.2 Idle Loop

Die Idle Loop ist die Hauptschleife des Idle Threads.

Sie prüft, ob Arbeit verfügbar geworden ist, koordiniert den Eintritt in einen Energiesparzustand und kehrt nach einem Wake-up zum Scheduler zurück.

---

### 4.3 Idle State

Ein Idle State ist ein Hardwarezustand, in dem eine CPU bei reduzierter Aktivität auf ein Ereignis wartet.

Idle States können sich unterscheiden durch:

- Energieverbrauch,
- Eintrittslatenz,
- Austrittslatenz,
- Kontextverlust,
- Zeit bis zum Break-even-Punkt.

---

### 4.4 Wake-up

Ein Wake-up ist ein Ereignis, das die CPU aus einem Idle State zurückholt.

Mögliche Quellen sind:

- Timer,
- Interrupt,
- Inter-Processor Interrupt,
- Geräteereignis,
- Scheduler-Anforderung,
- CPU-Hotplug-Ereignis.

---

### 4.5 Tickless Idle

Tickless Idle bedeutet, dass periodische Scheduler-Ticks während längerer Leerlaufphasen ausgesetzt werden können.

Stattdessen wird der nächste tatsächlich benötigte Timerzeitpunkt programmiert.

---

## 5. Betrachtete Alternativen

### 5.1 Leere Scheduler-Auswahl ohne Thread

Der Scheduler gibt bei einer leeren Run Queue keinen Thread zurück und versetzt die CPU direkt in einen Wartezustand.

#### Vorteile

- kein zusätzliches Threadobjekt,
- geringer Speicherbedarf,
- scheinbar einfacher Leerlaufpfad.

#### Nachteile

- kein gültiger Threadkontext,
- zahlreiche Sonderfälle im Scheduler,
- schwierige Integration in Telemetrie und Debugging,
- komplizierter CPU-Start und CPU-Hotplug,
- erhöhtes Risiko inkonsistenter CPU-lokaler Zustände.

Diese Alternative wird verworfen.

---

### 5.2 Ein globaler Idle Thread

Das gesamte System verwendet einen einzelnen Idle Thread, der zwischen CPUs migriert.

#### Vorteile

- nur ein Threadobjekt,
- geringe Anzahl spezieller Kernelobjekte.

#### Nachteile

- ein Thread kann nicht gleichzeitig auf mehreren CPUs laufen,
- Migrationen wären ständig erforderlich,
- CPU-lokale Zustände wären schwer verwaltbar,
- für Mehrkernsysteme ungeeignet,
- Race Conditions beim parallelen Leerlauf mehrerer CPUs.

Diese Alternative wird verworfen.

---

### 5.3 Normaler Kernelthread mit niedrigster Priorität

Der Idle Thread wird als gewöhnlicher Kernelthread mit sehr niedriger Priorität implementiert.

#### Vorteile

- Wiederverwendung normaler Threadmechanismen,
- einfache Integration in den Scheduler.

#### Nachteile

- könnte blockiert, migriert oder beendet werden,
- könnte irrtümlich Ressourcen besitzen,
- könnte mit normalen Idle-Aufgaben verwechselt werden,
- kritische Invarianten wären nicht ausreichend geschützt.

Diese Alternative wird verworfen.

---

### 5.4 Spezieller CPU-lokaler Idle Thread

Jede aktive logische CPU besitzt einen eigenen, nicht migrierbaren Idle Thread.

Der Thread ist in das allgemeine Threadmodell integriert, besitzt jedoch zusätzliche unveränderliche Invarianten.

#### Vorteile

- gültiger Kontext auf jeder CPU,
- klare CPU-lokale Zuständigkeit,
- geringe Scheduler-Komplexität,
- gute Energieverwaltungsintegration,
- saubere Telemetrie,
- geeignet für CPU-Hotplug und Mehrkernsysteme.

#### Nachteile

- ein spezieller Thread pro CPU,
- zusätzliche Invarianten im Thread- und Schedulercode,
- eigene Lebenszyklusregeln erforderlich.

Diese Alternative wird gewählt.

---

## 6. Entscheidung

NovaOS erstellt für jede online geschaltete logische CPU genau einen CPU-lokalen Idle Thread.

Der Idle Thread:

- gehört zur Scheduler-Klasse `NOVA_SCHED_IDLE`,
- besitzt die interne Priorität `0`,
- ist fest an genau eine logische CPU gebunden,
- wird niemals auf eine andere CPU migriert,
- befindet sich nicht in einer normalen Run Queue,
- ist immer als Fallback für seine CPU verfügbar,
- darf nicht regulär blockiert werden,
- darf nicht durch Userspace beendet werden,
- besitzt keinen Userspace-Adressraum,
- führt keine normale Anwendungs- oder Hintergrundarbeit aus,
- koordiniert den sicheren Eintritt in CPU-Energiesparzustände.

Der Scheduler wählt den Idle Thread ausschließlich dann aus, wenn kein anderer zulässiger Thread auf der CPU ausgeführt werden kann.

---

## 7. Ein Idle Thread pro CPU

Jede aktive logische CPU besitzt ein eigenes Idle-Thread-Objekt.

```text
CPU 0 ── Idle Thread 0
CPU 1 ── Idle Thread 1
CPU 2 ── Idle Thread 2
CPU 3 ── Idle Thread 3
```

Die Zuordnung ist während der Online-Phase der CPU unveränderlich.

Der Idle Thread verwendet:

- CPU-lokalen Kernelstack,
- CPU-lokale Scheduler-Daten,
- CPU-lokale Energieverwaltungsinformationen,
- CPU-lokale Leerlaufstatistiken.

Ein Idle Thread darf niemals gleichzeitig auf mehreren CPUs ausgeführt werden.

---

## 8. Scheduler-Klasse und Priorität

Der Idle Thread gehört zur Klasse:

```c
NOVA_SCHED_IDLE
```

Er besitzt die Priorität:

```text
0
```

Diese Werte sind unveränderlich.

Der Idle Thread:

- erhält keine dynamischen Prioritäts-Boosts,
- nimmt nicht am Aging teil,
- erhält keine Prioritätsvererbung,
- besitzt keine normale Zeitscheibe,
- besitzt keinen Anspruch auf garantierten Fortschritt,
- wird von jedem anderen lauffähigen Thread verdrängt.

Ein normaler Thread der Klasse `NOVA_SCHED_IDLE` ist nicht mit dem CPU-lokalen Kernel-Idle-Thread gleichzusetzen.

---

## 9. Verhältnis zur Idle Scheduler Class

ADR-PROC-0003 definiert eine allgemeine Idle Scheduler Class.

Diese kann zwei unterschiedliche Arten von Arbeit enthalten:

1. den CPU-lokalen Kernel-Idle-Thread,
2. optionale Userspace- oder Kernelarbeit ohne Fortschrittsgarantie.

Der CPU-lokale Idle Thread besitzt innerhalb dieser Klasse eine Sonderrolle.

Optionale Idle-Arbeit darf ausgeführt werden, wenn sie explizit lauffähig ist. Sie wird wie ein normaler planbarer Thread behandelt und verdrängt den eigentlichen CPU-Idle-Thread.

Die Reihenfolge lautet:

```text
reguläre Scheduler-Klassen
        ↓
optionale Idle-Arbeit
        ↓
CPU-lokaler Kernel-Idle-Thread
```

Der Kernel-Idle-Thread bleibt somit der absolute letzte Fallback.

---

## 10. Scheduler-Auswahl

Die Auswahl des nächsten Threads folgt konzeptionell diesem Ablauf:

```c
nova_thread_t* nova_scheduler_select_next(nova_cpu_t* cpu)
{
    nova_thread_t* next = nova_runqueue_select(cpu->runqueue);

    if (next != NULL)
    {
        return next;
    }

    return cpu->idle_thread;
}
```

`nova_scheduler_select_next()` darf für eine online geschaltete CPU niemals `NULL` zurückgeben.

Ist kein regulärer Thread verfügbar, wird immer der CPU-lokale Idle Thread ausgewählt.

---

## 11. Run-Queue-Behandlung

Der CPU-lokale Idle Thread wird nicht wie ein normaler Thread in die Run Queue eingereiht.

Dadurch werden folgende Probleme vermieden:

- unnötiges Ein- und Ausreihen,
- falsche Lastberechnung,
- Migration durch den Load Balancer,
- Verwechslung mit regulärer Idle-Arbeit,
- künstliche Erhöhung der Run-Queue-Länge.

Die Run Queue kann leer sein. Der Scheduler interpretiert eine leere Run Queue als Auswahl des CPU-lokalen Idle Threads.

Der Idle Thread wird in Statistiken separat von der Run Queue behandelt.

---

## 12. Threadzustände

Ein normaler Thread kann mehrere Lebenszykluszustände besitzen.

Der Idle Thread verwendet nur eine eingeschränkte Teilmenge:

```text
CREATED
   ↓
READY
   ↓
RUNNING
   ↕
IDLE_WAIT
   ↓
OFFLINE
```

| Zustand | Bedeutung |
|---|---|
| `CREATED` | Idle-Thread-Objekt wurde angelegt |
| `READY` | Thread ist als CPU-Fallback vorbereitet |
| `RUNNING` | Idle Thread wird aktuell ausgeführt |
| `IDLE_WAIT` | CPU befindet sich in einem Warte- oder Energiesparzustand |
| `OFFLINE` | zugehörige CPU ist offline |

Der Idle Thread verwendet keine normalen Zustände wie:

```text
BLOCKED
SUSPENDED
TERMINATED
ZOMBIE
```

Ein Übergang in einen dieser Zustände stellt eine Verletzung einer Kernelinvariante dar.

---

## 13. Idle Loop

Die Idle Loop muss möglichst klein bleiben.

Ein konzeptioneller Ablauf lautet:

```c
void nova_idle_thread_main(void)
{
    nova_cpu_t* cpu = nova_cpu_current();

    for (;;)
    {
        nova_idle_prepare(cpu);

        if (nova_scheduler_has_runnable_work(cpu))
        {
            nova_idle_cancel(cpu);
            nova_scheduler_reschedule();
            continue;
        }

        nova_idle_enter(cpu);
        nova_idle_leave(cpu);

        nova_scheduler_reschedule();
    }
}
```

Die konkrete Implementierung ist architekturabhängig.

Die Idle Loop darf keine unbeschränkten oder blockierenden Operationen ausführen.

---

## 14. Vermeidung verlorener Wake-ups

Zwischen der Prüfung auf verfügbare Arbeit und dem Eintritt in einen Hardware-Idle-State besteht eine Race Condition.

Problematischer Ablauf:

```text
CPU prüft Run Queue: leer
        ↓
anderer Prozessor reiht einen Thread ein
        ↓
Wake-up wird zu früh verarbeitet
        ↓
CPU tritt danach in den Idle State ein
        ↓
lauffähiger Thread wartet unnötig
```

NovaOS muss dieses Rennen durch eine architekturspezifisch atomare oder äquivalente Sequenz verhindern.

Die Implementierung muss sicherstellen:

1. Der Idle-Zustand wird als vorbereitet markiert.
2. Neue Arbeit erkennt den Idle-Zustand.
3. Ein Wake-up oder Rescheduling-Interrupt wird nicht verloren.
4. Vor dem tatsächlichen Schlafen wird die Run Queue erneut geprüft.
5. Speicherbarrieren sichern die sichtbare Reihenfolge.
6. Die CPU schläft nur, wenn weiterhin keine Arbeit vorhanden ist.

---

## 15. Rescheduling

Wird ein regulärer Thread lauffähig, während der Idle Thread ausgeführt wird, muss die betroffene CPU zum Scheduler zurückkehren.

Dies kann ausgelöst werden durch:

- lokalen Timer,
- Geräteinterrupt,
- Inter-Processor Interrupt,
- IPC-Ereignis,
- Aufwecken eines Threads,
- Änderung einer CPU-Affinität,
- CPU-Hotplug-Ereignis.

Der Idle Thread darf einen lauffähigen regulären Thread nicht bis zum nächsten periodischen Scheduler-Tick warten lassen.

Falls erforderlich, sendet der Kernel einen Rescheduling-Interrupt an die betroffene CPU.

---

## 16. Energieverwaltung

Der Idle Thread bildet die kontrollierte Verbindung zwischen Scheduler und CPU-Energieverwaltung.

Der Scheduler liefert Informationen wie:

- erwartete Leerlaufdauer,
- Zeitpunkt des nächsten Timers,
- Anzahl lauffähiger Threads,
- CPU-Isolationsstatus,
- aktive Echtzeitreservierungen.

Die Energieverwaltung wählt daraus einen geeigneten Idle State.

Ein tieferer Idle State kann:

- weniger Energie verbrauchen,
- eine längere Eintrittszeit besitzen,
- eine längere Wake-up-Latenz besitzen,
- Teile des CPU-Kontexts verlieren,
- zusätzliche Koordination mit anderen CPUs erfordern.

Der Scheduler selbst bestimmt nicht die hardwareabhängige Idle-State-Sequenz. Diese Verantwortung liegt bei der Architektur- und Energieverwaltungsschicht.

---

## 17. Auswahl des Idle States

Die Auswahl eines Idle States berücksichtigt mindestens:

- erwartete Leerlaufdauer,
- nächsten Timertermin,
- Austrittslatenz,
- Energieeinsparung,
- Break-even-Zeit,
- aktuelle CPU-Frequenz,
- aktive Echtzeitgarantien,
- thermischen Zustand,
- gemeinsame Energiezustände eines CPU-Clusters.

Konzeptionell gilt:

```text
selected_state =
    deepest_state_with(
        exit_latency <= allowed_latency
        and break_even_time <= expected_idle_duration
    )
```

Ist kein tiefer Zustand geeignet, verwendet die CPU einen leichten Wartezustand.

---

## 18. Tickless Idle

NovaOS unterstützt Tickless Idle.

Wenn keine periodische Schedulerarbeit erforderlich ist, kann der reguläre Timer-Tick auf einer unbeschäftigten CPU ausgesetzt werden.

Vor dem Eintritt programmiert der Kernel den nächsten notwendigen Zeitpunkt.

Dieser kann bestimmt werden durch:

- Ablauf eines Thread-Timers,
- Auffüllung eines Echtzeitbudgets,
- Deadline-Überwachung,
- Scheduler-Wartung,
- System-Timeout,
- geplante CPU-Operation.

Nach dem Wake-up wird die vergangene Zeit anhand einer monotonen Zeitquelle berechnet.

Das System darf nicht davon ausgehen, dass während des Idle States periodische Ticks eingetroffen sind.

---

## 19. Polling und Warten

Der Idle Thread kann abhängig von Hardware und erwarteter Leerlaufdauer unterschiedliche Strategien verwenden:

- kurzes aktives Polling,
- leichtes CPU-Warten,
- tiefer Hardware-Idle-State.

Aktives Polling ist nur zulässig, wenn:

- die erwartete Wartezeit sehr kurz ist,
- der Energieverbrauch vertretbar ist,
- die Wake-up-Latenz dadurch wesentlich reduziert wird,
- eine Systemrichtlinie es erlaubt.

Unbegrenztes Busy Waiting im Idle Thread ist nicht zulässig.

---

## 20. Echtzeitinteraktion

Aktive Echtzeitreservierungen beeinflussen die Idle-State-Auswahl.

Der Idle Thread muss berücksichtigen:

- nächste Echtzeitaktivierung,
- zulässigen Wake-up-Jitter,
- maximale Austrittslatenz,
- CPU-Reservierung,
- Interrupt-Affinität.

Ein tiefer Idle State darf nicht gewählt werden, wenn seine maximale Austrittslatenz eine genehmigte Echtzeitgarantie gefährdet.

Die Echtzeit-Admission-Control muss bekannte Idle-State-Latenzen in ihre Bewertung einbeziehen.

---

## 21. Mehrkernsysteme

Jede CPU entscheidet grundsätzlich lokal über ihren Idle-Zustand.

Globale Koordination ist erforderlich, wenn:

- mehrere CPUs einen gemeinsamen Cache teilen,
- ein CPU-Cluster nur gemeinsam abgeschaltet werden kann,
- ein gemeinsamer Takt- oder Spannungsbereich betroffen ist,
- eine CPU für Interrupts oder Timer verfügbar bleiben muss,
- globale Schedulerarbeit ansteht.

Eine CPU darf nicht in einen tiefen Zustand wechseln, wenn dadurch die korrekte Funktion anderer CPUs gefährdet wird.

---

## 22. Lastverteilung

Eine CPU, die ihren Idle Thread ausführt, gilt als grundsätzlich verfügbar für neue Arbeit.

Der Load Balancer kann versuchen:

- einen zulässigen Thread von einer überlasteten CPU zu übernehmen,
- einen neu erzeugten Thread auf der Idle-CPU zu platzieren,
- Cache- und NUMA-Lokalität zu berücksichtigen,
- Echtzeit- und Affinitätsregeln einzuhalten.

Der Idle Thread selbst:

- nimmt nicht an der Lastverteilung teil,
- wird nicht migriert,
- wird nicht als reguläre Schedulerlast gezählt.

---

## 23. Idle Pull

Bevor eine CPU in einen tieferen Idle State wechselt, darf sie optional prüfen, ob von einer anderen CPU geeignete Arbeit übernommen werden kann.

Diese Idle-Pull-Operation muss begrenzt sein.

Sie darf:

- keine langen globalen Sperren halten,
- keine Echtzeitgarantien verletzen,
- keine unzulässigen CPU-Affinitäten ignorieren,
- nicht zu ständigem Thread-Pingpong führen,
- den Eintritt in den Idle State nicht unverhältnismäßig verzögern.

Ist keine geeignete Arbeit verfügbar, setzt die CPU den Idle-Eintritt fort.

---

## 24. Heterogene Prozessoren

Auf Prozessoren mit unterschiedlichen Kerntypen können Idle Threads verschiedene hardwareabhängige Routinen verwenden.

Beispiele:

- Performance-Kerne besitzen andere Idle States als Effizienzkerne.
- CPU-Cluster besitzen gemeinsame Abschaltzustände.
- einzelne Kerne unterstützen unterschiedliche Wake-up-Quellen.

Der generische Idle-Thread-Lebenszyklus bleibt identisch.

Architektur- und plattformspezifische Backends liefern:

- verfügbare Idle States,
- Eintrittsfunktionen,
- Austrittslatenzen,
- Energieeigenschaften,
- unterstützte Wake-up-Quellen.

---

## 25. NUMA-Systeme

Auf NUMA-Systemen bleibt der Idle Thread fest an seine CPU und damit an deren NUMA-Knoten gebunden.

Der Idle Thread führt keine normale Speicherallokation aus.

CPU-lokale Idle-Daten sollen bevorzugt auf dem zugehörigen NUMA-Knoten liegen.

Der Scheduler darf eine Idle-CPU für Arbeit eines anderen NUMA-Knotens verwenden, muss dabei jedoch Speicher- und Migrationskosten berücksichtigen.

---

## 26. CPU-Start

Beim Start einer zusätzlichen CPU wird der Idle Thread vorbereitet, bevor die CPU für normale Schedulerarbeit freigegeben wird.

Die Reihenfolge lautet:

```text
CPU erkennen
    ↓
CPU-lokale Daten anlegen
    ↓
Idle-Thread-Objekt erzeugen
    ↓
Kernelstack vorbereiten
    ↓
Architekturkontext initialisieren
    ↓
Idle Thread starten
    ↓
CPU in Scheduler aufnehmen
    ↓
CPU als online markieren
```

Eine CPU darf nicht als online gelten, solange kein funktionsfähiger Idle Thread vorhanden ist.

---

## 27. CPU-Offlining

Vor dem Offlining einer CPU muss der Kernel:

1. neue Threadplatzierungen auf dieser CPU verhindern,
2. migrierbare Threads auf andere CPUs verschieben,
3. Echtzeitreservierungen erneut prüfen,
4. lokale Timer übertragen oder abbrechen,
5. Interrupt-Affinitäten anpassen,
6. CPU-lokale Hintergrundarbeit abschließen,
7. zum Idle Thread wechseln,
8. die CPU über eine architekturspezifische Routine stoppen.

Der Idle Thread ist der letzte reguläre Ausführungskontext vor dem Abschalten der CPU.

Während des Offlining darf er in den Zustand `OFFLINE` übergehen.

---

## 28. CPU-Ausfall

Bei einem unerwarteten CPU-Ausfall kann der zugehörige Idle Thread nicht regulär beendet werden.

Der Kernel muss:

- die CPU als ausgefallen markieren,
- den Idle Thread als nicht mehr ausführbar kennzeichnen,
- CPU-lokale Ressourcen sichern oder isolieren,
- betroffene Echtzeitgarantien neu bewerten,
- Diagnoseinformationen erzeugen,
- gegebenenfalls Recovery-Maßnahmen einleiten.

Das Idle-Thread-Objekt darf erst freigegeben werden, wenn keine andere Kernelkomponente mehr darauf zugreifen kann.

---

## 29. Kernelstack

Jeder Idle Thread besitzt einen eigenen Kernelstack.

Der Stack:

- wird vor dem Online-Schalten der CPU reserviert,
- bleibt während der gesamten Online-Phase verfügbar,
- darf nicht ausgelagert werden,
- besitzt Guard Pages gemäß ADR-MM-0013,
- ist fest der zugehörigen CPU zugeordnet,
- darf nicht für andere Threads wiederverwendet werden, solange die CPU online ist.

Der Idle-Pfad soll nur wenig Stack verwenden.

Große lokale Variablen und tiefe Aufrufketten sind zu vermeiden.

---

## 30. Adressraum

Der Idle Thread besitzt keinen eigenen Userspace-Adressraum.

Er verwendet den definierten Kerneladressraum beziehungsweise einen sicheren CPU-lokalen Kernelkontext.

Beim Wechsel vom letzten Userspace-Thread zum Idle Thread muss der Kernel:

- den notwendigen Adressraumzustand sichern,
- Sicherheitsgrenzen beibehalten,
- gegebenenfalls einen Kerneladressraum aktivieren,
- architekturspezifische Adressraumoptimierungen berücksichtigen.

Der Idle Thread darf keine Userspace-Adressen dereferenzieren.

---

## 31. Ressourcenbesitz

Der Idle Thread darf grundsätzlich keine normalen Ressourcen besitzen.

Insbesondere darf er nicht dauerhaft besitzen:

- Userspace-Handles,
- Dateideskriptoren,
- normale Mutexe,
- IPC-Kanäle,
- Gerätehandles,
- dynamische Speicherbereiche mit unklarer Lebensdauer,
- Prozessressourcen.

Kurzlebige interne CPU-lokale Sperren sind nur zulässig, wenn:

- sie nicht blockieren,
- ihre maximale Haltedauer streng begrenzt ist,
- sie vor Eintritt in einen Hardware-Idle-State freigegeben werden.

Der Idle Thread darf niemals schlafen, während er eine Sperre hält, die von regulären Threads benötigt wird.

---

## 32. Keine normale Hintergrundarbeit

Der CPU-lokale Idle Thread führt keine normale Hintergrundarbeit aus.

Nicht im Idle Thread auszuführen sind beispielsweise:

- Dateisystem-Scrubbing,
- Speicherkompression,
- Telemetrieaufbereitung,
- Protokollierung auf Datenträger,
- Garbage Collection,
- Indexierung,
- Software-Updates,
- KI-Berechnungen,
- Cache-Bereinigung mit unbeschränkter Laufzeit.

Solche Aufgaben müssen als normale Kernel- oder Userspace-Threads modelliert werden.

Sie können der Scheduler-Klasse `BACKGROUND` oder der allgemeinen `IDLE`-Klasse zugeordnet werden.

Dadurch bleiben sie:

- präemptierbar,
- abrechenbar,
- beendbar,
- diagnostizierbar,
- vom eigentlichen CPU-Idle-Pfad getrennt.

---

## 33. Begrenzte Idle-Housekeeping-Arbeit

Sehr kurze CPU-lokale Wartungsoperationen dürfen im Idle-Pfad ausgeführt werden, wenn sie für die korrekte Funktion des Kernels erforderlich sind.

Beispiele:

- Aktualisierung CPU-lokaler Idle-Zähler,
- Abschluss unmittelbar fälliger CPU-lokaler Zustandsübergänge,
- Prüfung auf ausstehende Rescheduling-Anforderungen,
- definierte quiescent-state-Markierung,
- Vorbereitung des Hardware-Idle-Zustands.

Diese Arbeit muss:

- streng begrenzt,
- nicht blockierend,
- ohne unbeschränkte Allokation,
- ohne normale I/O,
- vollständig präemptierbar oder architekturseitig kurz sein.

---

## 34. Telemetrie und Diagnose

Für jeden CPU-lokalen Idle Thread werden mindestens folgende Daten erfasst:

- zugehörige CPU-ID,
- Idle-Thread-ID,
- Anzahl der Aktivierungen,
- gesamte Idle-Zeit,
- Zeit pro Idle State,
- Anzahl der Idle-State-Eintritte,
- Anzahl abgebrochener Idle-Eintritte,
- Anzahl sofortiger Wake-ups,
- durchschnittliche Idle-Dauer,
- längste Idle-Dauer,
- Wake-up-Latenz,
- Wake-up-Ursache,
- Anzahl empfangener Rescheduling-Interrupts,
- Zeit im aktiven Polling,
- Zeit in Hardware-Wartezuständen.

Die Telemetrie wird CPU-lokal gesammelt.

Eine aufwendige Aufbereitung erfolgt außerhalb des Idle Threads.

---

## 35. Leerlaufabrechnung

Die Zeit des CPU-lokalen Idle Threads wird nicht als normale Prozess- oder Thread-CPU-Zeit abgerechnet.

NovaOS unterscheidet mindestens:

```text
user_time
kernel_time
interrupt_time
realtime_time
idle_time
offline_time
```

Optionale Arbeit der allgemeinen Idle Scheduler Class zählt nicht als Hardware-Idle-Zeit.

Eine CPU gilt nur dann als tatsächlich idle, wenn der CPU-lokale Kernel-Idle-Thread ausgeführt wird oder die CPU sich in einem durch ihn eingeleiteten Hardware-Idle-State befindet.

---

## 36. Debugging

Debugger und Diagnosewerkzeuge müssen Idle Threads eindeutig kennzeichnen.

Eine mögliche Namenskonvention lautet:

```text
idle/0
idle/1
idle/2
idle/3
```

Idle Threads müssen als spezielle Kernelthreads dargestellt werden.

Werkzeuge dürfen sie nicht:

- als hängende Anwendung melden,
- als normale CPU-intensive Prozesse darstellen,
- zur Benutzerbeendigung anbieten,
- in normale Prozessgruppen einordnen.

Eine CPU, die fast vollständig im Idle Thread verbringt, ist normalerweise nicht blockiert, sondern unbeschäftigt.

---

## 37. Watchdog-Interaktion

Der Idle Thread darf nicht fälschlicherweise einen Lockup-Watchdog auslösen.

Der Watchdog muss unterscheiden zwischen:

- regulärem Hardware-Idle,
- deaktivierten Interrupts,
- festhängender Idle Loop,
- verlorenem Wake-up,
- tatsächlichem CPU-Lockup.

Der Idle Thread oder die Architekturbackend-Schicht kann dem Watchdog erwartete tiefe Schlafzustände mitteilen.

Bleibt eine CPU außerhalb eines gültigen Idle States ohne Fortschritt, darf der Watchdog eine Diagnose oder Recovery auslösen.

---

## 38. Sicherheitsmodell

Idle Threads sind interne Kernelobjekte.

Userspace darf:

- keinen Idle Thread erzeugen,
- keinen Idle Thread beenden,
- seine Priorität nicht verändern,
- seine CPU-Affinität nicht verändern,
- seinen Kontext nicht manipulieren,
- ihn nicht blockieren,
- keine Signale an ihn senden.

Diagnosezugriff darf über kontrollierte, schreibgeschützte Schnittstellen erfolgen.

Selbst administrative Prozesse erhalten keinen direkten Schreibzugriff auf Idle-Thread-Objekte.

---

## 39. API-Grundmodell

Idle Threads besitzen keine allgemeine öffentliche Steuerungs-API.

Kernelinterne Schnittstellen können beispielsweise lauten:

```c
nova_status_t nova_idle_thread_create(
    nova_cpu_t* cpu,
    nova_thread_t** idle_thread
);

nova_thread_t* nova_idle_thread_get(
    nova_cpu_t* cpu
);

void nova_idle_prepare(
    nova_cpu_t* cpu
);

void nova_idle_cancel(
    nova_cpu_t* cpu
);

void nova_idle_enter(
    nova_cpu_t* cpu
);

void nova_idle_leave(
    nova_cpu_t* cpu
);
```

Für Diagnosezwecke kann eine schreibgeschützte Abfrage bereitgestellt werden:

```c
nova_status_t nova_cpu_get_idle_info(
    nova_cpu_id_t cpu_id,
    nova_idle_info_t* info
);
```

Ein mögliches Informationsmodell lautet:

```c
typedef struct
{
    nova_cpu_id_t cpu_id;
    nova_thread_id_t idle_thread_id;

    uint64_t total_idle_time_ns;
    uint64_t idle_entry_count;
    uint64_t aborted_entry_count;
    uint64_t wakeup_count;
    uint64_t last_idle_duration_ns;
    uint64_t longest_idle_duration_ns;

    uint32_t current_idle_state;
    uint32_t last_wakeup_reason;
} nova_idle_info_t;
```

Die endgültige ABI wird in einer separaten Spezifikation festgelegt.

---

## 40. Architekturabstraktion

Der generische Idle Thread verwendet ein architekturspezifisches Backend.

Eine mögliche Schnittstelle lautet:

```c
typedef struct nova_arch_idle_ops
{
    nova_status_t (*initialize_cpu)(
        nova_cpu_t* cpu
    );

    uint32_t (*select_state)(
        nova_cpu_t* cpu,
        uint64_t expected_idle_ns,
        uint64_t max_exit_latency_ns
    );

    void (*enter_state)(
        nova_cpu_t* cpu,
        uint32_t state
    );

    void (*wake_cpu)(
        nova_cpu_t* cpu
    );

    void (*shutdown_cpu)(
        nova_cpu_t* cpu
    );
} nova_arch_idle_ops_t;
```

Mögliche architekturspezifische Mechanismen sind:

- `HLT` oder `MWAIT` auf x86,
- `WFI` oder `WFE` auf ARM,
- plattformspezifische Firmware-Aufrufe,
- virtuelle Idle-Hinweise unter einem Hypervisor.

Die generische Schedulerlogik darf nicht direkt von einem bestimmten CPU-Befehl abhängen.

---

## 41. Virtualisierung

Unter einem Hypervisor signalisiert der Idle Thread, dass die virtuelle CPU momentan keine Arbeit besitzt.

Der Hypervisor kann daraufhin:

- die virtuelle CPU pausieren,
- physische CPU-Zeit anderen Gästen zuweisen,
- den Gast bei einem virtuellen Interrupt wieder aktivieren.

Aktives Busy Waiting ist in virtuellen Maschinen besonders zu begrenzen.

Die Idle-Implementierung muss Hypervisor-Erkennung und paravirtualisierte Idle-Mechanismen unterstützen können.

---

## 42. Fehlerbehandlung

Folgende Zustände gelten als Fehler:

- eine online geschaltete CPU besitzt keinen Idle Thread,
- der Scheduler liefert für eine aktive CPU `NULL`,
- ein Idle Thread wird auf der falschen CPU ausgeführt,
- ein Idle Thread wird in eine normale Run Queue eingereiht,
- ein Idle Thread geht in den Zustand `BLOCKED`,
- ein Idle Thread wird beendet,
- ein Idle Thread besitzt eine andere Priorität als `0`,
- ein Idle Thread erhält eine andere Scheduler-Klasse,
- ein Idle Thread hält beim Schlafen eine unzulässige Sperre,
- ein ungültiger Idle State wird ausgewählt.

Wiederherstellbare Fehler sollen:

- den tiefen Idle State abbrechen,
- einen sicheren leichten Wartezustand verwenden,
- ein Diagnoseereignis erzeugen,
- die betroffene Energieoptimierung deaktivieren.

Ein Kernel-Panic ist zulässig, wenn eine zentrale Idle- oder Scheduler-Invariante verletzt ist und keine sichere Weiterführung möglich ist.

---

## 43. Konsequenzen

### 43.1 Positive Konsequenzen

- jede aktive CPU besitzt immer einen gültigen Threadkontext,
- der Scheduler benötigt keinen threadlosen Sonderzustand,
- klare Integration der Energieverwaltung,
- gute Unterstützung für Tickless Idle,
- sichere CPU-Hotplug-Abläufe,
- präzise Leerlaufstatistiken,
- eindeutige Trennung zwischen CPU-Leerlauf und Hintergrundarbeit,
- bessere Diagnose verlorener Wake-ups,
- gute Mehrkern- und Virtualisierungsunterstützung.

### 43.2 Negative Konsequenzen

- jede CPU benötigt einen zusätzlichen Kernelthread und Stack,
- spezielle Lebenszyklusregeln sind erforderlich,
- der Idle-Pfad ist stark architekturabhängig,
- tiefe Energiesparzustände erhöhen die Wake-up-Komplexität,
- verlorene Wake-ups müssen mit besonderer Sorgfalt verhindert werden.

### 43.3 Risiken

- Race Condition beim Eintritt in den Idle State,
- verloren gegangene Rescheduling-Interrupts,
- zu tiefe Idle States bei Echtzeitlast,
- fehlerhafte Timerprogrammierung im Tickless-Modus,
- unnötiges Busy Waiting,
- falsche Idle-Zeitabrechnung,
- CPU-Hotplug mit noch aktiven Ressourcen.

Diese Risiken werden begrenzt durch:

- erneute Run-Queue-Prüfung,
- Speicherbarrieren,
- architekturspezifisch sichere Wait-Sequenzen,
- Wake-up-Interrupts,
- Idle-State-Latenzgrenzen,
- CPU-lokale Telemetrie,
- definierte Hotplug-Zustandsübergänge.

---

## 44. Auswirkungen auf andere Subsysteme

### 44.1 Scheduler

Der Scheduler muss bei einer leeren Run Queue immer den CPU-lokalen Idle Thread auswählen.

### 44.2 Threadverwaltung

Die Threadverwaltung muss spezielle unveränderliche Idle-Thread-Invarianten unterstützen.

### 44.3 Energieverwaltung

Die Energieverwaltung wählt auf Basis der erwarteten Leerlaufzeit einen geeigneten Hardwarezustand.

### 44.4 Timer

Das Timersystem muss Tickless Idle und die Programmierung des nächsten erforderlichen Ereignisses unterstützen.

### 44.5 CPU-Hotplug

CPU-Start und CPU-Offlining müssen mit dem Lebenszyklus des Idle Threads verbunden sein.

### 44.6 Echtzeitscheduler

Echtzeit-Deadlines und Wake-up-Latenzen begrenzen die zulässigen Idle States.

### 44.7 Telemetrie

Leerlaufzeiten und Wake-up-Ursachen müssen CPU-lokal erfasst werden.

### 44.8 Watchdog

Der Watchdog muss gültige Idle-Zustände von CPU-Lockups unterscheiden.

### 44.9 Virtualisierung

Die Virtualisierungsschicht kann paravirtualisierte Warte- und Wake-up-Mechanismen bereitstellen.

---

## 45. Implementierungsrichtlinien

1. Jede online geschaltete CPU erhält genau einen Idle Thread.
2. Der Idle Thread wird vor Freigabe der CPU für den Scheduler erstellt.
3. Der Idle Thread wird nicht in die normale Run Queue eingereiht.
4. Der Scheduler verwendet ihn ausschließlich als letzten Fallback.
5. Die Idle Loop bleibt klein und führt keine normale Hintergrundarbeit aus.
6. Der Idle Thread darf keine blockierenden Operationen ausführen.
7. Vor dem Eintritt in einen Idle State wird die Run Queue erneut geprüft.
8. Speicherbarrieren sichern die Koordination mit anderen CPUs.
9. Die erste Implementierung darf zunächst nur einen leichten Hardware-Wartezustand verwenden.
10. Tiefe Idle States werden erst nach funktionierender Timer-, Wake-up- und Telemetrieunterstützung ergänzt.
11. Tickless Idle wird als getrennte Ausbaustufe implementiert.
12. Architekturspezifische Befehle werden hinter einer Idle-Backend-Schnittstelle gekapselt.
13. Idle-Zeit wird getrennt von regulärer Kernelzeit abgerechnet.
14. CPU-Offlining endet immer im CPU-lokalen Idle Thread.

---

## 46. Testanforderungen

Mindestens folgende Szenarien müssen getestet werden:

- Erzeugung des Boot-CPU-Idle-Threads,
- Erzeugung eines Idle Threads für jede zusätzliche CPU,
- Schedulerauswahl bei leerer Run Queue,
- Präemption des Idle Threads durch einen normalen Thread,
- Präemption durch einen Echtzeitthread,
- sofortiger Wake-up nach Einreihung neuer Arbeit,
- Race Condition zwischen Leerprüfung und Idle-Eintritt,
- wiederholter schneller Idle-Eintritt und Austritt,
- Tickless Idle,
- Timer-Wake-up,
- Geräteinterrupt-Wake-up,
- Inter-Processor-Interrupt-Wake-up,
- Load Balancing auf eine Idle-CPU,
- Idle Pull,
- CPU-Affinitätsbeschränkungen,
- heterogene Prozessorkerne,
- NUMA-Systeme,
- tiefe Idle States,
- thermische Einschränkungen,
- aktive Echtzeitreservierungen,
- CPU-Hotplug,
- CPU-Offlining,
- unerwarteter CPU-Ausfall,
- Virtualisierung,
- Watchdog während eines langen Idle States,
- ungültiger Versuch zur Prioritätsänderung,
- ungültiger Versuch zur Migration,
- ungültiger Versuch zur Beendigung,
- Stack-Guard-Page-Auslösung,
- korrekte Idle-Zeitabrechnung,
- Telemetrie unter häufigen Wake-ups.

---

## 47. Nicht Bestandteil dieser Entscheidung

Diese ADR legt nicht abschließend fest:

- konkrete Hardware-Idle-States,
- genaue Eintritts- und Austrittslatenzen,
- den vollständigen Tickless-Timer-Algorithmus,
- CPU-spezifische Energieverbrauchsmodelle,
- genaue Idle-Polling-Grenzen,
- konkrete Load-Balancing-Intervalle,
- plattformspezifische Firmware-Aufrufe,
- die endgültige öffentliche Diagnose-ABI.

Diese Details werden in nachfolgenden ADRs und NPSPEC-Dokumenten definiert.

---

## 48. Verbindliche Invarianten

1. Jede online geschaltete logische CPU besitzt genau einen Idle Thread.
2. Der Scheduler liefert für eine online geschaltete CPU immer einen gültigen Thread.
3. Der CPU-lokale Idle Thread wird nur ausgeführt, wenn kein anderer zulässiger Thread lauffähig ist.
4. Der Idle Thread gehört ausschließlich zur Klasse `NOVA_SCHED_IDLE`.
5. Der Idle Thread besitzt ausschließlich die Priorität `0`.
6. Der Idle Thread ist fest an genau eine CPU gebunden.
7. Der Idle Thread darf niemals migriert werden.
8. Der Idle Thread befindet sich nicht in einer normalen Run Queue.
9. Der Idle Thread darf nicht regulär blockiert, suspendiert oder beendet werden.
10. Der Idle Thread besitzt keinen Userspace-Adressraum.
11. Der Idle Thread darf keine normale Hintergrundarbeit ausführen.
12. Der Idle Thread darf nicht mit einer unzulässigen gehaltenen Sperre schlafen.
13. Neue lauffähige Arbeit muss einen Idle Thread rechtzeitig aufwecken.
14. Die Prüfung auf Arbeit und der Idle-Eintritt dürfen keinen Wake-up verlieren.
15. Tiefe Idle States dürfen genehmigte Echtzeitgarantien nicht verletzen.
16. Eine CPU darf erst als online gelten, wenn ihr Idle Thread vollständig vorbereitet ist.
17. CPU-Offlining wird im CPU-lokalen Idle Thread abgeschlossen.
18. Idle-Zeit wird getrennt von regulärer Thread- und Prozesszeit abgerechnet.
19. Userspace erhält keinen direkten Schreibzugriff auf Idle-Thread-Objekte.
20. KI- oder lernbasierte Komponenten dürfen Wake-up-, Sicherheits- oder Echtzeitgrenzen nicht umgehen.

---

## 49. Referenzen

- ADR-PROC-0001 – Process Lifecycle
- ADR-PROC-0002 – Thread Lifecycle
- ADR-PROC-0003 – Scheduler Classes
- ADR-PROC-0004 – Prioritäten
- ADR-PROC-0005 – Echtzeitprozesse
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
- NPSPEC-SCHEDULER-IDLE-0001 – Idle Thread Specification
- NPSPEC-REALTIME-0001 – Realtime Process and Thread Specification
- NPSPEC-THREAD-0001 – Thread Specification

---

## 50. Zusammenfassung der Entscheidung

NovaOS erstellt für jede online geschaltete logische CPU genau einen eigenen Idle Thread.

Der Idle Thread ist ein spezieller, nicht migrierbarer Kernelthread der Scheduler-Klasse `NOVA_SCHED_IDLE` mit der unveränderlichen Priorität `0`. Er wird ausschließlich dann ausgeführt, wenn auf seiner CPU keine andere zulässige Arbeit vorhanden ist.

Der Idle Thread befindet sich nicht in einer normalen Run Queue und kann nicht regulär blockiert, suspendiert oder beendet werden. Er stellt den letzten gültigen Ausführungskontext einer CPU bereit und koordiniert den sicheren Übergang in Hardware-Energiesparzustände.

Normale Hintergrund- oder Wartungsarbeiten werden nicht im CPU-lokalen Idle Thread ausgeführt. Sie müssen als eigene planbare Threads modelliert werden.

Die Idle-Implementierung verhindert verlorene Wake-ups, unterstützt Tickless Idle, berücksichtigt Echtzeitgrenzen und bildet die Grundlage für Energieverwaltung, CPU-Hotplug, Mehrkernbetrieb und präzise Leerlaufdiagnose.