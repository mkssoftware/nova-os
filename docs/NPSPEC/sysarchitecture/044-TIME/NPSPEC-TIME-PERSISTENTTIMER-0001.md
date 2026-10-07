# NPSPEC-TIME-PERSISTENTTIMER-0001 – Nova Persistent Timer

## Status

Angenommen

## Kategorie

Time / Persistent Timer

## Zweck

NovaOS definiert persistente Timer für zeitgesteuerte Ereignisse, die Neustart, Shutdown, Suspend oder längere Inaktivität des Systems überstehen müssen.

Persistente Timer speichern ihre zeitliche Absicht dauerhaft und werden nach Wiederherstellung des Systems gegen die aktuelle Zeit neu bewertet.

## Grundprinzipien

```text
Persistent Timer ≠ Running Timer
Persistent Timer ≠ RTC Alarm
Persistent Timer ≠ Background Process
Persistence ≠ Guaranteed Wakeup
Deadline ≠ Execution Guarantee
System Off ≠ Timer Lost
```

## Modell

```text
PersistentTimer
├── TimerID
├── OwnerID
├── ClockSemantic
├── TargetTime
├── Tolerance
├── WakePolicy
├── MissPolicy
├── PayloadReference
├── Generation
└── State
```

## Lebenszyklus

```text
Create
  ↓
Persist
  ↓
Armed
  ↓
System Offline / Suspend / Restart
  ↓
Restore
  ↓
Revalidate
  ↓
Execute / Reschedule / Expire / Cancel
```

Der Timer muss nicht während ausgeschalteter Systemzustände aktiv ausgeführt werden.

## Zeitsemantik

Persistente Timer müssen explizit definieren, auf welcher Zeitsemantik sie beruhen.

Typische Varianten:

```text
Absolute UTC Instant
Civil Time Event
Elapsed-Time Intent
Recurring Civil Schedule
```

Eine reine monotone Deadline kann einen vollständigen Shutdown nicht ohne zusätzliche persistierte Referenz überstehen.

## Persistenz

Persistiert werden nur die zur Rekonstruktion notwendigen Informationen.

Runtime-spezifische Objekte wie:

```text
Process Handle
Thread Pointer
Kernel Timer Object
Temporary Capability Handle
```

dürfen nicht als dauerhafte Timer-Identität verwendet werden.

## Wiederherstellung

Beim Boot oder Resume:

```text
Load Persistent Timers
        ↓
Validate
        ↓
Resolve Time Semantics
        ↓
Compare Current Time
        ↓
Rearm or Apply MissPolicy
```

## Verpasste Timer

Liegt die Zielzeit bereits in der Vergangenheit, entscheidet die `MissPolicy`.

Unterstützte Strategien können sein:

```text
ExecuteImmediately
Skip
Reschedule
Coalesce
Expire
```

Die Policy muss vor Persistierung eindeutig festgelegt sein.

## Wakeup

Ein persistenter Timer darf eine Wake-Anforderung besitzen:

```text
NoWake
WakeIfSupported
WakeRequired
```

Falls Plattform und Hardware dies unterstützen, darf NovaOS einen geeigneten RTC-, Firmware- oder Platform-Wake-Mechanismus programmieren.

```text
Persistent Timer
      ↓
Wake Policy
      ↓
Platform Wake Provider
      ↓
RTC / Firmware / Hardware
```

`WakeRequired` bedeutet nicht, dass ausgeschaltete oder physisch stromlose Hardware garantiert geweckt werden kann.

## Änderungen der Wall Clock

Bei Änderung oder Synchronisation der Wall Clock müssen absolute Timer anhand ihrer ursprünglichen Zeitsemantik neu bewertet werden.

Monotone und Civil-Time-Semantik dürfen dabei nicht miteinander vermischt werden.

## Sicherheit

Ein wiederhergestellter Timer besitzt nicht automatisch die früheren Runtime-Berechtigungen seines Owners.

Vor Ausführung müssen Identität, Policy und erforderliche Authority erneut validiert werden.

Persistierte Timerdaten müssen gegen Manipulation geschützt werden.

## Normative Anforderungen

1. NovaOS MUSS persistente Timer unterstützen können.
2. Persistente Timer MÜSSEN Neustarts und Shutdown logisch überstehen können.
3. Jeder persistente Timer MUSS seine Zeitsemantik explizit definieren.
4. Runtime-Handles DÜRFEN nicht als persistente Identität verwendet werden.
5. Timer MÜSSEN nach Wiederherstellung erneut validiert werden.
6. Bereits verpasste Timer MÜSSEN über eine definierte MissPolicy behandelt werden.
7. Persistente Timer MÜSSEN mit Plattform-Wake-Mechanismen integrierbar sein.
8. Wake-Anforderungen DÜRFEN keine nicht vorhandene Hardwarefähigkeit voraussetzen.
9. Änderungen der Wall Clock MÜSSEN entsprechend der Timer-Semantik behandelt werden.
10. Persistierte Timerdaten MÜSSEN gegen unautorisierte Manipulation geschützt werden.
11. Frühere Runtime-Authority DARF nach Neustart nicht automatisch wiederhergestellt werden.
12. TimerID, Zielzeit, Zeitsemantik, WakePolicy, MissPolicy und Zustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-CIVIL-0001`
- `NPSPEC-TIME-RTC-0001`
- `NPSPEC-TIME-TIMER-0001`
- `NPSPEC-TIME-DEADLINE-0001`
- `NPSPEC-TIME-SYNCHRONIZATION-0001`
- `NPSPEC-POWER-WAKE-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann zeitgesteuerte Ereignisse dauerhaft über Neustarts, Suspend und ausgeschaltete Systemzustände hinweg erhalten. Persistente Timer werden anhand ihrer ursprünglichen Zeitsemantik rekonstruiert, sicher revalidiert und bei Bedarf mit vorhandenen Hardware-Wake-Mechanismen verbunden.