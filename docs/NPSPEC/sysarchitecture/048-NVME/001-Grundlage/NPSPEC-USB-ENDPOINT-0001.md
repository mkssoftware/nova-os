# NPSPEC-USB-ENDPOINT-0001 – USB Endpoints

## Status

Angenommen

## Kategorie

USB / Endpoint / Transfer

## Zweck

Diese Spezifikation beschreibt USB Endpoints als adressierbare Kommunikationskanäle zwischen Host und Gerät.

## Endpoint-Typen

NovaOS MUSS folgende Endpoint-Typen unterstützen:

- Control,
- Bulk,
- Interrupt,
- Isochronous.

## Eigenschaften

Endpoint-Objekte MÜSSEN enthalten:

- Device Address,
- Endpoint Number,
- Direction,
- Transfer Type,
- Max Packet Size,
- Interval,
- Toggle-/Stream-Zustand,
- Controller-spezifische Bindung.

## Lebenszyklus

Endpoints entstehen aus Descriptoren und werden beim Wechsel der Configuration oder beim Disconnect ungültig.

## Akzeptanzkriterien

- Endpoint 0 ist immer Control.
- Class Driver können Endpoints nur über Core-Handles verwenden.
- Deaktivierte Endpoints nehmen keine Transfers mehr an.

