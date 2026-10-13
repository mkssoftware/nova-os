# NPSPEC-NVME-POWER-0001 – NVMe Power Management

## Status

Angenommen

## Kategorie

NVMe / Power / Energieverwaltung

## Zweck

Diese Spezifikation definiert Energieverwaltung für NVMe Controller und Namespaces.

## Anforderungen

- Power States MÜSSEN über Identify Controller ermittelt werden.
- Aktiver Power State DARF nur nach Queue-Drain oder geeigneter Controllerregel geändert werden.
- Suspend/Resume MUSS Queues und Controllerzustand konsistent wiederherstellen.
- Thermal- und Power-Warnungen MÜSSEN an Health/Telemetry gemeldet werden.

## Akzeptanzkriterien

- Controller kann in einen niedrigeren Power State wechseln.
- Resume stellt Admin und I/O Queues wieder her.
- Power-Fehler führen zu kontrollierter Degradation.

