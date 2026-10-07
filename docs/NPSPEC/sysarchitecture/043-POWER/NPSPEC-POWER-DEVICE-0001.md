# NPSPEC-POWER-DEVICE-0001 – Nova Device Power Management

## Status

Angenommen

## Kategorie

Power / Device

## Zweck

NovaOS definiert ein einheitliches Modell zur Energieverwaltung einzelner Geräte.

Geräte dürfen unabhängig vom globalen Systemzustand in geeignete Energiezustände wechseln, sofern Nutzung, Treiberzustand, Hardwareabhängigkeiten, Wake-Anforderungen und Execution Contracts dies zulassen.

## Grundprinzipien

```text
Device Power State ≠ System Power State
Device State ≠ Driver State
Idle Device ≠ Powered Off Device
Power Saving ≠ Device Removal
Requested State ≠ Effective State
Wake Capability ≠ Authority
```

## Modell

```text
DevicePowerContext
├── DeviceID
├── PowerDomainID
├── SupportedStates[]
├── CurrentState
├── RequestedState
├── WakeCapabilities[]
├── Dependencies[]
├── Constraints
└── State
```

Die Power-Identität bleibt an die stabile Geräteidentität gebunden.

## Gerätezustände

NovaOS verwendet abstrakte Gerätezustände:

```text
Active
Idle
LowPower
Standby
Off
Unavailable
```

Der Platform- oder Device-Provider bildet diese auf konkrete Hardwarezustände ab.

Beispielsweise können ACPI-Geräte intern D-States verwenden, ohne diese Semantik höheren Schichten aufzuzwingen.

## Zustandsauswahl

```text
Device Activity
      +
Pending I/O
      +
Wake Requirements
      +
Power Policy
      +
Dependencies
      +
Execution Contracts
        ↓
Device Power Decision
```

Der tiefste Zustand ist nicht automatisch der optimale Zustand.

Transition-Latenz und erwartete Inaktivitätsdauer müssen berücksichtigt werden können.

## Power Domains

Mehrere Geräte dürfen eine gemeinsame Power Domain verwenden:

```text
Power Domain
├── Device A
├── Device B
└── Device C
```

Eine gemeinsame Domain darf nur abgeschaltet werden, wenn kein abhängiges Gerät sie benötigt.

## Dependencies

Geräte können Energieabhängigkeiten besitzen:

```text
Device
  ↓
Bus
  ↓
Controller
  ↓
Power Domain
```

NovaOS muss diese Abhängigkeiten bei Power Transitions berücksichtigen.

## Runtime Power Management

Nicht verwendete Geräte dürfen während des normalen Systembetriebs automatisch in niedrigere Energiezustände wechseln.

```text
Active
  ↓ inactivity
Idle
  ↓
LowPower
```

Neue Nutzung muss eine kontrollierte Reaktivierung auslösen.

## Wake

Geräte dürfen als Wake Source dienen:

```text
Keyboard
Mouse
Network Adapter
USB Device
Sensor
Storage Controller
```

Wake-Fähigkeit und aktivierte Wake-Berechtigung bleiben getrennt.

## I/O-Koordination

Ein Gerät darf nicht in einen inkompatiblen Energiezustand wechseln, solange relevante I/O-Operationen aktiv sind.

```text
Pending I/O
    ↓
Quiesce
    ↓
Power Transition
```

Treiber und I/O-Subsystem müssen Power Transitions koordinieren.

## Fehlerverhalten

Schlägt eine Transition fehl:

```text
Transition Failure
       ↓
Verify State
       ↓
Safe Supported State
       ↓
Report Failure
```

Der angeforderte Zustand darf nicht automatisch als tatsächlich erreicht gelten.

## Hotplug

Power Management darf Geräteentfernung nicht mit einem Power-Off-Zustand verwechseln.

```text
Powered Off ≠ Removed
Removed ≠ Suspended
```

Hotplug-Ereignisse müssen separat behandelt werden.

## Normative Anforderungen

1. NovaOS MUSS Energiezustände einzelner Geräte verwalten können.
2. Device Power State und System Power State MÜSSEN getrennte Konzepte bleiben.
3. Gerätezustände MÜSSEN hardwareunabhängig abstrahiert werden.
4. Geräte DÜRFEN während des normalen Betriebs automatisch Energiesparzustände verwenden.
5. Aktive I/O-Operationen MÜSSEN bei Power Transitions berücksichtigt werden.
6. Geräteabhängigkeiten und gemeinsame Power Domains MÜSSEN berücksichtigt werden.
7. Wake-Fähigkeit und aktivierte Wake-Konfiguration MÜSSEN getrennt bleiben.
8. Transition-Latenzen MÜSSEN bei der Zustandsauswahl berücksichtigt werden können.
9. Angeforderter und tatsächlich erreichter Zustand MÜSSEN unterscheidbar sein.
10. Power-Off, Suspend und physische Geräteentfernung MÜSSEN getrennt behandelt werden.
11. Fehlgeschlagene Transitionen MÜSSEN sicher behandelt werden.
12. DeviceID, Power Domain, aktueller Zustand, Wake-Konfiguration und aktive Constraints MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-PLATFORM-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-SYSTEM-DRIVERS-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann den Energiezustand einzelner Geräte unabhängig und koordiniert steuern. Geräte dürfen bei Inaktivität Energie sparen und bei Bedarf reaktiviert werden, während I/O, Wake-Anforderungen, gemeinsame Power Domains, Hardwareabhängigkeiten und Systemzustände konsistent berücksichtigt bleiben.