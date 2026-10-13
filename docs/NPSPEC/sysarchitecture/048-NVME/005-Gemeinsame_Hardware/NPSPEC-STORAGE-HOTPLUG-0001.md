# NPSPEC-STORAGE-HOTPLUG-0001 – Storage Hotplug

## Status

Angenommen

## Kategorie

Storage / Hotplug / Geräteverwaltung

## Zweck

Diese Spezifikation definiert Hotplug-Verhalten für Storage-Geräte.

## Ereignisse

Storage Hotplug MUSS folgende Events bereitstellen:

- BlockDeviceAdded,
- BlockDeviceRemoved,
- MediaChanged,
- DeviceOffline,
- DeviceRecovered.

## Anforderungen

- Entfernen eines Geräts MUSS offene I/O abbrechen.
- Dateisysteme MÜSSEN über Device Removal informiert werden.
- Wiederkehrende Geräte DÜRFEN nicht automatisch dieselbe Laufzeitidentität erhalten.
- Wechselmedien MÜSSEN Medienwechsel separat melden.

## Akzeptanzkriterien

- USB-Stick-Disconnect setzt Blockgerät offline.
- NVMe Controller Reset kann DeviceRecovered erzeugen.
- Handles auf entfernte Geräte werden ungültig.

