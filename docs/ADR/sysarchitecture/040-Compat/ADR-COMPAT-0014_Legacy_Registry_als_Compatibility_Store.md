# ADR-COMPAT-0014 – Legacy Registry als Compatibility Store

## Status

Angenommen

## Kategorie

Compatibility / Configuration / Win32 / Legacy

## Kontext

Windows-Anwendungen können von Registry-Semantik abhängig sein.

Sie erwarten beispielsweise:

```text
Keys
Values
Hives
Hierarchische Pfade
Per-User Configuration
Per-System Configuration
```

NovaOS benötigt jedoch keine native Windows-Registry als Bestandteil seiner eigenen Konfigurationsarchitektur.

Eine globale Registry würde fremde Legacy-Semantik unnötig in das native Systemmodell übernehmen.

## Entscheidung

NovaOS bildet die Windows Registry als **Compatibility Store innerhalb der Win32 Compatibility Personality** ab.

```text
Win32 Application
       ↓
Registry API
       ↓
Compatibility Store
       ↓
Nova Configuration / Storage
```

Der Compatibility Store stellt die erwartete Registry-Semantik bereit, ohne eine native NovaOS-Registry einzuführen.

## Registry View

Eine Anwendung erhält eine virtuelle Registry-Ansicht.

Beispiele:

```text
HKEY_LOCAL_MACHINE
HKEY_CURRENT_USER
HKEY_CLASSES_ROOT
```

Diese Strukturen sind Compatibility Views und keine nativen NovaOS-Konfigurationsobjekte.

## Speicherung

Registry-Daten werden auf geeignete native NovaOS-Storage- oder Configuration-Mechanismen abgebildet.

```text
Registry Key
     ↓
Compatibility Mapping
     ↓
Nova Object / Configuration Data
```

Das physische Speicherformat bleibt eine interne NovaOS-Entscheidung.

## Isolation

Compatibility Domains können eigene Registry Views besitzen.

```text
Application A → Registry View A
Application B → Registry View B
Shared Runtime → Shared View
```

Dadurch können Anwendungen unterschiedliche oder inkompatible Registry-Zustände parallel verwenden.

## Virtualisierung

Bestimmte Registry-Bereiche können:

```text
Private
Shared
Read-Only
Virtual
Redirected
```

bereitgestellt werden.

Legacy-Anwendungen müssen dadurch keine globalen Schreibrechte auf Systemkonfiguration erhalten.

## Zugriffskontrolle

Registry-Rechte werden auf native NovaOS-Autorisierung abgebildet.

```text
Registry Access
      ↓
Compatibility Validation
      ↓
Nova Capability / Permission
```

Ein Registry-Pfad erzeugt keine eigene Autorität.

## Native Konfiguration

Native NovaOS-Komponenten verwenden den Compatibility Store nicht als primären Konfigurationsmechanismus.

```text
Native Nova Software
        ↓
Nova Configuration Model

Win32 Software
        ↓
Registry Compatibility Store
```

Beide Modelle bleiben architektonisch getrennt.

## Migration und Export

Registry-Zustände dürfen importiert, exportiert, versioniert oder gesichert werden, sofern dies für Legacy-Kompatibilität erforderlich ist.

Dabei bleiben Security-, Privacy- und Data-Sovereignty-Regeln von NovaOS maßgeblich.

## Normative Anforderungen

1. NovaOS DARF die Windows Registry nicht als natives globales Konfigurationsmodell übernehmen.
2. Registry-Semantik MUSS innerhalb der Win32 Compatibility Personality bereitgestellt werden können.
3. Registry Keys und Hives MÜSSEN als Compatibility Views behandelt werden.
4. Registry-Daten MÜSSEN auf native NovaOS-Storage- oder Configuration-Mechanismen abbildbar sein.
5. Compatibility Domains MÜSSEN isolierte Registry Views besitzen können.
6. Registry-Zugriffe MÜSSEN den nativen NovaOS-Sicherheitsregeln unterliegen.
7. Registry-Pfade DÜRFEN keine implizite Autorität erzeugen.
8. Native NovaOS-Komponenten SOLLEN nicht von Registry-Semantik abhängig sein.
9. Unterschiedliche Legacy-Registry-Versionen oder Zustände SOLLEN parallel betreibbar sein.

## Konsequenzen

### Positiv

- Registry-abhängige Windows-Anwendungen können unterstützt werden,
- NovaOS benötigt keine native globale Registry,
- Registry-Zustände können pro Compatibility Domain isoliert werden,
- Legacy-Konfiguration bleibt vom nativen NovaOS-Modell getrennt.

### Negativ

- Registry-Semantik benötigt zusätzliche Übersetzungslogik,
- gemeinsam genutzte Registry-Bereiche erfordern kontrollierte Synchronisation.

## Abhängigkeiten

- `ADR-COMPAT-0003_Compatibility_Personalities`
- `ADR-COMPAT-0006_Win32_über_API_und_ABI_Translation`
- `ADR-COMPAT-0008_Legacy_Software_in_Compatibility_Domains`
- `ADR-COMPAT-0010_Legacy_Runtime_side_by_side`
- `ADR-COMPAT-0013_Legacy_Filesystem_Views_als_Projektion`
- `ADR-CAPABILITY-0001`
- `ADR-STATE-0006_State_Versioning_und_History`

## Ergebnis

NovaOS behandelt die Windows Registry als virtuelle Kompatibilitätsstruktur:

```text
Windows Registry Semantics
          ↓
Compatibility Store
          ↓
Native NovaOS Storage
```

Damit können Registry-abhängige Anwendungen funktionieren, ohne dass die Windows Registry Bestandteil der nativen NovaOS-Architektur wird.