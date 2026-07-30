# ADR-KERNEL-0020: Tickless Kernel

* **ADR-ID:** ADR-KERNEL-0020
* **Titel:** Tickless Kernel
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel-Architektur
* **Gültigkeitsbereich:** Scheduler, Timer Manager, HAL, SMP, Power Manager, Interrupt Manager
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0003, ADR-KERNEL-0016, ADR-KERNEL-0018, ADR-KERNEL-0019, NPSPEC-TIMER-0002

---

# 1. Zusammenfassung

NovaOS implementiert einen **Tickless Kernel (Dynamic Tick)** als Standardbetriebsmodus.

Im Gegensatz zu klassischen periodischen Betriebssystem-Timern erzeugt NovaOS keine festen Scheduler-Ticks in regelmäßigen Zeitabständen. Stattdessen werden Timer-Interrupts ausschließlich dann ausgelöst, wenn sie tatsächlich benötigt werden. Dadurch sinken Energieverbrauch, Interruptlast und Scheduler-Jitter, während gleichzeitig die Skalierbarkeit auf Mehrkernsystemen verbessert wird. Tickless-Kernel reduzieren unnötige Scheduling-Ticks und verbessern insbesondere Energieeffizienz sowie Echtzeit- und HPC-Szenarien.

---

# 2. Kontext

Traditionelle Betriebssysteme erzeugen periodische Timer-Interrupts.

Beispiele:

* 100 Hz
* 250 Hz
* 500 Hz
* 1000 Hz

Diese Interrupts treten unabhängig davon auf, ob tatsächlich Arbeit vorhanden ist.

Dies führt zu:

* unnötigen CPU-Weckvorgängen
* höherem Energieverbrauch
* größerem Scheduler-Jitter
* schlechterer Skalierbarkeit

---

# 3. Problemstellung

Es musste entschieden werden,

* wie Zeitereignisse verarbeitet werden,
* ob periodische Scheduler-Ticks notwendig sind,
* wie Idle-CPUs behandelt werden,
* wie Echtzeitanforderungen berücksichtigt werden,
* und wie der Energieverbrauch reduziert werden kann.

---

# 4. Entscheidung

NovaOS verwendet einen vollständig ticklosen Kernel.

Dabei gilt:

* keine periodischen Scheduler-Ticks,
* Timer werden ausschließlich bei Bedarf programmiert,
* Idle-CPUs erhalten keine unnötigen Interrupts,
* alle Zeitereignisse werden ereignisgesteuert verarbeitet.

---

# 5. Motivation

Diese Architektur bietet:

* geringeren Energieverbrauch
* weniger Interrupts
* geringeren Scheduler-Jitter
* bessere Echtzeiteigenschaften
* höhere Skalierbarkeit
* längere Akkulaufzeit mobiler Geräte

---

# 6. Designprinzipien

Der Tickless Kernel basiert auf:

* Ereignisorientierung
* Dynamischer Zeitplanung
* Minimalen Interrupts
* Energieeffizienz
* Determinismus
* Architekturunabhängigkeit

---

# 7. Architekturübersicht

```text
             Timer Request

                   │

                   ▼

            Timer Manager

                   │

                   ▼

      Nächsten Ablauf berechnen

                   │

                   ▼

       HAL Timer Programmierung

                   │

                   ▼

          Hardware Timer

                   │

             Interrupt nur

           bei Bedarf auslösen

                   │

                   ▼

        Scheduler / Event Manager
```

---

# 8. Tickless-Prinzip

Der Kernel besitzt keinen festen Zeittakt.

Stattdessen wird immer nur der nächste tatsächlich benötigte Timer programmiert.

Existiert kein bevorstehendes Ereignis, bleibt die CPU im Leerlauf.

---

# 9. Dynamische Timer

Alle Kernel-Timer arbeiten dynamisch.

Beispiele:

* Sleep
* Timeout
* Scheduler
* Watchdog
* Netzwerk
* Dateisystem
* IPC
* Animationen

---

# 10. Idle-CPUs

Befindet sich ein Prozessor im Idle-Zustand,

* werden keine periodischen Scheduler-Ticks erzeugt,
* kann die CPU tiefere Energiesparzustände erreichen,
* erfolgt das Aufwecken ausschließlich durch reale Ereignisse.

Das Unterdrücken von Scheduler-Ticks auf inaktiven CPUs reduziert den Energieverbrauch deutlich.

---

# 11. Laufende CPUs

Auch aktive Prozessoren erhalten nur dann Timer-Interrupts,

* wenn ein Timer abläuft,
* ein Scheduler-Ereignis notwendig wird,
* oder Hardware dies erfordert.

---

# 12. Timer Manager

Der Timer Manager übernimmt:

* Timerregistrierung
* Timerlöschung
* Sortierung
* Priorisierung
* Neuberechnung
* Hardwareprogrammierung

---

# 13. Schedulerintegration

Der Scheduler arbeitet vollständig ticklos.

Er wird aktiviert durch:

* Timer
* Interrupts
* IPC
* Ereignisse
* Wake-Ups
* Prozesswechsel

Ein periodischer Scheduler-Tick existiert nicht.

---

# 14. Timerauflösung

NovaOS verwendet hochauflösende Timer.

Die konkrete Auflösung hängt ausschließlich von der Hardwareplattform ab.

---

# 15. Hardwareunterstützung

Die HAL unterstützt unter anderem:

* HPET
* APIC Timer
* ARM Generic Timer
* RISC-V Timer
* zukünftige Hardwaretimer

Alle Unterschiede werden vollständig abstrahiert.

---

# 16. Mehrkernbetrieb

Jeder CPU-Kern besitzt:

* eigene Timerverwaltung
* eigene Ablaufzeit
* lokale Timerprogrammierung
* CPU-lokale Statistiken

Dadurch entstehen keine unnötigen globalen Synchronisationen.

---

# 17. Timer-Neuprogrammierung

Wird ein neuer Timer registriert,

* prüft der Timer Manager den nächsten Ablaufzeitpunkt,
* programmiert den Hardwaretimer gegebenenfalls neu,
* informiert den Scheduler bei Bedarf.

---

# 18. Energieverwaltung

Der Tickless Kernel unterstützt:

* tiefe Sleep States
* Core Parking
* CPU Hotplug
* dynamische Frequenzanpassung
* mobiles Energiemanagement

Je länger Prozessoren ungestört im Leerlauf verbleiben können, desto effizienter können Energiesparmechanismen arbeiten.

---

# 19. Echtzeitbetrieb

Der Tickless Kernel verbessert:

* Vorhersagbarkeit
* geringeren Scheduler-Jitter
* kürzere Reaktionszeiten
* geringere Interruptlast

Für bestimmte Echtzeit- oder Spezialanwendungen kann jedoch weiterhin ein periodischer Tick Vorteile bieten. NovaOS erlaubt daher eine architekturseitige Konfigurationsoption für Spezialplattformen, obwohl der Tickless-Modus der Standard bleibt.

---

# 20. Ereignisintegration

Der Tickless Kernel arbeitet direkt mit:

* Event Manager
* Scheduler
* Interrupt Manager
* Timer Manager
* Power Manager

zusammen.

---

# 21. Sicherheit

Der Kernel schützt vor:

* verlorenen Timerereignissen
* doppelten Timerauslösungen
* Zeitüberläufen
* fehlerhafter Hardwareprogrammierung
* inkonsistenten Zeitquellen

---

# 22. Performance

Der Tickless Kernel soll:

* minimale Interruptanzahl
* geringe CPU-Auslastung
* niedrigen Scheduler-Jitter
* hohe Skalierbarkeit

ermöglichen.

---

# 23. Debug-Unterstützung

Optional unterstützt NovaOS:

* Timerstatistiken
* Timerlatenzmessung
* Wake-Up-Analyse
* Interruptstatistik
* Scheduler-Jitter-Analyse
* Timer-Tracing

---

# 24. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* virtuelle Timer
* Hardware-Offload-Timer
* Cluster-Timer
* Echtzeit-Timerklassen
* KI-gestützte Timeroptimierung
* adaptive Timerstrategien

---

# 25. Auswirkungen

Positive Auswirkungen:

* geringerer Energieverbrauch
* weniger Interrupts
* geringere CPU-Last
* bessere Akkulaufzeit
* geringerer Scheduler-Jitter
* bessere Skalierbarkeit

Negative Auswirkungen:

* komplexere Timerverwaltung
* häufigere Timer-Neuprogrammierung
* höhere Anforderungen an die HAL
* anspruchsvollere Fehlersuche bei Zeitproblemen

---

# 26. Verworfene Alternativen

## Periodischer Scheduler-Tick

Vorteile:

* einfache Implementierung
* konstante Zeitbasis

Nachteile:

* hoher Energieverbrauch
* unnötige Interrupts
* schlechtere Skalierbarkeit

---

## Hybrider Tick-Modus

Vorteile:

* einfachere Migration bestehender Systeme

Nachteile:

* höhere Komplexität
* uneinheitliches Verhalten
* eingeschränkte Optimierungsmöglichkeiten

---

## Plattformabhängige Timerverwaltung

Vorteile:

* maximale Hardwareoptimierung

Nachteile:

* geringe Portabilität
* hoher Wartungsaufwand
* inkonsistente Kernelarchitektur

---

# 27. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* ausschließlich den Timer Manager verwenden,
* keine periodischen Timer voraussetzen,
* Timer möglichst kurz halten,
* CPU-lokale Timer bevorzugen,
* den Tickless-Betrieb vollständig unterstützen.

---

# 28. Teststrategie

Der Tickless Kernel wird geprüft durch:

* Unit-Tests
* Timergenauigkeitstests
* Energieverbrauchsmessungen
* Mehrkerntests
* Stresstests
* Langzeittests
* Scheduler-Jitter-Messungen
* Performancebenchmarks

---

# 29. Konsequenzen

Der Tickless Kernel wird zur verbindlichen Zeitarchitektur von NovaOS.

Alle zukünftigen Kernelmodule, Schedulerkomponenten und Gerätetreiber müssen davon ausgehen, dass keine periodischen Scheduler-Ticks existieren. Zeitabhängige Funktionen sind ausschließlich über den Timer Manager und ereignisgesteuerte Mechanismen zu implementieren. Dadurch erreicht NovaOS eine energieeffiziente, skalierbare und moderne Kernelarchitektur, die sowohl Desktop-, Mobil- als auch Serverplattformen optimal unterstützt.

---

# 30. Zusammenfassung

NovaOS implementiert einen **vollständig ticklosen Kernel**, bei dem Scheduler- und Timer-Interrupts ausschließlich bei tatsächlichem Bedarf ausgelöst werden. Durch die enge Zusammenarbeit von Timer Manager, Scheduler, HAL und Power Manager werden unnötige CPU-Weckvorgänge vermieden, der Energieverbrauch reduziert und die Skalierbarkeit auf Mehrkernsystemen verbessert. Diese Architektur bildet die Grundlage für eine moderne, effiziente und deterministische Zeitverwaltung innerhalb des NovaOS-Kernels.
