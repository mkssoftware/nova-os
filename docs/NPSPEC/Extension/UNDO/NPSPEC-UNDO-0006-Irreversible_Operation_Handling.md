# NPSPEC-UNDO-0006 – Irreversible Operation Handling

## Status

Angenommen

## Zweck

Diese Spezifikation definiert den Umgang mit Aktionen, die nicht vollständig rückgängig gemacht werden können.

Ziel ist, irreversible Wirkungen früh zu erkennen, transparent zu behandeln und soweit möglich durch sichere Ersatz- oder Ausgleichsmaßnahmen zu begrenzen.

## Grundprinzip

```text
Semantic Action
    ↓
Reversibility Check
    ↓
REVERSIBLE
COMPENSATABLE
CONDITIONAL
IRREVERSIBLE
```

Eine irreversible Aktion darf nicht als normales Undo behandelt werden.

## Irreversible Operation

Eine Operation gilt als irreversibel, wenn ihr ursprünglicher Zustand oder ihre externe Wirkung nicht zuverlässig wiederhergestellt werden kann.

Beispiele:

```text
externe Nachricht gesendet
physische Geräteaktion ausgeführt
Daten sicher überschrieben
externe Zahlung bestätigt
öffentliche Veröffentlichung erfolgt
```

## Klassifikation

Vor der Ausführung soll jede undo-relevante Aktion ihre Rücknehmbarkeit deklarieren.

Mindestens:

```text
REVERSIBLE
COMPENSATABLE
CONDITIONAL
IRREVERSIBLE
UNKNOWN
```

`UNKNOWN` muss vorsichtig behandelt werden und darf nicht automatisch als reversibel gelten.

## Vorabprüfung

Vor einer irreversiblen oder bedingt reversiblen Aktion soll NovaOS prüfen:

```text
can_restore_state
can_compensate
external_effect
data_loss_risk
required_confirmation
```

Falls möglich, soll vor der eigentlichen Aktion ein sicherer Wiederherstellungspunkt erzeugt werden.

Beispiele:

```text
Snapshot
MicroCheckpoint
Backup
Version
TaskCapsule State
```

## Point of No Return

Eine Aktion darf einen expliziten Punkt besitzen, ab dem vollständiges Undo nicht mehr garantiert werden kann.

Beispiel:

```text
prepare
    ↓
validate
    ↓
POINT_OF_NO_RETURN
    ↓
external_commit
```

Vor diesem Punkt kann die Aktion regulär abgebrochen werden.

Nach diesem Punkt kann nur noch eine Compensation oder Schadensbegrenzung möglich sein.

## Benutzerbestätigung

Für Aktionen mit erheblicher irreversibler Wirkung darf eine explizite Bestätigung verlangt werden.

Die Bestätigung soll die tatsächliche Konsequenz beschreiben.

Beispiel:

```text
Diese Daten werden sicher überschrieben und können danach
nicht durch NovaOS wiederhergestellt werden.
```

Unkritische technische Details sollen den Nutzer nicht unnötig belasten.

## Compensation

Auch irreversible Operationen können eine semantische Gegenaktion besitzen.

Beispiel:

```text
reservation.confirm
    ↓
reservation.cancel
```

oder:

```text
publication.publish
    ↓
publication.remove
```

Die ursprüngliche externe Wirkung gilt dadurch nicht automatisch als ungeschehen.

NovaOS muss zwischen:

```text
reversed
```

und:

```text
compensated
```

unterscheiden.

## Externe Wirkungen

Bei Aktionen außerhalb der Kontrolle von NovaOS muss der externe Zustand berücksichtigt werden.

Beispiele:

```text
Remote Service
Network Peer
Payment Provider
Printer
Machine Controller
```

NovaOS darf keine erfolgreiche Rücknahme behaupten, wenn das externe System diese nicht bestätigt hat.

## Datenverlust

Destruktive Operationen sollen, soweit technisch sinnvoll, zunächst reversibel ausgeführt werden.

Beispiel:

```text
delete
    ↓
recoverable storage
    ↓ retention expires
secure erase
```

Erst nach Ablauf oder bewusster Freigabe kann der Zustand irreversibel werden.

## Undo-Historie

Irreversible Aktionen bleiben Teil der Undo-Historie.

Beispiel:

```text
Action:
    secure.erase

Undo Status:
    IRREVERSIBLE
```

Die Aktion darf nicht aus der Historie verschwinden, nur weil keine Gegenoperation existiert.

## Teilweise Reversibilität

Eine Aktion kann mehrere Wirkungen besitzen.

Beispiel:

```text
Report veröffentlichen
```

führt zu:

```text
local file update
remote upload
notification send
```

Dabei kann gelten:

```text
local file update  → REVERSIBLE
remote upload      → COMPENSATABLE
notification send  → IRREVERSIBLE
```

NovaOS muss diese Wirkungen getrennt behandeln können.

## Fehler nach Point of No Return

Tritt nach dem irreversiblen Commit ein Fehler auf, muss der Zustand klar markiert werden.

Beispiel:

```text
COMMITTED_WITH_FAILURE
```

NovaOS darf dann nicht versuchen, die Aktion durch blindes Retry zu duplizieren.

Stattdessen sind möglich:

```text
verify_external_state
compensate
repair_local_state
request_user_action
```

## Causality

Irreversible Wirkungen und mögliche Compensation müssen mit `Nova.Causality` verbunden bleiben.

Beispiel:

```text
Original Action
    ↓
Irreversible Effect
    ↓
Compensation Attempt
```

Dadurch bleibt nachvollziehbar, welche Wirkung tatsächlich eingetreten ist.

## Beispiel

```text
SemanticAction {
    type:
        data.secure_erase

    reversibility:
        IRREVERSIBLE
}
```

Vor Ausführung:

```text
Preconditions:
    backup_checked
    user_authorized
    target_verified
```

Ausführung:

```text
prepare
    ↓
verify target
    ↓
POINT_OF_NO_RETURN
    ↓
secure erase
```

Danach:

```text
undo:
    unavailable
```

Die Aktion bleibt dennoch vollständig dokumentiert.

## Normative Anforderungen

1. Irreversible Aktionen MÜSSEN eindeutig als solche klassifizierbar sein.
2. `UNKNOWN` DARF nicht automatisch als reversibel behandelt werden.
3. Ein Point of No Return SOLL für relevante Operationen explizit darstellbar sein.
4. NovaOS DARF eine Compensation nicht als vollständige Rücknahme darstellen, wenn die ursprüngliche Wirkung fortbesteht.
5. Externe Rücknahmen MÜSSEN bestätigt werden, bevor sie als erfolgreich gelten.
6. Teilweise reversible Wirkungen MÜSSEN getrennt behandelbar sein.
7. Irreversible Aktionen MÜSSEN in der Undo- und Causality-Historie erhalten bleiben.
8. Nach einem irreversiblen Commit DÜRFEN nicht idempotente Aktionen nicht blind erneut ausgeführt werden.

## Abgrenzung

Diese NPSPEC definiert:

- irreversible Aktionen
- Point of No Return
- irreversible externe Wirkungen
- teilweise Reversibilität
- sichere Behandlung fehlender Undo-Möglichkeiten

Nicht Bestandteil sind:

- allgemeine Compensation-Logik
- Konfliktauflösung
- Transaktionsgrenzen
- Backup- oder Snapshot-Implementierung

## Zugehörige NPSPECs

- `NPSPEC-UNDO-0001 – Semantic Action Model`
- `NPSPEC-UNDO-0002 – Compensation Operations`
- `NPSPEC-UNDO-0003 – Undo Transaction Boundaries`
- `NPSPEC-UNDO-0004 – Cross-Capability Undo Coordination`
- `NPSPEC-UNDO-0005 – Undo Conflict Detection & Resolution`
- `NPSPEC-CAUSAL-0001 – Causality Graph Model`