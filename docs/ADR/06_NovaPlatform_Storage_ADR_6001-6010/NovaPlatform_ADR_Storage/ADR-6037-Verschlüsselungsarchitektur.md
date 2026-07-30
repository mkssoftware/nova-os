# ADR-6037: Verschlüsselungsarchitektur

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Security Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS / Nova OS
- **Ersetzt:** Keine

---

# 1. Kontext

NovaFS besitzt bereits folgende Sicherheitsmechanismen:

- Copy-on-Write
- Merkle-Tree
- Prüfsummen
- Capability-System
- Protection Policies
- Snapshot-Architektur
- Trust-Backend-Abstraktion
- TPM-Unterstützung (optional)

Diese Komponenten gewährleisten Integrität und Wiederherstellbarkeit.

Zum Schutz der Vertraulichkeit gespeicherter Daten führt NovaFS zusätzlich eine vollständig integrierte **objektbasierte Verschlüsselungsarchitektur** ein.

Die Verschlüsselung erfolgt transparent innerhalb des Dateisystems.

Anwendungen arbeiten ausschließlich mit unverschlüsselten Daten.

---

# 2. Problemstellung

Ohne Verschlüsselung können gespeicherte Daten bei Verlust oder Diebstahl eines Datenträgers vollständig gelesen werden.

Ein modernes Dateisystem benötigt deshalb:

- Datenschutz im Ruhezustand (Data-at-Rest)
- unterschiedliche Schlüssel pro Objekt
- transparente Verschlüsselung
- Schlüsselrotation
- Integration mit Snapshots
- Integration mit Copy-on-Write
- Hardwarebeschleunigung
- zukünftige Algorithmusmigration

Eine globale Datenträgerverschlüsselung allein genügt diesen Anforderungen nicht.

Dateisystembasierte Verschlüsselung erlaubt dagegen unterschiedliche Schlüssel für verschiedene Dateien oder Verzeichnisbäume und lässt sich direkt in die Dateisystemarchitektur integrieren. :contentReference[oaicite:0]{index=0}

---

# 3. Betrachtete Alternativen

## Alternative A – Keine Verschlüsselung

Alle Daten werden im Klartext gespeichert.

**Vorteile**

- maximale Geschwindigkeit
- einfache Implementierung

**Nachteile**

- keine Vertraulichkeit
- ungeeignet für mobile Geräte
- hoher Datenschutzverlust

---

## Alternative B – Vollständige Datenträgerverschlüsselung

Der gesamte Datenträger wird verschlüsselt.

**Vorteile**

- einfache Nutzung
- transparent für Anwendungen

**Nachteile**

- keine unterschiedlichen Schlüssel
- keine objektbezogene Richtlinie
- eingeschränkte Integration in NovaFS

---

## Alternative C – Objektbasierte Verschlüsselung

Jedes Objekt besitzt eigene Verschlüsselungsinformationen.

Die Schlüssel werden aus einem Master Key abgeleitet.

**Vorteile**

- maximale Flexibilität
- unterschiedliche Schutzstufen
- optimale NovaFS-Integration
- objektbezogene Schlüssel
- zukünftige Algorithmusmigration

**Nachteile**

- komplexere Schlüsselverwaltung
- zusätzlicher Verwaltungsaufwand

---

# 4. Entscheidung

NovaFS verwendet eine **objektbasierte transparente Verschlüsselungsarchitektur**.

Jedes Objekt kann unabhängig verschlüsselt werden.

Die eigentlichen Datenschlüssel werden niemals direkt gespeichert.

Sie werden aus einem Master Key und objektspezifischen Informationen abgeleitet.

Die Verwendung eines Key-Derivation-Mechanismus mit objektspezifischen Nonces entspricht bewährten Ansätzen moderner Dateisystemverschlüsselung. :contentReference[oaicite:1]{index=1}

---

# 5. Begründung

## Objektorientierung

Da NovaFS objektbasiert arbeitet, lässt sich Verschlüsselung direkt an Objekte koppeln.

---

## Unterschiedliche Schutzstufen

Nicht jedes Objekt muss verschlüsselt werden.

Die Entscheidung erfolgt über:

- Protection Policies
- Storage Class
- Administratorrichtlinien

---

## Transparenz

Für Anwendungen bleibt die Verschlüsselung vollständig unsichtbar.

---

## Zukunftssicherheit

Algorithmen können später ersetzt werden, ohne die Gesamtarchitektur zu verändern.

---

## Integration

Die Verschlüsselung ergänzt:

- Merkle-Trees
- Prüfsummen
- Copy-on-Write
- Snapshots
- Tiering
- Replikation

---

# 6. Architektur

```text
Application

        │

NovaFS API

        │

Encryption Manager

        │

Key Manager

        │

Trust Backend

        │

Storage Object
```

Die Verschlüsselung erfolgt vollständig innerhalb von NovaFS.

---

# 7. Schlüsselhierarchie

NovaFS verwendet eine mehrstufige Schlüsselhierarchie.

```text
Root Key

↓

Master Key

↓

Subvolume Key

↓

Object Key

↓

Data Encryption Key
```

Direkte Root Keys werden niemals zum Verschlüsseln von Daten verwendet.

---

# 8. Schlüsselableitung

Jedes Objekt besitzt:

- Object ID
- Nonce
- Version
- Key-ID

Daraus wird ein individueller Datenschlüssel abgeleitet.

Hierfür wird eine standardisierte Key Derivation Function (KDF), beispielsweise HKDF, verwendet. Die Trennung zwischen Master Key und objektspezifischen Datenschlüsseln verbessert Sicherheit und Schlüsselverwaltung. :contentReference[oaicite:2]{index=2}

---

# 9. Unterstützte Algorithmen

NovaFS definiert zunächst folgende Algorithmen:

| Algorithmus | Einsatzgebiet |
|--------------|---------------|
| AES-256-GCM | Standard |
| ChaCha20-Poly1305 | Softwareoptimiert |
| XChaCha20-Poly1305 | Große Nonce-Räume |
| AES-256-XTS | Datenträgerkompatibilität |
| NONE | Keine Verschlüsselung |

Weitere Algorithmen können ergänzt werden.

Standardalgorithmus für neue Objekte ist:

**AES-256-GCM**

---

# 10. Objektmetadaten

Jedes verschlüsselte Objekt besitzt zusätzlich:

- Encryption Algorithm
- Key Version
- Nonce
- Key-ID
- Flags

```text
Object

├── Encryption Algorithm
├── Key Version
├── Nonce
├── Key ID
└── Flags
```

Der eigentliche Schlüssel wird niemals im Objekt gespeichert.

---

# 11. Schreibvorgang

```text
Anwendung

↓

Kompression

↓

Verschlüsselung

↓

Prüfsumme

↓

Merkle Tree

↓

Speichern
```

Die Prüfsumme wird über die gespeicherten verschlüsselten Daten berechnet.

---

# 12. Lesevorgang

```text
Objekt

↓

Prüfsumme

↓

Merkle Tree

↓

Entschlüsselung

↓

Dekompression

↓

Anwendung
```

Erst nach erfolgreicher Integritätsprüfung erfolgt die Entschlüsselung.

---

# 13. Schlüsselrotation

NovaFS unterstützt Schlüsselrotation.

```text
Master Key V1

↓

Neue Version

↓

Master Key V2

↓

Neue Objekte

↓

Migration
```

Bestehende Daten können schrittweise auf neue Schlüssel migriert werden.

---

# 14. Integration mit TPM

Ist ein TPM vorhanden, können Master Keys dort geschützt gespeichert oder versiegelt werden.

Fehlt ein TPM, übernimmt das Trust-Backend die Schlüsselverwaltung.

Dadurch bleibt die Architektur vollständig hardwareunabhängig.

---

# 15. Integration mit Snapshots

Snapshots enthalten keine zusätzlichen Schlüssel.

Die vorhandenen Objektreferenzen bleiben erhalten.

Nur neu erzeugte Objekte erhalten neue Datenschlüssel.

---

# 16. Integration mit Tiered Storage

Objektmigrationen ändern die Verschlüsselung nicht.

Ein Objekt bleibt unabhängig vom Speicherort identisch verschlüsselt.

---

# 17. Positive Konsequenzen

Die Verschlüsselungsarchitektur bietet zahlreiche Vorteile.

## Hohe Vertraulichkeit

Gestohlene Datenträger enthalten ausschließlich verschlüsselte Daten.

---

## Objektbezogene Schlüssel

Jedes Objekt kann unabhängig geschützt werden.

---

## Flexible Richtlinien

Protection Policies bestimmen den Verschlüsselungsgrad.

---

## Hardwareunterstützung

TPM und zukünftige Hardwaremodule lassen sich direkt integrieren.

---

## Zukunftssicherheit

Neue Algorithmen können später ergänzt werden.

---

## Optimale NovaFS-Integration

Die Verschlüsselung arbeitet direkt mit:

- Copy-on-Write
- Merkle-Trees
- Snapshots
- Tiering
- Replikation
- Self-Healing

zusammen.

---

# 18. Negative Konsequenzen und Risiken

## CPU-Belastung

Verschlüsselung benötigt zusätzliche Rechenleistung.

---

## Schlüsselverwaltung

Ein leistungsfähiger Key Manager ist erforderlich.

---

## Algorithmusmigration

Neue Algorithmen können eine schrittweise Datenmigration erfordern.

---

# 19. Auswirkungen auf andere Module

Diese Entscheidung betrifft insbesondere:

- Encryption Manager
- Key Manager
- Trust Backend
- TPM Manager
- Object Manager
- Merkle Manager
- Snapshot Manager
- Tiering Manager
- Transaction Manager
- Recovery Manager
- Capability Manager

Alle zukünftigen Datenschutzfunktionen SOLLEN auf dieser Verschlüsselungsarchitektur aufbauen.

---

# 20. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Die Verschlüsselung MUSS vollständig transparent erfolgen.
- Jedes Objekt DARF einen eigenen Datenschlüssel besitzen.
- Datenschlüssel MÜSSEN aus einem Master Key abgeleitet werden.
- Master Keys DÜRFEN niemals direkt zur Datenverschlüsselung verwendet werden.
- Die Schlüsselableitung MUSS über eine standardisierte KDF erfolgen.
- Die Verschlüsselungsalgorithmen MÜSSEN austauschbar und versionierbar sein.
- TPM-Unterstützung MUSS optional bleiben.
- Die Verschlüsselung MUSS vollständig mit Copy-on-Write kompatibel sein.
- Snapshots DÜRFEN keine erneute Verschlüsselung bestehender Objekte auslösen.
- Alle Schlüsseloperationen MÜSSEN vollständig protokolliert werden.

---

# 21. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6029 – Capability-basierter Systemschutz
- ADR-6030 – Merkle-Tree für Systemintegrität
- ADR-6031 – TPM als optionale Sicherheitskomponente
- ADR-6032 – Trust-Backend-Abstraktion
- ADR-6034 – Snapshot-Architektur
- ADR-6036 – Kompressionsarchitektur
- zukünftige ADR – Encryption Manager
- zukünftige ADR – Key Manager
- Linux Kernel – Filesystem-level Encryption (fscrypt) :contentReference[oaicite:3]{index=3}
- Linux Kernel – Encryption Modes and Key Derivation :contentReference[oaicite:4]{index=4}

---

# 22. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Objektbasierte Verschlüsselungsarchitektur als verbindlicher Bestandteil der NovaFS-Architektur angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.