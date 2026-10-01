# NPSPEC-ADAPTIVE-USEROVERRIDE-0001 – Nova Adaptive User Override

## Status

Angenommen

## Kategorie

Adaptive System / User Control / Override / Policy

## Zweck

NovaOS definiert einen systemweiten Mechanismus, mit dem autorisierte Nutzer adaptive Entscheidungen gezielt überschreiben, begrenzen oder deaktivieren können.

```text
Adaptive Decision
       ↓
User Override
       ↓
Constraint Validation
       ↓
Effective Decision
```

Der Nutzer behält damit Kontrolle über adaptive Optimierungen, ohne Safety-, Security- oder andere verbindliche Hard Constraints umgehen zu können.

## Grundprinzipien

```text
User Override ≠ Unlimited Authority
User Preference ≠ Capability
Override ≠ Security Bypass
Override ≠ Hard Constraint Modification
Adaptive Decision ≠ User Decision

Explicit User Decision > Adaptive Optimization
```

## Override Model

```text
UserOverride
├── OverrideID
├── Target
├── RequestedState
├── Scope
├── Authority
└── State
```

Optional:

```text
IdentityID
ExecutionID
ResourceID
ProviderID
PolicyID
StartTime
Expiration
Reason
Persistence
ProvenanceID
```

## Override Targets

Overrides können adaptive Funktionen betreffen wie:

```text
Prediction
Scheduler
Cache
Prefetch
Preload
Memory
Power
Network
Storage
Provider Selection
Placement
Algorithm Selection
```

## Override Types

NovaOS unterstützt mindestens:

```text
Disable
Enable
Prefer
Avoid
Force
Limit
Pin
ResetToDefault
```

`Force` darf nur innerhalb zulässiger Hard Constraints wirken.

## Scope

Ein Override kann begrenzt werden auf:

```text
Current Operation
Execution
Application
Object
Resource
Provider
Device
Session
User
System
```

Overrides sollen möglichst den kleinsten notwendigen Scope verwenden.

## Priority

NovaOS verwendet folgende Priorität:

```text
Safety
  ↓
Security
  ↓
Sovereignty / Trust
  ↓
Hard System Constraints
  ↓
Explicit User Override
  ↓
Soft Preferences
  ↓
Adaptive Optimization
```

Adaptive Komponenten dürfen eine gültige explizite Nutzerentscheidung nicht selbstständig rückgängig machen.

## Temporary Override

Overrides können zeitlich begrenzt sein.

```text
Override
   ↓
Active
   ↓
Expiration
   ↓
Previous Policy
```

Beispiele:

```text
Disable Prefetch for Session
Pin Execution to Local Device
Avoid Network Provider for 1 Hour
Disable Adaptive Power temporarily
```

## Persistent Override

Bestimmte Overrides können persistent gespeichert werden.

```text
User Preference
      ↓
Persistent Override
      ↓
Future Decisions
```

Persistenz muss explizit erkennbar sein und widerrufen werden können.

## Adaptive Disable

Nutzer können adaptive Optimierungen vollständig oder selektiv deaktivieren.

```text
Adaptive System
├── Prediction      OFF
├── Prefetch        OFF
├── Preload         OFF
├── Policy Learning OFF
└── Base System     ON
```

Die grundlegende Systemfunktion muss erhalten bleiben.

## Policy Learning

User Overrides dürfen nicht automatisch als Trainingssignal interpretiert werden.

```text
User Override ≠ Learning Consent
```

Falls Overrides für Policy Learning verwendet werden sollen, muss dies durch die dafür geltende Policy erlaubt sein.

## Conflict Handling

Widersprüchliche Overrides müssen deterministisch aufgelöst werden.

Berücksichtigt werden können:

```text
Authority
Specificity
Scope
Timestamp
Explicit Priority
Policy
```

Nicht auflösbare Konflikte müssen sichtbar gemacht werden.

## Invalid Override

Ein Override wird abgelehnt, wenn er verbindliche Grenzen verletzt.

```text
User Request
     ↓
Constraint Validation
     ↓
Hard Constraint Violation
     ↓
Reject
```

Die Ablehnung soll einen maschinenlesbaren Reason Code liefern.

## Feedback

NovaOS kann die Auswirkungen eines Overrides beobachten:

```text
Override
   ↓
Execution
   ↓
Observed Result
```

Diese Beobachtung darf den Override nicht automatisch rückgängig machen.

## Safe Fallback

Bei ungültigen, abgelaufenen oder nicht mehr anwendbaren Overrides:

```text
Override Invalid
      ↓
Remove Override
      ↓
Valid Base / User Policy
```

Es darf nicht automatisch auf eine unsichere adaptive Entscheidung zurückgefallen werden.

## Security

Overrides benötigen entsprechende Authority.

```text
User Identity
     +
Capability
     +
Policy
     ↓
Allowed Override
```

Ein Nutzer darf nur Bereiche überschreiben, für die er autorisiert ist.

## Privacy

Override-Historien können Nutzerpräferenzen offenlegen.

Daher gelten:

```text
Data Minimization
Retention
Security Labels
Controlled Access
Controlled Export
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
OverrideID
Target
Scope
Requested State
Effective State
Authority
Start Time
Expiration
Persistence
Conflict State
Reason
Rejected Constraint
Provenance
```

Adaptive Decision Observability soll anzeigen können, wenn eine Entscheidung aufgrund eines User Overrides getroffen wurde.

## Normative Anforderungen

1. NovaOS MUSS explizite User Overrides für adaptive Systeme unterstützen.
2. Gültige User Overrides MÜSSEN Vorrang vor adaptiver Optimierung besitzen.
3. User Overrides DÜRFEN Safety-, Security-, Trust-, Sovereignty- oder andere Hard Constraints NICHT umgehen.
4. Overrides MÜSSEN einen definierten Scope besitzen.
5. Overrides SOLLEN zeitlich begrenzbar sein.
6. Persistente Overrides MÜSSEN widerrufbar sein.
7. Adaptive Komponenten DÜRFEN gültige Overrides NICHT selbstständig aufheben.
8. Nutzer MÜSSEN adaptive Funktionen selektiv deaktivieren können, sofern ihre Authority dies erlaubt.
9. Das Deaktivieren adaptiver Funktionen DARF die grundlegende Systemfunktion NICHT verhindern.
10. `Force` MUSS weiterhin alle Hard Constraints respektieren.
11. Widersprüchliche Overrides MÜSSEN deterministisch auflösbar sein.
12. Nicht auflösbare Konflikte MÜSSEN sichtbar gemacht werden.
13. Abgelehnte Overrides SOLLEN einen maschinenlesbaren Reason Code liefern.
14. User Overrides DÜRFEN NICHT automatisch als Zustimmung zu Policy Learning interpretiert werden.
15. Overrides MÜSSEN Capability- und Policy-Prüfungen durchlaufen.
16. Abgelaufene Overrides DÜRFEN NICHT weiter angewendet werden.
17. Override-Historien MÜSSEN Privacy- und Retention-Regeln beachten.
18. Aktive Overrides und ihre Auswirkungen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-ADAPTIVE-PREDICTION-0001`
- `NPSPEC-ADAPTIVE-FEEDBACK-0001`
- `NPSPEC-ADAPTIVE-POLICYLEARNING-0001`
- `NPSPEC-ADAPTIVE-SCHEDULER-0001`
- `NPSPEC-ADAPTIVE-CACHE-0001`
- `NPSPEC-ADAPTIVE-PREFETCH-0001`
- `NPSPEC-ADAPTIVE-PRELOAD-0001`
- `NPSPEC-ADAPTIVE-MEMORY-0001`
- `NPSPEC-ADAPTIVE-POWER-0001`
- `NPSPEC-ADAPTIVE-NETWORK-0001`
- `NPSPEC-ADAPTIVE-STORAGE-0001`
- `NPSPEC-OBSERVABILITY-DECISION-0001`
- `NPSPEC-OBSERVABILITY-INTROSPECTION-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `ADR-ARCH-0092`

## Ergebnis

```text
Adaptive Proposal
       +
Explicit User Decision
       ↓
Authority + Constraint Check
       ↓
Effective Override
       ↓
Execution
       ↓
Observable Result
```

NovaOS erhält damit eine klare Kontrollgrenze zwischen selbstständiger Optimierung und menschlicher Entscheidung: Das System darf lernen, vorhersagen und optimieren, aber eine gültige explizite Nutzerentscheidung besitzt Vorrang vor adaptiven Soft-Entscheidungen, solange die verbindlichen Sicherheits- und Systemgrenzen eingehalten werden.