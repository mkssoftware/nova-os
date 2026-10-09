
# NPSPEC-LOGIC-CUSTOMSCRIPT-0001 – NovaOS Logic Graph Custom Script

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / NovaLang-Skripte

## Zweck

Definiert Custom Script Nodes zur Ausführung benutzerdefinierter NovaLang-Logik innerhalb eines Logic Graphs.

Ziel ist die flexible Verarbeitung von Daten, ohne die Capability- und Sicherheitsarchitektur von NovaOS zu umgehen.

## Architektur

Das Custom-Script-System besteht aus:

- **Script Node:** Einbindung des Skripts in den Graphen.
- **Script Compiler:** Übersetzung über den regulären NovaLang-Compiler.
- **Port Binder:** Zuordnung von Eingängen und Ausgängen.
- **Script Executor:** Ausführung über die NovaLang Runtime.
- **Execution Context:** Verwaltung lokaler Variablen und Ressourcen.
- **Diagnostic Bridge:** Weiterleitung von Compiler- und Laufzeitfehlern.

## Sprachmodell

Custom Scripts verwenden `.nlf` und exakt dieselbe NovaLang-Syntax und -Semantik wie `.nova`.

Es existiert kein separater Skriptdialekt.

Unterstützt werden reguläre NovaLang-Konstrukte wie Bedingungen, Schleifen, Funktionen, Klassen und asynchrone Verarbeitung.

Die Ausführung unterliegt den Einschränkungen des jeweiligen Graph-Kontexts.

## Ein- und Ausgänge

Jeder Script Node besitzt deklarierte, typisierte Ports.

- Eingänge stellen Daten und autorisierte Ressourcen bereit.
- Ausgänge liefern Ergebnisse an nachfolgende Knoten.
- Porttypen müssen mit den NovaLang-Signaturen übereinstimmen.
- Fehler werden über definierte Fehlerausgänge bereitgestellt.

Nicht deklarierte externe Abhängigkeiten sind unzulässig.

## Capability-Modell

Custom Scripts dürfen keine System-Capabilities eigenständig anfordern oder aufrufen.

Benötigte Systemfunktionen werden als explizite Capability Nodes im Logic Graph angeordnet.

Beispielhafter Ablauf:

`Network Capability → Custom Script → Data Processing Capability`

Das Skript verarbeitet ausschließlich die über seine Ports bereitgestellten Daten und autorisierten Ressourcen.

## Ausführung und Isolation

Jeder Script-Aufruf besitzt einen kontrollierten Ausführungskontext.

Die NovaLang Runtime erzwingt Speicherisolation, Ressourcenlimits und Abbruchregeln.

Asynchrone Ausführungen verwenden Structured Concurrency.

Nicht behandelte Fehler werden an die Graph Runtime weitergeleitet.

## Normative Anforderungen

1. Custom Scripts MÜSSEN reguläres NovaLang verwenden.
2. `.nlf` DARF keinen eigenständigen NovaLang-Dialekt definieren.
3. Custom Scripts MÜSSEN über den regulären NovaLang-Compiler verarbeitet werden.
4. Ein- und Ausgänge MÜSSEN typisiert und vor der Ausführung geprüft werden.
5. Scripts DÜRFEN keine Capabilities eigenständig anfordern oder aufrufen.
6. Systemzugriffe MÜSSEN über explizite Capability Nodes erfolgen.
7. Scripts DÜRFEN ausschließlich deklarierte Eingaben und autorisierte Ressourcen verwenden.
8. Jede Ausführung MUSS einen kontrollierten Laufzeitkontext besitzen.
9. Speicher-, Zeit- und Ressourcenlimits MÜSSEN eingehalten werden.
10. Asynchrone Ausführungen MÜSSEN kontrolliert abbrechbar sein.
11. Compiler- und Laufzeitfehler MÜSSEN im Logic Graph diagnostizierbar sein.
12. Custom Scripts MÜSSEN unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält vollständig integrierte NovaLang Custom Scripts für Logic Graphs, die flexible Programmierung mit expliziten Capability-Grenzen und kontrollierter Ausführung verbinden.
