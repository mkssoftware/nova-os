# NPSPEC-PROGRAM-TRUST-0001 – Nova Program Package Trust Integration

## Status

Angenommen

## Kategorie

Program / Security / Trust

## Zweck

NovaOS definiert die Bewertung der Vertrauenswürdigkeit klassischer Programme und ihrer Program Packages.

Trust beschreibt, ob Identität, Herkunft, Integrität und Vertrauenskette eines Programms ausreichend nachgewiesen werden können.

## Grundprinzipien

```text
Trust ≠ Permission
Trust ≠ Authority
Signature ≠ Trust
Installed ≠ Trusted
Trusted ≠ Safe
Unknown ≠ Trusted
```

Trust und Berechtigungen bleiben getrennte Sicherheitsmechanismen.

## Trust-Modell

NovaOS bewertet mindestens:

```text
ProgramID
Publisher Identity
Package Integrity
Signature
Provenance
Trust Chain
Package State
```

Das Ergebnis wird als nachvollziehbarer Trust State bereitgestellt.

## Trust States

Mindestens folgende Zustände werden unterstützt:

```text
Trusted
Untrusted
Unknown
Invalid
Revoked
```

`Unknown` darf niemals automatisch als `Trusted` behandelt werden.

## Signaturen

Program Packages können kryptografisch signiert werden.

```text
Program Package
      ↓
Signature Verification
      ↓
Publisher Identity
      ↓
Trust Evaluation
```

Eine gültige Signatur beweist die Bindung an einen Schlüssel beziehungsweise eine Identität, erzeugt aber allein noch kein Vertrauen.

## Integrität

Das vollständige sicherheitsrelevante Program Package muss in die Integritätsprüfung einbezogen werden:

```text
App/
Resources/
SYS/
Manifest
```

Manipulationen nach der Signierung müssen erkennbar sein.

## Provenance

NovaOS soll die Herkunft eines Program Packages nachvollziehen können.

Dazu können gehören:

```text
Publisher
Repository
Package Source
Build Information
Signature Chain
Version
```

Fehlende Provenance muss als solche erkennbar bleiben.

## Installation und Update

Trust wird mindestens bei:

```text
Installation
Update
Rollback
Execution nach relevanter Änderung
```

erneut geprüft.

Ein Update darf den Trust State einer vorherigen Version nicht automatisch übernehmen.

## Widerruf

Publisher-, Zertifikats- oder Paketvertrauen muss widerrufbar sein.

```text
Trusted
   ↓
Revocation
   ↓
Revoked
```

Ein widerrufener Trust State muss bei zukünftigen Trust-Entscheidungen berücksichtigt werden.

## Berechtigungen

Trust erzeugt keine Capability.

```text
Trust Evaluation
      ≠
Permission Evaluation
```

Auch ein vertrauenswürdiges Programm erhält nur explizit autorisierte Runtime-Capabilities.

Umgekehrt darf Policy die Ausführung nicht vertrauenswürdiger Programme einschränken oder verhindern.

## Normative Anforderungen

1. NovaOS MUSS Program Trust unabhängig von Program Permissions bewerten.
2. Program Packages MÜSSEN auf Integrität prüfbar sein.
3. Kryptografische Signaturen MÜSSEN verifizierbar sein.
4. Eine gültige Signatur DARF nicht automatisch `Trusted` bedeuten.
5. `Unknown` DARF NICHT als `Trusted` behandelt werden.
6. Private `SYS`-Komponenten MÜSSEN in die Trust- und Integritätsbewertung einbezogen werden.
7. Sicherheitsrelevante Manifest-Änderungen MÜSSEN erkannt werden.
8. Updates und Rollbacks MÜSSEN ihre jeweilige Trust-Bewertung besitzen.
9. Trust MUSS widerrufbar sein.
10. Trust DARF keine Runtime-Authority erzeugen.

## Abhängigkeiten

- `NPSPEC-PROGRAM-PACKAGE-0001`
- `NPSPEC-PROGRAM-MANIFEST-0001`
- `NPSPEC-PROGRAM-SYS-0001`
- `NPSPEC-PROGRAM-INSTALL-0001`
- `NPSPEC-PROGRAM-UPDATE-0001`
- `NPSPEC-PROGRAM-ROLLBACK-0001`
- `NPSPEC-PROGRAM-PERMISSION-0001`

## Ergebnis

NovaOS erhält ein nachvollziehbares Trust-Modell für klassische Programme. Identität, Signatur, Integrität und Herkunft werden geprüft, ohne Vertrauen mit Berechtigungen gleichzusetzen oder einem vertrauenswürdigen Programm automatisch zusätzliche Authority zu gewähren.
