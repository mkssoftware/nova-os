# ADR-CRYPTO-0003 – Per-Object Encryption

## Status

Angenommen

## Kategorie

Cryptography / Storage / Data Protection / Key Management

## Kontext

Eine ausschließlich volumen- oder datenträgerweite Verschlüsselung schützt Daten primär gegenüber dem physischen Speichermedium.

Nach dem Entsperren eines Volumes befinden sich jedoch Objekte mit unterschiedlichen Schutzanforderungen innerhalb derselben Storage Domain.

NovaOS besitzt mit NovaFile, Object Storage, Semantic Types, Information Flow Control und Data Sovereignty bereits ein objektorientiertes Datenmodell.

Die Verschlüsselungsarchitektur soll deshalb Schutzrichtlinien auch auf Ebene einzelner logischer Objekte ermöglichen.

---

## Entscheidung

NovaOS unterstützt **Per-Object Encryption als native Storage- und Crypto-Eigenschaft**.

```text
Storage
 ├── Object A → Encryption Policy A
 ├── Object B → Encryption Policy B
 └── Object C → Encryption Policy C
```

Ein logisches Objekt kann unabhängig von der Verschlüsselung des darunterliegenden Volumes kryptographisch geschützt werden.

Per-Object Encryption ergänzt Volume Encryption und ersetzt sie nicht grundsätzlich.

---

## Grundprinzip

```text
Encryption follows the protected object,
not only the storage device.
```

---

## Objektbezogene Crypto Policy

Ein Objekt kann eine eigene Crypto Policy besitzen.

Diese kann unter anderem festlegen:

- ob Verschlüsselung erforderlich ist,
- welche Security Strength erforderlich ist,
- welche Key Policy gilt,
- welche Crypto Provider zulässig sind,
- ob Hardware Protection erforderlich ist,
- ob Schlüssel exportierbar sein dürfen.

Der konkrete Algorithmus bleibt gemäß Crypto Agility austauschbar.

---

## Schlüsseltrennung

NovaOS muss unterschiedliche Objekte kryptographisch voneinander isolieren können.

```text
Object A → Key Context A
Object B → Key Context B
Object C → Key Context C
```

Dies erfordert nicht zwingend einen vollständig unabhängigen gespeicherten Schlüssel pro Objekt.

Hierarchische Schlüsselableitung und andere Key-Management-Verfahren dürfen verwendet werden, sofern die resultierende Isolation erhalten bleibt.

Die konkrete Schlüsselhierarchie wird in NPSPECs definiert.

---

## NovaFile und Object Storage

Per-Object Encryption wird direkt in das native Storage-Modell integriert.

```text
NovaFile / Storage Object
 ├── Payload
 ├── Metadata
 ├── Semantic Properties
 └── Crypto Policy
```

Welche Metadaten selbst verschlüsselt werden müssen, wird durch die jeweilige Policy bestimmt.

---

## Information Flow und Data Sovereignty

Die Crypto Policy eines Objekts kann aus dessen semantischen Sicherheitsanforderungen abgeleitet werden.

```text
Semantic Type
      +
Information Flow Policy
      +
Sovereignty Policy
      ↓
Object Crypto Requirements
```

Verschlüsselung hebt bestehende Information-Flow- oder Sovereignty-Regeln nicht auf.

---

## Kopieren, Verschieben und Replikation

Der Schutz eines Objekts darf durch Datenbewegung nicht unbeabsichtigt verloren gehen.

```text
Encrypted Object
      ↓
Copy / Move / Replicate
      ↓
Policy Validation
      ↓
Protected Destination
```

Falls eine Zielumgebung die erforderliche Crypto Policy nicht erfüllen kann, muss die Operation abgelehnt oder gemäß expliziter Policy neu geplant werden.

---

## Sharing

Mehrere autorisierte Subjects können Zugriff auf dasselbe verschlüsselte Objekt erhalten, ohne dass dafür zwangsläufig das Objekt selbst mehrfach gespeichert werden muss.

Die Berechtigung zum Entschlüsseln bleibt von der Berechtigung zum Exportieren des verwendeten Schlüssels getrennt.

---

## Snapshots und Backups

Snapshots, Versionen und Backups müssen den kryptographischen Schutz eines Objekts erhalten.

Eine Storage-Kopie darf nicht automatisch zu einer unverschlüsselten Kopie führen.

Key Rotation, Recovery und langfristige Archivierung werden über die Key-Management- und Crypto-Migrationsmechanismen behandelt.

---

## Crypto Agility und Re-Encryption

Per-Object Encryption muss Algorithmus- und Schlüsselwechsel ermöglichen.

```text
Object
 ↓
Old Crypto Policy
 ↓
Re-Encryption
 ↓
New Crypto Policy
```

Re-Encryption darf die logische Identität des Objekts nicht unnötig verändern.

---

## Fail-Closed

Verlangt die Object Policy Verschlüsselung und kann diese nicht bereitgestellt werden, darf das Objekt nicht ungeschützt gespeichert oder übertragen werden.

```text
Encryption Required
        ↓
Requirement unavailable
        ↓
Fail / Replan
```

Ein stiller unverschlüsselter Fallback ist unzulässig.

---

## Normative Anforderungen

1. NovaOS MUSS Per-Object Encryption nativ unterstützen.
2. Per-Object Encryption MUSS unabhängig von Volume Encryption verwendbar sein.
3. Objekte MÜSSEN eigene Crypto Policies besitzen können.
4. Unterschiedliche Objekte MÜSSEN kryptographisch voneinander isolierbar sein.
5. Die konkrete Schlüsselhierarchie DARF von der logischen Objektidentität abstrahiert werden.
6. NovaFile und native Storage Objects MÜSSEN Per-Object Encryption unterstützen können.
7. Payload und Metadaten MÜSSEN getrennte Schutzanforderungen besitzen können.
8. Copy, Move, Replication, Snapshot und Backup DÜRFEN erforderlichen Objektschutz NICHT stillschweigend entfernen.
9. Sharing DARF keine unnötige Offenlegung von Schlüsselmaterial erfordern.
10. Secret Use und Secret Export MÜSSEN getrennte Authorities bleiben.
11. Per-Object Encryption MUSS Crypto Agility und Key Rotation unterstützen.
12. Verschlüsselung DARF Information Flow Control oder Data Sovereignty NICHT ersetzen.
13. Kann eine erforderliche Object Crypto Policy nicht erfüllt werden, MUSS die Operation fail-closed behandelt werden.
14. Crypto-Zustand und Policy eines Objekts MÜSSEN kontrolliert introspektierbar sein, ohne Schlüsselmaterial offenzulegen.

---

## Konsequenzen

### Positive Konsequenzen

- feinere Isolation als reine Volume Encryption,
- unterschiedliche Schutzstufen innerhalb desselben Volumes,
- kryptographischer Schutz kann logischen Objekten folgen,
- bessere Unterstützung für Sharing, Backup und verteilten Storage,
- direkte Integration mit NovaFile, IFC und Data Sovereignty.

### Negative Konsequenzen

- komplexeres Key Management,
- zusätzliche Metadaten für Crypto Policies,
- Rotation, Sharing und Recovery benötigen definierte Schlüsselhierarchien.

---

## Verworfene Alternativen

### Nur Full-Volume Encryption

Verworfen.

Sie bietet keine ausreichende kryptographische Trennung einzelner Objekte innerhalb eines entsperrten Volumes.

### Ein gemeinsamer Schlüssel für alle Objekte

Verworfen.

Eine Kompromittierung hätte einen unnötig großen Schutzbereich.

### Eigener vollständig gespeicherter Schlüssel für jedes Objekt als zwingende Architektur

Verworfen.

Dies würde die Implementierung unnötig festlegen und skalierbare Schlüsselableitungsmodelle verhindern.

---

## Abhängigkeiten

- `ADR-CRYPTO-0001_Crypto_Agility`
- `ADR-CRYPTO-0002_Verschlüsselung_at_Rest_und_in_Transit`
- `ADR-STORAGE-0002_Natives_Object_Storage_Modell`
- `ADR-STORAGE-0009_NovaFile_als_semantischer_Dateicontainer`
- `ADR-STORAGE-0016_Verschlüsselung_als_Storage_Policy`
- `ADR-SECURITY-0015_Information_Flow_Control`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-SECURITY-0019_Secret_Use_und_Secret_Export_getrennt`
- `ADR-SECURITY-0020_Hardware_Backend_für_schützenswerte_Schlüssel`

---

## Zugehörige NPSPECs

- `NPSPEC-CRYPTO-PER-OBJECT-ENCRYPTION-0001`
- `NPSPEC-CRYPTO-OBJECT-KEY-HIERARCHY-0001`
- `NPSPEC-CRYPTO-OBJECT-CRYPTO-POLICY-0001`
- `NPSPEC-CRYPTO-OBJECT-KEY-ROTATION-0001`
- `NPSPEC-CRYPTO-OBJECT-SHARING-0001`
- `NPSPEC-CRYPTO-PER-OBJECT-ENCRYPTION-TEST-0001`

---

## Ergebnis

NovaOS bindet kryptographischen Schutz an das logische Datenobjekt:

```text
Object
 ├── Identity
 ├── Data
 ├── Security Policy
 └── Crypto Policy
          ↓
      Key Context
          ↓
      Encryption
```

Damit kann ein Objekt seinen erforderlichen kryptographischen Schutz über Storage-, Snapshot-, Backup- und Replikationsgrenzen hinweg beibehalten, ohne ausschließlich vom Schutz des darunterliegenden Volumes abhängig zu sein.