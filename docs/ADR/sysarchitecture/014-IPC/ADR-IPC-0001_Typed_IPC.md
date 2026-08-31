# ADR-IPC-0001 – Typed IPC

## Status

Angenommen

## Kategorie

IPC / Semantic Types / Contracts / Communication / Security

## Kontext

Inter-Process Communication verbindet voneinander isolierte Execution Domains.

Klassische IPC-Systeme behandeln Nachrichten häufig primär als Bytefolgen, Speicherbereiche oder untypisierte Nachrichten. Die eigentliche Bedeutung der übertragenen Daten wird dadurch in anwendungsspezifische Protokolle verlagert.

NovaOS besitzt dagegen systemweite Semantic Types, Explicit Contracts, Shared Objects und ein einheitliches Data/Object Pipelining.

Diese Typinformation soll an IPC-Grenzen nicht verloren gehen.

---

## Entscheidung

NovaOS verwendet **Typed IPC als natives IPC-Modell**.

IPC-Endpunkte und Nachrichten besitzen explizite, maschinenprüfbare Typen und Contracts.

```text
Sender
   ↓
Typed Message / Object
   ↓
IPC Contract
   ↓
Receiver
```

Die IPC-Infrastruktur kennt damit nicht nur die transportierten Bytes, sondern auch deren vereinbarte strukturelle und semantische Bedeutung.

---

## Grundprinzip

```text
IPC transfers typed information,
not anonymous bytes.
```

---

## IPC Contracts

Kommunikation erfolgt über explizite IPC Contracts.

Ein Contract beschreibt mindestens die für die Kommunikation relevanten:

- Operationen,
- Input Types,
- Output Types,
- Fehlersemantik,
- erforderlichen Authorities.

Konkrete Serialisierungs-, ABI- und Wire-Format-Regeln werden in NPSPECs definiert.

---

## Semantic Types

Typed IPC integriert die systemweiten Semantic Types.

```text
Raw Bytes
    ↓
Structured Type
    ↓
Semantic Type
```

Dadurch kann beispielsweise zwischen beliebigen Bytefolgen und semantisch definierten Daten wie Bild, Audio, Dokument, Device Event oder Storage Object unterschieden werden.

IPC darf Semantic-Type-Information nicht unnötig auf primitive Datentypen reduzieren.

---

## Type Identity

IPC-Typen benötigen stabile und versionierbare Type Identities.

Die Identität eines Typs darf nicht allein von:

- Speicherlayout,
- Programmiersprache,
- Prozessadresse,
- Compilerimplementation

abhängen.

Damit können unterschiedliche Execution Domains und zukünftige Implementierungen denselben logischen Contract verstehen.

---

## Typprüfung

Sender und Empfänger müssen bezüglich des verwendeten IPC Contracts kompatibel sein.

```text
Sender Type
     ↓
Contract Validation
     ↓
Receiver Type
```

Inkompatible Typen dürfen nicht stillschweigend als kompatible Daten interpretiert werden.

Explizit definierte Konvertierungen können über geeignete Capabilities oder Transformationen erfolgen.

---

## Sprachunabhängigkeit

Typed IPC ist Bestandteil der NovaOS-Systemarchitektur und nicht ausschließlich an NovaLang gebunden.

NovaLang soll Typed IPC nativ abbilden können, andere Sprachen können dieselben IPC Contracts über geeignete Bindings verwenden.

Die systemweite Type Identity bleibt dabei sprachunabhängig.

---

## Datenübertragung

Typed IPC definiert die Semantik der Kommunikation, nicht zwangsläufig eine bestimmte physische Transportform.

Abhängig von Datentyp und Größe können beispielsweise verwendet werden:

```text
Small Typed Message
        ↓
Direct IPC Transfer

Large Typed Object
        ↓
Shared Buffer / Memory Object
```

Damit bleibt Typed IPC mit Zero-Copy und Copy-Avoidance vereinbar.

---

## Authority

Die Kenntnis eines IPC-Typs oder Endpunkts verleiht keine Authority.

```text
Type Compatibility
        +
IPC Capability
        ↓
Communication Allowed
```

Der Zugriff auf IPC-Endpunkte wird durch das NovaOS Capability- und Security-Modell kontrolliert.

Typprüfung ersetzt keine Autorisierung.

---

## Versionierung

IPC Contracts und ihre Typen müssen evolvierbar sein.

Kompatible Erweiterungen sollen möglich sein, ohne alle beteiligten Komponenten gleichzeitig aktualisieren zu müssen.

Inkompatible Änderungen benötigen eine neue Contract- beziehungsweise Type-Version oder eine explizite Adaptionsschicht.

Dies unterstützt die NovaOS-Prinzipien Evolvable Architecture und Live Evolution.

---

## Introspection

Typed IPC soll kontrolliert introspektierbar sein.

NovaOS kann dadurch unter anderem erkennen:

```text
Endpoint
   ↓
IPC Contract
   ↓
Input / Output Types
   ↓
Connected Execution Domains
```

Sicherheitskritische Inhalte oder nicht autorisierte Kommunikationsdetails dürfen dabei nicht offengelegt werden.

---

## Normative Anforderungen

1. NovaOS MUSS Typed IPC als natives IPC-Modell unterstützen.
2. IPC Contracts MÜSSEN Input-, Output- und Fehlersemantik explizit beschreiben können.
3. IPC MUSS die systemweiten Semantic Types unterstützen.
4. IPC Type Identities MÜSSEN stabil und versionierbar sein können.
5. Type Identity DARF NICHT von virtuellen Adressen oder einem konkreten Speicherlayout abhängig sein.
6. Sender und Empfänger MÜSSEN bezüglich des verwendeten IPC Contracts kompatibel sein.
7. Inkompatible Typen DÜRFEN NICHT stillschweigend akzeptiert werden.
8. Typed IPC MUSS sprachunabhängig definierbar sein.
9. NovaLang SOLL Typed IPC nativ abbilden können.
10. Typed IPC MUSS mit Shared Buffers, Memory Objects und Zero-Copy kombinierbar sein.
11. Type Compatibility DARF NICHT als Authority interpretiert werden.
12. IPC-Zugriffe MÜSSEN dem Capability- und Security-Modell unterliegen.
13. IPC Contracts MÜSSEN kontrolliert evolvierbar sein.
14. IPC Contracts und Type-Beziehungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- weniger untypisierte Protokollgrenzen,
- systemweite Semantic Types bleiben über Prozessgrenzen erhalten,
- frühzeitige Erkennung inkompatibler Kommunikation,
- bessere Tooling- und Introspection-Möglichkeiten,
- sprachübergreifende Contracts,
- direkte Integration mit Zero-Copy und Data/Object Pipelines.

### Negative Konsequenzen

- Type Identities und Contract-Versionierung müssen systemweit definiert werden,
- Kompatibilitätsregeln zwischen Versionen benötigen klare Spezifikationen,
- Legacy-Protokolle benötigen gegebenenfalls Adapter.

---

## Verworfene Alternativen

### IPC ausschließlich als Byte-Stream

Verworfen.

Dadurch würde die semantische Typinformation an einer zentralen Systemgrenze verloren gehen.

### IPC-Typen ausschließlich über Speicherlayout definieren

Verworfen.

Dies würde Kommunikation unnötig an ABI, Architektur und Implementierung koppeln.

### Typed IPC ausschließlich für NovaLang

Verworfen.

Systemweite IPC Contracts müssen sprachunabhängig nutzbar bleiben.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0009_Unified_Data_Object_Pipelining`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0019_Evolvable_System_Architecture`
- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-DATAMOVE-0001_Zero_Copy_Datenpfade`
- `ADR-DATAMOVE-0002_Shared_Buffers_als_systemweite_Abstraktion`
- `ADR-MEMORY-0008_Shared_Memory_über_explizite_Shared_Objects`

---

## Zugehörige NPSPECs

- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-IPC-CONTRACT-0001`
- `NPSPEC-IPC-TYPE-IDENTITY-0001`
- `NPSPEC-IPC-TYPE-COMPATIBILITY-0001`
- `NPSPEC-IPC-BINDING-0001`
- `NPSPEC-IPC-TYPED-TEST-0001`

---

## Ergebnis

IPC wird in NovaOS als typisierte Systemkommunikation modelliert:

```text
Execution Domain A
        ↓
Typed IPC Contract
        ↓
Semantic Data / Object
        ↓
Execution Domain B
```

Damit bleiben Struktur und semantische Bedeutung von Daten auch über Isolationsgrenzen hinweg Bestandteil der Systemarchitektur.