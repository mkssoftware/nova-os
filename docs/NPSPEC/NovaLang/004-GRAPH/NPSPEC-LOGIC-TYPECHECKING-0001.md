
# NPSPEC-LOGIC-TYPECHECKING-0001 – NovaOS Logic Graph Type Checking

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Typprüfung

## Zweck

Definiert die statische Typprüfung von Logic Graphs vor ihrer Ausführung.

Ziel ist die frühzeitige Erkennung inkompatibler Ports, ungültiger Verbindungen und fehlerhafter NovaLang-Schnittstellen.

## Architektur

Das Type Checking besteht aus:

- **Graph Type Checker:** Koordination der Typprüfung.
- **Port Type Validator:** Prüfung der Porttypen.
- **Connection Type Validator:** Prüfung verbundener Typen.
- **Generic Type Resolver:** Auflösung generischer Typen.
- **Conversion Validator:** Prüfung zulässiger Konvertierungen.
- **Diagnostic Reporter:** Bereitstellung präziser Fehlermeldungen.

Die Typprüfung verwendet den NovaLang Type Checker und die Regeln aus `NPSPEC-LOGIC-TYPES-0001`.

## Prüfverfahren

Die Typprüfung erfolgt in folgenden Schritten:

1. Knoten- und Portdefinitionen auflösen.
2. Typreferenzen und generische Parameter prüfen.
3. Ein- und Ausgangstypen vergleichen.
4. Zulässige Typkonvertierungen bestimmen.
5. Script-Signaturen mit Portdefinitionen abgleichen.
6. Fehler und Warnungen erzeugen.

Die Prüfung muss unabhängig von der grafischen Darstellung erfolgen.

## NovaLang-Integration

Custom Scripts innerhalb von `.nlf` werden mit dem regulären NovaLang Type Checker geprüft.

Es gelten dieselben Regeln für:

- Zuweisungskompatibilität
- Generics
- Nullability
- Typinferenz
- Konvertierungen
- Funktionsparameter und Rückgabewerte

Abweichende Typregeln für Logic Graphs sind nicht zulässig.

## Inkrementelle Typprüfung

Bei Änderungen an Knoten, Ports oder Verbindungen werden nur die betroffenen Abhängigkeiten erneut geprüft, soweit dies sicher möglich ist.

Veraltete Prüfergebnisse müssen verworfen werden.

Fehlerhafte Graphteile dürfen andere, unabhängige Prüfungen nicht verhindern.

## Diagnose

Typfehler müssen den betroffenen Knoten, Port oder die Verbindung eindeutig identifizieren.

Diagnosen enthalten Fehlerart, Ursache und nach Möglichkeit einen konkreten Korrekturhinweis.

## Normative Anforderungen

1. Logic Graphs MÜSSEN vor der Ausführung statisch typgeprüft werden.
2. Die Prüfung MUSS die regulären NovaLang-Typregeln verwenden.
3. Alle verbundenen Ports MÜSSEN auf Typkompatibilität geprüft werden.
4. Generische Typen und Nullability MÜSSEN berücksichtigt werden.
5. Script-Signaturen MÜSSEN mit ihren Portdefinitionen übereinstimmen.
6. Unzulässige implizite Konvertierungen MÜSSEN abgelehnt werden.
7. Typfehler MÜSSEN präzise diagnostiziert werden.
8. Änderungen MÜSSEN eine erneute Prüfung betroffener Abhängigkeiten auslösen.
9. Graphen mit blockierenden Typfehlern DÜRFEN nicht ausgeführt werden.
10. Typprüfung DARF keine Capability-Berechtigungen erzeugen.
11. Der Type Checker MUSS ohne grafischen Editor nutzbar sein.
12. Die Typprüfung MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaOS erhält eine konsistente, inkrementelle und NovaLang-kompatible Typprüfung für sichere Logic-Graph-Verbindungen und ausführbare Solutions.
