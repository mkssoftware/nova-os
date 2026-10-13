# NPSPEC-USB-BLOCKDEVICE-0001 – USB Block Device Integration

## Status

Angenommen

## Kategorie

USB / Blockgerät / Storage Integration

## Zweck

Diese Spezifikation definiert, wie USB-Massenspeicher als NovaOS-Blockgeräte registriert werden.

## Blockgerät-Modell

Ein USB-Blockgerät MUSS folgende Eigenschaften melden:

- stabile Geräte-ID,
- Blockgröße,
- Blockanzahl,
- Read-only-Status,
- Removable-Status,
- Transporttyp.

## I/O-Verhalten

Block-I/O MUSS in SCSI-Kommandos übersetzt und über BOT oder UASP transportiert werden.

## Hotplug

Beim Disconnect MUSS das Blockgerät offline gehen und offene I/O-Operationen abbrechen.

## Akzeptanzkriterien

- USB-Massenspeicher erscheinen im Storage-Block-Stack.
- Read-only-Medien verhindern Schreibzugriffe.
- Medium Removal erzeugt ein Storage-Hotplug-Event.

