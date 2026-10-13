# NPSPEC-NVME-HEALTH-0001 – NVMe Health und Telemetrie

## Status

Angenommen

## Kategorie

NVMe / Health / Observability

## Zweck

Diese Spezifikation definiert Health-Erfassung und Telemetrie für NVMe-Geräte.

## Health-Daten

NovaOS SOLL mindestens erfassen:

- SMART / Health Information,
- Temperatur,
- Available Spare,
- Percentage Used,
- Data Units Read/Written,
- Media and Data Integrity Errors,
- Unsafe Shutdowns.

## Integration

Health-Daten MÜSSEN in Diagnostics, Metrics und Storage-Health einfließen.

## Akzeptanzkriterien

- SMART/Health kann über Admin Command gelesen werden.
- Kritische Warnungen erzeugen strukturierte Events.
- Health-Daten können ohne laufende I/O-Blockade abgefragt werden.

