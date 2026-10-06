# NPSPEC-COMPAT-LEGACYDRIVER-0001 – Nova Legacy Driver Compatibility

## Status

Angenommen

## Kategorie

Compatibility / Legacy Driver

## Zweck

NovaOS definiert eine isolierte Kompatibilitätsumgebung für ältere oder fremde Gerätetreiber, die nicht für das native NovaOS-Treibermodell entwickelt wurden.

Legacy-Treiber dürfen unterstützt werden, ohne ihre Kernel-, Privileg- oder Gerätemodelle in den nativen NovaOS-Kernel zu übernehmen.

## Grundprinzipien

```text
Legacy Driver ≠ Nova Driver
Legacy Kernel Mode ≠ Nova Kernel Mode
Driver Compatibility ≠ Device Authority
Foreign Driver ABI ≠ Nova Driver ABI
Driver Isolation > Compatibility
Legacy Privilege ≠ Nova Authority
```

## Modell

```text
Legacy Driver
      ↓
Legacy Driver Domain
      ↓
Compatibility Personality
├── Driver ABI
├── Driver API
├── Syscall Translation
├── Hardware Interface
└── Runtime
      ↓
Nova Driver Framework
      ↓
Authorized Device Handle
      ↓
Hardware
```

## Legacy Driver Domain

Legacy-Treiber werden grundsätzlich in einer isolierten Driver Domain ausgeführt.

```text
Legacy Driver Domain
├── Private Address Space
├── Compatibility Runtime
├── Restricted Capabilities
├── Virtualized Kernel Interfaces
└── Controlled Device Access
```

Ein Fehler des Legacy-Treibers soll nicht unmittelbar den NovaOS-Kernel kompromittieren.

## Treiber-ABI

Fremde Treiber-ABIs werden über die Compatibility-Architektur bereitgestellt.

```text
Legacy Driver ABI
       ↓
ABI Translation
       ↓
Compatibility Interface
       ↓
Nova Driver Framework
```

Die native NovaOS-Treiber-ABI bleibt davon unabhängig.

## Kernel-Schnittstellen

Von einem Legacy-Treiber erwartete Kernel-Funktionen werden ausschließlich als kontrollierte Compatibility-Schnittstellen bereitgestellt.

Der Treiber erhält dadurch keinen direkten Zugriff auf interne NovaOS-Kernelstrukturen.

Nicht unterstützte Kerneloperationen müssen kontrolliert fehlschlagen.

## Gerätezugriff

Gerätezugriff erfolgt über explizit autorisierte Ressourcen:

```text
Legacy Driver
      ↓
Compatibility Device Interface
      ↓
Device Capability
      ↓
Authorized Device Handle
      ↓
Nova HAL / Driver Framework
```

Das Laden eines Treibers erzeugt keine Geräte-Authority.

## MMIO, Port-I/O und DMA

Direkte Hardwareoperationen müssen kontrolliert vermittelt werden.

```text
MMIO
Port I/O
DMA
Interrupts
```

dürfen nur innerhalb explizit freigegebener Ressourcenbereiche erfolgen.

DMA muss durch NovaOS-Speicherschutz und, sofern verfügbar, IOMMU-Isolation begrenzt werden.

## Interrupts

Fremde Interruptmodelle werden auf kontrollierte NovaOS-Interruptmechanismen abgebildet.

Ein Legacy-Treiber darf keine beliebigen nativen Interrupt Handler installieren oder interne Interruptstrukturen verändern.

## Hardware-Emulation

Falls ein Treiber bestimmte historische Hardware erwartet, darf er mit `NPSPEC-COMPAT-HARDWAREEMULATION-0001` kombiniert werden:

```text
Legacy Driver
      ↓
Emulated Device
      ↓
Nova Backend
```

## Fehlerbehandlung

Fehler eines Legacy-Treibers müssen auf dessen Driver Domain begrenzt werden können.

NovaOS darf bei Fehlern:

```text
Stop
Restart
Isolate
Revoke Device Access
Fallback
Disable Driver
```

ausführen.

Ein fehlerhafter Legacy-Treiber darf nicht automatisch einen Systemabsturz verursachen.

## Trust und Sicherheit

Legacy-Treiber gelten nicht allein aufgrund ihrer Herkunft oder Signatur als vertrauenswürdig.

Trust, Integrität, Compatibility und Device Authority werden getrennt bewertet.

Unsichere oder unbekannte Treiber dürfen stärker isoliert oder vollständig blockiert werden.

## Normative Anforderungen

1. Legacy-Treiber DÜRFEN nicht direkt Bestandteil des nativen NovaOS-Kernels werden.
2. Legacy-Treiber SOLLEN in isolierten Driver Domains ausgeführt werden.
3. Fremde Driver-ABIs MÜSSEN von der nativen NovaOS-Treiber-ABI getrennt bleiben.
4. Fremde Kernel-Schnittstellen MÜSSEN kontrolliert vermittelt werden.
5. Legacy-Treiber DÜRFEN keine implizite Device Authority erhalten.
6. MMIO, Port-I/O, DMA und Interruptzugriff MÜSSEN explizit begrenzt werden.
7. DMA MUSS gegen unautorisierten Speicherzugriff geschützt werden.
8. Legacy-Kernelprivilegien DÜRFEN keine NovaOS-Kernelauthority erzeugen.
9. Fehler MÜSSEN soweit möglich auf die jeweilige Driver Domain begrenzt bleiben.
10. Device Authority MUSS bei Bedarf widerrufbar sein.
11. Hardware-Emulation DARF als Compatibility-Backend verwendet werden.
12. Treiberidentität, ABI, Gerätebindung, Trust und Fehlerzustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-COMPAT-ABI-0001`
- `NPSPEC-COMPAT-PERSONALITY-0001`
- `NPSPEC-COMPAT-ABITRANSLATION-0001`
- `NPSPEC-COMPAT-HARDWAREEMULATION-0001`
- `NPSPEC-SYSTEM-DRIVERS-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`

## Ergebnis

NovaOS kann ältere oder fremde Gerätetreiber über isolierte Legacy Driver Domains weiterverwenden. Fremde Driver-ABIs, Kernel-Schnittstellen und Hardwarezugriffe werden kontrolliert vermittelt, während der native Kernel, das NovaOS-Treibermodell und die Capability-basierten Sicherheitsgrenzen unabhängig und geschützt bleiben.