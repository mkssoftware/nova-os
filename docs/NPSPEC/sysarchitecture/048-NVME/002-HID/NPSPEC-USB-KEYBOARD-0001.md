# NPSPEC-USB-KEYBOARD-0001 – USB Tastatur

## Status

Angenommen

## Kategorie

USB / HID / Tastatur

## Zweck

Diese Spezifikation definiert die Verarbeitung von USB-HID-Tastaturen in NovaOS.

## Eingabemodell

USB-Tastaturberichte MÜSSEN in ein geräteunabhängiges Key-Event-Modell übersetzt werden.

## Anforderungen

- Modifier-Tasten MÜSSEN separat geführt werden.
- Key-Press und Key-Release MÜSSEN unterscheidbar sein.
- Ghosting oder ungültige Keycodes DÜRFEN keine falschen Systemaktionen auslösen.
- Layout-Übersetzung MUSS außerhalb des USB-Treibers erfolgen.

## Frühboot

Für Bootmanager und frühe Kernelphasen darf das HID-Boot-Keyboard-Protokoll verwendet werden.

## Akzeptanzkriterien

- Standardtasten, Enter, Escape und Pfeiltasten werden erkannt.
- Modifier wie Shift, Ctrl und Alt werden korrekt gemeldet.
- Entfernen der Tastatur beendet offene Interrupt-Transfers.

