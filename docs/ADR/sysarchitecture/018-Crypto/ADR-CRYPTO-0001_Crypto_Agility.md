# ADR-CRYPTO-0001 – Crypto Agility

## Status

Angenommen

## Kategorie

Cryptography / Security / Trust / Evolvability

## Kontext

Kryptographische Algorithmen und Parameter besitzen keine unbegrenzte Lebensdauer.

Neue kryptographische Angriffe, steigende Rechenleistung, regulatorische Anforderungen, Hardwareänderungen und zukünftige Post-Quantum-Verfahren können dazu führen, dass heute geeignete Verfahren später ersetzt werden müssen.

Ein langfristig ausgelegtes Betriebssystem wie NovaOS darf deshalb seine Sicherheitsarchitektur nicht fest an einzelne Algorithmen wie RSA, AES, SHA oder bestimmte Kurven koppeln.

---

## Entscheidung

NovaOS verwendet **Crypto Agility als grundlegendes Architekturprinzip**.

Kryptographische Anforderungen werden von konkreten Algorithmen getrennt.

```text
Security Requirement
        ↓
Cryptographic Policy
        ↓
Eligible Algorithms
        ↓
Algorithm / Provider Selection
        ↓
Cryptographic Operation
```

Protokolle, Dateiformate und Systemkomponenten dürfen kryptographische Verfahren nicht unnötig fest verdrahten.

---

## Grundprinzip

```text
Cryptographic intent
        ≠
Cryptographic algorithm
```

Algorithmen sind austauschbare Mechanismen.

Security Policy und Contracts bestimmen, welche Mechanismen zulässig sind.

---

## Semantische Crypto-Operationen

Höhere Systemschichten sollen kryptographische Absichten ausdrücken.

Beispiele:

```text
Sign
Verify
Encrypt
Decrypt
Hash
Authenticate
DeriveKey
GenerateKey
```

Die Auswahl des konkreten Algorithmus erfolgt anhand des jeweiligen Crypto Contracts und der Security Policy.

---

## Crypto Contract

Eine kryptographische Operation kann Anforderungen definieren wie:

- Security Level,
- Algorithm Class,
- Key Requirements,
- Hardware Protection,
- Determinism Requirements,
- Compliance Requirements,
- Compatibility Requirements,
- Allowed oder Forbidden Algorithms.

Damit bleibt die gewünschte Sicherheitseigenschaft unabhängig von einem einzelnen Verfahren beschreibbar.

---

## Algorithmusauswahl

NovaOS darf standardmäßig einen geeigneten Algorithmus automatisch auswählen.

```text
Crypto Operation
      ↓
Hard Security Constraints
      ↓
Allowed Algorithms
      ↓
Policy Selection
```

Eine explizite Algorithmuswahl durch Anwendungen ist möglich, sofern sie alle Hard Security Constraints erfüllt.

Ein verbotener oder sicherheitstechnisch nicht mehr zulässiger Algorithmus darf nicht durch einen einfachen Override erzwungen werden.

---

## Provider-Modell

Kryptographische Implementierungen werden über Provider abstrahiert.

```text
Crypto API
   ↓
Crypto Provider
   ├── Software
   ├── CPU Acceleration
   ├── TPM
   ├── Secure Element
   ├── HSM
   └── Future Provider
```

Provider können unterschiedliche Algorithmen, Hardwarebeschleunigung und Sicherheitsmerkmale bereitstellen.

---

## Algorithmus-Lifecycle

NovaOS muss kryptographische Verfahren kontrolliert klassifizieren können.

Konzeptionelle Zustände können beispielsweise sein:

```text
Preferred
Allowed
Deprecated
Restricted
Forbidden
```

Die konkreten Zustände und Übergangsregeln werden in NPSPECs definiert.

Neue Schlüssel oder Signaturen sollen keine veralteten Verfahren verwenden, wenn die aktive Policy dies nicht mehr erlaubt.

---

## Versionierte Formate und Protokolle

Kryptographisch geschützte Datenstrukturen müssen den verwendeten Algorithmus und erforderliche Parameter eindeutig identifizieren können.

```text
Protected Object
 ├── Format Version
 ├── Algorithm Identifier
 ├── Parameters
 └── Cryptographic Data
```

Dadurch können alte und neue Verfahren während kontrollierter Migrationsphasen parallel unterstützt werden.

---

## Migration

Crypto Agility muss den Übergang zwischen Verfahren ermöglichen.

Beispiele:

```text
Old Algorithm
      ↓
Migration Period
      ↓
Old + New
      ↓
New Algorithm
```

Migration darf bestehende Security Policies nicht abschwächen.

Für langfristig gespeicherte Daten müssen Re-Encryption, Re-Signing oder andere geeignete Migrationsmechanismen möglich sein.

---

## Post-Quantum-Fähigkeit

Die Architektur darf keine Annahmen treffen, die ausschließlich für klassische Kryptographie gelten.

Insbesondere müssen Algorithmus- und Key-Repräsentationen ausreichend flexibel sein, um zukünftige Post-Quantum- und hybride Verfahren integrieren zu können.

Post-Quantum-Kryptographie wird damit ermöglicht, ohne sie als einzige zukünftige Kryptographieform festzuschreiben.

---

## Fail-Closed

Ist kein Algorithmus verfügbar, der die erforderlichen Hard Security Constraints erfüllt, muss die Operation fehlschlagen.

```text
Required Security
       ↓
No Eligible Algorithm
       ↓
Fail
```

NovaOS darf nicht automatisch auf einen schwächeren Algorithmus zurückfallen.

---

## Normative Anforderungen

1. NovaOS MUSS Crypto Agility als systemweites Architekturprinzip verwenden.
2. Kryptographische Anforderungen MÜSSEN von konkreten Algorithmen trennbar sein.
3. Kryptographische Algorithmen MÜSSEN über austauschbare Provider implementierbar sein.
4. Security Policies MÜSSEN Algorithmen erlauben, einschränken, veralten lassen oder verbieten können.
5. Hard Security Constraints MÜSSEN vor Performance- oder Hardwareoptimierungen ausgewertet werden.
6. Anwendungen DÜRFEN konkrete Algorithmen anfordern, sofern die aktive Security Policy diese erlaubt.
7. Ein Algorithmus-Override DARF Security Policies NICHT umgehen.
8. Kryptographische Formate MÜSSEN verwendete Algorithmen und relevante Parameter eindeutig identifizieren können.
9. NovaOS MUSS kontrollierte Migration zwischen kryptographischen Verfahren ermöglichen.
10. Die Architektur MUSS zukünftige Post-Quantum- und hybride Verfahren integrieren können.
11. Hardware- und Software-Provider MÜSSEN unter derselben übergeordneten Crypto-Architektur nutzbar sein.
12. Ist kein zulässiges Verfahren verfügbar, MUSS die Operation fail-closed fehlschlagen.
13. Kryptographische Auswahlentscheidungen MÜSSEN kontrolliert introspektierbar und auditierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- langfristig evolvierbare Kryptographie,
- Austausch kompromittierter oder veralteter Algorithmen,
- Unterstützung zukünftiger Post-Quantum-Verfahren,
- einheitliche Nutzung von Software- und Hardware-Kryptographie,
- zentrale Durchsetzung kryptographischer Security Policies.

### Negative Konsequenzen

- zusätzliche Provider- und Policy-Komplexität,
- kryptographische Formate benötigen Algorithmus- und Versionsinformationen,
- Migration langfristig gespeicherter Daten muss berücksichtigt werden.

---

## Verworfene Alternativen

### Feste Algorithmen in System-APIs

Verworfen.

Dies würde spätere Migrationen unnötig erschweren.

### Ein dauerhaft vorgeschriebener Algorithmus pro Operation

Verworfen.

Die langfristige Sicherheit eines einzelnen kryptographischen Verfahrens kann nicht garantiert werden.

### Automatischer Fallback auf schwächere Verfahren

Verworfen.

Verfügbarkeit darf kryptographische Hard Constraints nicht umgehen.

---

## Abhängigkeiten

- `ADR-ARCH-0019_Evolvable_System_Architecture`
- `ADR-SECURITY-0013_Code_Integrity_und_Code_Signing`
- `ADR-SECURITY-0018_Fail_Closed_für_Sicherheitsentscheidungen`
- `ADR-SECURITY-0019_Secret_Use_und_Secret_Export_getrennt`
- `ADR-SECURITY-0020_Hardware_Backend_für_schützenswerte_Schlüssel`

---

## Zugehörige NPSPECs

- `NPSPEC-CRYPTO-AGILITY-0001`
- `NPSPEC-CRYPTO-POLICY-0001`
- `NPSPEC-CRYPTO-ALGORITHM-REGISTRY-0001`
- `NPSPEC-CRYPTO-PROVIDER-0001`
- `NPSPEC-CRYPTO-MIGRATION-0001`
- `NPSPEC-CRYPTO-AGILITY-TEST-0001`

---

## Ergebnis

NovaOS bindet kryptographische Sicherheitsanforderungen nicht dauerhaft an einzelne Algorithmen:

```text
Security Requirement
        ↓
Crypto Policy
        ↓
Eligible Algorithms
        ↓
Best Suitable Provider
        ↓
Cryptographic Operation
```

Dadurch können kryptographische Verfahren kontrolliert ergänzt, ersetzt, eingeschränkt oder vollständig entfernt werden, ohne die übergeordnete Sicherheitsarchitektur neu entwerfen zu müssen.