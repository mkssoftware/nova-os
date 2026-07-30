# ADR-MM-0012: NUMA Memory Policy

* **ADR-ID:** ADR-MM-0012
* **Titel:** NUMA Memory Policy
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Memory Management
* **Gültigkeitsbereich:** Virtual Memory Manager (VMM), Physical Memory Manager (PMM), Scheduler, NUMA Manager, Process Manager
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-MM-0001, ADR-MM-0004, ADR-MM-0007, ADR-MM-0010, ADR-MM-0011, ADR-KERNEL-0018

---

# 1. Zusammenfassung

NovaOS implementiert eine **NUMA Memory Policy**, welche die Platzierung physischer Speicherseiten in Systemen mit **Non-Uniform Memory Access (NUMA)** steuert.

Die Policy entscheidet, auf welchem NUMA-Knoten neue Speicherseiten angelegt, migriert oder bevorzugt verwendet werden. Ziel ist es, Speicher möglichst nahe an der ausführenden CPU bereitzustellen, um Speicherlatenzen zu reduzieren, Speicherbandbreite zu erhöhen und die Skalierbarkeit auf Mehrsockel- und Hochleistungssystemen deutlich zu verbessern. NUMA-Memory-Policies steuern, aus welchem NUMA-Knoten Speicher bevorzugt alloziert wird und können systemweit, pro Prozess oder für einzelne virtuelle Speicherbereiche gelten.

---

# 2. Kontext

In NUMA-Systemen besitzt jeder Prozessorknoten eigenen lokalen Arbeitsspeicher.

Speicherzugriffe unterscheiden sich abhängig von:

* CPU
* NUMA-Knoten
* Speichercontroller
* Interconnect
* Speichertechnologie

Lokaler Speicher ist deutlich schneller erreichbar als entfernter Speicher.

---

# 3. Problemstellung

Es musste entschieden werden,

* wie Speicherknoten ausgewählt werden,
* wann Migration erfolgen soll,
* welche Richtlinien unterstützt werden,
* wie Anwendungen eigene Policies definieren können,
* und wie automatische Optimierungen erfolgen.

---

# 4. Entscheidung

NovaOS implementiert eine mehrstufige NUMA Memory Policy.

Dabei gilt:

* Standard ist lokale Allokation.
* Prozesse können eigene Policies besitzen.
* Virtual Memory Regions können eigene Policies besitzen.
* Der Kernel kann Seiten dynamisch migrieren.
* Alle Entscheidungen erfolgen transparent.

---

# 5. Motivation

Diese Architektur bietet:

* geringere Speicherlatenz
* höhere Speicherbandbreite
* bessere Cache-Lokalität
* bessere Skalierung
* optimale Mehrsockelunterstützung
* Zukunftssicherheit

---

# 6. Designprinzipien

Die NUMA Memory Policy basiert auf:

* Locality First
* Transparenz
* Modularität
* Adaptivität
* Hardwareabstraktion
* deterministischen Richtlinien

---

# 7. Architekturübersicht

```text
                  Anwendung

                       │

                       ▼

               Virtual Memory Region

                       │

                       ▼

              NUMA Memory Policy

                       │

          +------------+------------+

          ▼                         ▼

   Local Node               Remote Node

          │                         │

          └------------+------------┘

                       ▼

            Physical Memory Manager
```

---

# 8. Policy-Ebenen

NovaOS unterstützt mehrere Ebenen:

* Systemrichtlinie
* Prozessrichtlinie
* Threadrichtlinie
* Virtual-Memory-Region-Richtlinie
* Kernelrichtlinie

Spezifischere Richtlinien überschreiben allgemeinere.

---

# 9. Standardrichtlinie

Die Standardrichtlinie lautet:

**Local Allocation**

Neue Seiten werden möglichst auf dem lokalen NUMA-Knoten angelegt.

Existiert kein lokaler Speicher,

wird automatisch ein geeigneter Ersatzknoten gewählt.

Die Standardstrategie moderner Betriebssysteme besteht darin, Speicher möglichst lokal zum ausführenden Prozessor zu allozieren.

---

# 10. Unterstützte Richtlinien

NovaOS definiert folgende Policies:

* Default
* Local
* Preferred
* Bind
* Interleave
* Weighted Interleave
* Adaptive
* Kernel Managed

Weitere Richtlinien können ergänzt werden.

---

# 11. Local Policy

Neue Seiten werden

möglichst auf dem lokalen NUMA-Knoten angelegt.

Diese Policy bietet:

* minimale Latenz
* optimale Cache-Lokalität
* geringe Interconnect-Last

---

# 12. Preferred Policy

Ein bevorzugter NUMA-Knoten wird definiert.

Ist dieser ausgelastet,

können alternative Knoten verwendet werden.

---

# 13. Bind Policy

Eine Bind Policy beschränkt Speicher ausschließlich auf definierte NUMA-Knoten.

Ist dort kein Speicher verfügbar,

schlägt die Allokation fehl oder folgt einer konfigurierbaren Fallback-Strategie.

---

# 14. Interleave Policy

Neue Seiten werden gleichmäßig auf mehrere NUMA-Knoten verteilt.

Dies eignet sich insbesondere für:

* Streaming
* wissenschaftliche Berechnungen
* Datenbanken
* große Speicherpools

Interleave verteilt Speicheranforderungen über mehrere Knoten, um Lastspitzen zu vermeiden und Bandbreite besser auszunutzen.

---

# 15. Weighted Interleave

NovaOS erweitert Interleave um Gewichtungen.

Beispiel:

```text
Node 0 : 50 %

Node 1 : 30 %

Node 2 : 20 %
```

Dadurch können unterschiedlich schnelle Speichertechnologien optimal genutzt werden.

---

# 16. Adaptive Policy

Die Adaptive Policy bewertet:

* Speicherlatenz
* CPU-Auslastung
* Threadmigration
* Zugriffshäufigkeit
* Bandbreite

Die optimale Platzierung wird dynamisch angepasst.

---

# 17. Virtual Memory Regions

Jede Virtual Memory Region kann

eine eigene NUMA Memory Policy besitzen.

Dadurch lassen sich

* Heap
* Shared Memory
* Memory Mappings
* Datenbanken

unterschiedlich behandeln.

Speicherrichtlinien können auf einzelne virtuelle Speicherbereiche angewendet werden und überschreiben dort allgemeinere Prozessrichtlinien.

---

# 18. Threadintegration

Der Scheduler übermittelt dem NUMA Manager:

* aktuelle CPU
* Zielknoten
* Threadmigrationen
* Auslastung

Dadurch kann Speicher lokal bereitgestellt werden.

---

# 19. Seitenmigration

NovaOS unterstützt automatische Seitenmigration.

Eine Migration kann erfolgen bei:

* Threadmigration
* Laständerung
* Speicherhotplug
* NUMA-Ungleichgewicht
* Administratorvorgaben

---

# 20. Zusammenarbeit mit Memory Reclamation

Vor einer Seitenmigration können:

* ungenutzte Seiten entfernt,
* komprimierte Seiten dekomprimiert,
* Caches bereinigt werden.

Dadurch sinkt der Migrationsaufwand.

---

# 21. Huge Pages

Huge Pages werden vollständig unterstützt.

Bei Bedarf können Huge Pages

* migriert,
* aufgeteilt,
* oder lokal neu aufgebaut werden.

---

# 22. Memory Hotplug

Neue NUMA-Knoten werden automatisch

in sämtliche Policies integriert.

Entfernte Knoten führen zur

automatischen Neuberechnung aller Richtlinien.

---

# 23. Heterogene Speichersysteme

Die Architektur unterstützt unterschiedliche Speichertypen.

Beispiele:

* DDR
* HBM
* CXL Memory
* Persistent Memory

Policies können unterschiedliche Speicherklassen berücksichtigen.

Moderne NUMA-Systeme können mehrere Speichertypen mit unterschiedlichen Latenzen und Bandbreiten innerhalb derselben Topologie enthalten.

---

# 24. Sicherheit

Die NUMA Memory Policy schützt vor:

* ungültigen Knotenzuweisungen
* inkonsistenten Policies
* fehlerhaften Migrationen
* Speicherverlust
* Race Conditions

---

# 25. Performance

Die Architektur soll ermöglichen:

* minimale Speicherlatenzen
* maximale Speicherbandbreite
* geringe Interconnect-Auslastung
* hohe Skalierbarkeit
* effiziente Mehrsockelsysteme

---

# 26. Debug-Unterstützung

Optional unterstützt NovaOS:

* NUMA-Topologiediagramme
* Knotenstatistiken
* Seitenmigrationsprotokolle
* Speicherheatmaps
* Policy-Dumps
* Performanceanalysen

---

# 27. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* KI-gestützte Policy-Auswahl
* automatische Workload-Klassifizierung
* adaptive Speicherhierarchien
* CXL Fabric Memory
* Remote Memory
* verteilte NUMA-Systeme

---

# 28. Auswirkungen

Positive Auswirkungen:

* geringere Speicherlatenzen
* bessere Cache-Lokalität
* höhere Speicherbandbreite
* bessere Skalierung
* optimale Serverleistung
* moderne NUMA-Unterstützung

Negative Auswirkungen:

* komplexere Speicherverwaltung
* zusätzlicher Verwaltungsaufwand
* aufwendigere Seitenmigration

---

# 29. Verworfene Alternativen

## Ausschließlich lokale Allokation

Vorteile:

* einfache Implementierung

Nachteile:

* schlechte Lastverteilung
* unausgewogene Speichernutzung

---

## Ausschließlich Interleave

Vorteile:

* gleichmäßige Lastverteilung

Nachteile:

* höhere Zugriffszeiten
* schlechtere Cache-Lokalität

---

## Keine NUMA-Unterstützung

Vorteile:

* geringe Implementierungskomplexität

Nachteile:

* deutliche Leistungseinbußen
* ungeeignet für moderne Mehrsockelsysteme

---

# 30. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* NUMA Policies ausschließlich über den NUMA Manager verwalten,
* keine direkten Knotenzuweisungen vornehmen,
* Seitenmigrationen zentral koordinieren,
* Hardwareinformationen ausschließlich über die HAL beziehen,
* sämtliche Policy-Änderungen atomar durchführen.

---

# 31. Teststrategie

Die NUMA Memory Policy wird geprüft durch:

* Unit-Tests
* Knotenzuweisungstests
* Interleave-Tests
* Migrations-Tests
* Mehrkerntests
* Hotplug-Tests
* Lasttests
* Performancebenchmarks

---

# 32. Konsequenzen

Die NUMA Memory Policy wird zur zentralen Entscheidungsinstanz für sämtliche Speicherplatzierungen innerhalb von NovaOS.

Durch die enge Zusammenarbeit mit Scheduler, Physical Memory Manager, Virtual Memory Manager, Memory Hotplug und Memory Reclamation entsteht eine adaptive Speicherarchitektur, die automatisch die optimale Balance zwischen Speicherlatenz, Bandbreite und Ressourcenauslastung findet.

---

# 33. Zukunftsperspektive

Langfristig soll die NUMA Memory Policy erweitert werden um:

* selbstlernende Optimierungsalgorithmen
* KI-gestützte Speicherplatzierung
* automatische Analyse von Workload-Profilen
* adaptive Speicherhierarchien
* Cloud- und Cluster-NUMA
* Telemetriegestützte Laufzeitoptimierung

---

# 34. Zusammenfassung

NovaOS implementiert eine **NUMA Memory Policy** als flexible und mehrstufige Strategie zur Platzierung physischer Speicherseiten auf NUMA-Systemen. Neben lokalen, bevorzugten, gebundenen und interleavten Richtlinien unterstützt die Architektur adaptive Entscheidungen sowie automatische Seitenmigration. Durch die Integration mit Virtual Memory Regions, Scheduler, Memory Hotplug und Memory Reclamation entsteht eine leistungsfähige, skalierbare und zukunftssichere Speicherverwaltung für moderne Mehrsockel- und Hochleistungssysteme. Die Architektur erlaubt Richtlinien auf System-, Prozess- und Bereichsebene und priorisiert standardmäßig lokale Speicherallokationen zur Minimierung der Zugriffszeiten.
