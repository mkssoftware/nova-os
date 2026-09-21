# ADR-COMPAT-0003 – Compatibility Personalities

## Status

Angenommen

## Kategorie

Compatibility / Runtime / Personality Layer

## Kontext

NovaOS soll Anwendungen aus unterschiedlichen Betriebssystem- und API-Welten ausführen können, ohne deren Architektur zum nativen NovaOS-Systemmodell zu machen.

POSIX-, Linux- und Win32-Anwendungen erwarten unterschiedliche:

```text
APIs
ABIs
Syscalls
Process Semantics
Filesystem Views
Error Codes
Environment Models
```

Eine einzige universelle Compatibility Layer würde diese unterschiedlichen Semantiken unnötig vermischen.

## Entscheidung

NovaOS führt **Compatibility Personalities** ein.

Eine Personality stellt einer Anwendung eine definierte Fremdsystem-Umgebung bereit und übersetzt deren Verhalten auf native NovaOS-Mechanismen.

```text
Application
    ↓
Compatibility Personality
    ↓
Nova APIs / Capabilities
    ↓
NovaABI
    ↓
NovaOS
```

## Personalities

Mögliche Personalities sind beispielsweise:

```text
Nova Native
POSIX
Linux
Win32
```

Weitere Personalities können später ergänzt werden.

Jede Personality bleibt logisch von der nativen NovaOS-Architektur getrennt.

## Personality Selection

Beim Start eines Programms bestimmt NovaOS die benötigte Ausführungsumgebung.

```text
Executable
    ↓
Identify Environment
    ↓
Select Personality
    ↓
Create Execution Context
```

Die Auswahl kann anhand von Executable-Format, Metadaten oder expliziter Konfiguration erfolgen.

## Übersetzung

Eine Compatibility Personality kann Fremdsystem-Aufrufe übersetzen.

Beispiel:

```text
Foreign API / Syscall
        ↓
Personality
        ↓
Native Nova Operation
```

Die Übersetzung darf native Sicherheits- und Ressourcenregeln nicht umgehen.

## Isolation

Compatibility Personalities besitzen keine besonderen Systemrechte allein aufgrund ihrer Kompatibilitätsfunktion.

Sie unterliegen weiterhin:

```text
Capabilities
Permissions
Resource Budgets
Sandboxing
Data Sovereignty
Trust Requirements
```

Eine Anwendung sieht nur die Umgebung, die ihre Personality bereitstellt.

## Personality-Versionen

Personalities sind versionierbar.

```text
Linux Personality v1
Linux Personality v2

Win32 Personality v1
Win32 Personality v2
```

Dadurch können unterschiedliche Kompatibilitätsstände parallel unterstützt werden, ohne die NovaABI entsprechend verändern zu müssen.

## Native Anwendungen

Native NovaOS-Anwendungen benötigen keine Legacy-Personality.

```text
Native Application
       ↓
Nova APIs
       ↓
NovaABI
```

Die native Nova-Umgebung bleibt der bevorzugte Ausführungspfad.

## Normative Anforderungen

1. NovaOS MUSS mehrere Compatibility Personalities unterstützen können.
2. Jede Personality MUSS eine klar definierte Fremdsystem-Semantik kapseln.
3. Personalities DÜRFEN die native NovaOS-Architektur nicht bestimmen.
4. Fremdsystem-Aufrufe MÜSSEN auf native NovaOS-Mechanismen übersetzt oder kontrolliert emuliert werden.
5. Compatibility Personalities MÜSSEN den normalen NovaOS-Sicherheitsregeln unterliegen.
6. Eine Personality DARF keine zusätzlichen Rechte allein durch ihre Existenz erhalten.
7. Personalities MÜSSEN unabhängig versionierbar sein.
8. Unterschiedliche Personality-Versionen SOLLEN parallel unterstützt werden können.
9. Neue Personalities MÜSSEN ohne grundlegende Änderung der NovaABI ergänzbar sein.

## Konsequenzen

### Positiv

- klare Trennung verschiedener Kompatibilitätsmodelle,
- POSIX-, Linux- und Win32-Semantik vermischen sich nicht mit NovaOS,
- Compatibility Environments können unabhängig weiterentwickelt werden,
- unterschiedliche Kompatibilitätsstände können parallel existieren.

### Negativ

- jede Personality benötigt eigene Übersetzungslogik,
- Unterschiede zwischen Fremdsystemen müssen explizit modelliert werden,
- vollständige Kompatibilität kann je Personality unterschiedlich weit reichen.

## Abhängigkeiten

- `ADR-COMPAT-0001_Native_Nova_Architektur_nicht_durch_POSIX_Linux_oder_Win32_bestimmen`
- `ADR-COMPAT-0002_Kleine_versionierte_NovaABI`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0019_Evolvierbar`
- `ADR-CAPABILITY-0001`

## Ergebnis

NovaOS kapselt Fremdsystem-Kompatibilität in eigenständigen Personalities:

```text
POSIX ─┐
Linux ─┼─→ Compatibility Personalities ─→ Native NovaOS
Win32 ─┘
```

Damit kann NovaOS unterschiedliche Software-Ökosysteme unterstützen, ohne deren Architektur in den nativen Systemkern zu übernehmen.