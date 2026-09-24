# NPSPEC-STORAGE-ENCRYPTION-0001 – Nova Storage Encryption

## Status

Entwurf

## Kategorie

Storage / Encryption / Confidentiality / Data Protection

## Zweck

NovaOS definiert Storage Encryption zum Schutz persistenter Daten vor unautorisiertem Zugriff auf physische Speichermedien.

```text
Logical Data
     ↓
Encryption
     ↓
Encrypted Storage
     ↓
Decryption
     ↓
Authorized Access
```

Verschlüsselung muss transparent mit dem NovaOS-Storage-Modell zusammenarbeiten können.

## Grundprinzip

```text
Encryption ≠ Authorization
Encryption ≠ Integrity
Encryption ≠ Object Identity
```

Verschlüsselung schützt gespeicherte Daten, ersetzt jedoch weder Capability-Prüfungen noch Integritätskontrollen.

## Verschlüsselungsebenen

NovaOS muss unterschiedliche Ebenen unterstützen können:

```text
Volume
Filesystem
Object
Payload
```

Mehrere Ebenen dürfen kombiniert werden, sofern dies durch die jeweilige Security-Policy verlangt wird.

## Schlüsselmodell

Datenverschlüsselung und Schlüsselverwaltung müssen logisch getrennt sein.

```text
Data
 ↓
Data Encryption Key
 ↓
Encrypted Data

Data Encryption Key
 ↓
Key Protection
 ↓
Protected Key
```

Schlüssel dürfen nicht ungeschützt gemeinsam mit den verschlüsselten Daten gespeichert werden.

## Schlüsselquellen

Schlüssel dürfen unter anderem geschützt werden durch:

```text
User Credential
TPM
Hardware Security
Recovery Key
System Key
External Key Provider
```

Die konkrete Methode wird durch Security- und Recovery-Policy bestimmt.

## Objektverschlüsselung

Storage-Objekte dürfen individuell verschlüsselt werden.

```text
ObjectID
   ↓
Authorized Key Access
   ↓
Encrypted Payload
```

Eine Änderung der Verschlüsselungsrepräsentation darf die logische `ObjectID` nicht verändern.

## Content Addressing

Content Addressing und Verschlüsselung müssen logisch getrennt bleiben.

```text
Logical Content
      ↓
ContentID
      ↓
Encryption
      ↓
Encrypted Representation
```

Die gleiche logische Information darf unterschiedlich verschlüsselt gespeichert werden, ohne ihre semantische Identität zu verlieren.

## Metadaten

NovaOS muss definieren können, welche Metadaten:

```text
Encrypted
Authenticated
Visible
```

gespeichert werden.

Sicherheitskritische oder personenbezogene Metadaten sollen verschlüsselt werden können.

## Integrität

Storage Encryption soll authentifizierte Verschlüsselung oder einen vergleichbaren Integritätsschutz unterstützen.

```text
Ciphertext
   +
Authentication Data
   ↓
Verify
   ↓
Decrypt
```

Manipulierte verschlüsselte Daten dürfen nicht stillschweigend als gültig verarbeitet werden.

## Schlüsselrotation

NovaOS muss Schlüsselrotation unterstützen können.

```text
Old Key
   ↓
Rekey
   ↓
New Key
```

Eine Schlüsselrotation darf keine Änderung der logischen Objektidentität erzwingen.

Rekeying soll transaktional und crash-sicher erfolgen können.

## Snapshots und Versionen

Snapshots und historische Versionen müssen bei Schlüsselverwaltung berücksichtigt werden.

```text
Object
├── Current Version
├── Old Versions
└── Snapshots
```

Das Entfernen eines Schlüssels darf nicht unbeabsichtigt benötigte Recovery-Daten unzugänglich machen.

## Recovery

NovaOS soll kontrollierte Recovery-Schlüssel unterstützen können.

Recovery darf die normalen Security-Regeln nicht dauerhaft umgehen.

Verlorene Schlüssel ohne vorhandenen Recovery-Pfad dürfen als nicht wiederherstellbarer Datenverlust behandelt werden.

## Hardwarebeschleunigung

NovaOS darf verfügbare kryptographische Hardwarebeschleunigung verwenden.

Die Wahl des Implementierungsproviders darf die definierte Verschlüsselungssemantik nicht verändern.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Encryption State
Encryption Scope
Algorithm
Key Identifier
Key State
Rekey State
Recovery Capability
```

Geheime Schlüssel oder sensibles Schlüsselmaterial dürfen dabei niemals offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Verschlüsselung persistenter Storage-Daten unterstützen können.
2. Verschlüsselung und Zugriffskontrolle MÜSSEN logisch getrennte Mechanismen bleiben.
3. Schlüssel DÜRFEN NICHT ungeschützt gemeinsam mit verschlüsselten Daten gespeichert werden.
4. Verschlüsselungsalgorithmen MÜSSEN versionierbar und austauschbar sein.
5. NovaOS SOLL authentifizierte Verschlüsselung unterstützen.
6. Manipulierte verschlüsselte Daten DÜRFEN NICHT stillschweigend akzeptiert werden.
7. Verschlüsselung DARF die logische `ObjectID` NICHT verändern.
8. Schlüsselrotation MUSS ohne Änderung der logischen Objektidentität möglich sein.
9. Rekeying SOLL transaktional und crash-sicher erfolgen.
10. Snapshots und Versionen MÜSSEN bei Schlüsselrotation und Schlüsselentfernung berücksichtigt werden.
11. Recovery-Schlüssel MÜSSEN durch definierte Security-Policy geschützt werden.
12. Introspection DARF niemals geheimes Schlüsselmaterial offenlegen.

## Abhängigkeiten

- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-CONTENTADDRESS-0001`
- `NPSPEC-STORAGE-TRANSACTION-0001`
- `NPSPEC-STORAGE-VERSIONING-0001`
- `NPSPEC-STORAGE-SNAPSHOT-0001`
- `NPSPEC-STORAGE-CHECKSUM-0001`
- `ADR-STORAGE-0012`

## Ergebnis

```text
Logical Storage Object
         ↓
Security Policy
         ↓
Encryption + Key Management
         ↓
Encrypted Physical Storage
```

NovaOS erhält damit eine Storage-Verschlüsselung, die Daten, Schlüsselverwaltung und Objektidentität klar trennt und mit Versionierung, Snapshots, Recovery und dem übrigen Storage-Modell zusammenarbeitet.