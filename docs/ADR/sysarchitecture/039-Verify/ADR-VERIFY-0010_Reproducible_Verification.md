# ADR-VERIFY-0010 – Reproducible Verification

## Status

Angenommen

## Kategorie

Verification / Reproducibility / Provenance

## Kontext

Verifikationsergebnisse sind nur belastbar, wenn nachvollziehbar ist, unter welchen Bedingungen sie entstanden sind.

Änderungen an:

```text
Source Code
Compiler
Build Configuration
Dependencies
Verification Tools
Specifications
Hardware Model
```

können zu anderen Ergebnissen führen.

NovaOS benötigt deshalb reproduzierbare Verifikationsprozesse.

## Entscheidung

NovaOS gestaltet kritische Verifikation so, dass Ergebnisse **eindeutig einer definierten und reproduzierbaren Verification Environment zugeordnet werden können**.

```text
Specification
     +
Source
     +
Toolchain
     +
Configuration
     ↓
Verification
     ↓
Reproducible Result
```

## Verification Identity

Ein Verifikationslauf erhält eine eindeutige Identität bzw. Provenance.

Diese referenziert mindestens relevante Versionen von:

```text
Component
Specification
Verification Tool
Configuration
Dependencies
```

Damit kann festgestellt werden, für welchen konkreten Systemstand ein Ergebnis gilt.

## Deterministische Inputs

Alle für die Verifikation relevanten Eingaben müssen eindeutig bestimmt oder dokumentiert sein.

Dazu gehören beispielsweise:

```text
Source Revision
Build Options
Feature Flags
Model Version
Contract Version
Test Data
Verification Rules
```

Nicht dokumentierte Umgebungsabhängigkeiten sollen vermieden werden.

## Toolchain

Verwendete Compiler, Analyzer, Model Checker und andere Verification Tools müssen versionierbar identifizierbar sein.

```text
Verification Result
        ↓
Toolchain Version
```

Ein Tool-Update kann eine erneute Verifikation erforderlich machen.

## Reproduzierbare Umgebung

Kritische Verifikationsläufe sollen in einer definierten Umgebung wiederholbar sein.

Dies kann beispielsweise über:

```text
Pinned Toolchains
Versioned Dependencies
Declarative Build Environments
Controlled Configuration
```

erreicht werden.

Die konkrete technische Umsetzung bleibt austauschbar.

## Ergebnisvergleich

Wiederholte Verifikationsläufe müssen vergleichbar sein.

```text
Run A
  ↓
Result A

Run B
  ↓
Result B

Result A ↔ Result B
```

Abweichungen müssen erkennbar und untersuchbar sein.

Bit-identische Ausgaben sind nur dort erforderlich, wo dies ausdrücklich als Verifikationsanforderung definiert wurde.

## Hardwareabhängige Verifikation

Nicht jede Verifikation ist vollständig hardwareunabhängig.

Bei hardwareabhängigen Eigenschaften müssen relevante Plattformmerkmale Teil der Verification Provenance sein.

Beispiele:

```text
CPU Architecture
Timing Properties
Memory Model
Device Characteristics
Firmware
```

## Verification Evidence

Ergebnisse können als Verification Evidence gespeichert werden.

```text
Component Version
Specification Version
Environment
Verification Method
Result
Evidence
```

Diese Evidence kann von Build-, Update-, Trust- und Deployment-Mechanismen verwendet werden.

## Normative Anforderungen

1. Kritische Verifikation MUSS eindeutig reproduzierbaren Eingaben zugeordnet werden können.
2. Komponenten- und Spezifikationsversionen MÜSSEN Bestandteil der Verification Provenance sein.
3. Verwendete Verification Tools MÜSSEN versionierbar identifizierbar sein.
4. Relevante Konfigurationen und Abhängigkeiten MÜSSEN nachvollziehbar sein.
5. Kritische Verifikationsumgebungen SOLLEN deklarativ reproduzierbar sein.
6. Wiederholte Verification Results MÜSSEN vergleichbar sein.
7. Abweichende Ergebnisse MÜSSEN erkennbar sein.
8. Hardwareabhängige Verifikation MUSS relevante Plattformmerkmale dokumentieren.
9. Alte Verification Evidence DARF nicht automatisch auf veränderte Komponenten oder Spezifikationen übertragen werden.

## Konsequenzen

### Positiv

- Verifikationsergebnisse werden nachvollziehbar,
- Ergebnisse können unabhängig wiederholt werden,
- Toolchain- und Konfigurationsänderungen werden sichtbar,
- Verification Evidence kann in Trust- und Release-Prozesse einfließen.

### Negativ

- Toolchains und Umgebungen müssen versioniert werden,
- Reproduktion hardwareabhängiger Prüfungen kann aufwendig sein.

## Abhängigkeiten

- `ADR-VERIFY-0001_Formale_Spezifikation_für_kritische_Komponenten`
- `ADR-VERIFY-0008_Static_und_Runtime_Verification`
- `ADR-VERIFY-0009_Contract_Verification`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0019_Evolvierbar`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`
- `ADR-REALTIME-0007_Record_Replay`

## Ergebnis

NovaOS betrachtet ein Verifikationsergebnis nicht isoliert:

```text
Result
  +
Specification
  +
Source
  +
Toolchain
  +
Environment
  =
Verification Evidence
```

Damit bleibt nachvollziehbar, **was geprüft wurde, womit es geprüft wurde und unter welchen Bedingungen das Ergebnis reproduziert werden kann**.