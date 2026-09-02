# ADR-IDENTITY-0006 – Lokale Identität als First-Class-Modell

## Status

Angenommen

## Kategorie

Identity Architecture / Local Identity

## Kontext

NovaOS muss vollständig ohne Cloud-, Netzwerk- oder externe Identity Provider funktionieren können.

Benutzer, Anwendungen, Dienste und Geräte benötigen deshalb native lokale Identitäten, die nicht lediglich als Offline-Ersatz für externe Accounts behandelt werden.

## Entscheidung

NovaOS behandelt **lokale Identität als First-Class-Modell**.

```text
NovaOS Identity
 ├── Local Identity
 └── Federated / External Identity
```

Eine lokale Identity ist eine vollständige native NovaOS-Identity und benötigt keine externe Identitätsquelle.

## Grundprinzip

```text
Local Identity ≠ Offline Fallback

Local Identity = Native Identity
```

Netzwerkzugang, Cloud-Konto oder externer Identity Provider dürfen keine Voraussetzung für eine vollständige lokale Identität sein.

## Lokale Identity Authority

NovaOS kann lokale Identities selbst erzeugen, verwalten und verifizieren.

Dazu gehören insbesondere:

- Identity Lifecycle,
- lokale Credentials,
- Credential Rotation,
- Recovery,
- lokale Identity Bindings.

Die konkrete Credential- und Recovery-Architektur wird in separaten Spezifikationen definiert.

## Lokale und externe Identitäten

Eine lokale Identity kann optional mit externen Identitäten verbunden werden.

```text
Local Identity
     │
     ├── External Identity A
     └── External Identity B
```

Diese Verknüpfungen ersetzen die lokale Identity nicht.

Das Entfernen eines externen Accounts darf deshalb nicht automatisch die lokale NovaOS-Identity zerstören.

## Offline-Fähigkeit

Lokale Authentication und lokale Autorisierungsentscheidungen müssen grundsätzlich ohne Netzwerkverbindung möglich sein.

```text
Local Identity
      ↓
Authentication
      ↓
Capabilities + Policies
      ↓
Local Operation
```

Externe Dienste dürfen zusätzliche externe Authentication verlangen, aber nicht die grundlegende lokale Systemidentität definieren.

## Identity und Authority

Auch eine lokal authentisierte Identity besitzt nicht automatisch vollständige Systemrechte.

```text
Local Identity ≠ Administrator
Local Identity ≠ Authority
```

Authority wird weiterhin durch Capabilities und Policies bestimmt.

## Privacy

Lokale Identitäten sollen ermöglichen, dass Identity-Daten vollständig auf dem Gerät verbleiben können.

Eine Synchronisierung oder Offenlegung gegenüber externen Identity Providern darf nicht allein durch die Existenz einer lokalen Identity erforderlich werden.

## Normative Anforderungen

1. NovaOS MUSS vollständige lokale Identities unterstützen.
2. Lokale Identities MÜSSEN ohne Cloud- oder Netzwerkverbindung verwendbar sein.
3. Lokale Identity DARF NICHT lediglich als Fallback eines externen Accounts modelliert werden.
4. NovaOS MUSS lokale Identity Lifecycles selbst verwalten können.
5. Lokale Identities MÜSSEN lokale Credentials verwenden können.
6. Externe Identities MÜSSEN optional mit lokalen Identities verknüpfbar sein.
7. Externe Identity Bindings DÜRFEN die lokale Identity nicht ersetzen.
8. Das Entfernen eines externen Bindings DARF die lokale Identity nicht automatisch entfernen.
9. Lokale Authentication MUSS grundsätzlich offline möglich sein.
10. Lokale Identity DARF keine automatische administrative Authority erzeugen.
11. Identity-Daten DÜRFEN ohne explizite Policy nicht automatisch an externe Dienste übertragen werden.
12. Lokale Identity Bindings MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- NovaOS bleibt vollständig offline nutzbar.
- keine technische Abhängigkeit von Cloud-Accounts,
- bessere Data Sovereignty und Privacy,
- externe Identity Provider bleiben optional,
- lokale und federierte Identitäten können sauber kombiniert werden.

### Negative Konsequenzen

- NovaOS benötigt eine vollständige lokale Identity-Infrastruktur.
- Recovery und Credential Management müssen lokal zuverlässig funktionieren.

## Verworfene Alternativen

### Cloud Identity als primäre Systemidentität

Verworfen, da NovaOS dadurch von externen Diensten abhängig würde.

### Lokaler Account nur als Offline-Fallback

Verworfen, da lokale Identität ein vollwertiges natives Systemmodell sein soll.

## Abhängigkeiten

- `ADR-IDENTITY-0001_Identity_getrennt_von_Display_Name_Credential_Process_und_Address`
- `ADR-IDENTITY-0002_Stabile_Systemidentitäten`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-PRIVACY-0001_Data_Minimization`

## Zugehörige NPSPECs

- `NPSPEC-IDENTITY-LOCAL-0001`
- `NPSPEC-IDENTITY-LOCAL-CREDENTIAL-0001`
- `NPSPEC-IDENTITY-LOCAL-LIFECYCLE-0001`
- `NPSPEC-IDENTITY-EXTERNAL-BINDING-0001`
- `NPSPEC-IDENTITY-LOCAL-RECOVERY-0001`
- `NPSPEC-IDENTITY-LOCAL-TEST-0001`

## Ergebnis

Lokale Identität ist in NovaOS kein eingeschränkter Offline-Modus, sondern ein vollständiges natives Identity-Modell:

```text
Local NovaOS Identity
        │
        ├── Local Credentials
        ├── Local Capabilities
        ├── Local Policies
        └── Optional External Bindings
```

Damit bleibt NovaOS unabhängig von externen Identity Providern, während externe Identitäten bei Bedarf zusätzlich eingebunden werden können.