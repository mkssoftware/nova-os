# ADR-COMPAT-0002 – Kleine versionierte NovaABI

## Status

Angenommen

## Kategorie

Compatibility / ABI / Kernel Interface / Evolvierbarkeit

## Kontext

Eine große, dauerhaft stabile Kernel-ABI würde interne Implementierungsdetails von NovaOS langfristig festschreiben.

Das erschwert:

```text
Kernel Evolution
Security Improvements
Performance Optimizations
Neue Architekturkonzepte
```

Gleichzeitig benötigen Userspace, Runtime und Compatibility Layer eine stabile Grenze zum System.

## Entscheidung

NovaOS definiert eine **kleine, klar versionierte native NovaABI**.

```text
Applications / Runtimes
        ↓
Nova APIs / Capabilities
        ↓
Small NovaABI
        ↓
Kernel
```

Die ABI enthält nur grundlegende Mechanismen, die tatsächlich eine stabile Kernelgrenze benötigen.

Höhere Funktionen werden außerhalb der Kernel-ABI über Capabilities, Services und Libraries bereitgestellt.

## ABI-Umfang

Die NovaABI soll sich auf primitive Systemmechanismen beschränken, beispielsweise:

```text
Execution
Memory Mapping
IPC
Capability Operations
Object Handles
Synchronization
Basic I/O
System Information
```

Komplexe Policies und High-Level-Funktionen gehören nicht in die stabile Kernel-ABI.

## Versionierung

Die NovaABI besitzt eine explizite Versionskennung.

```text
NovaABI
├── Version
├── Feature Set
└── Supported Operations
```

Komponenten müssen feststellen können, welche ABI-Version und welche optionalen Features verfügbar sind.

## Evolution

Neue Funktionen sollen bevorzugt **additiv** eingeführt werden.

```text
ABI v1
  ↓
ABI v2
  ↓
ABI v3
```

Bestehende ABI-Verträge werden nicht unnötig verändert.

Wenn inkompatible Änderungen notwendig sind, müssen sie über eine neue ABI-Version erfolgen.

## ABI und API

ABI und API werden getrennt betrachtet.

```text
Application
    ↓
High-Level Nova API
    ↓
Runtime / Libraries
    ↓
NovaABI
    ↓
Kernel
```

Die öffentliche NovaOS-API darf sich weiterentwickeln, ohne dass jede Änderung eine Kernel-ABI-Erweiterung benötigt.

## Compatibility Layer

POSIX-, Linux- und Win32-Kompatibilität werden auf die NovaABI und native NovaOS-Dienste abgebildet.

```text
POSIX / Linux / Win32
          ↓
Compatibility Layer
          ↓
Nova API / NovaABI
```

Fremde ABIs dürfen die native NovaABI nicht unnötig vergrößern.

## Feature Discovery

ABI-Nutzer dürfen nicht ausschließlich aus einer Versionsnummer auf einzelne Funktionen schließen müssen.

Optional verfügbare Mechanismen sollen explizit erkennbar sein.

```text
ABI Version
+
Feature Discovery
```

Dadurch können Kernel und Userspace unabhängig evolvieren.

## Normative Anforderungen

1. Die NovaABI MUSS klein gehalten werden.
2. Die NovaABI MUSS explizit versioniert sein.
3. Nur grundlegende Kernelmechanismen SOLLEN Bestandteil der stabilen ABI sein.
4. High-Level-Policies DÜRFEN NICHT unnötig in der Kernel-ABI festgeschrieben werden.
5. Neue ABI-Funktionen SOLLEN bevorzugt additiv eingeführt werden.
6. Inkompatible Änderungen MÜSSEN über eine neue ABI-Version erkennbar sein.
7. Optionale ABI-Funktionen MÜSSEN erkennbar bzw. abfragbar sein.
8. Nova API und NovaABI MÜSSEN getrennt evolvierbar bleiben.
9. Compatibility Layer DÜRFEN die native NovaABI nicht durch Fremdsystem-Semantik bestimmen.

## Konsequenzen

### Positiv

- geringe langfristige ABI-Last,
- Kernel bleibt intern evolvierbar,
- klare Grenze zwischen Mechanismus und höherer Systemfunktion,
- Compatibility Layer können unabhängig entwickelt werden,
- weniger dauerhaft festgeschriebene Kernel-Schnittstellen.

### Negativ

- High-Level-Funktionen benötigen zusätzliche Runtime- oder Service-Schichten,
- ABI-Versionierung und Feature Discovery müssen dauerhaft gepflegt werden.

## Abhängigkeiten

- `ADR-COMPAT-0001_Native_Nova_Architektur_nicht_durch_POSIX_Linux_oder_Win32_bestimmen`
- `ADR-ARCH-0002_Mechanism_und_Policy`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0019_Evolvierbar`
- `ADR-CAPABILITY-0001`

## Ergebnis

NovaOS verwendet keine große, historisch anwachsende Kernel-ABI, sondern:

```text
Small
+
Versioned
+
Extensible
NovaABI
```

Die NovaABI bildet eine stabile minimale Systemgrenze, ohne die interne Architektur von NovaOS langfristig festzuschreiben.