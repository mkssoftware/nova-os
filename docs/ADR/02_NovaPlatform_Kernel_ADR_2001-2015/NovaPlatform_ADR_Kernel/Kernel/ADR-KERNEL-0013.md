# ADR-KERNEL-0013: Capability-Sicherheit

* **ADR-ID:** ADR-KERNEL-0013
* **Titel:** Capability-Sicherheit
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel-Architektur
* **Gültigkeitsbereich:** Security Manager, Object Manager, Handle-System, Process Manager, Systemaufrufe
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0011, ADR-KERNEL-0012, NPSPEC-SECURITY-0003

---

# 1. Zusammenfassung

NovaOS verwendet ein **Capability-basiertes Sicherheitsmodell** als primären Mechanismus zur Autorisierung von Kerneloperationen.

Jede Berechtigung wird durch eine **nicht fälschbare Capability** repräsentiert, die genau definiert, **welches Objekt**, **welche Operationen** und **unter welchen Bedingungen** zulässig sind. Es existieren keine globalen Sonderrechte oder impliziten Berechtigungen. Sämtliche Autorität wird ausschließlich über Capabilities delegiert. Dieses Sicherheitsmodell folgt dem Prinzip der minimalen Rechtevergabe (*Least Privilege*) und verhindert sogenannte *Ambient Authority*.

---

# 2. Kontext

Klassische Betriebssysteme verwenden häufig:

* Benutzerkonten
* Gruppen
* Access Control Lists (ACL)
* Administratorrechte
* Root-Rechte

Diese Modelle führen häufig dazu, dass Prozesse deutlich mehr Rechte besitzen als tatsächlich benötigt werden.

---

# 3. Problemstellung

Es musste entschieden werden,

* wie Zugriffsrechte vergeben werden,
* wie Berechtigungen überprüft werden,
* wie Rechte delegiert werden,
* wie Rechte entzogen werden,
* und wie Sicherheitsverletzungen minimiert werden.

---

# 4. Entscheidung

NovaOS verwendet ausschließlich Capability-basierte Autorisierung.

Dabei gilt:

* jede Berechtigung wird explizit vergeben,
* jede Capability besitzt einen eindeutigen Gültigkeitsbereich,
* jede Capability ist nicht fälschbar,
* Kerneloperationen benötigen immer eine passende Capability,
* fehlende Capabilities führen unmittelbar zur Ablehnung der Operation.

---

# 5. Motivation

Diese Architektur bietet:

* geringere Angriffsfläche
* Least Privilege
* keine impliziten Rechte
* einfache Delegation
* sichere Sandboxen
* bessere Isolation

---

# 6. Designprinzipien

Die Capability-Sicherheit basiert auf:

* Least Privilege
* Explicit Authority
* Unforgeable References
* Delegation
* Revocation
* Isolation
* Transparenz

---

# 7. Architekturübersicht

```text
                Benutzerprozess

                      │

               Capability Handle

                      │

                      ▼

             Capability Manager

                      │

          Rechteprüfung + Validierung

                      │

                      ▼

              Kernel Object Manager

                      │

                      ▼

                 Kernelobjekt
```

---

# 8. Definition einer Capability

Eine Capability beschreibt:

* Zielobjekt
* erlaubte Operationen
* Einschränkungen
* Besitzer
* Gültigkeitsstatus

Sie enthält niemals einen direkten Kernelzeiger.

---

# 9. Eigenschaften

Capabilities sind:

* nicht fälschbar
* nicht erratbar
* typisiert
* kryptographisch oder kernelintern geschützt
* ausschließlich durch den Kernel erzeugbar

---

# 10. Prinzip der minimalen Rechte

Ein Prozess erhält ausschließlich diejenigen Capabilities, die für seine aktuelle Aufgabe erforderlich sind.

Zusätzliche Rechte werden niemals automatisch vergeben.

---

# 11. Keine Ambient Authority

NovaOS kennt keine impliziten Systemrechte.

Insbesondere existieren keine Sonderkonten, deren Identität automatisch uneingeschränkten Zugriff gewährt. Jede Operation muss durch eine passende Capability autorisiert sein.

---

# 12. Capability-Typen

Beispiele:

* File Capability
* Directory Capability
* Device Capability
* Process Capability
* Thread Capability
* Window Capability
* IPC Capability
* Network Capability
* Timer Capability
* Driver Capability

---

# 13. Rechte

Eine Capability kann unter anderem folgende Rechte enthalten:

* Read
* Write
* Execute
* Delete
* Create
* Configure
* Duplicate
* Transfer
* Manage

---

# 14. Rechtekombinationen

Mehrere Rechte können kombiniert werden.

Beispiel:

```text
Datei A

Capability:

✔ Read

✔ Write

✖ Delete

✖ Execute
```

---

# 15. Delegation

Ein Prozess darf Capabilities weitergeben, sofern:

* dies ausdrücklich erlaubt ist,
* der Kernel die Delegation genehmigt,
* die Rechte nicht erweitert werden.

Delegierte Capabilities dürfen niemals mehr Rechte besitzen als die ursprüngliche Capability.

---

# 16. Revocation

Capabilities können jederzeit entzogen werden.

Gründe:

* Prozessende
* Sicherheitsverletzung
* Benutzerentscheidung
* Richtlinienänderung
* Objektlöschung

---

# 17. Capability-Vererbung

Beim Erzeugen neuer Prozesse können Capabilities vererbt werden.

Die Vererbung erfolgt ausschließlich explizit.

---

# 18. Capability-Transfer

Capabilities können über definierte IPC-Mechanismen zwischen Prozessen übertragen werden.

Der Kernel prüft jede Übertragung.

---

# 19. Capability-Prüfung

Vor jeder Kerneloperation erfolgt:

1. Handle-Auflösung
2. Capability-Prüfung
3. Rechteprüfung
4. Objekttypprüfung
5. Ausführung

---

# 20. Objektbindung

Jede Capability verweist exakt auf ein Kernelobjekt.

Eine Capability darf niemals mehrere Objekte gleichzeitig repräsentieren.

---

# 21. Prozessisolation

Capabilities gelten ausschließlich innerhalb des jeweiligen Sicherheitskontextes.

Andere Prozesse besitzen keinen Zugriff auf fremde Capabilities.

---

# 22. Kernel-Capabilities

Der Kernel besitzt interne System-Capabilities.

Diese werden ausschließlich innerhalb privilegierter Kernelkomponenten verwendet und niemals an Benutzerprogramme weitergegeben.

---

# 23. Sandboxing

Sandboxen entstehen automatisch durch eingeschränkte Capability-Sätze.

Eine Anwendung kann ausschließlich auf Ressourcen zugreifen, für die sie explizit autorisiert wurde.

---

# 24. Sicherheit

Die Capability-Sicherheit schützt vor:

* Rechteeskalation
* Confused-Deputy-Angriffen
* unautorisierten Objektzugriffen
* impliziten Administratorrechten
* Ambient Authority

Capability-basierte Modelle gelten als wirksamer Schutz gegen den *Confused Deputy*-Effekt, da Autorität explizit mit der Capability und nicht mit der Identität des Prozesses verbunden ist.

---

# 25. Performance

Das System soll:

* schnelle Capability-Prüfungen
* geringe Speicherkosten
* minimale Kernelaufrufe
* hohe Parallelität

ermöglichen.

---

# 26. Debug-Unterstützung

Optional speichert das System:

* Capability-Ersteller
* Delegationskette
* Rechtehistorie
* Nutzungsstatistik
* Widerrufshistorie

---

# 27. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* zeitlich begrenzte Capabilities
* kryptographisch signierte Capabilities
* Netzwerk-Capabilities
* Container-Capabilities
* Virtualisierungs-Capabilities
* Hardware-Capabilities

---

# 28. Auswirkungen

Positive Auswirkungen:

* sehr hohe Systemsicherheit
* konsequentes Least-Privilege-Modell
* einfache Sandboxen
* kontrollierte Rechtevergabe
* geringe Angriffsfläche
* saubere Delegation

Negative Auswirkungen:

* höhere Implementierungskomplexität
* zusätzlicher Verwaltungsaufwand
* sorgfältige Planung der Capability-Hierarchie erforderlich

---

# 29. Verworfene Alternativen

## Klassisches Benutzer-/Gruppenmodell

Vorteile:

* weit verbreitet
* bekannte Konzepte

Nachteile:

* implizite Rechte
* schwierige Delegation
* hohe Angriffsfläche

---

## Access Control Lists (ACL)

Vorteile:

* feingranulare Rechte

Nachteile:

* komplexe Verwaltung
* schwierige Nachvollziehbarkeit
* eingeschränkte Delegation

---

## Root-/Administrator-Modell

Vorteile:

* einfache Administration

Nachteile:

* vollständige Rechteeskalation
* hohe Sicherheitsrisiken
* Verletzung des Least-Privilege-Prinzips

---

# 30. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* sämtliche Rechte ausschließlich über Capabilities prüfen,
* niemals implizite Rechte vergeben,
* Delegationen ausschließlich über Kernel-APIs durchführen,
* Capability-Widerrufe sofort berücksichtigen,
* Capability-Prüfungen vor jeder privilegierten Operation durchführen.

---

# 31. Teststrategie

Die Capability-Sicherheit wird geprüft durch:

* Unit-Tests
* Rechteprüfungen
* Delegationstests
* Revocation-Tests
* Sandbox-Tests
* Penetrationstests
* Lasttests
* Langzeittests

---

# 32. Konsequenzen

Die Capability-Sicherheit bildet die zentrale Autorisierungsschicht von NovaOS.

Alle zukünftigen Kernelkomponenten müssen Ressourcen ausschließlich über Capabilities absichern. Benutzeridentitäten, Rollen oder Gruppen können ergänzende Verwaltungsinformationen bereitstellen, dürfen jedoch niemals die Capability-Prüfung umgehen oder ersetzen. Dadurch entsteht ein konsistentes Sicherheitsmodell mit expliziter Rechtevergabe, kontrollierter Delegation und hoher Widerstandsfähigkeit gegen Rechteeskalationen.

---

# 33. Zusammenfassung

NovaOS implementiert eine **Capability-basierte Sicherheitsarchitektur**, bei der jede privilegierte Operation eine explizite, nicht fälschbare Capability erfordert. Dieses Modell ersetzt implizite Systemrechte durch fein granulierte Autorisierung nach dem Prinzip der minimalen Rechtevergabe. Dadurch werden Isolation, Delegation und Sandboxing wesentlich vereinfacht und die Sicherheit der gesamten Kernelarchitektur nachhaltig verbessert.
