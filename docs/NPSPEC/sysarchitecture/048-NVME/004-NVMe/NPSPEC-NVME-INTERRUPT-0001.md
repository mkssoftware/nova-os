# NPSPEC-NVME-INTERRUPT-0001 – NVMe Interrupts

## Status

Angenommen

## Kategorie

NVMe / Interrupt / Completion

## Zweck

Diese Spezifikation definiert Interruptnutzung für NVMe Completion Queues.

## Anforderungen

- MSI-X SOLL bevorzugt werden.
- MSI DARF als Fallback genutzt werden.
- Legacy INTx DARF nur für Kompatibilität verwendet werden.
- Interrupt Handler MÜSSEN kurz bleiben und Completion-Arbeit an Bottom Halves oder Worker delegieren können.

## Queue-Zuordnung

Jede I/O Completion Queue SOLL einem Interrupt Vector zugeordnet werden können.

## Akzeptanzkriterien

- Completion Interrupt weckt betroffene Queue.
- Verlorene Interrupts können per Polling-Fallback erkannt werden.
- Interrupt Masking verhindert keine dauerhaften Hänger.

