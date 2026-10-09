
# NPSPEC-STUDIO-GRAPH-DATA-PREVIEW-0001 – NovaLang Studio Graph Data Preview

## Status

Angenommen

## Kategorie

NovaOS / NovaLang Studio / Logic Graph Editor / Datenvorschau

## Zweck

Definiert die direkte Vorschau von Daten innerhalb eines Logic Graphs.

Ziel ist, Eingaben, Ausgaben und Zwischenergebnisse von Knoten schnell untersuchen zu können, ohne zusätzliche Programme öffnen zu müssen.

## Architektur

Das Data-Preview-System besteht aus:

- **Preview Controller:** Verwaltung der Datenvorschau.
- **Data Inspector:** Untersuchung typisierter Werte.
- **Preview Renderer:** Darstellung verschiedener Datentypen.
- **Data Source Bridge:** Zugriff auf autorisierte Laufzeitdaten.
- **Preview Cache:** Zwischenspeicherung begrenzter Vorschauwerte.

Das System verwendet die bestehenden Logic Graph Runtime- und Debugging-Schnittstellen.

## Unterstützte Datentypen

Die Vorschau unterstützt:

- Zahlen, Boolean und Zeichenketten
- Arrays und Collections
- Objekte und Strukturen
- Tabellen und strukturierte Datensätze
- JSON und XML
- Bilder und Mediendaten
- Binärdaten als Hexadezimalansicht
- Fehler- und Ereignisdaten

Weitere Darstellungsformen können über registrierte Preview Renderer ergänzt werden.

## Bedienung

Daten können direkt an Knoten und Ports untersucht werden.

Unterstützt werden:

- Vorschau durch Anklicken eines Ports
- Kompakte Vorschau beim Überfahren mit der Maus
- Erweiterte Ansicht im Property Inspector
- Anheften mehrerer Vorschaufenster
- Aufklappen verschachtelter Daten
- Suche innerhalb strukturierter Werte
- Kopieren autorisierter Daten

Die Vorschau darf den Arbeitsfluss im Graph Editor nicht unterbrechen.

## Laufzeitintegration

Während einer Debug-Sitzung werden verfügbare Ein- und Ausgangswerte angezeigt.

Bei asynchronen Knoten muss zwischen wartenden, verfügbaren und fehlerhaften Ergebnissen unterschieden werden.

Ohne aktive Ausführung können deklarierte Standardwerte und gespeicherte Testdaten angezeigt werden.

## Performance

Große Datenmengen werden nur teilweise geladen und bei Bedarf erweitert.

Vorschauen unterliegen definierten Speicher-, Zeit- und Größenlimits.

Die Darstellung darf weder Graph-Ausführung noch Benutzeroberfläche blockieren.

## Sicherheit

Die Datenvorschau darf ausschließlich autorisierte Werte anzeigen.

Geschützte Daten müssen gemäß den Capability- und Debug-Berechtigungen ausgeblendet werden.

Das Öffnen einer Vorschau darf keine Capability-Operation oder Graph-Ausführung auslösen.

## Normative Anforderungen

1. Der Graph Editor MUSS eine integrierte Datenvorschau bereitstellen.
2. Portwerte MÜSSEN direkt untersuchbar sein.
3. Primitive und strukturierte NovaLang-Datentypen MÜSSEN unterstützt werden.
4. Bilder, Tabellen und Binärdaten SOLLEN geeignete Darstellungen erhalten.
5. Verschachtelte Daten MÜSSEN schrittweise untersucht werden können.
6. Asynchrone Ergebniszustände MÜSSEN erkennbar sein.
7. Große Datenmengen MÜSSEN ressourcenbegrenzt verarbeitet werden.
8. Vorschauen DÜRFEN die Graph-Ausführung nicht blockieren.
9. Nicht autorisierte Daten DÜRFEN nicht angezeigt werden.
10. Das Öffnen einer Vorschau DARF keine Systemoperation auslösen.
11. Die Data Preview MUSS ohne KI funktionieren.

## Ergebnis

NovaLang Studio erhält eine schnelle, typsichere und ressourcenschonende Datenvorschau, mit der Werte und Zwischenergebnisse direkt innerhalb des Logic Graph Editors untersucht werden können.
