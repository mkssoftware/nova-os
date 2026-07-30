# ADR-MM-0004: Virtual Memory Regions

* **ADR-ID:** ADR-MM-0004
* **Titel:** Virtual Memory Regions
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Memory Management
* **Gültigkeitsbereich:** Virtual Memory Manager (VMM), Process Manager, Loader, IPC, Shared Memory, Memory Mapping
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0009, ADR-MM-0001, ADR-MM-0003, ADR-KERNEL-0014

---

# 1. Zusammenfassung

NovaOS verwaltet den virtuellen Adressraum jedes Prozesses über **Virtual Memory Regions (VMRs)**.

Eine Virtual Memory Region beschreibt einen zusammenhängenden virtuellen Adressbereich mit identischen Eigenschaften wie Zugriffsrechten, Speicherquelle, Cache-Verhalten und Lebensdauer. Der Virtual Memory Manager verwaltet ausschließlich Regionen; die eigentlichen Seitenzuordnungen erfolgen unabhängig davon über die Seitentabellen. Moderne Betriebssysteme verwalten virtuelle Adressräume über Regionen (VMAs), die zusammenhängende Bereiche mit identischen Eigenschaften beschreiben.

---

# 2. Kontext

Ein Prozess besteht aus mehreren logisch getrennten Speicherbereichen.

Beispiele:

* Programmcode
* Konstanten
* globale Daten
* Heap
* Stack
* Shared Memory
* Memory Mappings
* JIT-Code
* MMIO

Diese Bereiche besitzen unterschiedliche Eigenschaften.

---

# 3. Problemstellung

Es musste entschieden werden,

* wie virtuelle Speicherbereiche beschrieben werden,
* wie Regionen verwaltet werden,
* wie Zugriffsrechte definiert werden,
* wie Dateimappings integriert werden,
* und wie spätere Erweiterungen möglich bleiben.

---

# 4. Entscheidung

NovaOS führt Virtual Memory Regions als zentrale Verwaltungseinheit des virtuellen Adressraums ein.

Dabei gilt:

* jede Region beschreibt genau einen zusammenhängenden Adressbereich,
* alle Seiten innerhalb einer Region besitzen identische Eigenschaften,
* Regionen sind unabhängig von der eigentlichen Seitentabelle,
* Regionen können dynamisch erzeugt, erweitert, geteilt und entfernt werden.

---

# 5. Motivation

Diese Architektur bietet:

* klare Trennung zwischen virtueller Struktur und physischer Abbildung
* einfache Speicherverwaltung
* effiziente Rechteprüfung
* flexible Speicherabbildungen
* Unterstützung moderner Speichertechniken
* hohe Erweiterbarkeit

---

# 6. Designprinzipien

Die Virtual Memory Regions basieren auf:

* Modularität
* Ereignisorientierung
* Seitenunabhängigkeit
* klaren Zuständigkeiten
* geringer Fragmentierung
* deterministischen Operationen

---

# 7. Architekturübersicht

```text
               Prozess
                  │
                  ▼
       Virtual Address Space
                  │
      +-----------+-----------+
      │           │           │
      ▼           ▼           ▼
     VMR         VMR         VMR
      │           │           │
      └-----------┬-----------┘
                  │
                  ▼
        Virtual Memory Manager
                  │
                  ▼
            Seitentabellen
                  │
                  ▼
         Physischer Speicher
```

---

# 8. Virtual Memory Region

Eine Virtual Memory Region besteht mindestens aus:

* Startadresse
* Endadresse
* Größe
* Rechte
* Regionstyp
* Speicherquelle
* Flags
* Eigentümer
* Referenzzähler

---

# 9. Regionstypen

NovaOS unterstützt unter anderem:

* Code
* Read Only Data
* Data
* Heap
* Stack
* Shared Memory
* File Mapping
* Anonymous Memory
* MMIO
* Guard Region
* Kernel Region

Weitere Typen können ergänzt werden.

---

# 10. Zugriffsrechte

Jede Region besitzt eigene Schutzattribute.

Beispiele:

* Read
* Write
* Execute
* Copy-on-Write
* Shared
* User
* Kernel
* No Access

Alle Seiten einer Region besitzen dieselben Grundattribute.

---

# 11. Speicherquelle

Eine Region kann ihren Inhalt beziehen aus:

* anonymem Speicher
* Datei
* Shared Memory
* Gerätespeicher
* Kernelobjekten
* zukünftigen Speichermedien

---

# 12. Regionserzeugung

Beim Erzeugen einer Region werden:

* Adressbereich reserviert
* Metadaten erzeugt
* Rechte gesetzt
* Seitentabellen vorbereitet
* Ereignisse registriert

Die eigentlichen Seiten werden bei Bedarf erzeugt.

---

# 13. Regionserweiterung

Regionen können erweitert werden, sofern:

* angrenzender virtueller Speicher frei ist,
* Schutzattribute identisch bleiben,
* keine Konflikte entstehen.

---

# 14. Regionsteilung

Eine Region kann automatisch geteilt werden.

Beispiele:

* unterschiedliche Rechte
* Teilfreigabe
* Teilabbildung
* Copy-on-Write

---

# 15. Regionszusammenführung

Benachbarte Regionen werden automatisch zusammengeführt, wenn:

* Rechte identisch sind,
* Speicherquelle identisch ist,
* Flags identisch sind,
* keine Trennung erforderlich bleibt.

Das reduziert Verwaltungsaufwand.

---

# 16. Adressraumverwaltung

Jeder Prozess besitzt einen eigenen virtuellen Adressraum.

Die Regionen eines Prozesses sind vollständig voneinander isoliert.

Der Kernel besitzt einen separaten globalen Adressbereich.

---

# 17. Lazy Allocation

Virtual Memory Regions reservieren zunächst nur virtuellen Adressraum.

Physische Seiten werden erst erzeugt bei:

* erstem Zugriff
* Page Fault
* expliziter Reservierung

Virtuelle Regionen können reserviert werden, ohne sofort physischen Speicher zuzuweisen; die eigentliche Zuordnung erfolgt bei Bedarf (Demand Paging).

---

# 18. File Mapping

Regionen können Dateien abbilden.

Eigenschaften:

* Read Only
* Read Write
* Execute
* Shared
* Private

Mehrere Prozesse können dieselbe Datei gemeinsam nutzen.

---

# 19. Shared Memory

Mehrere Prozesse können dieselbe Region gemeinsam verwenden.

Eigenschaften:

* gemeinsame physische Seiten
* getrennte Adressräume
* identische Rechte
* Referenzzählung

---

# 20. Copy-on-Write

Virtual Memory Regions unterstützen Copy-on-Write.

Beim ersten Schreibzugriff:

* neue Seite erzeugen
* Daten kopieren
* Seitentabelle aktualisieren

Die Region selbst bleibt erhalten.

---

# 21. Guard Regions

NovaOS unterstützt Guard Regions.

Sie dienen unter anderem für:

* Stack Overflow Detection
* Heap Protection
* Sicherheitsbereiche
* Debugging

---

# 22. Speicherabbildungen

Eine Region kann vollständig oder teilweise abbilden:

* Dateien
* Geräte
* Kernelobjekte
* Shared Memory
* zukünftige Speichertypen

---

# 23. Datenstruktur

Die Regionen eines Prozesses werden in einer geordneten Suchstruktur verwaltet.

Anforderungen:

* schnelle Bereichssuche
* schnelle Einfügung
* schnelle Löschung
* effiziente Nachbarschaftsabfragen

NovaOS verwendet hierfür standardmäßig einen balancierten Suchbaum. Andere interne Implementierungen können später ersetzt werden, ohne die öffentliche Architektur zu verändern. Moderne Kernel verwenden spezialisierte Baumstrukturen zur Verwaltung der Regionen eines Adressraums.

---

# 24. Synchronisation

Die Verwaltung erfolgt mittels:

* Reader-Writer-Locks
* atomaren Operationen
* lockarmen Suchstrukturen
* CPU-lokalen Hilfsstrukturen

---

# 25. Sicherheit

Der Virtual Memory Manager schützt vor:

* überlappenden Regionen
* ungültigen Rechten
* Adressraumkorruption
* inkonsistenten Zuordnungen
* fehlerhaften Speicherabbildungen

---

# 26. Performance

Die Architektur soll ermöglichen:

* schnelle Bereichssuche
* geringe Verwaltungsdaten
* wenige Fragmentierungen
* effiziente Page-Fault-Behandlung
* gute Mehrkernskalierung

---

# 27. Debug-Unterstützung

Optional unterstützt NovaOS:

* Region Dumps
* Mapping-Historie
* Regionstatistiken
* Fragmentierungsanalyse
* Zugriffstracing
* Schutzverletzungsanalyse

---

# 28. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* Memory Hotplug
* CXL Memory
* Persistent Memory
* Tiered Memory
* verschlüsselte Regionen
* Hardware Memory Tagging

---

# 29. Auswirkungen

Positive Auswirkungen:

* klare Struktur des virtuellen Adressraums
* einfache Rechteverwaltung
* flexible Speicherabbildungen
* Unterstützung moderner Speichertechniken
* gute Wartbarkeit
* hohe Skalierbarkeit

Negative Auswirkungen:

* zusätzlicher Verwaltungsaufwand
* komplexere Regionensynchronisation
* höherer Speicherbedarf für Metadaten

---

# 30. Verworfene Alternativen

## Verwaltung ausschließlich über Seitentabellen

Vorteile:

* geringe Metadaten

Nachteile:

* aufwendige Bereichsoperationen
* schlechte Erweiterbarkeit
* ineffiziente Rechteverwaltung

---

## Feste Speichersegmente

Vorteile:

* einfache Implementierung

Nachteile:

* geringe Flexibilität
* schlechte Speicherplatznutzung
* ungeeignet für moderne Anwendungen

---

## Lineare Regionenliste

Vorteile:

* einfacher Aufbau

Nachteile:

* langsame Bereichssuche
* schlechte Skalierbarkeit
* hoher Verwaltungsaufwand

---

# 31. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* virtuelle Speicherbereiche ausschließlich über Virtual Memory Regions verwalten,
* keine direkten Manipulationen an Seitentabellen außerhalb des Virtual Memory Managers durchführen,
* Regionen nicht überlappen lassen,
* Schutzattribute konsistent halten,
* Änderungen ausschließlich über die APIs des Virtual Memory Managers durchführen.

---

# 32. Teststrategie

Die Virtual Memory Regions werden geprüft durch:

* Unit-Tests
* Bereichssuchtests
* Rechteprüfungen
* Copy-on-Write-Tests
* File-Mapping-Tests
* Shared-Memory-Tests
* Mehrkerntests
* Langzeittests

---

# 33. Konsequenzen

Virtual Memory Regions bilden die zentrale Beschreibungsschicht des virtuellen Adressraums in NovaOS.

Alle zukünftigen Komponenten – insbesondere Loader, Shared Memory, Memory Mapping, Copy-on-Write und der Virtual Memory Manager – arbeiten ausschließlich mit Regionen. Die eigentliche Seitenverwaltung bleibt davon getrennt, wodurch eine modulare, skalierbare und langfristig wartbare Speicherarchitektur entsteht.

---

# 34. Zusammenfassung

NovaOS verwaltet den virtuellen Adressraum über **Virtual Memory Regions (VMRs)**. Jede Region beschreibt einen zusammenhängenden Bereich mit einheitlichen Eigenschaften wie Zugriffsrechten, Speicherquelle und Typ. Regionen werden unabhängig von den Seitentabellen verwaltet und ermöglichen flexible Funktionen wie File Mapping, Shared Memory, Guard Pages und Copy-on-Write. Dadurch entsteht eine moderne, erweiterbare und performante Architektur für die virtuelle Speicherverwaltung.
