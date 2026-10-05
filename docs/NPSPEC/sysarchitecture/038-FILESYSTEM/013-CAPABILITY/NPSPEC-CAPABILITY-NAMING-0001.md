# NPSPEC-CAPABILITY-NAMING-0001 – Nova Capability Naming

## Status

Angenommen

## Kategorie

Capability / Naming

## Zweck

NovaOS definiert verbindliche Regeln für die Benennung von Capabilities.

Capability-Namen müssen stabil, eindeutig, maschinenlesbar und unabhängig von UI-Sprache, Registry-Struktur, Provider und Implementierung sein.

## Grundformat

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

Dabei gilt:

```text
Segment 1       = Domain
Segment 2       = Authority
Segment 3..n-1  = Namespace
Letztes Segment = Name
```

## Schreibweise

Capability-IDs verwenden ausschließlich kanonische technische Bezeichner.

Bevorzugt wird:

```text
lowercase
```

Beispiele:

```text
de.nova.network.connect
de.nova.network.http.request
de.nova.storage.object.read
de.nova.image.filter.gaussian
```

Nicht zulässig sind sprachabhängige Anzeigenamen innerhalb der Capability-ID.

```text
de.nova.network.verbinden     → nicht verwenden
de.nova.network.connect       → korrekt
```

## Domain

Die Domain bezeichnet den Namensraum des Herausgebers beziehungsweise Ökosystems.

Beispiele:

```text
de.nova
com.vendor
org.project
```

Sie dient der globalen Kollisionsvermeidung.

## Authority

Das zweite Segment bezeichnet die verantwortliche Authority innerhalb der Domain.

```text
de.nova
   └─ nova = Authority
```

Eine Authority kontrolliert die von ihr definierten Capability-Namespaces.

## Namespace

Der Namespace beschreibt die fachliche Einordnung:

```text
network
network.http
storage.object
image.filter
media.audio
```

Namespaces sollen nach Funktion und nicht nach konkreter Implementierung benannt werden.

## Name

Das letzte Segment beschreibt die konkrete Fähigkeit oder Operation:

```text
read
write
connect
request
decode
encode
gaussian
```

Der Name soll möglichst präzise und dauerhaft semantisch stabil sein.

## Providerunabhängigkeit

Provider- oder Produktnamen dürfen nicht zur Unterscheidung konkreter Implementierungen in die Capability-ID eingebaut werden.

```text
de.nova.image.decode
```

kann beispielsweise von mehreren Providern implementiert werden.

```text
CapabilityID
├── Provider A
├── Provider B
└── Provider C
```

## Versionierung

Versionsnummern gehören nicht in die Capability-ID.

Nicht:

```text
de.nova.image.decode.v2
```

Sondern:

```text
CapabilityID: de.nova.image.decode
Version: 2
```

Capability-Identität und Version bleiben getrennt.

## Lokalisierung

Capability-IDs werden nicht lokalisiert.

Die UI darf lokalisierte Anzeigenamen verwenden:

```text
CapabilityID:
de.nova.network.connect

Deutsch:
Netzwerkverbindung herstellen

Englisch:
Connect to network
```

Die technische Identität bleibt unverändert.

## Semantische Stabilität

Eine bestehende Capability-ID darf nicht für eine grundlegend andere Bedeutung wiederverwendet werden.

```text
gleiche Bedeutung
→ gleiche CapabilityID

inkompatible neue Bedeutung
→ neue CapabilityID
```

## Normative Anforderungen

1. Capability-Namen MÜSSEN dem Schema `domain.authority.namespace.name` folgen.
2. Das erste Segment MUSS die Domain darstellen.
3. Das zweite Segment MUSS die Authority darstellen.
4. Alle mittleren Segmente MÜSSEN den Namespace bilden.
5. Das letzte Segment MUSS den konkreten Capability-Namen bilden.
6. Capability-IDs MÜSSEN kanonische technische Bezeichner verwenden.
7. Capability-IDs DÜRFEN nicht lokalisiert werden.
8. UI-Anzeigenamen MÜSSEN von der technischen Capability-ID getrennt bleiben.
9. Namespaces SOLLEN nach Funktion und nicht nach Implementierung strukturiert werden.
10. Provider-Identitäten DÜRFEN die Capability-Identität nicht bestimmen.
11. Versionsnummern DÜRFEN nicht Bestandteil der Capability-ID sein.
12. Registry-Pfade DÜRFEN die Capability-ID nicht bestimmen.
13. Bestehende Capability-IDs DÜRFEN nicht mit inkompatibler Bedeutung wiederverwendet werden.
14. Capability-Namen SOLLEN kurz, eindeutig und semantisch stabil sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-REGISTRY-CAPABILITY-0001`
- `NPSPEC-TRUST-CAPABILITY-0001`
- `NPSPEC-POLICY-CAPABILITY-0001`

## Ergebnis

NovaOS verwendet ein einheitliches, global eindeutiges und sprachunabhängiges Benennungssystem für Capabilities. Identität, Version, Provider, Registry-Struktur und lokalisierte Darstellung bleiben dadurch konsequent voneinander getrennt.