# ADR-PROC-0005 – Echtzeitprozesse

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-PROC-0005 |
| Titel | Echtzeitprozesse |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-30 |
| Bereich | Kernel / Prozesse / Threads / Echtzeit / Scheduling |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-PROC-0001, ADR-PROC-0002, ADR-PROC-0003, ADR-PROC-0004 |
| Nachfolgende Spezifikation | NPSPEC-REALTIME-0001 |

---

## 1. Kontext

NovaOS soll neben klassischen Desktop-, Server- und Hintergrundanwendungen auch zeitkritische Aufgaben unterstützen.

Dazu gehören beispielsweise:

- professionelle Audiowiedergabe und Audioaufnahme,
- Videoverarbeitung,
- Robotik und Maschinensteuerung,
- Sensorverarbeitung,
- industrielle Kommunikation,
- Regelungs- und Steuerungsalgorithmen,
- zeitkritische Eingabeverarbeitung,
- wissenschaftliche Messsysteme,
- Soft-PLC-Anwendungen,
- sicherheitsüberwachte Steuerungsdienste.

Diese Arbeitslasten benötigen vorhersehbare Reaktions- und Ausführungszeiten.

Ein normaler Prozess erhält CPU-Zeit nach Fairness-, Interaktions- und Lastverteilungsregeln. Für Echtzeitaufgaben reicht dies nicht aus. Ein Echtzeitprozess benötigt reservierbare Ressourcen, kontrollierte Prioritäten und begrenzte Ausführungslatenzen.

Nach ADR-PROC-0002 sind ausschließlich Threads direkt planbare Einheiten. Der Begriff „Echtzeitprozess“ bezeichnet daher keinen Prozess, der als Ganzes vom Scheduler ausgeführt wird.

Ein Echtzeitprozess ist ein Prozess mit:

- mindestens einem Echtzeitthread,
- einer genehmigten Echtzeit-Ressourcenreservierung,
- definierten zeitlichen Anforderungen,
- kontrollierten Speicher- und I/O-Eigenschaften,
- passenden Capabilities,
- einer überwachten Fehler- und Überlastungsstrategie.

Nicht jeder Thread eines Echtzeitprozesses muss selbst ein Echtzeitthread sein.

---

## 2. Problemstellung

NovaOS benötigt ein Echtzeitmodell, das zeitliche Garantien ermöglicht, ohne die Stabilität des Gesamtsystems zu gefährden.

Dabei müssen folgende Probleme gelöst werden:

- Echtzeitthreads können normale Systemarbeit verdrängen.
- Ein fehlerhafter Echtzeitthread könnte eine CPU dauerhaft blockieren.
- Seitenfehler und Speicherallokationen können unvorhersehbare Latenzen erzeugen.
- Treiber, Interrupts und Firmware können Echtzeitgarantien verletzen.
- Synchrone IPC kann Prioritätsinversion verursachen.
- Unbegrenzte Echtzeitprioritäten stellen ein Sicherheitsrisiko dar.
- Überbuchte CPU-Zeit kann zugesagte Deadlines unmöglich machen.
- Mehrkern- und NUMA-Migrationen können die Laufzeit beeinflussen.
- Energie- und Temperaturmanagement können CPU-Leistung verändern.
- Hardwarefehler und externe Ereignisse bleiben teilweise unvorhersehbar.
- Benutzeranwendungen dürfen keine falschen Garantien erhalten.

Das Echtzeitmodell muss deshalb zeitliche Garantien immer an explizite Reservierungen, Systemgrenzen und überprüfbare Bedingungen binden.

---

## 3. Entscheidungsziele

Die Echtzeitarchitektur soll:

1. deterministisches Scheduling ermöglichen,
2. harte und weiche Echtzeitanforderungen unterscheiden,
3. die Auslastung vor der Zulassung prüfen,
4. CPU-Zeit durch Budgets und Perioden reservieren,
5. das Gesamtsystem vor fehlerhaften Echtzeitprozessen schützen,
6. Speicher-, IPC- und I/O-Latenzen berücksichtigen,
7. Prioritätsinversion verhindern oder begrenzen,
8. Mehrkern- und NUMA-Systeme unterstützen,
9. Deadline-Verletzungen zuverlässig erkennen,
10. definierte Reaktionen auf Überlastung bereitstellen,
11. Echtzeitentscheidungen lokal diagnostizierbar machen,
12. keine Garantien versprechen, die die Hardware nicht erfüllen kann.

---

## 4. Begriffsdefinitionen

### 4.1 Echtzeit

Echtzeit bedeutet, dass ein Ergebnis nicht nur logisch korrekt, sondern innerhalb einer festgelegten Zeitgrenze verfügbar sein muss.

Echtzeit bedeutet nicht automatisch „besonders schnell“.

Eine Berechnung mit einer Deadline von einer Sekunde kann eine Echtzeitaufgabe sein, während eine Berechnung von einer Millisekunde ohne zugesicherte Zeitgrenze keine Echtzeitaufgabe sein muss.

---

### 4.2 Deadline

Die Deadline ist der späteste Zeitpunkt, zu dem ein Auftrag oder eine Aktivierung abgeschlossen sein muss.

NovaOS unterscheidet:

- relative Deadline,
- absolute Deadline.

```text
absolute_deadline =
    activation_time + relative_deadline
```

---

### 4.3 Laufzeitbudget

Das Laufzeitbudget beschreibt die maximale CPU-Zeit, die ein Echtzeitthread innerhalb einer Periode beanspruchen darf.

---

### 4.4 Periode

Die Periode beschreibt den zeitlichen Abstand wiederkehrender Aktivierungen oder die Dauer eines Reservierungsfensters.

---

### 4.5 Worst-Case Execution Time

Die Worst-Case Execution Time, kurz WCET, ist die angenommene maximale CPU-Ausführungszeit eines Auftrags unter den definierten Systembedingungen.

NovaOS kann eine angegebene WCET nicht automatisch als mathematisch bewiesen betrachten. Sie bleibt eine Eigenschaft der Anwendung, ihrer Analyse und der eingesetzten Plattform.

---

### 4.6 Jitter

Jitter beschreibt die Abweichung eines tatsächlichen Ereigniszeitpunkts vom erwarteten Zeitpunkt.

Beispiele:

- verspätetes Aufwecken,
- schwankende Startzeit,
- variierende Interruptlatenz,
- wechselnde Ausführungsdauer.

---

### 4.7 Deadline Miss

Ein Deadline Miss liegt vor, wenn ein Echtzeitauftrag seine definierte Deadline nicht einhält.

---

## 5. Echtzeitkategorien

NovaOS unterscheidet drei Echtzeitkategorien.

### 5.1 Harte Echtzeit

Bei harter Echtzeit gilt eine Deadline-Verletzung als System- oder Funktionsfehler.

Beispiele:

- bestimmte Maschinensteuerungen,
- zeitkritische Schutzabschaltungen,
- streng deterministische Regelkreise.

Harte Echtzeit darf nur zugesagt werden, wenn die gesamte relevante Ausführungskette dafür qualifiziert ist.

Dazu gehören:

- CPU,
- Timer,
- Interruptsystem,
- Kernel,
- Speicher,
- Treiber,
- Busse,
- Geräte,
- Firmware,
- Anwendung.

---

### 5.2 Feste Echtzeit

Bei fester Echtzeit ist ein verspätetes Ergebnis wertlos, eine einzelne Verletzung führt jedoch nicht zwingend zu einem gefährlichen Systemzustand.

Beispiele:

- Verarbeitung eines bereits abgelaufenen Sensorfensters,
- zeitgebundene Medienpakete,
- bestimmte Kommunikationszyklen.

Verspätete Ergebnisse werden verworfen oder als ungültig markiert.

---

### 5.3 Weiche Echtzeit

Bei weicher Echtzeit reduziert eine Deadline-Verletzung die Qualität, führt aber nicht unmittelbar zu einem Funktionsausfall.

Beispiele:

- Audio- und Videowiedergabe,
- Animationen,
- interaktive Medienverarbeitung,
- nicht sicherheitskritische Robotik.

Gelegentliche Deadline-Verletzungen können toleriert werden, müssen aber messbar bleiben.

---

## 6. Betrachtete Alternativen

### 6.1 Keine native Echtzeitunterstützung

Zeitkritische Anwendungen werden wie normale Anwendungen behandelt.

#### Vorteile

- geringer Implementierungsaufwand,
- einfacher Scheduler,
- keine Echtzeitreservierungen erforderlich.

#### Nachteile

- keine zeitlichen Garantien,
- unvorhersehbare Latenzen,
- ungeeignet für Robotik und Steuerung,
- keine verlässliche Audio- und Medienverarbeitung.

Diese Alternative wird verworfen.

---

### 6.2 Unbegrenzte feste Echtzeitprioritäten

Privilegierte Prozesse erhalten hohe feste Prioritäten und dürfen unbegrenzt ausgeführt werden.

#### Vorteile

- einfache Umsetzung,
- geringe Scheduler-Komplexität,
- niedrige Latenz bei geringer Last.

#### Nachteile

- ein fehlerhafter Thread kann das System blockieren,
- keine Überlastungskontrolle,
- keine zeitliche Isolation,
- hohes Sicherheitsrisiko,
- keine verlässliche Mehrmandantenfähigkeit.

Diese Alternative wird verworfen.

---

### 6.3 Separater Echtzeit-Kernel

Echtzeitaufgaben laufen in einem separaten Kernel oder Hypervisor unterhalb des normalen NovaOS-Kernels.

#### Vorteile

- starke Isolation,
- sehr gute Determinierbarkeit möglich,
- normaler Kernel kann nachrangig betrieben werden.

#### Nachteile

- hohe Architekturkomplexität,
- doppelte Treiber- und Ressourcenverwaltung,
- schwierige Integration,
- hoher Entwicklungsaufwand,
- widerspricht zunächst dem einheitlichen Kernelmodell.

Diese Alternative wird für die erste Architektur verworfen.

Eine spätere Safety- oder Hypervisor-Erweiterung bleibt möglich.

---

### 6.4 Reservierungsbasiertes Echtzeitmodell

Echtzeitthreads erhalten kontrollierte Budgets, Perioden, Deadlines und CPU-Zuordnungen.

Vor der Zulassung findet eine Admission Control statt.

#### Vorteile

- zeitliche Isolation,
- kontrollierbare Echtzeitlast,
- Schutz des restlichen Systems,
- nachvollziehbare Garantien,
- Unterstützung verschiedener Echtzeitkategorien,
- gute Mehrkernintegration.

#### Nachteile

- höhere Implementierungskomplexität,
- Reservierungen müssen global koordiniert werden,
- Anwendungen müssen zeitliche Anforderungen angeben,
- Garantien hängen weiterhin von Hardware und Treibern ab.

Diese Alternative wird gewählt.

---

## 7. Entscheidung

NovaOS verwendet ein reservierungsbasiertes Echtzeitmodell.

Echtzeitprozesse erhalten keine pauschale oder unbegrenzte Vorrangstellung. Stattdessen werden Echtzeitanforderungen für einzelne Threads explizit beschrieben, geprüft und genehmigt.

Eine Reservierung enthält mindestens:

```text
runtime_budget
period
relative_deadline
realtime_policy
realtime_category
cpu_affinity
overrun_policy
deadline_miss_policy
```

Ein Echtzeitthread darf nur aktiviert werden, wenn:

1. der Prozess die erforderliche Capability besitzt,
2. die Parameter gültig sind,
3. ausreichend Echtzeitkapazität verfügbar ist,
4. die Admission Control zustimmt,
5. die benötigten Ressourcen echtzeitgeeignet sind,
6. eine definierte Überlastungsstrategie existiert.

Das Betriebssystem reserviert immer einen Teil der CPU-Zeit für:

- Scheduler,
- Interruptverarbeitung,
- Kernelwartung,
- Recovery-Aufgaben,
- normale Systemdienste.

Echtzeitprozesse dürfen niemals die gesamte Systemkapazität reservieren.

---

## 8. Prozess- und Threadmodell

Ein Echtzeitprozess ist weiterhin ein normaler NovaOS-Prozess gemäß ADR-PROC-0001.

Er besitzt zusätzlich:

- ein Echtzeitprofil,
- ein Echtzeitbudget,
- Echtzeit-Capabilities,
- erlaubte Echtzeit-Policies,
- eine CPU-Menge,
- Speicherreservierungen,
- Überlastungsrichtlinien,
- Telemetriegrenzen.

Innerhalb des Prozesses können unterschiedliche Threadtypen existieren:

```text
Echtzeitprozess
├── Echtzeitthread
├── normaler Steuerungsthread
├── Hintergrundthread für Protokollierung
└── I/O- oder Kommunikationsthread
```

Nur Threads mit einer genehmigten Echtzeitreservierung gehören zur Scheduler-Klasse `NOVA_SCHED_REALTIME`.

Ein gesamter Prozess darf nicht allein durch eine Prozessmarkierung alle seine Threads automatisch in Echtzeit ausführen.

---

## 9. Echtzeit-Policies

NovaOS unterstützt folgende Echtzeit-Policies:

| Policy | Zweck |
|---|---|
| `NOVA_RT_FIFO` | feste Priorität ohne reguläre Zeitscheibe |
| `NOVA_RT_ROUND_ROBIN` | feste Priorität mit Zeitscheiben |
| `NOVA_RT_DEADLINE` | deadline- und reservierungsbasiertes Scheduling |

### 9.1 FIFO

Bei `NOVA_RT_FIFO` läuft der höchstpriorisierte lauffähige Thread, bis er:

- blockiert,
- freiwillig die CPU abgibt,
- sein Budget verbraucht,
- beendet wird,
- von einem höher priorisierten Echtzeitthread präemptiert wird.

Auch FIFO-Threads unterliegen einem Laufzeitbudget.

Unbegrenzte FIFO-Ausführung ist nicht zulässig.

---

### 9.2 Round Robin

Bei `NOVA_RT_ROUND_ROBIN` teilen sich Threads gleicher effektiver Priorität die CPU über definierte Zeitscheiben.

Die Zeitscheibe:

- ist pro Scheduler-Konfiguration begrenzt,
- zählt gegen das Laufzeitbudget,
- darf die Deadline nicht überschreiten,
- kann architekturabhängig angepasst werden.

---

### 9.3 Deadline

Bei `NOVA_RT_DEADLINE` wird die zeitliche Reservierung durch folgende Werte definiert:

```text
runtime <= deadline <= period
```

Der Scheduler bevorzugt den zulässigen Thread mit der frühesten absoluten Deadline.

Die Policy eignet sich für periodische und sporadische Echtzeitaufgaben.

---

## 10. Admission Control

Jede neue oder veränderte Echtzeitreservierung wird vor ihrer Aktivierung geprüft.

Die Admission Control berücksichtigt mindestens:

- Laufzeitbudget,
- Periode,
- Deadline,
- CPU-Affinität,
- vorhandene Echtzeitreservierungen,
- Kernel-Reserve,
- Interruptlast,
- CPU-Leistung,
- CPU-Isolation,
- heterogene Prozessorkerne,
- thermische Grenzen,
- NUMA-Bindung.

Eine vereinfachte Auslastung eines periodischen Threads lautet:

```text
utilization = runtime_budget / period
```

Die aufsummierte Echtzeitauslastung einer CPU darf die konfigurierte Echtzeitobergrenze nicht überschreiten.

```text
sum(realtime_utilization) <= realtime_capacity
```

Die Echtzeitkapazität ist kleiner als die gesamte CPU-Kapazität.

Beispiel:

```text
Gesamtkapazität:      100 %
Echtzeitobergrenze:    80 %
Systemreserve:         20 %
```

Die konkreten Standardwerte werden in der Spezifikation festgelegt.

Eine abgelehnte Reservierung darf nicht teilweise oder unbemerkt aktiviert werden.

---

## 11. CPU-Reservierung

Eine Echtzeitreservierung kann gelten für:

- eine bestimmte CPU,
- eine definierte CPU-Menge,
- eine reservierte CPU-Gruppe,
- einen Echtzeit-CPU-Pool.

Harte Echtzeitaufgaben sollen bevorzugt auf isolierten oder kontrollierten CPUs ausgeführt werden.

Dabei gelten folgende Regeln:

1. Eine CPU darf nicht vollständig von Systemarbeit abgeschnitten werden.
2. Kernel- und Interruptaufgaben benötigen weiterhin eine definierte Reserve.
3. Echtzeitthreads werden nicht ohne Prüfung auf andere CPUs migriert.
4. Die Admission Control muss jede zulässige Ziel-CPU berücksichtigen.
5. CPU-Hotplug darf eine bestehende Garantie nicht stillschweigend verletzen.
6. Fällt eine reservierte CPU aus, wird die definierte Recovery-Policy ausgeführt.

---

## 12. Laufzeitbudgets

Jeder Echtzeitthread besitzt ein Laufzeitbudget.

Das Budget wird verbraucht, während der Thread tatsächlich CPU-Zeit erhält.

Zum Budget zählen grundsätzlich:

- Userspace-Ausführung,
- direkte Systemaufrufe,
- im Auftrag des Threads ausgeführte Kernelarbeit,
- zurechenbare synchrone IPC-Arbeit.

Nicht eindeutig zurechenbare Interruptarbeit wird gesondert erfasst und bei der Kapazitätsplanung berücksichtigt.

Nach Verbrauch des Budgets wird der Thread abhängig von seiner Policy:

- bis zur nächsten Periode gedrosselt,
- temporär herabgestuft,
- gestoppt,
- über einen Recovery-Thread behandelt,
- kontrolliert beendet.

Das Budget wird nicht allein durch Blockierung oder freiwillige CPU-Abgabe zurückgesetzt.

---

## 13. Budgetauffüllung

Periodische Reservierungen erhalten ihr Budget zu Beginn jeder neuen Periode zurück.

Dabei gelten folgende Regeln:

- nicht verwendetes Budget wird standardmäßig nicht unbegrenzt übertragen,
- Budgetübertragung muss ausdrücklich aktiviert werden,
- angespartes Budget besitzt eine feste Obergrenze,
- eine Budgetauffüllung darf keine bereits verstrichene Deadline reaktivieren,
- verspätete Aktivierungen werden entsprechend der konfigurierten Policy behandelt.

Für sporadische Aufgaben verwendet NovaOS einen kontrollierten Replenishment-Mechanismus, der verhindert, dass mehrere Aktivierungen die reservierte Auslastung überschreiten.

---

## 14. Deadline-Überwachung

Der Kernel überwacht Echtzeit-Deadlines mit monotonen hochauflösenden Timern.

Für jeden Echtzeitauftrag werden mindestens erfasst:

- Aktivierungszeit,
- absolute Deadline,
- Startzeit,
- Abschlusszeit,
- verbrauchte CPU-Zeit,
- verbleibendes Budget,
- Startlatenz,
- Abschlusslatenz,
- Deadline-Miss-Status.

Eine Deadline-Verletzung erzeugt ein lokales Kernelereignis.

Das Ereignis enthält:

```text
process_id
thread_id
reservation_id
activation_time
deadline
completion_time
runtime_used
miss_duration
miss_reason
recovery_action
```

Die Protokollierung darf den Echtzeitpfad nicht durch blockierende I/O-Operationen verzögern.

---

## 15. Verhalten bei Deadline-Verletzung

Jede Reservierung besitzt eine definierte Deadline-Miss-Policy.

Mögliche Reaktionen sind:

| Policy | Reaktion |
|---|---|
| `REPORT` | Verletzung erfassen und Ausführung fortsetzen |
| `DROP_JOB` | verspäteten Auftrag verwerfen |
| `THROTTLE` | Thread bis zur nächsten Periode drosseln |
| `DEMOTE` | Thread temporär in eine niedrigere Klasse verschieben |
| `RESTART_THREAD` | betroffenen Thread kontrolliert neu starten |
| `RESTART_PROCESS` | Prozess über Supervisor neu starten |
| `ACTIVATE_FALLBACK` | definierten Fallback-Dienst aktivieren |
| `TERMINATE` | Thread oder Prozess beenden |
| `SYSTEM_SAFE_STATE` | systemdefinierten sicheren Zustand anfordern |

Nicht jede Anwendung darf jede Reaktion konfigurieren.

Sicherheitsrelevante Reaktionen benötigen zusätzliche Capabilities und eine registrierte Supervisor- oder Safety-Komponente.

---

## 16. Verhalten bei Budgetüberschreitung

Eine Budgetüberschreitung ist nicht identisch mit einem Deadline Miss.

Ein Thread kann sein Budget verbrauchen, bevor die Deadline erreicht ist. Er kann ebenfalls eine Deadline verletzen, obwohl noch Budget vorhanden ist, beispielsweise durch Blockierung.

Bei Budgetüberschreitung wird der Thread standardmäßig:

1. von der CPU entfernt,
2. als gedrosselt markiert,
3. bis zur nächsten Auffüllung blockiert,
4. über ein nicht blockierendes Ereignis benachrichtigt,
5. in der Echtzeit-Telemetrie erfasst.

Ein Echtzeitthread darf sein Budget nicht durch wiederholtes Blockieren, Migrieren oder Neuerzeugen umgehen.

---

## 17. Speicherverwaltung

Echtzeitprozesse benötigen kontrollierbare Speicherlatenzen.

NovaOS unterstützt deshalb:

- Vorabreservierung von physischem Speicher,
- Sperren relevanter Seiten im Arbeitsspeicher,
- Vorabaufbau erforderlicher Seitentabellen,
- Vorberührung von Stack- und Heap-Seiten,
- feste oder begrenzte Speicherpools,
- NUMA-Bindung,
- deterministische Kernelobjekt-Pools.

In einem harten Echtzeitabschnitt sind standardmäßig nicht zulässig:

- nachladbare Seitenfehler,
- Speicher-Swapping,
- unbeschränkte Heap-Erweiterungen,
- blockierende Speicherkompression,
- unvorhersehbare Copy-on-Write-Auflösungen,
- Speicherallokationen mit unbeschränkter Wartezeit.

Ein Echtzeitprozess darf Speicher außerhalb eines Echtzeitabschnitts normal vorbereiten und anschließend in einen gesperrten Echtzeitzustand überführen.

---

## 18. Echtzeitabschnitte

Ein Thread kann einen expliziten Echtzeitabschnitt betreten.

Vor dem Eintritt prüft der Kernel:

- aktive Reservierung,
- verfügbares Budget,
- gültige CPU-Zuordnung,
- vorbereiteten Speicher,
- erlaubte Systemaufrufe,
- erforderliche Capabilities.

Innerhalb eines Echtzeitabschnitts gelten strengere Regeln.

Beispiele:

```text
- keine unbeschränkten Speicherallokationen
- keine blockierenden Dateisystemoperationen
- keine nicht echtzeitfähigen Systemaufrufe
- keine dynamische Modulladung
- keine unkontrollierten Klassenwechsel
```

Das Verlassen des Abschnitts hebt diese zusätzlichen Einschränkungen wieder auf.

---

## 19. Systemaufrufe

Systemaufrufe werden in Echtzeitfähigkeitsklassen eingeteilt.

| Klasse | Bedeutung |
|---|---|
| `RT_SAFE` | für Echtzeitabschnitte zugelassen |
| `RT_BOUNDED` | zugelassen mit dokumentierter oberer Latenzgrenze |
| `RT_CONDITIONAL` | nur unter definierten Vorbedingungen zugelassen |
| `RT_UNSAFE` | innerhalb harter Echtzeitabschnitte nicht zulässig |

Beispiele für potenziell nicht echtzeitfähige Operationen:

- Dateisystemzugriffe ohne vorab reservierte Daten,
- dynamisches Laden von Programmen oder Modulen,
- Namensauflösung über das Netzwerk,
- unbeschränkte Speicherallokationen,
- Prozessstart,
- Geräteoperationen ohne garantierte Antwortzeit.

Die endgültige Einstufung erfolgt in den jeweiligen Subsystem-Spezifikationen.

---

## 20. Synchronisation

Echtzeitthreads dürfen nicht unkontrolliert auf normale Sperren warten.

Echtzeitfähige Synchronisationsobjekte müssen mindestens eines der folgenden Verfahren unterstützen:

- Prioritätsvererbung,
- Priority Ceiling,
- zeitbegrenztes Warten,
- nicht blockierende Algorithmen,
- kontrollierte Besitzerbeendigung.

Jede blockierende Operation eines Echtzeitthreads muss:

- eine Zeitgrenze besitzen oder
- Teil einer analysierten Abhängigkeitskette sein.

Unbegrenztes Warten auf einen nicht echtzeitfähigen Thread ist innerhalb harter Echtzeitabschnitte unzulässig.

---

## 21. Prioritätsinversion

Blockiert ein Echtzeitthread auf einer Ressource, die von einem niedriger priorisierten Thread gehalten wird, wird gemäß ADR-PROC-0004 eine temporäre Prioritätsvererbung aktiviert.

Dabei gelten zusätzliche Echtzeitregeln:

1. Die Vererbung muss transitiv funktionieren.
2. Der Ressourcenhalter erhält nur die notwendige effektive Priorität.
3. Die zugehörige CPU-Zeit muss abrechenbar bleiben.
4. Die Vererbung endet unmittelbar nach Freigabe der Ressource.
5. Lange Blockierungen erzeugen ein Diagnoseereignis.
6. Critical-Prioritäten werden nicht an Userspace übertragen.
7. Echtzeitbudgets dürfen nicht unbegrenzt umgangen werden.

---

## 22. IPC

NovaOS unterstützt echtzeitfähige IPC-Pfade.

Ein echtzeitfähiger IPC-Kanal besitzt mindestens:

- vorab reservierte Nachrichtenpuffer,
- begrenzte Nachrichtengröße,
- begrenzte Warteschlangenlänge,
- definierte Überlastungsstrategie,
- Prioritätsweitergabe,
- Zeitüberschreitungen,
- nicht blockierende Fehlerpfade.

Bei synchroner IPC kann die effektive Priorität und Deadline kontrolliert an den Serverthread weitergegeben werden.

```text
Echtzeit-Client
      ↓
synchroner IPC-Aufruf
      ↓
echtzeitfähiger Serverthread
      ↓
temporäre Prioritäts- und Deadline-Weitergabe
      ↓
Antwort oder Timeout
```

Ein normaler Dienst wird nicht automatisch echtzeitfähig, nur weil ein Echtzeitthread ihn aufruft.

---

## 23. I/O und Treiber

Eine Echtzeitgarantie kann nur I/O-Vorgänge einschließen, wenn der gesamte relevante Pfad echtzeitfähig ist.

Dazu gehören:

- Treiber,
- Gerätescheduler,
- Bus,
- DMA-Verwaltung,
- Interruptverarbeitung,
- Gerät,
- Firmware.

Echtzeitfähige Treiber sollen:

- Speicher vorab reservieren,
- begrenzte Warteschlangen verwenden,
- Interruptarbeit minimieren,
- längere Arbeit in priorisierte Worker verlagern,
- Zeitüberschreitungen unterstützen,
- unbeschränkte Sperren vermeiden,
- Latenzgrenzen dokumentieren.

Ein Gerät ohne bekannte maximale Antwortzeit darf nicht als hart echtzeitfähig gekennzeichnet werden.

---

## 24. Interrupts

Interruptlatenz ist Bestandteil der Echtzeitanalyse.

NovaOS unterscheidet:

- kurze unmittelbare Interruptbehandlung,
- verzögerte Interruptarbeit,
- priorisierte Interrupt-Worker.

Regeln:

1. Unmittelbare Interruptbehandlung muss möglichst kurz bleiben.
2. Längere Arbeit wird in einen geeigneten Worker verschoben.
3. Interrupt-Worker erhalten kontrollierte Prioritäten.
4. Interrupt-Stürme müssen begrenzt werden.
5. Echtzeit-CPUs sollen möglichst von nicht relevanten Interrupts entlastet werden.
6. Interrupt-Affinitäten müssen konfigurierbar sein.
7. Interruptlatenzen werden lokal gemessen.

---

## 25. Timer und Zeitbasis

Echtzeitprozesse verwenden ausschließlich monotone Zeitquellen.

Die Zeitbasis muss:

- hochauflösend,
- monoton,
- pro CPU synchronisiert oder korrigierbar,
- gegen Frequenzwechsel stabil,
- für Deadline-Berechnungen geeignet sein.

Die Echtzeitschnittstelle darf nicht direkt von veränderbaren Kalenderzeiten abhängig sein.

Eine Änderung der Systemuhr darf bestehende Echtzeit-Deadlines nicht verschieben.

---

## 26. Energie- und Temperaturverwaltung

Energie- und Temperaturmanagement können Echtzeitgarantien beeinflussen.

Deshalb gelten folgende Regeln:

- genehmigte Garantien müssen bekannte CPU-Frequenzen berücksichtigen,
- Frequenzwechsel dürfen Reservierungen nicht unbemerkt ungültig machen,
- thermische Drosselung muss als möglicher Garantieverlust gemeldet werden,
- Echtzeit-CPUs können definierte Mindestleistungszustände erhalten,
- harte Hardware-Schutzgrenzen besitzen immer Vorrang.

NovaOS darf Hardware nicht über sichere Temperatur- oder Leistungsgrenzen hinaus betreiben, um eine Deadline einzuhalten.

---

## 27. Mehrkern- und NUMA-Systeme

Echtzeitthreads können an CPUs und NUMA-Knoten gebunden werden.

Die Platzierung berücksichtigt:

- reservierte CPU-Kapazität,
- Speicherlokalität,
- Cache-Lokalität,
- Interrupt-Affinität,
- Gerätezuständigkeit,
- CPU-Leistungsklasse,
- thermische Bedingungen.

Unkontrollierte Migrationen sind zu vermeiden.

Eine Migration ist zulässig, wenn:

- die Ziel-CPU Teil der genehmigten CPU-Menge ist,
- dort ausreichende Kapazität vorhanden ist,
- die Deadline weiterhin eingehalten werden kann,
- Speicher- und Cache-Kosten berücksichtigt wurden.

---

## 28. CPU-Hotplug und Ausfälle

Eine CPU mit aktiven Echtzeitreservierungen darf nicht ohne Vorbereitung entfernt werden.

Vor einem geplanten CPU-Hotplug muss der Kernel:

1. betroffene Reservierungen ermitteln,
2. alternative CPUs prüfen,
3. die Admission Control erneut ausführen,
4. Threads kontrolliert migrieren oder anhalten,
5. Anwendungen über Änderungen informieren.

Bei einem ungeplanten CPU-Ausfall wird die konfigurierte Recovery-Policy ausgeführt.

Mögliche Reaktionen:

- Migration auf eine Reserve-CPU,
- reduzierte Qualitätsstufe,
- Aktivierung eines Fallback-Prozesses,
- Übergang in einen sicheren Zustand,
- kontrollierter Abbruch der Echtzeitfunktion.

---

## 29. Sicherheitsmodell

Echtzeitfunktionen sind privilegierte Ressourcen.

Mögliche Capabilities sind:

```text
CAP_RT_CREATE
CAP_RT_CONFIGURE
CAP_RT_HARD
CAP_RT_RESERVE_CPU
CAP_RT_LOCK_MEMORY
CAP_RT_SET_AFFINITY
CAP_RT_CONTROL_FOREIGN
CAP_RT_SAFE_STATE
CAP_RT_ADMIN
```

Capabilities können eingeschränkt werden auf:

- bestimmte Prozesse,
- bestimmte Threads,
- bestimmte CPU-Mengen,
- maximale Priorität,
- maximales Laufzeitbudget,
- bestimmte Echtzeitkategorien,
- bestimmte Policies,
- bestimmte Geräte,
- eine begrenzte Gültigkeitsdauer.

Eine Capability ersetzt weder die Admission Control noch die Ressourcenprüfung.

---

## 30. Ressourcenlimits

Echtzeitressourcen werden auf mehreren Ebenen begrenzt:

- Thread,
- Prozess,
- Benutzer,
- Sicherheitsdomäne,
- Container,
- CPU,
- CPU-Gruppe,
- Gesamtsystem.

Mögliche Limits sind:

```text
max_realtime_threads
max_runtime_per_period
max_realtime_utilization
max_locked_memory
max_realtime_priority
max_reserved_cpus
max_ipc_buffers
max_deadline_misses
```

Ein Prozess darf seine Limits nicht durch das Erzeugen weiterer Prozesse oder Threads umgehen.

---

## 31. API-Grundmodell

Die öffentliche Schnittstelle soll mindestens folgende Operationen unterstützen:

```c
nova_status_t nova_realtime_reservation_create(
    const nova_realtime_params_t* params,
    nova_handle_t* reservation
);

nova_status_t nova_realtime_reservation_attach(
    nova_handle_t reservation,
    nova_handle_t thread
);

nova_status_t nova_realtime_reservation_update(
    nova_handle_t reservation,
    const nova_realtime_params_t* params
);

nova_status_t nova_realtime_reservation_query(
    nova_handle_t reservation,
    nova_realtime_info_t* info
);

nova_status_t nova_realtime_reservation_release(
    nova_handle_t reservation
);

nova_status_t nova_realtime_section_enter(
    nova_handle_t reservation
);

nova_status_t nova_realtime_section_leave(void);
```

Ein mögliches Datenmodell lautet:

```c
typedef enum
{
    NOVA_RT_SOFT = 0,
    NOVA_RT_FIRM,
    NOVA_RT_HARD
} nova_realtime_category_t;

typedef enum
{
    NOVA_RT_FIFO = 0,
    NOVA_RT_ROUND_ROBIN,
    NOVA_RT_DEADLINE
} nova_realtime_policy_t;

typedef enum
{
    NOVA_RT_MISS_REPORT = 0,
    NOVA_RT_MISS_DROP_JOB,
    NOVA_RT_MISS_THROTTLE,
    NOVA_RT_MISS_DEMOTE,
    NOVA_RT_MISS_RESTART_THREAD,
    NOVA_RT_MISS_RESTART_PROCESS,
    NOVA_RT_MISS_ACTIVATE_FALLBACK,
    NOVA_RT_MISS_TERMINATE,
    NOVA_RT_MISS_SYSTEM_SAFE_STATE
} nova_realtime_miss_policy_t;

typedef struct
{
    nova_realtime_category_t category;
    nova_realtime_policy_t policy;
    nova_realtime_miss_policy_t miss_policy;

    uint8_t priority;
    uint8_t reserved[7];

    uint64_t runtime_ns;
    uint64_t period_ns;
    uint64_t deadline_ns;

    nova_cpu_set_t cpu_affinity;

    uint64_t max_locked_memory;
    uint32_t flags;
} nova_realtime_params_t;
```

Die endgültige ABI wird in einer separaten Spezifikation festgelegt.

---

## 32. Echtzeit-Reservierungsobjekt

Jede genehmigte Reservierung wird als Kernelobjekt dargestellt.

Das Objekt enthält mindestens:

```text
reservation_id
owner_process
attached_thread
category
policy
priority
runtime_budget
remaining_budget
period
relative_deadline
absolute_deadline
cpu_affinity
admission_state
overrun_policy
miss_policy
statistics
generation
```

Reservierungsobjekte sind über Handles und Capabilities geschützt.

Das Schließen eines Handles beendet eine Reservierung nicht automatisch, solange noch gültige Kernelreferenzen oder Threadbindungen bestehen.

---

## 33. Lebenszyklus einer Reservierung

Eine Reservierung durchläuft folgende Zustände:

```text
CREATED
   ↓
VALIDATING
   ↓
ADMITTED
   ↓
ACTIVE
   ↓
THROTTLED
   ↓
ACTIVE

ACTIVE
   ↓
SUSPENDED
   ↓
ACTIVE

ACTIVE
   ↓
REVOKED
   ↓
RELEASED
```

Mögliche Zustände:

| Zustand | Bedeutung |
|---|---|
| `CREATED` | Objekt wurde erzeugt, aber noch nicht geprüft |
| `VALIDATING` | Parameter und Kapazität werden geprüft |
| `ADMITTED` | Reservierung wurde zugelassen |
| `ACTIVE` | Reservierung ist an einen Thread gebunden |
| `THROTTLED` | Budget ist vorübergehend erschöpft |
| `SUSPENDED` | Reservierung wurde kontrolliert ausgesetzt |
| `REVOKED` | Reservierung wurde durch Systemrichtlinie entzogen |
| `RELEASED` | Reservierung wurde endgültig freigegeben |

Alle Zustandswechsel müssen atomar und diagnostizierbar sein.

---

## 34. Prozessstart

Ein Programm darf ein Echtzeitprofil in seinen Metadaten angeben.

Dieses Profil ist lediglich eine Anforderung und keine automatische Genehmigung.

Beim Prozessstart prüft der Systemdienst:

- Programmsignatur oder Vertrauensstatus,
- angeforderte Capabilities,
- Benutzer- und Systemrichtlinien,
- benötigte CPUs,
- benötigten gesperrten Speicher,
- erforderliche Geräte,
- zulässige Echtzeitkategorie.

Erst danach können konkrete Reservierungen angelegt werden.

Ein Prozess startet standardmäßig zunächst ohne aktive Echtzeitgarantie.

---

## 35. Prozessbeendigung

Bei der Beendigung eines Echtzeitprozesses muss der Kernel:

1. neue Aktivierungen verhindern,
2. Echtzeitthreads kontrolliert stoppen,
3. Reservierungen deaktivieren,
4. Prioritätsvererbungen entfernen,
5. gesperrten Speicher freigeben,
6. IPC-Abhängigkeiten auflösen,
7. Fallback- oder Supervisor-Dienste informieren,
8. Ressourcen an die Admission Control zurückgeben,
9. abschließende Telemetrie erzeugen.

Ein Prozess darf keine aktiven Echtzeitreservierungen oder CPU-Bindungen hinterlassen.

---

## 36. Supervisor und Recovery

Kritische Echtzeitprozesse können einem Supervisor zugeordnet werden.

Der Supervisor kann:

- Heartbeats überwachen,
- Deadline-Verletzungen empfangen,
- Threads oder Prozesse neu starten,
- eine reduzierte Betriebsart aktivieren,
- einen Fallback-Prozess starten,
- einen sicheren Zustand anfordern.

Der Supervisor muss von der überwachten Anwendung getrennt sein.

Ein Supervisor ist nicht automatisch selbst echtzeitfähig. Falls seine Reaktion zeitlich garantiert werden muss, benötigt er eine eigene Reservierung.

---

## 37. Sichere Zustände

NovaOS kann systemdefinierte sichere Zustände koordinieren, garantiert jedoch nicht ohne Weiteres die funktionale Sicherheit einer Gesamtanlage.

Ein sicherer Zustand kann beispielsweise bedeuten:

- Aktoren deaktivieren,
- Ausgänge auf definierte Werte setzen,
- Maschine kontrolliert stoppen,
- Steuerung an einen Fallback-Controller übergeben,
- fehlerhafte Datenströme sperren.

Die konkrete Bedeutung wird von der jeweiligen Anwendung und Hardware definiert.

Ein allgemeiner Kernel-Panic ist nicht automatisch ein sicherer Zustand.

---

## 38. Telemetrie und Diagnose

Für jeden Echtzeitthread werden mindestens folgende Werte erfasst:

- Anzahl der Aktivierungen,
- Anzahl abgeschlossener Aufträge,
- Anzahl der Deadline-Verletzungen,
- Anzahl der Budgetüberschreitungen,
- minimale Laufzeit,
- maximale Laufzeit,
- durchschnittliche Laufzeit,
- maximale Startlatenz,
- maximale Abschlusslatenz,
- gemessener Jitter,
- Anzahl der Präemptionen,
- Anzahl der CPU-Migrationen,
- Dauer von Sperrblockierungen,
- Dauer von IPC-Aufrufen,
- Anzahl der Prioritätsvererbungen,
- Drosselungszeit,
- Reservierungszustand.

Die Telemetrie verwendet:

- vorab reservierte Ringpuffer,
- nicht blockierende Schreibpfade,
- begrenzte Ereignisgrößen,
- verzögerte Aufbereitung außerhalb des Echtzeitpfades.

---

## 39. Garantien und Garantiegrenzen

NovaOS unterscheidet zwischen:

- angeforderter Garantie,
- zugelassener Garantie,
- aktuell erfüllbarer Garantie,
- tatsächlich gemessener Einhaltung.

Eine Garantie gilt nur unter dokumentierten Bedingungen.

Mögliche Einschränkungen sind:

- nicht qualifizierte Hardware,
- System Management Interrupts,
- unkontrollierbare Firmware,
- thermische Drosselung,
- fehlerhafte Geräte,
- externe Busblockierungen,
- nicht echtzeitfähige Treiber,
- physische Hardwareausfälle.

NovaOS darf eine harte Echtzeitgarantie nicht ausweisen, wenn bekannte Komponenten der Ausführungskette keine begrenzbare Maximalzeit besitzen.

---

## 40. Sicherheitskritische Systeme

Die Echtzeitmechanismen von NovaOS bilden eine technische Grundlage, aber keine automatische Zertifizierung für sicherheitskritische Anwendungen.

Für einen Einsatz nach Normen wie:

- IEC 61508,
- ISO 26262,
- EN 50128,
- IEC 62304,
- DO-178C

sind zusätzliche Prozesse, Nachweise, Tests, Werkzeuge und Zertifizierungen erforderlich.

Eine allgemeine NovaOS-Installation darf nicht ohne entsprechende Qualifikation als sicherheitszertifiziert bezeichnet werden.

---

## 41. Fehlerbehandlung

Eine Echtzeitanforderung wird abgelehnt, wenn:

- Parameter ungültig sind,
- `runtime > deadline` gilt,
- `deadline > period` gilt,
- die erforderliche Capability fehlt,
- die CPU-Kapazität nicht ausreicht,
- der Speicher nicht reserviert werden kann,
- die CPU-Affinität ungültig ist,
- die gewünschte Policy nicht unterstützt wird,
- ein benötigtes Gerät nicht echtzeitfähig ist,
- eine Sicherheitsrichtlinie widerspricht.

Mögliche Statuswerte sind:

```text
NOVA_STATUS_OK
NOVA_STATUS_INVALID_ARGUMENT
NOVA_STATUS_PERMISSION_DENIED
NOVA_STATUS_ADMISSION_DENIED
NOVA_STATUS_CAPACITY_EXCEEDED
NOVA_STATUS_MEMORY_NOT_LOCKED
NOVA_STATUS_DEVICE_NOT_REALTIME
NOVA_STATUS_POLICY_CONFLICT
NOVA_STATUS_RESERVATION_REVOKED
NOVA_STATUS_DEADLINE_MISSED
NOVA_STATUS_BUDGET_EXHAUSTED
NOVA_STATUS_NOT_SUPPORTED
```

Eine ungültige Userspace-Anforderung darf keinen Kernel-Panic auslösen.

---

## 42. Konsequenzen

### 42.1 Positive Konsequenzen

- kontrollierte Unterstützung zeitkritischer Anwendungen,
- Schutz vor unbegrenzter Echtzeitlast,
- zeitliche Isolation durch Reservierungen,
- klare Unterscheidung zwischen harter, fester und weicher Echtzeit,
- überprüfbare CPU-Budgets,
- definierte Reaktion auf Deadline-Verletzungen,
- gute Integration in das Capability-Modell,
- Unterstützung von Mehrkern- und NUMA-Systemen,
- detaillierte Diagnosemöglichkeiten,
- Grundlage für Robotik und industrielle Anwendungen.

### 42.2 Negative Konsequenzen

- erhebliche Scheduler- und Kernelkomplexität,
- hoher Test- und Analyseaufwand,
- Anwendungen müssen zeitliche Anforderungen kennen,
- Speicher und I/O benötigen eigene Echtzeitpfade,
- ein Teil der Ressourcen bleibt als Reserve ungenutzt,
- Hardware und Firmware können Garantien begrenzen,
- harte Echtzeit erfordert zusätzliche Qualifikation.

### 42.3 Risiken

- falsche WCET-Angaben,
- Überbuchung von CPU-Kapazität,
- fehlerhafte Budgetabrechnung,
- unkontrollierte Interruptlatenzen,
- Prioritätsinversion,
- thermisch bedingter Garantieverlust,
- unbemerkte nicht echtzeitfähige Treiberpfade,
- fehlerhafte Recovery-Reaktionen.

Diese Risiken werden begrenzt durch:

- Admission Control,
- feste Reservierungen,
- Laufzeitüberwachung,
- Deadline-Telemetrie,
- Capability-Prüfungen,
- Speicherreservierung,
- echtzeitfähige IPC- und Synchronisationsobjekte,
- definierte Fallback- und Recovery-Policies.

---

## 43. Auswirkungen auf andere Subsysteme

### 43.1 Scheduler

Der Scheduler muss Budgets, Perioden, Deadlines, Drosselung und CPU-Reservierungen verwalten.

### 43.2 Speicherverwaltung

Die Speicherverwaltung muss gesperrte Seiten, vorab reservierte Pools und NUMA-Bindung unterstützen.

### 43.3 IPC

IPC benötigt begrenzte Warteschlangen, vorab reservierte Puffer und kontrollierte Prioritätsweitergabe.

### 43.4 Synchronisation

Synchronisationsobjekte benötigen Prioritätsvererbung, Priority Ceiling und begrenzte Wartezeiten.

### 43.5 Treibermodell

Treiber müssen ihre Echtzeitfähigkeit und Latenzgrenzen deklarieren können.

### 43.6 Energieverwaltung

Leistungs- und Temperaturänderungen müssen mit aktiven Echtzeitgarantien koordiniert werden.

### 43.7 Capability-System

Alle privilegierten Echtzeitoperationen werden über feingranulare Capabilities geschützt.

### 43.8 Telemetrie

Echtzeitereignisse müssen ohne blockierende Ausgabe erfasst werden.

### 43.9 Prozess-Supervision

Supervisor-Dienste müssen Deadline-Verletzungen und Prozessausfälle behandeln können.

---

## 44. Implementierungsrichtlinien

1. Zunächst wird weiche Echtzeit unterstützt.
2. FIFO und Round Robin dürfen vor Deadline Scheduling implementiert werden.
3. Jede Echtzeitpolicy benötigt von Beginn an ein Laufzeitbudget.
4. Unbegrenzte Echtzeitthreads sind nicht zulässig.
5. Die Admission Control wird vor Aktivierung jeder Reservierung ausgeführt.
6. Der Echtzeitpfad darf keine blockierenden Speicherallokationen durchführen.
7. Timer verwenden ausschließlich monotone hochauflösende Zeitquellen.
8. Deadline- und Budgetereignisse werden über vorab reservierte Puffer erfasst.
9. Harte Echtzeit wird erst aktiviert, wenn Speicher, IPC, Timer, Treiber und Interruptpfade ausreichend qualifiziert sind.
10. Scheduler- und Reservierungsparameter werden über versionierte Strukturen bereitgestellt.
11. Reservierungen müssen atomar aktiviert, geändert und entfernt werden.
12. Fehler in einer Echtzeitanwendung dürfen nicht automatisch den gesamten Kernel zum Absturz bringen.

---

## 45. Testanforderungen

Mindestens folgende Szenarien müssen getestet werden:

- Erzeugung einer gültigen Reservierung,
- Abweisung ungültiger Zeitparameter,
- Abweisung ohne Capability,
- Abweisung bei unzureichender CPU-Kapazität,
- Aktivierung eines Echtzeitthreads,
- FIFO-Scheduling,
- Round-Robin-Scheduling,
- Deadline-Scheduling,
- Budgetverbrauch und Drosselung,
- periodische Budgetauffüllung,
- Deadline-Verletzung,
- konfigurierte Miss-Policies,
- mehrere Echtzeitthreads auf einer CPU,
- Echtzeitthreads auf mehreren CPUs,
- CPU-Affinität,
- NUMA-Bindung,
- Prioritätsvererbung,
- transitive Prioritätsvererbung,
- synchrone Echtzeit-IPC,
- IPC-Timeout,
- gesperrte Speicherseiten,
- verbotener Seitenfehler im Echtzeitabschnitt,
- Echtzeitabschnitt mit nicht erlaubtem Systemaufruf,
- Interrupt-Sturm,
- thermische Drosselung,
- CPU-Hotplug,
- ungeplanter CPU-Ausfall,
- Prozessbeendigung mit aktiver Reservierung,
- Thread-Absturz während eines Echtzeitauftrags,
- Supervisor-Neustart,
- Fallback-Aktivierung,
- Telemetrie unter hoher Ereignislast,
- parallele Änderung mehrerer Reservierungen,
- Verhalten unter Speicherknappheit,
- Schutz der Systemreserve.

---

## 46. Nicht Bestandteil dieser Entscheidung

Diese ADR legt nicht abschließend fest:

- konkrete Standardwerte für die Echtzeitobergrenze,
- genaue Scheduler-Datenstrukturen,
- den vollständigen Admission-Control-Algorithmus,
- WCET-Analysemethoden,
- konkrete Interruptlatenzgrenzen,
- die vollständige Liste echtzeitfähiger Systemaufrufe,
- Zertifizierungsverfahren,
- gerätespezifische Echtzeitgarantien,
- die endgültige öffentliche ABI,
- einen separaten Safety-Hypervisor.

Diese Details werden in nachfolgenden ADRs und NPSPEC-Dokumenten definiert.

---

## 47. Verbindliche Invarianten

1. Nur Threads werden direkt vom Scheduler ausgeführt.
2. Ein Echtzeitprozess erhält keine pauschale Echtzeitpriorität für alle Threads.
3. Jeder Echtzeitthread benötigt eine genehmigte Reservierung.
4. Jede Reservierung besitzt ein begrenztes Laufzeitbudget.
5. Echtzeitkapazität darf niemals die vollständige CPU-Kapazität belegen.
6. Vor jeder Aktivierung oder wesentlichen Änderung erfolgt eine Admission Control.
7. Echtzeitfunktionen erfordern passende Capabilities.
8. Seitenfehler und unbeschränkte Speicherallokationen sind in harten Echtzeitabschnitten unzulässig.
9. Prioritätsvererbung darf keine unbegrenzte CPU-Nutzung ermöglichen.
10. Deadline-Verletzungen müssen erkannt und diagnostizierbar gemacht werden.
11. Budgetüberschreitungen dürfen das Gesamtsystem nicht dauerhaft blockieren.
12. Kalenderzeitänderungen dürfen Echtzeit-Deadlines nicht verändern.
13. CPU-Migrationen dürfen genehmigte Reservierungen nicht unkontrolliert verletzen.
14. Ein Kernel-Panic ist nicht automatisch ein sicherer Systemzustand.
15. NovaOS darf keine harte Echtzeitgarantie ausweisen, wenn die relevante Ausführungskette nicht begrenzbar ist.
16. Eine Capability ersetzt niemals Admission Control oder Ressourcenprüfung.
17. KI- oder lernbasierte Komponenten dürfen keine Echtzeitbudgets, Deadlines oder Schutzgrenzen umgehen.

---

## 48. Referenzen

- ADR-PROC-0001 – Process Lifecycle
- ADR-PROC-0002 – Thread Lifecycle
- ADR-PROC-0003 – Scheduler Classes
- ADR-PROC-0004 – Prioritäten
- ADR-KERNEL-0100 – Kernel Object Graph statt isolierter Subsysteme
- ADR-KERNEL-0101 – Ereignisgesteuerter Kernel-Core
- ADR-KERNEL-0102 – Einheitliches Objektmodell für Kernel und Userspace
- ADR-KERNEL-0103 – Native Capability-basierte Ressourcenverwaltung
- ADR-KERNEL-0104 – Integrierte Telemetrie- und Diagnoseschnittstellen
- ADR-KERNEL-0105 – ABI-Stabilität über versionierte Kernel-Services
- ADR-MM-0012 – NUMA Memory Policy
- ADR-MM-0013 – Guard Pages
- ADR-MM-0014 – DMA Memory
- ADR-MM-0015 – Kernel Stack Layout
- NPSPEC-SCHEDULER-0001 – Scheduler Specification
- NPSPEC-SCHEDULER-PRIORITY-0001 – Scheduler Priority Specification
- NPSPEC-REALTIME-0001 – Realtime Process and Thread Specification
- NPSPEC-PROCESS-0001 – Process Specification
- NPSPEC-THREAD-0001 – Thread Specification

---

## 49. Zusammenfassung der Entscheidung

NovaOS verwendet ein reservierungsbasiertes Echtzeitmodell.

Ein Echtzeitprozess ist ein normaler Prozess mit mindestens einem genehmigten Echtzeitthread und zusätzlichen Ressourcen-, Sicherheits- und Überwachungsrichtlinien. Da ausschließlich Threads planbare Einheiten sind, erhält nicht der gesamte Prozess automatisch eine Echtzeitpriorität.

Jeder Echtzeitthread benötigt ein begrenztes Laufzeitbudget, eine Periode, eine Deadline, eine Scheduler-Policy, eine CPU-Zuordnung und eine definierte Fehlerstrategie. Vor seiner Aktivierung prüft die Admission Control, ob die benötigte Kapazität tatsächlich verfügbar ist.

NovaOS unterstützt weiche, feste und harte Echtzeit sowie die Policies FIFO, Round Robin und Deadline. Speicher, IPC, Synchronisation, Interrupts, Treiber und I/O müssen in die Echtzeitanalyse einbezogen werden.

Unbegrenzte Echtzeitthreads sind nicht zulässig. Ein Teil der CPU-Kapazität bleibt stets für Kernel-, Recovery- und normale Systemarbeit reserviert. Harte Echtzeit darf nur zugesagt werden, wenn die gesamte relevante Ausführungskette überprüfbare maximale Latenzen besitzt.