# NPSPEC-CAPABILITY-NAMESPACE-0001 – Nova Capability Namespace

## Status

Angenommen

## Kategorie

Capability / Namespace

## Zweck

NovaOS definiert die hierarchische fachliche Struktur innerhalb einer `CapabilityID`.

Der Capability Namespace organisiert Fähigkeiten semantisch, ohne daraus Authority, Berechtigungsvererbung oder eine Abhängigkeit von Registry- oder Dateisystempfaden abzuleiten.

## Grundprinzipien

```text
Capability Namespace ≠ Filesystem Namespace
Capability Namespace ≠ Registry Path
Namespace Membership ≠ Authority
Parent Namespace ≠ Parent Permission
Namespace ≠ Provider
Namespace ≠ Capability
```

## Struktur

Eine Capability-ID besitzt das Schema:

```text
domain.authority.namespace.name
```

Beispiel:

```text
de.nova.image.filter.gaussian
```

Aufteilung:

```text
de            → Domain
nova          → Authority
image.filter  → Namespace
gaussian      → Name
```

Der Namespace besteht aus allen Segmenten zwischen `authority` und `name`.

## Hierarchie

Namespaces können fachlich hierarchisch aufgebaut werden:

```text
image
├── decode
├── encode
└── filter
    ├── gaussian
    ├── sharpen
    └── resize
```

Daraus können beispielsweise entstehen:

```text
de.nova.image.decode
de.nova.image.encode
de.nova.image.filter.gaussian
de.nova.image.filter.sharpen
de.nova.image.filter.resize
```

## Semantische Organisation

Namespaces sollen Funktionen nach ihrem fachlichen Zusammenhang gruppieren.

Beispiele:

```text
network.http
storage.object
image.filter
media.audio
device.input
security.crypto
```

Konkrete Provider, Implementierungen oder Hardwarevarianten gehören nicht in den Namespace, sofern sie nicht selbst Teil der semantischen Fähigkeit sind.

## Keine Authority-Vererbung

Namespace-Hierarchie erzeugt keine Berechtigungsvererbung.

```text
de.nova.image.filter.gaussian
```

gewährt weder automatisch:

```text
de.nova.image.filter
```

noch:

```text
de.nova.image
```

noch andere Capabilities innerhalb desselben Namespace.

Jede Capability bleibt eine eigenständige Authority.

## Keine Wildcard-Authority

Ein Namespace-Ausdruck wie:

```text
de.nova.image.*
```

darf nicht implizit als Capability-Token für alle darunterliegenden Capabilities interpretiert werden.

Falls Policy mehrere Capabilities gemeinsam behandelt, bleibt dies eine Policy-Regel und keine einzelne Capability-Authority.

## Registry

Die Capability Registry darf Namespaces für:

```text
Discovery
Grouping
Filtering
UI Presentation
Documentation
```

verwenden.

Die physische oder logische Registry-Struktur bestimmt jedoch nicht den Capability Namespace.

## Stabilität

Namespaces sollen langfristig semantisch stabil bleiben.

Reorganisationen der UI, Registry oder internen Implementierung dürfen keine Umbenennung stabiler Capability-IDs erzwingen.

## Normative Anforderungen

1. Der Capability Namespace MUSS Bestandteil der `CapabilityID` sein.
2. Er MUSS aus allen Segmenten zwischen `authority` und `name` bestehen.
3. Capability Namespaces MÜSSEN fachlich und semantisch strukturiert werden.
4. Capability Namespace und Filesystem Namespace MÜSSEN getrennte Konzepte bleiben.
5. Registry-Pfade DÜRFEN den Capability Namespace nicht bestimmen.
6. Provider DÜRFEN die Namespace-Identität nicht bestimmen.
7. Namespace-Hierarchie DARF keine Authority-Vererbung erzeugen.
8. Parent Namespaces DÜRFEN keine impliziten Rechte auf Child Capabilities erzeugen.
9. Child Capabilities DÜRFEN keine impliziten Rechte auf Parent Namespaces erzeugen.
10. Wildcard-Namespace-Ausdrücke DÜRFEN nicht automatisch Capability-Authority darstellen.
11. Registry Discovery DARF nach Capability Namespaces filtern können.
12. Namespace-Änderungen DÜRFEN stabile Capability-IDs nicht ohne semantischen Grund verändern.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-NAMING-0001`
- `NPSPEC-REGISTRY-CAPABILITY-0001`
- `NPSPEC-POLICY-CAPABILITY-0001`

## Ergebnis

NovaOS verwendet Capability Namespaces zur stabilen semantischen Organisation seiner Fähigkeiten. Die Hierarchie verbessert Discovery und Strukturierung, erzeugt jedoch keinerlei implizite Authority oder Berechtigungsvererbung.