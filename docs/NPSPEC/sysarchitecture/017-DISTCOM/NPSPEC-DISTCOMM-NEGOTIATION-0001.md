# NPSPEC-DISTCOMM-NEGOTIATION-0001 – Nova Distributed Negotiation

## Status

Angenommen

## Kategorie

Distributed Communication / Negotiation / Capability Discovery

## Zweck

NovaOS definiert eine gemeinsame Negotiation-Infrastruktur, mit der Kommunikationspartner vor oder während einer verteilten Kommunikation kompatible Protokolle, Versionen, Schemas, Encodings und Fähigkeiten bestimmen können.

```text
Endpoint A
    ↓
Supported Capabilities
    ↕
Negotiation
    ↕
Supported Capabilities
    ↓
Endpoint B
```

Negotiation verhindert, dass Kommunikationspartner implizit identische Fähigkeiten voraussetzen.

## Grundprinzip

```text
Supported ≠ Selected
Compatible ≠ Authorized
Negotiated ≠ Trusted
Preferred ≠ Required
Fallback ≠ Security Downgrade
```

Negotiation bestimmt eine gemeinsame technische Kommunikationsbasis, erzeugt jedoch keine zusätzlichen Berechtigungen.

## Negotiation-Modell

Eine Negotiation besitzt mindestens:

```text
NegotiationContext
├── NegotiationID
├── Local Capabilities
├── Remote Capabilities
├── Requirements
├── Selected Profile
└── State
```

Optional:

```text
Security Context
ExecutionContract
Network Intent
Resource Budget
Sovereignty Policy
Deadline
Fallback Policy
```

## Aushandelbare Eigenschaften

NovaOS soll mindestens folgende Eigenschaften aushandeln können:

```text
Protocol
Protocol Version
Schema Version
Serialization Format
Compression
Streaming Support
Security Features
Optional Extensions
```

Weitere Eigenschaften müssen erweiterbar sein.

## Anforderungen und Präferenzen

Negotiation unterscheidet zwischen:

```text
Required
Preferred
Supported
Forbidden
```

Beispiel:

```text
TLS 1.3        Required
HTTP/3         Preferred
HTTP/2         Supported
Legacy TLS     Forbidden
```

Eine Präferenz darf niemals eine zwingende Anforderung überschreiben.

## Auswahl

Die Auswahl erfolgt aus der Schnittmenge kompatibler Möglichkeiten.

```text
Local Support
      ∩
Remote Support
      ∩
Policy
      ∩
Security
      ∩
ExecutionContract
      ↓
Selected Profile
```

Existiert kein gültiges Profil, muss die Negotiation kontrolliert fehlschlagen.

## Priorität

Bei mehreren gültigen Optionen gilt grundsätzlich:

```text
Safety
  ↓
Security
  ↓
Sovereignty / Trust
  ↓
Hard Requirements
  ↓
Explicit User Decisions
  ↓
Soft Preferences
  ↓
Adaptive Optimization
```

Performance darf Sicherheits- oder Policy-Anforderungen nicht überschreiben.

## Version Negotiation

Protokoll- und Schema-Versionen müssen explizit ausgehandelt werden können.

```text
Endpoint A: v1, v2, v3
Endpoint B: v2, v3

Result: v2 oder v3
```

Die Auswahl muss zusätzlich Kompatibilitäts- und Policy-Regeln berücksichtigen.

Die numerisch höchste Version ist nicht automatisch die korrekte Auswahl.

## Schema Negotiation

Schema Negotiation verwendet:

`NPSPEC-DISTCOMM-SCHEMA-0001`

```text
TypeID
  ↓
Supported Schema Versions
  ↓
Compatibility Check
  ↓
Selected Schema
```

Eine strukturell vorhandene Version darf nicht ausgewählt werden, wenn ihre Semantik inkompatibel ist.

## Serialization Negotiation

Serialization Negotiation verwendet:

`NPSPEC-DISTCOMM-SERIALIZATION-0001`

Beispiel:

```text
Binary
Canonical Binary
Structured Text
```

Die Auswahl darf Anforderungen an Determinismus, Ressourcenverbrauch und Sicherheit berücksichtigen.

## Protokoll-Negotiation

Transport- und Anwendungsprotokolle dürfen unabhängig ausgehandelt werden.

Beispiel:

```text
HTTP/3 → QUIC
HTTP/2 → TCP + TLS
HTTP/1.1 → TCP + TLS
```

Die Anwendung soll nach Möglichkeit nicht von der konkret ausgewählten Transportvariante abhängig sein.

## Feature Negotiation

Optionale Funktionen müssen explizit erkennbar sein.

```text
Streaming
Compression
Zero-Copy
Multiplexing
Resume
Extensions
```

Nicht unterstützte optionale Funktionen dürfen deaktiviert werden.

Zwingend benötigte Funktionen dürfen nicht stillschweigend entfernt werden.

## Security Downgrade Protection

Fallback darf nicht zu einem unbemerkten Sicherheits-Downgrade führen.

```text
Secure Option Failed
        ↓
Legacy Insecure Option
        ↓
Reject
```

Ein Downgrade darf nur stattfinden, wenn er durch die geltende Policy ausdrücklich erlaubt ist.

## Caching

Negotiation-Ergebnisse dürfen gecacht werden.

```text
Peer / Service
     ↓
Negotiated Profile
     ↓
Cache
```

Cache-Einträge müssen begrenzt und invalidierbar sein.

Änderungen an:

```text
Security Policy
Schema
Protocol Support
Trust State
Service Version
```

müssen eine erneute Negotiation auslösen können.

## Dynamische Neuverhandlung

Bestimmte Kommunikationsbeziehungen dürfen während ihrer Lebensdauer neu bewertet werden.

```text
Active
  ↓
Environment Change
  ↓
Renegotiate
  ↓
Continue / Migrate / Fail
```

Eine Neuverhandlung darf bestehende Sicherheitsgarantien nicht unbemerkt abschwächen.

## Fehler

Negotiation-Fehler müssen strukturiert unterscheidbar sein.

```text
NoCompatibleProtocol
NoCompatibleVersion
NoCompatibleSchema
NoCompatibleEncoding
SecurityRequirementFailed
PolicyConflict
ResourceLimitExceeded
NegotiationTimeout
```

## Ressourcensteuerung

NovaOS muss Grenzen unterstützen für:

```text
Negotiation Attempts
Negotiation Time
Capability Count
Version Count
Extension Count
Negotiation Data Size
Cache Entries
Fallback Attempts
```

Manipulierte Gegenstellen dürfen keine unbegrenzte Negotiation erzeugen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
NegotiationID
Local Capabilities
Remote Capabilities
Requirements
Selected Protocol
Selected Version
Selected Schema
Selected Encoding
Rejected Alternatives
Decision Reason
State
```

Sensible Sicherheitsinformationen dürfen nicht ungefiltert sichtbar werden.

## Normative Anforderungen

1. NovaOS MUSS eine gemeinsame Negotiation-Infrastruktur bereitstellen können.
2. Protokolle, Versionen, Schemas und Encodings MÜSSEN aushandelbar sein.
3. Zwingende Anforderungen MÜSSEN von Präferenzen unterschieden werden.
4. Negotiation DARF keine zusätzlichen Berechtigungen erzeugen.
5. Die Auswahl MUSS Security-, Trust- und Sovereignty-Regeln berücksichtigen.
6. Inkompatible Semantik DARF NICHT allein aufgrund struktureller Kompatibilität akzeptiert werden.
7. Fallback DARF Sicherheitsanforderungen NICHT unbemerkt abschwächen.
8. Nicht unterstützte optionale Features DÜRFEN deaktiviert werden.
9. Zwingend benötigte Features DÜRFEN NICHT stillschweigend entfernt werden.
10. Negotiation-Ergebnisse DÜRFEN kontrolliert gecacht werden.
11. Negotiation und Fallback-Versuche MÜSSEN ressourcenbegrenzt sein.
12. Auswahlentscheidungen und Ablehnungsgründe SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DISTCOMM-HTTP-0001`
- `NPSPEC-DISTCOMM-TLS-0001`
- `NPSPEC-DISTCOMM-RPC-0001`
- `NPSPEC-DISTCOMM-SCHEMA-0001`
- `NPSPEC-DISTCOMM-SERIALIZATION-0001`
- `NPSPEC-NETWORK-INTENT-0001`
- `NPSPEC-NETWORK-SOVEREIGNTY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-DISTCOMM-0010`

## Ergebnis

```text
Local Capabilities
        ↘
         Negotiation
        ↗
Remote Capabilities
        ↓
Policy + Security + Constraints
        ↓
Compatible Communication Profile
        ↓
Distributed Communication
```

NovaOS erhält damit eine gemeinsame Negotiation-Infrastruktur, die kompatible Kommunikationsprofile dynamisch bestimmen kann, ohne Sicherheits-, Semantik- oder Policy-Anforderungen zugunsten technischer Kompatibilität aufzuweichen.