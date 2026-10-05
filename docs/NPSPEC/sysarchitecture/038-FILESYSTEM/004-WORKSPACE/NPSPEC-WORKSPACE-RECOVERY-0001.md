# NPSPEC-WORKSPACE-RECOVERY-0001 – Nova Workspace Recovery

## Status

Angenommen

## Kategorie

Workspace / Recovery

## Zweck

NovaOS definiert die kontrollierte Wiederherstellung eines Workspace nach Absturz, Neustart, beschädigtem Zustand oder fehlgeschlagener Wiederherstellung.

Ziel ist, möglichst viel des letzten gültigen Arbeitskontexts wiederherzustellen, ohne beschädigte Zustände oder ungültige Authority zu übernehmen.

## Grundprinzipien

```text
Recovery ≠ Rollback
Recovery ≠ Backup
Recovered State ≠ Trusted State
Saved Permission ≠ Valid Authority
Partial Recovery ≠ Failure
```

Workspace Recovery arbeitet auf Basis von Manifest, State und UI State.

## Recovery-Modell

```text
Workspace
├── Manifest
├── State
├── UI State
├── Resource References
└── Recovery Information
```

Die eigentlichen Benutzerdaten bleiben davon getrennt.

## Recovery-Ablauf

```text
Detect Recovery Need
        ↓
Load Manifest
        ↓
Validate State
        ↓
Resolve Resources
        ↓
Validate Permissions
        ↓
Restore Valid Components
        ↓
Verify Workspace
```

Ungültige Komponenten dürfen nicht ungeprüft übernommen werden.

## Wiederherstellungsumfang

NovaOS soll soweit möglich wiederherstellen:

```text
Workspace-Struktur
geöffnete Ressourcen
Solutions
Programme
UI-Zustand
Navigation
Workspace Settings
```

Prozessspeicher oder nicht deklarierter Laufzeitzustand wird nicht automatisch rekonstruiert.

## Partielle Wiederherstellung

Fehlende oder beschädigte Komponenten dürfen eine teilweise Wiederherstellung ermöglichen.

Beispiele:

```text
Resource unavailable
Solution missing
Program unavailable
UI State invalid
Remote Storage offline
```

Der restliche Workspace soll weiterhin geöffnet werden können.

## Konsistenz

NovaOS muss zwischen gültigem, unvollständigem und beschädigtem Workspace State unterscheiden können.

Falls der letzte Zustand nicht verwendbar ist, darf auf einen vorherigen gültigen Zustand oder einen definierten Grundzustand zurückgegriffen werden.

## Berechtigungen

Recovery erzeugt keine Authority.

```text
Recovered Reference
       ↓
Resolve Identity
       ↓
Permission Validation
       ↓
Authorized Handle
```

Gespeicherte Capability-Anforderungen müssen erneut geprüft werden.

## Absturzsicherheit

Workspace State und UI State sollen so persistiert werden, dass ein Absturz nicht automatisch den letzten gültigen Zustand zerstört.

Unvollständige Schreibvorgänge müssen erkannt werden können.

## Introspection

Recovery soll mindestens sichtbar machen können:

```text
Recovered
Skipped
Unavailable
Invalid
Permission Denied
```

Damit bleibt nachvollziehbar, welche Bestandteile wiederhergestellt wurden.

## Normative Anforderungen

1. NovaOS MUSS Workspace Recovery unabhängig von Benutzerdaten durchführen können.
2. Manifest, State und UI State MÜSSEN vor Wiederherstellung validierbar sein.
3. Beschädigte Komponenten DÜRFEN nicht ungeprüft übernommen werden.
4. Partielle Wiederherstellung MUSS unterstützt werden.
5. Fehlende Ressourcen DÜRFEN die Wiederherstellung des restlichen Workspace nicht verhindern.
6. Recovery DARF keine Authority aus gespeichertem Zustand erzeugen.
7. Berechtigungen MÜSSEN erneut validiert werden.
8. Unvollständige State-Schreibvorgänge MÜSSEN erkennbar sein.
9. Ein vorheriger gültiger Zustand DARF als Recovery-Fallback verwendet werden.
10. Das Recovery-Ergebnis MUSS introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-WORKSPACE-MODEL-0001`
- `NPSPEC-WORKSPACE-MANIFEST-0001`
- `NPSPEC-WORKSPACE-STATE-0001`
- `NPSPEC-WORKSPACE-UISTATE-0001`
- `NPSPEC-WORKSPACE-PERMISSION-0001`
- `NPSPEC-FILESYSTEM-TRANSACTION-0001`

## Ergebnis

NovaOS kann einen Workspace nach Fehlern oder Unterbrechungen kontrolliert und möglichst vollständig wiederherstellen. Beschädigte oder nicht verfügbare Bestandteile werden isoliert behandelt, während gültige Arbeitszustände erhalten bleiben und sämtliche Authority erneut geprüft wird.