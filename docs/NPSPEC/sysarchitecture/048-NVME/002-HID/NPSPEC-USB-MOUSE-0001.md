# NPSPEC-USB-MOUSE-0001 – USB Maus

## Status

Angenommen

## Kategorie

USB / HID / Maus

## Zweck

Diese Spezifikation definiert die Verarbeitung von USB-HID-Mäusen und kompatiblen Pointer-Geräten.

## Eingabedaten

Mausreports MÜSSEN mindestens unterstützen:

- relative X/Y-Bewegung,
- Tastenstatus,
- Scrollrad, sofern vorhanden.

## Anforderungen

- Pointer-Deltas MÜSSEN in NovaOS Pointer Events übersetzt werden.
- Beschleunigung und Zeigergeschwindigkeit gehören zur Input-Policy, nicht zum USB-Treiber.
- Überläufe und ungültige Reports MÜSSEN verworfen werden.

## Akzeptanzkriterien

- Bewegung erzeugt relative Pointer Events.
- Linke, rechte und mittlere Taste werden erkannt.
- Disconnect stoppt Polling ohne Kernel-Hänger.

