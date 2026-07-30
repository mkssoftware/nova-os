# ADR-KERNEL-0006: Virtueller Adressraum

* **ADR-ID:** ADR-KERNEL-0006
* **Titel:** Virtueller Adressraum
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel-Architektur
* **Gültigkeitsbereich:** Memory Manager, Process Manager, Scheduler, HAL
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0002, ADR-KERNEL-0004, NPSPEC-KERNEL-0006, NPSPEC-KERNEL-0007

---

# 1. Zusammenfassung

NovaOS verwendet für jeden Prozess einen **eigenständigen virtuellen Adressraum**, der vollständig vom physischen Speicher entkoppelt ist. Die Übersetzung virtueller Adressen in physische Speicheradressen erfolgt über die Memory Management Unit (MMU) und kernelverwaltete Seitentabellen. Dadurch werden Prozessisolierung, Speicherschutz, gemeinsamer Speicher sowie moderne Speichertechniken wie Copy-on-Write und Memory Mapping ermöglicht.

---

# 2. Kontext

Der virtuelle Adressraum bildet die Grundlage der Speicherverwaltung.

Er beeinflusst unmittelbar:

* Prozessisolierung
* Speicherschutz
* Performance
* Speicherzuweisung
* Shared Memory
* Memory Mapping
* Copy-on-Write
* Paging

---

# 3. Problemstellung

Es musste entschieden werden,

* wie Prozesse auf Speicher zugreifen,
* wie physischer Speicher verwaltet wird,
* wie Prozesse voneinander isoliert werden,
* wie Kernel- und Userspeicher getrennt werden,
* und wie zukünftige Architekturen unterstützt werden.

---

# 4. Entscheidung

NovaOS verwendet einen vollständig virtualisierten Adressraum.

Dabei gilt:

* jeder Prozess besitzt einen eigenen virtuellen Adressraum,
* Kernel und Benutzerprogramme besitzen getrennte Speicherbereiche,
* physische Adressen sind für Anwendungen niemals sichtbar,
* sämtliche Speicherzugriffe erfolgen über virtuelle Adressen.

---

# 5. Motivation

Diese Architektur ermöglicht:

* vollständige Prozessisolierung
* Speicherschutz
* flexible Speicherverwaltung
* effiziente Speicherzuweisung
* bessere Skalierbarkeit
* Unterstützung moderner CPU-Architekturen

---

# 6. Designprinzipien

Der virtuelle Adressraum basiert auf folgenden Prinzipien:

* Isolation
* Transparenz
* Sicherheit
* Skalierbarkeit
* Architekturunabhängigkeit
* Erweiterbarkeit
* deterministische Speicherverwaltung

---

# 7. Architekturübersicht

```text
                    Virtuelle Adresse

                           │

                           ▼

                 Memory Management Unit

                           │

                           ▼

                    Seitentabellen

                           │

                           ▼

                  Physischer Speicher
```

---

# 8. Adressraum pro Prozess

Jeder Prozess besitzt einen vollständig unabhängigen virtuellen Adressraum.

Speicherbereiche verschiedener Prozesse dürfen sich ausschließlich über explizit freigegebene gemeinsame Speicherbereiche überschneiden.

---

# 9. Trennung von Kernel und Benutzerbereich

Der virtuelle Adressraum ist logisch in zwei Bereiche unterteilt:

* Userspace
* Kernelspace

Benutzerprogramme besitzen niemals direkten Zugriff auf Kernelbereiche.

---

# 10. Kernel Address Space

Der Kernel besitzt einen gemeinsamen virtuellen Adressraum.

Dieser enthält unter anderem:

* Kernelcode
* Kerneldaten
* Kernel Heap
* Kernel Stacks
* Gerätemappings
* Seitentabellen
* HAL-Strukturen

---

# 11. User Address Space

Jeder Prozess besitzt mindestens folgende Bereiche:

* Programmcode
* Konstanten
* Initialisierte Daten
* Nicht initialisierte Daten
* Heap
* Thread Stacks
* Shared Memory
* Memory Mappings

---

# 12. Seitengröße

NovaOS arbeitet mit einer architekturabhängigen Seitengröße.

Standardmäßig wird eine Seitengröße von **4 KiB** verwendet.

Größere Seitengrößen (Huge Pages) können zusätzlich unterstützt werden.

---

# 13. Seitentabellen

Die Abbildung virtueller Seiten auf physische Seiten erfolgt ausschließlich über Seitentabellen.

Diese werden vollständig durch den Kernel verwaltet.

---

# 14. Speicherattribute

Jede Speicherseite besitzt definierte Attribute.

Beispiele:

* Lesen
* Schreiben
* Ausführen
* Benutzerzugriff
* Kernelzugriff
* Cache-Eigenschaften
* Copy-on-Write
* Shared

---

# 15. Schutzmechanismen

Nicht erlaubte Speicherzugriffe führen zu einer Ausnahme (Page Fault).

Der Kernel entscheidet anschließend über:

* Nachladen
* Speicherzuweisung
* Prozessbeendigung
* Ausnahmebehandlung

---

# 16. Shared Memory

Mehrere Prozesse können definierte Speicherbereiche gemeinsam nutzen.

Dabei bleiben alle übrigen Bereiche vollständig isoliert.

---

# 17. Memory Mapping

Dateien oder Geräte können in den virtuellen Adressraum eingebunden werden.

Dadurch entfällt das Kopieren großer Datenbereiche.

---

# 18. Copy-on-Write

NovaOS unterstützt Copy-on-Write.

Physische Speicherseiten werden erst bei einer tatsächlichen Schreiboperation dupliziert.

---

# 19. Demand Paging

Speicherseiten werden nur bei Bedarf bereitgestellt.

Nicht verwendete Seiten belegen keinen physischen Speicher.

---

# 20. Kernel Heap

Der Kernel Heap befindet sich ausschließlich im Kernel Address Space.

Benutzerprogramme besitzen darauf keinen Zugriff.

---

# 21. Thread Stacks

Jeder Thread besitzt:

* eigenen Stack
* Guard Page
* Stackattribute

Stacküberläufe müssen zuverlässig erkannt werden.

---

# 22. Speicherfragmentierung

Der virtuelle Adressraum erlaubt eine weitgehende Entkopplung von virtueller und physischer Fragmentierung.

Dadurch kann physischer Speicher effizient genutzt werden.

---

# 23. NUMA-Unterstützung

Die Architektur darf zukünftige NUMA-Systeme unterstützen.

Die Speicherplatzierung erfolgt dabei transparent für Anwendungen.

---

# 24. DMA

Direkter Gerätezugriff erfolgt über speziell verwaltete DMA-Speicherbereiche.

Diese werden durch den Memory Manager kontrolliert.

---

# 25. Mehrarchitektur-Unterstützung

Der virtuelle Adressraum unterstützt:

* x86
* x86_64
* ARM64
* RISC-V

Architekturspezifische Unterschiede werden ausschließlich innerhalb der HAL gekapselt.

---

# 26. Performance

Die Speicherverwaltung soll:

* geringe TLB-Invalidierungen,
* effiziente Seitentabellen,
* minimale Fragmentierung,
* schnelle Adressübersetzung

ermöglichen.

---

# 27. Sicherheit

Der virtuelle Adressraum schützt vor:

* Speicherkorruption
* unerlaubten Kernelzugriffen
* Prozessüberschreitungen
* unautorisierten Speicherzugriffen

Jeder Prozess sieht ausschließlich seinen eigenen virtuellen Speicher.

---

# 28. Erweiterbarkeit

Die Architektur ermöglicht zukünftige Erweiterungen:

* Huge Pages
* Verschlüsselter Speicher
* Memory Compression
* Shared Virtual Memory
* IOMMU-Integration
* Persistenter Speicher

---

# 29. Auswirkungen

Positive Auswirkungen:

* vollständige Prozessisolierung
* hohe Sicherheit
* flexible Speicherverwaltung
* moderne Speichertechniken
* gute Skalierbarkeit

Negative Auswirkungen:

* höherer Verwaltungsaufwand
* zusätzlicher Speicherbedarf für Seitentabellen
* TLB-Verwaltung erforderlich

---

# 30. Verworfene Alternativen

## Gemeinsamer Adressraum

Vorteile:

* einfache Kommunikation

Nachteile:

* keine Isolation
* geringe Sicherheit
* hohe Fehleranfälligkeit

---

## Physische Speicheradressierung

Vorteile:

* einfache Implementierung

Nachteile:

* keine Virtualisierung
* schlechte Skalierbarkeit
* keine Speicherschutzmechanismen

---

## Segmentbasierte Speicherverwaltung

Vorteile:

* logische Speicherbereiche

Nachteile:

* höhere Komplexität
* geringere Flexibilität
* moderne Prozessoren sind auf Paging optimiert

---

# 31. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* ausschließlich virtuelle Adressen verwenden,
* niemals physische Adressen an Benutzerprogramme weitergeben,
* Speicherattribute konsequent auswerten,
* Seitentabellen ausschließlich über den Memory Manager verändern,
* Prozessisolierung jederzeit gewährleisten.

---

# 32. Teststrategie

Der virtuelle Adressraum wird geprüft durch:

* Unit-Tests
* Integrationstests
* Speicherschutztests
* Copy-on-Write-Tests
* Shared-Memory-Tests
* Paging-Tests
* Lasttests
* Langzeittests

---

# 33. Konsequenzen

Diese Architekturentscheidung definiert das grundlegende Speichermodell von NovaOS.

Alle zukünftigen Komponenten des Memory Managers, Process Managers, Schedulers, Dateisystems und der Geräteverwaltung müssen den virtuellen Adressraum als verbindliche Grundlage verwenden. Änderungen an dieser Architektur würden nahezu sämtliche Kernelsubsysteme betreffen und erfordern eine neue Architekturentscheidung.

---

# 34. Zusammenfassung

NovaOS verwendet für jeden Prozess einen **eigenständigen virtuellen Adressraum**, der vollständig vom physischen Speicher abstrahiert ist. Die MMU und kernelverwaltete Seitentabellen übernehmen die Adressübersetzung und ermöglichen Speicherschutz, Prozessisolierung, Shared Memory, Memory Mapping sowie moderne Speichertechniken wie Copy-on-Write und Demand Paging. Diese Architektur bildet die Grundlage einer sicheren, skalierbaren und leistungsfähigen Speicherverwaltung im NovaOS-Kernel.
