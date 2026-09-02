# ADR-CRYPTO-0008 – Post-Quantum Agility und Hybridverfahren

## Status

Angenommen

## Kategorie

Cryptography / Post-Quantum Security / Crypto Agility

## Kontext

Quantencomputer können langfristig heute verbreitete Public-Key-Verfahren gefährden.

Gleichzeitig ist nicht vorhersehbar, welche Post-Quantum-Verfahren langfristig sicher, effizient und für alle NovaOS-Plattformen geeignet sein werden.

NovaOS darf deshalb weder dauerhaft von klassischer Public-Key-Kryptographie noch von einem einzelnen Post-Quantum-Verfahren abhängig sein.

Während der Migration müssen klassische und Post-Quantum-Verfahren kontrolliert kombiniert werden können.

---

## Entscheidung

NovaOS erweitert seine Crypto-Agility-Architektur um **Post-Quantum Agility und Hybridverfahren**.

```text
Crypto Requirement
        ↓
Crypto Policy
        ↓
 ┌──────┼──────────┐
 ↓      ↓          ↓
Classic PQC      Hybrid
        ↓
Crypto Provider
```

Post-Quantum-Kryptographie wird als austauschbare Algorithmusklasse behandelt und nicht als separate Kryptographiearchitektur.

---

## Grundprinzip

```text
Prepare for quantum-resistant cryptography
without betting the system on one algorithm.
```

---

## Post-Quantum Agility

NovaOS muss neue Post-Quantum-Verfahren über dieselben abstrahierten Crypto-Mechanismen integrieren können wie klassische Verfahren.

Dies betrifft insbesondere:

- Signaturen,
- Key Establishment,
- Authentifizierung,
- Zertifikate und Trust,
- Code Signing,
- Secure Boot,
- Netzwerkkommunikation,
- langfristig geschützte Daten.

Algorithmen bleiben durch Crypto Policy auswählbar, einschränkbar und austauschbar.

---

## Hybridverfahren

NovaOS unterstützt Hybridverfahren, bei denen mehrere kryptographische Verfahren gemeinsam eine Sicherheitsoperation bilden.

Beispiel:

```text
Classical Algorithm
        +
Post-Quantum Algorithm
        ↓
Hybrid Construction
        ↓
Combined Security Result
```

Hybridverfahren ermöglichen Übergangsphasen, in denen weder ausschließlich auf klassische noch ausschließlich auf neue Post-Quantum-Verfahren vertraut werden soll.

Die konkrete Kombination wird in NPSPECs definiert.

---

## Hybrid Security Policy

Eine Crypto Policy kann beispielsweise verlangen:

```text
Classical Required
Post-Quantum Required
Hybrid Required
Post-Quantum Preferred
```

Ein Hybridverfahren muss als zusammengehöriger Crypto Contract behandelt werden.

NovaOS darf nicht stillschweigend nur einen Teil eines vorgeschriebenen Hybridverfahrens verwenden.

---

## Algorithmus- und Formatidentität

Kryptographische Formate müssen mehrere Algorithmen und deren Parameter eindeutig beschreiben können.

```text
Crypto Object
 ├── Format Version
 ├── Algorithm Set
 │    ├── Classical
 │    └── Post-Quantum
 ├── Parameters
 └── Cryptographic Data
```

Die Architektur darf keine festen Größenannahmen für Schlüssel, Signaturen oder Ciphertexts voraussetzen.

---

## Migration

NovaOS unterstützt kontrollierte Übergänge:

```text
Classical
    ↓
Hybrid
    ↓
Post-Quantum
```

Diese Reihenfolge ist keine zwingende globale Migration.

Die aktive Crypto Policy bestimmt, welche Verfahren für einen konkreten Anwendungsfall erforderlich oder zulässig sind.

Bestehende Daten, Schlüssel und Signaturen müssen gemäß ihrer jeweiligen Lifecycle- und Migrationspolicy behandelt werden.

---

## Harvest-Now-Decrypt-Later

Für langfristig vertrauliche Daten muss die Crypto Policy berücksichtigen können, dass heute aufgezeichnete verschlüsselte Kommunikation möglicherweise zukünftig entschlüsselt werden könnte.

Daten mit entsprechend langer Schutzdauer können deshalb frühzeitig Post-Quantum- oder Hybrid-Schutz verlangen.

```text
Data Lifetime
      +
Threat Horizon
      ↓
Crypto Requirement
```

---

## Hardware und Performance

Post-Quantum-Verfahren können andere Anforderungen an:

- Schlüsselgrößen,
- Signaturgrößen,
- Speicher,
- Bandbreite,
- CPU,
- Hardwarebeschleunigung

besitzen.

Diese Eigenschaften werden über Resource Economy und Crypto Provider berücksichtigt.

Performance darf jedoch keine vorgeschriebenen kryptographischen Hard Constraints abschwächen.

---

## Trust und Boot

Post-Quantum- und Hybridverfahren müssen langfristig auch in die Nova.Trust- und Boot-Architektur integrierbar sein.

Dies betrifft insbesondere:

```text
Code Signing
Secure Boot
Measured Boot
Update Verification
Device Identity
Remote Attestation
```

Die Trust-Architektur darf daher keine dauerhafte Abhängigkeit von einem einzelnen klassischen Signaturverfahren besitzen.

---

## Fail-Closed

Verlangt eine Policy Post-Quantum- oder Hybrid-Schutz und steht kein zulässiger Provider zur Verfügung, muss die Operation fehlschlagen.

```text
Hybrid Required
      ↓
Only Classical Available
      ↓
Fail
```

Ein stiller kryptographischer Downgrade ist unzulässig.

---

## Normative Anforderungen

1. NovaOS MUSS Post-Quantum-Kryptographie innerhalb der allgemeinen Crypto-Agility-Architektur unterstützen können.
2. NovaOS DARF NICHT dauerhaft an ein einzelnes Post-Quantum-Verfahren gebunden sein.
3. Klassische, Post-Quantum- und hybride Verfahren MÜSSEN durch Crypto Policies unterscheidbar sein.
4. NovaOS MUSS Hybridverfahren unterstützen können.
5. Vorgeschriebene Hybridverfahren DÜRFEN NICHT stillschweigend auf einen einzelnen Bestandteil reduziert werden.
6. Kryptographische Formate MÜSSEN mehrere Algorithmen und Parameter beschreiben können.
7. Crypto APIs DÜRFEN KEINE festen Annahmen über Schlüssel-, Signatur- oder Ciphertext-Größen voraussetzen.
8. Migration zwischen klassischen, hybriden und Post-Quantum-Verfahren MUSS unterstützt werden.
9. Langfristige Schutzdauer MUSS bei der Auswahl kryptographischer Verfahren berücksichtigt werden können.
10. Post-Quantum-Verfahren MÜSSEN in Code Signing, Trust, Boot und Kommunikation integrierbar sein.
11. Ressourcen- und Performancekosten MÜSSEN berücksichtigt werden, DÜRFEN aber Hard Security Constraints NICHT abschwächen.
12. Ist ein vorgeschriebener Post-Quantum- oder Hybrid-Schutz nicht verfügbar, MUSS die Operation fail-closed fehlschlagen.
13. Algorithmus- und Migrationszustände MÜSSEN kontrolliert auditierbar und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Vorbereitung auf zukünftige Quantenbedrohungen,
- keine Abhängigkeit von einem einzelnen PQC-Verfahren,
- kontrollierte Migration über Hybridverfahren,
- langfristig evolvierbare Trust- und Boot-Architektur,
- Schutz besonders langlebiger vertraulicher Daten.

### Negative Konsequenzen

- größere Schlüssel, Signaturen und Nachrichten können Ressourcenverbrauch erhöhen,
- Hybridverfahren erhöhen Protokoll- und Formatkomplexität,
- mehrere Algorithmusgenerationen müssen während Migrationen parallel unterstützt werden.

---

## Verworfene Alternativen

### Sofortige vollständige Ablösung klassischer Kryptographie

Verworfen.

Eine kontrollierte Migration und Interoperabilität müssen möglich bleiben.

### Ein fest ausgewählter Post-Quantum-Algorithmus für NovaOS

Verworfen.

Auch Post-Quantum-Verfahren können zukünftig ersetzt oder eingeschränkt werden müssen.

### Post-Quantum-Kryptographie erst bei konkreter Quantenbedrohung

Verworfen.

Langfristig vertrauliche Daten können bereits vorher durch Harvest-Now-Decrypt-Later gefährdet sein.

### Stiller Fallback von Hybrid auf Classical

Verworfen.

Dies würde vorgeschriebene Security Properties verletzen.

---

## Abhängigkeiten

- `ADR-ARCH-0019_Evolvable_System_Architecture`
- `ADR-CRYPTO-0001_Crypto_Agility`
- `ADR-CRYPTO-0002_Verschlüsselung_at_Rest_und_in_Transit`
- `ADR-CRYPTO-0004_Hierarchische_Key_Architecture`
- `ADR-CRYPTO-0005_Key_Wrapping_und_Key_Rotation`
- `ADR-SECURITY-0013_Code_Integrity_und_Code_Signing`
- `ADR-SECURITY-0014_Secure_und_Measured_Boot`
- `ADR-SECURITY-0018_Fail_Closed_für_Sicherheitsentscheidungen`

---

## Zugehörige NPSPECs

- `NPSPEC-CRYPTO-PQC-0001`
- `NPSPEC-CRYPTO-PQC-POLICY-0001`
- `NPSPEC-CRYPTO-HYBRID-0001`
- `NPSPEC-CRYPTO-HYBRID-KEY-ESTABLISHMENT-0001`
- `NPSPEC-CRYPTO-HYBRID-SIGNATURE-0001`
- `NPSPEC-CRYPTO-PQC-MIGRATION-0001`
- `NPSPEC-CRYPTO-PQC-TEST-0001`

---

## Ergebnis

NovaOS behandelt den Übergang zu quantenresistenter Kryptographie als evolutionären Prozess:

```text
Classical
    ↓
Hybrid
    ↓
Post-Quantum
    ↓
Future Algorithms
```

Die Architektur bleibt dabei algorithmusunabhängig.

Dadurch kann NovaOS neue Post-Quantum-Verfahren einführen, klassische und neue Verfahren während Übergangsphasen kombinieren und auch zukünftige kryptographische Generationen integrieren, ohne die grundlegende Security-Architektur erneut entwerfen zu müssen.