# ADR-PROC-0007 – CPU Affinity

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-PROC-0007 |
| Titel | CPU Affinity |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-30 |
| Bereich | Kernel / Prozesse / Threads / Scheduling / CPU-Topologie |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-PROC-0001, ADR-PROC-0002, ADR-PROC-0003, ADR-PROC-0004, ADR-PROC-0005, ADR-PROC-0006 |
| Nachfolgende Spezifikation | NPSPEC-SCHEDULER-AFFINITY-0001 |

---

## 1. Kontext

NovaOS muss Threads auf Einzelkern-, Mehrkern-, NUMA- und heterogenen Prozessorsystemen effizient ausführen.

Der Scheduler kann einen lauffähigen Thread grundsätzlich auf unterschiedlichen logischen CPUs einplanen. Eine freie Migration zwischen CPUs ist jedoch nicht immer optimal oder zulässig.

Gründe für eine gezielte CPU-Zuordnung sind:

- Erhaltung warmer CPU-Caches,
- Nutzung lokaler NUMA-Speicherbereiche,
- Einhaltung von Echtzeitgarantien,
- Trennung kritischer und normaler Arbeitslasten,
- Nutzung bestimmter CPU-Leistungsklassen,
- Energieoptimierung,
- Lizenz- oder Kompatibilitätsanforderungen,
- Interrupt- und Geräteaffinität,
- CPU-Isolation,
- Debugging und Leistungsmessung,
- virtuelle Maschinen,
- sicherheitsrelevante Ausführungsdomänen.

Gleichzeitig darf CPU Affinity nicht dazu führen, dass Threads unnötig auf einer überlasteten CPU warten, obwohl andere geeignete CPUs verfügbar sind.

NovaOS benötigt daher ein Affinitätsmodell, das verbindliche Einschränkungen und unverbindliche Platzierungspräferenzen voneinander trennt.

---

## 2. Problemstellung

Eine einzelne CPU-ID pro Thread ist für moderne Systeme nicht ausreichend.

Ein Thread kann:

- auf mehreren CPUs ausgeführt werden dürfen,
- eine bestimmte CPU bevorzugen,
- nur CPUs eines NUMA-Knotens verwenden,
- nur Performance- oder Effizienzkerne verwenden,
- eine reservierte Echtzeit-CPU benötigen,
- während CPU-Hotplug seine zulässige CPU verlieren,
- durch Prozess-, Benutzer- oder Sicherheitsrichtlinien weiter eingeschränkt werden.

Mehrere Affinitätsquellen können gleichzeitig gelten:

```text
Hardwareverfügbarkeit
        ∩
Systemrichtlinie
        ∩
Sicherheitsdomäne
        ∩
Prozessaffinität
        ∩
Threadaffinität
        ∩
Echtzeitreservierung
```

Werden diese Einschränkungen nicht eindeutig kombiniert, können folgende Probleme entstehen:

- Ausführung auf einer unzulässigen CPU,
- nicht mehr lauffähige Threads,
- unnötige CPU-Migrationen,
- verletzte Echtzeitgarantien,
- schlechte NUMA-Lokalität,
- thermische Überlastung,
- Instabilität bei CPU-Hotplug,
- widersprüchliche Benutzer- und Systemrichtlinien.

---

## 3. Entscheidungsziele

Das Affinitätsmodell soll:

1. mehrere zulässige CPUs pro Thread unterstützen,
2. harte Einschränkungen und weiche Präferenzen trennen,
3. Prozess- und Threadaffinität kombinieren,
4. CPU-Hotplug sicher behandeln,
5. Mehrkern-, SMT-, NUMA- und heterogene Systeme unterstützen,
6. Echtzeit- und Sicherheitsgrenzen durchsetzen,
7. Cache- und Speicherlokalität berücksichtigen,
8. unnötige Migrationen vermeiden,
9. kontrollierte automatische Platzierung ermöglichen,
10. Affinitätsentscheidungen vollständig diagnostizierbar machen,
11. eine skalierbare CPU-Maskendarstellung verwenden,
12. stabile APIs trotz wachsender CPU-Anzahl ermöglichen.

---

## 4. Begriffsdefinitionen

### 4.1 CPU Affinity

CPU Affinity beschreibt, auf welchen logischen CPUs ein Thread ausgeführt werden darf oder bevorzugt ausgeführt werden soll.

---

### 4.2 Harte Affinität

Eine harte Affinität ist eine verbindliche CPU-Einschränkung.

Der Scheduler darf einen Thread niemals außerhalb dieser CPU-Menge ausführen.

---

### 4.3 Weiche Affinität

Eine weiche Affinität beschreibt bevorzugte CPUs.

Der Scheduler soll diese CPUs verwenden, darf aber unter definierten Bedingungen auf andere zulässige CPUs ausweichen.

---

### 4.4 CPU-Maske

Eine CPU-Maske ist eine Bitmenge, in der jedes Bit eine logische CPU repräsentiert.

Beispiel:

```text
CPU 0 = Bit 0
CPU 1 = Bit 1
CPU 2 = Bit 2
CPU 3 = Bit 3

Maske 0b0101 = CPU 0 und CPU 2
```

---

### 4.5 Zulässige CPU-Menge

Die zulässige CPU-Menge enthält alle CPUs, auf denen ein Thread nach Anwendung sämtlicher harter Einschränkungen ausgeführt werden darf.

---

### 4.6 Effektive CPU-Menge

Die effektive CPU-Menge enthält alle momentan online geschalteten und tatsächlich verwendbaren CPUs innerhalb der zulässigen CPU-Menge.

---

### 4.7 Bevorzugte CPU-Menge

Die bevorzugte CPU-Menge enthält CPUs, die der Scheduler vorrangig verwenden soll.

Sie ist nicht zwingend.

---

### 4.8 Home CPU

Die Home CPU ist die CPU, auf der ein Thread bevorzugt verbleiben soll, solange keine stärkeren Gründe für eine Migration bestehen.

---

## 5. Betrachtete Alternativen

### 5.1 Keine konfigurierbare Affinität

Der Scheduler entscheidet vollständig selbst über die CPU-Platzierung.

#### Vorteile

- einfache Userspace-Schnittstelle,
- vollständige Freiheit für den Scheduler,
- geringe Verwaltungskosten.

#### Nachteile

- keine CPU-Isolation,
- keine gezielte Echtzeitplatzierung,
- keine kontrollierte NUMA-Bindung,
- ungeeignet für spezielle Hardware- und Testanforderungen,
- erschwerte Performanceanalyse.

Diese Alternative wird verworfen.

---

### 5.2 Bindung an genau eine CPU

Jeder Thread kann optional an eine einzelne CPU gebunden werden.

#### Vorteile

- einfache Implementierung,
- eindeutige Platzierung,
- leicht verständliches Modell.

#### Nachteile

- geringe Ausfallsicherheit,
- schlechte Lastverteilung,
- problematisch bei CPU-Hotplug,
- keine CPU-Gruppen,
- ungeeignet für große Systeme.

Diese Alternative wird verworfen.

---

### 5.3 Ausschließlich harte CPU-Masken

Jeder Thread besitzt nur eine verbindliche CPU-Maske.

#### Vorteile

- klare Regeln,
- einfache Validierung,
- vorhersehbare Platzierung.

#### Nachteile

- keine ausdrücklichen Präferenzen,
- Anwendungen müssen unnötig starre Einschränkungen setzen,
- schlechtere automatische Lastverteilung,
- erhöhte Gefahr leerer effektiver CPU-Mengen.

Diese Alternative wird verworfen.

---

### 5.4 Hierarchisches Affinitätsmodell

NovaOS kombiniert harte CPU-Mengen, weiche Präferenzen, Prozessrichtlinien, Threadrichtlinien und automatische Schedulerentscheidungen.

#### Vorteile

- flexible und kontrollierte Platzierung,
- klare Sicherheitsgrenzen,
- gute Lastverteilung,
- Unterstützung von NUMA und heterogenen CPUs,
- robuste CPU-Hotplug-Behandlung,
- gute Echtzeitintegration.

#### Nachteile

- komplexere Berechnung der effektiven CPU-Menge,
- mehrere Affinitätsfelder pro Thread,
- mehr Diagnose- und Testaufwand.

Diese Alternative wird gewählt.

---

## 6. Entscheidung

NovaOS verwendet ein hierarchisches CPU-Affinitätsmodell.

Jeder Thread besitzt mindestens:

```text
requested_cpu_mask
allowed_cpu_mask
effective_cpu_mask
preferred_cpu_mask
current_cpu
home_cpu
last_cpu
affinity_mode
affinity_generation
```

Die tatsächlich zulässige CPU-Menge wird durch die Schnittmenge aller verbindlichen Einschränkungen gebildet.

```text
effective_cpu_mask =
    online_cpu_mask
    ∩ system_allowed_mask
    ∩ security_domain_mask
    ∩ process_allowed_mask
    ∩ thread_allowed_mask
    ∩ reservation_allowed_mask
```

Die bevorzugte CPU-Menge beeinflusst nur die Auswahl innerhalb der effektiven CPU-Menge.

```text
effective_preferred_mask =
    preferred_cpu_mask
    ∩ effective_cpu_mask
```

Der Scheduler darf einen Thread niemals außerhalb seiner effektiven CPU-Menge ausführen.

---

## 7. Affinitätsmodi

NovaOS definiert folgende Affinitätsmodi:

| Modus | Bedeutung |
|---|---|
| `NOVA_AFFINITY_AUTO` | Scheduler entscheidet vollständig innerhalb der erlaubten Systemgrenzen |
| `NOVA_AFFINITY_PREFERRED` | angegebene CPUs werden bevorzugt |
| `NOVA_AFFINITY_RESTRICTED` | Ausführung nur innerhalb der angegebenen CPU-Menge |
| `NOVA_AFFINITY_PINNED` | Bindung an genau eine logische CPU |
| `NOVA_AFFINITY_ISOLATED` | Ausführung innerhalb eines reservierten oder isolierten CPU-Pools |

---

### 7.1 Auto

Im Modus `NOVA_AFFINITY_AUTO` besitzt der Thread keine selbst gesetzte CPU-Einschränkung.

System-, Prozess-, Sicherheits- und Reservierungsgrenzen gelten weiterhin.

---

### 7.2 Preferred

Im Modus `NOVA_AFFINITY_PREFERRED` wird eine CPU-Menge bevorzugt.

Ist dort keine sinnvolle Ausführung möglich, darf der Scheduler eine andere CPU aus der effektiven CPU-Menge auswählen.

---

### 7.3 Restricted

Im Modus `NOVA_AFFINITY_RESTRICTED` darf der Thread ausschließlich auf den angegebenen CPUs ausgeführt werden.

Diese Einschränkung ist verbindlich.

---

### 7.4 Pinned

Im Modus `NOVA_AFFINITY_PINNED` darf der Thread ausschließlich auf einer bestimmten CPU ausgeführt werden.

CPU-Pinning ist ein Sonderfall harter Affinität.

---

### 7.5 Isolated

Im Modus `NOVA_AFFINITY_ISOLATED` darf der Thread ausschließlich innerhalb eines kontrollierten CPU-Pools ausgeführt werden.

Dieser Modus ist insbesondere vorgesehen für:

- Echtzeitaufgaben,
- virtuelle Maschinen,
- sicherheitskritische Dienste,
- reproduzierbare Benchmarks,
- störungsempfindliche Compute-Aufgaben.

---

## 8. Prozessaffinität

Jeder Prozess besitzt eine erlaubte CPU-Menge.

Diese begrenzt alle Threads des Prozesses.

Ein Thread kann seine eigene Affinität weiter einschränken, aber ohne entsprechende Capability nicht über die Prozessgrenze hinaus erweitern.

Beispiel:

```text
Prozessmaske:  CPU 0, 1, 2, 3
Threadmaske:   CPU 1, 2

Ergebnis:      CPU 1, 2
```

Ein Prozess darf eine Standardpräferenz definieren, die neue Threads erben.

Eine Änderung der Prozessaffinität muss alle betroffenen Threads neu bewerten.

---

## 9. Threadaffinität

Die Threadaffinität ist die feinste reguläre Platzierungsregel.

Sie gilt ausschließlich für einen bestimmten Thread.

Ein Thread darf:

- die Prozessmaske übernehmen,
- seine zulässige CPU-Menge weiter einschränken,
- eine bevorzugte CPU-Menge angeben,
- mit entsprechender Capability seine Einschränkung innerhalb der übergeordneten Systemgrenzen erweitern.

Die Threadaffinität verändert nicht automatisch die Prozessaffinität anderer Threads.

---

## 10. Vererbung

Ein neuer Prozess erbt standardmäßig die erlaubte CPU-Menge seines Elternprozesses.

Ein neuer Thread erbt:

- die Prozessaffinität,
- die Standardpräferenz des Prozesses,
- gegebenenfalls die Affinität des erzeugenden Threads,
- die zugehörigen Sicherheits- und Ressourcenlimits.

Die konkrete Vererbungsregel wird durch Erzeugungsflags gesteuert.

Mögliche Flags sind:

```text
NOVA_THREAD_INHERIT_PROCESS_AFFINITY
NOVA_THREAD_INHERIT_CALLER_AFFINITY
NOVA_THREAD_USE_AUTOMATIC_AFFINITY
NOVA_THREAD_USE_EXPLICIT_AFFINITY
```

Eine Vererbung darf keine übergeordneten CPU-Grenzen umgehen.

---

## 11. Harte und weiche Affinität

Harte und weiche Affinität werden getrennt gespeichert.

```text
Harte Affinität:
    Wo darf der Thread laufen?

Weiche Affinität:
    Wo soll der Thread bevorzugt laufen?
```

Die harte Maske ist Teil der Scheduler- und Sicherheitsinvarianten.

Die weiche Maske ist ein Optimierungshinweis.

Bei Konflikten gilt:

```text
Sicherheit und Gültigkeit
        vor
Echtzeitgarantie
        vor
harter Affinität
        vor
weicher Affinität
        vor
allgemeiner Lastoptimierung
```

Die harte Affinität darf allerdings nicht stillschweigend verletzt werden. Ist sie nicht erfüllbar, muss der Thread warten oder eine definierte Fehler- beziehungsweise Recovery-Policy ausführen.

---

## 12. Effektive CPU-Menge

Die effektive CPU-Menge wird bei jeder relevanten Änderung neu berechnet.

Auslöser sind:

- Änderung der Systemmaske,
- Änderung der Sicherheitsdomäne,
- Änderung der Prozessaffinität,
- Änderung der Threadaffinität,
- Aktivierung oder Änderung einer Echtzeitreservierung,
- CPU-Hotplug,
- CPU-Ausfall,
- Änderung eines CPU-Pools.

Konzeptionell:

```c
effective =
    online
    & system_allowed
    & security_allowed
    & process_allowed
    & thread_allowed
    & reservation_allowed;
```

Die Berechnung muss atomar gegenüber Schedulerentscheidungen sichtbar werden.

---

## 13. Leere effektive CPU-Menge

Eine leere effektive CPU-Menge bedeutet, dass ein Thread momentan auf keiner CPU ausgeführt werden darf.

Dieser Zustand darf nicht ignoriert werden.

Mögliche Reaktionen sind:

| Situation | Reaktion |
|---|---|
| temporäre CPU-Unerreichbarkeit | Thread wartet im Zustand `AFFINITY_BLOCKED` |
| ungültige Userspace-Anforderung | Änderung wird abgelehnt |
| CPU-Hotplug | Hotplug abbrechen oder Recovery-Policy ausführen |
| CPU-Ausfall | Thread blockieren, migrieren oder Prozess benachrichtigen |
| Echtzeitreservierung | Garantie widerrufen oder Fallback aktivieren |
| kritischer Kernelthread | CPU-Offlining ablehnen oder sichere Ersatz-CPU bestimmen |

Ein Thread darf niemals außerhalb seiner harten Affinität ausgeführt werden, nur um eine leere effektive CPU-Menge zu umgehen.

---

## 14. Affinity-Blocked-Zustand

NovaOS führt den Threadzustand `AFFINITY_BLOCKED` ein.

Ein Thread erreicht diesen Zustand, wenn:

- seine harte Affinität grundsätzlich gültig ist,
- momentan jedoch keine entsprechende CPU online oder verfügbar ist.

Der Thread bleibt im Kernel registriert, wird aber in keine reguläre Run Queue eingereiht.

Sobald wieder eine zulässige CPU verfügbar ist, kann der Thread in den Zustand `READY` wechseln.

Die Wartezeit wird diagnostisch erfasst.

---

## 15. CPU-Auswahl

Bei mehreren zulässigen CPUs bewertet der Scheduler mindestens:

- bevorzugte CPU-Menge,
- aktuelle CPU,
- Home CPU,
- letzte CPU,
- Run-Queue-Auslastung,
- Scheduler-Klasse,
- Priorität,
- Cache-Lokalität,
- NUMA-Lokalität,
- CPU-Leistungsklasse,
- Energiezustand,
- thermische Grenzen,
- Echtzeitreservierungen,
- Migrationskosten.

Eine mögliche Bewertungsfunktion lautet konzeptionell:

```text
cpu_score =
    affinity_preference
    + cache_locality
    + numa_locality
    + performance_fit
    + energy_fit
    - runqueue_load
    - migration_cost
    - thermal_penalty
```

Die genaue Bewertungsfunktion wird in der Scheduler-Spezifikation definiert.

---

## 16. Home CPU

Der Scheduler kann jedem Thread eine Home CPU zuordnen.

Die Home CPU dient der Stabilisierung der Platzierung.

Solange:

- die CPU zulässig ist,
- keine erhebliche Überlastung besteht,
- keine Echtzeitregel widerspricht,
- keine Energie- oder Temperaturgrenze verletzt wird,

soll der Thread bevorzugt dort ausgeführt werden.

Die Home CPU ist keine harte Bindung.

Sie kann bei Bedarf geändert werden.

---

## 17. Cache-Affinität

Nach der Ausführung auf einer CPU befinden sich Teile des Arbeitszustands eines Threads wahrscheinlich in deren Caches.

Der Scheduler soll deshalb unnötige Migrationen vermeiden.

Berücksichtigt werden können:

- L1- und L2-Cache-Lokalität,
- gemeinsamer Last-Level-Cache,
- SMT-Geschwister,
- zuletzt ausgeführte CPU,
- Zeit seit der letzten Ausführung,
- Größe des vermuteten Working Sets.

Cache-Affinität ist eine weiche Präferenz und darf harte System- oder Sicherheitsgrenzen nicht überschreiben.

---

## 18. NUMA-Affinität

CPU-Affinität und Speicherpolitik müssen auf NUMA-Systemen zusammenarbeiten.

Ein Thread soll bevorzugt auf CPUs ausgeführt werden, die seinem aktiven Speicher nahe sind.

Mögliche Konstellationen:

```text
CPU folgt Speicher
Speicher folgt CPU
CPU und Speicher bleiben fest gebunden
interleavter Speicher über mehrere NUMA-Knoten
```

Eine Änderung der CPU-Affinität kann eine Neubewertung der NUMA-Memory-Policy auslösen.

Eine Speichermigration erfolgt nicht automatisch bei jeder CPU-Migration. Sie benötigt eine Kosten-Nutzen-Prüfung.

---

## 19. Heterogene Prozessoren

NovaOS unterstützt Prozessoren mit unterschiedlichen CPU-Leistungsklassen.

Beispiele:

- Performance-Kerne,
- Effizienzkerne,
- stromsparende Kerne,
- Kerne mit besonderen Befehlssatzerweiterungen.

Threads können Präferenzen angeben wie:

```text
NOVA_CPU_CLASS_ANY
NOVA_CPU_CLASS_PERFORMANCE
NOVA_CPU_CLASS_EFFICIENCY
NOVA_CPU_CLASS_LOW_LATENCY
NOVA_CPU_CLASS_HIGH_THROUGHPUT
```

Eine CPU-Klassenpräferenz ist standardmäßig weich.

Eine harte Anforderung muss ausdrücklich angegeben und vom Kernel validiert werden.

---

## 20. CPU-Fähigkeiten

Ein Thread oder Prozess kann bestimmte CPU-Fähigkeiten benötigen.

Beispiele:

- Vektorbefehle,
- Kryptografieerweiterungen,
- Virtualisierungserweiterungen,
- spezielle Matrixbeschleunigung,
- deterministische Timerfunktionen.

Die effektive CPU-Menge wird in diesem Fall zusätzlich eingeschränkt:

```text
effective_cpu_mask &=
    cpus_supporting(required_features)
```

Ein Prozess darf sich nicht auf eine CPU-Fähigkeit verlassen, die außerhalb seiner effektiven CPU-Menge nicht überall verfügbar ist, sofern keine kontrollierte dynamische Dispatch-Lösung verwendet wird.

---

## 21. SMT und Thread-Geschwister

Mehrere logische CPUs können sich denselben physischen Kern teilen.

Der Scheduler muss unterscheiden zwischen:

- logischer CPU,
- physischem Kern,
- SMT-Geschwistern,
- CPU-Cluster,
- NUMA-Knoten.

Ein Thread kann Präferenzen oder Einschränkungen definieren wie:

- SMT-Geschwister vermeiden,
- SMT-Geschwister gemeinsam nutzen,
- exklusiven physischen Kern anfordern,
- Threads desselben Prozesses auf gemeinsame Caches legen,
- sicherheitskritische Threads auf getrennte physische Kerne verteilen.

Exklusive Kernnutzung ist eine privilegierte Ressourcenreservierung und keine einfache Affinitätseinstellung.

---

## 22. CPU-Pools

NovaOS unterstützt verwaltete CPU-Pools.

Ein CPU-Pool ist eine benannte Menge logischer CPUs mit definierten Nutzungsregeln.

Beispiele:

```text
system
interactive
realtime
background
virtualization
isolated-compute
```

Ein Prozess oder Thread kann einem CPU-Pool zugeordnet werden.

Die Poolmaske wird als zusätzliche harte Einschränkung verwendet:

```text
effective_cpu_mask &= cpu_pool_mask
```

CPU-Pools werden durch privilegierte Systemdienste verwaltet.

---

## 23. CPU-Isolation

Isolierte CPUs werden nicht für allgemeine Schedulerlast verwendet.

Sie können reserviert werden für:

- harte oder feste Echtzeitaufgaben,
- virtuelle Maschinen,
- reproduzierbare Benchmarks,
- sicherheitskritische Dienste,
- störungsempfindliche Datenverarbeitung.

Isolation kann umfassen:

- Ausschluss normaler Threads,
- kontrollierte Interrupt-Affinität,
- reduzierte Kernel-Housekeeping-Arbeit,
- feste Timerzuordnung,
- begrenzte Load-Balancer-Aktivität.

Eine isolierte CPU bleibt dennoch unter Kontrolle des Kernels und benötigt einen eigenen Idle Thread.

---

## 24. Echtzeitprozesse

Echtzeitreservierungen können eine eigene zulässige CPU-Menge festlegen.

Dabei gelten folgende Regeln:

1. Die CPU-Menge wird durch die Admission Control geprüft.
2. Reservierte CPU-Kapazität muss auf allen zulässigen Ziel-CPUs berücksichtigt werden.
3. Unkontrollierte Migrationen sind nicht zulässig.
4. Idle-State-Latenzen der Ziel-CPUs müssen zur Garantie passen.
5. CPU-Hotplug erfordert eine erneute Admission Control.
6. Eine Migration darf die Deadline nicht unkontrolliert gefährden.

Für harte Echtzeit kann CPU-Pinning oder ein isolierter CPU-Pool erforderlich sein.

---

## 25. Idle Thread

Der CPU-lokale Idle Thread gemäß ADR-PROC-0006 besitzt eine unveränderliche Affinität zu genau einer CPU.

Für ihn gilt:

```text
allowed_cpu_mask   = genau eine CPU
effective_cpu_mask = genau diese CPU, solange sie online ist
preferred_cpu_mask = genau diese CPU
current_cpu        = genau diese CPU
```

Der Idle Thread darf niemals migriert oder einer anderen CPU zugeordnet werden.

---

## 26. Kernelthreads

Kernelthreads besitzen ebenfalls CPU-Affinitäten.

Mögliche Kategorien sind:

- CPU-lokale Kernelthreads,
- gerätelokale Worker,
- NUMA-lokale Worker,
- global migrierbare Kernelthreads,
- Echtzeit-Kernelworker,
- Housekeeping-Threads.

CPU-lokale Kernelthreads dürfen ihre zugehörige CPU nicht verlassen.

Migrierbare Kernelthreads sollen möglichst breite Affinitätsmengen verwenden, damit das System Last ausgleichen kann.

---

## 27. Interrupt-Affinität

Interrupt-Affinität ist technisch von Threadaffinität getrennt, muss aber mit ihr koordiniert werden.

Für zeitkritische Geräte soll gelten:

```text
Geräteinterrupt
      ↓
Interrupt-Worker
      ↓
verarbeitender Thread
```

Diese Komponenten sollen nach Möglichkeit:

- auf derselben CPU,
- im selben CPU-Cluster oder
- auf demselben NUMA-Knoten

platziert werden.

Eine Threadaffinität verändert nicht automatisch die Interrupt-Affinität eines Geräts.

Eine koordinierte Änderung erfolgt über eine separate privilegierte Schnittstelle.

---

## 28. Affinitätsänderung

Eine Affinitätsänderung durchläuft folgende Schritte:

1. Eingabeparameter validieren,
2. Zugriffsrechte prüfen,
3. übergeordnete CPU-Grenzen anwenden,
4. neue effektive CPU-Menge berechnen,
5. Echtzeit- und Ressourcenregeln prüfen,
6. Änderung atomar veröffentlichen,
7. Run-Queue-Zustand aktualisieren,
8. gegebenenfalls Migration anfordern,
9. Telemetrieereignis erzeugen.

Eine teilweise Änderung ist nicht zulässig.

Entweder wird die gesamte neue Konfiguration übernommen oder die bisherige bleibt aktiv.

---

## 29. Affinitätsänderung eines laufenden Threads

Wird die aktuelle CPU aus der erlaubten CPU-Menge eines laufenden Threads entfernt, muss der Thread migriert werden.

Der Ablauf lautet:

```text
neue Maske veröffentlichen
        ↓
Thread als Migration erforderlich markieren
        ↓
sicheren Präemptionspunkt erreichen
        ↓
Thread von aktueller Run Queue entfernen
        ↓
Ziel-CPU auswählen
        ↓
Thread dort einreihen
```

Ein Userspace-Thread darf nicht dauerhaft auf einer inzwischen unzulässigen CPU weiterlaufen.

Kurze Kernelabschnitte dürfen die Migration bis zu einem sicheren Präemptionspunkt verzögern.

---

## 30. Affinitätsänderung eines blockierten Threads

Bei einem blockierten Thread wird die neue Affinität gespeichert.

Beim späteren Aufwecken wählt der Scheduler ausschließlich eine CPU aus der dann gültigen effektiven CPU-Menge.

Ist diese Menge leer, wechselt der Thread in `AFFINITY_BLOCKED`.

---

## 31. Migration

Eine Migration kann ausgelöst werden durch:

- Affinitätsänderung,
- Load Balancing,
- CPU-Hotplug,
- CPU-Ausfall,
- thermische Überlastung,
- Energieoptimierung,
- NUMA-Neuausrichtung,
- Echtzeitreservierung,
- CPU-Pool-Änderung.

Jede Migration muss:

- den Threadzustand erhalten,
- Run-Queue-Invarianten einhalten,
- CPU-lokale Schedulerdaten aktualisieren,
- Timer und Abrechnungsdaten berücksichtigen,
- die Affinitätsgeneration prüfen,
- diagnostizierbar sein.

---

## 32. Migrationsbegrenzung

Zu häufige Migrationen können die Leistung verschlechtern.

NovaOS verwendet deshalb eine Migrationshysterese.

Eine Migration erfolgt nur, wenn mindestens einer der folgenden Gründe ausreichend stark ist:

- aktuelle CPU ist nicht mehr zulässig,
- erhebliche Lastdifferenz,
- Echtzeitgarantie erfordert Migration,
- thermische oder energetische Grenze,
- CPU wird offline geschaltet,
- erwarteter NUMA- oder Cachevorteil übersteigt die Migrationskosten.

Kosmetische Lastunterschiede allein sollen keine ständigen Migrationen verursachen.

---

## 33. CPU-Hotplug

Beim geplanten Offlining einer CPU muss der Kernel alle betroffenen Affinitäten prüfen.

Für jeden betroffenen Thread gilt:

- Ist eine andere effektive CPU verfügbar, wird der Thread migriert.
- Ist keine andere CPU verfügbar, wird die Hotplug-Operation abgelehnt oder eine definierte Recovery-Policy ausgeführt.
- Echtzeitreservierungen werden erneut geprüft.
- CPU-lokale Kernelthreads werden kontrolliert beendet oder in einen Offline-Zustand versetzt.
- Der Idle Thread schließt das CPU-Offlining ab.

Eine CPU darf nicht offline geschaltet werden, wenn dadurch ein unverzichtbarer Thread ohne zulässige CPU verbleibt.

---

## 34. CPU-Ausfall

Bei einem ungeplanten CPU-Ausfall wird die effektive CPU-Menge aller betroffenen Threads neu berechnet.

Mögliche Reaktionen:

- automatische Migration,
- Wechsel in `AFFINITY_BLOCKED`,
- Widerruf einer Echtzeitgarantie,
- Aktivierung einer Reserve-CPU,
- Start eines Fallback-Prozesses,
- Übergang in einen sicheren Zustand.

Die harte Affinität wird bei einem CPU-Ausfall nicht stillschweigend erweitert.

Eine automatische Erweiterung ist nur zulässig, wenn vorher eine entsprechende Fallback-Maske konfiguriert wurde.

---

## 35. Fallback-Affinität

Ein Thread kann optional eine Fallback-CPU-Menge definieren.

Diese wird nur aktiviert, wenn die primäre CPU-Menge nicht mehr verfügbar ist.

```text
primary_cpu_mask
        ↓ nicht verfügbar
fallback_cpu_mask
        ↓ validieren
effective_cpu_mask
```

Die Fallback-Maske:

- muss vorab autorisiert sein,
- unterliegt denselben Sicherheitsgrenzen,
- darf Echtzeitgarantien nicht ungeprüft übernehmen,
- wird als Affinitätsereignis protokolliert.

---

## 36. Sicherheit und Isolation

CPU-Affinität ist Teil des Ressourcen- und Sicherheitsmodells.

Sie kann verwendet werden, um:

- sicherheitskritische Dienste zu isolieren,
- nicht vertrauenswürdige Arbeitslasten zu begrenzen,
- Cache-Seitenkanäle zu reduzieren,
- SMT-Nachbarschaften zu kontrollieren,
- bestimmte Hardwarefunktionen exklusiv zuzuweisen.

Affinity allein stellt jedoch keine vollständige Sicherheitsisolation dar.

Zusätzlich können erforderlich sein:

- getrennte physische Kerne,
- Cache-Partitionierung,
- Speicherisolation,
- Interruptisolierung,
- Gerätezuordnung,
- zeitliche Isolation.

---

## 37. Capability-Modell

Privilegierte Affinitätsoperationen werden über Capabilities geschützt.

Mögliche Capabilities sind:

```text
CAP_SCHED_SET_AFFINITY
CAP_SCHED_EXPAND_AFFINITY
CAP_SCHED_PIN_CPU
CAP_SCHED_CONTROL_FOREIGN
CAP_CPU_POOL_USE
CAP_CPU_POOL_ADMIN
CAP_CPU_ISOLATE
CAP_CPU_EXCLUSIVE
CAP_SCHED_ADMIN
```

Ein Thread darf seine eigene CPU-Menge grundsätzlich weiter einschränken.

Eine Erweiterung über die aktuelle Prozess- oder Sicherheitsgrenze hinaus benötigt eine passende Capability.

Das Ändern der Affinität eines fremden Threads benötigt:

- Zugriff auf das Threadobjekt,
- eine passende Capability,
- eine gültige Zielmaske,
- Einhaltung aller übergeordneten CPU-Grenzen.

---

## 38. API-Grundmodell

Die öffentliche Schnittstelle soll mindestens folgende Operationen unterstützen:

```c
nova_status_t nova_thread_set_affinity(
    nova_handle_t thread,
    const nova_affinity_config_t* config
);

nova_status_t nova_thread_get_affinity(
    nova_handle_t thread,
    nova_affinity_info_t* info
);

nova_status_t nova_process_set_affinity(
    nova_handle_t process,
    const nova_affinity_config_t* config
);

nova_status_t nova_process_get_affinity(
    nova_handle_t process,
    nova_affinity_info_t* info
);
```

Mögliche Affinitätsmodi:

```c
typedef enum
{
    NOVA_AFFINITY_AUTO = 0,
    NOVA_AFFINITY_PREFERRED,
    NOVA_AFFINITY_RESTRICTED,
    NOVA_AFFINITY_PINNED,
    NOVA_AFFINITY_ISOLATED
} nova_affinity_mode_t;
```

Eine mögliche Konfiguration lautet:

```c
typedef struct
{
    uint32_t structure_size;
    uint32_t structure_version;

    nova_affinity_mode_t mode;
    uint32_t flags;

    nova_cpu_set_t allowed_cpus;
    nova_cpu_set_t preferred_cpus;
    nova_cpu_set_t fallback_cpus;

    nova_cpu_id_t home_cpu;
    nova_cpu_class_t preferred_cpu_class;
} nova_affinity_config_t;
```

Eine mögliche Informationsstruktur lautet:

```c
typedef struct
{
    nova_affinity_mode_t mode;

    nova_cpu_set_t requested_cpus;
    nova_cpu_set_t allowed_cpus;
    nova_cpu_set_t effective_cpus;
    nova_cpu_set_t preferred_cpus;
    nova_cpu_set_t fallback_cpus;

    nova_cpu_id_t current_cpu;
    nova_cpu_id_t home_cpu;
    nova_cpu_id_t last_cpu;

    uint64_t generation;
    uint32_t flags;
} nova_affinity_info_t;
```

Die endgültige ABI wird in einer separaten Spezifikation festgelegt.

---

## 39. Skalierbare CPU-Mengen

NovaOS darf CPU-Mengen nicht auf einen festen 32- oder 64-Bit-Wert beschränken.

`nova_cpu_set_t` muss Systeme mit einer wachsenden Anzahl logischer CPUs unterstützen.

Mögliche Implementierungen sind:

- versionierte variable Bitmaps,
- dynamische CPU-Mengen,
- eingebettete kleine Masken mit Erweiterungsbereich,
- Kernelobjekte für große CPU-Sets.

Öffentliche APIs müssen die Größe einer übergebenen CPU-Maske explizit angeben.

Unbekannte zusätzliche Bits werden entsprechend der ABI-Version validiert oder ignoriert.

---

## 40. CPU-IDs

CPU-IDs sind stabile logische Kennungen innerhalb einer laufenden NovaOS-Instanz.

Eine CPU-ID ist nicht automatisch identisch mit:

- APIC-ID,
- Hardware-Thread-ID,
- Firmware-Prozessor-ID,
- NUMA-Knoten-ID,
- physischer Kernnummer.

Die Topologieverwaltung stellt die Zuordnung bereit.

CPU-IDs dürfen während einer laufenden Systeminstanz nicht stillschweigend für eine andere CPU wiederverwendet werden, solange noch Referenzen auf die alte CPU existieren.

---

## 41. Affinitätsgeneration

Jede relevante Affinitätsänderung erhöht eine Generation.

Die Generation wird verwendet für:

- Erkennung veralteter Schedulerentscheidungen,
- sichere Migrationen,
- Run-Queue-Aktualisierungen,
- CPU-Hotplug,
- parallele Affinitätsänderungen.

Ein Scheduler darf eine Migration nicht mit einer veralteten Affinitätsgeneration abschließen.

Vor dem Einreihen auf der Ziel-CPU muss die aktuelle Generation erneut geprüft werden.

---

## 42. Telemetrie und Diagnose

Für jeden Thread werden mindestens folgende Affinitätsinformationen bereitgestellt:

- angeforderte CPU-Menge,
- erlaubte CPU-Menge,
- effektive CPU-Menge,
- bevorzugte CPU-Menge,
- Fallback-CPU-Menge,
- aktuelle CPU,
- Home CPU,
- letzte CPU,
- Affinitätsmodus,
- Affinitätsgeneration,
- Anzahl der Migrationen,
- Gründe der letzten Migration,
- Zeit im Zustand `AFFINITY_BLOCKED`,
- Anzahl abgelehnter Affinitätsänderungen,
- Anzahl verletzter Präferenzen.

Mögliche Migrationsgründe sind:

```text
THREAD_WAKEUP
LOAD_BALANCE
AFFINITY_CHANGE
CPU_HOTPLUG
CPU_FAILURE
REALTIME_RESERVATION
NUMA_REBALANCE
THERMAL_PRESSURE
ENERGY_POLICY
CPU_POOL_CHANGE
FALLBACK_ACTIVATION
```

---

## 43. Fehlerbehandlung

Eine Affinitätsänderung wird abgelehnt, wenn:

- die CPU-Maske syntaktisch ungültig ist,
- unbekannte CPU-IDs unzulässig verwendet werden,
- die verlangte harte Maske leer ist,
- die erforderliche Capability fehlt,
- übergeordnete CPU-Grenzen verletzt werden,
- eine Echtzeitreservierung ungültig würde,
- eine exklusive CPU nicht verfügbar ist,
- die Ziel-CPU offline ist und keine entsprechende Policy dies erlaubt,
- die Strukturversion nicht unterstützt wird.

Mögliche Statuswerte sind:

```text
NOVA_STATUS_OK
NOVA_STATUS_INVALID_ARGUMENT
NOVA_STATUS_INVALID_CPU_SET
NOVA_STATUS_CPU_OFFLINE
NOVA_STATUS_EMPTY_AFFINITY
NOVA_STATUS_PERMISSION_DENIED
NOVA_STATUS_RESOURCE_CONFLICT
NOVA_STATUS_REALTIME_CONFLICT
NOVA_STATUS_CPU_POOL_DENIED
NOVA_STATUS_AFFINITY_BLOCKED
NOVA_STATUS_NOT_SUPPORTED
```

Eine ungültige Userspace-Anforderung darf keinen Kernel-Panic auslösen.

---

## 44. Konsequenzen

### 44.1 Positive Konsequenzen

- flexible CPU-Zuordnung,
- klare Trennung harter und weicher Affinität,
- bessere Cache- und NUMA-Lokalität,
- kontrollierte Echtzeitplatzierung,
- Unterstützung heterogener Prozessoren,
- sichere CPU-Hotplug-Behandlung,
- CPU-Pools und Isolation werden möglich,
- bessere Diagnose von Migrationen und Platzierungsproblemen,
- skalierbare Unterstützung großer Mehrprozessorsysteme.

### 44.2 Negative Konsequenzen

- komplexere Schedulerentscheidungen,
- mehrere CPU-Mengen pro Thread,
- höherer Aufwand bei CPU-Hotplug,
- Migrationen müssen generationengesichert sein,
- Affinitäts- und NUMA-Policies können sich widersprechen,
- starre Benutzereinstellungen können die Leistung verschlechtern.

### 44.3 Risiken

- leere effektive CPU-Mengen,
- zu häufige Migrationen,
- unnötig starres CPU-Pinning,
- schlechte Lastverteilung,
- veraltete Affinitätsinformationen,
- verletzte Echtzeitgarantien bei CPU-Ausfall,
- Sicherheitsprobleme durch unberechtigte Erweiterung,
- fehlerhafte Topologieannahmen.

Diese Risiken werden begrenzt durch:

- hierarchische Maskenberechnung,
- Affinitätsgenerationen,
- Capability-Prüfungen,
- Fallback-Masken,
- Migrationshysterese,
- erneute Admission Control,
- vollständige Telemetrie.

---

## 45. Auswirkungen auf andere Subsysteme

### 45.1 Scheduler

Der Scheduler darf einen Thread ausschließlich auf CPUs seiner effektiven CPU-Menge ausführen.

### 45.2 Prozessverwaltung

Prozesse benötigen erlaubte CPU-Mengen und vererbbare Standardpräferenzen.

### 45.3 Threadverwaltung

Der Thread Control Block muss Affinitätsmasken, CPU-IDs und eine Generation speichern.

### 45.4 Load Balancer

Der Load Balancer muss harte Affinitäten, Präferenzen und Migrationskosten berücksichtigen.

### 45.5 Echtzeitscheduler

Echtzeitreservierungen müssen an zulässige CPUs und deren Kapazitäten gebunden werden.

### 45.6 Speicherverwaltung

Die NUMA Memory Policy muss mit CPU-Platzierung und Migration koordiniert werden.

### 45.7 Energieverwaltung

CPU-Leistungsklassen, Energiezustände und thermische Grenzen beeinflussen die weiche Platzierung.

### 45.8 CPU-Hotplug

Vor dem Offlining einer CPU müssen sämtliche betroffenen Affinitäten geprüft werden.

### 45.9 Interruptverwaltung

Interrupt- und Threadaffinitäten sollen für zeitkritische Geräte gemeinsam optimiert werden.

### 45.10 Capability-System

Privilegierte Erweiterungen, CPU-Pinning und Isolation werden durch Capabilities geschützt.

---

## 46. Implementierungsrichtlinien

1. CPU-Mengen werden als skalierbare Bitmengen implementiert.
2. Harte und weiche Affinitäten werden getrennt gespeichert.
3. Die effektive CPU-Menge wird aus allen harten Einschränkungen berechnet.
4. Der Scheduler prüft die aktuelle Affinitätsgeneration vor jeder Migration.
5. Ein Thread darf niemals außerhalb seiner effektiven CPU-Menge laufen.
6. Die erste Implementierung darf mit `AUTO`, `RESTRICTED` und `PINNED` beginnen.
7. `PREFERRED`, CPU-Pools und heterogene CPU-Klassen können anschließend ergänzt werden.
8. CPU-Hotplug muss von Beginn an leere effektive Masken erkennen.
9. Affinitätsänderungen werden atomar übernommen oder vollständig abgelehnt.
10. Der Scheduler vermeidet unnötige Migrationen durch eine Hysterese.
11. Echtzeitmigrationen benötigen eine erneute Kapazitätsprüfung.
12. Der Idle Thread erhält eine unveränderliche Einzel-CPU-Maske.
13. Öffentliche CPU-Masken enthalten Größen- und Versionsinformationen.
14. CPU-Topologie und logische CPU-IDs werden getrennt verwaltet.

---

## 47. Testanforderungen

Mindestens folgende Szenarien müssen getestet werden:

- automatische Affinität,
- bevorzugte Affinität,
- harte Affinität,
- CPU-Pinning,
- Prozessaffinität,
- Threadaffinität,
- Vererbung bei Prozesserzeugung,
- Vererbung bei Threaderzeugung,
- Einschränkung der eigenen CPU-Menge,
- verweigerte unberechtigte Erweiterung,
- autorisierte Erweiterung,
- Änderung eines laufenden Threads,
- Änderung eines blockierten Threads,
- leere effektive CPU-Menge,
- Übergang in `AFFINITY_BLOCKED`,
- Wiederaktivierung nach CPU-Online,
- Migration zwischen CPUs,
- parallele Affinitätsänderungen,
- Affinitätsgenerationskonflikte,
- Load Balancing,
- Migrationshysterese,
- CPU-Hotplug,
- CPU-Ausfall,
- Fallback-Affinität,
- Echtzeitreservierungen,
- NUMA-Platzierung,
- heterogene CPU-Kerne,
- CPU-Fähigkeitsmasken,
- SMT-Geschwister,
- CPU-Pools,
- CPU-Isolation,
- Kernelthreads,
- unveränderliche Idle-Thread-Affinität,
- Interrupt- und Threadaffinität,
- Systeme mit mehr als 64 CPUs,
- ungültige CPU-Masken,
- Telemetrie bei häufigen Migrationen.

---

## 48. Nicht Bestandteil dieser Entscheidung

Diese ADR legt nicht abschließend fest:

- die konkrete Bitmap-Implementierung,
- die vollständige CPU-Topologiedarstellung,
- exakte Migrationsschwellen,
- die vollständige CPU-Bewertungsfunktion,
- konkrete NUMA-Migrationsalgorithmen,
- Cache-Partitionierungsmechanismen,
- plattformspezifische CPU-Klassen,
- die Interrupt-Affinitäts-API,
- die endgültige öffentliche ABI.

Diese Details werden in nachfolgenden ADRs und NPSPEC-Dokumenten definiert.

---

## 49. Verbindliche Invarianten

1. CPU Affinity gilt für direkt planbare Threads.
2. Jeder Thread besitzt eine erlaubte und eine effektive CPU-Menge.
3. Der Scheduler darf einen Thread niemals außerhalb seiner effektiven CPU-Menge ausführen.
4. Harte Einschränkungen besitzen Vorrang vor weichen Präferenzen.
5. Die effektive CPU-Menge ist die Schnittmenge aller gültigen harten Einschränkungen.
6. Eine weiche Präferenz darf keine Sicherheits- oder Echtzeitgrenze überschreiben.
7. Ein Thread darf seine CPU-Menge ohne Capability nur weiter einschränken.
8. Eine Erweiterung über Prozess- oder Sicherheitsgrenzen benötigt eine passende Capability.
9. Affinitätsänderungen werden atomar übernommen oder vollständig abgelehnt.
10. Eine leere effektive CPU-Menge darf nicht ignoriert werden.
11. Threads ohne verfügbare zulässige CPU wechseln in einen definierten Wartezustand.
12. CPU-Hotplug darf keinen unverzichtbaren Thread ohne zulässige CPU zurücklassen.
13. Echtzeitmigrationen benötigen eine erneute Garantieprüfung.
14. Der CPU-lokale Idle Thread ist unveränderlich an genau eine CPU gebunden.
15. CPU-Mengen müssen mehr als 64 CPUs unterstützen können.
16. Jede relevante Affinitätsänderung erhöht die Affinitätsgeneration.
17. Eine Migration darf nicht mit einer veralteten Affinitätsgeneration abgeschlossen werden.
18. CPU-Pinning bedeutet keine automatische exklusive Nutzung des physischen Kerns.
19. Affinität allein stellt keine vollständige Sicherheitsisolation dar.
20. KI- oder lernbasierte Platzierungsmechanismen dürfen harte Affinitäts-, Capability- oder Echtzeitgrenzen nicht umgehen.

---

## 50. Referenzen

- ADR-PROC-0001 – Process Lifecycle
- ADR-PROC-0002 – Thread Lifecycle
- ADR-PROC-0003 – Scheduler Classes
- ADR-PROC-0004 – Prioritäten
- ADR-PROC-0005 – Echtzeitprozesse
- ADR-PROC-0006 – Idle Thread
- ADR-KERNEL-0100 – Kernel Object Graph statt isolierter Subsysteme
- ADR-KERNEL-0101 – Ereignisgesteuerter Kernel-Core
- ADR-KERNEL-0102 – Einheitliches Objektmodell für Kernel und Userspace
- ADR-KERNEL-0103 – Native Capability-basierte Ressourcenverwaltung
- ADR-KERNEL-0104 – Integrierte Telemetrie- und Diagnoseschnittstellen
- ADR-KERNEL-0105 – ABI-Stabilität über versionierte Kernel-Services
- ADR-MM-0011 – Memory Hotplug
- ADR-MM-0012 – NUMA Memory Policy
- ADR-MM-0014 – DMA Memory
- NPSPEC-SCHEDULER-0001 – Scheduler Specification
- NPSPEC-SCHEDULER-AFFINITY-0001 – CPU Affinity Specification
- NPSPEC-REALTIME-0001 – Realtime Process and Thread Specification
- NPSPEC-PROCESS-0001 – Process Specification
- NPSPEC-THREAD-0001 – Thread Specification

---

## 51. Zusammenfassung der Entscheidung

NovaOS verwendet ein hierarchisches CPU-Affinitätsmodell für die Platzierung von Threads auf logischen CPUs.

Harte Affinitäten definieren, auf welchen CPUs ein Thread ausgeführt werden darf. Weiche Affinitäten definieren bevorzugte CPUs, von denen der Scheduler bei Bedarf abweichen kann.

Die effektive CPU-Menge entsteht aus der Schnittmenge von Hardwareverfügbarkeit, Systemrichtlinien, Sicherheitsdomäne, Prozessaffinität, Threadaffinität, CPU-Pool und möglichen Echtzeitreservierungen.

Ein Thread darf niemals außerhalb seiner effektiven CPU-Menge ausgeführt werden. Ist keine zulässige CPU verfügbar, wechselt er in den Zustand `AFFINITY_BLOCKED` oder löst eine definierte Recovery-Policy aus.

Das Modell unterstützt CPU-Pinning, CPU-Pools, Isolation, NUMA-Systeme, heterogene Prozessoren, Echtzeitreservierungen, CPU-Hotplug und Systeme mit mehr als 64 logischen CPUs. Affinitätsgenerationen schützen Migrationen vor veralteten Entscheidungen.