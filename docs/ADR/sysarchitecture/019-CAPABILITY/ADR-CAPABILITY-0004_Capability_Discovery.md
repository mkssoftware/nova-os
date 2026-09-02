# ADR-CAPABILITY-0004 – Capability Discovery

## Status

Angenommen

## Kategorie

Capability Architecture / Discovery / Resolution

## Kontext

NovaOS stellt Systemfunktionen als Capabilities bereit. Anwendungen, Systemdienste, Pipelines und KI-Agenten müssen verfügbare Fähigkeiten finden können, ohne konkrete Provider, Bibliotheken oder Implementierungen zu kennen.

Die Capability Registry enthält Informationen über bekannte Capabilities und Provider. Darauf aufbauend benötigt NovaOS eine semantische Discovery-Schicht.

Discovery beantwortet:

```text
Welche Capabilities können diese Anforderung erfüllen?
```

Sie entscheidet noch nicht, welcher Provider tatsächlich verwendet wird.

---

## Entscheidung

NovaOS führt **Capability Discovery als native systemweite Funktion** ein.

```text
Capability Requirement
        ↓
Discovery
        ↓
Matching Capabilities
        ↓
Candidate Providers
        ↓
Capability Resolution
```

Discovery arbeitet auf Capability Contracts, Semantic Types und Registry-Metadaten statt auf Dateinamen oder konkreten Implementierungen.

---

## Grundprinzip

```text
Discover by capability and semantics,
not by implementation location.
```

---

## Discovery-Anfragen

Eine Discovery-Anfrage kann unterschiedliche Informationen enthalten:

```text
Capability Identity
Namespace
Semantic Function
Input Types
Output Types
Contract Requirements
Execution Properties
```

Eine exakte Capability-ID ist damit nicht zwingend erforderlich.

Beispielsweise kann ein Consumer nach einer Fähigkeit suchen, die:

```text
Image → Image
```

abbildet und eine bestimmte semantische Operation unterstützt.

---

## Exakte Discovery

Ist die Capability-ID bekannt, kann direkt danach gesucht werden.

```text
de.nova.image.filter.gaussian
        ↓
Discovery
        ↓
Matching Capability
```

Dies ist der eindeutigste Discovery-Modus.

---

## Semantische Discovery

NovaOS unterstützt zusätzlich Discovery anhand semantischer Anforderungen.

```text
Requirement
   ↓
Input: Image
Output: Image
Function: Noise Reduction
   ↓
Discovery
   ↓
Matching Capabilities
```

Semantic Types bilden dabei eine zentrale Grundlage.

Die konkreten Matching-Regeln werden in NPSPECs definiert.

---

## Namespace Discovery

Capabilities können anhand ihrer Namespace-Struktur gesucht und gruppiert werden.

Beispiel:

```text
de.nova.image.*
de.nova.image.filter.*
```

Namespaces dienen dabei der Discovery und Organisation.

Sie erzeugen keine automatische Vererbung oder Security Authority.

---

## Discovery und Registry

Capability Discovery verwendet die Capability Registry als wichtige Informationsquelle.

```text
Registry
   ↓
Capability Metadata
   ↓
Discovery Engine
   ↓
Candidates
```

Weitere Discovery Provider können ebenfalls eingebunden werden, beispielsweise für entfernte oder dynamisch verfügbare Fähigkeiten.

---

## Discovery und Resolution

Discovery und Resolution bleiben getrennt.

```text
Discovery
→ Was könnte die Anforderung erfüllen?

Resolution
→ Welche konkrete Option soll verwendet werden?
```

Discovery darf daher mehrere Kandidaten zurückgeben.

Resolution berücksichtigt anschließend beispielsweise:

- Security,
- Trust,
- Data Sovereignty,
- Execution Contract,
- Resource Budget,
- Deadline,
- Determinismus,
- Locality,
- Provider Health.

---

## Discovery und Security

Das Finden einer Capability gewährt keine Berechtigung zu ihrer Verwendung.

```text
Discovered
    ≠
Authorized
```

Darüber hinaus können bereits Discovery-Informationen selbst geschützt sein.

NovaOS muss deshalb kontrollieren können, welche Capabilities und Provider ein Consumer überhaupt entdecken darf.

---

## Dynamische Discovery

Discovery muss Änderungen der Systemumgebung berücksichtigen können.

Beispiele:

```text
Device Hotplug
Provider Installation
Provider Removal
Service Start / Stop
Remote Service Availability
Hot Replacement
```

Discovery-Ergebnisse sind daher grundsätzlich Momentaufnahmen und keine dauerhafte Verfügbarkeitsgarantie.

---

## Lokale und entfernte Capabilities

Lokale und entfernte Capabilities können über dieselbe Discovery-Abstraktion gefunden werden.

```text
Capability Requirement
        ↓
Discovery
     /     \
    ↓       ↓
 Local    Remote
```

Location Transparency darf dabei Kosten, Trust, Sovereignty oder Verfügbarkeit nicht verbergen.

Diese Eigenschaften müssen für die spätere Resolution verfügbar bleiben.

---

## Discovery Caching

Discovery-Ergebnisse dürfen gecacht werden.

Caches müssen jedoch an geeignete Registry-Versionen, Providerzustände oder Gültigkeitsinformationen gebunden sein.

Ein gecachtes Ergebnis darf nicht als Garantie interpretiert werden, dass der Provider zum späteren Ausführungszeitpunkt noch verfügbar ist.

---

## Introspection

Capability Discovery wird in Architecture Introspection integriert.

Nach entsprechender Autorisierung soll nachvollziehbar sein:

```text
Discovery Requirement
Matched Capabilities
Candidate Providers
Rejected Candidates
Relevant Constraints
```

Damit kann insbesondere erklärt werden, warum eine erwartete Capability gefunden oder nicht gefunden wurde.

---

## Normative Anforderungen

1. NovaOS MUSS eine systemweite Capability Discovery bereitstellen.
2. Discovery MUSS anhand stabiler Capability-IDs möglich sein.
3. Discovery MUSS semantische Anforderungen berücksichtigen können.
4. Semantic Types MÜSSEN für Capability Discovery verwendbar sein.
5. Namespace-basierte Discovery MUSS unterstützt werden.
6. Discovery DARF NICHT von physischen Installationspfaden abhängig sein.
7. Discovery MUSS mehrere passende Capabilities und Provider zurückgeben können.
8. Discovery und Capability Resolution MÜSSEN getrennte Verantwortlichkeiten bleiben.
9. Discovery DARF NICHT automatisch Authority zur Verwendung einer Capability gewähren.
10. Discovery-Informationen MÜSSEN selbst durch Security Policies einschränkbar sein.
11. Dynamisch hinzugefügte oder entfernte Provider MÜSSEN berücksichtigt werden können.
12. Lokale und entfernte Capabilities MÜSSEN über dieselbe logische Discovery-Architektur auffindbar sein können.
13. Discovery-Ergebnisse DÜRFEN gecacht werden, sofern ihre Gültigkeit kontrolliert werden kann.
14. Ein Discovery-Ergebnis DARF NICHT als dauerhafte Verfügbarkeitsgarantie gelten.
15. Discovery-Entscheidungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Anwendungen müssen konkrete Provider nicht kennen,
- Fähigkeiten können anhand ihrer Semantik gefunden werden,
- neue Provider können dynamisch integriert werden,
- lokale und entfernte Fähigkeiten nutzen dasselbe Discovery-Modell,
- Capability-basierte adaptive Anwendungen werden möglich.

### Negative Konsequenzen

- semantisches Matching benötigt klar definierte Verträge,
- dynamische Discovery benötigt Cache-Invalidierung,
- große Capability-Bestände benötigen effiziente Indizierung.

---

## Verworfene Alternativen

### Discovery ausschließlich über Capability-ID

Verworfen.

Consumer sollen auch Fähigkeiten anhand semantischer Anforderungen finden können.

### Discovery über Dateisystem oder Bibliotheksnamen

Verworfen.

Implementierungsort und Capability Identity sind bewusst getrennt.

### Discovery wählt direkt den Provider

Verworfen.

Discovery und Resolution besitzen unterschiedliche Verantwortlichkeiten.

### Discovery bedeutet automatisch Nutzungsberechtigung

Verworfen.

Auffindbarkeit und Authority müssen getrennt bleiben.

---

## Abhängigkeiten

- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-CAPABILITY-0002_Domain_Authority_Namespace_Name_als_Capability_Identity`
- `ADR-CAPABILITY-0003_Capability_Registry`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`

---

## Zugehörige NPSPECs

- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-QUERY-0001`
- `NPSPEC-CAPABILITY-SEMANTIC-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-NAMESPACE-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-CACHE-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-SECURITY-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-TEST-0001`

---

## Ergebnis

NovaOS ermöglicht die Suche nach Systemfähigkeiten unabhängig von deren konkreter Implementierung:

```text
"What capability do I need?"
            ↓
      Capability Discovery
            ↓
   Matching Capabilities
            ↓
     Candidate Providers
            ↓
   Capability Resolution
```

Damit können Anwendungen Fähigkeiten sowohl über stabile Identitäten als auch über semantische Anforderungen entdecken, während Provider-Auswahl, Autorisierung und tatsächliche Ausführung weiterhin klar getrennte Systemaufgaben bleiben.