# NPSPEC-USB-XHCI-0001 – xHCI Host Controller

## Status

Angenommen

## Kategorie

USB / xHCI / Controller

## Zweck

Diese Spezifikation definiert den NovaOS-Treiber für xHCI-kompatible USB-Host-Controller.

## Initialisierung

Der xHCI-Treiber MUSS:

1. PCI BARs und Capability Register validieren,
2. Controller stoppen und resetten,
3. DCBAA, Command Ring, Event Ring und Scratchpad-Strukturen anlegen,
4. Interrupts konfigurieren,
5. Root-Hub-Ports an den USB Core melden.

## Speicher und DMA

Alle xHCI-Ringe MÜSSEN DMA-sicher alloziert werden. Ringadressen MÜSSEN physisch adressierbar und korrekt ausgerichtet sein.

## Ringmodell

```text
Command Ring -> Controller Commands
Transfer Ring -> Endpoint Transfers
Event Ring    -> Completion Events
```

TRBs DÜRFEN erst sichtbar gemacht werden, nachdem ihre Inhalte vollständig geschrieben wurden. Cycle Bits MÜSSEN korrekt verwaltet werden.

## Fehlerbehandlung

Command- und Transfer-Completion-Codes MÜSSEN in NovaOS-Statuscodes übersetzt werden. Unbekannte Completion-Codes MÜSSEN als kontrollierter Controllerfehler behandelt werden.

## Akzeptanzkriterien

- Controller Reset führt zu einem definierten Ready-Zustand.
- Command Ring kann Enable Slot, Address Device und Configure Endpoint ausführen.
- Event Ring liefert Completion Events ohne Polling-Endlosschleife.

