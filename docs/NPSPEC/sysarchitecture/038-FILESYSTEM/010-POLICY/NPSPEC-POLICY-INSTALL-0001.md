# NPSPEC-POLICY-INSTALL-0001 – Nova Install Policy

## Status

Angenommen

## Kategorie

Policy / Installation

## Zweck

NovaOS definiert die Policy-Regeln für die Installation von Programmen, Solutions, Systemkomponenten und anderen installierbaren Paketen.

Die Install Policy entscheidet, ob und unter welchen Bedingungen eine Installation zulässig ist. Sie ersetzt weder Trust-Prüfung noch Capability-Authority.

## Grundprinzipien

```text
Install Policy ≠ Trust
Install Policy ≠ Permission
Install Authority ≠ Runtime Authority
Allowed Installation ≠ Trusted Software
Installed ≠ Executable
Installed ≠ Authorized
```

## Policy-Kontext

Eine Installationsentscheidung kann berücksichtigen:

```text
Package Identity
Package Type
Publisher Identity
Install Scope
Trust State
Integrity State
Signature
Provenance
Compatibility
Requested Capabilities
Dependencies
System State
User Policy
System Policy
```

## Installationsbereiche

Policies können unterschiedliche Regeln für Installationsbereiche definieren:

```text
User
System
```

Eine Systeminstallation darf strengere Anforderungen besitzen als eine Benutzerinstallation.

## Entscheidungsmodell

```text
Install Request
      ↓
Installation Authority
      ↓
Integrity / Trust
      ↓
Compatibility
      ↓
Install Policy
      ↓
Allow / Restrict / Deny
```

Optional kann eine explizite Benutzerbestätigung verlangt werden.

## Policy-Ergebnisse

```text
Allow
AllowWithConfirmation
AllowRestricted
Deny
```

`AllowRestricted` kann beispielsweise zusätzliche Isolation oder eingeschränkte Integrationsmöglichkeiten erzwingen.

## Capability-Anforderungen

Angeforderte Runtime-Capabilities dürfen bei der Installationsentscheidung berücksichtigt werden.

```text
Package
   ↓
Requested Capabilities
   ↓
Policy Evaluation
```

Die Installation darf diese Capabilities jedoch nicht automatisch gewähren.

Runtime-Berechtigungen werden separat entschieden.

## Quellen und Herkunft

Install Policies dürfen Regeln für Paketquellen definieren:

```text
Trusted Repository
Verified Publisher
Local Package
Enterprise Source
Unknown Source
```

Eine erlaubte Quelle macht ein einzelnes Paket nicht automatisch vertrauenswürdig.

## Systemkomponenten

Für Kernel, Treiber, Services, Runtimes und andere kritische Systemkomponenten dürfen strengere Regeln gelten:

```text
Required Trust Level
Required Signature
Required Provenance
Required Verification
Required Recovery Path
```

## Policy-Priorität

Bei Konflikten gilt die NovaOS Policy-Priorität:

```text
Safety
  ↓
Security
  ↓
Sovereignty / Trust
  ↓
Hard System Constraints
  ↓
Explicit User Decisions
  ↓
Soft Preferences
  ↓
Adaptive Optimization
```

Eine Benutzerentscheidung darf höhere Sicherheits- oder Systemgrenzen nicht automatisch überschreiben.

## Normative Anforderungen

1. NovaOS MUSS Installationsentscheidungen policy-basiert steuern können.
2. Install Authority und Runtime Authority MÜSSEN getrennt bleiben.
3. Install Scope MUSS bei der Policy-Auswertung berücksichtigt werden können.
4. Trust, Integrität und Herkunft MÜSSEN als Policy-Eingaben verwendbar sein.
5. Capability-Anforderungen DÜRFEN die Installationsentscheidung beeinflussen.
6. Installation DARF angeforderte Runtime-Capabilities nicht automatisch gewähren.
7. Systemkomponenten DÜRFEN strengeren Installationsregeln unterliegen.
8. Unbekannte oder nicht vertrauenswürdige Quellen MÜSSEN einschränkbar oder blockierbar sein.
9. Policy-Ergebnisse MÜSSEN mindestens Zulassen, Einschränken und Ablehnen unterstützen.
10. Höhere Safety- und Security-Regeln DÜRFEN nicht durch niedrigere Policy-Ebenen überschrieben werden.
11. Policy-Änderungen DÜRFEN bereits erteilte Authority nicht implizit erweitern.
12. Installationsentscheidung und Entscheidungsgrund MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-POLICY-0001`
- `NPSPEC-PROGRAM-INSTALL-0001`
- `NPSPEC-PROGRAM-INSTALLSCOPE-0001`
- `NPSPEC-SYSTEM-TRUST-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-TRUST-PROGRAM-0001`
- `NPSPEC-TRUST-SOLUTION-0001`
- `NPSPEC-TRUST-SYSTEMCOMPONENT-0001`

## Ergebnis

NovaOS besitzt eine zentrale Policy-Schicht für Installationsentscheidungen. Pakete werden anhand von Scope, Trust, Integrität, Herkunft, Kompatibilität und Sicherheitsanforderungen bewertet, während Installation und spätere Runtime-Authority strikt getrennt bleiben.