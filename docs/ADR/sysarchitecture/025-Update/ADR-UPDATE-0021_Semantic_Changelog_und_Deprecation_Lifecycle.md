# ADR-UPDATE-0021 – Semantic Changelog und Deprecation Lifecycle

## Status

Angenommen

## Kategorie

Update / Compatibility / Lifecycle

## Kontext

Updates verändern nicht nur Versionsnummern, sondern können Fähigkeiten, Schnittstellen und Verhalten eines Systems ändern.

Freitext-Changelogs reichen für NovaOS nicht aus, da Resolver, Update-System und Anwendungen Änderungen maschinell bewerten können müssen.

Gleichzeitig dürfen veraltete APIs und Capabilities nicht ohne definierten Übergang entfernt werden.

## Entscheidung

NovaOS verwendet **Semantic Changelogs** und einen definierten **Deprecation Lifecycle**.

Änderungen werden strukturiert und maschinenlesbar beschrieben.

```text
Update
  ↓
Semantic Changelog
  ↓
Compatibility Evaluation
  ↓
Update Decision
```

## Semantic Changelog

Ein Changelog kann Änderungen klassifizieren als:

```text
Added
Changed
Deprecated
Removed
Security
Breaking
```

Betroffene Komponenten, Capabilities oder Contracts müssen eindeutig referenzierbar sein.

## Breaking Changes

Inkompatible Änderungen müssen explizit als solche gekennzeichnet werden.

```text
Capability V1
     ↓
Breaking Change
     ↓
Capability V2
```

Der Resolver darf eine Breaking Change nicht automatisch als kompatiblen Ersatz behandeln.

## Deprecation Lifecycle

Veraltete Schnittstellen durchlaufen einen definierten Lifecycle.

```text
Supported
   ↓
Deprecated
   ↓
Removal Scheduled
   ↓
Removed
```

`Deprecated` bedeutet weiterhin verfügbar, aber nicht mehr für neue Abhängigkeiten empfohlen.

## Deprecation

Eine Deprecation muss mindestens angeben können:

```text
Affected Capability
Replacement
Deprecation Version
Planned Removal
```

Dadurch können Consumer rechtzeitig migriert werden.

## Removal

Eine deprecated Capability darf erst entfernt werden, wenn die geltenden Lifecycle- und Compatibility-Regeln dies erlauben.

Vor der Entfernung muss NovaOS erkennen können, ob weiterhin relevante Consumer davon abhängen.

```text
Deprecated Capability
        ↓
Dependency Check
        ↓
Still Required?
   ┌────┴────┐
   ↓         ↓
  Yes        No
   ↓         ↓
Keep       Remove
```

## Resolver

Der Capability Resolver soll deprecated Provider oder Schnittstellen für neue Bindungen vermeiden, wenn eine geeignete unterstützte Alternative existiert.

Bestehende kompatible Bindungen dürfen während der Übergangsphase erhalten bleiben.

## Update Pipeline

Vor einem Update kann der Semantic Changelog verwendet werden, um relevante Auswirkungen zu bestimmen.

```text
Current State
     ↓
Semantic Changelog
     ↓
Dependency Evaluation
     ↓
Compatible / Migration Required
```

## Normative Anforderungen

1. NovaOS MUSS maschinenlesbare Semantic Changelogs unterstützen können.
2. Änderungen MÜSSEN semantisch klassifizierbar sein.
3. Breaking Changes MÜSSEN explizit gekennzeichnet werden.
4. Deprecations MÜSSEN betroffene Schnittstellen und mögliche Ersatzmechanismen referenzieren können.
5. Deprecated Komponenten SOLLEN für neue Bindungen vermieden werden.
6. Entfernung MUSS einem definierten Deprecation Lifecycle folgen.
7. Vor einer Entfernung MÜSSEN relevante Abhängigkeiten geprüft werden.
8. Capability Resolver und Update Pipeline MÜSSEN Semantic Changelog und Deprecation State berücksichtigen können.

## Konsequenzen

### Positive Konsequenzen

- Update-Auswirkungen werden maschinell bewertbar.
- Breaking Changes werden explizit sichtbar.
- Anwendungen erhalten planbare Migrationszeiträume.
- veraltete Capabilities können kontrolliert entfernt werden.

### Negative Konsequenzen

- Änderungen benötigen strukturierte Metadaten.
- deprecated Komponenten müssen teilweise länger parallel gepflegt werden.

## Abhängigkeiten

- `ADR-UPDATE-0001_Deklarativer_Softwarezustand`
- `ADR-UPDATE-0008_Side_By_Side_Versionen`
- `ADR-UPDATE-0009_Capability_Based_Dependency_Resolution`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0019_Evolvierbar`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-SEMANTIC-CHANGELOG-0001`
- `NPSPEC-UPDATE-DEPRECATION-LIFECYCLE-0001`
- `NPSPEC-UPDATE-COMPATIBILITY-CHANGE-0001`

## Ergebnis

NovaOS behandelt Änderungen als strukturierte Systeminformation:

```text
Change
   ↓
Semantic Classification
   ↓
Compatibility
   ↓
Deprecation / Migration
   ↓
Controlled Removal
```

Der zentrale Grundsatz lautet:

```text
Änderungen werden angekündigt,
semantisch beschrieben
und kontrolliert eingeführt oder entfernt.
```