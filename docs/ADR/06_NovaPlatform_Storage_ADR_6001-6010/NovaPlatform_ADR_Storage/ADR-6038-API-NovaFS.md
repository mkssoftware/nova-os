# ADR-6038: API-Design von NovaFS

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** API Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS / Nova SDK
- **Ersetzt:** Keine

---

# 1. Kontext

NovaFS ist ein vollständig objektbasiertes Dateisystem.

Die Architektur basiert bereits auf:

- Object Storage
- Copy-on-Write
- Transaktionen
- Merkle-Trees
- Snapshots
- Protection Policies
- Capability-System
- Storage Pools

Die öffentliche API muss diese Architektur vollständig widerspiegeln.

Sie soll:

- einfach verständlich sein,
- stabil bleiben,
- sprachunabhängig funktionieren,
- objektorientiert aufgebaut sein,
- atomare Operationen unterstützen,
- langfristig binär kompatibel bleiben.

Dabei folgt NovaFS dem Grundsatz einer stabilen, dateisystemneutralen Schnittstelle mit klar definierten Operationen und einer Trennung zwischen öffentlicher API und interner Implementierung. :contentReference[oaicite:0]{index=0}

---

# 2. Problemstellung

Viele Dateisysteme besitzen APIs, welche:

- eng an POSIX gekoppelt sind,
- blockorientiert arbeiten,
- interne Strukturen offenlegen,
- schwer erweiterbar sind,
- mehrere konkurrierende APIs besitzen.

NovaFS benötigt stattdessen eine moderne API, welche direkt auf Objekten arbeitet.

---

# 3. Betrachtete Alternativen

## Alternative A – POSIX als einzige API

NovaFS stellt ausschließlich POSIX-Systemaufrufe bereit.

**Vorteile**

- maximale Kompatibilität
- vorhandene Software funktioniert

**Nachteile**

- objektorientierte Funktionen fehlen
- neue Features schwer integrierbar
- moderne Konzepte nur eingeschränkt nutzbar

---

## Alternative B – Proprietäre Einzel-APIs

Jede NovaFS-Komponente besitzt eine eigene API.

**Vorteile**

- maximale Flexibilität

**Nachteile**

- inkonsistente Schnittstellen
- hoher Wartungsaufwand
- schwierige Erweiterbarkeit

---

## Alternative C – Einheitliche objektorientierte NovaFS-API

Alle Funktionen werden über eine zentrale API bereitgestellt.

Objekte bilden die kleinste adressierbare Einheit.

**Vorteile**

- konsistente Architektur
- einfache Erweiterbarkeit
- sprachunabhängig
- moderne Objektorientierung
- stabile Schnittstellen

**Nachteile**

- initial höherer Entwicklungsaufwand

---

# 4. Entscheidung

NovaFS verwendet eine **einheitliche objektorientierte API**.

Alle Funktionen greifen ausschließlich auf NovaFS-Objekte zu.

Interne Implementierungsdetails bleiben vollständig verborgen.

---

# 5. Begründung

## Objektorientierung

Alle Ressourcen besitzen dieselbe Grundstruktur.

Beispiele:

- Datei
- Verzeichnis
- Snapshot
- Subvolume
- Storage Pool
- Gerät
- Protection Policy
- Generation

Dadurch entsteht eine konsistente API.

---

## Sprachunabhängigkeit

Die API bildet die Grundlage für Bindings in:

- C
- C++
- Rust
- C#
- Python
- Java
- Go
- NovaLang

---

## Zukunftssicherheit

Neue Objekttypen können ergänzt werden, ohne bestehende APIs zu verändern.

---

## Kapselung

Interne Datenstrukturen bleiben verborgen.

Die API beschreibt ausschließlich das öffentliche Verhalten.

---

# 6. Architektur

```text
Application

        │

Nova SDK

        │

NovaFS Public API

        │

Object Manager

        │

Storage Engine
```

Die Anwendungen kommunizieren ausschließlich mit der Public API.

---

# 7. API-Prinzipien

Die NovaFS-API basiert auf folgenden Grundregeln:

- Objektorientierung
- stabile Schnittstellen
- Versionierung
- Thread-Sicherheit
- atomare Operationen
- Capability-basierte Sicherheit
- transaktionale Änderungen
- sprachunabhängige Bindings

---

# 8. API-Ebenen

NovaFS definiert mehrere API-Schichten.

```text
Application API

↓

High-Level API

↓

Object API

↓

Transaction API

↓

Storage API

↓

Internal Engine
```

Jede Ebene besitzt klar definierte Verantwortlichkeiten.

---

# 9. Objektmodell

Alle Objekte besitzen mindestens:

```text
Object

├── Object ID
├── UUID
├── Object Type
├── Metadata
├── Version
├── Generation
├── Flags
└── Capabilities
```

Jedes Objekt wird über eine eindeutige Object-ID angesprochen.

---

# 10. Standardoperationen

Jeder Objekttyp unterstützt mindestens folgende Operationen:

- Create
- Open
- Read
- Write
- Update
- Delete
- Clone
- Snapshot
- Lock
- Unlock
- Get Metadata
- Set Metadata

Nicht unterstützte Operationen liefern einen definierten Fehlercode.

---

# 11. Transaktionen

Alle schreibenden Operationen erfolgen innerhalb einer Transaktion.

```text
Begin Transaction

↓

Operationen

↓

Commit

↓

Rollback
```

Dadurch bleiben alle Änderungen atomar.

Transaktionale Dateisystem-APIs haben sich als wirksames Mittel erwiesen, um inkonsistente Zustände über mehrere Objekte hinweg zu vermeiden. :contentReference[oaicite:1]{index=1}

---

# 12. Fehlerbehandlung

Alle API-Funktionen liefern strukturierte Fehlerobjekte.

Beispiel:

```text
Result

├── Status
├── Error Code
├── Message
├── Details
└── Context
```

Es werden keine plattformabhängigen Fehlercodes verwendet.

---

# 13. Versionierung

Die API besitzt eine eigene Versionsverwaltung.

```text
Major

Minor

Patch
```

Neue Funktionen dürfen ausschließlich Minor-Versionen erweitern.

Breaking Changes erfordern eine neue Major-Version.

---

# 14. Ereignismodell

NovaFS unterstützt Ereignisse.

Beispiele:

- Objekt erstellt
- Objekt gelöscht
- Snapshot erzeugt
- Tiering abgeschlossen
- Recovery gestartet
- Self-Healing abgeschlossen
- Integritätsfehler erkannt

Anwendungen können diese Ereignisse abonnieren.

---

# 15. Erweiterbarkeit

Neue Objekttypen können ergänzt werden.

Neue Funktionen werden über Feature-Flags registriert.

Dadurch bleibt die API langfristig kompatibel.

---

# 16. Sicherheit

Jeder API-Aufruf wird durch das Capability-System geprüft.

```text
API Call

↓

Capability Check

↓

Object Manager

↓

Storage Engine
```

Direkte Zugriffe auf interne Strukturen sind nicht zulässig.

---

# 17. Positive Konsequenzen

Die NovaFS-API bietet zahlreiche Vorteile.

## Einheitliche Architektur

Alle Komponenten verwenden dieselbe Programmierschnittstelle.

---

## Hohe Wartbarkeit

Interne Änderungen beeinflussen Anwendungen nicht.

---

## Einfache Sprachanbindung

Neue SDKs können ohne Änderungen am Dateisystem entwickelt werden.

---

## Hohe Sicherheit

Alle Zugriffe werden zentral geprüft.

---

## Zukunftssicherheit

Neue Funktionen können ergänzt werden, ohne bestehende Anwendungen zu verändern.

---

## Optimale NovaFS-Integration

Alle bisherigen ADRs greifen auf dieselbe API zurück.

---

# 18. Negative Konsequenzen und Risiken

## Höherer Entwicklungsaufwand

Eine stabile API benötigt sorgfältige Planung.

---

## Versionierung

Langfristige Kompatibilität muss konsequent gepflegt werden.

---

## Dokumentation

Die API benötigt eine vollständige Referenzdokumentation sowie SDK-Beispiele.

---

# 19. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Object Manager
- Transaction Manager
- Capability Manager
- Snapshot Manager
- Recovery Manager
- Storage Pool Manager
- Tiering Manager
- Encryption Manager
- Compression Manager
- Nova SDK

Alle zukünftigen Komponenten MÜSSEN ausschließlich über die öffentliche NovaFS-API kommunizieren.

---

# 20. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Alle öffentlichen Funktionen MÜSSEN Teil der NovaFS Public API sein.
- Interne Datenstrukturen DÜRFEN niemals öffentlich zugänglich sein.
- Alle schreibenden Operationen MÜSSEN transaktional erfolgen.
- Alle API-Aufrufe MÜSSEN Capability-Prüfungen durchlaufen.
- Die API MUSS sprachunabhängig implementierbar sein.
- Neue Funktionen MÜSSEN rückwärtskompatibel ergänzt werden.
- Jeder Objekttyp MUSS eine einheitliche Grundschnittstelle besitzen.
- Fehler MÜSSEN über strukturierte Fehlerobjekte zurückgegeben werden.
- Alle öffentlichen Schnittstellen MÜSSEN versioniert werden.

---

# 21. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6014 – Objektbasiertes Dateisystem
- ADR-6015 – Transaktionales Journaling
- ADR-6029 – Capability-basierter Systemschutz
- ADR-6034 – Snapshot-Architektur
- ADR-6037 – Verschlüsselungsarchitektur
- zukünftige ADR – Nova SDK
- zukünftige ADR – Plugin-System
- Oracle WebNFS – Extended Filesystem API Architecture :contentReference[oaicite:2]{index=2}
- *The Mikado Filesystem* – RPC-basierte Dateisystem-API :contentReference[oaicite:3]{index=3}
- *The Design and Implementation of the Wave Transactional Filesystem* :contentReference[oaicite:4]{index=4}

---

# 22. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Einheitliche objektorientierte NovaFS Public API als verbindlicher Bestandteil der NovaFS-Architektur angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.