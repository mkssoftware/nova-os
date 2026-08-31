# ADR-IPC-0003 – Zero-Copy IPC

## Status

Angenommen

## Kategorie

IPC / Zero-Copy / Data Movement / Shared Memory / Performance

## Kontext

IPC kann erhebliche Datenmengen zwischen isolierten Execution Domains übertragen.

Ein klassischer IPC-Pfad kopiert Daten häufig mehrfach:

```text
Sender Buffer
    ↓ copy
Kernel Buffer
    ↓ copy
Receiver Buffer
```

Für kleine Nachrichten ist dieser Aufwand häufig akzeptabel. Bei großen Datenobjekten, Multimedia, Storage, Netzwerkdaten oder Compute-Pipelines können solche Kopien jedoch erhebliche CPU-, Speicherbandbreiten- und Cache-Kosten verursachen.

NovaOS besitzt bereits Shared Memory Objects, Shared Buffers und eine systemweite Copy-Avoidance-Architektur. IPC soll diese Mechanismen nativ nutzen.

---

## Entscheidung

NovaOS unterstützt **Zero-Copy IPC als nativen Datenpfad für geeignete IPC-Kommunikation**.

Statt Daten zwischen Execution Domains zu kopieren, können autorisierte Memory Objects beziehungsweise Shared Buffers direkt geteilt oder übertragen werden.

```text
Execution Domain A
        ↓
Shared Buffer / Memory Object
        ↓
IPC
        ↓
Execution Domain B
```

Zero-Copy ist ein bevorzugter Optimierungspfad, aber keine zwingende Voraussetzung für jede IPC-Operation.

---

## Grundprinzip

```text
Transfer references and ownership
before copying payload data.
```

---

## Kleine und große Nachrichten

Typed IPC bleibt unabhängig vom verwendeten Transportmechanismus.

Kleine Nachrichten können direkt übertragen werden:

```text
Small Message
    ↓
Direct IPC
```

Größere Daten können über Shared Objects übertragen werden:

```text
Typed Message
    ↓
Object / Buffer Reference
    ↓
Shared Memory Object
```

Die konkrete Auswahl zwischen Copy und Zero-Copy wird nicht durch diese ADR festgelegt.

---

## Shared Objects

Zero-Copy IPC basiert auf den systemweiten Shared Memory Objects und Shared Buffers.

Die physische Datenidentität bleibt dabei unabhängig von den virtuellen Adressen der beteiligten Execution Domains.

```text
Domain A Mapping ─┐
                  ↓
             Memory Object
                  ↑
Domain B Mapping ─┘
```

Beide Domains müssen den Buffer daher nicht an derselben virtuellen Adresse abbilden.

---

## Ownership und Sharing

NovaOS unterscheidet zwischen Ownership Transfer und Shared Access.

```text
Ownership Transfer
A owns → B owns

Shared Access
A owns → A + B access
```

Ownership Transfer soll bevorzugt werden, wenn der Sender die Daten nach der Übergabe nicht mehr benötigt.

Gemeinsamer Zugriff erfordert explizite Synchronisation.

---

## Capability Integration

Eine IPC Capability gewährt nicht automatisch Zugriff auf ein übertragenes Memory Object.

Der Empfänger benötigt die entsprechende Object- beziehungsweise Buffer-Authority.

```text
IPC Authority
      +
Object Authority
      ↓
Zero-Copy Access
```

Damit bleibt Capability-Based IPC auch bei gemeinsamem Speicher erhalten.

---

## Security und Data Sovereignty

Zero-Copy darf keine Isolation oder Information-Flow-Regeln umgehen.

Vor einer Freigabe müssen insbesondere:

- Security,
- Trust,
- Data Sovereignty,
- Zugriffsrechte

erfüllt sein.

Ist direktes Sharing nicht zulässig, kann ein kontrollierter Copy-Pfad verwendet oder die Operation abgelehnt werden.

---

## Copy Fallback

Zero-Copy wird nicht erzwungen.

Eine Kopie darf verwendet werden, wenn sie aufgrund von:

- Isolation,
- Security,
- Datenkonvertierung,
- Locality,
- Speicherlayout,
- Ressourcenverbrauch

notwendig oder insgesamt effizienter ist.

```text
IPC Transfer
     ↓
Eligible Strategies
 ├── Zero-Copy
 └── Copy
```

Dies entspricht dem NovaOS-Prinzip Copy Avoidance vor blindem Zero-Copy.

---

## Resource Economy

Zero-Copy-Ressourcen müssen der Resource Economy zugerechnet werden.

Dies betrifft insbesondere:

- Shared Memory,
- Mappings,
- Pinned Memory,
- Buffer-Lifetime,
- Synchronisationsressourcen.

Zero-Copy darf nicht als kostenloser Ressourcenpfad behandelt werden.

---

## Normative Anforderungen

1. NovaOS MUSS Zero-Copy IPC für geeignete Datenübertragungen unterstützen.
2. Zero-Copy IPC SOLL auf Shared Memory Objects und Shared Buffers aufbauen.
3. Zero-Copy DARF NICHT für jede IPC-Operation erzwungen werden.
4. Typed IPC MUSS unabhängig vom konkreten Copy- oder Zero-Copy-Transport bleiben.
5. Memory Objects MÜSSEN in unterschiedlichen Execution Domains unabhängig adressierbar sein.
6. Ownership Transfer und Shared Access MÜSSEN getrennte Semantiken besitzen.
7. Ownership Transfer SOLL bevorzugt werden, wenn der Sender die Daten nicht weiter benötigt.
8. Shared writable access MUSS explizite Synchronisation ermöglichen.
9. IPC Authority und Object Authority MÜSSEN getrennte Konzepte bleiben.
10. Zero-Copy DARF Security-, Trust- oder Data-Sovereignty-Grenzen NICHT umgehen.
11. Ein kontrollierter Copy-Fallback MUSS möglich sein, wenn Sharing nicht geeignet ist.
12. Locality und Gesamtkosten SOLLEN bei der Transferstrategie berücksichtigt werden.
13. Zero-Copy-Ressourcen MÜSSEN der Resource Economy zurechenbar sein.
14. Der tatsächlich verwendete IPC-Datenpfad MUSS kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- weniger unnötige Speicheroperationen,
- geringerer CPU- und Speicherbandbreitenverbrauch,
- effizientere Übertragung großer Datenobjekte,
- direkte Integration in Data/Object Pipelines,
- gute Grundlage für Multimedia-, Storage- und Compute-Pipelines.

### Negative Konsequenzen

- Buffer-Lifetime und Ownership müssen explizit verwaltet werden,
- Shared Memory benötigt Synchronisation,
- Zero-Copy kann zusätzliche Mapping- und Ressourcenbindung verursachen.

---

## Verworfene Alternativen

### IPC kopiert grundsätzlich alle Daten

Verworfen.

Große Datenübertragungen würden unnötig CPU-Zeit und Speicherbandbreite verbrauchen.

### IPC verwendet grundsätzlich Shared Memory

Verworfen.

Für kleine Nachrichten kann eine direkte Kopie einfacher und effizienter sein.

### Zero-Copy ohne separates Authority-Modell

Verworfen.

Gemeinsamer Speicher darf die Isolation zwischen Execution Domains nicht umgehen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-IPC-0001_Typed_IPC`
- `ADR-IPC-0002_Capability_Based_IPC`
- `ADR-ARCH-0006_Zero_Copy`
- `ADR-ARCH-0009_Unified_Data_Object_Pipelining`
- `ADR-DATAMOVE-0001_Zero_Copy_Datenpfade`
- `ADR-DATAMOVE-0002_Shared_Buffers_als_systemweite_Abstraktion`
- `ADR-DATAMOVE-0005_Copy_Avoidance_vor_blinder_Zero_Copy_Optimierung`
- `ADR-MEMORY-0008_Shared_Memory_über_explizite_Shared_Objects`

---

## Zugehörige NPSPECs

- `NPSPEC-IPC-ZEROCOPY-0001`
- `NPSPEC-IPC-SHARED-BUFFER-0001`
- `NPSPEC-IPC-OWNERSHIP-0001`
- `NPSPEC-IPC-OBJECT-AUTHORITY-0001`
- `NPSPEC-IPC-COPY-FALLBACK-0001`
- `NPSPEC-IPC-ZEROCOPY-TEST-0001`

---

## Ergebnis

NovaOS kann große IPC-Daten über gemeinsame Memory Objects übertragen, ohne sie unnötig zwischen Execution Domains zu kopieren:

```text
Sender
   ↓
Shared Buffer
   ↓
Typed + Capability-Based IPC
   ↓
Receiver
```

Zero-Copy bleibt dabei eine kontrollierte Optimierung innerhalb der bestehenden Security-, Resource- und Data-Movement-Architektur.