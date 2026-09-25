# NPSPEC-BOOT-MEASURED-0001 – Nova Measured Boot

## Status

Angenommen

## Kategorie

Boot / Security / Trust / Measurement

## Zweck

NovaOS erfasst während des Bootvorgangs kryptografische Messwerte sicherheitsrelevanter Komponenten, damit der entstandene Bootzustand später nachvollzogen und bewertet werden kann.

```text
Boot Component
      ↓
Hash / Measurement
      ↓
Measurement Log
      ↓
Trust Evaluation
```

## Grundprinzip

Measured Boot entscheidet nicht selbst, ob eine Komponente ausgeführt werden darf.

```text
Secure Boot   → Verify → Allow / Reject
Measured Boot → Measure → Record / Report
```

Beide Mechanismen können kombiniert werden.

## Messkette

Sicherheitsrelevante Bootstufen sollen vor oder während ihrer Aktivierung gemessen werden.

```text
Firmware
   ↓
Bootloader
   ↓
Kernel
   ↓
Critical Boot State
```

Messungen müssen eindeutig der jeweiligen Komponente oder dem jeweiligen Zustand zugeordnet werden können.

## Messwerte

Ein Measurement-Eintrag beschreibt mindestens:

```text
ComponentID
ComponentVersion
Hash
Algorithm
BootPhase
Sequence
```

Optional können Provenance-, Signatur- oder Plattforminformationen referenziert werden.

## TPM

Ist ein Trusted Platform Module verfügbar, sollen Messwerte in geeignete Platform Configuration Registers erweitert werden können.

```text
Component
   ↓
Hash
   ↓
TPM Extend
   ↓
PCR State
```

Die konkrete TPM-Version darf nicht die allgemeine Nova-Bootarchitektur bestimmen.

## Measurement Log

Zusätzlich zu Hardwaremessungen führt NovaOS ein strukturiertes Measurement Log.

```text
Measurement Log
 ├── Firmware
 ├── Bootloader
 ├── Configuration
 ├── Kernel
 └── Boot State
```

Das Log muss mit dem gemessenen Bootzustand korrelierbar sein.

## Trust Evaluation

Nach dem Boot können Messwerte für weitere Entscheidungen verwendet werden.

Beispiele:

```text
Attestation
System Integrity
Diagnostics
Security Policy
Forensics
```

Ein Messwert allein erzeugt jedoch keine Berechtigung.

## Änderungen

Unterschiedliche legitime Konfigurationen können unterschiedliche Messwerte erzeugen.

NovaOS darf daher nicht grundsätzlich erwarten:

```text
Valid System = One Fixed Hash
```

Trust Policies müssen Versionen, Konfigurationen und erlaubte Zustände berücksichtigen können.

## Systeme ohne TPM

Measured Boot muss auch ohne TPM grundlegende Softwaremessungen durchführen können.

```text
TPM Available
   → Hardware-backed Measurements

TPM Unavailable
   → Software Measurement Log
```

Die unterschiedliche Vertrauensstärke muss erkennbar bleiben.

## Normative Anforderungen

1. NovaOS MUSS sicherheitsrelevante Boot-Komponenten messen können.
2. Messungen MÜSSEN kryptografische Hashverfahren verwenden.
3. Messwerte MÜSSEN eindeutig einer Komponente oder einem Bootzustand zugeordnet werden können.
4. Ein strukturiertes Measurement Log MUSS bereitgestellt werden können.
5. Ein vorhandenes TPM SOLL für hardwaregestützte Bootmessungen verwendet werden können.
6. Measured Boot MUSS von Secure Boot logisch getrennt bleiben.
7. Messwerte DÜRFEN NICHT automatisch mit Vertrauen oder Autorisierung gleichgesetzt werden.
8. Measurement Logs SOLLEN nach dem Boot für autorisierte Systemkomponenten verfügbar sein.
9. NovaOS SOLL Remote Attestation auf Grundlage der Messwerte ermöglichen können.
10. Systeme ohne TPM DÜRFEN NICHT dieselbe Vertrauensstärke wie hardwaregestützte Messungen vortäuschen.
11. Messungen SOLLEN versionierte und legitime Systemänderungen berücksichtigen können.
12. Messdaten MÜSSEN gegen unautorisierte Manipulation soweit technisch möglich geschützt werden.

## Abhängigkeiten

- `NPSPEC-BOOT-ARCH-0001`
- `NPSPEC-BOOT-CHAIN-0001`
- `NPSPEC-BOOT-SECURE-0001`
- `NPSPEC-HAL-FIRMWARE-0001`
- `ADR-BOOT-0006`
- `ADR-TRUST-0001_Trust_als_Evidence_basiertes_System`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`

## Ergebnis

```text
Boot Chain
    ↓
Measurements
    ↓
Protected Evidence
    ↓
Trust Evaluation / Attestation
```

NovaOS erhält damit einen nachvollziehbaren Nachweis darüber, welche Komponenten und Zustände tatsächlich am Bootvorgang beteiligt waren.