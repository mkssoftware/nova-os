# NPSPEC-COMPAT-0007 – Compatibility Trust & Security

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie Trust- und Sicherheitsanforderungen bei Kompatibilitätsentscheidungen berücksichtigt werden.

Ziel ist, technisch kompatible, aber sicherheitsseitig unzulässige Komponenten oder Pfade auszuschließen.

## Grundprinzip

```text
Compatibility Path
    +
Trust
    +
Security Policy
    ↓
Security Validation
    ↓
TRUSTED / RESTRICTED / BLOCKED
```

## Trust-Anforderungen

Komponenten und Transformationen dürfen Trust-Anforderungen besitzen.

Beispiel:

```text
required_trust:
    verified
```

Eine Komponente mit geringerem Trust-Level darf diesen Pfad nicht erfüllen.

## Sicherheitskontext

Die Bewertung kann unter anderem berücksichtigen:

```text
identity
signature
origin
permissions
information_labels
execution_context
```

Kompatibilität hängt damit nicht ausschließlich von Typen und Versionen ab.

## Adapter und Transformationen

Auch Adapter und Transformationen müssen sicherheitsgeprüft werden.

```text
Trusted Source
    ↓
Untrusted Adapter
    ↓
Target
```

Ein solcher Pfad darf nicht automatisch als vertrauenswürdig gelten.

## Trust-Vererbung

Ein Kompatibilitätspfad kann höchstens den Trust garantieren, den seine schwächste relevante Komponente zulässt.

Eine nachfolgende vertrauenswürdige Komponente darf einen unsicheren vorherigen Schritt nicht automatisch aufwerten.

## Security Constraints

Sicherheitsrichtlinien dürfen Kompatibilitätspfade vollständig ausschließen.

Beispiele:

```text
untrusted_code:
    denied

remote_adapter:
    denied

unsigned_migration:
    denied
```

## Ergebnis

Mindestens folgende Ergebnisse müssen unterscheidbar sein:

```text
TRUSTED
RESTRICTED
BLOCKED
UNKNOWN
```

`UNKNOWN` darf nicht automatisch wie `TRUSTED` behandelt werden.

## Beispiel

```text
Source:
    Document.Schema@1

Adapter:
    third_party
    trust: untrusted

Target:
    Document.Schema@2

Policy:
    required_trust: verified
```

Ergebnis:

```text
BLOCKED
```

Obwohl der Pfad technisch kompatibel ist.

## Normative Anforderungen

1. Kompatibilitätsprüfung MUSS Trust- und Sicherheitsanforderungen berücksichtigen können.
2. Technische Kompatibilität DARF nicht automatisch als sicherheitsseitige Zulässigkeit gelten.
3. Adapter und Transformationen MÜSSEN in die Trust-Bewertung einbezogen werden.
4. Ein unsicherer Zwischenschritt DARF nicht durch spätere Komponenten automatisch aufgewertet werden.
5. Security Policies MÜSSEN Kompatibilitätspfade blockieren können.
6. NovaOS MUSS mindestens `TRUSTED`, `RESTRICTED`, `BLOCKED` und `UNKNOWN` unterscheiden können.
7. `UNKNOWN` DARF nicht automatisch als vertrauenswürdig behandelt werden.

## Abgrenzung

Diese NPSPEC definiert:

- Trust in Kompatibilitätspfaden
- Security Constraints
- Trust-Bewertung von Adaptern
- sicherheitsabhängige Pfadfreigabe

Nicht Bestandteil sind:

- allgemeines Identitätsmanagement
- Signaturverfahren
- Information-Flow-Policies
- allgemeine Sandbox-Mechanismen

## Zugehörige NPSPECs

- `NPSPEC-COMPAT-0001 – Compatibility Descriptor`
- `NPSPEC-COMPAT-0003 – Compatibility Graph Model`
- `NPSPEC-COMPAT-0004 – Adapter & Transformation Nodes`
- `NPSPEC-COMPAT-0005 – Compatibility Path Resolution`
- `NPSPEC-COMPAT-0006 – Compatibility Validation`
- `NPSPEC-INFOFLOW-0005 – Information Flow Policy`
- `NPSPEC-EVIDENCE-0004 – Evidence Integrity & Signing`