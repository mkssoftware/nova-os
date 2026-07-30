# ADR-KERNEL-0015: Ereignismodell

* **ADR-ID:** ADR-KERNEL-0015
* **Titel:** Ereignismodell
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel-Architektur
* **Gültigkeitsbereich:** Gesamter Kernel
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0003, ADR-KERNEL-0011, ADR-KERNEL-0012, ADR-KERNEL-0013, NPSPEC-EVENT-0001

---

# 1. Zusammenfassung

NovaOS verwendet ein **einheitliches, kernelweites Ereignismodell (Event Model)** als Standardmechanismus für asynchrone Kommunikation zwischen Kernelkomponenten.

Nahezu alle Zustandsänderungen innerhalb des Kernels werden als standardisierte Ereignisse (Events) dargestellt. Dadurch werden Kernelkomponenten entkoppelt, Polling vermieden und eine einheitliche Grundlage für Scheduler, Geräteverwaltung, IPC, Dateisysteme, Netzwerk, GUI und Benachrichtigungssystem geschaffen. Ereignisorientierte Architekturen reduzieren unnötiges Polling und ermöglichen eine lose Kopplung zwischen Komponenten.

---

# 2. Kontext

Innerhalb eines Betriebssystems treten permanent Ereignisse auf.

Beispiele:

* Tastatureingaben
* Mausbewegungen
* Timerabläufe
* Prozessende
* Threadstart
* Netzwerkpakete
* Dateiänderungen
* Speicherfehler
* Geräteereignisse
* IPC-Nachrichten

Ohne ein einheitliches Ereignismodell würden Kernelkomponenten unterschiedliche Mechanismen implementieren.

---

# 3. Problemstellung

Es musste entschieden werden,

* wie Kernelereignisse dargestellt werden,
* wie Ereignisse verteilt werden,
* wie Komponenten benachrichtigt werden,
* wie Prioritäten behandelt werden,
* und wie zukünftige Erweiterungen integriert werden.

---

# 4. Entscheidung

NovaOS implementiert ein zentrales Ereignismodell.

Dabei gilt:

* sämtliche Kernelereignisse besitzen ein standardisiertes Format,
* Ereignisse werden ausschließlich über den Event Manager verteilt,
* Kernelkomponenten kommunizieren bevorzugt ereignisorientiert,
* Polling soll nur verwendet werden, wenn keine Ereignisquelle existiert.

---

# 5. Motivation

Diese Architektur bietet:

* lose Kopplung
* hohe Skalierbarkeit
* geringe CPU-Last
* einfache Erweiterbarkeit
* konsistente Kernelarchitektur
* gute Debugbarkeit

---

# 6. Designprinzipien

Das Ereignismodell basiert auf:

* Ereignisorientierung
* Asynchronität
* Determinismus
* Modularität
* Thread-Sicherheit
* Erweiterbarkeit

---

# 7. Architekturübersicht

```text
             Ereignisquelle

                    │

                    ▼

             Event Manager

                    │

      +-------------+-------------+

      ▼             ▼             ▼

 Scheduler     Device Manager   IPC

      ▼             ▼             ▼

 Dateisystem    GUI-System     Netzwerk

                    │

                    ▼

             Ereignisempfänger
```

---

# 8. Definition eines Ereignisses

Ein Ereignis beschreibt einen bereits eingetretenen Systemzustand.

Ein Ereignis verändert niemals selbst den Systemzustand, sondern informiert andere Komponenten darüber.

Ein Ereignis stellt eine unveränderliche Information über ein eingetretenes Ereignis dar.

---

# 9. Eigenschaften

Jedes Ereignis besitzt mindestens:

* Event-ID
* Ereignistyp
* Zeitstempel
* Quelle
* Ziel (optional)
* Priorität
* Nutzdaten
* Flags

---

# 10. Event-ID

Jedes Ereignis besitzt eine eindeutige Event-ID.

Diese dient ausschließlich:

* Debugging
* Tracing
* Logging
* Diagnose

---

# 11. Ereignistypen

Beispiele:

* ProcessCreated
* ProcessExited
* ThreadStarted
* ThreadStopped
* MemoryChanged
* DeviceAttached
* DeviceRemoved
* FileModified
* NetworkPacket
* WindowClosed
* TimerExpired
* InterruptReceived

Neue Ereignistypen können jederzeit ergänzt werden.

---

# 12. Ereignisquellen

Ereignisse können erzeugt werden durch:

* Kernel
* Scheduler
* Interrupts
* Gerätetreiber
* Timer
* IPC
* Dateisystem
* Netzwerk
* Benutzerprogramme (über definierte Systemaufrufe)

---

# 13. Event Manager

Der Event Manager übernimmt:

* Registrierung
* Verteilung
* Priorisierung
* Filterung
* Diagnose
* Statistik

Er ist die zentrale Instanz der Ereignisverwaltung.

---

# 14. Event Queue

Ereignisse werden zunächst in Event Queues abgelegt.

Eigenschaften:

* FIFO innerhalb gleicher Priorität
* thread-sicher
* mehrere Warteschlangen möglich
* prioritätsbewusst

Dedizierte Ereigniswarteschlangen verhindern, dass stark frequentierte Ereignisse andere Ereignistypen verdrängen.

---

# 15. Prioritäten

Jedes Ereignis besitzt eine Priorität.

Beispiele:

* Critical
* High
* Normal
* Low
* Background

---

# 16. Ereignisverteilung

Der Event Manager verteilt Ereignisse ausschließlich an registrierte Empfänger.

Nicht registrierte Komponenten erhalten keine Benachrichtigung.

---

# 17. Abonnementmodell

Kernelkomponenten registrieren sich für bestimmte Ereignistypen.

Unterstützt werden:

* einzelner Ereignistyp
* Ereignisgruppen
* Filter
* Objektfilter
* Prozessfilter

---

# 18. Broadcast-Ereignisse

Ein Ereignis kann:

* an genau einen Empfänger,
* an mehrere Empfänger,
* oder systemweit verteilt werden.

---

# 19. Zielgerichtete Ereignisse

Ein Ereignis kann optional einen eindeutigen Empfänger besitzen.

Beispiele:

* bestimmter Thread
* bestimmter Prozess
* bestimmtes Kernelobjekt

---

# 20. Ereignisreihenfolge

Innerhalb derselben Prioritätsklasse bleibt die Reihenfolge erhalten.

NovaOS garantiert deterministische Verarbeitung identischer Ereignisfolgen.

---

# 21. Interrupt-Ereignisse

Hardware-Interrupts erzeugen Kernelereignisse.

Der Interrupt Handler erzeugt dabei lediglich ein Ereignis und delegiert die eigentliche Verarbeitung an den Event Manager.

Dadurch bleiben Interrupt-Service-Routinen möglichst kurz.

---

# 22. Scheduler-Integration

Der Scheduler verwendet Ereignisse unter anderem für:

* Threadstart
* Threadende
* Wake-Up
* Sleep
* Timer
* Synchronisation

---

# 23. Objektintegration

Alle Kernelobjekte können Ereignisse erzeugen.

Beispiele:

* Datei geändert
* Prozess beendet
* Fenster geschlossen
* Gerät getrennt
* Socket verbunden

---

# 24. Sicherheit

Der Kernel prüft:

* Ereignisquelle
* Berechtigungen
* Zielobjekt
* Capability
* Integrität der Nutzdaten

Benutzerprogramme dürfen keine privilegierten Kernelereignisse erzeugen.

---

# 25. Thread-Sicherheit

Der Event Manager arbeitet vollständig thread-sicher.

Zum Einsatz kommen:

* atomare Operationen
* lockfreie Queues (wo sinnvoll)
* Spinlocks
* CPU-lokale Warteschlangen

---

# 26. Performance

Das Ereignismodell soll:

* minimale Latenzen
* geringe Synchronisationskosten
* hohe Parallelität
* geringe Speicherkosten

ermöglichen.

---

# 27. Debug-Unterstützung

Optional unterstützt NovaOS:

* Event-Tracing
* Event-Logging
* Zeitstempelanalyse
* Latenzmessung
* Ereignisstatistiken
* Event-Historie

---

# 28. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* verteilte Ereignisse
* Cluster-Ereignisse
* Remote Events
* Prioritätsklassen je CPU
* persistente Ereignisprotokolle
* KI-gestützte Ereignisanalyse

---

# 29. Auswirkungen

Positive Auswirkungen:

* lose Kopplung
* hohe Modularität
* geringere CPU-Auslastung
* weniger Polling
* einfache Erweiterbarkeit
* bessere Diagnosemöglichkeiten

Negative Auswirkungen:

* zusätzlicher Verwaltungsaufwand
* Ereigniswarteschlangen benötigen Speicher
* Prioritätsverwaltung erhöht die Komplexität

---

# 30. Verworfene Alternativen

## Reines Polling

Vorteile:

* einfache Implementierung

Nachteile:

* hohe CPU-Auslastung
* unnötige Latenzen
* schlechte Skalierbarkeit

---

## Direkte Funktionsaufrufe

Vorteile:

* geringe Latenz

Nachteile:

* starke Kopplung
* geringe Erweiterbarkeit
* schwierige Modularisierung

---

## Ereignisse ohne zentrale Verwaltung

Vorteile:

* einfache lokale Implementierung

Nachteile:

* inkonsistente APIs
* schwierige Diagnose
* keine systemweite Priorisierung

---

# 31. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* Ereignisse ausschließlich über den Event Manager erzeugen,
* registrierte Ereignistypen verwenden,
* keine direkten Ereigniswarteschlangen implementieren,
* Polling nur als Ausnahme einsetzen,
* Ereignisse unveränderlich behandeln.

---

# 32. Teststrategie

Das Ereignismodell wird geprüft durch:

* Unit-Tests
* Lasttests
* Parallelitätstests
* Prioritätstests
* Broadcast-Tests
* Latenzmessungen
* Langzeittests
* Stresstests

---

# 33. Konsequenzen

Das Ereignismodell wird zur verbindlichen Kommunikationsgrundlage sämtlicher Kernelkomponenten.

Alle zukünftigen Kernelmodule, Gerätetreiber und Subsysteme müssen ihre asynchronen Zustandsänderungen über standardisierte Ereignisse veröffentlichen. Dadurch entsteht eine einheitliche, lose gekoppelte und hoch skalierbare Kernelarchitektur, die sowohl auf Desktop-Systemen als auch auf Servern und eingebetteten Plattformen effizient arbeitet.

---

# 34. Zusammenfassung

NovaOS implementiert ein **zentrales Ereignismodell**, das sämtliche asynchronen Zustandsänderungen innerhalb des Systems über standardisierte Ereignisse abbildet. Der Event Manager übernimmt Registrierung, Priorisierung und Verteilung der Ereignisse und entkoppelt dadurch Kernelkomponenten voneinander. Dies reduziert Polling, verbessert die Skalierbarkeit und schafft eine konsistente Grundlage für Scheduler, Geräteverwaltung, IPC, Dateisysteme, Netzwerk und grafische Benutzeroberflächen.
