# NPSPEC-WORKSPACE-COLLABORATION-0001 – Nova Workspace Collaboration

## Status

Angenommen

## Kategorie

Workspace / Collaboration

## Zweck

NovaOS ermöglicht mehreren Benutzern die kontrollierte Zusammenarbeit innerhalb eines Workspace.

Zusammenarbeit erfolgt auf explizit freigegebenen Ressourcen und überträgt weder automatisch den gesamten Workspace noch dessen Berechtigungen.

## Grundprinzipien

```text
Collaboration ≠ Shared Authority
Workspace Sharing ≠ Resource Sharing
Resource Sharing ≠ Ownership Transfer
Presence ≠ Permission
Collaboration Scope ≠ Entire Workspace
```

Freigaben müssen auf den tatsächlich benötigten Arbeitsbereich begrenzt werden können.

## Collaboration Session

Eine Zusammenarbeit wird als eigener Kontext behandelt:

```text
CollaborationSession
├── SessionID
├── WorkspaceID
├── Participants
├── SharedResources
├── Permissions
└── State
```

Die Session kann temporär oder persistent sein.

## Teilnehmer

Jeder Teilnehmer besitzt eine eigene Identität und eigene Authority.

```text
Workspace
├── User A
├── User B
└── User C
```

Die Berechtigungen eines Teilnehmers dürfen nicht automatisch auf andere Teilnehmer übertragen werden.

## Ressourcenfreigabe

Ressourcen müssen einzeln oder als definierte Gruppe freigegeben werden können.

Beispiel:

```text
Workspace
├── Datei A      → geteilt
├── Datei B      → privat
└── Solution C   → privat
```

Das Freigeben einer geöffneten Datei darf auf genau diese Datei begrenzt werden, ohne Zugriff auf andere Workspace-Ressourcen zu gewähren.

## Gemeinsame Bearbeitung

NovaOS darf gleichzeitige Bearbeitung derselben Ressource unterstützen.

```text
Shared Object
├── User A Cursor
├── User B Cursor
└── Shared Changes
```

Teilnehmer können eigene Cursor, Auswahlbereiche und Presence-Informationen besitzen.

Änderungen müssen eindeutig einem Teilnehmer beziehungsweise Sicherheitskontext zugeordnet werden können.

## Synchronisation

Gemeinsame Änderungen müssen konsistent synchronisiert werden.

Konflikte dürfen nicht stillschweigend zu Datenverlust führen.

Je nach Ressourcentyp können geeignete Verfahren wie:

```text
Operation Ordering
Conflict Detection
Merge
Transactional Commit
```

verwendet werden.

## Berechtigungen

Collaboration verwendet das Workspace-Capability-Modell.

```text
Share Request
     ↓
Permission Evaluation
     ↓
Attenuated Capability
     ↓
Participant
```

Delegierte Authority muss auf die freigegebene Ressource und die erlaubten Operationen begrenzt sein.

## Widerruf

Freigaben müssen jederzeit widerrufbar sein.

Beim Widerruf werden zugehörige Collaboration-Capabilities und Handles entsprechend ihrer Policy ungültig gemacht.

Bereits autorisiert gespeicherte Änderungen bleiben Teil der Ressourcenhistorie.

## Normative Anforderungen

1. NovaOS MUSS Collaboration auf einzelne Ressourcen begrenzen können.
2. Workspace-Freigabe DARF keine vollständige Workspace-Authority implizieren.
3. Jeder Teilnehmer MUSS einen eigenen Sicherheitskontext besitzen.
4. Berechtigungen MÜSSEN pro Teilnehmer begrenzbar sein.
5. Das Teilen einer Datei DARF keinen Zugriff auf andere Workspace-Ressourcen erzeugen.
6. Gleichzeitige Bearbeitung MUSS Konflikte kontrolliert behandeln können.
7. Änderungen SOLLEN einem Teilnehmer zuordenbar sein.
8. Presence-Informationen DÜRFEN keine Authority erzeugen.
9. Collaboration-Capabilities MÜSSEN widerrufbar sein.
10. Collaboration DARF die bestehenden Workspace- und Filesystem-Sicherheitsgrenzen nicht umgehen.

## Abhängigkeiten

- `NPSPEC-WORKSPACE-MODEL-0001`
- `NPSPEC-WORKSPACE-RELATION-0001`
- `NPSPEC-WORKSPACE-CAPABILITY-0001`
- `NPSPEC-WORKSPACE-PERMISSION-0001`
- `NPSPEC-FILESYSTEM-TRANSACTION-0001`
- `NPSPEC-CAPABILITY-DELEGATION-0001`
- `NPSPEC-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`

## Ergebnis

NovaOS ermöglicht sichere Zusammenarbeit innerhalb eines Workspace bis hin zur gemeinsamen Bearbeitung einzelner geöffneter Dateien. Teilnehmer erhalten ausschließlich die für die freigegebenen Ressourcen benötigte Authority, während private Workspace-Inhalte und Berechtigungen isoliert bleiben.