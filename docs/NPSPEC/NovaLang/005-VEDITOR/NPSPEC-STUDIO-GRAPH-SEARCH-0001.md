
# NPSPEC-STUDIO-GRAPH-SEARCH-0001 – NovaLang Studio Graph Search

## Status

Angenommen

## Kategorie

NovaOS / NovaLang Studio / Logic Graph Editor / Suche

## Zweck

Definiert die schnelle Suche und Navigation innerhalb von Logic Graphs.

Ziel ist, Knoten, Verbindungen, Capabilities und Subgraphs auch in umfangreichen Solutions unmittelbar auffindbar zu machen.

## Architektur

Das Search-System besteht aus:

- **Search Controller:** Steuerung von Suchanfragen.
- **Graph Search Index:** Indexierung durchsuchbarer Graphinformationen.
- **Search Filter:** Eingrenzung der Suchergebnisse.
- **Result Navigator:** Navigation zu gefundenen Elementen.
- **Search Highlight Overlay:** Hervorhebung von Treffern im Canvas.

Die Suche verwendet das bestehende Graphmodell und dessen Metadaten.

## Suchbereiche

Durchsuchbar sind:

- Knotenname und Node-ID
- Knotentyp und Kategorie
- Capability-ID und Beschreibung
- Portnamen und Datentypen
- Verbindungen und referenzierte Knoten
- Subgraphs und Funktionen
- Custom-Script-Referenzen
- Eigenschaften und Beschreibungen

Die Suche kann auf den aktuellen Graphen oder die gesamte Solution begrenzt werden.

## Suchfunktionen

Unterstützt werden:

- Sofortsuche während der Eingabe
- Teilwortsuche
- Groß-/Kleinschreibungsunabhängige Suche
- Filter nach Knotentyp und Kategorie
- Suche nach exakten IDs
- Suche nach verbundenen Elementen
- Vorwärts- und Rückwärtsnavigation zwischen Treffern

Eine erweiterte Suche kann reguläre Ausdrücke unterstützen.

## Bedienung

Die Suche ist über eine unmittelbar erreichbare Suchleiste und `Strg+F` verfügbar.

Suchergebnisse werden mit Namen, Typ und Graphzugehörigkeit angezeigt.

Ein ausgewählter Treffer wird auf dem Canvas zentriert und hervorgehoben.

Bei Treffern in Subgraphs erfolgt die Navigation unter Erhaltung des bisherigen Arbeitskontexts.

## Indexierung und Performance

Der Suchindex wird beim Öffnen eines Graphen erstellt und bei Änderungen inkrementell aktualisiert.

Große Graphen dürfen die Benutzeroberfläche während der Suche nicht blockieren.

Nicht mehr gültige Suchergebnisse müssen automatisch aktualisiert oder entfernt werden.

## Sicherheit

Die Suche darf nur Informationen anzeigen, auf die der Benutzer innerhalb des aktuellen Arbeitskontexts zugreifen darf.

Geschützte Capability-Daten und Laufzeitwerte dürfen nicht ohne entsprechende Berechtigung durchsucht werden.

Suchoperationen dürfen keine Graph-Ausführung auslösen.

## Normative Anforderungen

1. Der Graph Editor MUSS eine integrierte Graphsuche bereitstellen.
2. Knoten, Ports, Capabilities und Subgraphs MÜSSEN durchsuchbar sein.
3. Die Suche MUSS den aktuellen Graphen und die gesamte Solution unterstützen.
4. Teilwortsuche und Suche nach exakten IDs MÜSSEN möglich sein.
5. Suchergebnisse MÜSSEN direkt auf dem Canvas erreichbar sein.
6. Treffer MÜSSEN visuell hervorgehoben werden.
7. Suchindizes MÜSSEN bei Graphänderungen aktualisiert werden.
8. Umfangreiche Suchvorgänge DÜRFEN die Benutzeroberfläche nicht blockieren.
9. Suchoperationen DÜRFEN die Graphdefinition nicht verändern.
10. Geschützte Informationen DÜRFEN nicht ohne Berechtigung offengelegt werden.
11. Die Graphsuche MUSS unabhängig von KI funktionieren.

## Ergebnis

NovaLang Studio erhält eine schnelle, kontextbezogene und ressourcenschonende Graphsuche, mit der sämtliche relevanten Elemente komplexer Logic Graphs unmittelbar gefunden und geöffnet werden können.
