# ADR-CAPABILITY-0012 – Capability-Based Drivers

## Status

Angenommen

## Kategorie

Capability Architecture / Driver Architecture / Hardware Abstraction

## Kontext

Klassische Treiber stellen häufig geräte- oder herstellerspezifische Schnittstellen bereit. Anwendungen und Systemdienste müssen dadurch teilweise wissen, welche konkrete Hardware oder welcher Treiber vorhanden ist.

NovaOS besitzt dagegen ein Capability-Modell, bei dem Consumer beschreiben, **welche Funktion** sie benötigen, während Provider bestimmen, **wie diese Funktion** umgesetzt wird.

Dieses Prinzip soll auch für Hardware gelten.

Beispielsweise soll eine Anwendung nicht zwingend einen bestimmten GPU-, Kamera- oder Audio-Treiber ansprechen müssen, sondern eine benötigte Capability anfordern können.

---

## Entscheidung

NovaOS integriert Treiber als **Capability Provider in die allgemeine Capability-Architektur**.

```text
Application / System Service
          ↓
Capability Requirement
          ↓
Discovery / Resolution
          ↓
Driver Capability Provider
          ↓
Driver
          ↓
Device
```

Treiber stellen die Fähigkeiten ihrer Geräte über explizite Capability Contracts bereit.

---

## Grundprinzip

```text
Applications request capabilities.

Drivers expose hardware capabilities.
```

Die konkrete Geräte- und Treiberimplementierung bleibt soweit möglich hinter dem Capability Contract verborgen.

---

## Device Capabilities

Ein Gerät kann mehrere Capabilities bereitstellen.

Beispiel:

```text
Camera Device
 ├── image.capture
 ├── video.capture
 ├── autofocus
 ├── exposure.control
 └── hardware.encode
```

Der Treiber registriert die unterstützten Capabilities beim Capability-System.

Die physische Geräteidentität bleibt von der Capability Identity getrennt.

---

## Driver als Provider

Ein Driver Capability Provider beschreibt mindestens:

```text
Capability Identity
Capability Contract
Provider Identity
Device Identity
Supported Semantic Types
Execution Properties
Resource Requirements
Hardware Properties
```

Mehrere Geräte oder Treiber können dieselbe Capability bereitstellen.

```text
video.encode
    │
    ├── GPU Driver A
    ├── GPU Driver B
    ├── Media Accelerator
    └── Software Provider
```

---

## Hardwareunabhängige Nutzung

Consumer sollen bevorzugt funktionale Anforderungen ausdrücken.

```text
video.encode
     ↓
Capability Resolution
     ↓
Available Provider
```

Dadurch kann NovaOS zwischen Hardware- und Softwareimplementierungen wählen, ohne dass die Anwendung ihre fachliche Logik ändern muss.

---

## HAL und Capability-Modell

HAL und Capability-System besitzen unterschiedliche Aufgaben.

```text
HAL
→ abstrahiert Hardwaremechanismen für Kernel und Treiber

Capability System
→ stellt semantische Funktionen für Consumer bereit
```

Ein Treiber kann HAL-Mechanismen verwenden und darauf aufbauend höherwertige Capabilities anbieten.

Das Capability-System ersetzt deshalb nicht die HAL.

---

## Device Model

Capability-Based Drivers werden mit dem deklarativen Device Model verbunden.

```text
Hardware System Graph
        ↓
Device
        ↓
Driver Binding
        ↓
Capability Registration
```

Hotplug, Device Removal und Driver Replacement müssen entsprechende Änderungen im Capability Registry State auslösen können.

---

## Provider Resolution

Sind mehrere geeignete Driver Provider verfügbar, kann NovaOS anhand des `Nova.ExecutionContract` auswählen.

Beispielsweise:

```text
Capability: video.encode

Candidates:
CPU Software Provider
Integrated GPU
Discrete GPU
Dedicated Media Engine
```

Die Auswahl kann unter anderem berücksichtigen:

- Hard Constraints,
- Semantic Types,
- Deadline,
- Determinismus,
- Resource Budget,
- Energieverbrauch,
- Locality,
- Provider Health,
- Trust.

---

## Direkter Gerätezugriff

Nicht jede Hardwareoperation muss über eine abstrakte funktionale Capability verborgen werden.

Low-Level-Systemkomponenten können bei entsprechender Authority explizite Device Capabilities verwenden.

```text
High-Level Capability
→ video.encode

Device Capability
→ controlled access to specific device functionality
```

Damit bleibt NovaOS sowohl für abstrakte Anwendungen als auch für hardwarenahe Systemsoftware geeignet.

---

## Security

Eine funktionale Capability gewährt keinen uneingeschränkten Gerätezugriff.

```text
Functional Capability
        ≠
Raw Device Authority
```

Gerätezugriff, DMA, Interrupts, MMIO und andere privilegierte Mechanismen bleiben durch das Driver Capability Model und die Security-Architektur geschützt.

---

## Driver Isolation

Capability-Based Drivers verwenden das hybride Kernel-/User-Mode-Driver-Modell von NovaOS.

Ein Provider kann abhängig von seinen Anforderungen in unterschiedlichen Isolation Domains laufen.

```text
Capability
    ↓
Driver Provider
    ↓
Driver Isolation Domain
    ↓
Device
```

Die Capability-Abstraktion darf diese Sicherheitsgrenzen nicht umgehen.

---

## Data Movement

Hardware-Capabilities werden in die systemweite Data-Movement-Architektur integriert.

Wo geeignet:

```text
Shared Buffer
     ↓
Driver Capability
     ↓
DMA / Device
```

Zero-Copy, Scatter/Gather, DMA und IOMMU sollen genutzt werden können, ohne Ownership-, Isolation- oder Security-Regeln zu verletzen.

---

## Hotplug und Live Replacement

Wenn ein Gerät oder Treiber entfernt wird, müssen dessen Provider entsprechend aktualisiert werden.

```text
Device Removed
      ↓
Provider Unavailable
      ↓
Capability Resolution
      ↓
Alternative Provider
```

Bei kompatiblen Alternativen kann NovaOS eine Capability neu auflösen.

Hot Reload und Live Replacement bleiben Bestandteil der Driver-Architektur.

---

## Introspection

NovaOS muss kontrolliert darstellen können:

```text
Device
Driver
Provided Capabilities
Capability Contracts
Provider State
Isolation Domain
Resource Usage
Selected Consumers
```

Damit wird nachvollziehbar, welche Hardware welche Systemfähigkeiten bereitstellt.

---

## Normative Anforderungen

1. NovaOS MUSS Treiber als Capability Provider integrieren können.
2. Treiber MÜSSEN Gerätefunktionen über explizite Capability Contracts bereitstellen können.
3. Capability Identity und physische Device Identity MÜSSEN getrennt bleiben.
4. Mehrere Driver Provider MÜSSEN dieselbe Capability bereitstellen können.
5. Anwendungen SOLLEN Hardwarefunktionen bevorzugt über semantische Capabilities anfordern können.
6. Capability Resolution MUSS zwischen Hardware- und Softwareprovidern wählen können.
7. HAL und Capability-System MÜSSEN getrennte Architekturebenen bleiben.
8. Capability-Based Drivers MÜSSEN mit dem deklarativen Device Model integriert sein.
9. Hotplug und Driver Lifecycle MÜSSEN den Zustand registrierter Capability Provider aktualisieren können.
10. Funktionale Capabilities DÜRFEN keinen impliziten Raw-Device-Zugriff gewähren.
11. Driver Isolation und Driver Capabilities MÜSSEN erhalten bleiben.
12. DMA, Shared Buffers und Zero-Copy MÜSSEN mit den bestehenden Isolation- und Security-Regeln integrierbar sein.
13. Driver Hot Reload und Live Replacement MÜSSEN mit Capability Resolution zusammenarbeiten können.
14. Device-, Driver- und Capability-Beziehungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Anwendungen werden stärker hardwareunabhängig,
- Hardwarebeschleunigung kann transparent genutzt werden,
- unterschiedliche Geräte können dieselben Systemfähigkeiten bereitstellen,
- Software-Fallbacks lassen sich in dasselbe Modell integrieren,
- neue Hardware kann bestehende Anwendungen ohne spezielle Integration beschleunigen.

### Negative Konsequenzen

- Treiber benötigen präzise Capability Contracts,
- nicht jede hardwarenahe Funktion lässt sich sinnvoll vollständig abstrahieren,
- Capability Registry und Device Lifecycle müssen eng synchronisiert werden.

---

## Verworfene Alternativen

### Anwendungen sprechen konkrete Treiber direkt an

Verworfen.

Dies würde Anwendungen unnötig an bestimmte Hardware und Hersteller binden.

### Capability-System ersetzt die HAL

Verworfen.

HAL und Capability-System abstrahieren unterschiedliche Ebenen.

### Jede Hardwarefunktion muss vollständig hardwareunabhängig sein

Verworfen.

Für hardwarenahe Systemsoftware bleiben explizite Device Capabilities erforderlich.

### Hardwareprovider besitzen automatisch höhere Priorität

Verworfen.

Hardwarebeschleunigung ist nicht automatisch die beste Wahl; Hard Constraints, Ressourcen, Energie, Locality und andere Faktoren müssen berücksichtigt werden.

---

## Abhängigkeiten

- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-CAPABILITY-0003_Capability_Registry`
- `ADR-CAPABILITY-0006_Capability_Negotiation`
- `ADR-CAPABILITY-0010_Capability_Sandboxing`
- `ADR-HAL-0001_Einheitliche_Hardware_Abstraction_Layer`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0007_Hotplug_über_deklarative_Device_Lifecycle_States`
- `ADR-DRIVER-0001_Hybrides_Kernel_und_User_Mode_Drivermodell`
- `ADR-DRIVER-0004_Driver_Capability_Model`
- `ADR-DRIVER-0006_Driver_Live_Replacement`
- `ADR-DRIVER-0009_Device_Model_als_deklarativer_Systemgraph`

---

## Zugehörige NPSPECs

- `NPSPEC-CAPABILITY-DRIVER-0001`
- `NPSPEC-CAPABILITY-DRIVER-REGISTRATION-0001`
- `NPSPEC-CAPABILITY-DEVICE-BINDING-0001`
- `NPSPEC-CAPABILITY-DRIVER-RESOLUTION-0001`
- `NPSPEC-CAPABILITY-DRIVER-LIFECYCLE-0001`
- `NPSPEC-CAPABILITY-DRIVER-TEST-0001`

---

## Ergebnis

NovaOS integriert Hardware in dasselbe Fähigkeitenmodell wie Software:

```text
Hardware
    ↓
Driver
    ↓
Capability Provider
    ↓
Capability Registry
    ↓
Resolution
    ↓
Application / System Service
```

Treiber werden damit nicht nur als gerätespezifische Schnittstellen betrachtet, sondern als kontrollierte Provider semantischer Systemfähigkeiten.

Dies ermöglicht hardwareunabhängige Anwendungen, systemweite Provider-Auswahl und transparente Nutzung neuer Hardwarebeschleuniger, ohne HAL, Driver Isolation oder Security-Grenzen aufzugeben.