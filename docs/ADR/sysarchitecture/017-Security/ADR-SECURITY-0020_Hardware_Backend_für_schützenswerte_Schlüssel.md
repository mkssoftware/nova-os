# ADR-SECURITY-0020 – Hardware Backend für schützenswerte Schlüssel

## Status

Angenommen

## Kategorie

Security / Cryptography / Key Management / Hardware Security

## Kontext

Besonders schützenswerte kryptographische Schlüssel sollten möglichst nicht als exportierbares Secret im normalen Arbeitsspeicher oder in gewöhnlichen Software-Key-Stores vorliegen.

Moderne Plattformen stellen hierfür unterschiedliche Sicherheitskomponenten bereit, beispielsweise:

- TPM,
- Secure Elements,
- Hardware Security Modules,
- Trusted Execution Environments,
- plattformspezifische Hardware-Key-Stores.

NovaOS benötigt eine gemeinsame Architektur, ohne sich dauerhaft an eine bestimmte Hardwaretechnologie zu binden.

---

## Entscheidung

NovaOS definiert eine **abstrakte Hardware-Backend-Schicht für schützenswerte kryptographische Schlüssel**.

```text
Application / System Service
          ↓
    Key Capability
          ↓
 Nova Secret / Crypto Layer
          ↓
 Hardware Backend Abstraction
          ↓
 ┌────────┼─────────┬─────────┐
 ↓        ↓         ↓         ↓
TPM   Secure Element   HSM   Future Backend
```

Anwendungen verwenden Schlüssel über ihre semantische Key Identity und Capabilities, nicht über hardwarespezifische APIs.

---

## Grundprinzip

```text
Use the key.
Do not expose the key.
```

---

## Nicht exportierbare Schlüssel

Hardware Backends müssen Schlüssel unterstützen können, deren privates Schlüsselmaterial die geschützte Hardware niemals verlässt.

```text
Caller
   ↓
Sign(Data)
   ↓
Hardware Backend
   ↓
Protected Key
   ↓
Signature
```

Der Caller erhält das Ergebnis der Operation, nicht den privaten Schlüssel.

---

## Backend-Abstraktion

NovaOS abstrahiert unterschiedliche Hardwaretechnologien über ein gemeinsames Key-Backend-Modell.

Backends deklarieren ihre Fähigkeiten, beispielsweise:

```text
Sign
Decrypt
Key Derivation
Key Generation
Sealing
Attestation
Non-Exportable Storage
```

Nicht jedes Backend muss sämtliche Funktionen unterstützen.

---

## Key Identity

Die logische Identität eines Schlüssels wird von dessen physischem Speicherort getrennt.

```text
Logical Key Identity
        ↓
Backend Resolution
        ↓
Physical Key Instance
```

Dadurch können unterschiedliche Hardwareplattformen dieselben höheren NovaOS-APIs verwenden.

---

## Capability Integration

Der Zugriff auf Hardware-Schlüssel erfolgt capability-basiert.

Unterschiedliche Operationen benötigen getrennte Authority.

```text
Key.Sign
Key.Decrypt
Key.Derive
Key.Attest
Key.Export
```

Eine Capability für eine Operation impliziert keine Authority für andere Operationen.

---

## Secret Export

Nicht exportierbare Hardware-Schlüssel besitzen keine normale Export-Authority.

Kann ein Backend Schlüssel exportieren, gelten weiterhin die Regeln aus:

`ADR-SECURITY-0019_Secret_Use_und_Secret_Export_getrennt`.

Eine Softwarekomponente darf Hardware-Schutz nicht umgehen, indem sie eine äquivalente exportierbare Kopie erzeugt, sofern die Key Policy dies verbietet.

---

## Key Policy

Schlüssel können Anforderungen an ihr zulässiges Backend besitzen.

Beispiele:

```text
Hardware Required
Hardware Preferred
Non-Exportable Required
Attestation Required
TPM Required
```

Ist ein erforderliches Sicherheitsmerkmal nicht verfügbar, darf NovaOS nicht stillschweigend auf ein schwächeres Software-Backend wechseln.

---

## Software-Fallback

Für Schlüssel ohne zwingende Hardwareanforderung darf ein geschütztes Software-Backend verwendet werden.

```text
Hardware Required
      ↓ unavailable
     Fail

Hardware Preferred
      ↓ unavailable
Software Backend
```

Ob ein Fallback zulässig ist, wird durch die Key Policy bestimmt.

---

## TPM und Boot Trust

TPM-Backends können zusätzlich mit Secure und Measured Boot verbunden werden.

Schlüssel können dadurch an definierte Systemzustände gebunden werden.

```text
Measured System State
        ↓
TPM Policy
        ↓
Key Use permitted
```

Die konkrete Sealing- und Attestation-Semantik wird in NPSPECs definiert.

---

## Hardwarewechsel und Migration

Hardwaregebundene Schlüssel können absichtlich nicht frei migrierbar sein.

NovaOS muss deshalb zwischen:

```text
Migratable Key
Non-Migratable Key
Recoverable Key
Hardware-Bound Key
```

unterscheiden können.

Migration oder Backup darf die ursprüngliche Key Policy nicht umgehen.

---

## Audit und Introspection

NovaOS muss für einen Schlüssel kontrolliert sichtbar machen können:

```text
Key Identity
Backend Type
Protection Level
Exportability
Supported Operations
Policy State
```

Privates Schlüsselmaterial darf dabei niemals durch Introspection oder Audit offengelegt werden.

---

## Normative Anforderungen

1. NovaOS MUSS eine gemeinsame Abstraktion für Hardware-Key-Backends bereitstellen.
2. Anwendungen SOLLEN nicht direkt von einer bestimmten Hardwaretechnologie abhängig sein müssen.
3. Hardware Backends MÜSSEN ihre unterstützten kryptographischen Fähigkeiten deklarieren können.
4. NovaOS MUSS nicht exportierbare Hardware-Schlüssel unterstützen.
5. Schlüsseloperationen MÜSSEN capability-basiert autorisierbar sein.
6. Unterschiedliche Schlüsseloperationen MÜSSEN getrennte Authority besitzen können.
7. Logische Key Identity MUSS vom physischen Hardware-Backend getrennt sein.
8. Key Policies MÜSSEN Hardware-Schutz verbindlich verlangen können.
9. Ein erforderliches Hardware-Backend DARF NICHT stillschweigend durch ein schwächeres Software-Backend ersetzt werden.
10. Software-Fallback DARF nur erfolgen, wenn die Key Policy dies erlaubt.
11. Hardware-Schutz DARF durch Export, Migration oder Backup NICHT unautorisiert abgeschwächt werden.
12. TPM-basierte Schlüssel MÜSSEN mit Secure und Measured Boot integrierbar sein.
13. Hardwaregebundene und migrierbare Schlüssel MÜSSEN unterscheidbar sein.
14. Backend- und Protection-Zustände MÜSSEN kontrolliert auditierbar und introspektierbar sein.
15. Audit und Introspection DÜRFEN privates Schlüsselmaterial NICHT offenlegen.

---

## Konsequenzen

### Positive Konsequenzen

- starke Isolation besonders kritischer Schlüssel,
- einheitliche API für unterschiedliche Sicherheitshardware,
- Unterstützung nicht exportierbarer Schlüssel,
- keine direkte Hardwareabhängigkeit höherer Anwendungen,
- Integration mit TPM, Attestation und Measured Boot.

### Negative Konsequenzen

- Hardware Backends besitzen unterschiedliche Fähigkeiten,
- Hardwarewechsel und Recovery können bei gebundenen Schlüsseln schwieriger sein,
- Backend-Abstraktion benötigt klare Capability- und Fehlersemantik.

---

## Verworfene Alternativen

### TPM als einziges Hardware-Backend

Verworfen.

NovaOS soll auch Secure Elements, HSMs und zukünftige Technologien unterstützen.

### Direkter Hardwarezugriff durch Anwendungen

Verworfen.

Dies würde Portabilität, Isolation und einheitliche Security Policies schwächen.

### Automatischer Software-Fallback

Verworfen.

Ein stiller Fallback könnte die ausdrücklich verlangte Schutzstufe eines Schlüssels reduzieren.

---

## Abhängigkeiten

- `ADR-BOOT-0007_TPM_Integration`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0013_Code_Integrity_und_Code_Signing`
- `ADR-SECURITY-0014_Secure_und_Measured_Boot`
- `ADR-SECURITY-0018_Fail_Closed_für_Sicherheitsentscheidungen`
- `ADR-SECURITY-0019_Secret_Use_und_Secret_Export_getrennt`

---

## Zugehörige NPSPECs

- `NPSPEC-SECURITY-KEY-BACKEND-0001`
- `NPSPEC-SECURITY-HARDWARE-KEY-0001`
- `NPSPEC-SECURITY-KEY-POLICY-0001`
- `NPSPEC-SECURITY-TPM-KEY-BACKEND-0001`
- `NPSPEC-SECURITY-HARDWARE-KEY-TEST-0001`

---

## Ergebnis

NovaOS trennt die logische Nutzung kryptographischer Schlüssel von deren physischer Speicherung:

```text
Key Identity
     ↓
Key Capability
     ↓
Crypto Operation
     ↓
Hardware Backend
     ↓
Protected Key
```

Besonders schützenswerte Schlüssel können dadurch innerhalb geeigneter Sicherheitshardware verbleiben, während Anwendungen ausschließlich kontrollierte kryptographische Operationen ausführen.