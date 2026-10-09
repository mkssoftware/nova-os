
# NPSPEC-STUDIO-GRAPH-CAPABILITY-PICKER-0001 – NovaLang Studio Graph Capability Picker

## Status

Angenommen

## Kategorie

NovaOS / NovaLang Studio / Logic Graph Editor / Capability Picker

## Zweck

Definiert die Auswahl und Einbindung von NovaOS-Capabilities innerhalb des Logic Graph Editors.

Ziel ist, verfügbare Systemfähigkeiten schnell zu finden, ihre Schnittstellen zu verstehen und sie direkt als Capability Nodes in Solutions einzufügen.

## Architektur

Der Capability Picker besteht aus:

- **Capability Registry Bridge:** Zugriff auf die zentrale NovaOS Capability Registry.
- **Capability Catalog:** Darstellung verfügbarer Fähigkeiten.
- **Search & Filter:** Suche nach Namen, Kategorien und Capability-IDs.
- **Contract Inspector:** Anzeige von Ports, Datentypen und Anforderungen.
- **Compatibility Checker:** Prüfung der Verwendbarkeit im aktuellen Graphen.
- **Node Factory Bridge:** Erstellung passender Capability Nodes.

Der Picker verwendet ausschließlich registrierte Capability-Verträge.

## Bedienkonzept

Der Capability Picker ist über die Ribbon-Oberfläche, die Node Library und das Kontextmenü des Graph Canvas erreichbar.

Unterstützt werden:

- Sofortsuche während der Eingabe
- Kategorien und Favoriten
- Zuletzt verwendete Capabilities
- Anzeige kompatibler Fähigkeiten
- Drag-and-drop auf das Canvas
- Direkte Einfügung an kompatiblen Ports
- Vorschau der Ein- und Ausgänge

Häufig benötigte Fähigkeiten müssen mit wenigen Interaktionen erreichbar sein.

## Capability-Darstellung

Jede Capability zeigt:

- Anzeigenamen und Beschreibung
- Eindeutige Capability-ID
- Version und Anbieter
- Eingangs- und Ausgangsports
- NovaLang-Datentypen
- Benötigte Berechtigungen
- Verfügbarkeit und Kompatibilität

Capability-IDs folgen dem Format `domain.authority.namespace.name`.

## Kontextabhängige Auswahl

Wird der Picker von einem Port aus geöffnet, werden kompatible Capabilities bevorzugt angezeigt.

Die Auswahl berücksichtigt Porttypen, Richtung, Capability-Verträge und Graphkontext.

Nicht kompatible Fähigkeiten werden gekennzeichnet oder ausgefiltert.

## Einbindung in den Logic Graph

Nach Auswahl wird ein Capability Node mit eindeutiger Node-ID erstellt.

Die deklarierten Ports und Parameter werden aus dem registrierten Capability-Vertrag übernommen.

Custom Scripts erhalten Systemdaten ausschließlich über explizit verbundene Capability Nodes.

Das Einfügen einer Capability führt diese nicht aus und erteilt keine Berechtigungen.

## Berechtigungen und Sicherheit

Der Picker zeigt erforderliche Berechtigungen verständlich an.

Die tatsächliche Autorisierung erfolgt ausschließlich über die zentrale NovaOS-Berechtigungsverwaltung und die verifizierte Solution-Identität.

Nicht verfügbare, inkompatible oder nicht vertrauenswürdige Capabilities dürfen nicht als ausführbar angeboten werden.

## Normative Anforderungen

1. NovaLang Studio MUSS einen integrierten Capability Picker bereitstellen.
2. Capabilities MÜSSEN aus der zentralen Capability Registry stammen.
3. Suche, Kategorien und Favoriten MÜSSEN unterstützt werden.
4. Capability-IDs, Versionen und Schnittstellen MÜSSEN einsehbar sein.
5. Erforderliche Berechtigungen MÜSSEN angezeigt werden.
6. Der Picker MUSS kontextabhängige Kompatibilitätsfilter unterstützen.
7. Capabilities MÜSSEN per Drag-and-drop eingefügt werden können.
8. Neue Capability Nodes MÜSSEN ihre Ports aus dem registrierten Vertrag erhalten.
9. Das Einfügen DARF keine Capability-Ausführung auslösen.
10. Der Picker DARF keine Berechtigungen selbst erteilen.
11. Custom Scripts DÜRFEN durch den Picker keinen direkten Systemzugriff erhalten.
12. Nicht vertrauenswürdige oder inkompatible Capabilities MÜSSEN erkannt werden.
13. Der Capability Picker MUSS unabhängig von KI funktionieren.

## Ergebnis

NovaLang Studio erhält einen schnellen, kontextbezogenen und sicheren Capability Picker, mit dem NovaOS-Systemfähigkeiten unmittelbar gefunden, geprüft und als typisierte Knoten in Logic Graphs eingebunden werden können.
