# NPSPEC-NVME-ARCHITECTURE-0001 – NVMe Architektur

## Status

Angenommen

## Kategorie

NVMe / Architektur / Storage

## Zweck

Diese Spezifikation definiert die NVMe-Architektur von NovaOS für PCIe-basierte Non-Volatile-Memory-Geräte.

## Architekturmodell

```text
PCI Discovery
  -> NVMe Controller Driver
  -> Admin Queue
  -> Identify / Namespace Discovery
  -> I/O Queue Pairs
  -> Block Device
  -> Storage Stack
```

## Anforderungen

- NVMe Controller MÜSSEN über PCI Discovery gefunden werden.
- Controllerinitialisierung MUSS Admin Queue, Controller Enable und Identify umfassen.
- Namespaces MÜSSEN als Blockgeräte registriert werden.
- I/O MUSS über Submission- und Completion-Queues laufen.

## Sicherheitsregeln

Controllerdaten, Identify-Strukturen und Completion-Status MÜSSEN validiert werden. DMA-Puffer MÜSSEN über die NovaOS-DMA-Schicht gemappt werden.

## Akzeptanzkriterien

- Ein NVMe-Controller kann initialisiert werden.
- Mindestens ein Namespace wird erkannt.
- Read/Write-I/O kann über Queue Pairs completed werden.

