# NPSPEC-TEMP-PROCESS-0001 – Nova Process Temporary Resources

## Status

Angenommen

## Kategorie

Temporary Resources / Process

## Zweck

NovaOS definiert temporäre Ressourcen, deren Lebensdauer an einen Prozess gebunden ist.

Process-Temp ermöglicht kurzlebige Dateien, Speicherbereiche und andere Ressourcen, die nach Ende des Prozesses nicht dauerhaft bestehen bleiben müssen.

## Grundprinzipien

```text
Process Temp ≠ User Data
Process Temp ≠ Program Data
Process Temp ≠ Global Temp
Process Lifetime → Temp Lifetime
Process Exit → Resource Cleanup
```

## Prozessbindung

Jede Process-Temp-Ressource wird eindeutig einem Prozess zugeordnet:

```text
ProcessID
   ↓
Temp Scope
   ↓
TempResource
```

Andere Prozesse erhalten dadurch nicht automatisch Zugriff.

## Lebenszyklus

```text
Process Start
     ↓
Allocate Temp Resource
     ↓
Use
     ↓
Release
     ↓
Process Exit
     ↓
Cleanup Remaining Resources
```

Ein Prozess darf Ressourcen bereits vor seinem Ende explizit freigeben.

## Ressourcentypen

Process-Temp kann unter anderem enthalten:

```text
Temporary Files
Buffers
Shared Memory
Intermediate Results
Generated Data
IPC Resources
```

Gemeinsam genutzte Ressourcen benötigen eine explizite Freigabe oder Delegation an andere Prozesse.

## Speicherung

Der Prozess darf keine bestimmte physische Temp-Position voraussetzen.

```text
Temp Request
     ↓
Temp Service
     ↓
Storage / Memory Selection
     ↓
Authorized Resource
```

NovaOS entscheidet abhängig von Ressourcentyp, Größe, Sicherheitsanforderungen und Ressourcenlage über die tatsächliche Ablage.

## Prozessende

Bei normalem Ende, Absturz oder erzwungener Beendigung werden verbleibende Process-Temp-Ressourcen freigebbar.

```text
Running → Exited
            ↓
       Reclaimable
            ↓
        Destroyed
```

Die Bereinigung darf verzögert erfolgen, solange die Ressource nicht erneut einem fremden Prozess zugeordnet wird.

## Resource Economy

Process-Temp unterliegt Ressourcenbudgets des Prozesses.

Limits können insbesondere gelten für:

```text
Memory
Storage
Object Count
Lifetime
```

Ein Prozess darf durch temporäre Ressourcen seine zulässigen Ressourcenbudgets nicht umgehen.

## Sicherheit

Process-Temp übernimmt den Sicherheitskontext des erzeugenden Prozesses.

Die Existenz einer temporären Ressource erzeugt keine zusätzliche Authority.

Sensible temporäre Daten müssen sicher bereinigbar sein.

## Normative Anforderungen

1. Process-Temp-Ressourcen MÜSSEN einem Prozess eindeutig zugeordnet sein.
2. Process-Temp MUSS gegenüber anderen Prozessen isolierbar sein.
3. Prozesse MÜSSEN temporäre Ressourcen explizit freigeben können.
4. Nach Prozessende MÜSSEN verbleibende Process-Temp-Ressourcen reclaimable werden.
5. Ein Prozess DARF keine feste physische Temp-Position voraussetzen.
6. Process-Temp MUSS Ressourcenbudgets berücksichtigen.
7. Process-Temp DARF keine zusätzliche Authority erzeugen.
8. Gemeinsam genutzte Temp-Ressourcen MÜSSEN explizit autorisiert werden.
9. Abstürze DÜRFEN keine dauerhaft verwaisten Process-Temp-Ressourcen erzeugen.
10. Sensible temporäre Ressourcen MÜSSEN sicher bereinigbar sein.

## Abhängigkeiten

- `NPSPEC-TEMP-MODEL-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-USERSPACE-TEMP-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`

## Ergebnis

NovaOS besitzt einen klar definierten temporären Ressourcenbereich pro Prozess. Ressourcen bleiben isoliert, unterliegen den Prozessbudgets und können nach Prozessende automatisch und sicher zurückgewonnen werden.