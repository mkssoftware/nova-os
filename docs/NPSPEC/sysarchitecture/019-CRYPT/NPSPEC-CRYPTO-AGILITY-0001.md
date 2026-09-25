# NPSPEC-CRYPTO-AGILITY-0001 – Nova Cryptographic Agility

## Status

Angenommen

## Kategorie

Cryptography / Architecture / Crypto Agility

## Zweck

NovaOS definiert Crypto Agility als Fähigkeit, kryptografische Algorithmen, Parameter und Provider kontrolliert auszutauschen, ohne Anwendungen oder Protokolle grundlegend ändern zu müssen.

```text
Crypto Request
     ↓
Crypto Policy
     ↓
Algorithm + Provider
     ↓
Execution
```

## Grundprinzipien

```text
Algorithm ≠ API
Provider ≠ Algorithm
Preferred ≠ Required
Fallback ≠ Downgrade
Legacy Support ≠ Permanent Support
```

Anwendungen sollen kryptografische Anforderungen beschreiben, statt unnötig konkrete Implementierungen fest einzubauen.

## Algorithmusmodell

Algorithmen besitzen einen Policy-Zustand:

```text
Preferred
Allowed
Legacy
Deprecated
Forbidden
```

Dieser Zustand darf abhängig sein von:

```text
Operation
Security Domain
Protocol
Key Type
Compliance Policy
```

## Auswahl

Die Auswahl erfolgt anhand von:

```text
Security Requirements
        +
Crypto Policy
        +
Protocol Requirements
        +
Provider Capabilities
        ↓
Selected Algorithm
```

Sicherheitsanforderungen haben Vorrang vor Performance-Präferenzen.

## Algorithmuswechsel

NovaOS muss Algorithmen austauschen können:

```text
Algorithm A
    ↓
Deprecation
    ↓
Migration
    ↓
Algorithm B
```

Neue Algorithmen sollen integrierbar sein, ohne öffentliche APIs oder Anwendungen unnötig zu verändern.

## Providerwechsel

Dieselbe kryptografische Operation darf unterschiedliche Provider verwenden:

```text
Software
CPU Acceleration
TPM
HSM
```

Ein Providerwechsel darf die geforderte Sicherheitssemantik nicht abschwächen.

## Parameter

Nicht nur Algorithmen, sondern auch Parameter müssen policygesteuert sein können:

```text
Key Size
Curve
Hash Function
KDF Parameters
Protocol Parameters
```

Unsichere Parameter müssen unabhängig vom Algorithmus deaktivierbar sein.

## Downgrade-Schutz

Kann kein zulässiges Verfahren verwendet werden, muss die Operation fehlschlagen können.

```text
Preferred unavailable
        ↓
Allowed alternative?
   ├── Yes → Continue
   └── No  → Fail
```

NovaOS darf nicht still auf ein verbotenes oder unsicheres Verfahren zurückfallen.

## Migration

Bestehende Daten und Schlüssel müssen kontrolliert auf neue Verfahren migrierbar sein.

```text
Existing Crypto State
        ↓
Migration Policy
        ↓
Re-encrypt / Re-sign / Re-key
        ↓
New Crypto State
```

Migration muss transaktional und überprüfbar erfolgen können.

## Kompatibilität

Legacy-Verfahren dürfen für kontrollierte Kompatibilität verfügbar bleiben.

Sie müssen:

```text
Explicit
Restricted
Auditable
Disableable
```

sein.

Legacy-Unterstützung darf keine neuen Daten standardmäßig mit veralteten Verfahren erzeugen.

## Introspection

NovaOS soll mindestens darstellen können:

```text
Algorithm
Policy State
Provider
Parameters
Selection Reason
Migration State
```

Geheime Schlüssel oder internes Schlüsselmaterial dürfen nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Crypto Agility unterstützen.
2. Anwendungen SOLLEN nicht unnötig an konkrete kryptografische Implementierungen gebunden sein.
3. Algorithmen MÜSSEN policygesteuert deaktivierbar sein.
4. Kryptografische Parameter MÜSSEN separat kontrollierbar sein.
5. Provider MÜSSEN austauschbar sein können.
6. Ein Providerwechsel DARF Sicherheitsanforderungen NICHT abschwächen.
7. Unsichere Fallbacks DÜRFEN NICHT automatisch erfolgen.
8. Legacy-Verfahren MÜSSEN explizit einschränkbar sein.
9. Bestehende kryptografische Daten SOLLEN kontrolliert migrierbar sein.
10. Auswahl- und Migrationszustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CRYPTO-ARCH-0001`
- `NPSPEC-SECURITY-CODESIGNING-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-CRYPTO-0002`

## Ergebnis

```text
Security Requirement
        ↓
Crypto Policy
        ↓
Best Allowed Algorithm
        ↓
Replace / Migrate when needed
```

NovaOS kann kryptografische Verfahren dadurch über Jahrzehnte kontrolliert weiterentwickeln, ohne Anwendungen dauerhaft an einzelne Algorithmen oder Provider zu binden.