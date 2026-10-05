# NPSPEC-WORKSPACE-SOLUTION-0001 – Nova Workspace Solution

## Status

Angenommen

## Kategorie

Workspace / Solution

## Zweck

NovaOS definiert die Einbindung von Solutions in einen Workspace.

Eine Solution stellt Fähigkeiten, UI und Logik für eine Aufgabe bereit, während der Workspace den übergeordneten Arbeitskontext mit Ressourcen und Zustand bildet.

## Grundprinzipien

```text
Workspace ≠ Solution
Solution ≠ Program
Solution Membership ≠ Authority
Solution State ≠ Workspace State
```

Ein Workspace kann keine, eine oder mehrere Solutions enthalten.

## Zuordnung

Solutions werden über ihre stabile Identität mit einem Workspace verbunden.

```text
Workspace
├── Solution A
├── Solution B
└── Solution C
```

Die Zuordnung kann im Workspace Manifest persistiert werden.

Eine Solution darf gleichzeitig in mehreren Workspaces verwendet werden.

## Integration

Beim Aktivieren einer Solution innerhalb eines Workspace erhält sie Zugriff auf den für sie freigegebenen Workspace-Kontext.

```text
Workspace
   ↓
Solution Context
   ├── Resources
   ├── State
   ├── UI Context
   └── Capabilities
```

Die Solution darf daraus eine aufgabenbezogene Oberfläche und Logik bereitstellen.

## Ressourcen

Workspace-Ressourcen werden nicht automatisch vollständig an eine Solution freigegeben.

Eine Solution erhält nur die Ressourcen, für die ihr innerhalb des Workspace eine entsprechende Authority bereitgestellt wurde.

Referenzen sollen über stabile Identitäten wie `ObjectID` erfolgen.

## Capabilities

Die von einer Solution benötigten Capabilities ergeben sich aus ihrer definierten Struktur und den verwendeten Capability-Komponenten.

```text
Solution
   ↓
Capability Requirements
   ↓
Workspace Capability Context
   ↓
Authorized Execution
```

Custom-NovaLang-Code darf keine zusätzliche Authority selbst anfordern oder erzeugen.

## Zustand

Solution-spezifischer Zustand darf innerhalb des Workspace State gespeichert werden.

Dabei muss zwischen:

```text
Workspace State
Solution State
User Data
```

unterschieden werden.

Das Entfernen einer Solution aus einem Workspace darf die eigentlichen Benutzerdaten nicht automatisch löschen.

## Lebenszyklus

```text
Attach
  ↓
Validate
  ↓
Resolve Capabilities
  ↓
Activate
  ↓
Run
  ↓
Suspend / Detach
```

Beim Entfernen oder Deaktivieren müssen workspacegebundene Capabilities und Laufzeitressourcen kontrolliert freigegeben werden.

## Wiederherstellung

Beim Wiederherstellen eines Workspace werden referenzierte Solutions erneut aufgelöst und validiert.

Fehlende oder inkompatible Solutions dürfen eine partielle Wiederherstellung des Workspace nicht grundsätzlich verhindern.

## Normative Anforderungen

1. Ein Workspace MUSS mehrere Solutions enthalten können.
2. Solutions MÜSSEN über stabile Identitäten referenziert werden.
3. Eine Solution DARF mehreren Workspaces zugeordnet sein.
4. Workspace-Mitgliedschaft DARF keine Authority erzeugen.
5. Solutions DÜRFEN nur autorisierte Workspace-Ressourcen erhalten.
6. Solution-Capabilities MÜSSEN innerhalb des Workspace-Kontexts begrenzbar sein.
7. Custom-NovaLang-Code DARF keine zusätzliche Authority selbst erzeugen.
8. Solution State MUSS von eigentlichen Benutzerdaten unterscheidbar sein.
9. Das Entfernen einer Solution DARF Benutzerdaten nicht automatisch löschen.
10. Fehlende Solutions MÜSSEN bei der Workspace-Wiederherstellung kontrolliert behandelt werden.

## Abhängigkeiten

- `NPSPEC-WORKSPACE-MODEL-0001`
- `NPSPEC-WORKSPACE-MANIFEST-0001`
- `NPSPEC-WORKSPACE-STATE-0001`
- `NPSPEC-WORKSPACE-RELATION-0001`
- `NPSPEC-WORKSPACE-CAPABILITY-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`

## Ergebnis

NovaOS kann Solutions als aufgabenbezogene Funktions- und UI-Komponenten in Workspaces einbinden. Der Workspace stellt Kontext und Ressourcen bereit, während jede Solution mit klar begrenzter Authority arbeitet und unabhängig vom Workspace identifizierbar und wiederverwendbar bleibt.