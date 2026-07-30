# NDA-0002 - Adaptive View Engine

## Status

Angenommen

## Entscheidung

Nova OS verwendet keine festen Bildschirmauflösungen als Grundlage für UI-Layouts.

Stattdessen wird eine Adaptive View Engine verwendet.

## Begründung

Nova OS soll automatisch funktionieren auf:

- Querformat
- Hochformat
- Ultra-Wide
- kleinen Displays
- großen Displays
- hoher DPI
- Touch-Geräten

## Referenz

Die Referenzauflösung für Skalierung ist:

```text
1920 x 1080