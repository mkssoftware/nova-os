# ADR-EXECUTION-0002 – Semantic Input und Output Types im ExecutionContract

## Status

Angenommen

## Kategorie

Execution Architecture / Semantic Types

## Kontext

Primitive Datentypen beschreiben die technische Repräsentation von Daten, aber nicht zwingend deren fachliche Bedeutung.

```text
float
int
byte[]
string
```

Für die automatische Auswahl von Capabilities, Algorithmen und Compute Providern muss NovaOS wissen, **welche semantische Bedeutung Ein- und Ausgaben besitzen**.

Beispiele:

```text
float[] → Audio.PCM.Samples
byte[]  → Image.Encoded.JPEG
matrix  → Math.Matrix.Float32
object  → Simulation.ParticleSet
```

Der `Nova.ExecutionContract` benötigt deshalb semantisch typisierte Ein- und Ausgaben.

## Entscheidung

`Nova.ExecutionContract` beschreibt Inputs und Outputs primär über **Semantic Types**.

```text
Semantic Input Types
        ↓
Nova.ExecutionContract
        ↓
Execution Planning
        ↓
Capability / Algorithm / Provider
        ↓
Semantic Output Types
```

Primitive Typen bleiben Bestandteil der technischen Repräsentation, sind jedoch nicht die alleinige Grundlage der Ausführungsplanung.

## Semantic Input Types

Jeder relevante Input kann einen Semantic Type besitzen.

```text
Input
 ├── Primitive Representation
 └── Semantic Type
```

Beispiel:

```text
Representation: float32[]
Semantic Type:  Audio.PCM.Samples
```

Dadurch kann NovaOS erkennen, welche Operationen mit den Daten semantisch zulässig sind.

## Semantic Output Types

Der Execution Contract beschreibt auch die erwartete Bedeutung des Ergebnisses.

```text
Input:
    Image.Raster.RGB

Capability:
    Image.Resize

Output:
    Image.Raster.RGB
```

Der Output Type ist Bestandteil des Contracts und nicht lediglich eine Beschreibung des zurückgegebenen Speicherformats.

## Mehrere Inputs und Outputs

Execution Contracts können mehrere semantisch typisierte Ein- und Ausgaben besitzen.

```text
Input A ──┐
Input B ──┼→ Execution → Output A
Input C ──┘              Output B
```

Jeder Ein- und Ausgang wird separat typisiert.

## Type Compatibility

Ein Algorithmus oder Provider darf nur verwendet werden, wenn seine unterstützten Typen mit dem Execution Contract kompatibel sind.

```text
Required Semantic Type
         ↓
Type Compatibility
         ↓
Supported / Rejected
```

Technisch identische Speicherrepräsentationen gelten nicht automatisch als semantisch kompatibel.

## Semantic Validation

Type Compatibility und Datenvalidierung bleiben getrennte Schritte.

```text
Semantic Type Check
        ↓
Semantic Validation
        ↓
Execution
```

Ein Objekt kann den richtigen Semantic Type besitzen und dennoch ungültige Inhalte enthalten.

## Semantic Conversion

Sind Input und erwarteter Typ nicht direkt kompatibel, kann eine explizite Semantic Conversion verwendet werden.

```text
Source Type
    ↓
Semantic Conversion
    ↓
Required Type
```

Konvertierungen müssen ihre Eigenschaften deklarieren.

Beispiele:

```text
Lossless
Lossy
Precision Changing
Representation Only
```

NovaOS darf keine semantisch relevante Konvertierung stillschweigend durchführen.

## Algorithmusauswahl

Semantic Types begrenzen die gültigen Algorithmen.

```text
Semantic Inputs
      +
Required Output
      ↓
Algorithm Registry
      ↓
Compatible Algorithms
```

Erst danach werden weitere Execution Constraints ausgewertet.

## Providerwahl

Compute Provider müssen die für den gewählten Algorithmus erforderlichen Repräsentationen verarbeiten können.

```text
Semantic Type
      ↓
Algorithm
      ↓
Physical Representation
      ↓
Compute Provider
```

Semantic Type und physische Datenrepräsentation bleiben getrennte Konzepte.

## Typed Pipelines

Semantische Inputs und Outputs ermöglichen direkt typisierte Execution Pipelines.

```text
Object A
[Image.Raster]
      ↓
Capability
      ↓
Object B
[Image.Edges]
      ↓
Capability
      ↓
Object C
[Geometry.Contours]
```

Inkompatible Pipeline-Verbindungen können vor der Ausführung erkannt werden.

## Metadata

Semantic Metadata kann einen Type ergänzen, ersetzt ihn jedoch nicht.

```text
Semantic Type:
    Image.Raster

Metadata:
    Width
    Height
    Color Space
    Resolution
```

Metadata kann für Algorithmusauswahl und Contract Validation verwendet werden.

## Type Versioning

Semantic Types müssen evolvierbar sein.

Execution Contracts müssen daher Typversionen oder definierte Kompatibilitätsregeln berücksichtigen können.

```text
Semantic Type
    +
Version
    ↓
Compatibility Check
```

Eine neue Typversion darf nicht automatisch als kompatibel angenommen werden.

## Normative Anforderungen

1. `Nova.ExecutionContract` MUSS Semantic Types für Inputs und Outputs ausdrücken können.
2. Primitive Datentypen DÜRFEN nicht als alleinige semantische Typinformation verwendet werden.
3. Jeder relevante Input und Output MUSS separat typisierbar sein.
4. Algorithmusauswahl MUSS Semantic Type Compatibility berücksichtigen.
5. Technisch identische Repräsentationen DÜRFEN nicht automatisch als semantisch kompatibel gelten.
6. Semantic Type Checking und Semantic Validation MÜSSEN getrennte Konzepte bleiben.
7. Notwendige Semantic Conversions MÜSSEN explizit modelliert werden.
8. Verlustbehaftete Konvertierungen DÜRFEN nicht stillschweigend erfolgen.
9. Semantic Types und physische Datenrepräsentation MÜSSEN getrennt bleiben.
10. Typed Execution Pipelines MÜSSEN vor der Ausführung auf Typkompatibilität prüfbar sein.
11. Semantic Metadata DARF Semantic Types ergänzen, aber nicht ersetzen.
12. Semantic Type Versioning und Compatibility MÜSSEN unterstützt werden können.
13. Output Types MÜSSEN Bestandteil der Contract Verification sein können.

## Konsequenzen

### Positive Konsequenzen

- fachlich falsche Datenverbindungen können früh erkannt werden,
- Algorithmusauswahl erhält semantische Informationen,
- Capabilities lassen sich sicher zu Pipelines kombinieren,
- technische Repräsentation und fachliche Bedeutung bleiben getrennt,
- automatische Conversion kann kontrolliert geplant werden.

### Negative Konsequenzen

- Semantic Types benötigen stabile Definitionen und Kompatibilitätsregeln,
- Legacy-Daten benötigen gegebenenfalls zusätzliche Typinformationen.

## Abhängigkeiten

- `ADR-EXECUTION-0001_Nova_ExecutionContract`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0009_Daten_und_Objekt_Pipelining`
- `ADR-SEMANTIC-0001_Semantic_Types_zusätzlich_zu_primitiven_Typen`
- `ADR-SEMANTIC-0002_Strong_Semantic_Typing`
- `ADR-SEMANTIC-0009_Semantic_Conversion`
- `ADR-SEMANTIC-0010_Semantic_Validation`
- `ADR-SEMANTIC-0012_Semantic_Execution`
- `ADR-ALGORITHM-0001_Algorithm_Families`
- `ADR-ALGORITHM-0008_Algorithm_Registry`

## Zugehörige NPSPECs

- `NPSPEC-EXECUTION-SEMANTIC-INPUT-0001`
- `NPSPEC-EXECUTION-SEMANTIC-OUTPUT-0001`
- `NPSPEC-EXECUTION-TYPE-COMPATIBILITY-0001`
- `NPSPEC-EXECUTION-TYPE-CONVERSION-0001`

## Ergebnis

Semantic Types werden Bestandteil des Execution Contracts und bilden die semantische Grenze jeder Ausführung:

```text
Semantic Input
      ↓
Nova.ExecutionContract
      ↓
Type Compatibility
      ↓
Algorithm Selection
      ↓
Provider Selection
      ↓
Execution
      ↓
Semantic Output
      ↓
Contract Verification
```

Der zentrale Grundsatz lautet:

```text
NovaOS muss nicht nur wissen,
wie Daten gespeichert sind.

Es muss wissen,
was diese Daten bedeuten
und welches Ergebnis
semantisch erwartet wird.
```