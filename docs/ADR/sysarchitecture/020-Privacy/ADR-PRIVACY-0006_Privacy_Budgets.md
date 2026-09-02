# ADR-PRIVACY-0006 – Privacy Budgets

## Status

Angenommen

## Kategorie

Privacy Architecture / Information Exposure / Privacy Accounting

## Kontext

Einzelne Datenzugriffe können für sich genommen datenschutzkonform erscheinen und dennoch in ihrer Summe zu einer erheblichen Informationspreisgabe führen.

Dies betrifft insbesondere:

- wiederholte Selective Disclosures,
- statistische Abfragen,
- Telemetrie,
- Analysefunktionen,
- KI- und Adaptionssysteme,
- aggregierte Daten,
- wiederholte Queries,
- abgeleitete Informationen.

Beispielsweise können mehrere begrenzte Abfragen gemeinsam Informationen rekonstruieren, die durch keine einzelne Abfrage direkt offengelegt wurden.

NovaOS benötigt deshalb neben klassischen Berechtigungen ein Modell zur Begrenzung **kumulativer Informationspreisgabe**.

---

## Entscheidung

NovaOS führt **Privacy Budgets als systemweite, policygesteuerte Begrenzung kumulativer Informationspreisgabe** ein.

```text
Privacy Context
      ↓
Privacy Budget
      ↓
Disclosure / Analysis Request
      ↓
Estimated Privacy Cost
      ↓
Policy Decision
      ↓
Allow / Reduce / Require Approval / Deny
```

Ein Privacy Budget beschreibt, wie viel Informationspreisgabe innerhalb eines definierten Kontextes zulässig ist.

---

## Grundprinzip

```text
A permitted disclosure is not necessarily
an unlimited repeatable disclosure.
```

Privacy Budgets ergänzen Capability-Based Security und Information Flow Control.

Sie erzeugen keine Authority und ersetzen keine bestehenden Sicherheitsregeln.

---

## Budget Scope

Privacy Budgets müssen an einen definierten Scope gebunden werden können.

Beispiele:

```text
User
Application
Execution Domain
Service
Capability
Dataset
Data Object
Session
Task Group
Remote Service
Purpose
```

Budgets dürfen hierarchisch kombiniert werden.

Beispielsweise:

```text
User Budget
    ↓
Application Budget
    ↓
Capability Budget
```

Eine untergeordnete Ebene darf dadurch kein größeres effektives Privacy Budget erhalten als ihre übergeordneten Grenzen erlauben.

---

## Privacy Cost

Privacy-relevante Operationen können einen Privacy Cost besitzen.

```text
Operation
    ↓
Privacy Cost Evaluation
    ↓
Budget Consumption
```

Der Privacy Cost kann unter anderem abhängig sein von:

- Semantic Data Labels,
- Sensitivity,
- Disclosure Detail,
- Datenmenge,
- Empfänger,
- Zweck,
- Aggregationsgrad,
- vorherigen Disclosures,
- Wiederholbarkeit,
- Rekonstruktionsrisiko.

Die konkrete Berechnung wird in NPSPECs definiert.

---

## Kumulative Disclosure

Privacy Budgets müssen wiederholte Informationspreisgabe berücksichtigen können.

```text
Disclosure A
    +
Disclosure B
    +
Disclosure C
    ↓
Cumulative Exposure
```

Damit wird verhindert, dass ein Consumer eine Schutzgrenze durch viele einzeln zulässige Abfragen umgeht.

---

## Selective Disclosure

Privacy Budgets werden direkt mit Selective Disclosure integriert.

```text
Requested Disclosure
        ↓
Minimum Required View
        ↓
Privacy Cost
        ↓
Remaining Budget
```

Wenn das vollständige gewünschte Disclosure das Budget überschreiten würde, kann eine kleinere zulässige View angeboten werden, sofern der Contract dies erlaubt.

---

## Statistische und analytische Abfragen

Privacy Budgets sollen insbesondere statistische Systeme unterstützen.

```text
Dataset
   ↓
Statistical Query
   ↓
Privacy Mechanism
   ↓
Privacy Budget Consumption
   ↓
Result
```

Die Architektur muss mathematisch definierte Privacy-Modelle wie Differential Privacy integrieren können.

Konkrete mathematische Verfahren und Budgetmodelle werden nicht durch diese ADR festgelegt.

---

## KI und adaptive Systeme

KI- und Adaptionssysteme unterliegen Privacy Budgets.

Beispielsweise können wiederholte Modellabfragen, Trainingszugriffe oder Personalisierungssignale kumulativ Informationen offenlegen.

```text
Data
  ↓
AI / Adaptation
  ↓
Privacy Accounting
  ↓
Allowed Learning / Result
```

Adaptive Systeme dürfen Privacy Budgets weder selbstständig erhöhen noch umgehen.

Prediction Error darf zur Optimierung innerhalb des vorhandenen Budgets verwendet werden.

---

## Budget Delegation

Privacy Budgets können kontrolliert an untergeordnete Operationen weitergegeben werden.

```text
Parent Budget
     ↓
Allocated Sub-Budget
     ↓
Child Operation
```

Dabei gilt:

```text
Allocated Budget ≤ Available Parent Budget
```

Budget-Delegation darf keine Vervielfachung des verfügbaren Privacy Budgets ermöglichen.

---

## Budget Exhaustion

Ist ein Privacy Budget ausgeschöpft, muss das System entsprechend der Policy reagieren.

Mögliche Ergebnisse sind:

```text
Deny
Reduced Disclosure
Aggregation
Anonymization
Wait for Budget Renewal
Explicit Approval
```

Ein ausgeschöpftes Budget darf nicht stillschweigend ignoriert werden.

Hard Constraints bleiben auch bei Graceful Degradation erhalten.

---

## Budget Renewal

Privacy Budgets können je nach Policy erneuerbar oder dauerhaft begrenzt sein.

Beispiele:

```text
Per Operation
Per Session
Per Day
Per Dataset
Per Purpose
Lifetime Budget
```

Eine zeitliche Erneuerung darf nicht automatisch verwendet werden, wenn dadurch langfristige kumulative Informationspreisgabe unkontrolliert möglich würde.

---

## Data Provenance und Semantic Labels

Privacy Accounting muss Semantic Data Labels und relevante Provenance berücksichtigen können.

```text
Data
 ├── Semantic Labels
 ├── Provenance
 └── Disclosure History
        ↓
Privacy Cost
```

Dadurch kann NovaOS erkennen, wenn mehrere Ergebnisse letztlich auf denselben geschützten Ausgangsdaten beruhen.

---

## Remote Services

Privacy Budgets gelten auch über System- und Netzwerkgrenzen hinweg.

Ein Remote Service erhält durch wiederholte `Nova.Call`-Operationen nicht automatisch unbegrenzten Zugriff auf zusätzliche Informationen.

Remote Privacy Accounting muss mit:

- Identity,
- Capability Context,
- Service Identity,
- Data Sovereignty,
- Information Flow Control

kombinierbar sein.

---

## Resource Budgets und Privacy Budgets

Privacy Budgets sind von Resource Budgets getrennt.

```text
Resource Budget
→ Wie viele Systemressourcen darf eine Operation verbrauchen?

Privacy Budget
→ Wie viel Information darf offengelegt werden?
```

Beide können Bestandteil eines `Nova.ExecutionContract` oder eines übergeordneten Policy Contexts sein.

---

## Fail-Safe Verhalten

Kann der verbleibende Privacy Cost einer sicherheitsrelevanten Operation nicht zuverlässig bestimmt werden, gilt:

```text
Unknown ≠ Free
```

Die Policy muss dann beispielsweise:

```text
Conservative Cost
Additional Classification
Reduced Disclosure
Deny
```

anwenden können.

Unsicherheit darf nicht automatisch als Privacy Cost von null behandelt werden.

---

## Introspection und Audit

NovaOS muss Privacy Budgets kontrolliert nachvollziehbar machen können.

Beispielsweise:

```text
Budget Scope
Initial Budget
Consumed Budget
Remaining Budget
Disclosure History
Operation Cost
Purpose
Recipient
Budget Decision
```

Dabei dürfen Audit und Introspection selbst keine zusätzliche unzulässige Informationspreisgabe erzeugen.

---

## Normative Anforderungen

1. NovaOS MUSS Privacy Budgets als systemweite Privacy-Policy unterstützen können.
2. Privacy Budgets MÜSSEN kumulative Informationspreisgabe begrenzen können.
3. Privacy Budgets MÜSSEN an definierte Scopes gebunden werden können.
4. Privacy-relevante Operationen MÜSSEN mit einem Privacy Cost bewertbar sein.
5. Wiederholte Disclosures MÜSSEN bei der Budgetbewertung berücksichtigt werden können.
6. Selective Disclosure MUSS mit Privacy Budgets integrierbar sein.
7. Statistische Privacy-Mechanismen wie Differential Privacy MÜSSEN architektonisch integrierbar sein.
8. KI- und Adaptionssysteme DÜRFEN Privacy Budgets NICHT umgehen oder eigenständig erhöhen.
9. Privacy Budgets MÜSSEN kontrolliert an untergeordnete Operationen delegierbar sein.
10. Budget-Delegation DARF Privacy Budget NICHT vervielfachen.
11. Budget Exhaustion MUSS zu einer expliziten Policy-Entscheidung führen.
12. Privacy Budgets MÜSSEN von Resource Budgets semantisch getrennt bleiben.
13. Semantic Data Labels und Provenance MÜSSEN in Privacy Accounting einbezogen werden können.
14. Unbekannter Privacy Cost DARF NICHT automatisch als kostenfrei behandelt werden.
15. Privacy Budgets DÜRFEN Capability-, IFC-, DLP- oder Sovereignty-Regeln NICHT abschwächen.
16. Budgetverbrauch und relevante Entscheidungen MÜSSEN kontrolliert auditierbar und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Schutz vor kumulativer Datenoffenlegung,
- wiederholte Abfragen können systemweit berücksichtigt werden,
- Selective Disclosure wird gegen schrittweise Rekonstruktion abgesichert,
- Differential Privacy und ähnliche Verfahren können integriert werden,
- KI- und Analysesysteme erhalten explizite Privacy-Grenzen.

### Negative Konsequenzen

- Privacy Cost ist nicht für jede Datenart trivial bestimmbar,
- verteilte Systeme benötigen konsistentes Privacy Accounting,
- komplexe Disclosure-Abhängigkeiten können zusätzlichen Policy-Aufwand erzeugen.

---

## Verworfene Alternativen

### Jede autorisierte Abfrage unbegrenzt wiederholen

Verworfen.

Mehrere einzeln harmlose Abfragen können gemeinsam sensible Informationen offenlegen.

### Privacy Budgets ausschließlich für Differential Privacy

Verworfen.

Kumulative Informationspreisgabe betrifft auch Selective Disclosure, KI, Telemetrie und andere Datenflüsse.

### Privacy Budget mit Resource Budget gleichsetzen

Verworfen.

Rechenaufwand und Informationspreisgabe sind unterschiedliche Ressourcen.

### Unbekannte Privacy Costs als null behandeln

Verworfen.

Dies würde fehlende Klassifikation zu einem Umgehungsmechanismus machen.

---

## Abhängigkeiten

- `ADR-PRIVACY-0001_Data_Minimization`
- `ADR-PRIVACY-0003_Semantic_Data_Labels`
- `ADR-PRIVACY-0004_Data_Loss_Prevention_über_semantische_Informationen`
- `ADR-PRIVACY-0005_Selective_Disclosure`
- `ADR-SECURITY-0015_Information_Flow_Control`
- `ADR-STORAGE-0008_Data_Provenance_im_Storage_Modell`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0014_Explicit_Contracts`

---

## Zugehörige NPSPECs

- `NPSPEC-PRIVACY-BUDGET-0001`
- `NPSPEC-PRIVACY-BUDGET-SCOPE-0001`
- `NPSPEC-PRIVACY-ACCOUNTING-0001`
- `NPSPEC-PRIVACY-BUDGET-DELEGATION-0001`
- `NPSPEC-PRIVACY-BUDGET-EXHAUSTION-0001`
- `NPSPEC-PRIVACY-DIFFERENTIAL-PRIVACY-0001`
- `NPSPEC-PRIVACY-BUDGET-TEST-0001`

---

## Ergebnis

NovaOS erweitert Datenschutz von der Betrachtung einzelner Zugriffe auf die kumulative Informationspreisgabe:

```text
Data
   ↓
Disclosure Requests
   ↓
Semantic Privacy Cost
   ↓
Privacy Accounting
   ↓
Privacy Budget
   ↓
Allow / Reduce / Approval / Deny
```

Damit kann NovaOS verhindern, dass viele einzeln zulässige Datenzugriffe gemeinsam eine Informationsmenge offenlegen, die als Ganzes niemals freigegeben werden sollte.