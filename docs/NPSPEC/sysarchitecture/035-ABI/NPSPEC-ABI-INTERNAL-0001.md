# NPSPEC-ABI-INTERNAL-0001 – Nova Internal ABI

## Status

Angenommen

## Kategorie

ABI / Internal Interface / Kernel Architecture

## Zweck

NovaOS definiert eine interne ABI für binäre Schnittstellen zwischen Kernelkomponenten, Treibern, Systemmodulen und anderen eng gekoppelten Systembestandteilen.

```text
Kernel Component
      ↓
Internal ABI
      ↓
Kernel Component / Driver / Module
```

Die Internal ABI ermöglicht modulare Entwicklung und den Austausch einzelner Komponenten, ohne diese Schnittstellen als dauerhaft stabile öffentliche ABI festzuschreiben.

## Grundprinzipien

```text
Internal ABI ≠ Stable Public ABI
Internal ABI ≠ Unspecified Interface
Internal ABI ≠ Direct Structure Access
Internal ABI ≠ Authority
Internal Compatibility ≠ Permanent Compatibility
```

Interne Schnittstellen dürfen sich weiterentwickeln, müssen aber innerhalb ihrer definierten Compatibility Boundary eindeutig spezifiziert sein.

## Internal ABI Model

```text
InternalABI
├── InterfaceID
├── Version
├── Architecture
├── InterfaceType
├── FeatureSet
└── CompatibilityPolicy
```

Optional:

```text
ProviderID
RequiredCapabilities
ExecutionContract
LifecycleState
DependencySet
```

## Einsatzbereiche

Die Internal ABI kann verwendet werden für:

```text
Kernel Modules
Kernel Subsystems
Drivers
HAL Components
Filesystem Drivers
Network Components
Runtime Components
System Modules
Compatibility Layers
```

Nicht jede interne Funktionsgrenze benötigt eine ABI.

## Interface Types

NovaOS unterscheidet beispielsweise:

```text
Kernel Internal ABI
Driver ABI
HAL ABI
Module ABI
Runtime ABI
Compatibility ABI
```

Diese können unterschiedliche Stabilitätsgarantien besitzen.

## Versionierung

Jede relevante interne ABI muss versionierbar sein.

```text
InterfaceID
    +
Version
```

Provider und Consumer müssen vor Bindung prüfen, ob ihre Versionen kompatibel sind.

```text
Consumer
   ↓
Required Interface Version
   ↓
Compatibility Check
   ↓
Provider
```

## Compatibility Policy

Interne Schnittstellen können definieren:

```text
Exact Version
Compatible Minor Version
Version Range
Feature-based Compatibility
```

Interne ABI-Kompatibilität muss nicht über unbegrenzte NovaOS-Versionen garantiert werden.

## Data Structures

Interne Strukturen dürfen nicht automatisch vollständig über ABI-Grenzen geteilt werden.

Bevorzugt:

```text
Versioned Structure
Opaque Handle
Explicit Accessor
Typed Interface
```

Statt:

```text
Direct Access to Private Kernel Structure
```

Dadurch können Implementierungen verändert werden, ohne unnötige Abhängigkeiten zu erzeugen.

## Function Interfaces

Binäre Funktionsschnittstellen müssen relevante Eigenschaften definieren:

```text
Calling Convention
Argument Types
Return Types
Ownership
Lifetime
Error Semantics
Concurrency Semantics
```

Compilerabhängiges Verhalten darf nicht unbeabsichtigt Bestandteil des Contracts werden.

## Handles

Interne Objekte sollen über kontrollierte Referenzen oder Handles weitergegeben werden, wenn direkte Pointer-Kopplung nicht erforderlich ist.

```text
Component A
    ↓
Internal Handle
    ↓
Component B
```

Direkte Pointer dürfen nur innerhalb ausdrücklich definierter gemeinsamer Trust- und Lifetime-Grenzen verwendet werden.

## Memory Ownership

Ownership muss an internen ABI-Grenzen eindeutig sein:

```text
Owned
Borrowed
Shared
Transferred
Pinned
```

Insbesondere bei Hot Replacement darf keine unklare Ownership zwischen alter und neuer Komponente bestehen.

## Capability Integration

Eine interne ABI verleiht keine zusätzliche Authority.

```text
Interface Available
       ≠
Operation Authorized
```

Treiber, Module und Services müssen weiterhin die erforderlichen Capabilities besitzen.

## Live Replacement

Die Internal ABI muss NovaOS Live Evolution unterstützen können.

```text
Old Component
      ↓
Quiesce
      ↓
Validate New ABI
      ↓
Transfer State
      ↓
New Component
      ↓
Verify
```

ABI-Kompatibilität allein garantiert dabei keine State-Kompatibilität.

```text
ABI Compatible ≠ State Compatible
```

## Driver Integration

Treiber können eine definierte interne Driver ABI verwenden.

```text
Driver
  ↓
Nova Driver ABI
  ↓
Driver Framework
  ↓
Kernel / HAL
```

Usermode-Treiber sollen nach Möglichkeit stärker isolierte IPC- und Capability-Schnittstellen verwenden.

## HAL Integration

Architekturspezifische Implementierungen können über eine definierte HAL ABI vom restlichen Kernel getrennt werden.

```text
Kernel
  ↓
HAL ABI
  ↓
x86 / ARM64 / Future Architecture
```

## Feature Negotiation

Komponenten sollen optionale Funktionen explizit aushandeln können.

```text
Required Features
        ∩
Provider Features
        ↓
Negotiated Interface
```

Fehlende optionale Features dürfen nicht automatisch zum vollständigen Bindungsfehler führen.

## Dependency Management

Interne ABI-Abhängigkeiten müssen erkennbar sein.

```text
Module A
├── ABI X >= 2
└── ABI Y Feature Z
```

Dadurch kann NovaOS vor Laden oder Austausch einer Komponente ihre Kompatibilität prüfen.

## Failure Handling

Ist eine erforderliche Internal ABI inkompatibel:

```text
Compatibility Failure
        ↓
Reject Component
        ↓
Alternative Provider
or
Previous Component
or
Degraded Mode
```

Eine inkompatible Komponente darf nicht auf Verdacht geladen werden.

## Security

Geladener Binärcode ist nicht allein aufgrund einer kompatiblen ABI vertrauenswürdig.

Vor Aktivierung können zusätzlich erforderlich sein:

```text
Code Integrity
Signature
Trust Validation
Capabilities
Isolation Policy
Version Policy
```

```text
ABI Compatible ≠ Trusted
```

## Debugging

Debug-Builds dürfen zusätzliche interne ABI-Prüfungen durchführen:

```text
Structure Size
Version
Magic
Ownership
Lifetime
Contract Assertions
```

Diese Prüfungen dürfen in Release-Builds optimiert werden, sofern notwendige Sicherheitsprüfungen erhalten bleiben.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
InterfaceID
Version
Provider
Consumer
Supported Features
Compatibility State
Lifecycle State
Dependencies
```

Interne Adressen oder sensitive Daten dürfen dadurch nicht unnötig offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS definierte interne Binärschnittstellen unterstützen können.
2. Internal ABI und Stable Public ABI MÜSSEN getrennte Stability Contracts besitzen.
3. Interne ABI-Schnittstellen MÜSSEN eindeutig identifizierbar sein.
4. Relevante interne ABI-Schnittstellen MÜSSEN versionierbar sein.
5. Provider und Consumer MÜSSEN ihre Kompatibilität vor Bindung prüfen können.
6. Internal ABI DARF langfristige Binärkompatibilität NICHT automatisch garantieren.
7. Private Kernelstrukturen SOLLEN NICHT unnötig Bestandteil interner ABI-Verträge werden.
8. Calling Convention und Datenlayout MÜSSEN für binäre Grenzen eindeutig sein.
9. Memory Ownership MUSS an relevanten ABI-Grenzen definiert sein.
10. Direkte Pointer MÜSSEN auf ausdrücklich erlaubte Trust- und Lifetime-Grenzen beschränkt werden.
11. Internal ABI DARF keine implizite Authority erzeugen.
12. Capability-Regeln MÜSSEN auch für interne Komponenten gelten.
13. ABI Compatibility und State Compatibility MÜSSEN getrennt behandelt werden.
14. Live Replacement MUSS ABI-Kompatibilität vor Umschaltung prüfen können.
15. Optionale Features SOLLEN über Feature Negotiation ausgehandelt werden.
16. ABI-Abhängigkeiten MÜSSEN vor Aktivierung einer Komponente validierbar sein.
17. Inkompatible Komponenten DÜRFEN NICHT ungeprüft aktiviert werden.
18. ABI-Kompatibilität DARF NICHT als Trust-Nachweis interpretiert werden.
19. Security- und Integrity-Prüfungen MÜSSEN unabhängig von ABI-Kompatibilität bleiben.
20. Internal ABI MUSS architekturspezifische Profile unterstützen können.
21. Interne ABI-Zustände SOLLEN autorisiert introspektierbar sein.
22. Internal ABI MUSS kontrollierte Evolution der NovaOS-Komponenten ermöglichen.

## Abhängigkeiten

- `NPSPEC-ABI-NOVA-0001`
- `NPSPEC-ABI-SYSCALL-0001`
- `NPSPEC-ABI-STABLE-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-CAPABILITY-DRIVER-0001`
- `NPSPEC-DRIVER-FRAMEWORK-0001`
- `NPSPEC-DRIVER-HOTRELOAD-0001`
- `NPSPEC-DRIVER-LIVEREPLACE-0001`
- `NPSPEC-HAL-PLATFORM-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `NPSPEC-TRUST-SOFTWARE-0001`
- `ADR-ARCH-0144`

## Ergebnis

```text
Component A
     ↓
Versioned Internal ABI
     ↓
Compatibility + Security Validation
     ↓
Component B
     ↓
Controlled Evolution / Replacement
```

NovaOS erhält damit eine klar definierte interne ABI-Schicht, die Kernelkomponenten, Treiber und Systemmodule binär voneinander entkoppelt, ohne die langfristigen Stabilitätsgarantien der öffentlichen Nova ABI auf interne Implementierungsdetails auszudehnen.