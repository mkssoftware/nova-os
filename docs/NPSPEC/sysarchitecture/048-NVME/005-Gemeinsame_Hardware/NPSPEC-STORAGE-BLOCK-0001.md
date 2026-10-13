# NPSPEC-STORAGE-BLOCK-0001 – Storage Block Layer

## Status

Angenommen

## Kategorie

Storage / Block / Kernel

## Zweck

Diese Spezifikation definiert die gemeinsame Blockgeräte-Schicht für NVMe, USB-Massenspeicher und weitere Storage-Treiber.

## Blockgerät

Ein Blockgerät MUSS melden:

- Geräte-ID,
- Blockgröße,
- Blockanzahl,
- Read-only-Status,
- Removable-Status,
- Flush-Fähigkeit,
- Health-Status.

## I/O

Block-I/O MUSS als strukturierter Request mit LBA, Länge, Richtung, Puffer, Timeout und Completion beschrieben werden.

## Anforderungen

- Out-of-range Requests MÜSSEN vor Treiberübergabe abgewiesen werden.
- Schreibzugriffe auf Read-only-Geräte MÜSSEN fehlschlagen.
- Flush-Semantik MUSS erhalten bleiben.

## Akzeptanzkriterien

- NVMe Namespace und USB-Stick erscheinen gleichartig als Blockgerät.
- I/O-Fehler werden einheitlich gemeldet.
- Blockgeräte können offline gesetzt werden.

