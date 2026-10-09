
# NPSPEC-STUDIO-GRAPH-NODELIBRARY-0001 – NovaLang Studio Graph Node Library

## Status

Angenommen

## Kategorie

NovaOS / NovaLang Studio / Logic Graph Editor / Node Library

## Zweck

Definiert die zentrale Bibliothek aller verfügbaren Knotentypen für den visuellen Logic Graph Editor.

Ziel ist, Systemfähigkeiten, Logikbausteine und Custom Scripts schnell auffindbar, verständlich und direkt verwendbar bereitzustellen.

## Architektur

Die Node Library besteht aus:

- **Node Registry:** Verwaltung registrierter Knotentypen.
- **Node Catalog:** Strukturierte Darstellung verfügbarer Knoten.
- **Node Search:** Suche nach Namen, Funktionen und Kategorien.
- **Node Factory:** Erstellung neuer Knoteninstanzen.
- **Node Metadata Provider:** Bereitstellung von Beschreibungen, Ports und Typinformationen.
- **Capability Bridge:** Integration der NovaOS Capability Registry.
- **Compatibility Filter:** Prüfung der Verfügbarkeit und Kompatibilität.

Die Node Library verwendet die bestehenden Logic-Graph-Knotenverträge.

## Knotenkategorien

Die Bibliothek unterstützt:

- **Capability Nodes:** Zugriff auf autorisierte NovaOS-Fähigkeiten.
- **Custom Script Nodes:** Ausführung von NovaLang-Scripts.
- **Data Nodes:** Konstanten, Werte und Datenumwandlungen.
- **Control Nodes:** Bedingungen, Schleifen und Verzweigungen.
- **Event Nodes:** Verarbeitung von Ereignissen.
- **State Nodes:** Lesen und Ändern von Graphzuständen.
- **Function Nodes:** Wiederverwendbare Funktionen.
- **Subgraph Nodes:** Einbindung anderer Logic Graphs.
- **UI Nodes:** Verbindung zur deklarativen Benutzeroberfläche.
- **Transaction Nodes:** Steuerung transaktionaler Abläufe.

Kategorien dienen der Organisation und bestimmen nicht die technische Identität eines Knotens.

## Knotenbeschreibung

Jeder Knotentyp besitzt:

- Eindeutige Typ-ID und Version
- Anzeigenamen und Beschreibung
- Kategorie und Suchbegriffe
- Eingangs- und Ausgangsports
- NovaLang-Typinformationen
- Konfigurationsparameter
- Ausführungsvertrag
- Optionale Capability-Anforderungen

Knoteninstanzen erhalten zusätzlich eine eigene eindeutige Node-ID.

## Suche und Bedienung

Die Node Library unterstützt:

- Volltextsuche
- Kategorien und Filter
- Favoriten
- Zuletzt verwendete Knoten
- Kontextabhängige Vorschläge
- Drag-and-drop auf das Graph Canvas
- Schnelles Einfügen über Tastatur und Kontextmenü

Häufig verwendete Knoten müssen ohne aufwendige Menüführung erreichbar sein.

## Capability-Integration

Verfügbare Systemfähigkeiten werden über die zentrale Capability Registry ermittelt.

Capability Nodes verwenden die registrierte Capability-ID und deren versionierten Vertrag.

Nicht verfügbare oder inkompatible Capabilities werden entsprechend gekennzeichnet.

Das Einfügen eines Capability Nodes erteilt keine Berechtigung.

## Custom Scripts und Erweiterungen

Custom Script Nodes verwenden die reguläre NovaLang Runtime und `.nlf`-Dateien.

Zusätzliche Knotentypen können über registrierte, versionierte Erweiterungen bereitgestellt werden.

Erweiterungen dürfen keine eigenen Sicherheitsregeln anstelle der NovaOS Capability- und Runtime-Prüfungen verwenden.

## Validierung und Performance

Die Node Library prüft Knotendefinitionen vor ihrer Bereitstellung auf gültige Metadaten und Schnittstellen.

Große Kataloge werden bei Bedarf inkrementell geladen und durchsucht.

Die Darstellung der Bibliothek darf die Graph-Ausführung nicht blockieren.

## Normative Anforderungen

1. NovaLang Studio MUSS eine zentrale Node Library bereitstellen.
2. Jeder Knotentyp MUSS eine eindeutige Typ-ID und Version besitzen.
3. Knotentypen MÜSSEN ihre Ports und NovaLang-Typen deklarieren.
4. Die Bibliothek MUSS alle unterstützten Standardknotenkategorien verwalten.
5. Capability Nodes MÜSSEN aus der zentralen Capability Registry ermittelt werden.
6. Suche, Kategorien und Favoriten MÜSSEN unterstützt werden.
7. Knoten MÜSSEN per Drag-and-drop eingefügt werden können.
8. Nicht verfügbare oder inkompatible Knoten MÜSSEN erkennbar sein.
9. Knotendefinitionen MÜSSEN vor ihrer Verwendung validiert werden.
10. Erweiterungen MÜSSEN versionierte Knotenverträge verwenden.
11. Das Einfügen eines Knotens DARF keine Systemberechtigung erteilen.
12. Die Node Library DARF keine Capability- oder Sicherheitsgrenzen umgehen.
13. Große Kataloge SOLLEN ressourcenschonend und inkrementell verarbeitet werden.
14. Alle grundlegenden Bibliotheksfunktionen MÜSSEN ohne KI verfügbar sein.

## Ergebnis

NovaLang Studio erhält eine zentrale, erweiterbare und übersichtliche Knotenbibliothek, über die sämtliche Logic-Graph-Bausteine und NovaOS-Capabilities schnell gefunden und direkt in Solutions verwendet werden können.
