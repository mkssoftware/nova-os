# NPSPEC-USB-BOT-0001 – USB Bulk-Only Transport

## Status

Angenommen

## Kategorie

USB / BOT / Massenspeicher

## Zweck

Diese Spezifikation definiert USB Bulk-Only Transport für Massenspeichergeräte.

## Transportmodell

BOT besteht aus:

```text
Command Block Wrapper
  -> Data Phase
  -> Command Status Wrapper
```

## Anforderungen

- CBW und CSW MÜSSEN signatur- und längenvalidiert werden.
- Tag-Werte MÜSSEN Antwort und Anfrage eindeutig verbinden.
- STALL auf Bulk-Endpunkten MUSS per ClearFeature behandelt werden.
- Reset Recovery MUSS Mass Storage Reset und Endpoint Clear umfassen.

## Akzeptanzkriterien

- READ(10) und WRITE(10) können übertragen werden.
- Falscher CSW-Tag führt zu Transportfehler.
- BOT Reset Recovery stellt einen definierten Zustand her.

