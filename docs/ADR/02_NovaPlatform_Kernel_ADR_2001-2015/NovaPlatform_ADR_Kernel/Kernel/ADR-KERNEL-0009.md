# ADR-KERNEL-0009: Virtual Memory Manager

* **ADR-ID:** ADR-KERNEL-0009
* **Titel:** Virtual Memory Manager
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel-Architektur
* **Gültigkeitsbereich:** Memory Manager, Process Manager, Physical Memory Manager, Kernel Heap
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0006, ADR-KERNEL-0008, NPSPEC-KERNEL-0007

---

# 1. Zusammenfassung

NovaOS verwendet einen **zentralen Virtual Memory Manager (VMM)**, der sämtliche virtuellen Adressräume des Systems verwaltet.

Der Virtual Memory Manager bildet die Verbindung zwischen den virtuellen Adressräumen der Prozesse und den vom Physical Memory Manager bereitgestellten physischen Speicherseiten. Er verwaltet Speicherabbildungen, Seitentabellen, Zugriffsrechte, Memory Mappings, Shared Memory und Copy-on-Write und arbeitet dabei eng mit der MMU der jeweiligen Prozessorarchitektur zusammen.

---

# 2. Kontext

Der Virtual Memory Manager ist die zentrale Instanz für sämtliche virtuellen Speicheroperationen.

Er verwaltet unter anderem:

* Prozessadressräume
* Kernel Address Space
* Seitentabellen
* Shared Memory
* Memory Mappings
* Copy-on-Write
* Demand Paging
* Speicherschutz

---

# 3. Problemstellung

Es musste entschieden werden,

* wie virtuelle Speicherbereiche verwaltet werden,
* wie virtuelle Adressen auf physische Seiten abgebildet werden,
* wie Zugriffsrechte umgesetzt werden,
* wie Prozesse voneinander isoliert werden,
* und wie moderne Speichertechniken unterstützt werden.

---

# 4. Entscheidung

NovaOS implementiert einen eigenständigen Virtual Memory Manager.

Dabei gilt:

* sämtliche virtuellen Speicherbereiche werden zentral verwaltet,
* jeder Prozess besitzt einen eigenen virtuellen Adressraum,
* physische Seiten werden ausschließlich über den Physical Memory Manager bezogen,
* sämtliche Speicherabbildungen erfolgen über Seitentabellen.

---

# 5. Motivation

Diese Architektur bietet:

* vollständige Prozessisolierung
* flexible Speicherverwaltung
* hohe Skalierbarkeit
* moderne Speichertechniken
* klare Trennung der Verantwortlichkeiten
* hohe Sicherheit

---

# 6. Designprinzipien

Der Virtual Memory Manager basiert auf:

* virtuellen Adressräumen
* Seitenverwaltung
* Speicherschutz
* Modularität
* Architekturunabhängigkeit
* Erweiterbarkeit
* deterministischem Verhalten

---

# 7. Architekturübersicht

```text
                    Anwendungen
                         │
                         ▼
               Virtual Address Space
                         │
                         ▼
              Virtual Memory Manager
                         │
                         ▼
           Seitentabellen (Page Tables)
                         │
                         ▼
           Physical Memory Manager (PMM)
                         │
                         ▼
                 Physischer Speicher
```

---

# 8. Verantwortungsbereich

Der Virtual Memory Manager verwaltet ausschließlich:

* virtuelle Speicherbereiche
* Seitentabellen
* Adressabbildungen
* Speicherattribute
* Shared Memory
* Memory Mappings

Nicht Bestandteil sind:

* physische Seitenverwaltung
* Kernel Heap
* DMA-Speicherverwaltung

---

# 9. Virtuelle Speicherbereiche

Der VMM verwaltet mindestens:

* Programmcode
* Konstanten
* Datensegment
* Heap
* Thread Stacks
* Shared Memory
* Memory Mapped Files
* Geräteabbildungen

---

# 10. Seitentabellen

Der VMM erzeugt und verwaltet sämtliche Seitentabellen.

Er ist verantwortlich für:

* Anlegen
* Aktualisieren
* Freigeben
* Synchronisieren

der Seitentabellen.

---

# 11. Speicherabbildungen

Virtuelle Seiten können abgebildet werden auf:

* physische RAM-Seiten
* gemeinsam genutzte Seiten
* Dateien
* Geräte
* persistente Speicherbereiche

---

# 12. Zugriffsrechte

Jede virtuelle Seite besitzt definierte Attribute.

Beispiele:

* Read
* Write
* Execute
* User
* Kernel
* Shared
* Copy-on-Write
* Cache Policy

---

# 13. Demand Paging

Speicherseiten werden erst dann physisch bereitgestellt, wenn sie tatsächlich benötigt werden.

Dies reduziert den Speicherverbrauch erheblich.

---

# 14. Copy-on-Write

Der VMM unterstützt Copy-on-Write.

Gemeinsam genutzte Seiten werden erst beim ersten Schreibzugriff dupliziert.

Dadurch werden Speicherbedarf und Prozessstarts optimiert.

---

# 15. Shared Memory

Mehrere Prozesse können definierte Speicherbereiche gemeinsam verwenden.

Die Verwaltung erfolgt vollständig durch den Virtual Memory Manager.

---

# 16. Memory Mapping

Dateien können direkt in den virtuellen Adressraum eingebunden werden.

Dies ermöglicht:

* effizienten Dateizugriff
* große Datenmengen
* gemeinsame Bibliotheken
* Memory Mapped I/O

---

# 17. Page Fault Handling

Greift ein Prozess auf eine nicht vorhandene oder nicht erlaubte Seite zu, erzeugt die CPU einen **Page Fault**.

Der Virtual Memory Manager entscheidet anschließend über:

* Speicherzuweisung
* Nachladen
* Copy-on-Write
* Ausnahmebehandlung
* Prozessbeendigung

---

# 18. Translation Lookaside Buffer (TLB)

Nach Änderungen an Seitentabellen sorgt der VMM für die erforderliche Aktualisierung oder Invalidierung des Translation Lookaside Buffer (TLB), damit Adressübersetzungen konsistent bleiben.

---

# 19. Prozesswechsel

Beim Wechsel zwischen Prozessen aktiviert der Scheduler den jeweils zugehörigen virtuellen Adressraum.

---

# 20. Kernel Address Space

Der Kernel Address Space bleibt für alle Prozesse identisch.

Nur der User Address Space wird beim Prozesswechsel ausgetauscht.

---

# 21. Huge Pages

Der VMM unterstützt optional große Speicherseiten.

Diese können insbesondere für:

* Datenbanken
* wissenschaftliche Berechnungen
* KI-Anwendungen
* große Speicherpuffer

verwendet werden.

---

# 22. NUMA-Unterstützung

Der VMM unterstützt zukünftige NUMA-Systeme.

Speicherabbildungen sollen bevorzugt lokale Speicherknoten verwenden.

---

# 23. Mehrarchitektur-Unterstützung

Der Virtual Memory Manager unterstützt:

* x86
* x86_64
* ARM64
* RISC-V

Architekturspezifische Seitentabellenformate werden ausschließlich innerhalb der HAL implementiert.

---

# 24. Synchronisation

Der Virtual Memory Manager arbeitet vollständig thread-sicher.

Je nach Komponente kommen zum Einsatz:

* Spinlocks
* Reader-Writer-Locks
* atomare Operationen
* CPU-lokale Datenstrukturen

---

# 25. Performance

Der VMM soll:

* geringe Page-Fault-Latenzen
* schnelle Seitentabellenoperationen
* minimale TLB-Invalidierungen
* hohe Parallelität

erreichen.

---

# 26. Sicherheit

Der Virtual Memory Manager gewährleistet:

* vollständige Prozessisolierung
* Kernel/User-Trennung
* Speicherschutz
* NX-Unterstützung
* kontrollierte Shared-Memory-Bereiche

---

# 27. Debug-Unterstützung

Optional unterstützt der VMM:

* Seitentabellenanalyse
* Memory Maps
* Page-Fault-Logging
* Leak Detection
* Speicherstatistiken

---

# 28. Erweiterbarkeit

Die Architektur ermöglicht zukünftige Erweiterungen:

* Memory Compression
* Verschlüsselter Speicher
* Tiered Memory
* CXL Memory
* Transparent Huge Pages
* IOMMU-Integration

---

# 29. Auswirkungen

Positive Auswirkungen:

* vollständige Speicherisolation
* flexible Speicherverwaltung
* hohe Skalierbarkeit
* moderne Speichertechniken
* effiziente Prozessverwaltung

Negative Auswirkungen:

* höhere Implementierungskomplexität
* Verwaltungsaufwand für Seitentabellen
* zusätzlicher Speicherbedarf für Metadaten

---

# 30. Verworfene Alternativen

## Direkte physische Speicheradressierung

Vorteile:

* einfache Implementierung

Nachteile:

* keine Isolation
* keine Speichervirtualisierung
* ungeeignet für Mehrprozessbetrieb

---

## Segmentbasierte Speicherverwaltung

Vorteile:

* logische Speicherbereiche

Nachteile:

* höhere Komplexität
* geringere Flexibilität
* moderne Prozessoren verwenden überwiegend Paging

---

## Prozessweiter gemeinsamer Adressraum

Vorteile:

* einfache Kommunikation

Nachteile:

* geringe Sicherheit
* keine Isolation
* hohe Fehleranfälligkeit

---

# 31. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* virtuelle Speicherbereiche ausschließlich über den VMM verwalten,
* physische Seiten ausschließlich über den PMM beziehen,
* Seitentabellen nicht direkt verändern,
* Speicherattribute konsequent berücksichtigen,
* Prozessisolierung jederzeit gewährleisten.

---

# 32. Teststrategie

Der Virtual Memory Manager wird geprüft durch:

* Unit-Tests
* Paging-Tests
* Shared-Memory-Tests
* Copy-on-Write-Tests
* Page-Fault-Tests
* Performancebenchmarks
* Langzeittests
* Stresstests

---

# 33. Konsequenzen

Der Virtual Memory Manager bildet die zentrale Steuerinstanz der virtuellen Speicherverwaltung in NovaOS.

Alle zukünftigen Kernelkomponenten, die virtuelle Speicherbereiche erzeugen, verändern oder freigeben, müssen ausschließlich über den VMM arbeiten. Änderungen an dieser Architektur hätten unmittelbare Auswirkungen auf Prozessverwaltung, Speicherverwaltung, Dateisysteme und den Scheduler.

---

# 34. Zusammenfassung

NovaOS verwendet einen **zentralen Virtual Memory Manager**, der sämtliche virtuellen Adressräume des Systems verwaltet und die Übersetzung virtueller Adressen in physische Speicherseiten steuert. Durch die Unterstützung von Demand Paging, Copy-on-Write, Shared Memory, Memory Mapping und einem umfassenden Speicherschutz bildet der VMM eine skalierbare, sichere und leistungsfähige Grundlage für die gesamte Speicherarchitektur des NovaOS-Kernels.
