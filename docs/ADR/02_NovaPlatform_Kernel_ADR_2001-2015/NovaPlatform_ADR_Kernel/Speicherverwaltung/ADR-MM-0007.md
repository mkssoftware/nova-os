# ADR-MM-0007: Huge Pages

* **ADR-ID:** ADR-MM-0007
* **Titel:** Huge Pages
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Memory Management
* **Gültigkeitsbereich:** Virtual Memory Manager (VMM), Page Allocator, Process Manager, Loader, Shared Memory, Scheduler
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-MM-0001, ADR-MM-0004, ADR-MM-0006, ADR-KERNEL-0009

---

# 1. Zusammenfassung

NovaOS unterstützt **Huge Pages** als Optimierung für speicherintensive Anwendungen und große zusammenhängende virtuelle Speicherbereiche.

Neben den standardmäßigen Speicherseiten (Base Pages) können deutlich größere Seiten verwendet werden. Dadurch reduziert sich die Anzahl der Seitentabelleneinträge sowie der Druck auf den Translation Lookaside Buffer (TLB). Das verbessert insbesondere die Leistung großer Speicherzugriffe und reduziert den Verwaltungsaufwand der virtuellen Speicherverwaltung. Große Speicherseiten verringern die Anzahl der Seitentabelleneinträge und reduzieren TLB-Misses, was insbesondere bei speicherintensiven Anwendungen die Leistung verbessern kann.

---

# 2. Kontext

Viele Anwendungen arbeiten mit sehr großen Speicherbereichen.

Beispiele:

* Datenbanken
* KI-Modelle
* wissenschaftliche Simulationen
* virtuelle Maschinen
* Grafiksysteme
* Compiler
* Multimedia
* Dateicaches

Bei Verwendung kleiner Seiten steigt die Anzahl notwendiger Seitentabelleneinträge erheblich.

---

# 3. Problemstellung

Es musste entschieden werden,

* wann große Seiten verwendet werden,
* welche Seitengrößen unterstützt werden,
* wie Huge Pages verwaltet werden,
* wie sie mit dem Page Allocator zusammenarbeiten,
* und wie Fragmentierung vermieden wird.

---

# 4. Entscheidung

NovaOS unterstützt mehrere Seitengrößen.

Dabei gilt:

* Base Pages bleiben Standard.
* Huge Pages werden optional verwendet.
* Die Auswahl erfolgt automatisch oder explizit.
* Alle Seitengrößen werden vollständig durch den Virtual Memory Manager unterstützt.

---

# 5. Motivation

Diese Architektur bietet:

* geringere TLB-Misses
* kleinere Seitentabellen
* weniger Verwaltungsaufwand
* höhere Speicherbandbreite
* bessere Performance großer Anwendungen
* gute Skalierbarkeit

---

# 6. Designprinzipien

Huge Pages basieren auf:

* Transparenz
* Modularität
* Lazy Allocation
* geringer Verwaltungsaufwand
* Hardwareunabhängigkeit
* Skalierbarkeit

---

# 7. Architekturübersicht

```text
         Virtuelle Region
                │
                ▼
      Virtual Memory Manager
                │
      +---------+---------+
      │                   │
      ▼                   ▼
   Base Pages        Huge Pages
      │                   │
      └---------+---------┘
                |
                ▼
         Seitentabellen
                |
                ▼
        Physischer Speicher
```

---

# 8. Unterstützte Seitengrößen

NovaOS definiert logisch drei Seitengrößen:

| Typ        | Beispielgröße* |
| ---------- | -------------- |
| Base Page  | 4 KiB          |
| Large Page | 2 MiB          |
| Giant Page | 1 GiB          |

*Die tatsächlichen Größen hängen von der unterstützten Prozessorarchitektur ab und werden durch die HAL bereitgestellt.

---

# 9. Hardwareabstraktion

Die HAL abstrahiert sämtliche architekturspezifischen Unterschiede.

Beispiele:

* x86-64
* ARM64
* RISC-V
* zukünftige Architekturen

Der Virtual Memory Manager arbeitet ausschließlich mit abstrakten Seitengrößen.

---

# 10. Auswahlstrategie

Huge Pages können verwendet werden:

* automatisch
* explizit durch Kernelkomponenten
* explizit durch den Loader
* über Richtlinien des Memory Managers

Der Standard bleibt die Base Page.

---

# 11. Automatische Promotion

Der Virtual Memory Manager kann mehrere benachbarte Base Pages automatisch zu einer Huge Page zusammenfassen.

Voraussetzungen:

* zusammenhängender Speicher
* identische Rechte
* identische Attribute
* vollständige Auslastung

---

# 12. Automatische Demotion

Eine Huge Page kann wieder in Base Pages zerlegt werden.

Beispiele:

* Copy-on-Write
* Teilfreigaben
* unterschiedliche Rechte
* Fragmentierung

---

# 13. Seitentabellen

Huge Pages reduzieren:

* Anzahl der Seitentabellen
* Seitentabellenspeicher
* Verwaltungsoperationen

Dadurch sinkt der Verwaltungsaufwand des Virtual Memory Managers.

---

# 14. TLB-Optimierung

Da eine einzelne Huge Page einen deutlich größeren Adressbereich beschreibt,

* werden weniger TLB-Einträge benötigt,
* sinken TLB-Misses,
* verbessert sich die Speicherzugriffsleistung.

Die Verringerung der TLB-Belastung ist einer der Hauptgründe für den Einsatz großer Seiten.

---

# 15. Speicherallokation

Der Page Allocator stellt zusammenhängende physische Speicherbereiche bereit.

Huge Pages werden ausschließlich erzeugt,

wenn ausreichend zusammenhängender Speicher verfügbar ist.

---

# 16. Virtual Memory Regions

Huge Pages sind vollständig in Virtual Memory Regions integriert.

Eine Region kann bestehen aus:

* ausschließlich Base Pages
* ausschließlich Huge Pages
* einer Kombination beider

---

# 17. Copy-on-Write

Copy-on-Write wird vollständig unterstützt.

Bei einem Schreibzugriff auf eine gemeinsam genutzte Huge Page kann diese automatisch in kleinere Seiten aufgeteilt werden.

---

# 18. Shared Memory

Shared Memory kann Huge Pages verwenden.

Dies verbessert insbesondere:

* KI-Anwendungen
* Multimedia
* Datenbanken
* virtuelle Maschinen

---

# 19. Mehrkernbetrieb

Huge Pages unterstützen:

* SMP
* CPU Hotplug
* NUMA
* atomare Seitentabellenaktualisierung
* TLB-Synchronisation

---

# 20. NUMA

Bei NUMA-Systemen bevorzugt NovaOS:

* lokale Huge Pages
* lokale Speicherbereiche
* CPU-nahe Allokationen

---

# 21. Fragmentierungsmanagement

Zur Vermeidung externer Fragmentierung unterstützt NovaOS:

* Promotion
* Demotion
* Speicherkompaktierung
* Hintergrundoptimierung

Große zusammenhängende Seiten erfordern ausreichend zusammenhängenden physischen Speicher und profitieren daher von einer aktiven Fragmentierungsverwaltung.

---

# 22. Sicherheit

Huge Pages schützen vor:

* inkonsistenten Seitentabellen
* fehlerhaften Promotionen
* ungültigen Demotionen
* Rechteverletzungen
* Speicherkorruption

---

# 23. Performance

Die Architektur soll ermöglichen:

* höhere TLB-Trefferraten
* geringeren Verwaltungsaufwand
* geringere CPU-Last
* bessere Speicherbandbreite
* hohe Skalierbarkeit

---

# 24. Debug-Unterstützung

Optional unterstützt NovaOS:

* Huge-Page-Statistiken
* Promotion-Historie
* Demotion-Historie
* TLB-Analysen
* Fragmentierungsanalyse
* Speicherkarten

---

# 25. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* transparente Huge Pages
* adaptive Seitengrößen
* Hardware Memory Tagging
* CXL Memory
* Tiered Memory
* KI-gestützte Promotion

---

# 26. Auswirkungen

Positive Auswirkungen:

* kleinere Seitentabellen
* geringere TLB-Misses
* höhere Speicherleistung
* geringere CPU-Belastung
* bessere Skalierbarkeit
* moderne Speicherarchitektur

Negative Auswirkungen:

* höherer Bedarf an zusammenhängendem Speicher
* komplexere Speicherverwaltung
* aufwendigere Fragmentierungsbehandlung

---

# 27. Verworfene Alternativen

## Ausschließlich Base Pages

Vorteile:

* einfache Verwaltung
* geringe Fragmentierungsanforderungen

Nachteile:

* hohe TLB-Belastung
* große Seitentabellen
* geringere Leistung bei großen Anwendungen

---

## Ausschließlich Huge Pages

Vorteile:

* maximale TLB-Effizienz

Nachteile:

* starke interne Fragmentierung
* ungeeignet für kleine Allokationen

---

## Feste Seitengröße

Vorteile:

* einfache Implementierung

Nachteile:

* keine Anpassung an unterschiedliche Workloads
* geringere Flexibilität

---

# 28. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* Huge Pages ausschließlich über den Virtual Memory Manager verwalten,
* Seitengrößen niemals direkt voraussetzen,
* Promotion und Demotion transparent behandeln,
* keine direkten Seitentabellenänderungen durchführen,
* Hardwaredetails ausschließlich über die HAL verwenden.

---

# 29. Teststrategie

Huge Pages werden geprüft durch:

* Unit-Tests
* Promotion-Tests
* Demotion-Tests
* TLB-Tests
* Mehrkerntests
* NUMA-Tests
* Fragmentierungstests
* Performancebenchmarks

---

# 30. Konsequenzen

Huge Pages werden zu einem integralen Bestandteil der virtuellen Speicherverwaltung von NovaOS.

Der Virtual Memory Manager entscheidet abhängig von Anwendung, Speicherlayout und Hardware über den optimalen Einsatz großer Seiten. Dadurch können speicherintensive Anwendungen erheblich von einer reduzierten TLB-Belastung und kleineren Seitentabellen profitieren, während klassische Anwendungen weiterhin effizient mit Base Pages arbeiten.

---

# 31. Zusammenfassung

NovaOS unterstützt **Huge Pages** als leistungsorientierte Erweiterung der virtuellen Speicherverwaltung. Neben normalen Speicherseiten können große Seiten dynamisch eingesetzt werden, um Seitentabellen zu verkleinern, TLB-Misses zu reduzieren und die Speicherleistung zu verbessern. Durch automatische Promotion und Demotion, vollständige Integration in Virtual Memory Regions sowie Unterstützung für NUMA, Copy-on-Write und Shared Memory entsteht eine flexible und zukunftssichere Architektur für moderne Mehrkernsysteme.
