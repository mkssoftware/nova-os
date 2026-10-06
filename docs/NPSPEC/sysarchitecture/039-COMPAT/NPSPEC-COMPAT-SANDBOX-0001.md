# NPSPEC-COMPAT-SANDBOX-0001 – Nova Compatibility Sandbox

## Status

Angenommen

## Kategorie

Compatibility / Sandbox

## Zweck

NovaOS definiert eine isolierte Ausführungsumgebung für fremde, ältere oder nicht ausreichend vertrauenswürdige Software innerhalb der Compatibility-Architektur.

Die Compatibility Sandbox begrenzt Ressourcen, Systemzugriffe und Auswirkungen einer Compatibility-Umgebung, ohne deren fremdes Sicherheits- oder Privilegmodell auf NovaOS zu übertragen.

## Grundprinzipien

```text
Sandbox ≠ Personality
Sandbox ≠ Permission
Sandbox ≠ Trust
Guest Privilege ≠ Nova Authority
Visibility ≠ Authority
Compatibility ≠ Security Exception
Isolation > Compatibility
```

## Modell

```text
Foreign Software
      ↓
Compatibility Personality
      ↓
Compatibility Sandbox
├── Isolated Process Context
├── Namespace View
├── Capability Set
├── Resource Limits
├── Compatibility Runtime
└── Security Policy
      ↓
NovaOS
```

## Sandbox-Kontext

Eine Sandbox wird mindestens beschrieben durch:

```text
CompatibilitySandbox
├── SandboxID
├── PrincipalID
├── PersonalityID
├── SecurityContext
├── NamespaceContext
├── CapabilitySet
├── ResourceBudget
└── IsolationProfile
```

## Isolation

Die Sandbox muss Compatibility-Software von kritischen NovaOS-Komponenten trennen.

Isolierbare Bereiche umfassen insbesondere:

```text
Memory
Processes
Filesystem / Namespace
IPC
Devices
Network
Configuration
Temporary Data
Runtime State
```

Die konkrete Isolation darf abhängig vom Risiko der ausgeführten Software verstärkt werden.

## Namespace

Eine Sandbox erhält eine kontrollierte Namespace-Ansicht:

```text
Nova Namespace
      ↓
Authorized Projection
      ↓
Compatibility Personality
      ↓
Sandbox Namespace
```

Sichtbare Ressourcen erzeugen keine automatische Authority.

Schreibbare Bereiche müssen explizit bestimmt werden.

## Capabilities

Die Sandbox erhält ausschließlich die für ihre Ausführung notwendigen Capabilities.

```text
Available Authority
       ∩
Policy
       ∩
Sandbox Restrictions
       =
Effective Authority
```

Die Sandbox darf Authority nur weiter einschränken, niemals erweitern.

## Ressourcen

NovaOS darf pro Sandbox Grenzen für Ressourcen festlegen:

```text
CPU
Memory
Storage
I/O
Network
GPU
Handles
Processes
Threads
Temporary Data
```

Überschreitungen müssen kontrolliert behandelt werden können.

## Compatibility-Komponenten

Innerhalb der Sandbox dürfen insbesondere ausgeführt werden:

```text
Legacy Runtime
ABI Translation
API Translation
Syscall Translation
Binary Translation
CPU Emulation
Hardware Emulation
Legacy Drivers
Protocol Providers
```

Besonders privilegierte Komponenten dürfen zusätzliche isolierte Domains erhalten.

## Kommunikation

Kommunikation über Sandbox-Grenzen erfolgt ausschließlich über kontrollierte NovaOS-Mechanismen.

```text
Sandbox
   ↓
Authorized IPC / Capability
   ↓
Nova Resource
```

Handles und Capabilities dürfen nur explizit und attenuiert übertragen werden.

## Fehlerbegrenzung

Fehler innerhalb einer Sandbox sollen auf ihren Ausführungskontext begrenzt bleiben.

NovaOS darf eine Sandbox:

```text
Suspend
Terminate
Restart
Revoke
Quarantine
Recover
```

ohne andere Compatibility-Umgebungen zu beeinträchtigen.

## Trust

Der Isolation Profile darf anhand des Trust-Zustands angepasst werden.

```text
Trusted      → Normal Isolation
Restricted   → Strong Isolation
Unknown      → Strong Isolation
Untrusted    → Maximum Allowed Isolation
Invalid      → Block
Revoked      → Block / Terminate
```

Trust allein erzeugt jedoch keine Authority.

## Normative Anforderungen

1. Compatibility-Software MUSS innerhalb eines expliziten Security Context ausgeführt werden.
2. Die Sandbox MUSS Speicher-, Namespace- und Ressourcenisolation unterstützen.
3. Fremde Privilegmodelle DÜRFEN keine NovaOS-Authority erzeugen.
4. Effektive Authority MUSS auf bereits vorhandene und durch Policy erlaubte Authority begrenzt bleiben.
5. Sandbox-Isolation DARF Authority niemals erweitern.
6. Schreibzugriffe auf NovaOS-Ressourcen MÜSSEN explizit autorisiert sein.
7. IPC über Sandbox-Grenzen MUSS kontrolliert erfolgen.
8. Capability- und Handle-Übertragung MUSS explizit und attenuiert erfolgen.
9. Ressourcenlimits MÜSSEN pro Sandbox durchsetzbar sein.
10. Fehler SOLLEN auf die betroffene Sandbox begrenzt bleiben.
11. Isolation MUSS abhängig von Trust und Risikoprofil verstärkt werden können.
12. SandboxID, Personality, Authority, Ressourcenverbrauch und Zustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-COMPAT-PERSONALITY-0001`
- `NPSPEC-COMPAT-LEGACYRUNTIME-0001`
- `NPSPEC-COMPAT-LEGACYDRIVER-0001`
- `NPSPEC-NAMESPACE-PROCESS-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-CAPABILITY-TRUST-0001`
- `NPSPEC-CAPABILITY-RESOURCES-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-SYSTEM-POLICY-0001`

## Ergebnis

NovaOS kapselt Compatibility-Software in kontrollierten Sandbox-Umgebungen. Fremde Programme, Runtimes, Übersetzungsschichten und Legacy-Komponenten erhalten nur die ausdrücklich erlaubten Ressourcen und Capabilities, während Fehler, fremde Privilegmodelle und unsichere Komponenten von der nativen NovaOS-Systemumgebung isoliert bleiben.