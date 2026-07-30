# ADR-KERNEL-0016: Interruptmodell

* **ADR-ID:** ADR-KERNEL-0016
* **Titel:** Interruptmodell
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel-Architektur
* **Gültigkeitsbereich:** HAL, Scheduler, Device Manager, Treiber, Event Manager, Process Manager
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0002, ADR-KERNEL-0003, ADR-KERNEL-0015, NPSPEC-INTERRUPT-0001

---

# 1. Zusammenfassung

NovaOS verwendet ein **mehrstufiges, prioritätsbasiertes Interruptmodell**, das Hardware-Interrupts, Prozessor-Ausnahmen und Software-Interrupts über eine gemeinsame Architektur verarbeitet.

Das Modell trennt die eigentliche Interruptannahme (Top Half) konsequent von der zeitintensiven Weiterverarbeitung (Bottom Half bzw. Deferred Work). Dadurch werden Interruptlatenzen minimiert, während aufwendige Arbeiten außerhalb des eigentlichen Interruptkontextes erfolgen. Dieses zweistufige Modell ist ein bewährter Ansatz moderner Betriebssysteme.

---

# 2. Kontext

Ein modernes Betriebssystem verarbeitet ständig:

* Hardware-Interrupts
* CPU-Ausnahmen
* Timer
* Software-Interrupts
* Interprozessor-Interrupts (IPI)
* Systemaufrufe
* Geräteereignisse

Diese Ereignisse müssen schnell, deterministisch und sicher verarbeitet werden.

---

# 3. Problemstellung

Es musste entschieden werden,

* wie Interrupts verarbeitet werden,
* wie Prioritäten vergeben werden,
* wie lange Interrupts laufen dürfen,
* wie Mehrkernsysteme unterstützt werden,
* und wie Interrupts mit Scheduler und Event Manager zusammenarbeiten.

---

# 4. Entscheidung

NovaOS implementiert ein einheitliches Interruptmodell.

Dabei gilt:

* alle Interrupts besitzen definierte Prioritäten,
* jede Interruptquelle besitzt einen Interrupt Descriptor,
* ISR und Deferred Work werden strikt getrennt,
* Interrupts werden vollständig über die HAL abstrahiert,
* der Scheduler darf unmittelbar nach einem Interrupt aktiviert werden.

---

# 5. Motivation

Diese Architektur bietet:

* geringe Interruptlatenzen
* hohe Systemreaktionsfähigkeit
* gute Skalierbarkeit
* klare Trennung der Verantwortlichkeiten
* Mehrkernunterstützung
* deterministisches Verhalten

---

# 6. Designprinzipien

Das Interruptmodell basiert auf:

* geringer Latenz
* kurzer ISR-Laufzeit
* Priorisierung
* Modularität
* Architekturunabhängigkeit
* Thread-Sicherheit
* Erweiterbarkeit

---

# 7. Architekturübersicht

```text
             Hardware

                │

                ▼

      Interrupt Controller

                │

                ▼

        HAL Interrupt Layer

                │

                ▼

     Interrupt Dispatcher

                │

      ┌─────────┴─────────┐

      ▼                   ▼

 Interrupt Service    Deferred Work
     Routine              Queue

      │                   │

      └─────────┬─────────┘

                ▼

          Event Manager

                ▼

        Scheduler / Kernel
```

---

# 8. Interruptquellen

NovaOS unterstützt:

* Hardware-Interrupts
* Software-Interrupts
* CPU-Ausnahmen
* NMI (Non Maskable Interrupt)
* Timerinterrupts
* Interprozessor-Interrupts (IPI)

---

# 9. Interrupt Descriptor

Jeder Interrupt besitzt einen Descriptor.

Dieser enthält mindestens:

* Interruptnummer
* Priorität
* Handler
* Flags
* CPU-Zuordnung
* Statistikdaten
* Status

---

# 10. Interruptprioritäten

Interrupts werden priorisiert.

Beispiel:

* Critical
* High
* Normal
* Low

Die konkrete Priorisierung ist architekturabhängig.

---

# 11. Interrupt Service Routine (ISR)

Die ISR besitzt ausschließlich folgende Aufgaben:

* Interrupt bestätigen
* Hardwarestatus sichern
* notwendige Register lesen
* Deferred Work einplanen
* Interrupt verlassen

ISR-Routinen müssen möglichst kurz ausgeführt werden, um die Interruptlatenz gering zu halten.

---

# 12. Deferred Work

Zeitintensive Arbeiten werden außerhalb des Interruptkontextes ausgeführt.

Beispiele:

* Dateisystemzugriffe
* Netzwerkverarbeitung
* GUI-Ereignisse
* Speicherallokationen
* komplexe Treiberlogik

---

# 13. Top Half / Bottom Half

NovaOS verwendet ein zweistufiges Interruptmodell.

Top Half:

* minimale Laufzeit
* keine blockierenden Operationen
* keine Speicherallokationen
* keine langen Schleifen

Bottom Half:

* vollständige Verarbeitung
* Schedulerfähig
* Ereigniserzeugung
* Kommunikation mit Kernelkomponenten

Dieses Modell reduziert die Zeit, in der sich das System im Interruptkontext befindet.

---

# 14. Interrupt Controller

Die HAL kapselt sämtliche Interruptcontroller.

Beispiele:

* PIC
* APIC
* IO-APIC
* GIC
* PLIC

Treiber arbeiten ausschließlich mit der HAL-Schnittstelle.

---

# 15. CPU-Ausnahmen

CPU-Ausnahmen werden ebenfalls über das Interruptmodell verarbeitet.

Beispiele:

* Divide by Zero
* Invalid Opcode
* General Protection Fault
* Page Fault
* Double Fault

---

# 16. Timerinterrupts

Der Systemtimer erzeugt periodische Interrupts.

Diese dienen unter anderem für:

* Scheduler
* Timer
* Zeitmessung
* Timeoutverwaltung

---

# 17. Interprozessor-Interrupts (IPI)

Mehrkernsysteme verwenden IPIs für:

* Scheduler
* TLB-Invalidierung
* Synchronisation
* CPU-Steuerung
* Kernelkoordination

---

# 18. Mehrkernbetrieb

Jeder Prozessorkern besitzt:

* eigene Interruptstatistiken
* eigene Interruptstacks
* eigene Deferred-Work-Queues
* lokale Interruptsteuerung

---

# 19. Verschachtelte Interrupts

NovaOS unterstützt verschachtelte Interrupts.

Ein Interrupt höherer Priorität darf einen Interrupt niedrigerer Priorität unterbrechen.

Die zulässigen Verschachtelungsregeln werden architekturabhängig durch die HAL umgesetzt.

---

# 20. Interruptmaskierung

Interrupts können temporär maskiert werden.

Dabei gilt:

* nur so kurz wie technisch notwendig,
* niemals während längerer Kerneloperationen,
* niemals in regulären Anwendungen.

Lange Phasen mit deaktivierten Interrupts erhöhen die Interruptlatenz und verschlechtern die Systemreaktion.

---

# 21. Interruptstacks

NovaOS verwendet dedizierte Interruptstacks.

Dadurch werden:

* Kernelstacks entlastet
* Stacküberläufe reduziert
* Interruptverschachtelungen sicher verarbeitet

---

# 22. Schedulerintegration

Nach Abschluss eines Interrupts darf der Scheduler entscheiden,

* ob der aktuelle Thread fortgesetzt wird,
* ob ein höher priorisierter Thread aktiviert wird,
* oder ob Deferred Work zuerst verarbeitet wird.

---

# 23. Eventintegration

Interrupts erzeugen standardisierte Kernelereignisse.

Diese werden anschließend durch den Event Manager verarbeitet.

Dadurch bleiben Interruptlogik und Ereignisverarbeitung voneinander getrennt.

---

# 24. Sicherheit

Der Kernel schützt vor:

* ungültigen Interruptnummern
* fehlerhaften Treibern
* Interruptstürmen
* mehrfacher Handlerregistrierung
* Manipulation der Interrupttabellen

---

# 25. Performance

Das Interruptmodell soll:

* minimale Interruptlatenz
* kurze ISR-Laufzeiten
* geringe Synchronisationskosten
* hohe Parallelität

ermöglichen.

---

# 26. Debug-Unterstützung

Optional unterstützt NovaOS:

* Interruptstatistiken
* Latenzmessungen
* Handlerlaufzeiten
* Interrupt-Historie
* Spurious-Interrupt-Erkennung
* Tracing

---

# 27. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* MSI
* MSI-X
* Message-basierte Interrupts
* virtuelle Interruptcontroller
* Echtzeitprioritäten
* CPU-Isolation

---

# 28. Auswirkungen

Positive Auswirkungen:

* geringe Latenzen
* hohe Reaktionsfähigkeit
* saubere Trennung von ISR und Deferred Work
* gute Mehrkernunterstützung
* hohe Skalierbarkeit
* einfache Erweiterbarkeit

Negative Auswirkungen:

* komplexere Implementierung
* zusätzlicher Verwaltungsaufwand
* Deferred-Work-Verwaltung benötigt Speicher

---

# 29. Verworfene Alternativen

## Vollständige Verarbeitung in der ISR

Vorteile:

* einfache Architektur

Nachteile:

* hohe Interruptlatenz
* schlechte Skalierbarkeit
* lange blockierte Interrupts

---

## Polling statt Interrupts

Vorteile:

* einfache Implementierung

Nachteile:

* hohe CPU-Auslastung
* schlechtere Reaktionszeiten
* unnötiger Energieverbrauch

---

## Architekturspezifische Interruptmodelle

Vorteile:

* maximale Hardwareoptimierung

Nachteile:

* geringe Portabilität
* hoher Wartungsaufwand
* inkonsistente Kernelarchitektur

---

# 30. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* ISR-Routinen möglichst kurz halten,
* blockierende Operationen außerhalb der ISR ausführen,
* Deferred Work für zeitintensive Aufgaben verwenden,
* Interrupts ausschließlich über die HAL registrieren,
* keine architekturspezifischen Interruptfunktionen direkt verwenden.

---

# 31. Teststrategie

Das Interruptmodell wird geprüft durch:

* Unit-Tests
* Interruptlatenztests
* Prioritätstests
* Mehrkerntests
* Nested-Interrupt-Tests
* Stresstests
* Langzeittests
* Performancebenchmarks

---

# 32. Konsequenzen

Das Interruptmodell bildet die verbindliche Grundlage für sämtliche Hardwareereignisse innerhalb von NovaOS.

Alle zukünftigen Gerätetreiber, Kernelmodule und Hardwareabstraktionen müssen dieses Modell verwenden. Die konsequente Trennung zwischen Interrupt Service Routine und Deferred Work stellt sicher, dass NovaOS auch unter hoher Last geringe Interruptlatenzen, hohe Skalierbarkeit und ein deterministisches Laufzeitverhalten erreicht.

---

# 33. Zusammenfassung

NovaOS implementiert ein **mehrstufiges, prioritätsbasiertes Interruptmodell**, das Hardware-Interrupts, Prozessor-Ausnahmen und Software-Interrupts über eine gemeinsame Architektur verarbeitet. Durch die konsequente Trennung zwischen kurzer Interrupt Service Routine und nachgelagerter Deferred-Work-Verarbeitung werden geringe Interruptlatenzen, hohe Parallelität und eine skalierbare Kernelarchitektur erreicht. Die HAL abstrahiert dabei sämtliche architekturspezifischen Unterschiede, sodass das Interruptmodell auf allen unterstützten Prozessorarchitekturen einheitlich verwendet werden kann.
