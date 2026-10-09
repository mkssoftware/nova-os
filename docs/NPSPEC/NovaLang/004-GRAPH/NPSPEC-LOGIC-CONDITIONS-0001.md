
# NPSPEC-LOGIC-CONDITIONS-0001 – NovaOS Logic Graph Conditions

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Bedingungen

## Zweck

Definiert die Auswertung logischer Bedingungen innerhalb eines NovaOS Logic Graphs.

Ziel ist eine typsichere, nachvollziehbare und deterministisch auswertbare Entscheidungslogik.

## Architektur

Das Condition-System besteht aus:

- **Condition Evaluator:** Auswertung von Bedingungen.
- **Expression Resolver:** Auflösung von Ausdrücken und Operanden.
- **Type Validator:** Prüfung der Datentypen.
- **Branch Controller:** Auswahl des Ausführungspfads.
- **Condition Diagnostics:** Erkennung und Meldung von Fehlern.

Die Auswertung verwendet die reguläre NovaLang-Semantik.

## Bedingungsarten

Unterstützt werden:

- **Comparison:** Gleichheit und Größenvergleiche.
- **Logical:** AND, OR und NOT.
- **Null Check:** Prüfung auf `Nothing`.
- **Type Check:** Prüfung von Datentypen.
- **Range Check:** Prüfung definierter Wertebereiche.
- **Composite:** Kombination mehrerer Bedingungen.

Bedingungen liefern grundsätzlich einen booleschen Wert.

## Auswertung

Bedingungen können Konstanten, Variablen, Portwerte und NovaLang-Ausdrücke verwenden.

Die Operatoren `AndAlso` und `OrElse` müssen ihre Kurzschlusssemantik beibehalten.

Eine Bedingung darf erst ausgewertet werden, wenn ihre erforderlichen Eingaben verfügbar sind.

## Verzweigungen

Das Ergebnis einer Bedingung kann Controlflow-Knoten steuern.

- `True` aktiviert den entsprechenden Ausführungspfad.
- `False` aktiviert den alternativen Ausführungspfad.
- Fehler werden über definierte Fehlerpfade behandelt.

Nicht ausgewählte Zweige dürfen nicht allein durch die Verzweigung ausgeführt werden.

## Sicherheit

Bedingungen dürfen keine zusätzlichen Capability-Berechtigungen erzeugen.

Systemabhängige Werte müssen über autorisierte Capability Nodes bereitgestellt werden.

Bedingungsausdrücke dürfen keine unkontrollierten Seiteneffekte verursachen.

## Normative Anforderungen

1. Bedingungen MÜSSEN die NovaLang-Typ- und Operatorsemantik verwenden.
2. Jede Bedingung MUSS ein boolesches Ergebnis liefern.
3. Vergleichsoperatoren MÜSSEN typgeprüft werden.
4. Logische Verknüpfungen MÜSSEN unterstützt werden.
5. Kurzschlussauswertung MUSS korrekt umgesetzt werden.
6. Fehlende oder ungültige Eingaben MÜSSEN definiert behandelt werden.
7. Verzweigungen DÜRFEN ausschließlich zulässige Ausführungspfade aktivieren.
8. Bedingungsauswertungen MÜSSEN bei gleichen Eingaben und gleichem Zustand reproduzierbar sein.
9. Fehler MÜSSEN dem verursachenden Ausdruck zugeordnet werden können.
10. Bedingungen DÜRFEN keine Capability-Grenzen umgehen.
11. Die Auswertung MUSS unabhängig vom grafischen Editor funktionieren.
12. Das Condition-System MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaOS erhält ein typsicheres und nachvollziehbares Bedingungssystem zur kontrollierten Steuerung von Entscheidungen und Verzweigungen innerhalb von Logic Graphs.
