# ADR-MM-0003: Kernel Heap Allocator (kmalloc / kfree)

* **ADR-ID:** ADR-MM-0003
* **Titel:** Kernel Heap Allocator (kmalloc / kfree)
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Memory Management
* **Gültigkeitsbereich:** Kernel, Treiber, VFS, Netzwerkstack, IPC, Scheduler, Device Manager
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-MM-0001, ADR-MM-0002, ADR-KERNEL-0018, ADR-KERNEL-0011

---

# 1. Zusammenfassung

NovaOS stellt einen allgemeinen **Kernel Heap Allocator** bereit, über den Kernelkomponenten variabel große Speicherbereiche anfordern und wieder freigeben können.

Die öffentliche Schnittstelle besteht aus **kmalloc()** und **kfree()**. Intern nutzt der Allocator den Slab Allocator für kleine Objekte und den Page Allocator für große Speicheranforderungen. Dadurch vereint die Architektur hohe Performance, geringe Fragmentierung und eine einfache Programmierschnittstelle. Moderne Kernel verwenden `kmalloc()` typischerweise für kleinere Kernelallokationen und greifen für größere Anforderungen direkt auf den Seitenallocator oder andere Mechanismen zurück.

---

# 2. Kontext

Nahezu jede Kernelkomponente benötigt dynamisch Speicher.

Beispiele:

* Dateipuffer
* Netzwerkpakete
* Treiberdaten
* Hash-Tabellen
* Warteschlangen
* temporäre Objekte
* Parser
* Kernelmodule

Nicht jede Speicheranforderung besitzt eine feste Objektgröße.

---

# 3. Problemstellung

Es musste entschieden werden,

* wie Kernelcode Speicher reserviert,
* wie unterschiedliche Größen unterstützt werden,
* wie Fragmentierung reduziert wird,
* wie Mehrkernsysteme unterstützt werden,
* und wie eine einfache API bereitgestellt wird.

---

# 4. Entscheidung

NovaOS implementiert einen allgemeinen Kernel Heap Allocator.

Dabei gilt:

* **kmalloc()** reserviert Speicher.
* **kfree()** gibt Speicher frei.
* kleine Blöcke stammen aus dem Slab Allocator.
* große Blöcke stammen direkt vom Page Allocator.
* die Auswahl erfolgt automatisch.

---

# 5. Motivation

Diese Architektur bietet:

* einfache API
* hohe Geschwindigkeit
* geringe Fragmentierung
* automatische Größenverwaltung
* SMP-Skalierbarkeit
* klare Trennung zwischen API und Implementierung

---

# 6. Designprinzipien

Der Kernel Heap basiert auf:

* Transparenz
* Modularität
* Skalierbarkeit
* Cache-Lokalität
* Wiederverwendung
* deterministischen Laufzeiten

---

# 7. Architekturübersicht

```text
             kmalloc()
                 │
                 ▼
      Kernel Heap Allocator
        │                 │
        ▼                 ▼
 Slab Allocator     Page Allocator
        │                 │
        └────────┬────────┘
                 ▼
          Physischer Speicher
```

---

# 8. Öffentliche API

Der Kernel Heap stellt mindestens folgende Funktionen bereit:

* kmalloc()
* kzalloc()
* krealloc()
* kfree()
* kcalloc()
* kmemdup()

Weitere Hilfsfunktionen können ergänzt werden.

---

# 9. kmalloc()

**kmalloc()** reserviert einen Speicherbereich.

Eigenschaften:

* variable Größe
* ausgerichtete Adressen
* keine Initialisierung
* möglichst geringe Latenz

`kmalloc()` ist die Standardfunktion für allgemeine Kernelallokationen kleinerer Speicherbereiche.

---

# 10. kzalloc()

**kzalloc()** entspricht kmalloc(),

initialisiert jedoch den gesamten Speicherbereich mit Nullwerten.

Geeignet für:

* Strukturen
* Arrays
* Sicherheitskritische Daten

---

# 11. kcalloc()

kcalloc() reserviert:

* mehrere Elemente
* gleicher Größe
* vollständig nullinitialisiert

Zusätzlich erfolgt eine Überlaufprüfung der Größenberechnung.

---

# 12. krealloc()

krealloc() verändert die Größe eines bestehenden Speicherblocks.

Dabei gilt:

* Daten bleiben erhalten
* Vergrößerung möglich
* Verkleinerung möglich
* Speicher kann verschoben werden

---

# 13. kfree()

kfree() gibt Speicher wieder frei.

Eigenschaften:

* NULL ist erlaubt
* Double Free wird erkannt
* Speicher gelangt automatisch zum richtigen Allocator zurück

`kfree()` gibt zuvor mit `kmalloc()` oder verwandten Funktionen reservierten Speicher frei; ein NULL-Zeiger wird dabei ignoriert.

---

# 14. Größenklassen

Der Kernel Heap arbeitet mit Größenklassen.

Beispiel:

|      Größe | Backend        |
| ---------: | -------------- |
|  ≤ 32 Byte | Slab           |
|  ≤ 64 Byte | Slab           |
| ≤ 128 Byte | Slab           |
| ≤ 256 Byte | Slab           |
| ≤ 512 Byte | Slab           |
|    ≤ 1 KiB | Slab           |
|    ≤ 2 KiB | Slab           |
|    ≤ 4 KiB | Slab           |
|    > 4 KiB | Page Allocator |

Die genauen Schwellenwerte sind architekturabhängig konfigurierbar.

---

# 15. Automatische Auswahl

Der Kernel Heap entscheidet automatisch,

welcher Backend-Allocator verwendet wird.

Der Aufrufer kennt diese Entscheidung nicht.

Dadurch bleibt die API langfristig stabil.

---

# 16. Alignment

Alle Speicherbereiche erfüllen mindestens:

* Maschinenwort-Alignment
* SIMD-Anforderungen
* Cache-Line-Anforderungen (optional)

Die garantierte Ausrichtung hängt von Architektur und Blockgröße ab.

---

# 17. Große Allokationen

Große Speicherbereiche werden direkt vom Page Allocator reserviert.

Beispiele:

* große Dateicaches
* DMA-Puffer
* Framebuffer
* große Tabellen

---

# 18. Kleine Allokationen

Kleine Speicherblöcke stammen ausschließlich aus dem Slab Allocator.

Dies reduziert:

* Fragmentierung
* Initialisierungskosten
* Verwaltungsaufwand

---

# 19. Mehrkernbetrieb

Der Kernel Heap unterstützt:

* CPU-lokale Caches
* parallele Allokationen
* lockarme Datenstrukturen
* NUMA-Richtlinien

---

# 20. NUMA-Unterstützung

Bei NUMA-Systemen erfolgt bevorzugt:

* lokale Speicherallokation
* lokale Objektcaches
* CPU-nahe Speicherplatzierung

---

# 21. Sicherheit

Der Kernel Heap schützt vor:

* Double Free
* Use-after-Free
* Speicherüberläufen
* ungültigen Zeigern
* Größenüberläufen

Optional können Guard-Mechanismen und Speichervergiftung (Memory Poisoning) aktiviert werden.

---

# 22. Debug-Unterstützung

Optional unterstützt NovaOS:

* Leak Detection
* Heap Dumps
* Allokationsstatistiken
* Call-Stack-Erfassung
* Poisoning
* Canary-Werte
* Guard Pages

Kernel-Tracepoints für `kmalloc()` und `kfree()` erleichtern die Analyse von Speicherlecks und Fragmentierung.

---

# 23. Performance

Der Kernel Heap soll:

* O(1)-Allokationen für kleine Blöcke
* geringe Fragmentierung
* minimale Sperrzeiten
* hohe Parallelität
* gute Cache-Lokalität

ermöglichen.

---

# 24. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* Memory Tagging
* Quarantäne für freigegebenen Speicher
* Sicherheitsisolierung
* Hardware-Memory-Tagging
* Heap-Profiler
* adaptive Größenklassen

---

# 25. Auswirkungen

Positive Auswirkungen:

* einfache API
* hohe Performance
* automatische Backend-Auswahl
* geringe Fragmentierung
* hohe Skalierbarkeit
* gute Wartbarkeit

Negative Auswirkungen:

* komplexere interne Implementierung
* zusätzlicher Verwaltungsaufwand
* größerer Debugaufwand

---

# 26. Verworfene Alternativen

## Ausschließlich Slab Allocator

Vorteile:

* einfache Architektur

Nachteile:

* ungeeignet für große Speicherbereiche

---

## Ausschließlich Page Allocator

Vorteile:

* geringe Komplexität

Nachteile:

* hohe Speicherverschwendung
* schlechte Performance bei kleinen Objekten

---

## Freier Listen-Heap

Vorteile:
* flexibel

Nachteile:
* hohe Fragmentierung
* schlechtere Vorhersagbarkeit
* geringere Mehrkernskalierung

---

# 27. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* dynamischen Speicher ausschließlich über den Kernel Heap reservieren,
* Speicher ausschließlich mit kfree() freigeben,
* keine eigenen allgemeinen Heap-Allocator implementieren,
* Größenklassen nicht direkt ansprechen,
* Backend-Allocator niemals direkt verwenden.

---

# 28. Teststrategie

Der Kernel Heap wird geprüft durch:

* Unit-Tests
* Mehrkerntests
* Fragmentierungstests
* Leak-Tests
* Langzeittests
* Lasttests
* Performancebenchmarks
* Sicherheitstests

---

# 29. Konsequenzen

Der Kernel Heap wird zur standardisierten Speicher-API innerhalb des NovaOS-Kernels.

Alle zukünftigen Kernelkomponenten, Gerätetreiber und Subsysteme verwenden ausschließlich **kmalloc()** und **kfree()** beziehungsweise deren Erweiterungen. Die konkrete Auswahl zwischen Slab Allocator und Page Allocator bleibt vollständig gekapselt und kann zukünftig optimiert oder erweitert werden, ohne bestehende Kernelkomponenten anzupassen.

---

# 30. Zusammenfassung

NovaOS implementiert einen allgemeinen **Kernel Heap Allocator** mit den zentralen Schnittstellen **kmalloc()** und **kfree()**. Kleine Speicheranforderungen werden automatisch über den Slab Allocator bedient, während größere Bereiche direkt vom Page Allocator stammen. Durch diese mehrstufige Architektur entstehen eine einfache Programmierschnittstelle, hohe Performance, geringe Fragmentierung und eine hervorragende Skalierbarkeit auf modernen Mehrkernsystemen.
