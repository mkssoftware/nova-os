# ADR-6031: TPM als optionale Sicherheitskomponente

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Security Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS / Nova OS
- **Ersetzt:** Keine

---

# 1. Kontext

Nova OS verfolgt das Ziel, ein sicheres Betriebssystem zu entwickeln, das sowohl auf moderner Enterprise-Hardware als auch auf älteren Computern, Embedded-Systemen und virtuellen Maschinen lauffähig ist.

Viele moderne Systeme verfügen über ein **Trusted Platform Module (TPM)**, welches kryptographische Schlüssel sicher speichern sowie Measured Boot und Remote Attestation unterstützen kann. TPMs werden als Hardware Root of Trust für Integritätsmessungen eingesetzt. :contentReference[oaicite:0]{index=0}

Gleichzeitig darf Nova OS **nicht** von einem TPM abhängig sein.

Der Betrieb ohne TPM muss vollständig unterstützt werden.

Das TPM dient ausschließlich als **optionale Sicherheitskomponente**, welche vorhandene Sicherheitsmechanismen erweitert.

---

# 2. Problemstellung

Ein verpflichtendes TPM besitzt mehrere Nachteile:

- ältere Hardware wird ausgeschlossen,
- virtuelle Maschinen besitzen nicht immer ein TPM,
- Embedded-Systeme verfügen häufig über keinen TPM-Chip,
- Reparaturen und Hardwaremigrationen werden erschwert.

Umgekehrt verzichtet ein vollständiger Verzicht auf TPM auf wichtige Sicherheitsfunktionen.

Nova OS benötigt daher eine flexible Architektur.

---

# 3. Betrachtete Alternativen

## Alternative A – TPM verpflichtend

Nova OS setzt grundsätzlich TPM 2.0 voraus.

**Vorteile**

- maximale Hardware-Sicherheit
- vereinfachte Sicherheitsarchitektur

**Nachteile**

- eingeschränkte Hardwarekompatibilität
- ältere Geräte werden ausgeschlossen
- ungeeignet für viele virtuelle Systeme

---

## Alternative B – Kein TPM

Nova OS verwendet ausschließlich Softwaremechanismen.

**Vorteile**

- maximale Kompatibilität
- einfache Hardwareanforderungen

**Nachteile**

- keine hardwaregestützte Schlüsselspeicherung
- keine Measured-Boot-Unterstützung
- keine TPM-basierte Attestation

---

## Alternative C – Optionales TPM

Nova OS funktioniert vollständig ohne TPM.

Ist ein TPM vorhanden, werden zusätzliche Sicherheitsfunktionen automatisch aktiviert.

**Vorteile**

- maximale Kompatibilität
- bessere Sicherheit auf unterstützter Hardware
- einfache Erweiterbarkeit
- keine Abhängigkeit von spezieller Hardware

**Nachteile**

- zwei Sicherheitsprofile müssen unterstützt werden
- zusätzlicher Implementierungsaufwand

---

# 4. Entscheidung

Nova OS verwendet das **TPM ausschließlich als optionale Sicherheitskomponente**.

Alle Kernfunktionen des Betriebssystems müssen auch ohne TPM vollständig nutzbar sein.

Ist ein TPM vorhanden und aktiviert, kann Nova OS zusätzliche Sicherheitsfunktionen verwenden.

---

# 5. Begründung

## Maximale Hardwarekompatibilität

Nova OS soll auf möglichst vielen Plattformen lauffähig sein.

Dazu gehören:

- Desktop-PCs
- Notebooks
- Server
- Industrie-PCs
- Embedded-Systeme
- virtuelle Maschinen
- Entwicklerhardware

---

## Hardware Root of Trust

Ein TPM kann kryptographische Schlüssel hardwaregeschützt speichern.

Dadurch verlassen private Schlüssel den TPM nicht. :contentReference[oaicite:1]{index=1}

---

## Unterstützung von Measured Boot

Während des Bootvorgangs können Firmware, Bootloader und Kernel gemessen werden.

Die Messwerte werden in den Platform Configuration Registers (PCR) des TPM abgelegt. :contentReference[oaicite:2]{index=2}

---

## Unterstützung von Remote Attestation

Der TPM kann die gemessenen PCR-Werte kryptographisch signieren.

Dadurch kann ein externer Server überprüfen, dass ein System mit einem erwarteten Softwarezustand gestartet wurde. :contentReference[oaicite:3]{index=3}

---

## Zukunftssicherheit

Durch die optionale Integration bleibt Nova OS offen für zukünftige Hardware-Sicherheitsmodule.

---

# 6. Architektur

```text
                 Nova OS

                     │

        ┌────────────┴────────────┐

        ▼                         ▼

 Software Security         Optional TPM

        │                         │

        └────────────┬────────────┘

                     ▼

             Security Manager
```

Der Security Manager entscheidet zur Laufzeit, welche Funktionen verfügbar sind.

---

# 7. Unterstützte TPM-Funktionen

Nova OS kann bei vorhandenem TPM unter anderem folgende Funktionen nutzen:

- sichere Schlüsselspeicherung
- Measured Boot
- Remote Attestation
- Hardware-gestützte Zufallszahlenerzeugung (sofern verfügbar)
- Versiegelung (Sealing) von Geheimnissen
- Plattformidentität
- Integritätsnachweise

Weitere Funktionen können zukünftig ergänzt werden.

---

# 8. Verhalten ohne TPM

Fehlt ein TPM, arbeitet Nova OS weiterhin vollständig.

Folgende Funktionen bleiben verfügbar:

- Merkle-Tree-Integritätsprüfung
- Prüfsummen
- Copy-on-Write
- Systemgenerationen
- Capability-System
- Self-Healing
- digitale Signaturen
- Secure Boot (plattformabhängig)

Lediglich TPM-spezifische Funktionen stehen nicht zur Verfügung.

---

# 9. Integration mit Merkle-Tree

Der TPM ersetzt den Merkle-Tree nicht.

Stattdessen können Merkle Roots zusätzlich im TPM abgesichert werden.

```text
Systemgeneration

↓

Merkle Root

↓

TPM (optional)

↓

Remote Attestation
```

Dadurch entsteht eine hardwaregestützte Vertrauenskette.

---

# 10. Integration mit Secure Boot

Secure Boot und TPM ergänzen sich.

Ein möglicher Ablauf:

```text
UEFI

↓

Secure Boot

↓

Bootloader

↓

Measured Boot

↓

TPM PCR

↓

Kernel

↓

Nova OS
```

Secure Boot verhindert das Laden nicht autorisierter Komponenten, während Measured Boot den tatsächlichen Startpfad kryptographisch dokumentiert. :contentReference[oaicite:4]{index=4}

---

# 11. Positive Konsequenzen

Die optionale TPM-Unterstützung bietet zahlreiche Vorteile.

## Hohe Kompatibilität

Nova OS bleibt auf älterer Hardware lauffähig.

---

## Erweiterte Sicherheit

Moderne Systeme profitieren von hardwaregestützter Kryptographie.

---

## Flexible Architektur

Der Sicherheitskern bleibt unabhängig von spezieller Hardware.

---

## Unterstützung von Remote Attestation

Unternehmen können die Integrität von Geräten nachweisen.

---

## Zukunftssicherheit

Weitere Hardware-Sicherheitsmodule können später integriert werden.

---

# 12. Negative Konsequenzen und Risiken

## Unterschiedliche Sicherheitsprofile

Systeme mit TPM besitzen zusätzliche Sicherheitsfunktionen.

---

## Zusätzlicher Implementierungsaufwand

Nova OS muss sowohl TPM- als auch Nicht-TPM-Systeme unterstützen.

---

## Herstellerunterschiede

Verschiedene TPM-Implementierungen können sich im Detail unterscheiden.

---

# 13. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Security Manager
- TPM Manager
- Secure Boot Manager
- Merkle Manager
- Generation Manager
- Boot Manager
- Key Manager
- Capability Manager
- Remote Attestation Manager
- Recovery Manager

Alle TPM-Funktionen MÜSSEN optional sein.

---

# 14. Umsetzungsregeln

Für Nova OS gelten folgende verbindliche Regeln:

- Nova OS MUSS vollständig ohne TPM lauffähig sein.
- TPM-Unterstützung MUSS automatisch erkannt werden.
- TPM-Funktionen DÜRFEN ausschließlich als Erweiterung verwendet werden.
- Private Schlüssel SOLLEN im TPM gespeichert werden, sofern verfügbar.
- Measured Boot SOLL TPM-PCRs verwenden.
- Remote Attestation SOLL TPM-basierte Nachweise unterstützen.
- Der Merkle-Tree MUSS unabhängig vom TPM funktionieren.
- TPM-spezifische Fehler DÜRFEN den normalen Systembetrieb nicht verhindern.
- Alle TPM-Operationen MÜSSEN klar versioniert und protokolliert werden.

---

# 15. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6027 – Unveränderliche Systemgenerationen
- ADR-6028 – Live-Update durch Namespace-Switch
- ADR-6029 – Capability-basierter Systemschutz
- ADR-6030 – Merkle-Tree für Systemintegrität
- zukünftige ADR – Secure Boot
- zukünftige ADR – Remote Attestation
- Microsoft Learn – How Windows uses the TPM :contentReference[oaicite:5]{index=5}
- Microsoft Learn – Secure the Windows boot process :contentReference[oaicite:6]{index=6}
- Microsoft Learn – TPM recommendations :contentReference[oaicite:7]{index=7}

---

# 16. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** TPM als optionale Sicherheitskomponente für Nova OS angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.