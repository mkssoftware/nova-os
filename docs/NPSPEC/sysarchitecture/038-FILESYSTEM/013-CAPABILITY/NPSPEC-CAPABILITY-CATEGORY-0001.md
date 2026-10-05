# NPSPEC-CAPABILITY-CATEGORY-0001 – Nova Capability Category

## Status

Angenommen

## Kategorie

Capability / Category

## Zweck

NovaOS definiert Kategorien als organisatorische und darstellungsbezogene Gruppierung von Capabilities.

Kategorien erleichtern Discovery, Navigation, Dokumentation und UI-Darstellung, sind jedoch ausdrücklich kein Bestandteil der Capability-Identität.

## Grundprinzipien

```text
Category ≠ CapabilityID
Category ≠ Namespace
Category ≠ Authority
Category ≠ Permission
Category Path ≠ Identity
Category Membership ≠ Capability Possession
```

## Modell

Eine Kategorie kann beschrieben werden durch:

```text
CapabilityCategory
├── CategoryID
├── ParentCategoryID
├── DisplayName
├── Description
├── Icon
├── Order
└── State
```

Capabilities können Kategorien über ihre stabile `CapabilityID` zugeordnet werden.

```text
Category
├── CapabilityID A
├── CapabilityID B
└── CapabilityID C
```

## Trennung vom Capability Namespace

Capability Namespace und Kategorie erfüllen unterschiedliche Aufgaben.

```text
CapabilityID:
de.nova.image.filter.gaussian

Namespace:
image.filter

mögliche Kategorie:
Bildbearbeitung
```

Die Kategorie darf geändert werden, ohne die `CapabilityID` zu verändern.

## Mehrfachzuordnung

Eine Capability darf mehreren Kategorien gleichzeitig zugeordnet sein.

Beispiel:

```text
de.nova.image.decode
├── Medien
├── Bilder
└── Datenverarbeitung
```

Dadurch entstehen keine Kopien oder zusätzlichen Capability-Identitäten.

## Hierarchie

Kategorien dürfen hierarchisch organisiert werden:

```text
Medien
├── Bilder
│   ├── Bearbeitung
│   └── Konvertierung
├── Audio
└── Video
```

Diese Hierarchie dient ausschließlich Organisation und Darstellung.

## Lokalisierung

Kategorie-Anzeigenamen dürfen lokalisiert werden.

```text
CategoryID: media.image

Deutsch:
Bilder

Englisch:
Images
```

Die technische `CategoryID` bleibt unabhängig von der UI-Sprache stabil.

## Registry

Die Capability Registry darf Kategorien verwenden für:

```text
Browsing
Filtering
Grouping
Search
Documentation
UI Presentation
```

Die Registry-Kategorie darf jedoch niemals zur Rekonstruktion oder Bestimmung einer `CapabilityID` verwendet werden.

## Änderungen

Capabilities dürfen zwischen Kategorien verschoben oder zusätzlichen Kategorien zugeordnet werden:

```text
Old Category
     ↓
Reclassification
     ↓
New Category
```

Dabei bleiben unverändert:

```text
CapabilityID
Version
Provider
Authority
Permissions
```

## Sicherheit

Kategorien besitzen keine Sicherheitssemantik.

Insbesondere erzeugt die Berechtigung für eine Capability keine Berechtigung für andere Capabilities derselben Kategorie.

```text
Category Membership ≠ Authority Inheritance
```

## Normative Anforderungen

1. NovaOS MUSS Capability-Kategorien unabhängig von Capability-IDs behandeln.
2. Kategorien DÜRFEN nicht Bestandteil der Capability-Identität sein.
3. Kategoriepfade DÜRFEN die `CapabilityID` nicht bestimmen.
4. Capability Namespace und Capability Category MÜSSEN getrennte Konzepte bleiben.
5. Eine Capability DARF mehreren Kategorien zugeordnet sein.
6. Kategorien DÜRFEN hierarchisch organisiert werden.
7. Kategorie-Anzeigenamen DÜRFEN lokalisiert werden.
8. Kategorieänderungen DÜRFEN die `CapabilityID` nicht verändern.
9. Kategoriezugehörigkeit DARF keine Authority erzeugen.
10. Kategoriehierarchie DARF keine Berechtigungsvererbung erzeugen.
11. Die Capability Registry MUSS Kategorien für Discovery und Darstellung verwenden können.
12. Kategorie, Zuordnungen und Capability-Identitäten MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-NAMING-0001`
- `NPSPEC-CAPABILITY-NAMESPACE-0001`
- `NPSPEC-REGISTRY-CAPABILITY-0001`
- `NPSPEC-REGISTRY-QUERY-0001`

## Ergebnis

NovaOS verwendet Capability-Kategorien ausschließlich zur flexiblen Organisation und Darstellung von Fähigkeiten. Capabilities können beliebig kategorisiert, umsortiert und mehrfach eingeordnet werden, ohne ihre stabile Identität, ihren Namespace oder ihre Authority zu verändern.