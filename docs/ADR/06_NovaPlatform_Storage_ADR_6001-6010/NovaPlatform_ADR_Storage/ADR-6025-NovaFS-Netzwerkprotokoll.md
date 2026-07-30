# ADR-6025: Eigenes NovaFS-Netzwerkprotokoll

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Cluster Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS
- **Ersetzt:** Keine

---

# 1. Kontext

NovaFS ist als verteiltes Dateisystem konzipiert.

Neben lokalen Datenträgern und Storage Pools sollen künftig auch mehrere Storage Nodes gemeinsam einen Cluster bilden.

Hierfür genügt ein allgemeines Dateifreigabeprotokoll (z. B. NFS oder SMB) nicht, da NovaFS intern zusätzliche Informationen übertragen muss, unter anderem:

- Objektmetadaten
- Transaktionen
- B+-Tree-Änderungen
- Journaleinträge
- Self-Healing-Kommandos
- Rebalancing
- Erasure-Coding-Fragmente
- Clusterstatus
- Heartbeats
- Node-Management

NovaFS benötigt daher ein eigenes Netzwerkprotokoll, das speziell auf die internen Anforderungen des Dateisystems abgestimmt ist.

---

# 2. Problemstellung

Die Verwendung vorhandener Netzwerkprotokolle besitzt mehrere Nachteile:

- fehlende Kenntnis der NovaFS-Objektstruktur
- unnötiger Protokoll-Overhead
- eingeschränkte Unterstützung transaktionaler Operationen
- keine integrierte Clusterverwaltung
- keine Unterstützung für Self-Healing
- keine Unterstützung für Erasure Coding
- keine native Objektkommunikation

NovaFS benötigt daher ein speziell entwickeltes Clusterprotokoll.

---

# 3. Betrachtete Alternativen

## Alternative A – SMB

SMB dient hauptsächlich der Dateifreigabe.

**Vorteile**

- weit verbreitet
- gute Betriebssystemunterstützung

**Nachteile**

- keine Objektkommunikation
- keine Clusterverwaltung
- hoher Overhead
- ungeeignet für interne Dateisystemkommunikation

---

## Alternative B – NFS

NFS ermöglicht den Zugriff auf entfernte Dateisysteme.

**Vorteile**

- einfach
- etabliert

**Nachteile**

- keine Integration mit NovaFS
- keine Transaktionsunterstützung
- keine objektorientierte Kommunikation

---

## Alternative C – Eigenes NovaFS-Protokoll

NovaFS entwickelt ein eigenes binäres Netzwerkprotokoll.

Dieses ist speziell für:

- Objektverwaltung
- Clusterbetrieb
- Replikation
- Self-Healing
- Metadaten
- Transaktionen

optimiert.

**Vorteile**

- optimale Performance
- vollständige Kontrolle
- minimale Latenz
- direkte Integration in NovaFS
- zukünftige Erweiterbarkeit

**Nachteile**

- höhere Entwicklungsaufwände
- eigenes Protokolldesign erforderlich

---

# 4. Entscheidung

NovaFS verwendet ein **eigenes binäres Netzwerkprotokoll**.

Dieses wird ausschließlich für die interne Kommunikation zwischen NovaFS-Komponenten verwendet.

Dateizugriffe von Anwendungen erfolgen weiterhin über das Betriebssystem und **nicht** direkt über das Netzwerkprotokoll.

Das Netzwerkprotokoll ist vollständig objektorientiert aufgebaut.

---

# 5. Begründung

## Optimierung für NovaFS

Das Protokoll kennt sämtliche NovaFS-Strukturen.

Dadurch entfallen unnötige Übersetzungsschichten.

---

## Minimale Latenz

Nur tatsächlich benötigte Informationen werden übertragen.

Dies reduziert:

- Netzwerkverkehr
- CPU-Last
- Speicherverbrauch

---

## Unterstützung transaktionaler Operationen

Transaktionen können direkt zwischen den Storage Nodes synchronisiert werden.

Dadurch bleibt das Dateisystem konsistent.

---

## Unterstützung von Self-Healing

Beschädigte Objekte können unmittelbar zwischen den beteiligten Nodes rekonstruiert werden.

---

## Zukunftssicherheit

Neue Funktionen können über neue Nachrichtentypen ergänzt werden, ohne bestehende Implementierungen grundlegend zu verändern.

---

# 6. Architektur

Das NovaFS-Protokoll besteht logisch aus mehreren Schichten.

```text
NovaFS Protocol

├── Transport Layer
├── Security Layer
├── Session Layer
├── Cluster Layer
├── Object Layer
├── Transaction Layer
├── Replication Layer
└── Recovery Layer
```

Jede Schicht besitzt klar definierte Aufgaben.

---

# 7. Nachrichtentypen

Das Protokoll unterstützt mindestens folgende Nachrichtentypen:

| Nachricht | Beschreibung |
|-----------|--------------|
| HELLO | Verbindungsaufbau |
| AUTH | Authentifizierung |
| HEARTBEAT | Alive-Prüfung |
| OBJECT_READ | Objekt lesen |
| OBJECT_WRITE | Objekt schreiben |
| OBJECT_DELETE | Objekt löschen |
| METADATA_UPDATE | Metadaten aktualisieren |
| TRANSACTION_BEGIN | Transaktion starten |
| TRANSACTION_COMMIT | Transaktion abschließen |
| TRANSACTION_ABORT | Transaktion abbrechen |
| REPLICATION | Daten replizieren |
| REBALANCE | Daten verschieben |
| RECOVERY | Wiederherstellung |
| SCRUB | Integritätsprüfung |
| HEALTH | Gesundheitsinformationen |
| NODE_JOIN | Knoten beitreten |
| NODE_LEAVE | Knoten verlassen |

Weitere Nachrichtentypen können später ergänzt werden.

---

# 8. Paketaufbau

Jedes Netzwerkpaket besitzt einen einheitlichen Header.

```text
Packet Header

Magic Number

Protocol Version

Packet Type

Flags

Transaction ID

Sequence Number

Payload Length

Checksum

Payload
```

Die Struktur ermöglicht eine effiziente Verarbeitung und zukünftige Erweiterungen.

---

# 9. Sicherheit

Alle Verbindungen müssen verschlüsselt und authentifiziert werden.

Mindestens folgende Mechanismen werden unterstützt:

- gegenseitige Authentifizierung der Clusterknoten
- Verschlüsselung aller Nutzdaten
- Integritätsprüfung aller Pakete
- Schutz vor Replay-Angriffen
- Rollen- und Berechtigungskonzept für Clusterknoten

Die konkrete kryptografische Ausgestaltung wird in einer separaten Spezifikation definiert.

---

# 10. Heartbeat-System

Alle Clusterknoten senden regelmäßig Heartbeats.

```text
Node A

        │

Heartbeat

        │

        ▼

Node B

        │

Timeout?

        │

Nein

↓

Node aktiv
```

Erfolgt innerhalb des definierten Zeitfensters keine Antwort, wird der betreffende Knoten als nicht erreichbar markiert.

---

# 11. Versionierung

Das Netzwerkprotokoll besitzt eine eigene Versionsnummer.

Beispiel:

```text
NovaFS Protocol

Major Version

Minor Version

Feature Flags
```

Dadurch können unterschiedliche NovaFS-Versionen innerhalb definierter Kompatibilitätsgrenzen miteinander kommunizieren.

---

# 12. Positive Konsequenzen

Die Einführung eines eigenen Netzwerkprotokolls bietet zahlreiche Vorteile.

## Optimale Performance

Keine unnötigen Protokollschichten.

---

## Geringe Latenz

Objekte können direkt übertragen werden.

---

## Vollständige Clusterintegration

Alle NovaFS-Komponenten verwenden dieselbe Kommunikationsbasis.

---

## Einfache Erweiterbarkeit

Neue Nachrichtentypen können ergänzt werden.

---

## Optimale Unterstützung von Self-Healing

Rekonstruktionen können direkt zwischen den Nodes erfolgen.

---

## Grundlage zukünftiger Geo-Cluster

Das Protokoll kann später für WAN-Verbindungen erweitert werden.

---

# 13. Negative Konsequenzen und Risiken

## Höherer Entwicklungsaufwand

Das Protokoll muss vollständig entwickelt, dokumentiert und gepflegt werden.

---

## Eigene Sicherheitsarchitektur

Authentifizierung, Verschlüsselung und Schlüsselverwaltung müssen implementiert werden.

---

## Kompatibilitätsmanagement

Neue Protokollversionen müssen mit älteren Versionen abgestimmt werden.

---

# 14. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Cluster Manager
- Network Manager
- Object Manager
- Transaction Manager
- Replication Manager
- Recovery Manager
- Self-Healing Manager
- Placement Engine
- Monitoring Manager
- Authentication Manager
- Security Manager

Alle Clusterfunktionen MÜSSEN ausschließlich über das NovaFS-Netzwerkprotokoll kommunizieren.

---

# 15. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Das NovaFS-Netzwerkprotokoll MUSS binär aufgebaut sein.
- Jede Nachricht MUSS einen standardisierten Header besitzen.
- Jede Verbindung MUSS authentifiziert werden.
- Alle übertragenen Daten MÜSSEN verschlüsselt sein.
- Jede Nachricht MUSS eine Integritätsprüfung besitzen.
- Heartbeats MÜSSEN regelmäßig übertragen werden.
- Alle Transaktionen MÜSSEN über das Protokoll synchronisiert werden.
- Neue Nachrichtentypen DÜRFEN die Abwärtskompatibilität nicht unbegründet verletzen.
- Das Protokoll MUSS versioniert werden.
- Das Protokoll SOLL für zukünftige Geo-Cluster und WAN-Verbindungen erweiterbar sein.

---

# 16. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6015 – Transaktionales Journaling
- ADR-6019 – Schutzrichtlinien pro Objekt
- ADR-6020 – Reed-Solomon als Standard-Erasure-Coding
- ADR-6021 – Failure Domains
- ADR-6022 – Zonenbasierte Datenverteilung
- ADR-6023 – Multi-Disk-Storage-Pools
- ADR-6024 – Native Cluster-Unterstützung
- zukünftige ADR – Cluster-Sicherheit
- zukünftige ADR – Konsensprotokoll
- zukünftige ADR – Replikationsstrategie

---

# 17. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Eigenes NovaFS-Netzwerkprotokoll als verbindlicher Bestandteil der Clusterarchitektur angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.