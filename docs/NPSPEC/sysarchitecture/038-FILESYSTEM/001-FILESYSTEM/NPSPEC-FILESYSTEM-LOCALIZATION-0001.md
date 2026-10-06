# NPSPEC-FILESYSTEM-LOCALIZATION-0001 – Nova Filesystem Localization

## Status

Angenommen

## Kategorie

Filesystem / Namespace / Localization

## Zweck

NovaOS trennt die stabile interne Identität von Filesystem-Bereichen von deren sichtbarer sprachabhängiger Bezeichnung.

Systembereiche können dadurch in der Sprache des Benutzers angezeigt werden, ohne dass Programme, Scripts, APIs oder interne Referenzen von übersetzten Pfadnamen abhängig werden.

```text
Stable Namespace Identity
        ↓
Localization Layer
        ↓
Localized Display Name
```

## Grundprinzipien

```text
Display Name ≠ Namespace Identity
Translation ≠ Rename
Locale Change ≠ Path Migration
Localized Name ≠ ObjectID
Localized Name ≠ Capability
Internal Name ≠ Required User Display Name
```

## Localization Model

```text
LocalizedNamespaceEntry
├── TargetID
├── LocalizationKey
├── InternalName
└── DisplayName
```

Beispiel:

```text
LocalizationKey: filesystem.user
InternalName:     User

de-DE → Benutzer
en-US → User
fr-FR → Utilisateur
```

Die Übersetzung verändert weder `TargetID` noch die zugrunde liegende Ressource.

## Stabile interne Namen

Systemdefinierte Namespace-Bereiche müssen intern sprachneutral adressierbar bleiben.

Beispiel:

```text
/
├── System/
├── User/
├── Apps/
├── Solutions/
├── Boot/
└── Volumes/
```

Die Benutzeroberfläche kann daraus beispielsweise darstellen:

```text
/
├── System/
├── Benutzer/
├── Apps/
├── Lösungen/
├── Boot/
└── Datenträger/
```

Diese Darstellung ist eine lokalisierte Sicht und keine physische Umbenennung.

## Namespace Resolution

Die interne Auflösung erfolgt über stabile Namespace-Identitäten.

```text
UI Display Name
      ↓
Namespace Entry
      ↓
TargetID
      ↓
ObjectID / VolumeID
```

Programme sollen nicht darauf angewiesen sein, dass ein Systembereich in einer bestimmten Sprache angezeigt wird.

## Sprachwechsel

Ein Wechsel der Systemsprache darf keine Migration des Filesystems erfordern.

```text
Deutsch

Benutzer
   ↓
TargetID 42

        ⇅ Locale Change

User
   ↓
TargetID 42
```

Es gilt:

```text
Locale Change
≠
Rename
≠
Move
≠
New Object
```

Bestehende ObjectIDs, Handles, Relations und Projections bleiben erhalten.

## Benutzerdefinierte Namen

Vom Benutzer selbst vergebene Datei- und Verzeichnisnamen werden nicht automatisch übersetzt.

```text
User-created Name
≠
Localized System Label
```

NovaOS muss zwischen systemdefinierten lokalisierbaren Bezeichnungen und tatsächlichen benutzerdefinierten Namen unterscheiden.

## Volumes

Volume-Namen dürfen benutzerdefiniert sein und werden nicht automatisch übersetzt.

Systemrollen oder UI-Beschreibungen eines Volumes können dagegen lokalisiert dargestellt werden.

```text
VolumeID: 73...
Name: Arbeit
Role: SystemData
```

Darstellung:

```text
de-DE → Systemdaten
en-US → System Data
```

Die `VolumeID` und der benutzerdefinierte Name bleiben unverändert.

## Projections

Lokalisierte Bezeichnungen können auf Projections angewendet werden.

```text
ProjectionID
     ↓
LocalizationKey
     ↓
Localized View
```

Mehrere sprachabhängige Darstellungen referenzieren weiterhin dieselbe Projection beziehungsweise dasselbe Target.

```text
Benutzer ─┐
User ─────┼──→ Same TargetID
Utilisateur ─┘
```

## Semantische Integration

Localization darf semantische Identitäten nicht verändern.

```text
SemanticTypeID
RelationTypeID
ObjectID
VolumeID
ProjectionID
```

bleiben sprachneutral.

Nur die Darstellung kann übersetzt werden.

Beispiel:

```text
RelationTypeID: nova.relation.derived-from

de-DE → Abgeleitet von
en-US → Derived from
```

## Scripts und APIs

Scripts, Programme und APIs dürfen nicht auf übersetzte Systembezeichnungen angewiesen sein.

Sie sollen bevorzugt verwenden:

```text
ObjectID
VolumeID
NamespaceID
ProjectionID
SemanticTypeID
Stable System Alias
```

Nicht:

```text
"Benutzer"
"User"
"Utilisateur"
```

als dauerhafte Identität.

## Konflikte

Eine Übersetzung kann mit einem vorhandenen benutzerdefinierten Namen kollidieren.

NovaOS muss solche Fälle deterministisch behandeln.

```text
Localized System Name
        +
User-created Name
        ↓
Conflict Resolution
```

Die interne Identität beider Einträge bleibt eindeutig.

Die UI darf zusätzliche Kennzeichnung oder disambiguierte Darstellung verwenden.

## Fallback

Fehlt eine Übersetzung:

```text
Requested Locale
      ↓
Translation Available?
   ↙             ↘
 Yes             No
  ↓               ↓
Localized      Fallback
```

Fallback kann auf eine definierte Basissprache oder einen stabilen Systemnamen erfolgen.

Fehlende Übersetzungen dürfen die Ressource nicht unzugänglich machen.

## Recovery

Recovery- und Diagnoseumgebungen müssen Ressourcen unabhängig von der installierten UI-Sprache eindeutig adressieren können.

```text
Localized UI
     ↓
Stable Namespace Identity
     ↓
Recovery
```

Dadurch kann beispielsweise dieselbe Systemressource in deutscher und englischer Recovery-Umgebung sicher erkannt werden.

## Capability Integration

Lokalisierung verändert keine Berechtigung.

```text
Localized Name
      ↓
TargetID
      ↓
Capability Check
```

Es gilt:

```text
Translated Name
≠
Authority
```

Ein Sprachwechsel darf weder Berechtigungen erteilen noch widerrufen.

## Cache

Lokalisierte Anzeigen dürfen gecacht werden.

Der Cache muss bei Änderungen an:

```text
Locale
Language Pack
Localization Data
Display Policy
```

invalidierbar sein.

Die zugrunde liegende Namespace-Struktur bleibt davon unabhängig.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
TargetID
Internal Name
LocalizationKey
Current Display Name
Current Locale
Fallback State
```

Die lokalisierte Darstellung darf nicht mit der stabilen Identität verwechselt werden.

## Normative Anforderungen

1. NovaOS MUSS Namespace-Identität und lokalisierte Darstellung trennen.
2. Systemdefinierte Namespace-Bereiche MÜSSEN sprachneutral identifizierbar sein.
3. Lokalisierte Namen DÜRFEN NICHT als dauerhafte Objektidentität verwendet werden.
4. Ein Sprachwechsel DARF ObjectIDs NICHT verändern.
5. Ein Sprachwechsel DARF VolumeIDs NICHT verändern.
6. Ein Sprachwechsel DARF ProjectionIDs NICHT verändern.
7. Ein Sprachwechsel DARF KEINE physische Rename- oder Move-Operation erfordern.
8. Programme und APIs SOLLEN stabile IDs oder sprachneutrale Systemidentitäten verwenden.
9. Scripts DÜRFEN NICHT zwingend von lokalisierten Systemnamen abhängig sein.
10. Benutzerdefinierte Datei- und Verzeichnisnamen DÜRFEN NICHT automatisch übersetzt werden.
11. Benutzerdefinierte Volume-Namen DÜRFEN NICHT automatisch übersetzt werden.
12. Systemrollen und Systembeschreibungen DÜRFEN lokalisiert dargestellt werden.
13. SemanticTypeIDs MÜSSEN sprachneutral bleiben.
14. RelationTypeIDs MÜSSEN sprachneutral bleiben.
15. Projections MÜSSEN unabhängig von ihrer lokalisierten Darstellung identifizierbar bleiben.
16. Mehrere lokalisierte Darstellungen DÜRFEN dieselbe Ressource referenzieren.
17. Namenskonflikte zwischen Localization und Benutzerobjekten MÜSSEN deterministisch behandelt werden.
18. Fehlende Übersetzungen MÜSSEN einen definierten Fallback besitzen.
19. Fehlende Übersetzungen DÜRFEN Ressourcen NICHT unzugänglich machen.
20. Recovery MUSS Ressourcen unabhängig von der UI-Sprache adressieren können.
21. Localization DARF KEINE Capability oder zusätzliche Authority erzeugen.
22. Sprachwechsel DARF bestehende autorisierte Handles NICHT allein aufgrund der Sprache ungültig machen.
23. Localization-Caches MÜSSEN invalidierbar sein.
24. Localization-State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-FILESYSTEM-SEMANTIC-0001`
- `NPSPEC-FILESYSTEM-RELATION-0001`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`
- `NPSPEC-STORAGE-NAMESPACE-0001`
- `NPSPEC-FSSTORAGE-VOLUME-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`

## Ergebnis

```text
Stable Filesystem Identity
          ↓
     Namespace Entry
          ↓
    Localization Key
          ↓
   ┌──────┼────────┐
   ↓      ↓        ↓
 de-DE   en-US    fr-FR
   ↓      ↓        ↓
Benutzer User  Utilisateur
   └──────┼────────┘
          ↓
      Same TargetID
```

NovaOS erhält damit ein sprachunabhängiges Filesystem-Modell, bei dem interne Identitäten stabil bleiben, während Systembereiche für den Benutzer in seiner jeweiligen Sprache dargestellt werden können. Ein Sprachwechsel verändert ausschließlich die Darstellung und niemals Objektidentität, Namespace-Struktur, Berechtigungen oder physische Speicherung.