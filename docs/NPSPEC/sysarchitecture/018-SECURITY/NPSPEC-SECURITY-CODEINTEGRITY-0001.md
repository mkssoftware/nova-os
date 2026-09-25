# NPSPEC-SECURITY-CODEINTEGRITY-0001 – Nova Code Integrity

## Status

Angenommen

## Kategorie

Security / Integrity / Code Integrity

## Zweck

NovaOS definiert Code Integrity zur Sicherstellung, dass ausgeführter Code dem zuvor verifizierten Inhalt entspricht und nicht unautorisiert verändert wurde.

```text
Code
  ↓
Integrity Verification
  ↓
Execution
  ↓
Protected Runtime State
```

Code Integrity ergänzt Code Signing, Verified Boot, Speicherprotektion und MAC.

## Grundprinzipien

```text
Signed ≠ Unmodified at Runtime
Loaded ≠ Trusted Forever
Writable Code ≠ Safe Code
Integrity ≠ Authorization
Integrity Verification ≠ Malware Detection
```

## Integritätsmodell

Ein ausführbares Artefakt besitzt mindestens:

```text
CodeIntegrity
├── ArtifactID
├── Expected Hash
├── Actual Hash
├── Verification State
└── Policy
```

Mögliche Zustände:

```text
Verified
Modified
Invalid
Unknown
Unverified
```

## Ladeprüfung

Vor Ausführung geschützten Codes muss dessen Integrität geprüft werden können.

```text
Artifact
   ↓
Code Signing
   ↓
Hash Verification
   ↓
Load
   ↓
Execute
```

Die Prüfung muss sich auf den tatsächlich geladenen Inhalt beziehen.

## Laufzeitschutz

Verifizierte Codebereiche sollen nach dem Laden nicht gleichzeitig beschreibbar und ausführbar sein.

```text
Writable XOR Executable
```

NovaOS soll W^X (`Write XOR Execute`) für geschützte Codebereiche erzwingen.

Unkontrollierte Änderungen an ausführbarem Speicher müssen verhindert oder erkannt werden.

## Dynamischer Code

JIT, Runtime-Codegenerierung und ähnliche Mechanismen müssen explizit behandelt werden.

```text
Writable
   ↓
Generate
   ↓
Validate / Seal
   ↓
Executable
```

Ein Speicherbereich darf nicht dauerhaft gleichzeitig beschreibbar und ausführbar bleiben.

## Bibliotheken und Module

Nachgeladene Komponenten müssen denselben Integritätsregeln unterliegen wie der Hauptprozess.

Dies gilt insbesondere für:

```text
Libraries
Plugins
Drivers
Kernel Modules
Runtime Extensions
```

## Policy

Code-Integrity-Anforderungen dürfen abhängig sein von:

```text
Security Domain
Artifact Type
Trust Level
Execution Context
```

Kernel, Treiber und sicherheitskritische Dienste dürfen strengere Anforderungen besitzen als normale Anwendungen.

## Manipulation

Wird eine Integritätsverletzung erkannt, muss NovaOS abhängig von der Policy reagieren können:

```text
Block Execution
Terminate Component
Quarantine
Rollback
Recovery
Audit
```

Eine beschädigte sicherheitskritische Komponente darf nicht still weiterverwendet werden.

## Self-Healing

Code Integrity darf mit NovaOS Self-Healing zusammenarbeiten.

```text
Integrity Failure
      ↓
Known-Good Version
      ↓
Transactional Restore
      ↓
Verify
```

Wiederherstellung darf nur aus verifizierten Quellen erfolgen.

## Introspection

NovaOS soll mindestens darstellen können:

```text
ArtifactID
Expected Hash
Verification State
Signature State
Runtime Integrity State
Policy
Violation
```

## Normative Anforderungen

1. NovaOS MUSS die Integrität ausführbaren Codes prüfen können.
2. Die Prüfung MUSS den tatsächlich geladenen Code erfassen können.
3. Verifizierter Code SOLL gegen unautorisierte Laufzeitänderungen geschützt werden.
4. Geschützte Speicherbereiche SOLLEN W^X verwenden.
5. Dynamisch erzeugter Code MUSS kontrolliert aktiviert werden.
6. Bibliotheken, Treiber und Module MÜSSEN in das Integritätsmodell integrierbar sein.
7. Integritätsverletzungen MÜSSEN erkennbar sein.
8. Sicherheitskritisch beschädigter Code DARF NICHT still weiterverwendet werden.
9. Self-Healing DARF nur verifizierte Ersatzkomponenten verwenden.
10. Integritätszustände SOLLEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-SECURITY-CODESIGNING-0001`
- `NPSPEC-SECURITY-MAC-0001`
- `NPSPEC-MEMORY-PROTECTION-0001`
- `NPSPEC-BOOT-VERIFIED-0001`
- `NPSPEC-BOOT-MEASURED-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `ADR-SECURITY-0010`

## Ergebnis

```text
Verified Artifact
       ↓
Integrity Check
       ↓
Protected Loading
       ↓
Runtime Protection
       ↓
Trusted Code State
```

NovaOS erhält damit eine durchgängige Code-Integrity-Schicht, die Manipulationen zwischen Verifikation, Laden und Ausführung verhindert oder erkennt und beschädigte Komponenten kontrolliert behandeln kann.