# ADR-6035: Tiered Storage und Rebalancing

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Storage Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS / Nova OS
- **Ersetzt:** Keine

---

# 1. Kontext

NovaFS verwaltet Speichergeräte unterschiedlicher Leistungsklassen innerhalb gemeinsamer Storage Pools.

Unterstützte Speichermedien umfassen unter anderem:

- Persistent Memory (PMem)
- NVMe SSD
- SATA SSD
- SAS SSD
- HDD
- Netzwerkspeicher
- Objekt-Storage
- Archivspeicher

Diese Speichermedien unterscheiden sich erheblich hinsichtlich:

- Latenz
- Bandbreite
- IOPS
- Kapazität
- Energieverbrauch
- Kosten

NovaFS führt daher eine Architektur für **Tiered Storage** mit automatischem **Rebalancing** ein.

Dabei werden Daten abhängig von Nutzung, Richtlinien und Systemzustand automatisch zwischen verschiedenen Speicherebenen verschoben, um Leistung und Speicherkosten zu optimieren. :contentReference[oaicite:0]{index=0}

---

# 2. Problemstellung

Eine statische Datenplatzierung führt zu:

- ineffizienter Nutzung schneller Speicher
- unnötig hohen Speicherkosten
- schlechter Lastverteilung
- ungleichmäßiger Geräteauslastung
- sinkender Gesamtleistung

NovaFS benötigt daher einen Mechanismus, der Daten automatisch auf geeignete Speicherklassen verteilt.

---

# 3. Betrachtete Alternativen

## Alternative A – Feste Datenplatzierung

Dateien verbleiben dauerhaft auf dem ursprünglich gewählten Datenträger.

**Vorteile**

- einfache Implementierung
- geringe Verwaltungslogik

**Nachteile**

- schlechte Ressourcenauslastung
- keine automatische Optimierung
- ineffiziente Nutzung schneller Medien

---

## Alternative B – Manuelle Migration

Administratoren verschieben Daten zwischen Speicherebenen.

**Vorteile**

- vollständige Kontrolle
- einfache Architektur

**Nachteile**

- hoher Verwaltungsaufwand
- fehleranfällig
- nicht skalierbar

---

## Alternative C – Automatisches Tiered Storage mit Rebalancing

NovaFS analysiert kontinuierlich den Datenzugriff und verschiebt Objekte automatisch zwischen Speicherklassen.

**Vorteile**

- optimale Performance
- geringere Kosten
- automatische Lastverteilung
- hohe Skalierbarkeit
- optimale Geräteauslastung

**Nachteile**

- höhere Implementierungskomplexität
- kontinuierliche Analyse erforderlich

---

# 4. Entscheidung

NovaFS verwendet **automatisches Tiered Storage mit intelligentem Rebalancing**.

Jedes Objekt besitzt eine definierte Storage Class.

Ein Tiering-Manager entscheidet anhand konfigurierbarer Richtlinien über die optimale Platzierung.

Das Rebalancing erfolgt transparent und ohne Unterbrechung des Systembetriebs.

---

# 5. Begründung

## Optimale Performance

Häufig genutzte ("Hot") Daten werden auf schnellen Speichermedien gespeichert.

---

## Niedrigere Kosten

Selten genutzte ("Cold") Daten werden automatisch auf kostengünstigere Speichermedien verschoben.

---

## Automatische Lastverteilung

Ungleichmäßig ausgelastete Geräte werden kontinuierlich ausgeglichen.

---

## Skalierbarkeit

Neue Geräte können jederzeit in einen Storage Pool aufgenommen werden.

Das Rebalancing verteilt vorhandene Daten automatisch.

---

## Integration mit NovaFS

Die Objektarchitektur ermöglicht die Migration einzelner Objekte ohne Auswirkungen auf Anwendungen.

---

# 6. Architektur

```text
Storage Pool

├── Tier 0  (Persistent Memory)
├── Tier 1  (NVMe SSD)
├── Tier 2  (SATA SSD)
├── Tier 3  (HDD)
├── Tier 4  (Nearline Storage)
└── Tier 5  (Archive / Object Storage)
```

Die Anzahl der Tiers ist nicht fest vorgegeben.

---

# 7. Storage Classes

NovaFS definiert standardmäßig folgende Storage Classes:

| Storage Class | Beschreibung |
|---------------|--------------|
| HOT | Maximale Performance |
| WARM | Mittlere Performance |
| COLD | Günstige Speicherung |
| ARCHIVE | Langzeitarchiv |
| SYSTEM | Betriebssystem |
| RECOVERY | Recovery-Umgebung |
| DATABASE | Datenbanken |
| MEDIA | Multimedia |
| BACKUP | Sicherungen |

Weitere Klassen können ergänzt werden.

---

# 8. Tiering-Kriterien

Der Tiering-Manager berücksichtigt unter anderem:

- Zugriffshäufigkeit
- Schreibintensität
- Leseintensität
- Objektgröße
- Alter der Daten
- verfügbare Kapazität
- Protection Policy
- Storage Class
- Energiezustand
- Administratorrichtlinien

Die Datenmigration erfolgt richtlinienbasiert und orientiert sich am tatsächlichen Nutzungsverhalten. :contentReference[oaicite:1]{index=1}

---

# 9. Rebalancing

Das Rebalancing sorgt für eine gleichmäßige Auslastung aller Geräte.

```text
Storage Pool

↓

Neue Geräte

↓

Auslastung prüfen

↓

Objekte verschieben

↓

Last ausgeglichen
```

Rebalancing erfolgt im Hintergrund.

---

# 10. Objektmigration

Die Migration eines Objektes erfolgt transaktional.

```text
Objekt

↓

Neues Ziel bestimmen

↓

Copy-on-Write

↓

Metadaten aktualisieren

↓

Alte Referenz freigeben
```

Der Objektzugriff bleibt während der Migration verfügbar.

Durch die Copy-on-Write-Architektur werden Änderungen atomar durchgeführt. :contentReference[oaicite:2]{index=2}

---

# 11. Integration mit Copy-on-Write

Tiering verwendet dieselben Mechanismen wie Copy-on-Write.

Während einer Migration werden bestehende Objekte nicht überschrieben.

Neue Referenzen werden erst nach erfolgreichem Abschluss aktiviert.

---

# 12. Integration mit Protection Policies

Bestimmte Schutzrichtlinien können die Speicherplatzierung beeinflussen.

Beispiele:

- SYSTEM → bevorzugt Tier 1
- DATABASE → Tier 0 oder Tier 1
- ARCHIVE → Tier 5
- RECOVERY → redundanter Tier 1

Dadurch lassen sich Leistungs- und Sicherheitsanforderungen kombinieren.

---

# 13. Integration mit Replikation

Bei replizierten Objekten kann jede Kopie auf unterschiedlichen Tiers gespeichert werden.

Beispiel:

```text
Replica 1

↓

NVMe

Replica 2

↓

HDD
```

Dies erhöht sowohl Verfügbarkeit als auch Wirtschaftlichkeit.

---

# 14. Positive Konsequenzen

Die Tiered-Storage-Architektur bietet zahlreiche Vorteile.

## Optimale Performance

Häufig genutzte Daten befinden sich auf schnellen Medien.

---

## Niedrigere Kosten

Selten genutzte Daten werden automatisch auf kostengünstige Speicher verschoben.

---

## Automatische Optimierung

Administratoren müssen Daten nicht manuell verschieben.

---

## Höhere Skalierbarkeit

Neue Geräte werden automatisch integriert.

---

## Gleichmäßige Lastverteilung

Alle Geräte werden effizient genutzt.

---

## Optimale NovaFS-Integration

Tiering arbeitet direkt mit Objekten, Subvolumes und Storage Pools zusammen.

---

# 15. Negative Konsequenzen und Risiken

## Höhere Komplexität

Ein Tiering-Manager muss kontinuierlich Entscheidungen treffen.

---

## Zusätzliche Hintergrundaktivität

Migrationen erzeugen zusätzliche I/O-Last.

---

## Richtlinienkonflikte

Mehrere Protection Policies können konkurrierende Anforderungen stellen.

---

# 16. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Tiering Manager
- Rebalancing Manager
- Storage Pool Manager
- Object Manager
- Zone Manager
- Protection Policy Manager
- Transaction Manager
- Self-Healing Manager
- Snapshot Manager
- Cluster Manager

Alle zukünftigen Storage-Optimierungen SOLLEN auf dieser Tiering-Architektur aufbauen.

---

# 17. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Jeder Storage Pool MUSS mehrere Speicherklassen unterstützen.
- Tiering MUSS transparent für Anwendungen erfolgen.
- Objektmigrationen MÜSSEN transaktional durchgeführt werden.
- Rebalancing DARF den laufenden Betrieb nicht unterbrechen.
- Copy-on-Write MUSS während jeder Migration verwendet werden.
- Protection Policies MÜSSEN bei Tiering-Entscheidungen berücksichtigt werden.
- Rebalancing MUSS automatisch neue Geräte integrieren.
- Administratoren MÜSSEN automatische und manuelle Tiering-Regeln definieren können.
- Alle Migrationen MÜSSEN vollständig protokolliert werden.

---

# 18. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6012 – Copy-on-Write für Metadaten
- ADR-6022 – Zonenbasierte Datenverteilung
- ADR-6023 – Multi-Disk-Storage-Pools
- ADR-6033 – Separates Recovery-Subvolume
- ADR-6034 – Snapshot-Architektur
- zukünftige ADR – Tiering Manager
- zukünftige ADR – Storage Policy Manager
- TechTarget – Tiered Storage Overview :contentReference[oaicite:3]{index=3}
- OpenZFS Documentation – Copy-on-Write :contentReference[oaicite:4]{index=4}
- *A Survey on Tiering and Caching in High-Performance Storage Systems* :contentReference[oaicite:5]{index=5}

---

# 19. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Tiered Storage und automatisches Rebalancing als verbindlicher Bestandteil der NovaFS-Architektur angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.