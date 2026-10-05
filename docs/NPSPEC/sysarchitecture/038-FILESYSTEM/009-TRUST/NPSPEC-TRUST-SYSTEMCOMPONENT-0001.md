# NPSPEC-TRUST-SYSTEMCOMPONENT-0001 – Nova System Component Trust

## Status

Angenommen

## Kategorie

Trust / System Component

## Zweck

NovaOS definiert die Vertrauensbewertung systemkritischer Komponenten.

Dazu gehören insbesondere Kernel-Komponenten, Treiber, Services, Module, Libraries, Runtimes, Framework-Provider und weitere Bestandteile von `/System`.

## Grundprinzipien

```text
Trust ≠ Authority
Trust ≠ Integrity
Signature ≠ Trust
Installed ≠ Trusted
System Component ≠ Automatically Trusted
Trusted ≠ Unrestricted
Verified ≠ Trusted
```

## Trust-Modell

Eine Systemkomponente wird mindestens anhand folgender Eigenschaften bewertbar:

```text
ComponentID
Version
Publisher / Provider Identity
Integrity
Signature
Provenance
Trust Chain
Verification State
Compatibility
Revocation State
```

Die stabile `ComponentID` bleibt von Pfad, Prozess, Datei und konkreter Laufzeitinstanz getrennt.

## Betroffene Komponenten

Das Modell gilt insbesondere für:

```text
Kernel Components
HAL Components
Drivers
Services
Modules
Libraries
Runtimes
Framework Providers
Security Components
Boot-related System Components
```

## Trust-Zustände

```text
Trusted
Restricted
Untrusted
Unknown
Invalid
Revoked
```

Je kritischer eine Komponente ist, desto strengere Trust-Anforderungen dürfen gelten.

## Prüfung

```text
System Component
       ↓
Identity
       ↓
Integrity
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

Eine gültige Signatur oder erfolgreiche Integritätsprüfung allein erzeugt nicht automatisch `Trusted`.

## Verified Core

Für sicherheitskritische Kernkomponenten darf NovaOS zusätzliche formale Verifikation verlangen.

Dies betrifft insbesondere:

```text
Memory Isolation
Capability Enforcement
IPC
Critical Kernel State
Security Boundaries
```

Formale Verifikation ergänzt Trust und Integrität, ersetzt diese jedoch nicht.

## Laufzeitprüfung

Trust darf nicht ausschließlich beim Installieren geprüft werden.

NovaOS kann Komponenten prüfen:

```text
At Boot
At Load
At Activation
After Update
After Recovery
After Modification
During Runtime Verification
```

## Änderungen

Wird eine sicherheitsrelevante Komponente verändert:

```text
Known Component
      ↓
Modification
      ↓
Integrity Changed
      ↓
Trust Invalidated
      ↓
Reverification
```

Ein zuvor gültiger Trust-Zustand darf nicht ungeprüft übernommen werden.

## Isolation

Nicht oder nur eingeschränkt vertrauenswürdige Komponenten dürfen abhängig von ihrer Funktion isoliert ausgeführt werden.

Insbesondere Treiber und Provider sollen, soweit technisch möglich, außerhalb kritischer Kernelbereiche isoliert werden.

## Revocation

Komponenten, Publisher, Zertifikate oder Trust-Beziehungen müssen widerrufbar sein.

NovaOS kann darauf reagieren durch:

```text
Block
Restrict
Isolate
Replace
Rollback
Recovery
```

## Normative Anforderungen

1. Systemkomponenten MÜSSEN unabhängig von ihrer Installation auf Trust bewertbar sein.
2. Komponenten MÜSSEN über stabile Identitäten verfügen.
3. Integrität, Signatur, Provenance und Trust Chain MÜSSEN prüfbar sein.
4. Eine gültige Signatur DARF nicht automatisch `Trusted` bedeuten.
5. `Unknown` DARF bei kritischen Komponenten nicht automatisch als vertrauenswürdig gelten.
6. Kritische Komponenten DÜRFEN zusätzliche Verifikationsanforderungen besitzen.
7. Sicherheitsrelevante Änderungen MÜSSEN eine erneute Trust-Bewertung auslösen.
8. Nicht vollständig vertrauenswürdige Komponenten SOLLEN soweit möglich isoliert werden.
9. Revocation MUSS unterstützt werden.
10. Trust DARF keine zusätzliche Authority erzeugen.
11. Update und Recovery MÜSSEN den Trust-Zustand neu bewerten können.
12. Trust-, Integritäts-, Verifikations- und Revocation-Zustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-TRUST-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-SYSTEM-PROTECTION-0001`
- `NPSPEC-SYSTEM-REGISTRY-0001`
- `NPSPEC-SYSTEM-UPDATES-0001`
- `NPSPEC-SYSTEM-RECOVERY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS behandelt Systemkomponenten nicht allein aufgrund ihrer Systemzugehörigkeit als vertrauenswürdig. Identität, Integrität, Herkunft, Trust Chain, Verifikation und Revocation bestimmen ihren Trust-Zustand, während Isolation, Update, Rollback und Recovery kontrolliert auf Trust-Veränderungen reagieren können.