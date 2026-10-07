# NPSPEC-POWER-PCIEASPM-0001 – Nova PCIe Active State Power Management

## Status

Angenommen

## Kategorie

Power / PCIe / ASPM

## Zweck

NovaOS definiert die Steuerung von PCI Express Active State Power Management (ASPM).

ASPM reduziert den Energieverbrauch inaktiver PCIe-Links durch geeignete Link-Energiesparzustände, ohne Geräte logisch zu entfernen oder deren normalen Betriebszustand aufzuheben.

## Grundprinzipien

```text
ASPM ≠ Device Suspend
ASPM ≠ Device Power State
ASPM ≠ PCIe Hotplug
Link State ≠ Device State
Deepest Link State ≠ Always Optimal
Firmware Configuration ≠ Always Valid
Power Saving ≠ Reliability Loss
```

## Architektur

```text
Runtime Power Manager
        ↓
PCIe Power Policy
        ↓
PCIe ASPM Manager
        ↓
PCIe Bus / Root Port
        ↓
PCIe Link
```

ASPM arbeitet mit Device Power Management und Runtime Power Management zusammen, bleibt jedoch ein eigenständiger Mechanismus.

## Link-Modell

```text
PCIeLinkPower
├── LinkID
├── UpstreamPort
├── DownstreamPort
├── SupportedStates[]
├── CurrentState
├── ExitLatency
├── Constraints
└── State
```

Die Fähigkeiten beider Link-Endpunkte müssen berücksichtigt werden.

## ASPM-Zustände

NovaOS muss mindestens folgende PCIe-ASPM-Zustände modellieren können:

```text
L0
L0s
L1
```

Unterstützte Plattformen dürfen zusätzliche L1-Unterzustände bereitstellen:

```text
L1.1
L1.2
```

Nur tatsächlich unterstützte und validierte Zustände dürfen aktiviert werden.

## Auswahl

```text
Link Activity
     +
Expected Idle Duration
     +
Exit Latency
     +
Device Requirements
     +
Power Policy
     +
Platform Constraints
       ↓
Effective ASPM Policy
```

Eine höhere Energieeinsparung darf nicht auf Kosten nicht erfüllbarer Latenzanforderungen erzwungen werden.

## Link-Endpunkte

ASPM wird als Eigenschaft eines Links behandelt.

```text
Root Port
    ↕
PCIe Link
    ↕
Endpoint
```

Ein Zustand darf nur verwendet werden, wenn die beteiligten Link-Endpunkte ihn gemeinsam unterstützen.

## Runtime Integration

Bei geringer Linkaktivität darf NovaOS aggressivere ASPM-Zustände zulassen.

Bei latenzkritischer oder hoher Aktivität darf die Policy tiefere Zustände begrenzen.

```text
High Activity → geringe ASPM-Tiefe
Low Activity  → höhere ASPM-Tiefe
```

Die konkrete Entscheidung bleibt policygesteuert.

## Device Power Management

ASPM und Device Power States werden koordiniert:

```text
Device Runtime State
        +
PCIe Link State
        ↓
Effective Power Behavior
```

Ein Device Suspend darf geeignete tiefere Linkzustände ermöglichen, ist aber nicht mit ASPM identisch.

## Plattform und Firmware

NovaOS darf Informationen aus:

```text
PCIe Configuration Space
ACPI
Firmware
Platform Quirks
Device Capabilities
```

verwenden.

Firmware-Vorgaben müssen berücksichtigt, dürfen jedoch bei erkannten Fehlern oder Inkompatibilitäten durch sichere NovaOS-Regeln eingeschränkt werden.

## Kompatibilität

Einige Geräte oder Plattformen können bestimmte ASPM-Zustände fehlerhaft implementieren.

NovaOS muss daher Quirks und gezielte Deaktivierung unterstützen können:

```text
Link
 ↓
Known Constraint
 ↓
Restricted ASPM State Set
```

Ein problematischer Zustand darf deaktiviert werden, ohne ASPM für das gesamte System abschalten zu müssen.

## Fehlerverhalten

Bei instabilem Linkverhalten muss NovaOS auf einen sichereren Zustand zurückfallen können:

```text
Link Error
    ↓
Restrict ASPM
    ↓
Shallower State
    ↓
Verify Stability
```

Stabilität und Datenintegrität haben Vorrang vor Energieeinsparung.

## Normative Anforderungen

1. NovaOS MUSS PCIe ASPM als Link-Power-Mechanismus behandeln.
2. ASPM und Device Power Management MÜSSEN getrennte Konzepte bleiben.
3. Fähigkeiten beider Link-Endpunkte MÜSSEN berücksichtigt werden.
4. Nur unterstützte und validierte ASPM-Zustände DÜRFEN aktiviert werden.
5. Exit-Latenzen MÜSSEN bei der Policy berücksichtigt werden.
6. L1-Unterzustände MÜSSEN unterstützt werden können.
7. Runtime Power Management MUSS ASPM-Policy beeinflussen können.
8. ASPM MUSS mit Device Power States koordinierbar sein.
9. Plattform- und Gerätequirks MÜSSEN unterstützt werden.
10. Problematische Zustände MÜSSEN pro Link eingeschränkt werden können.
11. Linkstabilität und Datenintegrität MÜSSEN Vorrang vor Energieoptimierung besitzen.
12. LinkID, unterstützte Zustände, aktive Policy, effektiver Zustand und Einschränkungen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-PLATFORM-0001`
- `NPSPEC-POWER-ACPI-0001`
- `NPSPEC-POWER-DEVICE-0001`
- `NPSPEC-POWER-RUNTIME-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-SYSTEM-DRIVERS-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann den Energieverbrauch von PCIe-Verbindungen durch kontrolliertes ASPM reduzieren. Linkaktivität, Exit-Latenz, Geräteanforderungen und Plattformbesonderheiten bestimmen die zulässigen Energiesparzustände, während Stabilität und Datenintegrität jederzeit Vorrang behalten.