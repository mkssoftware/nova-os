# ADR-MM-0013: Guard Pages

* **ADR-ID:** ADR-MM-0013
* **Titel:** Guard Pages
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Memory Management
* **Gültigkeitsbereich:** Virtual Memory Manager (VMM), Process Manager, Thread Manager, Loader, Memory Allocator, Kernel
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-MM-0004, ADR-MM-0006, ADR-MM-0008, ADR-KERNEL-0009

---

# 1. Zusammenfassung

NovaOS implementiert **Guard Pages** als grundlegenden Sicherheitsmechanismus der virtuellen Speicherverwaltung.

Guard Pages sind bewusst nicht nutzbare Speicherseiten, die zwischen kritischen Speicherbereichen platziert werden. Jeder Zugriff auf eine Guard Page löst unmittelbar einen Page Fault aus und verhindert dadurch Speicherüberläufe, Stack Overflows, Heap-Korruptionen oder Zugriffe außerhalb zulässiger Speichergrenzen. Guard Pages werden in modernen Betriebssystemen unter anderem zur Erkennung von Stack-Überläufen eingesetzt und lösen beim Zugriff gezielt einen Speicherzugriffsfehler aus.

---

# 2. Kontext

Virtuelle Speicherbereiche grenzen häufig direkt aneinander.

Beispiele:

* Thread Stacks
* Kernel Stacks
* Heap
* Shared Memory
* Memory Mappings
* DMA-Puffer
* Kernelobjekte

Ein Überlauf könnte benachbarte Speicherbereiche beschädigen.

---

# 3. Problemstellung

Es musste entschieden werden,

* wo Guard Pages eingesetzt werden,
* welche Zugriffsrechte gelten,
* wie Zugriffe erkannt werden,
* wie Fehler behandelt werden,
* und wie sich Guard Pages in den Virtual Memory Manager integrieren.

---

# 4. Entscheidung

NovaOS verwendet Guard Pages systemweit.

Dabei gilt:

* Guard Pages besitzen niemals gültige Nutzdaten.
* Sie sind grundsätzlich nicht zugreifbar.
* Jeder Zugriff erzeugt einen Page Fault.
* Guard Pages werden automatisch durch den Virtual Memory Manager verwaltet.

---

# 5. Motivation

Diese Architektur bietet:

* frühzeitige Fehlererkennung
* Schutz vor Stack Overflow
* Schutz vor Heap Overflow
* höhere Systemsicherheit
* einfachere Fehlersuche
* bessere Stabilität

---

# 6. Designprinzipien

Guard Pages basieren auf:

* Fail Fast
* Speicherisolation
* Transparenz
* Hardwareunterstützung
* Modularität
* deterministische Fehlerbehandlung

---

# 7. Architekturübersicht

```text
        Virtueller Speicher

 ┌──────────────┐
 │ Thread Stack │
 └──────────────┘

 ┌──────────────┐
 │ Guard Page   │
 └──────────────┘

 ┌──────────────┐
 │ Heap         │
 └──────────────┘

 ┌──────────────┐
 │ Guard Page   │
 └──────────────┘

 ┌──────────────┐
 │ Shared Mem   │
 └──────────────┘
```

---

# 8. Eigenschaften

Eine Guard Page besitzt:

* virtuelle Adresse
* Seitengröße
* No-Access-Attribut
* Regionenzugehörigkeit
* Typ
* Status

Sie enthält niemals Nutzdaten.

---

# 9. Platzierung

Guard Pages können verwendet werden zwischen:

* Thread Stack und Heap
* Kernel Stack und Kernel Heap
* Memory Mappings
* Shared Memory Bereichen
* JIT-Code
* sensiblen Kernelobjekten

---

# 10. Stack Guard Pages

Jeder Thread erhält mindestens eine Guard Page.

Sie befindet sich

am Ende des Stackbereichs in Wachstumsrichtung.

Dadurch wird ein Stack Overflow sofort erkannt. Virtuell gemappte Kernel-Stacks mit Guard Pages erkennen Stack-Überläufe unmittelbar und verhindern schwer nachvollziehbare Speicherkorruption.

---

# 11. Kernel Guard Pages

Auch Kernel Stacks besitzen Guard Pages.

Dies schützt insbesondere vor:

* Kernel Stack Overflow
* Rekursion
* Treiberfehlern
* Speicherkorruption

---

# 12. Heap Guard Pages

Optional können Guard Pages

zwischen Heap-Blöcken

oder am Ende großer Allokationen eingefügt werden.

Dies erleichtert Debugging und erhöht die Sicherheit.

---

# 13. Guard Regions

Mehrere Guard Pages können

zu einer Guard Region zusammengefasst werden.

Dies eignet sich insbesondere für:

* MMIO
* Sicherheitsbereiche
* reservierte Adressräume
* experimentelle Funktionen

---

# 14. Zugriffsrechte

Guard Pages besitzen grundsätzlich:

* No Read
* No Write
* No Execute

Jeder Zugriff erzeugt einen Ausnahmezustand.

---

# 15. Page Fault Integration

Greift ein Prozess auf eine Guard Page zu,

erkennt der Page Fault Handler:

* Guard Page
* Zugriffstyp
* Prozess
* Thread
* Adresse

Anschließend wird die definierte Fehlerbehandlung gestartet.

---

# 16. Stackwachstum

NovaOS unterstützt optional dynamisches Stackwachstum.

In diesem Fall kann

der Zugriff auf die erste Guard Page

zum kontrollierten Erweitern des Stacks verwendet werden,

anschließend wird automatisch eine neue Guard Page angelegt. Einige Betriebssysteme verwenden Guard Pages als Auslöser für automatisches Stackwachstum und verschieben danach die Guard Page an das neue Stackende.

---

# 17. Virtual Memory Regions

Guard Pages werden

als eigene Virtual Memory Regions

verwaltet.

Sie besitzen:

* eigene Attribute
* eigene Metadaten
* eigene Rechte

---

# 18. Memory Mapping

Memory Mappings können

optional Guard Pages

an ihren Grenzen erhalten.

Dadurch werden fehlerhafte Bereichsüberschreitungen früh erkannt.

---

# 19. Shared Memory

Shared Memory unterstützt Guard Pages

an den Grenzen gemeinsam genutzter Bereiche,

um fehlerhafte Zugriffe sofort zu erkennen.

---

# 20. Mehrkernbetrieb

Guard Pages unterstützen:

* SMP
* CPU Hotplug
* atomare Seitentabellenänderungen
* parallele Page Faults

---

# 21. Sicherheit

Guard Pages schützen vor:

* Stack Overflow
* Heap Overflow
* Buffer Overflow
* Out-of-Bounds-Zugriffen
* Speicherkorruption
* versehentlichen Überschreibungen

---

# 22. Performance

Die Architektur soll ermöglichen:

* minimalen Speicheraufwand
* schnelle Fehlererkennung
* geringe Laufzeitkosten
* einfache Verwaltung
* hohe Skalierbarkeit

Da Guard Pages nur bei einem fehlerhaften Zugriff einen Page Fault auslösen, entsteht im Normalbetrieb praktisch kein Laufzeitaufwand.

---

# 23. Debug-Unterstützung

Optional unterstützt NovaOS:

* Guard-Page-Statistiken
* Speicherkarten
* Fault-Historie
* Stackanalysen
* Heapanalysen
* Overflow-Diagnosen

---

# 24. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* Hardware Memory Tagging
* Shadow Stacks
* Stack Clash Protection
* adaptive Guard Regions
* KI-gestützte Speicheranalyse
* erweiterte Debug-Modi

---

# 25. Auswirkungen

Positive Auswirkungen:

* höhere Systemsicherheit
* frühzeitige Fehlererkennung
* einfachere Fehlersuche
* robuste Speicherverwaltung
* geringeres Risiko von Speicherkorruption
* moderne Sicherheitsarchitektur

Negative Auswirkungen:

* geringfügig höherer virtueller Adressraumbedarf
* zusätzlicher Verwaltungsaufwand
* etwas mehr Seitentabellenverwaltung

---

# 26. Verworfene Alternativen

## Keine Guard Pages

Vorteile:

* minimale Implementierung

Nachteile:

* Speicherfehler werden häufig zu spät erkannt
* höhere Korruptionsgefahr

---

## Ausschließlich Compiler-Schutz

Vorteile:

* geringer Kernelaufwand

Nachteile:

* schützt nicht alle Speicherbereiche
* abhängig vom Compiler

---

## Ausschließlich Laufzeitprüfungen

Vorteile:

* flexible Diagnose

Nachteile:

* hoher Laufzeitaufwand
* geringere Zuverlässigkeit

---

# 27. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* Guard Pages ausschließlich über den Virtual Memory Manager erzeugen,
* Guard Pages niemals für Nutzdaten verwenden,
* Zugriffe ausschließlich über den Page Fault Handler behandeln,
* keine direkten Änderungen an Guard Pages durchführen,
* sämtliche Metadaten atomar aktualisieren.

---

# 28. Teststrategie

Guard Pages werden geprüft durch:

* Unit-Tests
* Stack-Overflow-Tests
* Heap-Overflow-Tests
* Kernel-Stack-Tests
* Mehrkerntests
* Page-Fault-Tests
* Langzeittests
* Performancebenchmarks

---

# 29. Konsequenzen

Guard Pages werden Bestandteil sämtlicher sicherheitskritischer Speicherbereiche von NovaOS.

Thread Stacks, Kernel Stacks, Heap-Bereiche, Shared Memory und Memory Mappings können dadurch gegen Überläufe und Bereichsverletzungen geschützt werden. Die Integration in den Virtual Memory Manager ermöglicht eine einheitliche Verwaltung aller Schutzseiten innerhalb des Betriebssystems.

---

# 30. Zukunftsperspektive

Langfristig soll das Guard-Page-System erweitert werden um:

* adaptive Platzierung abhängig vom Sicherheitsprofil
* automatische Stack-Clash-Erkennung
* Integration mit Shadow Stacks
* Hardware Memory Tagging
* KI-gestützte Analyse wiederkehrender Speicherfehler
* sicherheitsabhängige Guard-Region-Größen

---

# 31. Zusammenfassung

NovaOS implementiert **Guard Pages** als zentralen Schutzmechanismus der virtuellen Speicherverwaltung. Nicht zugreifbare Speicherseiten werden gezielt zwischen kritischen Speicherbereichen platziert, um Stack-Überläufe, Heap-Korruptionen und Bereichsverletzungen unmittelbar zu erkennen. Durch die vollständige Integration in Virtual Memory Regions, den Virtual Memory Manager und den Page Fault Handler entsteht eine moderne, performante und sicherheitsorientierte Speicherarchitektur. Moderne Betriebssysteme setzen Guard Pages insbesondere zum Schutz von Thread- und Kernel-Stacks ein, um Speicherfehler unmittelbar zu erkennen und eine Ausbreitung von Korruption zu verhindern.
