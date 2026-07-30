# ADR-KERNEL-0010: NUMA-Unterstützung

* **ADR-ID:** ADR-KERNEL-0010
* **Titel:** NUMA-Unterstützung
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel-Architektur
* **Gültigkeitsbereich:** Memory Manager, Scheduler, Process Manager, HAL
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0003, ADR-KERNEL-0008, ADR-KERNEL-0009, NPSPEC-KERNEL-0028

---

# 1. Zusammenfassung

NovaOS besitzt eine **native NUMA-Unterstützung (Non-Uniform Memory Access)**.

Der Kernel erkennt NUMA-Topologien automatisch und berücksichtigt diese bei der Speicherzuweisung, Threadplanung und Lastverteilung. Ziel ist es, Speicherzugriffe möglichst auf den lokalen NUMA-Knoten eines Prozessors zu halten, um Speicherlatenzen zu reduzieren und die Skalierbarkeit auf Mehrsockelsystemen deutlich zu verbessern. NUMA-Systeme unterscheiden zwischen lokalem und entferntem Speicher, wobei lokaler Speicher mit geringerer Latenz erreichbar ist.

---

# 2. Kontext

Moderne Mehrprozessorsysteme besitzen häufig mehrere Speichercontroller.

Dadurch entstehen:

* lokale Speicherbereiche
* entfernte Speicherbereiche
* unterschiedliche Speicherlatenzen
* unterschiedliche Speicherbandbreiten

Ein NUMA-bewusster Kernel kann diese Unterschiede zur Leistungssteigerung nutzen.

---

# 3. Problemstellung

Es musste entschieden werden,

* ob NovaOS NUMA unterstützt,
* wie Speicherknoten verwaltet werden,
* wie Threads platziert werden,
* wie Speicher migriert werden kann,
* und wie sich das System auf Nicht-NUMA-Hardware verhält.

---

# 4. Entscheidung

NovaOS implementiert eine vollständige NUMA-Unterstützung.

Dabei gilt:

* automatische Erkennung der NUMA-Topologie,
* Verwaltung mehrerer Speicherknoten,
* bevorzugte Allokation aus lokalem Speicher,
* NUMA-bewusstes Scheduling,
* vollständige Transparenz für Anwendungen.

---

# 5. Motivation

Diese Architektur bietet:

* geringere Speicherlatenzen
* höhere Speicherbandbreite
* bessere Skalierbarkeit
* höhere Mehrkernleistung
* effizientere Threadplatzierung
* bessere Energieeffizienz

---

# 6. Designprinzipien

Die NUMA-Unterstützung basiert auf:

* Locality First
* Transparenz
* Skalierbarkeit
* automatischer Optimierung
* Modularität
* Architekturunabhängigkeit

---

# 7. Architekturübersicht

```text
                 NUMA-System

      +-------------------------------+

      |           CPU 0               |

      |       Lokaler Speicher        |

      +---------------+---------------+

                      │

          Hochgeschwindigkeitslink

                      │

      +---------------+---------------+

      |           CPU 1               |

      |       Lokaler Speicher        |

      +-------------------------------+

               NovaOS Kernel

       NUMA-Aware Scheduler

       NUMA-Aware Memory Manager
```

---

# 8. NUMA-Knoten

Ein NUMA-Knoten besteht mindestens aus:

* Prozessor(en)
* lokalem Arbeitsspeicher
* lokalen Geräten
* Knoteninformationen

Die genaue Topologie wird durch Firmware und Hardware vorgegeben.

---

# 9. Topologieerkennung

Während des Systemstarts erkennt der Kernel automatisch:

* Anzahl der NUMA-Knoten
* CPUs je Knoten
* Speicherbereiche
* Knotendistanzen
* Interconnects

---

# 10. Knotendistanzen

Zwischen zwei NUMA-Knoten existiert eine Distanzmetrik.

Beispiel:

```text
Node 0 → Node 0 = 10

Node 0 → Node 1 = 20

Node 0 → Node 2 = 30
```

Kleinere Werte bedeuten geringere Speicherlatenz.

---

# 11. Lokale Speicherallokation

Standardmäßig werden Speicherseiten auf dem NUMA-Knoten allokiert, auf dem der ausführende Thread läuft.

Dieses Prinzip wird als **Local Allocation Policy** verwendet.

---

# 12. Entfernte Speicherzugriffe

Ist lokaler Speicher nicht verfügbar, darf Speicher von benachbarten NUMA-Knoten verwendet werden.

Die Auswahl erfolgt anhand der geringsten Knotendistanz.

---

# 13. NUMA-Aware Scheduler

Der Scheduler berücksichtigt:

* Threadstandort
* Speicherstandort
* CPU-Auslastung
* Knotenauslastung
* Affinitäten

Ziel ist die Minimierung entfernter Speicherzugriffe.

---

# 14. CPU-Affinität

Threads können:

* frei migrieren,
* bevorzugte NUMA-Knoten besitzen,
* fest an einen NUMA-Knoten gebunden werden.

---

# 15. Speichermigration

Der Kernel kann Speicherseiten zwischen NUMA-Knoten verschieben.

Dies erfolgt ausschließlich, wenn dadurch eine langfristige Leistungsverbesserung zu erwarten ist.

---

# 16. Threadmigration

Auch Threads können zwischen NUMA-Knoten migrieren.

Dabei bewertet der Scheduler:

* Speicherlokalität
* CPU-Auslastung
* Interconnect-Kosten
* Cache-Lokalität

---

# 17. Shared Memory

Gemeinsam genutzter Speicher kann:

* lokal,
* interleaved,
* oder explizit einem NUMA-Knoten zugeordnet werden.

---

# 18. Speicherinterleaving

Optional unterstützt NovaOS eine Interleaving-Strategie.

Speicherseiten werden dabei gleichmäßig auf mehrere NUMA-Knoten verteilt.

Dies eignet sich insbesondere für stark parallelisierte Anwendungen.

---

# 19. Speicherpolitik

Der Kernel unterstützt verschiedene NUMA-Richtlinien.

Beispiele:

* Local
* Preferred
* Interleaved
* Fixed Node

Die konkrete API wird in einer separaten NPSPEC definiert.

---

# 20. Kernelobjekte

Kernelinterne Objekte werden möglichst auf dem NUMA-Knoten erzeugt, auf dem sie überwiegend verwendet werden.

---

# 21. DMA

DMA-Speicher wird bevorzugt auf dem NUMA-Knoten des jeweiligen Gerätes reserviert.

---

# 22. Mehrarchitektur-Unterstützung

Die NUMA-Unterstützung gilt für:

* x86
* x86_64
* ARM64
* RISC-V

Architekturspezifische Unterschiede werden innerhalb der HAL gekapselt.

---

# 23. Systeme ohne NUMA

Auf Systemen ohne NUMA arbeitet NovaOS automatisch im UMA-Modus.

Der Kernel verwendet intern weiterhin dieselben Schnittstellen.

Dadurch entsteht kein zusätzlicher Programmieraufwand für Kernelkomponenten.

---

# 24. Synchronisation

NUMA-spezifische Verwaltungsdaten werden thread-sicher verwaltet.

Dabei kommen unter anderem zum Einsatz:

* Spinlocks
* atomare Operationen
* CPU-lokale Datenstrukturen

---

# 25. Performance

Die NUMA-Unterstützung soll:

* entfernte Speicherzugriffe minimieren,
* lokale Speicherzugriffe maximieren,
* Speicherbandbreite erhöhen,
* Scheduler-Migrationen optimieren.

---

# 26. Sicherheit

NUMA verändert keinerlei Sicherheitsmodell.

Prozessisolierung, Speicherrechte und Zugriffskontrollen bleiben unverändert bestehen.

---

# 27. Debug-Unterstützung

Optional unterstützt NovaOS:

* NUMA-Statistiken
* Knotenauslastung
* Speichermigrationen
* Remote-Zugriffe
* Scheduler-Analysen
* Topologie-Dumps

---

# 28. Erweiterbarkeit

Die Architektur ermöglicht zukünftige Erweiterungen:

* automatische Speicheroptimierung
* KI-gestützte Platzierungsalgorithmen
* CXL-Speicher
* Memory Tiering
* Hotplug von NUMA-Knoten

---

# 29. Auswirkungen

Positive Auswirkungen:

* bessere Skalierbarkeit
* geringere Speicherlatenz
* höhere Speicherbandbreite
* bessere Mehrkernleistung
* optimale Nutzung großer Serversysteme

Negative Auswirkungen:

* höhere Implementierungskomplexität
* zusätzlicher Verwaltungsaufwand
* Optimierungsentscheidungen verursachen Laufzeitkosten

---

# 30. Verworfene Alternativen

## Keine NUMA-Unterstützung

Vorteile:

* einfachere Implementierung

Nachteile:

* deutlich geringere Leistung auf NUMA-Systemen
* schlechte Skalierbarkeit

---

## Vollständig manuelle NUMA-Verwaltung

Vorteile:

* maximale Kontrolle

Nachteile:

* hohe Komplexität
* fehleranfällig
* ungeeignet für allgemeine Anwendungen

---

## Starre CPU-Speicher-Zuordnung

Vorteile:

* einfache Verwaltung

Nachteile:

* geringe Flexibilität
* schlechte Lastverteilung

---

# 31. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* NUMA-Knoten berücksichtigen,
* lokale Speicherallokation bevorzugen,
* keine Annahmen über eine einheitliche Speicherlatenz treffen,
* NUMA-Informationen ausschließlich über Kernel-APIs beziehen.

---

# 32. Teststrategie

Die NUMA-Unterstützung wird geprüft durch:

* Unit-Tests
* Topologietests
* Speicherlokalitätstests
* Threadmigrationstests
* Performancebenchmarks
* Lasttests
* Langzeittests

---

# 33. Konsequenzen

Die NUMA-Unterstützung bildet die Grundlage für den Betrieb von NovaOS auf modernen Mehrsockel- und Hochleistungssystemen.

Alle zukünftigen Komponenten des Memory Managers, Schedulers und Process Managers müssen NUMA-Topologien berücksichtigen. Gleichzeitig bleibt die Architektur vollständig kompatibel zu klassischen UMA-Systemen, wodurch NovaOS ohne funktionale Einschränkungen sowohl auf älteren Desktoprechnern als auch auf modernen Serverplattformen eingesetzt werden kann.

---

# 34. Zusammenfassung

NovaOS implementiert eine **native NUMA-Unterstützung**, die Speicherverwaltung und Threadplanung an die physische Speicher- und Prozessortopologie anpasst. Durch lokale Speicherallokation, NUMA-bewusstes Scheduling, Speichermigration und flexible Speicherpolitiken werden Speicherlatenzen reduziert und die Skalierbarkeit auf Mehrprozessorsystemen verbessert, ohne die Kompatibilität zu klassischen UMA-Systemen zu beeinträchtigen.
