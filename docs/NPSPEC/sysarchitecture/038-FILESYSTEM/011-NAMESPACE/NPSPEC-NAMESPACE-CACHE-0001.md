# NPSPEC-NAMESPACE-CACHE-0001 – Nova Namespace Cache

## Status

Angenommen

## Kategorie

Namespace / Cache

## Zweck

NovaOS definiert einen Cache für bereits ausgeführte Namespace-Auflösungen.

Der Namespace Cache beschleunigt wiederholte Pfadauflösungen, ohne selbst Quelle der Wahrheit für Namespace-Struktur, Objektidentität oder Berechtigungen zu sein.

## Grundprinzipien

```text
Cache ≠ Source of Truth
Cache Entry ≠ Authority
Cached Path ≠ Object Identity
Cache Hit ≠ Permission Grant
Stale Entry ≠ Valid Resolution
Namespace Version Determines Validity
```

## Cache-Modell

Ein Cache-Eintrag kann enthalten:

```text
NamespaceCacheEntry
├── NamespaceID
├── NamespaceVersion
├── ContextID
├── Path
├── ObjectID / ResourceID
├── ResolutionMetadata
└── State
```

Optional:

```text
OverlayVersion
ProjectionVersion
MountVersion
ProviderID
Expiration
```

## Auflösung

```text
Path
 ↓
Namespace Cache
 ↓
Cache Hit?
 ├── Yes → Validate Entry
 │           ↓
 │        ObjectID / ResourceID
 │
 └── No  → Namespace Resolution
             ↓
          Cache Result
```

Ein Cache-Treffer darf nur verwendet werden, wenn der Eintrag noch zum aktuellen Namespace-Zustand passt.

## Kontextabhängigkeit

Namespace-Auflösungen sind kontextabhängig.

Daher müssen Cache-Einträge mindestens den relevanten Namespace-Kontext berücksichtigen:

```text
System
User
Process
Application
Solution
Workspace
Recovery
```

Ein Ergebnis aus einem Kontext darf nicht ungeprüft in einem anderen Kontext verwendet werden.

## Invalidierung

Cache-Einträge müssen invalidierbar sein bei Änderungen an:

```text
Namespace Version
Mounts
Projections
Overlays
Namespace Entries
Virtual Providers
Object Removal
Relevant Policy
```

Invalidierung darf gezielt oder generationsbasiert erfolgen.

## Negative Ergebnisse

Auch fehlgeschlagene Auflösungen dürfen kurzfristig gecacht werden:

```text
NotFound
Unavailable
ProviderUnavailable
```

Negative Cache-Einträge müssen besonders strikt invalidiert werden, damit neu verfügbare Ressourcen erkannt werden.

## Objektidentität

Ein Cache-Eintrag speichert das Ergebnis einer Auflösung, erzeugt jedoch keine neue Identität.

```text
Path
 ↓
Cached Resolution
 ↓
ObjectID
```

Die `ObjectID` bleibt unabhängig vom Cache bestehen.

## Sicherheit

Der Cache darf keine Capability-Tokens oder dauerhafte Authority speichern.

Nach der Auflösung gelten weiterhin die aktuellen Sicherheitsprüfungen:

```text
Cached ObjectID
      ↓
Capability / Permission Check
      ↓
Authorized Handle
```

Eine frühere erfolgreiche Autorisierung darf nicht allein aufgrund eines Cache-Eintrags wiederverwendet werden.

## Virtuelle Ressourcen

Bei virtuellen Namespace-Einträgen muss berücksichtigt werden, ob der zuständige Provider und die referenzierte Ressource weiterhin gültig sind.

Ein Providerwechsel oder Provider-Ausfall kann eine Invalidierung erforderlich machen.

## Ressourcen

Der Namespace Cache unterliegt System Resource Policy.

Unter Speicherdruck dürfen Einträge verworfen werden, da sie vollständig aus autoritativen Namespace-Daten rekonstruierbar sein müssen.

## Normative Anforderungen

1. NovaOS SOLL Namespace-Auflösungen cachen können.
2. Der Namespace Cache DARF nicht als autoritative Namespace-Quelle dienen.
3. Cache-Einträge MÜSSEN an einen definierten Namespace-Kontext gebunden sein.
4. Namespace-Versionen MÜSSEN zur Gültigkeitsprüfung verwendet werden können.
5. Änderungen an relevanten Mounts, Projektionen und Overlays MÜSSEN Cache-Einträge invalidieren können.
6. Cache-Treffer DÜRFEN keine Authority erzeugen.
7. Capability- und Permission-Prüfungen MÜSSEN unabhängig vom Cache bestehen bleiben.
8. Der Cache DARF keine aktiven Capability-Tokens speichern.
9. Negative Resolution-Ergebnisse DÜRFEN gecacht werden.
10. Veraltete Cache-Einträge DÜRFEN nicht als gültige Resolution verwendet werden.
11. Cache-Einträge MÜSSEN ohne Verlust autoritativer Informationen verwerfbar sein.
12. Cache-Zustand, Treffer, Misses und Invalidierungsgrund MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NAMESPACE-RESOLUTION-0001`
- `NPSPEC-NAMESPACE-PERMISSION-0001`
- `NPSPEC-NAMESPACE-OVERLAY-0002`
- `NPSPEC-NAMESPACE-VIRTUAL-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-SYSTEM-RESOURCES-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann Namespace-Auflösungen effizient zwischenspeichern, ohne Pfade, Cache-Einträge oder frühere Auflösungen mit Objektidentität oder Authority gleichzusetzen. Versions- und kontextabhängige Invalidierung stellt sicher, dass Änderungen am Namespace korrekt berücksichtigt werden.