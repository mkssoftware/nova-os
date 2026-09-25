# NPSPEC-HAL-HOTPLUG-0001 – Nova Hardware Hotplug

## Status

Angenommen

## Kategorie

HAL / Hardware / Hotplug / Lifecycle

## Zweck

NovaOS unterstützt das kontrollierte Hinzufügen, Entfernen und Ändern von Hardware während des laufenden Betriebs.

```text
Hardware Change
      ↓
Platform / Bus
      ↓
HAL Hotplug
      ↓
Driver / Resource Update
```

## Grundprinzip

Hardware darf nicht als dauerhaft unveränderlich angenommen werden.

```text
Discover
   ↓
Validate
   ↓
Attach / Detach
   ↓
Update System State
```

Hotplug wird als kontrollierte Zustandsänderung behandelt.

## Unterstützte Ressourcen

Hotplug kann unter anderem betreffen:

```text
CPU
Memory
PCI / PCIe Devices
USB Devices
Storage
Network Devices
Displays
Virtual Hardware
```

Nicht jede Plattform muss jede Hotplug-Art unterstützen.

## Hinzufügen

Neue Hardware durchläuft grundsätzlich:

```text
Detected
   ↓
Identify
   ↓
Validate
   ↓
Allocate Resources
   ↓
Bind Driver
   ↓
Activate
```

Erst nach erfolgreicher Initialisierung darf die Ressource als verfügbar gelten.

## Entfernen

Entfernbare Hardware soll kontrolliert stillgelegt werden.

```text
Removal Request
      ↓
Stop New Operations
      ↓
Drain / Cancel
      ↓
Release Resources
      ↓
Detach Driver
      ↓
Remove
```

## Überraschendes Entfernen

NovaOS muss auch unerwartetes Entfernen behandeln können.

```text
Device Lost
    ↓
Invalidate Access
    ↓
Cancel / Fail Operations
    ↓
Release Resources
    ↓
Update System State
```

Verlorene Hardware darf nicht weiterhin als verfügbar behandelt werden.

## CPU- und Memory-Hotplug

CPU- oder Speicheränderungen müssen mit Scheduler und Speicherverwaltung koordiniert werden.

```text
CPU Removal
   ↓
Migrate Tasks
   ↓
Remove CPU

Memory Removal
   ↓
Migrate / Release Pages
   ↓
Remove Memory
```

Eine Ressource darf erst entfernt werden, wenn ihre verbleibende Nutzung dies zulässt.

## Topologie

Hotplug-Ereignisse aktualisieren die Hardwaretopologie.

```text
Old Topology
     ↓
Hotplug Event
     ↓
Topology Update
     ↓
New Topology
```

Abhängige Komponenten müssen über relevante Änderungen informiert werden können.

## Ressourcen

Durch Hotplug hinzugefügte oder entfernte Ressourcen müssen mit der Nova Resource Economy synchronisiert werden.

```text
Hardware Change
      ↓
Resource Capacity Change
      ↓
Resource Economy
```

## Fehlerverhalten

Schlägt die Aktivierung neuer Hardware fehl, darf sie nicht teilweise als betriebsbereit erscheinen.

Bei Entfernung müssen unklare Zustände explizit behandelt werden.

```text
Unknown ≠ Available
```

## Normative Anforderungen

1. NovaOS MUSS Hardwareänderungen zur Laufzeit erkennen können, sofern die Plattform dies unterstützt.
2. Hotplug MUSS als kontrollierte Zustandsänderung behandelt werden.
3. Neue Hardware MUSS vor Aktivierung erkannt und validiert werden.
4. Ressourcen MÜSSEN vor Nutzung vollständig initialisiert sein.
5. Kontrolliertes Entfernen MUSS laufende Nutzung berücksichtigen.
6. Überraschendes Entfernen MUSS sicher behandelt werden können.
7. Entfernte Hardware DARF NICHT weiterhin als verfügbar gelten.
8. CPU- und Memory-Hotplug MÜSSEN mit den zuständigen Subsystemen koordiniert werden.
9. Hardwaretopologie MUSS nach Hotplug-Ereignissen aktualisiert werden.
10. Änderungen der verfügbaren Ressourcen MÜSSEN der Resource Economy bekannt gemacht werden.
11. Treiber MÜSSEN einen definierten Attach- und Detach-Lebenszyklus unterstützen, wenn ihre Hardware hotplugfähig ist.
12. Nicht unterstützte Hotplug-Funktionen MÜSSEN eindeutig erkennbar sein.

## Abhängigkeiten

- `NPSPEC-HAL-0001`
- `NPSPEC-HAL-PLATFORM-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-HAL-NUMA-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`

## Ergebnis

```text
Hardware Change
      ↓
Controlled Lifecycle
      ↓
Topology + Resources Update
      ↓
Consistent System State
```

NovaOS kann Hardware damit im laufenden Betrieb hinzufügen oder entfernen, ohne die Konsistenz von Topologie, Ressourcen und Systemzustand aufzugeben.