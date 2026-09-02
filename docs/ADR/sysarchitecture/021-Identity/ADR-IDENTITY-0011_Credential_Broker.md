# ADR-IDENTITY-0011 – Credential Broker

## Status

Angenommen

## Kategorie

Identity Architecture / Authentication / Credential Management

## Kontext

NovaOS unterstützt unterschiedliche Credentials und Authentisierungsverfahren, beispielsweise Passkeys, Hardware Credentials, Zertifikate und federierte Credentials.

Anwendungen und Dienste sollen diese Credentials nicht direkt verwalten oder auf geheimes Credential-Material zugreifen müssen.

Eine zentrale Vermittlung ist erforderlich, die Credentials sicher auswählt und für Authentisierung verwendet.

## Entscheidung

NovaOS führt einen **Credential Broker** als systemweite Vermittlungsschicht zwischen Identity, Credentials und Credentials anfordernden Komponenten ein.

```text
Application / Service
        ↓
Credential Request
        ↓
Credential Broker
        ↓
Policy + Identity Context
        ↓
Credential Provider
        ↓
Authentication Result
```

Der Broker stellt kontrollierte Credential-Nutzung bereit, ohne geheimes Credential-Material unnötig offenzulegen.

## Grundprinzip

```text
Use Credential
≠
Access Credential Secret
```

Eine Anwendung kann die Nutzung eines Credentials anfordern, ohne dessen privaten Schlüssel oder anderes geheimes Material zu erhalten.

## Credential Provider

Konkrete Credential-Technologien werden über Provider integriert.

Beispiele:

```text
Passkey Provider
TPM Provider
Secure Element Provider
Certificate Provider
Federation Provider
Legacy Credential Provider
```

Der Credential Broker bleibt dadurch unabhängig von einzelnen Credential-Technologien.

## Credential-Auswahl

Der Broker kann geeignete Credentials anhand des angeforderten Authentication Context auswählen.

Dabei können unter anderem berücksichtigt werden:

- Identity,
- Authentication Strength,
- Credential-Eigenschaften,
- Step-Up-Anforderungen,
- Trust Context,
- Security Policy.

Eine explizite Credential-Auswahl kann zugelassen werden, sofern sie die geltenden Policies erfüllt.

## Credential Isolation

Geheime Credential-Daten sollen möglichst innerhalb ihres geschützten Providers verbleiben.

```text
Private Key
    ↓
Protected Credential Provider
    ↓
Cryptographic Operation
    ↓
Result
```

Der Credential Broker vermittelt die Operation, anstatt Schlüsselmaterial zwischen Anwendungen zu verteilen.

## Authority

Der Zugriff auf den Credential Broker erzeugt keine automatische Berechtigung zur Verwendung beliebiger Credentials.

Credential-Nutzung muss explizit autorisiert werden.

```text
Credential Discovery
        ≠
Credential Use Authority
```

## Lokale und federierte Credentials

Der Broker unterstützt sowohl lokale als auch federierte Authentication-Verfahren.

Externe Provider bleiben dabei hinter derselben kontrollierten Broker-Abstraktion integrierbar.

Lokale Authentication darf weiterhin ohne Cloud-Abhängigkeit funktionieren.

## Step-Up Authentication

Der Credential Broker ist die zentrale Vermittlungsschicht für Step-Up Authentication.

```text
Operation
    ↓
Required Authentication Strength
    ↓
Credential Broker
    ↓
Suitable Credential / MFA
```

Damit müssen Anwendungen keine eigene Credential- oder MFA-Logik implementieren.

## Normative Anforderungen

1. NovaOS MUSS einen systemweiten Credential Broker bereitstellen.
2. Anwendungen SOLLEN Credentials über den Credential Broker verwenden.
3. Credential-Nutzung MUSS von direktem Zugriff auf Credential Secrets getrennt sein.
4. Der Broker MUSS unterschiedliche Credential Provider unterstützen können.
5. Credential Provider MÜSSEN austauschbar und erweiterbar sein.
6. Credential-Auswahl MUSS Security- und Authentication-Policies berücksichtigen.
7. Credential Discovery DARF keine Credential Use Authority erzeugen.
8. Credential-Nutzung MUSS explizit autorisierbar sein.
9. Geheimes Credential-Material SOLL den geschützten Provider möglichst nicht verlassen.
10. Der Broker MUSS Step-Up Authentication und MFA unterstützen können.
11. Lokale Credentials MÜSSEN ohne verpflichtende Cloud-Abhängigkeit verwendbar sein.
12. Sicherheitsrelevante Credential-Nutzung MUSS kontrolliert auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Anwendungen müssen Credentials nicht selbst verwalten,
- private Schlüssel können in geschützten Providern verbleiben,
- einheitliche Passkey-, MFA- und Step-Up-Integration,
- neue Credential-Technologien können über Provider ergänzt werden,
- zentrale Durchsetzung von Authentication Policies.

### Negative Konsequenzen

- der Credential Broker wird eine sicherheitskritische Systemkomponente,
- Provider benötigen klar definierte Trust- und Isolationseigenschaften.

## Verworfene Alternativen

### Credential-Verwaltung durch jede Anwendung

Verworfen, da dies Credential-Schutz und Policy-Durchsetzung fragmentieren würde.

### Direkte Übergabe privater Schlüssel

Verworfen, da Credential-Nutzung keinen Zugriff auf geheimes Schlüsselmaterial erfordern soll.

### Feste Credential-Technologie

Verworfen, da NovaOS Crypto- und Identity-Agility unterstützen muss.

## Abhängigkeiten

- `ADR-IDENTITY-0001_Identity_getrennt_von_Display_Name_Credential_Process_und_Address`
- `ADR-IDENTITY-0006_Lokale_Identität_als_First_Class_Modell`
- `ADR-IDENTITY-0007_Federated_Identity`
- `ADR-IDENTITY-0009_Passkeys_und_MFA`
- `ADR-IDENTITY-0010_Step_Up_Authentication`
- `ADR-SECURITY-0019_Secret_Use_und_Secret_Export_getrennt`
- `ADR-SECURITY-0020_Hardware_Backend_für_geschützte_Schlüssel`
- `ADR-CRYPTO-0001_Crypto_Agility`

## Zugehörige NPSPECs

- `NPSPEC-IDENTITY-CREDENTIAL-BROKER-0001`
- `NPSPEC-IDENTITY-CREDENTIAL-PROVIDER-0001`
- `NPSPEC-IDENTITY-CREDENTIAL-SELECTION-0001`
- `NPSPEC-IDENTITY-CREDENTIAL-AUTHORITY-0001`
- `NPSPEC-IDENTITY-CREDENTIAL-BROKER-TEST-0001`

## Ergebnis

NovaOS zentralisiert die kontrollierte Verwendung von Authentication Credentials über einen Credential Broker:

```text
Identity
   ↓
Credential Broker
   ↓
Policy
   ↓
Credential Provider
   ↓
Authentication Proof
```

Anwendungen erhalten damit die Fähigkeit, Credentials sicher zu verwenden, ohne geheimes Credential-Material selbst besitzen oder verwalten zu müssen.