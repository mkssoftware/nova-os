# ADR-IDENTITY-0008 – Delegated Context statt Impersonation

## Status

Angenommen

## Kategorie

Identity Architecture / Delegation / Security Context

## Kontext

Traditionelle Systeme erlauben Diensten oder Prozessen teilweise, vollständig als andere Identität aufzutreten.

Eine solche Impersonation verwischt jedoch:

- ursprüngliche Identity,
- ausführende Identity,
- delegierte Authority,
- Verantwortlichkeit,
- Audit-Kette.

NovaOS benötigt stattdessen ein Modell, bei dem eine Komponente im Auftrag einer anderen Identity handeln kann, ohne selbst diese Identity zu werden.

## Entscheidung

NovaOS verwendet **Delegated Context statt vollständiger Identity Impersonation**.

```text
Caller Identity
      ↓
Delegated Context
      ↓
Service Identity
      ↓
Operation
```

Die ausführende Identity bleibt erhalten.

Der Delegated Context beschreibt explizit, in wessen Auftrag und mit welcher delegierten Authority eine Operation ausgeführt wird.

## Grundprinzip

```text
Acting for ≠ Becoming
```

Ein Service, der für einen Benutzer handelt, wird nicht zum Benutzer.

Stattdessen bleiben mindestens folgende Informationen unterscheidbar:

```text
Original Identity
Acting Identity
Delegated Authority
Delegation Chain
Operation Context
```

## Delegated Context

Ein Delegated Context kann insbesondere enthalten:

- delegierende Identity,
- ausführende Identity,
- delegierte Capabilities,
- Purpose,
- Scope,
- Lifetime,
- Causation Context.

Die konkrete Repräsentation wird in NPSPECs definiert.

## Authority

Delegation darf keine zusätzliche Authority erzeugen.

```text
Delegated Authority
    ⊆
Delegator Authority
```

Die ausführende Komponente kann außerdem weiterhin durch ihre eigene Sandbox, MAC-, Trust-, Sovereignty- oder andere System-Policies eingeschränkt sein.

## Delegation Chains

Delegated Contexts können kontrolliert über mehrere Komponenten weitergegeben werden.

```text
User
 ↓
Application
 ↓
Service
 ↓
Capability Provider
```

Dabei muss die ursprüngliche Causation nachvollziehbar bleiben.

Jede weitere Delegation darf Authority nur erhalten oder weiter einschränken, wenn dies ausdrücklich zulässig ist.

## Keine vollständige Impersonation

NovaOS-native Komponenten dürfen ihre tatsächliche Identity nicht durch eine fremde Identity ersetzen.

Legacy-Schnittstellen, die Impersonation voraussetzen, müssen über kontrollierte Compatibility Bridges abgebildet werden.

Intern bleibt die NovaOS-Identity- und Delegation-Semantik erhalten.

## Audit und Introspection

Sicherheitsrelevante Operationen müssen unterscheiden können:

```text
Who requested?
Who executed?
Under which delegated authority?
```

Dadurch bleibt die Verantwortungs- und Causation-Kette auch über Service- und Prozessgrenzen hinweg nachvollziehbar.

## Normative Anforderungen

1. NovaOS MUSS Delegated Context als natives Modell für Handeln im Auftrag anderer Identities verwenden.
2. Die ausführende Identity MUSS von der delegierenden Identity getrennt bleiben.
3. Native Identity Impersonation SOLL NICHT als allgemeines Autorisierungsmodell verwendet werden.
4. Delegierte Authority DARF die Authority des Delegators NICHT erweitern.
5. Delegated Contexts MÜSSEN lifecycle- und scopegebunden sein können.
6. Delegation MUSS mit Capability Delegation und Attenuation integrierbar sein.
7. Delegation Chains MÜSSEN die relevante Causation erhalten.
8. Weiterdelegation MUSS explizit kontrollierbar sein.
9. Security-, Trust-, IFC- und Sovereignty-Regeln DÜRFEN durch Delegation nicht umgangen werden.
10. Delegated Context MUSS über IPC- und Nova.Call-Grenzen propagierbar sein.
11. Legacy Impersonation MUSS über kontrollierte Compatibility-Mechanismen abbildbar sein.
12. Delegierende und ausführende Identity MÜSSEN kontrolliert auditierbar und introspektierbar bleiben.

## Konsequenzen

### Positive Konsequenzen

- klare Verantwortlichkeit,
- keine versteckte Identity-Übernahme,
- Least-Privilege-Delegation,
- nachvollziehbare Causation Chains,
- bessere Integration mit Capability-Based Security.

### Negative Konsequenzen

- Delegation Context muss über Systemgrenzen propagiert werden,
- Legacy-Impersonation benötigt Compatibility Mapping.

## Verworfene Alternativen

### Vollständige Identity Impersonation

Verworfen, da tatsächliche und vertretene Identity nicht mehr sauber unterscheidbar wären.

### Nur ausführende Identity berücksichtigen

Verworfen, da der ursprüngliche Auftraggeber und dessen Authority verloren gingen.

## Abhängigkeiten

- `ADR-IDENTITY-0001_Identity_getrennt_von_Display_Name_Credential_Process_und_Address`
- `ADR-IDENTITY-0003_Ephemeral_Workload_Identity`
- `ADR-IDENTITY-0005_Service_und_Application_Identity`
- `ADR-CAPABILITY-0007_Capability_Delegation`
- `ADR-CAPABILITY-0008_Capability_Attenuation`
- `ADR-SECURITY-0009_Capability_Delegation_explizit`
- `ADR-IPC-0007_Causation_und_Tracing_über_IPC_Grenzen`

## Zugehörige NPSPECs

- `NPSPEC-IDENTITY-DELEGATED-CONTEXT-0001`
- `NPSPEC-IDENTITY-DELEGATION-CHAIN-0001`
- `NPSPEC-IDENTITY-DELEGATED-AUTHORITY-0001`
- `NPSPEC-IDENTITY-DELEGATED-CONTEXT-PROPAGATION-0001`
- `NPSPEC-IDENTITY-DELEGATED-CONTEXT-TEST-0001`

## Ergebnis

NovaOS ersetzt klassische Impersonation durch explizite Delegation:

```text
Original Identity
       ↓
Delegated Context
       ↓
Acting Identity
       ↓
Operation
```

Damit bleibt jederzeit unterscheidbar, **wer eine Operation angefordert hat, wer sie tatsächlich ausführt und welche Authority dafür delegiert wurde**.