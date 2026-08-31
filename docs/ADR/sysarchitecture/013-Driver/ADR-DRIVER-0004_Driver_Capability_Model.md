# ADR-DRIVER-0004 – Driver Capability Model

## Status

Angenommen

## Kategorie

Driver / Capabilities / Security / Hardware Access / Contracts

## Kontext

Treiber benötigen Zugriff auf privilegierte Hardware- und Systemfunktionen. Die bloße Tatsache, dass eine Komponente als Treiber geladen wurde, darf jedoch keine pauschale Systemberechtigung erzeugen.

Die bereits definierten Driver Sandboxes und Driver Isolation Domains benötigen deshalb ein explizites Autorisierungsmodell, das festlegt, welche Funktionen und Ressourcen ein Treiber tatsächlich verwenden darf.

Dieses Modell soll sich in die allgemeine Capability-Architektur von NovaOS integrieren und kein separates Berechtigungssystem ausschließlich für Treiber schaffen.

---

## Entscheidung

NovaOS verwendet ein **Capability-basiertes Autorisierungsmodell für Treiber**.

Ein Treiber erhält ausschließlich explizite Capabilities für die Hardware- und Systemressourcen, die er zur Erfüllung seines Driver Contracts benötigt.

```text
Driver Identity
      ↓
Driver Contract
      ↓
Trust + Policy
      ↓
Granted Capabilities
      ↓
Authorized Device Access
```

Die Eigenschaft „Driver“ selbst verleiht keine implizite Authority.

---

## Grundprinzip

```text
Driver status grants no authority.

Capabilities grant specific authority.
```

---

## Driver Capabilities

Driver Capabilities repräsentieren konkrete Autorität über definierte Ressourcen oder Funktionen.

Dazu können beispielsweise gehören:

- Zugriff auf ein bestimmtes Device,
- MMIO Mapping,
- Interrupt-Nutzung,
- DMA-Nutzung,
- IOMMU Context,
- Device Queue,
- Power-Management-Funktion,
- kontrollierte HAL- oder Kernel-Funktion.

Capabilities müssen auf den erforderlichen Scope begrenzbar sein.

---

## Device-spezifische Authority

Capabilities sollen möglichst an konkrete Hardware-Ressourcen gebunden werden.

```text
Driver
  ↓
Capability
  ↓
Device X
```

Eine Capability für Device X darf nicht automatisch Zugriff auf Device Y oder auf die gesamte Geräteklasse ermöglichen.

Dadurch bleibt die Authority auch bei mehreren gleichartigen Geräten begrenzt.

---

## Driver Contract

Ein Driver Contract beschreibt, welche Capabilities ein Treiber für seine Funktion benötigt.

```text
Required Capabilities
        ↓
Policy Evaluation
        ↓
Granted Capabilities
```

Deklarierte Anforderungen sind dabei keine automatische Berechtigung.

NovaOS entscheidet anhand von Security-, Trust- und System-Policies, welche Capabilities tatsächlich gewährt werden.

---

## Capability und Resource Assignment

Capability und Ressourcenzuweisung bleiben logisch getrennt.

```text
Capability
→ darf eine Ressource verwenden

Resource Assignment
→ welche konkrete Ressource wurde zugewiesen
```

Ein Treiber benötigt beide Voraussetzungen, wenn eine Operation sowohl Authority als auch eine konkrete Hardware-Zuweisung voraussetzt.

---

## DMA und IOMMU

DMA Authority muss explizit über das Capability-Modell kontrolliert werden.

Eine DMA Capability darf nur Zugriff auf die dafür autorisierten DMA-Kontexte und Buffer ermöglichen.

```text
Driver Capability
       ↓
DMA Context
       ↓
IOMMU Mapping
       ↓
Authorized Buffer
```

Damit kann das Capability-Modell nicht durch direkten Device-DMA umgangen werden.

---

## Capability Delegation

Capabilities dürfen nur kontrolliert delegiert werden.

Ein Treiber darf einer anderen Driver Domain keine stärkere Authority übertragen, als er selbst besitzt.

Delegation muss durch die allgemeinen NovaOS-Security- und Capability-Regeln begrenzt werden.

---

## Revocation

Driver Capabilities müssen entziehbar sein, wenn die zugrunde liegende Authority nicht mehr gültig ist.

Gründe können beispielsweise sein:

- Driver Termination,
- Device Removal,
- Driver Replacement,
- Security Policy Change,
- Trust Revocation,
- Resource Reassignment.

Der Capability-Lifecycle muss mit dem Device- und Driver-Lifecycle verbunden sein.

---

## Kernel- und User-Mode-Treiber

Das Capability-Modell gilt konzeptionell für beide Treibertypen.

Bei User-Mode-Treibern bildet es eine technisch durchsetzbare Sandbox-Grenze.

Kernel-Mode-Treiber besitzen aufgrund ihrer Ausführungsumgebung weitergehende technische Möglichkeiten, müssen ihre legitimen Hardware- und Systemzugriffe dennoch über explizite Driver Contracts und Capabilities beschreiben.

Kernel Mode darf nicht als semantische „All Capabilities“-Berechtigung modelliert werden.

---

## Nova.Trust

Die Vergabe privilegierter Driver Capabilities wird mit Nova.Trust verbunden.

```text
Identity
   ↓
Signature
   ↓
Provenance
   ↓
Trust
   ↓
Capability Grant
   ↓
Audit
```

Besonders privilegierte Capabilities können entsprechend höhere Trust-Anforderungen besitzen.

---

## Fail-Safe Default

Kann NovaOS nicht eindeutig feststellen, ob ein Treiber eine Capability erhalten darf, wird sie nicht gewährt.

```text
Unknown Authority
      ↓
Denied
```

Fehlende Capabilities dürfen nicht durch permissive Fallbacks ersetzt werden.

---

## Normative Anforderungen

1. NovaOS MUSS Driver Authority über explizite Capabilities modellieren.
2. Die Eigenschaft „Driver“ DARF keine pauschale System Authority verleihen.
3. Driver Capabilities MÜSSEN auf konkrete Ressourcen oder Funktionen begrenzbar sein.
4. Device-spezifische Capabilities SOLLEN auf konkrete Device Identities beschränkt werden.
5. Driver Contracts MÜSSEN benötigte Capabilities deklarieren können.
6. Deklarierte Capability-Anforderungen DÜRFEN NICHT automatisch gewährt werden.
7. Capability Granting MUSS Security-, Trust- und System-Policies berücksichtigen.
8. Capability und Resource Assignment MÜSSEN logisch getrennte Konzepte bleiben.
9. DMA-Zugriff MUSS explizit autorisierbar sein.
10. Capability Delegation DARF keine Authority-Erweiterung ermöglichen.
11. Driver Capabilities MÜSSEN kontrolliert widerrufbar sein.
12. Capability-Lifetimes MÜSSEN mit Driver- und Device-Lifecycles integrierbar sein.
13. Kernel-Mode-Treiber DÜRFEN semantisch NICHT als automatisch vollständig autorisiert modelliert werden.
14. Unklare oder fehlende Authority MUSS nach dem Fail-Safe-Prinzip zur Verweigerung führen.
15. Capability Grants und Revocations MÜSSEN kontrolliert auditier- und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- konsequentes Least-Privilege-Modell,
- präzise Begrenzung von Hardwarezugriffen,
- Integration mit Driver Sandboxing,
- kontrollierbare DMA- und Device-Nutzung,
- widerrufbare Treiberrechte,
- einheitliches Capability-Modell statt eigener Driver-Sonderlösung.

### Negative Konsequenzen

- Driver Contracts müssen benötigte Authorities explizit beschreiben,
- Capability-Lifecycle und Device-Lifecycle müssen koordiniert werden,
- Kernel-Mode-Code kann technisch nicht so stark eingeschränkt werden wie User-Mode-Treiber.

---

## Verworfene Alternativen

### Geladene Treiber sind automatisch privilegiert

Verworfen.

Dies widerspricht Least Privilege und würde die Driver Sandbox erheblich schwächen.

### Berechtigungen ausschließlich pro Treiberklasse

Verworfen.

Authority muss auf konkrete Ressourcen und Geräte begrenzbar sein.

### Separates Driver-Permission-System

Verworfen.

Treiber sollen das allgemeine NovaOS-Capability-, Trust- und Security-Modell verwenden.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-DRIVER-0001_Hybrides_Kernel_und_User_Mode_Drivermodell`
- `ADR-DRIVER-0002_Driver_Sandboxing`
- `ADR-DRIVER-0003_Driver_Isolation_Domains`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-HAL-0007_Hotplug_über_deklarative_Device_Lifecycle_States`

---

## Zugehörige NPSPECs

- `NPSPEC-DRIVER-CAPABILITY-0001`
- `NPSPEC-DRIVER-CAPABILITY-GRANT-0001`
- `NPSPEC-DRIVER-CAPABILITY-DELEGATION-0001`
- `NPSPEC-DRIVER-CAPABILITY-REVOCATION-0001`
- `NPSPEC-DRIVER-CAPABILITY-DMA-0001`
- `NPSPEC-DRIVER-CAPABILITY-TEST-0001`

---

## Ergebnis

NovaOS behandelt Treiberprivilegien nicht als implizite Folge des Treiberstatus:

```text
Driver
  ↓
Explicit Capabilities
  ↓
Assigned Resources
  ↓
Authorized Hardware Access
```

Damit wird Hardwarezugriff zu einer expliziten, begrenzten und widerrufbaren Authority.

Die zentrale Architekturregel lautet:

```text
Being a driver grants nothing.

Capabilities grant exactly what is required.
```