# ADR-STATE-0003 – State Machines

## Status

Angenommen

## Kategorie

State Management / Lifecycle / System Model

## Kontext

Viele NovaOS-Objekte besitzen einen definierten Lebenszyklus.

Beispiele:

```text
Service:   Stopped → Starting → Running → Stopping
Device:    Detected → Initialized → Ready → Failed
Execution: Created → Running → Completed
```

Reine Statusfelder reichen nicht aus, da nicht jeder Übergang zwischen Zuständen zulässig ist.

## Entscheidung

NovaOS verwendet **explizite State Machines** für Komponenten mit definiertem Lebenszyklus.

Eine State Machine beschreibt:

```text
States
Transitions
Transition Conditions
Failure States
```

Beispiel:

```text
Created
   ↓
Starting
   ↓
Running
   ↓
Stopping
   ↓
Stopped
```

## Zustandsübergänge

Übergänge müssen explizit definiert sein.

```text
Current State
     +
Event / Operation
     +
Constraints
     ↓
Next State
```

Nicht definierte Übergänge sind ungültig.

Beispiel:

```text
Stopped → Running
```

kann intern verlangen:

```text
Stopped → Starting → Running
```

## Transition Authority

Ein möglicher Übergang bedeutet nicht automatisch, dass er ausgeführt werden darf.

```text
Valid Transition
      +
Authorization
      +
Hard Constraints
      ↓
Allowed Transition
```

Security-, Safety-, Realtime- und andere Hard Constraints bleiben gültig.

## Intermediate States

Länger dauernde Operationen sollen explizite Zwischenzustände besitzen.

Beispiele:

```text
Starting
Stopping
Updating
Recovering
Migrating
```

Dadurch wird vermieden, dass ein Objekt während einer laufenden Operation fälschlich als vollständig im Zielzustand erscheint.

## Failure States

Fehlerzustände werden explizit modelliert.

```text
Running
   ↓
Failure
   ↓
Failed
   ↓
Recovering
   ↓
Running
```

Ein Fehler darf nicht durch einen normalen Zustand verborgen werden.

## Desired und Actual State

State Machines beschreiben primär den **Actual State** und seine gültigen Übergänge.

Der Desired State kann einen Zielzustand vorgeben:

```text
Desired: Running

Actual:
Stopped → Starting → Running
```

Der Reconciler verwendet die State Machine, um einen gültigen Pfad zum Zielzustand zu bestimmen.

## Versionierung

State-Machine-Definitionen müssen evolvierbar und versionierbar sein.

Persistierter Zustand muss einer bekannten State-Machine-Version zugeordnet werden können.

## Observability

Zustandsänderungen sollen mindestens nachvollziehbar machen:

```text
Object ID
Old State
Transition
New State
Result
```

Fehlgeschlagene Übergänge müssen erkennbar bleiben.

## Normative Anforderungen

1. NovaOS MUSS explizite State Machines für Komponenten mit definierten Lebenszyklen unterstützen.
2. Zustände und zulässige Übergänge MÜSSEN explizit definiert sein.
3. Nicht definierte Übergänge MÜSSEN abgelehnt werden.
4. Übergänge MÜSSEN Authorization und Hard Constraints respektieren.
5. Länger dauernde Übergänge SOLLEN explizite Zwischenzustände verwenden.
6. Fehlerzustände MÜSSEN explizit darstellbar sein.
7. Desired State DARF ungültige Zustandsübergänge nicht erzwingen.
8. State-Machine-Definitionen MÜSSEN versionierbar sein.
9. Zustandsübergänge MÜSSEN beobachtbar sein.

## Konsequenzen

### Positiv

- klar definierte Komponenten-Lebenszyklen,
- ungültige Zustandswechsel werden verhindert,
- Reconciliation erhält gültige Übergangspfade,
- Fehler- und Zwischenzustände bleiben sichtbar,
- Self-Healing kann gezielt auf Zustände reagieren.

### Negativ

- Komponenten benötigen explizite Lifecycle-Modelle,
- Änderungen an State Machines erfordern Versionsmanagement.

## Abhängigkeiten

- `ADR-STATE-0001_Globaler_System_State_Graph`
- `ADR-STATE-0002_Desired_und_Actual_State`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-DECLARATIVE-0001`
- `ADR-AUTONOMY-0003_Self_Configuration`
- `ADR-AUTONOMY-0005_Self_Healing`

## Ergebnis

NovaOS modelliert Lebenszyklen nicht als beliebig veränderbare Statuswerte, sondern als explizite State Machines:

```text
State
  ↓
Valid Transition
  ↓
State
```

Damit sind nicht nur aktuelle Zustände bekannt, sondern auch die Regeln, nach denen sich das System zwischen ihnen bewegen darf.