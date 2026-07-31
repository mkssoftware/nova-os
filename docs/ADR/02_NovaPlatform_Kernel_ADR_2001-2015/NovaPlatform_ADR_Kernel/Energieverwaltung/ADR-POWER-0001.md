# ADR-POWER-0001 – CPU Idle

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-POWER-0001 |
| Titel | CPU Idle |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Energieverwaltung, Scheduler und CPU-Architektur |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-PROC-0003, ADR-PROC-0006, ADR-PROC-0007, ADR-DEV-0005, ADR-KERNEL-0101 |
| Nachfolgende Spezifikation | NPSPEC-POWER-CPU-IDLE |

---

## 1. Kontext

Wenn auf einer CPU keine ausführbaren Threads vorhanden sind, soll NovaOS die CPU nicht in einer aktiven Warteschleife weiterlaufen lassen.

Moderne Prozessoren stellen unterschiedliche Idle-Zustände bereit, die:

- Stromverbrauch reduzieren,
- Wärmeentwicklung senken,
- Akkulaufzeit verlängern,
- Lüfteraktivität verringern,
- gemeinsam genutzte Prozessorkomponenten abschalten können.

Tiefere Idle-Zustände benötigen jedoch mehr Zeit zum Eintritt und Aufwachen.

---

## 2. Problemstellung

NovaOS muss bei jedem Idle-Übergang zwischen Energieeinsparung und Reaktionszeit abwägen.

Dabei müssen berücksichtigt werden:

- erwartete Idle-Dauer,
- nächster Timertermin,
- Echtzeit- und Latenzanforderungen,
- Interruptlast,
- CPU-Topologie,
- gemeinsame Package-Zustände,
- CPU-Hotplug,
- fehlerhafte Firmwareinformationen,
- ältere Hardware ohne moderne Idle-Schnittstellen.

Ein falscher Zustand kann zu hoher Latenz, verpassten Fristen oder instabilem Systemverhalten führen.

---

## 3. Entscheidung

NovaOS implementiert ein zentrales CPU-Idle-Framework als Teil des Power Managers.

Der Scheduler meldet, wenn eine logische CPU keine ausführbare Arbeit besitzt. Das CPU-Idle-Framework wählt anschließend einen geeigneten architekturspezifischen Idle-Zustand.

Die Auswahl erfolgt dynamisch anhand von:

- prognostizierter Idle-Dauer,
- maximal erlaubter Exit-Latenz,
- Energieprofil,
- Zustand anderer CPUs,
- verfügbaren Wake-up-Quellen,
- historischen Messwerten.

Es existiert immer ein sicherer, flacher Fallback-Zustand.

---

## 4. Abgrenzung zum Idle Thread

Der Idle Thread aus ADR-PROC-0006 ist das Schedulerobjekt, das ausgeführt wird, wenn keine andere Arbeit vorhanden ist.

Der Idle Thread:

1. verarbeitet notwendige Scheduler- und CPU-Wartungsaufgaben,
2. berechnet den nächsten Wake-up-Zeitpunkt,
3. ruft das CPU-Idle-Framework auf,
4. kehrt nach einem Interrupt zurück,
5. prüft erneut auf ausführbare Threads.

Der Idle Thread selbst entscheidet nicht über hardwarespezifische C-States.

---

## 5. Architektur

```text
Scheduler
   |
   | CPU ohne Arbeit
   v
Idle Thread
   |
   v
CPU-Idle-Governor
   |
   v
Architektur-Backend
   |
   v
Hardware-Idle-Zustand
```

Der Governor entscheidet über die Tiefe. Das Architektur-Backend führt den konkreten CPU-Befehl oder Firmwareübergang aus.

---

## 6. Idle-Zustände

NovaOS verwendet plattformunabhängige Idle-Deskriptoren:

```c
typedef struct cpu_idle_state {
    uint32_t state_id;
    uint32_t flags;
    uint64_t exit_latency_ns;
    uint64_t target_residency_ns;
    uint64_t estimated_power_cost;
} cpu_idle_state_t;
```

Jeder Zustand beschreibt mindestens:

- eindeutige lokale ID,
- Exit-Latenz,
- minimale sinnvolle Verweildauer,
- Energieeinsparung,
- notwendige Wake-up-Quellen,
- Auswirkungen auf Timer und CPU-Kontext,
- Gültigkeit für Core oder Package.

---

## 7. Fallback-Zustand

Jede unterstützte CPU besitzt einen flachen Fallback-Zustand.

Auf x86 basiert dieser typischerweise auf:

```text
HLT bis zum nächsten nicht maskierten Interrupt
```

Wenn selbst der Hardware-Idle-Zustand als unsicher erkannt wird, kann NovaOS vorübergehend kontrolliertes Polling verwenden.

Dauerhaftes Busy Waiting ist jedoch kein normaler Idle-Modus.

Der Fallback darf keine Firmwareabhängigkeit besitzen.

---

## 8. Erkennung der Idle-Zustände

Mögliche Informationsquellen sind:

- CPU-Architekturfunktionen,
- ACPI,
- UEFI- oder Plattformtabellen,
- Device Tree,
- Hypervisor-Schnittstellen,
- NovaOS-Plattformtreiber.

Alle gemeldeten Zustände werden validiert.

Unplausible, unbekannte oder bekanntermaßen fehlerhafte Zustände werden deaktiviert.

NovaOS darf Firmwareangaben nicht ungeprüft übernehmen.

---

## 9. Idle-Governor

Der Idle-Governor wählt den Zustand anhand von:

```text
erwartete Idle-Dauer
größer als Zielverweildauer
und
Exit-Latenz innerhalb des erlaubten Limits
```

Zusätzlich berücksichtigt er:

- Zeit bis zum nächsten Timer,
- durchschnittliche tatsächliche Verweildauer,
- frühe Wake-ups,
- Energieprofil,
- Echtzeitanforderungen,
- QoS-Latenzgrenzen,
- Package-Abhängigkeiten.

Der Governor muss auch dann korrekt arbeiten, wenn seine Prognose falsch ist.

---

## 10. Idle-Prognose

Die erwartete Idle-Dauer wird geschätzt aus:

- nächstem Scheduler-Timer,
- Geräte- und Netzwerktimern,
- periodischer Kernelarbeit,
- bisherigen Idle-Intervallen,
- bekannten Interruptquellen,
- aktuellen Prozesslastmustern.

Externe Interrupts bleiben grundsätzlich nicht exakt vorhersehbar.

Die Prognose ist daher eine Optimierung und keine Garantie.

Fehleinschätzungen fließen in spätere Entscheidungen ein.

---

## 11. Latenzanforderungen

Subsysteme können zeitlich begrenzte CPU-Latenzanforderungen registrieren.

```c
status_t power_cpu_latency_request(
    uint64_t maximum_exit_latency_ns,
    cap_handle_t *request_handle
);
```

Solange die Capability existiert, wählt NovaOS nur Zustände mit einer zulässigen Exit-Latenz.

Mögliche Nutzer sind:

- Echtzeitprozesse,
- Audiowiedergabe,
- zeitkritische Treiber,
- Netzwerkverarbeitung,
- Mess- und Steueranwendungen.

Unprivilegierte Prozesse dürfen nicht unbegrenzt systemweite Tieflastzustände blockieren.

---

## 12. Timer und Tickless Idle

NovaOS verwendet nach Möglichkeit Tickless Idle.

Wenn eine CPU inaktiv ist:

- wird der periodische Scheduler-Tick ausgesetzt,
- wird nur der nächste erforderliche Termin programmiert,
- werden unnötige Aufweckvorgänge vermieden,
- werden abgelaufene Timer nach dem Wake-up verarbeitet.

Kann der lokale CPU-Timer einen tiefen Idle-Zustand nicht überstehen, verwendet NovaOS eine geeignete Broadcast- oder Plattformtimerquelle.

---

## 13. Wake-up-Quellen

Ein Idle-Zustand darf nur verwendet werden, wenn mindestens eine zuverlässige Wake-up-Quelle aktiv ist.

Mögliche Quellen:

- lokaler CPU-Timer,
- Inter-Processor Interrupt,
- Geräteinterrupt,
- Plattformtimer,
- Power-Management-Ereignis,
- Hypervisor-Ereignis.

Vor Eintritt in einen tiefen Zustand wird geprüft, ob der nächste notwendige Termin das System sicher aufwecken kann.

Ein Zustand mit unzuverlässiger Wake-up-Quelle wird deaktiviert.

---

## 14. Race-freier Idle-Eintritt

Zwischen der Prüfung auf Arbeit und dem Hardware-Idle-Befehl darf kein Wake-up verloren gehen.

Der Ablauf muss atomar oder architektursicher sein:

1. Interruptzustand kontrollieren,
2. Scheduler-Queue prüfen,
3. ausstehende Reschedule- und Wake-up-Flags prüfen,
4. Idle-Zustand vorbereiten,
5. Interruptzustellung ermöglichen,
6. Hardware-Idle-Befehl ausführen.

Wird währenddessen Arbeit verfügbar, darf die CPU nicht unbegrenzt im Idle-Zustand verbleiben.

---

## 15. Mehrkernsysteme

Auf Mehrkernsystemen werden Idle-Entscheidungen pro logischer CPU getroffen.

Zusätzlich können tiefere Zustände von mehreren CPUs abhängen:

- Core Idle
- Cluster Idle
- Package Idle
- System Idle

Ein Package-Zustand darf nur aktiviert werden, wenn alle betroffenen CPUs und Geräte dies zulassen.

Koordination darf keine globale Sperre im häufigen Schedulerpfad erfordern.

---

## 16. Scheduler-Integration

Der Scheduler kann Energie sparen durch:

- Bündelung von Arbeit auf weniger CPUs,
- längere Idle-Zeiten ungenutzter CPUs,
- Vermeidung unnötiger Threadmigration,
- Berücksichtigung von CPU-Affinität,
- Deaktivierung ungenutzter CPUs über CPU-Hotplug,
- Erhaltung von Echtzeit- und Fairnessgarantien.

Energieeinsparung darf feste Affinitäten und Echtzeitzusagen nicht verletzen.

Der Scheduler bestimmt die Arbeitsverteilung; der Idle-Governor bestimmt den Hardwarezustand einer inaktiven CPU.

---

## 17. Polling-Modus

Für extrem kurze erwartete Idle-Intervalle kann kontrolliertes Polling sinnvoll sein.

Polling ist nur zulässig, wenn:

- die erwartete Dauer sehr kurz ist,
- eine geringe Latenz wichtiger als Energieeinsparung ist,
- das Energieprofil es erlaubt,
- ein festes Zeit- oder Iterationslimit besteht.

Nach Ablauf des Limits wechselt die CPU in einen echten Hardware-Idle-Zustand.

Unbegrenztes Polling ist verboten.

---

## 18. Virtualisierung

Unter einem Hypervisor kann NovaOS paravirtualisierte Idle- oder Yield-Mechanismen verwenden.

Beispiele:

- CPU an Hypervisor zurückgeben,
- virtuelles HLT,
- paravirtualisierter Wait-Befehl,
- Steal-Time-Berücksichtigung.

NovaOS darf virtuelle Idle-Zeiten nicht ungeprüft als physische Energieeinsparung interpretieren.

Das Architektur-Backend wählt die für die virtuelle Plattform vorgesehene Methode.

---

## 19. Energieprofile

Die CPU-Idle-Auswahl reagiert auf das aktive Energieprofil:

| Profil | Verhalten |
|---|---|
| Power Saver | tiefere Zustände früher bevorzugen |
| Balanced | Energie und Latenz abwägen |
| Performance | flache Zustände und geringe Exit-Latenz bevorzugen |
| Real-time | nur explizit zulässige Zustände verwenden |

Thermische Schutzmaßnahmen dürfen unabhängig vom Profil stärkere Energiesparmaßnahmen anfordern.

---

## 20. Fehlerbehandlung

Ein Idle-Zustand kann deaktiviert werden, wenn:

- Wake-ups verspätet oder unzuverlässig sind,
- Timer verloren gehen,
- CPU-Kontext beschädigt wird,
- Firmwarefehler auftreten,
- wiederholt ungewöhnliche Exit-Latenzen gemessen werden,
- die Plattform einen Zustand als fehlerhaft meldet.

NovaOS fällt anschließend auf einen flacheren Zustand zurück.

Der sichere Fallback-Zustand bleibt verfügbar, solange die CPU grundsätzlich lauffähig ist.

---

## 21. Capability-Rechte

Gewöhnliche Anwendungen konfigurieren keine CPU-Idle-Zustände direkt.

Administrative Schnittstellen können folgende Rechte verwenden:

| Recht | Bedeutung |
|---|---|
| `CPU_IDLE_RIGHT_INSPECT` | Zustände und Statistiken lesen |
| `CPU_IDLE_RIGHT_LATENCY` | begrenzte Latenzanforderung stellen |
| `CPU_IDLE_RIGHT_DISABLE_STATE` | fehlerhaften Zustand deaktivieren |
| `CPU_IDLE_RIGHT_POLICY` | Governor- und Profilrichtlinie ändern |
| `CPU_IDLE_RIGHT_ADMIN` | CPU-Idle-Framework verwalten |

Latenzanforderungen besitzen eine Capability-Lebenszeit und optional ein Timeout.

---

## 22. Sicherheit und Datenschutz

Für CPU Idle gelten folgende Regeln:

- Firmwarezustände werden validiert.
- Anwendungen können keine beliebigen Hardware-Idle-Befehle ausführen.
- Latenzanforderungen werden begrenzt und autorisiert.
- Idle-Statistiken enthalten keine detaillierten Prozessaktivitätsverläufe für unprivilegierte Aufrufer.
- fehlgeschlagene Zustände werden sicher deaktiviert.
- der Idle-Pfad darf keine Kernel- oder Speicherisolation verändern.
- Debugging darf keine dauerhaft unsicheren Idle-Zustände aktivieren.

CPU-Idle-Telemetrie bleibt lokal.

---

## 23. Diagnose

Das Framework erfasst mindestens:

- Eintrittsanzahl pro Zustand,
- gesamte Verweildauer,
- durchschnittliche Verweildauer,
- frühe Wake-ups,
- gemessene Exit-Latenzen,
- fehlgeschlagene Zustände,
- aktive Latenzanforderungen,
- Zeit im Polling-Modus,
- Zeit im Fallback-Zustand.

Messungen dürfen den Idle-Pfad nicht wesentlich verlängern.

Detailstatistiken können CPU-lokal gesammelt und später zusammengeführt werden.

---

## 24. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_IDLE_STATE_UNAVAILABLE` | Zustand ist nicht verfügbar |
| `STATUS_IDLE_STATE_DISABLED` | Zustand wurde deaktiviert |
| `STATUS_LATENCY_CONSTRAINT` | Zustand verletzt eine Latenzanforderung |
| `STATUS_TIMER_UNAVAILABLE` | keine sichere Wake-up-Zeitquelle vorhanden |
| `STATUS_CPU_OFFLINE` | Ziel-CPU ist nicht aktiv |
| `STATUS_PLATFORM_ERROR` | Plattformübergang ist fehlgeschlagen |
| `STATUS_ACCESS_DENIED` | erforderliches Recht fehlt |
| `STATUS_LIMIT_EXCEEDED` | zu viele oder zu strenge Anforderungen |
| `STATUS_NOT_SUPPORTED` | Plattform unterstützt die Funktion nicht |

---

## 25. Alternative Ansätze

### 25.1 Immer nur HLT verwenden

**Vorteile:**

- einfach und robust,
- gute Unterstützung älterer x86-Hardware.

**Nachteile:**

- moderne tiefe Energiesparzustände bleiben ungenutzt,
- höhere Leistungsaufnahme.

**Entscheidung:** HLT bleibt Fallback, aber nicht der einzige Zustand.

### 25.2 Idle-Zustand ausschließlich nach nächstem Timer wählen

**Vorteile:**

- einfache Entscheidung.

**Nachteile:**

- externe Interrupts und reale Lastmuster bleiben unberücksichtigt,
- häufige Fehlentscheidungen.

**Entscheidung:** Timer bleibt ein wichtiges Signal, wird aber durch historische und qualitative Daten ergänzt.

### 25.3 Dauerhaftes Polling für minimale Latenz

**Vorteile:**

- sehr schnelle Reaktion.

**Nachteile:**

- hoher Stromverbrauch und Wärmeentwicklung.

**Entscheidung:** Nur kurzzeitig und streng begrenzt zulässig.

---

## 26. Konsequenzen

### Positive Konsequenzen

- niedrigerer Energieverbrauch,
- geringere Wärmeentwicklung,
- längere Akkulaufzeit,
- Unterstützung alter und moderner CPUs,
- tickless Idle,
- dynamische Latenzsteuerung,
- sichere Fallback-Strategie.

### Negative Konsequenzen

- plattformspezifische Backends erforderlich,
- Prognosen können falsch sein,
- tiefe Zustände erhöhen die Wake-up-Latenz,
- Package-Zustände benötigen Mehrkernkoordination.

### Risiken

- verlorene Wake-ups,
- fehlerhafte Firmwarezustände,
- verpasste Echtzeitfristen,
- defekte Timer in tiefen Zuständen,
- unnötiges Polling,
- hoher Koordinationsaufwand auf vielen CPUs.

---

## 27. Implementierungsrichtlinien

- Der Idle Thread delegiert Hardwareentscheidungen an das CPU-Idle-Framework.
- Jede CPU besitzt einen sicheren flachen Fallback-Zustand.
- Firmwareinformationen werden vor Verwendung validiert.
- Idle-Eintritt und Wake-up-Prüfung müssen race-frei sein.
- Tiefe Zustände werden nur mit zuverlässiger Wake-up-Quelle verwendet.
- Exit-Latenzen müssen aktive QoS- und Echtzeitanforderungen einhalten.
- Tickless Idle wird verwendet, sofern sichere Timer verfügbar sind.
- Polling ist immer zeitlich oder durch Iterationen begrenzt.
- Fehlerhafte Zustände werden zur Laufzeit deaktiviert.
- Scheduler- und Power-Entscheidungen bleiben logisch getrennt.

---

## 28. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Idle Thread und Fallback-Zustand,
- mehrere Hardware-Idle-Zustände,
- Wake-up durch Timer und Geräteinterrupts,
- race-freien Idle-Eintritt,
- Tickless Idle,
- Latenzanforderungen,
- Echtzeitlast,
- Mehrkern- und Package-Zustände,
- CPU-Affinität und CPU-Hotplug,
- fehlerhafte Firmwareangaben,
- unzuverlässige Timer,
- begrenzten Polling-Modus,
- Virtualisierung,
- wiederholte Idle-Ein- und Austritte.

---

## 29. Verbindliche Invarianten

1. Der Scheduler-Idle-Thread und die Hardware-Idle-Auswahl bleiben getrennte Verantwortlichkeiten.
2. Jede CPU besitzt einen sicheren flachen Fallback-Zustand.
3. Idle-Zustände werden vor der Verwendung validiert.
4. Idle-Eintritt darf keine Wake-ups verlieren.
5. Ein tiefer Zustand wird nur mit zuverlässiger Wake-up-Quelle verwendet.
6. Aktive Latenzanforderungen begrenzen die zulässige Exit-Latenz.
7. Tickless Idle darf notwendige Timerereignisse nicht verlieren.
8. Polling ist immer streng begrenzt.
9. Fehlerhafte Idle-Zustände können zur Laufzeit deaktiviert werden.
10. CPU-Idle-Telemetrie bleibt lokal und datensparsam.

---

## 30. Zusammenfassung

NovaOS implementiert ein zentrales CPU-Idle-Framework, das bei inaktiven CPUs dynamisch einen geeigneten Hardware-Ruhezustand auswählt.

Der Scheduler aktiviert seinen Idle Thread, während ein Governor anhand der erwarteten Idle-Dauer, Exit-Latenz, Timer, Energieprofile und Echtzeitanforderungen über die Zustandstiefe entscheidet.

Ein race-freier Eintritt, zuverlässige Wake-up-Quellen und ein stets verfügbarer flacher Fallback sorgen dafür, dass Energieeinsparung nicht auf Kosten der Systemstabilität oder zugesicherter Reaktionszeiten erfolgt.