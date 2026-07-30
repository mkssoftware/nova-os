# ADR-KERNEL-0100: Kernel Object Graph statt isolierter Subsysteme

* **ADR-ID:** ADR-KERNEL-0100
* **Titel:** Kernel Object Graph statt isolierter Subsysteme
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel Architecture
* **Gültigkeitsbereich:** Gesamter NovaOS-Kernel
* **Ersetzt:** Klassische subsystemorientierte Kernelarchitektur
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0011 (Kernel Object Model), ADR-KERNEL-0012 (Handle-System), ADR-KERNEL-0013 (Capability-Sicherheit), ADR-KERNEL-0015 (Ereignismodell), ADR-MM-0004 (Virtual Memory Regions)

---

# 1. Zusammenfassung

NovaOS ersetzt die klassische Trennung in isolierte Kernel-Subsysteme durch einen **Kernel Object Graph (KOG)**.

Anstatt Speicherverwaltung, Scheduler, Dateisystem, Netzwerk oder Treiber als voneinander getrennte Module zu betrachten, wird das gesamte Betriebssystem als zusammenhängender Graph aus Kernelobjekten modelliert.

Jedes Kernelobjekt besitzt definierte Beziehungen zu anderen Objekten. Dadurch entsteht ein konsistentes, introspektierbares und ereignisorientiertes Systemmodell. Objektmanager in modernen Betriebssystemen verwalten Kernelressourcen bereits als Objekte mit Lebenszyklus, Typinformationen und Referenzen; NovaOS erweitert dieses Prinzip zu einem vollständigen Objektgraphen.

---

# 2. Kontext

Traditionelle Kernel bestehen aus:

* Memory Manager
* Scheduler
* IPC
* Device Manager
* Filesystem
* Netzwerk
* Treiber
* Security

Diese Komponenten kommunizieren häufig über spezialisierte APIs und interne Kopplungen.

Mit wachsender Systemgröße entstehen:

* Redundanzen
* Inkonsistenzen
* schwierige Erweiterbarkeit
* komplexe Abhängigkeiten

---

# 3. Problemstellung

Es musste entschieden werden,

* wie Kernelkomponenten miteinander interagieren,
* wie Objektbeziehungen modelliert werden,
* wie Abhängigkeiten nachvollziehbar bleiben,
* wie Ereignisse propagiert werden,
* und wie zukünftige Erweiterungen integriert werden.

---

# 4. Entscheidung

NovaOS verwendet einen zentralen **Kernel Object Graph**.

Dabei gilt:

* Alles ist ein Kernelobjekt.
* Beziehungen werden explizit modelliert.
* Der Graph bildet die vollständige Kernelstruktur ab.
* APIs arbeiten auf Objekten statt auf Subsystemgrenzen.
* Neue Komponenten erweitern den Graph statt die Architektur.

---

# 5. Motivation

Diese Architektur bietet:

* einheitliches Architekturmodell
* geringere Kopplung
* bessere Erweiterbarkeit
* vollständige Nachvollziehbarkeit
* konsistente Ereignisverarbeitung
* vereinfachte Diagnostik

---

# 6. Designprinzipien

Der Kernel Object Graph basiert auf:

* Everything is an Object
* Graph statt Hierarchie
* lose Kopplung
* Capability-basierter Zugriff
* Ereignisorientierung
* deterministische Lebenszyklen

---

# 7. Architekturübersicht

```text
                 Kernel Root
                     │

      ┌──────────────┼──────────────┐
      │              │              │
      ▼              ▼              ▼
   Process        Device        Filesystem
      │              │              │
      ▼              ▼              ▼
    Thread        Driver        Directory
      │              │              │
      ▼              ▼              ▼
     VMR         DMA Buffer        File
      │              │              │
      └──────────────┼──────────────┘
                     |
                     ▼
             Kernel Object Graph
```

---

# 8. Grundidee

Jedes Kernelobjekt besitzt:

* eindeutige Objekt-ID
* Objekttyp
* Beziehungen
* Metadaten
* Ereigniskanäle
* Capability-Liste
* Referenzzähler

---

# 9. Kernelobjekte

Beispiele:

* Prozess
* Thread
* Datei
* Verzeichnis
* Socket
* Gerät
* Treiber
* DMA Buffer
* Timer
* Semaphore
* Fenster
* Shared Memory
* Scheduler Queue
* Virtual Memory Region

---

# 10. Objektbeziehungen

Objekte können Beziehungen besitzen:

* Parent
* Child
* Owner
* Uses
* Depends On
* Maps
* Contains
* References
* Observes

Beziehungen sind typisiert.

---

# 11. Graphstruktur

Der Kernel Object Graph ist

kein Baum,

sondern ein gerichteter Graph.

Dadurch können mehrere Beziehungen gleichzeitig existieren.

---

# 12. Objektlebenszyklus

Jedes Objekt besitzt:

* Created
* Initialized
* Active
* Suspended
* Destroy Pending
* Destroyed

Der Lebenszyklus wird zentral verwaltet. Moderne Objektmanager verwalten den Lebenszyklus von Kernelobjekten einschließlich Referenzzählung und automatischer Freigabe nach dem letzten Verweis.

---

# 13. Referenzverwaltung

Alle Beziehungen verwenden:

* Referenzzählung
* schwache Referenzen
* Capability-Referenzen
* Ereignisreferenzen

Zyklen werden erkannt.

---

# 14. Objektattribute

Objekte besitzen:

* Name
* UUID
* Typ
* Version
* Besitzer
* Flags
* Sicherheitsstufe
* Erstellungszeit

---

# 15. Objektgraph-API

Der Kernel stellt allgemeine Operationen bereit:

* Create
* Destroy
* Connect
* Disconnect
* Lookup
* Query
* Enumerate
* Observe

---

# 16. Ereignisverarbeitung

Alle Kernelereignisse werden

über Objektbeziehungen propagiert.

Beispiele:

* Datei geändert
* Gerät entfernt
* Thread beendet
* Speicher erweitert
* Netzwerk getrennt

---

# 17. Schedulerintegration

Schedulerobjekte besitzen Beziehungen zu:

* CPU
* Thread
* Prozess
* Timer
* Priorität
* NUMA-Knoten

---

# 18. Speicherintegration

Memory Management verwendet ebenfalls den Graph.

Beziehungen:

* Prozess → Address Space
* Address Space → VMR
* VMR → Physical Pages
* Physical Pages → NUMA Node

---

# 19. Dateisystemintegration

Das Dateisystem besteht ausschließlich aus Objekten.

Beispiele:

* Volume
* Directory
* File
* Stream
* Snapshot
* Mount Point

---

# 20. Geräteintegration

Geräte werden modelliert als:

* Device
* Driver
* Bus
* Interrupt
* DMA Channel
* Power Domain

---

# 21. Sicherheitsmodell

Capabilities beziehen sich

immer auf Objekte,

nicht auf Subsysteme.

Dadurch bleibt das Sicherheitsmodell konsistent.

---

# 22. Introspection

Der gesamte Kernelgraph kann analysiert werden.

Mögliche Abfragen:

* Welche Objekte referenzieren dieses Objekt?
* Welche Abhängigkeiten existieren?
* Welche Ressourcen nutzt ein Prozess?
* Welche Geräte sind beteiligt?

---

# 23. Debug-Unterstützung

Optional unterstützt NovaOS:

* Graph Explorer
* Objektinspektor
* Referenzanalyse
* Leak Detection
* Ereignisverfolgung
* Graphvisualisierung

---

# 24. Performance

Die Architektur soll ermöglichen:

* konstante Objektzugriffe
* effiziente Traversierung
* geringe Kopplung
* schnelle Ereignisweiterleitung
* hohe Skalierbarkeit

---

# 25. Erweiterbarkeit

Neue Kernelkomponenten benötigen lediglich:

* neuen Objekttyp
* neue Beziehungen
* neue Ereignisse

Bestehende Architektur bleibt unverändert.

---

# 26. Auswirkungen

Positive Auswirkungen:

* konsistente Kernelarchitektur
* vereinfachte Erweiterbarkeit
* bessere Diagnose
* geringere Kopplung
* einheitliches Sicherheitsmodell
* bessere Visualisierbarkeit

Negative Auswirkungen:

* komplexere Objektverwaltung
* zusätzlicher Speicherbedarf
* höherer Implementierungsaufwand

---

# 27. Verworfene Alternativen

## Klassische Subsystemarchitektur

Vorteile:

* bekannte Architektur
* einfache Dokumentation

Nachteile:

* starke Kopplung
* viele Spezialfälle
* schwer erweiterbar

---

## Vollständig servicebasierter Kernel

Vorteile:

* hohe Isolation

Nachteile:

* hoher Kommunikationsaufwand
* zusätzliche Latenzen

---

## Objektbaum

Vorteile:

* einfache Struktur

Nachteile:

* Mehrfachbeziehungen kaum darstellbar
* ungeeignet für moderne Kernelarchitekturen

---

# 28. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* ausschließlich Kernelobjekte verwenden,
* Beziehungen explizit registrieren,
* Referenzen korrekt verwalten,
* Ereignisse über den Objektgraph propagieren,
* keine impliziten Subsystemabhängigkeiten erzeugen.

---

# 29. Teststrategie

Der Kernel Object Graph wird geprüft durch:

* Unit-Tests
* Graphintegritätstests
* Referenzzählungstests
* Zyklenerkennung
* Lasttests
* Mehrkerntests
* Langzeittests
* Performancebenchmarks

---

# 30. Konsequenzen

Der Kernel Object Graph wird zur zentralen Architektur von NovaOS.

Alle Kernelkomponenten kommunizieren über gemeinsame Objektbeziehungen anstatt über starre Subsystemgrenzen. Dadurch entsteht ein konsistentes, introspektierbares und langfristig wartbares Architekturmodell.

---

# 31. Zukunftsperspektive

Langfristig soll der Kernel Object Graph erweitert werden um:

* Live-Graph-Visualisierung
* KI-gestützte Analyse von Objektbeziehungen
* automatische Erkennung von Architekturproblemen
* verteilte Kernelobjekte
* Cluster-übergreifende Objektgraphen
* selbstoptimierende Objektplatzierung

---

# 32. Zusammenfassung

NovaOS ersetzt klassische, isolierte Kernel-Subsysteme durch einen **Kernel Object Graph**, in dem sämtliche Ressourcen, Prozesse, Speicherbereiche, Geräte und Kernelmechanismen als Objekte mit expliziten Beziehungen modelliert werden. Dadurch entsteht eine einheitliche Architektur für Ressourcenverwaltung, Ereignisverarbeitung und Sicherheit. Während klassische Betriebssysteme bereits objektbasierte Kernelressourcen mit Referenzzählung und Lebenszyklus verwenden, erweitert NovaOS dieses Konzept zu einem vollständigen, graphbasierten Architekturmodell, das alle Kernelkomponenten miteinander verbindet.
