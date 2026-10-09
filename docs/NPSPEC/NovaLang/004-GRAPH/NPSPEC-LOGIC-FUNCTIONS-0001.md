
# NPSPEC-LOGIC-FUNCTIONS-0001 – NovaOS Logic Graph Functions

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Funktionen

## Zweck

Definiert wiederverwendbare Funktionen innerhalb eines NovaOS Logic Graphs.

Ziel ist die Kapselung von Berechnungen und Logik mit typisierten Parametern, Rückgabewerten und einheitlicher NovaLang-Semantik.

## Architektur

Das Function-System besteht aus:

- **Function Registry:** Verwaltung der Funktionsdefinitionen.
- **Function Resolver:** Auflösung von Funktionsreferenzen.
- **Signature Validator:** Prüfung von Parametern und Rückgabewerten.
- **Function Invoker:** Kontrollierter Funktionsaufruf.
- **Execution Context:** Verwaltung lokaler Variablen und Aufrufzustände.

Die Ausführung erfolgt über die Graph Runtime und die NovaLang Runtime.

## Funktionsmodell

Jede Funktion besitzt:

- Eindeutige Function-ID
- Namen
- Typisierte Parameter
- Optionalen Rückgabetyp
- Versionierte Signatur
- Definierte Implementierung

Funktionen können durch NovaLang-Code oder gekapselte Subgraphs implementiert werden.

## Funktionsarten

- **Pure Function:** Berechnung ohne beobachtbare Seiteneffekte.
- **Stateful Function:** Verarbeitung mit explizitem Zustand.
- **Async Function:** Asynchrone Verarbeitung.
- **Subgraph Function:** Funktion auf Grundlage eines Teilgraphen.

Funktionen ohne Rückgabewert entsprechen der `Sub`-Semantik von NovaLang.

## Parameter und Rückgabewerte

Parameter verwenden die regulären NovaLang-Typregeln.

Unterstützt werden:

- Wertparameter
- Referenzparameter gemäß NovaLang-Vertrag
- Optionale Parameter
- Generische Parameter
- Typisierte Rückgabewerte

Parameter und Rückgabewerte werden über die Ports des Funktionsknotens abgebildet.

## Ausführung

Jeder Funktionsaufruf erhält einen eigenen Ausführungskontext.

Lokale Variablen und temporäre Zustände bleiben zwischen unabhängigen Aufrufen isoliert.

Rekursive Aufrufe sind zulässig, unterliegen jedoch definierten Ressourcen- und Tiefenlimits.

Asynchrone Aufrufe verwenden Structured Concurrency.

## Capability-Integration

Funktionen dürfen keine zusätzlichen Systemberechtigungen anfordern.

Benötigte Systemzugriffe müssen durch explizite Capability Nodes im Logic Graph bereitgestellt werden.

Funktionen dürfen ausschließlich übergebene autorisierte Ressourcen gemäß deren Verträgen verwenden.

## Normative Anforderungen

1. Logic Graphs MÜSSEN wiederverwendbare Funktionen unterstützen.
2. Jede Funktion MUSS eine eindeutige ID und eine typisierte Signatur besitzen.
3. Funktionsparameter und Rückgabewerte MÜSSEN der NovaLang-Semantik entsprechen.
4. Funktionen MÜSSEN durch NovaLang-Code oder Subgraphs implementierbar sein.
5. Funktionsaufrufe MÜSSEN vor der Ausführung typgeprüft werden.
6. Jeder Aufruf MUSS einen isolierten lokalen Ausführungskontext besitzen.
7. Pure Functions DÜRFEN keine beobachtbaren Seiteneffekte verursachen.
8. Rekursion MUSS durch Ressourcenlimits begrenzbar sein.
9. Asynchrone Funktionen MÜSSEN kontrolliert abbrechbar sein.
10. Änderungen an Funktionssignaturen MÜSSEN abhängige Aufrufe erneut validieren.
11. Funktionen DÜRFEN keine Capability-Berechtigungen erzeugen oder erweitern.
12. Das Function-System MUSS unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält ein typsicheres und wiederverwendbares Funktionssystem, das NovaLang-Funktionen und Subgraphs einheitlich in die Ausführung von Logic Graphs integriert.
