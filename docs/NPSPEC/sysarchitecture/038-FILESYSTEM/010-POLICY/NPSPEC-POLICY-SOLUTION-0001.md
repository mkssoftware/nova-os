# NPSPEC-POLICY-SOLUTION-0001 – Nova Solution Policy

## Status

Angenommen

## Kategorie

Policy / Solution

## Zweck

NovaOS definiert die Policy für Ausführung, Berechtigungen und sicherheitsrelevante Änderungen von Solutions.

Eine Solution darf ausschließlich die Capabilities verwenden, die durch ihren Logic Graph explizit eingebunden und für den aktuellen Kontext autorisiert wurden.

## Grundprinzipien

```text
Solution ≠ Program
SolutionID ≠ Authority
Trust ≠ Permission
Declared Capability ≠ Granted Capability
Custom Script ≠ Authority Source
User Approval ≠ Universal Override
```

## Policy-Kontext

Eine Entscheidung kann berücksichtigen:

```text
SolutionID / GUID
Solution Version
Integrity State
Trust State
Logic Graph
Capability Requirements
Custom NovaLang Code
User Identity
Workspace
Execution Context
Stored Permission Decisions
System Policy
```

## Entscheidungsmodell

```text
Solution Execution
       ↓
Identity / Integrity
       ↓
Trust Evaluation
       ↓
Logic Graph Analysis
       ↓
Required Capabilities
       ↓
Stored Permissions
       ↓
Solution Policy
       ↓
Allow / Restrict / Ask / Deny
```

## Capability-Modell

Capabilities müssen explizit Bestandteil des Logic Graph sein.

Beispiel:

```text
Network Capability
        ↓
Custom Script
        ↓
Storage Capability
```

Das Custom Script erhält ausschließlich die Daten und Authority, die durch den definierten Graph-Kontext bereitgestellt werden.

Ein Script darf keine zusätzliche System-Capability selbst anfordern oder erzeugen.

## Persistente Berechtigungen

Berechtigungsentscheidungen dürfen an die stabile Solution-Identität gebunden werden:

```text
SolutionID
+
Integrity State
+
Capability Requirements
+
Permission Decision
```

Dadurch muss eine unveränderte Solution nicht bei jedem Start erneut nach denselben Berechtigungen fragen.

## Änderungen

Sicherheitsrelevante Änderungen müssen erkannt werden.

Dazu gehören insbesondere:

```text
Neue Capability
Erweiterte Capability
Logic-Graph-Änderung
Custom-Code-Änderung
Geänderter Datenzugriff
Geänderter Netzwerkzugriff
Geänderter Gerätezugriff
```

Eine solche Änderung kann bestehende Berechtigungsentscheidungen ungültig machen oder eine erneute Bestätigung verlangen.

## Policy-Ergebnisse

```text
Allow
AllowRestricted
AskUser
Deny
```

Eine Einschränkung darf einzelne Capabilities oder deren Scope reduzieren, ohne die gesamte Solution blockieren zu müssen.

## Workspace

Wird eine Solution innerhalb eines Workspace ausgeführt, darf sie nur Authority verwenden, die sowohl für die Solution als auch für den Workspace-Kontext zulässig ist.

```text
Solution Authority
       ∩
Workspace Authority
       =
Effective Authority
```

Der Workspace erweitert die Authority einer Solution nicht automatisch.

## Normative Anforderungen

1. Solutions MÜSSEN über eine stabile `SolutionID` beziehungsweise GUID identifizierbar sein.
2. Eine SolutionID DARF keine Authority erzeugen.
3. Verwendete System-Capabilities MÜSSEN explizit im Logic Graph erkennbar sein.
4. Custom-NovaLang-Code DARF keine zusätzliche System-Authority selbst erzeugen oder anfordern.
5. Capability-Anforderungen MÜSSEN policy-basiert bewertet werden.
6. Persistente Berechtigungen MÜSSEN an die validierte Solution-Identität gebunden sein.
7. Sicherheitsrelevante Änderungen MÜSSEN eine Neubewertung bestehender Berechtigungen auslösen können.
8. Trust und Permission MÜSSEN getrennt bleiben.
9. Solutions MÜSSEN mit eingeschränkter Authority ausführbar sein können.
10. Workspace-Kontexte DÜRFEN Solution-Authority nicht implizit erweitern.
11. Höhere Safety- und Security-Regeln DÜRFEN nicht durch Solution- oder Benutzerentscheidungen überschrieben werden.
12. Policy-Entscheidung, effektive Capabilities und Entscheidungsgrund MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-POLICY-0001`
- `NPSPEC-POLICY-CAPABILITY-0001`
- `NPSPEC-TRUST-SOLUTION-0001`
- `NPSPEC-WORKSPACE-SOLUTION-0001`
- `NPSPEC-WORKSPACE-CAPABILITY-0001`
- `NPSPEC-WORKSPACE-PERMISSION-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS behandelt Solutions als deklarativ kontrollierte Ausführungseinheiten. Ihre effektive Authority entsteht ausschließlich aus explizit eingebundenen, autorisierten Capabilities und dem aktuellen Ausführungskontext, während Custom Code, Trust oder die stabile Solution-Identität selbst keine zusätzlichen Systemrechte erzeugen können.