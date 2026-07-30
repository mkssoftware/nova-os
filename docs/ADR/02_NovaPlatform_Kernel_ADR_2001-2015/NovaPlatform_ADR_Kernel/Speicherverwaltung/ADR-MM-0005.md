# ADR-MM-0005: Shared Memory

* **ADR-ID:** ADR-MM-0005
* **Titel:** Shared Memory
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Memory Management
* **Gültigkeitsbereich:** Virtual Memory Manager (VMM), IPC, Process Manager, Object Manager, Scheduler
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-MM-0004, ADR-KERNEL-0009, ADR-KERNEL-0011, ADR-KERNEL-0013

---

# 1. Zusammenfassung

NovaOS implementiert **Shared Memory** als primären Mechanismus für den schnellen Datenaustausch zwischen Prozessen.

Ein Shared-Memory-Objekt besteht aus einem Satz physischer Speicherseiten, die gleichzeitig in die virtuellen Adressräume mehrerer Prozesse eingeblendet werden können. Dadurch können große Datenmengen ohne Kopieroperationen gemeinsam genutzt werden. Synchronisation erfolgt bewusst getrennt vom Speichermechanismus über Kernelobjekte wie Mutexe, Semaphoren oder Event-Objekte. Shared Memory gehört zu den schnellsten Formen der Interprozesskommunikation, da Prozesse direkt auf denselben Speicherbereich zugreifen.

---

# 2. Kontext

Viele Anwendungen müssen Daten gemeinsam verwenden.

Beispiele:

* Multimedia
* Datenbanken
* Browser
* KI-Modelle
* Grafiksysteme
* IPC
* Dateicache
* gemeinsam genutzte Bibliotheken

Ein Kopieren großer Datenmengen wäre ineffizient.

---

# 3. Problemstellung

Es musste entschieden werden,

* wie Speicher zwischen Prozessen geteilt wird,
* wie Rechte verwaltet werden,
* wie Lebensdauer geregelt wird,
* wie Synchronisation erfolgt,
* und wie Sicherheit gewährleistet wird.

---

# 4. Entscheidung

NovaOS implementiert Shared Memory als eigenständiges Kernelobjekt.

Dabei gilt:

* Shared Memory besitzt einen eigenen Objekttyp.
* Die physische Speichermenge existiert nur einmal.
* Jeder Prozess erhält seine eigene virtuelle Einblendung.
* Rechte werden pro Mapping vergeben.
* Synchronisation erfolgt getrennt.

---

# 5. Motivation

Diese Architektur bietet:

* extrem schnelle IPC
* keine Datenkopien
* geringe CPU-Last
* flexible Speicherfreigabe
* hohe Skalierbarkeit
* effiziente Mehrkernnutzung

---

# 6. Designprinzipien

Shared Memory basiert auf:

* Zero-Copy
* Objektorientierung
* Capability-Sicherheit
* Referenzzählung
* virtuellem Mapping
* Modularität

---

# 7. Architekturübersicht

```text
           Shared Memory Object

                    │

        +-----------+-----------+

        ▼                       ▼

   Physische Seiten       Referenzzähler

        │

        ▼

 +------+-------+--------+

 ▼              ▼        ▼

Prozess A   Prozess B   Prozess C

      Virtuelle Einblendungen
```

---

# 8. Shared Memory Objekt

Ein Shared-Memory-Objekt besitzt mindestens:

* Objekt-ID
* Größe
* Referenzzähler
* Eigentümer
* Capability-Liste
* Flags
* Seitensammlung
* Sicherheitsattribute

---

# 9. Lebenszyklus

Ein Shared-Memory-Objekt besitzt folgende Zustände:

* Created
* Mapped
* Shared
* Unmapped
* Destroying
* Destroyed

Das Objekt wird automatisch entfernt, sobald keine Referenzen mehr existieren.

---

# 10. Erzeugung

Beim Erzeugen werden:

* Kernelobjekt erzeugt
* virtuelle Region reserviert
* physische Seiten vorbereitet
* Rechte definiert
* Referenzzähler initialisiert

Die eigentlichen Seiten können bei Bedarf per Demand Paging angelegt werden.

---

# 11. Mapping

Ein Prozess kann ein Shared-Memory-Objekt an beliebiger freier virtueller Adresse einblenden.

Dabei bleiben:

* physische Seiten identisch,
* virtuelle Adressen unabhängig.

Mehrere Prozesse können dieselben physischen Seiten unter unterschiedlichen virtuellen Adressen einblenden.

---

# 12. Unmapping

Beim Entfernen eines Mappings:

* virtuelle Region löschen
* Referenzzähler reduzieren
* TLB aktualisieren
* Objektzustand prüfen

Die physische Speichermenge bleibt erhalten, solange weitere Referenzen existieren.

---

# 13. Referenzzählung

Jedes Shared-Memory-Objekt besitzt einen atomaren Referenzzähler.

Das Objekt wird automatisch freigegeben, wenn:

* keine virtuellen Einblendungen,
* keine Handles,
* keine Kernelreferenzen

mehr vorhanden sind.

---

# 14. Zugriffsrechte

Rechte werden pro Mapping vergeben.

Unterstützt werden:

* Read
* Write
* Execute
* Copy-on-Write
* Read Only
* Kernel Only
* User Only

Dadurch kann derselbe Speicher für verschiedene Prozesse unterschiedliche Schutzattribute besitzen.

---

# 15. Synchronisation

Shared Memory übernimmt **keine** Synchronisation.

NovaOS verwendet dafür:

* Mutex
* Semaphore
* Read-Write-Lock
* Event
* Condition Variable

Diese Trennung verhindert unnötige Komplexität und entspricht dem etablierten Betriebssystemdesign. Shared Memory selbst koordiniert konkurrierende Zugriffe nicht; Synchronisation erfolgt über separate Mechanismen.

---

# 16. Copy-on-Write

Optional kann ein Mapping als Copy-on-Write erstellt werden.

Beim ersten Schreibzugriff:

* neue physische Seite erzeugen
* Daten kopieren
* Seitentabelle aktualisieren

Andere Prozesse bleiben unverändert.

---

# 17. NUMA-Unterstützung

Bei NUMA-Systemen bevorzugt NovaOS:

* lokalen Speicher
* CPU-nahe Seiten
* NUMA-optimierte Platzierung

---

# 18. Mehrkernbetrieb

Shared Memory unterstützt:

* parallele Zugriffe
* CPU-lokale TLB-Optimierungen
* SMP
* CPU Hotplug

Alle Synchronisationsmechanismen sind vollständig mehrkernfähig.

---

# 19. Kernelintegration

Shared Memory wird verwendet durch:

* IPC
* Grafiksystem
* Dateicache
* Netzwerk
* KI-Subsystem
* Multimedia
* Shared Libraries

---

# 20. Speicherverwaltung

Der Virtual Memory Manager übernimmt:

* Mapping
* Unmapping
* Rechteänderungen
* Seitentabellen
* Page Faults

Der Shared-Memory-Manager verwaltet ausschließlich das Objekt selbst.

---

# 21. Sicherheit

Shared Memory schützt vor:

* unberechtigtem Zugriff
* Mapping fremder Objekte
* Rechteeskalation
* doppelter Freigabe
* inkonsistenten Referenzen

Der Zugriff erfolgt ausschließlich über Capability-basierte Handles.

---

# 22. Performance

Die Architektur soll ermöglichen:

* Zero-Copy-Kommunikation
* minimale Latenz
* geringe CPU-Last
* hohe Bandbreite
* sehr gute Mehrkernskalierung

---

# 23. Debug-Unterstützung

Optional unterstützt NovaOS:

* Mapping-Historie
* Referenzanalyse
* Leak Detection
* Objektstatistiken
* Seitentracing
* Zugriffsstatistiken

---

# 24. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* verschlüsselte Shared-Memory-Bereiche
* CXL Memory
* GPU Shared Memory
* Hardware Memory Tagging
* Persistente Shared Memory Objekte
* verteiltes Shared Memory

---

# 25. Auswirkungen

Positive Auswirkungen:

* extrem schnelle IPC
* keine Datenkopien
* geringe CPU-Belastung
* hohe Skalierbarkeit
* effiziente Speichernutzung
* moderne Architektur

Negative Auswirkungen:

* Synchronisation notwendig
* komplexere Rechteverwaltung
* zusätzlicher Verwaltungsaufwand

---

# 26. Verworfene Alternativen

## Message Passing ausschließlich

Vorteile:

* einfache Synchronisation

Nachteile:

* hohe Kopierkosten
* geringe Bandbreite
* hohe CPU-Last

---

## Dateibasierter Datenaustausch

Vorteile:

* einfache Implementierung

Nachteile:

* langsam
* hoher I/O-Aufwand
* ungeeignet für Echtzeitkommunikation

---

## Gemeinsamer globaler Adressraum

Vorteile:

* keine Mappingverwaltung

Nachteile:

* fehlende Isolation
* erhebliche Sicherheitsprobleme
* ungeeignet für moderne Betriebssysteme

---

# 27. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* Shared Memory ausschließlich über den Shared-Memory-Manager erzeugen,
* Zugriffe ausschließlich über Capability-Handles durchführen,
* Synchronisation getrennt behandeln,
* Referenzzählung korrekt verwenden,
* keine direkten Seitentabellenmanipulationen durchführen.

---

# 28. Teststrategie

Shared Memory wird geprüft durch:

* Unit-Tests
* Mapping-Tests
* Unmapping-Tests
* Mehrprozess-Tests
* Mehrkerntests
* Copy-on-Write-Tests
* Rechteprüfungen
* Langzeittests

---

# 29. Konsequenzen

Shared Memory wird zur Standardlösung für den hochperformanten Datenaustausch zwischen Prozessen innerhalb von NovaOS.

Alle zukünftigen IPC-Komponenten, Grafiksysteme, Multimedia-Frameworks, KI-Subsysteme und gemeinsam genutzten Bibliotheken können denselben physisch vorhandenen Speicher effizient gemeinsam verwenden. Durch die Kombination aus Virtual Memory Regions, Capability-Sicherheit und Referenzzählung entsteht eine sichere, skalierbare und leistungsfähige Architektur.

---

# 30. Zusammenfassung

NovaOS implementiert **Shared Memory** als objektbasierten Mechanismus für den gemeinsamen Zugriff mehrerer Prozesse auf dieselben physischen Speicherseiten. Jeder Prozess erhält eine eigene virtuelle Einblendung mit individuell festgelegten Zugriffsrechten, während Synchronisation bewusst über separate Kernelobjekte erfolgt. Durch Zero-Copy-Kommunikation, Referenzzählung und Capability-basierte Zugriffskontrolle entsteht eine moderne, sichere und hochperformante Grundlage für Interprozesskommunikation und gemeinsam genutzte Daten.
