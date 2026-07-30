# ADR-KERNEL-0001: Monolithischer Kernel mit modularer Architektur

* **ADR-ID:** ADR-KERNEL-0001
* **Titel:** Monolithischer Kernel mit modularer Architektur
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel-Architektur
* **Gültigkeitsbereich:** Gesamter NovaOS-Kernel
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-BOOT-1020, RFC-BOOT-0002, RFC-BOOT-0005, NPSPEC-KERNEL-0001

---

# 1. Zusammenfassung

NovaOS verwendet einen **monolithischen Kernel**, dessen interne Architektur konsequent modular aufgebaut ist. Alle Kernkomponenten laufen im privilegierten Kernelmodus, sind jedoch klar voneinander getrennt und kommunizieren ausschließlich über definierte interne Schnittstellen.

Dadurch werden die Leistungsfähigkeit eines klassischen monolithischen Kernels mit der Wartbarkeit einer modularen Architektur kombiniert.

---

# 2. Kontext

Die Kernelarchitektur gehört zu den grundlegendsten Entscheidungen eines Betriebssystems.

Sie beeinflusst unter anderem:

* Performance
* Speicherverwaltung
* Treiberarchitektur
* Sicherheit
* Wartbarkeit
* Erweiterbarkeit
* Debugbarkeit
* Bootprozess

Da NovaOS langfristig sowohl Desktop-, Server-, Embedded- als auch Robotiksysteme unterstützen soll, muss die Architektur hohe Leistung mit klaren internen Strukturen verbinden.

---

# 3. Problemstellung

Es musste entschieden werden, welche Kernelarchitektur NovaOS verwendet.

Mögliche Alternativen:

* Monolithischer Kernel
* Mikrokernel
* Hybridkernel
* Exokernel
* Unikernel

Jede dieser Architekturen besitzt unterschiedliche Eigenschaften hinsichtlich Performance, Komplexität und Wartbarkeit.

---

# 4. Entscheidung

NovaOS verwendet einen **monolithischen Kernel mit modularer Architektur**.

Das bedeutet:

* Alle Kernkomponenten laufen im Kernelmodus.
* Komponenten sind logisch voneinander getrennt.
* Kommunikation erfolgt ausschließlich über definierte Kernel-APIs.
* Interne Datenstrukturen werden nicht direkt geteilt.
* Erweiterungen erfolgen über klar definierte Subsysteme.

---

# 5. Motivation

Diese Architektur bietet mehrere Vorteile:

* minimale Kontextwechsel
* geringe Latenz
* hoher Datendurchsatz
* direkte Kommunikation zwischen Subsystemen
* einfache Optimierung kritischer Pfade
* gute Skalierbarkeit
* klare Verantwortlichkeiten

---

# 6. Designprinzipien

Der NovaOS-Kernel basiert auf folgenden Prinzipien:

* Modularität
* Kapselung
* Versionierte interne Schnittstellen
* Objektorientiertes Kernelmodell
* Klare Verantwortlichkeiten
* Erweiterbarkeit
* Deterministisches Verhalten

---

# 7. Architekturübersicht

```text
                   +-----------------------------+
                   |        System Calls         |
                   +-------------+---------------+
                                 |
                                 v
+---------------------------------------------------------------+
|                        Nova Kernel                            |
|                                                               |
| +----------------+ +----------------+ +---------------------+ |
| | Process Manager| | Memory Manager | | Scheduler           | |
| +----------------+ +----------------+ +---------------------+ |
|                                                               |
| +----------------+ +----------------+ +---------------------+ |
| | Object Manager | | Device Manager | | Security Manager    | |
| +----------------+ +----------------+ +---------------------+ |
|                                                               |
| +----------------+ +----------------+ +---------------------+ |
| | VFS            | | IPC            | | Network             | |
| +----------------+ +----------------+ +---------------------+ |
|                                                               |
+---------------------------------------------------------------+
                                 |
                                 v
                    Hardware Abstraction Layer
                                 |
                                 v
                              Hardware
```

---

# 8. Kernelsubsysteme

Der Kernel besteht mindestens aus folgenden Subsystemen:

* Hardware Abstraction Layer
* Scheduler
* Process Manager
* Thread Manager
* Physical Memory Manager
* Virtual Memory Manager
* Object Manager
* Handle Manager
* IPC
* Virtual File System
* Device Manager
* Driver Framework
* Security Manager
* Timer
* Netzwerkstack
* Power Manager
* Logging

Jedes Subsystem besitzt eine klar definierte Verantwortung.

---

# 9. Modulbegriff

Ein Kernelmodul ist eine logisch abgeschlossene Funktionseinheit innerhalb des Kernels.

Beispiele:

* Scheduler
* VFS
* Netzwerkstack
* Speicherverwaltung

Module sind **keine** separat ladbaren Kernelmodule im Sinne von Linux-Kernelmodulen, sondern Bestandteile eines gemeinsamen Kernelabbilds.

Eine spätere Unterstützung dynamisch ladbarer Kernelmodule wird dadurch nicht ausgeschlossen.

---

# 10. Kommunikationsmodell

Subsysteme kommunizieren ausschließlich über dokumentierte interne Schnittstellen.

Nicht zulässig sind:

* direkte Zugriffe auf private Datenstrukturen
* globale Zustände ohne Synchronisation
* zyklische Abhängigkeiten

---

# 11. Objektmodell

Der Kernel verwendet ein gemeinsames Objektmodell.

Beispiele:

* Prozess
* Thread
* Datei
* Verzeichnis
* Gerät
* Ereignis
* Timer
* Socket

Jedes Objekt besitzt einen eindeutigen Typ und einen definierten Lebenszyklus.

---

# 12. Speicherverwaltung

Die Speicherverwaltung ist vollständig als eigenes Subsystem implementiert.

Andere Kernelkomponenten greifen ausschließlich über definierte Speicher-APIs darauf zu.

---

# 13. Scheduler

Der Scheduler besitzt keine Kenntnisse über interne Implementierungsdetails anderer Subsysteme.

Er arbeitet ausschließlich mit standardisierten Thread- und Prozessobjekten.

---

# 14. Geräteverwaltung

Treiber kommunizieren über den Device Manager.

Direkte Kommunikation zwischen unabhängigen Treibern ist nicht vorgesehen.

---

# 15. Sicherheitsmodell

Sicherheitsprüfungen werden zentral durchgeführt.

Subsysteme implementieren keine voneinander abweichenden Sicherheitsmechanismen.

---

# 16. Fehlerisolierung

Ein Fehler in einem Subsystem darf nicht zu inkonsistenten Zuständen anderer Kernelkomponenten führen.

Dazu werden:

* definierte Rückgabecodes,
* Assertions,
* Validierungen
* und Panic-Richtlinien

verwendet.

---

# 17. Synchronisation

Gemeinsam genutzte Ressourcen werden ausschließlich über definierte Synchronisationsmechanismen geschützt.

Beispiele:

* Spinlocks
* Mutexe
* Reader-Writer-Locks
* Atomare Operationen

---

# 18. Erweiterbarkeit

Neue Subsysteme können ergänzt werden, sofern:

* bestehende Schnittstellen erhalten bleiben,
* keine zyklischen Abhängigkeiten entstehen,
* und die Modulgrenzen eingehalten werden.

---

# 19. Auswirkungen

Positive Auswirkungen:

* sehr hohe Performance
* geringe Latenzen
* einfacher Datenaustausch
* gute Debugbarkeit
* geringer Overhead

Negative Auswirkungen:

* Fehler im Kernel können das Gesamtsystem betreffen.
* Höhere Anforderungen an Codequalität und Tests.
* Sicherheitsfehler besitzen größere Auswirkungen als bei einem Mikrokernel.

---

# 20. Verworfene Alternativen

## Mikrokernel

Vorteile:

* sehr gute Isolation
* hohe Fehlertoleranz

Nachteile:

* hoher IPC-Aufwand
* höhere Latenzen
* größere Komplexität

---

## Hybridkernel

Vorteile:

* flexible Architektur

Nachteile:

* uneinheitliche Trennung
* höherer Implementierungsaufwand
* schwieriger zu analysieren

---

## Exokernel

Vorteile:

* maximale Flexibilität

Nachteile:

* sehr komplex
* hoher Entwicklungsaufwand
* geringe Werkzeugunterstützung

---

## Unikernel

Vorteile:

* minimale Größe

Nachteile:

* ungeeignet als universelles Desktopbetriebssystem

---

# 21. Implementierungsrichtlinien

Alle neuen Kernelkomponenten müssen:

* einem klaren Subsystem zugeordnet sein,
* definierte Schnittstellen besitzen,
* dokumentiert werden,
* Unit-Tests ermöglichen,
* keine versteckten globalen Abhängigkeiten erzeugen.

---

# 22. Kompatibilität

Die modulare Architektur darf zukünftige Erweiterungen nicht behindern.

Neue Funktionen müssen ohne Änderungen bestehender Subsysteme integrierbar sein, sofern deren öffentliche Schnittstellen ausreichend sind.

---

# 23. Teststrategie

Die Architektur wird validiert durch:

* Unit-Tests
* Integrationstests
* Schnittstellentests
* Lasttests
* Stresstests
* Langzeittests
* Codeanalysen

---

# 24. Konsequenzen

Diese Entscheidung bildet die Grundlage für sämtliche Kernelentwicklungen von NovaOS.

Alle zukünftigen ADRs und NPSPEC-Dokumente müssen mit dieser Architekturentscheidung vereinbar sein. Änderungen an dieser Entscheidung haben weitreichende Auswirkungen auf nahezu alle Kernelkomponenten und bedürfen einer neuen ADR mit entsprechender Migrationsstrategie.

---

# 25. Zusammenfassung

NovaOS setzt auf einen **monolithischen Kernel mit einer streng modularen internen Architektur**. Dieses Design verbindet die hohe Performance und geringe Latenz eines klassischen monolithischen Kernels mit einer klar strukturierten Aufteilung in unabhängige Subsysteme. Durch definierte Schnittstellen, ein gemeinsames Objektmodell und konsequente Kapselung entsteht eine langfristig wartbare, erweiterbare und leistungsfähige Kernelbasis, die als Fundament für alle weiteren Komponenten des NovaOS-Kernels dient.
