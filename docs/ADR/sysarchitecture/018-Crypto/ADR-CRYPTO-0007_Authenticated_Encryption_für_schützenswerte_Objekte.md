# ADR-CRYPTO-0007 – Authenticated Encryption für schützenswerte Objekte

## Status

Angenommen

## Kategorie

Cryptography / Data Integrity / Storage / Object Security

## Kontext

Verschlüsselung schützt die Vertraulichkeit von Daten, verhindert allein jedoch nicht zuverlässig deren unerkannte Manipulation.

Für schützenswerte NovaOS-Objekte müssen deshalb sowohl:

- Vertraulichkeit,
- Integrität,
- Authentizität

kryptographisch abgesichert werden.

Dies betrifft insbesondere NovaFiles, Storage Objects, Secrets, Konfigurationsobjekte, Backups und andere sensible persistente Daten.

---

## Entscheidung

NovaOS verwendet für schützenswerte Objekte standardmäßig **Authenticated Encryption**, sofern Vertraulichkeit und Integrität gemeinsam erforderlich sind.

```text
Plaintext
   +
Authenticated Metadata
   ↓
Authenticated Encryption
   ↓
Ciphertext + Authentication Data
```

Bevorzugt werden AEAD-Verfahren oder kryptographisch gleichwertige Konstruktionen gemäß der aktiven Crypto Policy.

---

## Grundprinzip

```text
Confidentiality without integrity
is insufficient for protected objects.
```

---

## AEAD als bevorzugtes Modell

NovaOS behandelt Authenticated Encryption with Associated Data (AEAD) als bevorzugte semantische Abstraktion.

```text
Encrypt(
    Plaintext,
    AssociatedData,
    Key,
    Nonce
)
```

Das konkrete kryptographische Verfahren wird nicht durch diese ADR festgelegt und unterliegt Crypto Agility.

---

## Authenticated Associated Data

Nicht verschlüsselte, aber sicherheitsrelevante Metadaten können kryptographisch an ein Objekt gebunden werden.

Beispiele:

```text
Object Identity
Object Type
Format Version
Crypto Policy
Key Generation
Security Context
```

Manipulation solcher gebundenen Metadaten muss bei der Authentizitätsprüfung erkannt werden.

Welche Metadaten als Associated Data verwendet werden, wird durch das jeweilige Objektformat definiert.

---

## Objektbindung

Ciphertext soll soweit erforderlich an seinen vorgesehenen semantischen Kontext gebunden werden.

Dadurch soll beispielsweise verhindert werden, dass ein gültiger verschlüsselter Payload unbemerkt einem anderen Objekt oder Kontext zugeordnet wird.

```text
Object Identity
      +
Object Context
      +
Payload
      ↓
Authenticated Encryption
```

Die konkrete Binding-Semantik wird in NPSPECs definiert.

---

## Nonce- und IV-Management

Authenticated-Encryption-Verfahren können Anforderungen an Nonces oder Initialisierungsvektoren besitzen.

NovaOS muss diese Anforderungen als Bestandteil des jeweiligen Crypto Contracts behandeln.

Fehlerhafte Nonce-Verwendung darf nicht durch Anwendungen oder Storage-Komponenten stillschweigend erzeugt werden können.

Die konkrete Nonce-Generierung, Persistenz und Wiederverwendungsprävention wird in NPSPECs definiert.

---

## Per-Object Encryption

Authenticated Encryption wird mit Per-Object Encryption kombiniert.

```text
Object
 ├── Payload
 ├── Authenticated Metadata
 └── Object Key Context
           ↓
          AEAD
```

Unterschiedliche Objekte können dadurch getrennte kryptographische Schutzkontexte besitzen.

---

## Versionen, Snapshots und Replikate

Jede gültige verschlüsselte Objektversion muss eigenständig authentifizierbar sein.

Snapshots und Replikation dürfen Authentication Data nicht entfernen oder ungültig verändern.

Authenticated Encryption allein verhindert jedoch nicht automatisch das Einspielen einer älteren, kryptographisch gültigen Objektversion.

Rollback Protection ist deshalb ein getrenntes Sicherheitsproblem.

---

## Fehlerverhalten

Schlägt die Authentizitätsprüfung fehl, darf NovaOS den entschlüsselten Inhalt nicht als gültiges Objekt bereitstellen.

```text
Ciphertext
    ↓
Authentication
    ├── Valid   → Decrypt / Use
    └── Invalid → Reject
```

Authentication Failure wird fail-closed behandelt.

---

## Crypto Agility

Die Architektur darf nicht dauerhaft an ein einzelnes AEAD-Verfahren gebunden sein.

```text
Authenticated Encryption Requirement
              ↓
         Crypto Policy
              ↓
       Eligible Algorithm
              ↓
        Crypto Provider
```

Algorithmus- und Key-Migration müssen möglich bleiben.

---

## Normative Anforderungen

1. NovaOS MUSS Authenticated Encryption für schützenswerte Objekte unterstützen.
2. Wenn Vertraulichkeit und kryptographische Integrität gemeinsam erforderlich sind, SOLL Authenticated Encryption verwendet werden.
3. AEAD MUSS als bevorzugte semantische Abstraktion unterstützt werden.
4. Das konkrete Verfahren MUSS der Crypto-Agility-Architektur unterliegen.
5. Sicherheitsrelevante unverschlüsselte Metadaten MÜSSEN als Associated Data authentifizierbar sein.
6. Objekte MÜSSEN kryptographisch an relevante semantische Kontexte bindbar sein.
7. Nonce- und IV-Anforderungen MÜSSEN durch die Crypto-Architektur korrekt durchsetzbar sein.
8. Per-Object Encryption MUSS mit Authenticated Encryption kombinierbar sein.
9. Snapshots, Backups und Replikate DÜRFEN erforderliche Authentication Data NICHT entfernen.
10. Eine fehlgeschlagene Authentizitätsprüfung MUSS fail-closed behandelt werden.
11. Nicht authentifizierter Klartext DARF nach einem Authentication Failure NICHT als gültiges Objekt bereitgestellt werden.
12. Authenticated Encryption DARF NICHT mit Rollback Protection gleichgesetzt werden.
13. Key Rotation und Algorithmusmigration MÜSSEN unterstützt werden können.
14. Crypto-Zustand und Authentication Failure MÜSSEN kontrolliert auditierbar sein, ohne Schlüssel oder schützenswerte Klartexte offenzulegen.

---

## Konsequenzen

### Positive Konsequenzen

- Vertraulichkeit und Integrität werden gemeinsam geschützt,
- Manipulation verschlüsselter Objekte wird erkennbar,
- Metadaten können kryptographisch an Payloads gebunden werden,
- gute Integration mit NovaFile und Per-Object Encryption,
- weniger Risiko fehlerhafter Kombination separater Crypto-Primitive.

### Negative Konsequenzen

- Nonce- und IV-Management wird sicherheitskritisch,
- zusätzliche Authentication Data benötigt Speicher,
- Rollback-Schutz benötigt weiterhin separate Mechanismen.

---

## Verworfene Alternativen

### Verschlüsselung ohne Authentifizierung

Verworfen.

Manipulationen könnten abhängig vom verwendeten Verfahren unerkannt bleiben.

### Separate selbst entwickelte Encrypt-then-MAC-Konstruktionen pro Subsystem

Verworfen.

NovaOS soll eine einheitliche, überprüfbare Crypto-Abstraktion verwenden.

### Authenticated Encryption als Rollback Protection

Verworfen.

Eine ältere kryptographisch gültige Version kann weiterhin authentisch sein.

---

## Abhängigkeiten

- `ADR-CRYPTO-0001_Crypto_Agility`
- `ADR-CRYPTO-0002_Verschlüsselung_at_Rest_und_in_Transit`
- `ADR-CRYPTO-0003_Per_Object_Encryption`
- `ADR-CRYPTO-0004_Hierarchische_Key_Architecture`
- `ADR-CRYPTO-0005_Key_Wrapping_und_Key_Rotation`
- `ADR-SECURITY-0018_Fail_Closed_für_Sicherheitsentscheidungen`
- `ADR-STORAGE-0015_Checksumming_für_Integrität`

---

## Zugehörige NPSPECs

- `NPSPEC-CRYPTO-AUTHENTICATED-ENCRYPTION-0001`
- `NPSPEC-CRYPTO-AEAD-0001`
- `NPSPEC-CRYPTO-AEAD-ASSOCIATED-DATA-0001`
- `NPSPEC-CRYPTO-AEAD-NONCE-0001`
- `NPSPEC-CRYPTO-OBJECT-BINDING-0001`
- `NPSPEC-CRYPTO-AUTHENTICATED-ENCRYPTION-TEST-0001`

---

## Ergebnis

NovaOS schützt schützenswerte verschlüsselte Objekte nicht nur gegen Offenlegung, sondern auch gegen unerkannte Manipulation:

```text
Object Payload
      +
Authenticated Metadata
      +
Key Context
      ↓
Authenticated Encryption
      ↓
Confidential + Authenticated Object
```

Authenticated Encryption wird damit zum bevorzugten kryptographischen Schutzmodell für Objekte, bei denen Vertraulichkeit und Integrität gemeinsam erforderlich sind.