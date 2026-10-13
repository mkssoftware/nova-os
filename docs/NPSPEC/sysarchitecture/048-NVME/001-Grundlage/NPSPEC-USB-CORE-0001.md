# NPSPEC-USB-CORE-0001 – USB Core

## Status

Angenommen

## Kategorie

USB / Core / Kernel

## Zweck

Der USB Core stellt die zentrale Abstraktionsschicht zwischen Host Controller Driver, Hub-Enumeration und USB-Class-Drivern bereit.

## Verantwortlichkeiten

Der USB Core MUSS folgende Aufgaben übernehmen:

- Registrierung von Host Controllern,
- Verwaltung von USB-Bussen,
- Geräteadressierung,
- Descriptor-Leseoperationen,
- Configuration- und Interface-Auswahl,
- Endpoint-Objekte,
- Transfer-Erzeugung und Routing,
- Fehlernormalisierung.

## Objektmodell

```text
UsbBus
├── HostController
├── RootHub
└── UsbDevice[]
    ├── Configuration[]
    ├── Interface[]
    └── Endpoint[]
```

Geräteobjekte MÜSSEN stabile ObjectIDs erhalten. Kurzlebige Hardwarezustände DÜRFEN nicht als stabile Identität verwendet werden.

## Initialisierung

USB Core darf erst aktiv werden, wenn Driver Discovery, DMA, Interrupt Manager und Kernel Object Registry verfügbar sind.

## Schnittstellen

Der Core MUSS interne APIs für Host Controller und Class Driver bereitstellen:

- `usb_core_register_controller`
- `usb_core_enumerate_port`
- `usb_core_submit_transfer`
- `usb_core_cancel_transfer`
- `usb_core_bind_interface_driver`

## Akzeptanzkriterien

- Mehrere Host Controller können registriert werden.
- Transfers werden einem eindeutigen Controller und Endpoint zugeordnet.
- Class Driver sehen keine Controller-spezifischen Details.

