# ADR-COMPAT-0001 – Native Nova-Architektur nicht durch POSIX, Linux oder Win32 bestimmen

## Status

Angenommen

## Kategorie

Compatibility / Architecture / ABI / Legacy

## Kontext

NovaOS soll bestehende Software und etablierte Ökosysteme nutzen können.

Dafür sind insbesondere Schnittstellen und Kompatibilität zu:

```text
POSIX
Linux
Win32
```

relevant.

Diese Systeme besitzen jedoch eigene Architekturmodelle, APIs, Sicherheitskonzepte und historische Einschränkungen.

Würde NovaOS seine interne Architektur danach ausrichten, könnten zentrale NovaOS-Konzepte wie Capabilities, Semantic Types, Objects, Execution Contracts und deklaratives State Management eingeschränkt werden.

## Entscheidung

Die **native NovaOS-Architektur wird unabhängig von POSIX, Linux und Win32 definiert**.

```text
Applications
     ↓
Compatibility Layer
     ↓
Native Nova APIs / Capabilities
     ↓
NovaOS Architecture
```

Kompatibilität wird als Übersetzungs- und Adaptionsschicht realisiert und bestimmt nicht das interne Systemmodell.

## Native Architektur

NovaOS-native Software verwendet bevorzugt die eigenen Systemkonzepte:

```text
Nova Objects
Capabilities
Semantic Types
Execution Contracts
Declarative State
Named Resources
Explicit Security Contexts
```

Diese bilden die langfristige Architektur des Systems.

## POSIX-Kompatibilität

POSIX-Schnittstellen dürfen bereitgestellt werden, werden intern jedoch auf native NovaOS-Mechanismen abgebildet.

```text
POSIX API
   ↓
POSIX Compatibility Layer
   ↓
Nova Capabilities
```

POSIX-Prozess-, Datei- oder Berechtigungsmodelle werden nicht automatisch zu internen NovaOS-Grundprinzipien.

## Linux-Kompatibilität

Linux-Kompatibilität wird als eigenständige Compatibility Domain behandelt.

Linux-spezifische Konzepte wie:

```text
Linux Syscalls
/proc
/sys
ioctl
Namespaces
Linux Device Semantics
```

dürfen emuliert oder übersetzt werden, ohne den nativen NovaOS-Kernel danach auszurichten.

## Win32-Kompatibilität

Win32-Anwendungen werden ebenfalls über eine isolierte Compatibility Layer ausgeführt.

```text
Win32 API
    ↓
Compatibility Runtime
    ↓
Nova APIs
```

Win32 Handles, Registry-Semantik oder Windows-spezifische Objektmodelle werden nicht zu nativen NovaOS-Systemkonzepten.

## Compatibility Boundaries

Jede Compatibility Layer bildet eine klar definierte Architekturgrenze.

```text
Legacy Semantics
      ↓
Translation
      ↓
Validation
      ↓
Native Nova Semantics
```

Legacy-Verhalten darf native Sicherheits- oder Systemregeln nicht umgehen.

## Native Evolution

NovaOS darf seine native Architektur weiterentwickeln, auch wenn bestehende Fremd-APIs diese Konzepte nicht direkt unterstützen.

Die Compatibility Layer übernimmt die notwendige Übersetzung.

Damit wird verhindert, dass historische Entscheidungen fremder Betriebssysteme die langfristige Entwicklung von NovaOS begrenzen.

## Normative Anforderungen

1. Die native NovaOS-Architektur DARF NICHT durch POSIX, Linux oder Win32 bestimmt werden.
2. Native NovaOS-Schnittstellen SOLLEN NovaOS-eigene Architekturkonzepte direkt abbilden.
3. Fremde APIs MÜSSEN über definierte Compatibility Boundaries integriert werden.
4. Legacy-Semantik DARF native Security-, Safety- oder Sovereignty-Regeln nicht umgehen.
5. POSIX-Kompatibilität DARF kein POSIX-internes Systemmodell erzwingen.
6. Linux-Kompatibilität DARF keine Linux-Kernelarchitektur im NovaOS-Kernel voraussetzen.
7. Win32-Kompatibilität DARF keine Windows-interne Architektur zum NovaOS-Systemmodell machen.
8. Compatibility Layer MÜSSEN unabhängig von der nativen Architektur evolvierbar sein.
9. Neue NovaOS-Funktionen DÜRFEN ohne entsprechende POSIX-, Linux- oder Win32-Abbildung existieren.

## Konsequenzen

### Positiv

- NovaOS bleibt architektonisch unabhängig,
- Legacy-Kompatibilität begrenzt keine zukünftigen Systemkonzepte,
- moderne NovaOS-Mechanismen können ohne Altlasten entwickelt werden,
- mehrere Compatibility Environments können parallel existieren.

### Negativ

- Compatibility Layer benötigen zusätzliche Übersetzungslogik,
- vollständige Fremdsystem-Kompatibilität kann komplex sein,
- bestimmte Legacy-Anwendungen können spezielle Emulation benötigen.

## Abhängigkeiten

- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0019_Evolvierbar`
- `ADR-CAPABILITY-0001`
- `ADR-OBJECT-0001`
- `ADR-SEMANTIC-0001`

## Ergebnis

NovaOS folgt dem Prinzip:

```text
Native Nova Architecture
        ↓
is the foundation

POSIX / Linux / Win32
        ↓
are compatibility environments
```

Kompatibilität wird auf NovaOS aufgebaut – **NovaOS wird nicht auf die Architektur seiner Kompatibilitätsschichten aufgebaut**.