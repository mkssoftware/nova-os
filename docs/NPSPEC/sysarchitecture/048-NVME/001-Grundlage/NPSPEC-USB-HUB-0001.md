# NPSPEC-USB-HUB-0001 – USB Hub Support

## Status

Angenommen

## Kategorie

USB / Hub / Topologie

## Zweck

Diese Spezifikation definiert Unterstützung für Root Hubs und externe USB Hubs.

## Hub-Modell

```text
UsbHub
├── Port[1]
├── Port[2]
└── Port[n]
```

Jeder Port besitzt einen eigenen Zustand und kann unabhängig enumeriert werden.

## Port-Zustände

NovaOS MUSS mindestens folgende Port-Zustände unterscheiden:

- Empty,
- Connected,
- Debouncing,
- Resetting,
- Enabled,
- Suspended,
- Error.

## Anforderungen

- Port-Change-Bits MÜSSEN zuverlässig abgearbeitet werden.
- Hub-Interrupt-Endpunkte MÜSSEN zur Änderungserkennung genutzt werden.
- Stromversorgung und Port-Reset DÜRFEN andere Ports nicht unnötig stören.

## Akzeptanzkriterien

- Geräte hinter einem externen Hub werden enumeriert.
- Disconnect entfernt alle nachgeordneten Geräteobjekte.
- Hubfehler isolieren den betroffenen Hubzweig.

