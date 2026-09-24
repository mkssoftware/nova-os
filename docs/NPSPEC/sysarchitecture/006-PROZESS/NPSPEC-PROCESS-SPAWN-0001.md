# NPSPEC-PROCESS-SPAWN-0001 – Nova Process Spawn

## Status

Entwurf

## Kategorie

Process / Spawn / Execution / Kernel

## Zweck

NovaOS definiert einen kontrollierten Mechanismus zur Erzeugung neuer Prozesse.

```text
Spawn Request
     ↓
Validate
     ↓
Create Process
     ↓
Initialize
     ↓
Start
```

## Grundprinzip

NovaOS verwendet ein direktes Spawn-Modell.

```text
Executable / Capability
        ↓
Spawn
        ↓
New Process
```

Ein bestehender Prozess muss nicht zunächst dupliziert werden.

```text
Spawn ≠ fork() + exec()
```

POSIX-`fork()` kann durch eine Compatibility Personality bereitgestellt werden, bestimmt jedoch nicht das native NovaOS-Prozessmodell.

## Spawn Request

Eine Spawn-Anforderung beschreibt mindestens:

```text
Execution Target
Arguments
Environment
Security Context
Capabilities
Resource Requirements
Execution Contract
Parent / Supervisor
```

Optionale Eigenschaften können abhängig vom Ausführungsziel ergänzt werden.

## Ablauf

```text
Spawn Request
      ↓
Validate Target
      ↓
Validate Authority
      ↓
Resolve Execution Target
      ↓
Create Process Object
      ↓
Create Address Space
      ↓
Assign Security Context
      ↓
Assign Capabilities
      ↓
Allocate Resources
      ↓
Load Execution Image
      ↓
Create Initial Thread
      ↓
Start Process
```

Der Prozess darf erst sichtbar als laufend gelten, wenn die erforderliche Initialisierung erfolgreich abgeschlossen wurde.

## Berechtigungen

Ein aufrufender Prozess darf einem neuen Prozess keine Rechte übertragen, die er selbst nicht delegieren darf.

```text
Parent Authority
       ↓
Delegation
       ↓
Child Authority
```

Dabei gilt:

```text
Spawn ≠ Privilege Escalation
```

Capabilities können beim Spawn eingeschränkt werden.

## Ressourcen

Der Spawn kann Ressourcenanforderungen über einen Execution Contract definieren.

Beispiele:

```text
Memory Budget
CPU Budget
Latency
Execution Location
Determinism
Device Requirements
```

Nicht erfüllbare Hard Requirements müssen zum Abbruch des Spawn-Vorgangs führen.

## Isolation

Der neue Prozess erhält grundsätzlich eine neue Prozess- und Speicherisolation.

Gemeinsame Ressourcen müssen explizit übertragen oder freigegeben werden.

```text
Parent Process
      │
      ├── explicit shared resources
      │
      ↓
Child Process
```

Eine Parent-Beziehung erzeugt keine automatische Speicherfreigabe.

## Handles und Capabilities

Handles oder Capabilities werden nur übertragen, wenn dies explizit vorgesehen und autorisiert ist.

Mögliche Übergabeformen:

```text
Delegate
Attenuate
Share
Transfer
```

Implizite Vererbung soll vermieden werden.

## Fehlerbehandlung

Schlägt der Spawn vor erfolgreichem Start fehl:

```text
Spawn
  ↓
Failure
  ↓
Abort Initialization
  ↓
Release Resources
  ↓
Return Error
```

Ein teilweise erzeugter Prozess darf nicht als normal laufender Prozess zurückbleiben.

## Parent und Supervisor

Der neue Prozess kann einem Parent oder Supervisor zugeordnet werden.

```text
Supervisor
    ↓
Process
```

Diese Beziehung dient Lifecycle-, Fehler- und Verwaltungsfunktionen, erzeugt aber keine automatischen Zugriffsrechte.

## Normative Anforderungen

1. NovaOS MUSS einen nativen Spawn-Mechanismus bereitstellen.
2. Der native Spawn DARF NICHT von POSIX-`fork()` abhängig sein.
3. Spawn Requests MÜSSEN vor der Prozesserzeugung validiert werden.
4. Das Ausführungsziel MUSS vor dem Start validiert werden.
5. Ein Spawn DARF keine unautorisierte Rechteerweiterung ermöglichen.
6. Capabilities DÜRFEN nur kontrolliert delegiert oder übertragen werden.
7. Hard Requirements eines Execution Contracts MÜSSEN eingehalten werden.
8. Neue Prozesse MÜSSEN standardmäßig eine eigene Isolationsdomäne erhalten.
9. Gemeinsame Ressourcen MÜSSEN explizit definiert werden.
10. Fehlgeschlagene Spawn-Vorgänge MÜSSEN reservierte Ressourcen bereinigen.
11. Teilweise initialisierte Prozesse DÜRFEN NICHT unkontrolliert weiterlaufen.
12. Parent- und Supervisor-Beziehungen DÜRFEN NICHT automatisch zusätzliche Berechtigungen erzeugen.

## Abhängigkeiten

- `NPSPEC-PROCESS-MODEL-0001`
- `NPSPEC-PROCESS-LIFECYCLE-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-PROCESS-0005`

## Ergebnis

```text
Execution Request
       ↓
Validate
       ↓
Controlled Spawn
       ↓
Isolated Process
       ↓
Initial Thread
       ↓
Running
```

NovaOS erhält damit einen direkten, sicheren und ressourcenkontrollierten Mechanismus zur Prozesserzeugung, ohne das native Prozessmodell an klassische `fork()`-Semantik zu binden.