
# NPSPEC-LOGIC-VARIABLES-0001 – NovaOS Logic Graph Variables

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Variablenverwaltung

## Zweck

Definiert die Deklaration, Verwendung und Verwaltung von Variablen innerhalb eines NovaOS Logic Graphs.

Ziel ist ein typsicherer Datenaustausch zwischen Knoten mit eindeutigem Gültigkeitsbereich und kontrollierten Änderungen.

## Architektur

Das Variablensystem besteht aus:

- **Variable Registry:** Verwaltung der Variablendefinitionen.
- **Variable Resolver:** Auflösung von Namen und Gültigkeitsbereichen.
- **Variable Store:** Speicherung aktueller Werte.
- **Variable Validator:** Prüfung von Typen und Zuweisungen.
- **Variable Observer:** Überwachung von Wertänderungen.

Die Variablenverwaltung verwendet das State-System aus `NPSPEC-LOGIC-STATE-0001`.

## Variablenmodell

Jede Variable besitzt:

- Eindeutige Variable-ID
- Namen
- NovaLang-kompatiblen Datentyp
- Gültigkeitsbereich
- Optionalen Initialwert
- Veränderbarkeitskennzeichnung

Variablen werden unabhängig von ihrer grafischen Darstellung identifiziert.

## Gültigkeitsbereiche

Unterstützt werden:

- **Node:** Innerhalb eines Knotens.
- **Subgraph:** Innerhalb eines Teilgraphen.
- **Graph:** Innerhalb des gesamten Graphen.
- **Solution:** Innerhalb derselben Solution.
- **Execution:** Innerhalb einer einzelnen Ausführung.

Variablen unterschiedlicher Solutions bleiben standardmäßig isoliert.

## Variablenoperationen

- Variable deklarieren
- Wert lesen
- Wert zuweisen
- Wertänderung beobachten
- Variable zurücksetzen
- Variable entfernen

Zuweisungen müssen die NovaLang-Typregeln einhalten.

Schreibgeschützte Variablen dürfen nach ihrer Initialisierung nicht verändert werden.

## Parallelität und Zustände

Gemeinsam genutzte veränderbare Variablen unterliegen den Synchronisationsregeln des State Managers.

Atomare Aktualisierungen müssen unterstützt werden.

Variablenänderungen können typisierte Ereignisse auslösen.

Persistenz wird ausschließlich über ausdrücklich deklarierte Zustandsbindungen bereitgestellt.

## Sicherheit

Variablen dürfen keine Capability-Berechtigungen erzeugen oder erweitern.

Autorisierte Ressourcenreferenzen dürfen nur innerhalb ihrer gültigen Zugriffs- und Lebenszyklusverträge gespeichert werden.

## Normative Anforderungen

1. Jede Variable MUSS eine eindeutige ID und einen Datentyp besitzen.
2. Variablentypen MÜSSEN der NovaLang-Typsemantik entsprechen.
3. Gültigkeitsbereiche MÜSSEN eindeutig definiert sein.
4. Variablen MÜSSEN vor ihrer Verwendung deklariert sein.
5. Zuweisungen MÜSSEN statisch und zur Laufzeit vertragsgemäß geprüft werden.
6. Schreibgeschützte Variablen DÜRFEN nicht verändert werden.
7. Gleichzeitige Zugriffe MÜSSEN konsistent behandelt werden.
8. Änderungen MÜSSEN beobachtbar sein.
9. Variablen unterschiedlicher Solutions MÜSSEN standardmäßig isoliert bleiben.
10. Variablen DÜRFEN keine Capability-Grenzen umgehen.
11. Variablenfehler MÜSSEN diagnostizierbar sein.
12. Das Variablensystem MUSS unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält ein einheitliches, typsicheres und zustandsintegriertes Variablensystem für die kontrollierte Datenverarbeitung innerhalb von Logic Graphs.
