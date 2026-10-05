# NPSPEC-SYSTEM-DRIVERS-0001 – Nova System Drivers

## Status

Angenommen

## Kategorie

System / Drivers

## Zweck

NovaOS definiert das systemweite Treibermodell für die kontrollierte Anbindung von Hardware und virtuellen Geräten.

Treiber werden über definierte Driver- und HAL-Schnittstellen eingebunden und sollen Fehler sowie Hardwarezugriffe möglichst vom restlichen System isolieren.

## Grundprinzipien

```text
Driver ≠ HAL
Driver ≠ Device
Driver ≠ Application
Driver Presence ≠ Device Authority
Kernel Driver ≠ Mandatory Driver Model
```

## Architektur

```text
System / Applications
        ↓
Device Services / I/O
        ↓
Driver Framework
        ↓
Driver
        ↓
HAL / Bus / Hardware
```

Treiber stellen gerätespezifische Funktionen über standardisierte NovaOS-Schnittstellen bereit.

## Treibermodi

NovaOS unterstützt mindestens:

```text
Kernel-mode Driver
User-mode Driver
```

User-mode-Treiber sollen bevorzugt werden, wenn Hardwareanforderungen, Performance und Latenz dies zulassen.

Kernel-mode-Treiber bleiben für Komponenten möglich, die privilegierten oder besonders hardwarenahen Zugriff benötigen.

## Driver Identity

Jeder registrierte Treiber besitzt eine stabile Identität unabhängig von:

```text
Dateiname
Installationspfad
Gerätepfad
Ladeadresse
```

Treiberidentität und Geräteidentität bleiben getrennt.

## Gerätebindung

```text
Device Discovery
      ↓
Device Identity
      ↓
Driver Matching
      ↓
Driver Validation
      ↓
Driver Binding
```

Ein Treiber kann mehrere kompatible Geräte unterstützen.

Ein Gerät darf abhängig vom Gerätetyp mehrere spezialisierte Treiberkomponenten verwenden.

## Capabilities

Treiber erhalten nur die Hardware- und Systemzugriffe, die für ihre Funktion erforderlich sind.

Beispiele:

```text
MMIO
Port I/O
Interrupts
DMA
IOMMU
Device Memory
Firmware Interface
```

Treiber dürfen keine globale System-Authority allein aufgrund ihres Treiberstatus erhalten.

## Isolation

Treiberfehler sollen möglichst auf Treiber und zugehöriges Gerät begrenzt bleiben.

Insbesondere User-mode-Treiber können in eigenen Sicherheits- und Prozesskontexten ausgeführt werden.

```text
Driver Failure
     ↓
Isolation
     ↓
Restart / Rebind / Degrade
```

Ein Treiberfehler soll nach Möglichkeit keinen vollständigen Systemausfall verursachen.

## Lifecycle

```text
Discover
  ↓
Load
  ↓
Validate
  ↓
Bind
  ↓
Active
  ↓
Suspend / Replace / Unbind
  ↓
Unload
```

NovaOS soll Hotplug, Hot Reload und Live Replacement unterstützen können, sofern Treiber und Gerät dies erlauben.

## Trust

Treiber müssen vor privilegierter Ausführung auf Integrität und Vertrauensstatus geprüft werden können.

Nicht vertrauenswürdige Treiber können abgelehnt oder in stärker isolierte Ausführungsumgebungen gezwungen werden.

## Normative Anforderungen

1. NovaOS MUSS ein einheitliches Driver Framework bereitstellen.
2. Treiber und HAL MÜSSEN getrennte Verantwortlichkeiten besitzen.
3. Kernel- und User-mode-Treiber MÜSSEN unterstützt werden können.
4. User-mode-Treiber SOLLEN bevorzugt werden, wenn technisch sinnvoll.
5. Treiber MÜSSEN stabile Identitäten besitzen können.
6. Gerätebindung MUSS kontrolliert und validierbar erfolgen.
7. Hardwarezugriffe MÜSSEN capability-basiert begrenzbar sein.
8. Treiber DÜRFEN keine implizite globale Authority erhalten.
9. Treiberfehler SOLLEN möglichst isoliert werden.
10. Treiber MÜSSEN kontrolliert geladen und entladen werden können.
11. Hotplug und Live Replacement SOLLEN unterstützt werden können.
12. Treiberzustand, Bindung und Fehler MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-KERNEL-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt ein einheitliches, capability-basiertes und isolierbares Treibermodell. Hardware kann über Kernel- oder User-mode-Treiber angebunden werden, während Treiberfehler, privilegierte Zugriffe und Lebenszyklen kontrolliert bleiben.