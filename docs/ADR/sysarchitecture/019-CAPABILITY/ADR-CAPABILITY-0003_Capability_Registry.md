# ADR-CAPABILITY-0003 – Capability Registry

## Status

Angenommen

## Kategorie

Capability Architecture / Registry / Discovery / Provider Management

## Kontext

NovaOS behandelt Capabilities als primäre Systemfähigkeiten.

Eine Capability kann von mehreren Implementierungen und Providern bereitgestellt werden. Anwendungen sollen deshalb nicht wissen müssen, welche Bibliothek, welcher Prozess, welches Gerät oder welcher Remote Service eine Fähigkeit konkret implementiert.

NovaOS benötigt eine systemweite Instanz, über die bekannte Capabilities und ihre Provider auffindbar und auflösbar sind.

---

## Entscheidung

NovaOS führt eine **Capability Registry** als systemweite logische Registry für Capabilities und Capability Provider ein.

```text
Capability Provider
        ↓
Registration
        ↓
Capability Registry
        ↓
Discovery / Resolution
        ↓
Capability Consumer
```

Die Registry verwaltet nicht die eigentliche Ausführung einer Capability, sondern die Informationen, die für Discovery und Resolution benötigt werden.

---

## Grundprinzip

```text
Registry knows what exists.

Resolver decides what fits.

Runtime executes it.
```

Diese Verantwortlichkeiten bleiben getrennt.

---

## Registry-Einträge

Ein Registry-Eintrag kann unter anderem enthalten:

```text
Capability Identity
Contract Version
Provider Identity
Provider Version
Supported Semantic Types
Execution Properties
Provider Location
Trust Information
Availability
Compatibility Information
```

Die konkrete Datenstruktur wird durch NPSPECs definiert.

---

## Mehrere Provider

Eine Capability kann gleichzeitig von mehreren Providern bereitgestellt werden.

```text
de.nova.image.resize
        │
        ├── CPU Provider
        ├── GPU Provider
        ├── NPU Provider
        └── Remote Provider
```

Die Registry darf deshalb keine feste 1:1-Beziehung zwischen Capability und Implementierung voraussetzen.

---

## Registrierung

Capability Provider müssen ihre Fähigkeiten deklarieren.

Registrierung kann abhängig vom Provider-Lifecycle erfolgen, beispielsweise bei:

- Systemstart,
- Installation,
- Aktivierung,
- Gerät-Erkennung,
- Hotplug,
- Service Discovery,
- Hot Replacement.

Registrierungen müssen validiert werden, bevor sie als verwendbar gelten.

---

## Discovery

Consumer können nach Capabilities suchen anhand von:

```text
Capability ID
Namespace
Semantic Function
Input Type
Output Type
Contract Version
Provider Properties
```

Discovery liefert mögliche Fähigkeiten oder Provider.

Discovery bedeutet weder Auswahl noch Autorisierung.

---

## Capability Resolution

Die Registry stellt Kandidaten bereit.

Die eigentliche Auswahl erfolgt durch die Capability-Resolution-Architektur.

```text
Capability Requirement
        ↓
Registry
        ↓
Candidate Providers
        ↓
Hard Constraints
        ↓
Eligible Providers
        ↓
Optimization
        ↓
Selected Provider
```

Dabei können unter anderem berücksichtigt werden:

- `Nova.ExecutionContract`,
- Semantic Types,
- Trust,
- Security,
- Data Sovereignty,
- Determinismus,
- Deadline,
- Resource Budget,
- Locality,
- Hardwareeigenschaften.

---

## Registry und Security

Die Registry ist keine Autorisierungsinstanz.

```text
Capability exists
      ≠
Caller may use it
```

Security Capabilities, ABAC, MAC, Trust und andere Sicherheitsmechanismen bleiben unabhängig davon verbindlich.

Auch die Registrierung eines Providers selbst kann entsprechende Authority erfordern.

---

## Dynamische Registry

Die Registry muss dynamische Änderungen unterstützen.

```text
Provider Added
Provider Removed
Provider Updated
Device Hotplug
Service Migration
Provider Health Change
```

Consumer dürfen nicht davon ausgehen, dass ein Registry-Eintrag dauerhaft verfügbar bleibt.

---

## Verteilte Capabilities

Remote Capabilities und Remote Provider können in dieselbe logische Discovery-Architektur integriert werden.

Die Registry muss dabei nicht sämtliche Informationen zentral auf einem einzelnen Rechner speichern.

```text
Local Registry View
       ↓
Local + Remote Discovery Sources
       ↓
Unified Capability View
```

Damit bleibt die Registry logisch systemweit, ohne einen zwingenden zentralen Single Point of Failure zu erzeugen.

---

## Caching

Registry-Informationen dürfen gecacht werden.

Caches müssen jedoch:

- versionierbar,
- invalidierbar,
- zeitlich oder ereignisbasiert aktualisierbar

sein.

Ein Cache darf veraltete Providerinformationen nicht dauerhaft als gültigen Systemzustand behandeln.

---

## Hot Replacement

Capability Registry und Hot Replacement müssen integriert sein.

```text
Provider V1
    ↓
Provider V2 registered
    ↓
Compatibility Validation
    ↓
New Resolution
    ↓
Provider V1 retired
```

Die Capability Identity kann dabei stabil bleiben.

---

## Introspection

Die Registry ist Bestandteil der Architecture Introspection.

NovaOS muss beispielsweise nachvollziehbar machen können:

```text
Capability
Registered Providers
Contract Versions
Provider State
Availability
Compatibility
Resolution Candidates
```

Sensitive Security- oder Providerinformationen dürfen dabei nur entsprechend vorhandener Authority sichtbar sein.

---

## Normative Anforderungen

1. NovaOS MUSS eine systemweite logische Capability Registry bereitstellen.
2. Die Registry MUSS Capabilities anhand ihrer stabilen Capability Identity verwalten können.
3. Eine Capability MUSS mehrere registrierte Provider besitzen können.
4. Provider MÜSSEN ihre unterstützten Capabilities deklarieren.
5. Registrierungen MÜSSEN vor ihrer Verwendung validiert werden.
6. Registry, Capability Resolution und Capability Execution MÜSSEN getrennte Verantwortlichkeiten bleiben.
7. Discovery MUSS anhand semantischer und vertraglicher Eigenschaften möglich sein.
8. Discovery DARF NICHT automatisch Authority zur Verwendung einer Capability verleihen.
9. Die Registry MUSS dynamisches Hinzufügen, Entfernen und Aktualisieren von Providern unterstützen.
10. Hotplug und Hot Replacement MÜSSEN integrierbar sein.
11. Remote Provider MÜSSEN in die logische Registry- und Discovery-Architektur integrierbar sein.
12. Die Registry DARF KEINEN zwingenden globalen Single Point of Failure voraussetzen.
13. Registry-Informationen DÜRFEN gecacht werden, wenn ihre Gültigkeit kontrolliert werden kann.
14. Veraltete oder ungültige Provider MÜSSEN aus der aktiven Resolution ausgeschlossen werden können.
15. Registry-Zustand und Providerzuordnungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- zentrale logische Sicht auf verfügbare Systemfähigkeiten,
- mehrere Implementierungen pro Capability,
- dynamische Provider-Auswahl,
- Hardware- und Providerwechsel ohne Änderung der Capability Identity,
- Unterstützung für Hotplug und Hot Replacement,
- lokale und entfernte Fähigkeiten können einheitlich entdeckt werden.

### Negative Konsequenzen

- Registry-Zustände müssen konsistent gehalten werden,
- dynamische Provider erzeugen Invalidierungs- und Lifecycle-Anforderungen,
- verteilte Discovery benötigt definierte Konsistenz- und Vertrauensregeln.

---

## Verworfene Alternativen

### Statische Capability-Liste im Betriebssystem

Verworfen.

Neue Provider, Geräte und Capabilities könnten nicht dynamisch integriert werden.

### Eine Capability besitzt genau einen Provider

Verworfen.

Dies würde automatische Provider- und Algorithmusauswahl verhindern.

### Registry führt Capabilities selbst aus

Verworfen.

Discovery, Resolution und Execution sollen getrennte Verantwortlichkeiten bleiben.

### Eine zwingend zentrale Registry-Instanz

Verworfen.

Dies würde einen unnötigen Single Point of Failure erzeugen und verteilte Systeme erschweren.

---

## Abhängigkeiten

- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-CAPABILITY-0002_Domain_Authority_Namespace_Name_als_Capability_Identity`
- `ADR-ARCH-0012_Hot_Replacement_Live_Evolution`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-DISTCOMM-0013_Integrierte_Service_Mesh_Funktionen`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`

---

## Zugehörige NPSPECs

- `NPSPEC-CAPABILITY-REGISTRY-0001`
- `NPSPEC-CAPABILITY-REGISTRATION-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-PROVIDER-METADATA-0001`
- `NPSPEC-CAPABILITY-REGISTRY-CACHE-0001`
- `NPSPEC-CAPABILITY-REGISTRY-LIFECYCLE-0001`
- `NPSPEC-CAPABILITY-REGISTRY-TEST-0001`

---

## Ergebnis

Die Capability Registry bildet den systemweiten Katalog der verfügbaren Fähigkeiten und ihrer Provider:

```text
Providers
    ↓
Capability Registry
    ↓
Discovery
    ↓
Candidate Providers
    ↓
Capability Resolution
    ↓
Selected Provider
    ↓
Execution
```

Damit können Capabilities unabhängig von ihrer konkreten Implementierung gefunden, dynamisch bereitgestellt und durch NovaOS entsprechend der aktuellen Anforderungen aufgelöst werden.