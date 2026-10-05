# NPSPEC-WORKSPACE-CAPABILITY-0001 – Nova Workspace Capability

## Status

Angenommen

## Kategorie

Workspace / Capability

## Zweck

NovaOS definiert, wie Capabilities innerhalb eines Workspace verwendet, begrenzt und an dessen Lebenszyklus gebunden werden.

Ein Workspace kann einen eigenen Capability-Kontext besitzen, erzeugt jedoch selbst keine Authority.

## Grundprinzipien

```text
Workspace ≠ Authority
Workspace Membership ≠ Permission
Capability Requirement ≠ Granted Capability
Workspace Capability ⊆ Granted Authority
```

Capabilities innerhalb eines Workspace folgen dem allgemeinen NovaOS-Capability-Modell.

## Capability-Kontext

Ein Workspace kann einen eigenen Capability Context besitzen:

```text
User Authority
      ↓
Workspace Context
      ↓
Program / Solution / Task
```

Dabei dürfen Rechte eingeschränkt, aber nicht ohne zusätzliche Autorisierung erweitert werden.

## Scope

Capabilities können auf einen Workspace begrenzt werden.

Beispiele:

```text
Workspace A
├── Read Object X
├── Write Object Y
└── Use Network Provider Z
```

Dieselbe Anwendung kann in einem anderen Workspace einen anderen Capability-Kontext besitzen.

## Ressourcen

Workspace Relations oder Manifest-Einträge erzeugen keine Berechtigung.

```text
Workspace Resource
       ↓
ObjectID
       ↓
Capability Check
       ↓
Authorized Handle
```

Die Authority muss unabhängig von der Workspace-Zugehörigkeit gültig sein.

## Programme und Solutions

Programme und Solutions erhalten innerhalb eines Workspace nur die für ihren jeweiligen Kontext autorisierten Capabilities.

Eine Solution darf benötigte Capabilities deklarieren, erhält diese jedoch erst nach erfolgreicher Berechtigungsprüfung.

Custom-NovaLang-Code kann keine zusätzliche Authority selbst erzeugen.

## Delegation

Ein Workspace darf vorhandene Authority kontrolliert an untergeordnete Komponenten delegieren.

```text
Workspace Capability
        ↓
Attenuation
        ↓
Program / Solution
```

Delegierte Authority darf niemals stärker sein als die ursprüngliche Authority.

## Lebenszyklus

Workspace-spezifische Capabilities können an den Workspace-Lebenszyklus gebunden werden:

```text
Open
  ↓
Activate Capabilities
  ↓
Workspace Active
  ↓
Close
  ↓
Revoke / Release
```

Persistenter Workspace State darf keine aktiven Capability Handles als gewöhnliche Daten konservieren.

## Wiederherstellung

Beim Wiederherstellen eines Workspace müssen benötigte Capabilities erneut auf Gültigkeit geprüft werden.

```text
Load Workspace
      ↓
Resolve Requirements
      ↓
Validate Authority
      ↓
Create Capability Context
```

Gespeicherte Anforderungen sind keine gespeicherten Berechtigungen.

## Normative Anforderungen

1. Ein Workspace DARF selbst keine Authority erzeugen.
2. Workspace-Mitgliedschaft DARF keine Berechtigung implizieren.
3. Workspace-Capabilities MÜSSEN auf bestehender autorisierter Authority basieren.
4. Workspace-spezifische Authority MUSS begrenzbar sein.
5. Delegierte Capabilities DÜRFEN die ursprüngliche Authority nicht erweitern.
6. Programme und Solutions MÜSSEN unterschiedliche Capability-Kontexte pro Workspace besitzen können.
7. Capability-Anforderungen DÜRFEN im Workspace Manifest gespeichert werden.
8. Gespeicherte Anforderungen DÜRFEN NICHT als gewährte Authority interpretiert werden.
9. Workspace-spezifische Capabilities MÜSSEN widerrufbar sein.
10. Beim Wiederherstellen MUSS die erforderliche Authority erneut validiert werden.

## Abhängigkeiten

- `NPSPEC-WORKSPACE-MODEL-0001`
- `NPSPEC-WORKSPACE-MANIFEST-0001`
- `NPSPEC-WORKSPACE-STATE-0001`
- `NPSPEC-WORKSPACE-RELATION-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`
- `NPSPEC-CAPABILITY-DELEGATION-0001`
- `NPSPEC-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`

## Ergebnis

NovaOS kann Capabilities gezielt an einen Workspace und dessen Lebenszyklus binden. Programme, Solutions und Aufgaben erhalten dadurch nur die für ihren jeweiligen Arbeitskontext notwendige Authority, ohne dass Workspace-Mitgliedschaft oder gespeicherter Zustand selbst Berechtigungen erzeugen.