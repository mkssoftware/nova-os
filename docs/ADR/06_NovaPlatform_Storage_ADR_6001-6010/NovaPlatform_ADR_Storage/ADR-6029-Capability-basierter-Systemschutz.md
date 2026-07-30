# ADR-6029: Capability-basierter Systemschutz

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Security Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS / Nova OS
- **Ersetzt:** Keine

---

# 1. Kontext

Nova OS verfolgt das Ziel, Sicherheit als grundlegenden Bestandteil der Systemarchitektur zu etablieren.

Klassische Betriebssysteme verwenden überwiegend:

- Benutzerrechte
- Gruppen
- Access Control Lists (ACL)
- Administrator-/Root-Konzepte

Diese Modelle führen häufig dazu, dass Prozesse deutlich mehr Berechtigungen besitzen als tatsächlich erforderlich.

Nova OS verwendet stattdessen einen **Capability-basierten Systemschutz**.

Dabei erhält jeder Prozess ausschließlich die Fähigkeiten (Capabilities), die er tatsächlich benötigt. Dieses Modell folgt dem Prinzip der geringsten Rechte (*Principle of Least Privilege*) und basiert auf nicht fälschbaren Berechtigungstoken. :contentReference[oaicite:0]{index=0}

---

# 2. Problemstellung

Ein identitätsbasierter Sicherheitsansatz besitzt mehrere Nachteile:

- übermäßige Berechtigungen
- privilegierte Prozesse
- große Angriffsfläche
- schwierige Isolation
- eingeschränkte Delegation einzelner Rechte

Nova OS benötigt daher ein Sicherheitsmodell, das Berechtigungen präzise und objektbezogen vergeben kann.

---

# 3. Betrachtete Alternativen

## Alternative A – Klassische Benutzerrechte

Rechte werden ausschließlich über Benutzer und Gruppen vergeben.

**Vorteile**

- bewährtes Verfahren
- hohe Kompatibilität

**Nachteile**

- grobe Berechtigungen
- hohe Privilegien
- eingeschränkte Isolation

---

## Alternative B – ACL-System

Objekte besitzen Access Control Lists.

**Vorteile**

- feinere Rechtevergabe
- etablierte Technologie

**Nachteile**

- komplexe Verwaltung
- Rechte werden objektseitig verwaltet
- schwierige Delegation

---

## Alternative C – Capability-basierter Systemschutz

Prozesse erhalten ausschließlich explizit zugewiesene Capabilities.

Eine Capability ist ein kryptografisch bzw. kernelseitig geschützter, nicht fälschbarer Berechtigungsnachweis für genau definierte Operationen. :contentReference[oaicite:1]{index=1}

**Vorteile**

- Principle of Least Privilege
- starke Isolation
- einfache Delegation
- geringe Angriffsfläche
- objektorientiertes Sicherheitsmodell

**Nachteile**

- höhere Implementierungskomplexität
- Anpassung bestehender Anwendungen erforderlich

---

# 4. Entscheidung

Nova OS verwendet ein **Capability-basiertes Sicherheitsmodell**.

Jeder Prozess besitzt ausschließlich die Capabilities, die ihm explizit zugewiesen wurden.

Es existiert **kein universeller Superuser**, der Sicherheitsprüfungen umgehen kann.

Jeder Zugriff erfolgt ausschließlich über gültige Capabilities.

---

# 5. Begründung

## Principle of Least Privilege

Jede Anwendung erhält ausschließlich die Rechte, die sie tatsächlich benötigt.

Dadurch wird die Angriffsfläche erheblich reduziert.

---

## Starke Isolation

Ein kompromittierter Prozess besitzt nur Zugriff auf die ihm zugewiesenen Ressourcen.

Andere Objekte bleiben geschützt.

---

## Objektorientierte Sicherheit

Da NovaFS objektbasiert arbeitet, lassen sich Capabilities direkt an Objekte koppeln.

Beispiele:

- Datei
- Verzeichnis
- Gerät
- Netzwerkport
- Kamera
- Mikrofon
- Prozess
- Speicherbereich
- IPC-Endpunkt

---

## Sichere Delegation

Eine Capability kann gezielt an einen anderen Prozess weitergegeben werden.

Dabei können Rechte reduziert werden.

Beispiel:

```text
Vollzugriff

↓

Nur Lesen

↓

Weitergabe
```

Ein Prozess kann niemals mehr Rechte delegieren, als er selbst besitzt. :contentReference[oaicite:2]{index=2}

---

## Grundlage für Sandboxing

Alle Anwendungen laufen standardmäßig in isolierten Sicherheitsdomänen.

Zusätzliche Rechte müssen explizit angefordert werden.

---

# 6. Architektur

```text
Application

        │

Capability Manager

        │

Capability Table

        │

Kernel Validation

        │

NovaFS Object
```

Jeder Zugriff wird über die Capability-Tabelle validiert.

---

# 7. Aufbau einer Capability

Jede Capability besteht mindestens aus:

- Capability-ID
- Objekt-ID
- Berechtigungen
- Besitzer
- Delegationsrechte
- Ablaufregeln
- Sicherheitsflags

```text
Capability

├── Capability ID
├── Object ID
├── Rights
├── Owner
├── Delegation
├── Expiration
└── Flags
```

---

# 8. Standard-Capabilities

Nova OS definiert mindestens folgende Capability-Klassen:

| Capability | Beschreibung |
|------------|--------------|
| FILE_READ | Datei lesen |
| FILE_WRITE | Datei schreiben |
| FILE_DELETE | Datei löschen |
| DIRECTORY_LIST | Verzeichnis lesen |
| NETWORK_CLIENT | Netzwerkzugriff |
| NETWORK_SERVER | Server öffnen |
| DEVICE_CAMERA | Kamera |
| DEVICE_MICROPHONE | Mikrofon |
| DEVICE_USB | USB-Geräte |
| PROCESS_CREATE | Prozesse starten |
| PROCESS_CONTROL | Prozesse steuern |
| IPC_CONNECT | IPC-Verbindung |
| SYSTEM_TIME | Systemzeit |
| POWER_CONTROL | Energieverwaltung |
| INSTALL_PACKAGE | Software installieren |
| ADMIN_POLICY | Systemrichtlinien verwalten |

Weitere Capability-Typen können ergänzt werden.

---

# 9. Laufzeitprüfung

Jeder Zugriff erfolgt nach folgendem Schema:

```text
System Call

        │

Capability vorhanden?

        │

Ja

↓

Objektzugriff

        │

Nein

↓

Access Denied
```

Der Kernel überprüft jede Capability vor der Ausführung.

---

# 10. Benutzerinteraktion

Benutzer können Capabilities während der Programmausführung freigeben oder verweigern.

Beispiele:

- Zugriff auf Dokumente
- Kamera
- Mikrofon
- Standort
- Netzwerk
- Zwischenablage
- Drucker

Anwendungen erhalten standardmäßig keine unnötigen Rechte.

---

# 11. Positive Konsequenzen

Der Capability-basierte Systemschutz bietet zahlreiche Vorteile.

## Höhere Sicherheit

Jede Anwendung besitzt nur minimale Rechte.

---

## Geringere Angriffsfläche

Ein kompromittierter Prozess kann nur innerhalb seiner Capabilities agieren.

---

## Einfache Delegation

Rechte können kontrolliert weitergegeben werden.

---

## Objektorientierte Architektur

Das Sicherheitsmodell passt direkt zur Objektarchitektur von NovaFS.

---

## Optimale Sandbox-Unterstützung

Jede Anwendung läuft standardmäßig isoliert.

---

## Zukunftssicherheit

Neue Gerätetypen und Systemdienste können über zusätzliche Capabilities integriert werden.

---

# 12. Negative Konsequenzen und Risiken

## Höherer Verwaltungsaufwand

Nova OS benötigt einen zentralen Capability Manager.

---

## Anpassung bestehender Anwendungen

Programme müssen fehlende Capabilities behandeln können.

---

## Umfangreiche Systemintegration

Kernel, IPC, Dateisystem und Gerätetreiber müssen das Capability-Modell unterstützen.

---

# 13. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Capability Manager
- Security Manager
- Object Manager
- Process Manager
- IPC Manager
- Device Manager
- Package Manager
- User Manager
- NovaFS Object Manager
- Kernel Security Layer

Alle sicherheitsrelevanten Komponenten MÜSSEN Capabilities unterstützen.

---

# 14. Umsetzungsregeln

Für Nova OS gelten folgende verbindliche Regeln:

- Jeder Zugriff MUSS über eine gültige Capability autorisiert werden.
- Capabilities MÜSSEN kernelseitig gegen Fälschung geschützt sein.
- Prozesse DÜRFEN keine impliziten Systemrechte besitzen.
- Capabilities DÜRFEN nur innerhalb definierter Regeln delegiert werden.
- Ein Prozess DARF niemals mehr Rechte weitergeben als er selbst besitzt.
- Anwendungen SOLLEN standardmäßig ohne zusätzliche Capabilities gestartet werden.
- Jede Capability MUSS eindeutig identifizierbar sein.
- Das Capability-System MUSS vollständig mit der Objektarchitektur von NovaFS kompatibel sein.
- Sicherheitsentscheidungen MÜSSEN unabhängig von Benutzer- oder Gruppenkennungen getroffen werden.

---

# 15. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6014 – Objektbasiertes Dateisystem
- ADR-6019 – Schutzrichtlinien pro Objekt
- ADR-6024 – Native Cluster-Unterstützung
- ADR-6025 – Eigenes NovaFS-Netzwerkprotokoll
- ADR-6027 – Unveränderliche Systemgenerationen
- ADR-6028 – Live-Update durch Namespace-Switch
- zukünftige ADR – Sandbox-Architektur
- zukünftige ADR – Benutzer- und Identitätsverwaltung
- IEEE Technology Navigator – Capability-based Security :contentReference[oaicite:3]{index=3}
- Genode OS Framework – Capability-based Security :contentReference[oaicite:4]{index=4}

---

# 16. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Capability-basierter Systemschutz als grundlegendes Sicherheitsmodell von Nova OS angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.