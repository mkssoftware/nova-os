# NPSPEC-UPDATE-SBOM-0001 – Nova Update SBOM

## Status

Angenommen

## Kategorie

Update / Supply Chain / Software Bill of Materials

## Zweck

NovaOS definiert eine maschinenlesbare Software Bill of Materials für Update-Artefakte.

Eine SBOM beschreibt, aus welchen Softwarekomponenten, Bibliotheken, Modulen und relevanten Abhängigkeiten ein Update besteht und verbindet diese Informationen mit Package-, Content-, Build- und Provenance-Identitäten.

```text
Update Package
     ↓
SBOM
├── Components
├── Versions
├── Dependencies
├── ContentIDs
└── Provenance
```

## Grundprinzipien

```text
SBOM ≠ Package Manifest
SBOM ≠ Dependency Resolver
SBOM ≠ Vulnerability Database
SBOM ≠ Trust Decision
Component Listed ≠ Component Trusted
Dependency Declared ≠ Dependency Safe
Version ≠ Unique Artifact Identity
SBOM Present ≠ SBOM Correct
```

## SBOM Model

```text
UpdateSBOM
├── SBOMID
├── PackageID
├── BuildID
├── ContentID
├── FormatVersion
├── Components
├── DependencyGraph
└── ProvenanceID
```

Optional:

```text
Supplier
License
SourceIdentity
SourceRevision
ComponentContentID
BuildMetadata
SecurityMetadata
ExternalReferences
VerificationEvidence
```

## Component Entry

Ein SBOM-Eintrag kann enthalten:

```text
ComponentID
Name
Version
Type
ContentID
Supplier
Source
Dependencies
License
BuildID
```

`ComponentID` soll unabhängig von Dateipfad oder Installationsort stabil bleiben.

## Dependency Graph

Abhängigkeiten werden als Graph modelliert.

```text
Kernel
├── Library A
│   └── Library C
└── Module B
    └── Library C
```

Transitive Abhängigkeiten sollen nachvollziehbar bleiben.

## Package Binding

Die SBOM muss eindeutig an das Update-Artefakt gebunden werden.

```text
PackageID
+
ContentID
+
SBOMID
```

Eine SBOM für ein anderes Artefakt darf nicht als gültige Beschreibung verwendet werden.

## Build Binding

Wenn Build-Provenance verfügbar ist:

```text
Source
  ↓
BuildID
  ↓
SBOM
  ↓
Package ContentID
```

Damit kann nachvollzogen werden, welche Komponenten tatsächlich in einem Build enthalten waren.

## Provenance Integration

SBOM und Update-Provenance ergänzen sich.

```text
SBOM
→ What is inside?

Provenance
→ Where did it come from?
```

Beide Informationen sollen über stabile IDs verbunden werden.

## Vulnerability Correlation

NovaOS kann SBOM-Daten verwenden, um bekannte Sicherheitsinformationen Komponenten zuzuordnen.

```text
ComponentID
   ↓
Security Intelligence
   ↓
Affected?
```

```text
SBOM Match
≠
Confirmed Vulnerability
```

Die eigentliche Sicherheitsbewertung bleibt eine separate Entscheidung.

## Update Impact Analysis

Vor einem Update kann NovaOS ermitteln:

```text
Added Components
Removed Components
Updated Components
Changed Dependencies
Changed Suppliers
Changed ContentIDs
```

Beispiel:

```text
SBOM v1
   ↓
Diff
   ↓
SBOM v2
```

Dies unterstützt Security-, Trust- und Compatibility-Prüfungen.

## Delta Updates

Auch bei Delta Updates muss die SBOM den resultierenden Zielzustand beschreiben.

```text
Base + Delta
     ↓
Target Artifact
     ↓
Target SBOM
```

Die SBOM darf nicht lediglich die Inhalte des Delta-Payloads beschreiben.

## Runtime Correlation

Soweit möglich soll eine laufende Komponente auf ihren SBOM-Eintrag zurückgeführt werden können.

```text
Running Component
      ↓
BuildID / ContentID
      ↓
SBOM Component
```

Damit kann NovaOS feststellen, welche bekannte Komponente tatsächlich aktiv ist.

## Driver und Firmware

SBOMs können auch relevante Bestandteile von:

```text
Drivers
Firmware Packages
Bootloader
Recovery Components
Runtime Components
```

beschreiben.

Firmware-interne Bestandteile sind nur erfassbar, soweit entsprechende Informationen verfügbar sind.

## Signing

Eine SBOM kann Bestandteil des signierten Update-Manifests oder separat kryptografisch gebunden sein.

```text
Package
+
SBOM
+
Signature
```

Manipulationen an sicherheitsrelevanten SBOM-Daten müssen erkennbar sein.

## Unvollständige SBOM

Nicht alle Komponenten liefern vollständige Informationen.

Zustände:

```text
Complete
Partial
Unavailable
Invalid
Unknown
```

```text
Missing SBOM
≠
No Dependencies
```

Security Policy darf vollständige SBOMs für bestimmte Komponenten verlangen.

## Formatversionierung

Das SBOM-Schema muss versioniert und erweiterbar sein.

```text
Known Optional Field
→ Process

Unknown Optional Field
→ Preserve / Ignore

Unknown Required Field
→ Reject / Limited Handling
```

## Speicherung

SBOM-Daten sollen nicht unnötig dupliziert werden.

Content-addressierte Speicherung kann identische Komponenten und SBOM-Fragmente gemeinsam referenzieren.

## Privacy

SBOMs dürfen keine unnötigen:

```text
Credentials
Secrets
Personal Data
Private Build Paths
Internal Tokens
```

enthalten.

## Retention

SBOMs müssen erhalten bleiben, solange sie für folgende Zustände relevant sind:

```text
Installed Component
Active Component
Rollback
Snapshot
A/B Slot
Recovery
Security Investigation
Provenance
```

## Provenance

NovaOS soll für eine SBOM nachvollziehen können:

```text
SBOMID
PackageID
BuildID
ContentID
Source
Creation Method
Signer
Verification State
Associated UpdateID
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Installed Components
Component Versions
ContentIDs
Dependency Graph
Supplier
Source
Associated Packages
Associated Updates
SBOM Completeness
Verification State
```

## Normative Anforderungen

1. NovaOS MUSS maschinenlesbare SBOMs für Update-Artefakte unterstützen.
2. Jede SBOM MUSS eindeutig identifizierbar sein.
3. SBOMs MÜSSEN an das beschriebene Update-Artefakt bindbar sein.
4. Versionsnummern DÜRFEN NICHT als alleinige Artefaktidentität verwendet werden.
5. Komponenten SOLLEN stabile ComponentIDs besitzen.
6. ContentIDs SOLLEN konkrete Komponenten kryptografisch identifizieren.
7. Direkte und transitive Abhängigkeiten SOLLEN darstellbar sein.
8. SBOMs SOLLEN mit Build-Provenance verbunden werden können.
9. SBOMs MÜSSEN mit Update-Provenance verknüpfbar sein.
10. NovaOS MUSS SBOM-Versionen vergleichen können.
11. Hinzugefügte, entfernte und geänderte Komponenten SOLLEN erkennbar sein.
12. Delta Updates MÜSSEN die SBOM des resultierenden Zielartefakts referenzieren können.
13. Laufende Komponenten SOLLEN mit SBOM-Einträgen korrelierbar sein.
14. Driver-, Firmware-, Boot- und Recovery-Pakete SOLLEN SBOMs unterstützen können.
15. Sicherheitsrelevante SBOM-Daten MÜSSEN gegen unerkannte Manipulation geschützt werden können.
16. SBOM-Daten DÜRFEN als Grundlage für Vulnerability Correlation verwendet werden.
17. Ein Vulnerability Match DARF NICHT automatisch als bestätigte Verwundbarkeit interpretiert werden.
18. Unvollständige SBOMs MÜSSEN explizit kennzeichenbar sein.
19. `Unknown` DARF NICHT als vollständige SBOM interpretiert werden.
20. Security Policy DARF vollständige SBOMs verlangen.
21. Das SBOM-Schema MUSS versionierbar sein.
22. Unbekannte optionale Felder SOLLEN erhalten oder sicher ignoriert werden können.
23. Unbekannte erforderliche Felder MÜSSEN Ablehnung oder eingeschränkte Verarbeitung auslösen können.
24. SBOMs DÜRFEN keine unnötigen Secrets oder personenbezogenen Daten enthalten.
25. Recovery- und Security-relevante SBOM-Daten DÜRFEN NICHT vorzeitig entfernt werden.
26. SBOM-Informationen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-PACKAGE-0001`
- `NPSPEC-UPDATE-DEPENDENCY-0001`
- `NPSPEC-UPDATE-SIGNING-0001`
- `NPSPEC-UPDATE-DELTA-0001`
- `NPSPEC-UPDATE-CONTENTADDRESS-0001`
- `NPSPEC-UPDATE-VERIFY-0001`
- `NPSPEC-UPDATE-PROVENANCE-0001`
- `NPSPEC-TRUST-SUPPLYCHAIN-0001`
- `NPSPEC-VERIFY-REPRODUCIBLE-0001`
- `ADR-ARCH-0186`

## Ergebnis

```text
Source + Dependencies
        ↓
Build
        ↓
SBOM + BuildID
        ↓
Package + ContentID
        ↓
Update
        ↓
Installed Components
        ↓
Runtime Correlation
        ↓
Security / Provenance Analysis
```

NovaOS erhält damit eine standardisierte Bestandsbeschreibung seiner Update-Artefakte. Dadurch können Abhängigkeiten, Build-Herkunft, Änderungen zwischen Versionen und betroffene Komponenten präzise nachvollzogen und mit Update-Provenance, Trust und Sicherheitsanalysen verbunden werden.