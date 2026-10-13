# NPSPEC-NVME-IO-0001 – NVMe I/O

## Status

Angenommen

## Kategorie

NVMe / I/O / Storage

## Zweck

Diese Spezifikation definiert Read, Write und Flush für NVMe-Namespaces.

## I/O-Pfad

```text
Block Request
  -> Namespace Mapping
  -> DMA Map
  -> NVMe Command
  -> Submission Queue
  -> Completion Queue
  -> Block Completion
```

## Anforderungen

- LBA und Transferlänge MÜSSEN gegen Namespace-Größe geprüft werden.
- Puffer MÜSSEN DMA-sicher gemappt werden.
- Flush MUSS Write-Cache-Semantik berücksichtigen.
- Short oder Partial Completion ist unzulässig und MUSS als Fehler gelten.

## Akzeptanzkriterien

- Read/Write Requests werden completed.
- Out-of-range I/O wird vor Submission abgewiesen.
- Flush liefert einen definierten Completion-Status.

