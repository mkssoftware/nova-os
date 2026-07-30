# ADR-KERNEL-0012: Handle-System

* **ADR-ID:** ADR-KERNEL-0012
* **Titel:** Handle-System
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel-Architektur
* **Gültigkeitsbereich:** Object Manager, Process Manager, Security Manager, IPC, Systemaufrufe
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0011, ADR-KERNEL-0004, NPSPEC-OBJECT-0002

---

# 1. Zusammenfassung

NovaOS verwendet ein **prozessbasiertes Handle-System** als einzige öffentliche Referenz auf Kernelobjekte.

Benutzerprogramme erhalten niemals direkte Zeiger auf Kernelobjekte. Stattdessen stellt der Kernel opake Handles bereit, die innerhalb einer prozessspezifischen Handle-Tabelle auf Kernelobjekte abgebildet werden. Dadurch werden Speicherisolation, Rechteprüfung und Lebensdauerverwaltung der Kernelobjekte gewährleistet. Dieses Konzept orientiert sich an bewährten objektbasierten Betriebssystemarchitekturen, wird jedoch speziell für NovaOS entwickelt.

---

# 2. Kontext

Nahezu alle Kernelobjekte müssen von Anwendungen verwendet werden.

Beispiele:

* Dateien
* Prozesse
* Threads
* Fenster
* Geräte
* Timer
* Ereignisse
* Mutexe
* Sockets
* Shared Memory
* Oberflächen

Direkte Zeiger auf Kernelobjekte würden die Systemstabilität und Sicherheit gefährden.

---

# 3. Problemstellung

Es musste entschieden werden,

* wie Anwendungen auf Kernelobjekte zugreifen,
* wie Zugriffsrechte geprüft werden,
* wie Handles verwaltet werden,
* wie Objektlebensdauern sichergestellt werden,
* und wie Prozessisolierung gewährleistet wird.

---

# 4. Entscheidung

NovaOS führt ein vollständig zentrales Handle-System ein.

Dabei gilt:

* Anwendungen arbeiten ausschließlich mit Handles.
* Kernelobjekte bleiben vollständig verborgen.
* Jeder Prozess besitzt eine eigene Handle-Tabelle.
* Der Object Manager verwaltet sämtliche Handles.
* Alle Zugriffe erfolgen über standardisierte Kernel-APIs.

---

# 5. Motivation

Diese Architektur bietet:

* vollständige Speicherisolation
* hohe Sicherheit
* einfache Ressourcenverwaltung
* konsistente APIs
* automatische Lebensdauerverwaltung
* einfache Rechteprüfung

---

# 6. Designprinzipien

Das Handle-System basiert auf:

* vollständiger Kapselung
* Prozessisolierung
* Referenzzählung
* objektorientierter Ressourcenverwaltung
* Sicherheit
* Erweiterbarkeit

---

# 7. Architekturübersicht

```text
                  Benutzerprogramm

                         │

                    Handle (64 Bit)

                         │

                         ▼

                Prozess-Handle-Tabelle

                         │

                         ▼

                  Object Manager

                         │

                         ▼

                  Kernel Object

                         │

                         ▼

              Kernel Heap + Referenzen
```

---

# 8. Handle-Definition

Ein Handle ist eine **opake Kennung**.

Ein Handle enthält niemals:

* Speicheradressen
* Objektzeiger
* interne Kernelstrukturen

Anwendungen dürfen den numerischen Wert eines Handles nicht interpretieren. Handles dienen ausschließlich als Identifikatoren für Kernelobjekte.

---

# 9. Handle-Tabelle

Jeder Prozess besitzt eine eigene Handle-Tabelle.

Diese enthält mindestens:

* Handle-ID
* Zielobjekt
* Zugriffsrechte
* Flags
* Referenzzähler
* Erstellungszeit

---

# 10. Prozessisolierung

Handles sind grundsätzlich nur innerhalb des erzeugenden Prozesses gültig.

Ein Handle eines Prozesses darf nicht direkt von einem anderen Prozess verwendet werden.

---

# 11. Handle-Erzeugung

Neue Handles dürfen ausschließlich durch den Object Manager erzeugt werden.

Eine direkte Erzeugung durch Kernelkomponenten ist unzulässig.

---

# 12. Handle-Auflösung

Vor jedem Objektzugriff erfolgt:

1. Suche in der Handle-Tabelle
2. Rechteprüfung
3. Referenzprüfung
4. Rückgabe des Kernelobjekts

---

# 13. Referenzzählung

Jeder offene Handle erhöht den Referenzzähler des zugehörigen Kernelobjekts.

Das Objekt wird erst zerstört, wenn:

* keine Referenzen mehr existieren,
* keine Handles geöffnet sind,
* keine Kernelkomponente das Objekt verwendet.

---

# 14. Zugriffsrechte

Jeder Handle besitzt einen eigenen Berechtigungssatz.

Beispiele:

* Lesen
* Schreiben
* Ausführen
* Synchronisieren
* Konfigurieren
* Löschen
* Vollzugriff

Ein Handle kann somit weniger Rechte besitzen als das zugrunde liegende Objekt.

---

# 15. Handle-Flags

Handles können optionale Eigenschaften besitzen.

Beispiele:

* vererbbar
* nur Kernel
* temporär
* dauerhaft
* exklusiv

---

# 16. Kernel-Handles

NovaOS unterstützt Kernel-Handles.

Diese sind ausschließlich aus dem Kernel erreichbar und niemals für Benutzerprogramme sichtbar. Sie dienen internen Kernelkomponenten zur sicheren Verwaltung von Ressourcen.

---

# 17. Handle-Vererbung

Beim Erzeugen neuer Prozesse können ausgewählte Handles vererbt werden.

Die Vererbung erfolgt ausschließlich explizit.

---

# 18. Handle-Duplizierung

Handles können kontrolliert dupliziert werden.

Dabei können:

* Zugriffsrechte reduziert
* Flags geändert
* Zielprozesse angegeben

werden.

---

# 19. Handle-Schließen

Beim Schließen eines Handles erfolgt:

* Entfernen aus der Handle-Tabelle
* Verringerung der Referenzzählung
* optionale Objektzerstörung

---

# 20. Handle-Namespace

Die numerischen Handle-Werte besitzen ausschließlich innerhalb eines Prozesses Bedeutung.

Zwischen Prozessen dürfen identische Handle-Werte existieren.

---

# 21. Thread-Sicherheit

Alle Operationen auf Handle-Tabellen sind vollständig thread-sicher.

Zum Einsatz kommen:

* atomare Operationen
* Reader-Writer-Locks
* Spinlocks
* lockfreie Datenstrukturen, sofern sinnvoll

---

# 22. Sicherheit

Das Handle-System schützt vor:

* direkten Kernelzugriffen
* ungültigen Zeigern
* Rechteeskalation
* Handle-Manipulation
* Use-after-Free

Jeder Zugriff wird anhand der im Handle hinterlegten Rechte geprüft.

---

# 23. Performance

Das Handle-System soll:

* schnelle Handle-Auflösung
* geringe Speicherkosten
* minimale Synchronisationskosten
* hohe Parallelität

ermöglichen.

---

# 24. Debug-Unterstützung

Optional speichert das System:

* Handle-Ersteller
* Erstellungszeit
* Referenzhistorie
* Objekttyp
* Rechte
* Status

---

# 25. Diagnosefunktionen

Der Kernel stellt Diagnosefunktionen bereit für:

* Handle-Leaks
* ungültige Handles
* doppelte Schließvorgänge
* Referenzfehler
* Rechteverletzungen

---

# 26. Erweiterbarkeit

Das Handle-System unterstützt zukünftige Erweiterungen:

* Capability-Handles
* kryptographisch signierte Handles
* verteilte Handles
* Container-Namespaces
* Virtualisierung

---

# 27. Auswirkungen

Positive Auswirkungen:

* hohe Sicherheit
* vollständige Objektkapselung
* konsistente APIs
* einfache Rechteverwaltung
* zentrale Ressourcenkontrolle
* gute Erweiterbarkeit

Negative Auswirkungen:

* zusätzlicher Verwaltungsaufwand
* Speicherbedarf für Handle-Tabellen
* geringe Laufzeitkosten bei der Handle-Auflösung

---

# 28. Verworfene Alternativen

## Direkte Kernelzeiger

Vorteile:

* maximale Geschwindigkeit

Nachteile:

* keine Isolation
* hohe Sicherheitsrisiken
* fehleranfällig

---

## Globale Handle-Tabelle

Vorteile:

* einfache Implementierung

Nachteile:

* schlechte Skalierbarkeit
* geringere Isolation
* Synchronisationsengpässe

---

## Objektnamen statt Handles

Vorteile:

* menschenlesbar

Nachteile:

* langsame Namensauflösung
* höherer Speicherbedarf
* ungeeignet für Hochleistungsoperationen

---

# 29. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* ausschließlich Handles an Benutzerprogramme zurückgeben,
* Kernelobjekte niemals direkt offenlegen,
* Referenzzählung konsequent verwenden,
* Rechteprüfungen vor jedem Zugriff durchführen,
* Handle-Tabellen ausschließlich über den Object Manager verwalten.

---

# 30. Teststrategie

Das Handle-System wird geprüft durch:

* Unit-Tests
* Rechteprüfungen
* Parallelitätstests
* Leak-Tests
* Stresstests
* Langzeittests
* Performancebenchmarks
* Sicherheitstests

---

# 31. Konsequenzen

Das Handle-System bildet die verbindliche Schnittstelle zwischen Benutzerprogrammen und Kernelobjekten.

Alle zukünftigen Kernelkomponenten müssen ihre Ressourcen ausschließlich über Handles exponieren. Direkte Zeiger auf Kernelobjekte sind außerhalb des Kernels grundsätzlich unzulässig. Dadurch entsteht eine konsistente, sichere und skalierbare Infrastruktur für sämtliche Kernelressourcen.

---

# 32. Zusammenfassung

NovaOS verwendet ein **prozessbasiertes Handle-System**, das sämtliche Zugriffe auf Kernelobjekte über opake Handles kapselt. Der Object Manager verwaltet Erzeugung, Auflösung, Rechteprüfung und Lebensdauer aller Handles. Dieses Modell gewährleistet Speicherisolation, Prozesssicherheit und eine einheitliche Ressourcenverwaltung und bildet damit eine zentrale Grundlage der NovaOS-Kernelarchitektur.
