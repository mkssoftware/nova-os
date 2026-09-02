# ADR-CRYPTO-0002 – Verschlüsselung at Rest und in Transit

## Status

Angenommen

## Kategorie

Cryptography / Security / Data Protection

## Kontext

NovaOS verarbeitet Daten sowohl persistent als auch während ihrer Übertragung zwischen Komponenten, Geräten und entfernten Systemen.

Dabei müssen zwei grundlegende Schutzbereiche unterschieden werden:

```text
At Rest
→ gespeicherte Daten

In Transit
→ übertragene Daten
```

Beide benötigen kryptographischen Schutz, dürfen jedoch nicht als identischer Sicherheitsmechanismus behandelt werden.

---

## Entscheidung

NovaOS unterstützt **Verschlüsselung at Rest und in Transit als systemweite, policygesteuerte Sicherheitsmechanismen**.

```text
Data
 ├── Stored       → Encryption at Rest
 └── Transferred  → Encryption in Transit
```

Die konkrete Verschlüsselung wird über die Crypto-Architektur und Crypto Policies gesteuert.

---

## Grundprinzip

```text
Protect data where it rests.
Protect data while it moves.
```

Verschlüsselung ersetzt dabei weder Autorisierung noch Information Flow Control oder Data Sovereignty.

---

## Encryption at Rest

Persistente Daten müssen abhängig von ihrer Security Policy verschlüsselbar sein.

Dies umfasst insbesondere:

- Volumes,
- Dateien und NovaFiles,
- Object Storage,
- Snapshots,
- Backups,
- temporäre persistente Daten,
- System- und Anwendungsdaten.

```text
Data
 ↓
Storage Encryption
 ↓
Persistent Storage
```

Die Storage-Schicht muss Verschlüsselung als native Policy unterstützen können.

---

## Encryption in Transit

Datenübertragungen müssen abhängig vom jeweiligen Security Contract verschlüsselt und authentisiert werden können.

Dies betrifft insbesondere:

- Netzwerkkommunikation,
- `Nova.Call`,
- Remote Execution,
- Service-Kommunikation,
- Overlay- und VPN-Verbindungen,
- verteilte Storage-Zugriffe.

```text
Sender
   ↓
Authenticated Encryption
   ↓
Transport
   ↓
Receiver
```

Die Verschlüsselung muss in das NetworkFlow- und Distributed-Communication-Modell integrierbar sein.

---

## End-to-End und Link Protection

NovaOS unterscheidet zwischen dem Schutz einzelner Transportabschnitte und echtem End-to-End-Schutz.

```text
Endpoint A
   ↓
Encrypted End-to-End Channel
   ↓
Endpoint B
```

Ein verschlüsselter Teilpfad darf nicht automatisch als End-to-End-Verschlüsselung dargestellt werden.

---

## Key Management

Verschlüsselungsschlüssel werden über die allgemeine Secret- und Key-Management-Architektur verwaltet.

Schlüssel können insbesondere:

- softwaregeschützt,
- hardwaregeschützt,
- nicht exportierbar,
- an Systemzustände gebunden

sein.

Secret Use und Secret Export bleiben getrennte Authorities.

---

## Crypto Agility

At-Rest- und In-Transit-Verschlüsselung dürfen nicht dauerhaft an einzelne Algorithmen gebunden sein.

```text
Encryption Requirement
        ↓
Crypto Policy
        ↓
Eligible Algorithm
        ↓
Crypto Provider
```

Algorithmen und Parameter müssen kontrolliert migrierbar sein.

---

## Data Sovereignty und Information Flow

Verschlüsselung verändert nicht automatisch die Zulässigkeit eines Datenflusses.

```text
Encrypted
    ≠
Sovereignty Compliant
```

Ein verschlüsselter Datenstrom darf keinen verbotenen Provider, Standort, Empfänger oder Transitpfad verwenden, wenn die aktive Policy dies untersagt.

---

## Metadaten

NovaOS darf nicht davon ausgehen, dass Payload-Verschlüsselung sämtliche Metadaten schützt.

Abhängig vom jeweiligen System können weiterhin Informationen sichtbar sein, beispielsweise:

- Kommunikationspartner,
- Datenmengen,
- Zeitpunkte,
- Routinginformationen,
- Storage-Strukturen.

Policies müssen zwischen Payload-Schutz und Metadaten-Schutz unterscheiden können.

---

## Hardwarebeschleunigung

NovaOS darf verfügbare kryptographische Hardwarebeschleunigung verwenden.

```text
Crypto Operation
      ↓
Eligible Providers
      ↓
Software / CPU / Hardware Backend
```

Performanceoptimierungen dürfen jedoch niemals erforderliche Security Properties abschwächen.

---

## Fail-Closed

Ist Verschlüsselung laut Security Policy zwingend erforderlich und kann kein zulässiger kryptographischer Schutz hergestellt werden, muss die Operation fehlschlagen.

```text
Encryption Required
       ↓
Protection unavailable
       ↓
Fail
```

Ein stiller Wechsel zu unverschlüsselter Speicherung oder Übertragung ist unzulässig.

---

## Normative Anforderungen

1. NovaOS MUSS Verschlüsselung at Rest und in Transit systemweit unterstützen.
2. At-Rest- und In-Transit-Verschlüsselung MÜSSEN getrennt policygesteuert werden können.
3. Storage-Systeme MÜSSEN native At-Rest-Verschlüsselung unterstützen können.
4. Netzwerk- und Distributed-Communication-Systeme MÜSSEN In-Transit-Verschlüsselung unterstützen.
5. Erforderliche End-to-End-Verschlüsselung DARF NICHT durch reine Link-Verschlüsselung ersetzt werden.
6. Schlüssel MÜSSEN über die NovaOS Secret- und Key-Management-Architektur verwaltet werden können.
7. Secret Use und Secret Export MÜSSEN getrennte Authorities bleiben.
8. Verschlüsselungsverfahren MÜSSEN Crypto Agility unterstützen.
9. Verschlüsselung DARF Capability-, IFC-, Trust- oder Sovereignty-Regeln NICHT ersetzen oder umgehen.
10. Policies MÜSSEN zwischen Payload- und Metadaten-Schutz unterscheiden können.
11. Hardwarebeschleunigung DARF erforderliche Security Properties NICHT abschwächen.
12. Ist vorgeschriebene Verschlüsselung nicht verfügbar, MUSS die Operation fail-closed fehlschlagen.
13. Relevante Verschlüsselungszustände MÜSSEN kontrolliert auditierbar und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- einheitlicher Schutz gespeicherter und übertragener Daten,
- Integration mit Storage, NetworkFlow und Nova.Call,
- zentrale Crypto- und Key-Policy,
- Unterstützung hardwaregeschützter Schlüssel,
- langfristige Algorithmusmigration durch Crypto Agility.

### Negative Konsequenzen

- zusätzlicher Aufwand für Key Management und Migration,
- Verschlüsselung verursacht Rechen- und Ressourcenaufwand,
- Metadatenschutz benötigt teilweise zusätzliche Mechanismen.

---

## Verworfene Alternativen

### Nur Storage-Verschlüsselung

Verworfen.

Daten wären während der Übertragung unzureichend geschützt.

### Nur Transportverschlüsselung

Verworfen.

Persistente Daten wären nach ihrer Speicherung nicht automatisch geschützt.

### Verschlüsselung als Ersatz für Zugriffskontrolle

Verworfen.

Kryptographischer Schutz und Authority sind unterschiedliche Sicherheitsmechanismen.

### Automatischer unverschlüsselter Fallback

Verworfen.

Dies würde Hard Security Constraints umgehen.

---

## Abhängigkeiten

- `ADR-CRYPTO-0001_Crypto_Agility`
- `ADR-STORAGE-0016_Verschlüsselung_als_Storage_Policy`
- `ADR-SECURITY-0015_Information_Flow_Control`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-SECURITY-0018_Fail_Closed_für_Sicherheitsentscheidungen`
- `ADR-SECURITY-0019_Secret_Use_und_Secret_Export_getrennt`
- `ADR-SECURITY-0020_Hardware_Backend_für_schützenswerte_Schlüssel`

---

## Zugehörige NPSPECs

- `NPSPEC-CRYPTO-ENCRYPTION-AT-REST-0001`
- `NPSPEC-CRYPTO-ENCRYPTION-IN-TRANSIT-0001`
- `NPSPEC-CRYPTO-END-TO-END-ENCRYPTION-0001`
- `NPSPEC-CRYPTO-ENCRYPTION-POLICY-0001`
- `NPSPEC-CRYPTO-ENCRYPTION-KEY-MANAGEMENT-0001`
- `NPSPEC-CRYPTO-ENCRYPTION-TEST-0001`

---

## Ergebnis

NovaOS behandelt Verschlüsselung als durchgängige Eigenschaft des Datenlebenszyklus:

```text
            Data
             │
      ┌──────┴──────┐
      ↓             ↓
   At Rest       In Transit
      ↓             ↓
 Encryption     Encryption
      └──────┬──────┘
             ↓
      Crypto Policy
```

Daten können damit sowohl während ihrer Speicherung als auch während ihrer Übertragung geschützt werden, ohne Verschlüsselung mit Autorisierung, Information Flow Control oder Data Sovereignty gleichzusetzen.