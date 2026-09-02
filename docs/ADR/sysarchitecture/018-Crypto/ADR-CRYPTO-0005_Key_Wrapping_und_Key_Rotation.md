# ADR-CRYPTO-0005 – Key Wrapping und Key Rotation

## Status

Angenommen

## Kategorie

Cryptography / Key Management / Lifecycle / Data Protection

## Kontext

Kryptographische Schlüssel besitzen einen eigenen Lebenszyklus. Sie können kompromittiert, veraltet, durch neue Policies ersetzt oder aufgrund kryptographischer Migration ausgetauscht werden müssen.

Eine Rotation darf jedoch nicht zwangsläufig erfordern, sämtliche geschützten Nutzdaten vollständig neu zu verschlüsseln.

NovaOS benötigt deshalb eine Architektur, die **Key Wrapping und Key Rotation gemeinsam** unterstützt.

---

## Entscheidung

NovaOS verwendet **Key Wrapping als zentralen Mechanismus zur Entkopplung von Datenverschlüsselung und übergeordnetem Schlüsselschutz** und unterstützt kontrollierte Key Rotation auf mehreren Ebenen.

```text
Key Encryption Key
        ↓
   Wrapped Data Key
        ↓
      Data
```

Dadurch kann beispielsweise ein Key Encryption Key rotiert werden, indem untergeordnete Schlüssel neu gewrappt werden, ohne die eigentlichen Nutzdaten neu verschlüsseln zu müssen.

---

## Grundprinzip

```text
Rotate protection independently
from protected data where possible.
```

---

## Key Wrapping

NovaOS unterscheidet mindestens zwischen:

```text
Key Encryption Key (KEK)
        ↓
Wrapped Key
        ↓
Data Encryption Key (DEK)
        ↓
Encrypted Data
```

Ein Wrapped Key wird nur innerhalb eines autorisierten kryptographischen Kontextes entpackt oder verwendet.

Key Wrapping darf Secret-Export-Regeln nicht umgehen.

---

## Rotationsebenen

NovaOS unterstützt unterschiedliche Rotationsarten.

### KEK Rotation

```text
Old KEK
   ↓
Wrapped DEK
   ↓ re-wrap
New KEK
   ↓
Wrapped DEK
```

Die Nutzdaten müssen dabei nicht neu verschlüsselt werden.

### DEK Rotation

```text
Old DEK
   ↓
Encrypted Data
   ↓ re-encrypt
New DEK
```

Hierbei kann eine tatsächliche Re-Encryption der betroffenen Daten erforderlich sein.

### Root- oder Domain-Key Rotation

Übergeordnete Schlüssel der hierarchischen Key Architecture müssen ebenfalls kontrolliert rotierbar sein.

Die Auswirkungen hängen von den jeweiligen Derivation- und Wrapping-Beziehungen ab.

---

## Rotationsgründe

Key Rotation kann unter anderem ausgelöst werden durch:

- reguläre Lifecycle Policy,
- vermutete oder bestätigte Kompromittierung,
- Änderung der Security Policy,
- Algorithmusmigration,
- Hardwarewechsel,
- Benutzer- oder Organisationswechsel,
- Revocation,
- kryptographische Compliance-Anforderungen.

Rotation darf nicht ausschließlich zeitbasiert modelliert werden.

---

## Crypto Agility

Key Rotation wird mit Crypto Agility kombiniert.

Eine Migration kann gleichzeitig:

```text
Key
Algorithm
Parameters
Provider
Protection Backend
```

ändern.

Dadurch können beispielsweise klassische Schlüssel kontrolliert durch neue oder hybride kryptographische Verfahren ersetzt werden.

---

## Hardwaregeschützte Schlüssel

Key Wrapping und Rotation müssen mit Hardware Backends zusammenarbeiten können.

```text
TPM / HSM / Secure Element
          ↓
         KEK
          ↓
     Wrapped Keys
```

Nicht exportierbare Hardware-Schlüssel dürfen für Wrapping-Operationen verwendet werden, ohne ihr Schlüsselmaterial offenzulegen.

---

## Per-Object Encryption

Per-Object Encryption kann jedem Objekt einen eigenen DEK oder kryptographisch isolierten Key Context zuordnen.

```text
Domain KEK
 ├── Wrapped Object Key A
 ├── Wrapped Object Key B
 └── Wrapped Object Key C
```

Dadurch kann der übergeordnete Schutz rotiert werden, ohne sämtliche Objekte neu zu verschlüsseln.

---

## Rotation und Verfügbarkeit

Rotation soll soweit möglich inkrementell und transaktional erfolgen.

Während einer Migration können alte und neue Key Generations kontrolliert parallel existieren.

```text
Generation N
     ↓
Migration
     ↓
Generation N + 1
     ↓
Retire N
```

Die konkrete Zustandsmaschine wird in NPSPECs definiert.

Ein Absturz während der Rotation darf keinen undefinierten kryptographischen Zustand erzeugen.

---

## Revocation und Kompromittierung

Rotation und Revocation sind unterschiedliche Mechanismen.

```text
Rotation
→ neuen Schlüssel verwenden

Revocation
→ alten Schlüssel nicht mehr autorisieren
```

Bei einer Kompromittierung kann beides erforderlich sein.

Eine Rotation allein macht bereits offengelegtes Schlüsselmaterial nicht wieder sicher.

---

## Backup und Recovery

Backup- und Recovery-Systeme müssen Key Generations berücksichtigen.

Ein Backup darf nicht unbrauchbar werden, nur weil der aktive Produktionsschlüssel rotiert wurde, sofern dessen Recovery Policy eine Wiederherstellung vorsieht.

Gleichzeitig dürfen alte Backup-Schlüssel keine unbeabsichtigte Umgehung aktueller Security Policies ermöglichen.

---

## Audit und Introspection

NovaOS muss kontrolliert nachvollziehen können:

```text
Key Identity
Key Generation
Wrapping Relationship
Rotation State
Crypto Policy
Provider / Backend
Revocation State
```

Schlüsselmaterial selbst darf dabei nicht offengelegt werden.

---

## Normative Anforderungen

1. NovaOS MUSS Key Wrapping und Key Rotation systemweit unterstützen.
2. KEKs und DEKs MÜSSEN als unterschiedliche kryptographische Rollen modellierbar sein.
3. KEK Rotation MUSS ohne vollständige Re-Encryption der Nutzdaten möglich sein, sofern die verwendete Key Architecture dies erlaubt.
4. DEK Rotation MUSS kontrollierte Re-Encryption unterstützen.
5. Schlüssel MÜSSEN eine eindeutig unterscheidbare Generation besitzen können.
6. Rotation MUSS mit der hierarchischen Key Architecture integrierbar sein.
7. Rotation MUSS Crypto Agility und Algorithmusmigration unterstützen.
8. Hardwaregeschützte und nicht exportierbare Schlüssel MÜSSEN für Wrapping und Rotation verwendbar sein können.
9. Key Wrapping DARF Secret-Export-Policies NICHT umgehen.
10. Rotation SOLL inkrementell und transaktional ausführbar sein.
11. Fehler während einer Rotation DÜRFEN keinen undefinierten Key State erzeugen.
12. Rotation und Revocation MÜSSEN getrennte Konzepte bleiben.
13. Backup und Recovery MÜSSEN Key Generations berücksichtigen.
14. Alte Key Generations MÜSSEN nach erfolgreicher Migration gemäß Policy kontrolliert deaktiviert oder entfernt werden können.
15. Key-Rotation- und Wrapping-Zustände MÜSSEN auditierbar und introspektierbar sein, ohne Schlüsselmaterial offenzulegen.

---

## Konsequenzen

### Positive Konsequenzen

- effiziente Rotation großer verschlüsselter Datenbestände,
- geringere Notwendigkeit vollständiger Re-Encryption,
- klare Trennung zwischen Daten- und Schutzschlüsseln,
- gute Integration mit Per-Object Encryption,
- kontrollierte Algorithmus- und Hardwaremigration.

### Negative Konsequenzen

- mehrere Key Generations können zeitweise parallel existieren,
- Wrapping-Beziehungen erhöhen die Key-Management-Komplexität,
- Recovery muss historische Key Generations berücksichtigen.

---

## Verworfene Alternativen

### Jede Rotation verschlüsselt sämtliche Daten neu

Verworfen.

Dies wäre bei großen Datenbeständen unnötig teuer und langsam.

### Keine Key Generations

Verworfen.

Migration und parallele Übergangszustände wären nicht eindeutig modellierbar.

### Rotation ersetzt Revocation

Verworfen.

Ein kompromittierter Schlüssel muss unabhängig von seiner Ablösung explizit als nicht mehr vertrauenswürdig beziehungsweise verwendbar behandelt werden können.

---

## Abhängigkeiten

- `ADR-CRYPTO-0001_Crypto_Agility`
- `ADR-CRYPTO-0002_Verschlüsselung_at_Rest_und_in_Transit`
- `ADR-CRYPTO-0003_Per_Object_Encryption`
- `ADR-CRYPTO-0004_Hierarchische_Key_Architecture`
- `ADR-SECURITY-0019_Secret_Use_und_Secret_Export_getrennt`
- `ADR-SECURITY-0020_Hardware_Backend_für_schützenswerte_Schlüssel`

---

## Zugehörige NPSPECs

- `NPSPEC-CRYPTO-KEY-WRAPPING-0001`
- `NPSPEC-CRYPTO-KEY-ROTATION-0001`
- `NPSPEC-CRYPTO-KEY-GENERATION-0001`
- `NPSPEC-CRYPTO-KEY-REWRAPPING-0001`
- `NPSPEC-CRYPTO-KEY-REENCRYPTION-0001`
- `NPSPEC-CRYPTO-KEY-ROTATION-RECOVERY-0001`
- `NPSPEC-CRYPTO-KEY-ROTATION-TEST-0001`

---

## Ergebnis

NovaOS trennt die Rotation des kryptographischen Schutzes soweit möglich von der erneuten Verschlüsselung der eigentlichen Daten:

```text
Root / Domain Key
       ↓
      KEK
       ↓
   Wrapped DEK
       ↓
Encrypted Data
```

Dadurch können Schlüsselhierarchien, Algorithmen und Hardware Backends kontrolliert weiterentwickelt und rotiert werden, während umfangreiche Datenbestände nur dann neu verschlüsselt werden müssen, wenn dies kryptographisch tatsächlich erforderlich ist.