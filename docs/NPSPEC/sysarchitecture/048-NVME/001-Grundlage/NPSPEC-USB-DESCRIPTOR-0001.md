# NPSPEC-USB-DESCRIPTOR-0001 – USB Descriptor Parsing

## Status

Angenommen

## Kategorie

USB / Descriptor / Validierung

## Zweck

Diese Spezifikation legt fest, wie NovaOS USB-Descriptoren liest, validiert und in interne Objektstrukturen überführt.

## Descriptor-Typen

NovaOS MUSS mindestens folgende Descriptoren unterstützen:

- Device Descriptor,
- Configuration Descriptor,
- Interface Descriptor,
- Endpoint Descriptor,
- String Descriptor,
- HID Descriptor,
- SuperSpeed Companion Descriptor.

## Validierung

Descriptor Parser MÜSSEN prüfen:

- Mindestlänge,
- Typfeld,
- Gesamtlänge,
- verschachtelte Grenzen,
- Endpoint-Richtung,
- Endpoint-Typ,
- Interface-Nummer und Alternate Setting.

## Sicherheitsregel

Descriptor-Daten sind untrusted input. Parser DÜRFEN niemals außerhalb des gelieferten Puffers lesen.

## Akzeptanzkriterien

- Korrekte Descriptor-Bäume werden als interne Strukturen sichtbar.
- Ungültige Längen werden abgewiesen.
- Unbekannte optionale Descriptoren können übersprungen werden.

