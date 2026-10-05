# NPSPEC-TEMP-CACHE-0001 – Nova Temporary Cache

## Status

Angenommen

## Kategorie

Temporary Resources / Cache

## Zweck

NovaOS definiert Cache als rekonstruierbare temporäre Ressource zur Beschleunigung von Zugriffen und Berechnungen.

Der Verlust eines Caches darf nicht zum Verlust der maßgeblichen Daten führen.

## Grundprinzipien

```text
Cache ≠ Persistent Data
Cache ≠ Authoritative Data
Cache ≠ User Data
Cache Entry ≠ Source Object
Cache Loss ≠ Data Loss
```

## Cache-Modell

Ein Cache-Eintrag besitzt mindestens:

```text
CacheEntry
├── ResourceID
├── OwnerID
├── SourceID
├── Scope
├── State
└── Size
```

Optional:

```text
TTL
Version
ContentID
LastAccess
Priority
ValidationPolicy
```

`SourceID` verweist auf die Ressource oder Berechnung, aus der der Cache rekonstruierbar ist.

## Scopes

Caches können unterschiedlichen Kontexten zugeordnet sein:

```text
Process
Program
Solution
Workspace
User
System
```

Caches verschiedener Sicherheitskontexte müssen isolierbar bleiben.

## Zugriff

```text
Request
   ↓
Cache Lookup
   ├── Valid Hit → Cached Result
   └── Miss / Invalid
             ↓
        Source Access
             ↓
        Rebuild Cache
```

Ein Cache-Treffer darf die normalen Zugriffsrechte auf die zugrunde liegende Ressource nicht umgehen.

## Gültigkeit

Cache-Einträge können durch:

```text
TTL
Source Version
ContentID
Explicit Invalidation
Policy Change
Permission Change
```

ungültig werden.

Veraltete Daten müssen als solche erkennbar oder gemäß Policy verworfen werden.

## Reclaim

Cache-Daten gelten grundsätzlich als reclaimable.

Unter Ressourcendruck kann NovaOS Cache-Einträge anhand von Größe, Alter, Nutzung, Priorität und Rekonstruktionskosten freigeben.

```text
Resource Pressure
      ↓
Select Cache Entries
      ↓
Reclaim
```

Anwendungen dürfen nicht davon ausgehen, dass ein Cache-Eintrag dauerhaft vorhanden bleibt.

## Sicherheit

Cache-Daten behalten die Schutzanforderungen ihrer Quelldaten.

Ein Cache darf keine zusätzliche Authority erzeugen oder Informationen zwischen nicht autorisierten Sicherheitskontexten offenlegen.

Sensible Cache-Inhalte müssen sicher bereinigbar sein.

## Normative Anforderungen

1. Cache-Daten MÜSSEN aus einer maßgeblichen Quelle rekonstruierbar sein.
2. Cache-Verlust DARF keinen Verlust maßgeblicher Daten verursachen.
3. Cache-Einträge MÜSSEN einem Owner und Scope zuordenbar sein.
4. Cache-Einträge MÜSSEN invalidierbar sein.
5. Veraltete Einträge DÜRFEN nicht unkontrolliert als aktuell behandelt werden.
6. Cache-Daten MÜSSEN unter Ressourcendruck reclaimable sein können.
7. Programme DÜRFEN die dauerhafte Existenz eines Cache-Eintrags nicht voraussetzen.
8. Cache-Zugriffe DÜRFEN bestehende Berechtigungen nicht umgehen.
9. Sicherheitskontexte MÜSSEN bei gemeinsam genutzten Caches berücksichtigt werden.
10. Sensible Cache-Daten MÜSSEN sicher bereinigbar sein.
11. Cache MUSS in die NovaOS Resource Economy integrierbar sein.
12. Cache DARF keine zusätzliche Authority erzeugen.

## Abhängigkeiten

- `NPSPEC-TEMP-MODEL-0001`
- `NPSPEC-TEMP-TTL-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`

## Ergebnis

NovaOS behandelt Cache als rekonstruierbare und jederzeit freigebbare Optimierung. Cache-Daten bleiben von maßgeblichen Daten getrennt, werden kontrolliert invalidiert und können unter Ressourcendruck ohne Verlust der eigentlichen Informationen zurückgewonnen werden.