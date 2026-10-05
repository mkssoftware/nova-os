# NPSPEC-TRUST-CAPABILITY-0001 – Nova Capability Provider Trust Policy

## Status

Angenommen

## Kategorie

Trust / Capability

## Zweck

NovaOS definiert die Vertrauensbewertung von Capability-Providern und deren Implementierungen.

Capability Trust stellt sicher, dass eine angeforderte Capability nur durch Provider bereitgestellt wird, deren Identität, Integrität, Herkunft und Trust-Zustand den jeweiligen Anforderungen entsprechen.

## Grundprinzipien

```text
Trust ≠ Authority
Trust ≠ Capability Token
CapabilityID ≠ ProviderID
Trusted Provider ≠ Permission
Discovery ≠ Trust
Signature ≠ Trust
```

## Trust-Modell

Die Bewertung kann mindestens berücksichtigen:

```text
CapabilityID
ProviderID
Provider Identity
Implementation Identity
Integrity
Signature
Provenance
Trust Chain
Version
Revocation State
```

Die Capability selbst und der konkrete Provider bleiben getrennte Identitäten.

## Trust-Zustände

```text
Trusted
Restricted
Untrusted
Unknown
Invalid
Revoked
```

`Unknown` darf bei sicherheitskritischen Capabilities nicht automatisch als vertrauenswürdig behandelt werden.

## Provider-Prüfung

```text
Capability Requirement
        ↓
Capability Registry
        ↓
Candidate Provider
        ↓
Identity / Integrity
        ↓
Trust / Provenance
        ↓
Revocation
        ↓
Trust State
```

Erst danach kann der Provider in die weitere Policy- und Ausführungsentscheidung einbezogen werden.

## Trust-Anforderungen

Capabilities dürfen Mindestanforderungen an ihren Provider definieren.

Beispiele:

```text
Minimum Trust Level
Required Provenance
Required Signature
Required Isolation
Verified Implementation
Allowed Provider
Allowed Trust Anchor
```

Besonders kritische Capabilities können strengere Anforderungen besitzen.

## Providerwechsel

Mehrere Provider dürfen dieselbe `CapabilityID` implementieren.

```text
CapabilityID
├── Provider A → Trusted
├── Provider B → Restricted
└── Provider C → Revoked
```

Ein Providerwechsel muss erneut gegen die Trust-Anforderungen geprüft werden.

## Verified Provider

Für sicherheitskritische Capabilities darf NovaOS formal verifizierte oder besonders geprüfte Implementierungen verlangen.

```text
Trust
+
Verification
+
Policy
→ Provider Eligibility
```

Verifikation ersetzt dabei weder Trust noch Capability-Authority.

## Revocation

Wird ein Provider, Zertifikat, Paket oder eine Trust-Beziehung widerrufen, muss dessen Eignung für zukünftige Capability-Ausführungen neu bewertet werden.

Aktive Authority wird entsprechend der Capability- und Revocation-Policy separat behandelt.

## Sicherheit

Capability Trust entscheidet, ob eine Implementierung grundsätzlich verwendet werden darf.

Die tatsächliche Nutzung benötigt weiterhin eine gültige Capability-Authority:

```text
Trusted Provider
      +
Capability Token / Handle
      +
Policy
      ↓
Authorized Execution
```

## Normative Anforderungen

1. NovaOS MUSS Capability Trust getrennt von Capability Authority behandeln.
2. Capability und Provider MÜSSEN getrennte Identitäten besitzen.
3. Provider MÜSSEN auf Identität und Integrität prüfbar sein.
4. Provenance und Trust Chain MÜSSEN berücksichtigt werden können.
5. Eine gültige Signatur DARF nicht automatisch `Trusted` bedeuten.
6. Capability-Anforderungen DÜRFEN einen Mindest-Trust definieren.
7. Sicherheitskritische Capabilities DÜRFEN verifizierte Provider verlangen.
8. Ein Providerwechsel MUSS eine erneute Trust-Bewertung ermöglichen.
9. `Unknown` DARF bei kritischen Capabilities nicht automatisch als vertrauenswürdig gelten.
10. Revocation MUSS unterstützt werden.
11. Trust DARF keine Capability-Authority erzeugen.
12. Provider-Trust und Entscheidungsgrund MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-REGISTRY-CAPABILITY-0001`
- `NPSPEC-SYSTEM-TRUST-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-SYSTEM-POLICY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS bewertet Capability-Provider unabhängig von der eigentlichen Capability-Authority. Dadurch können mehrere Implementierungen derselben Capability sicher gegeneinander bewertet, ungeeignete oder widerrufene Provider ausgeschlossen und für kritische Funktionen gezielt besonders vertrauenswürdige oder verifizierte Implementierungen verlangt werden.
