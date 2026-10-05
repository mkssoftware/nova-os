# NPSPEC-REGISTRY-QUERY-0001 – Nova Registry Query

## Status

Angenommen

## Kategorie

Registry / Query

## Zweck

NovaOS definiert ein einheitliches Abfragemodell für System-Registries.

Clients können Registry-Einträge anhand stabiler Identitäten, Typen, Eigenschaften und Anforderungen suchen, ohne die interne Speicherung der jeweiligen Registry kennen zu müssen.

## Grundprinzipien

```text
Query ≠ Authority
Result ≠ Permission
Discovery ≠ Access
Registry Query ≠ Filesystem Search
Query Interface ≠ Registry Storage
```

## Query-Modell

Eine Registry-Abfrage kann enthalten:

```text
RegistryQuery
├── RegistryType
├── Conditions
├── Requirements
├── Sort
├── Limit
└── Cursor
```

Optional:

```text
Scope
VersionConstraint
Compatibility
StateFilter
Projection
```

## Unterstützte Registries

Das gemeinsame Query-Modell kann insbesondere verwendet werden für:

```text
Capability Registry
Type Registry
Service Registry
Algorithm Registry
Device Registry
Codec Registry
System Registry
```

Registry-spezifische Felder dürfen das gemeinsame Modell erweitern.

## Bedingungen

Abfragen unterstützen logische Verknüpfungen:

```text
AND
OR
NOT
```

Beispiele:

```text
CapabilityID = de.nova.image.filter.gaussian

DeviceType = GPU
AND State = Ready

Codec supports Video
AND HardwareAcceleration = true
```

## Auflösung

```text
Query
  ↓
Registry
  ↓
Filter
  ↓
Compatibility Check
  ↓
Policy Visibility
  ↓
Result Set
```

Die Registry liefert nur Einträge, die im jeweiligen Abfragekontext sichtbar sein dürfen.

## Ergebnisse

Ergebnisse referenzieren stabile Registry-Identitäten und dürfen zusätzliche Metadaten enthalten.

```text
RegistryQueryResult
├── EntryID
├── RegistryType
├── Version
├── State
└── Metadata
```

Ein Ergebnis stellt keine Capability und keine Zugriffsberechtigung dar.

## Pagination und Streaming

Große Ergebnismengen müssen begrenzbar sein.

```text
Limit
Cursor
Streaming
```

Ein Cursor ist an den jeweiligen Query-Kontext gebunden und darf nicht als dauerhafte Objektidentität verwendet werden.

## Konsistenz

Registry-Abfragen sollen einen definierten Registry-Zustand oder eine Generation referenzieren können.

Ändert sich die Registry während einer Abfrage, muss das Ergebnis entweder konsistent fortgeführt oder kontrolliert als veraltet erkannt werden können.

## Sicherheit

Abfragen dürfen keine geschützten Registry-Informationen offenlegen, für die der aufrufende Kontext keine Discovery-Berechtigung besitzt.

```text
Can Discover ≠ Can Use
Can Inspect ≠ Can Execute
```

## Normative Anforderungen

1. NovaOS MUSS ein einheitliches Registry-Query-Modell bereitstellen.
2. Registry-spezifische Erweiterungen MÜSSEN möglich sein.
3. Queries MÜSSEN nach stabilen Identitäten und Eigenschaften filtern können.
4. `AND`, `OR` und `NOT` MÜSSEN unterstützt werden können.
5. Versions- und Kompatibilitätsanforderungen MÜSSEN ausdrückbar sein.
6. Große Ergebnismengen MÜSSEN begrenzbar oder paginierbar sein.
7. Query-Ergebnisse DÜRFEN keine Authority erzeugen.
8. Discovery-Berechtigungen MÜSSEN bei geschützten Registry-Daten berücksichtigt werden.
9. Registry-Änderungen während einer Abfrage MÜSSEN kontrolliert behandelbar sein.
10. Query-Ausführung MUSS Ressourcenlimits unterliegen können.
11. Registry Query DARF nicht von der internen Registry-Speicherstruktur abhängen.
12. Query, Ergebnisgeneration und Auswahlkriterien MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-REGISTRY-0001`
- `NPSPEC-REGISTRY-CAPABILITY-0001`
- `NPSPEC-REGISTRY-TYPE-0001`
- `NPSPEC-REGISTRY-SERVICE-0001`
- `NPSPEC-REGISTRY-ALGORITHM-0001`
- `NPSPEC-REGISTRY-DEVICE-0001`
- `NPSPEC-REGISTRY-CODEC-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt ein gemeinsames Abfragemodell für seine Registries. Komponenten können verfügbare Typen, Services, Capabilities, Algorithmen, Geräte und Codecs einheitlich entdecken und filtern, ohne dadurch Authority zu erhalten oder von der internen Registry-Implementierung abhängig zu sein.