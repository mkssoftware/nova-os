# NPSPEC-HAL-IOMMU-0001 – Nova IOMMU Abstraction

## Status

Entwurf

## Kategorie

HAL / IOMMU / DMA / Memory Isolation / Devices

## Zweck

NovaOS verwendet eine IOMMU-Abstraktion, um DMA-Zugriffe von Geräten auf definierte Speicherbereiche zu begrenzen und Geräte voneinander zu isolieren.

```text
Device
   ↓
IOMMU
   ↓
Authorized Memory
```

## Grundprinzip

Geräte erhalten keinen pauschalen Zugriff auf den physischen Speicher.

```text
Device
   ↓
IOMMU Domain
   ↓
IOVA Mapping
   ↓
Physical Memory
```

Die HAL stellt dafür eine einheitliche Schnittstelle unabhängig von der konkreten IOMMU-Hardware bereit.

## IOMMU-Domain

Geräte werden kontrollierten IOMMU-Domains zugeordnet.

```text
Domain
 ├── Device A
 ├── Device B
 └── DMA Mappings
```

Eine Domain definiert den für ihre Geräte erreichbaren Adressraum.

## DMA-Mapping

DMA-Buffer werden explizit in eine Domain eingebunden.

```text
Buffer
  ↓
Map
  ↓
IOVA
  ↓
Device
```

Ein Mapping besitzt mindestens:

```text
Domain
IOVA
Physical Range
Length
Permissions
Lifetime
```

Nach Ende der Nutzung muss das Mapping entfernt werden.

## Isolation

IOMMU-Domains sollen Geräte und Treiber voneinander isolieren.

```text
Device A → Domain A → Memory A
Device B → Domain B → Memory B
```

Ein Gerät darf nicht auf Speicher außerhalb seiner autorisierten Mappings zugreifen.

## Berechtigungen

Mappings sollen mindestens unterscheiden können:

```text
Read
Write
ReadWrite
```

Die gewährten Rechte müssen auf das für die Operation notwendige Minimum begrenzt werden.

## IOMMU-Gruppen

Geräte, die hardwarebedingt nicht sicher voneinander isoliert werden können, müssen als gemeinsame Isolationseinheit behandelt werden.

```text
IOMMU Group
 ├── Device A
 └── Device B
```

NovaOS darf keine stärkere Isolation behaupten, als die Hardware tatsächlich bereitstellen kann.

## Fault Handling

Unzulässige DMA-Zugriffe müssen erkannt und kontrolliert behandelt werden.

```text
Invalid DMA Access
       ↓
IOMMU Fault
       ↓
Identify Device / Domain
       ↓
Block / Report / Recover
```

Fehler sollen über die System-Observability nachvollziehbar sein.

## Systeme ohne IOMMU

Ist keine geeignete IOMMU vorhanden:

```text
IOMMU unavailable
       ↓
Restricted DMA Model
```

NovaOS muss diesen Zustand explizit kennen.

Sicherheitsgarantien dürfen nicht vorgetäuscht werden.

## Virtualisierung

Die Abstraktion soll auch virtuelle IOMMU-Implementierungen unterstützen.

```text
Physical IOMMU
Virtual IOMMU
Platform-specific IOMMU
        ↓
Nova IOMMU API
```

## Normative Anforderungen

1. NovaOS MUSS verfügbare IOMMUs über eine gemeinsame HAL-Schnittstelle abstrahieren.
2. DMA-Mappings MÜSSEN explizit einer IOMMU-Domain zugeordnet werden können.
3. Geräte SOLLEN nur auf autorisierte Speicherbereiche zugreifen können.
4. Mapping-Berechtigungen MÜSSEN auf das notwendige Minimum beschränkt werden.
5. IOMMU-Mappings MÜSSEN eine definierte Lebensdauer besitzen.
6. Nicht mehr benötigte Mappings MÜSSEN entfernt werden.
7. Hardwarebedingte IOMMU-Gruppen MÜSSEN berücksichtigt werden.
8. IOMMU-Faults MÜSSEN einem Gerät oder einer Domain zugeordnet werden können.
9. Fehlende IOMMU-Unterstützung MUSS explizit erkennbar sein.
10. NovaOS DARF keine Isolation garantieren, die von der Hardware nicht gewährleistet wird.
11. IOMMU-Nutzung MUSS mit dem DMA-System integrierbar sein.
12. IOMMU-Konfiguration DARF Capability- und Security-Regeln nicht umgehen.

## Abhängigkeiten

- `NPSPEC-HAL-0001`
- `NPSPEC-HAL-PLATFORM-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-ARCH-ZEROCOPY-0001`
- `ADR-HAL-0007`

## Ergebnis

```text
Device
   ↓
IOMMU Domain
   ↓
Controlled DMA Mapping
   ↓
Authorized Memory
```

NovaOS erhält damit eine kontrollierte DMA-Isolation, bei der Geräte nur auf explizit freigegebene Speicherbereiche zugreifen können.