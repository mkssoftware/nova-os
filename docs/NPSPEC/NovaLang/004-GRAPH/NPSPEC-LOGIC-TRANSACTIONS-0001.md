
# NPSPEC-LOGIC-TRANSACTIONS-0001 – NovaOS Logic Graph Transactions

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Transaktionen

## Zweck

Definiert transaktionale Operationen innerhalb eines NovaOS Logic Graphs.

Ziel ist die konsistente Ausführung zusammengehöriger Zustandsänderungen mit kontrolliertem Commit, Rollback und Fehlerverhalten.

## Architektur

Das Transaction-System besteht aus:

- **Transaction Manager:** Verwaltung von Transaktionen.
- **Transaction Context:** Verwaltung beteiligter Operationen und Zustände.
- **Consistency Validator:** Prüfung definierter Konsistenzbedingungen.
- **Commit Coordinator:** Koordination erfolgreicher Änderungen.
- **Rollback Manager:** Rücknahme nicht bestätigter Änderungen.
- **Transaction Logger:** Nachvollziehbarkeit von Transaktionsabläufen.

Das System verwendet die Zustandsverwaltung und die Graph Runtime.

## Transaktionsmodell

Jede Transaktion besitzt:

- Eindeutige Transaction-ID
- Zugehörigen Ausführungskontext
- Definierte beteiligte Ressourcen
- Transaktionsstatus
- Optionale Zeit- und Ressourcenlimits

Unterstützte Zustände sind `Created`, `Active`, `Committing`, `Committed`, `RollingBack` und `Aborted`.

## Ausführung

Transaktionen können mehrere Knoten und Subgraphs umfassen.

- **Begin:** Transaktion starten.
- **Execute:** Operationen innerhalb des Kontexts ausführen.
- **Validate:** Konsistenzbedingungen prüfen.
- **Commit:** Änderungen verbindlich übernehmen.
- **Rollback:** Nicht bestätigte Änderungen zurücknehmen.

Atomarität gilt ausschließlich für Ressourcen, die am Transaktionsprotokoll teilnehmen.

## Externe Capabilities

Capability Nodes müssen ihre Transaktionsunterstützung deklarieren.

Nicht transaktionale Seiteneffekte, beispielsweise bereits versendete Netzwerkdaten, können nicht automatisch zurückgerollt werden.

Solche Operationen müssen ausdrücklich gekennzeichnet und gegebenenfalls durch Kompensationsoperationen behandelt werden.

## Parallelität und Isolation

Gleichzeitige Transaktionen müssen definierte Isolations- und Konfliktregeln einhalten.

Zustandsänderungen dürfen keine inkonsistenten Zwischenstände für andere Ausführungen sichtbar machen, soweit der gewählte Isolationsvertrag dies garantiert.

Verschachtelte Transaktionen sind nur mit eindeutig definierter Commit- und Rollback-Semantik zulässig.

## Fehlerbehandlung

Bei Fehlern vor dem Commit werden transaktionale Änderungen zurückgenommen.

Fehlgeschlagene Rollbacks und Kompensationen müssen diagnostiziert werden.

Abbruch und Ressourcenlimits bleiben auch während einer Transaktion wirksam.

## Normative Anforderungen

1. Die Graph Runtime MUSS transaktionale Ausführungsbereiche unterstützen.
2. Jede Transaktion MUSS eine eindeutige Transaction-ID besitzen.
3. Begin, Commit und Rollback MÜSSEN definiert sein.
4. Zustandsänderungen MÜSSEN die vereinbarten Konsistenzregeln einhalten.
5. Atomarität DARF nur für tatsächlich transaktionale Ressourcen zugesichert werden.
6. Capability Nodes MÜSSEN ihre Transaktionseigenschaften deklarieren.
7. Nicht rücknehmbare Seiteneffekte MÜSSEN erkennbar sein.
8. Parallele Transaktionen MÜSSEN Konflikt- und Isolationsregeln einhalten.
9. Fehler MÜSSEN einen kontrollierten Abbruch oder Rollback auslösen.
10. Transaktionen DÜRFEN keine Capability-Berechtigungen erzeugen oder erweitern.
11. Commit-, Rollback- und Fehlerzustände MÜSSEN diagnostizierbar sein.
12. Das Transaction-System MUSS unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine kontrollierte Transaktionsverwaltung für Logic Graphs, die konsistente Zustandsänderungen ermöglicht und die Grenzen externer Seiteneffekte ausdrücklich berücksichtigt.
