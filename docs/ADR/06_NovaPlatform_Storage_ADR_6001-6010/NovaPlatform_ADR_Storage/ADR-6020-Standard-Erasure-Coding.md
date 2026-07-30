# ADR-6020: Reed-Solomon als Standard-Erasure-Coding

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Storage Protection
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS
- **Ersetzt:** Keine

---

## 1. Kontext

NovaFS soll sowohl auf einzelnen Computern als auch auf hochverfügbaren Storage-Pools und verteilten Clustern eingesetzt werden.

Während Spiegelung (Mirroring) eine sehr hohe Verfügbarkeit bietet, verdoppelt oder verdreifacht sie den benötigten Speicherplatz.

Für große Speichersysteme ist daher ein effizienteres Verfahren erforderlich.

NovaFS soll Erasure Coding verwenden, um:

- Speicherplatz effizienter zu nutzen,
- mehrere gleichzeitige Ausfälle zu tolerieren,
- automatische Rekonstruktion zu ermöglichen,
- Self-Healing zu unterstützen,
- große Storage-Pools wirtschaftlich zu betreiben.

Reed-Solomon-Codes sind die am weitesten verbreitete Familie von Erasure-Codes in produktiven Speichersystemen und bilden die Grundlage vieler moderner Objekt- und Cluster-Speicher. :contentReference[oaicite:0]{index=0}

---

## 2. Problemstellung

Eine reine Spiegelung besitzt mehrere Nachteile:

- hoher Speicherverbrauch,
- hohe Schreibkosten,
- ineffiziente Ressourcennutzung,
- schlechte Skalierung bei großen Storage-Clustern.

NovaFS benötigt daher ein Verfahren, das:

- weniger Redundanz benötigt,
- mehrere gleichzeitige Laufwerksausfälle toleriert,
- Daten vollständig rekonstruieren kann,
- online erweitert werden kann.

---

## 3. Betrachtete Alternativen

### Alternative A – Ausschließlich Spiegelung

Alle Daten werden mehrfach gespeichert.

**Vorteile**

- einfache Implementierung
- schnelle Wiederherstellung
- hohe Leseleistung

**Nachteile**

- hoher Speicherverbrauch
- schlechte Skalierung
- hohe Kosten

---

### Alternative B – XOR-Parität (RAID-5/6)

Paritätsinformationen werden mittels XOR berechnet.

**Vorteile**

- geringer Speicherbedarf
- bewährte Technik

**Nachteile**

- begrenzte Fehlertoleranz
- eingeschränkte Skalierung
- ungeeignet für große verteilte Speicher

---

### Alternative C – Reed-Solomon Erasure Coding

Die Daten werden in mehrere Daten- und Paritätsfragmente aufgeteilt.

Jedes Fragment wird unabhängig gespeichert.

Die ursprünglichen Daten können rekonstruiert werden, solange ausreichend Fragmente vorhanden sind.

**Vorteile**

- sehr hohe Speichereffizienz
- mehrere gleichzeitige Ausfälle möglich
- mathematisch optimale Redundanz (MDS-Code)
- ideale Grundlage für Storage-Pools und Cluster

**Nachteile**

- höherer CPU-Aufwand
- komplexere Implementierung
- Rekonstruktion langsamer als Spiegelung

Reed-Solomon-Codes gelten als Maximum-Distance-Separable-(MDS)-Codes und bieten für eine gegebene Redundanz die maximal mögliche Wiederherstellungsfähigkeit. :contentReference[oaicite:1]{index=1}

---

## 4. Entscheidung

NovaFS verwendet **Reed-Solomon als Standardverfahren für Erasure Coding**.

Erasure Coding wird über die jeweilige Schutzrichtlinie (Protection Policy) aktiviert.

Nicht jede Datei muss Erasure Coding verwenden.

Die Entscheidung erfolgt objektbezogen.

---

## 5. Begründung

### Hohe Speichereffizienz

Reed-Solomon benötigt erheblich weniger Speicherplatz als vollständige Spiegelungen.

Beispiel:

```text
Mirror x3

100 GB Daten

↓

300 GB Speicher
```

```text
RS 8+2

100 GB Daten

↓

125 GB Speicher
```

---

### Hohe Fehlertoleranz

Mehrere gleichzeitige Laufwerksausfälle können ohne Datenverlust toleriert werden.

Beispiel:

```text
RS 8+2

8 Datenfragmente

2 Paritätsfragmente

↓

Beliebige zwei Fragmente dürfen ausfallen.
```

---

### Optimale Grundlage für Storage Pools

Fragmente können auf:

- unterschiedliche SSDs,
- HDDs,
- Controller,
- Server,
- Racks,
- Standorte

verteilt werden.

Dadurch wird das Risiko gemeinsamer Ausfälle erheblich reduziert.

---

### Integration in Self-Healing

Beschädigte Fragmente können automatisch rekonstruiert werden.

Der Benutzer bemerkt den Ausfall im Idealfall nicht.

---

### Unterstützung zukünftiger Cluster

Reed-Solomon eignet sich sowohl für lokale Pools als auch für verteilte Speichercluster und ist daher eine geeignete Standardtechnologie für NovaFS. :contentReference[oaicite:2]{index=2}

---

## 6. Standardprofile

NovaFS definiert mindestens folgende Reed-Solomon-Profile:

| Profil | Daten | Parität | Tolerierte Ausfälle |
|---------|------:|---------:|--------------------:|
| EC_2_1 | 2 | 1 | 1 |
| EC_4_2 | 4 | 2 | 2 |
| EC_6_2 | 6 | 2 | 2 |
| EC_8_2 | 8 | 2 | 2 |
| EC_8_3 | 8 | 3 | 3 |
| EC_8_4 | 8 | 4 | 4 |
| EC_12_4 | 12 | 4 | 4 |
| EC_16_4 | 16 | 4 | 4 |

Weitere Profile können zukünftig ergänzt werden.

---

## 7. Speicherung

Beispiel:

```text
Datei

↓

Fragmentierung

↓

D1
D2
D3
D4
D5
D6
D7
D8

↓

P1
P2
```

Die Fragmente werden anschließend entsprechend der Failure-Domain-Regeln verteilt.

---

## 8. Rekonstruktion

Bei Ausfall eines Fragments erfolgt automatisch:

```text
Fragment fehlt

        │

        ▼

Prüfsumme ungültig

        │

        ▼

Reed-Solomon Decoder

        │

        ▼

Fragment rekonstruieren

        │

        ▼

Neues Fragment speichern
```

Dieser Vorgang erfolgt vollständig transparent.

---

## 9. Failure Domains

Reed-Solomon-Fragmente DÜRFEN niemals gemeinsam auf derselben Failure Domain gespeichert werden.

Failure Domains können sein:

- Block
- Zone
- SSD
- HDD
- Controller
- Storage Node
- Rack
- Rechenzentrum

Dadurch werden gleichzeitige Ausfälle minimiert.

---

## 10. Positive Konsequenzen

Die Verwendung von Reed-Solomon bietet zahlreiche Vorteile.

### Hohe Speichereffizienz

Weniger Redundanz als Spiegelung.

---

### Hohe Verfügbarkeit

Mehrere gleichzeitige Ausfälle können toleriert werden.

---

### Unterstützung von Self-Healing

Beschädigte Fragmente werden automatisch rekonstruiert.

---

### Gute Skalierbarkeit

Geeignet für sehr große Storage-Pools.

---

### Clusterfähigkeit

Fragmente können standortübergreifend verteilt werden.

---

### Flexible Konfiguration

Jede Schutzrichtlinie kann unterschiedliche Reed-Solomon-Profile verwenden.

---

## 11. Negative Konsequenzen und Risiken

### Höherer CPU-Aufwand

Kodierung und Rekonstruktion benötigen zusätzliche Rechenleistung.

---

### Höhere Rekonstruktionskosten

Die Wiederherstellung eines Fragments ist aufwendiger als bei einer Spiegelung.

---

### Komplexere Implementierung

NovaFS benötigt:

- Reed-Solomon Encoder
- Reed-Solomon Decoder
- Fragment Manager
- Failure-Domain Manager

---

### Höhere Schreiblatenz

Beim Schreiben müssen zunächst Paritätsfragmente berechnet werden.

---

## 12. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Protection Policy Manager
- Storage Pool Manager
- Fragment Manager
- Object Manager
- Extent Manager
- Checksum Tree
- Self-Healing Manager
- Recovery Manager
- Cluster Manager
- Failure-Domain Manager
- Tiering Manager

Alle zukünftigen Speicherprofile SOLLEN Reed-Solomon unterstützen.

---

## 13. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Reed-Solomon MUSS das Standardverfahren für Erasure Coding sein.
- Erasure Coding MUSS über Schutzrichtlinien aktiviert werden.
- Fragmente MÜSSEN auf unterschiedliche Failure Domains verteilt werden.
- Jedes Fragment MUSS eine eigene Prüfsumme besitzen.
- Rekonstruktionen MÜSSEN vollständig transaktional erfolgen.
- Beschädigte Fragmente SOLLEN automatisch rekonstruiert werden.
- Neue Erasure-Coding-Profile DÜRFEN bestehende Profile nicht verändern.
- Spiegelung und Erasure Coding MÜSSEN parallel unterstützt werden.

---

## 14. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6018 – Prüfsummen für Nutzdaten
- ADR-6019 – Schutzrichtlinien pro Datei
- zukünftige ADR – Failure Domains
- zukünftige ADR – Storage Pools
- zukünftige ADR – Self-Healing
- zukünftige ADR – Cluster-Replikation
- ACM Transactions on Storage – *A Survey of the Past, Present, and Future of Erasure Coding for Storage Systems* :contentReference[oaicite:3]{index=3}

---

## 15. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Reed-Solomon als Standard-Erasure-Coding für NovaFS angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.