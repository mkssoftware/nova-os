# ADR-KERNEL-0004: Prozessmodell

* **ADR-ID:** ADR-KERNEL-0004
* **Titel:** Prozessmodell
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel-Architektur
* **Gültigkeitsbereich:** Process Manager, Scheduler, Memory Manager, Security Manager
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0001, ADR-KERNEL-0003, NPSPEC-KERNEL-0004

---

# 1. Zusammenfassung

NovaOS verwendet ein **prozesszentriertes Ausführungsmodell**, bei dem ein Prozess ausschließlich als Container für Ressourcen dient, während Threads die eigentlichen Ausführungseinheiten darstellen.

Ein Prozess besitzt mindestens einen Thread und verwaltet seinen virtuellen Adressraum, Handles, Sicherheitsattribute sowie sämtliche zugehörigen Systemressourcen. Dieses Modell entspricht dem Aufbau moderner Betriebssysteme, in denen Prozesse Ressourcen kapseln und Threads den ausführbaren Code darstellen.

---

# 2. Kontext

Der Process Manager bildet gemeinsam mit Scheduler und Speicherverwaltung das Fundament des NovaOS-Kernels.

Das Prozessmodell beeinflusst unmittelbar:

* Multitasking
* Speicherverwaltung
* Sicherheit
* IPC
* Scheduling
* Rechteverwaltung
* Ressourcenverwaltung
* Debugging

---

# 3. Problemstellung

Es musste entschieden werden,

* welche Aufgaben ein Prozess übernimmt,
* welche Aufgaben Threads übernehmen,
* welche Ressourcen gemeinsam genutzt werden,
* wie Prozesse isoliert werden,
* und wie Prozesse miteinander interagieren.

---

# 4. Entscheidung

NovaOS verwendet folgendes Modell:

* Prozesse verwalten Ressourcen.
* Threads führen Code aus.
* Jeder Prozess besitzt mindestens einen Thread.
* Mehrere Threads können denselben Prozess gemeinsam verwenden.
* Prozesse besitzen voneinander getrennte virtuelle Adressräume.

---

# 5. Motivation

Diese Architektur bietet:

* klare Verantwortlichkeiten
* hohe Skalierbarkeit
* gute Isolation
* effizientes Multithreading
* einfache Ressourcenverwaltung
* hohe Sicherheit

---

# 6. Designprinzipien

Das Prozessmodell basiert auf:

* Ressourcenkapselung
* Threadbasierter Ausführung
* vollständiger Isolation
* objektorientiertem Kernelmodell
* deterministischen Zuständen
* Erweiterbarkeit

---

# 7. Architekturübersicht

```text
                    Process

        +----------------------------------+
        | Virtueller Adressraum            |
        | Handles                          |
        | Sicherheitskontext               |
        | Ressourcen                       |
        +---------------+------------------+
                        |
        +---------------+------------------+
        | Thread 1                         |
        | Thread 2                         |
        | Thread 3                         |
        +----------------------------------+
```

---

# 8. Definition eines Prozesses

Ein Prozess ist die logische Verwaltungseinheit eines laufenden Programms.

Er besitzt insbesondere:

* virtuellen Speicher
* Handle-Tabelle
* Prozess-ID
* Sicherheitskontext
* Ressourcen
* Threadliste

---

# 9. Prozessidentität

Jeder Prozess besitzt:

* eindeutige Prozess-ID (PID)
* Erstellungszeitpunkt
* Status
* Prioritätsklasse
* Besitzer
* Sicherheitskontext

---

# 10. Virtueller Adressraum

Jeder Prozess besitzt einen eigenen virtuellen Adressraum.

Andere Prozesse dürfen diesen Speicher nicht direkt lesen oder verändern.

Ausnahmen erfolgen ausschließlich über definierte Kernelmechanismen.

---

# 11. Threads

Ein Prozess enthält mindestens einen Thread.

Threads teilen gemeinsam:

* Adressraum
* Heap
* offene Dateien
* Kernelobjekte
* IPC-Ressourcen

Jeder Thread besitzt dagegen seinen eigenen Stack, Registersatz und Ausführungskontext.

---

# 12. Ressourcenverwaltung

Ein Prozess verwaltet:

* Dateien
* Geräte
* IPC-Objekte
* Speicher
* Handles
* Synchronisationsobjekte

---

# 13. Sicherheitskontext

Der Sicherheitskontext enthält:

* Benutzerkennung
* Gruppen
* Capability-Satz
* Berechtigungen
* Integritätsstufe

---

# 14. Prozesszustände

Ein Prozess kann sich mindestens in folgenden Zuständen befinden:

```text
CREATED
↓
READY
↓
RUNNING
↓
WAITING
↓
SUSPENDED
↓
TERMINATED
```

---

# 15. Lebenszyklus

Der Lebenszyklus umfasst:

* Erstellung
* Initialisierung
* Ausführung
* Blockierung
* Fortsetzung
* Terminierung
* Ressourcenfreigabe

---

# 16. Eltern-Kind-Beziehungen

NovaOS unterstützt optionale Prozesshierarchien.

Ein Elternprozess kann:

* Kindprozesse erzeugen,
* deren Beendigung überwachen,
* Ressourcen vererben,
* definierte Rechte übertragen.

---

# 17. Prozessisolierung

Prozesse sind vollständig voneinander isoliert.

Eine direkte Speicherfreigabe zwischen Prozessen ist nicht zulässig.

Gemeinsamer Speicher muss explizit eingerichtet werden.

---

# 18. Prozesskommunikation

Die Kommunikation erfolgt ausschließlich über definierte IPC-Mechanismen.

Beispiele:

* Nachrichten
* Shared Memory
* Pipes
* RPC
* Events

---

# 19. Handles

Kernelobjekte werden ausschließlich über Handles referenziert.

Direkte Zeiger auf Kernelstrukturen werden niemals an Userspace-Prozesse weitergegeben.

---

# 20. Ressourcenvererbung

Bei der Prozesserzeugung kann festgelegt werden:

* keine Vererbung
* ausgewählte Ressourcen
* vollständige Vererbung

---

# 21. Prioritätsklassen

Jeder Prozess besitzt eine Prioritätsklasse.

Threads können innerhalb dieser Klasse individuelle Prioritäten besitzen.

---

# 22. Mehrprozessbetrieb

NovaOS unterstützt beliebig viele gleichzeitig laufende Prozesse, sofern ausreichend Ressourcen verfügbar sind.

---

# 23. Fehlerisolierung

Ein fehlerhafter Prozess darf andere Prozesse nicht beeinträchtigen.

Kernel und übrige Prozesse bleiben weiterhin funktionsfähig.

---

# 24. Debugging

Das Prozessmodell unterstützt:

* Breakpoints
* Speicheranalyse
* Threadanalyse
* Crash Dumps
* Performanceanalyse

---

# 25. Performance

Die Prozessverwaltung soll:

* kurze Erzeugungszeiten
* schnelle Terminierung
* effiziente Kontextwechsel
* geringe Speicherverwaltungskosten

gewährleisten.

---

# 26. Erweiterbarkeit

Das Prozessmodell muss zukünftige Erweiterungen ermöglichen.

Beispiele:

* Container
* Sandboxen
* Jobobjekte
* Namespaces
* virtuelle Prozesse

---

# 27. Auswirkungen

Positive Auswirkungen:

* hohe Stabilität
* gute Isolation
* effizientes Multithreading
* klare Ressourcenverwaltung
* hohe Sicherheit

Negative Auswirkungen:

* Prozesswechsel verursachen höheren Aufwand als Threadwechsel.
* Separate Adressräume benötigen zusätzlichen Speicher.
* IPC ist aufwendiger als Kommunikation zwischen Threads.

---

# 28. Verworfene Alternativen

## Threadloses Prozessmodell

Vorteile:

* einfache Implementierung

Nachteile:

* keine Parallelität innerhalb eines Prozesses
* schlechte Skalierbarkeit

---

## Gemeinsamer globaler Adressraum

Vorteile:

* einfache Kommunikation

Nachteile:

* geringe Sicherheit
* schlechte Isolation
* hohe Fehleranfälligkeit

---

## Ein Thread pro Prozess

Vorteile:

* geringe Komplexität

Nachteile:

* keine moderne Multithreading-Unterstützung

---

# 29. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* Prozesse ausschließlich über den Process Manager erzeugen,
* Kernelobjekte über Handles verwalten,
* Prozessisolierung respektieren,
* Sicherheitskontexte berücksichtigen,
* keine direkten Speicherzugriffe zwischen Prozessen erlauben.

---

# 30. Teststrategie

Das Prozessmodell wird validiert durch:

* Unit-Tests
* Integrationstests
* Lasttests
* Isolationstests
* Sicherheitstests
* Langzeittests
* Stresstests

---

# 31. Konsequenzen

Diese Architekturentscheidung definiert das grundlegende Prozessmodell von NovaOS.

Alle zukünftigen Kernelkomponenten müssen Prozesse als Ressourcencontainer und Threads als Ausführungseinheiten behandeln. Änderungen an diesem Modell würden erhebliche Auswirkungen auf Scheduler, Speicherverwaltung, IPC, Sicherheitsarchitektur und Systemaufrufe haben.

---

# 32. Zusammenfassung

NovaOS verwendet ein modernes Prozessmodell, bei dem Prozesse Ressourcen kapseln und Threads die eigentlichen Ausführungseinheiten darstellen. Jeder Prozess besitzt einen eigenen virtuellen Adressraum, einen Sicherheitskontext und eine Menge verwalteter Kernelobjekte. Dieses Modell schafft eine klare Trennung zwischen Ressourcenverwaltung und Programmausführung und bildet die Grundlage für Skalierbarkeit, Stabilität und Sicherheit des gesamten Betriebssystems.
