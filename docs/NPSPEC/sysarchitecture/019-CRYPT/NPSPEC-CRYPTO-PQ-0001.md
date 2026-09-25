# NPSPEC-CRYPTO-PQ-0001 – Nova Post-Quantum Cryptography

## Status

Angenommen

## Kategorie

Cryptography / Post-Quantum / Crypto Agility

## Zweck

NovaOS definiert eine Architektur für Post-Quantum Cryptography (PQC), damit kryptografische Verfahren gegen zukünftige Angriffe durch leistungsfähige Quantencomputer austauschbar und migrierbar bleiben.

```text
Crypto Requirement
       ↓
Crypto Policy
       ↓
Classical / Post-Quantum / Hybrid
       ↓
Crypto Provider
```

Post-Quantum-Verfahren werden in die bestehende Nova-Crypto-Architektur integriert und bilden kein separates Kryptosystem.

## Grundprinzipien

```text
Post-Quantum ≠ Automatically Secure
Quantum Resistant ≠ Trusted Implementation
New Algorithm ≠ Immediate Replacement
Hybrid ≠ Double Security
Crypto Agility ≠ Automatic Migration
```

## PQ-Modell

NovaOS muss Post-Quantum-Verfahren mindestens für folgende Klassen integrieren können:

```text
Key Establishment / KEM
Digital Signatures
Hybrid Key Establishment
Hybrid Signatures
```

Konkrete Algorithmen werden nicht dauerhaft in die Architektur eingebaut.

```text
AlgorithmID
     ↓
Crypto Policy
     ↓
Provider
```

## Hybrid-Verfahren

Während der Migration muss NovaOS hybride Verfahren unterstützen können.

```text
Classical Algorithm
        +
Post-Quantum Algorithm
        ↓
Hybrid Cryptographic Result
```

Damit können bestehende Verfahren und PQ-Verfahren während Übergangsphasen gemeinsam verwendet werden.

Die genaue Kombination muss durch die Crypto Policy definiert werden.

## Crypto Agility

PQC muss vollständig in `Nova Cryptographic Agility` integriert sein.

Algorithmen können Zustände besitzen:

```text
Experimental
Allowed
Preferred
Deprecated
Forbidden
```

NovaOS muss neue PQ-Verfahren integrieren können, ohne Anwendungen grundlegend ändern zu müssen.

## Schlüsselverwaltung

Post-Quantum-Schlüssel werden über den normalen Nova Key Store verwaltet.

```text
KeyID
 ↓
Key Store
 ↓
PQ Provider
```

Schlüssel dürfen abhängig vom Verfahren deutlich größer sein als klassische Schlüssel.

Resource Budgets müssen dies berücksichtigen.

## Signaturen

Post-Quantum-Signaturen müssen in bestehende Mechanismen integrierbar sein:

```text
Code Signing
Secure Boot
Updates
Packages
Certificates
Stored Objects
```

Bestehende Signaturen dürfen während einer Migration parallel zu PQ-Signaturen verwendet werden.

## Netzwerk und Protokolle

PQC muss in sichere Kommunikationsprotokolle integrierbar sein.

```text
TLS / Secure Protocol
        ↓
Negotiation
        ↓
Classical / PQ / Hybrid
```

Negotiation darf keinen unbemerkten Downgrade auf ein durch Policy verbotenes Verfahren erlauben.

## Migration

NovaOS muss kontrollierte Migration unterstützen:

```text
Classical
   ↓
Hybrid
   ↓
Post-Quantum
```

Migration darf abhängig sein von:

```text
Security Policy
Data Lifetime
Threat Model
Protocol Support
Provider Support
Compatibility
```

Langfristig vertrauliche Daten können frühere Migration erfordern als kurzlebige Daten.

## Provider

PQ-Algorithmen müssen über die normale Provider-Architektur implementierbar sein:

```text
Software
Accelerated Software
Hardware
Future Security Hardware
```

Anwendungen dürfen nicht von einer bestimmten Implementierung abhängig sein.

## Ressourcensteuerung

PQC kann größere:

```text
Keys
Signatures
Ciphertexts
Memory Requirements
CPU Requirements
```

verursachen.

Diese Kosten müssen in:

```text
ExecutionContract
Resource Budget
Network QoS
Storage
```

berücksichtigt werden können.

## Fehlerverhalten

Mindestens folgende Fehler müssen unterscheidbar sein:

```text
UnsupportedAlgorithm
PolicyDenied
NegotiationFailed
ProviderFailure
ResourceLimit
MigrationRequired
```

Ein Fehler darf keinen stillen kryptografischen Downgrade verursachen.

## Introspection

NovaOS soll mindestens darstellen können:

```text
Algorithm
Algorithm Class
Classical / PQ / Hybrid
Provider
Policy State
Migration State
```

Geheimes Schlüsselmaterial darf nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Post-Quantum-Algorithmen integrieren können.
2. PQC MUSS die bestehende Nova-Crypto-Architektur verwenden.
3. PQ-Algorithmen DÜRFEN NICHT fest in Anwendungen eingebaut werden müssen.
4. Hybrid-Verfahren MÜSSEN unterstützt werden können.
5. PQ-Schlüssel MÜSSEN über den normalen Key Store verwaltbar sein.
6. PQ-Signaturen MÜSSEN in bestehende Signaturmechanismen integrierbar sein.
7. Kryptografische Negotiation DARF keine unerlaubten Downgrades ermöglichen.
8. Migration MUSS schrittweise und policygesteuert möglich sein.
9. PQ-spezifische Ressourcenanforderungen MÜSSEN berücksichtigt werden können.
10. PQC MUSS vollständig mit Crypto Agility vereinbar sein.

## Abhängigkeiten

- `NPSPEC-CRYPTO-ARCH-0001`
- `NPSPEC-CRYPTO-AGILITY-0001`
- `NPSPEC-CRYPTO-KEYSTORE-0001`
- `NPSPEC-CRYPTO-KEYROTATION-0001`
- `NPSPEC-SECURITY-CODESIGNING-0001`
- `NPSPEC-DISTCOMM-TLS-0001`
- `NPSPEC-DISTCOMM-NEGOTIATION-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-CRYPTO-0010`

## Ergebnis

```text
Classical Crypto
       ↓
Hybrid Migration
       ↓
Post-Quantum Crypto
       ↓
Future Algorithm Replacement
```

NovaOS erhält damit eine post-quantenfähige Kryptografiearchitektur, ohne das System dauerhaft an heutige PQ-Algorithmen zu binden.