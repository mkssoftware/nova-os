# ADR-OBJECT-0003 – Object Versioning

## Status

Angenommen

## Kategorie

Object Architecture / Versioning

## Kontext

NovaOS-Objekte können sich im Laufe ihrer Lebensdauer verändern.

Dabei muss zwischen der stabilen Identität eines Objekts und seinem jeweiligen Zustand unterschieden werden.

```text
Object Identity
      ≠
Object Version
```

Ohne explizite Versionierung wären Änderungen, Referenzen, Rollback und parallele Bearbeitung nur schwer eindeutig nachvollziehbar.

## Entscheidung

NovaOS unterstützt **Object Versioning**.

Eine stabile Object ID kann mehrere eindeutig identifizierbare Versionen besitzen.

```text
Object ID
 ├── Version 1
 ├── Version 2
 └── Version 3
```

Die Object Identity bleibt bestehen, während sich der Zustand des Objekts zwischen Versionen ändern kann.

## Version Identity

Jede persistierte Version muss innerhalb des Objekts eindeutig identifizierbar sein.

```text
Object ID + Version ID
        ↓
Exact Object State
```

Damit kann sowohl das logische Objekt als auch ein konkreter Zustand referenziert werden.

## Änderungen

Eine persistierte Änderung kann eine neue Version erzeugen.

```text
Version N
    ↓
Change
    ↓
Version N+1
```

Bestehende Versionen dürfen nicht stillschweigend umgedeutet werden.

## Referenzen

Referenzen können entweder auf:

```text
Object ID
```

oder auf:

```text
Object ID + Version ID
```

zeigen.

Eine reine Object-ID-Referenz folgt dem jeweils gültigen Zustand. Eine versionsgebundene Referenz bezeichnet einen konkreten Zustand.

## Rollback

Eine frühere Version kann als Grundlage für einen neuen aktuellen Zustand verwendet werden.

```text
V1 → V2 → V3
          ↓
     Restore V1
          ↓
         V4
```

Die Historie bleibt dadurch nachvollziehbar und wird nicht rückwirkend überschrieben.

## Parallelität

Object Versioning bildet die Grundlage für parallele Änderungen.

```text
       V1
      /  \
    V2A  V2B
```

Konflikterkennung und Merge-Verhalten werden separat definiert.

## Speicherung

Nicht jede Version muss eine vollständige physische Kopie des Objekts enthalten.

Implementierungen dürfen beispielsweise:

```text
Snapshots
Copy-on-Write
Deltas
Content Addressing
```

verwenden.

Das logische Versionierungsmodell bleibt davon unabhängig.

## Normative Anforderungen

1. NovaOS MUSS mehrere Versionen eines Objekts unterstützen können.
2. Object Identity und Object Version MÜSSEN getrennte Konzepte sein.
3. Persistierte Versionen MÜSSEN eindeutig identifizierbar sein.
4. Referenzen MÜSSEN auf ein Objekt oder eine konkrete Objektversion zeigen können.
5. Frühere Versionen DÜRFEN nicht durch einen Rollback rückwirkend verändert werden.
6. Parallele Versionen MÜSSEN darstellbar sein.
7. Das logische Versionierungsmodell DARF nicht von einer bestimmten Storage-Technik abhängen.

## Konsequenzen

### Positive Konsequenzen

- Objektidentität bleibt trotz Änderungen stabil.
- konkrete historische Zustände können referenziert werden.
- Rollback wird nachvollziehbar.
- parallele Bearbeitung wird möglich.
- Storage kann Versionen effizient speichern.

### Negative Konsequenzen

- Versionshistorien benötigen zusätzlichen Speicher.
- Lifecycle und Garbage Collection alter Versionen müssen geregelt werden.

## Abhängigkeiten

- `ADR-OBJECT-0001_Unified_System_Object_Model`
- `ADR-OBJECT-0002_Stabile_Object_Identity`
- `ADR-ARCH-0009_Daten_und_Objekt_Pipelining`
- `ADR-FILESYSTEM-0003_Pfad_unabhaengige_Objektidentitaet`

## Zugehörige NPSPECs

- `NPSPEC-OBJECT-VERSIONING-0001`
- `NPSPEC-OBJECT-VERSION-ID-0001`
- `NPSPEC-OBJECT-VERSION-REFERENCE-0001`

## Ergebnis

NovaOS trennt dauerhaftes Objekt und konkreten Objektzustand:

```text
Stable Object ID
      ↓
Version History
 ├── V1
 ├── V2
 └── V3
```

Der zentrale Grundsatz lautet:

```text
Die Identität bleibt stabil.

Der Zustand ist versioniert.
```