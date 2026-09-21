# ADR-COMPAT-0009 – Legacy Driver in Isolation oder VM

## Status

Angenommen

## Kategorie

Compatibility / Drivers / Isolation / Virtualization

## Kontext

Legacy-Software kann von Treibern abhängen, für die kein nativer NovaOS-Treiber verfügbar ist.

Solche Treiber können:

```text
Kernel Privileges erwarten
Direkten Hardwarezugriff benötigen
Legacy Driver APIs verwenden
Unsichere Annahmen enthalten
Nicht mehr gepflegt werden
```

Die direkte Ausführung fremder Legacy-Treiber im NovaOS-Kernel würde Sicherheits-, Stabilitäts- und Architekturgrenzen gefährden.

## Entscheidung

Legacy-Treiber werden **nicht direkt als native NovaOS-Kerneltreiber ausgeführt**.

Stattdessen werden sie abhängig von ihren Anforderungen in einer isolierten Driver Domain oder innerhalb einer virtuellen Maschine betrieben.

```text
Legacy Driver
     ↓
┌───────────────────────┐
│ Isolated Driver Domain│
│          oder         │
│ Virtual Machine       │
└───────────────────────┘
     ↓
Controlled Device Access
     ↓
NovaOS
```

## Isolated Driver Domain

Kann ein Legacy-Treiber mit begrenzter Übersetzung betrieben werden, wird eine isolierte Driver Domain bevorzugt.

```text
Legacy Driver
     ↓
Driver Compatibility Layer
     ↓
Restricted Capabilities
     ↓
Nova Driver / Device Interface
```

Die Domain erhält ausschließlich die benötigten Ressourcen und Gerätezugriffe.

## Virtual Machine

Benötigt ein Treiber umfangreiche Eigenschaften seines ursprünglichen Betriebssystems oder direkten Kernelkontext, wird er innerhalb einer VM ausgeführt.

```text
Application
     ↓
Guest OS
     ↓
Legacy Driver
     ↓
Virtual Device / Passthrough
     ↓
NovaOS
```

Die VM bildet dabei die notwendige Legacy-Systemumgebung.

## Hardwarezugriff

Direkter Hardwarezugriff wird nur explizit und kontrolliert bereitgestellt.

Mögliche Verfahren sind:

```text
Virtual Device
Mediated Device
Restricted MMIO
Restricted Port I/O
IOMMU-Protected DMA
Device Passthrough
```

Der Legacy-Treiber darf keine unkontrollierte DMA- oder Speicherautorität erhalten.

## Sicherheitsgrenze

Legacy-Treiber gelten grundsätzlich nicht als vertrauenswürdiger Bestandteil des NovaOS-Kernels.

```text
Legacy Driver
      ↓
Isolation Boundary
      ↓
NovaOS Kernel
```

Ein kompromittierter oder fehlerhafter Legacy-Treiber soll die Kernelintegrität nicht direkt verletzen können.

## Fehlerisolation

Fehler sollen auf Driver Domain oder VM begrenzt bleiben.

```text
Driver Crash
     ↓
Domain / VM Failure
     ↓
Restart / Recovery
```

Soweit möglich kann NovaOS die Umgebung neu starten, ohne das Gesamtsystem neu starten zu müssen.

## Native Migration

Existiert später ein geeigneter nativer NovaOS-Treiber, soll dieser gegenüber der Legacy-Lösung bevorzugt werden.

```text
Legacy Driver
     ↓
Temporary Compatibility
     ↓
Native Nova Driver
```

Legacy-Treiber sind damit eine Kompatibilitätslösung und keine Grundlage der langfristigen Treiberarchitektur.

## Normative Anforderungen

1. Legacy-Treiber DÜRFEN NICHT ungeprüft im nativen NovaOS-Kernel ausgeführt werden.
2. Legacy-Treiber MÜSSEN über eine Isolation Boundary ausgeführt werden.
3. Eine isolierte Driver Domain SOLL gegenüber einer vollständigen VM bevorzugt werden, wenn sie ausreichend kompatibel ist.
4. Eine VM SOLL verwendet werden, wenn der Treiber wesentliche Eigenschaften seines ursprünglichen Betriebssystems benötigt.
5. Hardwarezugriff MUSS explizit begrenzt werden.
6. DMA MUSS soweit technisch möglich durch IOMMU oder vergleichbare Mechanismen isoliert werden.
7. Legacy-Treiber DÜRFEN keine implizite globale Systemautorität erhalten.
8. Treiberfehler SOLLEN auf die jeweilige Domain oder VM begrenzt bleiben.
9. Native NovaOS-Treiber SOLLEN langfristig gegenüber Legacy-Treibern bevorzugt werden.

## Konsequenzen

### Positiv

- ältere Hardware kann weiterhin nutzbar bleiben,
- Legacy-Treiber gefährden nicht unmittelbar den NovaOS-Kernel,
- Treiberfehler können stärker isoliert werden,
- bestehende Treiber können als Übergangslösung dienen.

### Negativ

- Isolation oder Virtualisierung erzeugt zusätzlichen Overhead,
- Hardware-Passthrough kann komplex sein,
- nicht jeder Legacy-Treiber lässt sich vollständig isoliert betreiben.

## Abhängigkeiten

- `ADR-COMPAT-0001_Native_Nova_Architektur_nicht_durch_POSIX_Linux_oder_Win32_bestimmen`
- `ADR-COMPAT-0003_Compatibility_Personalities`
- `ADR-COMPAT-0007_Execution_Compatibility_Ladder`
- `ADR-COMPAT-0008_Legacy_Software_in_Compatibility_Domains`
- `ADR-DRIVER-0001`
- `ADR-CAPABILITY-0012_Sandboxing`
- `ADR-VERIFY-0003_Memory_Safety`
- `ADR-ARCH-0016_Fail_Safe_Defaults`

## Ergebnis

NovaOS folgt für Legacy-Treiber dem Prinzip:

```text
Legacy Driver
     ↓
Isolation first
     ↓
VM when necessary
     ↓
Never unrestricted in Nova Kernel
```

Damit bleibt ältere Hardware nutzbar, ohne die Sicherheit und Evolvierbarkeit der nativen NovaOS-Treiberarchitektur aufzugeben.