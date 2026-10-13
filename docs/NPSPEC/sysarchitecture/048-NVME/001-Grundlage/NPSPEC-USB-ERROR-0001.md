# NPSPEC-USB-ERROR-0001 – USB Fehlerbehandlung

## Status

Angenommen

## Kategorie

USB / Fehler / Recovery

## Zweck

Diese Spezifikation definiert Fehlerklassen, Recovery und Diagnose für USB.

## Fehlerklassen

NovaOS MUSS mindestens unterscheiden:

- DescriptorError,
- EnumerationTimeout,
- TransferTimeout,
- Stall,
- Babble,
- CRCError,
- Disconnect,
- ControllerHalt,
- DMAFault.

## Recovery

Recovery MUSS gestuft erfolgen:

```text
Transfer Retry
  -> Endpoint Reset
  -> Interface Rebind
  -> Device Reset
  -> Port Disable
  -> Controller Recovery
```

## Diagnose

USB-Fehler MÜSSEN strukturierte Diagnoseeinträge mit Bus, Port, Device, Endpoint und Statuscode erzeugen.

## Akzeptanzkriterien

- Ein STALL kann über ClearFeature behandelt werden.
- Wiederholte Fehler führen zu Degradation statt Endlosschleife.
- Controllerfehler erzeugen Boot-/Runtime-Health-Einträge.

