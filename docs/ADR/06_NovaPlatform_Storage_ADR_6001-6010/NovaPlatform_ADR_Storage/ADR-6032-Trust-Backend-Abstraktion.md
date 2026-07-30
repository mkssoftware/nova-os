# ADR-6032: Trust-Backend-Abstraktion

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Security Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS / Nova OS
- **Ersetzt:** Keine

---

# 1. Kontext

Nova OS soll auf einer Vielzahl unterschiedlicher Hardwareplattformen betrieben werden können.

Hierzu gehören unter anderem:

- Desktop-PCs
- Server
- Embedded-Systeme
- Industrie-PCs
- Virtuelle Maschinen
- ARM-Systeme
- Cloud-Instanzen

Nicht jede Plattform verfügt über dieselben Sicherheitskomponenten.

Beispiele:

- TPM 2.0
- Firmware Root of Trust
- Secure Enclave
- Hardware Security Module (HSM)
- Virtuelle TPMs
- reine Software-Implementierungen

Eine direkte Abhängigkeit von einer bestimmten Hardware würde die Portierbarkeit erheblich einschränken.

Nova OS führt deshalb eine **Trust-Backend-Abstraktion** ein.

Alle sicherheitsrelevanten Komponenten kommunizieren ausschließlich mit einer einheitlichen Trust-API.

Die konkrete Implementierung wird durch ein austauschbares Backend bereitgestellt. Dieses Architekturprinzip reduziert Hardwareabhängigkeiten und ermöglicht die Integration verschiedener Vertrauensanker hinter einer stabilen Schnittstelle. :contentReference[oaicite:0]{index=0}

---

# 2. Problemstellung

Eine direkte Bindung an ein bestimmtes Trust-Modul führt zu:

- geringer Portabilität,
- schwieriger Wartbarkeit,
- hoher Hardwareabhängigkeit,
- mehrfach implementierter Sicherheitslogik,
- erschwerter Erweiterbarkeit.

Nova OS benötigt daher eine Hardware-unabhängige Vertrauensarchitektur.

---

# 3. Betrachtete Alternativen

## Alternative A – Direkte TPM-Anbindung

Alle Sicherheitsfunktionen verwenden ausschließlich TPM-Aufrufe.

**Vorteile**

- einfache Implementierung
- vollständige Nutzung der TPM-Funktionen

**Nachteile**

- starke Hardwareabhängigkeit
- keine Unterstützung alternativer Trust-Module
- eingeschränkte Portabilität

---

## Alternative B – Mehrere unabhängige Implementierungen

Jedes Sicherheitsmodul implementiert die Unterstützung für TPM, HSM oder Software selbst.

**Vorteile**

- hohe Flexibilität

**Nachteile**

- redundanter Code
- inkonsistentes Verhalten
- hoher Wartungsaufwand

---

## Alternative C – Trust-Backend-Abstraktion

Alle Sicherheitskomponenten greifen ausschließlich auf eine gemeinsame Trust-Schnittstelle zu.

Die konkrete Implementierung erfolgt über austauschbare Backends.

**Vorteile**

- hardwareunabhängig
- wartungsfreundlich
- leicht erweiterbar
- einheitliches Sicherheitsmodell
- einfache Testbarkeit

**Nachteile**

- zusätzliche Abstraktionsschicht
- etwas höherer Implementierungsaufwand

---

# 4. Entscheidung

Nova OS führt eine **Trust-Backend-Abstraktion** ein.

Alle sicherheitsrelevanten Komponenten kommunizieren ausschließlich mit einer standardisierten Trust-API.

Die Auswahl des geeigneten Backends erfolgt zur Laufzeit oder während des Systemstarts.

---

# 5. Begründung

## Hardwareunabhängigkeit

Nova OS kann auf unterschiedlichen Plattformen betrieben werden, ohne den Sicherheitskern anzupassen.

---

## Erweiterbarkeit

Neue Vertrauensanker können durch zusätzliche Backends integriert werden.

Beispiele:

- TPM 2.0
- Virtuelles TPM
- HSM
- Secure Enclave
- zukünftige Hardwaremodule

---

## Einheitliche Programmierschnittstelle

Alle Sicherheitsmodule verwenden dieselbe API.

Dadurch bleibt der übrige Systemcode unverändert.

---

## Bessere Testbarkeit

Für Entwicklung und Continuous Integration kann ein Software-Backend verwendet werden.

Dadurch lassen sich Sicherheitsfunktionen ohne spezielle Hardware testen.

---

## Zukunftssicherheit

Die Architektur erlaubt die Integration zukünftiger Sicherheitsmodule ohne Änderungen an den Kernkomponenten.

---

# 6. Architektur

```text
                Nova OS

                    │

        Security Manager

                    │

            Trust API

                    │

     ┌──────────────┼──────────────┐

     ▼              ▼              ▼

 TPM Backend   HSM Backend   Software Backend

                    │

          Weitere Backends
```

Alle Aufrufe erfolgen ausschließlich über die Trust API.

---

# 7. Standardisierte Trust-API

Die Trust-API stellt mindestens folgende Funktionen bereit:

- Schlüssel erzeugen
- Schlüssel speichern
- Schlüssel löschen
- Daten signieren
- Signaturen prüfen
- Zufallszahlen erzeugen
- Integritätsmessungen abrufen
- Vertrauensstatus prüfen
- Geheimnisse versiegeln
- Geheimnisse freigeben

Backend-spezifische Funktionen dürfen die standardisierte API erweitern, jedoch nicht verändern.

---

# 8. Unterstützte Backends

Nova OS definiert zunächst folgende Backends:

| Backend | Beschreibung |
|----------|--------------|
| Software | Referenzimplementierung |
| TPM 2.0 | Hardware Root of Trust |
| Virtuelles TPM | Virtuelle Maschinen |
| HSM | Externe Hardware Security Module |
| Mock Backend | Tests und Entwicklung |

Weitere Backends können ergänzt werden.

---

# 9. Backend-Auswahl

Beim Systemstart erfolgt die Auswahl automatisch.

```text
Systemstart

↓

Hardware erkennen

↓

Trust Backend auswählen

↓

Initialisieren

↓

Security Manager
```

Die Auswahl kann durch Systemrichtlinien überschrieben werden.

---

# 10. Fehlerbehandlung

Falls ein Backend nicht verfügbar ist:

```text
Backend Fehler

↓

Alternatives Backend vorhanden?

↓

Ja

↓

Wechsel

↓

Nein

↓

Software Backend

↓

Warnung protokollieren
```

Dadurch bleibt das System grundsätzlich funktionsfähig.

---

# 11. Integration mit TPM

Das TPM-Backend implementiert unter anderem:

- Measured Boot
- PCR-Verwaltung
- Schlüsselspeicherung
- Attestation
- Sealing

Die übrigen Systemkomponenten kennen ausschließlich die Trust-API.

---

# 12. Integration mit HSM

Ein HSM-Backend kann dieselben API-Aufrufe bereitstellen.

Dadurch lassen sich zentrale Schlüsselverwaltungen oder Unternehmenslösungen integrieren, ohne den Sicherheitskern anzupassen. Dieses Entkopplungsprinzip wird auch bei Cloud-HSM-Architekturen verwendet, um Anwendungen von hardwarespezifischen APIs zu isolieren. :contentReference[oaicite:1]{index=1}

---

# 13. Positive Konsequenzen

Die Trust-Backend-Abstraktion bietet zahlreiche Vorteile.

## Hardwareunabhängigkeit

Nova OS bleibt auf unterschiedlichster Hardware lauffähig.

---

## Erweiterbarkeit

Neue Trust-Module können jederzeit ergänzt werden.

---

## Einheitliche Architektur

Alle Sicherheitskomponenten verwenden dieselbe API.

---

## Höhere Wartbarkeit

Hardwareabhängiger Code ist auf die jeweiligen Backends beschränkt.

---

## Einfache Tests

Eine Softwareimplementierung ermöglicht reproduzierbare Tests ohne Spezialhardware.

---

## Zukunftssicherheit

Neue Sicherheitsmodule können integriert werden, ohne bestehende Anwendungen anzupassen.

---

# 14. Negative Konsequenzen und Risiken

## Zusätzliche Abstraktionsschicht

Die Trust-API erhöht den Implementierungsaufwand.

---

## Unterschiedliche Fähigkeiten

Nicht jedes Backend unterstützt alle Funktionen.

Ein Capability-System für Backends ist daher erforderlich.

---

## Pflege mehrerer Implementierungen

Jedes Backend muss separat entwickelt und getestet werden.

---

# 15. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Security Manager
- TPM Manager
- HSM Manager
- Key Manager
- Secure Boot Manager
- Remote Attestation Manager
- Merkle Manager
- Capability Manager
- Generation Manager
- Recovery Manager

Alle sicherheitsrelevanten Komponenten MÜSSEN ausschließlich über die Trust-API mit Vertrauensankern kommunizieren.

---

# 16. Umsetzungsregeln

Für Nova OS gelten folgende verbindliche Regeln:

- Sicherheitsmodule MÜSSEN ausschließlich die Trust-API verwenden.
- Direkte TPM- oder HSM-Aufrufe außerhalb der Backends SIND NICHT ZULÄSSIG.
- Trust-Backends MÜSSEN austauschbar sein.
- Die Auswahl des Backends MUSS automatisch erfolgen können.
- Ein Software-Backend MUSS immer verfügbar sein.
- Backends MÜSSEN ihre unterstützten Fähigkeiten deklarieren.
- Backend-Wechsel DÜRFEN keine Änderungen an der öffentlichen API erfordern.
- Fehler eines Backends DÜRFEN nicht zum Ausfall des gesamten Sicherheitssystems führen.
- Alle Backends MÜSSEN dieselben Sicherheitsgarantien der Trust-API einhalten.

---

# 17. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6029 – Capability-basierter Systemschutz
- ADR-6030 – Merkle-Tree für Systemintegrität
- ADR-6031 – TPM als optionale Sicherheitskomponente
- zukünftige ADR – Secure Boot
- zukünftige ADR – Remote Attestation
- zukünftige ADR – Key Manager
- Google Cloud – Cloud HSM Architecture (Abstraktion von HSM-Hardware) :contentReference[oaicite:2]{index=2}
- Technische Beschreibung hardwarebasierter Vertrauensanker und TPM/HSM-Integration :contentReference[oaicite:3]{index=3}

---

# 18. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Trust-Backend-Abstraktion als verbindliche Sicherheitsarchitektur für Nova OS angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.