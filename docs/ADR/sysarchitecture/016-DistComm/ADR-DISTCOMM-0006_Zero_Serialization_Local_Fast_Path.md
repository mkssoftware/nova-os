# ADR-DISTCOMM-0006 – Zero-Serialization Local Fast Path

## Status

Angenommen

## Kategorie

Distributed Communication / Nova.Call / Zero-Copy / Serialization / Local Fast Path

## Kontext

`Nova.Call` stellt eine einheitliche Abstraktion für lokale, IPC-basierte und entfernte Aufrufe bereit.

Für verteilte Kommunikation verwendet NovaOS Typed Schemas und versionierte Serialisierung, damit Daten unabhängig von ABI, Hardwarearchitektur, Prozessgrenzen und Softwareversionen übertragen werden können.

Bei lokal ausgeführten Calls kann eine vollständige Serialisierung jedoch unnötige Kosten verursachen.

Befinden sich Caller und Callee auf demselben System und existiert eine kompatible, sicher teilbare Datenrepräsentation, soll NovaOS Daten direkt übergeben können, ohne sie zuerst in ein Wire Format zu serialisieren und anschließend wieder zu deserialisieren.

---

## Entscheidung

NovaOS führt einen **Zero-Serialization Local Fast Path** für geeignete lokale `Nova.Call`-Aufrufe ein.

```text
Nova.Call
    ↓
Target Resolution
    ↓
Local Target?
 ├── No  → Versioned Serialization
 └── Yes
       ↓
Compatible Representation?
 ├── No  → Serialization
 └── Yes → Zero-Serialization Fast Path
```

Der Fast Path ist eine Ausführungsoptimierung.

Der logische Call Contract und die Typed-Schema-Semantik bleiben unabhängig vom gewählten Datenpfad identisch.

---

## Grundprinzip

```text
Same contract.
Same semantic types.
Less representation conversion.
```

und:

```text
Zero Serialization
    ≠
Zero Validation
```

---

## Fast-Path-Modell

Der lokale Fast Path darf geeignete Daten direkt als:

- Shared Objects,
- Shared Buffers,
- immutable Objects,
- typed Views,
- kontrollierte Referenzen

zwischen kompatiblen Kommunikationsdomänen übertragen.

Konzeptionell:

```text
Caller Object
     ↓
Typed Shared Representation
     ↓
Nova.Call
     ↓
Callee View
```

Eine Zwischenrepräsentation als klassischer serialisierter Byte Stream ist dabei nicht erforderlich.

---

## Voraussetzung: lokale Ausführung

Zero Serialization ist ausschließlich für Kommunikationspfade zulässig, bei denen NovaOS die notwendige gemeinsame beziehungsweise kontrolliert teilbare Repräsentation bereitstellen kann.

Ein Wechsel auf Remote-Ausführung muss jederzeit auf den normalen serialisierten Pfad zurückfallen können.

```text
Local
  ↓
Zero Serialization possible

Remote
  ↓
Versioned Serialization required
```

Damit bleibt Location Transparency erhalten, ohne Remote-Kommunikation fälschlich wie Shared Memory zu behandeln.

---

## Schema-Kompatibilität

Zero Serialization darf die Typed-Schema-Architektur nicht umgehen.

Caller und Callee müssen eine kompatible Interpretation der gemeinsam verwendeten Daten besitzen.

```text
Type Identity
     +
Schema Compatibility
     +
Representation Compatibility
     =
Fast Path Eligible
```

Schema-Kompatibilität und physische Representation Compatibility sind getrennte Bedingungen.

Zwei semantisch kompatible Schema-Versionen müssen nicht zwangsläufig dasselbe Speicherlayout besitzen.

---

## ABI-Unabhängigkeit

Distributed Contracts bleiben unabhängig vom nativen ABI.

Der Fast Path darf intern ABI- oder laufzeitspezifische Optimierungen verwenden, diese dürfen jedoch nicht Bestandteil des stabilen `Nova.Call`-Contracts werden.

```text
Stable Contract
     ↓
Implementation
 ├── Native Fast Path
 └── Serialized Path
```

Damit kann NovaOS interne Repräsentationen später ändern, ohne Distributed APIs zu brechen.

---

## Ownership und Lifetime

Direkt übergebene Daten benötigen explizite Ownership- und Lifetime-Regeln.

NovaOS muss verhindern, dass:

- Daten vorzeitig freigegeben,
- Referenzen nach Ablauf ihres Lifetimes verwendet,
- mutable Daten unkontrolliert gleichzeitig verändert

werden.

Der Fast Path integriert sich deshalb in die systemweiten Shared-Buffer-, Object- und Structured-Concurrency-Lifetime-Modelle.

---

## Mutable Daten

Gemeinsam veränderbare Objekte dürfen nicht allein zur Vermeidung von Serialisierung unkontrolliert geteilt werden.

Bevorzugt werden:

```text
Immutable Object
Shared Read-Only View
Explicit Shared Mutable Object
Ownership Transfer
```

Mutable Sharing benötigt einen expliziten Contract und geeignete Synchronisation.

---

## Security und Isolation

Zero Serialization darf Execution-Domain-Isolation nicht aufheben.

Ein Callee erhält nur Zugriff auf Daten, die für den jeweiligen Call explizit freigegeben wurden.

```text
Shared Representation
      ↓
Scoped Mapping / Reference
      ↓
Authorized Callee
```

Direkte Pointer oder uneingeschränkter Zugriff auf den Speicher des Callers sind keine zulässige allgemeine Distributed-Call-Semantik.

---

## Capability-Semantik

Der Fast Path verändert keine Capability- oder Authority-Regeln.

```text
Fast Path
    ≠
Trusted Path
```

Remote Capabilities, lokale Capabilities und delegierte Authority müssen unabhängig von der physischen Datenrepräsentation geprüft werden.

---

## Data Sovereignty und Information Flow

Auch lokal geteilte Daten unterliegen Data-Sovereignty- und Information-Flow-Regeln.

Die Vermeidung einer Serialisierung stellt keine Berechtigung zur Datenweitergabe dar.

Ein lokaler Fast Path darf daher nur eingerichtet werden, wenn der Informationsfluss zwischen Caller und Callee zulässig ist.

---

## Fallback

Kann der Fast Path nicht sicher oder korrekt verwendet werden, fällt NovaOS auf einen kompatiblen Standardpfad zurück.

```text
Fast Path unavailable
        ↓
Typed Serialization
        ↓
Normal Nova.Call
```

Der Fallback darf die logische Semantik des Calls nicht verändern.

Ist Zero Serialization lediglich bevorzugt, ist dieser Fallback transparent möglich.

Ist eine bestimmte Eigenschaft explizit als Hard Requirement definiert, muss ein nicht erfüllbarer Contract entsprechend fehlschlagen.

---

## Migration von lokal zu remote

Da Services ihren Ausführungsort ändern können, darf ein Call Contract nicht von Zero Serialization abhängig sein, sofern dies nicht ausdrücklich gefordert wird.

```text
Service Local
    ↓
Zero-Serialization Fast Path

Service Migrated
    ↓
Serialized Remote Path
```

Die Anwendung verwendet weiterhin denselben logischen `Nova.Call`.

Dadurch bleibt die Service-Schnittstelle location-transparent.

---

## Resource Economy

Die Wahl zwischen Zero Serialization und normaler Serialisierung ist eine Ressourcenentscheidung.

NovaOS darf insbesondere berücksichtigen:

- Payload Size,
- Mapping Cost,
- Memory Pressure,
- Cache Locality,
- NUMA Locality,
- Lifetime,
- Synchronization Cost,
- Copy Cost,
- Serialization Cost.

Zero Serialization soll nur verwendet werden, wenn es tatsächlich sinnvoll ist.

---

## Copy Avoidance vor blindem Fast Path

Ein Zero-Serialization-Pfad ist nicht automatisch schneller.

Bei kleinen Objekten können:

- Mapping,
- Referenzverwaltung,
- Synchronisation,
- Cache-Effekte,
- Lifetime Management

teurer sein als eine einfache Kopie oder Serialisierung.

Deshalb gilt:

```text
Avoid unnecessary work,
not every copy at any cost.
```

NovaOS darf für kleine oder ungeeignete Daten bewusst den normalen Pfad wählen.

---

## Deterministic Mode

Im Deterministic Mode muss die Wahl des Datenpfads kontrollierbar sein.

Wenn unterschiedliche Fast-Path-Entscheidungen beobachtbare Laufzeiteigenschaften beeinflussen, muss der Contract beziehungsweise die Policy eine reproduzierbare Auswahl ermöglichen.

Die semantischen Ergebnisse des Calls dürfen durch die Wahl des Pfads nicht verändert werden.

---

## Introspection

NovaOS muss kontrolliert darstellen können, welcher Datenpfad für einen `Nova.Call` verwendet wurde.

Beispielsweise:

```text
Nova.Call
 ├── Target: Local
 ├── Schema: Compatible
 ├── Representation: Compatible
 └── Data Path: ZeroSerialization
```

oder:

```text
Nova.Call
 ├── Target: Local
 ├── Schema: Compatible
 ├── Representation: Incompatible
 └── Data Path: Serialized
```

Damit bleiben Performanceentscheidungen nachvollziehbar.

---

## Normative Anforderungen

1. NovaOS MUSS einen Zero-Serialization Local Fast Path für geeignete lokale `Nova.Call`-Aufrufe unterstützen können.
2. Zero Serialization MUSS eine Optimierung unterhalb des stabilen Call Contracts bleiben.
3. Der Fast Path DARF Typed Schemas und Semantic Types NICHT umgehen.
4. Schema-Kompatibilität und Representation Compatibility MÜSSEN getrennt bewertet werden können.
5. Distributed Contracts DÜRFEN NICHT von einem nativen ABI oder internen Speicherlayout abhängig werden.
6. Direkt geteilte Daten MÜSSEN explizite Ownership- und Lifetime-Regeln besitzen.
7. Mutable Sharing MUSS explizit kontrolliert und synchronisiert werden.
8. Der Fast Path DARF Execution-Domain-Isolation NICHT aufheben.
9. Direkte Datenübergabe DARF Capability-, Authority-, Trust- oder Security-Prüfungen NICHT umgehen.
10. Information-Flow- und Data-Sovereignty-Regeln MÜSSEN auch für lokale Fast Paths gelten.
11. Bei fehlender Fast-Path-Eignung MUSS ein kompatibler serialisierter Fallback möglich sein, sofern der Contract dies zulässt.
12. Ein Wechsel zwischen lokaler und entfernter Ausführung DARF den logischen `Nova.Call`-Contract NICHT verändern.
13. Zero Serialization DARF NICHT allein aufgrund technischer Verfügbarkeit bevorzugt werden, wenn ein anderer Datenpfad günstiger ist.
14. Die Auswahl MUSS Resource Economy, Locality und Datenbewegung berücksichtigen können.
15. Deterministic Mode MUSS die Fast-Path-Auswahl kontrollieren können.
16. Die verwendete Datenrepräsentation und relevante Fallback-Entscheidungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- lokale `Nova.Call`-Aufrufe können Serialisierungs- und Kopierkosten vermeiden,
- große Datenobjekte können effizient zwischen lokalen Diensten übertragen werden,
- Shared Buffers und Zero-Copy werden direkt mit Distributed Communication verbunden,
- derselbe Call Contract funktioniert weiterhin für lokale und entfernte Ausführung,
- Schema Evolution und Location Transparency bleiben erhalten,
- NovaOS kann je nach Daten- und Systemzustand den günstigsten Datenpfad wählen.

### Negative Konsequenzen

- zusätzlich zur Schema-Kompatibilität muss Representation Compatibility geprüft werden,
- Ownership und Lifetime gemeinsam genutzter Daten werden komplexer,
- Fast-Path-Entscheidungen benötigen zusätzliche Introspection,
- lokale und entfernte Ausführung können unterschiedliche Performancecharakteristiken besitzen.

---

## Verworfene Alternativen

### Immer serialisieren

Verworfen.

Dies würde bei lokalen Calls unnötige CPU-, Speicher- und Kopierkosten erzeugen.

### Direkte Pointer zwischen Execution Domains

Verworfen.

Unkontrollierte Pointer würden Isolation, Lifetime-Sicherheit und Evolvierbarkeit verletzen.

### Zero Serialization als Bestandteil des API Contracts

Verworfen.

Dies würde Anwendungen an lokale Ausführung und konkrete Speicherrepräsentationen koppeln und Location Transparency beschädigen.

### Zero Serialization immer bevorzugen

Verworfen.

Für kleine oder kurzlebige Daten kann eine Kopie beziehungsweise Serialisierung günstiger sein als Shared-Memory- und Lifetime-Management.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0006_Zero_Copy`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0009_Unified_Data_Object_Pipelining`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-DATAMOVE-0001_Zero_Copy_Datenpfade`
- `ADR-DATAMOVE-0002_Shared_Buffers_als_systemweite_Abstraktion`
- `ADR-DATAMOVE-0005_Copy_Avoidance_vor_blinder_Zero_Copy_Optimierung`
- `ADR-DATAMOVE-0006_Data_und_Cache_Locality`
- `ADR-IPC-0001_Typed_IPC`
- `ADR-IPC-0003_Zero_Copy_IPC`
- `ADR-DISTCOMM-0001_Nova_Call_als_einheitliche_Distributed_Call_Abstraktion`
- `ADR-DISTCOMM-0002_Location_Transparency_ohne_Kosten_und_Fehler_zu_verbergen`
- `ADR-DISTCOMM-0005_Typed_Schemas_und_versionierte_Serialisierung`

---

## Zugehörige NPSPECs

- `NPSPEC-DISTCOMM-ZERO-SERIALIZATION-0001`
- `NPSPEC-DISTCOMM-LOCAL-FAST-PATH-0001`
- `NPSPEC-DISTCOMM-REPRESENTATION-COMPATIBILITY-0001`
- `NPSPEC-DISTCOMM-SHARED-OBJECT-0001`
- `NPSPEC-DISTCOMM-FAST-PATH-LIFETIME-0001`
- `NPSPEC-DISTCOMM-FAST-PATH-FALLBACK-0001`
- `NPSPEC-DISTCOMM-ZERO-SERIALIZATION-TEST-0001`

---

## Ergebnis

NovaOS kann lokale `Nova.Call`-Kommunikation ohne unnötige Serialisierung ausführen:

```text
                     Nova.Call
                         ↓
                  Target Resolution
                         ↓
              ┌──────────┴──────────┐
              ↓                     ↓
            Local                 Remote
              ↓                     ↓
 Schema + Representation       Typed Schema
      Compatibility                 ↓
              ↓                Serialization
      ┌───────┴───────┐             ↓
      ↓               ↓         Network Path
Compatible       Incompatible
      ↓               ↓
Zero-Serialization  Serialization
      ↓               ↓
      └───────┬───────┘
              ↓
        Same Call Contract
```

Damit kann NovaOS die Vorteile direkter lokaler Datenübergabe nutzen, ohne Typed Schemas, Isolation, Evolvierbarkeit oder die einheitliche `Nova.Call`-Abstraktion aufzugeben.