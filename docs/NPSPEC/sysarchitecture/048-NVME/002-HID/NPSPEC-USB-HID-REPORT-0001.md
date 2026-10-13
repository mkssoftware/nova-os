# NPSPEC-USB-HID-REPORT-0001 – HID Report Parsing

## Status

Angenommen

## Kategorie

USB / HID / Report Descriptor

## Zweck

Diese Spezifikation definiert, wie NovaOS HID Report Descriptoren analysiert und daraus Eingabefelder ableitet.

## Parser-Regeln

Der Parser MUSS folgende Elemente verstehen:

- Main Items,
- Global Items,
- Local Items,
- Usage Pages,
- Report Size,
- Report Count,
- Logical Minimum/Maximum,
- Report ID.

## Sicherheit

Report Descriptoren sind untrusted input. Der Parser MUSS Endlosschleifen, Überläufe und Pufferüberschreitungen verhindern.

## Ausgabe

Der Parser erzeugt eine interne Report Map, die Report-Bits auf semantische Eingabefelder abbildet.

## Akzeptanzkriterien

- Boot-Keyboard-Reports können beschrieben werden.
- Mausreports mit X/Y/Button-Feldern werden erkannt.
- Ungültige Größen oder Counts werden abgewiesen.

