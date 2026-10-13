# NPSPEC-NVME-DMA-0001 – NVMe DMA

## Status

Angenommen

## Kategorie

NVMe / DMA / Speicher

## Zweck

Diese Spezifikation definiert DMA-Regeln für NVMe Commands, Queues und Datenpuffer.

## Anforderungen

- Queues MÜSSEN DMA-sicher, ausgerichtet und controllerkonform alloziert werden.
- Datenpuffer MÜSSEN vor Submission gemappt und nach Completion entmappt werden.
- PRP-Listen oder SGLs MÜSSEN physische Segmentgrenzen korrekt abbilden.
- IOMMU-Domains MÜSSEN, falls verfügbar, genutzt werden.

## Sicherheit

Der Controller darf nur Zugriff auf explizit gemappte Speicherbereiche erhalten.

## Akzeptanzkriterien

- Nicht zusammenhängende Puffer können über PRP/SGL beschrieben werden.
- DMA-Faults führen zu Request-Fehler und Diagnose.
- Unmap erfolgt auch bei Timeout und Cancel.

