# ADR-6030: Merkle-Tree für Systemintegrität

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Security Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS / Nova OS
- **Ersetzt:** Keine

---

# 1. Kontext

NovaFS setzt bereits auf:

- Copy-on-Write
- Prüfsummen für Daten und Metadaten
- unveränderliche Systemgenerationen
- transaktionale Updates
- Self-Healing

Diese Mechanismen erkennen Datenfehler und gewährleisten Konsistenz.

Sie erlauben jedoch allein noch keinen kryptographischen Nachweis, dass ein kompletter Systemzustand unverändert geblieben ist.

NovaFS führt deshalb einen **Merkle-Tree** als zentrale Integritätsstruktur ein.

Jedes schützenswerte Objekt wird Teil eines kryptographischen Hash-Baums.

Bereits eine einzelne Änderung an einem Objekt verändert den Root Hash der gesamten Systemgeneration. Merkle-Bäume werden in modernen Integritätssystemen wie fs-verity und dm-verity eingesetzt, um Datenänderungen effizient und kryptographisch nachweisbar zu erkennen. :contentReference[oaicite:0]{index=0}

---

# 2. Problemstellung

Ein Dateisystem benötigt eine Möglichkeit,

- Manipulationen sicher zu erkennen,
- Integrität effizient zu prüfen,
- einzelne Objekte nachzuweisen,
- komplette Systemgenerationen kryptographisch zu identifizieren,
- digitale Signaturen auf den gesamten Systemzustand anzuwenden.

Eine einfache Prüfsumme pro Datei reicht hierfür nicht aus.

---

# 3. Betrachtete Alternativen

## Alternative A – Einzelne Prüfsummen

Jedes Objekt besitzt lediglich einen Hash.

**Vorteile**

- einfache Implementierung
- geringer Verwaltungsaufwand

**Nachteile**

- kein Gesamtnachweis
- keine effizienten Integritätsbeweise
- keine kryptographische Identität einer Generation

---

## Alternative B – Vollständiges Nachhashen

Bei jeder Integritätsprüfung werden sämtliche Objekte erneut gehasht.

**Vorteile**

- einfache Architektur

**Nachteile**

- hoher Rechenaufwand
- schlechte Skalierbarkeit
- ineffizient bei großen Dateisystemen

---

## Alternative C – Merkle-Tree

Alle Objekte werden in einem kryptographischen Hash-Baum organisiert.

Die Wurzel (Merkle Root) repräsentiert den vollständigen Zustand einer Systemgeneration.

**Vorteile**

- logarithmische Integritätsnachweise
- kryptographische Gesamtrepräsentation
- effiziente Verifikation einzelner Objekte
- ideale Grundlage für digitale Signaturen
- hohe Skalierbarkeit

**Nachteile**

- zusätzlicher Verwaltungsaufwand
- Hash-Baum muss gepflegt werden

---

# 4. Entscheidung

NovaFS verwendet einen **Merkle-Tree als zentrale Integritätsstruktur**.

Jede Systemgeneration besitzt genau einen Merkle Root.

Der Merkle Root beschreibt den vollständigen kryptographischen Zustand sämtlicher geschützter Objekte.

Alle Integritätsprüfungen erfolgen über diesen Hash-Baum.

---

# 5. Begründung

## Kryptographischer Gesamtnachweis

Eine Systemgeneration kann durch einen einzigen Hash eindeutig beschrieben werden.

Bereits eine Änderung an einem einzelnen Bit verändert den Merkle Root.

---

## Effiziente Verifikation

Zur Überprüfung eines einzelnen Objektes muss nicht das gesamte Dateisystem gelesen werden.

Es genügt der Pfad vom Objekt bis zum Root.

Die Verifikation erfolgt in logarithmischer Zeit. :contentReference[oaicite:1]{index=1}

---

## Grundlage für digitale Signaturen

Der Merkle Root kann digital signiert werden.

Damit wird die vollständige Systemgeneration authentifiziert.

---

## Unterstützung von Secure Boot

Der Bootloader kann den Root Hash überprüfen.

Nur vertrauenswürdige Generationen dürfen gestartet werden.

---

## Optimale Integration in NovaFS

Der Merkle-Tree ergänzt:

- Copy-on-Write
- Checksum Tree
- Objektbaum
- Systemgenerationen

ohne deren Aufgaben zu ersetzen.

---

# 6. Architektur

```text
System Generation

├── Object Tree
├── Directory Tree
├── Extent Tree
├── Checksum Tree
└── Merkle Tree
         │
         ▼
    Merkle Root
```

Der Merkle Root repräsentiert die gesamte Generation.

---

# 7. Aufbau

Jedes Blatt des Merkle-Trees beschreibt genau ein Objekt.

```text
Merkle Root

        │

 ┌──────┴──────┐

Hash A      Hash B

 │  │          │  │

O1 O2        O3 O4
```

Jeder innere Knoten wird aus den Hashwerten seiner Kinder berechnet.

---

# 8. Integritätsprüfung

Die Prüfung erfolgt wie folgt:

```text
Objekt lesen

        │

Hash berechnen

        │

Merkle-Pfad prüfen

        │

Root Hash

        │

Signierter Root

        │

Integrität bestätigt
```

Nur der notwendige Pfad wird verarbeitet.

---

# 9. Aktualisierung

Durch Copy-on-Write müssen nur geänderte Zweige neu berechnet werden.

```text
Objekt geändert

        │

Leaf Hash neu

        │

Eltern aktualisieren

        │

Neuer Root Hash
```

Unveränderte Teilbäume bleiben bestehen.

Dadurch werden Aktualisierungen effizient durchgeführt.

---

# 10. Beziehung zu Systemgenerationen

Jede Systemgeneration besitzt:

- Generation-ID
- Root Object
- Merkle Root
- digitale Signatur

```text
Generation 15

↓

Merkle Root A

↓

Generation 16

↓

Merkle Root B
```

Somit besitzt jede Generation eine eindeutige kryptographische Identität.

---

# 11. Self-Healing

Wird ein Integritätsfehler erkannt:

```text
Hash ungültig

        │

Merkle-Pfad prüfen

        │

Fehler lokalisieren

        │

Protection Policy

        │

Self-Healing

        │

Objekt rekonstruieren
```

Dadurch können beschädigte Objekte gezielt identifiziert werden.

---

# 12. Digitale Signaturen

NovaFS signiert ausschließlich den Merkle Root.

Nicht jede Datei muss einzeln signiert werden.

```text
Objekte

↓

Merkle Tree

↓

Merkle Root

↓

Digitale Signatur
```

Dieses Verfahren reduziert den Verwaltungsaufwand erheblich.

---

# 13. Positive Konsequenzen

Der Merkle-Tree bietet zahlreiche Vorteile.

## Kryptographische Integrität

Die gesamte Systemgeneration wird durch einen einzigen Root Hash repräsentiert.

---

## Effiziente Verifikation

Einzelne Objekte können ohne vollständiges Nachhashen überprüft werden.

---

## Unterstützung von Secure Boot

Der Bootloader kann Generationen schnell validieren.

---

## Digitale Signaturen

Nur der Merkle Root muss signiert werden.

---

## Optimale Integration

Copy-on-Write und Merkle-Tree ergänzen sich ideal.

---

## Zukunftssicherheit

Der Merkle-Tree bildet die Grundlage für:

- Remote Attestation
- Vertrauensnachweise
- verteilte Integritätsprüfungen
- Cluster-Validierung

---

# 14. Negative Konsequenzen und Risiken

## Zusätzliche Metadaten

Der Hash-Baum benötigt zusätzlichen Speicherplatz.

---

## Verwaltungsaufwand

Hash-Bäume müssen aktuell gehalten werden.

---

## Kryptographische Migration

Ein Wechsel des Hash-Algorithmus erfordert die Neuerzeugung der Merkle-Bäume.

---

# 15. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Merkle Manager
- Checksum Manager
- Object Manager
- Generation Manager
- Boot Manager
- Secure Boot Manager
- Transaction Manager
- Self-Healing Manager
- Recovery Manager
- Cluster Manager

Alle zukünftigen Integritätsmechanismen SOLLEN den Merkle-Tree verwenden.

---

# 16. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Jede Systemgeneration MUSS genau einen Merkle Root besitzen.
- Jedes geschützte Objekt MUSS Bestandteil eines Merkle-Trees sein.
- Änderungen MÜSSEN den betroffenen Teilbaum aktualisieren.
- Der Merkle Root MUSS kryptographisch eindeutig sein.
- Der Merkle Root SOLL digital signierbar sein.
- Integritätsprüfungen SOLLEN über Merkle-Proofs erfolgen.
- Der Merkle-Tree MUSS mit Copy-on-Write kompatibel sein.
- Self-Healing MUSS Merkle-Verifikationen unterstützen.
- Der verwendete Hash-Algorithmus MUSS austauschbar und versionierbar sein.

---

# 17. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6012 – Copy-on-Write für Metadaten
- ADR-6014 – Objektbasiertes Dateisystem
- ADR-6017 – Prüfsummen für Metadaten
- ADR-6018 – Prüfsummen für Nutzdaten
- ADR-6027 – Unveränderliche Systemgenerationen
- ADR-6029 – Capability-basierter Systemschutz
- zukünftige ADR – Secure Boot
- zukünftige ADR – Remote Attestation
- Linux Kernel – fs-verity (Merkle-Tree-basierte Dateiintegrität) :contentReference[oaicite:2]{index=2}
- Linux Kernel – dm-verity (Merkle-Tree für Blockgeräte) :contentReference[oaicite:3]{index=3}
- USENIX FAST – *On Scalable Integrity Checking for Secure Cloud Disks* :contentReference[oaicite:4]{index=4}

---

# 18. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Merkle-Tree als zentrale Integritätsstruktur für NovaFS und Nova OS angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.