# NPSPEC-USB-CONTROLLER-0001 – USB Controller Abstraktion

## Status

Angenommen

## Kategorie

USB / Controller / HAL

## Zweck

Diese Spezifikation definiert die controllerunabhängige Schnittstelle, über die der USB Core mit Host-Controller-Treibern kommuniziert.

## Controller-Vertrag

Ein USB Host Controller MUSS folgende Operationen anbieten:

- `start`
- `stop`
- `reset`
- `enumerate_port`
- `address_device`
- `configure_endpoint`
- `submit_transfer`
- `cancel_transfer`

## Zustandsmodell

```text
Discovered
  -> Initialized
  -> Running
  -> Degraded
  -> Stopped
```

Ein Controller darf Transfers nur im Zustand `Running` annehmen.

## Integration

Controller-Treiber MÜSSEN mit Driver Discovery, Interrupt Manager, DMA Mapping und Boot Health integriert sein.

## Isolation

Controller-spezifische Register und DMA-Strukturen DÜRFEN nicht an Class Driver weitergegeben werden.

## Akzeptanzkriterien

- Der USB Core kann Controller verschiedener Implementierungen gleich behandeln.
- Controllerfehler werden auf Bus- oder Portebene isoliert.
- Stop/Reset räumt ausstehende Transfers kontrolliert ab.

