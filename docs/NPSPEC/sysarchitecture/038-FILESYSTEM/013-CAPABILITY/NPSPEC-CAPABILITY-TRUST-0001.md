# NPSPEC-CAPABILITY-TRUST-0001 – Nova Capability Trust Integration

## Status

Angenommen

## Kategorie

Capability / Trust

## Zweck

NovaOS definiert die Vertrauensbewertung von Capability-Implementierungen und ihren Providern.

Trust bestimmt, unter welchen Sicherheitsbedingungen eine Implementierung als Provider einer Capability verwendet werden darf. Die semantische `CapabilityID` selbst besitzt keine automatische Vertrauensstellung.

## Grundprinzipien

```text
CapabilityID ≠ Trust
Provider ≠ Capability
Trust ≠ Authority
Trust ≠ Permission
Signature ≠ Trust
Installed ≠ Trusted
Trusted Provider ≠ Unrestricted Provider
```

## Trust-Modell

Eine Capability-Implementierung kann anhand folgender Eigenschaften bewertet werden:

```text
CapabilityID
ProviderID
PackageID
Implementation Version
Integrity
Signature
Provenance
Trust Chain
Verification State
Revocation State
```

## Trust-Zustände

Mindestens folgende Zustände werden unterstützt:

```text
Trusted
Restricted
Untrusted
Unknown
Invalid
Revoked
```

Trust wird für eine konkrete Implementierung beziehungsweise ihren Provider bewertet, nicht pauschal für die semantische Capability.

## Provider-Auswahl

Mehrere Provider können dieselbe Capability implementieren:

```text
CapabilityID
├── Provider A → Trusted
├── Provider B → Restricted
└── Provider C → Unknown
```

Registry, Policy und Execution Contract dürfen den Trust-Zustand bei der Provider-Auswahl berücksichtigen.

Ein Execution Contract kann beispielsweise verlangen:

```text
RequiredTrustLevel: Trusted
```

Provider, die diese Anforderung nicht erfüllen, dürfen nicht ausgewählt werden.

## Trust-Prüfung

```text
Provider
   ↓
Identity
   ↓
Implementation Integrity
   ↓
Signature
   ↓
Provenance
   ↓
Trust Chain
   ↓
Verification
   ↓
Revocation
   ↓
Trust State
```

Eine gültige Signatur allein reicht nicht für den Zustand `Trusted`.

## Änderungen

Änderungen an einer Capability-Implementierung müssen erkannt werden können.

```text
Known Implementation
        ↓
Modification / Update
        ↓
Integrity Changed
        ↓
Trust Re-evaluation
```

Ein zuvor ermittelter Trust-Zustand darf bei sicherheitsrelevanten Änderungen nicht ungeprüft übernommen werden.

## Isolation

Provider mit reduziertem Trust dürfen abhängig von Policy stärker isoliert werden.

Beispiele:

```text
Separate Process
Restricted Namespace
Reduced Capabilities
Resource Limits
No Delegation
Local Execution Only
```

Eine Isolation ersetzt jedoch keine Trust-Prüfung.

## Authority

Trust erzeugt keine Capability-Authority.

```text
Trusted Provider
      ≠
Authorized Caller
```

Ebenso darf ein vertrauenswürdiger Provider ausschließlich die Authority verwenden, die ihm für seine konkrete Ausführung bereitgestellt wurde.

## Revocation

Provider, Packages, Signaturen oder Trust-Beziehungen müssen widerrufbar sein.

Eine Revocation kann zu:

```text
Block
Restrict
Provider Reselection
Isolation
Rollback
```

führen.

Bereits bestehende Ausführungen müssen gemäß Sicherheits- und Revocation-Policy behandelt werden.

## Normative Anforderungen

1. Capability Trust MUSS für konkrete Provider beziehungsweise Implementierungen bewertbar sein.
2. Eine `CapabilityID` DARF nicht automatisch einen Trust-Zustand besitzen.
3. Provider-, Package- und Capability-Identität MÜSSEN getrennt bleiben.
4. Integrität, Signatur, Provenance und Trust Chain MÜSSEN prüfbar sein.
5. Eine gültige Signatur DARF nicht automatisch `Trusted` bedeuten.
6. Mehrere Provider derselben Capability DÜRFEN unterschiedliche Trust-Zustände besitzen.
7. Provider-Auswahl MUSS Trust-Anforderungen berücksichtigen können.
8. Sicherheitsrelevante Implementierungsänderungen MÜSSEN eine erneute Trust-Bewertung auslösen können.
9. Eingeschränkt vertrauenswürdige Provider MÜSSEN isolierbar sein.
10. Trust DARF keine Capability-Authority oder Permission erzeugen.
11. Revocation MUSS unterstützt werden.
12. Execution Contracts MÜSSEN Mindestanforderungen an den Provider-Trust definieren können.
13. Trust-Zustand, Provider, Herkunft und Entscheidungsgrund MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-PACKAGE-0001`
- `NPSPEC-CAPABILITY-MANIFEST-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-FSCAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-REGISTRY-CAPABILITY-0001`
- `NPSPEC-POLICY-CAPABILITY-0001`
- `NPSPEC-SYSTEM-TRUST-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`

## Ergebnis

NovaOS bewertet Trust auf Ebene konkreter Capability-Provider und Implementierungen. Dadurch können mehrere Implementierungen derselben Capability mit unterschiedlichen Vertrauensstufen koexistieren und anhand von Policy und Execution Contract sicher ausgewählt werden, ohne Trust mit Capability-Identität, Permission oder Authority zu vermischen.
