# ADR-OBJECT-0002 – Stabile Object Identity

## Status

Angenommen

## Kategorie

Object Architecture / Identity

## Kontext

NovaOS-Objekte können verschoben, umbenannt, repliziert oder über unterschiedliche logische Sichten dargestellt werden.

Pfad, Name oder aktueller Speicherort eignen sich deshalb nicht als dauerhafte Identität eines Objekts.

```text
Name      → veränderlich
Path      → veränderlich
Location  → veränderlich
```

Für Relationships, Permissions, Metadaten und systemweite Referenzen wird eine stabile Identität benötigt.

## Entscheidung

Jedes dauerhaft adressierbare NovaOS-Objekt erhält eine **stabile Object ID**.

```text
Object
  ↓
Stable Object ID
```

Die Object ID bleibt über normale Änderungen an Name, Pfad oder Speicherort hinweg erhalten.

## Grundprinzip

```text
Identity ≠ Location
Identity ≠ Name
Identity ≠ Path
```

Die Object ID beschreibt, **welches Objekt** gemeint ist.

Andere Eigenschaften beschreiben lediglich, wo oder wie dieses Objekt aktuell erreichbar ist.

## Lebenszyklus

Die Object ID wird bei der Erzeugung eines neuen Objekts vergeben.

```text
Create Object
     ↓
Assign Object ID
     ↓
Object Lifetime
```

Sie bleibt für die Lebensdauer dieses Objekts stabil.

## Verschieben und Umbenennen

Operationen wie:

```text
Rename
Move
Mount
Change View
```

dürfen die Object ID nicht verändern.

Beispiel:

```text
/Docs/report.md
       ↓
Move
       ↓
/Archive/report.md

Object ID bleibt gleich
```

## Kopieren

Eine echte Kopie erzeugt ein neues Objekt und damit eine neue Object ID.

```text
Object A
   ↓ Copy
Object B

ID(A) ≠ ID(B)
```

Referenzen oder alternative Sichten auf dasselbe Objekt erzeugen dagegen keine neue Identität.

## Referenzen

Systeminterne dauerhafte Referenzen sollen bevorzugt Object IDs verwenden.

```text
Object A
    ↓
Object ID B
    ↓
Object B
```

Dadurch bleiben Beziehungen auch nach Verschieben oder Umbenennen erhalten.

## Handles

Laufzeit-Handles sind keine dauerhaften Object IDs.

```text
Object ID → stabile Identität
Handle    → temporärer Zugriff
```

Ein Objekt kann während seiner Lebensdauer mehrere Handles besitzen.

## Sicherheit

Der Besitz oder die Kenntnis einer Object ID verleiht keine Berechtigung auf das Objekt.

```text
Know Object ID
      ≠
Authorized Access
```

Permissions und Capabilities werden weiterhin separat geprüft.

## Normative Anforderungen

1. Dauerhaft adressierbare NovaOS-Objekte MÜSSEN stabile Object IDs besitzen können.
2. Object IDs MÜSSEN unabhängig von Name, Pfad und Speicherort sein.
3. Umbenennen oder Verschieben DARF die Object ID nicht verändern.
4. Eine echte Objektkopie MUSS eine neue Object ID erhalten.
5. Alternative Sichten auf dasselbe Objekt DÜRFEN keine neue Object Identity erzeugen.
6. Persistente Beziehungen SOLLEN Object IDs statt Pfaden verwenden.
7. Handles und Object IDs MÜSSEN getrennte Konzepte bleiben.
8. Eine Object ID DARF keine Zugriffsberechtigung implizieren.

## Konsequenzen

### Positive Konsequenzen

- Referenzen bleiben trotz Verschieben und Umbenennen gültig.
- Semantic Relationships werden stabil.
- Pfad und Identität sind sauber getrennt.
- mehrere logische Sichten auf dasselbe Objekt werden möglich.

### Negative Konsequenzen

- Object IDs müssen dauerhaft verwaltet werden.
- Import, Kopieren und Replikation benötigen klare Identitätsregeln.

## Abhängigkeiten

- `ADR-OBJECT-0001_Unified_System_Object_Model`
- `ADR-FILESYSTEM-0003_Pfad_unabhaengige_Objektidentitaet`
- `ADR-FILESYSTEM-0004_Mehrfache_logische_Sichten_auf_ein_Objekt`
- `ADR-SEMANTIC-0008_Semantic_Relationships`
- `ADR-ARCH-0007_Location_Transparency`

## Zugehörige NPSPECs

- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-OBJECT-ID-LIFECYCLE-0001`
- `NPSPEC-OBJECT-REFERENCE-0001`

## Ergebnis

NovaOS trennt die Identität eines Objekts von dessen aktueller Darstellung und Position:

```text
Stable Object ID
       ↓
     Object
   ┌───┼────┐
   ↓   ↓    ↓
 Name Path Location
```

Der zentrale Grundsatz lautet:

```text
Ein Objekt bleibt dasselbe Objekt,
auch wenn sich sein Name,
sein Pfad oder sein Speicherort ändert.
```