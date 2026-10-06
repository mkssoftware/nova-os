# NPSPEC-COMPAT-API-0001 – Nova Compatibility API Translation

## Status

Angenommen

## Kategorie

Compatibility / API

## Zweck

NovaOS definiert eine kontrollierte API-Kompatibilitätsschicht für Programme, die Schnittstellen fremder Betriebssysteme, Plattformen oder Laufzeitumgebungen erwarten.

Fremde APIs werden durch Compatibility Provider auf native NovaOS-Schnittstellen und Capabilities abgebildet, ohne Bestandteil der nativen NovaOS-API zu werden.

## Grundprinzipien

```text
Foreign API ≠ Nova API
API Compatibility ≠ ABI Compatibility
API Call ≠ Syscall
API Availability ≠ Authority
API Translation ≠ Permission
Foreign Privilege ≠ Nova Authority
```

## Modell

```text
Foreign Application
        ↓
Foreign API Call
        ↓
Compatibility Personality
        ↓
API Compatibility Provider
        ↓
Semantic Translation
        ↓
Nova API / Capability
        ↓
NovaOS
```

## API-Profil

Eine unterstützte API wird durch ein versioniertes Profil beschrieben:

```text
CompatibilityAPI
├── API-ID
├── PersonalityID
├── Version
├── Interfaces
├── Functions
├── DataTypes
├── Constants
├── ErrorModel
└── CompatibilityLevel
```

Mehrere Versionen derselben API dürfen parallel unterstützt werden.

## Übersetzung

API-Aufrufe werden anhand ihrer Semantik und nicht ausschließlich anhand ihres Funktionsnamens übersetzt.

```text
Foreign Function
├── Parameters
├── Data Types
├── Flags
├── State
└── Expected Semantics
        ↓
Translation
        ↓
Nova Operation
```

Die Compatibility-Schicht darf Parameter, Datenstrukturen, Zustände und Rückgabewerte konvertieren.

## Direkte Abbildung

Existiert eine semantisch passende NovaOS-Funktion:

```text
Foreign API
    ↓
Validation
    ↓
Nova API / Capability
```

soll eine möglichst direkte Abbildung verwendet werden.

## Zusammengesetzte Abbildung

Eine fremde API-Funktion darf aus mehreren NovaOS-Operationen aufgebaut werden:

```text
Foreign API Call
      ↓
Compatibility Provider
      ├── Nova Operation A
      ├── Nova Operation B
      └── Nova Operation C
```

Die erwartete beobachtbare Semantik soll soweit möglich erhalten bleiben.

## Zustandsbehaftete APIs

Fremde API-Zustände dürfen innerhalb der jeweiligen Personality verwaltet werden:

```text
API State
Handle Tables
Configuration
Sessions
Contexts
Runtime Objects
```

Dieser Zustand bleibt vom nativen NovaOS-Systemzustand getrennt.

## Handles und Objekte

Fremde API-Objekte und Handles werden auf kontrollierte Compatibility-Objekte beziehungsweise autorisierte NovaOS-Handles abgebildet.

```text
Foreign Handle
      ↓
Compatibility Object
      ↓
Authorized Nova Handle
```

Die Abbildung darf keine zusätzliche Authority erzeugen.

## Capability-Integration

Benötigt eine fremde API geschützte Systemfunktionen, erfolgt der Zugriff über NovaOS-Capabilities.

```text
Foreign API Request
        ↓
Semantic Operation
        ↓
Capability / Policy Check
        ↓
Authorized Nova Operation
```

Beispielsweise erzeugt das Vorhandensein einer Netzwerk-API keine Netzwerkberechtigung.

## API und Syscall

API- und Syscall-Kompatibilität bleiben getrennte Ebenen:

```text
Application
    ↓
Foreign API
    ↓
API Translation
    ↓
Foreign Syscall
        oder
Native Nova Operation
```

Ein Compatibility Provider darf einen API-Aufruf direkt auf native NovaOS-Mechanismen abbilden, ohne eine fremde Syscall-Schicht zu durchlaufen.

## Fehlerübersetzung

NovaOS-Fehler werden auf das Fehlermodell der fremden API abgebildet.

Der ursprüngliche NovaOS-Fehler soll für Diagnose und Introspection erhalten bleiben.

## Kompatibilitätszustand

API-Funktionen können gekennzeichnet werden als:

```text
NativeMapping
Translated
Emulated
PartiallySupported
Unsupported
Unavailable
```

## Normative Anforderungen

1. Fremde APIs DÜRFEN nicht automatisch Bestandteil der nativen NovaOS-API werden.
2. Jede Compatibility API MUSS eindeutig identifizierbar und versionierbar sein.
3. API-Kompatibilität MUSS von ABI- und Syscall-Kompatibilität getrennt bleiben.
4. API-Aufrufe MÜSSEN anhand ihrer Semantik übersetzt werden.
5. Parameter und Datenstrukturen MÜSSEN vor der nativen Verwendung validiert werden.
6. Fremde Handles MÜSSEN kontrolliert auf NovaOS-Ressourcen abgebildet werden.
7. API-Übersetzung DARF keine Authority erzeugen.
8. Geschützte Operationen MÜSSEN Capability- und Policy-Prüfungen unterliegen.
9. Eine API-Funktion DARF aus mehreren nativen Operationen emuliert werden.
10. Nicht unterstützte Funktionen MÜSSEN kontrolliert fehlschlagen.
11. Mehrere API-Versionen MÜSSEN parallel unterstützt werden können.
12. API-Profil, Provider, Version und Unterstützungsgrad MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-COMPAT-ABI-0001`
- `NPSPEC-COMPAT-PERSONALITY-0001`
- `NPSPEC-COMPAT-SYSCALL-0001`
- `NPSPEC-COMPAT-POSIX-0001`
- `NPSPEC-COMPAT-LINUX-0001`
- `NPSPEC-COMPAT-WIN32-0001`
- `NPSPEC-SYSTEM-RUNTIME-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`

## Ergebnis

NovaOS kann fremde Programmierschnittstellen über versionierte Compatibility API Provider auf native APIs und Capabilities abbilden. Fremde Funktionen, Datentypen, Handles, Zustände und Fehlermodelle bleiben Bestandteil ihrer Compatibility Personality, während native NovaOS-Schnittstellen, Capabilities und Sicherheitsregeln unabhängig und maßgeblich bleiben.