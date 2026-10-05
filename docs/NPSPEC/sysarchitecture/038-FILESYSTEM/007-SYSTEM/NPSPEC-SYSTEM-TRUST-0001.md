# NPSPEC-SYSTEM-TRUST-0001 – Nova System Trust

## Status

Angenommen

## Kategorie

System / Trust

## Zweck

NovaOS definiert das systemweite Trust-Modell zur Bewertung von Identität, Integrität, Herkunft und Vertrauenswürdigkeit von Systemkomponenten.

Trust unterstützt Sicherheitsentscheidungen, erzeugt jedoch niemals selbst Authority.

## Grundprinzipien

```text
Trust ≠ Authority
Trust ≠ Permission
Trust ≠ Identity
Signature ≠ Trust
Integrity ≠ Trust
Unknown ≠ Trusted
```

## Trust-Modell

Eine Trust-Bewertung kann berücksichtigen:

```text
Identity
Integrity
Signature
Provenance
Trust Chain
Attestation
Revocation State
Policy
```

Das Ergebnis wird als expliziter Trust State bereitgestellt.

## Trust States

Mindestens folgende Zustände werden unterstützt:

```text
Trusted
Restricted
Untrusted
Unknown
Invalid
Revoked
```

`Unknown` darf nicht automatisch als `Trusted` behandelt werden.

## Vertrauenskette

```text
Trust Anchor
     ↓
Identity / Signature
     ↓
Provenance
     ↓
Component
     ↓
Trust Evaluation
     ↓
Policy Decision
```

Vertrauensanker müssen explizit definiert und verwaltbar sein.

## Systemkomponenten

Trust kann insbesondere bewertet werden für:

```text
Kernel
Drivers
System Modules
System Services
Libraries
Runtimes
Framework Provider
Boot Components
Programs
```

Die notwendige Vertrauensstufe kann abhängig von der Sicherheitsrelevanz unterschiedlich sein.

## Boot-Vertrauenskette

NovaOS integriert Trust mit:

```text
Secure Boot
Measured Boot
Verified Boot
Runtime Verification
```

Dadurch kann eine Vertrauenskette vom Systemstart bis zu laufenden Komponenten aufgebaut werden.

Ein erfolgreicher Boot-Trust ersetzt jedoch keine Runtime-Prüfungen.

## Provenance

NovaOS soll die Herkunft sicherheitsrelevanter Komponenten nachvollziehen können.

Dazu können gehören:

```text
Publisher
Repository
Build
Package
Version
Signature
Update Source
```

Fehlende Provenance bleibt ausdrücklich als unbekannt erkennbar.

## Widerruf

Vertrauen muss widerrufbar sein.

```text
Trusted
   ↓
Revocation
   ↓
Revoked
```

Widerrufene Identitäten, Schlüssel oder Komponenten dürfen nicht aufgrund früherer Bewertungen weiterhin als vertrauenswürdig gelten.

## Trust und Security

Trust liefert Informationen an die Security Policy:

```text
Trust State
     ↓
Security Policy
     ↓
Allow / Restrict / Isolate / Deny
```

Die daraus resultierende Authority wird weiterhin ausschließlich durch das Capability- und Permission-Modell bestimmt.

## Introspection

NovaOS muss Trust-Entscheidungen nachvollziehbar darstellen können:

```text
Component Identity
Trust State
Trust Source
Signature State
Provenance
Revocation State
Evaluation Reason
```

## Normative Anforderungen

1. NovaOS MUSS ein systemweites Trust-Modell bereitstellen.
2. Trust MUSS von Identity, Permission und Authority getrennt bleiben.
3. `Unknown` DARF nicht automatisch als `Trusted` behandelt werden.
4. Kryptografische Signaturen MÜSSEN verifizierbar sein.
5. Eine gültige Signatur DARF allein keinen Trust garantieren.
6. Vertrauensanker MÜSSEN explizit verwaltbar sein.
7. Sicherheitskritische Systemkomponenten MÜSSEN auf Integrität und Trust prüfbar sein.
8. Provenance SOLL für sicherheitsrelevante Komponenten nachvollziehbar sein.
9. Trust MÜSSEN widerrufbar sein.
10. Boot- und Runtime-Trust MÜSSEN miteinander verknüpfbar sein.
11. Trust-Entscheidungen MÜSSEN introspektierbar sein.
12. Trust DARF keine zusätzliche Authority erzeugen.

## Abhängigkeiten

- `NPSPEC-SYSTEM-BOOT-0001`
- `NPSPEC-SYSTEM-KERNEL-0001`
- `NPSPEC-SYSTEM-DRIVERS-0001`
- `NPSPEC-SYSTEM-MODULES-0001`
- `NPSPEC-SYSTEM-SERVICES-0001`
- `NPSPEC-SYSTEM-LIBRARIES-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-PROGRAM-TRUST-0001`

## Ergebnis

NovaOS besitzt ein durchgängiges Trust-Modell vom Bootprozess bis zu laufenden Systemkomponenten. Identität, Signatur, Integrität, Provenance und Widerruf fließen in nachvollziehbare Trust-Entscheidungen ein, ohne Trust mit Berechtigungen oder Authority gleichzusetzen.