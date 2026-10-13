# NPSPEC-NVME-QUEUE-0001 – NVMe Queues

## Status

Angenommen

## Kategorie

NVMe / Queue / I/O

## Zweck

Diese Spezifikation definiert Submission Queues und Completion Queues für NVMe.

## Queue-Modell

```text
Submission Queue
  -> Doorbell
  -> Controller
  -> Completion Queue
  -> Interrupt/Poll
```

## Anforderungen

- Admin Queue Pair MUSS vor allen I/O Queues existieren.
- I/O Queue Pairs MÜSSEN DMA-sicher und ausgerichtet sein.
- Queue Depth MUSS controllerkonform begrenzt werden.
- Phase Tags MÜSSEN korrekt ausgewertet werden.
- Doorbell Writes DÜRFEN erst nach Command Write sichtbar werden.

## Completion

Jede Command ID darf nur eine aktive Operation pro Queue referenzieren. Completion muss Queue, Command ID und Status validieren.

## Akzeptanzkriterien

- Admin Commands werden completed.
- I/O Commands können parallel ausstehen.
- Queue Full wird kontrolliert zurückgegeben.

