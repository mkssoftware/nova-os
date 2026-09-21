# ADR-EXECUTION-0005 – Data Sovereignty im ExecutionContract

## Status

Angenommen

## Kategorie

Execution Architecture / Data Sovereignty

## Kontext

NovaOS kann Ausführungen lokal, auf anderen Geräten, innerhalb eines Clusters oder über Remote Compute durchführen.

Dabei darf die technisch oder wirtschaftlich optimale Ausführung nicht automatisch zulässig sein.

Daten können Anforderungen besitzen wie:

```text
LocalOnly
DeviceOnly
OrganizationOnly
TrustedCluster
RegionRestricted
NoRemoteProcessing
```

Data Sovereignty muss deshalb bereits bei der Ausführungsplanung als verbindlicher Bestandteil des `Nova.ExecutionContract` berücksichtigt werden.

## Entscheidung

`Nova.ExecutionContract` unterstützt explizite **Data Sovereignty Constraints**.

```text
Execution Request
       ↓
Data Classification
       +
Sovereignty Policy
       ↓
Nova.ExecutionContract
       ↓
Allowed Execution Locations
       ↓
Algorithm + Provider Selection
```

Data Sovereignty ist ein **Hard Constraint**.

Performance, Kosten, Energieverbrauch oder adaptive Optimierung dürfen diese Anforderungen niemals überschreiben.

## Sovereignty Policy

Der Execution Contract kann festlegen, wo Daten verarbeitet werden dürfen.

Beispiele:

```text
LocalOnly
DeviceOnly
TrustedCluster
OrganizationOnly
RegionRestricted
SpecificLocation
NoRemoteProcessing
```

Mehrere Anforderungen können kombiniert werden.

## Datenabhängige Constraints

Sovereignty Requirements können aus den beteiligten Datenobjekten abgeleitet werden.

```text
Input Objects
      ↓
Semantic Metadata
      ↓
Data Classification
      ↓
Sovereignty Requirements
      ↓
Execution Contract
```

Der Request darf bestehende strengere Anforderungen eines Datenobjekts nicht stillschweigend abschwächen.

## Provider Filtering

Sovereignty Constraints werden vor Performance-Optimierungen ausgewertet.

```text
Candidate Providers
       ↓
Sovereignty Check
       ↓
Trust Check
       ↓
Authorization
       ↓
Valid Providers
       ↓
Performance / Energy Optimization
```

Ein schnellerer Provider ist irrelevant, wenn er die Sovereignty Requirements nicht erfüllt.

## Local und Remote Execution

Lokale Ausführung ist nicht automatisch zulässig und Remote Execution nicht automatisch unzulässig.

Entscheidend ist der Contract.

```text
Execution Location
       ↓
Sovereignty Compatible?
 ├── Yes → weitere Prüfung
 └── No  → Reject
```

Auch lokale Provider können ausgeschlossen werden, wenn sie außerhalb des erlaubten Trust- oder Security-Kontexts liegen.

## Data Movement

Data Sovereignty gilt nicht nur für die eigentliche Berechnung, sondern auch für notwendige Datenbewegungen.

```text
Input Location
      ↓
Transfer Path
      ↓
Execution Location
      ↓
Result Location
```

Jeder relevante Transfer muss mit den Sovereignty Constraints vereinbar sein.

## Temporäre Daten

Auch temporäre Daten unterliegen dem Contract.

Dazu gehören beispielsweise:

```text
Buffers
Caches
Temporary Files
Intermediate Results
Checkpoints
Logs
Crash Data
```

Eine zulässige Berechnung darf nicht durch unzulässige temporäre Speicherung die Sovereignty Policy umgehen.

## Resultate

Sovereignty Constraints können auch für erzeugte Ergebnisse gelten.

```text
Protected Input
      ↓
Execution
      ↓
Derived Output
      ↓
Sovereignty Classification
```

Abgeleitete Daten dürfen nicht automatisch als ungeschützt betrachtet werden.

## Trust und Authorization

Data Sovereignty und Trust sind getrennte Bedingungen.

```text
Correct Region
    ≠
Trusted Provider
    ≠
Authorized Provider
```

Ein Provider muss alle relevanten Bedingungen erfüllen.

## Constraint Propagation

Bei verschachtelten oder zusammengesetzten Ausführungen müssen Sovereignty Constraints weitergegeben werden.

```text
Parent Execution
      ↓
Child Execution A
Child Execution B
Child Execution C
```

Unteroperationen dürfen strengere Anforderungen des übergeordneten Contracts nicht umgehen.

## Replanning und Fallback

Fällt ein Provider aus, darf NovaOS nur auf einen anderen Provider wechseln, wenn dieser ebenfalls alle Sovereignty Constraints erfüllt.

```text
Provider Failure
      ↓
Replanning
      ↓
Sovereignty Check
      ↓
Valid Alternative?
 ├── Yes → Continue
 └── No  → Contract Unsatisfied
```

Ein automatischer Fallback auf einen unzulässigen Standort ist verboten.

## Verification

Sovereignty-relevante Eigenschaften müssen vor einer Remote Execution ausreichend verifiziert sein.

```text
Declared Location
       ↓
Evidence
       ↓
Sovereignty Verification
       ↓
Execution Allowed
```

Für Hard Constraints gilt:

```text
Unknown ≠ Satisfied
```

Unbekannte Standort-, Trust- oder Sovereignty-Eigenschaften dürfen nicht als erfüllt angenommen werden.

## Normative Anforderungen

1. `Nova.ExecutionContract` MUSS Data Sovereignty Constraints ausdrücken können.
2. Data Sovereignty MUSS als Hard Constraint behandelt werden.
3. Sovereignty Requirements MÜSSEN aus Datenklassifikation und Semantic Metadata ableitbar sein.
4. Bestehende strengere Datenanforderungen DÜRFEN nicht stillschweigend abgeschwächt werden.
5. Providerwahl MUSS Sovereignty Constraints vor Performance-Optimierungen prüfen.
6. Datenbewegungen MÜSSEN den Sovereignty Constraints entsprechen.
7. Temporäre Daten und Zwischenergebnisse MÜSSEN vom Sovereignty-Modell erfasst werden.
8. Abgeleitete Ergebnisse MÜSSEN ihre relevanten Sovereignty-Eigenschaften erhalten oder explizit neu klassifiziert werden.
9. Data Sovereignty, Trust und Authorization MÜSSEN getrennt geprüft werden.
10. Verschachtelte Ausführungen MÜSSEN relevante Sovereignty Constraints propagieren.
11. Replanning und Fallback DÜRFEN Sovereignty Constraints nicht abschwächen.
12. Remote Execution DARF nur auf nachweislich zulässigen Providern stattfinden.
13. `Unknown` DARF einen Hard Sovereignty Constraint nicht erfüllen.
14. Performance, Energie oder adaptive Optimierung DÜRFEN Data Sovereignty niemals überstimmen.

## Konsequenzen

### Positive Konsequenzen

- Datenkontrolle wird Teil der Ausführungsarchitektur,
- Remote Compute kann sicher in die normale Execution Planning Pipeline integriert werden,
- unerlaubte Datenbewegungen können bereits vor der Ausführung verhindert werden,
- Sovereignty-Regeln gelten konsistent für lokale, verteilte und entfernte Ausführung.

### Negative Konsequenzen

- verfügbare Compute Provider können deutlich eingeschränkt werden,
- Standort- und Trust-Eigenschaften müssen verlässlich nachweisbar sein,
- verteilte Ausführungsplanung wird komplexer.

## Abhängigkeiten

- `ADR-EXECUTION-0001_Nova_ExecutionContract`
- `ADR-EXECUTION-0002_Semantic_Input_und_Output_Types_im_ExecutionContract`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-PRIVACY-0003_Semantic_Data_Labels`
- `ADR-PRIVACY-0007_Data_Sovereignty_Pipeline`
- `ADR-TRUST-0001_Trust_als_Evidence_basiertes_System`
- `ADR-DISTRIBUTED-0009_Location_Transparent_Execution`
- `ADR-DISTRIBUTED-0010_Remote_Execution_unter_Sovereignty_und_Trust_Constraints`
- `ADR-COMPUTE-0009_Providerwahl_unter_Execution_Contracts`

## Zugehörige NPSPECs

- `NPSPEC-EXECUTION-DATA-SOVEREIGNTY-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-PROPAGATION-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-VERIFICATION-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-LOCATION-0001`

## Ergebnis

Data Sovereignty wird direkt in die Ausführungsplanung integriert:

```text
Data
  ↓
Classification
  ↓
Sovereignty Policy
  ↓
Nova.ExecutionContract
  ↓
Allowed Locations
  ↓
Trust + Authorization
  ↓
Algorithm + Provider
  ↓
Execution
```

Der zentrale Grundsatz lautet:

```text
NovaOS darf eine Ausführung
nicht dort durchführen,
wo sie lediglich technisch möglich ist.

Sie darf nur dort stattfinden,
wo die beteiligten Daten
verarbeitet werden dürfen.
```