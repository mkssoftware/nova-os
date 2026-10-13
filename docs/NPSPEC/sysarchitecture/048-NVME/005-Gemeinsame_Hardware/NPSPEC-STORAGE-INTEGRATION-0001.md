# NPSPEC-STORAGE-INTEGRATION-0001 – Storage Integration

## Status

Angenommen

## Kategorie

Storage / Integration / NovaOS

## Zweck

Diese Spezifikation definiert die Integration von Hardware-Storage in NovaOS VFS, NovaFS, Diagnostics und Boot.

## Integrationspunkte

Storage-Treiber MÜSSEN integrieren mit:

- Driver Framework,
- DMA Mapping,
- Interrupt Manager,
- Block Layer,
- VFS/NovaFS,
- Metrics,
- Diagnostics,
- Boot Health.

## Bootfähigkeit

Ein Storage-Gerät darf als Bootmedium verwendet werden, wenn es vor Kernel-Handoff oder in der frühen Kernelphase eindeutig identifiziert und lesbar ist.

## Fehler und Health

Storage-Fehler MÜSSEN sowohl requestbezogen als auch gerätebezogen sichtbar sein. Health-Daten SOLLEN für UI und Diagnose bereitstehen.

## Akzeptanzkriterien

- NVMe und USB-Massenspeicher liefern Blockgeräte an denselben Stack.
- Fehler sind in Diagnostics sichtbar.
- Boot- und Runtime-Storage verwenden konsistente Identitätsmodelle.

