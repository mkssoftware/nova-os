# ADR-KERNEL-0011: Kernel Object Model

* **ADR-ID:** ADR-KERNEL-0011
* **Titel:** Kernel Object Model
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel-Architektur
* **Gültigkeitsbereich:** Gesamter Kernel
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0004, ADR-KERNEL-0007, ADR-KERNEL-0009, NPSPEC-OBJECT-0001

---

# 1. Zusammenfassung

NovaOS verwendet ein **einheitliches Kernel Object Model (KOM)**.

Alle dauerhaft verwalteten Kernelressourcen werden als Kernelobjekte dargestellt. Dadurch erhalten sämtliche Kernelkomponenten ein gemeinsames Lebenszyklusmodell, einheitliche Referenzzählung, standardisierte Rechteverwaltung sowie konsistente Verwaltungs- und Debugmechanismen. Das Konzept orientiert sich an bewährten objektbasierten Kernelarchitekturen, bleibt jedoch vollständig auf die Anforderungen von NovaOS zugeschnitten.

---

# 2. Kontext

Ein moderner Kernel verwaltet tausende unterschiedlicher Ressourcen.

Beispiele:

* Prozesse
* Threads
* Dateien
* Verzeichnisse
* Geräte
* Treiber
* Fenster
* Timer
* Ereignisse
* Mutexe
* IPC-Kanäle
* Netzwerkverbindungen

Ohne ein gemeinsames Objektmodell entstehen unterschiedliche Lebenszyklen, uneinheitliche APIs und erhöhter Wartungsaufwand.

---

# 3. Problemstellung

Es musste entschieden werden,

* wie Kernelressourcen dargestellt werden,
* wie ihre Lebensdauer verwaltet wird,
* wie Referenzen funktionieren,
* wie Objekte identifiziert werden,
* und wie zukünftige Kernelkomponenten integriert werden.

---

# 4. Entscheidung

NovaOS führt ein universelles Kernel Object Model ein.

Dabei gilt:

* jede Kernelressource ist ein Objekt,
* jedes Objekt besitzt einen eindeutigen Typ,
* jedes Objekt besitzt einen standardisierten Objektkopf,
* Objekte werden ausschließlich über den Object Manager erzeugt und verwaltet,
* sämtliche Kernelkomponenten verwenden dieselbe Objektinfrastruktur.

---

# 5. Motivation

Diese Architektur bietet:

* einheitliche Kernelarchitektur
* geringeren Entwicklungsaufwand
* bessere Wartbarkeit
* konsistente APIs
* einfachere Fehleranalyse
* höhere Erweiterbarkeit

---

# 6. Designprinzipien

Das Kernel Object Model basiert auf:

* Objektorientierung auf Systemebene
* Komposition statt Vererbung
* Referenzzählung
* eindeutiger Typisierung
* Thread-Sicherheit
* Modularität
* Erweiterbarkeit

---

# 7. Architekturübersicht

```text
                  Object Manager

                         │

        ┌────────────────┼────────────────┐

        ▼                ▼                ▼

   Kernel Object     Object Cache     Handle Manager

        │

        ▼

+------------------------------+

| Standard Object Header        |

+------------------------------+

| Typspezifische Daten          |

+------------------------------+
```

---

# 8. Grundstruktur

Jedes Kernelobjekt besteht aus zwei Bereichen:

* standardisierter Objektkopf
* typspezifische Nutzdaten

Dadurch können Kernelkomponenten unabhängig vom Objekttyp arbeiten.

---

# 9. Objektkopf

Der Objektkopf enthält mindestens:

* Objekt-ID
* Objekttyp
* Referenzzähler
* Status
* Flags
* Besitzer
* Sicherheitsinformationen
* Erstellungszeitpunkt
* Debuginformationen

---

# 10. Objekt-ID

Jedes Kernelobjekt besitzt eine systemweit eindeutige **Object ID (OID)**.

Die OID bleibt während der gesamten Lebensdauer unverändert.

Sie dient ausschließlich der Identifikation und niemals als direkter Speicherzeiger.

---

# 11. Objekttyp

Jedes Objekt besitzt exakt einen Typ.

Beispiele:

* Process
* Thread
* File
* Directory
* Device
* Driver
* Window
* Event
* Mutex
* Timer
* Socket
* Surface
* Font
* Theme

Weitere Typen können jederzeit ergänzt werden.

---

# 12. Referenzzählung

Alle Kernelobjekte verwenden Referenzzählung.

Ein Objekt wird automatisch zerstört, sobald:

* keine Referenzen mehr existieren,
* keine Handles mehr geöffnet sind,
* keine Kernelkomponente das Objekt verwendet.

Dieses Modell vereinfacht die Verwaltung des Objektlebenszyklus erheblich.

---

# 13. Lebenszyklus

Der Lebenszyklus eines Kernelobjekts besteht aus:

1. Erzeugen
2. Initialisieren
3. Registrieren
4. Verwenden
5. Freigeben
6. Zerstören

---

# 14. Erzeugung

Kernelobjekte dürfen ausschließlich durch den Object Manager erzeugt werden.

Direkte Speicherallokationen außerhalb des Object Managers sind für Kernelobjekte nicht zulässig.

---

# 15. Speicherverwaltung

Alle Kernelobjekte werden über den Kernel Heap angelegt.

Der Object Manager verwaltet:

* Allokation
* Initialisierung
* Registrierung
* Freigabe

---

# 16. Objektregistrierung

Nach erfolgreicher Erzeugung wird jedes Objekt in den globalen Objektkatalog aufgenommen.

Dadurch können:

* Debugwerkzeuge
* Diagnosemodule
* Sicherheitsmechanismen
* Statistikmodule

auf sämtliche Objekte zugreifen.

---

# 17. Objektattribute

Jedes Objekt besitzt definierte Attribute.

Beispiele:

* Name
* Typ
* Größe
* Besitzer
* Erstellungszeit
* Flags
* Berechtigungen

---

# 18. Handles

Benutzerprogramme greifen niemals direkt auf Kernelobjekte zu.

Sie verwenden ausschließlich **Handles**, welche intern auf Kernelobjekte verweisen. Dieses Konzept entkoppelt interne Kernelstrukturen von Benutzerschnittstellen und ermöglicht eine sichere Ressourcenverwaltung.

---

# 19. Objekt-Namespace

Kernelobjekte können optional in einem hierarchischen Namespace organisiert werden.

Beispiel:

```text
/

├── Process

├── Device

├── Driver

├── IPC

├── Session

└── System
```

Ein hierarchischer Objektnamensraum erleichtert Verwaltung, Auffindbarkeit und Zugriffskontrolle.

---

# 20. Objektbeziehungen

Objekte können Beziehungen besitzen.

Beispiele:

* Prozess → Threads
* Gerät → Treiber
* Fenster → Oberfläche
* Datei → Dateisystem
* Socket → Netzwerkadapter

---

# 21. Sicherheit

Jedes Objekt besitzt Sicherheitsinformationen.

Diese definieren:

* Eigentümer
* Zugriffsrechte
* Capabilities
* Auditinformationen

---

# 22. Synchronisation

Das Kernel Object Model arbeitet vollständig thread-sicher.

Zum Einsatz kommen:

* atomare Referenzzähler
* Spinlocks
* Reader-Writer-Locks
* lockfreie Datenstrukturen, sofern sinnvoll

---

# 23. Debug-Unterstützung

Optional speichert jedes Objekt:

* Erstellungsort
* Erstellungszeit
* Stacktrace
* Referenzhistorie
* Speicherstatistiken

---

# 24. Objekt-Caches

Häufig verwendete Objekte dürfen in Caches gehalten werden.

Beispiele:

* Threads
* Prozesse
* Timer
* Mutexe
* Dateiobjekte

---

# 25. Erweiterbarkeit

Neue Kernelobjekte können ergänzt werden, ohne bestehende Komponenten anzupassen.

Hierfür müssen lediglich:

* neuer Objekttyp
* Initialisierungsroutine
* Verwaltungsfunktionen

registriert werden.

---

# 26. Performance

Das Kernel Object Model soll:

* schnelle Objekterzeugung
* geringe Speicherkosten
* minimale Synchronisationskosten
* hohe Parallelität

ermöglichen.

---

# 27. Auswirkungen

Positive Auswirkungen:

* einheitliche Kernelarchitektur
* zentrale Ressourcenverwaltung
* konsistente APIs
* bessere Debugbarkeit
* höhere Wartbarkeit
* einfache Erweiterbarkeit

Negative Auswirkungen:

* zusätzlicher Objektkopf
* höherer Verwaltungsaufwand
* Referenzzählung verursacht geringe Laufzeitkosten

---

# 28. Verworfene Alternativen

## Ressourcenspezifische Strukturen

Vorteile:

* geringerer Verwaltungsaufwand

Nachteile:

* keine gemeinsame Infrastruktur
* inkonsistente APIs
* schwierige Erweiterbarkeit

---

## Direkte Zeigerverwaltung

Vorteile:

* einfache Implementierung

Nachteile:

* fehleranfällig
* schwierige Lebensdauerverwaltung
* hohe Gefahr von Speicherfehlern

---

## Garbage Collector

Vorteile:

* automatische Speicherverwaltung

Nachteile:

* nicht deterministisch
* ungeeignet für Kernelbetrieb
* zusätzliche Laufzeitkosten

---

# 29. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* Kernelobjekte ausschließlich über den Object Manager erzeugen,
* Referenzzählung konsequent verwenden,
* Objekte niemals direkt freigeben,
* ausschließlich standardisierte Kernelobjekte verwenden,
* keine eigenen Objektverwaltungssysteme implementieren.

---

# 30. Teststrategie

Das Kernel Object Model wird geprüft durch:

* Unit-Tests
* Referenzzählungstests
* Lasttests
* Parallelitätstests
* Leak-Tests
* Langzeittests
* Debugtests

---

# 31. Konsequenzen

Das Kernel Object Model bildet die gemeinsame Grundlage sämtlicher Kernelressourcen in NovaOS.

Alle zukünftigen Kernelkomponenten müssen ihre Ressourcen als standardisierte Kernelobjekte implementieren und über den Object Manager verwalten. Dadurch entsteht eine konsistente Kernelarchitektur mit einheitlichem Lebenszyklus, zentraler Ressourcenverwaltung und klar definierten Schnittstellen zwischen den einzelnen Subsystemen.

---

# 32. Zusammenfassung

NovaOS verwendet ein **einheitliches Kernel Object Model**, in dem sämtliche Kernelressourcen als standardisierte Objekte mit gemeinsamem Objektkopf, Referenzzählung und definiertem Lebenszyklus verwaltet werden. Der Object Manager übernimmt Erzeugung, Registrierung und Freigabe der Objekte und schafft damit eine robuste, erweiterbare und konsistente Grundlage für die gesamte Kernelarchitektur.
