# ADR-UPDATE-0009 – Capability-Based Dependency Resolution

## Status

Angenommen

## Kategorie

Update Architecture / Dependency Resolution / Capabilities

## Kontext

Klassische Paketmanager modellieren Abhängigkeiten häufig als direkte Beziehungen zwischen konkreten Paketen:

```text
Application A
    ↓
Package B
    ↓
Package C
```

Dadurch wird ein Consumer unnötig an eine bestimmte Implementierung gebunden.

Für NovaOS ist jedoch entscheidend, **welche Fähigkeit benötigt wird**, nicht welches konkrete Paket diese Fähigkeit bereitstellt.

Beispiel:

```text
Application
    ↓
needs:
image.decode.jpeg
```

Diese Capability könnte von unterschiedlichen Providern bereitgestellt werden:

```text
Nova.Image
Third-Party Codec
Hardware Codec
Remote Provider
```

NovaOS benötigt deshalb eine Dependency Resolution, die primär auf deklarativen Capabilities und Contracts statt auf festen Paketnamen basiert.

## Entscheidung

NovaOS verwendet **Capability-Based Dependency Resolution**.

Consumer deklarieren benötigte Fähigkeiten:

```text
Consumer
   ↓
Capability Requirement
   ↓
Resolver
   ↓
Compatible Provider
```

Der Resolver bestimmt anhand von Capability Contract, Version, Trust, Policy, Ressourcen und Execution Contract einen geeigneten konkreten Provider.

## Grundprinzip

```text
Depend on capability,
not implementation.
```

Ein Consumer soll grundsätzlich beschreiben:

```text
What is required?
```

und nicht:

```text
Which package must provide it?
```

## Capability Requirement

Eine Abhängigkeit wird als Capability Requirement beschrieben.

Beispiel:

```text
requires:
    capability: image.decode.jpeg
```

Zusätzliche Anforderungen können definiert werden:

```text
Version
Semantic Types
Quality
Latency
Determinism
Trust
Security
Data Sovereignty
Hardware Requirements
```

## Provider

Ein Provider deklariert die von ihm angebotenen Capabilities.

```text
Provider A
 ├── image.decode.jpeg
 ├── image.decode.png
 └── image.resize
```

Mehrere Provider können dieselbe Capability anbieten.

```text
image.decode.jpeg
      ↑
 ┌────┼────┐
 ↓    ↓    ↓
P1   P2   P3
```

## Resolver

Der Resolver ermittelt zunächst alle grundsätzlich geeigneten Provider.

```text
Capability Requirement
        ↓
Provider Discovery
        ↓
Candidate Providers
        ↓
Contract Filtering
        ↓
Policy Filtering
        ↓
Selection
```

Die Auswahl erfolgt nicht ausschließlich anhand einer Versionsnummer.

## Hard Constraints

Nicht verhandelbare Anforderungen werden als Hard Constraints behandelt.

Beispiele:

```text
Capability Contract
Required Semantic Types
Minimum Trust
Required Determinism
Data Sovereignty
Security Policy
Hardware Compatibility
ABI/API Compatibility
```

Ein Provider, der einen Hard Constraint nicht erfüllt, darf nicht ausgewählt werden.

## Soft Constraints

Nach Erfüllung aller Hard Constraints kann zwischen mehreren gültigen Providern optimiert werden.

Beispiele:

```text
Performance
Energy Consumption
Memory Usage
Latency
Locality
Cache Availability
Preferred Provider
```

Diese Kriterien dürfen keine Hard Constraints überschreiben.

## Execution Contract

Capability Resolution muss mit dem systemweiten Execution Contract integrierbar sein.

Beispiel:

```text
Capability:
    audio.noise_cancel

Execution Contract:
    latency <= 10 ms
    deterministic = required
    locality = local
```

Der Resolver darf nur Provider auswählen, die diesen Contract erfüllen können.

## Semantic Types

Capability Requirements können Semantic Types verwenden.

```text
Input:
    Audio.PCM.Stereo

Output:
    Audio.PCM.Stereo
```

Dadurch kann NovaOS prüfen, ob Provider semantisch kompatible Ein- und Ausgaben besitzen.

Ein gleichartiger Byte-Buffer allein reicht nicht als Kompatibilitätsnachweis.

## Provider-Versionen

Ein Provider kann in mehreren Versionen parallel vorhanden sein.

```text
Provider X
 ├── V1
 ├── V2
 └── V3
```

Der Resolver kann dadurch Side-by-Side-Versionen berücksichtigen.

## Content ID

Nach erfolgreicher Resolution wird der konkrete Provider auf ein unveränderliches Artefakt aufgelöst.

```text
Capability
   ↓
Provider
   ↓
Version
   ↓
Content ID
```

Der resultierende Software- oder Execution-State kann dadurch exakt reproduziert werden.

## Direkte Paketabhängigkeiten

Direkte Paketabhängigkeiten bleiben für Fälle möglich, in denen tatsächlich eine konkrete Implementierung benötigt wird.

```text
requires:
    exact-content: H42
```

Dies ist jedoch nicht der Standard für funktionale Abhängigkeiten.

Der bevorzugte Mechanismus lautet:

```text
Capability Requirement
```

## Provider-Präferenz

Ein Consumer oder eine Policy kann eine Provider-Präferenz angeben.

```text
Preferred Provider = Nova.Image
```

Diese Präferenz ist ein Soft Constraint, sofern keine explizite Forced-Provider-Regel besteht.

## Forced Provider

Für spezielle Anforderungen kann ein bestimmter Provider erzwungen werden.

```text
Forced Provider
      ↓
Contract Validation
      ↓
Use or Fail
```

Auch ein erzwungener Provider darf zwingende Security- oder System-Hard-Constraints nicht umgehen.

## Hardware Provider

Capabilities können durch Hardware oder Hardware-nahe Provider bereitgestellt werden.

```text
video.decode.h265
      ↓
 ┌────┴────┐
 ↓         ↓
CPU       GPU
Provider  Provider
```

Der Resolver kann abhängig von Hardware, Energiebedarf und Execution Contract einen geeigneten Provider auswählen.

## Lokale und entfernte Provider

Eine Capability kann grundsätzlich lokal oder remote bereitgestellt werden, sofern der Contract dies erlaubt.

```text
Capability
   ↓
 ┌─┴─────────┐
 ↓           ↓
Local      Remote
Provider   Provider
```

Data-Sovereignty-, Privacy-, Security- und Latency-Regeln müssen dabei berücksichtigt werden.

Ein Remote Provider darf nicht ausgewählt werden, wenn der Contract lokale Verarbeitung verlangt.

## Fallback

Für eine Capability können mehrere kompatible Provider existieren.

```text
Primary Provider
      ↓
Failure
      ↓
Compatible Alternative
```

Ein Fallback darf jedoch nur erfolgen, wenn der alternative Provider denselben erforderlichen Contract erfüllt.

Fallback ist keine Berechtigung zur stillen Abschwächung von Anforderungen.

## Graceful Degradation

Wenn kein Provider den vollständigen optionalen Funktionsumfang bereitstellt, kann NovaOS einen reduzierten Provider verwenden, sofern der Consumer dies explizit erlaubt.

```text
Preferred Capability
       ↓
Unavailable
       ↓
Allowed Degraded Capability
```

Hard Constraints dürfen dabei nicht abgeschwächt werden.

## Installation

Fehlt ein geeigneter Provider lokal, kann der Resolver feststellen, ob ein vertrauenswürdiger Provider verfügbar gemacht werden kann.

```text
Capability Requirement
        ↓
No local provider
        ↓
Provider Catalog
        ↓
Candidate Package
        ↓
Trust + Policy Check
        ↓
Acquire
```

Die tatsächliche Bereitstellung erfolgt über die Transactional Update Pipeline.

## Systemintegrierte Capabilities

NovaOS kann grundlegende Capabilities bereits als Bestandteil des Systems bereitstellen.

Beispiele:

```text
Nova.Math
Nova.Compute
Nova.Graphics
Nova.Crypto
Nova.Media
Nova.Simulation
```

Consumer verwenden dennoch Capability Contracts und sollen nicht unnötig an interne Implementierungsdetails gekoppelt werden.

## Update

Ein Provider-Update kann eine neue Version zusätzlich bereitstellen.

```text
Provider V1
     ↓
Install V2
     ↓
V1 + V2
```

Neue Resolutionen können anschließend V2 auswählen, während bestehende Bindungen V1 weiterverwenden können.

## Provider-Wechsel

Ein bestehender Provider kann kontrolliert durch einen kompatiblen Provider ersetzt werden.

```text
Capability
   ↓
Provider A
   ↓
Prepare Provider B
   ↓
Validate Contract
   ↓
Atomic Switch
   ↓
Provider B
```

Der Wechsel muss die geltenden Capability- und Execution-Contracts erhalten.

## Reproduzierbarkeit

Dynamische Capability Resolution und reproduzierbare Ausführung werden getrennt behandelt.

Vor der Resolution:

```text
Need Capability X
```

Nach der Resolution:

```text
Capability X
    ↓
Provider P
Version V
Content ID H
```

Dieser konkrete Resolution State kann gespeichert werden.

Dadurch kann NovaOS später denselben Providerzustand reproduzieren.

## Dependency Graph

Der aufgelöste Dependency Graph enthält konkrete Providerbindungen.

```text
Application
 ├── Capability A
 │      └── Provider P1 → H10
 │
 └── Capability B
        └── Provider P7 → H21
```

Damit bleiben abstrakte Anforderungen und konkrete Ausführung getrennt.

## Konflikte

Providerkonflikte müssen explizit erkannt werden.

Beispiele:

```text
incompatible ABI
exclusive hardware ownership
conflicting security policy
resource conflict
version conflict
```

Der Resolver darf keinen scheinbar gültigen Graph erzeugen, dessen Provider gemeinsam nicht ausführbar sind.

## Resolution Scope

Die Auflösung kann abhängig vom Anwendungsfall erfolgen für:

```text
System
User Session
Application
Process
Execution
Transaction
```

Der Scope bestimmt, wie lange eine konkrete Providerbindung gültig bleibt.

## Stabilität

Eine bereits aufgelöste Bindung darf nicht bei jeder Ausführung willkürlich verändert werden.

Providerwechsel müssen kontrolliert und nachvollziehbar erfolgen.

Dies verhindert:

```text
Execution 1 → Provider A
Execution 2 → Provider B
Execution 3 → Provider C
```

ohne erklärbaren Grund.

## Security

Ein Provider muss vor Auswahl die erforderlichen Security-Anforderungen erfüllen.

Dazu gehören abhängig vom Contract:

```text
Authorization
Integrity
Trust
Provenance
Isolation
Permissions
Data Classification
```

Capability-Kompatibilität allein reicht nicht für eine Auswahl.

## Trust

Der Resolver muss Trust als eigene Entscheidungsdimension behandeln.

```text
Provider supports Capability
          ≠
Provider is trusted
```

Eine gültige Capability-Implementierung kann aufgrund fehlender Trust-Evidenz ausgeschlossen werden.

## Provenance

Die Provenance eines Providers muss in die Resolution einfließen können.

```text
Provider
   ↓
Content ID
   ↓
Provenance
   ↓
Trust Decision
```

Dadurch können beispielsweise Organisations- oder Systemrichtlinien bestimmte Herkunftsquellen verlangen oder ausschließen.

## Data Sovereignty

Capability Resolution muss Datenhoheit berücksichtigen.

Beispiel:

```text
Data:
    locality = EU
    remote_processing = forbidden
```

Ein technisch kompatibler Cloud Provider wäre damit kein gültiger Candidate.

## Resource Budgets

Provider können anhand verfügbarer Ressourcen gefiltert werden.

```text
Memory Budget
CPU Budget
GPU Budget
Energy Budget
Storage Budget
```

Ein Provider, dessen Mindestanforderungen nicht erfüllt werden können, darf nicht ausgewählt werden.

## Deterministic Mode

Wenn ein Execution Contract deterministische Ausführung verlangt, dürfen nur entsprechend geeignete Provider ausgewählt werden.

```text
Determinism = Required
       ↓
Filter Providers
       ↓
Deterministic Providers only
```

## Offline-Betrieb

NovaOS muss Capability Resolution auch ohne Netzwerk durchführen können.

```text
Capability Requirement
        ↓
Local Provider Index
        ↓
Resolution
```

Fehlt offline ein benötigter Provider, muss dies explizit gemeldet werden.

Ein Netzwerkzugriff darf nicht stillschweigend vorausgesetzt werden.

## Caching

Resolution-Ergebnisse können gecacht werden.

Der Cache muss jedoch invalidiert beziehungsweise neu bewertet werden, wenn sich relevante Bedingungen ändern.

Beispiele:

```text
Provider Revoked
Policy Changed
Hardware Changed
Contract Changed
Provider Removed
```

## Revocation

Wird ein Provider widerrufen, darf er für neue Resolutionen nicht mehr ausgewählt werden.

```text
Provider P
   ↓
Revoked
   ↓
Remove from Candidate Set
```

Bestehende Bindungen werden entsprechend Security- und Recovery-Policy behandelt.

## Explainability

NovaOS muss erklären können, warum ein Provider ausgewählt oder abgelehnt wurde.

Beispiel:

```text
Selected Provider B

Reason:
- Capability matched
- Trust requirement satisfied
- deterministic
- local execution
- lower resource cost
```

oder:

```text
Provider A rejected:
Data Sovereignty violation
```

Dies ist insbesondere für Debugging, Security und adaptive Systementscheidungen erforderlich.

## Garbage Collection

Ein Provider darf nicht entfernt werden, solange er noch benötigt wird durch:

```text
Active Binding
Running Process
Desired State
Snapshot
Rollback
A/B System Base
Recovery
Pinned Resolution
```

Capability Resolution und Package Garbage Collection müssen deshalb integriert sein.

## Recovery

Recovery muss frühere Resolution States rekonstruieren können.

```text
Recovery State
      ↓
Capability Bindings
      ↓
Provider Content IDs
      ↓
Restore
```

Dadurch kann ein bekannter Softwarezustand mit denselben Providerbindungen wiederhergestellt werden.

## NovaDOS

NovaDOS muss für kritische System-Capabilities die zugehörigen Providerbindungen bestimmen können.

Insbesondere müssen Known-Good- und Recovery-Zustände ohne laufenden normalen Resolver rekonstruierbar bleiben.

## Normative Anforderungen

1. NovaOS MUSS funktionale Softwareabhängigkeiten als Capability Requirements ausdrücken können.
2. Consumer SOLLEN von Capabilities statt konkreten Paketimplementierungen abhängen.
3. Mehrere Provider MÜSSEN dieselbe Capability bereitstellen können.
4. Der Resolver MUSS Capability Requirements auf konkrete Provider auflösen können.
5. Hard Constraints MÜSSEN vor Provider-Auswahl vollständig erfüllt sein.
6. Soft Constraints DÜRFEN erst nach Erfüllung aller Hard Constraints zur Optimierung verwendet werden.
7. Capability Resolution MUSS Execution Contracts berücksichtigen können.
8. Semantic Types MÜSSEN in Capability Contracts verwendbar sein.
9. Side-by-Side-Versionen von Providern MÜSSEN berücksichtigt werden können.
10. Ein aufgelöster Provider MUSS auf eine konkrete Content ID bindbar sein.
11. Direkte Paket- oder Content-Abhängigkeiten MÜSSEN für ausdrücklich implementierungsspezifische Fälle möglich bleiben.
12. Provider-Präferenzen DÜRFEN Hard Constraints nicht überschreiben.
13. Forced Provider DÜRFEN zwingende Security- und Systemregeln nicht umgehen.
14. Hardware Provider MÜSSEN als Capability Provider behandelbar sein.
15. Lokale und Remote Provider MÜSSEN durch denselben abstrakten Capability-Mechanismus behandelbar sein können.
16. Remote Provider DÜRFEN nur ausgewählt werden, wenn Privacy-, Security-, Latency- und Data-Sovereignty-Contracts dies erlauben.
17. Fallback Provider MÜSSEN die erforderlichen Hard Constraints weiterhin erfüllen.
18. Graceful Degradation DARF nur erfolgen, wenn der Consumer dies ausdrücklich erlaubt.
19. Fehlende Provider MÜSSEN über die Transactional Update Pipeline bereitstellbar sein können.
20. Providerwechsel MÜSSEN kontrolliert und atomar erfolgen können.
21. Resolution States MÜSSEN für reproduzierbare Ausführung konkret speicherbar sein.
22. Der aufgelöste Dependency Graph MUSS konkrete Provider- und Content-ID-Bindungen enthalten können.
23. Providerkonflikte MÜSSEN vor Aktivierung erkannt werden.
24. Resolution Scopes MÜSSEN explizit definierbar sein.
25. Bestehende Bindungen DÜRFEN nicht ohne kontrollierten Grund willkürlich verändert werden.
26. Capability-Kompatibilität DARF Trust-, Security- oder Provenance-Prüfungen nicht ersetzen.
27. Data-Sovereignty-Anforderungen MÜSSEN als Hard Constraints behandelbar sein.
28. Resource Budgets MÜSSEN bei der Provider-Auswahl berücksichtigt werden können.
29. Deterministic Mode MUSS ungeeignete Provider ausschließen können.
30. Capability Resolution MUSS für lokal verfügbare Provider offline funktionieren können.
31. Revoked Provider DÜRFEN nicht für neue Resolutionen ausgewählt werden.
32. NovaOS MUSS die Auswahl oder Ablehnung eines Providers erklärbar machen können.
33. Garbage Collection DARF keine noch referenzierten Provider entfernen.
34. Recovery MUSS konkrete frühere Providerbindungen rekonstruieren können.
35. Kritische Recovery-Bindungen MÜSSEN durch NovaDOS offline bestimmbar sein.

## Konsequenzen

### Positive Konsequenzen

- Anwendungen werden von konkreten Implementierungen entkoppelt,
- Provider können ausgetauscht werden,
- Hardware- und Softwareimplementierungen können dieselbe Capability bereitstellen,
- Side-by-Side-Versionen lassen sich sauber integrieren,
- NovaOS kann Provider anhand von Performance, Energie und Ressourcen optimieren,
- Security, Trust und Data Sovereignty werden Bestandteil der Dependency Resolution,
- reproduzierbare Ausführung bleibt durch konkrete Resolution States möglich.

### Negative Konsequenzen

- der Resolver wird komplexer als ein klassischer Paketabhängigkeitsresolver,
- Capability Contracts müssen präzise definiert werden,
- Provider benötigen umfangreiche Metadaten,
- dynamische Auswahl und reproduzierbare Ausführung müssen sauber getrennt werden,
- Konfliktauflösung über mehrere Provider kann aufwendig werden.

## Verworfene Alternativen

### Direkte Paketabhängigkeiten als Standard

Verworfen, da Consumer dadurch unnötig an konkrete Implementierungen gekoppelt würden.

### Auswahl ausschließlich anhand der Versionsnummer

Verworfen, da Version allein weder Capability-Kompatibilität noch Trust oder Execution Requirements beschreibt.

### Ein globaler Provider pro Capability

Verworfen, da unterschiedliche Consumer unterschiedliche Contracts und Versionen benötigen können.

### Automatisch immer schnellsten Provider wählen

Verworfen, da Performance nur ein Soft Constraint unter mehreren ist und Security, Determinism oder Data Sovereignty Vorrang besitzen können.

### Dynamische Auswahl ohne gespeicherten Resolution State

Verworfen, da reproduzierbare Ausführung und zuverlässiger Rollback dadurch erschwert würden.

## Abhängigkeiten

- `ADR-UPDATE-0001_Deklarativer_Softwarezustand`
- `ADR-UPDATE-0002_Transactional_Update_Pipeline`
- `ADR-UPDATE-0003_Atomic_Updates`
- `ADR-UPDATE-0006_Content_Addressed_Packages`
- `ADR-UPDATE-0007_Immutable_System_Basis`
- `ADR-UPDATE-0008_Side_By_Side_Versionen`
- `ADR-CAPABILITY-0001_Capability_Based_Systemmodell`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0020_KI_nicht_Kernel_Voraussetzung`
- `ADR-TRUST-0003_Signatur_ist_nicht_Vertrauen`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-CAPABILITY-RESOLVER-0001`
- `NPSPEC-UPDATE-CAPABILITY-REQUIREMENT-0001`
- `NPSPEC-UPDATE-PROVIDER-MANIFEST-0001`
- `NPSPEC-UPDATE-PROVIDER-SELECTION-0001`
- `NPSPEC-UPDATE-PROVIDER-CONSTRAINTS-0001`
- `NPSPEC-UPDATE-RESOLUTION-STATE-0001`
- `NPSPEC-UPDATE-DEPENDENCY-GRAPH-0001`
- `NPSPEC-UPDATE-PROVIDER-FALLBACK-0001`
- `NPSPEC-UPDATE-PROVIDER-REVOCATION-0001`
- `NPSPEC-UPDATE-CAPABILITY-RESOLVER-TEST-0001`

## Ergebnis

NovaOS löst funktionale Abhängigkeiten nicht primär über Paketnamen, sondern über benötigte Fähigkeiten auf:

```text
Consumer
   ↓
Capability Requirement
   ↓
Hard Constraints
   ↓
Candidate Providers
   ↓
Policy + Optimization
   ↓
Selected Provider
   ↓
Version
   ↓
Content ID
```

Damit bleibt der Consumer von der konkreten Implementierung entkoppelt, während der tatsächlich verwendete Provider weiterhin eindeutig und reproduzierbar festgehalten werden kann.

Der zentrale Grundsatz lautet:

```text
Software fordert eine Fähigkeit an.

NovaOS entscheidet,
welcher gültige Provider
diese Fähigkeit bereitstellt.
```