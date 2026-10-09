
# NPSPEC-NOVALANG-RESOURCE-LIMITS-0001 – NovaLang Resource Limits

## Status

Angenommen

## Kategorie

NovaLang / Runtime / Ressourcenverwaltung

## Zweck

Definiert die Begrenzung und Überwachung von Systemressourcen bei der Ausführung von NovaLang-Programmen, Tasks und Solutions.

Ziel sind Systemstabilität, faire Ressourcennutzung und kontrolliertes Verhalten bei Überlastung.

## Ressourcenmodell

Jeder Execution Context besitzt ein definiertes Ressourcenbudget.

| Ressource | Begrenzung |
|---|---|
| Memory | Maximaler Speicherverbrauch |
| CPU | CPU-Zeit und Rechenbudget |
| Tasks | Anzahl gleichzeitiger Tasks |
| Threads | Anzahl verwendbarer Threads |
| Execution Time | Maximale Ausführungsdauer |
| Handles | Anzahl geöffneter Ressourcen |
| IPC | Nachrichtenanzahl und Datenvolumen |
| Stack | Maximale Stackgröße |
| JIT | Kompilierungszeit und Code-Cache-Größe |

Zusätzliche Ressourcenarten können über NovaOS erweitert werden.

## Budgethierarchie

Ressourcenlimits werden hierarchisch verwaltet:

`NovaOS → Prozess → Runtime → Execution Context → Task`

- Untergeordnete Kontexte dürfen ihre übergeordneten Limits nicht überschreiten.
- Budgets können aufgeteilt und reserviert werden.
- Nicht verwendete Reservierungen können zurückgegeben werden.
- Eine Budgeterhöhung benötigt die entsprechende Autorisierung.
- Gemeinsame Ressourcen müssen eindeutig abgerechnet werden.

## Limittypen

NovaLang unterscheidet:

- **Hard Limit:** Darf nicht überschritten werden.
- **Soft Limit:** Löst Warnungen oder Drosselung aus.
- **Deadline:** Zeitpunkt, bis zu dem eine Operation abgeschlossen sein soll.
- **Quota:** Verbrauchsgrenze innerhalb eines definierten Zeitraums.

Limits werden durch die Runtime überwacht und durch NovaOS abgesichert, soweit Betriebssystemressourcen betroffen sind.

## Verhalten bei Grenzüberschreitung

1. Grenzüberschreitung erkennen.
2. Neue Ressourcenanforderungen gegebenenfalls verweigern.
3. Betroffene Tasks drosseln oder abbrechen.
4. Definierte Fehlerdiagnose erzeugen.
5. Ressourcen kontrolliert freigeben.
6. Übergeordneten Execution Context informieren.

Hard Limits dürfen nicht durch automatische Wiederholungen umgangen werden.

## Structured Concurrency

- Untergeordnete Tasks erben die Budgetgrenzen ihres Task-Scopes.
- Parallele Tasks teilen das verfügbare Gesamtbudget.
- Cancellation muss bei Ressourcenüberschreitungen propagiert werden.
- Abgebrochene Tasks dürfen keine Ressourcen dauerhaft zurückhalten.

## NovaOS-Integration

- NovaOS bleibt die verbindliche Instanz für CPU-, Speicher- und Prozesslimits.
- Die NovaLang Runtime verwaltet zusätzliche logische Budgets.
- AOT, JIT und Interpreter unterliegen denselben Ressourcenregeln.
- Sandbox- und Solution-Policies können strengere Limits definieren.
- Capabilities dürfen nur innerhalb autorisierter Ressourcenbudgets verwendet werden.
- Logic-Graph-Skripte dürfen ihre Limits nicht eigenständig erhöhen.

## Diagnostik

Die Runtime stellt pro Execution Context folgende Informationen bereit:

- Aktueller Ressourcenverbrauch
- Zugewiesene Limits
- Grenzüberschreitungen
- Drosselungs- und Abbruchereignisse

Diagnosedaten dürfen keine geschützten Informationen anderer Kontexte offenlegen.

## Normative Anforderungen

1. NovaLang MUSS konfigurierbare Ressourcenlimits unterstützen.
2. Ressourcenbudgets MÜSSEN hierarchisch verwaltet werden.
3. Untergeordnete Kontexte DÜRFEN übergeordnete Hard Limits nicht überschreiten.
4. CPU-, Speicher-, Task- und Ausführungszeitlimits MÜSSEN unterstützt werden.
5. Grenzüberschreitungen MÜSSEN kontrolliert behandelt werden.
6. Ressourcenfreigaben MÜSSEN auch bei Cancellation und Exceptions erfolgen.
7. AOT, JIT und Interpreter MÜSSEN denselben Ressourcen-Policies unterliegen.
8. Budgeterhöhungen MÜSSEN ausdrücklich autorisiert werden.
9. Die Runtime MUSS Ressourcenverbrauch und Limitverletzungen diagnostizierbar machen.

## Ergebnis

NovaLang erhält eine hierarchische Ressourcenverwaltung mit Hard Limits, Soft Limits, Quotas und Deadlines. NovaOS erzwingt die Systemgrenzen, während die Runtime Tasks und Execution Contexts kontrolliert verwaltet.
