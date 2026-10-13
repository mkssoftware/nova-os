# NPSPEC-USB-HID-0001 – USB HID

## Status

Angenommen

## Kategorie

USB / HID / Eingabe

## Zweck

Diese Spezifikation definiert die allgemeine USB-HID-Unterstützung für Tastaturen, Mäuse und weitere Human Interface Devices.

## Modell

HID-Geräte werden über Interfaces gebunden. Ein HID-Class-Driver MUSS HID Descriptor, Report Descriptor und Interrupt-Endpunkte auswerten.

## Anforderungen

- HID-Geräte MÜSSEN ohne gerätespezifische Sonderpfade enumerierbar sein.
- Report Descriptoren MÜSSEN validiert werden.
- Eingabedaten MÜSSEN in NovaOS Input Events übersetzt werden.
- Fehlerhafte Reports DÜRFEN den Input Stack nicht blockieren.

## Boot-Protokoll

NovaOS DARF USB-HID-Boot-Protokoll für frühe Tastatur- und Mausunterstützung verwenden, MUSS aber langfristig Report-Descriptor-basiertes HID unterstützen.

## Akzeptanzkriterien

- Eine HID-Tastatur kann Eingabeevents erzeugen.
- Eine HID-Maus kann Pointer-Events erzeugen.
- Ungültige Report Descriptoren führen zu kontrolliertem Gerätefehler.

