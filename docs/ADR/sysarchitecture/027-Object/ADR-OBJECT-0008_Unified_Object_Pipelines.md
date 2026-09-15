# ADR-OBJECT-0008 – Unified Object Pipelines

## Status

Angenommen

## Kategorie

Object Architecture / Pipelines / Composability

## Kontext

NovaOS soll Daten und Systemobjekte zwischen Capabilities verarbeiten können, ohne für jede Kombination spezielle Integrationen zu benötigen.

Dateien, Streams, Geräte, Datenobjekte und andere Ressourcen basieren bereits auf dem Unified System Object Model und können semantisch typisiert werden.

Darauf soll ein gemeinsames Pipeline-Modell aufbauen.

## Entscheidung

NovaOS verwendet **Unified Object Pipelines**.

Objekte können direkt zwischen kompatiblen Capabilities weitergereicht werden.

```text
Object
  ↓
Capability A
  ↓
Object
  ↓
Capability B
  ↓
Object
```

Die Pipeline basiert auf Object Identity, Semantic Types und Capability Contracts.

## Grundmodell

Eine Pipeline besteht aus:

```text
Input Object
    ↓
Capability
    ↓
Output Object
```

Mehrere Schritte können verkettet werden:

```text
Object A
   ↓
Capability A
   ↓
Object B
   ↓
Capability B
   ↓
Object C
```

## Typprüfung

Jeder Pipeline-Schritt muss semantisch kompatibel sein.

```text
Output Type A
      ↓
Contract Check
      ↓
Input Type B
```

Inkompatible Übergänge werden nicht ausgeführt.

## Konvertierung

Falls erforderlich, können definierte Semantic Conversions eingefügt werden.

```text
Object<Type A>
      ↓
Conversion
      ↓
Object<Type B>
      ↓
Capability
```

Lossy Conversions müssen entsprechend ihrer Policy behandelt werden.

## Object Identity

Pipelines dürfen sowohl bestehende Objekte verändern als auch neue Objekte erzeugen.

```text
Mutable Operation
→ gleiche Object ID + neue Version

Transform Operation
→ neue Object ID
```

Die jeweilige Semantik muss durch den Capability Contract definiert sein.

## Provenance

Pipeline-Schritte können Provenance erzeugen.

```text
Source Object
      ↓
Capability
      ↓
Result Object
      ↓
Provenance
```

Dadurch bleibt nachvollziehbar, aus welchen Objekten und Operationen ein Ergebnis entstanden ist.

## Zero-Copy

Object Pipelines sollen unnötige Datenkopien vermeiden.

```text
Object Handle
     ↓
Capability A
     ↓
Capability B
```

Wo technisch möglich, werden Handles, Shared Memory oder andere Zero-Copy-Mechanismen verwendet.

## Execution

Die Ausführung einer Pipeline kann durch NovaOS geplant werden.

Dabei können berücksichtigt werden:

```text
Semantic Compatibility
Execution Contracts
Resource Budget
Trust
Locality
Hardware Availability
```

## Fehler

Ein Fehler in einem Pipeline-Schritt muss eindeutig einem Schritt und Objektzustand zugeordnet werden können.

Nachfolgende Schritte dürfen nicht mit einem ungültigen Ergebnis fortgesetzt werden.

## Normative Anforderungen

1. NovaOS MUSS ein gemeinsames Pipeline-Modell für Systemobjekte unterstützen.
2. Pipeline-Schritte MÜSSEN über Capability Contracts beschreibbar sein.
3. Ein- und Ausgaben MÜSSEN semantisch kompatibel sein.
4. Semantic Conversions MÜSSEN als Pipeline-Schritte verwendbar sein.
5. Object Identity und Object Versioning MÜSSEN während der Pipeline erhalten beziehungsweise eindeutig weitergeführt werden.
6. Pipeline-Schritte MÜSSEN Provenance erzeugen können.
7. Object Pipelines SOLLEN Zero-Copy verwenden, wenn dies technisch möglich ist.
8. Fehlgeschlagene Pipeline-Schritte DÜRFEN keine ungültigen Ergebnisse an nachfolgende Schritte weitergeben.
9. Pipeline-Ausführung MUSS Execution Contracts berücksichtigen können.

## Konsequenzen

### Positive Konsequenzen

- einheitliche Verarbeitung unterschiedlicher Systemobjekte,
- Capabilities können frei kombiniert werden,
- weniger anwendungsspezifische Integrationen,
- Provenance und Versionierung bleiben über Verarbeitungsketten erhalten,
- Zero-Copy kann systemweit genutzt werden.

### Negative Konsequenzen

- Pipeline-Planung benötigt Contract- und Typprüfung,
- komplexe Pipelines benötigen Fehler- und Lifecycle-Management.

## Abhängigkeiten

- `ADR-ARCH-0006_Zero_Copy`
- `ADR-ARCH-0009_Daten_und_Objekt_Pipelining`
- `ADR-OBJECT-0001_Unified_System_Object_Model`
- `ADR-OBJECT-0003_Object_Versioning`
- `ADR-OBJECT-0004_Object_Provenance`
- `ADR-OBJECT-0007_Immutable_und_Mutable_Object_Semantik`
- `ADR-SEMANTIC-0006_Typed_Capabilities`
- `ADR-SEMANTIC-0009_Semantic_Conversion`
- `ADR-SEMANTIC-0012_Semantic_Execution`

## Zugehörige NPSPECs

- `NPSPEC-OBJECT-PIPELINE-0001`
- `NPSPEC-OBJECT-PIPELINE-CONTRACT-0001`
- `NPSPEC-OBJECT-PIPELINE-EXECUTION-0001`

## Ergebnis

NovaOS kann Objekte über ein gemeinsames Modell durch unterschiedliche Capabilities verarbeiten:

```text
Typed Object
     ↓
Capability
     ↓
Typed Object
     ↓
Capability
     ↓
Typed Object
```

Der zentrale Grundsatz lautet:

```text
Objekte werden nicht
an einzelne Programme gebunden.

Sie fließen durch
kompatible Capabilities.
```