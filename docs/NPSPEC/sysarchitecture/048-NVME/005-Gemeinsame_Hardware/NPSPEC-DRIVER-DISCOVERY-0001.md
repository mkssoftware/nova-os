# NPSPEC-DRIVER-DISCOVERY-0001 – Driver Discovery

## Status

Angenommen

## Kategorie

Driver / Discovery / Hardware

## Zweck

Diese Spezifikation definiert, wie NovaOS Hardwaregeräte erkennt und passenden Treibern zuordnet.

## Quellen

Driver Discovery MUSS Geräte aus folgenden Quellen verarbeiten können:

- PCI/PCIe,
- ACPI,
- USB Enumeration,
- Firmware-Tabellen,
- virtuelle Geräte.

## Matching

Matching-Regeln DÜRFEN verwenden:

- Bus-Typ,
- Vendor ID,
- Device ID,
- Class/Subclass/Protocol,
- Interface Class,
- Capability Flags,
- Firmware Properties.

## Anforderungen

- Mehrere passende Treiber MÜSSEN deterministisch priorisiert werden.
- Treiberbindung MUSS auditierbar sein.
- Nicht gebundene Geräte bleiben sichtbar.

## Akzeptanzkriterien

- NVMe und xHCI werden über PCI erkannt.
- USB Class Driver werden über Interface Descriptoren gebunden.
- Fehlendes Matching erzeugt keinen Bootfehler.

