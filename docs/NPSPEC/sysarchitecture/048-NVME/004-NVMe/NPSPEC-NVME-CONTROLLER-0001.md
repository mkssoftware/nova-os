# NPSPEC-NVME-CONTROLLER-0001 – NVMe Controller

## Status

Angenommen

## Kategorie

NVMe / Controller / PCIe

## Zweck

Diese Spezifikation definiert Initialisierung und Verwaltung eines NVMe-Controllers.

## Initialisierung

Der Treiber MUSS:

1. PCI BAR und Registerlayout validieren,
2. Controller deaktivieren,
3. Admin Submission/Completion Queue anlegen,
4. AQA, ASQ und ACQ programmieren,
5. Controller aktivieren,
6. CSTS.RDY abwarten,
7. Identify Controller ausführen.

## Zustände

```text
Discovered -> Disabled -> AdminReady -> Running -> Degraded -> Failed
```

## Anforderungen

- Registerzugriffe MÜSSEN mit Memory-Barriers abgesichert werden.
- Timeouts MÜSSEN Controller-Hänger erkennen.
- Controller Reset MUSS alle Queues invalidieren.

## Akzeptanzkriterien

- CAP, VS, CC und CSTS werden korrekt gelesen.
- Enable/Disable endet in definiertem Zustand.
- Identify Controller liefert plausible Daten.

