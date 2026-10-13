# NPSPEC-NVME-ERROR-0001 – NVMe Fehlerbehandlung

## Status

Angenommen

## Kategorie

NVMe / Fehler / Recovery

## Zweck

Diese Spezifikation definiert Fehlerklassen und Recovery für NVMe.

## Fehlerklassen

NovaOS MUSS unterscheiden:

- Command Timeout,
- Invalid Completion,
- Queue Error,
- Namespace Error,
- Media Error,
- Controller Fatal Status,
- DMA Fault,
- Reset Required.

## Recovery

Recovery MUSS gestuft erfolgen:

```text
Command Retry
  -> Queue Reset
  -> Controller Reset
  -> Namespace Offline
  -> Device Failed
```

## Akzeptanzkriterien

- Timeout führt nicht zu unbegrenztem Warten.
- Fatal Controller Status deaktiviert I/O kontrolliert.
- Media Errors werden an den Storage Stack weitergereicht.

