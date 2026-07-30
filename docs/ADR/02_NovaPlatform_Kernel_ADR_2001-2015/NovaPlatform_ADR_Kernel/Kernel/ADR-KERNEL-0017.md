# ADR-KERNEL-0017: Exceptionmodell

* **ADR-ID:** ADR-KERNEL-0017
* **Titel:** Exceptionmodell
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel-Architektur
* **Gültigkeitsbereich:** HAL, CPU-Abstraktion, Memory Manager, Scheduler, Process Manager, Event Manager
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0016, ADR-KERNEL-0015, ADR-KERNEL-0009, NPSPEC-EXCEPTION-0001

---

# 1. Zusammenfassung

NovaOS verwendet ein **einheitliches Exceptionmodell** zur Behandlung sämtlicher Prozessor-Ausnahmen und softwareseitig erkannter Ausnahmezustände.

Das Modell definiert einen standardisierten Ablauf für Erkennung, Klassifizierung, Diagnose, Wiederherstellung und – falls erforderlich – den kontrollierten Abbruch eines Prozesses oder des Kernels. Recoverbare Ausnahmen werden nach Möglichkeit behandelt, während nicht behebbaren Fehlern deterministisch begegnet wird. Moderne Betriebssysteme unterscheiden dabei klar zwischen recoverbaren Fehlern (z. B. Page Faults) und fatalen Prozessor-Ausnahmen.

---

# 2. Kontext

Während der Programmausführung können Ausnahmen entstehen.

Beispiele:

* Division durch Null
* Ungültige Instruktionen
* Seitenfehler
* Speicherzugriffsfehler
* Alignment-Fehler
* Privilegverletzungen
* Debug-Ausnahmen
* Maschinenfehler

Diese Ausnahmen werden unmittelbar von der CPU erkannt.

---

# 3. Problemstellung

Es musste entschieden werden,

* wie Prozessor-Ausnahmen behandelt werden,
* wann eine Wiederherstellung zulässig ist,
* wann Prozesse beendet werden,
* wann ein Kernel Panic ausgelöst wird,
* und wie Diagnoseinformationen gesammelt werden.

---

# 4. Entscheidung

NovaOS implementiert ein zentrales Exceptionmodell.

Dabei gilt:

* sämtliche CPU-Ausnahmen werden über den Exception Manager verarbeitet,
* recoverbare Ausnahmen werden bevorzugt behandelt,
* fatale Kernel-Ausnahmen führen kontrolliert zum Kernel Panic,
* Benutzerprozesse dürfen isoliert beendet werden, ohne das Gesamtsystem zu gefährden.

---

# 5. Motivation

Diese Architektur bietet:

* deterministisches Verhalten
* hohe Stabilität
* saubere Fehlerdiagnose
* einfache Erweiterbarkeit
* konsistente Fehlerbehandlung
* klare Trennung zwischen Benutzer- und Kernelmodus

---

# 6. Designprinzipien

Das Exceptionmodell basiert auf:

* Determinismus
* Recoverability
* Isolation
* Diagnostik
* Thread-Sicherheit
* Architekturunabhängigkeit

---

# 7. Architekturübersicht

```text
                 CPU

                  │

                  ▼

          Exception Entry

                  │

                  ▼

         HAL Exception Layer

                  │

                  ▼

         Exception Manager

          │             │

          ▼             ▼

 Recovery Handler   Panic Manager

          │             │

          ▼             ▼

 Scheduler      Kernel Diagnostic
```

---

# 8. Definition einer Exception

Eine Exception ist ein synchrones Ereignis, das unmittelbar durch die Ausführung einer Instruktion ausgelöst wird.

Sie unterscheidet sich von Interrupts dadurch, dass sie immer einer konkreten Instruktion zugeordnet werden kann. Prozessor-Ausnahmen entstehen synchron zum Programmablauf und werden durch die CPU selbst ausgelöst.

---

# 9. Exceptionklassen

NovaOS unterscheidet:

* Fault
* Trap
* Abort
* Machine Exception
* Software Exception

---

# 10. Recoverbare Exceptions

Recoverbare Ausnahmen können behandelt werden.

Beispiele:

* Page Fault
* Guard Page
* Lazy Allocation
* Copy-on-Write
* Demand Paging

Der Kernel versucht zunächst die Ursache zu beheben und setzt anschließend die Programmausführung fort. Page Faults können beispielsweise zum Nachladen einer Speicherseite genutzt werden.

---

# 11. Nicht recoverbare Exceptions

Nicht behebbar sind unter anderem:

* Double Fault
* Triple Fault
* schwere Maschinenfehler
* Kernel Stack Corruption
* beschädigte Kerneldaten

Diese führen unmittelbar zum Panic Manager.

---

# 12. Benutzer- und Kernelmodus

NovaOS unterscheidet strikt zwischen:

* User Exceptions
* Kernel Exceptions

Benutzerausnahmen dürfen ausschließlich den betroffenen Prozess beeinflussen.

Kernel-Ausnahmen können das Gesamtsystem betreffen.

---

# 13. Exception Manager

Der Exception Manager übernimmt:

* Klassifizierung
* Analyse
* Recovery
* Logging
* Statistik
* Eskalation

---

# 14. Exception Descriptor

Jede Exception besitzt einen Descriptor.

Dieser enthält mindestens:

* Exception-ID
* CPU-Nummer
* Prozess-ID
* Thread-ID
* Fehlercode
* Registersatz
* Zeitstempel

---

# 15. Registersicherung

Beim Eintritt einer Exception werden gesichert:

* allgemeine Register
* Programmzähler
* Stack Pointer
* Statusregister
* Architekturregister

Dadurch ist eine vollständige Diagnose möglich.

---

# 16. Page Fault Behandlung

Page Faults besitzen Sonderbehandlung.

Mögliche Ursachen:

* nicht geladene Seite
* Copy-on-Write
* Schutzverletzung
* ungültige Adresse

Der Memory Manager entscheidet über die weitere Verarbeitung.

---

# 17. Privilegverletzungen

Versucht ein Benutzerprozess:

* privilegierte Instruktionen,
* Kerneladressen,
* Kernelregister,
* geschützte Geräte

zu verwenden, wird die Operation sofort beendet und der Prozess erhält einen entsprechenden Fehler.

---

# 18. Maschinenfehler

Hardwarefehler werden gesondert behandelt.

Beispiele:

* ECC-Fehler
* Cachefehler
* CPU-Fehler
* Busfehler

Je nach Schwere erfolgt:

* Recovery
* Geräteabschaltung
* Kernel Panic

---

# 19. Exception Recovery

Recoverbare Exceptions dürfen:

* Speicherseiten nachladen
* Threads fortsetzen
* Ressourcen neu initialisieren
* Kernelobjekte reparieren

Recovery darf niemals inkonsistente Zustände erzeugen.

---

# 20. Exception-Eskalation

Kann eine Exception nicht behandelt werden, erfolgt folgende Eskalation:

1. Recovery
2. Prozessabbruch
3. Kernel Panic

---

# 21. Schedulerintegration

Nach erfolgreicher Recovery entscheidet der Scheduler,

* ob der aktuelle Thread fortgesetzt wird,
* ob ein anderer Thread aktiviert wird,
* oder ob der Prozess beendet wird.

---

# 22. Ereignisintegration

Jede Exception erzeugt ein standardisiertes Kernelereignis.

Dieses wird dem Event Manager gemeldet.

Dadurch stehen Diagnosen, Tracing und Statistiken systemweit zur Verfügung.

---

# 23. Debug-Unterstützung

NovaOS unterstützt:

* Breakpoints
* Single Step
* Watchpoints
* Exception Tracing
* Stack Tracing

Debug-Ausnahmen werden getrennt von normalen Fehlerausnahmen behandelt.

---

# 24. Sicherheit

Das Exceptionmodell schützt vor:

* ungültigen Speicherzugriffen
* Kernelkorruption
* Rechteeskalation
* beschädigten Prozessen
* undefiniertem Verhalten

---

# 25. Performance

Das Exceptionmodell soll:

* geringe Latenzen
* schnelle Recovery
* minimale Kontextwechsel
* effiziente Diagnose

ermöglichen.

---

# 26. Thread-Sicherheit

Die Exceptionverarbeitung arbeitet vollständig thread-sicher.

Mehrere CPU-Kerne können gleichzeitig unterschiedliche Exceptions behandeln.

CPU-lokale Datenstrukturen minimieren Synchronisationsaufwand.

---

# 27. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* Virtualisierungs-Exceptions
* Hypervisor-Exceptions
* KI-gestützte Fehleranalyse
* Hardware Recovery Services
* Remote Diagnostics

---

# 28. Auswirkungen

Positive Auswirkungen:

* hohe Stabilität
* saubere Fehlerisolierung
* konsistente Diagnose
* deterministische Recovery
* gute Mehrkernunterstützung
* einfache Erweiterbarkeit

Negative Auswirkungen:

* komplexere Implementierung
* zusätzlicher Verwaltungsaufwand
* höhere Anforderungen an Diagnosekomponenten

---

# 29. Verworfene Alternativen

## Direkte Behandlung in jedem Subsystem

Vorteile:

* einfache lokale Implementierung

Nachteile:

* inkonsistente Fehlerbehandlung
* hoher Wartungsaufwand
* doppelte Implementierungen

---

## Sofortiger Kernel Panic bei jeder Exception

Vorteile:

* einfache Implementierung

Nachteile:

* geringe Verfügbarkeit
* schlechte Benutzererfahrung
* keine Recovery möglich

---

## Ausschließlich prozesslokale Behandlung

Vorteile:

* geringe Kernelkomplexität

Nachteile:

* fehlende zentrale Diagnose
* keine systemweite Fehleranalyse

---

# 30. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* Exceptions ausschließlich über den Exception Manager behandeln,
* Registerzustände vollständig sichern,
* Recoverys deterministisch durchführen,
* Kernel- und Benutzer-Exceptions strikt trennen,
* jede Exception protokollieren.

---

# 31. Teststrategie

Das Exceptionmodell wird geprüft durch:

* Unit-Tests
* Fault-Injection-Tests
* Page-Fault-Tests
* Mehrkerntests
* Recovery-Tests
* Stresstests
* Langzeittests
* Performancebenchmarks

---

# 32. Konsequenzen

Das Exceptionmodell bildet die verbindliche Grundlage für sämtliche Prozessor-Ausnahmen innerhalb von NovaOS.

Alle zukünftigen Kernelmodule, Gerätetreiber und Speicherverwaltungen müssen Ausnahmen ausschließlich über den Exception Manager behandeln. Recoverbare Fehler werden bevorzugt automatisch behoben, während nicht recoverbare Kernelzustände kontrolliert in den Panic Manager überführt werden. Dadurch entsteht eine robuste, nachvollziehbare und langfristig wartbare Fehlerarchitektur.

---

# 33. Zusammenfassung

NovaOS implementiert ein **zentrales Exceptionmodell**, das sämtliche Prozessor-Ausnahmen einheitlich verarbeitet. Recoverbare Fehler werden durch den Exception Manager analysiert und – sofern möglich – automatisch behoben, während schwerwiegende Kernel-Ausnahmen kontrolliert eskaliert werden. Die klare Trennung zwischen Benutzer- und Kernelmodus sowie die Integration mit Event Manager, Scheduler und Memory Manager schaffen eine robuste, deterministische und hochgradig diagnostizierbare Kernelarchitektur.
