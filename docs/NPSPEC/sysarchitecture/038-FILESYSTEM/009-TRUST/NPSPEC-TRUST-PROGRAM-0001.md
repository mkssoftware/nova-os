# NPSPEC-TRUST-PROGRAM-0001 – Nova Program Trust

## Status

Angenommen

## Kategorie

Trust / Program

## Zweck

NovaOS definiert die Vertrauensbewertung klassischer Programme.

Program Trust bewertet Identität, Integrität, Herkunft und Vertrauenskette eines Programmpakets. Trust entscheidet jedoch nicht selbst über die Berechtigungen des Programms.

## Grundprinzipien

```text
Trust ≠ Permission
Trust ≠ Capability
Signature ≠ Trust
Installed ≠ Trusted
Publisher Identity ≠ Authority
Trusted Program ≠ Unrestricted Program
```

## Trust-Modell

Die Bewertung kann mindestens berücksichtigen:

```text
ProgramID
Publisher Identity
Package Integrity
Digital Signature
Provenance
Trust Chain
Package Version
Revocation State
Private Dependencies
```

Das gesamte sicherheitsrelevante Programmpaket einschließlich privater `SYS`-Abhängigkeiten muss berücksichtigt werden.

## Trust-Zustände

```text
Trusted
Restricted
Untrusted
Unknown
Invalid
Revoked
```

`Unknown` darf nicht automatisch als vertrauenswürdig behandelt werden.

## Prüfung

```text
Program Package
      ↓
Identity Check
      ↓
Integrity Check
      ↓
Signature Verification
      ↓
Provenance / Trust Chain
      ↓
Revocation Check
      ↓
Trust State
```

Eine technisch gültige Signatur beweist lediglich die Signaturbeziehung und erzeugt nicht automatisch den Zustand `Trusted`.

## Private Abhängigkeiten

Private Komponenten unter:

```text
/Apps/<Program>/SYS/
```

sind Bestandteil des Trust-Kontexts des Programms.

Manipulierte oder nicht vertrauenswürdige private Abhängigkeiten können den Trust-Zustand des gesamten Programms beeinflussen.

## Installation und Start

Trust kann zu mehreren Zeitpunkten geprüft werden:

```text
Package Acquisition
Installation
Update
Program Start
Runtime Verification
Suspicious Change
```

Installation allein erzeugt keinen positiven Trust-Zustand.

## Änderungen und Updates

Ändert sich sicherheitsrelevanter Programmcode oder eine private Abhängigkeit, muss der bisherige Trust-Zustand neu bewertet werden.

```text
Known Package
    ↓
Modification
    ↓
Trust Invalidated
    ↓
Reverification
```

## Trust und Berechtigungen

Trust beeinflusst Policy-Entscheidungen, ersetzt jedoch niemals Capability-Prüfungen.

```text
Trust State
    +
Security Policy
    +
Capability Authority
    ↓
Execution Decision
```

Auch ein vollständig vertrauenswürdiges Programm erhält nur explizit autorisierte Capabilities.

## Revocation

Wird ein Publisher, Zertifikat, Paket oder eine relevante Trust-Beziehung widerrufen, muss NovaOS den Program Trust neu bewerten können.

Abhängig von Policy kann das Programm blockiert, eingeschränkt oder isoliert werden.

## Normative Anforderungen

1. NovaOS MUSS Program Trust unabhängig von Program Permissions behandeln.
2. Programme MÜSSEN anhand ihrer stabilen `ProgramID` bewertbar sein.
3. Paketintegrität MUSS überprüfbar sein.
4. Digitale Signaturen MÜSSEN verifizierbar sein.
5. Eine gültige Signatur DARF nicht automatisch `Trusted` bedeuten.
6. Provenance und Trust Chain MÜSSEN berücksichtigt werden können.
7. Private `SYS`-Abhängigkeiten MÜSSEN in die Trust-Bewertung einbezogen werden.
8. `Unknown` DARF nicht automatisch als `Trusted` behandelt werden.
9. Sicherheitsrelevante Änderungen MÜSSEN eine erneute Trust-Prüfung auslösen können.
10. Revocation MUSS unterstützt werden.
11. Trust DARF keine Capability oder Runtime-Authority erzeugen.
12. Trust-Zustand und Entscheidungsgrund MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-PROGRAM-TRUST-0001`
- `NPSPEC-PROGRAM-PACKAGE-0001`
- `NPSPEC-PROGRAM-SYS-0001`
- `NPSPEC-PROGRAM-PERMISSION-0001`
- `NPSPEC-SYSTEM-TRUST-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-SYSTEM-POLICY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine einheitliche Vertrauensbewertung für klassische Programme und deren private Abhängigkeiten. Identität, Integrität, Signatur, Herkunft und Revocation bestimmen den Trust-Zustand, während tatsächliche Zugriffsrechte weiterhin ausschließlich über das Capability- und Sicherheitsmodell vergeben werden.