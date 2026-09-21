# ADR-COMPAT-0004 – POSIX als Kompatibilitätsschicht

## Status

Angenommen

## Kategorie

Compatibility / POSIX / API / Runtime

## Kontext

POSIX stellt eine weit verbreitete Schnittstelle für portable Unix- und Linux-Software bereit.

Eine native Übernahme des POSIX-Modells würde jedoch zentrale NovaOS-Konzepte an historische Unix-Abstraktionen wie:

```text
File Descriptors
Signals
Fork
Unix Permissions
Path-basierte Ressourcen
```

binden.

NovaOS benötigt POSIX-Kompatibilität, ohne POSIX zum internen Architekturmodell zu machen.

## Entscheidung

NovaOS implementiert POSIX als **Compatibility Layer innerhalb einer POSIX Compatibility Personality**.

```text
POSIX Application
       ↓
POSIX API
       ↓
POSIX Personality
       ↓
Nova APIs / Capabilities
       ↓
NovaABI
       ↓
NovaOS
```

POSIX-Semantik wird soweit möglich auf native NovaOS-Mechanismen übersetzt.

## POSIX API

Die Kompatibilitätsschicht kann standardisierte POSIX-Funktionen bereitstellen, beispielsweise:

```text
File Operations
Processes
Threads
Signals
Sockets
Synchronization
Time
Environment
```

Der Umfang der unterstützten POSIX-Version muss explizit dokumentiert und versioniert sein.

## Übersetzung

POSIX-Konzepte werden auf native NovaOS-Konzepte abgebildet.

Beispiel:

```text
POSIX File Descriptor
        ↓
Compatibility Mapping
        ↓
Nova Object / Capability
```

Die POSIX-Darstellung bleibt dabei auf die Compatibility Personality beschränkt.

## Prozessmodell

POSIX-Prozesssemantik wird emuliert oder auf NovaOS-Ausführungsmechanismen abgebildet.

Insbesondere:

```text
fork()
exec()
signals
process groups
```

dürfen nicht voraussetzen, dass NovaOS intern dasselbe Prozessmodell verwendet.

## Dateisystem

POSIX-Anwendungen erhalten eine POSIX-kompatible Dateisystemansicht.

```text
POSIX Path View
      ↓
VFS / Compatibility Mapping
      ↓
Nova Objects
```

Pfadbasierte POSIX-Identität ersetzt nicht die stabile objektbasierte Identität von NovaOS.

## Berechtigungen

POSIX-Berechtigungen werden auf NovaOS-Sicherheitsmechanismen abgebildet.

```text
POSIX Permissions
       ↓
Compatibility Policy
       ↓
Nova Capabilities / Security
```

POSIX-Rechte dürfen keine stärkere Autorität erzeugen als der zugrunde liegende NovaOS-Sicherheitskontext erlaubt.

## POSIX Extensions

Nicht standardisierte Unix- oder Linux-Erweiterungen sind nicht automatisch Bestandteil der POSIX-Schicht.

Linux-spezifische Funktionen gehören in die entsprechende Linux Compatibility Personality.

```text
POSIX Standard → POSIX Personality
Linux Specific → Linux Personality
```

## Normative Anforderungen

1. POSIX MUSS als Compatibility Layer und nicht als native NovaOS-Architektur behandelt werden.
2. Unterstützte POSIX-Versionen und Funktionen MÜSSEN explizit definiert sein.
3. POSIX-Aufrufe SOLLEN auf native NovaOS-Mechanismen übersetzt werden.
4. POSIX File Descriptors DÜRFEN nicht zur nativen Ressourcenidentität von NovaOS werden.
5. POSIX-Pfade DÜRFEN die objektbasierte Identität von NovaOS nicht ersetzen.
6. POSIX-Berechtigungen DÜRFEN NovaOS-Capability- und Security-Regeln nicht umgehen.
7. POSIX-Prozesssemantik DARF kein identisches internes NovaOS-Prozessmodell voraussetzen.
8. Linux-spezifische Erweiterungen SOLLEN von der eigentlichen POSIX-Schicht getrennt bleiben.
9. POSIX-Kompatibilität MUSS unabhängig von der NovaABI versionierbar und evolvierbar bleiben.

## Konsequenzen

### Positiv

- portable Unix-Software kann leichter auf NovaOS gebracht werden,
- bestehende POSIX-Codebasen können wiederverwendet werden,
- NovaOS bleibt intern unabhängig vom Unix-Systemmodell,
- POSIX und Linux-Kompatibilität bleiben sauber getrennt.

### Negativ

- einige POSIX-Konzepte benötigen aufwendige Übersetzung,
- vollständige POSIX-Semantik kann zusätzliche Runtime-Unterstützung erfordern,
- semantische Unterschiede zwischen POSIX und NovaOS müssen explizit behandelt werden.

## Abhängigkeiten

- `ADR-COMPAT-0001_Native_Nova_Architektur_nicht_durch_POSIX_Linux_oder_Win32_bestimmen`
- `ADR-COMPAT-0002_Kleine_versionierte_NovaABI`
- `ADR-COMPAT-0003_Compatibility_Personalities`
- `ADR-CAPABILITY-0001`
- `ADR-OBJECT-0001`

## Ergebnis

NovaOS unterstützt POSIX nach dem Prinzip:

```text
POSIX Software
      ↓
POSIX Compatibility
      ↓
Native NovaOS
```

POSIX ermöglicht die Ausführung bestehender Software, ohne dass Unix-Semantik zur grundlegenden Architektur von NovaOS wird.