# ADR-MM-0015: Kernel Stack Layout

* **ADR-ID:** ADR-MM-0015
* **Titel:** Kernel Stack Layout
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Memory Management
* **Gültigkeitsbereich:** Kernel, Scheduler, Thread Manager, Interrupt Manager, Exception Manager, Virtual Memory Manager
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0005, ADR-KERNEL-0016, ADR-KERNEL-0017, ADR-MM-0004, ADR-MM-0013

---

# 1. Zusammenfassung

NovaOS definiert ein standardisiertes **Kernel Stack Layout** für sämtliche Kernel-Threads, Benutzerthreads und CPU-interne Ausführungskontexte.

Jeder Kernel-Stack besitzt eine klar definierte Speicherstruktur mit Guard Pages, Architekturinformationen, Ausnahme- und Interrupt-Frames sowie einem reservierten Bereich für zukünftige Erweiterungen. Ziel ist eine sichere, deterministische und auf allen unterstützten Prozessorarchitekturen einheitliche Stackorganisation. Moderne Kernel verwenden häufig virtuell gemappte Kernel-Stacks mit Guard Pages, um Stack-Überläufe unmittelbar zu erkennen und Speicherkorruption zu verhindern.

---

# 2. Kontext

Der Kernel verwendet Stacks für:

* Systemaufrufe
* Interrupts
* Exceptions
* Scheduler
* Kernel-Threads
* Gerätetreiber
* Rekursion
* lokale Variablen

Die Stackstruktur beeinflusst unmittelbar:

* Stabilität
* Performance
* Sicherheit
* Debugbarkeit

---

# 3. Problemstellung

Es musste entschieden werden,

* wie Kernel-Stacks aufgebaut sind,
* welche Bereiche reserviert werden,
* wie Stack-Überläufe erkannt werden,
* wie Interrupts integriert werden,
* und wie zukünftige Erweiterungen berücksichtigt werden.

---

# 4. Entscheidung

NovaOS verwendet einen einheitlichen Kernel-Stack-Aufbau.

Dabei gilt:

* Jeder Thread besitzt einen eigenen Kernel-Stack.
* Jeder Stack besitzt Guard Pages.
* Alle Kernel-Stacks folgen derselben Struktur.
* Architekturabhängige Bereiche sind klar gekapselt.

---

# 5. Motivation

Diese Architektur bietet:

* konsistente Speicherstruktur
* einfache Fehlersuche
* bessere Sicherheit
* hohe Portierbarkeit
* gute Erweiterbarkeit
* deterministisches Verhalten

---

# 6. Designprinzipien

Das Kernel Stack Layout basiert auf:

* Einheitlichkeit
* Isolation
* Hardwareunabhängigkeit
* Fail Fast
* Modularität
* deterministischen Speichergrenzen

---

# 7. Architekturübersicht

```text
         Hohe Adressen
 ┌───────────────────────────────┐
 │ Guard Page                    │
 ├───────────────────────────────┤
 │ Reserved Expansion Area       │
 ├───────────────────────────────┤
 │ Exception / Interrupt Frames  │
 ├───────────────────────────────┤
 │ Scheduler Context             │
 ├───────────────────────────────┤
 │ Funktionsaufrufe              │
 │ Lokale Variablen              │
 │ Rücksprungadressen            │
 ├───────────────────────────────┤
 │ Thread Control Block Pointer  │
 ├───────────────────────────────┤
 │ Guard Page                    │
 └───────────────────────────────┘
          Niedrige Adressen
```

---

# 8. Stackgröße

Die Kernel-Stackgröße ist architekturabhängig.

Sie muss:

* seitenbasiert sein
* ausrichtbar sein
* Guard Pages berücksichtigen
* konfigurierbar sein

Standardgrößen werden vom jeweiligen Architekturport festgelegt.

---

# 9. Stackausrichtung

Alle Kernel-Stacks müssen:

* natürlich ausgerichtet sein
* SIMD-Anforderungen erfüllen
* ABI-konform sein
* Cachefreundlich angeordnet werden

---

# 10. Guard Pages

Jeder Kernel-Stack besitzt:

* eine untere Guard Page
* eine obere Guard Page

Ein Zugriff außerhalb des gültigen Bereichs löst unmittelbar einen Page Fault aus. Führende und nachfolgende Guard Pages ermöglichen die sofortige Erkennung von Kernel-Stack-Überläufen.

---

# 11. Thread Control Block

Am unteren Stackbereich befindet sich die Referenz auf:

* Thread Control Block
* Schedulerinformationen
* Thread-ID
* CPU-Zuordnung

Dadurch kann der Kernel jederzeit den aktuellen Thread bestimmen.

---

# 12. Laufzeitbereich

Der größte Teil des Stacks dient:

* lokalen Variablen
* Funktionsparametern
* Rücksprungadressen
* Compilerinformationen

Dieser Bereich wächst dynamisch während der Ausführung.

---

# 13. Scheduler Context

Für Kontextwechsel reserviert NovaOS einen eigenen Bereich.

Er enthält:

* Registerabbild
* Stack Pointer
* Program Counter
* Statusregister
* Architekturregister

---

# 14. Interrupt Frames

Bei Interrupts werden automatisch gespeichert:

* CPU Register
* Flags
* Rücksprungadresse
* Fehlercode
* Interruptnummer

Architekturabhängige Erweiterungen sind zulässig.

---

# 15. Exception Frames

Exception Frames enthalten zusätzlich:

* Exceptiontyp
* Fault-Adresse
* Fehlercode
* CPU-Kontext
* Diagnoseinformationen

---

# 16. Erweiterungsbereich

Ein reservierter Bereich ermöglicht spätere Integration von:

* Shadow Stacks
* CET
* GCS
* Stack Tracing
* KI-Diagnose

Ohne Änderung des grundsätzlichen Layouts.

---

# 17. Virtuell gemappte Stacks

Kernel-Stacks werden grundsätzlich virtuell gemappt.

Vorteile:

* Guard Pages
* flexible Platzierung
* bessere Isolation
* einfachere Diagnose

Virtuell gemappte Kernel-Stacks erleichtern Guard Pages, schnellere Freigabe und bessere Diagnose von Stackfehlern.

---

# 18. Stack Overflow

Bei einem Stack Overflow:

* wird ein Page Fault ausgelöst,
* der Exception Manager aktiviert,
* der Fehler protokolliert,
* ein Kernel Panic oder Recovery eingeleitet.

---

# 19. Stack Switching

NovaOS unterstützt Stackwechsel bei:

* Systemaufrufen
* Interrupts
* Exceptions
* CPU-Wechsel
* Schedulerwechsel

Jeder Wechsel erfolgt kontrolliert.

---

# 20. Mehrkernbetrieb

Das Layout unterstützt:

* SMP
* CPU Hotplug
* Per-CPU-Stacks
* parallele Interrupts
* atomare Kontextwechsel

---

# 21. Sicherheit

Das Kernel Stack Layout schützt vor:

* Stack Overflow
* Stack Underflow
* Speicherkorruption
* fehlerhaften Rücksprüngen
* unautorisierten Speicherzugriffen

---

# 22. Performance

Die Architektur soll ermöglichen:

* kurze Kontextwechsel
* geringe Cache-Misses
* schnelle Interruptverarbeitung
* effiziente Stacknutzung
* hohe Skalierbarkeit

---

# 23. Debug-Unterstützung

Optional unterstützt NovaOS:

* Stack-Dumps
* Stack-Backtraces
* Stack-Heatmaps
* Rekursionserkennung
* Stackverbrauchsanalyse
* Overflow-Diagnosen

---

# 24. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* Shadow Stack
* Intel CET
* ARM Guarded Control Stack
* Hardware Stack Protection
* Memory Tagging
* KI-gestützte Stackanalyse

---

# 25. Auswirkungen

Positive Auswirkungen:

* konsistenter Kernelaufbau
* höhere Sicherheit
* einfache Analyse
* bessere Wartbarkeit
* moderne Architektur
* gute Skalierbarkeit

Negative Auswirkungen:

* leicht höherer virtueller Speicherbedarf
* zusätzlicher Verwaltungsaufwand
* höhere Implementierungskomplexität

---

# 26. Verworfene Alternativen

## Gemeinsamer Kernel-Stack

Vorteile:

* geringer Speicherverbrauch

Nachteile:

* keine Parallelität
* hohe Fehleranfälligkeit
* ungeeignet für SMP

---

## Physisch zusammenhängende Kernel-Stacks

Vorteile:

* einfache Allokation

Nachteile:

* keine Guard Pages
* schwierigere Diagnose
* höhere Fragmentierung

---

## Variable Stacklayouts

Vorteile:

* flexible Implementierung

Nachteile:

* schwierige Wartung
* geringe Portierbarkeit
* komplizierte Debugging-Werkzeuge

---

# 27. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* ausschließlich den standardisierten Stackaufbau verwenden,
* Guard Pages aktivieren,
* keine Daten außerhalb des gültigen Stackbereichs speichern,
* Architekturinformationen ausschließlich in reservierten Bereichen ablegen,
* Stackwechsel ausschließlich über Kernelmechanismen durchführen.

---

# 28. Teststrategie

Das Kernel Stack Layout wird geprüft durch:

* Unit-Tests
* Stack-Overflow-Tests
* Interrupttests
* Exceptiontests
* Scheduler-Tests
* Mehrkerntests
* Langzeittests
* Performancebenchmarks

---

# 29. Konsequenzen

Das Kernel Stack Layout bildet die verbindliche Grundlage sämtlicher Kernel-Ausführungskontexte in NovaOS.

Alle Threads, Interrupts und Exceptions verwenden dieselbe definierte Struktur. Dadurch entstehen konsistente Speicherlayouts, reproduzierbare Fehleranalysen sowie eine robuste Basis für zukünftige Sicherheitsmechanismen wie Shadow Stacks oder hardwaregestützte Kontrollflussüberwachung.

---

# 30. Zukunftsperspektive

Langfristig soll das Kernel Stack Layout erweitert werden um:

* adaptive Stackgrößen
* dynamische Stackerweiterung
* automatische Stackverbrauchsanalyse
* hardwaregestützte Stackintegrität
* KI-gestützte Overflow-Prognosen
* vollständige Integration zukünftiger CPU-Sicherheitsmechanismen

---

# 31. Zusammenfassung

NovaOS implementiert ein einheitliches **Kernel Stack Layout** mit virtuell gemappten Stacks, Guard Pages, standardisierten Scheduler-, Interrupt- und Exception-Frames sowie klar definierten Erweiterungsbereichen. Diese Architektur erhöht die Sicherheit, verbessert die Debugbarkeit und schafft eine stabile Grundlage für zukünftige Technologien wie Shadow Stacks, Intel CET oder ARM Guarded Control Stack. Virtuell gemappte Kernel-Stacks mit Guard Pages gelten als bewährte Architektur zur sofortigen Erkennung von Stacküberläufen und zur Vermeidung schwer diagnostizierbarer Speicherkorruption.
