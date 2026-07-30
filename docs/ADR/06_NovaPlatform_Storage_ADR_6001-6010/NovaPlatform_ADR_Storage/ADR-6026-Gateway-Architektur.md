# ADR-6026: Gateway-Architektur (SMB/NFS/WebDAV/S3)

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Interoperabilität
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS
- **Ersetzt:** Keine

---

# 1. Kontext

NovaFS besitzt eine eigene interne Speicherarchitektur und ein eigenes Clusterprotokoll (ADR-6025).

Anwendungen, Betriebssysteme und Fremdsysteme erwarten jedoch standardisierte Zugriffsprotokolle.

Hierzu gehören insbesondere:

- SMB
- NFS
- WebDAV
- S3
- zukünftige Speicherprotokolle

NovaFS soll diese Protokolle unterstützen, ohne seine interne Architektur an deren Einschränkungen anzupassen.

Daher wird eine Gateway-Architektur eingeführt.

Diese trennt die externe Kommunikation vollständig von der internen NovaFS-Objektverwaltung. Eine solche Trennung entspricht bewährten Architekturmustern zur Entkopplung interner Systeme von externen Schnittstellen. :contentReference[oaicite:0]{index=0}

---

# 2. Problemstellung

Eine direkte Implementierung aller Fremdprotokolle innerhalb des Dateisystemkerns würde zu folgenden Problemen führen:

- starke Kopplung
- hoher Wartungsaufwand
- komplexere Sicherheitsarchitektur
- schwierige Erweiterbarkeit
- vermischte Verantwortlichkeiten

NovaFS benötigt daher eine saubere Trennung zwischen internem Dateisystem und externen Zugriffsschnittstellen.

---

# 3. Betrachtete Alternativen

## Alternative A – Direkte Integration

Jedes Netzwerkprotokoll wird direkt in NovaFS implementiert.

**Vorteile**

- keine zusätzliche Übersetzungsschicht

**Nachteile**

- hohe Komplexität
- starke Kopplung
- schwierige Wartbarkeit

---

## Alternative B – Betriebssystemdienste

Das Betriebssystem stellt SMB-, NFS- oder WebDAV-Dienste bereit.

**Vorteile**

- einfache Implementierung
- Wiederverwendung vorhandener Software

**Nachteile**

- keine Kontrolle über Optimierungen
- eingeschränkte Nutzung von NovaFS-Funktionen
- uneinheitliches Verhalten

---

## Alternative C – Native Gateway-Architektur

Alle externen Protokolle werden über eigenständige Gateway-Komponenten bereitgestellt.

Die Gateways kommunizieren ausschließlich mit dem NovaFS-Netzwerkprotokoll.

**Vorteile**

- saubere Schichten
- einfache Erweiterbarkeit
- hohe Wartbarkeit
- klare Verantwortlichkeiten
- vollständige Nutzung aller NovaFS-Funktionen

**Nachteile**

- zusätzlicher Übersetzungsschritt
- Implementierung mehrerer Gateways erforderlich

---

# 4. Entscheidung

NovaFS verwendet eine **Gateway-Architektur**.

Alle externen Speicherprotokolle werden über eigenständige Gateway-Dienste bereitgestellt.

Der Dateisystemkern kennt ausschließlich:

- Objekte
- Transaktionen
- das NovaFS-Netzwerkprotokoll

Der Kern besitzt keine direkte Kenntnis über SMB, NFS, WebDAV oder S3.

---

# 5. Begründung

## Klare Schichten

Die interne Architektur bleibt unabhängig von externen Standards.

---

## Erweiterbarkeit

Neue Protokolle können jederzeit ergänzt werden.

Beispiele:

- FTP
- SCP
- rsync
- FUSE
- iSCSI
- zukünftige Cloud-Protokolle

---

## Wartbarkeit

Jedes Gateway besitzt einen klar abgegrenzten Verantwortungsbereich.

---

## Optimale Nutzung der NovaFS-Funktionen

Gateways können:

- Snapshots
- Schutzrichtlinien
- Versionierung
- Quotas
- Objektattribute

auf geeignete Weise nach außen abbilden.

---

## Zukunftssicherheit

Änderungen an SMB oder S3 erfordern keine Änderungen am Dateisystemkern.

---

# 6. Architektur

```text
                Anwendungen

                     │

      ┌──────────────┼──────────────┐

      ▼              ▼              ▼

    SMB            NFS          WebDAV

      │              │              │

      └──────────────┼──────────────┘

                     ▼

             Gateway Layer

                     │

                     ▼

        NovaFS Network Protocol

                     │

                     ▼

              NovaFS Cluster

                     │

                     ▼

              Storage Pools
```

Alle Gateways verwenden ausschließlich das NovaFS-Netzwerkprotokoll.

---

# 7. Standard-Gateways

NovaFS definiert mindestens folgende Gateway-Typen:

| Gateway | Beschreibung |
|----------|--------------|
| SMB Gateway | Windows-Dateifreigaben |
| NFS Gateway | Unix-/Linux-Dateifreigaben |
| WebDAV Gateway | HTTP-basierter Dateizugriff |
| S3 Gateway | Objektzugriff über S3-kompatible API |
| REST Gateway | Programmierschnittstelle |
| FUSE Gateway | Lokale Einbindung |
| SDK Gateway | Native Programmbibliotheken |

Weitere Gateways können später ergänzt werden.

---

# 8. Gateway-Aufgaben

Jedes Gateway übernimmt mindestens folgende Aufgaben:

- Authentifizierung
- Autorisierung
- Protokollübersetzung
- Sitzungsverwaltung
- Objektabbildung
- Fehlerübersetzung
- Performance-Caching
- Kompatibilitätsfunktionen

Das Gateway besitzt keinen direkten Zugriff auf physische Datenträger.

---

# 9. Objektabbildung

Ein Gateway übersetzt externe Zugriffe in NovaFS-Objekte.

```text
SMB Datei

        │

        ▼

Gateway

        │

        ▼

NovaFS Objekt

        │

        ▼

Object Manager
```

Diese Übersetzung erfolgt vollständig transparent.

---

# 10. Sicherheit

Jedes Gateway besitzt eine eigene Sicherheitskomponente.

Diese übernimmt:

- Benutzeranmeldung
- Rollenverwaltung
- Zugriffskontrolle
- Protokollspezifische Authentifizierung
- Sitzungsverwaltung

Die eigentliche Objektberechtigung wird anschließend durch NovaFS überprüft.

---

# 11. Performance

Gateways dürfen lokale Optimierungen verwenden.

Beispiele:

- Metadaten-Caching
- Read-Ahead
- Write-Back-Puffer
- Verbindungspooling
- Batch-Operationen

Dabei darf die Konsistenz des Dateisystems nicht beeinträchtigt werden.

---

# 12. Positive Konsequenzen

Die Gateway-Architektur bietet zahlreiche Vorteile.

## Saubere Trennung

Der Dateisystemkern bleibt unabhängig von externen Standards.

---

## Einfache Erweiterbarkeit

Neue Netzwerkprotokolle können problemlos ergänzt werden.

---

## Wartbarkeit

Jedes Gateway kann unabhängig entwickelt und aktualisiert werden.

---

## Hohe Kompatibilität

NovaFS kann gleichzeitig unterschiedliche Client-Protokolle bereitstellen.

---

## Optimale Sicherheit

Jedes Gateway besitzt eine eigene Authentifizierungs- und Sitzungsschicht.

---

## Zukunftssicherheit

Neue Protokolle können integriert werden, ohne den Dateisystemkern zu verändern.

---

# 13. Negative Konsequenzen und Risiken

## Zusätzliche Übersetzungsschicht

Jeder Zugriff durchläuft eine zusätzliche Verarbeitungsebene.

---

## Höherer Entwicklungsaufwand

Mehrere Gateway-Implementierungen müssen entwickelt und gepflegt werden.

---

## Protokollspezifische Unterschiede

Nicht alle Eigenschaften externer Protokolle lassen sich vollständig auf NovaFS-Objekte abbilden.

---

# 14. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Gateway Manager
- Authentication Manager
- Object Manager
- Network Manager
- Transaction Manager
- Security Manager
- Metadata Manager
- Snapshot Manager
- Quota Manager
- Monitoring Manager

Alle externen Dateizugriffe SOLLEN ausschließlich über Gateways erfolgen.

---

# 15. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Der Dateisystemkern DARF keine direkte Unterstützung für SMB, NFS, WebDAV oder S3 enthalten.
- Jeder externe Zugriff MUSS über ein Gateway erfolgen.
- Alle Gateways MÜSSEN ausschließlich das NovaFS-Netzwerkprotokoll verwenden.
- Gateways MÜSSEN unabhängig voneinander entwickelt und aktualisiert werden können.
- Gateways DÜRFEN keine direkten Datenträgerzugriffe durchführen.
- Authentifizierung MUSS vor jedem Objektzugriff erfolgen.
- Gateway-Caches DÜRFEN die Konsistenz von NovaFS nicht beeinträchtigen.
- Neue Protokolle SOLLEN als eigenständige Gateway-Module implementiert werden.
- Gateways MÜSSEN protokollspezifische Funktionen transparent auf NovaFS-Objekte abbilden.

---

# 16. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6014 – Objektbasiertes Dateisystem
- ADR-6019 – Schutzrichtlinien pro Objekt
- ADR-6023 – Multi-Disk-Storage-Pools
- ADR-6024 – Native Cluster-Unterstützung
- ADR-6025 – Eigenes NovaFS-Netzwerkprotokoll
- zukünftige ADR – Authentifizierung
- zukünftige ADR – Rollen- und Berechtigungssystem
- zukünftige ADR – REST API
- Google Cloud – Architecture Decision Records :contentReference[oaicite:1]{index=1}
- Überblick über verteilte Dateisystemarchitekturen :contentReference[oaicite:2]{index=2}

---

# 17. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Gateway-Architektur als verbindlicher Bestandteil der NovaFS-Architektur angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.