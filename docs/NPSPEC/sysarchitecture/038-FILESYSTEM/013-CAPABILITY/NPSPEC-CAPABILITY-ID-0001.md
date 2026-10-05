# NPSPEC-CAPABILITY-ID-0001 – Nova Capability ID

## Status

Angenommen

## Kategorie

Capability / Identity

## Zweck

NovaOS definiert ein global eindeutiges und dauerhaft stabiles Identifikationsschema für Capabilities.

Die `CapabilityID` beschreibt, welche Fähigkeit gemeint ist. Sie ist unabhängig von Provider, Implementierung, Registry-Pfad, Berechtigung und konkreter Capability-Instanz.

## Grundprinzipien

```text
CapabilityID ≠ Authority
CapabilityID ≠ Capability Token
CapabilityID ≠ ProviderID
CapabilityID ≠ Implementation
CapabilityID ≠ Registry Path
CapabilityID ≠ Permission
Knowledge ≠ Possession
```

## Format

Capability-IDs besitzen verbindlich folgende Struktur:

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

- das erste Segment ist immer `domain`
- das zweite Segment ist immer `authority`
- alle weiteren Segmente außer dem letzten bilden den `namespace`
- das letzte Segment ist immer der `name`

## Identität

Die vollständige Zeichenfolge bildet die stabile Identität einer Capability:

```text
CapabilityID
    ↓
de.nova.image.filter.gaussian
```

Die Identität darf nicht aus ihrem Speicherort oder ihrer Kategorie abgeleitet werden.

Beispielsweise darf:

```text
Registry/Image/Filters/
```

lediglich der Organisation oder Darstellung dienen.

## Namespace

Der Capability-Namespace ermöglicht hierarchische fachliche Gliederung:

```text
de.nova.network.connect
de.nova.network.http.request
de.nova.image.decode
de.nova.image.filter.gaussian
de.nova.storage.object.read
```

Namespace-Hierarchie erzeugt keine Vererbungsbeziehung für Authority.

Der Besitz von:

```text
de.nova.image.filter.gaussian
```

erzeugt beispielsweise keine Authority für:

```text
de.nova.image
```

oder andere Capabilities desselben Namespace.

## Provider

Mehrere Provider dürfen dieselbe `CapabilityID` implementieren:

```text
CapabilityID
├── Provider A
├── Provider B
└── Provider C
```

Ein Providerwechsel verändert die Capability-Identität nicht.

## Versionierung

Die semantische Identität und die Version bleiben getrennt:

```text
CapabilityID
+
CapabilityVersion
```

Kompatible Weiterentwicklungen können dieselbe `CapabilityID` verwenden.

Inkompatible Bedeutungsänderungen dürfen die bestehende Capability-Identität nicht stillschweigend umdefinieren.

## Authority

Die Kenntnis einer Capability-ID gewährt keinerlei Rechte.

```text
CapabilityID
      ↓
Discovery
      ↓
Policy Evaluation
      ↓
Capability Token / Handle
      ↓
Authority
```

Nur eine gültige Capability-Instanz beziehungsweise ein autorisiertes Handle repräsentiert tatsächliche Authority.

## Normative Anforderungen

1. Jede NovaOS-Capability MUSS eine eindeutige `CapabilityID` besitzen.
2. Capability-IDs MÜSSEN dem Schema `domain.authority.namespace.name` entsprechen.
3. Das erste Segment MUSS `domain` repräsentieren.
4. Das zweite Segment MUSS `authority` repräsentieren.
5. Alle Segmente zwischen `authority` und `name` MÜSSEN den Namespace bilden.
6. Das letzte Segment MUSS den Capability-Namen bilden.
7. Capability-IDs MÜSSEN unabhängig von Registry-Pfaden bleiben.
8. Capability-IDs MÜSSEN unabhängig von Providern und Implementierungen bleiben.
9. Mehrere Provider DÜRFEN dieselbe `CapabilityID` implementieren.
10. Namespace-Hierarchie DARF keine implizite Authority-Vererbung erzeugen.
11. Capability-Version und Capability-ID MÜSSEN getrennt behandelt werden.
12. Kenntnis oder Discovery einer `CapabilityID` DARF keine Authority erzeugen.
13. Inkompatible semantische Änderungen DÜRFEN bestehende Capability-IDs nicht stillschweigend umdefinieren.
14. Capability-ID, Version und registrierte Provider MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-REGISTRY-CAPABILITY-0001`
- `NPSPEC-POLICY-CAPABILITY-0001`
- `NPSPEC-TRUST-CAPABILITY-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt mit der `CapabilityID` eine stabile, providerunabhängige Identität für Systemfähigkeiten. Das Schema `domain.authority.namespace.name` ermöglicht weltweit eindeutige und hierarchisch organisierte Capability-Namen, ohne Identität, Version, Implementierung oder tatsächliche Authority miteinander zu vermischen.