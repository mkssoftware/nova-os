# ADR-COMPAT-0006 – Win32 über API- und ABI-Translation

## Status

Angenommen

## Kategorie

Compatibility / Win32 / API / ABI

## Kontext

Viele Windows-Anwendungen verwenden Win32- und Windows-spezifische Schnittstellen, die weit über portable APIs hinausgehen.

Dazu gehören beispielsweise:

```text
Win32 API
NT-style Handles
PE Executables
DLL Loading
Registry
Windows Threads
Synchronization
Windowing
COM
```

Eine direkte Übernahme dieser Architektur würde NovaOS an historische Windows-Konzepte binden.

## Entscheidung

NovaOS unterstützt Win32 über eine **Win32 Compatibility Personality mit API- und ABI-Translation**.

```text
Windows Application
        ↓
Win32 API / ABI
        ↓
Win32 Personality
        ↓
API / ABI Translation
        ↓
Nova APIs / Capabilities
        ↓
NovaABI
        ↓
NovaOS
```

Windows-Semantik wird übersetzt oder emuliert, ohne zum nativen NovaOS-Systemmodell zu werden.

## API Translation

Win32-Funktionsaufrufe werden auf geeignete NovaOS-Funktionen abgebildet.

Beispiel:

```text
CreateFile()
    ↓
Win32 Translation
    ↓
Nova Object / Capability
```

Dabei bleibt das für die Anwendung erwartete Win32-Verhalten innerhalb der Compatibility Personality erhalten.

## ABI Translation

Die Personality berücksichtigt relevante ABI-Eigenschaften wie:

```text
Calling Conventions
Data Structures
Handles
Error Codes
Thread Environment
Executable Format
```

Diese Eigenschaften bleiben von der nativen NovaABI getrennt.

## PE und DLLs

Windows-Programme können über einen kompatiblen Loader für:

```text
PE
PE+
DLL
```

geladen werden.

DLL-Abhängigkeiten werden innerhalb der Win32-Umgebung aufgelöst und dürfen native NovaOS-Komponenten nicht unkontrolliert überschreiben.

## Windows Handles

Win32 Handles werden innerhalb der Personality auf native NovaOS-Objekte und Capabilities abgebildet.

```text
Win32 HANDLE
     ↓
Compatibility Mapping
     ↓
Nova Object / Capability
```

Ein Windows Handle ist keine native NovaOS-Objektidentität.

## Registry

Registry-Zugriffe werden als Compatibility Service bereitgestellt.

```text
Windows Registry API
        ↓
Virtual Registry View
        ↓
Nova Configuration / Storage
```

NovaOS benötigt intern keine Windows Registry.

## Nicht direkt abbildbare Funktionen

Win32-Funktionen ohne direktes NovaOS-Äquivalent dürfen:

```text
Translate
Emulate
Compose
Virtualize
Reject
```

Nicht unterstützte Funktionen müssen definiertes Windows-kompatibles Fehlerverhalten liefern.

## Sicherheit

Die Win32 Personality ist eine Security Boundary.

Windows-Anwendungen unterliegen weiterhin:

```text
Capabilities
Permissions
Sandboxing
Resource Budgets
Trust Requirements
Data Sovereignty
```

Windows-Kompatibilität darf keine zusätzlichen nativen Rechte erzeugen.

## Versionierung

Die unterstützte Win32-Kompatibilität wird unabhängig von der NovaABI versioniert.

```text
Win32 Compatibility Version
            ≠
NovaABI Version
```

Dadurch kann die Windows-Kompatibilität erweitert werden, ohne die native Kernel-ABI entsprechend anzupassen.

## Normative Anforderungen

1. Win32-Kompatibilität MUSS außerhalb der nativen NovaOS-Architektur implementiert werden.
2. Win32-APIs MÜSSEN über eine definierte Translation Boundary verarbeitet werden.
3. Windows-ABI-Eigenschaften DÜRFEN die native NovaABI nicht bestimmen.
4. Win32 Handles MÜSSEN auf kontrollierte NovaOS-Ressourcen abgebildet werden.
5. Registry-Semantik DARF keine native NovaOS-Registry voraussetzen.
6. Nicht unterstützte Funktionen MÜSSEN definiert fehlschlagen.
7. Win32-Kompatibilität MUSS NovaOS-Capability- und Security-Regeln respektieren.
8. Die Win32 Compatibility Personality MUSS unabhängig versionierbar sein.
9. Neue Win32-Funktionen DÜRFEN keine Erweiterung der NovaABI erzwingen, sofern sie außerhalb des Kernels abbildbar sind.

## Konsequenzen

### Positiv

- bestehende Windows-Anwendungen können unterstützt werden,
- NovaOS bleibt unabhängig von der Windows-Architektur,
- Win32-Kompatibilität kann separat weiterentwickelt werden,
- native NovaOS-Sicherheitsmechanismen bleiben maßgeblich.

### Negativ

- Win32 besitzt umfangreiche und komplexe Semantik,
- bestimmte APIs benötigen aufwendige Emulation,
- vollständige Anwendungskompatibilität kann zusätzliche Runtime-Komponenten erfordern.

## Abhängigkeiten

- `ADR-COMPAT-0001_Native_Nova_Architektur_nicht_durch_POSIX_Linux_oder_Win32_bestimmen`
- `ADR-COMPAT-0002_Kleine_versionierte_NovaABI`
- `ADR-COMPAT-0003_Compatibility_Personalities`
- `ADR-CAPABILITY-0001`
- `ADR-OBJECT-0001`

## Ergebnis

Win32-Kompatibilität folgt dem Modell:

```text
Win32 API / ABI
       ↓
Translation / Emulation
       ↓
Native NovaOS Mechanisms
```

NovaOS kann damit Windows-Anwendungen unterstützen, ohne Win32 oder die Windows-Systemarchitektur zur Grundlage des eigenen Betriebssystems zu machen.