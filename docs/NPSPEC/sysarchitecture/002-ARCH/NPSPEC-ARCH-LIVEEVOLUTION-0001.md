# NPSPEC-ARCH-LIVEEVOLUTION-0001 – Nova Live Evolution

## Status

Angenommen

## Kategorie

Architecture / Evolution / Hot Replacement / Runtime

## Zweck

NovaOS muss zentrale Komponenten aktualisieren, ersetzen und weiterentwickeln können, ohne grundsätzlich einen vollständigen Systemneustart zu benötigen.

```text
Running System
      ↓
New Component Version
      ↓
Validate
      ↓
State Transfer
      ↓
Atomic Switch
      ↓
New Version Active
```

## Grundprinzip

Live Evolution trennt:

```text
Stable Contract
      ↓
Implementation
```

Komponenten dürfen ersetzt werden, solange ihre erforderlichen Contracts und Systembedingungen weiterhin erfüllt werden.

## Austauschbare Komponenten

Live Evolution kann unter anderem verwendet werden für:

```text
Services
Drivers
Capability Provider
System Modules
Algorithms
UI Components
Runtime Components
```

Nicht jede Komponente muss live austauschbar sein.

## Ablauf

Ein Live Replacement folgt grundsätzlich:

```text
Load
  ↓
Validate
  ↓
Prepare
  ↓
Transfer State
  ↓
Switch
  ↓
Verify
  ↓
Retire Old Version
```

Der Wechsel soll aus Sicht abhängiger Komponenten atomar erfolgen.

## State Transfer

Zustandsbehaftete Komponenten müssen ihren relevanten Zustand kontrolliert übertragen können.

```text
Old Component
      ↓
Export State
      ↓
Validate / Convert
      ↓
Import State
      ↓
New Component
```

Implementierungsinterner Zustand muss nicht zwangsläufig übernommen werden.

## Versionierte Contracts

Schnittstellen und Zustandsformate müssen versionierbar sein.

```text
Contract v1
Contract v2
Contract v3
```

Kompatibilität muss vor dem Wechsel geprüft werden.

## Laufende Operationen

Für aktive Operationen kann NovaOS abhängig von deren Semantik:

```text
Finish
Drain
Pause
Migrate
Restart
```

verwenden.

Operationen dürfen nicht unkontrolliert zwischen inkompatiblen Implementierungen wechseln.

## Rollback

Scheitert die neue Version vor erfolgreicher Aktivierung:

```text
New Version
    ↓
Verification Failed
    ↓
Rollback
    ↓
Old Version
```

Nach irreversiblen Zustandsänderungen ist Rollback nur zulässig, wenn dies explizit unterstützt wird.

## Sicherheit

Neue Komponenten müssen vor ihrer Aktivierung dieselben Sicherheits-, Trust- und Capability-Regeln erfüllen wie regulär gestartete Komponenten.

Live Evolution darf keine Berechtigungen übertragen, die der neuen Version nicht zustehen.

## Abhängigkeiten

Abhängige Komponenten werden über das System Model ermittelt.

```text
Component
   ↓
Dependencies
   ↓
Compatibility Check
```

Ein Austausch darf nicht erfolgen, wenn dadurch harte Abhängigkeiten verletzt werden.

## Normative Anforderungen

1. NovaOS SOLL geeignete Komponenten ohne vollständigen Systemneustart austauschen können.
2. Live Evolution MUSS auf stabilen und versionierten Contracts basieren.
3. Neue Komponenten MÜSSEN vor Aktivierung validiert werden.
4. Zustandsübertragung MUSS explizit definiert sein, wenn Zustand erhalten werden soll.
5. Der aktive Versionswechsel SOLL atomar erfolgen.
6. Laufende Operationen MÜSSEN kontrolliert behandelt werden.
7. Abhängigkeiten MÜSSEN vor dem Austausch geprüft werden.
8. Live Evolution DARF Security-, Trust- oder Capability-Regeln nicht umgehen.
9. Fehlgeschlagene Aktivierungen SOLLEN auf die vorherige Version zurückfallen können.
10. Komponenten, die nicht sicher live ersetzt werden können, MÜSSEN einen kontrollierten Neustart verwenden.

## Abhängigkeiten

- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-ARCH-0012_Hot_Replacement`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0019_Evolvierbar`
- `ADR-STATE-0006_State_Versioning_und_History`
- `ADR-STATE-0008_State_Rollback`

## Ergebnis

```text
Stable Contracts
      ↓
Replace Implementation
      ↓
Preserve Valid State
      ↓
Verify
      ↓
Continue Execution
```

NovaOS kann sich dadurch im laufenden Betrieb weiterentwickeln, ohne Stabilität, Zustand oder Sicherheitsgrenzen unnötig aufzugeben.