
# NPSPEC-LOGIC-EDGES-0001 – NovaOS Logic Graph Edges

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Verbindungen

## Zweck

Definiert die Verbindungen zwischen Knoten eines NovaOS Logic Graphs.

Ziel ist die typsichere, nachvollziehbare und kontrollierte Übertragung von Daten, Ereignissen und Ausführungsabhängigkeiten.

## Architektur

Eine Edge besteht aus:

- **Edge ID:** Eindeutige Verbindungsidentität.
- **Source:** Referenz auf Knoten und Ausgangsport.
- **Target:** Referenz auf Knoten und Eingangsport.
- **Edge Type:** Daten-, Ereignis-, Steuerungs- oder Ressourcenverbindung.
- **Metadata:** Optionale Editor- und Diagnoseinformationen.

Edges besitzen keine eigenständigen Systemberechtigungen.

## Verbindungstypen

- **Data Edge:** Überträgt typisierte Werte.
- **Event Edge:** Leitet Ereignisse und Nutzdaten weiter.
- **Control Edge:** Definiert Ausführungsabhängigkeiten.
- **Resource Edge:** Übergibt autorisierte Ressourcenreferenzen.

Die Semantik richtet sich nach den verbundenen Ports und deren Verträgen.

## Validierung

Eine Verbindung ist gültig, wenn:

- Quell- und Zielknoten existieren.
- Beide referenzierten Ports existieren.
- Richtung, Portart und Datentyp kompatibel sind.
- Kardinalitätsregeln eingehalten werden.
- Keine unzulässigen Abhängigkeitszyklen entstehen.

Ungültige Verbindungen dürfen nicht ausgeführt werden.

## Ausführungssemantik

Edges definieren den logischen Übertragungsweg, führen jedoch selbst keine Programmlogik aus.

Die Graph Runtime übernimmt Übertragung, Synchronisation und Fehlerbehandlung gemäß den Portverträgen.

Zyklen sind nur mit ausdrücklich definierten Zustands-, Ereignis- oder Rückkopplungsmechanismen zulässig.

## Ressourcen und Sicherheit

Resource Edges übertragen ausschließlich gültige, autorisierte Ressourcenreferenzen.

Die Weitergabe darf weder Berechtigungen erweitern noch Capability-Prüfungen umgehen.

Lebensdauer und Zugriffsrechte der übertragenen Ressourcen müssen erhalten bleiben.

## Normative Anforderungen

1. Jede Edge MUSS eine eindeutige Edge-ID besitzen.
2. Jede Edge MUSS einen gültigen Quell- und Zielport referenzieren.
3. Verbindungen MÜSSEN vor der Ausführung validiert werden.
4. Portarten und Datentypen MÜSSEN kompatibel sein.
5. Kardinalitätsregeln MÜSSEN eingehalten werden.
6. Unzulässige Zyklen MÜSSEN erkannt werden.
7. Edges DÜRFEN keine eigenen Capability-Berechtigungen erzeugen.
8. Ressourcenrechte DÜRFEN durch Verbindungen nicht erweitert werden.
9. Änderungen an Verbindungen MÜSSEN nachvollziehbar und versionierbar sein.
10. Fehlerhafte Verbindungen MÜSSEN diagnostizierbar sein.
11. Die Ausführungssemantik MUSS unabhängig von der grafischen Darstellung sein.
12. Das Verbindungssystem MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaOS erhält ein einheitliches, typsicheres Verbindungssystem für die kontrollierte Kommunikation und Ausführungssteuerung innerhalb von Logic Graphs.
