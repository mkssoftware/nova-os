
# NPSPEC-STUDIO-GRAPH-TEMPLATES-0001 – NovaLang Studio Graph Templates

## Status

Angenommen

## Kategorie

NovaOS / NovaLang Studio / Logic Graph Editor / Vorlagen

## Zweck

Definiert wiederverwendbare Vorlagen für Logic Graphs und Subgraphs.

Ziel ist die schnelle Erstellung häufig benötigter Abläufe durch vorgefertigte, anpassbare und validierte Graphstrukturen.

## Architektur

Das Template-System besteht aus:

- **Template Manager:** Verwaltung und Bereitstellung von Vorlagen.
- **Template Catalog:** Übersicht verfügbarer Vorlagen.
- **Template Loader:** Laden und Instanziieren von Vorlagen.
- **Parameter Resolver:** Anpassung konfigurierbarer Vorlagenparameter.
- **Template Validator:** Prüfung der Graphstruktur und Abhängigkeiten.
- **Template Serializer:** Speicherung eigener Vorlagen.

Das System verwendet das bestehende Logic-Graph-Schema und dessen Serialisierungsformat.

## Vorlagentypen

Unterstützt werden:

- **Node Templates:** Vorkonfigurierte einzelne Knoten.
- **Graph Templates:** Vollständige Logic Graphs.
- **Subgraph Templates:** Wiederverwendbare Teilgraphen.
- **Capability Templates:** Typische Kombinationen von Systemfähigkeiten.
- **Solution Templates:** Ausgangsstrukturen für vollständige Solutions.

Vorlagen können systemseitig bereitgestellt oder vom Benutzer erstellt werden.

## Vorlagenmodell

Jede Vorlage besitzt:

- Eindeutige Template-ID und Version
- Namen und Beschreibung
- Kategorie und Suchbegriffe
- Graphdefinition oder Graphfragment
- Konfigurierbare Parameter
- Erforderliche Capability-Verträge
- Kompatibilitätsinformationen

Vorlagen dürfen keine bereits erteilten Berechtigungen enthalten.

## Bedienung

Der Editor unterstützt:

- Vorlagen durchsuchen und filtern
- Vorschau vor dem Einfügen
- Drag-and-drop auf das Graph Canvas
- Parameter vor der Übernahme anpassen
- Ausgewählte Knoten als Vorlage speichern
- Eigene Vorlagen bearbeiten und löschen
- Favoriten und zuletzt verwendete Vorlagen

Häufig verwendete Vorlagen müssen unmittelbar erreichbar sein.

## Instanziierung

Beim Einfügen werden neue Knoten- und Verbindungsidentitäten erzeugt.

Interne Referenzen müssen entsprechend angepasst werden.

Externe Abhängigkeiten und Capability-Anforderungen werden geprüft.

Das Einfügen erfolgt als atomare Editoraktion und muss über Undo rückgängig gemacht werden können.

## Validierung und Versionierung

Vorlagen werden vor ihrer Verwendung anhand des Graphschemas validiert.

Inkompatible Knotentypen, fehlende Capabilities oder ungültige Verbindungen müssen erkannt werden.

Änderungen an einer Vorlage dürfen bereits erstellte Graphinstanzen nicht unbeabsichtigt verändern.

## Sicherheit

Vorlagen gelten unabhängig von ihrer Herkunft zunächst als nicht vertrauenswürdige Graphdefinitionen.

Das Laden oder Einfügen darf keine Graph-Ausführung auslösen.

Capability-Berechtigungen werden ausschließlich über die reguläre Solution-Autorisierung erteilt.

## Normative Anforderungen

1. NovaLang Studio MUSS wiederverwendbare Graphvorlagen unterstützen.
2. Vorlagen MÜSSEN eindeutige IDs und Versionen besitzen.
3. Knoten, Graphen und Subgraphs MÜSSEN als Vorlagen speicherbar sein.
4. Vorlagen MÜSSEN durchsuchbar und direkt einfügbar sein.
5. Konfigurierbare Parameter MÜSSEN unterstützt werden.
6. Beim Einfügen MÜSSEN neue Instanzidentitäten erzeugt werden.
7. Interne Referenzen MÜSSEN konsistent angepasst werden.
8. Vorlagen MÜSSEN vor ihrer Verwendung validiert werden.
9. Fehlende oder inkompatible Abhängigkeiten MÜSSEN angezeigt werden.
10. Das Einfügen MUSS atomar und rückgängig machbar sein.
11. Vorlagen DÜRFEN keine Berechtigungen erteilen oder Capability-Grenzen umgehen.
12. Das Template-System MUSS ohne KI funktionieren.

## Ergebnis

NovaLang Studio erhält ein versioniertes und erweiterbares Vorlagensystem, mit dem wiederkehrende Logic-Graph-Strukturen schnell, sicher und ohne unnötige Neuentwicklung eingesetzt werden können.
