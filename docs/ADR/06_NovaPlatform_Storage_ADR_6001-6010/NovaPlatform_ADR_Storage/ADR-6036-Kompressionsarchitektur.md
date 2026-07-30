# ADR-6036: Kompressionsarchitektur

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Storage Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS / Nova OS
- **Ersetzt:** Keine

---

# 1. Kontext

NovaFS verwaltet Daten objektbasiert und verwendet bereits folgende Kernmechanismen:

- Copy-on-Write
- Objektbasierte Speicherung
- Prüfsummen
- Merkle-Trees
- Storage Pools
- Tiered Storage
- Protection Policies
- Snapshots

Zur Reduzierung des Speicherverbrauchs sowie zur Verbesserung der I/O-Leistung führt NovaFS eine integrierte **transparente Kompressionsarchitektur** ein.

Die Kompression erfolgt vollständig transparent für Anwendungen.

Anwendungen arbeiten ausschließlich mit den unkomprimierten Daten.

---

# 2. Problemstellung

Unkomprimierte Speicherung führt zu:

- höherem Speicherverbrauch
- längeren Lese- und Schreibvorgängen
- höherem Netzwerkverkehr
- geringerer Cache-Effizienz
- höheren Speicherkosten

NovaFS benötigt daher eine moderne, flexible und objektorientierte Kompressionsarchitektur.

---

# 3. Betrachtete Alternativen

## Alternative A – Keine Kompression

Alle Daten werden unverändert gespeichert.

**Vorteile**

- einfache Implementierung
- minimale CPU-Belastung

**Nachteile**

- hoher Speicherbedarf
- unnötige I/O-Last
- schlechte Cache-Ausnutzung

---

## Alternative B – Globale Dateisystemkompression

Das gesamte Dateisystem verwendet einen einzigen Kompressionsalgorithmus.

**Vorteile**

- einfache Verwaltung
- konsistentes Verhalten

**Nachteile**

- keine Optimierung für unterschiedliche Datentypen
- geringe Flexibilität

---

## Alternative C – Objektbasierte adaptive Kompression

Jedes Objekt besitzt eigene Kompressionseinstellungen.

Algorithmus und Kompressionsstufe können abhängig von Objekt, Subvolume oder Protection Policy gewählt werden.

**Vorteile**

- hohe Flexibilität
- optimale Speicherplatzausnutzung
- bessere Performance
- optimale NovaFS-Integration
- transparente Verarbeitung

**Nachteile**

- komplexere Metadaten
- höherer Verwaltungsaufwand

---

# 4. Entscheidung

NovaFS verwendet eine **objektbasierte transparente Kompressionsarchitektur**.

Die Kompression erfolgt auf Objektebene.

Jedes Objekt kann:

- unkomprimiert,
- mit unterschiedlichen Algorithmen oder
- mit unterschiedlichen Kompressionsstufen

gespeichert werden.

Die Auswahl erfolgt automatisch oder über Richtlinien.

---

# 5. Begründung

## Transparenz

Anwendungen müssen keinerlei Änderungen unterstützen.

Die Dekompression erfolgt automatisch beim Lesen.

---

## Optimale Speicherplatzausnutzung

Komprimierbare Daten benötigen deutlich weniger Speicherplatz.

---

## Höhere I/O-Leistung

Kleinere Datenmengen reduzieren:

- Datentransfers
- SSD-Schreibvorgänge
- Netzwerkverkehr
- Cache-Auslastung

---

## Flexible Algorithmen

Je nach Datentyp können unterschiedliche Algorithmen verwendet werden.

---

## Zukunftssicherheit

Neue Kompressionsalgorithmen können jederzeit ergänzt werden.

---

# 6. Architektur

```text
Application

        │

NovaFS API

        │

Compression Manager

        │

Compression Backend

        │

Storage Object
```

Die Kompression erfolgt vollständig innerhalb von NovaFS.

---

# 7. Unterstützte Algorithmen

NovaFS definiert zunächst folgende Algorithmen:

| Algorithmus | Einsatzgebiet |
|--------------|---------------|
| NONE | keine Kompression |
| LZ4 | maximale Geschwindigkeit |
| ZSTD | Standard |
| ZLIB | hohe Kompressionsrate |
| GZIP | Kompatibilität |
| LZMA | Archivierung |

Weitere Algorithmen können ergänzt werden.

Als Standardalgorithmus wird **Zstandard (ZSTD)** verwendet, da er in modernen Dateisystemen ein sehr gutes Verhältnis zwischen Kompressionsrate und Geschwindigkeit bietet. :contentReference[oaicite:0]{index=0}

---

# 8. Kompressionsstufen

Jeder Algorithmus kann mehrere Stufen unterstützen.

Beispiel:

```text
FAST

BALANCED

HIGH

MAXIMUM
```

Die konkrete Abbildung auf algorithmenspezifische Parameter erfolgt innerhalb des Compression Managers.

---

# 9. Objektmetadaten

Jedes Objekt speichert zusätzlich:

- Kompressionsalgorithmus
- Kompressionsstufe
- ursprüngliche Größe
- komprimierte Größe
- Kompressionsrate
- Version

```text
Object

├── Compression Algorithm
├── Compression Level
├── Original Size
├── Stored Size
├── Compression Ratio
└── Flags
```

---

# 10. Adaptive Kompression

NovaFS entscheidet optional automatisch, ob Daten komprimiert werden.

Dabei werden unter anderem berücksichtigt:

- Datentyp
- Dateigröße
- Entropie
- Kompressionsgewinn
- CPU-Auslastung
- Storage Class
- Administratorrichtlinien

Bereits komprimierte oder schlecht komprimierbare Daten können automatisch unkomprimiert gespeichert werden, um unnötigen CPU-Aufwand zu vermeiden. :contentReference[oaicite:1]{index=1}

---

# 11. Integration mit Copy-on-Write

Kompression erfolgt vor dem Schreiben neuer Objekte.

```text
Objekt

↓

Komprimieren

↓

Checksum

↓

Merkle Tree

↓

Speichern
```

Bereits gespeicherte Objekte bleiben unverändert.

---

# 12. Integration mit Prüfsummen

Prüfsummen werden grundsätzlich über die gespeicherten (komprimierten) Objekte berechnet.

Beim Lesen erfolgt:

```text
Objekt

↓

Checksum prüfen

↓

Dekomprimieren

↓

Anwendung
```

Dadurch wird sowohl die Integrität der gespeicherten Daten als auch eine korrekte Dekompression sichergestellt.

---

# 13. Integration mit Tiered Storage

Der Tiering Manager berücksichtigt zusätzlich:

- Kompressionsrate
- tatsächlichen Speicherverbrauch
- CPU-Kosten
- Zugriffshäufigkeit

Komprimierte Objekte können dadurch effizienter zwischen Speicherklassen verschoben werden.

---

# 14. Snapshot-Unterstützung

Snapshots speichern keine zusätzlichen Daten.

Komprimierte Objekte werden gemeinsam genutzt.

Copy-on-Write erzeugt nur für geänderte Objekte neue komprimierte Versionen.

---

# 15. Positive Konsequenzen

Die Kompressionsarchitektur bietet zahlreiche Vorteile.

## Geringerer Speicherbedarf

Komprimierbare Daten benötigen deutlich weniger Speicherplatz.

---

## Schnellere I/O

Kleinere Datenmengen reduzieren Schreib- und Leseoperationen.

---

## Höhere Cache-Effizienz

Mehr Daten passen in Arbeitsspeicher und Cache.

---

## Flexible Algorithmen

Jedes Objekt kann optimal komprimiert werden.

---

## Transparenz

Anwendungen benötigen keine Anpassungen.

---

## Optimale NovaFS-Integration

Kompression arbeitet direkt mit:

- Objekten
- Snapshots
- Tiering
- Storage Pools
- Merkle-Trees
- Prüfsummen

zusammen.

---

# 16. Negative Konsequenzen und Risiken

## CPU-Belastung

Kompression benötigt zusätzliche Rechenleistung.

---

## Zusätzliche Metadaten

Kompressionsinformationen müssen gespeichert werden.

---

## Ungeeignete Datentypen

Bereits komprimierte Daten profitieren häufig nicht von zusätzlicher Kompression und können sogar mehr Rechenzeit verursachen. :contentReference[oaicite:2]{index=2}

---

# 17. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Compression Manager
- Object Manager
- Tiering Manager
- Storage Pool Manager
- Snapshot Manager
- Checksum Manager
- Merkle Manager
- Transaction Manager
- Backup Manager
- Self-Healing Manager

Alle zukünftigen Speicheroptimierungen SOLLEN die Kompressionsarchitektur berücksichtigen.

---

# 18. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Kompression MUSS vollständig transparent erfolgen.
- Jedes Objekt DARF einen eigenen Kompressionsalgorithmus besitzen.
- Der Standardalgorithmus SOLL ZSTD sein.
- Kompressionsalgorithmen MÜSSEN austauschbar und versionierbar sein.
- Adaptive Kompression SOLL automatisch ungeeignete Daten erkennen.
- Copy-on-Write MUSS vollständig mit der Kompression kompatibel sein.
- Prüfsummen MÜSSEN für alle gespeicherten Objekte berechnet werden.
- Snapshots DÜRFEN keine erneute Kompression bereits vorhandener Objekte auslösen.
- Die Kompression DARF Anwendungen nicht sichtbar sein.

---

# 19. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6012 – Copy-on-Write für Metadaten
- ADR-6014 – Objektbasiertes Dateisystem
- ADR-6030 – Merkle-Tree für Systemintegrität
- ADR-6034 – Snapshot-Architektur
- ADR-6035 – Tiered Storage und Rebalancing
- zukünftige ADR – Compression Manager
- zukünftige ADR – Storage Policy Manager
- Btrfs Documentation – Transparent Compression :contentReference[oaicite:3]{index=3}
- FreeBSD OpenZFS Handbook – Zstandard Compression :contentReference[oaicite:4]{index=4}
- Meta Engineering – Zstandard and Btrfs Compression :contentReference[oaicite:5]{index=5}

---

# 20. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Objektbasierte Kompressionsarchitektur als verbindlicher Bestandteil der NovaFS-Architektur angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.