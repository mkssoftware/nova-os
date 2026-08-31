# ADR-ARCH-0019 – Evolvierbare Systemarchitektur

## Status

Angenommen

## Kategorie

Systemarchitektur / Evolution / Kompatibilität / Versionierung / Migration / Langzeitarchitektur

## Kontext

NovaOS ist als langfristig evolvierbares Betriebssystem konzipiert.

Ein Betriebssystem muss über viele Jahre neue Anforderungen aufnehmen können:

- neue Hardwarearchitekturen,
- neue Beschleuniger,
- neue Speichertechnologien,
- neue Sicherheitsmechanismen,
- neue Netzwerkmodelle,
- neue Datenformate,
- neue Capabilities,
- neue Algorithmen,
- neue Programmiersprachen- und ABI-Versionen,
- neue UI-Paradigmen,
- neue KI- und Agentensysteme,
- neue regulatorische Anforderungen.

Eine Architektur, deren grundlegende Strukturen nur durch vollständige Neuentwicklung verändert werden können, erzeugt langfristig technische Sackgassen.

Gleichzeitig darf Evolvierbarkeit nicht bedeuten, dass jede Schnittstelle permanent instabil ist.

NovaOS benötigt deshalb eine Architektur, die zwei scheinbar gegensätzliche Ziele verbindet:

```text
Stable semantic foundations
+
Evolvable implementations
```

Die Architektur soll stabile Bedeutungen und Contracts bereitstellen, während konkrete Implementierungen, Provider, Algorithmen und interne Strukturen weiterentwickelt oder ersetzt werden können.

---

## Entscheidung

NovaOS MUSS als **evolvierbare Systemarchitektur** entworfen werden.

Systemkomponenten sollen so gestaltet werden, dass sie unabhängig weiterentwickelt, versioniert, ersetzt und – soweit erforderlich – migriert werden können.

Die zentrale Trennung lautet:

```text
Stable Semantics
      │
      ▼
Versioned Contracts
      │
      ▼
Replaceable Implementations
      │
      ▼
Continuous Evolution
```

Dabei gilt:

```text
Preserve meaning where possible.

Evolve implementation where useful.

Break compatibility only deliberately.
```

Evolvierbarkeit ist keine einzelne Update-Technik, sondern eine Eigenschaft der gesamten Architektur.

---

## Stabile semantische Grenzen

Langfristige Stabilität soll primär an semantischen Contracts und nicht an konkreten Implementierungsdetails ausgerichtet werden.

Beispiel:

```text
Image.Resize
```

bleibt als Capability semantisch stabil, während sich darunter ändern können:

```text
Algorithm
Provider
GPU Backend
Memory Representation
Scheduler
Implementation Language
```

Anwendungen sollen dadurch nicht von unnötigen Implementierungsdetails abhängig werden.

---

## Versionierte Contracts

Systemgrenzen müssen versionierbar sein.

Dies betrifft insbesondere:

- Capability Contracts,
- Component Contracts,
- Execution Contracts,
- Semantic Types,
- IPC-Schemas,
- persistente Datenstrukturen,
- Treiberschnittstellen,
- NovaABI,
- NovaLang-Schnittstellen,
- Konfigurationsschemas.

Eine neue Version muss nicht automatisch inkompatibel sein.

Kompatibilität wird explizit bestimmt.

Mögliche Zustände sind:

```text
Compatible
BackwardCompatible
ForwardCompatible
Adaptable
Migratable
Incompatible
```

---

## Unabhängige Versionierung

Nicht das gesamte NovaOS muss eine einzige technische Versionsgrenze besitzen.

Subsysteme und Contracts dürfen unabhängig versioniert werden.

Beispiel:

```text
NovaOS
├── Capability Contract v4
├── Semantic Type Schema v3
├── Storage Provider v7
├── Graphics Provider v12
└── NovaABI v2
```

Dadurch können einzelne Bereiche evolvieren, ohne unnötig das gesamte System zu verändern.

---

## Capability-Evolution

Capabilities besitzen stabile Identitäten und versionierte Contracts.

Eine Capability kann neue Provider, Algorithmen oder Optimierungen erhalten, ohne ihre semantische Identität zu verlieren.

Beispiel:

```text
de.nova.image.filter.gaussian
```

kann zunächst durch:

```text
CPU Provider
```

und später zusätzlich durch:

```text
GPU Provider
NPU Provider
Distributed Provider
```

implementiert werden.

---

## Provider-Evolution

Provider sind austauschbare Implementierungen einer Capability.

Neue Provider dürfen eingeführt werden, solange sie die erforderlichen Contracts erfüllen.

Dadurch kann NovaOS zukünftige Hardware unterstützen, ohne die darüberliegende semantische API neu zu definieren.

---

## Algorithmus-Evolution

Die automatische Algorithmusauswahl ermöglicht es, neue Algorithmen einzuführen, ohne Anwendungscode ändern zu müssen.

```text
Sort(data)
```

kann über die Lebensdauer des Systems unterschiedliche optimale Algorithmen verwenden.

Explizit erzwungene Algorithmen bleiben davon unterscheidbar.

---

## Semantic-Type-Evolution

Semantic Types müssen versionierbar und erweiterbar sein.

Neue Versionen dürfen:

- zusätzliche Constraints,
- neue Metadaten,
- neue Repräsentationen,
- neue Konvertierungsmöglichkeiten

einführen.

Semantische Änderungen müssen von rein technischen Repräsentationsänderungen unterscheidbar bleiben.

---

## Schema Evolution

Persistente und über IPC übertragene Datenstrukturen benötigen explizite Schema-Evolution.

Ein Schema soll definieren können:

```text
Version
Compatibility
Defaults
Unknown Fields
Migration
Validation
```

Unbekannte Felder dürfen erhalten werden, wenn das jeweilige Format dies unterstützt.

Neue sicherheitsrelevante Semantik darf jedoch nicht durch permissive Defaults entstehen.

---

## State Migration

Wenn sich persistenter oder laufender Zustand strukturell verändert, muss Migration möglich sein.

Konzeptionell:

```text
State v1
   ↓
Migration
   ↓
State v2
   ↓
Validation
   ↓
Activation
```

Migrationen müssen fehlschlagen können, ohne den letzten gültigen Zustand unnötig zu zerstören.

---

## Migration Chains

Mehrere Versionen können Migration Chains benötigen:

```text
v1 → v2 → v3 → v4
```

Für häufige oder langfristige Formate können direkte Migrationen vorgesehen werden:

```text
v1 ─────────→ v4
```

wenn dies sicherer oder effizienter ist.

---

## Hot Replacement

Diese ADR baut auf `ADR-ARCH-0012_Hot_Replacement_und_Live_Evolution` auf.

Hot Replacement ist ein Mechanismus der Evolvierbarkeit.

Evolvierbare Architektur ist jedoch umfassender.

Sie umfasst zusätzlich:

- Contract Evolution,
- Schema Evolution,
- Datenmigration,
- API Evolution,
- Capability Evolution,
- Architekturtransformation,
- langfristige Kompatibilität.

Nicht jede Evolution muss live erfolgen.

Mögliche Evolutionsformen sind:

```text
Live
Rolling
Restart
Reboot
Offline Migration
Recovery Environment
```

---

## Declarative System Model

Das deklarative Systemmodell unterstützt Evolution, weil Desired State von konkreten Implementierungen getrennt bleibt.

Beispiel:

```text
Desired:
    Image Processing Capability
```

statt:

```text
Desired:
    exact provider binary version 17
```

sofern keine explizite Bindung erforderlich ist.

Nach einem Update kann Reconciliation einen neuen kompatiblen Provider auswählen.

---

## Explizite Contracts

Diese ADR baut auf `ADR-ARCH-0014_Explizite_Contracts_zwischen_Systemkomponenten` auf.

Explizite Contracts bilden die wichtigste Grenze für kontrollierte Evolution.

Eine Komponente darf ersetzt werden, wenn der neue Zustand die erforderlichen Contracts weiterhin erfüllt oder eine explizit unterstützte Migration verfügbar ist.

---

## Composability und Orthogonalität

Diese ADR ergänzt `ADR-ARCH-0017_Composability_und_Orthogonalität`.

Orthogonale Architektur reduziert den Umfang notwendiger Änderungen.

Neue Hardware soll beispielsweise primär die Provider-Dimension erweitern und nicht gleichzeitig Änderungen an:

```text
Capability Semantics
Security Model
UI API
Application Logic
```

erzwingen.

Je unabhängiger Architekturachsen evolvieren können, desto geringer ist der systemweite Änderungsradius.

---

## Kompatibilität

Kompatibilität ist mehrdimensional.

Eine neue Komponente kann ABI-kompatibel sein und trotzdem semantisch inkompatibel.

NovaOS muss deshalb mindestens unterscheiden können:

```text
Interface Compatibility
ABI Compatibility
Semantic Compatibility
Contract Compatibility
State Compatibility
Security Compatibility
Trust Compatibility
Determinism Compatibility
Resource Compatibility
```

---

## Adapter

Wenn direkte Kompatibilität nicht gegeben ist, können explizite Adapter verwendet werden.

```text
Old Contract
     ↓
Adapter
     ↓
New Contract
```

Adapter sind Übergangsmechanismen und dürfen semantische Unterschiede nicht verstecken.

---

## Compatibility Layer

Legacy- und Fremdsystemkompatibilität wird über klar abgegrenzte Compatibility Layers bereitgestellt.

Beispiele:

```text
POSIX
Linux
Win32
Legacy NovaABI
```

Diese Schichten dürfen die native NovaOS-Architektur nicht dauerhaft auf das jeweilige Fremdmodell festlegen.

---

## Deprecation

Veraltete Schnittstellen müssen kontrolliert auslaufen können.

Ein möglicher Lebenszyklus lautet:

```text
Active
  ↓
Deprecated
  ↓
Compatibility Only
  ↓
Removed
```

Deprecation soll:

- maschinenlesbar,
- dokumentierbar,
- introspektierbar

sein.

---

## Keine ewige Kompatibilitätsgarantie

NovaOS verpflichtet sich nicht, jede historische Schnittstelle unbegrenzt nativ zu erhalten.

Langfristige Kompatibilität kann durch:

- Adapter,
- Compatibility Provider,
- Virtualisierung,
- Container,
- Migration,
- Konvertierung

bereitgestellt werden.

Dadurch kann die native Architektur weiterentwickelt werden.

---

## NovaABI

NovaABI muss explizit versioniert werden.

Die native ABI soll möglichst klein und stabil gehalten werden.

High-Level-Semantik soll bevorzugt über versionierte Nova APIs, Capabilities und Contracts bereitgestellt werden.

Dadurch wird verhindert, dass historische Low-Level-ABI-Details die gesamte zukünftige Architektur bestimmen.

---

## Kernel-Evolution

Auch der Kernel muss evolvierbar bleiben.

Nicht jede Kernelstruktur wird deshalb als dauerhaft öffentliche ABI behandelt.

Interne Kernelstrukturen dürfen sich verändern.

Stabile Grenzen sollen nur dort geschaffen werden, wo sie tatsächlich benötigt werden.

---

## Driver-Evolution

Treiber sollen über versionierte Contracts mit Kernel, HAL und Device Infrastructure kommunizieren.

Neue Treibermodelle dürfen parallel zu älteren Generationen existieren, wenn dies für Migration erforderlich ist.

Legacy-Treiber können stärker isoliert werden.

---

## Hardware-Evolution

Neue Hardwareklassen sollen primär durch neue:

```text
Providers
Drivers
HAL Components
Execution Targets
Resource Models
```

integriert werden.

Anwendungen sollen dadurch möglichst wenig angepasst werden müssen.

---

## Architektur-Evolution

Evolvierbarkeit betrifft nicht nur Komponenten, sondern auch Architekturgraphen.

Ein System kann beispielsweise von:

```text
A → B → C
```

zu:

```text
A → D → E → C
```

evolvieren, wenn der externe Contract erhalten bleibt.

---

## Transactional Evolution

Komplexe Evolutionsschritte sollen soweit sinnvoll transaktional erfolgen.

```text
Plan
 ↓
Validate
 ↓
Stage
 ↓
Migrate
 ↓
Activate
 ↓
Verify
 ↓
Commit
```

Bei Fehler:

```text
Rollback
```

oder:

```text
Forward Recovery
```

---

## Known-Good State

Vor kritischen Evolutionsschritten soll ein bekannter gültiger Zustand erhalten oder rekonstruierbar sein.

Dies gilt insbesondere für:

- Systemupdates,
- Bootkomponenten,
- Storage-Migrationen,
- Security Policies,
- Kernelkomponenten,
- kritische Services.

---

## Fail-Safe Evolution

Diese ADR integriert `ADR-ARCH-0016_Fail_Safe_Defaults`.

Eine neue Version darf nicht allein deshalb aktiviert werden, weil sie neuer ist.

```text
New
```

ist nicht gleich:

```text
Valid
```

und nicht gleich:

```text
Trusted
```

Bei fehlgeschlagener Validierung muss ein sicherer Zustand erhalten oder hergestellt werden.

---

## Graceful Degradation

Während einer Migration kann eine reduzierte Betriebsart verwendet werden.

Beispiel:

```text
Full Service
    ↓
Migration
    ↓
Read-Only Service
    ↓
New Generation
    ↓
Full Service
```

wenn der Contract dies zulässt.

---

## Security und Trust

Evolution darf keine implizite Privilegienerweiterung erzeugen.

Neue Versionen müssen weiterhin:

- Capability Security,
- Trust Requirements,
- Data Sovereignty,
- Information Flow,
- Isolation Requirements

erfüllen.

Neue Permissions dürfen für bestehende Anwendungen nicht automatisch als gewährt gelten.

---

## Deterministic Mode

Deterministische Ausführungen können konkrete Versionen fixieren.

Beispiel:

```text
Capability Version
Provider Generation
Algorithm Version
Semantic Type Version
```

Ein Update darf eine laufende deterministisch gebundene Ausführung nicht unkontrolliert verändern.

---

## Architecture Introspection

Evolutionszustände müssen introspektierbar sein.

Beispiel:

```text
Component:
    Nova.Image.Provider

Current Generation:
    41

Candidate Generation:
    42

Compatibility:
    Compatible

State:
    Staged
```

Nach einem Wechsel sollen relevante Versions- und Migrationsinformationen nachvollziehbar bleiben.

---

## Provenance

Für kritische Komponenten und Daten soll nachvollziehbar sein:

```text
Original Version
Migration Path
Current Version
Provider Generation
Migration Tool
Validation Result
```

---

## Adaptive Systeme

Adaptive Optimierung darf neue Provider oder Algorithmen bevorzugen, sobald sie validiert und freigegeben wurden.

Adaptive Systeme dürfen jedoch keine inkompatiblen Migrationen selbstständig erfinden.

Fundamentale Evolution muss ohne KI möglich bleiben.

---

## Entwicklungsprinzip

Neue NovaOS-Komponenten sollen nicht nur auf ihre aktuelle Funktion geprüft werden.

Bei Architekturentscheidungen soll zusätzlich gefragt werden:

```text
Can this be replaced?
Can this be versioned?
Can this be migrated?
Can this coexist with another version?
Can compatibility be determined?
Can old state be recovered?
```

---

## Normative Anforderungen

1. NovaOS MUSS Evolvierbarkeit als systemweites Architekturprinzip behandeln.
2. Stabile Semantik SOLL von konkreter Implementierung getrennt werden.
3. Relevante Systemcontracts MÜSSEN versionierbar sein.
4. Komponenten SOLLEN unabhängig versionierbar sein.
5. Capability-Identität SOLL von Provider-Versionen unabhängig bleiben.
6. Neue Provider SOLLEN ohne Änderung der Capability-Semantik integrierbar sein.
7. Neue Algorithmen SOLLEN hinter bestehenden Capabilities integrierbar sein.
8. Semantic Types MÜSSEN kontrolliert evolvieren können.
9. Persistente Schemas MÜSSEN Versionierung unterstützen.
10. IPC-Schemas MÜSSEN Versionierung unterstützen.
11. Schema-Kompatibilität MUSS explizit bestimmbar sein.
12. State Migration MUSS für relevante stateful Komponenten möglich sein.
13. Migration MUSS validierbar sein.
14. Fehlgeschlagene Migration DARF den letzten gültigen Zustand NICHT unnötig zerstören.
15. Hot Replacement DARF als Evolutionsmechanismus verwendet werden.
16. Evolution MUSS NICHT zwingend live erfolgen.
17. Restart-, Reboot- und Offline-Migration MÜSSEN zulässige Evolutionsformen bleiben.
18. Explizite Contracts MÜSSEN kontrollierte Substitution ermöglichen.
19. Kompatibilität MUSS über reine ABI-Kompatibilität hinausgehen.
20. Semantic Compatibility MUSS berücksichtigt werden.
21. Security Compatibility MUSS berücksichtigt werden.
22. Trust Compatibility MUSS berücksichtigt werden.
23. State Compatibility MUSS berücksichtigt werden.
24. Adapter DÜRFEN für kontrollierte Evolution eingesetzt werden.
25. Adapter DÜRFEN semantische Inkompatibilität NICHT verschleiern.
26. Compatibility Layers DÜRFEN die native Architektur NICHT bestimmen.
27. Deprecation MUSS kontrolliert möglich sein.
28. Veraltete Interfaces MÜSSEN nicht unbegrenzt Teil der nativen Architektur bleiben.
29. NovaABI MUSS versionierbar sein.
30. Interne Kernelstrukturen MÜSSEN NICHT automatisch öffentliche stabile ABI sein.
31. Driver Contracts MÜSSEN evolvierbar sein.
32. Neue Hardware SOLL primär über Provider, Driver und Execution Targets integrierbar sein.
33. Architekturgraphen MÜSSEN evolvieren können.
34. Kritische Evolutionsschritte SOLLEN transaktional durchgeführt werden.
35. Known-Good States SOLLEN für kritische Änderungen verfügbar sein.
36. Eine neue Version DARF NICHT allein aufgrund ihrer Neuheit aktiviert werden.
37. Updates MÜSSEN Trust- und Security-Prüfungen erfüllen.
38. Neue Permissions DÜRFEN NICHT automatisch bestehenden Anwendungen gewährt werden.
39. Graceful Degradation DARF während Migrationen verwendet werden.
40. Deterministic Execution MUSS relevante Versionen fixieren können.
41. Laufende deterministische Ausführungen DÜRFEN NICHT unkontrolliert auf neue Generationen wechseln.
42. Evolutionszustände MÜSSEN introspektierbar sein.
43. Relevante Migrationen SOLLEN Provenance besitzen.
44. Adaptive Systeme DÜRFEN nur validierte Evolutionsoptionen verwenden.
45. Fundamentale Evolutionsmechanismen MÜSSEN ohne KI funktionieren.
46. Komponenten SOLLEN auf Replaceability geprüft werden.
47. Komponenten SOLLEN auf Versionability geprüft werden.
48. Stateful Komponenten SOLLEN auf Migratability geprüft werden.
49. Mehrere Generationen DÜRFEN während Übergangsphasen koexistieren.
50. NovaOS MUSS Architekturänderungen kontrolliert ermöglichen, ohne Stabilität mit Stillstand gleichzusetzen.

---

## Konsequenzen

### Positive Konsequenzen

- NovaOS kann langfristig neue Hardware integrieren.
- einzelne Subsysteme können unabhängig weiterentwickelt werden.
- Anwendungen hängen weniger von Implementierungsdetails ab.
- Provider und Algorithmen können ersetzt werden.
- Hot Replacement wird erleichtert.
- persistente Daten können kontrolliert migriert werden.
- Compatibility Layers verhindern, dass Legacy-Modelle die native Architektur dominieren.
- Sicherheitsmodelle können weiterentwickelt werden.
- technische Schulden durch ewige ABI-Bindung werden reduziert.
- größere Architekturänderungen werden planbarer.

### Negative Konsequenzen

- Versionierungs- und Migrationslogik erhöht die Komplexität.
- Contract-Kompatibilität muss systematisch geprüft werden.
- mehrere Generationen können zeitweise parallel existieren.
- Adapter und Compatibility Layers benötigen Wartung.
- State Migration kann teuer und fehleranfällig sein.
- Deprecation benötigt langfristige Planung.
- Tests müssen mehrere Versionen und Migrationspfade berücksichtigen.

---

## Verworfene Alternativen

### Vollständig unveränderliche Systemarchitektur

Verworfen.

Langfristig würde dies neue Hardware, Sicherheitsmodelle und Ausführungsformen unnötig behindern.

### Vollständige Abwärtskompatibilität für immer

Verworfen.

Historische Entscheidungen würden die native Architektur dauerhaft einschränken.

### Breaking Changes ohne Migrationsmodell

Verworfen.

Dies würde Systemupdates, Anwendungen und persistente Daten unnötig gefährden.

### Eine globale Systemversion für alle Komponenten

Verworfen.

Subsysteme besitzen unterschiedliche Evolutionsgeschwindigkeiten.

### ABI als einzige Stabilitätsgrenze

Verworfen.

Semantische Contracts sind für NovaOS wichtiger als bloße Binärkompatibilität.

### KI-gesteuerte automatische Architekturmigration

Als fundamentaler Mechanismus verworfen.

KI darf Migration unterstützen, aber sichere Evolution muss deterministisch und ohne KI möglich sein.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage`
- `ADR-ARCH-0004_Transaktionale_Systemoperationen`
- `ADR-ARCH-0008_Semantic_Types_als_systemweites_Typsystem`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0012_Hot_Replacement_und_Live_Evolution`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts_zwischen_Systemkomponenten`
- `ADR-ARCH-0015_Graceful_Degradation_als_Systemprinzip`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0017_Composability_und_Orthogonalität`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`
- `ADR-EXECUTION-0001_Nova_ExecutionContract`

---

## Zugehörige NPSPECs

- `NPSPEC-EVOLUTION-MODEL-0001`
- `NPSPEC-EVOLUTION-VERSIONING-0001`
- `NPSPEC-EVOLUTION-COMPATIBILITY-0001`
- `NPSPEC-EVOLUTION-MIGRATION-0001`
- `NPSPEC-EVOLUTION-STATE-MIGRATION-0001`
- `NPSPEC-EVOLUTION-DEPRECATION-0001`
- `NPSPEC-EVOLUTION-ADAPTER-0001`
- `NPSPEC-EVOLUTION-KNOWNGOOD-0001`
- `NPSPEC-EVOLUTION-TRANSACTION-0001`
- `NPSPEC-EVOLUTION-INTROSPECTION-0001`
- `NPSPEC-CAPABILITY-EVOLUTION-0001`
- `NPSPEC-CONTRACT-EVOLUTION-0001`
- `NPSPEC-SCHEMA-EVOLUTION-0001`
- `NPSPEC-SEMANTICTYPE-EVOLUTION-0001`
- `NPSPEC-ABI-VERSIONING-0001`
- `NPSPEC-DRIVER-VERSIONING-0001`
- `NPSPEC-COMPATIBILITY-ADAPTER-0001`
- `NPSPEC-COMPATIBILITY-DEPRECATION-0001`
- `NPSPEC-HOTREPLACE-GENERATION-0001`
- `NPSPEC-HOTREPLACE-STATEMIGRATION-0001`

---

## Verifikation

Die Architektur muss insbesondere folgende Szenarien testen:

- Upgrade einer kompatiblen Provider-Version,
- inkompatiblen Provider ablehnen,
- Schema-Migration alter persistenter Daten,
- fehlgeschlagene Migration mit Erhalt des Known-Good State,
- Hot Replacement zwischen kompatiblen Generationen,
- State Migration bei stateful Komponenten,
- parallele Koexistenz mehrerer Generationen,
- Contract Negotiation zwischen unterschiedlichen Versionen,
- Adapter zwischen alten und neuen Contracts,
- Deprecation alter Schnittstellen,
- Rollback nach fehlgeschlagener Aktivierung,
- Anti-Rollback bei sicherheitskritischen Versionen,
- deterministisch gepinnte Ausführung während eines Updates,
- Migration ohne Netzwerk-, Cloud- oder KI-Abhängigkeit.

---

## Ergebnis

NovaOS betrachtet seine Architektur nicht als statischen Endzustand.

Stattdessen gilt:

```text
Stable Semantics
      │
      ▼
Explicit Versioned Contracts
      │
      ▼
Replaceable Components
      │
      ▼
Migratable State
      │
      ▼
Transactional Evolution
      │
      ▼
Verification
      │
      ▼
New Known-Good State
```

Damit soll NovaOS über lange Zeiträume weiterentwickelt werden können, ohne entweder:

```text
everything must remain forever
```

oder:

```text
everything may break at any time
```

zu wählen.

Die Architekturregel lautet:

```text
Stable where stability creates value.

Evolvable where change creates value.

Compatible where practical.

Migratable where necessary.

Replaceable by design.
```