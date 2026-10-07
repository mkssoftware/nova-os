# NPSPEC-POWER-NETWORK-0001 – Nova Network Power Management

## Status

Angenommen

## Kategorie

Power / Network

## Zweck

NovaOS definiert die energieeffiziente Steuerung von Netzwerkhardware und Netzwerkverbindungen.

Network Power Management reduziert den Energieverbrauch inaktiver oder gering ausgelasteter Netzwerkkomponenten, ohne aktive Verbindungen, Netzwerkzustände, Wake-Anforderungen oder Execution Contracts unkontrolliert zu beeinträchtigen.

## Grundprinzipien

```text
Network Power State ≠ Network State
Interface Idle ≠ Disconnected
Low Power ≠ Link Down
Link State ≠ Device Power State
Power Saving ≠ Connection Loss
Wake Capability ≠ Active Wake Policy
```

## Architektur

```text
Network Stack
     ↓
Network Power Manager
     ↓
Runtime Power Manager
     ↓
Network Driver
     ↓
Platform / HAL
     ↓
Network Hardware
```

Netzwerkprotokolle bleiben von konkreten Hardware-Energiesparmechanismen abstrahiert.

## Modell

```text
NetworkPowerContext
├── InterfaceID
├── DeviceID
├── LinkState
├── PowerState
├── ActivityState
├── WakeCapabilities[]
├── LatencyRequirements
├── Constraints
└── Policy
```

## Unterstützte Geräte

Das Modell gilt unter anderem für:

```text
Ethernet
Wi-Fi
Cellular
Bluetooth Networking
USB Network
Virtual Network Devices
Registered Network Providers
```

Technologiespezifische Mechanismen bleiben hinter dem jeweiligen Provider verborgen.

## Energiezustände

NovaOS abstrahiert mindestens:

```text
Active
Idle
LowPower
Standby
Off
Unavailable
```

Ein niedriger Power State bedeutet nicht automatisch, dass die logische Netzwerkschnittstelle entfernt oder ihre Konfiguration verworfen wird.

## Aktivität

Die Zustandsauswahl darf berücksichtigen:

```text
Traffic
Active Connections
Pending Packets
Latency Requirements
Wake Requirements
Link Activity
Execution Contracts
Power Source
```

Hintergrundverkehr darf anders behandelt werden als latenzkritische Kommunikation.

## Netzwerkverbindungen

Power Management muss bestehende Netzwerkzustände berücksichtigen.

```text
Active Connection
       ↓
Power Decision
       ↓
Compatible Low-Power State
```

Ein Energiesparzustand darf keine Verbindung unkontrolliert unterbrechen, wenn deren Vertrag oder Policy dies nicht erlaubt.

## Wake

Netzwerkhardware darf Wake-Funktionen bereitstellen:

```text
Wake-on-LAN
Magic Packet
Pattern Match
Link Change
Registered Network Event
```

Wake-Fähigkeit und aktivierte Wake Policy bleiben getrennt.

## Technologiespezifische Mechanismen

Provider dürfen beispielsweise verwenden:

```text
Ethernet Energy Efficient Ethernet
Wi-Fi Power Save
Wi-Fi Target Wake Time
Radio Sleep States
PCIe Runtime Power Management
USB Runtime Power Management
Platform-Specific Network Power States
```

NovaOS bleibt gegenüber diesen Mechanismen abstrahiert.

## Connectivity Preservation

NovaOS darf zwischen verschiedenen Energiesparstrategien unterscheiden:

```text
Maintain Full Connectivity
Maintain Wake Connectivity
Maintain Limited Connectivity
Disconnect Allowed
```

Die zulässige Strategie ergibt sich aus Policy und Execution Contracts.

## Multi-Interface

Bei mehreren Netzwerkinterfaces darf NovaOS deren Energiezustände koordinieren:

```text
Ethernet
Wi-Fi
Cellular
```

Nicht benötigte Interfaces dürfen reduziert oder deaktiviert werden, sofern Connectivity-, Routing-, Sovereignty- und User-Constraints dies erlauben.

## Fehlerverhalten

Schlägt eine Power Transition fehl:

```text
Failure
  ↓
Verify Interface / Link State
  ↓
Restore Safe State
  ↓
Report / Degrade
```

Netzwerkerreichbarkeit darf nicht aufgrund eines angenommenen, aber nicht tatsächlich erreichten Zustands falsch bewertet werden.

## Normative Anforderungen

1. NovaOS MUSS Network Power Management hardware- und technologieunabhängig abstrahieren.
2. Network State und Device Power State MÜSSEN getrennte Konzepte bleiben.
3. Idle und Disconnected MÜSSEN unterscheidbar sein.
4. Aktive Verbindungen und Pending Traffic MÜSSEN bei Power-Entscheidungen berücksichtigt werden.
5. Latenz- und Connectivity-Anforderungen MÜSSEN berücksichtigt werden können.
6. Wake-Fähigkeit und aktivierte Wake Policy MÜSSEN getrennt bleiben.
7. Technologiespezifische Energiesparmechanismen MÜSSEN hinter Providern abstrahiert werden.
8. Mehrere Netzwerkinterfaces MÜSSEN koordiniert werden können.
9. Execution Contracts DÜRFEN zulässige Connectivity- und Power-Zustände begrenzen.
10. Angeforderter und effektiver Power State MÜSSEN unterscheidbar sein.
11. Fehlgeschlagene Transitionen MÜSSEN sicher behandelt werden.
12. InterfaceID, Link State, Power State, Wake Policy und aktive Constraints MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-PLATFORM-0001`
- `NPSPEC-POWER-DEVICE-0001`
- `NPSPEC-POWER-RUNTIME-0001`
- `NPSPEC-NETWORK-INTENT-0001`
- `NPSPEC-NETWORK-INTROSPECTION-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`

## Ergebnis

NovaOS kann Netzwerkhardware abhängig von Aktivität, Verbindungsanforderungen, Latenz, Wake Policy und Energiezustand dynamisch optimieren. Netzwerkzustand und Hardware-Power-State bleiben getrennt, sodass Energie eingespart werden kann, ohne notwendige Konnektivität oder aktive Kommunikationsverträge unkontrolliert zu beeinträchtigen.