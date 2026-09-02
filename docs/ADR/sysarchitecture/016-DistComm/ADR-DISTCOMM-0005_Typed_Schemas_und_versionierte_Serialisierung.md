# ADR-DISTCOMM-0005 – Typed Schemas und versionierte Serialisierung

## Status

Angenommen

## Kategorie

Distributed Communication / Serialization / Semantic Types / Schema Evolution / Nova.Call

## Kontext

`Nova.Call` ermöglicht typisierte Kommunikation über lokale Execution-Domain-, IPC- und Netzwerkgrenzen hinweg.

Sobald Daten eine Adressraum-, Prozess-, Architektur- oder Systemgrenze überschreiten, kann ihre interne Speicherrepräsentation jedoch nicht als stabiles Kommunikationsformat betrachtet werden.

Unterschiedliche Teilnehmer können:

- unterschiedliche Softwareversionen,
- unterschiedliche CPU-Architekturen,
- unterschiedliche interne Datenlayouts,
- unterschiedliche Implementierungen

verwenden.

NovaOS benötigt deshalb ein stabiles, typisiertes und evolvierbares Serialisierungsmodell für Distributed Communication.

Dieses Modell muss Schema Evolution ermöglichen, ohne jeden Versionsunterschied automatisch zu einem Protokollbruch zu machen.

---

## Entscheidung

NovaOS verwendet **Typed Schemas mit explizit versionierter und evolvierbarer Serialisierung** für Distributed Communication.

```text
Semantic Object
      ↓
Typed Schema
      ↓
Versioned Serialization
      ↓
Transport
      ↓
Versioned Deserialization
      ↓
Semantic Object
```

Die logische Bedeutung eines Datentyps wird vom konkreten Speicherlayout und vom verwendeten Transport getrennt.

---

## Grundprinzip

```text
Semantic Type
    ≠
Memory Layout
    ≠
Wire Representation
```

und:

```text
Schemas evolve.
Meaning must remain explicit.
```

---

## Typed Schemas

Distributed übertragbare Daten besitzen ein explizites Schema.

Ein Schema beschreibt insbesondere:

- Type Identity,
- Fields,
- Field Types,
- Semantic Types,
- Required und Optional Properties,
- Constraints,
- Version beziehungsweise Evolution Metadata.

Die konkrete Schema-Syntax wird in NPSPECs definiert.

---

## Stabile Type Identity

Typen benötigen eine stabile Identität, die nicht ausschließlich von:

- Dateinamen,
- Speicheradressen,
- Compiler-internen Typnummern,
- Prozesszustand

abhängt.

```text
Type Identity
     ↓
Schema
     ↓
Version
```

Die Type Identity bleibt über kompatible Schema-Evolution hinweg erhalten.

---

## Versionierte Serialisierung

Serialisierte Daten müssen ausreichend Versionsinformationen enthalten oder über ihren Contract eindeutig einer Schema-Version zugeordnet werden können.

```text
Type
 ├── Schema Identity
 ├── Schema Version
 └── Payload
```

Die konkrete Wire-Repräsentation darf je nach Provider oder Anwendungsfall variieren, solange die erforderliche Semantik erhalten bleibt.

---

## Schema Evolution

NovaOS unterscheidet zwischen kompatiblen und inkompatiblen Schemaänderungen.

Typische kompatible Evolution kann beispielsweise umfassen:

- Hinzufügen optionaler Felder,
- Einführung neuer optionaler Eigenschaften,
- Erweiterung bekannter Wertebereiche, sofern semantisch zulässig.

Potentiell inkompatible Änderungen umfassen beispielsweise:

- Änderung der Bedeutung eines bestehenden Feldes,
- inkompatible Änderung eines Feldtyps,
- Entfernung zwingend erforderlicher Semantik.

Die exakten Compatibility Rules werden in NPSPECs definiert.

---

## Keine implizite Bedeutungsänderung

Ein bestehendes Feld darf nicht lediglich unter Beibehaltung seiner technischen Repräsentation eine neue semantische Bedeutung erhalten.

```text
Same Bytes
    ≠
Same Meaning
```

Semantisch inkompatible Änderungen benötigen eine neue Schema-Version beziehungsweise gegebenenfalls eine neue Type Identity.

---

## Unknown Fields

Empfänger müssen unbekannte Felder entsprechend der jeweiligen Schema- und Compatibility-Regeln behandeln können.

```text
Known Field   → Interpret
Unknown Field → Preserve / Ignore / Reject
```

Die konkrete Behandlung hängt vom Contract und der Schema-Semantik ab.

Unbekannte optionale Erweiterungen sollen nicht automatisch zu einem Kommunikationsabbruch führen.

---

## Required Fields

Fehlt eine für die Interpretation zwingend erforderliche Information, darf der Empfänger keine Bedeutung erfinden.

```text
Required Semantic Data Missing
            ↓
       Explicit Failure
```

Default-Werte dürfen nur verwendet werden, wenn sie durch das Schema semantisch definiert sind.

---

## Schema Negotiation

Kommunikationspartner können unterstützte Schema-Versionen beziehungsweise kompatible Repräsentationen aushandeln.

```text
Caller Versions
       ↓
Schema Negotiation
       ↑
Callee Versions
       ↓
Compatible Representation
```

Negotiation soll bevorzugt auf kompatiblen Schema- beziehungsweise Contract-Eigenschaften basieren und nicht auf hart codierten Softwareversionsnummern.

---

## Integration mit Nova.Call

`Nova.Call` verwendet Typed Schemas für Parameter, Ergebnisse und strukturierte Fehler.

```text
Nova.Call
 ├── Typed Arguments
 ├── Typed Result
 └── Typed Error
```

Call Contracts können festlegen, welche Schema- oder Kompatibilitätsanforderungen gelten.

---

## Semantic Types

Typed Serialization integriert sich direkt mit den Semantic Types von NovaOS.

Beispielsweise sollen semantisch unterschiedliche Werte nicht allein deshalb austauschbar sein, weil sie dieselbe primitive Repräsentation besitzen.

```text
Milliseconds
Meters
Bytes
UserIdentity
Temperature
```

können technisch primitive Werte verwenden, bleiben jedoch semantisch unterschiedliche Typen.

Serialisierung muss diese Bedeutung erhalten können.

---

## Architekturunabhängigkeit

Wire-Repräsentationen dürfen nicht von der nativen Speicherrepräsentation einer CPU abhängig sein.

Unterschiede wie:

- Endianness,
- Pointer Width,
- Alignment,
- Padding,
- ABI Layout

dürfen nicht unkontrolliert Bestandteil eines Distributed Contracts werden.

Damit können unterschiedliche Hardwarearchitekturen miteinander kommunizieren.

---

## Zero-Copy und lokale Optimierung

Typed Serialization bedeutet nicht, dass jeder Call zwingend eine vollständige Byte-Serialisierung durchführen muss.

Bei kompatiblen lokalen Kommunikationspfaden darf NovaOS optimierte Repräsentationen verwenden:

```text
Typed Object
    ↓
Shared Object / Shared Buffer
    ↓
Typed View
```

Dabei muss die logische Schema- und Type-Semantik erhalten bleiben.

Zero-Copy ist eine Optimierung und verändert nicht den Contract.

---

## Transportunabhängigkeit

Schemas sind nicht an einen bestimmten Netzwerktransport gebunden.

```text
Typed Schema
      ↓
Nova.Call
      ↓
IPC / TCP / QUIC / Overlay / Future Provider
```

Ein Wechsel des Providers darf die logische Bedeutung der übertragenen Daten nicht verändern.

---

## Security

Deserialisierung ist eine Trust Boundary.

Eingehende Daten müssen gegen das erwartete Schema und relevante Constraints validiert werden.

Insbesondere dürfen:

- ungültige Längen,
- unzulässige Werte,
- fehlerhafte Referenzen,
- unerlaubte Typen,
- Ressourcenmissbrauch durch extrem große Strukturen

nicht ungeprüft übernommen werden.

Schema-Konformität allein erzeugt jedoch keine Authority oder Trust.

---

## Resource Economy

Serialisierung und Deserialisierung unterliegen der systemweiten Resource Economy.

Relevant sind insbesondere:

- Payload Size,
- Memory Allocation,
- CPU Cost,
- Buffer Usage,
- Schema Complexity.

Ein Remote Peer darf durch gültig aussehende, aber unverhältnismäßig komplexe Datenstrukturen keine unbegrenzten lokalen Ressourcen beanspruchen.

---

## Schema Registry und Introspection

NovaOS muss Schema- und Typinformationen kontrolliert introspektierbar machen können.

Konzeptionell:

```text
Type Identity
     ↓
Schema Versions
     ↓
Compatibility Information
     ↓
Consumers / Providers
```

Eine zentrale Registry kann die Verwaltung unterstützen, darf jedoch keine zwingende Laufzeitabhängigkeit für jeden Distributed Call darstellen.

Notwendige Schema-Informationen müssen auch ohne permanente Verfügbarkeit eines zentralen Registry-Dienstes korrekt verarbeitet werden können.

---

## Deterministic Mode

Im Deterministic Mode müssen Serialisierung und Schemaauflösung reproduzierbar sein.

Nicht deterministische automatische Schema- oder Formatentscheidungen müssen kontrolliert beziehungsweise deaktivierbar sein.

Bei identischem Contract und identischer Eingabe soll eine deterministische Repräsentation erzeugbar sein, sofern dies vom jeweiligen Format unterstützt wird.

---

## Normative Anforderungen

1. NovaOS MUSS Typed Schemas für Distributed Communication unterstützen.
2. Distributed Types MÜSSEN eine stabile Type Identity besitzen können.
3. Serialisierte Daten MÜSSEN einer definierten Schema-Version beziehungsweise einem eindeutig bestimmten Schema zugeordnet werden können.
4. Schema Evolution MUSS unterstützt werden.
5. Kompatible und inkompatible Schemaänderungen MÜSSEN unterscheidbar sein.
6. Bestehende Felder DÜRFEN ihre Semantik NICHT implizit verändern.
7. Unbekannte optionale Felder SOLLEN gemäß Schema-Policy verarbeitet werden können, ohne automatisch einen Fehler auszulösen.
8. Fehlende zwingend erforderliche Semantik DARF NICHT durch erfundene Werte ersetzt werden.
9. Schema-Versionen beziehungsweise kompatible Repräsentationen MÜSSEN aushandelbar sein können.
10. `Nova.Call` MUSS Typed Schemas für Argumente, Ergebnisse und strukturierte Fehler verwenden können.
11. Semantic Types MÜSSEN über Serialisierungsgrenzen hinweg erhalten werden können.
12. Wire-Repräsentationen DÜRFEN NICHT unkontrolliert vom nativen ABI oder Speicherlayout einer Plattform abhängen.
13. Lokale Zero-Copy-Optimierungen DÜRFEN die logische Schema-Semantik NICHT verändern.
14. Schema- und Type-Semantik MUSS unabhängig vom verwendeten Transport bleiben.
15. Eingehende serialisierte Daten MÜSSEN vor ihrer Verwendung validiert werden.
16. Deserialisierung MUSS Resource Budgets berücksichtigen können.
17. Schema-Konformität DARF NICHT mit Authority oder Trust gleichgesetzt werden.
18. Schema-Informationen MÜSSEN kontrolliert introspektierbar sein.
19. Eine zentrale Schema Registry DARF keine zwingende Laufzeitabhängigkeit für jeden Call darstellen.
20. Deterministic Mode MUSS reproduzierbare Serialisierungsentscheidungen ermöglichen können.

---

## Konsequenzen

### Positive Konsequenzen

- stabile Kommunikation zwischen unterschiedlichen NovaOS-Versionen,
- Unterstützung heterogener CPU-Architekturen,
- kontrollierte Schema Evolution,
- Semantic Types bleiben über Systemgrenzen erhalten,
- Services können unabhängig voneinander weiterentwickelt werden,
- `Nova.Call` erhält ein einheitliches typisiertes Datenmodell,
- lokale Zero-Copy-Pfade bleiben weiterhin möglich.

### Negative Konsequenzen

- Schema-Versionierung erhöht den Entwicklungsaufwand,
- Compatibility Rules müssen langfristig gepflegt werden,
- Deserialisierung benötigt zusätzliche Validierung,
- inkompatible Schemaänderungen benötigen explizite Migration oder neue Versionen.

---

## Verworfene Alternativen

### Native Speicherstrukturen direkt übertragen

Verworfen.

ABI, Alignment, Pointer, Endianness und interne Layoutänderungen würden Distributed Contracts instabil machen.

### Unversionierte Serialisierung

Verworfen.

Ohne Schema Evolution würden Softwareupdates unnötig häufig zu Kommunikationsabbrüchen führen.

### Softwareversionsnummer als Protokollversion

Verworfen.

Kompatibilität soll anhand konkreter Schema- und Contract-Eigenschaften bestimmt werden und nicht allein anhand einer globalen Produktversion.

### Ein einziges fest vorgeschriebenes Wire-Format für alle Fälle

Verworfen.

NovaOS benötigt unterschiedliche Optimierungsmöglichkeiten für lokale IPC-, Netzwerk-, Storage- und zukünftige Kommunikationspfade.

Der logische Schema-Contract bleibt einheitlich; die physische Repräsentation darf providerabhängig optimiert werden.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0009_Unified_Data_Object_Pipelining`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0019_Evolvable_System_Architecture`
- `ADR-DATAMOVE-0001_Zero_Copy_Datenpfade`
- `ADR-DATAMOVE-0002_Shared_Buffers_als_systemweite_Abstraktion`
- `ADR-IPC-0001_Typed_IPC`
- `ADR-IPC-0003_Zero_Copy_IPC`
- `ADR-DISTCOMM-0001_Nova_Call_als_einheitliche_Distributed_Call_Abstraktion`
- `ADR-DISTCOMM-0002_Location_Transparency_ohne_Kosten_und_Fehler_zu_verbergen`
- `ADR-DISTCOMM-0003_Remote_Capabilities`
- `ADR-DISTCOMM-0004_Capability_Delegation_über_Netzwerkgrenzen`

---

## Zugehörige NPSPECs

- `NPSPEC-DISTCOMM-SCHEMA-0001`
- `NPSPEC-DISTCOMM-TYPE-IDENTITY-0001`
- `NPSPEC-DISTCOMM-SERIALIZATION-0001`
- `NPSPEC-DISTCOMM-SCHEMA-VERSIONING-0001`
- `NPSPEC-DISTCOMM-SCHEMA-EVOLUTION-0001`
- `NPSPEC-DISTCOMM-SCHEMA-NEGOTIATION-0001`
- `NPSPEC-DISTCOMM-DESERIALIZATION-SECURITY-0001`
- `NPSPEC-DISTCOMM-SERIALIZATION-TEST-0001`

---

## Ergebnis

NovaOS verwendet für Distributed Communication keine instabilen Speicherrepräsentationen, sondern explizite, typisierte und evolvierbare Schemas:

```text
Semantic Type
      ↓
Stable Type Identity
      ↓
Versioned Schema
      ↓
Serialization
      ↓
Distributed Transport
      ↓
Schema Validation
      ↓
Deserialization
      ↓
Semantic Type
```

Damit können NovaOS-Komponenten unabhängig weiterentwickelt und über unterschiedliche Softwareversionen, Execution Domains, Netzwerkgrenzen und Hardwarearchitekturen hinweg miteinander verbunden werden, ohne die semantische Bedeutung ihrer Daten an ein bestimmtes ABI oder Wire-Format zu koppeln.