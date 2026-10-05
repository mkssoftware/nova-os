# NPSPEC-CAPABILITY-DISCOVERY-0001 – Nova Capability Discovery

## Status

Angenommen

## Kategorie

Capability / Discovery

## Zweck

NovaOS definiert die systemweite Auffindbarkeit verfügbarer Capabilities, Provider und Implementierungen.

Discovery beantwortet, welche Capabilities grundsätzlich vorhanden und für einen Kontext sichtbar sind. Sie vergibt keine Berechtigungen und entscheidet noch nicht über die konkrete Ausführung.

## Grundprinzipien

```text
Discovery ≠ Authority
Discovery ≠ Permission
Discovery ≠ Resolution
Discovery ≠ Execution
Visible Capability ≠ Usable Capability
Registered Provider ≠ Selected Provider
```

## Discovery-Modell

Eine Discovery-Anfrage kann enthalten:

```text
CapabilityDiscovery
├── CapabilityID
├── Namespace
├── Category
├── SemanticInput
├── SemanticOutput
├── Operation
├── VersionConstraint
├── Compatibility
└── Context
```

Das Ergebnis kann enthalten:

```text
CapabilityID
CapabilityVersion
InterfaceVersion
ProviderID
ImplementationID
Availability
Compatibility
TrustState
Metadata
```

## Discovery-Ablauf

```text
Discovery Request
       ↓
Capability Registry
       ↓
Context Filter
       ↓
Version / Interface Filter
       ↓
Semantic Filter
       ↓
Compatibility Information
       ↓
Visible Candidates
```

Die tatsächliche Provider- und Implementierungsauswahl erfolgt anschließend durch Resolution.

## Suche nach CapabilityID

Ist die benötigte Capability bekannt, kann direkt gesucht werden:

```text
de.nova.image.filter.gaussian
        ↓
Capability Registry
        ↓
Available Providers
```

## Semantische Discovery

Ist keine konkrete CapabilityID bekannt, darf anhand gewünschter Ein- und Ausgaben gesucht werden:

```text
Input:  Image
Output: Image
Operation: Filter
        ↓
Matching Capabilities
```

Dadurch können Solutions, Agenten und generative Oberflächen passende Fähigkeiten dynamisch ermitteln.

## Kontext

Discovery kann abhängig vom aktuellen Kontext unterschiedliche Ergebnisse liefern:

```text
System
User
Program
Solution
Workspace
Process
Recovery
```

Nicht sichtbare oder durch Policy ausgeblendete Provider müssen nicht im Ergebnis erscheinen.

## Registry

Die Capability Registry ist die primäre Quelle für Discovery-Metadaten.

Sie enthält keine Capability-Tokens, Credentials oder aktive Authority.

```text
Registry Entry
      ≠
Capability Handle
```

## Verfügbarkeit

Discovery muss zwischen Registrierung und tatsächlicher Verfügbarkeit unterscheiden können.

Beispiel:

```text
Registered
Available
TemporarilyUnavailable
Incompatible
Restricted
Disabled
Unknown
```

Eine registrierte Capability ist nicht automatisch ausführbar.

## Dynamische Änderungen

Capability Discovery muss Änderungen an installierten Paketen, Providern und Implementierungen erkennen können.

```text
Install
Update
Disable
Enable
Remove
Provider Failure
Hardware Change
```

Discovery-Ergebnisse und Caches müssen entsprechend invalidierbar sein.

## Sicherheit

Discovery darf keine Authority erzeugen.

```text
Discover
   ↓
Resolve
   ↓
Permission / Policy
   ↓
Authorized Handle
   ↓
Execute
```

Informationen über sicherheitskritische Capabilities dürfen abhängig vom Sicherheitskontext eingeschränkt sichtbar sein.

## Normative Anforderungen

1. NovaOS MUSS Capabilities über ihre stabile `CapabilityID` auffindbar machen können.
2. Discovery MUSS mehrere Provider und Implementierungen derselben Capability erkennen können.
3. Discovery MUSS Versions- und Interface-Anforderungen berücksichtigen können.
4. Semantische Input- und Output-Typen MÜSSEN als Suchkriterien verwendbar sein.
5. Discovery MUSS kontextabhängig filterbar sein.
6. Registrierung und tatsächliche Verfügbarkeit MÜSSEN getrennt behandelt werden.
7. Discovery DARF keine Permission oder Authority erzeugen.
8. Registry-Einträge DÜRFEN keine Capability-Tokens oder Credentials enthalten.
9. Discovery DARF keine konkrete Implementierung als ausgeführt betrachten.
10. Dynamische Änderungen MÜSSEN Discovery-Ergebnisse aktualisieren beziehungsweise invalidieren können.
11. Sicherheitskritische Discovery-Informationen MÜSSEN durch Policy einschränkbar sein.
12. Discovery MUSS Ergebnisse an Capability Resolution übergeben können.
13. Suchkriterien, Kandidaten und Filtergrund MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-NAMESPACE-0001`
- `NPSPEC-CAPABILITY-CATEGORY-0001`
- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-SEMANTICTYPE-0001`
- `NPSPEC-CAPABILITY-IMPLEMENTATION-0001`
- `NPSPEC-CAPABILITY-VERSIONING-0001`
- `NPSPEC-CAPABILITY-COMPATIBILITY-0001`
- `NPSPEC-CAPABILITY-TRUST-0001`
- `NPSPEC-REGISTRY-CAPABILITY-0001`
- `NPSPEC-REGISTRY-QUERY-0001`

## Ergebnis

NovaOS kann verfügbare Fähigkeiten anhand stabiler Identitäten, semantischer Typen, Operationen, Versionen und Kontext dynamisch entdecken. Discovery liefert geeignete Kandidaten für die nachfolgende Resolution, ohne dadurch Provider auszuwählen, Berechtigungen zu vergeben oder Authority zu erzeugen.