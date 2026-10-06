# NPSPEC-COMPAT-INTROSPECTION-0001 – Nova Compatibility Introspection

## Status

Angenommen

## Kategorie

Compatibility / Introspection

## Zweck

NovaOS definiert eine einheitliche Introspection-Schnittstelle für die Compatibility-Architektur.

System, Nutzer, Diagnosewerkzeuge und autorisierte Agenten können damit erkennen, welche Compatibility-Komponenten verwendet werden, warum sie benötigt werden und welchen aktuellen Zustand sie besitzen.

## Grundprinzipien

```text
Introspection ≠ Authority
Visibility ≠ Permission
Diagnostic Access ≠ Modification
Compatibility State ≠ Trust State
Reported State ≠ Source of Authority
Sensitive Metadata ≠ Public Metadata
```

## Modell

```text
Compatibility Environment
├── Personality
├── ABI
├── API
├── Syscall Translation
├── Runtime
├── Binary Translation
├── Emulation
└── Sandbox
        ↓
Compatibility Introspection
        ↓
Structured State
        ↓
Diagnostics / UI / Agent / Logging
```

## Introspection-Modell

Eine Compatibility-Umgebung soll mindestens folgende Informationen bereitstellen können:

```text
CompatibilityInfo
├── CompatibilityID
├── PrincipalID
├── PersonalityID
├── TargetPlatform
├── Architecture
├── Components[]
├── Versions[]
├── Providers[]
├── CompatibilityState
├── TrustState
├── IsolationState
└── Diagnostics
```

## Komponenten

Für einzelne Compatibility-Komponenten können unter anderem sichtbar sein:

```text
ComponentID
Type
Version
Provider
State
CompatibilityLevel
Fallback
TranslationMode
ResourceUsage
Errors
```

## Ausführungspfad

NovaOS soll nachvollziehbar machen können, wie eine fremde Operation ausgeführt wird.

Beispiel:

```text
Win32 Application
      ↓
Win32 API Provider
      ↓
ABI Translation
      ↓
Syscall Translation
      ↓
Nova Capability
      ↓
NovaOS
```

Dadurch können unnötige Übersetzungs- oder Emulationsschichten erkannt werden.

## Compatibility-Zustand

Einheitliche Zustände dürfen beispielsweise umfassen:

```text
Native
Compatible
Translated
Emulated
PartiallySupported
Degraded
Unsupported
Unavailable
Blocked
Failed
```

Der Zustand soll auf die verursachende Komponente zurückführbar sein.

## Fehlerdiagnose

Compatibility-Fehler sollen strukturierte Diagnoseinformationen liefern:

```text
Component
Operation
ErrorCode
CompatibilityLayer
Provider
Expected
Actual
FallbackAttempted
Cause
```

Fehler einer fremden API dürfen zusätzlich zum übersetzten Fremdfehler den internen NovaOS-Diagnosegrund erhalten.

## Performance

Introspection darf sichtbar machen, wenn Compatibility zusätzliche Kosten verursacht, beispielsweise durch:

```text
ABI Translation
Binary Translation
CPU Emulation
Hardware Emulation
Additional Copies
Sandbox Boundaries
```

Introspection darf jedoch nicht selbst zu einer erheblichen Belastung des normalen Ausführungspfads werden.

## Sicherheit

Introspection unterliegt dem NovaOS-Berechtigungsmodell.

Sensible Informationen wie:

```text
Memory Addresses
Credentials
Capability Tokens
Protected Handles
Private Data
Security Secrets
```

dürfen nicht allein aufgrund von Compatibility Introspection offengelegt werden.

Capability Tokens und andere Authority-Träger dürfen niemals als normale Diagnosewerte ausgegeben werden.

## Normative Anforderungen

1. Compatibility-Komponenten MÜSSEN ihren grundlegenden Zustand introspektierbar machen.
2. Personality, Version und verwendete Provider MÜSSEN ermittelbar sein.
3. Translation-, Emulations- und Fallback-Zustände MÜSSEN unterscheidbar sein.
4. Compatibility-, Trust- und Isolation-State MÜSSEN getrennt dargestellt werden.
5. Fehler MÜSSEN auf die verursachende Compatibility-Schicht zurückführbar sein.
6. Der effektive Compatibility-Ausführungspfad SOLL nachvollziehbar sein.
7. Introspection DARF keine Authority erzeugen oder erweitern.
8. Introspection MUSS bestehende Capability- und Policy-Grenzen beachten.
9. Capability Tokens, Credentials und vergleichbare Secrets DÜRFEN nicht offengelegt werden.
10. Diagnoseinformationen SOLLEN maschinenlesbar bereitgestellt werden.
11. Introspection SOLL nur minimale zusätzliche Laufzeitkosten verursachen.
12. Änderungen des Compatibility-Zustands SOLLEN für Logging und Diagnose beobachtbar sein.

## Abhängigkeiten

- `NPSPEC-COMPAT-PERSONALITY-0001`
- `NPSPEC-COMPAT-VERSIONING-0001`
- `NPSPEC-COMPAT-TESTING-0001`
- `NPSPEC-COMPAT-SANDBOX-0001`
- `NPSPEC-CAPABILITY-TRUST-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`

## Ergebnis

NovaOS macht Compatibility-Umgebungen als strukturierte und diagnostizierbare Systemzustände sichtbar. Personality, Provider, Versionen, Übersetzungen, Emulationen, Fehler und Fallbacks können nachvollzogen werden, ohne dadurch Sicherheitsgrenzen zu umgehen oder zusätzliche Authority zu erzeugen.