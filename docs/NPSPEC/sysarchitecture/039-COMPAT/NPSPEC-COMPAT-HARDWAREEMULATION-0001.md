# NPSPEC-COMPAT-HARDWAREEMULATION-0001 – Nova Hardware Emulation

## Status

Angenommen

## Kategorie

Compatibility / Hardware Emulation

## Zweck

NovaOS definiert Hardware-Emulation für Software, die bestimmte physische oder virtuelle Hardware erwartet, welche auf dem Host nicht vorhanden oder nicht direkt kompatibel ist.

Emulierte Hardware stellt das erwartete Geräteverhalten bereit, ohne dem Guest direkten oder unkontrollierten Zugriff auf reale Host-Hardware zu geben.

## Grundprinzipien

```text
Emulated Device ≠ Physical Device
Guest Device Access ≠ Host Device Authority
Hardware Emulation ≠ CPU Emulation
Hardware Emulation ≠ Driver Compatibility
Virtual Address ≠ Physical Address
Emulation ≠ Authority
```

## Modell

```text
Guest Software
      ↓
Guest Driver / Hardware Access
      ↓
Emulated Device
      ↓
Device Model
      ↓
Optional Host Backend
      ↓
Nova Driver / Capability
      ↓
Physical Hardware
```

Ein emuliertes Gerät muss nicht zwingend auf reale Hardware zugreifen. Es darf vollständig softwarebasiert implementiert sein.

## Gerätemodell

Ein emuliertes Gerät wird mindestens beschrieben durch:

```text
HardwareEmulation
├── DeviceProfile
├── DeviceType
├── InterfaceModel
├── RegisterModel
├── MemoryModel
├── InterruptModel
├── DMA Model
└── Backend
```

## Unterstützte Geräteklassen

Hardware-Emulation darf unter anderem für folgende Geräte eingesetzt werden:

```text
Storage
Network
Graphics
Audio
Input
Serial
Timer
RTC
Firmware Interfaces
Legacy Devices
```

## Register und I/O

Hardwarezugriffe des Guests werden abgefangen und auf das jeweilige Gerätemodell übertragen:

```text
Guest MMIO / Port I/O
        ↓
Validate
        ↓
Emulated Register
        ↓
Device Operation
```

Guest-I/O darf nicht ungeprüft auf reale Host-I/O-Bereiche weitergeleitet werden.

## Interrupts

Emulierte Geräte dürfen virtuelle Interrupts erzeugen.

```text
Emulated Device
      ↓
Virtual Interrupt
      ↓
Guest CPU / Guest Driver
```

Virtuelle Interrupts bleiben von nativen NovaOS-Interrupts getrennt.

## DMA

Guest-DMA wird ausschließlich innerhalb kontrollierter Speicherbereiche ausgeführt:

```text
Guest DMA Request
       ↓
Validate Mapping
       ↓
Emulated / Mapped Buffer
       ↓
Optional Host I/O
```

Eine Guest-DMA-Adresse darf niemals ungeprüft als physische Host-Adresse verwendet werden.

## Host-Backends

Ein Gerätemodell darf unterschiedliche Backends verwenden:

```text
Software Backend
Nova Device
File / Object
Network Service
Virtual Resource
Remote Provider
```

Der Zugriff auf geschützte Host-Ressourcen erfolgt ausschließlich über autorisierte NovaOS-Handles und Capabilities.

## Passthrough

Direkter Hardware-Passthrough ist keine Hardware-Emulation.

Falls Passthrough unterstützt wird, muss er als gesonderter, stärker privilegierter Ausführungsmodus behandelt werden und die NovaOS-IOMMU-, Capability- und Isolation-Regeln einhalten.

## Zusammenspiel mit CPU-Emulation

Hardware- und CPU-Emulation dürfen kombiniert werden:

```text
Foreign System
├── CPU Emulation
├── Memory Model
└── Hardware Emulation
```

Beide Komponenten bleiben logisch getrennt und können unabhängig ersetzt oder optimiert werden.

## Zustand

Gerätezustände dürfen serialisierbar sein, um Funktionen wie:

```text
Suspend
Resume
Snapshot
Migration
Recovery
Deterministic Replay
```

zu unterstützen.

## Normative Anforderungen

1. Emulierte und physische Geräte MÜSSEN eindeutig getrennt bleiben.
2. Guest-Hardwarezugriffe MÜSSEN vor ihrer Ausführung validiert werden.
3. Guest-I/O DÜRFEN nicht ungeprüft auf Host-I/O abgebildet werden.
4. Virtuelle Interrupts MÜSSEN von nativen Interrupts getrennt bleiben.
5. Guest-DMA DARF keine unautorisierten Host-Speicherbereiche erreichen.
6. Host-Backends MÜSSEN über kontrollierte NovaOS-Ressourcen zugreifen.
7. Hardware-Emulation DARF keine zusätzliche Authority erzeugen.
8. Guest-Treiber DÜRFEN keine native Geräte-Authority erhalten.
9. Passthrough MUSS von emulierter Hardware unterschieden werden.
10. Hardware- und CPU-Emulation MÜSSEN kombinierbar bleiben.
11. Gerätezustände SOLLEN für Snapshot, Migration und Recovery serialisierbar sein.
12. DeviceProfile, Backend, Zustand und Fehler MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-COMPAT-PERSONALITY-0001`
- `NPSPEC-COMPAT-CPUEMULATION-0001`
- `NPSPEC-COMPAT-BINARYTRANSLATION-0001`
- `NPSPEC-SYSTEM-DRIVERS-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`

## Ergebnis

NovaOS kann erwartete Hardware kontrolliert als virtuelle Gerätemodelle bereitstellen. Register, I/O, Interrupts, DMA und Gerätezustände werden innerhalb der Compatibility-Umgebung emuliert, während reale Host-Hardware ausschließlich über autorisierte NovaOS-Treiber, Handles und Capabilities erreichbar bleibt.