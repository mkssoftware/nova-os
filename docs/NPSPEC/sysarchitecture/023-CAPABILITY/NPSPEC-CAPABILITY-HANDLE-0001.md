# NPSPEC-CAPABILITY-HANDLE-0001 – Nova Capability Handle

## Status

Angenommen

## Kategorie

Capability / Handle / Security

## Zweck

NovaOS definiert Capability Handles als lokale, prozess- oder kontextgebundene Referenzen auf Capability-Objekte.

```text
Process / Workload
       ↓
Capability Handle
       ↓
Capability Table
       ↓
Capability
       ↓
Target Object
```

Ein Handle ist keine Capability Identity und darf außerhalb seines gültigen Security Contexts keine Autorität besitzen.

## Grundprinzipien

```text
Handle ≠ Capability
Handle ≠ CapabilityID
Handle ≠ ObjectID
Handle Value ≠ Authority
Same Handle Value ≠ Same Capability
Closed Handle ≠ Valid Authority
Handle Guessing ≠ Access
```

## Handle-Modell

Ein Handle referenziert einen geschützten Eintrag:

```text
CapabilityHandle
├── Handle Value
├── CapabilityID
├── Rights View
├── Security Context
└── State
```

Der Kernel kontrolliert die Zuordnung:

```text
Handle
  ↓
Capability Table Entry
  ↓
Capability Object
```

Anwendungen dürfen diese Zuordnung nicht selbst manipulieren.

## Lokaler Namensraum

Handles gelten innerhalb eines definierten Kontextes.

```text
Process A: Handle 42 → Capability X
Process B: Handle 42 → Capability Y
```

Daher besitzt der numerische Handle-Wert keine globale Bedeutung.

## Capability Table

Jeder relevante Security Context kann eine eigene Capability Table besitzen.

```text
Capability Table
├── Handle 1 → Capability A
├── Handle 2 → Capability B
└── Handle 3 → Capability C
```

Die Tabelle muss gegen direkten unautorisierten Zugriff geschützt sein.

## Rechte

Ein Handle darf eine eingeschränkte Sicht auf eine Capability besitzen.

```text
Capability
Rights: Read + Write

      ↓ Handle Creation

Handle
Rights: Read
```

Dabei gilt:

```text
Authority(Handle) ⊆ Authority(Capability)
```

Ein Handle darf niemals zusätzliche Rechte erzeugen.

## Erstellung

Handles entstehen ausschließlich über kontrollierte Mechanismen:

```text
Capability Acquisition
Capability Delegation
IPC Transfer
Object Creation
Service Connection
```

Die bloße Kenntnis eines Handle-Werts darf keine Capability erzeugen.

## Übertragung

Handles dürfen nicht durch normale Datenkopie zwischen Prozessen übertragen werden.

```text
Process A
   ↓
Explicit Capability Transfer
   ↓
Kernel Validation
   ↓
Process B
   ↓
New Local Handle
```

Der empfangende Prozess erhält einen eigenen lokalen Handle.

```text
Handle(A) ≠ Handle(B)
```

Beide können dennoch auf dieselbe Capability oder auf eine attenuierte Ableitung verweisen.

## Lebenszyklus

Handles besitzen einen eigenen Lebenszyklus:

```text
Create
  ↓
Active
  ↓
Close
  ↓
Invalid
```

Das Schließen eines Handles löscht nicht automatisch die zugrunde liegende Capability.

## Revocation

Wird die referenzierte Capability widerrufen:

```text
Capability Revoked
       ↓
Handle Resolution
       ↓
Invalid / Revoked
```

Bestehende Handles dürfen eine Revocation nicht umgehen.

## Stale Handles

NovaOS muss Wiederverwendungsfehler verhindern.

```text
Handle 42 → Capability A
Close
Handle 42 → Capability B
```

Ein alter Verweis auf Handle 42 darf nicht versehentlich Zugriff auf Capability B erhalten.

Geeignete Verfahren umfassen:

```text
Generation Counters
Opaque Handles
Validated Table Entries
```

## Vererbung

Handles dürfen nicht automatisch an neue Prozesse oder Workloads vererbt werden.

```text
Parent
  ↓
Explicit Spawn Contract
  ↓
Selected Capabilities
  ↓
Child
```

Dies unterstützt Least Privilege und verhindert Ambient Authority.

## Introspection

Autorisierte Komponenten dürfen mindestens abfragen:

```text
Handle
CapabilityID
Target Type
Rights
Constraints
State
```

Der Zugriff auf fremde Capability Tables benötigt separate Autorität.

## Normative Anforderungen

1. NovaOS MUSS Capability Handles als geschützte lokale Referenzen behandeln.
2. Handle-Werte DÜRFEN keine globale Identität besitzen.
3. Kenntnis eines Handle-Werts DARF keine Autorität erzeugen.
4. Capability Tables MÜSSEN kernelkontrolliert und geschützt sein.
5. Handles DÜRFEN keine größeren Rechte als die referenzierte Capability besitzen.
6. Prozessübergreifende Übertragung MUSS explizit erfolgen.
7. Der Empfänger einer Übertragung MUSS einen eigenen lokalen Handle erhalten.
8. Geschlossene oder stale Handles DÜRFEN keine Autorität mehr besitzen.
9. Capability Revocation MUSS bestehende Handles unwirksam machen können.
10. Handle-Vererbung MUSS explizit kontrollierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-TOKEN-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-DELEGATION-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-PROCESS-SPAWN-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-IPC-CAPABILITY-0001`
- `ADR-CAPABILITY-0003`

## Ergebnis

```text
Capability
    ↓
Protected Capability Table
    ↓
Context-local Handle
    ↓
Kernel Validation
    ↓
Authorized Operation
```

NovaOS erhält damit sichere, lokale Capability Handles, die effizienten Objektzugriff ermöglichen, ohne globale Handle-Namensräume, Ambient Authority oder unkontrollierte Vererbung einzuführen.