# ADR-CAPABILITY-0002 – Domain Authority Namespace Name als Capability Identity

## Status

Angenommen

## Kategorie

Capability Architecture / Identity / Naming

## Kontext

Capabilities benötigen eine stabile und global eindeutig strukturierbare Identität.

Diese Identität darf nicht von:

- Dateipfaden,
- Installationsorten,
- Registry-Kategorien,
- Provider-Namen,
- Implementierungen,
- Geräten oder
- UI-Kategorien

abhängen.

Eine Capability muss dieselbe Identität behalten können, auch wenn ihre Implementierung, ihr Provider oder ihr physischer Speicherort geändert wird.

NovaOS benötigt deshalb ein verbindliches Identitätsschema für alle Capabilities.

---

## Entscheidung

NovaOS verwendet für Capability-IDs verbindlich die Struktur:

```text
domain.authority.namespace.name
```

Die Segmente besitzen folgende Bedeutung:

```text
domain     = globale Namensdomäne
authority  = verantwortliche Namensautorität
namespace  = fachliche oder funktionale Hierarchie
name       = konkreter Capability-Name
```

Beispiel:

```text
de.nova.image.filter.gaussian
```

wird interpretiert als:

```text
Domain:     de
Authority:  nova
Namespace:  image.filter
Name:       gaussian
```

---

## Grundprinzip

```text
Capability Identity
    ≠
Physical Location
    ≠
Provider
    ≠
Implementation
```

Die Capability-ID bezeichnet die Fähigkeit selbst.

---

## Segmentregeln

Eine Capability-ID besitzt mindestens:

```text
domain.authority.name
```

Wenn weitere Segmente vorhanden sind, gilt:

```text
erstes Segment  → domain
zweites Segment → authority
letztes Segment → name
dazwischen      → namespace
```

Beispiel:

```text
org.example.audio.processing.noise.reduction
```

ergibt:

```text
Domain:     org
Authority:  example
Namespace:  audio.processing.noise
Name:       reduction
```

Der Namespace kann somit aus mehreren Segmenten bestehen.

---

## Domain

Die `domain` bildet die oberste Namensdomäne.

Beispiele:

```text
de
org
com
edu
```

Die konkrete Vergabe- und Registrierungssemantik wird in den zugehörigen NPSPECs definiert.

---

## Authority

Die `authority` identifiziert die für den darunterliegenden Capability-Namensraum verantwortliche Instanz.

Beispiel:

```text
de.nova.*
```

bedeutet, dass `nova` innerhalb der Domain `de` die entsprechende Authority darstellt.

Eine Authority ist eine Namensautorität und nicht automatisch eine Security Authority oder Vertrauensaussage.

```text
Naming Authority
    ≠
Security Authority
    ≠
Trust
```

---

## Namespace

Der Namespace organisiert Capabilities semantisch.

Beispiele:

```text
image.filter
audio.codec
data.sort
graphics.render
ai.inference
```

Namespaces dienen der strukturierten Identifikation und Discovery.

Sie definieren jedoch nicht automatisch Vererbung, Berechtigungen oder Implementierungsabhängigkeiten.

---

## Name

Das letzte Segment bezeichnet die konkrete Capability innerhalb ihres Namensraums.

Beispiel:

```text
de.nova.image.filter.gaussian
                            ↑
                           name
```

Der Name muss innerhalb seines vollständigen Namespace-Kontextes eindeutig sein.

---

## Physische Unabhängigkeit

Die Capability:

```text
de.nova.image.filter.gaussian
```

kann beispielsweise implementiert sein durch:

```text
/System/...
/Apps/...
Driver
GPU Provider
Remote Provider
```

Die Capability-ID bleibt dabei unverändert.

Ein Verschieben oder Austauschen einer Implementierung erzeugt keine neue Capability Identity.

---

## Provider und Implementierung

Mehrere Provider können dieselbe Capability anbieten.

```text
de.nova.image.filter.gaussian
        │
        ├── CPU Provider
        ├── SIMD Provider
        ├── GPU Provider
        └── Remote Provider
```

Provider benötigen deshalb eigene Identitäten, die nicht mit der Capability-ID verwechselt werden dürfen.

---

## Versionierung

Die Capability-Version ist kein Bestandteil der grundlegenden Capability-ID.

```text
Capability Identity
        +
Capability Contract Version
```

Dadurch bleibt die semantische Identität über kompatible Weiterentwicklungen hinweg stabil.

Inkompatible Bedeutungsänderungen dürfen nicht stillschweigend unter derselben Identität erfolgen.

Die konkreten Evolutionsregeln werden separat spezifiziert.

---

## Discovery und Registry

Capability Registry und Discovery verwenden die Capability-ID als primären semantischen Schlüssel.

```text
Capability ID
      ↓
Registry / Discovery
      ↓
Compatible Providers
```

Kategorieordner oder UI-Gruppierungen dürfen zusätzliche Metadaten darstellen, bestimmen aber nicht die Identität.

---

## Security

Der Besitz oder die Kenntnis einer Capability-ID gewährt keine Berechtigung.

```text
Know Capability ID
        ≠
Authority to Execute
```

Die tatsächliche Nutzung unterliegt weiterhin dem Capability-Based Security Model, Trust, Policies und weiteren Hard Constraints.

---

## Normative Anforderungen

1. Jede NovaOS-Capability MUSS eine stabile Capability-ID besitzen.
2. Capability-IDs MÜSSEN der Struktur `domain.authority.namespace.name` folgen.
3. Das erste Segment MUSS als `domain` interpretiert werden.
4. Das zweite Segment MUSS als `authority` interpretiert werden.
5. Das letzte Segment MUSS als `name` interpretiert werden.
6. Alle Segmente zwischen `authority` und `name` MÜSSEN gemeinsam den `namespace` bilden.
7. Der Namespace DARF aus mehreren Segmenten bestehen.
8. Der physische Speicherort DARF die Capability Identity NICHT bestimmen.
9. Provider und Implementierung DÜRFEN NICHT Bestandteil der semantischen Capability Identity sein.
10. Mehrere Provider MÜSSEN dieselbe Capability-ID bereitstellen können.
11. Capability-Versionen SOLLEN getrennt von der grundlegenden Capability-ID modelliert werden.
12. Inkompatible semantische Änderungen DÜRFEN NICHT stillschweigend unter derselben Capability Identity erfolgen.
13. Registry und Discovery SOLLEN die Capability-ID als primären Identifikator verwenden.
14. Naming Authority DARF NICHT mit Security Authority oder Trust gleichgesetzt werden.
15. Kenntnis einer Capability-ID DARF keine Security Authority verleihen.

---

## Konsequenzen

### Positive Konsequenzen

- stabile Capability-Identitäten,
- unabhängig von Installationsort und Implementierung,
- globale Namespace-Struktur,
- mehrere Provider pro Capability möglich,
- einfache Registry- und Discovery-Integration,
- Capability-Verträge können unabhängig von Implementierungen evolvieren.

### Negative Konsequenzen

- Namensvergabe benötigt verbindliche Regeln,
- Authorities müssen Namenskollisionen verhindern,
- semantische Änderungen benötigen kontrollierte Evolutionsregeln.

---

## Verworfene Alternativen

### Capability Identity über Dateipfad

Verworfen.

Physische Struktur und semantische Identität müssen unabhängig voneinander sein.

### Provider als Bestandteil der Capability-ID

Verworfen.

Eine Capability muss mehrere austauschbare Provider besitzen können.

### Version direkt in jeder Capability-ID

Verworfen.

Kompatible Weiterentwicklung soll die stabile semantische Identität nicht unnötig verändern.

### Kategorieordner bestimmt Capability Identity

Verworfen.

Kategorien sind Organisations- und Darstellungsinformationen, keine Identitätsquelle.

---

## Abhängigkeiten

- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0019_Evolvable_System_Architecture`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`

---

## Zugehörige NPSPECs

- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-NAMING-0001`
- `NPSPEC-CAPABILITY-NAMESPACE-0001`
- `NPSPEC-CAPABILITY-AUTHORITY-0001`
- `NPSPEC-CAPABILITY-VERSIONING-0001`
- `NPSPEC-CAPABILITY-IDENTITY-TEST-0001`

---

## Ergebnis

NovaOS verwendet eine stabile hierarchische Capability Identity:

```text
domain.authority.namespace.name
```

mit der verbindlichen Interpretation:

```text
First       → Domain
Second      → Authority
Middle      → Namespace
Last        → Name
```

Damit bleibt die Identität einer Fähigkeit unabhängig davon, wo sie gespeichert ist, welcher Provider sie implementiert oder auf welcher Hardware sie ausgeführt wird.