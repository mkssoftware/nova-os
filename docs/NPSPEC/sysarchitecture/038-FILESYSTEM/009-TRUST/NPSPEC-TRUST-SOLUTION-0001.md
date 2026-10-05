# NPSPEC-TRUST-SOLUTION-0001 – Nova Solution Trust

## Status

Angenommen

## Kategorie

Trust / Solution

## Zweck

NovaOS definiert die Vertrauensbewertung von Solutions.

Eine Solution besteht aus deklarativer UI, Logic Graph, Capabilities, Custom-NovaLang-Code und weiteren Solution-Artefakten. Trust stellt sicher, dass Identität, Integrität und sicherheitsrelevante Änderungen der gesamten Solution nachvollziehbar bewertet werden können.

## Grundprinzipien

```text
Trust ≠ Permission
SolutionID ≠ Solution Content
GUID ≠ Trust
Valid Signature ≠ Trust
Trusted Solution ≠ Trusted Capability Provider
Trusted Solution ≠ Unrestricted Authority
```

## Solution-Identität

Jede Solution besitzt eine dauerhaft eindeutige Identität:

```text
SolutionID / GUID
```

Diese Identität bleibt über normale Änderungen und Updates erhalten.

Die GUID allein beweist jedoch weder Integrität noch Herkunft.

## Trust-Modell

Die Bewertung kann mindestens berücksichtigen:

```text
SolutionID
Publisher Identity
Manifest
UI Definition
Logic Graph
Custom NovaLang Code
Capability Requirements
Integrity
Signature
Provenance
Trust Chain
Version
Revocation State
```

Alle sicherheitsrelevanten Bestandteile müssen in die Integritätsbewertung einbezogen werden.

## Prüfung

```text
Solution
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
Revocation
   ↓
Trust State
```

Mögliche Zustände:

```text
Trusted
Restricted
Untrusted
Unknown
Invalid
Revoked
```

## Änderungen

NovaOS muss Änderungen an einer bereits bekannten Solution erkennen können.

```text
Known Solution
      ↓
Modification
      ↓
Integrity Changed
      ↓
Trust Re-evaluation
```

Besonders relevant sind Änderungen an:

```text
Logic Graph
Capabilities
Custom Code
Data Access
Network Access
Device Access
Security-Relevant UI Flows
```

## Capability-Bezug

Eine Solution erhält Authority ausschließlich über ihre tatsächlich verwendeten und autorisierten Capabilities.

```text
Solution Trust
      ≠
Capability Authority
```

Custom-NovaLang-Code kann keine zusätzlichen Systemrechte selbst erzeugen oder anfordern.

Eine vertrauenswürdige Solution darf daher ausschließlich mit den Capabilities arbeiten, die ihr explizit zur Verfügung gestellt wurden.

## Berechtigungsbindung

Persistente Berechtigungsentscheidungen dürfen an die stabile Solution-Identität gebunden werden.

Sicherheitsrelevante Änderungen müssen jedoch eine erneute Prüfung auslösen können.

```text
SolutionID
+
Integrity State
+
Capability Requirements
→ Permission Re-evaluation
```

Damit verhindert die stabile GUID, dass bei unveränderter Solution unnötig erneut gefragt wird, ohne veränderten Code automatisch zu legitimieren.

## Revocation

Publisher, Signaturen, Solution-Versionen oder andere Trust-Beziehungen müssen widerrufbar sein.

Revocation kann abhängig von Policy zu Einschränkung, Blockierung oder erneuter Verifikation führen.

## Normative Anforderungen

1. Jede Solution MUSS eine stabile eindeutige Identität besitzen.
2. Eine GUID allein DARF nicht als Integritäts- oder Trust-Nachweis gelten.
3. Sicherheitsrelevante Solution-Artefakte MÜSSEN auf Integrität prüfbar sein.
4. Änderungen am Logic Graph und an Capability-Anforderungen MÜSSEN erkannt werden können.
5. Sicherheitsrelevante Änderungen MÜSSEN eine erneute Trust- oder Permission-Prüfung auslösen können.
6. Eine gültige Signatur DARF nicht automatisch `Trusted` bedeuten.
7. Provenance und Trust Chain MÜSSEN berücksichtigt werden können.
8. `Unknown` DARF nicht automatisch als `Trusted` behandelt werden.
9. Solution Trust DARF keine Capability-Authority erzeugen.
10. Custom Code DARF keine Authority außerhalb des Capability-Modells erzeugen.
11. Revocation MUSS unterstützt werden.
12. Trust-Zustand und Entscheidungsgrund MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-WORKSPACE-SOLUTION-0001`
- `NPSPEC-WORKSPACE-CAPABILITY-0001`
- `NPSPEC-TRUST-CAPABILITY-0001`
- `NPSPEC-SYSTEM-TRUST-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-SYSTEM-POLICY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann Solutions dauerhaft identifizieren und gleichzeitig jede sicherheitsrelevante Änderung an ihren Bestandteilen erkennen. Trust, Integrität und Berechtigungen bleiben getrennt, sodass eine stabile Solution-Identität persistente Berechtigungen ermöglicht, ohne veränderten Code oder neue Capabilities automatisch zu legitimieren.