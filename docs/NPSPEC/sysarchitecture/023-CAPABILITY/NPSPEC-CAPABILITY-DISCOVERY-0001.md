# NPSPEC-CAPABILITY-DISCOVERY-0001 – Nova Capability Discovery

## Status

Angenommen

## Kategorie

Capability / Discovery / Security

## Zweck

NovaOS definiert Capability Discovery als kontrollierten Mechanismus, mit dem Komponenten verfügbare Fähigkeiten und Dienste finden können, ohne dadurch automatisch Zugriff oder Autorität zu erhalten.

```text
Discovery Request
       ↓
Capability Discovery
       ↓
Filtered Candidates
       ↓
Authority Acquisition
       ↓
Capability
```

Discovery und Authority Acquisition bleiben strikt getrennt.

## Grundprinzipien

```text
Discoverable ≠ Accessible
Visible ≠ Authorized
Service Found ≠ Capability Granted
ObjectID Known ≠ Object Accessible
CapabilityID Known ≠ Capability Possessed
Discovery ≠ Enumeration Permission
Discovery ≠ Ambient Authority
```

## Discovery-Modell

Eine Discovery-Anfrage besitzt mindestens:

```text
CapabilityDiscoveryRequest
├── Requester Identity
├── Requested Capability Type
├── Purpose
└── Security Context
```

Optional:

```text
Semantic Type
Target Type
Required Interface
Trust Requirement
Sovereignty Requirement
Location Preference
Resource Requirements
ExecutionContract
```

Das Ergebnis enthält nur für den Requester sichtbare Kandidaten.

## Semantische Discovery

NovaOS soll Capabilities nach ihrer Bedeutung statt ausschließlich nach Namen oder Pfaden finden können.

```text
"Image Decoder"
"Network Connection"
"Document Renderer"
"GPU Compute"
"Secure Storage"
```

Beispiel:

```text
Requested Capability
    ↓
Semantic Requirements
    ↓
Compatible Providers
    ↓
Policy Filtering
    ↓
Candidate Set
```

Dadurch bleibt der Aufrufer von konkreten Implementierungen entkoppelt.

## Discovery und Authority

Discovery darf keine Capability automatisch übertragen.

```text
Discover
   ↓
Candidate
   ↓
Policy / Trust / Authority Check
   ↓
Acquire Capability
```

Die tatsächliche Autorität entsteht erst durch einen kontrollierten Capability-Acquisition- oder Delegationsvorgang.

## Filterung

Discovery-Ergebnisse müssen anhand des Requester-Kontexts filterbar sein.

Berücksichtigt werden können:

```text
Identity
Existing Capabilities
Security Domain
Trust State
Privacy Policy
Sovereignty Policy
ExecutionContract
System Policy
```

Nicht sichtbare Ressourcen dürfen nicht durch Discovery als Seiteneffekt vollständig enumerierbar werden.

## Provider

Mehrere Provider können dieselbe semantische Capability anbieten.

```text
Capability Request
      ↓
Provider A
Provider B
Provider C
      ↓
Policy Selection
```

Provider können lokal, remote oder hardwarebasiert sein.

```text
Capability Semantics ≠ Provider Identity
```

## Location Transparency

Discovery soll physische Position nicht unnötig offenlegen.

```text
Requested Capability
       ↓
Discovery
       ↓
Local / Remote / Migrated Provider
```

Location Transparency darf jedoch Trust-, Security- oder Sovereignty-Regeln nicht umgehen.

## Trust

Discovery kann Trust-Anforderungen berücksichtigen.

```text
Required Trust
      ↓
Provider Trust Evaluation
      ↓
Eligible Candidates
```

Ein gefundener vertrauenswürdiger Provider erhält dadurch keine zusätzliche Autorität gegenüber dem Requester.

## Capability Acquisition

Nach Auswahl eines Kandidaten erfolgt die eigentliche Autoritätsvergabe.

```text
Candidate
   ↓
Acquisition Request
   ↓
Policy Validation
   ↓
Capability Creation / Delegation
   ↓
Handle / Token
```

Dabei gelten Least Privilege und Attenuation.

## Dynamische Umgebung

Discovery muss Änderungen der verfügbaren Capabilities unterstützen.

```text
Provider Added
Provider Removed
Provider Migrated
Trust Changed
Capability Revoked
```

Bestehende Capabilities bleiben davon getrennt und unterliegen ihrem eigenen Lebenszyklus.

## Introspection

Autorisierte Komponenten sollen mindestens abfragen können:

```text
Capability Type
Semantic Interface
Provider Identity
Availability
Trust State
Location Class
Acquisition Requirements
```

Sensible Details dürfen abhängig von Security- und Privacy-Policy verborgen bleiben.

## Normative Anforderungen

1. NovaOS MUSS Discovery und Authority Acquisition strikt trennen.
2. Discovery DARF NICHT automatisch eine Capability erzeugen.
3. Discovery-Ergebnisse MÜSSEN abhängig vom Security Context filterbar sein.
4. Kenntnis einer Capability oder eines Targets DARF keine Autorität erzeugen.
5. Semantische Capability Discovery SOLL unterstützt werden.
6. Mehrere Provider MÜSSEN dieselbe Capability-Semantik anbieten können.
7. Discovery MUSS lokale und entfernte Provider unterstützen können.
8. Trust-, Security- und Sovereignty-Regeln MÜSSEN bei Discovery berücksichtigt werden können.
9. Discovery DARF keine unautorisierte vollständige Ressourcen-Enumeration ermöglichen.
10. Capability Acquisition MUSS weiterhin über explizite kontrollierte Autoritätsvergabe erfolgen.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-TOKEN-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-DELEGATION-0001`
- `NPSPEC-IDENTITY-0001`
- `NPSPEC-TRUST-POLICY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-DISTCOMM-REMOTECAPABILITY-0001`
- `ADR-CAPABILITY-0004`

## Ergebnis

```text
Semantic Requirement
        ↓
Controlled Discovery
        ↓
Policy-filtered Candidates
        ↓
Explicit Acquisition
        ↓
Capability
        ↓
Handle / Token
```

NovaOS erhält damit eine sichere Capability-Discovery-Schicht, über die Komponenten benötigte Fähigkeiten dynamisch finden können, ohne Sichtbarkeit, Auffindbarkeit oder Provider-Auswahl mit tatsächlicher Autorität gleichzusetzen.