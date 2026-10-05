# NPSPEC-WORKSPACE-STATE-0001 – Nova Workspace State

## Status

Angenommen

## Kategorie

Workspace / State

## Zweck

NovaOS definiert den Workspace State als den aktuellen, wiederherstellbaren Arbeitszustand eines Workspace.

Der State ergänzt das Workspace Manifest um veränderliche Laufzeit- und Sitzungsinformationen.

## Grundprinzipien

```text
Workspace State ≠ Workspace Manifest
Workspace State ≠ User Data
Workspace State ≠ Authority
Workspace State ≠ Process Memory
```

Das Manifest beschreibt den Workspace. Der State beschreibt dessen aktuellen Arbeitszustand.

## State-Modell

```text
WorkspaceState
├── WorkspaceID
├── StateVersion
├── ActiveResources
├── UIState
├── NavigationState
├── SolutionState
├── ProgramState
└── SessionState
```

Nicht jeder Zustand muss persistent oder wiederherstellbar sein.

## Arbeitszustand

Der Workspace State kann beispielsweise enthalten:

```text
geöffnete Dateien
aktive Ressource
Fensterzustände
UI-Layout
Navigation
Auswahlzustände
aktive Solutions
laufende Arbeitskontexte
```

Die eigentlichen Nutzdaten bleiben getrennt vom Workspace State.

## Ressourcenreferenzen

Persistente Ressourcen sollen über stabile Identitäten referenziert werden.

```text
Workspace State
      ↓
ObjectID
      ↓
Current Location
```

Pfadänderungen oder Storage-Migration dürfen gespeicherte Referenzen nicht automatisch ungültig machen.

## Laufzeitstatus

Flüchtige Zustände dürfen gespeichert werden, sofern eine sichere Wiederherstellung möglich ist.

Prozessspeicher oder beliebige interne Programmzustände werden nicht automatisch persistiert.

Programme und Solutions müssen explizit angeben, welche Zustände wiederherstellbar sind.

## Persistenz

Workspace State darf:

```text
temporär
sitzungsgebunden
persistent
```

sein.

Persistenter State muss versioniert und konsistent gespeichert werden.

Zusammengehörige Änderungen sollen transaktional geschrieben werden.

## Wiederherstellung

```text
Load Workspace
      ↓
Load State
      ↓
Validate
      ↓
Resolve Resources
      ↓
Check Authority
      ↓
Restore Available State
```

Nicht verfügbare Ressourcen oder Komponenten dürfen zu einer partiellen Wiederherstellung führen.

## Sicherheit

Gespeicherter State erzeugt keine Authority.

Beim Wiederherstellen müssen benötigte Berechtigungen weiterhin gültig sein.

Sicherheitskritische Tokens, Credentials oder Capability Handles dürfen nicht ungeschützt als gewöhnlicher Workspace State gespeichert werden.

## Fehlerbehandlung

Beschädigter oder inkompatibler State darf das Öffnen des Workspace nicht grundsätzlich verhindern.

NovaOS soll auf einen gültigen früheren Zustand oder einen definierten Grundzustand zurückfallen können.

## Normative Anforderungen

1. NovaOS MUSS Workspace State vom Workspace Manifest trennen.
2. Persistenter Workspace State MUSS einer `WorkspaceID` zugeordnet sein.
3. Persistente Ressourcen SOLLEN über stabile Identitäten referenziert werden.
4. Workspace State DARF keine Nutzdaten unnötig duplizieren.
5. Persistenter State MUSS versionierbar sein.
6. Zusammengehörige State-Änderungen SOLLEN transaktional gespeichert werden.
7. Programme und Solutions MÜSSEN wiederherstellbaren Zustand explizit deklarieren können.
8. Gespeicherter State DARF keine Authority erzeugen.
9. Fehlende Ressourcen MÜSSEN eine partielle Wiederherstellung erlauben können.
10. Beschädigter State MUSS kontrolliert behandelt werden.

## Abhängigkeiten

- `NPSPEC-WORKSPACE-MODEL-0001`
- `NPSPEC-WORKSPACE-MANIFEST-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-FILESYSTEM-TRANSACTION-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`

## Ergebnis

NovaOS kann den aktuellen Arbeitszustand eines Workspace unabhängig von dessen Manifest und Nutzdaten speichern und kontrolliert wiederherstellen. Dadurch können Arbeitskontexte über Sitzungen und Neustarts hinweg fortgesetzt werden, ohne Identität, Daten und Authority miteinander zu vermischen.