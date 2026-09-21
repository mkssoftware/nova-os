# ADR-COMPAT-0007 – Execution Compatibility Ladder

## Status

Angenommen

## Kategorie

Compatibility / Execution / Runtime / Fallback

## Kontext

NovaOS soll Software aus unterschiedlichen Ökosystemen ausführen können.

Dabei existieren verschiedene Möglichkeiten mit unterschiedlichen Eigenschaften bezüglich:

```text
Performance
Compatibility
Isolation
Overhead
Integration
Security
```

Eine einzelne Kompatibilitätsstrategie ist deshalb nicht für jede Anwendung geeignet.

## Entscheidung

NovaOS verwendet eine **Execution Compatibility Ladder**.

Für nicht-native Software wird bevorzugt die leichteste Ausführungsform verwendet, welche die benötigte Kompatibilität korrekt bereitstellen kann.

```text
Native Execution
      ↓
API / ABI Translation
      ↓
Compatibility Runtime
      ↓
Container / Isolated Environment
      ↓
Virtual Machine
      ↓
Emulation
```

Die Leiter beschreibt eine bevorzugte Reihenfolge, keine zwingende starre Kette.

## Stufe 1 – Native Execution

Native NovaOS-Software wird direkt über Nova APIs und NovaABI ausgeführt.

```text
Nova Application
      ↓
Nova APIs
      ↓
NovaABI
```

Dies ist der bevorzugte Ausführungspfad.

## Stufe 2 – API / ABI Translation

Wenn möglich, wird Fremdsoftware über Compatibility Personalities übersetzt.

Beispiele:

```text
POSIX → Nova
Linux ABI → Nova
Win32 → Nova
```

Dadurch bleibt der Overhead gering und die Anwendung kann native NovaOS-Ressourcen verwenden.

## Stufe 3 – Compatibility Runtime

Benötigt eine Anwendung umfangreichere Fremdsystem-Semantik, kann eine zusätzliche Runtime verwendet werden.

Diese stellt beispielsweise Bibliotheken, Frameworks oder virtuelle Systemdienste bereit.

## Stufe 4 – Isolierte Umgebung

Benötigt Software eine stärker abgeschlossene Fremdsystemumgebung, kann NovaOS eine isolierte Compatibility Environment bereitstellen.

```text
Application
     ↓
Compatibility Environment
     ↓
Controlled Nova Resources
```

Die Isolation bleibt den NovaOS-Sicherheitsregeln untergeordnet.

## Stufe 5 – Virtualisierung

Ist eine ausreichend genaue Systemumgebung erforderlich, kann ein vollständiges Gastbetriebssystem virtualisiert werden.

```text
Application
     ↓
Guest OS
     ↓
Virtual Machine
     ↓
NovaOS
```

Dies erhöht Kompatibilität, aber auch Ressourcenbedarf und Integrationsaufwand.

## Stufe 6 – Emulation

Für fremde Hardwarearchitekturen oder nicht direkt ausführbare Software kann Emulation eingesetzt werden.

```text
Foreign Architecture
        ↓
Emulation
        ↓
NovaOS
```

Dies ist typischerweise die aufwendigste Ausführungsform.

## Auswahl

NovaOS kann anhand von:

```text
Executable Type
Required ABI
Required APIs
CPU Architecture
Compatibility Requirements
Security Constraints
Available Providers
```

eine geeignete Stufe bestimmen.

Eine explizite Nutzer- oder Administratorentscheidung kann die Auswahl innerhalb zulässiger Systemgrenzen beeinflussen.

## Fallback

Scheitert eine bevorzugte Stufe, darf NovaOS eine andere zulässige Stufe versuchen.

```text
Translation fails
       ↓
Runtime
       ↓
Virtualization
```

Ein Fallback darf keine Hard Constraints abschwächen.

## Normative Anforderungen

1. NovaOS MUSS mehrere Compatibility-Ausführungsformen unterstützen können.
2. Native Ausführung SOLL für native NovaOS-Software bevorzugt werden.
3. Fremdsoftware SOLL möglichst über die leichteste ausreichend kompatible Stufe ausgeführt werden.
4. Compatibility Personalities SOLLEN vor vollständiger Virtualisierung bevorzugt werden, wenn sie die benötigte Semantik korrekt bereitstellen.
5. Jede Stufe MUSS NovaOS-Security- und Capability-Regeln respektieren.
6. Fallback zwischen Stufen DARF keine Hard Constraints umgehen.
7. Die gewählte Compatibility-Stufe MUSS für Diagnose und Observability erkennbar sein.
8. Eine Anwendung DARF nicht automatisch zusätzliche Autorität erhalten, weil eine stärkere Compatibility-Stufe verwendet wird.
9. Neue Compatibility-Techniken MÜSSEN in die Ladder integrierbar sein, ohne die native Architektur zu verändern.

## Konsequenzen

### Positiv

- hohe Kompatibilität bei möglichst geringem Overhead,
- einfache Anwendungen benötigen keine vollständige VM,
- schwierige Legacy-Fälle können trotzdem unterstützt werden,
- neue Compatibility-Techniken lassen sich ergänzen.

### Negativ

- mehrere Ausführungspfade müssen gepflegt werden,
- unterschiedliche Stufen können unterschiedliches Verhalten zeigen,
- automatische Auswahl benötigt zuverlässige Compatibility-Metadaten.

## Abhängigkeiten

- `ADR-COMPAT-0001_Native_Nova_Architektur_nicht_durch_POSIX_Linux_oder_Win32_bestimmen`
- `ADR-COMPAT-0002_Kleine_versionierte_NovaABI`
- `ADR-COMPAT-0003_Compatibility_Personalities`
- `ADR-COMPAT-0004_POSIX_als_Kompatibilitätsschicht`
- `ADR-COMPAT-0005_Linux_ABI_über_Syscall_Translation`
- `ADR-COMPAT-0006_Win32_über_API_und_ABI_Translation`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-EXECUTION-0010_Automatic_Execution_Planning`

## Ergebnis

NovaOS verwendet eine abgestufte Kompatibilitätsstrategie:

```text
Native
  ↓
Translation
  ↓
Runtime
  ↓
Isolation
  ↓
Virtualization
  ↓
Emulation
```

Dadurch wird für jede Anwendung die benötigte Kompatibilität bereitgestellt, ohne grundsätzlich die schwerste oder komplexeste Ausführungsform verwenden zu müssen.