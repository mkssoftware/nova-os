
# NPSPEC-LOGIC-PORTS-0001 – NovaOS Logic Graph Ports

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Ports

## Zweck

Definiert die Schnittstellen zwischen Knoten eines NovaOS Logic Graphs.

Ziel ist ein typsicherer und kontrollierter Austausch von Daten, Ereignissen und Ressourcen.

## Architektur

Jeder Port besitzt:

- **Port ID:** Eindeutige Identität innerhalb des Knotens.
- **Direction:** Eingang oder Ausgang.
- **Data Type:** NovaLang-kompatibler Datentyp.
- **Port Kind:** Daten, Ereignis oder Steuerung.
- **Cardinality:** Zulässige Anzahl von Verbindungen.
- **Default Value:** Optionaler Standardwert.
- **Metadata:** Beschreibung und Darstellungsinformationen.

Ports werden durch den jeweiligen Knotentyp definiert.

## Porttypen

- **Data Ports:** Übertragen typisierte Werte.
- **Event Ports:** Übertragen Ereignisse mit optionalen Nutzdaten.
- **Control Ports:** Definieren explizite Ausführungsabhängigkeiten.
- **Resource Ports:** Übertragen kontrollierte Ressourcenreferenzen.

Resource Ports unterliegen den jeweiligen Capability- und Lebenszyklusregeln.

## Verbindungskompatibilität

Eine Verbindung ist zulässig, wenn:

- Ausgang und Eingang kompatible Portarten besitzen.
- Die Datentypen gemäß NovaLang-Typsystem kompatibel sind.
- Die Kardinalitätsregeln eingehalten werden.
- Keine unzulässigen Ausführungsabhängigkeiten entstehen.

Implizite Typumwandlungen sind nur zulässig, wenn sie durch NovaLang eindeutig definiert sind.

## Datenübertragung

Daten werden über definierte Werte- oder Referenzsemantik übertragen.

Veränderbare Ressourcen dürfen nur entsprechend ihrer Zugriffsrechte weitergegeben werden.

Asynchrone Ereignisse und Datenströme müssen kontrollierbare Puffer- und Abbruchmechanismen unterstützen.

## Capability-Integration

Capability Nodes stellen Ergebnisse und autorisierte Ressourcen über ihre Ausgangsports bereit.

Die Verbindung mit einem Script Node erteilt keine zusätzlichen Berechtigungen.

Ein Script darf ausschließlich die übergebenen Daten und Ressourcen gemäß deren Vertrag verwenden.

## Normative Anforderungen

1. Jeder Port MUSS eine stabile Port-ID besitzen.
2. Jeder Port MUSS Richtung, Art und Datentyp definieren.
3. Verbindungen MÜSSEN vor der Ausführung typgeprüft werden.
4. Datentypen MÜSSEN mit dem NovaLang-Typsystem übereinstimmen.
5. Kardinalitätsregeln MÜSSEN eingehalten werden.
6. Ungültige Verbindungen MÜSSEN diagnostiziert werden.
7. Ressourcenreferenzen DÜRFEN keine zusätzlichen Berechtigungen erzeugen.
8. Zustands- und Lebenszyklusregeln MÜSSEN bei Ressourcenübergaben erhalten bleiben.
9. Asynchrone Übertragungen MÜSSEN kontrolliert abbrechbar sein.
10. Portdefinitionen MÜSSEN unabhängig von der grafischen Darstellung funktionieren.
11. Änderungen an Portdefinitionen MÜSSEN versionierbar und validierbar sein.
12. Das Portsystem MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaOS erhält ein einheitliches, typsicheres Portsystem zur kontrollierten Verbindung von Capabilities, NovaLang-Skripten und weiteren Logic-Graph-Knoten.
