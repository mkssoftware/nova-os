# ADR-PROC-0009 – Job Objects

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-PROC-0009 |
| Titel | Job Objects |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-30 |
| Bereich | Kernel / Prozesse / Ressourcenverwaltung / Isolation |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-PROC-0001, ADR-PROC-0002, ADR-PROC-0003, ADR-PROC-0004, ADR-PROC-0005, ADR-PROC-0007 |
| Nachfolgende Spezifikation | NPSPEC-JOB-0001 |

---

## 1. Kontext

NovaOS benötigt eine Möglichkeit, mehrere zusammengehörige Prozesse gemeinsam zu verwalten.

Moderne Anwendungen bestehen häufig nicht mehr aus einem einzelnen Prozess.

Beispiele:

- grafische Anwendung mit Hilfsprozessen,
- Browser mit getrennten Renderer-Prozessen,
- Entwicklungsumgebung mit Compiler- und Analyseprozessen,
- lokale KI-Anwendung mit Modell- und Worker-Prozessen,
- Systemdienst mit mehreren Hilfsdiensten,
- virtuelle Maschine mit Geräte- und Verwaltungsprozessen,
- Kompatibilitätsumgebung für Windows-, Android- oder Linux-Anwendungen,
- Container oder Sandbox,
- zeitlich begrenzte Compute-Aufgabe,
- überwachte Prozessgruppe.

Diese Prozesse sollen gemeinsam:

- gestartet,
- überwacht,
- begrenzt,
- pausiert,
- fortgesetzt,
- beendet,
- abgerechnet,
- einer Sicherheits- oder Ressourcenrichtlinie zugeordnet

werden können.

Eine einfache Eltern-Kind-Beziehung reicht hierfür nicht aus. Prozesse können ihren Elternprozess überleben, weitere Prozesse erzeugen oder unabhängig voneinander gestartet werden.

NovaOS führt deshalb Job Objects als eigenständige Kernelobjekte zur hierarchischen Verwaltung von Prozessgruppen ein.

---

## 2. Problemstellung

Ohne Job Objects müssten Anwendungen und Systemdienste jeden Prozess einzeln verwalten.

Dadurch entstehen mehrere Probleme:

- Kindprozesse können der Überwachung entkommen.
- Ressourcenlimits lassen sich nur pro Prozess festlegen.
- Eine Anwendung kann Limits durch zusätzliche Prozesse umgehen.
- Gemeinsames Beenden ist fehleranfällig.
- CPU- und Speicherverbrauch lassen sich nicht zuverlässig zusammenfassen.
- Prozessgruppen besitzen keine stabile Kernelidentität.
- verschachtelte Dienste und Sandboxes sind schwer abzubilden.
- Supervisoren müssen komplexe Prozesslisten im Userspace führen.
- Race Conditions zwischen Prozesserzeugung und Gruppenzuordnung sind möglich.
- Sicherheitsrichtlinien können beim Prozessstart kurzzeitig fehlen.

Job Objects müssen diese Probleme lösen, ohne Prozesse unnötig an eine starre Eltern-Kind-Hierarchie zu binden.

---

## 3. Entscheidungsziele

Job Objects sollen:

1. mehrere Prozesse als verwaltete Einheit zusammenfassen,
2. hierarchische Ressourcenlimits ermöglichen,
3. das Umgehen von Limits durch Kindprozesse verhindern,
4. atomare Prozesszuordnung unterstützen,
5. gemeinsame Lebenszyklusoperationen bereitstellen,
6. CPU-, Speicher-, I/O- und Objektverbrauch abrechnen,
7. mit dem Capability-Modell zusammenarbeiten,
8. Sandboxes und Kompatibilitätsumgebungen unterstützen,
9. verschachtelte Jobs erlauben,
10. Ereignisse und Telemetrie bereitstellen,
11. keine vollständige Containerimplementierung erzwingen,
12. eine stabile Grundlage für Supervisoren und Systemdienste bilden.

---

## 4. Begriffsdefinitionen

### 4.1 Job Object

Ein Job Object ist ein Kernelobjekt, das Prozesse hierarchisch zusammenfasst und gemeinsame Richtlinien, Limits, Statistiken und Lebenszyklusoperationen bereitstellt.

---

### 4.2 Mitgliedsprozess

Ein Mitgliedsprozess ist ein Prozess, der direkt einem Job Object zugeordnet ist.

---

### 4.3 Direkte Mitgliedschaft

Ein Prozess ist direktes Mitglied des Jobs, dem sein Prozessobjekt unmittelbar zugeordnet wurde.

---

### 4.4 Indirekte Mitgliedschaft

Ein Prozess ist indirektes Mitglied eines übergeordneten Jobs, wenn er einem untergeordneten Job angehört.

---

### 4.5 Root Job

Ein Root Job besitzt keinen übergeordneten Job.

NovaOS kann mehrere administrativ getrennte Root Jobs besitzen. Zusätzlich existiert ein interner System-Root-Job.

---

### 4.6 Child Job

Ein Child Job ist einem Parent Job untergeordnet und erbt beziehungsweise verschärft dessen Richtlinien und Limits.

---

### 4.7 Job Controller

Ein Job Controller ist ein Prozess oder Systemdienst mit einem Handle und den erforderlichen Capabilities zur Verwaltung eines Jobs.

---

## 5. Abgrenzung zu anderen Konzepten

Job Objects sind nicht identisch mit:

- Prozessen,
- Eltern-Kind-Prozessbeziehungen,
- Benutzergruppen,
- Sicherheitsdomänen,
- Sessions,
- Containern,
- CPU-Pools,
- Scheduler-Klassen.

Ein Job Object kann diese Konzepte referenzieren oder begrenzen, ersetzt sie aber nicht vollständig.

Ein Container kann beispielsweise aus folgenden Komponenten bestehen:

```text
Job Object
+ Sicherheitsdomäne
+ Namespace
+ Dateisystemansicht
+ Netzwerkisolation
+ Gerätepolicy
```

Das Job Object übernimmt dabei vor allem Lebenszyklus, Ressourcenverwaltung und Abrechnung.

---

## 6. Betrachtete Alternativen

### 6.1 Ausschließliche Eltern-Kind-Verwaltung

Ressourcen und Lebenszyklus werden über den Prozessbaum verwaltet.

#### Vorteile

- kein zusätzliches Kernelobjekt,
- einfache Grundstruktur,
- natürliche Vererbung.

#### Nachteile

- Prozesse können Eltern überleben,
- Umparenting verändert die Verwaltungsstruktur,
- gemeinsam gestartete, aber nicht verwandte Prozesse sind schwer gruppierbar,
- Ressourcenlimits sind unflexibel,
- ungeeignet für komplexe Anwendungen und Sandboxes.

Diese Alternative wird verworfen.

---

### 6.2 Userspace-Prozesslisten

Ein Supervisor verwaltet eine Liste von Prozess-IDs.

#### Vorteile

- geringer Kernelaufwand,
- flexible Userspace-Logik.

#### Nachteile

- Race Conditions beim Prozessstart,
- Prozess-IDs können ungültig oder wiederverwendet werden,
- Kindprozesse können unbemerkt entstehen,
- Limits lassen sich nicht sicher durchsetzen,
- keine atomaren Gruppenoperationen.

Diese Alternative wird verworfen.

---

### 6.3 Flache Kernel-Prozessgruppen

Prozesse können genau einer nicht hierarchischen Gruppe angehören.

#### Vorteile

- einfache Implementierung,
- gemeinsame Signalisierung und Abrechnung möglich.

#### Nachteile

- keine verschachtelten Limits,
- schlechte Abbildung komplexer Anwendungen,
- keine delegierbare Ressourcenhierarchie,
- unzureichend für Container und Systemdienste.

Diese Alternative wird verworfen.

---

### 6.4 Hierarchische Job Objects

Prozesse werden in eigenständigen, hierarchischen Kernelobjekten organisiert.

#### Vorteile

- sichere Ressourcenvererbung,
- hierarchische Abrechnung,
- atomare Gruppenoperationen,
- Unterstützung von Supervisoren und Sandboxes,
- Schutz vor Limitumgehung durch Kindprozesse,
- gute Erweiterbarkeit.

#### Nachteile

- höhere Kernelkomplexität,
- Richtlinienvererbung muss eindeutig definiert werden,
- Verschieben von Prozessen erfordert strenge Regeln,
- umfangreiche Telemetrie und Tests notwendig.

Diese Alternative wird gewählt.

---

## 7. Entscheidung

NovaOS führt hierarchische Job Objects als native Kernelobjekte ein.

Ein Job Object kann enthalten:

- direkt zugeordnete Prozesse,
- untergeordnete Job Objects,
- Ressourcenlimits,
- Scheduler-Richtlinien,
- CPU-Affinitätsgrenzen,
- Speichergrenzen,
- I/O-Grenzen,
- Objekt- und Handlegrenzen,
- Lebenszyklusregeln,
- Sicherheitsreferenzen,
- Ereigniswarteschlangen,
- aggregierte Statistiken.

Jeder Userspace-Prozess gehört zu jedem Zeitpunkt genau einem direkten Job Object an.

Über die Jobhierarchie kann ein Prozess indirekt mehreren übergeordneten Jobs angehören.

---

## 8. System-Root-Job

Beim Systemstart erzeugt NovaOS einen internen System-Root-Job.

Dieser bildet die Wurzel der allgemeinen Jobhierarchie.

```text
System Root Job
├── Kernel Services
├── System Services
├── User Sessions
├── Sandboxes
└── Compatibility Environments
```

Der System-Root-Job:

- kann nicht durch Userspace beendet werden,
- kann nicht einem anderen Job untergeordnet werden,
- besitzt keine normalen Ressourcenlimits,
- wird ausschließlich durch vertrauenswürdige Kernel- und Systemkomponenten verwaltet.

Kernelthreads müssen nicht zwingend normale Mitglieder dieses Jobs sein. Für sie können eigene interne Verwaltungsgruppen existieren.

---

## 9. Hierarchie

Ein Job kann genau einen Parent Job besitzen.

Ein Parent Job kann mehrere Child Jobs enthalten.

Zyklen sind nicht zulässig.

```text
Job A
├── Job B
│   ├── Prozess 1
│   └── Prozess 2
└── Job C
    └── Job D
        └── Prozess 3
```

Prozess 3 ist:

- direktes Mitglied von Job D,
- indirektes Mitglied von Job C,
- indirektes Mitglied von Job A.

Die maximale Hierarchietiefe wird durch eine Systemgrenze beschränkt.

---

## 10. Mitgliedschaft

Jeder Prozess besitzt eine Referenz auf genau einen direkten Job.

Die Mitgliedschaft wird im Prozessobjekt gespeichert.

```c
process->job
```

Ein Prozess ohne gültiges Job Object darf nicht in den normalen Userspace-Startzustand wechseln.

Der Prozessstart muss die Jobzuordnung abschließen, bevor der erste Userspace-Thread ausführbar wird.

Dadurch wird verhindert, dass ein Prozess kurzzeitig außerhalb seiner Ressourcen- und Lebenszyklusregeln läuft.

---

## 11. Atomare Prozesserzeugung

Eine Prozesserzeugungs-API muss die direkte Angabe eines Ziel-Jobs ermöglichen.

Konzeptionell:

```c
nova_status_t nova_process_create(
    const nova_process_create_info_t* create_info,
    nova_handle_t* process
);
```

Die Erzeugungsinformationen enthalten ein Job-Handle oder eine vererbbare Jobrichtlinie.

Der Ablauf lautet:

```text
Prozessobjekt erzeugen
        ↓
Ziel-Job prüfen
        ↓
Joblimits reservieren
        ↓
Prozess atomar zuordnen
        ↓
Adressraum und Threads vorbereiten
        ↓
Prozess ausführbar machen
```

Schlägt die Jobzuordnung fehl, wird der Prozess nicht gestartet.

---

## 12. Vererbung bei Prozesserzeugung

Ein neuer Prozess erbt standardmäßig das direkte Job Object seines Elternprozesses.

Ein Prozess darf einen Kindprozess einem Child Job zuordnen, wenn er die erforderliche Capability besitzt.

Ein Kindprozess darf nicht automatisch in einen weniger eingeschränkten Job wechseln.

Mögliche Erzeugungsregeln sind:

```text
INHERIT_CURRENT_JOB
ASSIGN_CHILD_JOB
ASSIGN_EXPLICIT_JOB
CREATE_CHILD_JOB
```

Explizite Zuordnungen unterliegen immer den Job-, Capability- und Sicherheitsrichtlinien.

---

## 13. Verschieben von Prozessen

Das nachträgliche Verschieben eines Prozesses zwischen Jobs ist eine privilegierte Operation.

Ein Verschieben ist nur zulässig, wenn:

- der Prozess noch nicht gestartet wurde oder kontrolliert angehalten ist,
- Quell- und Ziel-Job die Operation erlauben,
- der Ziel-Job mindestens gleich strenge Sicherheitsgrenzen besitzt,
- alle Ziel-Limits eingehalten werden,
- keine nicht übertragbaren Reservierungen bestehen,
- keine Echtzeitgarantie ungültig wird,
- die erforderlichen Capabilities vorhanden sind.

Standardmäßig dürfen laufende Prozesse nicht in einen weniger eingeschränkten Job verschoben werden.

---

## 14. Keine Flucht aus Jobgrenzen

Ein Prozess darf seine Jobmitgliedschaft nicht selbst entfernen.

Er darf außerdem nicht:

- einen Kindprozess außerhalb der erlaubten Jobhierarchie erzeugen,
- Ressourcen über einen weniger eingeschränkten Hilfsprozess beziehen,
- Limits durch neue Child Jobs umgehen,
- Jobhandles ohne entsprechende Rechte weitergeben,
- seine Statistiken oder Abrechnung zurücksetzen.

Alle Nachkommen unterliegen mindestens den Beschränkungen ihrer übergeordneten Jobs.

---

## 15. Richtlinienvererbung

Richtlinien werden von Parent Jobs an Child Jobs vererbt.

Ein Child Job darf eine geerbte harte Grenze grundsätzlich nur weiter einschränken.

Beispiel:

```text
Parent CPU-Limit: 80 %
Child CPU-Limit:  40 %  → zulässig
Child CPU-Limit: 100 %  → nicht zulässig
```

Richtlinien werden klassifiziert als:

| Typ | Verhalten |
|---|---|
| harte Obergrenze | Child darf nur weiter einschränken |
| harte Untergrenze | Child darf sie nicht unterschreiten |
| geerbter Standardwert | Child darf mit Berechtigung überschreiben |
| unveränderliche Policy | Child darf sie nicht ändern |
| aggregiertes Budget | Nutzung aller Child Jobs wird gemeinsam gezählt |

---

## 16. Effektive Richtlinien

Die effektive Richtlinie eines Jobs ergibt sich aus seiner eigenen Konfiguration und allen übergeordneten Jobs.

Konzeptionell:

```text
effective_limit =
    strengste gültige Grenze
    entlang der Parent-Kette
```

Für Mengen, beispielsweise CPU-Masken, gilt:

```text
effective_cpu_mask =
    parent_effective_cpu_mask
    ∩ job_cpu_mask
```

Für numerische Obergrenzen gilt typischerweise:

```text
effective_max =
    min(parent_effective_max, job_configured_max)
```

Die konkrete Kombinationsregel wird pro Richtlinientyp definiert.

---

## 17. Ressourcenarten

Ein Job kann mindestens folgende Ressourcen begrenzen:

- Anzahl der Prozesse,
- Anzahl der Threads,
- CPU-Zeit,
- CPU-Auslastung,
- Scheduler-Klassen,
- maximale Priorität,
- Echtzeitreservierungen,
- CPU-Affinität,
- physischer Speicher,
- virtueller Speicher,
- gesperrter Speicher,
- Kernelobjekte,
- Handles,
- IPC-Puffer,
- offene Dateien,
- I/O-Bandbreite,
- Netzwerkbandbreite,
- Gerätezugriff,
- GPU- und Compute-Ressourcen.

Nicht alle Ressourcen müssen in der ersten Implementierungsstufe verfügbar sein.

---

## 18. Prozesslimit

Ein Job kann die maximale Anzahl direkt oder hierarchisch enthaltener Prozesse begrenzen.

Mögliche Grenzwerte:

```text
max_direct_processes
max_total_processes
```

`max_direct_processes` zählt nur direkte Mitglieder.

`max_total_processes` zählt alle Prozesse in der gesamten Unterhierarchie.

Die Reservierung eines Prozessplatzes erfolgt vor dem ausführbaren Prozessstart.

---

## 19. Threadlimit

Ein Job kann die Anzahl der Threads begrenzen.

Mögliche Grenzwerte:

```text
max_threads_per_process
max_direct_threads
max_total_threads
```

Ein Prozess darf das Threadlimit nicht durch Erzeugen zusätzlicher Prozesse umgehen.

Threadplätze werden vor der vollständigen Threaderzeugung reserviert.

---

## 20. CPU-Zeitlimit

Ein Job kann ein kumulatives CPU-Zeitlimit besitzen.

Gezählt wird die CPU-Zeit aller enthaltenen Prozesse und Child Jobs.

Mögliche Policies beim Erreichen des Limits:

- Ereignis erzeugen,
- neue Prozesse verweigern,
- Threads drosseln,
- Job pausieren,
- Prozesse kontrolliert beenden,
- gesamten Job beenden.

CPU-Zeitlimits verwenden eine monotone Abrechnung und werden nicht durch Änderungen der Kalenderzeit beeinflusst.

---

## 21. CPU-Auslastungslimit

Ein Job kann die nutzbare CPU-Kapazität über ein Zeitfenster begrenzen.

Beispiel:

```text
CPU-Limit: 200 %
```

Auf einem Mehrkernsystem bedeutet dies eine Gesamtkapazität von zwei vollständig ausgelasteten logischen CPUs.

Die Begrenzung kann umgesetzt werden über:

- Scheduler-Gewichte,
- Laufzeitbudgets,
- periodische Auffüllung,
- CPU-Pools,
- Drosselung.

Die konkrete Umsetzung hängt von Scheduler-Klasse und Jobpolicy ab.

---

## 22. CPU-Affinität

Ein Job kann eine erlaubte CPU-Menge definieren.

Alle Mitgliedsprozesse und Threads werden dadurch eingeschränkt.

```text
effective_thread_affinity =
    system_mask
    ∩ job_effective_mask
    ∩ process_mask
    ∩ thread_mask
```

Ein Child Job darf seine CPU-Menge weiter einschränken, aber nicht ohne Berechtigung über die effektive Parent-Maske hinaus erweitern.

CPU Affinity folgt ADR-PROC-0007.

---

## 23. Scheduler-Klassen und Prioritäten

Ein Job kann festlegen:

- erlaubte Scheduler-Klassen,
- maximale Priorität,
- Standard-Scheduler-Klasse,
- erlaubte Nice-Werte,
- maximale Anzahl von Echtzeitthreads,
- maximales Echtzeitbudget.

Ein Prozess darf durch eine Scheduleränderung keine Jobgrenze umgehen.

Beispiel:

```text
Job erlaubt:
    NORMAL
    BATCH
    BACKGROUND

Prozess fordert:
    REALTIME

Ergebnis:
    Anforderung wird abgelehnt
```

---

## 24. Echtzeitressourcen

Ein Job kann Echtzeitressourcen begrenzen.

Mögliche Grenzen:

```text
max_realtime_threads
max_realtime_priority
max_realtime_runtime
max_realtime_utilization
allowed_realtime_cpus
allowed_realtime_policies
```

Eine Capability allein reicht nicht aus, um Jobgrenzen zu überschreiten.

Admission Control und Jobprüfung werden gemeinsam ausgeführt.

---

## 25. Speicherlimit

Ein Job kann Speichergrenzen definieren für:

- privaten physischen Speicher,
- gemeinsam zugeordneten Speicher,
- virtuellen Adressraum,
- gesperrten Speicher,
- Kernelmetadaten,
- gemeinsam genutzte IPC-Puffer.

Mögliche Limits:

```text
max_committed_memory
max_resident_memory
max_locked_memory
max_virtual_memory
max_kernel_memory
```

Die Zählung gemeinsam genutzten Speichers muss pro Speicherart eindeutig definiert werden.

---

## 26. Speicherabrechnung

NovaOS unterscheidet mindestens:

- exklusiv dem Job zugeordneten Speicher,
- gemeinsam genutzten Speicher,
- hierarchisch zugerechneten Speicher,
- reservierten, aber noch nicht belegten Speicher.

Gemeinsam genutzter Speicher kann nach einer definierten Policy abgerechnet werden:

- vollständig jedem beteiligten Job,
- anteilig,
- nur dem Eigentümer,
- einem gemeinsamen Ressourcenobjekt.

Die verwendete Policy muss diagnostizierbar sein.

---

## 27. Speicherüberlastung

Wird ein Speicherlimit erreicht, kann NovaOS abhängig von der Jobpolicy:

- neue Allokationen ablehnen,
- ein Warnereignis erzeugen,
- Hintergrundthreads drosseln,
- Cachebereiche des Jobs freigeben,
- ausgewählte Prozesse beenden,
- den gesamten Job beenden.

Ein Job darf nicht automatisch Speicher anderer Sicherheitsdomänen freigeben.

Für Echtzeitjobs gelten gesonderte Reservierungsregeln.

---

## 28. I/O-Limits

Ein Job kann I/O-Ressourcen begrenzen.

Dazu gehören:

- Lese- und Schreibbandbreite,
- I/O-Operationen pro Sekunde,
- Anzahl ausstehender Anforderungen,
- Priorität von Blockgerätezugriffen,
- Netzwerkbandbreite,
- Anzahl offener Verbindungen.

I/O-Limits werden soweit möglich an die verursachenden Prozesse und Jobs weitergegeben.

Synchrone Kernelarbeit muss dem verursachenden Job zugerechnet werden können.

---

## 29. Handle- und Objektlimits

Ein Job kann die Anzahl verwendeter Kernelobjekte und Handles begrenzen.

Beispiele:

```text
max_handles
max_events
max_mutexes
max_ipc_channels
max_shared_memory_objects
max_timers
```

Dadurch wird verhindert, dass eine Anwendung durch massenhaft erzeugte Kernelobjekte globale Kernelressourcen erschöpft.

Objektlimits gelten zusätzlich zu globalen Systemgrenzen.

---

## 30. Lebenszykluszustände

Ein Job Object besitzt einen eigenen Lebenszyklus.

```text
CREATED
   ↓
ACTIVE
   ↕
PAUSED
   ↓
TERMINATING
   ↓
TERMINATED
   ↓
RELEASED
```

| Zustand | Bedeutung |
|---|---|
| `CREATED` | Job wurde erzeugt, enthält aber noch keine aktiven Prozesse |
| `ACTIVE` | Prozesse dürfen ausgeführt und erzeugt werden |
| `PAUSED` | enthaltene Threads werden nicht regulär ausgeführt |
| `TERMINATING` | Beendigung wurde eingeleitet |
| `TERMINATED` | keine aktiven Mitgliedsprozesse mehr vorhanden |
| `RELEASED` | Kernelobjekt wurde endgültig freigegeben |

Ein Job kann nach `TERMINATED` abhängig von seiner Policy erneut Prozesse aufnehmen oder endgültig geschlossen bleiben.

---

## 31. Pausieren eines Jobs

Beim Pausieren eines Jobs werden die enthaltenen Prozesse und Threads kontrolliert angehalten.

Neue ausführbare Threads werden nicht regulär eingeplant.

Ausnahmen können gelten für:

- Kernel-Recovery-Arbeit,
- definierte Supervisoren,
- sichere Beendigungsroutinen,
- nicht aufschiebbare Ressourcenfreigabe.

Das Pausieren erfolgt hierarchisch und betrifft standardmäßig auch alle Child Jobs.

Ein Job darf nicht als pausiert gemeldet werden, solange noch nicht ausgenommene Threads regulär laufen.

---

## 32. Fortsetzen eines Jobs

Beim Fortsetzen werden pausierte Threads wieder entsprechend ihrer vorherigen Zustände aktiviert.

Threads, die bereits vor dem Pausieren blockiert waren, bleiben blockiert.

```text
READY vor Pause
    → READY nach Fortsetzung

BLOCKED vor Pause
    → BLOCKED nach Fortsetzung
```

Vor dem Fortsetzen werden aktuelle Ressourcen-, Affinitäts- und Sicherheitsrichtlinien erneut geprüft.

---

## 33. Beenden eines Jobs

Das Beenden eines Jobs betrifft standardmäßig:

- alle direkten Mitgliedsprozesse,
- alle Child Jobs,
- alle indirekten Mitgliedsprozesse.

Der Ablauf lautet:

1. Job in `TERMINATING` versetzen,
2. neue Prozesse und Child Jobs verweigern,
3. Prozesse über bevorstehende Beendigung informieren,
4. optionale Grace Period gewähren,
5. verbleibende Prozesse kontrolliert beenden,
6. Ressourcen freigeben,
7. Abschlussereignis erzeugen,
8. Job in `TERMINATED` versetzen.

Die Beendigung muss auch dann vollständig fortgesetzt werden, wenn ein Mitgliedsprozess nicht kooperiert.

---

## 34. Kill-on-Close

Ein Job kann die Policy `KILL_ON_LAST_CONTROLLER_CLOSE` verwenden.

Wenn das letzte verwaltende Controller-Handle geschlossen wird, wird der Job beendet.

Diese Policy eignet sich für:

- kurzlebige Anwendungsgruppen,
- Build- und Testaufträge,
- Sandbox-Prozesse,
- überwachte Hilfsprozesse.

Nicht jedes Handle gilt als Controller-Handle.

Beobachtungs- und Statistikhandles lösen kein Kill-on-Close aus.

---

## 35. Leerer Job

Ein Job kann ein Ereignis erzeugen, sobald er keine aktiven Prozesse und Child Jobs mehr enthält.

Mögliche Ereignisse:

```text
JOB_EMPTY
JOB_ACTIVE
JOB_TERMINATED
```

Das Empty-Ereignis muss Race Conditions mit gleichzeitig gestarteten Prozessen berücksichtigen.

Ein Job gilt nur als leer, wenn:

- keine direkten Prozesse vorhanden sind,
- keine aktiven Child Jobs vorhanden sind,
- keine reservierten Prozessstarts ausstehen.

---

## 36. Beendigungsreihenfolge

NovaOS garantiert standardmäßig keine feste Reihenfolge, in der Mitgliedsprozesse beendet werden.

Ein Job kann optional eine kontrollierte Shutdown-Policy definieren:

```text
children_first
parents_first
dependency_order
parallel
```

Eine Abhängigkeitsreihenfolge benötigt explizite Prozess- oder Dienstabhängigkeiten.

Die normale Prozess-Elternbeziehung allein definiert keine sichere Shutdown-Reihenfolge.

---

## 37. Graceful Shutdown

Ein Job kann vor der erzwungenen Beendigung eine Grace Period erhalten.

Der Ablauf kann lauten:

```text
JOB_SHUTDOWN_REQUESTED
        ↓
Anwendungen beenden freiwillig
        ↓
Grace Period endet
        ↓
verbleibende Prozesse werden erzwungen beendet
```

Die Grace Period:

- verwendet monotone Zeit,
- besitzt eine maximale Systemgrenze,
- kann bei kritischen Fehlern übersprungen werden,
- darf keine Sicherheitsgrenzen aufheben.

---

## 38. Supervisoren

Ein Job kann einen Supervisor referenzieren.

Der Supervisor kann:

- Prozessstarts überwachen,
- Prozessbeendigungen empfangen,
- Abstürze erkennen,
- Prozesse neu starten,
- Ressourcenverletzungen behandeln,
- Fallback-Jobs aktivieren,
- Qualitätsstufen reduzieren.

Der Supervisor soll standardmäßig außerhalb des überwachten Jobs laufen, damit er bei einer Jobbeendigung weiter handlungsfähig bleibt.

Ein Supervisor benötigt eigene Ressourcen und Capabilities.

---

## 39. Restart-Policies

Ein Job kann definierte Restart-Policies besitzen.

Mögliche Policies:

| Policy | Bedeutung |
|---|---|
| `NEVER` | kein automatischer Neustart |
| `ON_FAILURE` | Neustart bei fehlerhafter Beendigung |
| `ALWAYS` | Neustart nach jeder Beendigung |
| `ON_ABNORMAL_EXIT` | Neustart bei Exception oder erzwungener Beendigung |
| `SUPERVISOR_DECIDES` | Supervisor entscheidet |

Restart-Policies müssen Wiederholungsschleifen begrenzen.

Mögliche Grenzen:

```text
max_restarts
restart_window
restart_backoff
maximum_backoff
```

---

## 40. Ereignismodell

Job Objects erzeugen strukturierte Ereignisse.

Mögliche Ereignisse sind:

```text
JOB_PROCESS_ADDED
JOB_PROCESS_EXITED
JOB_CHILD_ADDED
JOB_CHILD_REMOVED
JOB_LIMIT_WARNING
JOB_LIMIT_REACHED
JOB_PAUSED
JOB_RESUMED
JOB_SHUTDOWN_REQUESTED
JOB_TERMINATING
JOB_EMPTY
JOB_TERMINATED
JOB_POLICY_CHANGED
JOB_SUPERVISOR_FAILED
```

Ereignisse können über:

- Event Objects,
- IPC-Kanäle,
- Completion Queues,
- lokale Telemetrie

bereitgestellt werden.

---

## 41. Ereignisüberlastung

Ein Job mit vielen Prozessen kann sehr viele Ereignisse erzeugen.

Deshalb werden Ereignisse über begrenzte Warteschlangen bereitgestellt.

Bei Überlastung kann NovaOS:

- gleichartige Ereignisse zusammenfassen,
- Zähler statt Einzelereignisse verwenden,
- ein `EVENTS_DROPPED`-Ereignis erzeugen,
- kritische Ereignisse priorisieren.

Kritische Lebenszyklusereignisse dürfen nicht unbemerkt verloren gehen.

---

## 42. Statistiken

Ein Job stellt mindestens folgende Statistiken bereit:

- Anzahl direkter Prozesse,
- Anzahl aller Prozesse,
- Anzahl direkter Child Jobs,
- Anzahl aller Child Jobs,
- Anzahl aktiver Threads,
- kumulative CPU-Zeit,
- aktuelle CPU-Auslastung,
- Speichernutzung,
- gesperrter Speicher,
- I/O-Bytes und I/O-Operationen,
- Anzahl der Handles,
- Anzahl der Kernelobjekte,
- Anzahl der Prozessstarts,
- Anzahl der Prozessbeendigungen,
- Anzahl der Limitverletzungen,
- Anzahl der Neustarts.

Statistiken werden hierarchisch aggregiert.

---

## 43. Ressourcenreservierung

Ein Job kann Ressourcen nicht nur begrenzen, sondern auch reservieren.

Mögliche Reservierungen sind:

- CPU-Budget,
- Echtzeitkapazität,
- physischer Speicher,
- gesperrter Speicher,
- IPC-Puffer,
- Gerätewarteschlangen.

Eine Reservierung ist keine Garantie, solange das jeweilige Subsystem sie nicht ausdrücklich bestätigt hat.

Reservierungen werden beim Erzeugen oder Ändern eines Jobs validiert.

---

## 44. Änderungen von Jobrichtlinien

Eine Jobrichtlinie kann geändert werden, wenn:

- das aufrufende Subjekt die erforderliche Capability besitzt,
- Parent-Job-Grenzen eingehalten werden,
- bestehende Nutzung mit der neuen Grenze vereinbar ist oder eine definierte Überlastungsreaktion existiert,
- Echtzeit- und Sicherheitsgarantien nicht unkontrolliert verletzt werden.

Das Erhöhen eines Limits und das Absenken eines Limits können unterschiedliche Berechtigungen benötigen.

Änderungen werden atomar veröffentlicht.

---

## 45. Absenken von Limits unter aktuelle Nutzung

Wird ein Limit unter die aktuelle Nutzung abgesenkt, muss eine definierte Policy angewendet werden.

Mögliche Policies:

| Policy | Verhalten |
|---|---|
| `REJECT_CHANGE` | Änderung wird abgelehnt |
| `ALLOW_OVERCOMMIT_TEMPORARILY` | neue Nutzung wird blockiert, bestehende darf auslaufen |
| `THROTTLE` | betroffene Ressourcen werden gedrosselt |
| `RECLAIM` | kontrollierte Freigabe wird angefordert |
| `TERMINATE_MEMBERS` | Prozesse werden nach Policy beendet |

Die Standardpolicy lautet `REJECT_CHANGE`, sofern keine andere Reaktion ausdrücklich konfiguriert wurde.

---

## 46. Sicherheitsmodell

Job Objects sind in das Capability-System integriert.

Mögliche Capabilities sind:

```text
CAP_JOB_CREATE
CAP_JOB_ASSIGN_PROCESS
CAP_JOB_CREATE_CHILD
CAP_JOB_SET_LIMITS
CAP_JOB_RAISE_LIMITS
CAP_JOB_SET_SECURITY
CAP_JOB_PAUSE
CAP_JOB_RESUME
CAP_JOB_TERMINATE
CAP_JOB_QUERY
CAP_JOB_CONTROL_FOREIGN
CAP_JOB_ADMIN
```

Capabilities können eingeschränkt werden auf:

- einen bestimmten Job,
- einen Job und seine Unterhierarchie,
- bestimmte Richtlinientypen,
- bestimmte Grenzwerte,
- bestimmte Lebenszyklusoperationen.

Ein Jobhandle allein gewährt nur die Rechte, die im Handle und in den Capabilities festgelegt sind.

---

## 47. Delegation

Ein Parent Job kann begrenzte Verwaltungsrechte an einen Child Job oder Controller delegieren.

Delegierte Rechte dürfen die Rechte und Limits des Parents nicht überschreiten.

Beispiel:

```text
Parent erlaubt Child:
    Prozesse erzeugen
    CPU-Limit bis maximal 40 %
    Speicherlimit bis maximal 2 GiB

Child darf nicht:
    CPU-Limit auf 80 % erhöhen
    Sicherheitsgrenze entfernen
    Prozess außerhalb der Hierarchie verschieben
```

Delegation wird explizit und widerrufbar modelliert.

---

## 48. Handle-Vererbung

Jobhandles werden nicht automatisch an Kindprozesse vererbt.

Eine Vererbung muss ausdrücklich erlaubt werden.

Dabei wird geprüft:

- welche Rechte das neue Handle besitzt,
- ob Controller-Rechte weitergegeben werden dürfen,
- ob Kill-on-Close beeinflusst wird,
- ob Sicherheitsdomänen die Weitergabe erlauben.

Beobachtungsrechte und Verwaltungsrechte werden getrennt behandelt.

---

## 49. Namensgebung

Ein Job kann optional einen Namen und beschreibende Metadaten besitzen.

Beispiele:

```text
system.session.42
application.nova-ide
sandbox.browser.renderer
compute.model-inference
compat.windows.application
```

Namen sind nicht die primäre Identität.

Die Kernelidentität wird über eine unveränderliche Job-ID und Handles dargestellt.

Namen können innerhalb eines Namespace eindeutig sein.

---

## 50. API-Grundmodell

Die öffentliche Schnittstelle soll mindestens folgende Operationen unterstützen:

```c
nova_status_t nova_job_create(
    const nova_job_create_info_t* create_info,
    nova_handle_t* job
);

nova_status_t nova_job_create_child(
    nova_handle_t parent_job,
    const nova_job_create_info_t* create_info,
    nova_handle_t* child_job
);

nova_status_t nova_job_assign_process(
    nova_handle_t job,
    nova_handle_t process
);

nova_status_t nova_job_set_limits(
    nova_handle_t job,
    const nova_job_limits_t* limits
);

nova_status_t nova_job_query(
    nova_handle_t job,
    nova_job_info_t* info
);

nova_status_t nova_job_pause(
    nova_handle_t job
);

nova_status_t nova_job_resume(
    nova_handle_t job
);

nova_status_t nova_job_terminate(
    nova_handle_t job,
    nova_status_t exit_status
);

nova_status_t nova_job_wait(
    nova_handle_t job,
    uint64_t timeout_ns
);
```

Die endgültige ABI wird in einer separaten Spezifikation festgelegt.

---

## 51. Erzeugungsinformationen

Eine mögliche Erzeugungsstruktur lautet:

```c
typedef struct
{
    uint32_t structure_size;
    uint32_t structure_version;

    nova_handle_t parent_job;
    nova_string_view_t name;

    uint32_t flags;
    uint32_t event_queue_capacity;

    nova_job_limits_t initial_limits;
    nova_job_policy_t initial_policy;
} nova_job_create_info_t;
```

Mögliche Flags sind:

```c
typedef enum
{
    NOVA_JOB_FLAG_NONE                          = 0,
    NOVA_JOB_FLAG_KILL_ON_LAST_CONTROLLER_CLOSE = 1u << 0,
    NOVA_JOB_FLAG_NO_BREAKAWAY                  = 1u << 1,
    NOVA_JOB_FLAG_PERMANENT_LIMITS              = 1u << 2,
    NOVA_JOB_FLAG_TERMINATE_ON_LIMIT             = 1u << 3,
    NOVA_JOB_FLAG_ALLOW_CHILD_JOBS               = 1u << 4,
    NOVA_JOB_FLAG_REUSABLE                       = 1u << 5
} nova_job_flags_t;
```

---

## 52. Ressourcenlimits

Eine mögliche Limitstruktur lautet:

```c
typedef struct
{
    uint32_t structure_size;
    uint32_t structure_version;

    uint64_t max_direct_processes;
    uint64_t max_total_processes;
    uint64_t max_total_threads;

    uint64_t max_cpu_time_ns;
    uint64_t cpu_budget_ns;
    uint64_t cpu_period_ns;

    uint64_t max_committed_memory;
    uint64_t max_resident_memory;
    uint64_t max_locked_memory;
    uint64_t max_kernel_memory;

    uint64_t max_handles;
    uint64_t max_kernel_objects;

    uint64_t max_read_bytes_per_second;
    uint64_t max_write_bytes_per_second;

    nova_cpu_set_t allowed_cpus;

    uint8_t max_priority;
    uint8_t max_realtime_priority;
    uint16_t reserved;

    uint32_t allowed_scheduler_classes;
    uint32_t flags;
} nova_job_limits_t;
```

Nicht unterstützte Limits werden über Versions- und Featureabfragen behandelt.

---

## 53. Informationsstruktur

Eine mögliche Informationsstruktur lautet:

```c
typedef struct
{
    nova_job_id_t job_id;
    nova_job_id_t parent_job_id;

    nova_job_state_t state;
    uint32_t flags;

    uint64_t direct_process_count;
    uint64_t total_process_count;
    uint64_t direct_child_job_count;
    uint64_t total_child_job_count;
    uint64_t total_thread_count;

    uint64_t total_user_time_ns;
    uint64_t total_kernel_time_ns;

    uint64_t committed_memory;
    uint64_t resident_memory;
    uint64_t locked_memory;

    uint64_t read_bytes;
    uint64_t written_bytes;

    uint64_t limit_violation_count;
    uint64_t generation;
} nova_job_info_t;
```

---

## 54. Kernelinternes Datenmodell

Ein Jobobjekt enthält konzeptionell:

```c
typedef struct nova_job
{
    nova_kernel_object_t object;

    nova_job_id_t id;
    nova_job_state_t state;

    struct nova_job* parent;

    nova_list_t child_jobs;
    nova_list_t direct_processes;

    nova_job_limits_t configured_limits;
    nova_job_limits_t effective_limits;
    nova_job_policy_t policy;

    nova_job_accounting_t accounting;
    nova_job_event_queue_t events;

    nova_lock_t hierarchy_lock;
    nova_lock_t member_lock;

    uint64_t generation;
    uint32_t flags;
} nova_job_t;
```

Die konkrete Speicheranordnung bleibt der Implementierung überlassen.

---

## 55. Synchronisation

Die Jobhierarchie wird durch klar getrennte Sperren geschützt.

Zu vermeiden ist eine einzelne globale Jobsperre für sämtliche Operationen.

Mögliche Sperrbereiche:

- Hierarchiesperre,
- Mitgliedschaftssperre,
- Richtliniensperre,
- Statistik- oder Ereignissperre.

Bei Operationen über mehrere Jobs gilt eine globale Sperrreihenfolge, beispielsweise nach Hierarchietiefe und Job-ID.

Dadurch sollen Deadlocks verhindert werden.

---

## 56. Generationenmodell

Jede relevante Jobänderung erhöht eine Jobgeneration.

Dazu gehören:

- Richtlinienänderungen,
- Limitänderungen,
- Prozesszuordnungen,
- Child-Job-Änderungen,
- Zustandsänderungen,
- Supervisoränderungen.

Generationen ermöglichen:

- Erkennung veralteter Abfragen,
- atomare Richtlinienaktualisierung,
- sichere Scheduler- und Ressourcenentscheidungen,
- effiziente Cache-Aktualisierung in Subsystemen.

---

## 57. Telemetrie und Diagnose

Für Job Objects werden mindestens folgende Informationen bereitgestellt:

- Job-ID,
- Parent-Job-ID,
- Name,
- Zustand,
- direkte und indirekte Mitglieder,
- konfigurierte Limits,
- effektive Limits,
- aktuelle Ressourcennutzung,
- historische Spitzenwerte,
- Limitverletzungen,
- Lebenszyklusereignisse,
- letzte Richtlinienänderung,
- verantwortlicher Controller,
- Jobgeneration.

Die Diagnose muss zwischen direkter und hierarchisch aggregierter Nutzung unterscheiden.

---

## 58. Persistenz

Job Objects sind standardmäßig nicht persistent über einen Neustart hinweg.

Nach einem Neustart werden sie durch:

- Systemkonfiguration,
- Dienstmanager,
- Sessionmanager,
- Containerverwaltung,
- Anwendungsstarter

neu erzeugt.

Persistente Jobdefinitionen können in Userspace-Konfigurationen gespeichert werden.

Kernelhandles und laufende Mitgliedschaften werden nicht über einen vollständigen Systemneustart hinweg erhalten.

---

## 59. Kompatibilitätsumgebungen

Job Objects bilden eine Grundlage für Kompatibilitätsumgebungen.

Beispiele:

- Windows-Anwendungen,
- Android-Anwendungen,
- Linux-Anwendungen,
- ältere NovaOS-ABI-Versionen.

Eine Kompatibilitätsumgebung kann pro Anwendung oder Anwendungsgruppe einen Job verwenden.

Dadurch können:

- sämtliche Hilfsprozesse gemeinsam beendet,
- Ressourcen zusammengezählt,
- CPU- und Speicherlimits gesetzt,
- Schedulerklassen kontrolliert,
- Sandboxes durchgesetzt

werden.

Die sichtbare Semantik kann durch die jeweilige Kompatibilitätsschicht angepasst werden.

---

## 60. Fehlerbehandlung

Eine Joboperation wird abgelehnt, wenn:

- das Jobhandle ungültig ist,
- die erforderliche Capability fehlt,
- ein Hierarchiezyklus entstehen würde,
- die maximale Tiefe überschritten wird,
- ein Prozess bereits unzulässig gebunden ist,
- ein Ziel-Limit verletzt wird,
- Parent-Grenzen überschritten werden,
- der Job sich in `TERMINATING` oder `RELEASED` befindet,
- eine Echtzeitreservierung ungültig würde,
- eine CPU-Maske unzulässig ist,
- eine Sicherheitsrichtlinie widerspricht.

Mögliche Statuswerte sind:

```text
NOVA_STATUS_OK
NOVA_STATUS_INVALID_ARGUMENT
NOVA_STATUS_INVALID_HANDLE
NOVA_STATUS_PERMISSION_DENIED
NOVA_STATUS_JOB_TERMINATING
NOVA_STATUS_JOB_TERMINATED
NOVA_STATUS_JOB_LIMIT_REACHED
NOVA_STATUS_JOB_HIERARCHY_CYCLE
NOVA_STATUS_JOB_DEPTH_EXCEEDED
NOVA_STATUS_PROCESS_ALREADY_ASSIGNED
NOVA_STATUS_RESOURCE_CONFLICT
NOVA_STATUS_POLICY_CONFLICT
NOVA_STATUS_REALTIME_CONFLICT
NOVA_STATUS_NOT_SUPPORTED
```

Eine ungültige Userspace-Joboperation darf keinen Kernel-Panic auslösen.

---

## 61. Konsequenzen

### 61.1 Positive Konsequenzen

- sichere Verwaltung komplexer Mehrprozessanwendungen,
- hierarchische Ressourcenlimits,
- Schutz vor Limitumgehung durch Kindprozesse,
- atomare Gruppenoperationen,
- zuverlässiges gemeinsames Beenden,
- Grundlage für Sandboxes und Container,
- gute Unterstützung für Supervisoren,
- aggregierte Ressourcenabrechnung,
- Integration mit Scheduler, Speicher und I/O,
- bessere Kompatibilitätsumgebungen.

### 61.2 Negative Konsequenzen

- höhere Kernelkomplexität,
- umfangreiche Richtlinienvererbung,
- zusätzliche Sperr- und Hierarchielogik,
- gemeinsame Ressourcen müssen eindeutig abgerechnet werden,
- nachträgliche Prozessverschiebung wird kompliziert,
- viele Subsysteme müssen Jobgrenzen berücksichtigen.

### 61.3 Risiken

- Deadlocks in der Jobhierarchie,
- fehlerhafte Ressourcenaggregation,
- Prozesse außerhalb eines Jobs,
- unkontrollierte Limitänderungen,
- Supervisor-Neustartschleifen,
- Ereignisüberlastung,
- Race Conditions beim Leerlauf- oder Empty-Zustand,
- Sicherheitsflucht durch verschachtelte Jobs.

Diese Risiken werden begrenzt durch:

- atomare Zuordnung,
- unvermeidbare Jobmitgliedschaft,
- hierarchische Sperrordnung,
- Capabilities,
- Generationen,
- begrenzte Ereigniswarteschlangen,
- Restart-Limits,
- verbindliche Invarianten.

---

## 62. Auswirkungen auf andere Subsysteme

### 62.1 Prozessverwaltung

Jeder Prozess benötigt eine direkte Referenz auf ein Job Object.

### 62.2 Threadverwaltung

Threaderzeugung und -abrechnung müssen Joblimits berücksichtigen.

### 62.3 Scheduler

CPU-Budgets, Schedulerklassen, Prioritäten und Affinitäten können durch Jobs begrenzt werden.

### 62.4 Echtzeitscheduler

Echtzeitreservierungen werden zusätzlich gegen Jobgrenzen geprüft.

### 62.5 Speicherverwaltung

Speicherbelegung und -reservierungen müssen einem Job zugerechnet werden.

### 62.6 I/O-System

I/O-Anforderungen müssen den verursachenden Job für Abrechnung und Drosselung mitführen.

### 62.7 Capability-System

Jobverwaltung und Delegation werden durch feingranulare Capabilities geschützt.

### 62.8 IPC

IPC-Puffer und Kanäle können gegen Joblimits abgerechnet werden.

### 62.9 Objektverwaltung

Kernelobjekte und Handles müssen einem Prozess und mittelbar einem Job zurechenbar sein.

### 62.10 Dienstmanager

Systemdienste und Anwendungsgruppen können jeweils in eigenen Jobs gestartet und überwacht werden.

### 62.11 Kompatibilitätsschichten

Windows-, Android- und Linux-Kompatibilitätsumgebungen können Job Objects zur Prozessgruppenverwaltung verwenden.

---

## 63. Implementierungsrichtlinien

1. Jeder Userspace-Prozess gehört von Beginn an genau einem direkten Job an.
2. Prozesszuordnung erfolgt vor dem ersten ausführbaren Userspace-Thread.
3. Die erste Implementierung unterstützt Prozesslimit, Threadlimit, CPU-Zeit und Speicherabrechnung.
4. Jobhierarchien werden von Beginn an zyklusfrei modelliert.
5. Child Jobs dürfen Parent-Grenzen nur weiter einschränken.
6. Nachträgliche Prozessverschiebungen werden zunächst auf nicht gestartete oder pausierte Prozesse begrenzt.
7. Jobbeendigung wirkt standardmäßig rekursiv.
8. Ressourcenplätze werden vor der vollständigen Objekterzeugung reserviert.
9. Der Kernel verwendet keine einzelne globale Sperre für alle Joboperationen.
10. Ereigniswarteschlangen besitzen feste Grenzen.
11. Kill-on-Close unterscheidet Controller- von Beobachtungshandles.
12. Richtlinien- und Mitgliedschaftsänderungen erhöhen die Jobgeneration.
13. Gemeinsame Ressourcen erhalten ausdrücklich definierte Abrechnungsregeln.
14. Joblimits werden zusätzlich zu globalen Systemgrenzen angewendet.
15. Eine Capability hebt Joblimits nicht automatisch auf.

---

## 64. Testanforderungen

Mindestens folgende Szenarien müssen getestet werden:

- Erzeugung eines Root Jobs,
- Erzeugung eines Child Jobs,
- Prozesszuordnung vor Prozessstart,
- Vererbung bei Kindprozessen,
- mehrere Prozesse in einem Job,
- verschachtelte Jobs,
- Erkennung von Hierarchiezyklen,
- maximale Hierarchietiefe,
- Prozesslimit,
- Threadlimit,
- CPU-Zeitlimit,
- CPU-Auslastungslimit,
- Speicherlimit,
- Handlelimit,
- I/O-Limit,
- CPU-Affinitätsgrenze,
- Scheduler-Klassenbegrenzung,
- Echtzeitlimit,
- Absenken eines Limits,
- parallele Prozessstarts,
- atomare Limitreservierung,
- Pausieren eines Jobs,
- Fortsetzen eines Jobs,
- rekursive Jobbeendigung,
- Graceful Shutdown,
- erzwungene Beendigung,
- Kill-on-Close,
- Job-Empty-Ereignis,
- Ereignisüberlastung,
- Supervisor-Neustart,
- Restart-Backoff,
- Prozessabsturz,
- Verschieben eines pausierten Prozesses,
- verweigerte Flucht aus dem Job,
- Capability-Delegation,
- Handle-Vererbung,
- Generationenkonflikte,
- Abrechnung gemeinsam genutzten Speichers,
- CPU-Hotplug bei Jobaffinität,
- Prozessbeendigung unter hoher Last,
- Freigabe des letzten Jobhandles.

---

## 65. Nicht Bestandteil dieser Entscheidung

Diese ADR legt nicht abschließend fest:

- vollständige Container-Namespaces,
- Dateisystemisolation,
- Netzwerkisolation,
- konkrete GPU-Abrechnung,
- die vollständige I/O-Scheduler-Implementierung,
- persistente Jobs über Neustarts,
- verteilte Jobs über mehrere Rechner,
- genaue Restart-Strategien einzelner Systemdienste,
- die endgültige öffentliche ABI,
- vollständige Windows-Job-Object-Kompatibilität.

Diese Details werden in nachfolgenden ADRs und NPSPEC-Dokumenten definiert.

---

## 66. Verbindliche Invarianten

1. Jeder Userspace-Prozess gehört jederzeit genau einem direkten Job Object an.
2. Ein Prozess wird erst nach erfolgreicher Jobzuordnung ausführbar.
3. Jobhierarchien dürfen keine Zyklen enthalten.
4. Ein Job besitzt höchstens einen Parent Job.
5. Child Jobs dürfen harte Parent-Grenzen nicht lockern.
6. Die gesamte Unterhierarchie zählt gegen aggregierte Parent-Limits.
7. Ein Prozess darf seine Jobmitgliedschaft nicht selbst entfernen.
8. Kindprozesse dürfen Jobgrenzen nicht umgehen.
9. Ressourcenplätze werden vor der endgültigen Prozess- oder Threaderzeugung reserviert.
10. Jobbeendigung verhindert neue Mitgliedschaften und Prozessstarts.
11. Rekursive Beendigung erfasst standardmäßig alle Child Jobs und Mitgliedsprozesse.
12. Eine Capability ersetzt keine Ressourcen- oder Admission-Control-Prüfung.
13. Controller- und Beobachtungsrechte werden getrennt behandelt.
14. Kill-on-Close wird nur durch das letzte Controller-Handle ausgelöst.
15. Jobrichtlinienänderungen werden atomar veröffentlicht.
16. Jede relevante Änderung erhöht die Jobgeneration.
17. CPU-, Speicher- und I/O-Nutzung muss dem verursachenden Job zurechenbar sein.
18. Ein Supervisor soll nicht ausschließlich vom überwachten Job abhängen.
19. Ein Job Object ersetzt nicht automatisch Sicherheitsdomäne, Namespace oder Container.
20. KI- oder lernbasierte Komponenten dürfen Job-, Capability-, Ressourcen- oder Sicherheitsgrenzen nicht umgehen.

---

## 67. Referenzen

- ADR-PROC-0001 – Process Lifecycle
- ADR-PROC-0002 – Thread Lifecycle
- ADR-PROC-0003 – Scheduler Classes
- ADR-PROC-0004 – Prioritäten
- ADR-PROC-0005 – Echtzeitprozesse
- ADR-PROC-0006 – Idle Thread
- ADR-PROC-0007 – CPU Affinity
- ADR-PROC-0008 – Context Switching
- ADR-KERNEL-0100 – Kernel Object Graph statt isolierter Subsysteme
- ADR-KERNEL-0101 – Ereignisgesteuerter Kernel-Core
- ADR-KERNEL-0102 – Einheitliches Objektmodell für Kernel und Userspace
- ADR-KERNEL-0103 – Native Capability-basierte Ressourcenverwaltung
- ADR-KERNEL-0104 – Integrierte Telemetrie- und Diagnoseschnittstellen
- ADR-KERNEL-0105 – ABI-Stabilität über versionierte Kernel-Services
- ADR-MM-0012 – NUMA Memory Policy
- NPSPEC-JOB-0001 – Job Object Specification
- NPSPEC-SCHEDULER-0001 – Scheduler Specification
- NPSPEC-SCHEDULER-AFFINITY-0001 – CPU Affinity Specification
- NPSPEC-REALTIME-0001 – Realtime Process and Thread Specification
- NPSPEC-PROCESS-0001 – Process Specification
- NPSPEC-THREAD-0001 – Thread Specification

---

## 68. Zusammenfassung der Entscheidung

NovaOS führt hierarchische Job Objects als native Kernelobjekte ein.

Jeder Userspace-Prozess gehört jederzeit genau einem direkten Job an. Über untergeordnete Jobs kann er zusätzlich indirektes Mitglied mehrerer übergeordneter Jobs sein.

Job Objects verwalten gemeinsam:

- Prozesse und Child Jobs,
- Ressourcenlimits,
- CPU- und Speicherbudgets,
- Scheduler- und Affinitätsgrenzen,
- Lebenszyklusoperationen,
- Ereignisse,
- Statistiken,
- Supervisor- und Restart-Policies.

Child Jobs dürfen die harten Grenzen ihrer Parent Jobs nur weiter einschränken. Prozesse können Limits nicht durch das Erzeugen von Kindprozessen oder weiteren Jobs umgehen.

Prozesszuordnung erfolgt atomar vor dem ersten ausführbaren Userspace-Thread. Jobs können hierarchisch pausiert, fortgesetzt und beendet werden. Kill-on-Close, Graceful Shutdown, Supervisoren und begrenzte Restart-Policies ermöglichen eine zuverlässige Verwaltung komplexer Mehrprozessanwendungen.

Job Objects bilden damit die Grundlage für Anwendungsgruppen, Systemdienste, Sandboxes, Container, Compute-Aufträge und Windows-, Android- oder Linux-Kompatibilitätsumgebungen.