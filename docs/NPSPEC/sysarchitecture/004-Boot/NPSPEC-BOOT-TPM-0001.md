# NPSPEC-BOOT-TPM-0001 – Nova TPM Integration

## Status

Entwurf

## Kategorie

Boot / TPM / Trust / Security

## Zweck

NovaOS integriert Trusted Platform Modules (TPM) als hardwaregestützte Vertrauenskomponente für Boot-Messungen, Schlüsselbindung und Attestation.

```text
Nova Boot
   ↓
TPM Interface
   ↓
PCR / Keys / Evidence
   ↓
Trust Evaluation
```

Das TPM unterstützt die Vertrauensarchitektur, bestimmt sie jedoch nicht allein.

## Grundprinzip

```text
TPM = Hardware-backed Trust Evidence
```

NovaOS darf aus der bloßen Existenz eines TPM keine automatische Vertrauensentscheidung ableiten.

```text
TPM Evidence
     ↓
Nova Trust Policy
     ↓
Decision
```

## TPM-Erkennung

Die Plattformschicht erkennt verfügbare TPM-Implementierungen und stellt sie über eine gemeinsame Schnittstelle bereit.

Unterstützt werden können:

```text
Discrete TPM
Firmware TPM
Virtual TPM
```

Die konkrete Implementierung muss für höhere Boot-Komponenten möglichst transparent bleiben.

## PCR

Platform Configuration Registers werden zur kryptografischen Abbildung des Bootzustands verwendet.

```text
Measurement
    ↓
PCR Extend
    ↓
New PCR State
```

PCR-Werte werden nicht direkt überschrieben, sondern durch definierte Extend-Operationen weiterentwickelt.

## Measured Boot

Das TPM kann die Messkette des Nova Measured Boot hardwaregestützt absichern.

```text
Bootloader
   ↓ Measure
Kernel
   ↓ Measure
Configuration
   ↓
PCR State
```

Das Measurement Log muss mit den relevanten PCR-Zuständen korrelierbar sein.

## Schlüsselbindung

Schlüssel können an definierte Plattform- oder Bootzustände gebunden werden.

```text
Key
 ↓
TPM Policy
 ↓
Expected PCR State
 ↓
Unseal
```

Dies kann beispielsweise zum Schutz von Datenträger- oder Systemschlüsseln verwendet werden.

## Attestation

NovaOS soll TPM-basierte Attestation unterstützen können.

```text
PCR State
+
Measurement Evidence
+
Attestation Key
       ↓
Attestation Evidence
```

Die Gegenstelle entscheidet anhand ihrer eigenen Trust Policy, wie diese Evidenz bewertet wird.

## TPM-Schlüssel

TPM-gebundene Schlüssel können verwendet werden für:

```text
Attestation
Sealing
Device Identity
Boot Trust
Cryptographic Operations
```

Private Schlüssel sollen das TPM nicht verlassen müssen, sofern Hardware und Anwendungsfall dies unterstützen.

## Fehler und Verfügbarkeit

TPM-Zustände müssen eindeutig unterscheidbar sein:

```text
Available
Unavailable
Disabled
Failed
Unknown
```

Dabei gilt:

```text
Unknown ≠ Trusted
Unavailable ≠ Compromised
```

Ein fehlendes TPM darf nicht automatisch einen Systemstart verhindern, sofern die aktive Security Policy kein TPM verlangt.

## Recovery

TPM-gebundene Geheimnisse müssen definierte Recovery-Strategien besitzen, wenn der Anwendungsfall dies erfordert.

Beispiele:

```text
Recovery Key
Authorized Recovery Environment
Key Migration
Administrative Recovery
```

Recovery darf die zugrunde liegende Security Policy nicht unkontrolliert umgehen.

## Normative Anforderungen

1. NovaOS SOLL TPM 2.0 als hardwaregestützte Trust-Komponente unterstützen.
2. TPM-Zugriffe MÜSSEN über definierte System- und HAL-Schnittstellen erfolgen.
3. Measured Boot MUSS TPM-PCRs verwenden können.
4. PCR-Messungen MÜSSEN mit dem Measurement Log korrelierbar sein.
5. TPM-gebundene Schlüssel SOLLEN an definierte Policies und Zustände gebunden werden können.
6. NovaOS SOLL TPM-basierte Attestation unterstützen können.
7. TPM-Evidenz DARF NICHT automatisch mit Vertrauen oder Autorisierung gleichgesetzt werden.
8. Private TPM-Schlüssel SOLLEN das TPM nicht verlassen, sofern dies technisch möglich ist.
9. Fehlende oder fehlerhafte TPM-Unterstützung MUSS explizit erkennbar sein.
10. `Unknown` DARF NICHT als vertrauenswürdiger TPM-Zustand behandelt werden.
11. Ein fehlendes TPM DARF den Boot nur verhindern, wenn eine Hard Security Policy dies verlangt.
12. Recovery für TPM-gebundene Geheimnisse MUSS kontrolliert und explizit definiert sein.

## Abhängigkeiten

- `NPSPEC-BOOT-ARCH-0001`
- `NPSPEC-BOOT-SECURE-0001`
- `NPSPEC-BOOT-MEASURED-0001`
- `NPSPEC-BOOT-VERIFIED-0001`
- `NPSPEC-HAL-PLATFORM-0001`
- `NPSPEC-HAL-FIRMWARE-0001`
- `ADR-BOOT-0008`
- `ADR-TRUST-0001_Trust_als_Evidence_basiertes_System`

## Ergebnis

```text
Boot Measurements
       ↓
TPM
       ↓
Protected Evidence / Keys
       ↓
Nova Trust Evaluation
```

NovaOS erhält damit eine hardwaregestützte Vertrauensbasis für Boot-Messungen, Schlüsselbindung und Attestation, ohne das TPM selbst zur alleinigen Instanz für Vertrauen oder Autorisierung zu machen.