# ADR-UPDATE-0011 – Reproducible Builds

## Status

Angenommen

## Kategorie

Update / Build / Trust

## Kontext

NovaOS muss nachvollziehen können, ob veröffentlichte Binärartefakte tatsächlich aus den angegebenen Quellen und Build-Eingaben entstanden sind.

Eine gültige Signatur bestätigt den Signierer, beweist jedoch nicht, dass ein Artefakt aus dem angegebenen Quellcode erzeugt wurde.

Reproduzierbare Builds schaffen dafür zusätzliche überprüfbare Evidenz.

## Entscheidung

Offizielle NovaOS-Komponenten SOLLEN reproduzierbar gebaut werden.

```text
Source + Build Inputs
        ↓
   Build A → H1
   Build B → H1
```

Identische definierte Eingaben sollen dasselbe Content-Addressed Build-Artefakt erzeugen.

Für sicherheitskritische Systemkomponenten ist Reproduzierbarkeit ein verpflichtendes Build-Ziel.

## Build Inputs

Alle relevanten Build-Eingaben müssen eindeutig bestimmbar sein.

Dazu gehören mindestens:

```text
Source Revision
Dependencies
Toolchain
Build Configuration
Target Architecture
Build Scripts
```

Nicht deterministische Einflüsse wie Zeitstempel, zufällige IDs oder umgebungsabhängige Pfade sollen vermieden beziehungsweise normalisiert werden.

## Verifikation

Build-Ergebnisse werden über ihre Content ID verglichen.

```text
Reference Build   → H42
Independent Build → H42

H42 == H42
      ↓
Reproducible
```

Abweichende Ergebnisse müssen als solche erkennbar sein.

## Trust

Reproduzierbarkeit liefert zusätzliche Trust Evidence, ersetzt jedoch keine anderen Trust-Mechanismen.

```text
Reproducible Build
        ≠
Trusted Software
```

Signatur, Provenance und Trust Policy bleiben eigenständige Prüfungen.

## Update Pipeline

Die Update Pipeline muss Reproducible-Build-Evidenz berücksichtigen können.

Für kritische Komponenten kann eine Policy beispielsweise verlangen:

```text
Valid Signature
      +
Valid Provenance
      +
Reproducible Build Evidence
```

## Normative Anforderungen

1. Offizielle NovaOS-Komponenten SOLLEN reproduzierbar buildbar sein.
2. Sicherheitskritische Komponenten MÜSSEN reproduzierbare Builds als Build-Ziel besitzen.
3. Build-relevante Eingaben MÜSSEN eindeutig identifizierbar sein.
4. Nicht deterministische Build-Einflüsse SOLLEN vermieden oder normalisiert werden.
5. Build-Ergebnisse MÜSSEN über ihre Content IDs vergleichbar sein.
6. Reproducibility DARF Signatur-, Provenance- oder Trust-Prüfungen nicht ersetzen.
7. Nova.Trust MUSS Reproducible-Build-Evidenz auswerten können.

## Konsequenzen

### Positive Konsequenzen

- Binärartefakte können unabhängig überprüft werden.
- Manipulationen in der Build-Pipeline werden leichter erkennbar.
- Supply-Chain-Trust wird verbessert.
- Builds werden nachvollziehbarer.

### Negative Konsequenzen

- Build-Umgebungen müssen kontrollierter sein.
- Toolchains können zusätzliche Maßnahmen für deterministische Ergebnisse benötigen.

## Abhängigkeiten

- `ADR-UPDATE-0006_Content_Addressed_Packages`
- `ADR-UPDATE-0010_Package_Signing_Provenance_und_SBOM`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`
- `ADR-TRUST-0011_Reproducible_Builds_als_Trust_Evidence`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-REPRODUCIBLE-BUILD-0001`
- `NPSPEC-UPDATE-BUILD-INPUTS-0001`
- `NPSPEC-UPDATE-BUILD-VERIFICATION-0001`

## Ergebnis

NovaOS strebt für definierte Build-Eingaben deterministische Artefakte an:

```text
Same Source
+ Same Inputs
+ Same Build Definition
        ↓
Same Content ID
```

Reproduzierbare Builds liefern damit überprüfbare Evidenz für die Herkunft veröffentlichter NovaOS-Artefakte.