# ADR-MM-0008: Memory Mapping

* **ADR-ID:** ADR-MM-0008
* **Titel:** Memory Mapping
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Memory Management
* **Gültigkeitsbereich:** Virtual Memory Manager (VMM), VFS, Loader, Process Manager, Shared Memory, Device Manager
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-MM-0004, ADR-MM-0005, ADR-MM-0006, ADR-KERNEL-0009

---

# 1. Zusammenfassung

NovaOS implementiert **Memory Mapping** als universellen Mechanismus, um Dateien, Geräte, Shared-Memory-Objekte oder anonyme Speicherbereiche direkt in den virtuellen Adressraum eines Prozesses einzubinden.

Memory Mapping ersetzt klassische Dateioperationen nicht, stellt jedoch für viele Anwendungsfälle eine deutlich effizientere Alternative dar. Anwendungen greifen direkt über virtuelle Adressen auf den Speicher zu, während der Virtual Memory Manager die benötigten Seiten automatisch lädt und verwaltet. Memory Mapping ermöglicht den direkten Zugriff auf Dateien oder anonyme Speicherbereiche über den virtuellen Adressraum; benötigte Seiten werden bei Bedarf automatisch geladen.

---

# 2. Kontext

Moderne Anwendungen arbeiten häufig mit:

* sehr großen Dateien
* Datenbanken
* Shared Memory
* ausführbaren Programmen
* Bibliotheken
* Speicherabbildern
* Gerätespeicher

Das vollständige Einlesen dieser Daten in den Arbeitsspeicher wäre ineffizient.

---

# 3. Problemstellung

Es musste entschieden werden,

* wie Dateien in den virtuellen Speicher eingebunden werden,
* wie anonyme Speicherbereiche erzeugt werden,
* wie Page Faults behandelt werden,
* wie verschiedene Mapping-Arten unterstützt werden,
* und wie Sicherheit gewährleistet wird.

---

# 4. Entscheidung

NovaOS verwendet Memory Mapping als universelle Abstraktion für virtuelle Speicherabbildungen.

Dabei gilt:

* jede Abbildung wird als Virtual Memory Region verwaltet,
* physische Seiten werden erst bei Bedarf geladen,
* alle Mapping-Typen verwenden dieselbe Architektur,
* das Mapping ist vollständig vom Dateisystem abstrahiert.

---

# 5. Motivation

Diese Architektur bietet:

* Zero-Copy-Zugriffe
* geringere CPU-Last
* geringeren Speicherverbrauch
* Lazy Loading
* hohe Performance
* einheitliche Speicherverwaltung

---

# 6. Designprinzipien

Memory Mapping basiert auf:

* Demand Paging
* Lazy Allocation
* Virtual Memory Regions
* Copy-on-Write
* Objektorientierung
* Hardwareabstraktion

---

# 7. Architekturübersicht

```text id="mmap001"
            Anwendung
                │
                ▼
      Virtual Memory Region
                │
                ▼
      Virtual Memory Manager
        │              │
        ▼              ▼
   Page Fault      Page Cache
        │              │
        └───────┬──────┘
                ▼
      Datei / Shared Memory /
      Anonymer Speicher / MMIO
```

---

# 8. Mapping-Typen

NovaOS unterstützt folgende Mapping-Arten:

* Anonymous Mapping
* File Mapping
* Shared Mapping
* Private Mapping
* Device Mapping
* Executable Mapping
* Kernel Mapping

Weitere Mapping-Typen können ergänzt werden.

---

# 9. Anonymous Mapping

Anonymous Mappings besitzen keine zugrunde liegende Datei.

Typische Anwendungsfälle:

* Heap
* Stack
* temporäre Speicherbereiche
* JIT-Compiler
* interne Kernelstrukturen

---

# 10. File Mapping

Beim File Mapping wird eine Datei direkt in den virtuellen Adressraum eingebunden.

Eigenschaften:

* Lazy Loading
* Seitencache
* Random Access
* große Dateien
* geringe Kopierkosten

Dateien können dadurch wie gewöhnlicher Speicher angesprochen werden.

---

# 11. Shared Mapping

Shared Mappings erlauben,

dass mehrere Prozesse dieselben physischen Seiten gemeinsam verwenden.

Änderungen sind für alle Prozesse sichtbar.

---

# 12. Private Mapping

Private Mappings verwenden Copy-on-Write.

Eigenschaften:

* gemeinsamer Ursprung
* private Änderungen
* automatische Seitenduplizierung
* unveränderte Quelldatei

---

# 13. Device Mapping

Memory Mapping unterstützt Gerätezugriffe.

Beispiele:

* Framebuffer
* PCI BAR
* MMIO
* GPU Memory
* FPGA
* Embedded Devices

Der Device Manager stellt die erforderlichen Ressourcen bereit.

---

# 14. Executable Mapping

Programme werden über Executable Mappings geladen.

Eigenschaften:

* Read
* Execute
* Demand Paging
* Lazy Loading
* Copy-on-Write für private Daten

Programmcode wird dadurch nur bei tatsächlichem Zugriff in den Speicher geladen.

---

# 15. Demand Paging

Memory Mapping arbeitet vollständig mit Demand Paging.

Beim ersten Zugriff:

* Page Fault
* Seite laden
* Seitentabelle aktualisieren
* Programmausführung fortsetzen

Nicht benötigte Seiten werden niemals geladen.

---

# 16. Page Cache

Dateibasierte Mappings verwenden den globalen Page Cache.

Vorteile:

* keine doppelten Kopien
* gemeinsame Nutzung
* geringerer Speicherbedarf
* höhere Performance

Mehrere Prozesse können dieselben zwischengespeicherten Dateiseiten gemeinsam nutzen.

---

# 17. Virtual Memory Regions

Jedes Mapping entspricht genau einer oder mehreren Virtual Memory Regions.

Diese enthalten:

* Adressbereich
* Rechte
* Mapping-Typ
* Quelle
* Flags
* Referenzen

---

# 18. Zugriffsrechte

Jedes Mapping besitzt eigene Attribute.

Unterstützt werden:

* Read
* Write
* Execute
* Copy-on-Write
* User
* Kernel
* No Access

---

# 19. Page Fault Handler

Der Page Fault Handler übernimmt:

* Seitennachladen
* Copy-on-Write
* Rechteprüfung
* Gerätezugriffe
* Seitenerzeugung

Nur gültige Zugriffe führen zum Nachladen einer Seite.

---

# 20. Mehrkernbetrieb

Memory Mapping unterstützt:

* SMP
* CPU Hotplug
* atomare Referenzzählung
* TLB-Synchronisation
* parallele Page Faults

---

# 21. NUMA-Unterstützung

Bei NUMA-Systemen bevorzugt NovaOS:

* lokale Speicherknoten
* CPU-nahe Seiten
* NUMA-optimierte Mappings

---

# 22. Sicherheit

Memory Mapping schützt vor:

* ungültigen Adressbereichen
* überlappenden Mappings
* Rechteverletzungen
* fehlerhaften Gerätezugriffen
* inkonsistenten Seitentabellen

---

# 23. Performance

Die Architektur soll ermöglichen:

* Zero-Copy-Zugriffe
* geringe CPU-Last
* minimale Kopieroperationen
* schnelle Random-Access-Zugriffe
* hohe Skalierbarkeit

Memory Mapping ist insbesondere bei großen Dateien und zufälligen Zugriffsmustern effizient, da nur tatsächlich benötigte Seiten eingelagert werden.

---

# 24. Debug-Unterstützung

Optional unterstützt NovaOS:

* Mapping-Tabellen
* Mapping-Historie
* Page-Fault-Tracing
* Seitencache-Analyse
* Mapping-Statistiken
* Rechteanalyse

---

# 25. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* verschlüsselte Mappings
* Persistent Memory
* CXL Memory
* GPU Memory Mapping
* Remote Memory Mapping
* Snapshot-Mappings

---

# 26. Auswirkungen

Positive Auswirkungen:

* hohe Performance
* geringe Speicherkopien
* kleiner Speicherverbrauch
* effiziente Dateiverarbeitung
* flexible Speicherverwaltung
* moderne Architektur

Negative Auswirkungen:

* komplexerer Virtual Memory Manager
* höherer Verwaltungsaufwand
* anspruchsvollere Fehlerdiagnose

---

# 27. Verworfene Alternativen

## Ausschließlich read()/write()

Vorteile:

* einfache Implementierung

Nachteile:

* hohe Kopierkosten
* geringere Performance
* zusätzlicher Speicherbedarf

---

## Vollständiges Einlesen jeder Datei

Vorteile:

* einfache Programmierung

Nachteile:

* hoher Speicherverbrauch
* schlechte Skalierbarkeit
* langsamer Programmstart

---

## Separate APIs für jeden Mapping-Typ

Vorteile:

* spezialisierte Implementierungen

Nachteile:

* inkonsistente Architektur
* höherer Wartungsaufwand
* redundanter Code

---

# 28. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* Speicherabbildungen ausschließlich über den Virtual Memory Manager erzeugen,
* Virtual Memory Regions verwenden,
* Page Faults zentral behandeln,
* keine direkten Seitentabellenmanipulationen durchführen,
* Hardwarezugriffe ausschließlich über die HAL und den Device Manager abwickeln.

---

# 29. Teststrategie

Memory Mapping wird geprüft durch:

* Unit-Tests
* File-Mapping-Tests
* Anonymous-Mapping-Tests
* Shared-Mapping-Tests
* Copy-on-Write-Tests
* Mehrkerntests
* Lasttests
* Performancebenchmarks

---

# 30. Konsequenzen

Memory Mapping wird zum universellen Mechanismus für virtuelle Speicherabbildungen innerhalb von NovaOS.

Alle zukünftigen Komponenten – insbesondere Loader, Shared Memory, Dateisystem, Grafiksystem und Gerätetreiber – verwenden dieselbe Mapping-Infrastruktur. Dadurch entsteht eine einheitliche, performante und skalierbare Architektur, die den direkten Zugriff auf Dateien, Geräte und gemeinsam genutzten Speicher ermöglicht.

---

# 31. Zusammenfassung

NovaOS implementiert **Memory Mapping** als zentrale Architekturkomponente der virtuellen Speicherverwaltung. Dateien, Geräte, Shared-Memory-Objekte und anonyme Speicherbereiche werden einheitlich als Virtual Memory Regions verwaltet und bei Bedarf per Demand Paging in den Adressraum eingebunden. Durch die enge Integration mit dem Virtual Memory Manager, dem Page Cache und Copy-on-Write entsteht eine moderne, leistungsfähige und erweiterbare Speicherarchitektur.
