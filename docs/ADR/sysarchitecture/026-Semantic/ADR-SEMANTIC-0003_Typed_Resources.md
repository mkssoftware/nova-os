# ADR-SEMANTIC-0003 – Typed Resources

## Status

Angenommen

## Kategorie

Semantic Architecture / Resources / Type System

## Kontext

NovaOS verwaltet zahlreiche Ressourcen wie Dateien, Geräte, Netzwerkendpunkte, Datenströme und Systemobjekte.

Eine Ressource soll nicht nur über Pfad, Handle oder ID beschrieben werden. Das System muss auch erkennen können, welche semantische Bedeutung und welche zulässigen Operationen sie besitzt.

## Entscheidung

NovaOS verwendet **Typed Resources**.

Jede typisierte Ressource kann zusätzlich zu ihrer Identität einen oder mehrere Semantic Types besitzen.

```text
Resource
 ├── Identity
 ├── Semantic Type
 └── Capabilities
```

Beispiele:

```text
File → Document.Markdown
File → Image.PNG
Device → Camera
Stream → Audio.PCM
Endpoint → Network.HTTP
```

## Ressourcenidentität

Der Semantic Type ist unabhängig von der Identität der Ressource.

```text
Resource ID
    ≠
Semantic Type
```

Dieselbe Ressource behält ihre Identität auch dann, wenn sich Pfad, Speicherort oder Sicht auf sie ändern.

## Typprüfung

Operationen können festlegen, welche Resource Types sie akzeptieren.

```text
Capability:
    image.resize

Input:
    Resource<Image>
```

Eine inkompatible Ressource wird vor der Ausführung zurückgewiesen.

## Mehrere Typen

Eine Ressource kann mehrere kompatible semantische Eigenschaften besitzen.

```text
Resource
 ├── File
 ├── Document
 └── Markdown
```

Dadurch können unterschiedliche Capabilities dieselbe Ressource verwenden, ohne deren konkrete Speicherform kennen zu müssen.

## Capabilities

Typed Resources und Capabilities werden getrennt behandelt.

```text
Resource Type
    ↓
Compatible Capabilities
```

Der Typ beschreibt, was eine Ressource ist.

Capabilities beschreiben, was mit ihr getan werden kann.

## Normative Anforderungen

1. NovaOS MUSS Ressourcen semantisch typisieren können.
2. Resource Identity und Semantic Type MÜSSEN getrennt behandelt werden.
3. Ressourcen MÜSSEN mehrere kompatible Semantic Types besitzen können.
4. Capabilities MÜSSEN Resource Types in ihren Contracts verwenden können.
5. Inkompatible Ressourcen MÜSSEN an typisierten Schnittstellen zurückgewiesen werden.
6. Pfad oder Speicherort DÜRFEN nicht die semantische Identität einer Ressource bestimmen.
7. Resource Types MÜSSEN erweiterbar sein.

## Konsequenzen

### Positive Konsequenzen

- Ressourcen werden nach Bedeutung statt nur nach Speicherform behandelt.
- Capabilities können automatisch passende Ressourcen erkennen.
- Daten und Geräte lassen sich über ein gemeinsames semantisches Modell verwenden.
- Pfad und Bedeutung bleiben voneinander getrennt.

### Negative Konsequenzen

- Ressourcen benötigen zusätzliche Typinformationen.
- Typbeziehungen müssen systemweit konsistent definiert werden.

## Abhängigkeiten

- `ADR-SEMANTIC-0001_Semantic_Types_zusätzlich_zu_primitiven_Typen`
- `ADR-SEMANTIC-0002_Strong_Semantic_Typing`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-FILESYSTEM-0003_Pfad_unabhaengige_Objektidentitaet`

## Zugehörige NPSPECs

- `NPSPEC-SEMANTIC-TYPED-RESOURCE-0001`
- `NPSPEC-SEMANTIC-RESOURCE-TYPE-0001`
- `NPSPEC-SEMANTIC-RESOURCE-COMPATIBILITY-0001`

## Ergebnis

NovaOS behandelt Ressourcen als semantisch typisierte Systemobjekte:

```text
Resource Identity
       +
Semantic Type
       ↓
Typed Resource
       ↓
Compatible Capability
```

Der zentrale Grundsatz lautet:

```text
Eine Ressource wird nicht nur
durch ihren Speicherort beschrieben,
sondern durch das,
was sie semantisch ist.
```