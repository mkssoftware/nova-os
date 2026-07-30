# ADR-KERNEL-0008: Physical Memory Manager

* **ADR-ID:** ADR-KERNEL-0008
* **Titel:** Physical Memory Manager
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel-Architektur
* **Gültigkeitsbereich:** Memory Manager, HAL, Virtual Memory Manager, Kernel Heap
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0002, ADR-KERNEL-0006, ADR-KERNEL-0007, NPSPEC-KERNEL-0006

---

# 1. Zusammenfassung

NovaOS verwendet einen **zentralen Physical Memory Manager (PMM)** zur Verwaltung des gesamten physischen Arbeitsspeichers.

Der PMM verwaltet physische Speicherseiten (Page Frames), stellt diese anderen Kernelkomponenten zur Verfügung und übernimmt deren Freigabe. Er bildet die unterste Ebene der Speicherverwaltung und dient als Grundlage für den Virtual Memory Manager, den Kernel Heap sowie sämtliche Seitentabellen. Für die Verwaltung physischer Seiten wird ein Buddy-Allocator als Kernalgorithmus eingesetzt, da er schnelle Allokationen zusammenhängender Speicherbereiche und kontrollierte Fragmentierung ermöglicht.

---

# 2. Kontext

Der Physical Memory Manager ist unmittelbar nach Abschluss des Bootvorgangs aktiv.

Er verwaltet:

* gesamten verfügbaren RAM
* reservierte Speicherbereiche
* Kernelabbild
* Bootstrukturen
* ACPI-Speicher
* DMA-Bereiche
* Gerätespeicher
* Seitentabellen

---

# 3. Problemstellung

Es musste entschieden werden,

* wie physischer Speicher verwaltet wird,
* wie freie Seiten gefunden werden,
* wie zusammenhängende Speicherbereiche bereitgestellt werden,
* wie Fragmentierung minimiert wird,
* und wie zukünftige Hardware unterstützt wird.

---

# 4. Entscheidung

NovaOS implementiert einen zentralen Physical Memory Manager.

Dabei gilt:

* Verwaltung ausschließlich physischer Seiten
* keine Kenntnis virtueller Adressen
* Bereitstellung zusammenhängender Speicherbereiche
* vollständige Trennung vom Virtual Memory Manager
* zentrale Verwaltung sämtlicher Page Frames

---

# 5. Motivation

Diese Architektur bietet:

* hohe Performance
* geringe Fragmentierung
* klare Trennung der Verantwortlichkeiten
* gute Skalierbarkeit
* Mehrkernunterstützung
* Unterstützung großer Speichermengen

---

# 6. Designprinzipien

Der Physical Memory Manager basiert auf:

* Seitenorientierung
* deterministischen Algorithmen
* Modularität
* Architekturunabhängigkeit
* Erweiterbarkeit
* hoher Parallelität

---

# 7. Architekturübersicht

```text
                   Bootloader
                        │
                        ▼
               Memory Map (UEFI/BIOS)
                        │
                        ▼
          Physical Memory Manager (PMM)
                        │
        ┌───────────────┼────────────────┐
        ▼               ▼                ▼
 Virtual Memory     Kernel Heap     DMA Manager
     Manager
```

---

# 8. Verantwortungsbereich

Der PMM verwaltet ausschließlich:

* physische Seiten
* freie Seiten
* reservierte Seiten
* belegte Seiten
* Speicherzonen
* Seitengruppen

Virtuelle Speicherabbildungen gehören nicht zum Aufgabenbereich.

---

# 9. Speichereinheit

Die kleinste verwaltete Einheit ist eine physische Speicherseite.

Standardgröße:

* 4 KiB

Weitere Seitengrößen können zusätzlich unterstützt werden.

---

# 10. Speichererkennung

Beim Systemstart übernimmt der PMM die Speicherkarte (Memory Map) vom Bootloader.

Dabei werden erkannt:

* freier RAM
* reservierter RAM
* Firmwarebereiche
* MMIO
* ACPI
* Kernelbereiche

---

# 11. Speicherreservierung

Vor der Initialisierung werden dauerhaft reserviert:

* Kernel
* Bootinformationen
* Seitentabellen
* HAL
* Initiale Kernel-Stacks
* PMM-Metadaten

Diese Bereiche dürfen niemals erneut vergeben werden.

---

# 12. Buddy-Allocator

Der PMM verwendet als primären Allokationsalgorithmus einen **Buddy-Allocator**.

Speicher wird dabei in Blöcken verwaltet, deren Größen Zweierpotenzen entsprechen.

Freigegebene benachbarte Blöcke werden automatisch wieder zusammengeführt. Dieses Verfahren bietet schnelle Allokation und reduziert externe Fragmentierung.

---

# 13. Seitenzustände

Jede Seite besitzt einen definierten Zustand.

Beispiele:

* FREE
* RESERVED
* USED
* KERNEL
* DMA
* MMIO
* BAD
* SHARED

---

# 14. Seiteninformationen

Für jede physische Seite werden mindestens gespeichert:

* Status
* Referenzzähler
* Eigentümer
* Flags
* Knotenzugehörigkeit (NUMA)
* Zonenzugehörigkeit

---

# 15. Speicherzonen

Der PMM unterstützt mehrere Speicherzonen.

Beispiele:

* DMA
* NORMAL
* HIGH MEMORY
* PERSISTENT MEMORY

Die konkrete Zoneneinteilung ist architekturabhängig.

---

# 16. Zusammenhängender Speicher

Der PMM kann mehrere zusammenhängende Seiten bereitstellen.

Dies wird benötigt für:

* DMA
* große Puffer
* Seitentabellen
* Huge Pages

---

# 17. NUMA-Unterstützung

Die Architektur unterstützt zukünftige NUMA-Systeme.

Seiten sollen bevorzugt aus dem lokalen Speicherknoten allokiert werden.

---

# 18. Referenzzählung

Gemeinsam genutzte physische Seiten verwenden Referenzzähler.

Eine Seite wird erst freigegeben, wenn der Referenzzähler Null erreicht.

---

# 19. Speicherfreigabe

Freigegebene Seiten werden:

* validiert,
* zusammengeführt,
* in die freien Listen zurückgeführt,
* für zukünftige Allokationen bereitgestellt.

---

# 20. Fragmentierung

Der PMM reduziert Fragmentierung durch:

* Buddy-Zusammenführung
* geeignete Zonenauswahl
* effiziente Allokationsstrategien

---

# 21. Synchronisation

Der PMM arbeitet vollständig thread-sicher.

Je nach Architektur können verwendet werden:

* Spinlocks
* atomare Operationen
* CPU-lokale Freilisten

---

# 22. Fehlerbehandlung

Fehlerhafte Speicherbereiche werden dauerhaft markiert.

Diese Seiten dürfen niemals allokiert werden.

---

# 23. Performance

Der PMM soll:

* konstante Allokationszeiten,
* geringe Sperrzeiten,
* hohe Parallelität,
* schnelle Freigaben

erreichen.

---

# 24. Sicherheit

Der PMM verhindert:

* doppelte Freigaben
* ungültige Allokationen
* Nutzung reservierter Seiten
* Speicherkorruption

---

# 25. Debug-Unterstützung

Optional unterstützt der PMM:

* Seitenstatistiken
* Leak Detection
* Referenzanalyse
* Fragmentierungsanalyse
* Speicherkarten-Dump

---

# 26. Mehrarchitektur-Unterstützung

Der PMM unterstützt:

* x86
* x86_64
* ARM64
* RISC-V

Architekturspezifische Unterschiede werden ausschließlich innerhalb der HAL behandelt.

---

# 27. Erweiterbarkeit

Die Architektur ermöglicht zukünftige Erweiterungen:

* Memory Hotplug
* Memory Hot Remove
* Verschlüsselter RAM
* Persistenter Speicher
* Tiered Memory
* CXL-Speicher

---

# 28. Auswirkungen

Positive Auswirkungen:

* hohe Performance
* geringe Fragmentierung
* klare Trennung der Speicherverwaltung
* gute Skalierbarkeit
* effiziente Mehrkernunterstützung

Negative Auswirkungen:

* komplexere Implementierung
* zusätzlicher Verwaltungsaufwand
* Metadaten benötigen Speicher

---

# 29. Verworfene Alternativen

## Bitmap-Allocator

Vorteile:

* einfache Implementierung
* geringer Speicherbedarf

Nachteile:

* langsame Suche nach zusammenhängenden Speicherbereichen
* schlechtere Skalierung bei großen RAM-Konfigurationen

---

## Stack-basierter Page Allocator

Vorteile:

* sehr schnelle Einzelseitenallokation

Nachteile:

* ungeeignet für größere zusammenhängende Speicherbereiche
* schwierige Fragmentierungsverwaltung

---

## Feste Speicherlisten

Vorteile:

* einfache Implementierung

Nachteile:

* geringe Flexibilität
* schlechte Anpassbarkeit an unterschiedliche Speichergrößen

---

# 30. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* physische Seiten ausschließlich über den PMM anfordern,
* Seiten nach der Nutzung freigeben,
* keine eigenen physischen Speicherverwalter implementieren,
* Referenzzählung beachten,
* reservierte Bereiche respektieren.

---

# 31. Teststrategie

Der PMM wird validiert durch:

* Unit-Tests
* Fragmentierungstests
* Lasttests
* Parallelitätstests
* NUMA-Tests
* Langzeittests
* Performancebenchmarks

---

# 32. Konsequenzen

Der Physical Memory Manager bildet die Grundlage sämtlicher Speicherverwaltung innerhalb von NovaOS.

Alle zukünftigen Komponenten des Virtual Memory Managers, Kernel Heaps, Device Managers und der Seitentabellenverwaltung müssen ihre physischen Seiten ausschließlich über den PMM beziehen. Änderungen an dieser Architektur würden weitreichende Auswirkungen auf die gesamte Speicherverwaltung des Systems haben.

---

# 33. Zusammenfassung

NovaOS verwendet einen zentralen **Physical Memory Manager**, der den gesamten physischen Arbeitsspeicher verwaltet und als unterste Schicht der Speicherarchitektur dient. Durch den Einsatz eines Buddy-Allocators, die Verwaltung von Speicherzonen, Referenzzählung und Unterstützung für Mehrkern- sowie zukünftige NUMA-Systeme entsteht eine leistungsfähige, skalierbare und robuste Grundlage für sämtliche Speicheroperationen innerhalb des NovaOS-Kernels.
