# NPSPEC-IPC-SHAREDMEMORY-0001 – Nova IPC Shared Memory

## Status

Angenommen

## Kategorie

IPC / Shared Memory / Data Movement

## Zweck

NovaOS definiert Shared Memory als kontrollierten IPC-Mechanismus, bei dem mehrere Prozesse oder Systemkomponenten auf denselben Speicherbereich zugreifen können.

```text
Process A ─┐
           ├→ Shared Memory Object
Process B ─┘
```

Shared Memory dient insbesondere für große Datenmengen, Zero-Copy-Kommunikation und gemeinsam genutzte Datenstrukturen.

## Grundprinzip

```text
Shared Memory ≠ Shared Address Space
Shared Memory ≠ Shared Authority
Mapping ≠ Ownership
Reference ≠ Permission
```

Jeder Teilnehmer behält seinen eigenen virtuellen Adressraum. Nur explizit freigegebene Speicherobjekte werden gemeinsam zugänglich gemacht.

## Shared Memory Object

Gemeinsam genutzter Speicher wird als eigenständiges Systemobjekt behandelt.

```text
SharedMemory
├── ObjectID
├── Size
├── Owner
├── Access Rights
├── Mappings
└── State
```

Optional:

```text
Semantic Type
Resource Budget
Lifetime Policy
Security Context
```

## Erstellung

Ein Prozess kann ein Shared-Memory-Objekt erzeugen und anschließend Zugriffsrechte delegieren.

```text
Create
  ↓
Allocate
  ↓
Grant Capability
  ↓
Map
  ↓
Use
```

Die Erstellung eines Objekts gewährt anderen Prozessen keine impliziten Rechte.

## Mapping

Dasselbe Shared-Memory-Objekt darf an unterschiedlichen virtuellen Adressen eingeblendet werden.

```text
Shared Memory Object
      ↙       ↘
0x400000     0x900000
Process A    Process B
```

Prozesslokale Pointer dürfen deshalb nicht als gemeinsame Objekt- oder Datenreferenzen vorausgesetzt werden.

## Zugriffsrechte

Mappings müssen mit expliziten Rechten erstellt werden können.

```text
Read
Write
ReadWrite
Execute
```

`Execute` soll standardmäßig nicht vergeben werden und muss separat autorisiert werden.

Ein lesender Teilnehmer benötigt keine Schreibberechtigung.

## Capability-Modell

Der Zugriff erfolgt über eine passende Capability.

```text
SharedMemory Capability
        ↓
Permission Check
        ↓
Mapping
```

Capabilities müssen auf Operationen und gegebenenfalls Teilbereiche des Speicherobjekts begrenzbar sein.

## Ownership

Das Shared-Memory-Objekt besitzt einen definierten Owner oder verantwortlichen Systemkontext.

```text
Owner
  ↓
Shared Memory
  ├→ Reader A
  ├→ Reader B
  └→ Writer C
```

Mapping und Ownership bleiben getrennte Konzepte.

## Synchronisation

Shared Memory stellt Speicher bereit, aber keine automatische Synchronisation.

```text
Shared Memory
      +
Synchronization
      ↓
Consistent Shared State
```

Je nach Datenstruktur können verwendet werden:

```text
Atomics
Mutex
Semaphore
Futex
Ring Buffer
Lock-Free Protocol
```

Die verwendete Synchronisationssemantik muss zwischen den Teilnehmern definiert sein.

## Lifetime

Das Speicherobjekt darf nicht zerstört werden, solange gültige Referenzen oder Mappings bestehen.

```text
Created
   ↓
Mapped
   ↓
Shared
   ↓
Unmapped
   ↓
Released
```

Prozessende muss dessen Mapping automatisch freigeben können.

## IPC-Integration

IPC-Nachrichten dürfen Shared-Memory-Referenzen übertragen.

```text
IPC Message
    ↓
SharedMemory Reference
    +
Capability
    ↓
Receiver Mapping
```

Die Referenz selbst darf keine Zugriffsberechtigung darstellen.

## Zero-Copy

Shared Memory bildet einen zentralen Mechanismus für Zero-Copy-IPC.

```text
Producer
   ↓ Write
Shared Memory
   ↓ Read
Consumer
```

Für ungeeignete oder nicht autorisierte Situationen muss ein Copy-Pfad verfügbar bleiben.

## Ressourcensteuerung

Shared Memory muss in die NovaOS-Ressourcenökonomie integriert werden.

Begrenzbar sein müssen insbesondere:

```text
Object Size
Total Shared Memory
Mapping Count
Pinned Memory
Lifetime
```

Ein Prozess darf durch Shared Memory keine unbegrenzten Speicherressourcen reservieren.

## Fehlerbehandlung

NovaOS muss definierte Fehler für mindestens folgende Situationen liefern können:

```text
Invalid Capability
Permission Denied
Invalid Mapping
Resource Limit
Object Removed
Invalid Range
```

Ein fehlerhafter Teilnehmer darf nicht automatisch Speicher außerhalb des freigegebenen Objekts beeinflussen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
ObjectID
Size
Owner
Mappings
Access Rights
Reference Count
Resource Usage
State
```

Speicherinhalte dürfen ohne entsprechende Berechtigung nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS kontrolliertes Shared Memory für IPC unterstützen.
2. Shared Memory DARF NICHT vollständige Adressräume zwischen Prozessen teilen.
3. Shared-Memory-Objekte MÜSSEN eindeutig identifizierbar sein.
4. Zugriff MUSS durch explizite Berechtigungen oder Capabilities kontrolliert werden.
5. Mapping und Ownership MÜSSEN getrennt behandelt werden.
6. Read-, Write- und Execute-Rechte MÜSSEN getrennt kontrollierbar sein.
7. Execute-Rechte SOLLEN standardmäßig nicht vergeben werden.
8. Prozesslokale Pointer DÜRFEN NICHT als allgemeine Shared-Memory-Referenzen vorausgesetzt werden.
9. Synchronisation MUSS explizit durch das verwendete Kommunikationsprotokoll definiert werden.
10. Shared-Memory-Lifetime MUSS bestehende Mappings und Referenzen berücksichtigen.
11. Shared-Memory-Ressourcen MÜSSEN begrenzbar und abrechenbar sein.
12. Shared Memory SOLL als Grundlage für effiziente Zero-Copy-IPC verwendet werden können.

## Abhängigkeiten

- `NPSPEC-IPC-0001`
- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-IPC-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-SHAREDBUFFER-0001`
- `NPSPEC-MEMORY-MAPPING-0001`
- `NPSPEC-MEMORY-SHARED-0001`
- `NPSPEC-SYNC-ATOMIC-0001`
- `NPSPEC-SYNC-FUTEX-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-IPC-0005`

## Ergebnis

```text
Process A
    ↓
Capability
    ↓
Shared Memory Object
    ↑
Capability
    ↑
Process B
```

NovaOS erhält damit einen kontrollierten Shared-Memory-Mechanismus für IPC, der effizienten gemeinsamen Datenzugriff ermöglicht, ohne Prozessisolation, Speicherrechte oder Capability-Grenzen aufzugeben.