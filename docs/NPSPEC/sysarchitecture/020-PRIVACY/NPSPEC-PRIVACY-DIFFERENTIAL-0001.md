# NPSPEC-PRIVACY-DIFFERENTIAL-0001 – Nova Differential Privacy

## Status

Angenommen

## Kategorie

Privacy / Data Protection / Differential Privacy

## Zweck

NovaOS definiert Differential Privacy als systemweite Fähigkeit, statistische Auswertungen zu ermöglichen und gleichzeitig den Informationsgewinn über einzelne Personen oder Datensätze mathematisch zu begrenzen.

```text
Sensitive Dataset
       ↓
Statistical Query
       ↓
Differential Privacy
       ↓
Privacy-Protected Result
```

## Grundprinzipien

```text
Aggregate Data ≠ Automatically Anonymous
Noise ≠ Privacy Guarantee
Differential Privacy ≠ Access Control
Privacy Budget ≠ Resource Budget
Repeated Queries ≠ Independent Privacy Cost
```

## Modell

Eine Differential-Privacy-Operation besitzt mindestens:

```text
DPContext
├── DatasetID
├── Query
├── Privacy Parameters
├── Mechanism
└── Privacy Cost
```

Optional:

```text
Purpose
Privacy Label
Security Domain
Sensitivity
Contribution Limit
Budget Owner
Expiration
```

## Privacy Budget

NovaOS muss Privacy Budgets verwalten können.

```text
Privacy Budget
      ↓
Query 1 → Cost
Query 2 → Cost
Query 3 → Cost
      ↓
Remaining Budget
```

Wiederholte Abfragen müssen kumulativ berücksichtigt werden.

Ist das zulässige Budget erschöpft, muss die Policy weitere Abfragen ablehnen oder ein neues ausdrücklich autorisiertes Budget verlangen.

## Parameter

Differential-Privacy-Mechanismen müssen ihre Datenschutzparameter explizit verwalten.

Beispiel:

```text
ε = Privacy Loss Parameter
δ = Failure Probability
```

Kleinere bzw. strengere Parameter können stärkeren Datenschutz bedeuten, dürfen aber nicht ohne Berücksichtigung der Ergebnisqualität gewählt werden.

## Sensitivität

Vor Anwendung eines Mechanismus muss die Sensitivität der Abfrage bestimmt oder begrenzt werden können.

```text
Dataset
   ↓
Contribution Bounds
   ↓
Sensitivity
   ↓
DP Mechanism
```

Unbegrenzte Beiträge einzelner Datensätze dürfen nicht unkontrolliert die Privacy-Garantie schwächen.

## Mechanismen

NovaOS muss unterschiedliche Mechanismen integrieren können:

```text
Noise Mechanisms
Aggregation
Sampling
Contribution Bounding
Private Histograms
Private Statistics
```

Konkrete Algorithmen sollen austauschbar und policygesteuert sein.

## Zufallszahlen

Differential-Privacy-Mechanismen müssen geeignete Zufallsquellen verwenden.

```text
NPSPEC-CRYPTO-RNG-0001
        ↓
DP Mechanism
```

Vorhersagbare Zufallswerte dürfen nicht für sicherheitsrelevante Privacy-Mechanismen verwendet werden.

## Datenminimierung

Differential Privacy ergänzt Data Minimization.

```text
Minimize Input
      ↓
Aggregate
      ↓
Differential Privacy
      ↓
Release Result
```

Nicht benötigte Rohdaten sollen nicht allein deshalb gesammelt werden, weil die spätere Ausgabe Differential Privacy verwendet.

## Privacy Labels

Privacy Labels können festlegen:

```text
DP Required
Maximum Privacy Budget
Allowed Purpose
Allowed Aggregation
Export Restrictions
```

DLP und Information Flow bleiben zusätzlich wirksam.

## Komposition

Mehrere Auswertungen desselben Datenbestands müssen gemeinsam betrachtet werden.

```text
Query A
+
Query B
+
Query C
↓
Combined Privacy Loss
```

NovaOS darf einzelne Abfragen nicht isoliert als sicher bewerten, wenn ihre Kombination das zulässige Privacy Budget überschreitet.

## Audit und Introspection

NovaOS soll mindestens darstellen können:

```text
DatasetID
Mechanism
Privacy Parameters
Consumed Budget
Remaining Budget
Purpose
Policy Decision
```

Rohdaten oder sensible Zwischenwerte dürfen dadurch nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Differential-Privacy-Mechanismen integrieren können.
2. Privacy Budgets MÜSSEN explizit verwaltbar sein.
3. Wiederholte Abfragen MÜSSEN bei der Budgetberechnung berücksichtigt werden.
4. Privacy-Parameter MÜSSEN explizit und policygesteuert sein.
5. Sensitivität und Contribution Limits MÜSSEN berücksichtigt werden können.
6. Privacy-Budget-Erschöpfung DARF NICHT still umgangen werden.
7. Differential Privacy DARF Data Minimization NICHT ersetzen.
8. DP-Mechanismen MÜSSEN geeignete sichere Zufallsquellen verwenden.
9. Kombinierte Privacy-Kosten MÜSSEN berücksichtigt werden können.
10. Privacy-Budget- und Mechanismuszustände SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-LABEL-0001`
- `NPSPEC-PRIVACY-DLP-0001`
- `NPSPEC-CRYPTO-RNG-0001`
- `NPSPEC-SECURITY-INFORMATIONFLOW-0001`
- `NPSPEC-SECURITY-AUDIT-0001`
- `ADR-PRIVACY-0006`

## Ergebnis

```text
Sensitive Dataset
       ↓
Bounded Query
       ↓
Privacy Budget
       ↓
Differential Privacy
       ↓
Protected Statistical Result
```

NovaOS erhält damit eine mathematisch fundierte Privacy-Fähigkeit für statistische Auswertungen, bei der wiederholte Abfragen, Privacy Budgets und der Informationsgewinn über einzelne Datensätze systemweit kontrolliert werden können.