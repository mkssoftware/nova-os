# ADR-KERNEL-0007: Kernel Heap

* **ADR-ID:** ADR-KERNEL-0007
* **Titel:** Kernel Heap
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel-Architektur
* **Gültigkeitsbereich:** Memory Manager, Object Manager, Device Manager, Driver Framework
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0006, NPSPEC-KERNEL-0008

---

# 1. Zusammenfassung

NovaOS verwendet einen **eigenständigen Kernel Heap** zur dynamischen Speicherverwaltung sämtlicher Kernelkomponenten.

Der Kernel Heap ist vollständig vom Userspace getrennt und dient ausschließlich der Allokation interner Kernelobjekte. Er wird durch den Kernel Memory Manager verwaltet und unterstützt verschiedene spezialisierte Allokatoren für unterschiedliche Objektgrößen und Anwendungsfälle. Moderne Kernel kombinieren hierfür häufig mehrere Allokationsstrategien, um sowohl Performance als auch Speichereffizienz zu optimieren.

---

# 2. Kontext

Viele Kernelobjekte entstehen erst zur Laufzeit.

Beispiele:

* Prozesse
* Threads
* Geräteobjekte
* Dateisystemobjekte
* Netzwerkpakete
* Treiberstrukturen
* IPC-Objekte
* Synchronisationsobjekte

Diese Objekte benötigen dynamisch verwalteten Speicher.

---

# 3. Problemstellung

Es musste entschieden werden,

* wie Kernelobjekte dynamisch gespeichert werden,
* welcher Speicherbereich verwendet wird,
* wie Fragmentierung reduziert wird,
* wie Performance optimiert wird,
* und wie Speichersicherheit gewährleistet wird.

---

# 4. Entscheidung

NovaOS verwendet einen dedizierten Kernel Heap.

Dabei gilt:

* ausschließlich Kernelcode darf auf den Kernel Heap zugreifen,
* Benutzerprogramme besitzen keinerlei Zugriff,
* sämtliche Kernelobjekte werden ausschließlich über Kernel-Allocator erzeugt,
* der Kernel Heap wird zentral durch den Memory Manager verwaltet.

---

# 5. Motivation

Diese Architektur bietet:

* schnelle Speicherallokation
* geringe Fragmentierung
* zentrale Speicherverwaltung
* hohe Sicherheit
* gute Skalierbarkeit
* einfache Speicheranalyse

---

# 6. Designprinzipien

Der Kernel Heap basiert auf:

* zentraler Verwaltung
* Speichersicherheit
* modularen Allokatoren
* geringer Fragmentierung
* deterministischem Verhalten
* Erweiterbarkeit

---

# 7. Architekturübersicht

```text
                 Kernel Heap

                        │

        +---------------+---------------+

        ▼                               ▼

 Kleine Objekte                 Große Objekte

        ▼                               ▼

 Slab/Cache Allocator         Page Allocator

        │                               │

        └───────────────┬───────────────┘

                        ▼

              Physical Memory Manager
```

---

# 8. Speicherbereich

Der Kernel Heap befindet sich vollständig innerhalb des Kernel Address Space.

Er ist:

* nicht ausführbar (NX),
* ausschließlich im Kernelmodus zugänglich,
* dauerhaft gemappt,
* vom Userspace isoliert.

---

# 9. Verantwortungsbereich

Der Kernel Heap verwaltet ausschließlich dynamische Kernelobjekte.

Nicht Bestandteil sind:

* Userspace Heap
* DMA-Puffer
* Seitentabellen
* Kernel Stacks
* Bootspeicher

---

# 10. Allokationsschnittstelle

Der Kernel stellt eine einheitliche API bereit.

Beispiele:

* allocate()
* allocate_zeroed()
* reallocate()
* free()

Architekturspezifische Details bleiben verborgen.

---

# 11. Kleine Objekte

Kleine Speicherblöcke werden über spezialisierte Objekt-Allocatoren verwaltet.

Dadurch werden:

* Fragmentierung reduziert,
* Cache-Lokalität verbessert,
* Allokationen beschleunigt.

---

# 12. Große Objekte

Große Speicherbereiche werden direkt über den Seitenallocator bereitgestellt.

Hierfür können mehrere zusammenhängende Seiten reserviert werden.

---

# 13. Objekt-Caches

Häufig verwendete Kernelobjekte können in Caches vorgehalten werden.

Beispiele:

* Prozesse
* Threads
* Inodes
* Handles
* Timer
* Dateideskriptoren

Dies reduziert den Allokationsaufwand erheblich.

---

# 14. Speicherinitialisierung

Neu allokierter Speicher kann je nach Anforderung:

* unverändert,
* mit Nullwerten initialisiert,
* oder mit Debugmustern gefüllt werden.

---

# 15. Speicherfreigabe

Freigegebener Speicher wird:

* validiert,
* in den passenden Allocator zurückgeführt,
* optional überschrieben,
* für zukünftige Allokationen bereitgestellt.

---

# 16. Fragmentierung

Die Speicherverwaltung soll interne und externe Fragmentierung minimieren.

Geeignete Allokationsverfahren werden abhängig von Objektgröße und Lebensdauer eingesetzt.

---

# 17. Speicherattribute

Jede Allokation besitzt definierte Eigenschaften.

Beispiele:

* Größe
* Ausrichtung
* Zugriffsrechte
* Cache-Eigenschaften
* Allokatortyp

---

# 18. Ausrichtung (Alignment)

Alle Allokationen erfüllen definierte Alignment-Anforderungen.

Spezielle Hardwareobjekte können größere Ausrichtungen verlangen.

---

# 19. Speichergrenzen

Der Kernel Heap wächst ausschließlich innerhalb des Kernel Address Space.

Eine Überschreitung reservierter Bereiche ist unzulässig.

---

# 20. Speicherüberwachung

Der Kernel führt Statistiken über:

* belegten Speicher
* freien Speicher
* Anzahl aktiver Allokationen
* Cache-Auslastung
* Fragmentierungsgrad

---

# 21. Fehlerbehandlung

Fehlgeschlagene Allokationen werden standardisiert behandelt.

Mögliche Reaktionen:

* Fehlercode zurückgeben
* Speicherbereinigung versuchen
* Recovery-Mechanismen aktivieren
* Kernel Panic bei kritischen Kernelstrukturen

---

# 22. Sicherheit

Der Kernel Heap schützt vor:

* Double Free
* Use-after-Free
* ungültigen Freigaben
* Pufferüberläufen (soweit technisch möglich)
* Speicherkorruption

Debug- und Diagnosefunktionen können zusätzliche Prüfungen aktivieren.

---

# 23. Synchronisation

Der Heap muss vollständig thread-sicher arbeiten.

Je nach Allokator können verwendet werden:

* Spinlocks
* CPU-lokale Heaps
* Lockfreie Datenstrukturen
* atomare Operationen

---

# 24. NUMA-Unterstützung

Die Architektur muss zukünftige NUMA-Systeme unterstützen.

Speicher soll möglichst lokal zum ausführenden Prozessor allokiert werden.

---

# 25. Performance

Der Kernel Heap soll:

* konstante Allokationszeiten für kleine Objekte,
* geringe Sperrzeiten,
* hohe Parallelität,
* minimale Fragmentierung

erreichen.

---

# 26. Debug-Unterstützung

Optional unterstützt der Kernel Heap:

* Leak Detection
* Speicherstatistiken
* Guard Regions
* Canary-Werte
* Red Zones
* Speichervergiftung (Memory Poisoning)

---

# 27. Erweiterbarkeit

Die Architektur ermöglicht zukünftige Erweiterungen:

* Slab Allocator
* SLUB
* SLOB
* Buddy-basierte Spezialallokatoren
* Objektspezifische Pools
* Persistente Kernelobjekte

---

# 28. Auswirkungen

Positive Auswirkungen:

* schnelle Speicherverwaltung
* hohe Performance
* geringe Fragmentierung
* gute Skalierbarkeit
* zentrale Kontrolle

Negative Auswirkungen:

* zusätzlicher Verwaltungsaufwand
* komplexere Implementierung
* Debugfunktionen verursachen zusätzlichen Speicherbedarf

---

# 29. Verworfene Alternativen

## Ein globaler einfacher Heap

Vorteile:

* sehr einfache Implementierung

Nachteile:

* schlechte Skalierbarkeit
* hohe Fragmentierung
* geringe Parallelität

---

## Ausschließliche Seitenallokation

Vorteile:

* einfache Verwaltung

Nachteile:

* erhebliche Speicherverschwendung bei kleinen Objekten
* schlechte Cache-Ausnutzung

---

## Feste Speicherpools

Vorteile:

* deterministische Laufzeiten

Nachteile:

* geringe Flexibilität
* ineffiziente Speichernutzung

---

# 30. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* ausschließlich den Kernel Heap für dynamische Objekte verwenden,
* Speicher nach der Nutzung freigeben,
* keine eigenen Allokatoren außerhalb definierter Schnittstellen implementieren,
* Speicherfehler erkennen und behandeln,
* Alignment-Vorgaben einhalten.

---

# 31. Teststrategie

Der Kernel Heap wird validiert durch:

* Unit-Tests
* Stresstests
* Leak-Tests
* Fragmentierungstests
* Parallelitätstests
* Langzeittests
* Performancebenchmarks

---

# 32. Konsequenzen

Diese Architekturentscheidung definiert den Kernel Heap als zentrale Infrastruktur für sämtliche dynamischen Kernelobjekte.

Alle zukünftigen Kernelkomponenten verwenden ausschließlich die standardisierte Speicherverwaltung des Memory Managers. Änderungen an dieser Architektur wirken sich unmittelbar auf nahezu alle Kernelsubsysteme aus und bedürfen einer neuen Architekturentscheidung.

---

# 33. Zusammenfassung

NovaOS verwendet einen **dedizierten Kernel Heap** zur dynamischen Speicherverwaltung aller Kernelobjekte. Durch die Kombination spezialisierter Allokatoren für kleine und große Speicherblöcke, zentraler Verwaltung sowie integrierter Sicherheits- und Diagnosemechanismen entsteht eine leistungsfähige, skalierbare und robuste Speicherverwaltung. Diese Architektur bildet die Grundlage für sämtliche dynamischen Kernelstrukturen innerhalb des NovaOS-Kernels.
