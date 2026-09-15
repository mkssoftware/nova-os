# ADR-UPDATE-0010 – Package Signing, Provenance und SBOM

## Status

Angenommen

## Kategorie

Update Architecture / Software Supply Chain / Trust / Security

## Kontext

Content Addressing stellt sicher, dass NovaOS ein konkretes Softwareartefakt eindeutig identifizieren und seine Integrität überprüfen kann.

Eine Content ID beantwortet jedoch nicht:

```text
Wer hat das Artefakt veröffentlicht?
Woher stammt es?
Wie wurde es gebaut?
Welche Quellen wurden verwendet?
Welche Komponenten enthält es?
Welche Abhängigkeiten besitzt es?
Ist es für diesen Zweck vertrauenswürdig?
```

Für eine belastbare Software-Supply-Chain benötigt NovaOS deshalb zusätzlich:

```text
Package Signing
Provenance
SBOM
```

Diese Informationen müssen mit dem konkreten unveränderlichen Artefakt verknüpft werden.

## Entscheidung

NovaOS integriert **Package Signing, Provenance und Software Bill of Materials (SBOM)** als systemweite Bestandteile der Update- und Package-Architektur.

```text
Package Artifact
      ↓
Content ID
   ┌──┼───────────┐
   ↓  ↓           ↓
Signature      Provenance
                  ↓
                 SBOM
```

Vor der Aktivierung eines Pakets werden diese Evidenzen entsprechend der geltenden Trust Policy ausgewertet.

## Grundprinzip

```text
Integrity
    +
Authenticity Evidence
    +
Provenance
    +
Component Transparency
    ↓
Trust Decision
```

Keine einzelne dieser Eigenschaften stellt allein vollständiges Vertrauen her.

## Trennung der Konzepte

NovaOS behandelt folgende Konzepte getrennt:

```text
Content ID
Signature
Provenance
SBOM
Trust
Authorization
```

Insbesondere gilt:

```text
Valid Signature
      ≠
Trusted Software
```

und:

```text
Complete SBOM
      ≠
Secure Software
```

## Package Signing

Softwareartefakte können kryptografisch signiert werden.

```text
Artifact
   ↓
Content ID
   ↓
Signature
```

Die Signatur muss eindeutig an das konkrete Artefakt beziehungsweise dessen kryptografische Identität gebunden sein.

## Signer Identity

Eine Signatur muss einer überprüfbaren Signer Identity zugeordnet werden können.

Beispiele:

```text
NovaOS Release Authority
Package Maintainer
Organization
Build Service
Authorized Third Party
```

Die Identität des Signers und dessen Authority müssen getrennt bewertet werden.

## Signaturprüfung

Vor Verwendung eines signierten Pakets muss NovaOS mindestens prüfen können:

```text
Signature Valid?
Signer Identity Valid?
Signing Credential Valid?
Credential Revoked?
Trust Anchor Valid for Purpose?
Policy satisfied?
```

Eine mathematisch gültige Signatur reicht nicht aus.

## Mehrere Signaturen

Ein Artefakt kann mehrere Signaturen besitzen.

```text
Artifact H42
 ├── Signature A
 ├── Signature B
 └── Signature C
```

Dadurch können beispielsweise getrennte Rollen nachgewiesen werden:

```text
Builder
Maintainer
Release Authority
Security Approval
```

Policies können mehrere unabhängige Signaturen verlangen.

## Threshold Policy

Für besonders kritische Komponenten kann NovaOS mehrere Freigaben verlangen.

Beispiel:

```text
Kernel Update

Required:
2 of 3 authorized release signers
```

Dadurch muss die Kompromittierung eines einzelnen Signing Keys nicht automatisch zur vollständigen Update-Autorisierung führen.

## Provenance

Provenance beschreibt die nachvollziehbare Herkunft eines Artefakts.

Sie kann unter anderem enthalten:

```text
Source Repository
Source Revision
Build Definition
Build Environment
Builder Identity
Build Inputs
Dependencies
Build Time
Build Parameters
Output Content ID
```

Die konkrete Struktur wird durch eine NPSPEC definiert.

## Provenance Chain

NovaOS muss Provenance über mehrere Stufen abbilden können.

```text
Source
  ↓
Build
  ↓
Intermediate Artifact
  ↓
Package
  ↓
System State
```

Dadurch kann nachvollzogen werden, wie ein konkretes Artefakt entstanden ist.

## Build Provenance

Build Provenance muss das erzeugte Artefakt eindeutig referenzieren.

```text
Build Inputs
     ↓
Build Process
     ↓
Artifact H42
```

Eine Provenance-Aussage für ein anderes Artefakt darf nicht auf H42 übertragen werden.

## Reproducible Builds

Reproducible Builds können zusätzliche Trust Evidence liefern.

```text
Source S
   ↓
Independent Build A → H42
Independent Build B → H42
```

Identische Content IDs liefern Evidenz dafür, dass die erzeugten Artefaktbytes übereinstimmen.

Reproduzierbarkeit ersetzt jedoch weder Signatur noch Trust Policy.

## SBOM

Jedes relevante Softwarepaket soll eine maschinenlesbare Software Bill of Materials besitzen.

Die SBOM beschreibt die im Artefakt enthaltenen beziehungsweise für dessen Erzeugung relevanten Softwarekomponenten.

Beispiel:

```text
Package
 ├── Library A
 ├── Library B
 ├── Runtime C
 └── Component D
```

## SBOM-Inhalt

Eine SBOM soll mindestens relevante Informationen enthalten können wie:

```text
Component Name
Version
Supplier
Content Identifier
Dependency Relationship
License Information
Package Identifier
Provenance Reference
```

Zusätzliche Informationen können abhängig vom Pakettyp ergänzt werden.

## SBOM-Formate

NovaOS soll etablierte maschinenlesbare SBOM-Formate unterstützen können.

Die interne Architektur darf jedoch nicht dauerhaft an ein einzelnes externes Format gekoppelt sein.

```text
External SBOM Format
        ↓
Normalized NovaOS Model
```

Dadurch können mehrere Formate importiert, validiert und ausgewertet werden.

## SBOM und Content ID

Eine SBOM muss eindeutig dem Artefakt zugeordnet werden, das sie beschreibt.

```text
Artifact H42
     ↓
SBOM for H42
```

Eine SBOM darf nicht stillschweigend auf eine veränderte Paketversion übertragen werden.

## Transitive Abhängigkeiten

NovaOS muss Abhängigkeitsbeziehungen über mehrere Ebenen nachvollziehen können.

```text
Package A
   ↓
Library B
   ↓
Library C
```

Dadurch können Sicherheits- oder Lizenzinformationen auch über transitive Abhängigkeiten ausgewertet werden.

## Vulnerability Analysis

SBOM-Informationen können verwendet werden, um betroffene Komponenten zu identifizieren.

```text
Vulnerability
     ↓
Affected Component
     ↓
SBOM Index
     ↓
Affected Packages
     ↓
Affected System States
```

Dadurch kann NovaOS feststellen, welche installierten oder gespeicherten Softwarezustände eine betroffene Komponente enthalten.

## Kein automatischer Sicherheitsbeweis

Eine SBOM beschreibt Bestandteile, beweist aber nicht deren Sicherheit.

```text
SBOM
  ≠
Security Proof
```

Ebenso bedeutet das Fehlen einer bekannten Schwachstelle nicht automatisch, dass ein Artefakt sicher ist.

## Trust Policy

Nova.Trust entscheidet anhand der verfügbaren Evidenz, ob ein Artefakt für einen konkreten Zweck akzeptiert werden darf.

Beispiel:

```text
Package
   ↓
Content Integrity
   ↓
Signature
   ↓
Signer Identity
   ↓
Provenance
   ↓
SBOM
   ↓
Policy
   ↓
Trust Decision
```

Die Trust-Anforderungen können je nach Softwareklasse unterschiedlich sein.

## Kritische Systemkomponenten

Für kritische Komponenten können strengere Anforderungen gelten.

Beispiele:

```text
Bootloader
Kernel
Security Service
Credential Broker
Update Service
```

Eine Policy kann verlangen:

```text
Trusted Signer
Verified Provenance
SBOM
Reproducible Build Evidence
Threshold Signatures
```

## Drittanbieter-Software

Drittanbieter-Software muss nicht zwangsläufig durch NovaOS selbst signiert sein.

NovaOS kann unterschiedliche Trust Anchors und Policies für unterschiedliche Zwecke verwenden.

```text
NovaOS System Software
Third-Party Application
Development Package
Local User Build
```

Diese Klassen können unterschiedliche Anforderungen besitzen.

## Lokale Builds

Vom Benutzer lokal erzeugte Software kann eine eigene Provenance besitzen.

```text
Local Source
     ↓
Local Build
     ↓
Artifact H73
     ↓
Local Provenance
```

Lokale Herkunft bedeutet weder automatisch vertrauenswürdig noch nicht vertrauenswürdig.

Die Policy entscheidet über die zulässige Verwendung.

## Developer Mode

Developer Mode kann weniger restriktive Softwarequellen zulassen.

Solche Ausnahmen müssen explizit sichtbar und auf den entsprechenden Trust Context begrenzt sein.

Ein unsigniertes Entwicklungsartefakt darf nicht automatisch zu einem vertrauenswürdigen Systemartefakt werden.

## Capability Provider

Capability Provider unterliegen denselben Supply-Chain-Prüfungen.

```text
Capability Requirement
       ↓
Provider Candidate
       ↓
Content ID
       ↓
Signature + Provenance + SBOM
       ↓
Trust Policy
       ↓
Eligible / Rejected
```

Ein funktional kompatibler Provider darf ausgeschlossen werden, wenn seine Trust-Anforderungen nicht erfüllt sind.

## Dependency Resolution

Der Capability Resolver muss Supply-Chain-Evidenz als Auswahlkriterium verwenden können.

```text
Provider A
Capability = valid
Trust = insufficient
        ↓
Rejected
```

Trust ist damit ein Hard Constraint, wenn der Execution Contract oder die System Policy dies verlangt.

## Update Pipeline

Die Transactional Update Pipeline integriert die Prüfungen vor Aktivierung.

```text
Acquire
   ↓
Verify Content ID
   ↓
Verify Signature
   ↓
Verify Provenance
   ↓
Evaluate SBOM
   ↓
Trust Policy
   ↓
Prepare / Reject
```

Nicht erfüllte Hard Constraints verhindern den Commit beziehungsweise die Aktivierung.

## Offline Verification

Kritische Supply-Chain-Evidenz muss soweit möglich offline prüfbar sein.

```text
Artifact
Signature
Provenance
SBOM
Trust Metadata
```

müssen deshalb zusammen mit dem relevanten Softwarezustand speicherbar sein.

Netzwerkzugriff darf für die grundlegende Integritätsprüfung eines bereits vorhandenen Known-Good-Zustands nicht zwingend erforderlich sein.

## Revocation

Signing Credentials, Signer Identities oder Artefakte können widerrufen werden.

```text
Signer
Credential
Artifact
Provider
```

Revocation muss von Nova.Trust berücksichtigt werden.

Ein Widerruf bedeutet jedoch nicht automatisch, dass ein laufendes System sofort zerstörerisch verändert werden muss.

Die Reaktion erfolgt entsprechend Security-, Update- und Recovery-Policy.

## Transparency

NovaOS soll relevante Supply-Chain-Informationen introspektierbar machen.

Beispiel:

```text
Package: nova.graphics
Version: 4.2
Content ID: H42
Signer: Nova Release Authority
Provenance: Verified
SBOM: Available
Trust State: Accepted
```

Die Darstellung darf komplexe kryptografische Details abstrahieren, ohne die zugrunde liegende Evidenz zu verlieren.

## Audit

Trust-relevante Entscheidungen müssen auditierbar sein.

```text
Artifact
   ↓
Evidence
   ↓
Policy
   ↓
Decision
   ↓
Audit Record
```

Dadurch muss später nachvollziehbar sein, warum ein Artefakt akzeptiert oder abgelehnt wurde.

## Privacy

Provenance und SBOM können sensible Informationen enthalten.

Beispiele:

```text
Internal Repository
Developer Identity
Build Infrastructure
Internal Component Names
```

NovaOS muss deshalb Disclosure-, Privacy- und Data-Sovereignty-Regeln auf Supply-Chain-Metadaten anwenden können.

## Retention

Supply-Chain-Evidenz muss mindestens solange erhalten bleiben, wie sie für:

```text
Active State
Rollback
Recovery
Audit
Compliance
```

benötigt wird.

Ein Paket darf nicht von seiner für Recovery erforderlichen Evidenz getrennt werden.

## A/B-Systembasis

A/B-Systembasen müssen ihre Supply-Chain-Evidenz unabhängig verifizieren können.

```text
Base A
 └── Package Evidence

Base B
 └── Package Evidence
```

Ein Candidate darf nicht allein deshalb vertrauenswürdig sein, weil die vorherige Basis vertrauenswürdig war.

## Snapshot-Based Rollback

Ein Snapshot muss auf die zugehörigen Paket- und Trust-Evidenzen referenzieren können.

```text
Snapshot
   ↓
Software State
   ↓
Content IDs
   ↓
Signatures / Provenance / SBOM
```

Dadurch kann auch ein wiederhergestellter Zustand erneut verifiziert werden.

## Immutable System Basis

Die Immutable System Basis muss als Zusammensetzung konkret verifizierter Artefakte beschreibbar sein.

```text
Immutable Base
 ├── H1 + Evidence
 ├── H2 + Evidence
 └── H3 + Evidence
```

Unveränderlichkeit ersetzt dabei keine Supply-Chain-Verifikation.

## NovaDOS

NovaDOS muss für kritische Recovery-Zustände grundlegende Supply-Chain-Evidenz offline prüfen können.

Mindestens relevant sind:

```text
Content ID
Signature
Known Trust Anchor
System State Association
```

Komplexere Online-Abfragen können im Offline-Recovery-Modus entfallen, dürfen aber nicht durch ungeprüfte Annahmen ersetzt werden.

## Normative Anforderungen

1. NovaOS MUSS Package Signing, Provenance und SBOM als getrennte Supply-Chain-Evidenzen unterstützen.
2. Signaturen MÜSSEN eindeutig an konkrete Artefakte beziehungsweise Content IDs bindbar sein.
3. Signaturen MÜSSEN einer überprüfbaren Signer Identity zugeordnet werden können.
4. Eine gültige Signatur DARF nicht automatisch als vollständige Trust-Entscheidung gelten.
5. NovaOS MUSS mehrere Signaturen für dasselbe Artefakt unterstützen können.
6. Kritische Komponenten MÜSSEN Threshold- beziehungsweise Multi-Signer-Policies unterstützen können.
7. Provenance MUSS konkrete Build-Outputs eindeutig referenzieren können.
8. Provenance MUSS Source-, Build- und Input-Beziehungen abbilden können.
9. Reproducible-Build-Evidenz MUSS in Trust-Entscheidungen einfließen können.
10. Reproducible Builds DÜRFEN Signatur- oder Trust-Prüfungen nicht ersetzen.
11. Relevante Softwarepakete SOLLEN eine maschinenlesbare SBOM besitzen.
12. SBOMs MÜSSEN eindeutig den von ihnen beschriebenen Artefakten zugeordnet werden können.
13. NovaOS SOLL mehrere etablierte SBOM-Formate über ein normalisiertes internes Modell unterstützen können.
14. Transitive Softwareabhängigkeiten MÜSSEN über SBOM- beziehungsweise Dependency-Daten nachvollziehbar sein können.
15. SBOM-Daten MÜSSEN zur Identifikation betroffener Pakete und Systemzustände verwendbar sein.
16. Eine SBOM DARF nicht als Sicherheitsbeweis behandelt werden.
17. Trust Policies MÜSSEN Anforderungen an Signatur, Provenance und SBOM definieren können.
18. Kritische Systemkomponenten MÜSSEN strengere Supply-Chain-Policies erhalten können als normale Anwendungen.
19. Drittanbieter-Software MUSS zweckgebundene eigene Trust Anchors verwenden können.
20. Lokale Builds MÜSSEN mit eigener Provenance behandelbar sein.
21. Developer Mode DARF Trust-Ausnahmen nicht stillschweigend auf normale Systemzustände übertragen.
22. Capability Provider MÜSSEN Supply-Chain-Trust-Prüfungen durchlaufen können.
23. Capability-Based Dependency Resolution MUSS Trust-Evidenz als Hard Constraint verwenden können.
24. Die Transactional Update Pipeline MUSS Supply-Chain-Prüfungen vor Aktivierung durchführen können.
25. Kritische vorhandene Softwarezustände MÜSSEN soweit erforderlich offline verifizierbar sein.
26. Revocation von Signern, Credentials und Artefakten MUSS berücksichtigt werden können.
27. Trust-Entscheidungen MÜSSEN nachvollziehbar und auditierbar sein.
28. Supply-Chain-Metadaten MÜSSEN Privacy- und Data-Sovereignty-Regeln unterliegen.
29. Für Rollback und Recovery erforderliche Supply-Chain-Evidenz DARF nicht vorzeitig entfernt werden.
30. A/B-Systembasen MÜSSEN ihre Artefakte unabhängig verifizieren können.
31. Snapshot-Zustände MÜSSEN auf ihre zugehörige Supply-Chain-Evidenz referenzieren können.
32. Die Immutable System Basis MUSS aus konkret identifizierbaren und verifizierbaren Artefakten beschreibbar sein.
33. NovaDOS MUSS grundlegende Trust-Evidenz kritischer Recovery-Artefakte offline prüfen können.

## Konsequenzen

### Positive Konsequenzen

- Herkunft von Software wird nachvollziehbar,
- konkrete Artefakte können eindeutig Signaturen und Provenance zugeordnet werden,
- SBOMs ermöglichen systemweite Komponenten- und Schwachstellenanalyse,
- kompromittierte oder widerrufene Provider können gezielt identifiziert werden,
- Update-, Rollback- und Recovery-Zustände behalten ihre Trust-Evidenz,
- kritische Komponenten können strengeren Policies unterliegen.

### Negative Konsequenzen

- zusätzliche Metadaten müssen gespeichert und gepflegt werden,
- Signing-Key- und Trust-Anchor-Lifecycle wird komplexer,
- SBOMs und Provenance können sensible Informationen enthalten,
- Supply-Chain-Verifikation erhöht den Aufwand der Update Pipeline.

## Verworfene Alternativen

### Nur Package Signing

Verworfen, da eine Signatur weder Herkunftskette noch enthaltene Komponenten ausreichend beschreibt.

### Nur Content Hashes

Verworfen, da Content IDs Integrität und Identität des Inhalts liefern, aber keine Aussage über Herkunft oder Trust treffen.

### Signatur bedeutet automatisch Vertrauen

Verworfen gemäß `ADR-TRUST-0003_Signatur_ist_nicht_Vertrauen`.

### SBOM nur für Entwicklerwerkzeuge

Verworfen, da das Betriebssystem selbst SBOM-Daten für Security, Updates, Recovery und Audit verwenden können soll.

### Eine zentrale globale Signing Authority

Verworfen, da Trust Anchors zweckgebunden sein sollen und NovaOS unterschiedliche Software- und Vertrauensdomänen unterstützen muss.

## Abhängigkeiten

- `ADR-UPDATE-0001_Deklarativer_Softwarezustand`
- `ADR-UPDATE-0002_Transactional_Update_Pipeline`
- `ADR-UPDATE-0003_Atomic_Updates`
- `ADR-UPDATE-0004_A_B_Systembasis`
- `ADR-UPDATE-0005_Snapshot_Based_Rollback`
- `ADR-UPDATE-0006_Content_Addressed_Packages`
- `ADR-UPDATE-0007_Immutable_System_Basis`
- `ADR-UPDATE-0008_Side_By_Side_Versionen`
- `ADR-UPDATE-0009_Capability_Based_Dependency_Resolution`
- `ADR-TRUST-0002_Identity_Signature_Provenance_Permission_Capability_Audit`
- `ADR-TRUST-0003_Signatur_ist_nicht_Vertrauen`
- `ADR-TRUST-0004_Trust_Anchors_nach_Zweck`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`
- `ADR-TRUST-0011_Reproducible_Builds_als_Trust_Evidence`
- `ADR-IDENTITY-0013_Identity_Lifecycle_und_Revocation`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-RESILIENCE-0022_NovaDOS_für_Offline_Recovery`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-PACKAGE-SIGNING-0001`
- `NPSPEC-UPDATE-SIGNER-IDENTITY-0001`
- `NPSPEC-UPDATE-MULTI-SIGNATURE-0001`
- `NPSPEC-UPDATE-PROVENANCE-0001`
- `NPSPEC-UPDATE-BUILD-PROVENANCE-0001`
- `NPSPEC-UPDATE-SBOM-0001`
- `NPSPEC-UPDATE-SBOM-NORMALIZATION-0001`
- `NPSPEC-UPDATE-SUPPLY-CHAIN-POLICY-0001`
- `NPSPEC-UPDATE-SUPPLY-CHAIN-REVOCATION-0001`
- `NPSPEC-UPDATE-SUPPLY-CHAIN-AUDIT-0001`
- `NPSPEC-UPDATE-OFFLINE-TRUST-VERIFICATION-0001`
- `NPSPEC-UPDATE-SUPPLY-CHAIN-TEST-0001`

## Ergebnis

NovaOS betrachtet ein Softwarepaket nicht nur als Datei oder Versionsnummer, sondern als eindeutig identifiziertes Artefakt mit überprüfbarer Supply-Chain-Evidenz:

```text
Package
   ↓
Content ID
   ↓
Integrity
   ↓
Signature
   ↓
Signer Identity
   ↓
Provenance
   ↓
SBOM
   ↓
Trust Policy
   ↓
Accept / Reject
```

Damit kann NovaOS nicht nur feststellen, **was** installiert werden soll, sondern auch nachvollziehen, **woher es stammt, wie es entstanden ist und welche Komponenten es enthält**.

Der zentrale Grundsatz lautet:

```text
Ein Paket wird nicht vertrauenswürdig,
nur weil es signiert ist.

NovaOS bewertet Inhalt,
Identität, Herkunft und Bestandteile
als getrennte Trust-Evidenzen.
```