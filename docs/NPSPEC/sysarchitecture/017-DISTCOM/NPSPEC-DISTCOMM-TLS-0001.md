# NPSPEC-DISTCOMM-TLS-0001 – Nova Transport Layer Security

## Status

Angenommen

## Kategorie

Distributed Communication / Security / TLS

## Zweck

NovaOS definiert eine gemeinsame TLS-Infrastruktur für authentifizierte und verschlüsselte Kommunikation zwischen lokalen oder entfernten Kommunikationspartnern.

```text
Application Protocol
        ↓
Nova TLS
        ↓
Transport
        ↓
Nova Network
```

TLS wird als gemeinsame Sicherheitsfähigkeit bereitgestellt, damit Anwendungen kryptografische Transportlogik nicht selbst implementieren müssen.

## Grundprinzip

```text
Encryption ≠ Authorization
Certificate ≠ Permission
TLS Session ≠ Application Identity
Secure Transport ≠ Trusted Application
```

TLS schützt einen Kommunikationskanal. Zugriffskontrolle und Capability-Prüfung bleiben separate Mechanismen.

## TLS-Modell

Eine TLS-Verbindung besitzt mindestens:

```text
TLSSession
├── SessionID
├── Role
├── Peer Identity
├── Protocol Version
├── Cipher Suite
├── Security State
└── Transport
```

Optional:

```text
ALPN
Certificate Chain
Trust Policy
Security Context
ExecutionContract
Network Intent
Lifetime
```

Die `SessionID` ist eine interne logische Identität und keine Netzwerkadresse.

## Protokollversionen

NovaOS soll moderne TLS-Versionen unterstützen.

```text
TLS 1.3 → bevorzugt
TLS 1.2 → kontrollierte Kompatibilität
Legacy   → standardmäßig nicht zulässig
```

Veraltete oder unsichere Protokollversionen und Algorithmen dürfen durch Systempolicy vollständig deaktiviert werden.

## Handshake

Der TLS-Handshake wird als kontrollierter Zustandsübergang behandelt.

```text
Created
   ↓
Negotiating
   ↓
Authenticating
   ↓
Establishing Keys
   ↓
Secure
```

Fehler führen zu:

```text
Failed
   ↓
Closed
```

Eine Verbindung darf erst nach erfolgreicher Sicherheitsprüfung als `Secure` gelten.

## Identitätsprüfung

Bei Serverauthentifizierung müssen mindestens geprüft werden können:

```text
Certificate Chain
Hostname / Service Identity
Validity
Trust Anchor
Revocation Policy
Signature
```

Eine erfolgreiche Verschlüsselung ohne erfolgreiche Identitätsprüfung darf nicht automatisch als vertrauenswürdige Verbindung gelten.

## Mutual TLS

NovaOS soll gegenseitige Authentifizierung unterstützen.

```text
Client Certificate
       ↕
TLS Handshake
       ↕
Server Certificate
```

mTLS darf für Systemdienste, Unternehmensnetzwerke und andere kontrollierte Kommunikationsdomänen verwendet werden.

## Trust Store

NovaOS soll eine zentrale Trust-Infrastruktur verwenden.

```text
System Trust Store
      ↓
Trust Policy
      ↓
TLS Validation
```

Zusätzliche Trust Stores dürfen für:

```text
Application
Namespace
Organization
Development
```

bereitgestellt werden.

Eine Anwendung darf globale Trust-Regeln nicht ohne entsprechende Berechtigung verändern.

## Schlüsselmaterial

Private Schlüssel und Session Secrets müssen geschützt behandelt werden.

```text
Key Material
    ↓
Protected Crypto Context
    ↓
TLS Engine
```

Geheime Schlüssel dürfen nicht über normale Logging-, Debug- oder Introspection-Schnittstellen ausgegeben werden.

Hardwaregestützte Schlüsselablage wie TPM-basierte Mechanismen darf verwendet werden.

## Session Resumption

TLS darf Session Resumption unterstützen.

```text
Previous Session
      ↓
Resume
      ↓
Reduced Handshake Cost
```

Wiederaufnahme darf Sicherheits- und Policy-Prüfungen nicht umgehen.

Session-Tickets und vergleichbare Geheimnisse müssen geschützt und zeitlich begrenzt sein.

## 0-RTT

Falls ein Protokoll 0-RTT unterstützt, muss Replay-Risiko explizit berücksichtigt werden.

```text
0-RTT Data
    ↓
Replay Possible
    ↓
Application Semantics
```

Nicht replay-sichere Operationen dürfen nicht automatisch über 0-RTT ausgeführt werden.

## HTTP und QUIC

TLS muss in moderne Kommunikationsprotokolle integrierbar sein.

```text
HTTP/1.1 ─┐
HTTP/2   ─┼→ TLS → TCP
          │
HTTP/3   ─┴→ QUIC + TLS
```

QUIC verwendet seine integrierte TLS-basierte Sicherheitsarchitektur und darf nicht künstlich durch eine zusätzliche klassische TLS-Schicht gekapselt werden.

## Network Intent und Sovereignty

TLS bleibt Bestandteil der übergeordneten Netzwerkpolicy.

```text
Network Intent
      ↓
Security Requirements
      ↓
TLS
```

TLS darf Routing-, VPN-, Firewall-, Trust- oder Sovereignty-Anforderungen nicht ersetzen oder umgehen.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Concurrent Handshakes
Active Sessions
Certificates
Session Cache
Session Tickets
Crypto Buffers
Handshake Time
```

Unvollständige oder bösartige Handshakes dürfen keinen unbegrenzten Ressourcenverbrauch verursachen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
SessionID
State
TLS Version
Cipher Suite
Peer Identity
Certificate State
Trust State
ALPN
Session Resumed
Failure Reason
```

Nicht ausgegeben werden dürfen:

```text
Private Keys
Session Secrets
Raw Credentials
Sensitive Key Material
```

## Normative Anforderungen

1. NovaOS MUSS eine gemeinsame TLS-Infrastruktur bereitstellen können.
2. TLS 1.3 SOLL die bevorzugte TLS-Version sein.
3. Unsichere Legacy-Versionen MÜSSEN durch Policy deaktivierbar sein.
4. Verschlüsselung und Identitätsprüfung MÜSSEN getrennt bewertet werden.
5. Zertifikats- und Trust-Prüfungen MÜSSEN zentral integrierbar sein.
6. NovaOS SOLL Mutual TLS unterstützen.
7. Private Schlüssel und Session Secrets MÜSSEN geschützt werden.
8. Session Resumption DARF Sicherheitsprüfungen NICHT umgehen.
9. 0-RTT MUSS Replay-Risiken berücksichtigen.
10. TLS DARF Firewall-, Sovereignty- oder Capability-Regeln NICHT umgehen.
11. TLS-Handshakes und Sessions MÜSSEN ressourcenbegrenzt sein.
12. TLS-Zustände SOLLEN ohne Offenlegung geheimer Schlüssel introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DISTCOMM-HTTP-0001`
- `NPSPEC-NETWORK-TCP-0001`
- `NPSPEC-NETWORK-QUIC-0001`
- `NPSPEC-NETWORK-FLOW-0001`
- `NPSPEC-NETWORK-INTENT-0001`
- `NPSPEC-NETWORK-SOVEREIGNTY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-DISTCOMM-0002`

## Ergebnis

```text
Application Protocol
        ↓
Identity + Trust Validation
        ↓
Nova TLS
        ↓
Authenticated Encryption
        ↓
TCP / QUIC
        ↓
Nova Network
```

NovaOS erhält damit eine zentrale TLS-Infrastruktur, die Verschlüsselung, Identitätsprüfung, Trust Management und Schlüsselverwaltung systemweit vereinheitlicht, ohne Transportverschlüsselung mit Autorisierung oder Vertrauen gleichzusetzen.