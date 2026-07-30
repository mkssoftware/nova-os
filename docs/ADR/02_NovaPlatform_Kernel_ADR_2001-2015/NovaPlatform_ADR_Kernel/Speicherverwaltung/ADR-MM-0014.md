# ADR-MM-0014: DMA Memory

* **ADR-ID:** ADR-MM-0014
* **Titel:** DMA Memory
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Memory Management
* **Gültigkeitsbereich:** Physical Memory Manager (PMM), Virtual Memory Manager (VMM), Device Manager, HAL, IOMMU Manager, Treiberframework
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-MM-0001, ADR-MM-0004, ADR-KERNEL-0002, ADR-KERNEL-0011

---

# 1. Zusammenfassung

NovaOS implementiert **DMA Memory** als eigenständiges Speichersubsystem zur Unterstützung von **Direct Memory Access (DMA)** zwischen Geräten und Arbeitsspeicher.

DMA ermöglicht es Hardwarekomponenten, Daten direkt zwischen Gerät und Hauptspeicher zu übertragen, ohne dass die CPU jedes Datenwort kopieren muss. NovaOS abstrahiert architekturspezifische Unterschiede wie IOMMU, Cache-Kohärenz, DMA-Adressräume und Gerätebeschränkungen vollständig über eine einheitliche DMA-Schnittstelle. DMA erlaubt Geräten den direkten Speicherzugriff ohne CPU-Kopieroperationen; moderne DMA-APIs kapseln dabei IOMMU, Cache-Kohärenz und unterschiedliche DMA-Adressräume.

---

# 2. Kontext

Nahezu alle modernen Geräte verwenden DMA.

Beispiele:

* NVMe SSD
* SATA Controller
* Netzwerkkarten
* USB Controller
* Audiohardware
* GPUs
* Kameras
* FPGA
* KI-Beschleuniger

Ein CPU-kopierter Datentransfer wäre ineffizient.

---

# 3. Problemstellung

Es musste entschieden werden,

* wie DMA-Speicher verwaltet wird,
* wie Geräte geeignete Speicherbereiche erhalten,
* wie Cache-Kohärenz sichergestellt wird,
* wie IOMMU integriert wird,
* und wie verschiedene DMA-Arten unterstützt werden.

---

# 4. Entscheidung

NovaOS führt einen **DMA Memory Manager (DMAM)** ein.

Dabei gilt:

* DMA-Speicher wird ausschließlich über den DMA Memory Manager verwaltet.
* Geräte arbeiten niemals direkt mit physischem Speicher.
* DMA-Adressen werden vollständig abstrahiert.
* Die HAL übernimmt alle architekturspezifischen Details.

---

# 5. Motivation

Diese Architektur bietet:

* Zero-Copy-Datenübertragung
* geringe CPU-Last
* hohe I/O-Leistung
* Geräteunabhängigkeit
* IOMMU-Unterstützung
* hohe Skalierbarkeit

---

# 6. Designprinzipien

DMA Memory basiert auf:

* Hardwareabstraktion
* Zero Copy
* IOMMU-Unterstützung
* Cache-Kohärenz
* Capability-Sicherheit
* Modularität

---

# 7. Architekturübersicht

```text
            Anwendung
                │
                ▼
          Gerätetreiber
                │
                ▼
        DMA Memory Manager
         │             │
         ▼             ▼
     IOMMU Manager     HAL
         │             │
         └──────┬──────┘
                ▼
        Physischer Speicher
                ▲
                │
            DMA-Gerät
```

---

# 8. Aufgaben des DMA Memory Managers

Der DMA Memory Manager übernimmt:

* DMA-Speicherallokation
* DMA-Freigabe
* DMA-Mapping
* DMA-Unmapping
* Cacheverwaltung
* Bounce Buffer
* Statistiken
* Diagnose

---

# 9. DMA-Speicherarten

NovaOS unterstützt:

* Coherent DMA
* Streaming DMA
* Scatter/Gather DMA
* DMA Pools
* Bounce Buffer
* Shared DMA

Die Architektur unterscheidet zwischen dauerhaft kohärentem DMA-Speicher und temporären Streaming-Mappings.

---

# 10. Coherent DMA

Coherent DMA eignet sich für:

* Ring Buffer
* Descriptorlisten
* Mailboxen
* Steuerstrukturen

CPU und Gerät sehen jederzeit denselben Datenbestand.

Kohärenter DMA-Speicher garantiert eine konsistente Sicht zwischen CPU und Gerät, ersetzt jedoch nicht notwendige Speicherbarrieren für Reihenfolgegarantien.

---

# 11. Streaming DMA

Streaming DMA eignet sich für:

* Massenspeicher
* Netzwerk
* Audio
* Video
* Kameras

Speicher wird:

* gemappt
* verwendet
* entmappt

Dadurch bleibt der Verwaltungsaufwand gering.

---

# 12. Scatter/Gather DMA

Scatter/Gather erlaubt

mehrere physische Speicherbereiche

als zusammenhängenden Datenstrom darzustellen.

Vorteile:

* weniger Kopien
* geringere Fragmentierungsprobleme
* effizientere Allokation

---

# 13. DMA Pools

Für kleine häufig benötigte Objekte

stellt NovaOS DMA Pools bereit.

Typische Objekte:

* Netzwerkdeskriptoren
* USB Requests
* Controllerstrukturen
* Warteschlangen

---

# 14. Bounce Buffer

Kann ein Gerät bestimmte Speicherbereiche nicht adressieren,

verwendet NovaOS automatisch Bounce Buffer.

Der DMA Memory Manager entscheidet transparent,

wann diese erforderlich sind.

Bounce Buffer werden automatisch eingesetzt, wenn Geräte den eigentlichen Zielspeicher nicht direkt adressieren können oder Sicherheitsanforderungen dies erfordern.

---

# 15. DMA Address Space

NovaOS unterscheidet:

* CPU Virtual Address
* CPU Physical Address
* DMA Address

Treiber arbeiten ausschließlich mit den durch den DMA Memory Manager bereitgestellten Adressen.

CPU-, physische und DMA-Adressen können sich unterscheiden, insbesondere bei Verwendung einer IOMMU.

---

# 16. IOMMU-Unterstützung

Die IOMMU übernimmt:

* Adressübersetzung
* Zugriffskontrolle
* Isolation
* Scatter/Gather-Unterstützung

Treiber müssen keine Hardwaredetails kennen.

---

# 17. Cache-Kohärenz

Je nach Plattform übernimmt NovaOS:

* Cache Flush
* Cache Invalidate
* Memory Barriers
* Cache Synchronisation

Diese Vorgänge erfolgen automatisch.

---

# 18. Virtual Memory Integration

DMA Memory arbeitet vollständig mit dem Virtual Memory Manager zusammen.

Virtuelle Adressen bleiben

von DMA-Operationen unberührt.

---

# 19. NUMA-Unterstützung

Bei NUMA-Systemen

werden DMA-Puffer

möglichst CPU- und Gerätenah angelegt.

Dies reduziert Speicherlatenzen.

---

# 20. Mehrkernbetrieb

DMA Memory unterstützt:

* SMP
* CPU Hotplug
* atomare Referenzzählung
* parallele DMA-Kanäle
* Thread-Sicherheit

---

# 21. Sicherheit

DMA Memory schützt vor:

* ungültigen DMA-Adressen
* Speicherüberschreitungen
* DMA-Angriffen
* fehlerhaften Geräten
* Rechteverletzungen

Die IOMMU begrenzt DMA-Zugriffe auf explizit freigegebene Speicherbereiche und verhindert dadurch unautorisierten Speicherzugriff durch Geräte.

---

# 22. Performance

Die Architektur soll ermöglichen:

* minimale CPU-Belastung
* maximale I/O-Bandbreite
* geringe Latenzen
* Zero-Copy-Datenpfade
* hohe Skalierbarkeit

---

# 23. Debug-Unterstützung

Optional unterstützt NovaOS:

* DMA-Statistiken
* Mapping-Historie
* IOMMU-Diagnosen
* Bufferanalyse
* Geräteprotokolle
* Performanceanalysen

---

# 24. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* CXL DMA
* GPU DMA
* RDMA
* Peer-to-Peer DMA
* Shared Virtual Addressing
* KI-Beschleuniger

---

# 25. Auswirkungen

Positive Auswirkungen:

* hohe I/O-Leistung
* geringe CPU-Auslastung
* moderne Geräteunterstützung
* sichere DMA-Isolation
* flexible Speicherverwaltung
* hohe Skalierbarkeit

Negative Auswirkungen:

* komplexere Speicherverwaltung
* zusätzlicher Verwaltungsaufwand
* erhöhter Implementierungsumfang

---

# 26. Verworfene Alternativen

## Direkte physische Speicherzugriffe

Vorteile:

* einfache Implementierung

Nachteile:

* keine Isolation
* geringe Sicherheit
* schlechte Portierbarkeit

---

## Geräteabhängige DMA-Implementierungen

Vorteile:

* geringe Anfangskomplexität

Nachteile:

* hoher Wartungsaufwand
* redundanter Code
* schlechte Erweiterbarkeit

---

## CPU-kopierte Datenübertragung

Vorteile:

* einfache Programmierung

Nachteile:

* hohe CPU-Belastung
* geringe Bandbreite
* schlechte Skalierung

---

# 27. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* DMA ausschließlich über den DMA Memory Manager verwenden,
* DMA-Mappings niemals manuell erzeugen,
* IOMMU ausschließlich über die HAL ansprechen,
* Cache-Kohärenz dem DMA Memory Manager überlassen,
* DMA-Ressourcen nach Verwendung ordnungsgemäß freigeben.

---

# 28. Teststrategie

DMA Memory wird geprüft durch:

* Unit-Tests
* DMA-Mapping-Tests
* IOMMU-Tests
* Scatter/Gather-Tests
* Bounce-Buffer-Tests
* Mehrkerntests
* Langzeittests
* Performancebenchmarks

---

# 29. Konsequenzen

DMA Memory bildet die zentrale Infrastruktur für sämtliche direkten Speicherzugriffe durch Hardwaregeräte in NovaOS.

Alle Gerätetreiber greifen ausschließlich über den DMA Memory Manager auf DMA-fähigen Speicher zu. Dadurch bleiben Treiber vollständig unabhängig von CPU-Architektur, IOMMU, Cache-Kohärenz und gerätespezifischen Einschränkungen. Gleichzeitig entsteht eine sichere und leistungsfähige Grundlage für moderne Hochgeschwindigkeitsgeräte.

---

# 30. Zukunftsperspektive

Langfristig soll der DMA Memory Manager erweitert werden um:

* automatische DMA-Optimierung je Gerät
* KI-gestützte Bufferplatzierung
* adaptive DMA-Pools
* RDMA-Unterstützung
* Peer-to-Peer-GPU-DMA
* Shared Virtual Addressing (SVA)

---

# 31. Zusammenfassung

NovaOS implementiert **DMA Memory** als eigenständiges Speichersubsystem für Direct Memory Access. Durch die Trennung von CPU-, physischem und DMA-Adressraum sowie die Integration von IOMMU, Cache-Kohärenz, Scatter/Gather, DMA-Pools und Bounce Buffern entsteht eine sichere, portable und hochperformante Architektur für sämtliche DMA-fähigen Geräte. Der DMA Memory Manager abstrahiert alle plattformspezifischen Unterschiede und stellt den Treibern eine einheitliche Schnittstelle für effiziente Zero-Copy-Datenübertragungen bereit.
