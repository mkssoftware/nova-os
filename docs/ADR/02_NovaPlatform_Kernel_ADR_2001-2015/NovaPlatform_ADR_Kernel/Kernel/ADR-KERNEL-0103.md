# ADR-KERNEL-0103: Native Capability-basierte Ressourcenverwaltung

* **ADR-ID:** ADR-KERNEL-0103
* **Titel:** Native Capability-basierte Ressourcenverwaltung
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel Architecture
* **Gültigkeitsbereich:** Kernel Object Manager, Security Manager, Handle Manager, Process Manager, IPC, Device Manager, Filesystem, Memory Manager, System Call Interface
* **Ersetzt:** ACL-zentrierte Ressourcenverwaltung
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0012, ADR-KERNEL-0013, ADR-KERNEL-0100, ADR-KERNEL-0101, ADR-KERNEL-0102
* **Entscheidungstyp:** Architekturentscheidung
* **Sicherheitsrelevanz:** Kritisch

---

# 1. Zusammenfassung

NovaOS verwendet eine **native capability-basierte Ressourcenverwaltung** als primären Mechanismus zur Autorisierung sämtlicher Zugriffe auf Kernelobjekte.

Jede Ressource wird ausschließlich über **nicht fälschbare Capabilities** angesprochen. Eine Capability referenziert genau ein Objekt und enthält den zulässigen Rechteumfang. Zugriffe erfolgen nicht aufgrund globaler Benutzeridentitäten oder Objekt-ACLs, sondern durch den Besitz einer gültigen Capability. Capability-basierte Sicherheit verwendet nicht fälschbare Berechtigungstoken, die eine Ressource und die erlaubten Operationen gemeinsam repräsentieren und so das Prinzip der minimalen Rechte unterstützen.

---

# 2. Kontext

Klassische Betriebssysteme verwenden überwiegend:

* Benutzerkennungen
* Gruppen
* ACLs
* Dateiberechtigungen
* globale Privilegien
* Prozessprivilegien

Diese Modelle führen häufig zu:

* Ambient Authority
* übermäßigen Berechtigungen
* komplizierten ACL-Strukturen
* schwer nachvollziehbaren Sicherheitsentscheidungen
* hoher Administrationskomplexität

---

# 3. Problemstellung

Es musste entschieden werden,

* wie Ressourcen autorisiert werden,
* wie Rechte delegiert werden,
* wie Rechte eingeschränkt werden,
* wie Capabilities widerrufen werden,
* wie Rechte zwischen Prozessen übertragen werden,
* wie Handles abgesichert werden,
* und wie sich das Sicherheitsmodell in den Kernel Object Graph integriert.

---

# 4. Entscheidung

NovaOS verwendet Capabilities als **einzige Autorität** für Objektzugriffe.

Dabei gilt:

* Jeder Objektzugriff benötigt mindestens eine Capability.
* Objektnamen verleihen keine Rechte.
* Objekt-IDs verleihen keine Rechte.
* Handles sind Capability-Träger.
* ACLs können als Kompatibilitätsschicht existieren, sind jedoch nicht die interne Autoritätsquelle.
* Rechte werden explizit delegiert und niemals implizit geerbt.

---

# 5. Motivation

Diese Architektur bietet:

* Least Privilege
* klare Rechtevergabe
* einfache Delegation
* sichere Sandboxen
* bessere Isolation
* geringe Kopplung
* kontrollierte Ressourcenweitergabe
* einheitliches Sicherheitsmodell

---

# 6. Designprinzipien

Die Capability-Verwaltung basiert auf:

* Least Privilege
* Default Deny
* expliziter Autorisierung
* Delegation statt Identität
* nicht fälschbaren Referenzen
* Widerrufbarkeit
* vollständiger Nachvollziehbarkeit
* Capability-Minimierung

---

# 7. Architekturübersicht

```text
                Anwendung
                     │
               Object Proxy
                     │
              Capability Handle
                     │
                     ▼
              Handle Resolver
                     │
                     ▼
             Capability Manager
                     │
        ┌────────────┼────────────┐
        ▼            ▼            ▼
  Kernel Object   Security    Object Graph
                     │
                     ▼
              Ressourcenzugriff
```

---

# 8. Grundprinzip

Ein Objekt kann ausschließlich verwendet werden,

wenn eine gültige Capability vorhanden ist.

Die Identität eines Prozesses allein genügt nicht.

---

# 9. Capability

Eine Capability beschreibt:

* Zielobjekt
* erlaubte Operationen
* Gültigkeitsbereich
* Version
* Integrität
* Delegierbarkeit
* Widerrufbarkeit

Eine Capability ist ein nicht fälschbares Autorisierungstoken und repräsentiert Objekt und Rechte gemeinsam.

---

# 10. Capability-Struktur

```text
Capability
├── Capability ID
├── Object ID
├── Rights Bitmap
├── Generation
├── Flags
├── Lifetime
├── Namespace
├── Owner
├── Integrity Tag
└── Version
```

---

# 11. Rechte

Standardrechte umfassen:

* Read
* Write
* Execute
* Configure
* Observe
* Wait
* Signal
* Enumerate
* Duplicate
* Transfer
* Destroy
* Administer

---

# 12. Capability-Klassen

NovaOS definiert:

* Object Capability
* Memory Capability
* Device Capability
* Filesystem Capability
* IPC Capability
* Scheduler Capability
* Security Capability
* Diagnostic Capability
* Service Capability

---

# 13. Handle

Ein Handle ist die geschützte Userspace-Repräsentation einer Capability.

Handles enthalten niemals:

* Kernelpointer
* Objektadressen
* interne Referenzen

---

# 14. Rechteprüfung

Vor jeder Operation prüft der Kernel:

1. Handle gültig
2. Generation gültig
3. Capability vorhanden
4. Rechte ausreichend
5. Objekt aktiv
6. Sicherheitsrichtlinien erfüllt

---

# 15. Delegation

Capabilities dürfen explizit delegiert werden.

Delegation kann:

* vollständig
* eingeschränkt
* zeitlich begrenzt
* einmalig
* nicht delegierbar

sein.

---

# 16. Rechteabschwächung

Delegierte Capabilities dürfen ausschließlich weniger Rechte besitzen.

```text
Original

Read
Write
Configure
 │
 ▼
Delegiert

Read
```

Eine Rechteerweiterung ist unzulässig.

---

# 17. Capability-Widerruf

NovaOS unterstützt:

* sofortigen Widerruf
* verzögerten Widerruf
* gruppenweisen Widerruf
* objektbezogenen Widerruf
* prozessbezogenen Widerruf

---

# 18. Capability-Vererbung

Neue Prozesse erhalten keine impliziten Rechte.

Vererbung erfolgt ausschließlich durch:

* explizite Übergabe
* Spawn-Regeln
* Sicherheitsrichtlinien

---

# 19. Objektintegration

Jedes Kernelobjekt besitzt:

* unterstützte Rechte
* Capability-Regeln
* Delegationsregeln
* Sicherheitsklasse

---

# 20. Kernel Object Graph

Capabilities beziehen sich direkt auf Objekte des Kernel Object Graph.

Rechte folgen niemals Objektpfaden automatisch.

---

# 21. Ereignisintegration

Capability-Ereignisse:

* Granted
* Revoked
* Restricted
* Expired
* Transferred
* Invalidated

werden über den Event Core veröffentlicht.

---

# 22. Lebensdauer

Capabilities besitzen:

* permanente Gültigkeit
* Prozesslebensdauer
* Sitzungslebensdauer
* Zeitlimit
* Objektlebensdauer

---

# 23. Namensräume

Capabilities können an Namensräume gebunden werden.

Ein Objekt kann in mehreren Namensräumen existieren,

ohne dass automatisch dieselben Rechte gelten.

---

# 24. Prozessmodell

Jeder Prozess besitzt:

* Capability Table
* Handle Table
* Default Capabilities
* Runtime Capabilities

---

# 25. Capability Table

```text
Process
 │
 ▼
Capability Table
├── Handle 1 → File
├── Handle 2 → Device
├── Handle 3 → Shared Memory
└── Handle 4 → Event
```

---

# 26. Speicherobjekte

Speicherzugriffe benötigen Capabilities.

Beispiele:

* Allocate
* Map
* Unmap
* Share
* Protect

---

# 27. Dateisystemobjekte

Dateien besitzen Capabilities für:

* Lesen
* Schreiben
* Löschen
* Umbenennen
* Attribute ändern
* Beobachten

---

# 28. Geräteobjekte

Geräte benötigen Capabilities für:

* Öffnen
* Konfigurieren
* DMA
* Firmware
* Energieverwaltung
* Diagnose

---

# 29. Netzwerkobjekte

Netzwerkrechte umfassen:

* Connect
* Listen
* Accept
* Send
* Receive
* Configure

---

# 30. IPC

IPC-Endpunkte verwenden ausschließlich Capability-basierte Autorisierung.

Es existieren keine global offenen Endpunkte.

---

# 31. Sandbox

Sandboxen besitzen ausschließlich explizit übergebene Capabilities.

Nicht übergebene Ressourcen sind vollständig unsichtbar.

---

# 32. Container

Container besitzen eigene Capability-Räume.

Capabilities werden beim Eintritt in einen Container neu bewertet.

---

# 33. Serviceobjekte

Systemdienste erhalten ausschließlich die Capabilities,

die sie tatsächlich benötigen.

---

# 34. Benutzeroberfläche

Auch UI-Komponenten verwenden Capabilities.

Beispiele:

* Display
* Fenster
* Zwischenablage
* Benachrichtigungen

---

# 35. Capability-Gruppen

Mehrere Capabilities können logisch gruppiert werden.

Beispiele:

* Camera Access
* Audio Access
* Storage Access
* Sensor Access

---

# 36. Capability-Bundles

Bundles vereinfachen Programminstallationen.

Intern bestehen sie ausschließlich aus Einzel-Capabilities.

---

# 37. Recovery

Nach einem Recovery:

* werden ungültige Capabilities entfernt,
* Handles aktualisiert,
* Ereignisse erzeugt.

---

# 38. Audit

Alle Capability-Operationen können protokolliert werden:

* Erzeugung
* Delegation
* Widerruf
* Verwendung
* Fehler

---

# 39. Datenschutz

Capability-Protokolle unterliegen:

* Datenminimierung
* Zugriffsschutz
* Aufbewahrungsrichtlinien
* lokaler Verarbeitung

---

# 40. Performance

Optimierungen:

* Per-CPU Capability Cache
* kompakte Handleprüfung
* generationbasierte Validierung
* lockfreie Tabellen
* Fast Paths

---

# 41. Sicherheit

Capabilities verhindern:

* Ambient Authority
* globale Administratorrechte
* Rechtevererbung
* Pfadmanipulation
* Rechteeskalation durch Objektnamen

Capability-Systeme setzen auf das Prinzip der minimalen Rechte, indem Prozesse ausschließlich über explizit erhaltene Berechtigungen verfügen.

---

# 42. Erweiterbarkeit

Neue Objekttypen definieren lediglich:

* neue Rechte
* neue Capability-Klassen
* neue Delegationsregeln

Das Sicherheitsmodell bleibt unverändert.

---

# 43. Auswirkungen

Positive Auswirkungen:

* konsistente Sicherheit
* geringe Kopplung
* einfache Delegation
* starke Isolation
* sichere Sandboxen
* kontrollierte Rechteweitergabe

Negative Auswirkungen:

* komplexerer Handle Manager
* höherer Implementierungsaufwand
* zusätzlicher Verwaltungsaufwand

---

# 44. Verworfene Alternativen

## ACL-zentrierte Sicherheit

Vorteile:

* bekanntes Modell

Nachteile:

* hohe Komplexität
* Ambient Authority
* schwierige Delegation

---

## Benutzerzentrierte Rechte

Vorteile:

* einfache Administration

Nachteile:

* globale Rechte
* schlechte Isolation

---

## Globale Administratorrechte

Vorteile:

* einfache Implementierung

Nachteile:

* vollständige Kompromittierung bei Fehlern
* ungeeignet für Least Privilege

---

# 45. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* ausschließlich Capabilities verwenden,
* keine Rechte aus Objektnamen ableiten,
* Delegationen explizit durchführen,
* Capability-Widerrufe unterstützen,
* Ereignisse über Capability-Änderungen veröffentlichen.

---

# 46. Teststrategie

Die Capability-Verwaltung wird geprüft durch:

* Unit-Tests
* Handle-Tests
* Delegationstests
* Widerrufstests
* Rechteabschwächung
* SMP-Tests
* Langzeittests
* Sicherheitstests
* Performancebenchmarks

---

# 47. Akzeptanzkriterien

Die ADR gilt als umgesetzt, wenn:

* alle Kernelobjekte ausschließlich über Capabilities erreichbar sind,
* Handles keine direkten Kernelreferenzen enthalten,
* Rechte vor jeder Operation geprüft werden,
* Delegation und Rechteabschwächung funktionieren,
* Capability-Widerruf konsistent umgesetzt ist,
* ACLs lediglich als optionale Kompatibilitätsschicht dienen.

---

# 48. Zukunftsperspektive

Langfristig soll das Capability-System erweitert werden um:

* kryptografisch signierte Capabilities
* hardwaregestützte Capability-Tags (z. B. CHERI-ähnliche Architekturen)
* verteilte Capabilities
* clusterweite Capability-Domänen
* KI-gestützte Rechteempfehlungen
* formale Verifikation sicherheitskritischer Capability-Pfade

---

# 49. Konsequenz

Die native capability-basierte Ressourcenverwaltung bildet das zentrale Sicherheitsmodell von NovaOS.

Jede Ressource wird ausschließlich über explizit vergebene, nicht fälschbare Capabilities verwendet. Dadurch entfallen globale Autoritäten und implizite Rechtevererbungen zugunsten eines konsistenten Least-Privilege-Modells.

---

# 50. Zusammenfassung

NovaOS implementiert eine **native capability-basierte Ressourcenverwaltung** als grundlegende Sicherheitsarchitektur. Alle Zugriffe auf Kernelobjekte erfolgen ausschließlich über geschützte Capabilities, die Objektreferenz und Rechteumfang gemeinsam definieren. Handles dienen als sichere Träger dieser Capabilities, während Delegation, Rechteabschwächung und Widerruf systemweit einheitlich umgesetzt werden. Das Ergebnis ist ein konsistentes, skalierbares und auf dem Prinzip der minimalen Rechte basierendes Sicherheitsmodell, das eng mit dem Kernel Object Graph und dem ereignisgesteuerten Kernel-Core verzahnt ist.
