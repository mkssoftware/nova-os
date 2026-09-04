# ADR-TRUST-0011 – Reproducible Builds als Trust Evidence

## Status

Angenommen

## Kategorie

Trust Architecture / Software Trust / Supply Chain

## Kontext

Eine signierte Software bestätigt nicht automatisch, dass das ausgelieferte Binärartefakt tatsächlich aus dem angegebenen Quellcode erzeugt wurde.

NovaOS benötigt deshalb zusätzliche Trust Evidence, mit der überprüft werden kann, ob veröffentlichte Softwareartefakte reproduzierbar aus definierten Quellen und Build-Eingaben hervorgehen.

Reproducible Builds ermöglichen dabei grundsätzlich, aus denselben definierten Quellen, Build-Anweisungen und relevanten Umgebungsbedingungen bitidentische Artefakte zu erzeugen. :contentReference[oaicite:0]{index=0}

## Entscheidung

Nova.Trust behandelt **Reproducible Builds als native Software- und Supply-Chain-Trust-Evidence**.

```text
Source + Build Inputs
        ↓
Independent Build
        ↓
Artifact
        ↓
Comparison
        ↓
Reproducibility Evidence
        ↓
Nova.Trust
```

Ein erfolgreich reproduzierter Build erhöht die überprüfbare Verbindung zwischen Source Provenance und ausgeliefertem Artefakt.

Er erzeugt jedoch nicht automatisch vollständiges Vertrauen.

## Grundprinzip

```text
Reproducible ≠ Trusted
Reproducible ≠ Safe
Reproducible ≠ Authorized
```

Reproduzierbarkeit beantwortet primär:

```text
Can the claimed build inputs
produce this artifact reproducibly?
```

Nova.Trust entscheidet anschließend, welche Vertrauenswirkung diese Evidence im jeweiligen Kontext besitzt.

## Build Inputs

Für reproduzierbare Builds müssen die relevanten Eingaben eindeutig bestimmbar sein.

Dazu können gehören:

```text
Source Revision
Dependencies
Build Instructions
Toolchain
Build Configuration
Relevant Environment Properties
```

Nicht relevante Eigenschaften der Build-Maschine sollen das resultierende Artefakt nicht beeinflussen.

## Artifact Verification

Reproduzierte Artefakte sollen nach Möglichkeit bitidentisch überprüfbar sein.

```text
Official Artifact
       ↓
Cryptographic Hash
       ↕
Rebuilt Artifact
```

Übereinstimmende Hashes können als starke Evidence dafür verwendet werden, dass beide Artefakte identisch sind.

## Independent Rebuilders

Nova.Trust kann Evidence mehrerer unabhängiger Rebuilder berücksichtigen.

```text
Source
 ├── Builder A → Hash X
 ├── Builder B → Hash X
 └── Builder C → Hash X
```

Mehrere unabhängige Übereinstimmungen können stärkere Trust Evidence liefern als die Wiederholung innerhalb derselben Build-Infrastruktur.

Die Anzahl der Rebuilder allein erzeugt jedoch keine automatische Trust-Entscheidung.

## Verhältnis zu Provenance

Reproducibility ergänzt Software-Provenance.

```text
Software Provenance
        +
Reproducible Build Evidence
        ↓
Stronger Supply Chain Evidence
```

Die Provenance beschreibt, wie ein Artefakt entstanden sein soll.

Reproducibility ermöglicht eine unabhängige Überprüfung dieser Beziehung.

## Verhältnis zu Signaturen

Signaturen und Reproducible Builds erfüllen unterschiedliche Aufgaben.

```text
Signature
    → Who signed this artifact?

Provenance
    → How was this artifact produced?

Reproducibility
    → Can this artifact be independently reproduced?
```

Keine dieser Eigenschaften ersetzt die anderen.

## Nicht reproduzierbare Software

Nicht reproduzierbare Software darf nicht automatisch als kompromittiert gelten.

```text
Not Reproducible
≠
Malicious
```

Fehlende Reproduzierbarkeit bedeutet lediglich, dass diese Form der Trust Evidence nicht verfügbar oder nicht erfolgreich bestätigt wurde.

Trust Policies können für besonders kritische Software dennoch erfolgreiche Reproduzierbarkeit verlangen.

## Systemkritische Komponenten

Für besonders sicherheitskritische Komponenten kann NovaOS strengere Trust Policies definieren.

Beispielsweise:

```text
Boot Components
Kernel
Security Services
Nova.Trust Components
Credential Infrastructure
Critical Drivers
```

Für solche Komponenten kann verifizierte Reproduzierbarkeit als Hard Trust Requirement festgelegt werden.

## Build-Abweichungen

Schlägt die Reproduktion fehl, muss die Abweichung als Trust-relevantes Ereignis behandelt werden können.

```text
Expected Artifact
       ≠
Rebuilt Artifact
       ↓
Trust Investigation
```

Eine Abweichung darf nicht automatisch ignoriert werden, wenn Reproducibility durch Policy vorgeschrieben ist.

## Normative Anforderungen

1. Nova.Trust MUSS Reproducible Builds als Software-Trust-Evidence unterstützen können.
2. Reproducibility MUSS von Signatur, Provenance und Authority getrennt bleiben.
3. Relevante Build Inputs MÜSSEN eindeutig referenzierbar sein können.
4. Reproduzierte Artefakte SOLLEN nach Möglichkeit bitidentisch vergleichbar sein.
5. Kryptographische Hashes MÜSSEN zur Artefaktidentifikation verwendet werden können.
6. Nova.Trust MUSS Evidence unabhängiger Rebuilder berücksichtigen können.
7. Mehrere erfolgreiche Rebuilds DÜRFEN die Trust Evidence verstärken.
8. Erfolgreiche Reproduzierbarkeit DARF nicht automatisch vollständiges Vertrauen erzeugen.
9. Fehlende Reproduzierbarkeit DARF nicht automatisch als bösartiges Verhalten interpretiert werden.
10. Trust Policies MÜSSEN Reproducibility als Hard Requirement definieren können.
11. Build-Abweichungen MÜSSEN als Trust-relevante Evidence behandelbar sein.
12. Reproducibility Evidence MUSS mit Software- und Supply-Chain-Provenance verknüpfbar sein.
13. Reproducibility DARF keine Capability Authority erzeugen.
14. Relevante Rebuild-Ergebnisse MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Binärartefakte können unabhängig gegen ihren angegebenen Quellcode geprüft werden,
- kompromittierte Build-Infrastrukturen werden schwerer unbemerkt nutzbar,
- Software-Provenance erhält zusätzliche überprüfbare Evidence,
- mehrere unabhängige Rebuilder können Supply-Chain-Vertrauen stärken,
- kritische NovaOS-Komponenten können strengere Build-Trust-Anforderungen erhalten.

### Negative Konsequenzen

- Build-Systeme müssen deterministische Artefakte ermöglichen,
- Toolchains und Build Inputs müssen ausreichend genau beschrieben werden,
- unabhängige Rebuild-Infrastruktur verursacht zusätzlichen Aufwand.

## Verworfene Alternativen

### Signatur des Build-Artefakts reicht aus

Verworfen, da eine Signatur nicht beweist, dass das Artefakt tatsächlich aus dem angegebenen Source entstanden ist.

### Reproducible Build bedeutet automatisch vertrauenswürdig

Verworfen, da auch reproduzierbarer Quellcode unsicher oder bösartig sein kann.

### Nicht reproduzierbare Software grundsätzlich verbieten

Verworfen, da Reproducibility eine Trust Evidence und kein universelles Ausführungskriterium ist.

## Abhängigkeiten

- `ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur`
- `ADR-TRUST-0003_Signatur_ist_nicht_Vertrauen`
- `ADR-TRUST-0005_Trust_Graph_statt_globalem_Trust_Score`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`
- `ADR-SECURITY-0013_Code_Integrity_und_Code_Signing`
- `ADR-STORAGE-0003_Content_Addressed_Storage`

## Zugehörige NPSPECs

- `NPSPEC-TRUST-REPRODUCIBLE-BUILD-0001`
- `NPSPEC-TRUST-REBUILD-EVIDENCE-0001`
- `NPSPEC-TRUST-REBUILDER-0001`
- `NPSPEC-TRUST-ARTIFACT-COMPARISON-0001`
- `NPSPEC-TRUST-REPRODUCIBILITY-POLICY-0001`
- `NPSPEC-TRUST-REPRODUCIBLE-BUILD-TEST-0001`

## Ergebnis

Nova.Trust verwendet Reproducible Builds als zusätzliche überprüfbare Verbindung zwischen Source und ausgeliefertem Artefakt:

```text
Source + Build Inputs
        ↓
Independent Rebuilders
        ↓
Bit-identical Artifacts
        ↓
Reproducibility Evidence
        ↓
Nova.Trust
```

Damit kann NovaOS nicht nur prüfen, **wer ein Artefakt signiert hat und woher es stammt**, sondern zusätzlich, **ob unabhängige Parteien dasselbe Artefakt aus den angegebenen Build-Eingaben reproduzieren können**.