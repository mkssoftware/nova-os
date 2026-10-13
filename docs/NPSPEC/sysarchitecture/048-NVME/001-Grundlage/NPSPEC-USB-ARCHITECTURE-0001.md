# NPSPEC-USB-ARCHITECTURE-0001 – USB Architektur

## Status

Angenommen

## Kategorie

USB / Architektur / Hardware

## Zweck

Diese Spezifikation definiert die grundlegende USB-Architektur von NovaOS. Sie beschreibt Host Controller, Root Hubs, Hubs, Geräte, Interfaces, Endpoints und Transfers als zusammenhängendes Treiber- und I/O-Modell.

## Architekturmodell

```text
PCI/ACPI Discovery
    ↓
USB Host Controller Driver
    ↓
USB Core
    ↓
Hub / Enumeration
    ↓
Class Driver
    ↓
Kernel I/O / Userspace Service
```

Der USB Core ist nicht selbst gerätespezifisch. Gerätespezifische Funktionalität wird durch Class Driver wie HID oder Mass Storage bereitgestellt.

## Anforderungen

- NovaOS MUSS USB-Geräte über ein einheitliches Device-Modell darstellen.
- Host Controller MUSSEN über das Driver Framework erkannt und gebunden werden.
- USB Core MUSS Enumeration, Descriptor Parsing, Endpoint-Verwaltung und Transfer-Routing koordinieren.
- Class Driver DÜRFEN keinen direkten Zugriff auf Host-Controller-Register erhalten.
- Alle USB-Transfers MÜSSEN über definierte Transfer-Objekte laufen.

## Sicherheitsregeln

USB-Geräte gelten als nicht vertrauenswürdig. Descriptor-Daten, Report-Daten und Massenspeicher-Antworten MÜSSEN validiert werden, bevor sie in Kernelzustand übernommen werden.

## Akzeptanzkriterien

- Ein xHCI Controller kann erkannt und initialisiert werden.
- Ein USB-Gerät kann über Hub-Enumeration sichtbar werden.
- Ein Class Driver kann an ein Interface gebunden werden.
- Fehlerhafte Descriptoren führen nicht zu undefiniertem Kernelverhalten.

