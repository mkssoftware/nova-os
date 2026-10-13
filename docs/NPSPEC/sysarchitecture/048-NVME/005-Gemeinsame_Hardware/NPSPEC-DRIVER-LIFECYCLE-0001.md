# NPSPEC-DRIVER-LIFECYCLE-0001 – Driver Lifecycle

## Status

Angenommen

## Kategorie

Driver / Lifecycle / Kernel

## Zweck

Diese Spezifikation definiert den Lebenszyklus von Hardwaretreibern in NovaOS.

## Zustände

```text
Discovered
  -> Matched
  -> Loaded
  -> Bound
  -> Running
  -> Suspended
  -> Stopping
  -> Unbound
  -> Failed
```

## Anforderungen

- `probe` darf Geräte nur validieren, nicht dauerhaft aktivieren.
- `bind` initialisiert Ressourcen.
- `start` aktiviert I/O.
- `stop` beendet neue I/O und drainiert laufende Operationen.
- `unbind` gibt Ressourcen frei.

## Fehlerbehandlung

Fehler in einem Treiber DÜRFEN nicht den gesamten Driver Manager blockieren.

## Akzeptanzkriterien

- Ein Treiber kann kontrolliert gestartet und gestoppt werden.
- Hotplug ruft Unbind/Stop in sicherer Reihenfolge auf.
- Fehlerzustände sind diagnostizierbar.

