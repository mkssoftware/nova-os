# ADR-IDENTITY-0009 – Passkeys und MFA

## Status

Angenommen

## Kategorie

Identity Architecture / Authentication / Credentials

## Kontext

NovaOS benötigt starke Authentisierung für lokale und federierte Identitäten.

Passwörter allein sind anfällig für Phishing, Wiederverwendung und Credential-Diebstahl. Gleichzeitig dürfen stärkere Authentisierungsverfahren nicht fest an einen einzelnen Anbieter, ein bestimmtes Gerät oder eine bestimmte Technologie gekoppelt werden.

## Entscheidung

NovaOS unterstützt **Passkeys und Multi-Factor Authentication (MFA) als native Bestandteile der Identity-Architektur**.

Passkeys werden für geeignete interaktive Authentisierung gegenüber klassischen Passwörtern bevorzugt.

```text
Identity
   ↓
Credential Set
   ├── Passkey
   ├── Hardware Credential
   ├── Biometric-backed Credential
   └── weitere Authentication Factors
```

Credentials authentisieren eine Identity, sind aber nicht selbst die Identity.

## Grundprinzip

```text
Identity ≠ Credential

Authentication Strength
        =
Credential Properties
+ Independent Factors
+ Trust Context
+ Policy
```

## Passkeys

NovaOS unterstützt kryptographische Passkey-Verfahren mit gerätegebundenen oder sicher synchronisierbaren Credentials.

Private Schlüssel sollen nach Möglichkeit durch geeignete Hardware wie TPM, Secure Element oder vergleichbare geschützte Key Stores abgesichert werden.

NovaOS darf Passkeys nicht an einen einzelnen Cloud- oder Plattformanbieter binden.

## Multi-Factor Authentication

Policies können für bestimmte Operationen mehrere unabhängige Authentisierungsfaktoren verlangen.

Beispiele:

```text
Possession
Knowledge
Inherence
Hardware-backed Proof
External Trusted Device
```

Mehrere Nachweise derselben Sicherheitsgrundlage dürfen nicht automatisch als unabhängige Faktoren gelten.

## Risk- und Context-Aware Authentication

NovaOS kann die erforderliche Authentisierungsstärke vom Kontext abhängig machen.

Beispielsweise können sensible Operationen eine stärkere Authentisierung verlangen als normale lokale Nutzung.

```text
Identity
   ↓
Operation + Context + Policy
   ↓
Required Authentication Strength
   ↓
Credential / MFA Verification
```

Adaptive Verfahren dürfen zusätzliche Authentisierung verlangen, aber harte Security Policies nicht abschwächen.

## Step-Up Authentication

Bereits authentisierte Sessions können für besonders sensible Operationen eine erneute oder stärkere Authentisierung verlangen.

```text
Authenticated Session
        ↓
Sensitive Operation
        ↓
Step-Up Authentication
        ↓
Operation
```

Dadurch muss nicht jede normale Systemoperation mit maximaler Authentisierungsstärke durchgeführt werden.

## Recovery

Credential-Verlust darf nicht zwangsläufig zum Verlust der zugrunde liegenden Identity führen.

Recovery muss jedoch mindestens den erforderlichen Trust- und Security-Anforderungen der betroffenen Identity entsprechen.

Unsichere Recovery-Verfahren dürfen starke Passkey- oder MFA-Sicherheit nicht faktisch umgehen.

## Lokale und federierte Identity

Passkeys und MFA müssen sowohl für lokale NovaOS-Identitäten als auch im Rahmen geeigneter Federation Provider verwendbar sein.

Lokale Passkey-Nutzung darf keine Cloud-Verbindung voraussetzen.

## Normative Anforderungen

1. NovaOS MUSS Passkeys als natives Authentication Credential unterstützen können.
2. Passkeys SOLLEN gegenüber klassischen Passwörtern bevorzugt werden, sofern der Einsatzkontext dies unterstützt.
3. NovaOS MUSS Multi-Factor Authentication unterstützen.
4. Identity und Authentication Credential MÜSSEN getrennte Konzepte bleiben.
5. Eine Identity MUSS mehrere Credentials und Authentication Factors besitzen können.
6. MFA-Policies MÜSSEN die Unabhängigkeit der verwendeten Faktoren berücksichtigen.
7. NovaOS MUSS Step-Up Authentication unterstützen können.
8. Authentication Requirements MÜSSEN kontext- und policyabhängig definierbar sein.
9. Adaptive Authentication DARF harte Security Policies nicht abschwächen.
10. Passkeys SOLLEN geeignete hardwaregeschützte Key Stores verwenden können.
11. Lokale Passkey-Authentisierung DARF keine Cloud-Abhängigkeit voraussetzen.
12. Recovery DARF die effektive Sicherheit von Passkeys oder MFA nicht unkontrolliert umgehen.

## Konsequenzen

### Positive Konsequenzen

- stärkere Phishing-Resistenz,
- weniger Abhängigkeit von Passwörtern,
- hardwaregestützte Credentials möglich,
- kontextabhängige MFA und Step-Up Authentication,
- Passkeys bleiben mit dem lokalen First-Class-Identity-Modell vereinbar.

### Negative Konsequenzen

- Credential Recovery wird sicherheitskritischer,
- unterschiedliche Hardware benötigt abstrahierte Credential Provider,
- MFA erhöht bei sensiblen Operationen den Interaktionsaufwand.

## Verworfene Alternativen

### Passwörter als primäres langfristiges Authentication-Modell

Verworfen, da stärkere kryptographische Verfahren verfügbar sind.

### Cloudgebundene Passkeys als Voraussetzung

Verworfen, da NovaOS lokale Identität und Offline-Nutzung als First-Class-Modell unterstützt.

### MFA für jede Operation erzwingen

Verworfen, da Authentication Strength kontext- und policyabhängig bestimmt werden soll.

## Abhängigkeiten

- `ADR-IDENTITY-0001_Identity_getrennt_von_Display_Name_Credential_Process_und_Address`
- `ADR-IDENTITY-0006_Lokale_Identität_als_First_Class_Modell`
- `ADR-IDENTITY-0007_Federated_Identity`
- `ADR-CRYPTO-0004_Hierarchical_Key_Architecture`
- `ADR-SECURITY-0020_Hardware_Backend_für_geschützte_Schlüssel`

## Zugehörige NPSPECs

- `NPSPEC-IDENTITY-PASSKEY-0001`
- `NPSPEC-IDENTITY-MFA-0001`
- `NPSPEC-IDENTITY-AUTHENTICATION-POLICY-0001`
- `NPSPEC-IDENTITY-STEP-UP-AUTHENTICATION-0001`
- `NPSPEC-IDENTITY-CREDENTIAL-RECOVERY-0001`
- `NPSPEC-IDENTITY-AUTHENTICATION-TEST-0001`

## Ergebnis

NovaOS verwendet Passkeys und MFA als native, policygesteuerte Authentication-Mechanismen:

```text
Identity
   ↓
Authentication Policy
   ↓
Passkey / MFA / Step-Up
   ↓
Verified Identity
   ↓
Capabilities + Authorization
```

Damit bleibt starke Authentisierung klar von Identity und Authority getrennt und funktioniert sowohl für lokale als auch für federierte NovaOS-Identitäten.