# NPSPEC-STORAGE-CONTENTADDRESS-0001 – Nova Content-Addressed Storage

## Status

Entwurf

## Kategorie

Storage / Content Addressing / Integrity / Deduplication

## Zweck

NovaOS definiert Content-Addressed Storage als Mechanismus, bei dem Daten anhand ihres Inhalts statt ausschließlich anhand ihres Speicherorts identifiziert werden können.

```text
Content
   ↓
Hash
   ↓
ContentID
   ↓
Stored Data
```

Dies ermöglicht Integritätsprüfung, Deduplication und effiziente Wiederverwendung identischer Inhalte.

## Grundprinzip

```text
ObjectID ≠ ContentID
ContentID ≠ Path
ContentID ≠ Physical Location
```

Die `ObjectID` identifiziert ein logisches Objekt.

Die `ContentID` identifiziert einen konkreten Dateninhalt.

```text
ObjectID
   ↓
Version
   ↓
ContentID
   ↓
Payload
```

Ändert sich der Inhalt, entsteht eine neue `ContentID`, während die `ObjectID` erhalten bleiben kann.

## ContentID

Eine `ContentID` wird aus dem Inhalt mittels kryptographischer Hashfunktion erzeugt.

```text
Payload
   ↓
Cryptographic Hash
   ↓
ContentID
```

Die verwendeten Hashverfahren müssen versioniert und austauschbar sein.

Eine ContentID soll deshalb den verwendeten Algorithmus eindeutig erkennen lassen.

## Speicherung

Physische Daten dürfen anhand ihrer `ContentID` gespeichert oder referenziert werden.

```text
ContentID A → Payload A
ContentID B → Payload B
ContentID C → Payload C
```

Mehrere Objekte dürfen denselben Inhalt referenzieren.

```text
Object A ─┐
Object B ─┼→ ContentID X → Payload
Object C ─┘
```

## Deduplication

Identische Inhalte müssen nicht mehrfach physisch gespeichert werden.

```text
Payload X
Payload X
Payload X
    ↓
ContentID X
    ↓
Single Stored Payload
```

Deduplication darf jedoch die logische Identität der referenzierenden Objekte nicht verändern.

## Versionierung

Content Addressing eignet sich zur Speicherung von Objektversionen.

```text
ObjectID
├── Version 1 → ContentID A
├── Version 2 → ContentID B
└── Version 3 → ContentID C
```

Identische Versionen dürfen dieselbe `ContentID` verwenden.

## Integrität

Beim Lesen kann der gespeicherte Inhalt erneut gehasht werden.

```text
Stored Payload
      ↓
Recalculate Hash
      ↓
Expected ContentID
      ↓
Match?
```

Eine Abweichung muss als Integritätsfehler behandelt werden.

## Metadaten

Metadaten und Payload müssen logisch getrennt adressierbar bleiben.

Eine Änderung von:

```text
Name
Path
Permissions
Relationships
Display Metadata
```

darf nicht zwangsläufig die `ContentID` des unveränderten Payloads verändern.

## Garbage Collection

Content darf physisch entfernt werden, wenn keine gültige Referenz mehr darauf besteht.

```text
ContentID
   ↓
References = 0
   ↓
Reclaimable
```

Snapshots, Versionen und Recovery-Zustände müssen dabei berücksichtigt werden.

## Sicherheit

Die Kenntnis einer `ContentID` erzeugt keine Zugriffsberechtigung.

```text
Know ContentID
      ≠
Authorized Access
```

Der Zugriff muss weiterhin über Object-, Capability- und Security-Regeln kontrolliert werden.

## Hash-Kollisionen

Hashgleichheit darf nicht automatisch als vollständiger Sicherheitsbeweis behandelt werden.

Für sicherheitskritische Operationen darf NovaOS zusätzliche Verifikation durchführen.

Hashverfahren müssen migrierbar sein, falls ein Algorithmus zukünftig nicht mehr als ausreichend sicher gilt.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
ContentID
Hash Algorithm
Content Size
Reference Count
Integrity State
Deduplication State
```

## Normative Anforderungen

1. NovaOS MUSS ContentIDs unabhängig von Pfad und physischem Speicherort darstellen können.
2. `ObjectID` und `ContentID` MÜSSEN logisch getrennt sein.
3. Eine Änderung des Payloads MUSS eine neue ContentID erzeugen.
4. Identische Inhalte DÜRFEN physisch dedupliziert werden.
5. Deduplication DARF die logische Objektidentität NICHT verändern.
6. ContentIDs MÜSSEN auf kryptographischen Hashverfahren basieren.
7. Hashverfahren MÜSSEN versionierbar und austauschbar sein.
8. NovaOS SOLL ContentIDs zur Integritätsprüfung verwenden können.
9. Die Kenntnis einer ContentID DARF KEINE Zugriffsberechtigung erzeugen.
10. Nicht mehr referenzierter Content MUSS kontrolliert reclaimbar sein.
11. Versionen und Snapshots MÜSSEN vor Content-Reclamation berücksichtigt werden.
12. ContentID, Integritätsstatus und Referenzen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-VFS-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-STORAGE-0003`

## Ergebnis

```text
Storage Object
      ↓
   Version
      ↓
   ContentID
      ↓
Content-addressed Payload
      ↓
Integrity + Deduplication
```

NovaOS erhält damit eine inhaltsbasierte Storage-Schicht, die stabile Objektidentitäten mit Integritätsprüfung, Versionierung und speichereffizienter Deduplication verbindet.