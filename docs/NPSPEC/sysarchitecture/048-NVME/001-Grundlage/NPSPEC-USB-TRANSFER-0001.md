# NPSPEC-USB-TRANSFER-0001 – USB Transfers

## Status

Angenommen

## Kategorie

USB / Transfer / I/O

## Zweck

Diese Spezifikation definiert das Transfermodell für USB-I/O in NovaOS.

## Transferarten

NovaOS MUSS folgende Transferarten abbilden:

- Control Transfer,
- Bulk Transfer,
- Interrupt Transfer,
- Isochronous Transfer.

## Transfer-Objekt

Ein Transfer MUSS enthalten:

- Endpoint Handle,
- Richtung,
- Pufferreferenz,
- Länge,
- Timeout,
- Completion Callback oder Completion Queue,
- Status.

## Completion

Transfers enden mit genau einem Completion-Status. Doppelte Completion ist unzulässig.

## DMA

Transferpuffer MÜSSEN vor Übergabe an den Host Controller DMA-sicher gemappt werden. Cache-Kohärenz MUSS durch die DMA-Schicht sichergestellt werden.

## Akzeptanzkriterien

- Control Transfers unterstützen Setup-, Data- und Status-Phase.
- Cancelled Transfers liefern einen definierten Abbruchstatus.
- Short Transfers werden gemäß Endpoint- und Transferregel bewertet.

