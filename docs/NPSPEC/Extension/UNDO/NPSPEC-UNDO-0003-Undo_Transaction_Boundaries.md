# NPSPEC-UNDO-0003 – Undo Transaction Boundaries

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie mehrere `Semantic Actions` zu einer gemeinsamen Undo-Einheit zusammengefasst werden.

Ziel ist, dass eine fachlich zusammengehörige Nutzeraktion auch als Einheit rückgängig gemacht werden kann.

## Grundprinzip

```text
User Action
    ↓
Undo Transaction
    ├── Semantic Action A
    ├── Semantic Action B
    └── Semantic Action C
```

Beim Undo wird die gesamte Transaktion kontrolliert kompensiert.

## Undo Transaction

Die logische Grundstruktur lautet:

```text
UndoTransaction {
    id
    intent
    actions[]
    dependencies
    boundary
    status
}
```

Eine Transaktion kann eine oder mehrere Semantic Actions enthalten.

## Transaction Boundary

Eine Boundary definiert, welche Aktionen fachlich zu einer gemeinsamen Undo-Einheit gehören.

Beispiel:

```text
"Projekt umbenennen"
```

kann intern aus folgenden Aktionen bestehen:

```text
directory.rename
metadata.update
reference.update
shortcut.update
```

Für den Nutzer bleibt dies eine einzelne Undo-Operation.

## Boundary-Arten

NovaOS muss mindestens folgende Grenzen unterstützen:

```text
EXPLICIT
INTENT
USER_ACTION
SYSTEM_TRANSACTION
```

### `EXPLICIT`

Eine Komponente definiert Anfang und Ende ausdrücklich.

### `INTENT`

Alle relevanten Aktionen eines Intents werden zu einer Undo-Einheit zusammengefasst.

### `USER_ACTION`

Mehrere technische Änderungen gehören zu einer einzelnen sichtbaren Nutzeraktion.

### `SYSTEM_TRANSACTION`

NovaOS gruppiert intern notwendige Änderungen zu einer atomaren Einheit.

## Verschachtelung

Undo Transactions dürfen verschachtelt sein.

Beispiel:

```text
Transaction A
    ├── Action 1
    └── Transaction B
            ├── Action 2
            └── Action 3
```

Eine untergeordnete Transaktion kann intern separat behandelt werden, bleibt aber Teil der übergeordneten Undo-Semantik.

## Commit

Eine Undo Transaction gilt erst nach erfolgreichem Abschluss ihrer relevanten Aktionen als bestätigt.

```text
OPEN
    ↓
COMMITTED
```

Nur bestätigte Transaktionen sollen regulär in der Undo-Historie erscheinen.

Fehlgeschlagene oder abgebrochene Transaktionen müssen separat behandelt werden.

## Undo-Reihenfolge

Abhängige Aktionen werden grundsätzlich in umgekehrter Reihenfolge kompensiert.

Ausführung:

```text
A
↓
B
↓
C
```

Undo:

```text
Compensate C
↓
Compensate B
↓
Compensate A
```

Bei unabhängigen Aktionen darf NovaOS eine andere sichere Reihenfolge oder parallele Compensation verwenden.

## Atomarität

NovaOS soll eine Undo Transaction möglichst vollständig kompensieren.

Kann dies nicht garantiert werden, muss der Zustand erkennbar bleiben.

Mögliche Ergebnisse:

```text
UNDONE
PARTIAL
FAILED
CONFLICT
```

`PARTIAL` darf nicht als erfolgreiches vollständiges Undo dargestellt werden.

## Abhängigkeiten

Die Transaction muss Abhängigkeiten zwischen ihren Actions darstellen können.

Beispiel:

```text
Action B
    depends_on Action A
```

Die Compensation-Reihenfolge wird daraus abgeleitet.

Eine reine zeitliche Reihenfolge ist dafür nicht ausreichend.

## Dynamische Erweiterung

Eine offene Undo Transaction darf weitere Actions aufnehmen.

Beispiel:

```text
OPEN
    ├── Action A
    └── Action B added
```

Nach `COMMITTED` darf die Transaktion nicht stillschweigend erweitert werden.

Spätere Änderungen müssen eine neue Transaktion oder explizite Folgebeziehung erzeugen.

## Parent- und Child-Intents

Aktionen aus Child-Intents dürfen Teil derselben Undo Transaction sein, wenn sie gemeinsam eine fachliche Aktion des Parent-Intents darstellen.

Beispiel:

```text
Parent Intent
    ├── Child A → Action A
    └── Child B → Action B

Undo Transaction
    ├── Action A
    └── Action B
```

## Persistenz

Undo Transactions müssen über Prozess- und Neustartgrenzen hinweg rekonstruierbar sein.

Mindestens zu speichern:

```text
transaction_id
status
actions
dependencies
intent_reference
boundary_type
```

## Beispiel

```text
UndoTransaction {
    id: undo:501

    intent:
        project.rename

    boundary:
        USER_ACTION

    actions {
        action:directory.rename
        action:metadata.update
        action:references.update
    }

    dependencies {
        references.update
            depends_on metadata.update

        metadata.update
            depends_on directory.rename
    }

    status:
        COMMITTED
}
```

Undo:

```text
references.update
    ↓ compensate

metadata.update
    ↓ compensate

directory.rename
    ↓ compensate
```

## Normative Anforderungen

1. Fachlich zusammengehörige Semantic Actions MÜSSEN zu einer Undo Transaction gruppierbar sein.
2. Transaction Boundaries MÜSSEN explizit bestimmbar sein.
3. Abhängigkeiten zwischen Actions MÜSSEN darstellbar sein.
4. Abhängige Compensation MUSS in einer sicheren Reihenfolge erfolgen.
5. Bereits bestätigte Transactions DÜRFEN nicht stillschweigend erweitert werden.
6. Teilweise oder fehlgeschlagene Undo-Transaktionen MÜSSEN als solche erkennbar bleiben.
7. Undo Transactions MÜSSEN persistent rekonstruierbar sein.
8. Verschachtelte Transactions MÜSSEN unterstützt werden können.

## Abgrenzung

Diese NPSPEC definiert:

- Undo Transaction Boundaries
- Gruppierung von Semantic Actions
- Abhängigkeiten
- Commit und Undo-Reihenfolge

Nicht Bestandteil sind:

- konkrete Compensation-Operationen
- Cross-Capability-Koordination
- Konfliktauflösung
- irreversible Operationen

## Zugehörige NPSPECs

- `NPSPEC-UNDO-0001 – Semantic Action Model`
- `NPSPEC-UNDO-0002 – Compensation Operations`
- `NPSPEC-UNDO-0004 – Cross-Capability Undo Coordination`
- `NPSPEC-UNDO-0005 – Undo Conflict Detection & Resolution`
- `NPSPEC-UNDO-0006 – Irreversible Operation Handling`