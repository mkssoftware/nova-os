# ADR-IDENTITY-0012 – Identity Recovery

## Status

Angenommen

## Kategorie

Identity Architecture / Recovery / Authentication

## Kontext

Der Verlust, die Beschädigung oder der Widerruf von Credentials darf nicht automatisch zum Verlust einer langlebigen NovaOS-Identity führen.

Gleichzeitig darf ein schwacher Recovery-Mechanismus starke Authentication-Verfahren wie Passkeys oder MFA nicht umgehen.

NovaOS benötigt deshalb ein eigenständiges, policygesteuertes Identity-Recovery-Modell.

## Entscheidung

NovaOS behandelt **Identity Recovery als kontrollierten Prozess zur Wiederherstellung des Zugriffs auf eine bestehende Identity**.

```text
Existing Identity
       ↓
Recovery Verification
       ↓
Identity Continuity
       ↓
New / Restored Credentials
```

Recovery erzeugt grundsätzlich keine neue Identity, sondern stellt den kontrollierten Zugriff auf die bestehende Identity wieder her.

## Grundprinzip

```text
Recover Access
≠
Create New Identity
```

Identity und Credentials bleiben getrennt.

Der Verlust eines Credentials bedeutet daher nicht automatisch den Verlust der Identity.

## Recovery Policy

Eine Identity kann eine explizite Recovery Policy besitzen.

Diese kann geeignete Recovery-Mechanismen festlegen, beispielsweise:

- Recovery Credentials,
- Hardware-backed Recovery,
- mehrere unabhängige Nachweise,
- autorisierte Recovery Identities,
- organisatorische Recovery-Verfahren,
- lokale Recovery-Verfahren.

Die konkrete Recovery-Policy wird in NPSPECs definiert.

## Sicherheitsniveau

Recovery darf nicht zum schwächsten Glied der Authentication-Architektur werden.

```text
Strong Authentication
        +
Weak Recovery
        =
Weak Effective Security
```

Die erforderliche Recovery Strength muss deshalb dem Schutzbedarf der Identity entsprechen.

## Credential Replacement

Nach erfolgreicher Recovery können verlorene oder kompromittierte Credentials:

```text
Revoke
Replace
Rotate
Rebind
```

werden.

Nicht mehr vertrauenswürdige Credentials dürfen dabei nicht unnötig gültig bleiben.

## Lokale Identity Recovery

Lokale NovaOS-Identitäten müssen ohne zwingende Cloud-Abhängigkeit wiederherstellbar sein können.

Recovery-Daten können dabei besonders geschützt, verschlüsselt oder hardwaregebunden gespeichert werden.

## Federated Identity Recovery

Die Recovery einer externen federierten Identity bleibt grundsätzlich Aufgabe des jeweiligen Identity Providers.

NovaOS kann anschließend das bestehende Federation Binding erneut verifizieren.

Die Recovery einer externen Identity darf nicht automatisch lokale NovaOS-Identity-Policies umgehen.

## Authority

Erfolgreiche Identity Recovery erzeugt keine zusätzliche Authority.

```text
Recovered Identity ≠ Elevated Authority
```

Bestehende Capability-, Security- und Policy-Regeln gelten weiterhin.

## Normative Anforderungen

1. NovaOS MUSS Identity Recovery als eigenständigen Identity-Lifecycle-Prozess unterstützen.
2. Credential-Verlust DARF nicht automatisch den Verlust einer langlebigen Identity bedeuten.
3. Recovery MUSS die bestehende Identity Continuity verifizieren.
4. Recovery DARF nicht automatisch eine neue Identity erzeugen.
5. Recovery Policies MÜSSEN pro Identity oder Identity-Klasse definierbar sein.
6. Recovery MUSS dem erforderlichen Sicherheitsniveau der Identity entsprechen.
7. Schwache Recovery-Verfahren DÜRFEN starke Authentication Policies nicht unkontrolliert umgehen.
8. Kompromittierte Credentials MÜSSEN im Rahmen der Recovery widerrufbar oder ersetzbar sein.
9. Lokale Identity Recovery MUSS ohne verpflichtende Cloud-Abhängigkeit möglich sein können.
10. Federated Recovery MUSS von lokaler NovaOS Identity Recovery getrennt bleiben.
11. Recovery DARF keine automatische zusätzliche Authority erzeugen.
12. Sicherheitsrelevante Recovery-Vorgänge MÜSSEN kontrolliert auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Credential-Verlust bedeutet nicht automatisch Identity-Verlust,
- Passkeys und MFA können sicher wiederhergestellt werden,
- lokale Identitäten bleiben unabhängig von Cloud-Anbietern,
- kompromittierte Credentials können kontrolliert ersetzt werden.

### Negative Konsequenzen

- Recovery-Infrastruktur ist selbst sicherheitskritisch,
- starke Recovery-Verfahren können zusätzlichen organisatorischen Aufwand erzeugen.

## Verworfene Alternativen

### Credential gleich Identity

Verworfen, da Credential-Verlust dadurch zwangsläufig Identity-Verlust bedeuten würde.

### Einfaches Passwort als universeller Recovery-Mechanismus

Verworfen, da dies starke Authentication-Verfahren effektiv abschwächen könnte.

### Cloud-Recovery als Pflicht

Verworfen, da lokale Identity ein First-Class-Modell von NovaOS ist.

## Abhängigkeiten

- `ADR-IDENTITY-0001_Identity_getrennt_von_Display_Name_Credential_Process_und_Address`
- `ADR-IDENTITY-0002_Stabile_Systemidentitäten`
- `ADR-IDENTITY-0006_Lokale_Identität_als_First_Class_Modell`
- `ADR-IDENTITY-0007_Federated_Identity`
- `ADR-IDENTITY-0009_Passkeys_und_MFA`
- `ADR-IDENTITY-0011_Credential_Broker`
- `ADR-SECURITY-0019_Secret_Use_und_Secret_Export_getrennt`

## Zugehörige NPSPECs

- `NPSPEC-IDENTITY-RECOVERY-0001`
- `NPSPEC-IDENTITY-RECOVERY-POLICY-0001`
- `NPSPEC-IDENTITY-RECOVERY-STRENGTH-0001`
- `NPSPEC-IDENTITY-CREDENTIAL-REPLACEMENT-0001`
- `NPSPEC-IDENTITY-RECOVERY-TEST-0001`

## Ergebnis

NovaOS trennt die langlebige Identity von ihren austauschbaren Credentials:

```text
Identity
   ↓
Credentials Lost
   ↓
Verified Recovery
   ↓
Credential Replacement
   ↓
Same Identity
```

Damit kann der Zugriff auf eine bestehende Identity sicher wiederhergestellt werden, ohne Identity Continuity, Authentication Strength oder das Capability-basierte Autorisierungsmodell aufzugeben.