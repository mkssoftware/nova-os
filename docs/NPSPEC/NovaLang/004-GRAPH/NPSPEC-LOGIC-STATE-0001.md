
# NPSPEC-LOGIC-STATE-0001 – NovaOS Logic Graph State

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Zustandsverwaltung

## Zweck

Definiert die Verwaltung von Zuständen innerhalb eines NovaOS Logic Graphs.

Ziel ist eine konsistente, typsichere und kontrollierte Speicherung und Veränderung von Zustandsdaten während der Graph-Ausführung.

## Architektur

Das State-System besteht aus:

- **State Manager:** Zentrale Zustandsverwaltung.
- **State Registry:** Registrierung und Identifikation von Zuständen.
- **State Store:** Speicherung aktueller Zustandswerte.
- **State Validator:** Prüfung von Typen und Zustandsänderungen.
- **State Observer:** Benachrichtigung über Änderungen.
- **State Snapshot Manager:** Erstellung und Wiederherstellung von Zustandsabbildern.

Die Zustandsverwaltung ist Bestandteil der Graph Runtime.

## Zustandsarten

Unterstützt werden:

- **Node State:** Interner Zustand eines Knotens.
- **Graph State:** Gemeinsam genutzter Zustand innerhalb eines Graphen.
- **Solution State:** Zustandsdaten über mehrere Graphen einer Solution hinweg.
- **Execution State:** Temporärer Zustand einer einzelnen Ausführung.
- **Persistent State:** Dauerhaft gespeicherter Zustand.

Jeder Zustand besitzt eine eindeutige Identität, einen NovaLang-kompatiblen Datentyp und einen definierten Gültigkeitsbereich.

## Zustandsänderungen

Zustände werden ausschließlich über definierte Operationen gelesen und verändert.

Änderungen müssen atomar erfolgen, wenn mehrere Ausführungen denselben Zustand verwenden.

Abhängige Knoten können über State-Change-Events benachrichtigt werden.

Unkontrollierte rekursive Zustandsänderungen müssen verhindert oder begrenzt werden.

## Persistenz

Persistente Zustände werden über autorisierte NovaOS-Speicherfähigkeiten gesichert.

Gespeicherte Zustände müssen versioniert und bei Wiederherstellung validiert werden.

Temporäre Zustände werden beim Ende ihres Ausführungskontexts freigegeben.

## Isolation und Sicherheit

Zustände unterschiedlicher Solutions sind standardmäßig isoliert.

Ein gemeinsamer Zugriff ist nur über ausdrücklich autorisierte Capabilities zulässig.

Zustandsreferenzen dürfen keine bestehenden Zugriffsrechte erweitern.

## Normative Anforderungen

1. Die Graph Runtime MUSS eine integrierte Zustandsverwaltung bereitstellen.
2. Jeder Zustand MUSS eindeutig identifizierbar und typisiert sein.
3. Zustandsbereiche MÜSSEN explizit definiert sein.
4. Zustandsänderungen MÜSSEN typsicher erfolgen.
5. Gleichzeitige Zugriffe MÜSSEN konsistent synchronisiert werden.
6. Atomare Zustandsänderungen MÜSSEN unterstützt werden.
7. Zustandsänderungen MÜSSEN Ereignisse auslösen können.
8. Snapshots und kontrollierte Wiederherstellung MÜSSEN unterstützt werden.
9. Persistente Zustände MÜSSEN über autorisierte NovaOS-Capabilities gespeichert werden.
10. Zustände verschiedener Solutions MÜSSEN standardmäßig isoliert sein.
11. Zustandsfehler und ungültige Übergänge MÜSSEN diagnostizierbar sein.
12. Das State-System MUSS unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine typsichere, transaktionale und isolierte Zustandsverwaltung für die zuverlässige Ausführung zustandsbehafteter Logic Graphs.
