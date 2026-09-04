# ADR-RESILIENCE-0020 – Explainable Self-Healing

## Status

Angenommen

## Kategorie

Resilience Architecture / Explainability / Self-Healing / Introspection

## Kontext

Nova.Resilience kann Fehler automatisch erkennen, klassifizieren und beheben.

Mit zunehmender Autonomie entsteht jedoch die Anforderung, nachvollziehen zu können:

- warum ein Fehler erkannt wurde,
- warum eine bestimmte Recovery gewählt wurde,
- welche Alternativen verworfen wurden,
- welche Authority und Policies beteiligt waren,
- warum ein Recovery Scope eskaliert wurde,
- warum ein Restore erlaubt oder verweigert wurde.

Ein Self-Healing-System, das seinen eigenen Eingriff nicht erklären kann, wäre schwer zu debuggen, zu auditieren und langfristig zu vertrauen.

## Entscheidung

NovaOS definiert **Explainable Self-Healing als verbindliche Eigenschaft von Nova.Resilience**.

Für relevante Recovery-Entscheidungen muss Nova.Resilience eine strukturierte Erklärung erzeugen können.

```text
Failure Evidence
      ↓
Classification
      ↓
Recovery Decision
      ↓
Execution
      ↓
Verification
      ↓
Explanation
```

Die Erklärung beschreibt die für die Entscheidung relevanten Fakten und Regeln, ohne interne Implementierungsdetails unnötig offenzulegen.

## Grundprinzip

```text
Automatic recovery
must remain explainable.
```

Explainability ist keine nachträgliche Protokollierung allein, sondern Bestandteil der Recovery-Architektur.

## Explainable Decision

Eine Recovery-Erklärung soll mindestens nachvollziehbar machen können:

```text
What failed?
Why was it classified this way?
What was affected?
What recovery was selected?
Why was this recovery selected?
Which alternatives existed?
Why were alternatives rejected?
What constraints applied?
Was recovery verified?
What state was restored?
```

Die konkrete Darstellung wird in NPSPECs definiert.

## Evidence

Erklärungen müssen auf tatsächlich verwendeter Evidence beruhen.

Beispiele:

```text
Health State
Progress Evidence
Failure Events
Resource Pressure
Circuit State
Trust State
Integrity State
Dependency State
Recovery Contract
RTO / RPO
```

Nova.Resilience darf keine Erklärung erzeugen, die eine Entscheidungsgrundlage behauptet, die tatsächlich nicht verwendet wurde.

## Recovery Plan

Ein Recovery Plan muss seine wesentlichen Auswahlgründe darstellen können.

Beispiel:

```text
Selected:
Provider B Replacement

Reason:
Provider A = Failed
Provider B = Healthy
Provider B satisfies Trust Requirement
Provider B satisfies Sovereignty Constraint
Provider B meets required RTO
```

Dies erlaubt sowohl technische Analyse als auch Benutzer- und Administratorverständnis.

## Rejected Alternatives

Wo mehrere relevante Recovery-Alternativen existierten, soll Nova.Resilience begründen können, warum diese nicht gewählt wurden.

Beispiel:

```text
Restart Provider A
→ rejected: repeated recovery failure

Provider C
→ rejected: sovereignty violation

System Restart
→ rejected: unnecessarily large recovery scope
```

Nicht jede theoretisch mögliche Alternative muss aufgeführt werden.

Relevant sind die tatsächlich bewerteten oder naheliegenden Kandidaten.

## Scope Escalation

Recovery-Scope-Eskalationen müssen erklärbar sein.

```text
Task Recovery
    ↓ failed verification
Workload Recovery
```

Die Erklärung muss den Grund für die Erweiterung des Recovery Scope nachvollziehbar machen können.

## Autonomy

Explainability wird mit Recovery Autonomy Levels integriert.

Besonders bei:

```text
L3 – Autonomous
L4 – Emergency Autonomous
```

muss nachvollziehbar sein, aufgrund welcher Policy Nova.Resilience ohne vorherige Approval handeln durfte.

## Security Recovery

Bei Quarantine oder anderen Security-Recovery-Aktionen muss die Erklärung die sicherheitsrelevante Ursache darstellen können, ohne sensible Evidence unnötig offenzulegen.

```text
Workload quarantined
because:
Integrity requirement violated
```

Sensitive Details bleiben durch Authority und Privacy Policy geschützt.

## Safe State

Beim Eintritt in einen Safe State muss erklärt werden können:

```text
Which guarantee could no longer be met?
Why was recovery not safe within the remaining budget?
Why was this Safe State selected?
```

Dies gilt besonders für Safety- und Realtime-Systeme.

## Recovery Contract Violations

Verletzungen von RTO oder RPO müssen explizit erklärbar sein.

Beispiel:

```text
RTO required: 500 ms
Actual recovery: 820 ms
Result: RTO violation
```

Nova.Resilience darf eine solche Recovery nicht lediglich als `successful` darstellen.

## Learning

Wenn lernende Mechanismen die Recovery-Auswahl beeinflussen, muss zwischen deterministischen Hard Constraints und lernbasierten Empfehlungen unterschieden werden.

```text
Hard Constraints
      ↓
Eligible Recovery Set
      ↓
Learned Preference
      ↓
Selected Recovery
```

Eine Erklärung muss kenntlich machen können, welcher Teil der Entscheidung aus festen Regeln und welcher aus adaptiver Optimierung stammt.

## Prediction Error

Nova.Resilience kann Prediction Error als Lernsignal verwenden.

Beispiel:

```text
Predicted:
Restart likely successful

Observed:
Restart failed verification

Prediction Error:
High

Adaptation:
Lower restart preference for this failure pattern
```

Die Lernhistorie darf Hard Constraints nicht verändern.

## Unterschiedliche Erklärungsebenen

NovaOS soll unterschiedliche Detailstufen unterstützen können.

Beispielsweise:

```text
User Explanation
Administrator Explanation
Developer Explanation
Audit Explanation
```

Alle Ebenen beziehen sich auf dieselbe zugrunde liegende Decision Evidence.

Sie unterscheiden sich nur in Detailgrad und Offenlegung.

## Privacy und Security

Explainability darf keine neue Datenleckquelle werden.

Erklärungen müssen weiterhin folgende Regeln einhalten:

```text
Data Minimization
Selective Disclosure
Capability Authorization
Information Flow Control
Data Sovereignty
Secret Protection
```

Eine ausreichende Erklärung muss möglich sein, ohne geheime Schlüssel, private Daten oder sicherheitskritische Interna offenzulegen.

## Determinismus

Im Deterministic Mode müssen Recovery-Entscheidung und Erklärung konsistent reproduzierbar sein können.

Gleiche relevante Inputs und Policies sollen zu derselben erklärbaren Entscheidungsstruktur führen, soweit der Contract dies verlangt.

## Audit und Architecture Introspection

Explainable Self-Healing wird mit Architecture Introspection und Audit integriert.

```text
Architecture Introspection
        +
Nova.Resilience
        ↓
Recovery Explanation
        ↓
Audit / Diagnostics / UI
```

Erklärung, Audit und Telemetrie dürfen dieselbe strukturierte Evidence verwenden, sollen aber getrennte Zugriffspolicies besitzen können.

## Normative Anforderungen

1. Nova.Resilience MUSS relevante automatische Recovery-Entscheidungen erklärbar machen können.
2. Erklärungen MÜSSEN auf tatsächlich verwendeter Evidence und Policies beruhen.
3. Erklärungen MÜSSEN Fehlerklasse, Recovery Scope und ausgewählte Recovery darstellen können.
4. Relevante verworfene Alternativen SOLLEN mit Ablehnungsgrund darstellbar sein.
5. Scope-Eskalationen MÜSSEN erklärbar sein.
6. L3- und L4-Recovery MÜSSEN den zulässigen Autonomy Context erklären können.
7. Recovery Verification und deren Ergebnis MÜSSEN Bestandteil der Erklärung sein können.
8. RTO- und RPO-Verletzungen MÜSSEN explizit erklärbar sein.
9. Safe-State-Entscheidungen MÜSSEN die verletzten oder nicht mehr garantierbaren Bedingungen darstellen können.
10. Quarantine und Security Recovery MÜSSEN erklärbar sein, ohne sensible Evidence unnötig offenzulegen.
11. Lernbasierte Recovery-Auswahl MUSS von Hard-Constraint-Entscheidungen unterscheidbar sein.
12. Prediction Error und daraus resultierende Anpassungen MÜSSEN nachvollziehbar sein können.
13. NovaOS SOLL unterschiedliche Erklärungsebenen für Benutzer, Administratoren, Entwickler und Audit unterstützen.
14. Unterschiedliche Erklärungsebenen MÜSSEN auf derselben zugrunde liegenden Decision Evidence basieren.
15. Explainability MUSS Data Minimization und Selective Disclosure einhalten.
16. Erklärungen DÜRFEN Secrets, geschützte Identity-Daten oder sensitive Systeminformationen nicht ohne passende Authority offenlegen.
17. Explainability DARF den Recovery-Pfad nicht zu einer correctness-kritischen Abhängigkeit machen.
18. Ein Fehler der Erklärungserzeugung DARF eine notwendige Safety- oder Security-Recovery nicht verhindern.
19. Im Deterministic Mode MUSS die relevante Entscheidungs- und Erklärungsstruktur reproduzierbar sein können.
20. Recovery-Erklärungen MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- autonome Recovery bleibt nachvollziehbar,
- Fehlentscheidungen können gezielter analysiert werden,
- Benutzer und Administratoren können verstehen, warum NovaOS eingegriffen hat,
- lernbasierte und regelbasierte Entscheidungen bleiben unterscheidbar,
- RTO-, RPO-, Safe-State- und Security-Entscheidungen werden transparenter,
- Audit und Architecture Introspection erhalten gemeinsame strukturierte Evidence.

### Negative Konsequenzen

- Recovery-Komponenten müssen Entscheidungsgründe strukturiert erfassen,
- Erklärungen benötigen zusätzliche Metadaten,
- sensible Informationen müssen sorgfältig aus Erklärungen gefiltert werden.

## Verworfene Alternativen

### Self-Healing als vollständig interne Black Box

Verworfen, da autonome Eingriffe sonst schwer auditierbar, debuggbar und vertrauenswürdig wären.

### Nur Logs als Erklärung

Verworfen, da Ereignislogs nicht automatisch die tatsächliche Entscheidungslogik und verworfenen Alternativen darstellen.

### Vollständige interne Entscheidungsdetails immer offenlegen

Verworfen, da dadurch Secrets, Security-Evidence oder geschützte Systeminformationen offengelegt werden könnten.

### Explainability als Voraussetzung für Emergency Recovery

Verworfen, da notwendige Safety- oder Security-Maßnahmen nicht durch einen Fehler der Erklärungsschicht blockiert werden dürfen.

## Abhängigkeiten

- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0002_Detect_Classify_Contain_Preserve_Plan_Recover_Verify_Restore_Learn`
- `ADR-RESILIENCE-0004_Kleinster_sicherer_Recovery_Scope`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-RESILIENCE-0014_Security_Failures_durch_Quarantine_statt_blindem_Restart`
- `ADR-RESILIENCE-0016_Safe_State_für_Safety_und_Realtime`
- `ADR-RESILIENCE-0017_RTO_und_RPO_als_Recovery_Contracts`
- `ADR-RESILIENCE-0019_Recovery_Autonomy_Levels`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-PRIVACY-0001_Data_Minimization`
- `ADR-PRIVACY-0005_Selective_Disclosure`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-EXPLAINABILITY-0001`
- `NPSPEC-RESILIENCE-DECISION-EVIDENCE-0001`
- `NPSPEC-RESILIENCE-RECOVERY-EXPLANATION-0001`
- `NPSPEC-RESILIENCE-ALTERNATIVE-EXPLANATION-0001`
- `NPSPEC-RESILIENCE-EXPLANATION-VIEW-0001`
- `NPSPEC-RESILIENCE-LEARNING-EXPLANATION-0001`
- `NPSPEC-RESILIENCE-EXPLAINABILITY-TEST-0001`

## Ergebnis

Nova.Resilience kombiniert autonome Recovery mit strukturierter Nachvollziehbarkeit:

```text
Failure Evidence
      ↓
Classification
      ↓
Recovery Candidates
      ↓
Policies + Contracts + Hard Constraints
      ↓
Recovery Decision
      ↓
Recover
      ↓
Verify
      ↓
Restore
      ↓
Structured Explanation
```

Damit kann NovaOS nicht nur selbstständig auf Fehler reagieren, sondern auch nachvollziehbar darstellen, **was erkannt wurde, warum eine bestimmte Recovery gewählt wurde, welche Grenzen galten und ob die Wiederherstellung tatsächlich erfolgreich war**.