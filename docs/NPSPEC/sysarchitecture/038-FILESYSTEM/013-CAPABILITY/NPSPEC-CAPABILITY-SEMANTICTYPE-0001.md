# NPSPEC-CAPABILITY-SEMANTICTYPE-0001 – Nova Capability Semantic Type

## Status

Angenommen

## Kategorie

Capability / Semantic Type

## Zweck

NovaOS definiert semantische Typen als gemeinsame Bedeutungsebene für Ein- und Ausgaben von Capabilities.

Capabilities beschreiben dadurch, welche Art von Information sie benötigen oder erzeugen, ohne an konkrete Dateiformate, Speicherorte, Programme oder Provider gebunden zu sein.

## Grundprinzipien

```text
SemanticType ≠ File Format
SemanticType ≠ File Extension
SemanticType ≠ ObjectID
SemanticType ≠ Capability
SemanticType ≠ Authority
Semantic Compatibility ≠ Permission
```

## Modell

Ein semantischer Typ wird über eine stabile Identität referenziert:

```text
SemanticTypeID
```

Die Type Registry stellt dazu mindestens bereit:

```text
SemanticType
├── SemanticTypeID
├── Version
├── Definition
├── Constraints
├── Relations
└── Compatibility
```

## Capability-Vertrag

Capabilities verwenden semantische Typen zur Beschreibung ihrer Datenflüsse:

```text
Semantic Input
      ↓
Capability
      ↓
Semantic Output
```

Beispiel:

```text
Image
  ↓
de.nova.image.filter.gaussian
  ↓
Image
```

Ein konkretes Bild kann dabei beispielsweise als PNG, JPEG, NovaFile, Buffer oder Stream vorliegen.

## Trennung vom Datenformat

Semantischer Typ und physische Repräsentation bleiben getrennt:

```text
Semantic Type: Image

Possible Representations:
├── PNG
├── JPEG
├── AVIF
├── NovaFile
├── Shared Buffer
└── Stream
```

Codec- oder Konvertierungs-Capabilities können zwischen konkreten Repräsentationen vermitteln.

## Kompatibilität

NovaOS muss prüfen können, ob ein Output semantisch als Input einer weiteren Capability geeignet ist:

```text
Capability A
    ↓
SemanticType X
    ↓
Compatibility Check
    ↓
Capability B
```

Dadurch können Capability-Pipelines automatisch zusammengesetzt und validiert werden.

## Spezialisierung

Semantische Typen dürfen Beziehungen besitzen:

```text
Media
  ↓
Image
  ↓
RasterImage
```

Eine solche Beziehung beschreibt Typkompatibilität, nicht Authority oder Berechtigungsvererbung.

## Logic Graph

Solutions können semantische Typen verwenden, um Verbindungen zwischen Capability-Knoten zu validieren:

```text
Capability A
    ↓ Image
Custom Script
    ↓ Image
Capability B
```

Inkompatible Verbindungen müssen bereits vor oder spätestens bei der Ausführung erkannt werden können.

## Providerunabhängigkeit

Alle Provider derselben Capability müssen die im Capability Interface definierten semantischen Typen kompatibel behandeln.

Provider-spezifische interne Datentypen dürfen nicht den öffentlichen semantischen Vertrag ersetzen.

## Sicherheit

Semantische Typinformation erzeugt keine Authority.

```text
Known SemanticTypeID
        ≠
Access to Data
```

Der Zugriff auf konkrete Objekte, Buffer oder Streams bleibt capability- und policy-basiert.

## Normative Anforderungen

1. NovaOS MUSS stabile `SemanticTypeID`s unterstützen.
2. Capability-Inputs und -Outputs MÜSSEN semantische Typen deklarieren können.
3. Semantischer Typ und physisches Datenformat MÜSSEN getrennt bleiben.
4. Semantische Typen MÜSSEN über die Type Registry auflösbar sein.
5. Typen MÜSSEN versionierbar sein.
6. Beziehungen und Kompatibilität zwischen Typen MÜSSEN ausdrückbar sein.
7. Capability-Pipelines MÜSSEN semantisch validierbar sein.
8. Logic Graphs MÜSSEN inkompatible Datenverbindungen erkennen können.
9. Provider MÜSSEN den öffentlichen semantischen Vertrag ihrer Capability einhalten.
10. Formatkonvertierung DARF die semantische Identität nicht automatisch verändern.
11. Kenntnis eines semantischen Typs DARF keine Daten-Authority erzeugen.
12. SemanticTypeID, Version, Beziehungen und Kompatibilität MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-INPUT-0001`
- `NPSPEC-CAPABILITY-OUTPUT-0001`
- `NPSPEC-CAPABILITY-PARAMETER-0001`
- `NPSPEC-REGISTRY-TYPE-0001`
- `NPSPEC-REGISTRY-CODEC-0001`
- `NPSPEC-FILESYSTEM-SEMANTIC-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`

## Ergebnis

NovaOS besitzt mit semantischen Typen eine gemeinsame Bedeutungsebene für Capability-Datenflüsse. Dadurch können Capabilities, Solutions und Logic Graphs unabhängig von konkreten Dateiformaten, Providern und Speicherorten kombiniert und automatisch auf Kompatibilität geprüft werden.