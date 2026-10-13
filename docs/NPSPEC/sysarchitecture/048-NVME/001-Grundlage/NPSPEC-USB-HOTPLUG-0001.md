# NPSPEC-USB-HOTPLUG-0001 – USB Hotplug

## Status

Angenommen

## Kategorie

USB / Hotplug / Runtime

## Zweck

Diese Spezifikation definiert das Verhalten beim Verbinden und Entfernen von USB-Geräten zur Laufzeit.

## Ereignismodell

USB Hotplug erzeugt strukturierte Events:

- DeviceConnected,
- DeviceEnumerated,
- DeviceRemoved,
- DeviceFailed,
- InterfaceBound,
- InterfaceUnbound.

## Anforderungen

- Disconnect MUSS alle offenen Transfers abbrechen.
- Entfernte Geräte MÜSSEN aus Object Registry und Device Tree entfernt werden.
- Handles auf entfernte Geräte MÜSSEN stale werden.
- Class Driver MÜSSEN Unbind-Callbacks erhalten.

## Race Handling

Hotplug und laufende I/O dürfen nicht zu Use-after-free führen. Referenzzählung oder vergleichbare Lebenszeitkontrolle ist verpflichtend.

## Akzeptanzkriterien

- Entfernen während I/O führt zu kontrolliertem Abbruch.
- Wiederverbinden erzeugt eine neue Geräteidentität.
- Hotplug-Events sind beobachtbar.

